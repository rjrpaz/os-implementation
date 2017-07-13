/**********************************************************************

	    Project 2: Multiprogramming Part I

			 SECTION 0102


  By:
      Mohamed S. Aboutabl
  e-mail:
      aboutabl@cs.umd.edu

  File:        proc.c
  Creadted on: 10/3/1992
  Contents:    User Processes to be run concurrently
	  1.    init() : Initial process, creates other ones & terminates
	  2..4  process_a() , _b() , and _c() are three examplar processes
	  that are to be run concurently
	  5.    wait() : busy wait for a key to be pressed

   Modified by: Charles Lin
   Modified on: February 12, 1996
     Changed naming conventions.

**********************************************************************/

#include "cmsc412.h"
#include "keyboard.h"
#define NULL 0

/* Prototypes */

int   Process_a ( int , char ** ) ;
int   Process_b ( int , char ** ) ;
int   Process_c ( int , char ** ) ;
void  Wait() ;

/*-------------------------------------------------------------------*/


int Init( int argc , char ** argv )
{
  int i ,     /* loop index */
      id ,    /* ID of child process */
      count ; /* child process's argc */

  char * argument[10] ; /* pointer to a maximum of 10 arguments */

  Cprintf ("\nThis is the init procedure\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  Cprintf ("Now I will create three child processes\n") ;

  count = 3 ;
  argument [0] = "process_a" ;
  argument [1] = "Proc_A 1st parameter" ;
  argument [2] = "Proc_A 2nd parameter" ;

  id = Proc_start ( Process_a , count , argument ) ;

  Cprintf ("Process A has been created with ID = %2d\n" , id ) ;
  Wait () ;

  count = 4 ;
  argument [0] = "process_B" ;
  argument [1] = "Proc_B 1st parameter" ;
  argument [2] = "Proc_B 2nd parameter" ;
  argument [3] = "Proc_B 3rd parameter" ;

  id = Proc_start ( Process_b , count , argument ) ;

  Cprintf ("Process B has been created with ID = %2d\n" , id ) ;
  Wait() ;

  count = 2 ;
  argument [0] = "Process_C" ;
  argument [1] = "Proc_C 1st parameter" ;

  id = Proc_start ( Process_c , count , argument ) ;

  Cprintf ("Process C has been created with ID = %2d\n" , id ) ;
  Wait() ;

  Cprintf ("\nNow, init is going to terminate \n") ;
  return ( 0 );

}

/*-------------------------------------------------------------------*/

int Process_a ( int argc , char ** argv )
{
  int i ;     /* loop index */

  Cprintf ("\nThis is procedure A\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  Cprintf ("Now process A starts looping\n") ;
  for ( i=1 ; i <= 2 ; i++) {
    Cprintf ("This is loop no %2d of process A\n" , i ) ;
    Cprintf ("\t\tProcess A will now yield\n");
    Wait() ;
    Yield() ;
  }

  Cprintf ("\nProcess A is TERMINATED\n") ;
  return ( 0 );

}

/*-------------------------------------------------------------------*/

int Process_b ( int argc , char ** argv )
{
  int i ;     /* loop index */

  Cprintf ("\nThis is procedure B\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  Cprintf ("Now process B starts looping\n") ;
  for ( i=1 ; i <= 3 ; i++) {
    Cprintf ("This is loop no %2d of process B\n" , i ) ;
    Cprintf ("\t\tProcess B will now yield\n");
    Wait() ;
    Yield() ;
  }

  Cprintf ("\nProcess B is TERMINATED\n") ;
  return ( 0 );

}


/*-------------------------------------------------------------------*/

int Process_c ( int argc , char ** argv )
{
  int i ;     /* loop index */

  Cprintf ("\nThis is procedure C\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  Cprintf ("Now process C starts looping\n") ;
  for ( i=1 ; i <= 4 ; i++) {
    Cprintf ("This is loop no %2d of process C\n" , i ) ;
    Cprintf ("\t\tProcess C will now yield\n");
    Wait() ;
    Yield() ;
  }

  Cprintf ("\nProcess C is TERMINATED\n") ;
  return ( 0 );

}

/*-------------------------------------------------------------------*/

void Wait ()   /* wait for a key from keyboard */
{
  Cprintf ("Press any key to continue\n") ;
  while( Get_char() == NULL ) ;
}

/*********************************************************************/
/*                   End of File proc.c                              */
/*********************************************************************/
