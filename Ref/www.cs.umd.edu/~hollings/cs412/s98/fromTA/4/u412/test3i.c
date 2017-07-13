/** test3i.c  --- Init module for test 3.
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
int   a_argc   = 2;
char* a_argv[] = {"test3a", "1"};  /* depth */

int   b_argc   = 2;
char* b_argv[] = {"test3b", "1"};
      


/***
 ***    Entry point
 ***/

int Proc( int argc, char** argv )
{
  struct {
    PROCPTR p;
    int     pid;
  } a, b;
 
  Cprintf( "test3i: [Spawning like you wouldn't believe...]\n" );

  a.p = Load_module( "test3a.mod" );
  if( a.p == NULL ) goto hell;

  a.pid = Proc_start( a.p, a_argc, a_argv );
  if( a.pid < 0 ) goto hell;

  b.p = Load_module( "test3b.mod" );
  if( b.p == NULL ) goto hell;

  b.pid = Proc_start( b.p, b_argc, b_argv );
  if( b.pid < 0 ) goto hell;

  Close_module( "test3a.mod" );
  Close_module( "test3b.mod" );
  
  Cprintf( "test3i: Ok.  I expect the following sequence:\n" );
  Cprintf( "test3i: A1 B1 A.q C2 B2 C3\n" );
  Cprintf( "test3i: [press a key, and we'll do something !!FUN!!]\n" );

  Get_char();

  //*****************************************
  //**  NOW FOR THE FUN
  //*****************************************
  strcpy( a_argv[0], "M" );
  strcpy( b_argv[0], "M" );

  a.p = Load_module( "test3a.mod" );
  if( a.p == NULL ) goto hell;

  a.pid = Proc_start( a.p, a_argc, a_argv );
  if( a.pid < 0 ) goto hell;

  b.p = Load_module( "test3b.mod" );
  if( b.p == NULL ) goto hell;

  b.pid = Proc_start( b.p, b_argc, b_argv );
  if( b.pid < 0 ) goto hell;

  Close_module( "test3a.mod" );
  Close_module( "test3b.mod" );

  Cprintf("test3i: Here we go!\n" );
  return 0;

hell:
  Cprintf( "test3a: Load/Start failure!\n" );
  Cprintf( "test3a: [aborting...]\n" );
  return 1;
}


