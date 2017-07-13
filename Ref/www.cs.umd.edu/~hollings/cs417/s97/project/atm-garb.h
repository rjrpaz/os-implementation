/*
 *	atm-garb.h
 *
 *	The public interface for the network garbler module.
 *
 *	Written by: ???
 *	Modified: Kacheong Poon (9-22-1994)
 *	Modified: Kacheong Poon (12-02-1994)
 *                Add garb_stat();
 *  Modified: Srinivasa Narayanan (11/19/1996)
 *				  changed garb_sendto() to two routines
 *				  garb_sendto_normal() and garb_sendto_routing()
 *			 	  to handle routing cells and normal data cells
 *				  differently. 
 *				  changed Garbstat to add new garbler statistics 
 *				  for routing cells.
 */

#ifndef ATM_GARB_H
#define ATM_GARB_H

#include <sys/types.h>
#include <sys/socket.h>


#ifdef __cplusplus
  extern "C" {
#endif

/*
 * garb_init()
 *
 *	This function must be called before any other function in the
 *	garbler module.  It reads parameters from the file "atm-garb.init"
 *	and initializes parameters and statistics. 
 */
extern void garb_init(void);

/*
 * int garb_sendto_normal(socket_descriptor, buffer, buffer_len, flags, to, 
 *                        tolen)
 * int buffer_len, flags, socket_descriptor, tolen;
 * const struct sockaddr *to;
 * const char *buffer;
 *
 *	garb_sendto_normal() has exactly the same syntax as sendto.  Note that
 *      buffer_len must be 53, the size of an ATM cell.
 */
extern int garb_sendto_normal(int, const char*, int, int, const struct sockaddr*, int);


/*
 * int garb_sendto_routing(socket_descriptor, buffer, buffer_len, flags, to,
 *                         tolen)
 * int buffer_len, flags, socket_descriptor, tolen;
 * const struct sockaddr *to;
 * const char *buffer;
 *
 *	garb_sendto_routing() has exactly the same syntax as sendto.  Note that
 *      buffer_len must be 53, the size of an ATM cell.
 */
extern int garb_sendto_routing(int, const char*, int, int, const struct sockaddr*, int);

/* 
 * void garb_print_stats(void)
 *
 * Prints garbler statistics
 */
extern void garb_print_stats(void);

#ifdef __cplusplus
  }
#endif

#endif /* ATM_GARB_H */

