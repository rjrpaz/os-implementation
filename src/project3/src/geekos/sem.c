#include <geekos/string.h>
#include <geekos/malloc.h>
#include <geekos/sem.h>
#include <geekos/user.h>
#include <geekos/kthread.h>


static struct Semaphore_List s_semList;

int Create_Semaphore (const char *name, int ival)
{
    struct Semaphore *sem = 0;
    int id = 1;

//Print("ARGS. name: %s, ival: %d\n", name, ival);
    /*
     * Obtiene el primer elemento de la lista y lo
     * recorre hasta el final para ver que ID le asigna
     * al semáforo solicitado (poco eficiente).
     */
/*
    if (Is_Semaphore_List_Empty(&s_semList))
        Print("LISTA VACIA\n");
*/

    sem = Get_Front_Of_Semaphore_List(&s_semList);
    while (sem != 0) {
        KASSERT(sem != Get_Next_In_Semaphore_List(sem));
        sem = Get_Next_In_Semaphore_List(sem);
        id++;
    }

    /* La cantidad máxima de semáforos está siendo utilizada. */
    if (id >= MAX_SEM)
        return -1;

//Print("ID: %d\n", id);

    sem->id = id;

    /* Define nombre del semáforo */
    snprintf(sem->name, sizeof(sem->name), "%s", name);

    /* Define valor inicial del semáforo */
    sem->value = ival;

    /* Referencia a 1 */
    sem->nref = 1;

    /* Agrega semáforo a la lista */
    Add_To_Back_Of_Semaphore_List(&s_semList, sem);
/*
    sem = Get_Front_Of_Semaphore_List(&s_semList);
    while (sem != NULL) {
        KASSERT(sem != Get_Next_In_Semaphore_List(sem));
        Print("ESTE ID ya esta: %d\n", sem->id);
        sem = Get_Next_In_Semaphore_List(sem);
    }

    if (Is_Semaphore_List_Empty(&s_semList))
        Print("LISTA VACIA DESPUES\n");
*/
    return id;
}



int Destroy_Semaphore (int id)
{
    struct Semaphore *sem = NULL;

    if (Get_Sem_By_Id(id, sem) < 0)
        return -1;

    Remove_From_Semaphore_List(&s_semList, sem);

    return 0;
}



/*
 * Encuentra un semaforo a partir del ID
 * Params:
 *   id: ID del semaforo que intenta ubicar
 *   sem: puntero donde almacena el semaforo encontrado
 * Returns: 0 si tiene exito o -1 si falla
 */
int Get_Sem_By_Id(int id, struct Semaphore *sem)
{
    int *semaphores = NULL;
    bool acceso = false;

    sem = NULL;
    /* Chequea que el thread tenga permisos para acceder al semáforo solicitado */
    semaphores = (int *) g_currentThread->userContext->semaphores;
    while (*semaphores != -1) {
        if (*semaphores == id) {
            acceso = true;
            break;
        }
        semaphores++;
    }
    if (! acceso)
        return -1;

    /* Ubico al semáforo solicitado */
    sem = Get_Front_Of_Semaphore_List(&s_semList);
    while (sem != NULL) {
        KASSERT(sem != Get_Next_In_Semaphore_List(sem));
        if (sem->id == id)
            break;
        sem = Get_Next_In_Semaphore_List(sem);
    }

    if (sem == NULL)
        return -1;

    return 0;
}

