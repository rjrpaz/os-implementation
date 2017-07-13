/** test3b.c  --- B module for test 3.
    Christopher League

     Test 3: Chain of loads, WITH cycles, AND a process
             started WITHIN modules.
             Assuming FIFO, it will print a sequence of codes, and
             then a special message.
 **/

#include "user412.h"
#include "io412.h"
// #include "klib.h"
#include <stdlib.h>
#include <string.h>

/** Local prototypes, etc. */
typedef int (*PROCPTR)( int, char** );

/** Arguments for children */
int   c_argc   = 2;
char* c_argv[] = {"test3c", "3"};
      
/***
 ***    Entry point
 ***/

int Proc( int argc, char** argv )
{
  PROCPTR p;
  int     pid;

  if( argv[0][0] == 'M' ) {
    strcpy( c_argv[0], "M" );
  }

  if( argv[1][0] == '2' ) {

    p = Load_module( "test3c.mod" );
    if( p == NULL ) goto hell;

    pid = Proc_start( p, c_argc, c_argv );
    if( pid < 0 ) goto hell;

    Close_module( "test3c.mod" );
  }

  if( argv[0][0] != 'M' ) {
    Cprintf( "B%s ", argv[1] );
  }
  else {
    if( argv[1][0] == '2' ) {
      Cprintf( "can sing mo" );
    }
    else {
      Cprintf( "know the " );
    }
  }
  return 0;

hell:
  Cprintf( "test3b: Load/Start failure!\n" );
  Cprintf( "test3b: [aborting...]\n" );
  return 1;
}
