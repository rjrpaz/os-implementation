
#ifndef CONFIG_NET_H
#define CONFIG_NET_H

/*
 * net-config.h - read a network configuration file and return my neighbors.
 *
 */

typedef unsigned char v6addrType[16];

#define CONFIG_MAX_HNAME	 64
#define CONFIG_MAXLINKS		 16

/* link - one neighbor */
typedef struct {
    char hostname[CONFIG_MAX_HNAME];	/* hostname for the neighbor */
    int port;				/* port number for this router */
    v6addrType ipV6addr;		/* global node id for this router */
} netLink;

/* information returned by parse routine about a single node */
typedef struct {
    netLink link[CONFIG_MAXLINKS];	/* data about each link */
    int numLinks;			/* number of links */
    v6addrType myV6addr;		/* global id for this router */
    int myPort;				/* port number for this router */
    char myName[CONFIG_MAX_HNAME];	/* my name */
} configInfo;


/* possible return values from config_get_info */
#define CONFIG_OK	0		/* info contains the data */
#define CONFIG_NO_FILE	1		/* unable to open the config file */
#define CONFIG_SYNTAX_ERROR	2	/* syntax error in the config file */
#define CONFIG_BAD_VALUE	3	/* invalid link # in the config file */
#define CONFIG_BAD_HOST		4	/* request node is not in the network */

/* interface to network configuration file */
/*   node - what's my node number */
/*   info - data about my neighbors */ 
int config_get_info(v6addrType node, configInfo *);

/* parse a string ipv6 address into an array of 16 octets 
   return 1 on sucess on 0 on failure.
   supports :: syntax f810::0001
*/
int parseV6Addr(char *str, v6addrType v6addr);

/* 
 * Return a convert a v6 address back into a string
 */
void unparseV6Addr(char *str, v6addrType v6addr);

#endif
