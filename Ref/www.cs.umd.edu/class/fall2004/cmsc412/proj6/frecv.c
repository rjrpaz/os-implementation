/* A fragmented send-receive test for Project 6
 * This is the receiver process
 *
 * Iulian Neamtiu
 *
 */

#include <conio.h>
#include <mq.h>
#include <process.h>

#define MAILBOX_SIZE 5
#define BUFFER_SIZE 5
#define NR_MESSAGES 10

char * process_name = "Receiver";

int main(int argc , char ** argv)
{
  int recvMQ;	/* id of MQ */
  char buffer[BUFFER_SIZE];
  int pid, i;

  pid = Get_PID();
  Print ("Starting process %d (%s)...\n", pid, process_name) ;

  recvMQ = Message_Queue_Create("fragmentation", MAILBOX_SIZE);

  for (i=0; i < NR_MESSAGES; i++) 
    {
      Message_Queue_Receive(recvMQ, buffer, BUFFER_SIZE);

      Print("Process %d (%s) received %d bytes\n", pid, process_name, BUFFER_SIZE);
    }

  Message_Queue_Destroy(recvMQ);

  Print ("\nProcess %d (%s) is done !\n", pid, process_name) ;

  return (0);

}
