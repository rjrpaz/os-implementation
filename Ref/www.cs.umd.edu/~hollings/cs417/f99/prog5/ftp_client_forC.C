/******************************************************************************
  Source File:    ftp_client.C
  
  Description:	FTP Client Application
  
  Author(s):	Ben Teitelbaum
  
  Class:		UW/CS640
  
  Modifications:
  
  $Log: ftp_client.C,v $
  
  ******************************************************************************/

#include <ctype.h>
#include "ftp_client.h"
#include "ftp_server.h"
extern "C" {
#include "config-net.h"
}
#include "wrappers.h"

// To start an FTP client application a thread must be created running
// ftpClientMain(); this is like the main() for the client application.
//
// The single argument passed in is an identifying name used for
// debugging purposees.
void*
ftpClientMain(void* cName) {
  
  // Client name helps disambiguate multiple client output
  assert(strlen((char*) cName) <= MAX_THREAD_NAME_LEN); 
  
  char thrName[MAX_THREAD_NAME_LEN + 1];
  char curDir[_POSIX_PATH_MAX + 1];

  // Initialize curDir and oldDir
  pthread_mutex_lock(&chDirLock);  
  getcwd(curDir, (size_t)(_POSIX_PATH_MAX + 1));
  pthread_mutex_unlock(&chDirLock);  

  strcpy(thrName, (char*)cName);
  
  ftp_cmd_t nextCmd;		// Next command from the luser
  char* arg;				// Pointer to the cmd arg, if there is one
  char* arg2;				// Pointer to second cmd arg, if there is one
  
  int windowSize;		// Int value of arg if cmd is:
  // setwindow [n]
  
  ifstream* sendFile = NULL;	  // File stream for outgoing file
  ofstream* recvFile = NULL;	  // File stream for incoming file
  
  int connectDesc 
    = FTP_DISCONNECTED;		// Connection descriptor identifying
  // the current connection 
  int tempConnectDesc;
  
  status_t stat;				// Place to store returned statuses
  int connStat;         
  int ret;
  v6addrType v6addr;
  
  struct timeval startTime;			// Start of x-mission
  struct timeval endTime;				// End of x-mission
  float elapsedSecs;
  
  msg_type_t msgType;
  
  char fileName[FTP_MAX_PAYLOAD_SIZE]; // Char buffer to hold file
  // name field from messages
  
  unsigned numBytesXFered;	// Number of bytes sent/recv'd
  
  ftp_msg_c inMsg(thrName);	// the incoming FTP message...
  ftp_msg_c outMsg(thrName);	// the outcoming FTP message...
  
  DO_FTP_WRITE_CONSOLE("FTP client started.\n");
  
  while(TRUE) {
    // Prompt the luser and get the next command
	 
    // Lock the console and get the next line
    pthread_mutex_lock(&consoleLock);  
    printf("[%s] -> ", thrName);
	 fflush(stdout);
    pthread_mutex_unlock(&consoleLock);  

    nextCmd = getFTPUserCommand(&arg, &arg2);
	 if (arg != NULL) {
		if (*arg == '?') {
		  DO_FTP_WRITE_CONSOLE("Invalid command syntax -- type 'help' for assistance.\n");
		  continue;
		}
	 }
	 
    switch(nextCmd) {
	  // Handle 'open [host]' command
	  case FTP_CMD_OPEN:
		// If already open, report the error
		if (connectDesc != FTP_DISCONNECTED) {
		  ftpShowError("Connection already exists!\nopen failed");
		  break;
		}
		
		// Otherwise, open an FTP connection to the spec'd IPv6 host ID
		printf("about to call socket\n");
		connectDesc = IPv6socket(IPV6_PROTO_TCP);
		printf("called socket\n");
		if (connectDesc < 0) {
		    DO_FTP_SHOW_ERROR("IPv6socket() returned an error");
		}
		parseV6Addr(arg, v6addr);
		printf("about to call connect\n");
		ret = IPv6connect(connectDesc, v6addr, 50000);
		printf("called connect\n");
		if (ret < 0) {
		  tempConnectDesc = connectDesc;
		  connectDesc = FTP_DISCONNECTED;
		  DO_FTP_SHOW_ERROR("IPv6connect() returned an error");
		} else {
		  DO_FTP_WRITE_CONSOLE3("Connected to host %s with descript %d\n", arg, connectDesc);
		}
		break;     // FTP_CMD_OPEN
		
	  // Handle 'close' command
	  case FTP_CMD_CLOSE:
		DO_FTP_CONNECTED_P("close failed");
		
		// Create a CLIABORT message and send it
		outMsg.init(FTP_CLIABORT);
		DO_FTP_SEND("close failed");
			

		// People don't seem to like having IPv6close() called by
		// both client and server on a close.  Now client just sends
		// disconnect request to server and server closes the connection
		DO_FTP_WRITE_CONSOLE2("Connection %d will be terminated by server\n",
									 connectDesc);
		connectDesc = FTP_DISCONNECTED;
		break;     // FTP_CMD_CLOSE

		// Wait for the ack from the server...
		DO_FTP_RECV("close failed");
		
		// Examine reply message...
		switch(inMsg.getType()) {
		 case FTP_CLIABORTOK:
		  if (IPv6close(connectDesc) == 0) {
		     DO_FTP_WRITE_CONSOLE2("Connection %d terminated\n", connectDesc);
		     connectDesc = FTP_DISCONNECTED;
		     break;
		  } else {
		     DO_FTP_SHOW_ERROR("IPv6close() returned an error -- invalid connection descriptor - non-fatal");
		  }
		  break;
		 default:
		  // Unexpected reply type from server
		  assert(FALSE);
		}
		break;     // FTP_CMD_CLOSE
		
	  // Handle 'send [file]' command
	  case FTP_CMD_SEND:
		DO_FTP_CONNECTED_P("send failed");
		
		// If there was a sendFile open already, close it
		if (sendFile != NULL) sendFile->close();
		
		// Make sure that the file exists and open it
		pthread_mutex_lock(&chDirLock);  
		chdir(curDir);
		sendFile = new ifstream(arg);
		pthread_mutex_unlock(&chDirLock);  
		
		if (sendFile->fail()) {
		  DO_FTP_SHOW_ERROR2("Cannot open file: %s -- send failed", arg);
		  break;
		}
		
		// Construct an FTP_SR message and send it
		outMsg.init(FTP_SR, arg);
		DO_FTP_SEND("send failed");
		
		// Get the acknowledgement from the server
		DO_FTP_RECV("send failed");
		
		switch(inMsg.getType()) {
		 case FTP_ERR:
		  DO_FTP_SHOW_ERROR("Server cannot receive file transfers now.");
		  break;
		
		 case FTP_SROK:
		  // First construct and send an FTP_FILESTART message ???
		  //	    outMsg.init(FTP_FILESTART);
		  //    DO_FTP_SEND("send failed");
		  
		  // Then, construct and send data messages until eof is
		  // reached
		  
		  numBytesXFered = 0;

		  gettimeofday(&startTime, NULL);
		  
		  while(TRUE) {
			 // Construct the next FTP data message
			 stat = outMsg.init(sendFile);
			 if (stat == STAT_FAIL) {
				DO_FTP_SHOW_ERROR2("Problem reading file: %s -- send failed",
										 arg);
				outMsg.init(FTP_CLIABORT);
				DO_FTP_SEND("send failed trying to send abort to server");
				break;

			 } else if (stat == STAT_EOF) {
				// Send the last packet
				DO_FTP_SEND("send failed after reading eof");

				numBytesXFered += outMsg.getLength();
				if (inMsg.getLength() > 0) {
				  printf(".");
				  fflush(stdout);
				}
				
				// Send EOF packet
				outMsg.init(FTP_EOF);
				DO_FTP_SEND("send failed trying to send FTP_EOF signal");
				
				// Wait for FTP_EOFOK acknowledgement from server
				// If server goes down at this point, we have a
				// potential race condition.  We really should define
				// whether an atm_recv() over a dead connection will
				// eventually time out at the sock level, or whether
				// it is the responsability of FTP to time out. !!!
				DO_FTP_RECV("send failed to get EOFOK from server");

				msgType = inMsg.getType();

				if (msgType == FTP_EOFOK) {
				  gettimeofday(&endTime, NULL);

				  elapsedSecs = (float)((endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec)/1000000.0);
				  
				  ftpShowMsg("\nSent %d bytes at %.2f bytes/sec\n",
								 numBytesXFered,
								 (float)numBytesXFered/ elapsedSecs);

				} else {
				  // Unexpected message returned by DO_FTP_RECV
				  DO_FTP_SHOW_ERROR2("Got unexpected message type %d", msgType);
				  assert(FALSE);
				}
				
				break;
			 } else if (stat == STAT_OK) {

				DO_FTP_SEND("send failed");
				numBytesXFered += outMsg.getLength();
				printf(".");
				fflush(stdout);

			 } else {
				// Unexpected stat returned by init
				DO_FTP_SHOW_ERROR2("Got unexpected stat from init %d", stat);
				assert(FALSE);
			 }
		  }
		  break;

		 default:
		  // Unexpected message type
		  assert(FALSE);
		}
		break;     // FTP_CMD_SEND
		
	  // Handle 'get [file]' command
	  case FTP_CMD_GET:
		
		DO_FTP_CONNECTED_P("get failed");
		
		// NOTE: File xfers that fail may leave the partially received
		// file on disk -- wrong semantics ???
		
		// Construct an FTP_GR message and send it
		outMsg.init(FTP_GR, arg);
		DO_FTP_SEND("get failed");
		
		// Get the acknowledgement from the server
		DO_FTP_RECV("get failed");
		switch(inMsg.getType()) {
		 case FTP_ERRFNAME:
		  DO_FTP_SHOW_ERROR2("File: %s is not available from the server",
									arg);
		  break;
		 case FTP_FILESTART:
		  // Open the recvFile to receive the transfer
		  pthread_mutex_lock(&chDirLock);  
		  chdir(curDir);
		  recvFile = new ofstream(arg); // Check for err !!!!
		  pthread_mutex_unlock(&chDirLock);  
		  
		  // Grab the consoleLock for the entire ls
		  pthread_mutex_lock(&consoleLock);                           

		  numBytesXFered = 0;
		  gettimeofday(&startTime, NULL);
		  
		  while(TRUE) {
			 DO_FTP_RECV("get failed");
			 if (inMsg.getType() == FTP_EOF) {
				// Append the data packet payload to the receive file
				// and break;
				(*recvFile) << inMsg;  // Check error !!!
				
				numBytesXFered += inMsg.getLength();
				if (inMsg.getLength() > 0) {
				  printf(".");
				  fflush(stdout);
				}
				
				gettimeofday(&endTime, NULL);
				elapsedSecs = (float)((endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec)/1000000.0);

				ftpShowMsg("\nReceived %d bytes at %.2f bytes/sec\n",
							  numBytesXFered,
							  (float)numBytesXFered/ elapsedSecs);

				pthread_mutex_unlock(&consoleLock);                           
				break;
			 } else if (inMsg.getType() == FTP_ERR) {
				pthread_mutex_unlock(&consoleLock);                           
				DO_FTP_SHOW_ERROR("File I/O problem at server -- connection down");
				recvFile->close();
				break;
			 } else if (inMsg.getType() == FTP_DATA) {
				// Append the data packet payload to the receive file
				(*recvFile) << inMsg;
				
				numBytesXFered += inMsg.getLength();
				printf(".");
				fflush(stdout);

			 } else { 
				// Unexpected message type
				assert(FALSE);
			 }
		  }

		  // Close recvFile and delete 
		  recvFile->close();
		  delete recvFile;

		  break;
		 default:
		  // Unexpected message type
		  assert(FALSE);
		}

		break;     // FTP_CMD_GET
		
      // Handle 'cd [dir]' command
	  case FTP_CMD_CD:
		DO_FTP_CONNECTED_P("cd failed");
		
		// Create a CDR message and send it
		outMsg.init(FTP_CDR, arg);
		DO_FTP_SEND("cd failed");
		
		// Get the reply from the server...
		DO_FTP_RECV("cd failed");
		
		// Extract FNAME field
		inMsg.GetPayload(fileName);
		
		switch(inMsg.getType()) {
		 case FTP_CDOK:
		  DO_FTP_WRITE_CONSOLE2("OK, directory is now: %s\n", fileName);
		  break;
		 case FTP_ERRFNAME: 
		  DO_FTP_SHOW_ERROR("Could not change directory -- perhaps directory does not exist.");
		  break;
		 default:
		  // Unexpected reply type from server
		  assert(FALSE);
		}      
		break;     // FTP_CMD_CD
		
      // Handle 'lcd [dir]' command
	  case FTP_CMD_LCD:
		pthread_mutex_lock(&chDirLock);  
		chdir(curDir);

		if (chdir(arg) != 0) {
		  pthread_mutex_unlock(&chDirLock);  
		  DO_FTP_SHOW_ERROR2("Cannot change to local directory %s", arg);
		}
		getcwd(curDir, (size_t)(_POSIX_PATH_MAX + 1));
		pthread_mutex_unlock(&chDirLock);  
		break;     // FTP_CMD_LCD
		
      // Handle 'ls' command
	  case FTP_CMD_LS:
		DO_FTP_CONNECTED_P("ls failed");
		
		// Create a LSR message and send it
		outMsg.init(FTP_LSR);
		DO_FTP_SEND("ls failed");
		
		// Wait for the ack from the server...
		DO_FTP_RECV("ls failed");
		switch(inMsg.getType()) {
		 case FTP_ERR:
		  DO_FTP_SHOW_ERROR("Server cannot list current directory.");
		  break;
		 case FTP_FILESTART:
		  // Grab the consoleLock for the entire ls
		  pthread_mutex_lock(&consoleLock);                           
		  
		  while(TRUE) {
			 // Can't call DO_FTP_RECV("ls failed") because we are
			 // holding consoleLock.
			 
			 // Receive next message, handling errors appropriately
			 connStat = inMsg.init(connectDesc);
			 if (connStat < 0) {
				switch (connStat) {
				 default:
				  ftpShowError("[%s] IPv6recv() returned an unexpected error code", thrName);
				  assert(FALSE);
				}
				ftpShowError("ls failed");
				connectDesc = FTP_DISCONNECTED;                     
				pthread_mutex_unlock(&consoleLock);
				break; 
			 }                                             

			 // Next message received, no errors encountered
			 if (inMsg.getType() == FTP_EOF) {
				pthread_mutex_unlock(&consoleLock);                           
				break;
			 } else if (inMsg.getType() == FTP_DATA) {
				// Write the payload to standard output
				cout << inMsg << endl;
			 } else {
				// Unexpected message type
				assert(FALSE);
			 }
		  }
		  break;
		 default:
		  // Unexpected message type
		  assert(FALSE);
		}
		break;     // FTP_CMD_LS
		
      // Handle 'setwindow [n]' command
	  case FTP_CMD_SET_WINDOW:
		sscanf(arg, "%d", &windowSize);
		switch(IPv6setSendLimit(connectDesc, windowSize)) {
		 case 0:
		  DO_FTP_WRITE_CONSOLE2("window size set to: %d", windowSize);
		  break;
		 case -1:
		  DO_FTP_SHOW_ERROR("IPv6setSendLimit() returned -1 -- some error occurred");
		  break;

		 default:
		  // Unexpected stat returned from IPv6setSendLimit
		  assert(FALSE);
		}
		break;     // FTP_CMD_SET_WINDOW
		
      // Handle 'help' command
	  case FTP_CMD_HELP:
		showFTPUsage();
		break;     // FTP_CMD_HELP

      // Print garbler stats
	  case FTP_CMD_GARB_STATS:
		garb_print_stats(); 
		break;     // FTP_CMD_GARB_STATS

		// Set the garbler probabilities
	  case FTP_CMD_SET_GARB:
#ifdef notdef
		{
		  char* probString[5];
		  probString[0] = "NORMAL_DISCARD_PROBABILITY";
		  probString[1] = "NORMAL_CORRUPT_PROBABILITY";
		  probString[2] = "NORMAL_TRAILER_PROBABILITY";
		  probString[3] = "ROUTING_DISCARD_PROBABILITY";
		  probString[4] = "ROUTING_CORRUPT_PROBABILITY";
		  int whichProb = atoi(arg);
		  int retVal;
		  float val;
		  sscanf(arg2, "%f", &val);
		  retVal = garb_set_prob(whichProb, val);
		  DO_FTP_WRITE_CONSOLE3("Setting %s to %f", probString[whichProb], val);
		  if (retVal == -1) {
		     DO_FTP_SHOW_ERROR("garb_set_prob() failed");
		  }
		}
#endif
		printf("FTP_CMD_SET_GARB not implemented\n");
		break;     // FTP_CMD_SET_GARB

	  // Silly scheme to verify that 640 students didn't spoof FTP
	  case FTP_CMD_VERIFY:
		fflush(0);
		break;

      // Handle 'quit' command
	  case FTP_CMD_QUIT:
		DO_FTP_WRITE_CONSOLE("Quiting this FTP thread, but not whole process.\n");
		pthread_exit(0);
		break;     // FTP_CMD_QUIT
		
      // Handle 'exit' command
	  case FTP_CMD_EXIT:
		DO_FTP_WRITE_CONSOLE("FTP exiting the whole ftp process...\n");
		exit(-1);
		break;     // FTP_CMD_EXIT
		
      // Handle unrecognized user input given at command line
	  case FTP_BAD_CMD:
		DO_FTP_WRITE_CONSOLE("Unknown command -- type 'help' for assistance.\n");
		break;     // FTP_BAD_CMD
		
	  default:
		// There is a bug in getFTPUserCommand if we get here.
		assert(FALSE);
	 }
  } 
  
  return 0;  // Must satisfy this signature to be a thread start function
}

