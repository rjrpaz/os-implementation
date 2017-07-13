
// A test program for GeekOS user mode

#include "libuser.h"
#include "libio.h"

int Main( int argc, char **argv )
{
    int fd;
    int sink, source;

    fd = MessageQueueCreate("pipe1");
    Printf("create mq returned %d\n", fd);

    source = Spawn_Program ("/c/cat.exe /c/file1", 0, fd);
    sink = Spawn_Program ("/c/wc.exe", fd, 1);
    Close(fd);

    Wait(sink);
    Wait(source);

    return 0;
}
