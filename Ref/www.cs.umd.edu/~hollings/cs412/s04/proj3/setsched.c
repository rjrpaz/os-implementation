// A test program for Sys_Get_System_Time()

#include "libuser.h"
#include "libio.h"
#include "atoi.h"

int Main( int argc, char * argv[] )
{
  int policy = 0 ;
  int quantum = 0;
  int retval;
  
  if ( argc < 3 )
    {
      Printf("Syntax is : setsched <sched> [quantum]\n");
      return 1;
    }
  
  policy = atoi (argv[1]);
  quantum = atoi (argv[2]);

  Printf ("Calling Set_Scheduling_Policy(%d, %d)...\n", policy, quantum);

  retval = Set_Scheduling_Policy( policy, quantum);
 
  Printf ("Set_Scheduling_Policy returned: %d\n", retval);
  
  return 0;
}
