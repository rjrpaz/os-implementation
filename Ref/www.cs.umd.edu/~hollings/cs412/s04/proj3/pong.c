#include "libuser.h"
#include "libio.h"

int Main( int argc , char ** argv )
{
  int i ;     	/* loop index */
  int scr_sem; 		/* id of screen semaphore */
  int time; 		/* current and start time */
  int ping,pong;	/* id of semaphores to sync processes b & c */

  time = Get_Time();
  scr_sem = Init_Semaphore ( "screen" , 1 ) ;   /* register for screen use */
  ping = Init_Semaphore ( "ping" , 1 ) ;    
  pong = Init_Semaphore ( "pong" , 0 ) ;   

  for (i=0; i < 5; i++) {
       P(ping);
       Printf ("Pong !\n") ;
       V(pong);
  }

  time = Get_Time() - time;
  P ( scr_sem ) ;
  Printf ("Process Pong is done at time: %d\n", time) ;
  V( scr_sem );

  return 0;
}
