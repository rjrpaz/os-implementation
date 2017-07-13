/**********************************************************************

	    Project 2: Multiprogramming Part I

  Simon Hawkin <cema@cs.umd.edu> 98/02/28
  - Based on the standard proc.c
  - Modified naming conventions
  - Enabled interrupts inside Wait()

**********************************************************************/

#include <stdlib.h>
#include <stdio.h>
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

void Wait (void);
int One (int, char**);

static spawnCount = 2000;

/*-------------------------------------------------------------------*/

int Init( int argc , char ** argv )
{
  int i ,     /* loop index */
      id ,    /* ID of child process */
      count ; /* child process's argc */

  char *argu [3];

  Cprintf ("\n\t\tTEST 3a\n\nRecursive spawns.\n");
  Cprintf ("Will spawn %d times\n", spawnCount );
  Wait();
  
  Cprintf ("\nThis is the init procedure\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  count = 2;
  argu [0] = "One";
  argu [1] = "0";
  id = Proc_start (One, count, argu);
  Cprintf ("Spawned id=%d\n", id);

  Cprintf ("\nInit will terminate now.\n" ) ;
  Wait();
  return ( 0 );
}

/*-------------------------------------------------------------------*/

#pragma argsused

int One (int count, char **arg)
{
  int recLevel;
  int id;
  int argC;
  char *argV [2];

  recLevel = atoi (arg [1]) +1;
  argC = 2;
  argV [0] = "One";
  argV [1] = "      ";
  sprintf (argV [1], "%d", recLevel );
  if ( recLevel < spawnCount ) {
    id = Proc_start (One, argC, argV);
    Cprintf ("Spawned (id=%d) %s %s\n", id, argV [0], argV [1] );
  }else {
    Cprintf ("Recursive bottom\n");
    Wait();
  }
  return (recLevel);
} /* One() */

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
