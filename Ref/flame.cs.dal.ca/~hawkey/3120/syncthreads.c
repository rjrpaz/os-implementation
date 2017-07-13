/* sample program to create posix threds and to demonstrate
   global variables with giving up the processor using sched_yield()
   but using a mutex to protect the global variable

   the compile line is:  
	gcc -o synthreads syncthreads.c -lpthread -lposix4

   - the thread routine "addup" adds up
   - the thread roudine "adddown" adds down
   - "myglobal" is the global variable being altered

   Original author of this code is unknown
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>

void *addup();
void *adddown();
int myglobal;
static pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;

main(){

   pthread_t tid_up, tid_down;

   myglobal = 0;
   printf("before threads, myglobal =%d\n", myglobal);

   if (pthread_create(&tid_up, NULL, addup, NULL))
      perror ("thread up not created");
   if (pthread_create(&tid_down, NULL, adddown, NULL))
      perror ("thread down not created");

   if (pthread_join (tid_up, NULL) !=0)
      perror("no thread up to join");
   if (pthread_join (tid_down, NULL) !=0)
      perror("no thread down to join");

   printf("joined with both threads, myglobal=%d\n", myglobal);

   exit (0);

}

void *addup(){
   int i;

   pthread_mutex_lock(&mylock);
   for (i=0; i<5; i++){

      myglobal++;
      printf("thread %d, myglobal=%d\n", pthread_self(), myglobal);
      sched_yield();
   }

   pthread_mutex_unlock(&mylock);

   pthread_exit(0);
}

void *adddown(){
   int i;

   pthread_mutex_lock(&mylock);
   for (i=0; i<5; i++){

      myglobal--;
      printf("thread %d, myglobal=%d\n", pthread_self(), myglobal);
      sched_yield();
   }

   pthread_mutex_unlock(&mylock);

   pthread_exit(0);
}

