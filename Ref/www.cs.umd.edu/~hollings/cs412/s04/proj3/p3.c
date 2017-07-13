#include "libuser.h"
#include "libio.h"

int Main(int argc, char** argv)
{
  int scr_sem,holdp3_sem;
  scr_sem = Init_Semaphore ( "screen" , 1 ) ;   /* register for screen use */
  holdp3_sem = Init_Semaphore ("holdp3_sem", 0);

  P(holdp3_sem);

  P ( scr_sem ) ;
  Printf("p3 executed\n");
  V( scr_sem );

  V(holdp3_sem);
  
  Finish_Semaphore(scr_sem);
  Finish_Semaphore(holdp3_sem);

  return 0;  
}
