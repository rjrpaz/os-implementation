
// A simple cat program for GeekOS

#include "libuser.h"

void Main( int argc, char *argv[] )
{
    int ret;
    int read;
    int inFd;
    fileStat stat;
    char buffer[1024];

    if (argc != 2) {
        Print("usage: cat <file>\n");
	Exit ();
    }

    inFd = Open(argv[1], O_READ);
    if (inFd < 0) {
        Print ("unable to open %s\n", argv[1]);
	Exit();
    }

    ret = Stat(inFd, &stat); 
    if (ret != 0) {
        Print ("error stating file\n");
    }
    if (stat.isDirectory) {
        Print ("cp can not copy directories\n");
	Exit();
    }

    for (read =0; read < stat.size; read += ret) {
        ret = Read(inFd, buffer, sizeof(buffer));
	if (ret < 0) {
	    Print("error reading file for copy\n");
	    Exit();
	}

	buffer[ret] = '\0';
	Write(1, buffer, ret);
    }

    Close(inFd);

    Exit();
}
