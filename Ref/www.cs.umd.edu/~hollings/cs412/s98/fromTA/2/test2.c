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

static int recLevel = 0;
static int recDepth = 8;

/*-------------------------------------------------------------------*/

int Init( int argc , char ** argv )
{
  int i ,     /* loop index */
      id ,    /* ID of child process */
      count ; /* child process's argc */

  Cprintf ("\n\t\tTEST 2\n\nInit recursive calls.\n");
  Cprintf ("Call #%d, remains %d\n", recLevel, recDepth - recLevel );
  Wait();
  
  Cprintf ("\nThis is the init procedure\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  if ( recLevel < recDepth ) {
    id = ++recLevel;
    Cprintf ("Calling init recursively (level %d)\n", id );
    i = Init (argc, argv);
    Cprintf ("Init (level %d) returned %d\n", id, i );
    Wait();
  }
  
  Cprintf ("\nInit will terminate now.\n" ) ;
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
