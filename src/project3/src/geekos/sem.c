#include <geekos/string.h>
#include <geekos/malloc.h>
#include <geekos/sem.h>
#include <geekos/user.h>
#include <geekos/kthread.h>

//#define DEBUG 1
#undef DEBUG

static struct Semaphore_List s_semList;
void Dump_All_Semaphore_List(void);
void listTest(void);

int Create_Semaphore (const char *name, int ival)
{
//    struct Semaphore *pSem = 0, *pSemNew = NULL, sem;
    struct Semaphore *pSem;
    int id = 0;

//Print("ARGS. name: %s, ival: %d\n", name, ival);
    /*
     * Obtiene el primer elemento de la lista y lo
     * recorre hasta el final para ver que ID le asigna
     * al semáforo solicitado (poco eficiente).
     */
    pSem = Get_Front_Of_Semaphore_List(&s_semList);
//    Print("LISTA ARRANCA EN %p\n", pSem);
    while (pSem != 0) {
        KASSERT(pSem != Get_Next_In_Semaphore_List(pSem));
        if (strcmp(pSem->name, name) == 0) {
            pSem->nref++;
#ifdef DEBUG
            Print("Semaforo %s ya existe con id %d.\n", name, pSem->id);
#endif
            Print("Semaforo %s ya existe con id %d.\n", name, pSem->id);
            return pSem->id;
        }
        pSem = Get_Next_In_Semaphore_List(pSem);
        id++;
    }

    /* La cantidad máxima de semáforos está siendo utilizada. */
    if (id >= MAX_SEM)
        return -1;

	pSem = Malloc(sizeof (struct Semaphore));
//    Print("ANTES: ");
//    Dump_All_Semaphore_List();
//    sem.id = id;
    pSem->id = id;

    /* Define nombre del semáforo */
//    snprintf(sem.name, sizeof(sem.name), "%s", name);
    snprintf(pSem->name, NAMESIZE, "%s", name);

    /* Define valor inicial del semáforo */
//    sem.value = ival;
    pSem->value = ival;

    /* Referencia a 1 */
//    sem.nref = 1;
    pSem->nref = 1;

/*
	pSemNew = Malloc(sizeof (struct Semaphore));
	pSemNew = &sem;
*/

    /* Agrega semáforo a la lista */
//    Add_To_Back_Of_Semaphore_List(&s_semList, &sem);
//    Add_To_Back_Of_Semaphore_List(&s_semList, pSemNew);
    Add_To_Back_Of_Semaphore_List(&s_semList, pSem);
/*
    sem = Get_Front_Of_Semaphore_List(&s_semList);
    while (sem != NULL) {
        KASSERT(sem != Get_Next_In_Semaphore_List(sem));
        Print("ESTE ID ya esta: %d\n", sem->id);
        sem = Get_Next_In_Semaphore_List(sem);
    }

*/
    Print("DESPUES: ");
    Dump_All_Semaphore_List();
#ifdef DEBUG
    Print("ID: %d\n", id);
#endif
    return id;
}



int Destroy_Semaphore (int id)
{
    struct Semaphore *sem = NULL;

    sem = Get_Sem_By_Id(id);

    if (sem == NULL)
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
struct Semaphore *Get_Sem_By_Id(int id)
{
 //   int *semaphores = NULL;
//    bool acceso = false;
    struct Semaphore *sem;

//    sem = NULL;

    Dump_All_Semaphore_List();
Print("Busco SEM %d\n", id);

    /* Chequea que el thread tenga permisos para acceder al semáforo solicitado */
/*
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
*/

    /* Ubico al semáforo solicitado */
    sem = Get_Front_Of_Semaphore_List(&s_semList);
    while (sem != NULL) {
        KASSERT(sem != Get_Next_In_Semaphore_List(sem));
//Print("Comparo con el de direcci�n %p\n", sem);
        if (sem->id == id) {
//Print("ES ESTE\n");
            break;
	    }
        sem = Get_Next_In_Semaphore_List(sem);
    }
//Print("ENCONTRADO. Es el de direccion %p. Se llama %s\n", sem, sem->name);

Print("ENCONTRADO. Es el de direccion %p. Se llama %s\n", sem, sem->name);
    return sem;
}

void Dump_All_Semaphore_List(void)
{
    struct Semaphore *sem;
    int count = 0;

    sem = Get_Front_Of_Semaphore_List(&s_semList);
//    Print("LISTA ARRANCA EN %p\n", sem);
//    Dump_All_Thread_List();

    while (sem != 0) {
        ++count;
        Print("<%lx> <%d>(%s) ", (ulong_t) sem, sem->id, sem->name);
        sem = Get_Next_In_Semaphore_List(sem);
    }
    Print("%d Number of sems\n", count);
}


void listTest()
{
    struct Semaphore sem1, sem2, sem3;
    sem1.id = 1;
    sem2.id = 2;
    sem3.id = 3;

    Dump_All_Semaphore_List();
    Add_To_Back_Of_Semaphore_List(&s_semList, &sem1);
    Add_To_Back_Of_Semaphore_List(&s_semList, &sem3);
    Add_To_Back_Of_Semaphore_List(&s_semList, &sem2);

    Dump_All_Semaphore_List();
    Remove_From_Semaphore_List(&s_semList, &sem1);
    Dump_All_Semaphore_List();
}
