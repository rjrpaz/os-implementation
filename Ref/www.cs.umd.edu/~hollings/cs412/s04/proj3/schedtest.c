#include "libuser.h"
#include "libio.h"
#include "atoi.h"

#if !defined (NULL)
#define NULL 0
#endif

int Main( int argc , char ** argv )
{
  int policy;
  int quantum;

  int id1, id2, id3;    	/* ID of child process */

    if (argc == 3) {
      if (!strcmp(argv[1], "rr")) {
          policy = SCHEDPOLICY_RR;
      } else if (!strcmp(argv[1], "mlf")) {
          policy = SCHEDPOLICY_MF;
      } else {
	  Printf("usage: %s [rr|mlf] <quantum>\n", argv[0]);
	  Exit();
      }
      quantum = atoi(argv[2]);
      Set_Scheduling_Policy(policy, quantum);
  } else {
      Printf("usage: %s [rr|mlf] <quantum>\n", argv[0]);
      Exit();
  }

  id3 = Spawn_Program ( "/c/sched3.exe" ) ;
  id1 = Spawn_Program ( "/c/sched1.exe" ) ;
  id2 = Spawn_Program ( "/c/sched2.exe" ) ;

  
  Wait(id1);
  Wait(id2);
  Wait(id3);
  Printf("\n");
  return 0;
}

