/**********************************************************************

	    Project 2: Multiprogramming Part I

			 SECTION 0102


  By:
      Mohamed S. Aboutabl
  e-mail:
      aboutabl@cs.umd.edu

  File:        kernel.h
  Creadted on: 10/3/1992
  Contents:    Kernel Functions declaration


**********************************************************************/

#define INT_ENABLE 0x0200
#define INT_DISABLE 0

/* Stack size is given in words */
#define STACK_SIZE  1024

/* System functions */
#define PROC_START 1
#define PROC_TERM  2
#define DEFER      3
#define NULL 0L

int  Proc_start ( int (*)() , int , char ** ) ;
void Proc_term (void) ;
void Yield (void) ;
void interrupt System_service () ;
void interrupt Yield_process () ;
void Scheduler (void) ;
void Dispatch (void) ;
void Shutdown (void) ;

/*********************************************************************/
/*                   End of File kernel.h                            */
/*********************************************************************/
