/* A ping-pong test for Project 6
 * This is the process launcher
 *
 * Iulian Neamtiu
 *
 */

#include <conio.h>
#include <process.h>

int main(int argc , char ** argv)
{
  int ping_pid, pong_pid;    	/* ID of child process */

  Print ("\nStarting ping-pong tests...\n") ;

  ping_pid = Spawn_Program ("/c/ping6.exe", "/c/ping6.exe");
  pong_pid = Spawn_Program ("/c/pong6.exe", "/c/pong6.exe");

  Wait(ping_pid);
  Wait(pong_pid);

  Print ("\nPing-pong tests completed.\n") ;

  return 0;
}

