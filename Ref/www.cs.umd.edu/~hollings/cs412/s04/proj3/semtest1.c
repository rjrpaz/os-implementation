#include "libuser.h"
#include "libio.h"

#if !defined (NULL)
#define NULL 0
#endif

int Main( int argc , char ** argv )
{
  int scr_sem;			/* sid of screen semaphore */
  int id1, id2, id3;    	/* ID of child process */
  

  scr_sem    = Init_Semaphore ( "screen" , 1 )  ;
  

  P ( scr_sem ) ;
  Printf ("Semtest1 begins\n");
  V ( scr_sem ) ;


  id3 = Spawn_Program ( "/c/p3.exe" ) ;
  P ( scr_sem ) ;
  Printf ("p3 created\n");
  V ( scr_sem ) ;
  id1 = Spawn_Program ( "/c/p1.exe" ) ;
  id2 = Spawn_Program ( "/c/p2.exe" ) ;
  

  Wait(id1);
  Wait(id2);
  Wait(id3);

  Finish_Semaphore(scr_sem);
  return 0;
}

