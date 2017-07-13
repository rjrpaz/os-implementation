/* A fragmented send-receive test for Project 6
 * This is the process launcher
 *
 * Iulian Neamtiu
 *
 */

#include <conio.h>
#include <process.h>

int main(int argc , char ** argv)
{
  int snd_pid, rcv_pid;    	/* ID of child process */

  Print ("\nStarting fragmentation send/receive tests...\n") ;

  snd_pid = Spawn_Program ("/c/fsend.exe", "/c/fsend.exe");
  rcv_pid = Spawn_Program ("/c/frecv.exe", "/c/frecv.exe");

  Wait(snd_pid);
  Wait(rcv_pid);

  Print ("\nFragmentation tests completed.\n") ;

  return 0;
}

