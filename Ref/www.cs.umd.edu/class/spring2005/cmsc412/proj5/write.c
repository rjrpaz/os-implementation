// A test program for GeekOS user mode

#include <conio.h>
#include <process.h>
#include <fileio.h>

int main(int argc, char **argv)
{
  Print("Formatting...\n");
  if (Format("ide1","gosfs") >= 0)
  {
     Print("Mounting...\n");
     if (Mount("ide1", "/d", "gosfs") >= 0)
     {
        Print("Writing...\n");
        int fd = Open("/d/testWrite", O_WRITE|O_CREATE);
        if (fd >= 0)
        {
           char buffer[100] = "Hello.  If you see this your write works.\n";
           if (Write(fd, buffer, 100) == 100)
              Print("Wrote file /d/testWrite\n");
        }
        Print("Sync...\n");
        Sync();
        Print("Done sync\n");
     }
  }

  return 0;
}


