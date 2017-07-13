/**********************************************************************

	    Project 2: Multiprogramming Part I

			 SECTION 0102


  By:
      Mohamed S. Aboutabl
  e-mail:
      aboutabl@cs.umd.edu

  File:        kernel.h
  Created on:  10/3/1992
  Contents:    Kernel Functions declaration

  Modified by : Charles Lin
  Modifications: A few name changes to follow convention
  Date:          February 12, 1996

  You may wish to add or change some functions, though it would be
 preferable if you did not.
**********************************************************************/

#define INT_ENABLE 0x0200
#define INT_DISABLE 0

/* Stack size is given in words. A word is 2 bytes. */
#define STACK_SIZE  1024

/* System functions */
#define PROC_START 1
#define PROC_TERM  2
#define DEFER      3
#define NULL 0L

int             Proc_start ( int (*)() , int , char ** ) ;
void            Proc_term () ;
void            Yield () ;
void interrupt  System_service () ;
void interrupt  Yield_process () ;
void            Scheduler () ;
void            Dispatch () ;
void            Shutdown () ;

/*********************************************************************/
/*                   End of File kernel.h                            */
/*********************************************************************/
