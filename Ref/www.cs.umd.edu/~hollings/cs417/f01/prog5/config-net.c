/*
 * config-net.c - read and parse a network configuration file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <memory.h>
#include <malloc.h>

#include "config-net.h"

char zeroAddr[16];

int parseV6Addr(char *inString, v6addrType v6addr)
{
    int ret;
    int val;
    char *str;
    int i = 0;
    char *temp;
    char *startStr;
    
    startStr = str = strdup(inString);

    for (i=0; i < 16; i++) v6addr[i] = 0;

    i=0;
    while (str) {
	ret = sscanf(str, "%x:", &val);
	if (ret == 1) {
	   v6addr[i] = val / 0x100;
	   v6addr[i+1] = val % 0x100;
	   i += 2;
	}
	str = strchr(str, ':');
	if (!str) {
	    free(startStr);
	    return 0;
	}
	str++;

	if (*str == ':') {
	    break;
	}
    }

    i = 15;
    temp = strrchr(str, ':');
    while (temp && *temp == ':') {
	/* scan backward to fill in trailing part of address */
	ret = sscanf(temp+1, "%x:", &val);
	if (ret == 1) {
	   v6addr[i-1] = val / 0x100;
	   v6addr[i] = val % 0x100;
	}

	/* null terminate the one we just parsed and back up */
	*temp = '\0';
	temp = strrchr(str, ':');
    }

    free(startStr);
    return 1;
}

static void convertHex(char *str, int val)
{
    if (val <= 9) {
	*str = val + '0';
    } else {
	*str = val + 'a' - 10;
    }
}

void unparseV6Addr(char *str, v6addrType v6addr)
{
    int i = 0;
    int usedColon = 0;

    for (i=0; i < 16; i += 2) {
	if (i) *(str++) = ':';
	if (i && (v6addr[i] == 0) && (v6addr[i+1] == 0) && !usedColon) {
	    usedColon = 1;
	    *(str++) = ':';
	    while ((i < 14) && (v6addr[i] == 0) && (v6addr[i+1] == 0)) i += 2;
	}
	if (i < 16) {
	    convertHex(str, v6addr[i]/16);
	    str++;
	    convertHex(str, v6addr[i]%16);
	    str++;
	    convertHex(str, v6addr[i+1]/16);
	    str++;
	    convertHex(str, v6addr[i+1]%16);
	    str++;
	}
    }
    *str = '\0';
}

int config_get_info(v6addrType targetv6addr, configInfo *info)
{
    int me;
    FILE *fp;
    char *ch;
    int count;
    int bytes;
    int lcount;
    int i, j, k;
    int id, port;
    char host[64];
    char line[512];
    static int called = 0;
    char name[CONFIG_MAX_HNAME];
    char *fileName = "network.config";
    configInfo fullConfig[CONFIG_MAXLINKS*CONFIG_MAXLINKS];
    configInfo dummyConfig;

    /* prevent multiple calls */
    if (called) abort();
    called = 1;

    memset(fullConfig, 0, sizeof(fullConfig));

    fp = fopen(fileName, "r");
    if (!fp) {
	return CONFIG_NO_FILE;
    }

    id = 0;
    while (fgets(line, sizeof(line)-1, fp)) {
	if (line[0] == '\n') continue;
	if (line[0] == '#') continue;
	if (line[0] == '/' && line[1] == '/') continue;

	count = sscanf(line, "Node %[0-9a-f:] (%s %d) links%n", host, name, &port, &bytes);
	if (count != 3 || (name[strlen(name)-1] != ',')) {
	    fclose(fp);
	    return CONFIG_SYNTAX_ERROR;
	}
	if (!parseV6Addr(host, fullConfig[id].myV6addr)) {
	    fclose(fp);
	    return CONFIG_BAD_VALUE;
	}

	name[strlen(name)-1] = '\0';

	/* read the integers with the link numbers */
	for (ch = &line[bytes],lcount=0;; ch += bytes) {
	    count = sscanf(ch, " %[:0-9a-f]%n", host, &bytes);
	    if (count != 1) {
		break;
	    }

	    if (!parseV6Addr(host, fullConfig[id].link[lcount++].ipV6addr)) {
		return CONFIG_BAD_VALUE;
	    }

	}

	fullConfig[id].numLinks = lcount;
	strcpy(fullConfig[id].myName, name);
	fullConfig[id].myPort =  port;
	id++;
    }

    fclose(fp);

    /* now populate connection information */
    for (i=0; i < id; i++) {
	for (j=0; j < fullConfig[i].numLinks; j++) {
	    for (k=0; k < id; k++) {
		if (!bcmp((char *) fullConfig[i].link[j].ipV6addr, 
			  (char *) fullConfig[k].myV6addr, 16)) {
		    fullConfig[i].link[j].port = fullConfig[k].myPort;	
		    strcpy(fullConfig[i].link[j].hostname,fullConfig[k].myName);
	        }
	    }
	}
    }

    if (!bcmp((char *) targetv6addr, (char *) zeroAddr, 16)) {
	/* return list of all hosts as directly connected */
	for (i=0; i < id; i++) {
	    dummyConfig.link[i].port = fullConfig[i].myPort;
	    strcpy(dummyConfig.link[i].hostname, fullConfig[i].myName);
	    memcpy(dummyConfig.link[i].ipV6addr, fullConfig[i].myV6addr, 16);
	}
	dummyConfig.numLinks = id;
	*info = dummyConfig;
	return CONFIG_OK;
    }

    for (i=0, me=-1; i < id; i++) {
	if (!bcmp((char *) targetv6addr, (char *) fullConfig[i].myV6addr, 16)) me = i;
    }
    if (me == -1) {
	return CONFIG_BAD_VALUE;
    }

    *info = fullConfig[me];
    return CONFIG_OK;
}
