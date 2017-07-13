/*---------------------------------------------------------------------------
 Program    : 2process.c
 Author     : Prof. Joseph NG (HKBU)
 Description: This program is to demonstrate the fork system call.
              It illustrates the return of the fork(), and the pid, as
              well as the ppid of the parent and the child processes.
 To compile : gcc -o 2process 2process.c
 To run     : 2process
----------------------------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
 int grandparent, parent, pid;

 grandparent = getppid();  /* get the parent pid */
 parent = getpid();        /* get the pid */

 if ((pid = fork()) == 0){ /* child process */
    printf("Child Process: pid return from fork()=%d getpid=%d parent=%d grandparent=%d getppid=%d\n",
    pid, getpid(), parent, grandparent, getppid());
 }else{ /* parent process */
    printf("Parent Process: pid return from fork()=%d getpid=%d parent=%d grandparent=%d getppid=%d\n",
    pid, getpid(), parent, grandparent, getppid());
 }
 return 0;
}
