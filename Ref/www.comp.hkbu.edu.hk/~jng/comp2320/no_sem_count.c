/*---------------------------------------------------------------------------
 Program    : no_sem_count.c
 Author     : Dr. Joseph NG (HKBU)
 Description: This program is to demonstrate mutual exclusion with no
              semaphore. Without the semaphore guiding the critical
              section, the seqno will not be properly updated.
 To compile : gcc -o no_sem_count no_sem_count.c
 To run     : no_sem_count 1 &; no_sem_count 0 &
 Note: You have to create a file called "seqnum" with a "0" in it, before 
       running the programs.
----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* #include "sem_pack.h" */

#define SEQFILE "seqnum" /* filename, it should contain an integer */
/* #define SEMKEY ((key_t) 23456L) */

int main(int argc, char* argv[])
{
 /* int semid; */
 int i, n, pid, seqno;
 int duration;
 FILE *fptr;

 if (argc > 1){
    duration = atoi(argv[1]);
    pid = getpid();
/*
    if ((semid = sem_create(SEMKEY, 1)) < 0)
       printf("can't open semaphore");
*/
    for (i = 0; i < 10; i++){
        /* sem_wait(semid);*/
        fptr = fopen(SEQFILE, "r");
        fscanf(fptr, "%d\n", &seqno);
        fclose(fptr);

        printf("pid = %d, seq# = %d\n", pid, seqno);
        seqno++;          /* increment the seq. no. */
        sleep(duration);

        fptr = fopen(SEQFILE, "w");
        fprintf(fptr, "%d\n", seqno);
        fclose(fptr);
        /* sem_signal(semid); */
        usleep(250); /* sleep for 250 msec */
    }
    /* sem_rm(semid); */
 }else{
    printf("Usage: %s <duration>\n", argv[0]);
    exit(0);
 }
}
