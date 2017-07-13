/*
 * ========================================================================
 *
 *	    Project 4: Loading & Executing Modules
 *
 *			 SECTION 0102
 *
 *
 *  File:        module3.c
 *  Created on:  11/11/1992
 * 
 *  Contents:    Test Module 3
 * 
 *  Modified on:   April 2, 1996
 *  Modifications: Changes to naming conventions (capitalize first letter
 *                 of function names)
 *
 * ========================================================================
 */

#include "queue.h"
#include "klib.h"
#include "io412.h"

int Proc3_1 () ;
int Proc3_2 () ;

int Proc (int argc , char ** argv)
{
  int scr , id1 , id2 ;
  int   count ;         /* child process's argc */
  char * argument[10] ; /* pointer to a maximum of 10 arguments */

  scr = Create_semaphore ( "screen" , 1 ) ;

  P ( scr ) ;
  Cprintf ("This is proc of module # 3 *** Two child processes follow\n") ;
  V ( scr ) ;

  count = 2 ;
  argument [0] = "Process 3_1" ;
  argument [1] = "Process 3_1 1st parameter" ;

  id1 = Proc_start ( Proc3_1 , count , argument ) ;
  if ( id1 < 1 ) {
    P ( scr ) ;
    Cprintf( "Module 3 is unable to proc_start process 3_1 id = %2d\n", id1 );
    V ( scr ) ;
  }

  count = 2 ;
  argument [0] = "Process 3_2" ;
  argument [1] = "Process 3_2 1st parameter" ;

  id2 = Proc_start ( Proc3_2 , count , argument ) ;
  if ( id2 < 1 ) {
    P ( scr ) ;
    Cprintf( "Module 3 is unable to proc_start process 3_2 id = %2d\n", id2 );
    V ( scr ) ;
  }

  P ( scr ) ;
  Cprintf( "Module 3 now terminates\n" ) ;
  V ( scr ) ;
}

/*-------------------------------------------------------------------*/

int Proc3_1 ( int argc , char ** argv )
{
  int scr ;

  scr = Create_semaphore ( "screen" , 1 ) ;

  P ( scr ) ;
  Cprintf ( "This is proc3_1 of module # 3\n" ) ;
  V ( scr ) ;
}


/*-------------------------------------------------------------------*/

int Proc3_2 ( int argc , char ** argv )
{
  int scr ;

  scr = Create_semaphore ( "screen" , 1 ) ;

  P ( scr ) ;
  Cprintf ( "This is proc3_2 of module # 3\n" ) ;
  V ( scr ) ;
}


/*********************************************************************/
/*                   End of File module3.c                           */
/*********************************************************************/
