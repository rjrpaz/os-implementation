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


**********************************************************************/

#include "cmsc412.h"
#include "keyboard.h"
#define NULL 0
#define DELAY 5000

int Process_a ( int , char ** ) ;
int Process_b ( int , char ** ) ;
int Process_c ( int , char ** ) ;
int Process_d ( int , char ** ) ;
int Process_e ( int , char ** ) ;
void Wait(void) ;

/*-------------------------------------------------------------------*/


int Init( int argc , char ** argv )
{
  int i ,     /* loop index */
		scr_sem,/* sid of screen semaphore */
		id ,    /* ID of child process */
		count ; /* child process's argc */

  char * argument[10] ; /* pointer to a maximum of 10 arguments */

  scr_sem = Create_semaphore ( "screen" , 1 )  ;

  P ( scr_sem ) ;
  Cprintf ("\n*************This is the init procedure*********\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
	 Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  Cprintf ("Now I will create three child processes\n") ;
  V ( scr_sem ) ;

  count = 3 ;
  argument [0] = "Process_a" ;
  argument [1] = "Proc_A 1st parameter" ;
  argument [2] = "Proc_A 2nd parameter" ;

  id = Proc_start ( Process_a , count , argument ) ;

  P ( scr_sem ) ;
  Cprintf ("***********Process A has been created with ID = %2d*****\n",id);
  V ( scr_sem ) ;

  Wait () ;

  count = 4 ;
  argument [0] = "Process_B" ;
  argument [1] = "Proc_B 1st parameter" ;
  argument [2] = "Proc_B 2nd parameter" ;
  argument [3] = "Proc_B 3rd parameter" ;

  id = Proc_start ( Process_b , count , argument ) ;

  P ( scr_sem ) ;
  Cprintf ("#######Process B has been created with ID = %2d#######\n",id);
  V ( scr_sem ) ;

  Wait() ;

  count = 2 ;
  argument [0] = "Process_C" ;
  argument [1] = "Proc_C 1st parameter" ;

  id = Proc_start ( Process_c , count , argument ) ;

  P ( scr_sem ) ;
  Cprintf ("$$$$$$Process C has been created with ID = %2d$$$$$$$$$\n",id);
  V ( scr_sem ) ;
  Wait() ;

	 count = 2 ;
  argument [0] = "Process_D" ;
  argument [1] = "Proc_D 1st parameter" ;

  id = Proc_start ( Process_d , count , argument ) ;

  P ( scr_sem ) ;
  Cprintf ("$$$$$$Process D has been created with ID = %2d$$$$$$$$$\n",id);
  V ( scr_sem ) ;
  Wait() ;
	 count = 2 ;
  argument [0] = "Process_E" ;
  argument [1] = "Proc_E 1st parameter" ;

  id = Proc_start ( Process_e , count , argument ) ;

  P ( scr_sem ) ;
  Cprintf ("$$$$$$Process E has been created with ID = %2d$$$$$$$$$\n",id);
  V ( scr_sem ) ;
  Wait() ;
  for ( i = 0 ; i < DELAY/4 ; i++ )  {  /* delay loop */
	  P ( scr_sem ) ;
	  Cprintf ("******** INIT %2d ********\n" , i) ;
	  V ( scr_sem ) ;
  }

  P ( scr_sem ) ;
  Cprintf ("\nNow, init is going to terminate \n") ;
  V ( scr_sem ) ;

  return ( 0 );

}

/*-------------------------------------------------------------------*/

int Process_a (int argc , char ** argv )
{
  int i,j ;     /* loop index */
  int scr_sem ; /* id of screen semaphore */

  scr_sem = Create_semaphore ( "screen" , 1 ) ;   /* register for screen use */

  for ( i=0 ; i < DELAY ; i++) {
    P ( scr_sem ) ;
    Cprintf ("AAAA %2d\n" , i) ;
    V ( scr_sem ) ;
    for ( j=0 ; j<= 1000 ; j++ ) ;
  }

  P ( scr_sem ) ;
  Cprintf ("\nProcess A is TERMINATED\n") ;
  V ( scr_sem ) ;

  return ( 0 );

}

/*-------------------------------------------------------------------*/

int Process_b (int argc , char ** argv )
{
  int i,j ;     /* loop index */
  int scr_sem ; /* id of screen semaphore */

  scr_sem = Create_semaphore ( "screen" , 1 ) ;   /* register for screen use */

  for ( i=0 ; i < DELAY ; i++) {
	 P ( scr_sem ) ;
	 Cprintf ("             BBBB %2d\n" , i ) ;
	 V ( scr_sem ) ;
	 for ( j=0 ; j<= 1000 ; j++ ) ;
  }

  P ( scr_sem ) ;
  Cprintf ("\nProcess B is TERMINATED\n") ;
  V ( scr_sem ) ;
  return ( 0 );

}


/*-------------------------------------------------------------------*/

int Process_c (int argc , char ** argv )
{
  int i,j ;     /* loop index */
  int scr_sem ; /* id of screen semaphore */

  scr_sem = Create_semaphore ( "screen" , 1 ) ;   /* register for screen use */

  for ( i=0 ; i < DELAY ; i++) {
	 P ( scr_sem ) ;
	 Cprintf ("                       CCCC %2d\n" , i ) ;
	 V ( scr_sem ) ;
	 for ( j=0 ; j<= 1000 ; j++ ) ;
  }

  P ( scr_sem ) ;
  Cprintf ("\nProcess C is TERMINATED\n") ;
  V ( scr_sem ) ;
  return ( 0 );

}

int Process_d (int argc , char ** argv )
{
  int i,j ;     /* loop index */
  int scr_sem ; /* id of screen semaphore */

  scr_sem = Create_semaphore ( "screen" , 1 ) ;   /* register for screen use */

  for ( i=0 ; i < DELAY/8 ; i++) {
	 P ( scr_sem ) ;
	 Cprintf ("                          DDD %2d\n" , i ) ;
	 V ( scr_sem ) ;
	 for ( j=0 ; j<= 1000 ; j++ ) ;
  }

  P ( scr_sem ) ;
  Cprintf ("\nProcess D is TERMINATED\n") ;
  V ( scr_sem ) ;
  return ( 0 );

}

int Process_e (int argc , char ** argv )
{
  int i,j ;     /* loop index */
  int scr_sem ; /* id of screen semaphore */

  scr_sem = Create_semaphore ( "screen" , 1 ) ;   /* register for screen use */

  for ( i=0 ; i < DELAY/10 ; i++) {
	 P ( scr_sem ) ;
	 Cprintf ("                               EEE%2d\n" , i ) ;
	 V ( scr_sem ) ;
	 for ( j=0 ; j<= 1000 ; j++ ) ;
  }

  P ( scr_sem ) ;
  Cprintf ("\nProcess E is TERMINATED\n") ;
  V ( scr_sem ) ;
  return ( 0 );

}

/*-------------------------------------------------------------------*/

void Wait ()   /* wait for a key from keyboard */
{
//  Cprintf ("Press any key to continue\n") ;
//  Get_char() ;
}

/*********************************************************************/
/*                   End of File proc.c                              */
/*********************************************************************/
