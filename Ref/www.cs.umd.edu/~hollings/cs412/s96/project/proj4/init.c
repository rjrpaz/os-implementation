/**********************************************************************

	    Project 4: Loading & Executing Modules

			 SECTION 0102


  By:
      Mohamed S. Aboutabl
  e-mail:
      aboutabl@cs.umd.edu

  File:        init.c
  Creadted on: 11/11/1992

  Contents:    Test Module 1


**********************************************************************/

#include "queue.h"
#include "klib.h"
#include "io412.h"

int Proc (int argc , char ** argv)
{
  int scr , id2 , id3 ;
  Procptr p2 , p3 ;
  int   count ;         /* child process's argc */
  char * argument[10] ; /* pointer to a maximum of 10 arguments */

  scr = Create_semaphore ( "screen" , 1 ) ;

  P ( scr ) ;
  Cprintf("This is init.mod **** I will load modules 2 and 3\n");
  V ( scr ) ;

  p2 = Load_module ( "module2.mod" ) ;
  p3 = Load_module ( "module3.mod" ) ;
  
  count = 3 ;
  argument [0] = "module_2" ;
  argument [1] = "module_2 1st parameter" ;
  argument [2] = "module_2 2nd parameter" ;

  id2 = Proc_start ( p2 , count , argument ) ;
  if ( id2 < 1 )  {  /* Unable to proc_start */
    P ( scr ) ;
    Cprintf ( "init.mod is unable to proc_start in module 2\n") ;
    Cprintf ( "process id = %2d \n" , id2 ) ;
    V( scr ) ;
  }

  count = 2 ;
  argument [0] = "module_3" ;
  argument [1] = "module_3 1st parameter" ;

  id3 = Proc_start ( p3 , count , argument ) ;
  if ( id3 < 1 )  {  /* Unable to proc_start */
    P ( scr ) ;
    Cprintf ( "init.mod is unable to proc_start in module 3\n") ;
    Cprintf ( "process id = %2d \n" , id3 ) ;
    V ( scr ) ;
  }

  P ( scr ) ;
  Cprintf( "Now init.mod will close sub modules 2 and 3\n" ) ;
  V ( scr ) ;

  Close_module ( "module2.mod" ) ;
  Close_module ( "module3.mod" ) ;

  P ( scr ) ;
  Cprintf("init.mod has successfully closed 2 and 3, and now terminates\n") ;
  V ( scr ) ;
}

/*********************************************************************/
/*                   End of File module1.c                           */
/*********************************************************************/
