/**********************************************************************

	    Project 4: Loading & Executing Modules

			 SECTION 0102


  File:        init.c
  Contents:    Test Module 1


**********************************************************************/

#include "queue.h"
#include "klib.h"
#include "io412.h"

typedef int (*Procptr) (int, char**);

int Proc (int argc , char ** argv)
{
  int scr , id2 , id3 ;
  Procptr p2 , p3 ;
  int   count ;         /* child process's argc */
  char * argument[10] ; /* pointer to a maximum of 10 arguments */

  scr = Create_semaphore ( "screen" , 1 ) ;

  P ( scr ) ;
  Cprintf("This is init.mod **** I will load modules A and B\n");
  V ( scr ) ;

  p2 = Load_module ( "test0a.mod" ) ;
  p3 = Load_module ( "test0b.mod" ) ;
  
  count = 3 ;
  argument [0] = "module A" ;
  argument [1] = "module A: 1st parameter" ;
  argument [2] = "module A: 2nd parameter" ;

  id2 = Proc_start ( p2 , count , argument ) ;
  if ( id2 < 1 )  {  /* Unable to proc_start */
    P ( scr ) ;
    Cprintf ( "init.mod is unable to proc_start in module A\n") ;
    Cprintf ( "process id = %2d \n" , id2 ) ;
    V( scr ) ;
  }

  count = 2 ;
  argument [0] = "module B" ;
  argument [1] = "module B: 1st parameter" ;

  id3 = Proc_start ( p3 , count , argument ) ;
  if ( id3 < 1 )  {  /* Unable to proc_start */
    P ( scr ) ;
    Cprintf ( "init.mod is unable to proc_start in module B\n") ;
    Cprintf ( "process id = %2d \n" , id3 ) ;
    V ( scr ) ;
  }

  P ( scr ) ;
  Cprintf( "Now init.mod will close sub modules A and B\n" ) ;
  V ( scr ) ;

  Close_module ( "test0a.mod" ) ;
  Close_module ( "test0b.mod" ) ;

  P ( scr ) ;
  Cprintf("init.mod has successfully closed A and B, and now terminates\n") ;
  V ( scr ) ;
}

/*********************************************************************/
/*                   End of File module1.c                           */
/*********************************************************************/
