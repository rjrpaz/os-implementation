/** test3a.c  --- A module for test 3.
    Christopher League

     Test 3: Chain of loads, WITH cycles, AND a process
             started WITHIN modules.
             Assuming FIFO, it will print a sequence of codes, and
             then a special message.
 **/

#include "io412.h"
#include "klib.h"
#include <stdlib.h>
#include <string.h>

/** Local prototypes, etc. */
typedef int (*PROCPTR)( int, char** );

/** Arguments for children */
int   q_argc   = 1;
char* q_argv[] = {"A"};

int   b_argc   = 2;
char* b_argv[] = {"test3b", "2"};
      
int   c_argc   = 2;
char* c_argv[] = {"test3c", "2"};
      
/** Aux function **/
int test3a_Q( int argc, char** argv )
{
  if( argv[0][0] != 'M' ) {
    Cprintf( "%s.q ", argv[0] );
  }
  else {
    Cprintf( "notes to " );
  }
  return 0;
}

/***
 ***    Entry point
 ***/

int Proc( int argc, char** argv )
{
  struct {
    PROCPTR p;
    int     pid;
  } q, b, c;

  if( argv[0][0] == 'M' ) {
    strcpy( q_argv[0], "M" );
    strcpy( b_argv[0], "M" );
    strcpy( c_argv[0], "M" );
  }

  q.p = NULL;
  q.pid = Proc_start( test3a_Q, q_argc, q_argv );
  if( q.pid < 0 ) goto hell;
//**
  c.p = Load_module( "test3c.mod" );
  if( c.p == NULL ) goto hell;

  c.pid = Proc_start( c.p, c_argc, c_argv );
  if( c.pid < 0 ) goto hell;
//**
  b.p = Load_module( "test3b.mod" );
  if( b.p == NULL ) goto hell;

  b.pid = Proc_start( b.p, b_argc, b_argv );
  if( b.pid < 0 ) goto hell;
//**
  Close_module( "test3b.mod" );
  Close_module( "test3c.mod" );

  if( argv[0][0] != 'M' ) {
    Cprintf( "A%s ", argv[1] );
  }
  else {
    Cprintf( "When you " );
  }
  return 0;

hell:
  Cprintf( "test3a: Load/Start failure!\n" );
  Cprintf( "test3a: [aborting...]\n" );
  return 1;
}
