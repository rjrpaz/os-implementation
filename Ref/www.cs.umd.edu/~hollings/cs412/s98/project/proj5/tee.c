/*
 * ========================================================================
 *
 *	    Project 5: 
 *
 *
 *
 *  By:
 *      Jeff Hollingsworth
 *  e-mail:
 *      hollings@cs.umd.edu
 *
 *  File:        tee.c
 *  Created on:  04/19/1996
 * 
 *  Contents:    A simple tee, read stdin and write to stdout and stderr
 * 
 *  Modification History:
 *	5/7/96	removed extra include files	jkh
 *
 * ========================================================================
 */

#include <string.h>
#include "klib.h"
#include "io412.h"
#include "codes.h"


int Proc( int argc , char ** argv )
{
  int i;
  int done;
  int ret;
  int current;
  char buffer[40];
  char line[40];

  done = 0;
  i = 0;
  memset(buffer, '\0', sizeof(buffer));
  while ( !done) {
      i = 0;
      memset(buffer, '\0', sizeof(buffer));
      ret = MQ_Receive(0, buffer, sizeof(buffer));
      while (i < ret) {
	  if (buffer[i] == ESC) {
	     /* send pending line */
	     if (current) {
		 ret = MQ_Send(1, line, current);
		 if (ret < 0) {
		     char str[] = "tee: Send Error\n";
		     MQ_Send(2, str, strlen(str)+1);
		     return(-1);
		 }
		 ret = MQ_Send(2, line, current);
	     }
	     done = 1;
	     buffer[0] = ESC;
	     MQ_Send(1, buffer, 1);
	     return(0);
	  }
          line[current] = buffer[i++];
 	  if (current == sizeof(line)-1) {
	     /* line is full, send it out */
	     ret = MQ_Send(1, line, current+1);
	     if (ret < 0) {
		 char str[] = "tee: Send Error";
		 MQ_Send(2, str, strlen(str)+1);
		 return(-1);
	     }
	     ret = MQ_Send(2, line, current+1);
	     memset(line, '\0', sizeof(line));
	     current = 0;
          } else if ((line[current] == '\n') ||
	       (line[current] == '\r')) {
	     /* reached end of line, send it */
	     ret = MQ_Send(1, line, current+1);
	     if (ret < 0) {
		 char str[] = "tee: Send Error";
		 MQ_Send(2, str, strlen(str)+1);
		 return(-1);
	     }
	     ret = MQ_Send(2, line, current+1);
	     memset(line, '\0', sizeof(line));
	     current = 0;
	  } else {
	     current++;
	  }
      }
   }
}
