/** test2i.c  --- Init module for test 2.
    Christopher League

Test 2:
 - Load a module that doesn't exist.
 - Load a module n times, start it thrice, close n times
        (will exhaust memory unless you have ref counts)
 **/

#include "io412.h"
#include "klib.h"
#include <stdio.h>

/** Local prototypes, etc. */
typedef int (*PROCPTR)( int, char** );

/** Arguments for children */
int   child_argc   = 2;
char* child_argv[] = {"test2a", "what a nice day"};


/***
 ***    Entry point
 ***/

int Proc( int argc, char** argv )
{
  PROCPTR p;
  int     pid;
  int     i, j;
  
  Cprintf( "test2i:   Hello!\n" );
  Cprintf( "test2i: [Loading a NON-EXISTENT module]\n" );

  p = Load_module( "DOES NOT EXIST" );

  Cprintf( "test2i:   proc == %p  => ", p );
  if( p == NULL ) {
    Cprintf( "good!\n" );
  }
  else {
    Cprintf( "UH-OH!?\n" );
  }

  Get_char();

  //*****************************************

  for( i = 0;  i < 3;  i++ ) {
    /** load it 15 times */
    Cprintf( "test2i: [Loading test2a " );
    for( j = 0;  j < 15;  j++ ) {
      p = Load_module( "test2a.mod" );
      Cprintf( "." );
    }
    Cprintf( "]\n" );
    Cprintf( "test2i: [Starting test2a; proc == %p;  ", p );
    sprintf( child_argv[1], "#%d", i );
    pid = Proc_start( p, child_argc, child_argv );
    Cprintf( "pid == %d]\n", pid );
  }

  /** close it 45 times */
  Cprintf( "test2i: [Closing test2a " );
  for( i = 0;  i < 45;  i++ ) {
    Close_module( "test2a.mod" );
    Cprintf( "." );
  }
  Cprintf( "]\n" );

  Cprintf( "test2i: [Terminating...]\n" );

  return 0;
}
