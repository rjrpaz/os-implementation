/*
 * Semaphores
 */

#ifndef GEEKOS_SEM_H
#define GEEKOS_SEM_H

#include <geekos/ktypes.h>
#include <geekos/list.h>

#define NAMESIZE 25
#define MAX_SEM 20

//extern struct Semaphore_List s_semList;

/* Cola de semaforos */
DEFINE_LIST(Semaphore_List, Semaphore);

struct Semaphore {
    /* ID del semaforo */
    int id;

    /* Nombre del semaforo */
    char name[NAMESIZE];

    /* Valor del semaforo */
    int value;

    /* Cantidad de referencias al semaforo */
    int nref;

    DEFINE_LINK(Semaphore_List, Semaphore);

    /* Lista de procesos que utilizan el semaforo */
//    struct Kernel_Thread* list;
};

IMPLEMENT_LIST(Semaphore_List, Semaphore);

int Create_Semaphore (const char *name, int ival);
int Destroy_Semaphore (int id);
struct Semaphore *Get_Sem_By_Id(int id);

#endif  /* GEEKOS_SEM_H */
