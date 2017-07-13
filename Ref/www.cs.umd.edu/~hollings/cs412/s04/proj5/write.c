// A test program for GeekOS user mode

#include "libuser.h"
#include "libio.h"

int Main( int argc, char *argv[] )
{
  Printf("Formatting...\n");
  if (Format(1) >= 0)
  {
     Printf("Mounting...\n");
     if (Mount("/d", 1, "gosfs") >= 0)
     {
        Printf("Writing...\n");
        int fd = Open("/d/testWrite", O_WRITE|O_CREATE);
        if (fd >= 0)
        {
           char buffer[100] = "Hello.  If you see this your write works.\n";
           if (Write(fd, buffer, 100) == 100)
              Printf("Wrote file /d/testWrite\n");
        }
     }
  }

  return 0;
}


