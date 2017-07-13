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

/*-------------------------------------------------------------------*/

int Init( int argc , char ** argv )
{
  int i ,     /* loop index */
      id ,    /* ID of child process */
      count ; /* child process's argc */

  char *argu [3];

  Cprintf ("\n\t\tTEST 6a\nA few processes with yields.\n");
  Wait();
  
  Cprintf ("\nThis is the init procedure\n");
  Cprintf ("I have %2d arguments. They are \n" , argc );
  for ( i=0 ; i<argc ; i++ )
    Cprintf ("Argument no. %2d is '%s'\n" , i , argv[i] ) ;

  count = 2;
  argu [0] = "One";
  argu [1] = "2000"; /* how many times to loop */
  id = Proc_start (One, count, argu);
  Cprintf ("Spawned One id=%d\n", id);

  count = 2;
  argu [0] = "Two";
  argu [1] = "2000"; /* how many times to loop */
  id = Proc_start (Two, count, argu);
  Cprintf ("Spawned Two id=%d\n", id);

  count = 2;
  argu [0] = "Three";
  argu [1] = "2"; /* how many times to loop */
  id = Proc_start (Three, count, argu);
  Cprintf ("Spawned Three id=%d\n", id);

  Cprintf ("\nInit will terminate now.\n" ) ;
  Wait();
  return ( 0 );
}

/*-------------------------------------------------------------------*/

int One (int count, char **arg)
{
  int i;
  int yieldCount, yieldCap;
  
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

  yieldCap = atoi (arg [1]);
  for ( yieldCount = 0; yieldCount < yieldCap; ++yieldCount ) {
    Cprintf ("%s is about to yield (%d)\n",
      "One", yieldCount );
    Yield ();
  }

  Cprintf ("%s has yielded %d times and is about to terminate\n",
    "One", yieldCount );
  Wait();
  return (0);
} /* One() */

int Two (int count, char **arg)
{
  int i;
  int yieldCount, yieldCap;
  
  Cprintf ("<%s> started with %d arguments:\n",
    "Two", count );
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

  yieldCap = atoi (arg [1]);
  for ( yieldCount = 0; yieldCount < yieldCap; ++yieldCount ) {
    Cprintf ("%s is about to yield (%d)\n",
      "Two", yieldCount );
    Yield ();
  }

  Cprintf ("%s has yielded %d times and is about to terminate\n",
    "Two", yieldCount );
  Wait();
  return (0);
} /* Two() */

int Three (int count, char **arg)
{
  int i;
  int yieldCount, yieldCap;
  
  Cprintf ("<%s> started with %d arguments:\n",
    "Three", count );
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

  yieldCap = atoi (arg [1]);
  for ( yieldCount = 0; yieldCount < yieldCap; ++yieldCount ) {
    Cprintf ("%s is about to yield (%d)\n",
      "Three", yieldCount );
    Yield ();
  }

  Cprintf ("%s has yielded %d times and is about to terminate\n",
    "Three", yieldCount );
  Wait();
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
