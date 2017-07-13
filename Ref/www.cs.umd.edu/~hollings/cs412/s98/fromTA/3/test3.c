/**********************************************************************
  proc3.c -- Tests semaphores by implementing control-flow based
	  on P() and V().

  Basically, we're implementing the partially-ordered preference
  graph in #9 on "Review Questions for Exam 1".
  Only each one happens in a LOOP!

  Output should be:
	 P1(0)  P3(0)  P2(0)  P5(0)  P4(0)  P6(0)  P7(0)
	 P1(1)  P3(1)  P2(1)  P5(1)  P4(1)  P6(1)  P7(1)
	 P1(2)  P3(2)  P2(2)  P5(2)  P4(2)  P6(2)  P7(2)
	 P1(3)  P3(3)  P2(3)  P5(3)  P4(3)  P6(3)  P7(3)
	 :  etc

  [3] A Gerber's test #3.

**********************************************************************/

#include "cmsc412.h"
#include "user412.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define ITERS 20

void Wait (void);
void WaitSem ( int scr_sem );   /* wait for a key from keyboard */
void busyWait ( int );
int  P1( int, char** );
int  P2( int, char** );
int  P3( int, char** );
int  P4( int, char** );
int  P5( int, char** );
int  P6( int, char** );
int  P7( int, char** );

int dumbArgc = 1;
char* dumbArgv[] = {"dumb"}; 

int Init( int ignoreC , char ** ignoreV )
{
  int scr_sem;

  scr_sem = Create_semaphore ( "screen" , 1 )  ;

  P(scr_sem);
  Cprintf ("\nTest 3: %d iterations\n\n", ITERS );
  Cprintf( "Init: Starting P1..P7.\n" );
  V(scr_sem);

  /** Create them in a pathological order, so they don't
		come out ok just by chance.  Brwwwhahahaa!! */
  Proc_start( P7, dumbArgc, dumbArgv );
  Proc_start( P6, dumbArgc, dumbArgv );
  Proc_start( P5, dumbArgc, dumbArgv );
  Proc_start( P4, dumbArgc, dumbArgv );
  Proc_start( P3, dumbArgc, dumbArgv );
  Proc_start( P2, dumbArgc, dumbArgv );
  Proc_start( P1, dumbArgc, dumbArgv );

  P(scr_sem);
  Cprintf( "Init: P1..P7 started... Init terminating!\n" );
  Wait( );
  V(scr_sem);
  return 0;
}


int loopIter = 0;


int P1( int ignoreC, char** ignoreV )
{
  int p1sem  = Create_semaphore( "P1sem", 0 );
  int scrsem = Create_semaphore( "screen", 1 );
  int loop1  = Create_semaphore( "loop1", 1 );
  int i;
  for( i = 0;  i < ITERS;  i++ ) {
    P(loop1);
    P(scrsem);
    Cprintf( "P1(%2d)  ", i );
    V(scrsem);
    V(p1sem);
    V(p1sem);
  }
  return 0;
}

int P2( int ignoreC, char** ignoreV )
{
  int p1sem  = Create_semaphore( "P1sem", 0 );
  int p2sem  = Create_semaphore( "P2sem", 0 );
  int scrsem = Create_semaphore( "screen", 1 );
  int loop2  = Create_semaphore( "loop2", 1 );
  int i;
  for( i = 0;  i < ITERS;  i++ ) {
    P(loop2);
    P(p1sem);
    P(scrsem);
    Cprintf( "P2(%2d)  ", i );
    V(scrsem);
    V(p2sem);
    V(p2sem);
  }
  return 0;
}

int P3( int ignoreC, char** ignoreV )
{
  int p1sem  = Create_semaphore( "P1sem", 0 );
  int p3sem  = Create_semaphore( "P3sem", 0 );
  int scrsem = Create_semaphore( "screen", 1 );
  int loop3  = Create_semaphore( "loop3", 1 );
  int i;
  for( i = 0;  i < ITERS;  i++ ) {
    P(loop3);
    P(p1sem);
    P(scrsem);
    Cprintf( "P3(%2d)  ", i );
    V(scrsem);
    V(p3sem);
  }
  return 0;
}

int P4( int ignoreC, char** ignoreV )
{
  int p2sem  = Create_semaphore( "P2sem", 0 );
  int p4sem  = Create_semaphore( "P4sem", 0 );
  int scrsem = Create_semaphore( "screen", 1 );
  int loop4  = Create_semaphore( "loop4", 1 );
  int i;
  for( i = 0;  i < ITERS;  i++ ) {
    P(loop4);
    P(p2sem);
    P(scrsem);
    Cprintf( "P4(%2d)  ", i );
    V(scrsem);
    V(p4sem);
  }
  return 0;
}

int P5( int ignoreC, char** ignoreV )
{
  int p2sem  = Create_semaphore( "P2sem", 0 );
  int p5sem  = Create_semaphore( "P5sem", 0 );
  int scrsem = Create_semaphore( "screen", 1 );
  int loop5  = Create_semaphore( "loop5", 1 );
  int i;
  for( i = 0;  i < ITERS;  i++ ) {
    P(loop5);
    P(p2sem);
    P(scrsem);
    Cprintf( "P5(%2d)  ", i );
    V(scrsem);
    V(p5sem);
  }
  return 0;
}

int P6( int ignoreC, char** ignoreV )
{
  int p3sem  = Create_semaphore( "P3sem", 0 );
  int p5sem  = Create_semaphore( "P5sem", 0 );
  int p6sem  = Create_semaphore( "P6sem", 0 );
  int scrsem = Create_semaphore( "screen", 1 );
  int loop6  = Create_semaphore( "loop6", 1 );
  int i;
  for( i = 0;  i < ITERS;  i++ ) {
    P(loop6);
    P(p3sem);
    P(p5sem);
    P(scrsem);
    Cprintf( "P6(%2d)  ", i );
    V(scrsem);
    V(p6sem);
  }
  return 0;
}

int P7( int ignoreC, char** ignoreV )
{
  int p4sem  = Create_semaphore( "P4sem", 0 );
  int p6sem  = Create_semaphore( "P6sem", 0 );
  int scrsem = Create_semaphore( "screen", 1 );
  int loop1  = Create_semaphore( "loop1", 1 );
  int loop2  = Create_semaphore( "loop2", 1 );
  int loop3  = Create_semaphore( "loop3", 1 );
  int loop4  = Create_semaphore( "loop4", 1 );
  int loop5  = Create_semaphore( "loop5", 1 );
  int loop6  = Create_semaphore( "loop6", 1 );
  int i;
  for( i = 0;  i < ITERS;  i++ ) {
    P(p4sem);
    P(p6sem);
    P(scrsem);
    Cprintf( "P7(%2d)\n", i );
    V(scrsem);
    // tell everyone else it's ok to loop again.
    V(loop6);  V(loop5);  V(loop4);
    V(loop3);  V(loop2);  V(loop1);
  }
  WaitSem( scrsem );
  return 0;
}


/*-------------------------------------------------------------------*/

void Wait (void)   /* wait for a key from keyboard */
{
  Cprintf ("Press any key to continue... ") ;
  Get_char() ;
  Cprintf ("\n");
}

void WaitSem ( int scr_sem )   /* wait for a key from keyboard */
{
  P( scr_sem );
  Cprintf ("Press any key to continue\n") ;
  V( scr_sem );
  Get_char() ;
}

void busyWait ( int scr_sem )
{
  int i;
  for( i = 0;  i < 32000;  i++ ) {
	 if( i%1000 == 0 ) {
		P( scr_sem );
		Cprintf( "." );
		V( scr_sem );
	 }
  }
}
