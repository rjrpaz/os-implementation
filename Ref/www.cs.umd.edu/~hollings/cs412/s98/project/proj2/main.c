/**********************************************************************

	    Project 2: Multiprogramming Part I

			 SECTION 0102


  By:
      Mohamed S. Aboutabl
  e-mail:
      aboutabl@cs.umd.edu

  File:        main.c
  Creadted on: 10/3/1992
  Contents:    main()  Initializes the OS,
		       Creates the init() initial process
		       Schedules init()
		       Clean-up after all processes terminate

   There are a few new types (such as process_ctrl_block) which
you will need to define.   For the time being, you may choose another
name, but you should be careful to modify any files provided to
you to reflect those changes.

**********************************************************************/

#include "dos.h"
#include "cmsc412.h"
#include "queue.h"
#include "kernel.h"
#include "screen.h"
#include "keyboard.h"

queuetype * readyq;           /* Ready Queue, global to all functions */
queuetype * keyboard_buff_ptr ;  /* Also, global for all functions */

process_ctrl_block runq;      /* Run Queue, only one item in the queue*/

int first_time = 1 ;          /* First call to scheduler() flag */
unsigned int main_ss ,        /* save the SS & SS of main() */
	     main_sp ;
extern int Init() ;

/*********************************************************************/

void main() {

  void interrupt ( * old_dos9h ) () ;  /* points at DOS keyboard
					 interrupt service routine */
  void interrupt ( * old_dos62 ) () ;  /* previous int 62h handler */
  void interrupt ( * old_dos63 ) () ;  /* previous int 63h handler */
  int id ,                             /* ID of init process */
      dosint ,                         /* Interrupt flag upon entry */
      count ;                          /* init process's argc */
  char * argument[10] ;    /* pointer to a maximum of 10 arguments */


  Init_malloc ();       /* Activate the safe memory management routines */
                        /* Init_malloc() uses malloc() which requires */
                        /* interrupts to be enabled */

  /* Initialization */
  dosint = Mod_int ( INT_DISABLE ) ;  /* Disable all interrupts */

  readyq = Q_create () ;          /* Create a queue for ready processes */
  keyboard_buff_ptr = Q_create () ;  /* Create a queue for the keyboard */


  old_dos9h = getvect ( 0x9  ) ;  /* Preserve dos ISR's */
  old_dos62 = getvect ( 0x62 ) ;
  old_dos63 = getvect ( 0x63 ) ;

  setvect ( 0x9 , Key_handler ); /* redirect Keyboard ISR from DOS to keyhand */
  setvect ( 0x62, System_service ) ;  /* Set kernel ISR */
  setvect ( 0x63 , Yield_process ) ;  /* Set yield ISR  */

  Init_screen () ;

  /* main loop */

  Cprintf ("\nThis is the main procedure\n");
  Cprintf ("Now I will init as a child processes\n") ;

  count = 3 ;
  argument [0] = "init" ;
  argument [1] = "init 1st parameter" ;
  argument [2] = "init 2nd parameter" ;
  id = Proc_start ( init , count , argument ) ;
  Cprintf ("Process init has been created with ID = %2d\n" , id ) ;

  Cprintf ("Now, main() is going to call scheduler() \n") ;
  Scheduler() ;

  /* Clean-up */
  Cprintf("\nNo more processes , I will finish up and exit OS\n");

  Q_destroy ( readyq ) ;
  Q_destroy ( keyboard_buff_ptr ) ;

  Free_malloc () ;  /* dactivate the safe memory management routines */

  setvect ( 0x9 , old_dos9h ) ;  /* Return to DOS keyboard driver */
  setvect ( 0x62, old_dos62 ) ;
  setvect ( 0x63, old_dos63 ) ;

  Mod_int ( dosint ) ;   /* restore interrupt flag */
  _AX  = 0x100 ;         /* restore hardware cursor */
  _CX  = 0xD0E ;
  geninterrupt ( 0x10 ) ;

} /* end of main() */

/*********************************************************************/
/*                   End of File main.c                              */
/*********************************************************************/
