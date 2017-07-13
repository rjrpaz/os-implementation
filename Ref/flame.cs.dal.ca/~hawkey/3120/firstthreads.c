/* sample program to create posix threds and to demonstrate
   global and local variables

   the compile line is:  gcc -o firstthreads firstthreads.c -lpthread

   - the thread routine is the function "doit"
   - "myglobal" is the global variable being altered
   - "x" is the local variable passed to each thread

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

#define NUMTHREADS 5

void *doit(void *);
int myglobal;

main(){

   int i, x;
   pthread_t my_tid[NUMTHREADS];

   x = 1;

   myglobal = 0;
   printf("before threads, myglobal =%d\n", myglobal);

   for (i=0; i < NUMTHREADS; i++){
      if (pthread_create(&my_tid[i], NULL, doit, (void *) x))
         perror ("thread not created");
   }

   for (i=0; i< NUMTHREADS; i++){
      if (pthread_join (my_tid[i], NULL) !=0)
         perror("no thread to join");
      else
         printf("joined with thread_id %d, x=%d, myglobal=%d\n",
                my_tid[i], x, myglobal);
   }
   exit (0);

}

void *doit(void *arg){
   int x= (int) arg;

   myglobal++;
   x = x + 10;

   printf("thread %d, x=%d, myglobal=%d\n", pthread_self(),
           x, myglobal);

   pthread_exit(0);
}
