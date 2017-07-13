#include "libuser.h"
#include "libio.h"

int Main( int argc , char ** argv )
{

  int i,j ;     	/* loop index */
  int holdsched3_sem;

  holdsched3_sem = Init_Semaphore("holdsched3_sem",0);

  for (i=0; i < 10; i++) {
    for(j=0;j<20000;j++);
    Printf("1");
  }

  V(holdsched3_sem);

  for (i=0; i < 10; i++) {
    for(j=0;j<20000;j++);
    Printf("1");
  }
  
  Finish_Semaphore(holdsched3_sem);

  return 0;
}
