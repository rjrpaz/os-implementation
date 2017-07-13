/******************************************************************************
Header:        aal7.h

Description:   AAL7 service interface prototypes

Author(s):     Ben Teitelbaum, Srinivasa Narayanan

Class:         UW/CS640 - Fall 1996

Modifications: 

*/

#ifndef AAL7_H
#define AAL7_H 

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
                Defines for status/return codes 
         (const's not used because not everyone is using C++)
******************************************************************************/
#define AAL7_STAT_OK            0 /* function was successful */

#define AAL7_STAT_FAIL         -1 /* an error occurred not covered by
                                     any other status code */

#define AAL7_STAT_BAD          -2 /* invalid argument */

#define AAL7_STAT_BAD_HOST     -3 /* host specified in a SAP does not
                                     exist */

#define AAL7_STAT_BAD_SAP      -4 /* remote SAP does not exist */

#define AAL7_STAT_INUSE        -5 /* the given SAP is already in use */

#define AAL7_STAT_CLOSED       -6 /* connection closed  */

#define AAL7_STAT_NOSPACE      -7 /* some resource exhausted */

#define AAL7_STAT_RANGE        -8 /* an argument was out-of-range */

#define AAL7_STAT_TIMEOUT      -9 /* a timeout occurred */

#define AAL7_STAT_UNREACHABLE -10 /* the network has become
                                     partitioned or the requested node
                                     is down */


/******************************************************************************
                Type definitions
******************************************************************************/

/******************************************************************************
Type:           host_ID

Description:    An identifier for hosts; should just be the node
                number from the network configuration file
******************************************************************************/
typedef int             host_ID;

/******************************************************************************
Type:           serv_ID

Description:    An identifier for AAL7 services

******************************************************************************/
typedef int             serv_ID;

/******************************************************************************
Type:           aal7_sap

Description:    A type for representing an AAL7 Service Access Point (SAP) 
******************************************************************************/
typedef struct 
{
  host_ID          hostID;
  serv_ID          servID;
} aal7_sap;



/******************************************************************************
               Function prototypes
******************************************************************************/

/******************************************************************************
Function:    aal7_init

Description: Hook to initialize AAL7 and all lower system layers;
             the application layer waits until this returns before
             creating any threads that call other AAL7 service
             interface routines

Parameters:  The command-line arguments, argc and argv are passed
             straight in from main()

Returns: 

  AAL7_STAT_OK - successful initialization
  AAL7_STAT_FAIL - an error occurred

******************************************************************************/

int aal7_init(int argc, 
              char **argv);


/******************************************************************************
Function:    aal7_terminate

Description: Terminate AAL7 and all lower system layers, cleaning up
             appropriately 

Parameters: none

Returns: 

  AAL7_STAT_OK - successful termination
  AAL7_STAT_FAIL - an error occurred

******************************************************************************/

int aal7_term(void);


/******************************************************************************
Function:    aal7_connect

Description: Establish a connection to a remote service access point (SAP)

Parameters:  remote_sap - the SAP to which a connection is desired

Returns:     

  >=0 - the connection was successfully established; the return value
        is a connection descriptor  
  AAL7_STAT_BAD_HOST - the host specified in the SAP does not exist
  AAL7_STAT_BAD_SAP - the remote service requested does not exist
  AAL7_STAT_NOSPACE - insufficient VC-table space exists to complete
                      the connection 
  AAL7_STAT_UNREACHABLE - remote host unreachable due to network partition
  AAL7_STAT_TIMEOUT - timed out trying to establish the connection
                      (timeout length to be determined by you) 

******************************************************************************/

int         aal7_connect(aal7_sap* remote_sap);


/******************************************************************************
Function:    aal7_listen

Description: Register a service access point (SAP) with AAL7

Parameters:  local_sap - the SAP of the service being provided
             limit     - the maximum number of clients that may have
                         connection requests pending for the given
                         service 

Returns: 

  >=0 - the given service was successfully registered; the return
        value is a local descriptor for the SAP to be used in
        subsequent calls to aal7_accept()
  AAL7_STAT_BAD - local_sap was not valid (i.e. was for different host)
  AAL7_STAT_INUSE - the given SAP is already in use
  AAL7_STAT_NOSPACE - all SAPs are already allocated
  AAL7_STAT_RANGE - the value specified for limit is not a positive integer

******************************************************************************/

int         aal7_listen(aal7_sap* local_sap,
                        int limit);

               
/******************************************************************************
Function:    aal7_accept

Description: Block the calling thread until a connection request on
             the given SAP results in the establishment of a new
             connection.

Parameters: aal7_sap_desc - a SAP descriptor 

Returns: 

  >=0 - the connection was successfully established; the return value
        is a connection descriptor for the new connection
  AAL7_STAT_BAD - the given SAP descriptor was not for a valid,
                  registered SAP
  AAL7_STAT_TIMEOUT - a connection request was made, but connection
                      establishment timed out 

******************************************************************************/

int         aal7_accept(int aal7_sap_desc);



/******************************************************************************
Function:    aal7_send

Description: Send data from a user buffer to a remote node across an
             established connection

Parameters:  aal7_conn_desc - a valid connection descriptor returned by
                              either aal7_accept() or aal7_connect()
             packet - pointer to some buffered data
             packet_len - number of bytes to send from packet buffer

Returns: 

  AAL7_STAT_OK - the data from packet was successfully enqueued by
                 AAL7 (packet can be reused)
  AAL7_STAT_BAD - either packet not a valid pointer, packet_len not
                  a positive integer, or aal7_conn_desc not a valid
                  connection descriptor
  AAL7_STAT_UNREACHABLE - connection closed; remote host unreachable
                          due to network partition 
  AAL7_STAT_CLOSED - the connection was closed for some other reason

******************************************************************************/

int         aal7_send(int aal7_conn_desc,
                      char* packet,
                      int packet_len);


/******************************************************************************
Function:    aal7_recv

Description: Receive data from a remote node

Parameters:  aal7_conn_desc - a valid connection descriptor returned by
                              either aal7_accept() or aal7_connect()
             packet_buf - pointer to a buffer where data can be placed 
             max_packet_len - maximum number of bytes that can be
                              placed in packet_buf

Returns: 

  > 0 - data correctly received; the return value is the number of
        bytes received and ready in packet_buf
  AAL7_STAT_BAD - either packet_buf not a valid pointer,
                  max_packet_len not a positive integer, or
                  aal7_conn_desc not a valid connection descriptor
  AAL7_STAT_UNREACHABLE - connection closed; remote host unreachable
                          due to network partition 
  AAL7_STAT_CLOSED - the connection was closed for some other reason

******************************************************************************/

int         aal7_recv(int aal7_conn_desc,
                      char *packet_buf,
                      int max_packet_len);
             

/******************************************************************************
Function:    aal7_disconnect

Description: Close the specified connection

Parameters:  aal7_conn_desc - a connection descriptor returned by either
                              aal7_accept() or aal7_connect()

Returns: 

  AAL7_STAT_OK - connection terminated
  AAL7_STAT_BAD - invalid connection descriptor
  AAL7_STAT_CLOSED - connection already closed 

******************************************************************************/

int         aal7_disconnect(int aal7_conn_desc);


/******************************************************************************
Function:    aal7_setMaxRecvWinSize

Description: Set the maximum size of the AAL7 receive window for the
             given connection

Parameters:  aal7_conn_desc - a valid connection descriptor
             window_size - new maximum window size (in AAL7 packets).

Returns: 
 
  AAL7_STAT_OK - maximum receive window size successfully changed
  AAL7_STAT_BAD - invalid connection descriptor or window size (must
                  be a positive integer) 
  AAL7_STAT_FAIL - an error occurred

******************************************************************************/

int         aal7_setMaxRecvWinSize(int aal7_conn_desc, 
                                   int window_size);


/******************************************************************************
Function:    dump_vc_table

Description: Cause the underlying ATM switch to display a formatted
             dump of the current contents of the VC table 

Parameters:  none

Returns: 
 
  AAL7_STAT_OK - succeeded
  AAL7_STAT_FAIL - an error occurred

******************************************************************************/

int         dump_vc_table(void);

#ifdef __cplusplus
}
#endif


#endif  /* AAL7_H */

