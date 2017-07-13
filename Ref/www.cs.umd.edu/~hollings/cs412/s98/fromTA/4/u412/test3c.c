/** test3c.c  --- C module for test 3.
    Christopher League

     Test 3: Chain of loads, WITH cycles, AND a process
             started WITHIN modules.
             Assuming FIFO, it will print a sequence of codes, and
             then a special message.
 **/

#include "user412.h"
#include "io412.h"
// #include "klib.h"
#include <string.h>

/***
 ***    Entry point
 ***/

int Proc( int argc, char** argv )
{
  if( argv[0][0] != 'M' ) {
    Cprintf( "C%s ", argv[1] );
  }
  else {
    if( argv[1][0] == '3' ) {
      Cprintf( "st anything!\n" );
      Cprintf( "[press a key]\n" );
      Get_char();
    }
    else {
      Cprintf( "sing\nyou " );
    }
  }
  return 0;
}


