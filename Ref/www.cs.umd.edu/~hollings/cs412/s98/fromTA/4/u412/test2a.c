/** test2a.c  --- A module for test 2.
    Christopher League

Test 2:
 - Load a module that doesn't exist.
 - Load a module n times, start it thrice, close n times
        (will exhaust memory unless you have ref counts)
 **/

#include "user412.h"
#include "io412.h"
// #include "klib.h"

/** Local prototypes, etc. */
//typedef int (*PROCPTR)( int, char** );

/** Arguments for children */
//int   child_argc   = 2;
//char* child_argv[] = {"test2a", "what a nice day"};


/***
 ***    Entry point
 ***/

int Proc( int argc, char** argv )
{
  Cprintf( "test2a:   Hello.\n" );
  Cprintf( "test2a:   argc == %d, argv == { '%s', '%s' }\n",
           argc, argv[0], (argc>=2)? argv[1] : "ERROR" );
  Cprintf( "test2a: [Terminating...]\n" );

  return 0;
}
