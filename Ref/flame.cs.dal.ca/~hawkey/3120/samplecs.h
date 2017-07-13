/* SAMPLECS.H

   Written by:  Kirstie Hawkey

   Implemented on:  March 6, 1999
   Modified: July 6, 2003

   This sample code consists of a client (SAMPLECLIENT.C) and a server
   SAMPLESERVER.C).

   SAMPLECLIENT is a client that that begins at 100 and adds sequentially the
   integers from 100 down to 1.  The addition is accomplished by sending
   a request to the server.  The requests are stored as a structure as
   follows:

         struct request{
            int first;      // first number to add 
            int second;     // second number to add 
            int result;     // result returned by the server 
         }

  SAMPLECLIENT will output to the screen the numbers added and the result 
  returned from the server.

  SAMPLESERVER will create the socket and print the port number to screen.  
  Upon accepting a request for a connection, the server forks a child process to
  handle the request until the client has terminated.

  The child server processes receive the message encoded from 3 integer
  fields.  They convert the message to the structure format that the clients
  use, and add the first two fields together, returning the sum in the
  third field.

  The server must be exited using ^C from the keyboard.

  The type defination of the request structure is included in this header
  file as both the client programs and server make use of them.

*/

#ifndef SAMPLECS_H

#define SAMPLECS_H
#define TRUE 		1
#define FREE_PORT	0	/* wildcard to get free port for server */

struct request{
   int first;        /* first number to add */
   int second;       /* second number to add */
   int result;       /* result returned by the server */
};

typedef struct request REQUEST;

#endif

