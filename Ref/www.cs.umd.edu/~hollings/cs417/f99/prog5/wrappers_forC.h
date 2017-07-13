/******************************************************************************
Header:         wrappers.h

Description:    Wrappers for functions to print error messages

Author(s):      Ben Teitelbaum

Class:          UW/CS640

Modifications:

$Log: ftp_server.h,v $

******************************************************************************/

#ifndef WRAPPER_H
#define WRAPPER_H

extern "C" {
#include "ipv6socket.h"
}
#include "ftp_client.h"
#include "ftp_server.h"

void
print_recv_error(int connStatus, int connectDesc, bool fromFTPD);

void
print_send_error(int connStatus, int connectDesc, bool fromFTPD);

#endif // WRAPPER_H
