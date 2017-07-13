#include "libuser.h"
#include "libio.h"

void PrintfDirectory(char *name)
{
    int fd;
    int ret;
    fileStat stat;
    dirEntry dirItem;

    // read the directory
    fd = Open(name, O_READ);
    if (fd < 0) {
	Printf("Error unable to open %s\n", name);
	return;
    }

    // stat the directory and see if its a directory
    ret = Stat(fd, &stat);
    if (!stat.isDirectory) {
        Printf("Error, %s is not a directory\n", name);
	return;
    }

    ret = 0;
    while (ret >= 0) {
	ret = Read(fd, (char *) &dirItem, 1);
	if (ret >= 0) {
	    if (dirItem.stats.isDirectory) 
		Printf("d ");
	    else 
		Printf("f ");
	    Printf("%d ", dirItem.stats.size);
	    Printf("%s\n", dirItem.name);
	}
    }

    Close(fd);
}

int Main( int argc, char **argv )
{
    if (argc != 2) {
        Printf("Usage: ls <filename>\n");
	Exit();
    }

    PrintfDirectory(argv[1]);
    return 0;
}
