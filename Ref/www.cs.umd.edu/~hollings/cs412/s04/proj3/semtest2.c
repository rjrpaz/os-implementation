#include "libuser.h"
#include "libio.h"

int Main( int argc, char ** argv )
{
  int semkey, result;

  /* Unauthorized call */
  result = P(0);
  if (result<0)
    Printf("+ Identified unauthorized call\n");
  else
    Printf("- Not checking for authority\n");

  /* Invalid SID*/
  result = P(-1);
  if (result<0)
    Printf("+ Identified invalid SID\n");
  else
    Printf("- Not checking for invalid SID\n");
    
  Printf("Init_Semaphore() called\n");
  semkey = Init_Semaphore("test", 1);
  Printf("Init_Semaphore() returned %d\n", semkey);

  if (semkey < 0)
    return 0;

  Printf("P() called\n");
  result = P(semkey);
  Printf("P() returned %d\n", result);

  Printf("V() called\n");
  result = V(semkey);
  Printf("V() returned %d\n", result);


  Printf("Finish_Semaphore() called\n");
  result = Finish_Semaphore(semkey);
  Printf("Finish_Semaphore() returned %d\n", result);

  /* Unauthorized call */
  result = V(semkey);
  if (result<0)
    Printf("+ Removed authority after finish\n");
  else
    Printf("- Not removed authority after finish\n");


  return 0;
}
