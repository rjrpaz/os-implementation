
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
 *  File:        wc.c
 *  Created on:  04/19/1996
 * 
 *  Contents:    A simple wc, read stdin and write count of lines and chars
 *               to stdout.
 * 
 *
 * ========================================================================
 */

#include <ctype.h>
#include <string.h>
#include "klib.h"
#include "codes.h"
#include "io412.h"


extern void exit(int);

void getOut(void)
{
    char buffer[512];

    /* send the eof */
    buffer[0] = ESC;
    (void)  MQ_Send(1, buffer, 1);
    exit(-1);
}

int Proc( int argc , char ** argv )
{
  int i;
  int ret;
  char *ch;
  int done;
  int inSpaceMode;
  char buffer[512];
  int words, bytes, lines;
  int doBytes, doWords, doLines;

  doBytes = doWords = doLines = 1;

  /* defaults back to off */
  if (argc != 1) doLines = doBytes = doWords = 0;
  for (i=1; i < argc; i++) {
     if (argv[i][0] == '-') {
         ch = argv[i]+1;
         while (*ch) {
             switch (*ch) {
  	         case 'l':
			doLines++;
		        break;
  	         case 'w':
			doWords++;
		        break;
  	         case 'c':
			doBytes++;
		        break;
		 default:
			Cprintf("Invalid argument %s\n", argv[i]);
			getOut();
			break;
             }
	     ch++;
         }
     } else {
	 Cprintf("Invalid argument %s\n", argv[i]);
	 getOut();
	 break;
     }
  }

  done = 0;
  memset(buffer, '\0', sizeof(buffer));
  inSpaceMode = words = bytes = lines = 0;
  while ( !done) {
      ret = MQ_Receive(0, buffer, sizeof(buffer));
      if (ret <= 0) break;

      bytes += ret;

      for (i=0; i < ret; i++) {
          /* ESC is EOF */
          if (buffer[0] == ESC) {
	     done = 1;
	     if(doWords) Cprintf("%d	", words);
	     if(doLines) Cprintf("%d	", lines);
	     if(doBytes) Cprintf("%d	", bytes);
	     Cprintf("\n");
	     buffer[0] = ESC;
	     ret = MQ_Send(1, buffer, 1);
	     break;
          }

	  if (isspace(buffer[i])) {
	     if (!inSpaceMode) words++;
	     inSpaceMode = 1;
	  } else {
	     inSpaceMode = 0;
	  }
	
	  if ((buffer[i] == '\n') ||
	      (buffer[i] == '\r')) {
	      lines++;
	      memset(buffer, '\0', sizeof(buffer));
          }
      }
  }
}
