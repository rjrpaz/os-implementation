// A test program for GeekOS user mode

#include "libuser.h"
#include "libio.h"

void PrintDirectory(char *name)
{
    int i;
    int fd;
    int ret;
    int count;
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
	    Printf("%s ", dirItem.name);
	    count = 0;
	    for (i=0; i < 4; i++) {
	        if (dirItem.stats.acls[i].uid) {
		    if (count) Printf(", ");
		    Printf("%d(", dirItem.stats.acls[i].uid);
		    if (dirItem.stats.acls[i].permission & O_READ) Printf("r");
		    if (dirItem.stats.acls[i].permission & O_WRITE) Printf("w");
		    Printf(")");
		    count = 1;
		}
	    }
	    Printf("\n");
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

    PrintDirectory(argv[1]);

    return 0;
}
