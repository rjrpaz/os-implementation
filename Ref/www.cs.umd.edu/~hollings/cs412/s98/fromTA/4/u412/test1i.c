/** test1i.c  --- Init module for test 1.
    Christopher League

Test 1 just loads a module, starts it, and closes it.
Both modules say hello, and that's about it.
It also tests Get_char, Kmalloc, and Kfree system calls.
 **/

#include "user412.h"
#include "io412.h"
// #include "klib.h"

/** Local prototypes, etc. */
typedef int (*PROCPTR)( int, char** );

/** Arguments for children */
int   child_argc   = 2;
char* child_argv[] = {"test1a", "what a nice day"};


/***
 ***    Entry point
 ***/

int Proc( int argc, char** argv )
{
  PROCPTR p;
  int     pid;
  char    c;
  
  Cprintf( "test1i:   Hello!\n" );
  Cprintf( "test1i: [Loading test1a...]\n" );

  p = Load_module( "test1a.mod" );

  Cprintf( "test1i:   proc == %p\n", p );
  Cprintf( "test1i: [Starting test1a...]\n" );

  pid = Proc_start( p, child_argc, child_argv );

  Cprintf( "test1i:   pid == %d\n", pid );
  Cprintf( "test1i: [Closing test1a...]\n" );

  Close_module( "test1a.mod" );

  Cprintf( "test1i: [Sleeping until keypress...]\n" );

  c = Get_char();

  Cprintf( "test1i:   c == '%c'\n", c );
  Cprintf( "test1i: [Terminating...]\n" );

  return 0;
}
