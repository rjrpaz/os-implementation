// A test program for semaphores

#include "libuser.h"
#include "libio.h"

int Main( int argc, char ** argv)
{
  int semkey, result;

  Printf("Init_Semaphore()...\n");
  semkey = Init_Semaphore("semtest", 3);
  Printf("Init_Semaphore() returned %d\n", semkey);

  if (semkey < 0)
    return 0;

  Printf("P()...\n");
  result = P(semkey);
  Printf("P() returned %d\n", result);

  Printf("P()...\n");
  result = P(semkey);
  Printf("P() returned %d\n", result);

  Printf("V()...\n");
  result = V(semkey);
  Printf("V() returned %d\n", result);


  Printf("Finish_Semaphore()...\n");
  result = Finish_Semaphore(semkey);
  Printf("Finish_Semaphore() returned %d\n", result);

  return 0;
}
