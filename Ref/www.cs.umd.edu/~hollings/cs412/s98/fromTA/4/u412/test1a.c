/** test1a.c  --- A module for test 1.
    Christopher League

Test 1 just loads a module, starts it, and closes it.
Both modules say hello, and that's about it.
It also tests Get_char, Kmalloc, and Kfree system calls.
 **/

#include "user412.h"
#include "io412.h"
// #include "klib.h"
#include <string.h>

/** Local prototypes, etc. */
//typedef int (*PROCPTR)( int, char** );

/** Arguments for children */
//int   child_argc   = 2;
//char* child_argv[] = {"test1a", "what a nice day"};


/***
 ***    Entry point
 ***/

int Proc( int argc, char** argv )
{
  char* buf;
  int   i;

  Cprintf( "test1a:   Hello.\n" );
  Cprintf( "test1a:   argc == %d, argv == { '%s', '%s' }\n",
           argc, argv[0], (argc>=2)? argv[1] : "ERROR" );

  Cprintf( "test1a: [Allocating buffer...]\n" );

  buf = Kmalloc( 20 );

  Cprintf( "test1a:   buf == %p\n", buf );
  Cprintf( "test1a: [Writing to buffer...]\n" );

  strcpy( buf, "<AbCdEfG" );
  strcat( buf, "hIjKlMn>" );

  Cprintf( "test1a:   buf SHOULD contain '<AbCdEfGhIjKlMn>'\n" );
  Cprintf( "test1a:   buf DOES   contain '%s'\n", buf );
  Cprintf( "test1a: [Freeing buffer...]\n" );

  Kfree( buf );

  Cprintf( "test1a: [Looping Kmalloc/Kfree...]\n" );

  for( i = 0;  i < 10;  i++ ) {
    buf = Kmalloc( 20 );
    strcpy( buf, "<AbCdEfG" );
    strcat( buf, "hIjKlMn>" );
    Cprintf( "[%p  '%s']  %s", buf, buf, (i%2==1)? "\n" : "" );
    Kfree( buf );
  }

  Cprintf( "test1a: [Terminating...]\n" );

  return 0;
}
