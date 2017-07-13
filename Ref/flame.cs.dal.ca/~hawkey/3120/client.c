/**********************************************************************
 * client.c --- Demonstrate a simple client.
 * Tom Kelliher
 *
 * This program will connect to a simple iterative server and exchange
 * messages.  The single command line argument is the server's hostname.
 * The server is expected to be accepting connection requests from
 * SERVER_PORT.
 *
 * The same message is sent three times over separate connections, 
 * demonstrating that different ephemeral ports are used for each
 * connection.
 **********************************************************************/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>


#define DATA "The sea is calm tonight, the tide is full . . ."
#define SERVER_PORT 5001
#define BUFFER_SIZE 1024


/* prototypes */
void die(const char *);
void pdie(const char *);


/**********************************************************************
 * main
 **********************************************************************/

int main(int argc, char *argv[]) {

   int sock;   /* fd for socket connection */
   struct sockaddr_in server;   /* Socket info. for server */
   struct sockaddr_in client;   /* Socket info. about us */
   int clientLen;   /* Length of client socket struct. */
   struct hostent *hp;   /* Return value from gethostbyname() */
   char buf[BUFFER_SIZE];   /* Received data buffer */
   int i;   /* loop counter */

   if (argc != 2)
      die("Usage: client hostname");

   /* Open 3 sockets and send same message each time. */

   for (i = 0; i < 3; ++i)
   {
      /* Open a socket --- not bound yet. */
      /* Internet TCP type. */
      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
         pdie("Opening stream socket");
      
      /* Prepare to connect to server. */
      bzero((char *) &server, sizeof(server));
      server.sin_family = AF_INET;
      if ((hp = gethostbyname(argv[1])) == NULL) {
         sprintf(buf, "%s: unknown host\n", argv[1]);
         die(buf);
      }
      bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
      server.sin_port = htons((u_short) SERVER_PORT);
      
      /* Try to connect */
      if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
         pdie("Connecting stream socket");
      
      /* Determine what port client's using. */
      clientLen = sizeof(client);
      if (getsockname(sock, (struct sockaddr *) &client, &clientLen))
         pdie("Getting socket name");
      
      if (clientLen != sizeof(client))
         die("getsockname() overwrote name structure");
      
      printf("Client socket has port %hu\n", ntohs(client.sin_port));
      
      /* Write out message. */
      if (write(sock, DATA, sizeof(DATA)) < 0)
         pdie("Writing on stream socket");
      
      /* Prepare our buffer for a read and then read. */
      bzero(buf, sizeof(buf));
      if (read(sock, buf, BUFFER_SIZE) < 0)
         pdie("Reading stream message");
      
      printf("C: %s\n", buf);
      
      /* Close this connection. */
      close(sock);
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


