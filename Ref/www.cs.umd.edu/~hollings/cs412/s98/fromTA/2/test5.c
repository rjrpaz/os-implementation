/**********************************************************************

	    Project 2: Multiprogramming Part I

  Simon Hawkin <cema@cs.umd.edu> 98/02/28
  - Based on the standard proc.c
  - Modified naming conventions
  - Enabled interrupts inside Wait()

**********************************************************************/

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

/*-------------------------------------------------------------------*/

int Init( int argc , char ** argv )
{
  int i ,     /* loop index */
      id ,    /* ID of child process */
      count ; /* child process's argc */

  char *argu [3];

  Cprintf ("\n\t\tTEST 5\n\nOne process with yields.\n");
  Wait();
  
  Cprintf ("\nThis is the init procedure\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  count = 2;
  argu [0] = "One";
  argu [1] = "One has no useful parameters";
  id = Proc_start (One, count, argu);
  Cprintf ("Spawned One id=%d\n", id);

  Cprintf ("\nInit will terminate now.\n" ) ;
  Wait();
  return ( 0 );
}

/*-------------------------------------------------------------------*/

int One (int count, char **arg)
{
  int i;
  
  Cprintf ("<%s> started with %d arguments:\n",
    "One", count );
  if (!arg) {
    Cprintf ("ERROR: arg=NULL\n");
    return (1);
  }
  for ( i = 0; i < count; ++i ) {
    if (!arg [i]) {
      Cprintf ("ERROR: arg [%d] = NULL\n", i);
      return (i+1);
    }
    Cprintf ("%d) <%s>\n", i, arg [i] );
  }
  Cprintf ("%s will yield 1st time now.\n ",
    "One");
  Wait();
  Yield ();

  if (!arg) {
    Cprintf ("ERROR: arg=NULL\n");
    return (1);
  }
  for ( i = 0; i < count; ++i ) {
    if (!arg [i]) {
      Cprintf ("ERROR: arg [%d] = NULL\n", i);
      return (i+1);
    }
    Cprintf ("%d) <%s>\n", i, arg [i] );
  }
  Cprintf ("%s will yield 2nd time now.\n ",
    "One");
  Wait();
  Yield();

  if (!arg) {
    Cprintf ("ERROR: arg=NULL\n");
    return (1);
  }
  for ( i = 0; i < count; ++i ) {
    if (!arg [i]) {
      Cprintf ("ERROR: arg [%d] = NULL\n", i);
      return (i+1);
    }
    Cprintf ("%d) <%s>\n", i, arg [i] );
  }
  Cprintf ("%s will terminate now.\n",
    "One");
  Wait();
  return (0);
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
