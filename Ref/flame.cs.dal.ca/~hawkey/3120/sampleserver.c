/*
   SAMPLESERVER.C

   Written by:  Kirstie Hawkey

   Implemented on:  March 6, 1999
   Modified on:  July 6, 2003
         
   Template used:  @(#)streamread.c        6.2 (Berkeley) 5/8/86  
            Copyright (c) 1986 Regents of the University of California.
            All rights reserved.  The Berkeley software License Agreement
            specifies the terms and conditions for redistribution.

   SAMPLESERVER creates a socket and prints the port number to screen.  Upon
   accepting a request for a connection, it forks a child process to deal
   with the request until such time as the client has terminated.

   The child server processes will receive a message that will be cast
   to the REQUEST structure. They will add the first 2 integers in the 
   structure and return the sum as the result.  The stucture will be converted
   to a string and returned to the client.

   This program will not exit unless it encounters an error.  It should be
   killed with ^C when finished.

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include "samplecs.h"


int main(){
   int sock;            /* file descriptor of server's socket */        
   int length;          /* length of socket name */
   struct sockaddr_in server;  /* internet domain socket structure */
   int msgsock;         /* file descriptor of child's socket */
   int bytes_read;      /* count of bytes returned by read() */
   REQUEST add;         /* addition request structure */
   int pid;             /* process id number */

   /* Create socket */
   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock < 0) {
      perror("opening stream socket");
      exit(1);
   }

   /* Name socket using wildcards */
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = FREE_PORT;
   if (bind(sock, (struct sockaddr *) &server, sizeof(server))) {
      perror("binding stream socket");
      exit(1);
   }

   /* Find out assigned port number and print it out */
   length = sizeof(server);
   if (getsockname(sock, (struct sockaddr *) &server, &length)) {
      perror("getting socket name");
      exit(1);
   }
   printf("Socket has port #%d\n", ntohs(server.sin_port));
   fflush(stdout);

   /* Start accepting connections */
   listen(sock, 5);

   /* when a connection is received, create a socket for the child process,
      and fork the child to handle the client's request */
   do{
      msgsock = accept(sock, 0, 0);

      pid = fork();

      /* pid < 0 - fork failed, output error message */
      if (pid <0)
         perror("error forking child");

      /* pid = 0 - process is child */
      else if (!pid){

         /* close real port-end in child */
         close (sock);

         /* if failed creating port assigned to client, output error */
         if (msgsock == -1)
            perror("accept");

         /* handle client until client exits */
         else do {

            /* clear buffer */
            bzero(&add, sizeof(add));

            /* read message from client */
            if ((bytes_read = read(msgsock, (char *) &add, sizeof(add))) < 0)
               perror("reading stream message");

            /* if a message was read, process it for client */
            if (bytes_read != 0){

               /* determine sum for client */
               add.result = add.first + add.second;

               /* send message to client */
               write(msgsock, (char *) &add, sizeof(add));

               /* echo results sent to client on screen */
               printf("results sent to client:  %d + %d = %d\n", add.first,
                       add.second, add.result);
            } 
            else
               /* 0 bytes sent from client - end connection */
               printf("Ending connection\n");
          
         } while (bytes_read != 0);  

         /* exit child process - client finished requests */
         exit (0);

      }  /* end of child code */

      /* parent process - close msgsock in preparation for next
         client */
      close(msgsock);

   } while (TRUE);
}


