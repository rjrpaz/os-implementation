/**********************************************************************

	    Project 1: Keyboard and Screen Drivers

			 SECTION 0102


  By:
      Mohamed S. Aboutabl
  e-mail:
      aboutabl@cs.umd.edu  ( DO NOT SEND Mohamed email.  He is no longer TA. )

  File:         main.c
  Created on:   9/22/1992
  Contents:     main()
  Modified by:  Charles Lin (changed the lettering conventions)
  Modified on:  Spring, 1996

**********************************************************************/

/*  You will need to write your own .h files -- except dos.h and cmsc412.h */

/* CMSC 412- Spring 1996

   NOTE:  These include files are suggested ways of breaking your files
            down.   You have been provided with codes.h, cmsc412.h.  You
            will need to write you own queue.h and queue.c, as well as
            screen.h and screen.c, and keyboard.h and keyboard.c.   It 
            might be useful to write down in your README file, which
            functions resides in which file, and what the purpose of the
            functions are.
*/

#include <dos.h>
#include "codes.h"
#include "cmsc412.h"
#include "queue.h"
#include "screen.h"
#include "keyboard.h"

/*  CMSC 412 -- Spring 1996
    NOTE: You should probably define a struct and typedef for queuetype, 
          and include it in queue.h
*/

queuetype * keyboard_buff_ptr ;  /* Global for all functions */


/* ============================================================ */

/*                          MAIN                                */

/* ============================================================ */

/*
 *   CMSC 412 -- Spring 1996
 *   You may wish to change the names of Q_Create to something else,
 *   though it will probably be easier to read your program if you
 *   stick to the suggested names.
 */

void main() {
  char ch;
  static void interrupt ( * old9h ) () ;  /* points at DOS keyboard
					     interrupt service routine */
  /* Initialization */
  Init_malloc (); /* Activate the safe memory management routines */
  keyboard_buff_ptr = Q_Create ();  /* Create a queue for the keyboard */

  old9h = getvect ( 0x9 );  /* redirect Keyboard ISR from DOS to Key_handler */

/* Key_handler is the suggested name for the keyboard handler.  This
   should be placed in keyboard.c and keyboard.h    If you change the
   name to something else, you will need to modify the following. */

  setvect ( 0x9 , Key_handler );

  Init_screen ();  /* You should define Init_screen() in screen.h/screen.c */

         /* ---------- main loop ----------- */

  while ( ( ch = Get_char() ) != ESC ) /* ESC is already defined in codes.h */
    if ( ch != (char) NULL )
      Put_char( ch ) ; /* Define Put_char() in screen.[ch] */

  /* Clean-up */

  Q_Destroy ( keyboard_buff_ptr ) ;  /* Define in queue.[ch] */
  Free_malloc () ;  /* deactivate the safe memory management routines */
  setvect ( 0x9 , old9h ) ;  /* Return to DOS keyboard driver */

  _AX  = 0x100 ;         /* restore hardware cursor */
  _CX  = 0xD0E ;
  geninterrupt ( 0x10 ) ;

} /* end of main() */

/*********************************************************************/
/*                      End of File main.c                           */
/*********************************************************************/
