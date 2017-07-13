/**********************************************************************
 * server.c --- Demonstrate a simple iterative server.
 * Tom Kelliher
 *
 * This program demonstrates a simple iterative server.  The server
 * opens a TCP connection on port SERVER_PORT and begins accepting
 * connections from anywhere.  It sits in an endless loop, so one must
 * send an INTR to terminate it.
 *
 * The server reads a message from the client, printing it to stdout.
 * Then, the server sends a simple message back to the client.
 **********************************************************************/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>


#define DATA "Danger Will Roger . . ."
#define TRUE 1
#define SERVER_PORT 5001
#define BUFFER_SIZE 1024


/* prototypes */
void die(const char *);
void pdie(const char *);


/**********************************************************************
 * main
 **********************************************************************/

int main(void) {

   int sock;   /* fd for main socket */
   int msgsock;   /* fd from accept return */
   struct sockaddr_in server;   /* socket struct for server connection */
   struct sockaddr_in client;   /* socket struct for client connection */
   int clientLen;   /* returned length of client from accept() */
   int rval;   /* return value from read() */
   char buf[BUFFER_SIZE];   /* receive buffer */

   /* Open a socket, not bound yet.  Type is Internet TCP. */
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      pdie("Opening stream socket");

   /*
      Prepare to bind.  Permit Internet connections from any client
      to our SERVER_PORT.
   */
   bzero((char *) &server, sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons(SERVER_PORT);
   if (bind(sock, (struct sockaddr *) &server, sizeof(server)))
      pdie("Binding stream socket");

   printf("Socket has port %hu\n", ntohs(server.sin_port));

   /* Set the listen queue to 5, the maximum. */
   listen(sock, 5);

   /* Loop, waiting for client connections. */
   /* This is an interactive server. */
   while (TRUE) {

      clientLen = sizeof(client);
      if ((msgsock = accept(sock, (struct sockaddr *) &client,
                            &clientLen)) == -1)
         pdie("Accept");
      else {
         /* Print information about the client. */
         if (clientLen != sizeof(client))
            pdie("Accept overwrote sockaddr structure.");

         printf("Client IP: %s\n", inet_ntoa(client.sin_addr));
         printf("Client Port: %hu\n", ntohs(client.sin_port));

         do {   /* Read from client until it's closed the connection. */
            /* Prepare read buffer and read. */
            bzero(buf, sizeof(buf));
            if ((rval = read(msgsock, buf, BUFFER_SIZE)) < 0)
               pdie("Reading stream message");

            if (rval == 0)   /* Client has closed the connection */
               fprintf(stderr, "Ending connection\n");
            else
               printf("S: %s\n", buf);

            /* Write back to client. */
            if (write(msgsock, DATA, sizeof(DATA)) < 0)
               pdie("Writing on stream socket");

         } while (rval != 0);
      }   /* else */

      close(msgsock);
   }

   exit(0);

}


/**********************************************************************
 * pdie --- Call perror() to figure out what's going on and die.
 **********************************************************************/

void pdie(const char *mesg) {

   perror(mesg);
   exit(1);
}


/**********************************************************************
 * die --- Print a message and die.
 **********************************************************************/

void die(const char *mesg) {

   fputs(mesg, stderr);
   fputc('\n', stderr);
   exit(1);
}


