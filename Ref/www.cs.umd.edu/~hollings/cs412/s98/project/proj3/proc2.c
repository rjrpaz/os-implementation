/**********************************************************************

	    Project 2: Multiprogramming Part I

			 SECTION 0102


  By:
      Jeff Hollingsworth
  e-mail:
      hollings@cs.umd.edu

  File:        proc2.c
  Created on:  3/6/1996
  Contents:    User Processes to be run concurrently
	  1.    Init() : Initial process, creates other ones & terminates
	  2..4  Long() , Ping() , and Pong() are three examplar processes
	  that are to be run concurently.  Long() is a CPU intensive
          job, while Ping() and Pong() bounce back between one another.
	  5.    Wait() : busy wait for a key to be pressed


**********************************************************************/

#include "cmsc412.h"
#include "keyboard.h"
#include "queue.h"
#include "kernel.h"
#define NULL 0
#define DELAY 500

extern int Get_time_of_day(void);
       int Long ( int , char ** ) ;
       int Ping ( int , char ** ) ;
       int Pong ( int , char ** ) ;
       void Wait( int ) ;

/*-------------------------------------------------------------------*/
/* ignore unused paramters for the processes */
#pragma warn -par

int Init( int argc , char ** argv )
{
  int i ,     /* loop index */
      scr_sem,/* sid of screen semaphore */
      id ,    /* ID of child process */
		count; /* child process's argc */

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
  argument [0] = "Long" ;
  argument [1] = "Proc_A 1st parameter" ;
  argument [2] = "Proc_A 2nd parameter" ;

  id = Proc_start ( Long , count , argument ) ;

  P ( scr_sem ) ;
  Cprintf ("***********Process A has been created with ID = %2d*****\n",id);
  V ( scr_sem ) ;

  count = 4 ;
  argument [0] = "Ping" ;
  argument [1] = "Proc_B 1st parameter" ;
  argument [2] = "Proc_B 2nd parameter" ;
  argument [3] = "Proc_B 3rd parameter" ;

  id = Proc_start ( Ping , count , argument ) ;

  P ( scr_sem ) ;
  Cprintf ("#######Process B has been created with ID = %2d#######\n",id);
  V ( scr_sem ) ;

  count = 2 ;
  argument [0] = "Pong" ;
  argument [1] = "Proc_C 1st parameter" ;

  id = Proc_start ( Pong , count , argument ) ;

  P ( scr_sem ) ;
  Cprintf ("$$$$$$Process C has been created with ID = %2d$$$$$$$$$\n",id);
  Cprintf ("\nNow, init is going to terminate \n") ;
  V ( scr_sem ) ;

  return ( 0 );
}

/*-------------------------------------------------------------------*/

int Long ( int argc , char ** argv )
{
  int i,j ;     	/* loop index */
  int scr_sem;		/* id of screen semaphore */
  int time, now; 	/* current and start time */

  now = time = Get_time_of_day();
  scr_sem = Create_semaphore ( "screen" , 1 ) ;   /* register for screen use */

  while (now < 200) {
      for ( j=0 ; j < 1000 ; j++) ;
      now = Get_time_of_day();
  }

  time = Get_time_of_day() - time;
  P ( scr_sem ) ;
  Cprintf ("\nProcess A is TERMINATED\n") ;
  Cprintf( "  Total time: %d\n", time );
  V( scr_sem );
  Wait( scr_sem );
  return ( 0 );

}

/*-------------------------------------------------------------------*/

int Ping ( int argc , char ** argv )
{
  int i,j ;     	/* loop index */
  int scr_sem; 		/* id of screen semaphore */
  int time; 		/* current and start time */
  int ping,pong;	/* id of semaphores to sync processes b & c */

  time = Get_time_of_day();
  scr_sem = Create_semaphore ( "screen" , 1 ) ;   /* register for screen use */
  ping = Create_semaphore ( "ping" , 1 ) ;   
  pong = Create_semaphore ( "pong" , 0 ) ;  

  for (i=0; i < 5; i++) {
       P(pong);
       for (j=0; j < 35; j++);
       V(ping);
  }

  time = Get_time_of_day() - time;
  P ( scr_sem ) ;
  Cprintf ("\nProcess B is TERMINATED\n") ;
  Cprintf( "Total time: %d\n", time );
  V( scr_sem );
  return ( 0 );

}


/*-------------------------------------------------------------------*/

int Pong ( int argc , char ** argv )
{
  int i,j ;     	/* loop index */
  int scr_sem; 		/* id of screen semaphore */
  int time; 		/* current and start time */
  int ping,pong;	/* id of semaphores to sync processes b & c */

  time = Get_time_of_day();
  scr_sem = Create_semaphore ( "screen" , 1 ) ;   /* register for screen use */
  ping = Create_semaphore ( "ping" , 1 ) ;    
  pong = Create_semaphore ( "pong" , 0 ) ;   

  for (i=0; i < 5; i++) {
       P(ping);
       for (j=0; j < 35; j++);
       V(pong);
  }

  time = Get_time_of_day() - time;
  P ( scr_sem ) ;
  Cprintf ("\nProcess C is TERMINATED\n") ;
  Cprintf( "Total time: %d\n", time );
  V( scr_sem );
  return ( 0 );

}
#pragma warn .par


/*-------------------------------------------------------------------*/

void Wait ( int scr_sem )   /* wait for a key from keyboard */
{
  P( scr_sem );
  Cprintf ("Press any key to continue\n") ;
  Get_char() ;
  V( scr_sem );
}

/*********************************************************************/
/*                   End of File proc.c                              */
/*********************************************************************/ 
