/*
 * ========================================================================
 *
 *    Project 5:
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
 * Simon Hawkin <cema@cs.umd.edu> 98/05/05
 * - Explicit flow control in the inner loop (break).
 * - Initialization.
 * - Increased line/buffer size.
 *
 * ========================================================================
 */

#include <string.h>
#include "klib.h"
#include "io412.h"
#include "codes.h"

#define LINE_SIZE 200

int Proc( int argc , char ** argv )
{
  int i;
  int done;
  int ret;
  int current = 0;
  char buffer[LINE_SIZE];
  char line[LINE_SIZE];

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
	break;
      } else if ((line[current] == '\n') || (line[current] == '\r')) {
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
	break;
      } else {
	current++;
      }
    }
  }
}
