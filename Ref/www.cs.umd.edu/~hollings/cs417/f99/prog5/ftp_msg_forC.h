/******************************************************************************
Header:		ftp_msg.h

Description:	FTP Message Class Prototype

Author(s):	Ben Teitelbaum

Class:		UW/CS640

Modifications:

$Log: ftp_msg.h,v $

******************************************************************************/

#ifndef FTP_MSG_H
#define FTP_MSG_H

/******************************************************************************
			     Include header files
******************************************************************************/
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "status.h"
#include "globals.h"
#include "types.h"
extern "C" {
#include "ipv6socket.h"
}

/******************************************************************************
			       Define constants
******************************************************************************/

// An FTP message has the following format:
//
// | --- 1 byte --- | --- 4 bytes --- | --- <= FTP_MAX_MSG_SIZE - 5 bytes --- |
//      MSGTYPE            LENGTH                    FNAME

// The FNAME field is a NULL-terminated string value that is set in
// the CDR, GR, and SR commands to either a directory to change to, or
// a filename to get or send

// An FTP data packet is a special case of an FTP message and has
// format:
//
// | --- 1 byte --- | --- 4 bytes --- | --- <= FTP_MAX_MSG_SIZE - 5 bytes --- |
//    MSGTYPE=DATA         LENGTH                The Payload
//
// Possible values of the MSGTYPE field are:
//
// FTP_GR         get request
// FTP_SR         send request
// FTP_SROK       send request acknowledgement
// FTP_CDR        change working directory request
// FTP_CDOK       change working directory acknowledgement
// FTP_LSR        list directory contents request
// FTP_ERRFNAME   file error
// FTP_ERR        generic error
// FTP_CLIABORT   client abort transfer request
// FTP_CLIABORTOK client abort transfer acknowledgment
// FTP_FILESTART  beginning of file
// FTP_EOF        end of file
// FTP_EOFOK      end of file acknowledgement
// FTP_DATA       data packet

typedef enum {FTP_BOGUS,         // 0
				  FTP_GR,				// 1
				  FTP_SR,				// 2
				  FTP_SROK,				// 3
				  FTP_CDR,				// 4
				  FTP_CDOK,				// 5
				  FTP_LSR,				// 6
				  FTP_ERRFNAME,		// 7
				  FTP_ERR,				// 8
				  FTP_CLIABORT,		// 9
				  FTP_CLIABORTOK,		// 10
				  FTP_FILESTART,		// 11
				  FTP_EOF,				// 12
				  FTP_EOFOK,         // 13
				  FTP_DATA,				// 14
				  FTP_MSG_TYPE_OVERFLOW} msg_type_t;

const int FTP_MAX_MSG_SIZE = 4095;
const int FTP_TYPE_FIELD_SIZE = 1;
const int FTP_LENGTH_FIELD_SIZE = 4;
const int FTP_HEADER_SIZE = FTP_TYPE_FIELD_SIZE + FTP_LENGTH_FIELD_SIZE;
const int FTP_MAX_PAYLOAD_SIZE = FTP_MAX_MSG_SIZE - FTP_HEADER_SIZE;

/******************************************************************************
			   Class and Type definitions
******************************************************************************/

class ftp_msg_c {
friend ostream& operator << (ostream&, ftp_msg_c&);
public:
  ftp_msg_c(char* oName) {
	 msgBuf = new char[FTP_MAX_MSG_SIZE];
    thrName = oName;
	 setLength(STAT_BAD);
  }

  // Three pseudo constructors!!!
  //
  // Note: init's can be called repeatedly on a ftp_msg_c object

  // This little pseudo-constructor for out-going signalling
  // messages...
  //
  // Returns:
  // STAT_OK  Always
  status_t init(msg_type_t msgType, char* fileName = NULL);

  // This little pseudo-constructor for out-going data packets; gets
  // next chunk (up to 4095 bytes) from specified file...
  //
  // Returns:
  // STAT_OK   if everything hunky dorey
  // STAT_EOF  if the eof has been reached
  // STAT_FAIL if there was an I/O error reading from f
  status_t init(ifstream* f);

  // This little pseudo-constructor for receiving the next FTP message
  // over the specified connection...
  //
  // Returns: STAT_OK or aal7 status codes
  int init(int aal7_conn_desc);

  // kill me
  ~ftp_msg_c() { delete msgBuf; };

  // Send the message over the given AAL7 connection
  //
  // Returns:  STAT_OK or aal7 error status code
  int sendOverConnection(int aal7_conn_desc) const;

  void GetPayload(char* dest) {
    strcpy(dest, msgBuf + FTP_HEADER_SIZE);
  }

// Public Accessors
public:
  // Get the message type
  msg_type_t getType(void) const { return (msg_type_t) msgBuf[0]; }

  int getLength(void) const;

  // Is msg valid?

  bool isValid(void) const { return (getLength() != STAT_BAD); }

// Private Accessors
private:
  void setType(const msg_type_t mt) { msgBuf[0] = (char) mt; }
  void setLength(int len);
  int recvNextMsg(int aal7_conn_desc);

// Private data members
private:
  char* msgBuf;								// Either FNAME or data payload
  char* thrName;							// Owning thread name
};

// Insertion operator overloaded so that ftp_msg_c's can dump
// themselves out to a stream.
ostream&
operator << (ostream& os, ftp_msg_c& msg);


/******************************************************************************
                           Preprocessor Macros
******************************************************************************/
#endif // FTP_MSG_H

