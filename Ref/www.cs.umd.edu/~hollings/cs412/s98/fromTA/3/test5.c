/*
 * Department of Computer Science
 * University of Maryland - College Park
 * CMSC 412 Operating Systems [Spring 1998]
 *
 * Project 3.
 * [5] Test: Smokers.
 *
 * Simon Hawkin <cema@cs.umd.edu> 04/10/1998
 * - This is just the standard solution to the Smokers problem.
 * - Pseudorandom: does not randomize the cycle start, so it is repeatable.
 *
 * Simon Hawkin <cema@cs.umd.edu> 04/16/1998
 * - Implemented the finite Agent cycle and stopping condition.
 */

#include <stdlib.h>
#include <stdio.h>
#include "user412.h"
#include "cmsc412.h"

int TobaccoSmoker (int argC, char **argV);
int PaperSmoker (int argC, char **argV);
int MatchSmoker (int argC, char **argV);
int CigaretteAgent (int argC, char **argV);
void Wait (void);

/*
 * Manipulatables.
 */
#define SMOKE_COUNT 9999uL

#define VERBAL /* Undefine for less verbiage. Define for progress report. */
/* #undef VERBAL */

/*
 * Shared data.
 */
int tobaccoCount = 0, /* careful: exact count */
  paperCount = 0, matchCount = 0;
static keepSmoking = 1; /* modified by the Agent only */
int verbal = 0;

int Init (int ignoreC, char **ignoreV)
{
  int semScreen; /* screen semaphore */
  int argC;
  char *argV [2];
  unsigned long smokeCount = SMOKE_COUNT;

#if defined (VERBAL)
  verbal = 1;
#else
  verbal = 0;
#endif
  tobaccoCount = paperCount = matchCount = 0;

  semScreen = Create_semaphore ("screen", 1);

  P (semScreen);
  Cprintf ("\n\n\tTest 5: Smokers\n\n");
  Wait();
  V (semScreen);

  argC = 2;
  argV [0] = "Cigarette Agent";
  argV [1] = "               "; /* Passing the number of smoking rounds! */
  sprintf (argV [1], "%luuL", smokeCount );
  Proc_start (CigaretteAgent, argC, argV);

  argC = 1;
  argV [0] = "Tobacco Smoker";
  Proc_start (TobaccoSmoker, argC, argV);

  argC = 1;
  argV [0] = "Paper Smoker";
  Proc_start (PaperSmoker, argC, argV);

  argC = 1;
  argV [0] = "Match Smoker";
  Proc_start (MatchSmoker, argC, argV);

  return (0);
} /* Init() */

/*
 * Assume arguments are okay.
 */
int CigaretteAgent (int ignoreC, char **argV)
{
  unsigned long smokeRound, smokeCount = atol (argV [1]);
  int smoker;
  int semScreen;
  int semTobacco, semPaper, semMatch;
  int semLock, semAgent;
  int errorCount = 0;

  semScreen = Create_semaphore ("screen", 1);

  semTobacco = Create_semaphore ("tobacco", 0);
  semPaper   = Create_semaphore ("paper", 0);
  semMatch   = Create_semaphore ("match", 0);

  semAgent = Create_semaphore ("agent", 1);
  semLock  = Create_semaphore ("lock", 1);

  for ( smokeRound = 0; smokeRound < smokeCount; ++smokeRound ) {
    P (semAgent);
    smoker = random (3) +1;
    P (semLock);
    switch (smoker) {
    case 1:
      ++tobaccoCount;
      ++paperCount;
      V (semMatch);
      break;
    case 2:
      ++tobaccoCount;
      ++matchCount;
      V (semPaper);
      break;
    case 3:
      ++paperCount;
      ++matchCount;
      V (semTobacco);
      break;
    } /* switch smoker */
    V (semLock);
  } /* smoke round */

  /*
   * Stop smoking.
   */
  keepSmoking = 0;
  /*
   * Release smokers.
   */
  V (semTobacco);
  V (semPaper);
  V (semMatch);

  P (semScreen);
  Cprintf ("\n%s rested after %lu smoking rounds.\n",
    argV [0], smokeCount );
  if ( errorCount ) {
    Cprintf ("%d e", errorCount );
  }else {
    Cprintf ("No e");
  }
  Cprintf ("rrors have been detected.\n");
  Wait();
  V (semScreen);

  return (0);
} /* CigaretteAgent() */

