/**********************************************************************

	    Project 2: Multiprogramming Part I

  Simon Hawkin <cema@cs.umd.edu> 98/02/28
  - Based on the standard proc.c
  - Modified naming conventions
  - Enabled interrupts inside Wait()

**********************************************************************/


#include "cmsc412.h"
#include "keyboard.h"
#include "kernel.h"

#if !defined (NULL)
#define NULL 0L
#endif

#if !defined (INT_ENABLE)
#define	INT_ENABLE	0x0200
#define	INT_DISABLE	0x0
#endif

void Wait(void);

/*-------------------------------------------------------------------*/

int Init( int argc , char ** argv )
{
  int i ,     /* loop index */
      id ,    /* ID of child process */
      count ; /* child process's argc */

  Cprintf ("\n\t\tTEST 1\n\nJust init.\n");
  Wait();
  
  Cprintf ("\nThis is the init procedure\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  Cprintf ("\nNow, init is going to terminate.\n") ;
  Wait();
  return ( 0 );
}

/*-------------------------------------------------------------------*/

void Wait ()   /* Busy-wait for a key from keyboard */
{
  int flag;

  Cprintf ("Press any key to continue... ") ;
  flag = Mod_int (INT_ENABLE);
  while( Get_char() == NULL ) ;
  Mod_int (flag);
  Cprintf ("\n") ;
}

/*********************************************************************/
/*                   End of File proc.c                              */
/*********************************************************************/
