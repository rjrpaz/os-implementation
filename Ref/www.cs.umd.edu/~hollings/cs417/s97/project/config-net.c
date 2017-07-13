/*
 * config-net.c - read and parse a network configuration file.
 *
 */

#include <stdio.h>
#include <memory.h>

#include "config-net.h"

int config_get_info(int me, configInfo *info)
{
    int i;
    FILE *fp;
    char *ch;
    int count;
    int bytes;
    int lcount;
    int id, port;
    char line[80];
    static int called = 0;
    char name[CONFIG_MAX_HNAME];
    char *fileName = "network.config";
    configInfo fullConfig[CONFIG_MAXLINKS*CONFIG_MAXLINKS];

    /* prevent multiple calls */
    if (called) abort();
    called = 1;

    memset(fullConfig, 0, sizeof(fullConfig));

    if ((me < 0) || (me > CONFIG_MAXLINKS*CONFIG_MAXLINKS)) {
	return CONFIG_BAD_HOST;
    }

    fp = fopen(fileName, "r");
    if (!fp) {
	return CONFIG_NO_FILE;
    }

    while (fgets(line, sizeof(line)-1, fp)) {
	if (line[0] == '\n') continue;
	if (line[0] == '/' && line[1] == '/') continue;

	count = sscanf(line, "Node %d (%s %d) links%n", &id, name, &port, &bytes);
	if (count != 3 || (name[strlen(name)-1] != ',')) {
	    fclose(fp);
	    return CONFIG_SYNTAX_ERROR;
	}
	if ((id < 0) || (id > CONFIG_MAXLINKS*CONFIG_MAXLINKS)) {
	    fclose(fp);
	    return CONFIG_BAD_VALUE;
	}

	name[strlen(name)-1] = '\0';

	/* read the integers with the link numbers */
	for (ch = &line[bytes],lcount=0;; ch += bytes) {
	    count = sscanf(ch, "%d %n", 
		&fullConfig[id].link[lcount++].nodeNumber, &bytes);
	    if ((fullConfig[id].link[lcount-1].nodeNumber < 0) ||
		(fullConfig[id].link[lcount-1].nodeNumber > 
		 CONFIG_MAXLINKS*CONFIG_MAXLINKS)) {
		fclose(fp);
		return CONFIG_BAD_VALUE;
	    }

	    if (count != 1) {
		lcount--;
		break;
	    }
	}

	fullConfig[id].myNodeNumber = id;
	fullConfig[id].numLinks = lcount;
	strcpy(fullConfig[id].myName, name);
	fullConfig[id].myPort =  port;
    }

    fclose(fp);

    for (i=0; i < fullConfig[me].numLinks; i++) {
	int j;
	j = fullConfig[me].link[i].nodeNumber;
	if (!fullConfig[j].myPort) {
	    return CONFIG_BAD_VALUE;
	}
	fullConfig[me].link[i].port = fullConfig[j].myPort;
	strcpy(fullConfig[me].link[i].hostname, fullConfig[j].myName);
    }

    *info = fullConfig[me];
    return CONFIG_OK;
}
