#include "libuser.h"
#include "libio.h"

int Main( int argc, char **argv )
{
    int fd;
    int ret;
    int read;
    fileStat stat;
    char buffer[47];

    ret = Open("/nosuchmount", O_READ);
    if (ret != -1) {
        Printf ("invalid return code open of invalid mount point\n");
    } else {
        Printf("Passed Test #1\n");
    }

    fd = Open("/c/a.exe", O_READ);
    if (fd != 0) {
        Printf ("invalid return code (%d) for open 0th file\n", ret);
    } else {
        Printf("Passed Test #2\n");
    }

    ret = Write(fd, buffer, sizeof(buffer));
    if (ret != -1) {
        Printf ("invalid return code (%d) for write to readonly file\n", ret);
    } else {
        Printf("Passed Test #2\n");
    }

    ret = Read(fd, buffer, sizeof(buffer));
    if (ret != sizeof(buffer)) {
        Printf ("invalid return code (%d) for read at start of file\n", ret);
    } else {
        Printf("Passed Test #4\n");
    }

    read = 0;
    while (ret > 0) {
	read += ret;
	ret = Read(fd, buffer, sizeof(buffer));
    }

    ret = Stat(93, &stat);
    if (ret != -1) {
        Printf ("invalid return code (%d) for stat of non-open file\n", ret);
    } else {
        Printf("Passed Test #5\n");
    }

    ret = Stat(fd, &stat);
    if (ret != 0) {
        Printf ("invalid return code (%d) for stat of open file\n", ret);
    } else {
        Printf("Passed Test #6\n");
    }

    if (read != stat.size) {
        Printf ("%d bytes read from file, stat reports %d int file\n", read, stat.size);
    } else {
        Printf("Passed Test #6\n");
    }

    ret = Close(72);
    if (ret != -1) {
        Printf ("invalid return code (%d) for close of non-open file\n", ret);
    } else {
        Printf("Passed Test #7\n");
    }

    ret = Close(fd);
    if (ret != 0) {
        Printf ("invalid return code (%d) for close of open file\n", ret);
    } else {
        Printf("Passed Test #8\n");
    }
    
    return 0;
}
