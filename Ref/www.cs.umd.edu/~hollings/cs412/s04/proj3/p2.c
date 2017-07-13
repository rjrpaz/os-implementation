#include "libuser.h"
#include "libio.h"

int Main( int argc , char ** argv )
{
  int i ;     	/* loop index */
  int scr_sem; 		/* id of screen semaphore */
  int prod_sem, cons_sem;
  int holdp3_sem;

  scr_sem = Init_Semaphore ( "screen" , 1 ) ;   /* register for screen use */
  prod_sem = Init_Semaphore ( "prod_sem" , 0 ) ;    
  cons_sem = Init_Semaphore ( "cons_sem" , 1 ) ;   
  holdp3_sem = Init_Semaphore ( "holdp3_sem", 0 ) ;

  for (i=0; i < 5; i++) {
    P(prod_sem);
    Printf ("Consumed %d\n",i) ;
    V(cons_sem);
  }
  
  V(holdp3_sem);
  
  Finish_Semaphore(scr_sem);
  Finish_Semaphore(prod_sem);
  Finish_Semaphore(cons_sem);
  Finish_Semaphore(holdp3_sem);
  return 0;
}
