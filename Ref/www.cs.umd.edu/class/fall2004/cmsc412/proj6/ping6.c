/* A ping-pong test for Project 6
 * This is the PING process
 *
 * Iulian Neamtiu
 *
 */

#include <conio.h>
#include <mq.h>
#include <process.h>

#define MAILBOX_SIZE 100
#define BUFFER_SIZE 5
#define NR_MESSAGES 5

char * sending  = "PING";
char * awaiting = "PONG";

int main(int argc , char ** argv)
{
  int sendMQ, recvMQ;	/* id of MQ's */
  char buffer[BUFFER_SIZE];
  int pid, i;

  pid = Get_PID();
  Print ("Starting process %d (%s)...\n", pid, sending) ;

  sendMQ = Message_Queue_Create("ping_MQ", MAILBOX_SIZE);
  recvMQ = Message_Queue_Create("pong_MQ", MAILBOX_SIZE);

  for (i=0; i < NR_MESSAGES; i++) 
    {
      Message_Queue_Send(sendMQ, sending, (BUFFER_SIZE - 1));
      Message_Queue_Receive(recvMQ, buffer, (BUFFER_SIZE - 1));

      buffer[BUFFER_SIZE - 1] = '\0';

      Print("Process %d sent %s, awaiting %s, got '%s'\n", pid, sending, awaiting, buffer);
    }

  Message_Queue_Destroy(sendMQ);
  Message_Queue_Destroy(recvMQ);

  Print ("\nProcess %d (%s) is done !\n", pid, sending) ;

  return (0);

}
