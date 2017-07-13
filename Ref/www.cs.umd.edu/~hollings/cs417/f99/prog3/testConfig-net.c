#include <stdio.h>

#include "config-net.h"

int main(int argc, char *argv[])
{
    char name[80];
    configInfo info;
    v6addrType addr;

    parseV6Addr(argv[1], addr);

    int ret = config_get_info(addr, &info);
    if (ret != CONFIG_OK) {
        printf("Error %d in parsing netinfo\n", ret);
	exit(-1);
    }
   
    unparseV6Addr(name, info.myV6addr);
    printf("host = %s\n", name);
    for (int i=0; i < info.numLinks; i++) {
        unparseV6Addr(name, info.link[i].ipV6addr);
	printf("  neighbor %d is %s\n", i, name);
    }

    return 0;
}
