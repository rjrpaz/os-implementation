#include "libuser.h"
#include "libio.h"

int Main( int argc , char ** argv )
{

  int holdsched3_sem;

  holdsched3_sem = Init_Semaphore("holdsched3_sem",0);

  P(holdsched3_sem);
  Printf("3");
  V(holdsched3_sem);

  Finish_Semaphore(holdsched3_sem);
  return 0;
}