// Returns a pointer to the first non-space before the newline
char*
skipSpaces(char* s) {
  int num;
  for(num = 0; isspace(*(s + num)) && *(s + num) != '\n'; num++);
  return s + num;
}

// Get the next user command and optional argument from the console
// This is pretty hideous -- I could redo it with libg++ regex's and
// Strings if we have extra time :^)
ftp_cmd_t 
getFTPUserCommand(char** arg1, char** arg2) {
  static char lineBuf[200];
  
  // Fill the lineBuf with special token: '?'
  for(int i = 0; i < 200; i++) {
	 lineBuf[i] = '?';
  }

  // Get the next line
  fgets(lineBuf, 200, stdin);
  if (lineBuf[strlen(lineBuf)-1] == '\n') lineBuf[strlen(lineBuf)-1] = '\0';
  
  // Set arg1 to NULL -- reset if there is actually an arg
  *arg1 = NULL;
  
  if (!strncmp(lineBuf, "cd", 2)) {
    *arg1 = skipSpaces(lineBuf + 3);
    return FTP_CMD_CD;
  } else if (!strcmp(lineBuf, "close")) {
    return FTP_CMD_CLOSE;
  } else if (!strcmp(lineBuf, "quit")) {
    return FTP_CMD_QUIT;
  } else if (!strcmp(lineBuf, "exit")) {
    return FTP_CMD_EXIT;
  } else if (!strncmp(lineBuf, "get", 3)) {
    *arg1 = skipSpaces(lineBuf + 4);
    return FTP_CMD_GET;
  } else if (!strcmp(lineBuf, "help")) {
    return FTP_CMD_HELP;
  } else if (!strncmp(lineBuf, "lcd", 3)) {
    *arg1 = skipSpaces(lineBuf + 4);
    return FTP_CMD_LCD;
  } else if (!strcmp(lineBuf, "ls")) {
    return FTP_CMD_LS;
  } else if (!strncmp(lineBuf, "open", 4)) {
    *arg1 = skipSpaces(lineBuf + 5);
    return FTP_CMD_OPEN;
  } else if (!strncmp(lineBuf, "send", 4)) {
    *arg1 = skipSpaces(lineBuf + 5);
    return FTP_CMD_SEND;
  } else if (!strncmp(lineBuf, "setwindow", 9)) {
    *arg1 = skipSpaces(lineBuf + 10);
    return FTP_CMD_SET_WINDOW;
  } else if (!strncmp(lineBuf, "verify", 6)) {
    return FTP_CMD_VERIFY;
  } else if (!strncmp(lineBuf, "set", 3)) {
    *arg1 = skipSpaces(lineBuf + 4);
	 /* First search for first non-space */
	 {
		int num;
		for(num = 0; !isspace(*(*arg1 + num)); num++);
		*arg2 = skipSpaces(*arg1 + num);
	 }
	 *(*arg2 - 1)='\0';
    return FTP_CMD_SET_GARB;
  } else if (!strncmp(lineBuf, "stats", 5)) {
    return FTP_CMD_GARB_STATS;
  } else {
    return FTP_BAD_CMD;
  }
}

