/**************************************************************************

  SAMPLECLIENT.C

  Written by:  Kirstie Hawkey

  Implemented on:  March 6, 1999
  Modified on:  July 6, 2003

  This program is a client that that begins at 100 and adds the integers
  sequentially from 100 down to 1.  The addition is accomplished by sending
  a request to the server.  The requests are stored as a structure as follows:

         struct request{
            int first;      // first number to add 
            int second;     // second number to add 
            int result;     // result returned by the server 
         }

  This client will output to the screen the numbers added and the result
  returned from the server.

*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include "samplecs.h"


int main(int argc, char *argv[]){

   int counter;                /* loop counter */
   int sock;                   /* socket file descriptor */
   int bytes_read;             /* count of bytes returned by read() */
   struct sockaddr_in server;  /* internet domain socket structure */
   struct hostent *hp, *gethostbyname(); /* host name structures */
   REQUEST add;                /* addition request structure */


   /* check for correct number of command line arguments */
   if (argc != 3){
      perror("correct command line format: cl_1 host port#");
      exit(1);
   }

   /* initialize result field of structure */
   add.result = 1000;          

   /* create a socket */
   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock < 0){
      perror("opening stream socket");
      exit(1);
   }

   /* connect socket using name specified by command line. */
   server.sin_family = AF_INET;
   hp = gethostbyname(argv[1]);
   if (hp == 0){
      perror("unknown host\n");
      exit(2);
   }

   bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
   server.sin_port = htons(atoi(argv[2]));

   if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0){
      perror("connecting stream socket");
      exit(1);
   }

   /* loop through numbers less than 1000, adding one at a time to previous
      result */
   for (counter = 999; counter > 0; counter--){
      add.first = counter;
      add.second = add.result;

      /* send message to server */
      if (write(sock, (char *) &add, sizeof(add)) < 0)
         perror("writing on stream socket");

      /* clear message buffer, block waiting for reply from server */
      bzero(&add, sizeof(add)); 
      if (bytes_read = read(sock, (char *) &add, sizeof(add)) < 0)
           perror("reading stream message");

      /* output addition and results to screen */
      printf("results from server:%d + %d = %d\n", add.first, add.second, 
              add.result);
   
   }
}



