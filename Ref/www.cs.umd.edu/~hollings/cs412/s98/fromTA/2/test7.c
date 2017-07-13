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
int Two (int, char**);
int Three (int, char**);

int static spawnCap = 3;

/*-------------------------------------------------------------------*/

int Init( int argc , char ** argv )
{
  int i ,     /* loop index */
      id ,    /* ID of child process */
      count ; /* child process's argc */

  char *argu [3];

  Cprintf ("\n\t\tTEST 7\n\nProcesses spawn each other %d times.\n",
    spawnCap);
  Wait();
  
  Cprintf ("\nThis is the init procedure\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  count = 2;
  argu [0] = "One";
  argu [1] = "0"; /* loop count */
  id = Proc_start (One, count, argu);
  Cprintf ("Spawned One id=%d\n", id);

  count = 2;
  argu [0] = "Two";
  argu [1] = "0"; /* loop count */
  id = Proc_start (Two, count, argu);
  Cprintf ("Spawned Two id=%d\n", id);

  Cprintf ("\nInit will terminate now.\n" ) ;
  Wait();
  return ( 0 );
}

/*-------------------------------------------------------------------*/

#pragma argsused

int One (int count, char **arg) /* args unused */
{
  int id;
  int static spawnCount = 0;
  int argC;
  char *argV [2];

  if ( ++spawnCount > spawnCap ) {
    Cprintf ("<%s> is done spawning and will terminate now.\n",
      "One");
    Wait ();
    return (0);
  }

  argC = 2;
  argV [0] = "Three";
  argV [1] = "      ";
  sprintf (argV [1], "%d", spawnCap );
  
  id = Proc_start (Three, argC, argV);
  Cprintf ("%s has spawned [%s %s] id=%d (%d times more)\n",
      "One", argV [0], argV [1], id, spawnCap - spawnCount );
  if ( spawnCap - spawnCount <= 0 ) {
    Wait();
  }

  return (0);
} /* One() */

/*-------------------------------------------------------------------*/

#pragma argsused

int Two (int count, char **arg) /* args unused */
{
  int id;
  int static spawnCount = 0;
  int argC;
  char *argV [2];

  if ( ++spawnCount > spawnCap ) {
    Cprintf ("<%s> is done spawning and will terminate now.\n",
      "Two");
    Wait ();
    return (0);
  }

  argC = 2;
  argV [0] = "One";
  argV [1] = "      ";
  sprintf (argV [1], "%d", spawnCap );
  
  id = Proc_start (One, argC, argV);
  Cprintf ("%s has spawned [%s %s] id=%d (%d times more)\n",
      "Two", argV [0], argV [1], id, spawnCap - spawnCount );

  if ( spawnCap - spawnCount <= 0 ) {
    Wait();
  }
  return (0);
} /* Two() */

/*-------------------------------------------------------------------*/

#pragma argsused

int Three (int count, char **arg) /* args unused */
{
  int id;
  int static spawnCount = 0;
  int argC;
  char *argV [2];

  if ( ++spawnCount > spawnCap ) {
    Cprintf ("<%s> is done spawning and will terminate now.\n",
      "Three");
    Wait ();
    return (0);
  }

  argC = 2;
  argV [0] = "Two";
  argV [1] = "      ";
  sprintf (argV [1], "%d", spawnCap );
  
  id = Proc_start (Two, argC, argV);
  Cprintf ("%s has spawned [%s %s] id=%d (%d times more)\n",
      "Three", argV [0], argV [1], id, spawnCap - spawnCount );

  if ( spawnCap - spawnCount <= 0 ) {
    Wait();
  }
  return (0);
} /* Three() */

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