// Print a usage message to standard out
void 
showFTPUsage() {
  pthread_mutex_lock(&consoleLock);
  cout << endl;
  cout << "cd [dir]       changes the server's current working directory\n";
  cout << "close          closes the active connection\n";
  cout << "quit           terminate the FTP client session\n";
  cout << "exit           exit the whole process\n";
  cout << "get [file]     gets the specified file from the FTP server\n";
  cout << "help           prints this message\n";
  cout << "lcd [dir]      changes the client's current directory\n";
  cout << "ls             retrieves a listing of the server's current directory\n";
  cout << "open [host]    opens a connection to the specified IPv6 host ID\n";
  cout << "send [file]    sends the specified file to the FTP server\n";
  cout << "setwindow [n]  sets the receive window size to n for future connections\n";
  cout << endl;
  pthread_mutex_unlock(&consoleLock);
}


// Print an FTP message to the console --  basically just a printf wrapper.
void
ftpShowMsg(char *format, ...) {
  va_list ap;
  // You will get an unused variable message here -- ignore it.
  va_start(ap, format);
  vfprintf(stdout, format, ap);
  va_end(ap);
  printf("\n");
  fflush(stdout);
}

// Print an FTP error message to the console and sound the bell --
// basically just a printf wrapper.
void
ftpShowError(char *format, ...) {
  va_list ap;
  // You will get an unused variable message here -- ignore it.
  printf("FTP ERROR - ");
  va_start(ap, format);
  vfprintf(stdout, format, ap);
  va_end(ap);
  printf("\n\a\a");
  fflush(stdout);
}





