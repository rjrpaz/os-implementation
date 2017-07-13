/*---------------------------------------------------------------------------
 Provide a simpler and easier to understand interface to the system V
 semaphore system calls. There are 7 functions available to the users:
 (courtesy to W. Richard Stevens "UNIX Networking Programming" pp. 146 - 151)
 
 id = sem_create(key, initval);    # create a semaphore with initial value
 id = sem_open(key);               # open a semaphore (must already exist)
 sem_wait(id);                     # wait = P = down by 1
 sem_signal(id);                   # signal = V = up by 1
 sem_op(id, amount);               # wait if (amount < 0)
                                   # signal if (amount > 0)
 sem_close(id);                    # close a semaphore
 sem_rm(id);                       # remove a semaphore(delete)

 We create and use a 3-member set for the requested semaphore.
 The first member, [0], is the actual semaphore value, and the second
 member, [1], is a counter used to know when all the processes have finished
 with the semaphore. The counter is initialized to a large number,
 decremented on every create or open and incremented on every close.
 This way we can use the "adjust" feature provided by system V so that
 any process that exits without calling sem_close() is accounted
 for. It doesn't help us if the last process does this (as we have
 no way of getting control to remove the semaphore) but it will
 work if any process other than the last does an exit (intentional
 or un-intentional).
 The third member, [2], of the semaphore set is used as a lock variable
 to aviod any race condition in the sem_create() and sem_close() functions.
---------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <errno.h>
extern int errno;

#define BIG_COUNT 10000 /* initial value of process counter */

/* Define the semaphore operation arrays for the semop() calls */
static struct sembuf    op_lock[2] = {
       2, 0, 0,         /* wait for [2] (lock) to equal 0 */
       2, 1, SEM_UNDO   /* then increment [2] to 1 - this locks it */
                        /* UNDO to release the lock if processes exits */
                        /* before explicitly unlocking */
};

static struct sembuf    op_endcreate[2] = {
       1, -1, SEM_UNDO, /* decrement [1] (proc counter) with undo on exit */
                        /* UNDO to adjust proc counter if process exits */
                        /* before explicitly calling sem_close() */
       2, -1, SEM_UNDO  /* then decrement [2] (lock) back to 0 */
};

static struct sembuf    op_open[1] = {
       1, -1, SEM_UNDO  /* decrement [1] (proc counter) with undo on exit */
};

static struct sembuf    op_close[3] = {
       2, 0, 0,         /* wait for [2] (lock) to equal to 0 */
       2, 1, SEM_UNDO,  /* then increment [2] to 1 - this locks it */
       1, 1, SEM_UNDO   /* then increment [1] (proc counter) */
};

static struct sembuf    op_unlock[1] = {
       2, -1, SEM_UNDO  /* decrement [2] (lock) back to 0 */
};

static struct sembuf    op_op[1] = {
       0, 99, SEM_UNDO  /* decrement or increment [0] with undo on exit */
                        /* the 99 is set to the actual amount to add or
                        /* subtract (positive or negative) */
};

union{
      int val;
      struct semid_ds *buf;
      ushort *array;
}semctl_arg;

/*---------------------------------------------------------------------------
 Create a semaphore with a specified initial value.
 If the semaphore already exists, we don't initialize it (of course).
 We return the semaphore ID if all OK, else -1.
 ---------------------------------------------------------------------------*/
int sem_create(key_t key, int initval)
{
 register int id, semval;

 if (key == IPC_PRIVATE)
    return (-2);  /* not intended for private semaphores */
   else if (key == (key_t) -1)
           return (-1); /* probably an ftok() error by caller */

again:
 if ((id = semget(key, 3, 0666 | IPC_CREAT)) < 0)
    return(-1); /* permission problem or tables full */
 /*
  When the semaphore is created, we know that the value of all 3 member is 0.
  Get a lock on the semaphore by waiting for [2] to equal to 0, then
  increment it.

  There is a race condition here. There is a possibility that
  between the semget() above and the semop() below, another
  process can call our sem_close() function which can remove
  the semaphore if that process is the last one using it.
  Therefore, we handle the error condition of an invalid
  semaphore ID specially below, and if it does happen, we just
  go back and create it again.
 */

 if (semop(id, &op_lock[0], 2) < 0){
    if (errno == EINVAL) goto again;
    printf("can't lock");
 }

 /*
  Get the value of the process counter. If it equals 0,
  then no one has initialized the semaphore yet.
 */

 semctl_arg.val = 0;
 if ((semval = semctl(id, 1, GETVAL, semctl_arg)) < 0)
    printf("can't GETVAL");

 if (semval == 0){
 /*
  We could initialize by doing a SETALL, but that
  would clear the adjust value that we set when we
  locked the semaphore above. Instead, we'll do 2
  system calls to initialize [0] and [1].
 */

 semctl_arg.val = initval;
 if (semctl(id, 0, SETVAL, semctl_arg) < 0)
    printf("can't SETVAL[0]");

 semctl_arg.val = BIG_COUNT;
 if (semctl(id, 1, SETVAL, semctl_arg) < 0)
    printf("can't SETVAL[1]");
 }

 /* Decrement the process counter and then release the lock. */
 if (semop(id, &op_endcreate[0], 2) < 0)
    printf("can't end create");
 return(id);
}

