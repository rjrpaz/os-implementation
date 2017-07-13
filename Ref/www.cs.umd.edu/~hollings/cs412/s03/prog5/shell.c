/**********************************************************************

	    Project 5: simple shell


  By:
      Jeffrey K. Hollingsworth
  e-mail:
      hollings@cs.umd.edu

  File:        init.c
  Creadted on: 05/05/1996
  Modified for new project 5/13/02


**********************************************************************/

#include "libuser.h"

int isspace(char ch)
{
    if ((ch == ' ') || (ch == '\t')) return 1;
    return 0;
}

typedef struct _command {
    char *command;	
    char *outFile;
    int pid;		/* process id for this thing */
} command;

command commands[10];

void split(char *line, int *numCommands);

void Main (int argc , char ** argv)
{
  int i;
  int ret;
  int done;
  int eoln;
  int debug;
  char *curr;
  char *dest;
  char line[256];
  int infd, outfd;
  char tempName[2];
  char strName[20];
  int numCommand;
  int   count ;         /* child process's argc */
  char command[256];

  done = 0;
  while (!done) {
     Print("shell %% ");

     eoln = 0;
     count = 0;

     Read_Line( line, sizeof(line) );

     line[strlen(line)-1] = '\0';

     /* blank line */
     if (!strcmp(line, "")) continue;

     if (!strcmp(line, "exit")) {
         return;
     } else if (!strcmp(line, "debug")) {
	 debug = 1;
	 continue;
     } else if (!strcmp(line, "nodebug")) {
	 debug = 0;
	 continue;
     }

    split(line, &numCommand);
    infd = 0;
    outfd = 1;

    for (i=0; i < numCommand-1; i++) {
        if (commands[i].outFile) {
	    Print("Error: file redirection for command other than last one in pipeline\n");
	    break;
	}
    }
    if (i != numCommand-1) continue;

    if (debug) {
	for (i=0; i < numCommand; i++) {
	    Print("command %d = %s\n", i, commands[i].command);
	}
    }

    for (i=0; i < numCommand; i++) {
 	 if (outfd != 1) infd = outfd;

         if (i == numCommand-1)  {
	     if (commands[i].outFile) {
		 /* last one goes to a file */
		 outfd = Open(commands[i].outFile, O_WRITE|O_CREATE);
		 if (outfd < 0) {
		     Print("Error opening %s\n", commands[i].outFile);
		 }
	     } else {
		 /* last one goes to stdout */
		 outfd = 1;
	     }
	 } else {
             strcpy(strName, "/dev/pipe");
	     tempName[0] = count + '0';
	     count++;
	     tempName[1] = '\0';
	     strcat(strName, tempName);
 	     outfd = MessageQueueCreate(strName);
	     if (outfd < 0) {
	         Print("pipe failed\n");
		 continue;
	     }
	 }

         /* now run it */
	 memset(command, '\0', sizeof(command));
         if (commands[i].command[0] != '/') {
	     //	add implict path to /c for programs
	     strcpy(command, "/c/");
             for (curr=commands[i].command, dest=&command[3]; *curr; curr++) {
                if (*curr == ' ') break;
                *(dest++) = *curr;
             }
             strcat(command, ".exe");
             strcat(command, curr);
             // Print("command is %s\n", command);
         } else {
             strcpy(command, commands[i].command);
         }

         commands[i].pid = Spawn_Program ( command , infd, outfd);
         if ( commands[i].pid <= 0 )  {  /* Unable to proc_start */
            Print ( "%s: Command not found\n", commands[i].command) ;
            continue;
         } else {
	    if (debug) Print("spawned pid %d, command = %s\n", 
		commands[i].pid, commands[i].command);
   	    if (infd != 0) Close(infd);
	 }
    }

    /* now wait for them all to finish */
    for (i=0; i < numCommand; i++) {
	if (commands[i].pid > 0) {
	    ret = Wait(commands[i].pid);
	    if (debug) Print("process %d exited\n", commands[i].pid);
	}
    }
    if (debug) Print("all processes exited\n");
  }
}


void split(char *line, int *numCommands)
{
   int i;
   char *ch;

   for (i=0; i < 10; i++) {
       commands[i].command = 0;
       commands[i].outFile = 0;
   }

   ch = line;
   *numCommands = 1;
   commands[0].command = ch;
   while (*ch) {
	if (*ch == '\n') break;

	if (isspace(*ch)) {
	    ch++;
	    /* skip white space */
	    while (*ch && isspace(*ch)) ch++;
	}

	if (*ch == '>') {
	    *ch = '\0';
	    ch++;
	    /* skip white space */
	    while (*ch && isspace(*ch)) ch++;
	    commands[*numCommands-1].outFile = ch;
	}
	if (*ch == '|') {
	   *ch = '\0';
	   ch++;
	   /* skip white space */
	   while (*ch && isspace(*ch)) ch++;

	   commands[*numCommands].command = ch;
	   (*numCommands)++;
	}

	/* check for background symbol */

	while (*ch && (!isspace(*ch)) && (*ch != '|')) ch++;
   }

   // remove trailing space(s) after last non-space in command
   for (i=0; i < *numCommands; i++) {
	ch = &commands[i].command[strlen(commands[i].command)-1];
	while (ch > commands[i].command) {
	    if (isspace(*ch)) {
		*ch = '\0';
		ch--;
	    } else {
		break;
	    }
	}
   }


}
