/*---------------------------------------------------------------------------
 Program    : 3process.c
 Author     : Dr. Joseph NG (HKBU)
 Description: This program will spawn 3 processes and to demonstrate
              the fork system call.
 To compile : gcc -o 3process 3process.c
 To run     : 3process
----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

main()
{
 int pid;
 int parent = 0; /* you can use this flag to identify the parent */

 if ((pid = fork()) == 0){    /* child process */
    printf("1st Child Process: pid=%d ppid=%d sleeping .....\n",
    getpid(), getppid());
    sleep(5); /* sleep for 5 sec. */
 }else{
    if ((pid = fork()) == 0){ /* 2nd child process */
       printf("2nd Child Process: pid=%d ppid=%d sleeping .....\n",
       getpid(), getppid());
       sleep(5); /* sleep for 5 sec. */
    }else{                    /* parent process */
       parent = 1; /* set the flag to indicate this is the parent */
       printf("Parent Process: pid=%d ppid=%d exiting\n",
       getpid(), getppid());
    }

    if (parent){ /* if it is the parent, we can do something special here */
       /* For example, the parent can issue sem_rm( ) here */
       exit(0);
    }
 }
}
