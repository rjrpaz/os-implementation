/*
 *  ipv6-garb.h
 *
 *  The public interface for the network garbler module.
 *
 *  Written by: ???
 *  Modified: Kacheong Poon (9-22-1994)
 *  Modified: Kacheong Poon (12-02-1994)
 *            Add garb_stat();
 *  Modified: Srinivasa Narayanan (11/19/1996)
 *            changed garb_sendto() to two routines
 *            garb_sendto_normal() and garb_sendto_routing()
 *            to handle routing cells and normal data cells
 *            differently. 
 *            changed Garbstat to add new garbler statistics 
 *            for routing cells.
 *  Modified: Michael D. Beynon (03-15-1999)
 *            Split garb_sendto_normal() into seperate data and signal
 *            calls.  Cleaned up code, and added stats for the new calls.
 *  Modified: Jeff Hollingsworth (10/6/99)
 *	      IPv6 project
 */

#ifndef IPV6_GARB_H
#define IPV6_GARB_H

#include <sys/types.h>
#include <sys/socket.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * garb_init()
 *
 *	This function must be called before any other function in the
 *	garbler module.  It reads parameters from the file "ipv6-garb.init"
 *	and initializes parameters and statistics. 
 */
extern void garb_init(void);


/*
 * garb_notpacked()
 *
 *      This function (if called) should be called after garb_init and before
 *      any other functions in the garbler module.  It forces interpretation
 *      of the ipv6 header to not assume the bit fields are manually packed in
 *      the header.  This fixes the problem for students using C/C++'s bitfield
 *      construct on a machine that is little endian.
 */
extern void garb_notpacked(void);


/*
 * int garb_sendto_XXX(...)
 *
 *	These routines have the same syntax as sendto().  The probabilities
 *      of discard, trailer drop, and corruption can all be set independently
 *      for each of the three calls.
 */
extern int garb_sendto(int socket_descriptor,
			    const char* buffer, int buffer_len, int flags,
			    const struct sockaddr* to, int tolen);

/* 
 * void garb_print_stats(void)
 *
 * Prints garbler statistics
 */
extern void garb_print_stats(void);

#ifdef __cplusplus
}
#endif

#endif /* IPV6_GARB_H */

