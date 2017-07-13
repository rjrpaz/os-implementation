/** test4i.c  --- Init module for test 4.
    Christopher League

     Test 4: Loads and immediately closes a module N times,
             without starting anything.  This forces you to load
             and relocate, unload and free.  Crimson and clover,
             over and over.
 **/

#include "user412.h"
#include "io412.h"
// #include "klib.h"
#include <string.h>

/** Local prototypes, etc. */
typedef int (*PROCPTR)( int, char** );

int   child_argc = 1;
char* child_argv[] = {"MSG"}; 

/***
 ***    Entry point
 ***/

#define ITERS 50

int Proc( int argc, char** argv )
{
  PROCPTR p;
  int     i;

  if( strcmp( argv[0], child_argv[0] ) == 0 ) {
    Cprintf( "\n\n\n" );
    Cprintf( "The purpose of this test has been to see if your\n" );
    Cprintf( "Load/relocation routines can take the beating of\n" );
    Cprintf( "repeatedly doing their job, and then having it\n" );
    Cprintf( "discarded.  Yeah, it's mean, but guess what...\n" );
    Cprintf( "If you are reading this,\n" );
    Cprintf( "              YOU SURVIVED!!!\n" );
    Cprintf( "[press key]\n" );
    Get_char();
  }
  else {
    Cprintf( "test4i: [Loading and Closing, over and over]\n" );
    for( i = 0;  i < ITERS;  i++ ) {
      p = Load_module( "test4i.mod" );
      Cprintf( "[%2d, %p]\n", i, p );
      Close_module( "test4i.mod" );
    }

    p = Load_module( "test4i.mod" );
    Proc_start( p, child_argc, child_argv );
    Close_module( "test4i.mod" );
  }
  return 0;
}
