
// A simple cat program for GeekOS

#include "libuser.h"
#include "libio.h"

int Main( int argc, char **argv)
{
    int ret;
    int read;
    int inFd;
    fileStat stat;
    char buffer[1024];

    if (argc != 2) {
        Printf("usage: cat <file>\n");
	Exit ();
    }

    inFd = Open(argv[1], O_READ);
    if (inFd < 0) {
        Printf ("unable to open %s\n", argv[1]);
	Exit();
    }

    ret = Stat(inFd, &stat); 
    if (ret != 0) {
        Printf ("error stating file\n");
    }
    if (stat.isDirectory) {
        Printf ("cp can not copy directories\n");
	Exit();
    }

    for (read =0; read < stat.size; read += ret) {
        ret = Read(inFd, buffer, sizeof(buffer));
	if (ret < 0) {
	    Printf("error reading file for copy\n");
	    Exit();
	}

	buffer[ret] = '\0';
	Write(1, buffer, ret);
    }

    Close(inFd);

    return 0;
}