/*
 * Assume arguments are okay.
 */
int TobaccoSmoker (int ignoreC, char **argV)
{
  int semLock, semAgent, semTobacco, semScreen;
  int errorCount = 0;

  semScreen = Create_semaphore ("screen", 1);

  semTobacco = Create_semaphore ("tobacco", 0);
  semAgent   = Create_semaphore ("agent", 1);
  semLock    = Create_semaphore ("lock", 1);

  while ( keepSmoking ) {
    P (semTobacco);
    if ( !keepSmoking ) break;
    P (semLock);
    --matchCount;
    --paperCount;
    if ( matchCount < 0 || paperCount < 0 ) {
      Cprintf ("ERROR\tERROR\tERROR\tnot enough resources\n");
      ++errorCount;
    }
    V (semAgent);
    if (verbal) Cprintf ("Tobacco\n");
    V (semLock);
  }

  P (semScreen);
  Cprintf ("\n%s rested.\n", argV [0] );
  if ( errorCount ) {
    Cprintf ("%d e", errorCount );
  }else {
    Cprintf ("No e");
  }
  Cprintf ("rrors have been detected.\n");
  Wait();
  V (semScreen);

  return (0);
} /* TobaccoSmoker() */

/*
 * Assume arguments are okay.
 */
int PaperSmoker (int ignoreC, char **argV)
{
  int semAgent, semLock, semPaper, semScreen;
  int errorCount = 0;

  semScreen = Create_semaphore ("screen", 1);
  
  semPaper = Create_semaphore ("paper", 0);
  semAgent = Create_semaphore ("agent", 1);
  semLock  = Create_semaphore ("lock", 1);

  while ( keepSmoking ) {
    P (semPaper);
    if ( !keepSmoking ) break;
    P (semLock);
    --matchCount;
    --tobaccoCount;
    if ( matchCount < 0 || tobaccoCount < 0 ) {
      Cprintf ("ERROR\tERROR\tERROR\tnot enough resources\n");
      ++errorCount;
    }
    V (semAgent);
    if (verbal) Cprintf ("\tPaper\n");
    V (semLock);
  }

  P (semScreen);
  Cprintf ("\n%s rested.\n", argV [0] );
  if ( errorCount ) {
    Cprintf ("%d e", errorCount );
  }else {
    Cprintf ("No e");
  }
  Cprintf ("rrors have been detected.\n");
  Wait();
  V (semScreen);

  return (0);
} /* PaperSmoker() */

/*
 * Assume arguments are okay.
 */
int MatchSmoker (int ignoreC, char **argV)
{
  int semAgent, semLock, semMatch, semScreen;
  int errorCount = 0;

  semScreen = Create_semaphore ("screen", 1);
  
  semMatch = Create_semaphore ("match", 0);
  semAgent = Create_semaphore ("agent", 1);
  semLock  = Create_semaphore ("lock", 1);

  while ( keepSmoking ) {
    P (semMatch);
    if ( !keepSmoking ) break;
    P (semLock);
    --paperCount;
    --tobaccoCount;
    if ( paperCount < 0 || tobaccoCount < 0 ) {
      Cprintf ("ERROR\tERROR\tERROR\tnot enough resources\n");
      ++errorCount;
    }
    V (semAgent);
    if (verbal) Cprintf ("\t\tMatch\n");
    V (semLock);
  }

  P (semScreen);
  Cprintf ("\n%s rested.\n", argV [0] );
  if ( errorCount ) {
    Cprintf ("%d e", errorCount );
  }else {
    Cprintf ("No e");
  }
  Cprintf ("rrors have been detected.\n");
  Wait();
  V (semScreen);

  return (0);
} /* MatchSmoker() */

void Wait (void)
{
  Cprintf ("Press any key to continue.\n");
  Get_char ();
} /* Wait() */
