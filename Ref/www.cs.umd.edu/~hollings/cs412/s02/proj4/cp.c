
// A simple CP program for GeekOS

#include "libuser.h"

void Main( int argc, char *argv[] )
{
    int ret;
    int read;
    int inFd;
    int outFd;
    fileStat stat;
    char buffer[1024];

    if (argc != 3) {
        Print("usage: cp <file1> <file2>\n");
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

    // now open destination file
    outFd = Open(argv[2], O_WRITE|O_CREATE);
    if (outFd < 0) {
        Print ("error opening %s", argv[2]);
	Exit();
    }

    for (read =0; read < stat.size; read += ret) {
        ret = Read(inFd, buffer, sizeof(buffer));
	if (ret < 0) {
	    Print("error reading file for copy\n");
	    Exit();
	}

	ret = Write(outFd, buffer, ret);
	if (ret < 0) {
	    Print("error writing file for copy\n");
	    Exit();
	}
    }

    Close(inFd);
    Close(outFd);

    Exit();
}
