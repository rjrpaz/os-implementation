#include "libuser.h"
#include "libio.h"

int Main( int argc , char ** argv )
{

  int i ;     	/* loop index */
  int scr_sem; 		/* id of screen semaphore */
  int prod_sem,cons_sem;	

  scr_sem = Init_Semaphore ( "screen" , 1 ) ;   /* register for screen use */
  prod_sem = Init_Semaphore ( "prod_sem" , 0 ) ;   
  cons_sem = Init_Semaphore ( "cons_sem" , 1 ) ;  

  for (i=0; i < 5; i++) {
    P(cons_sem);
    Printf ("Produced %d\n",i) ;
    V(prod_sem);
  }
  
  Finish_Semaphore(scr_sem);
  Finish_Semaphore(prod_sem);
  Finish_Semaphore(cons_sem);

  return 0;
}
