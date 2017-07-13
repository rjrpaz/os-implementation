/*
 * Department of Computer Science
 * University of Maryland - College Park
 * CMSC 412 Operating Systems [Spring 1998]
 *
 * Project 3.
 * [0] A simple test. Just plays with a semaphore.
 *
 * Simon Hawkin <cema@cs.umd.edu> 04/08/1998
 * - Decided to write a simple test.
 */

#include <stdio.h>
#include <stdlib.h>
#include "user412.h"
#include "cmsc412.h"

#if !defined (max2)
#define max2( a, b ) ((a) > (b) ? (a) : (b))
#endif

int Play (int argC, char **argV);
void WaitScreen (void);

int Init (int ignoreC, char **ignoreV)
{
  int semScreen; /* screen semaphore */
  int argC;
  char *argV [10];
  int playCount1 = 900, playCount2 = 600;

  semScreen = Create_semaphore ("screen", 1);

  P (semScreen);
  Cprintf ("\n\n\tTest 0: Just playing with semaphores.\n");
  Cprintf ("\t\tCounts: %d, %d\n\n", playCount1, playCount2 );
  WaitScreen();
  V (semScreen);

  argC = 2;
  argV [0] = "Player One";
  argV [1] = "          "; /* Passing play semaphore count! */
  sprintf (argV [1], "%d", playCount1);
  Proc_start (Play, argC, argV);
  
  argC = 2;
  argV [0] = "Player Two";
  argV [1] = "          "; /* Passing play semaphore count! */
  sprintf (argV [1], "%d", playCount2);
  Proc_start (Play, argC, argV);

  P (semScreen);
  Cprintf ("\nPlay completed.\n");
  V (semScreen);
  
  return (0);
} /* Init() */

/*
 * Assume arguments are okay.
 */
int Play (int ignoreC, char **argV)
{
  int i;
  int playCount = atoi (argV [1]);
  int semPlay, semScreen;

  semScreen = Create_semaphore ("screen", 1);
  semPlay = Create_semaphore ("play", playCount);
  
  for ( i = 0; i < playCount; ++i ) {
    P (semScreen);
    P (semPlay);
    Cprintf ("%s\tP\t\t%4d\n", argV [0], i );
    V (semScreen);

    P (semScreen);
    V (semPlay);
    Cprintf ("%s\t\tV\t%4d\n", argV [0], i );
    V (semScreen);
  }

  P (semScreen);
  Cprintf ("%s is done playing.\n", argV [0] );
  WaitScreen();
  V (semScreen);
  
  return (0);
} /* Play() */

/*
 * To be called inside P..V (semScreen).
 */
void WaitScreen (void)
{
  Cprintf ("Press any key to continue.\n");
  Get_char ();
} /* Wait() */
