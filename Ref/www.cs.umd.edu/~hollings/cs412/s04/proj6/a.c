/* Testing for message queues */

#include "libuser.h"
#include "libio.h"

int Main( int argc , char ** argv )
{
  int fd1, fd2;
  int sink, source;
  int fdin;
  int pipe1, pipe2, pipe3;
  int ret;
  int points = 0;

  /*
    First Part, Testing Wrong Argument
  */

  fd1 = MessageQueueCreate("cat");
  fd2 = MessageQueueCreate("cat");
  if (fd1 < 0 || fd2 < 0) {}
  else {
     points+= 5;
  }

  Close(fd2);
  Close(fd1);

  fdin = Open("/c/test1", O_READ);
  source = Spawn_Program ("/c/cat.exe", fdin, -1);
  Close(fdin);
  Wait(source);
  if (source == -1) {
    points+= 5;
  }

  /*
    Second Part, Standard Test ( a | b ) form
  */

  fd1 = MessageQueueCreate("cat");
  source = Spawn_Program ("/c/cat.exe /c/test1", 0, fd1);
  sink = Spawn_Program ("/c/wc.exe", fd1, 1);
  Close(fd1);
  Wait(source);
  Wait(sink);
  Printf("*********If output '5 18 92' + 15*******\n");

  /*
    Third Part, a | b | c Form
  */

  /* cat | wc | wc */
  fd1 = MessageQueueCreate("dog1");
  fd2 = MessageQueueCreate("dog2");

  pipe1 = Spawn_Program ("/c/cat.exe /c/test1", 0, fd1);
  pipe2 = Spawn_Program ("/c/wc.exe", fd1, fd2);
  pipe3 = Spawn_Program ("/c/wc.exe", fd2, 1);
  Close(fd1);
  Close(fd2);

  Wait(pipe1);
  Wait(pipe2);
  Wait(pipe3);
  Printf("*****If output '1 3 18' + 15*********\n");

  Printf("*****Points so far=%d (add in visual output points)********\n", points);

  return 0;
}

