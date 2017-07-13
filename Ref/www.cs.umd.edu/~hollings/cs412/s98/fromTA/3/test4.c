/**********************************************************************
  proc4.c -- Tests whether semaphores get recycled...

	It will exhaust all your semaphores 5 times over.
	Each time, you should see
		passX: I've EXHAUSTED all your semaphores.
		passX: =======> There were 20            // or whatever
					  ^^^^
          If this value keeps decreasing, we're in trouble.

  [4] A Gerber's test #4.

  **********************************************************************/

#include "cmsc412.h"
#include "user412.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

void Wait ( int scr_sem );   /* wait for a key from keyboard */

/* global */
int p1passes = 0;

#define MAX_PASSES 4

int P1( int, char** );

int dumbArgc = 1;
char* dumbArgv[] = {"dumb"};

int Init( int ignoreC , char ** ignoreV )
{
  int r;
  int scrsem = Create_semaphore( "screen", 1 );
  
  // Test Illegal use of semaphores
  r = P(scrsem);
  Cprintf ("\nTest 4 - semaphores. %d passes\n\n", MAX_PASSES );
  Cprintf("Init: Legal use of P returns %d\n", r );
  r = P(5);
  Cprintf("Init: ILLegal use of P returns %d\n", r );
  r = V(scrsem);
  Cprintf("Init: Legal use of V returns %d\n", r );
  r = V(6);
  Cprintf("Init: ILLegal use of V returns %d\n", r );

  Wait(scrsem);

  Proc_start( P1, dumbArgc, dumbArgv );
  return 0;
}

int P1( int ignoreC, char** ignoreV )
{
  int scrsem = Create_semaphore( "screen", 1 );
  int s, i = 0;
  Cprintf( "pass%d: Create_sem( screen ) => %d\n", p1passes, scrsem );
  do {
    char nm[] = "a++";
    nm[0] = 'A' + (i++);    // make a unique name
    // try to create a sem
    s = Create_semaphore( nm, 3 );  // ival doesn't matter
    Cprintf( "pass%d: Create_sem( \"%s\" ) => %d\n", p1passes, nm, s );
    // check return value
  } while( s >= 0 );
  Cprintf( "pass%d:  I've EXHAUSTED all your semaphores!\n", p1passes );
  Cprintf( "pass%d:  ======> There were %d.\n", p1passes, i );
  if( p1passes < MAX_PASSES ) {
    Cprintf( "pass%d: Now, I'm going to terminate to release them\n", p1passes );
    Cprintf( "pass%d: and do it all over again!\n", p1passes );
    Wait(scrsem);
    p1passes ++;
    Proc_start( P1, dumbArgc, dumbArgv );
    return 0;
  }
  else {
    Cprintf( "pass%d: Ok, that does it...\n", p1passes );
    Wait(scrsem);
  }

  return (0);
}

/*-------------------------------------------------------------------*/

void Wait ( int scr_sem )   /* wait for a key from keyboard */
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


