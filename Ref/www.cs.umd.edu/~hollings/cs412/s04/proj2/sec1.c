// A test program for GeekOS user mode
// tests printing a bad pointer
// should print:

//  -><-Passed security test if arrows touch



#include "libuser.h"


#define SYSCALL "int $0x90"


int Evil_Print_String(char * message, int len){
   int num = SYS_PRINT, rc;

   __asm__ __volatile__ (
      SYSCALL
      : "=a" (rc)// system call returns value in eax
      : "a" (num), "b" (message), "c" (len)
   );

   return rc;
}

int Main( int argv, char ** argc )
{
  Print_String("->");
  if (0 == Evil_Print_String( (char *)0xffffffff, 20 )){
    Print_String("You shouldn't have printed that!\n");
  } else {
    Print_String("<-Passed security test if arrows touch\n");
  }

  return 0;
}

