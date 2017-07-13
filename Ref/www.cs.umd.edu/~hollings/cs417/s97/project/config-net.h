
/*
 * net-config.h - read a network configuration file and return my neighbors.
 *
 */

#define CONFIG_MAX_HNAME	 64
#define CONFIG_MAXLINKS		 16

/* link - one neighbor */
typedef struct {
    char hostname[CONFIG_MAX_HNAME];	/* hostname for the neighbor */
    int port;				/* port number for this router */
    int nodeNumber;			/* global node id for this router */
} netLink;

/* information returned by parse routine about a single node */
typedef struct {
    netLink link[CONFIG_MAXLINKS];	/* data about each link */
    int numLinks;			/* number of links */
    int myNodeNumber;			/* global id for this router */
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
int config_get_info(int node, configInfo *);
