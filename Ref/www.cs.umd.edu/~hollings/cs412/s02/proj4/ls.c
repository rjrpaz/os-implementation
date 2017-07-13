// A test program for GeekOS user mode

#include "libuser.h"

void PrintDirectory(char *name)
{
    int fd;
    int ret;
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
	    Print("%s\n", dirItem.name);
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