/*-------------------------------------------------------------------------
 Open a semaphore that must already exist.
 This function should be used, instead of sem_create(), if the caller
 knows that the semaphore must already exist. For example a client
 from a client-server pair would use this, if it is the server's
 responsiblity to create the semaphore.
 We return the semaphore ID if all OK, else -1.
 -------------------------------------------------------------------------*/
int sem_open(key_t key)
{
 register int id;
 if (key == IPC_PRIVATE)
    return(-1); /* not intended for private semaphores */
 else if (key == (key_t) -1)
    return(-1); /* probably an ftok() error by caller */

 if ((id = semget(key, 3, 0)) < 0)
    return(-1); /* doesn't exist, or tables full */

 /* Decrement the process counter. We don't need a lock to do this */
 if (semop(id, &op_open[0], 1) < 0)
    printf("can't open");
 return(id);
}

/*-------------------------------------------------------------------------
 Remove a semaphore
 This call is intended to be called by a server, for example,
 when it is being shut down, as we do an IPC_RMID on the semaphore,
 regardless whether other processes may be using it or not.
 Most other processes should use sem_close() below.
 ------------------------------------------------------------------------*/
sem_rm(int id)
{
 semctl_arg.val = 0;
 if (semctl(id, 0, IPC_RMID, semctl_arg) < 0)
    printf("can't IPC_RMID");
}

/*------------------------------------------------------------------------
 Close a semaphore
 Unlike the remove function above, this function is for a process
 to call before it exits, when it is done with the semaphore.
 We "decrement" the counter of processes using the semaphore, and
 if this was the last one, we can remove the semaphore.
 ------------------------------------------------------------------------*/
sem_close(int id)
{
 register int semval;

 /* The following semop() first gets a lock on the semaphore, 
    then increments [1] - the process counter */
 if (semop(id, &op_close[0], 3) < 0)
    printf("can't semop");

 /* Now that we have a lock, read the value of the process counter to see
    if this is the last reference to the semaphore.
    There is a race condition here - see the comments in sem_create(). */
 semctl_arg.val = 0;
 if ((semval = semctl(id, 1, GETVAL, semctl_arg)) < 0)
    printf("can't GETVAL");

 if (semval > BIG_COUNT)
    printf("sem[1] > BIG_COUNT");
 else if (semval == BIG_COUNT)
         sem_rm(id);
 else
         if (semop(id, &op_unlock[0], 1) < 0)
            printf("can't unlock"); /* unlock */
}

/*------------------------------------------------------------------------
 Wait until a semaphore's value is greater than 0, then decrement it by 1
 Dijkstra's P operation. Tanenbaum's DOWN operation.
 -----------------------------------------------------------------------*/
sem_wait(int id)
{
 sem_op(id, -1);
}

/*------------------------------------------------------------------------
 Increment a semaphore by 1.
 Dijkstra's V operation. Tanenbaum's UP operation.
 -----------------------------------------------------------------------*/
sem_signal(int id)
{
 sem_op(id, 1);
}

/*------------------------------------------------------------------------
 General semaphore operation. Increment or decrement by a user-specified
 amount (positive or negative; amount can't be zero).
 -----------------------------------------------------------------------*/
sem_op(int id, int value)
{
 if ((op_op[0].sem_op = value) == 0)
    printf("can't have value == 0");

 if (semop(id, &op_op[0], 1) < 0)
    printf("sem_op error");
}
