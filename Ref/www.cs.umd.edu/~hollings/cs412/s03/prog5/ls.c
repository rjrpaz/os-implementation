// A test program for GeekOS user mode

#include "libuser.h"

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
	Print("Error unable to open %s\n", name);
	return;
    }

    // stat the directory and see if its a directory
    ret = Stat(fd, &stat);
    if (!stat.isDirectory) {
        Print("Error, %s is not a directory\n", name);
	return;
    }

    ret = 0;
    while (ret >= 0) {
	ret = Read(fd, (char *) &dirItem, sizeof(dirItem));
	if (ret >= 0) {
	    if (dirItem.stats.isDirectory) 
		Print("d ");
	    else 
		Print("f ");
	    Print("%d ", dirItem.stats.size);
	    Print("%s ", dirItem.name);
	    count = 0;
	    for (i=0; i < 4; i++) {
	        if (dirItem.stats.acls[i].uid) {
		    if (count) Print(", ");
		    Print("%d(", dirItem.stats.acls[i].uid);
		    if (dirItem.stats.acls[i].permission & O_READ) Print("r");
		    if (dirItem.stats.acls[i].permission & O_WRITE) Print("w");
		    Print(")");
		    count = 1;
		}
	    }
	    Print("\n");
	}
    }

    Close(fd);
}

void Main( int argc, char *argv[] )
{
    if (argc != 2) {
        Print("Usage: ls <filename>\n");
	Exit();
    }

    PrintDirectory(argv[1]);
}
