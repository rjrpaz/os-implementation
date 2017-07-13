/*
 * ========================================================================
 *
 *	    Project 4: Loading & Executing Modules
 *
 *  File:        module2.c
 *  Created on:  11/11/1992
 * 
 *  Contents:    Test Module 2
 * 
 *  Modified by:   Charles Lin
 *  Modified on:   April 2, 1996
 *  Modifications: Changes to naming conventions (capitalize first letter
 *                 of function names)
 *
 * ========================================================================
 */

#include "user412.h"
// #include "queue.h"
// #include "klib.h"
#include "io412.h"

int Proc2_1() ;
int Proc2_2() ;

int Proc( int argc , char ** argv )
{
  int scr , id1 , id2 ;
  int   count ;         /* child process's argc */
  char * argument[10] ; /* pointer to a maximum of 10 arguments */

  scr = Create_semaphore ( "screen" , 1) ;

  P ( scr ) ;
  Cprintf ( "This is proc of module # 2 *** Two child processes follow\n" ) ;
  V ( scr ) ;

  count = 2 ;
  argument [0] = "process 2_1" ;
  argument [1] = "process 2_1 1st parameter" ;

  id1 = Proc_start ( Proc2_1 , count , argument ) ;
  if ( id1 < 1 ) {
    P ( scr ) ;
    Cprintf("Module 2 is unable to proc_start process 2_1 id = %2d\n",id1);
    V ( scr ) ;
  }

  count = 2 ;
  argument [0] = "process 2_2" ;
  argument [1] = "process 2_2 1st parameter" ;
  id2 = Proc_start ( Proc2_2 , count , argument ) ;
  if ( id2 < 1 ) {
    P ( scr ) ;
    Cprintf("Module 2 is unable to proc_start process 2_2 id = %2d\n",id2);
    V ( scr ) ;
  }
  
  P ( scr ) ;
  Cprintf( "Module 2 now terminates\n") ;
  V ( scr ) ;
}

/*-------------------------------------------------------------------*/

int Proc2_1 ( int argc , char ** argv )
{
  int scr ;

  scr = Create_semaphore ( "screen" , 1 ) ;

  P ( scr ) ;
  Cprintf ( "This is proc2_1 of module # 2\n" ) ;
  V ( scr ) ;
}

/*-------------------------------------------------------------------*/

int Proc2_2 ( int argc , char ** argv )
{
  int scr ;

  scr = Create_semaphore ( "screen" , 1 ) ;

  P ( scr ) ;
  Cprintf ( "This is proc2_2 of module # 2\n" ) ;
  V ( scr ) ;
}

/*********************************************************************/
/*                   End of File module2.c                           */
/*********************************************************************/
