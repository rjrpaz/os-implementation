#include "libuser.h"
#include "libio.h"

int Main(int argc, char ** argv)
{
  int i, j ;     	/* loop index */
  int scr_sem;		/* id of screen semaphore */
  int now, start, elapsed; 		

  start = Get_Time();
  scr_sem = Init_Semaphore ( "screen" , 1 ) ;   /* register for screen use */

  for (i=0; i < 200; i++) {
      for ( j=0 ; j < 10000 ; j++) ;
      now = Get_Time();
  }

  elapsed = Get_Time() - start;
  P ( scr_sem ) ;
  Printf ("Process Long is done at time: %d\n", elapsed) ;
  V( scr_sem );

  return 0;
}

