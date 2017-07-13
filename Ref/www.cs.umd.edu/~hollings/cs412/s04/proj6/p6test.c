/* Testing for message queues */

#include "libuser.h"
#include "libio.h"

int copyFile(char *file1, char *file2)
{
    int ret;
    int read;
    int inFd;
    int outFd;
    fileStat stat;
    char buffer[1024];

    Printf("copying %s to %s\n", file1, file2);

    inFd = Open(file1, O_READ);
    if (inFd < 0) {
        Printf ("unable to open %s\n", file1);
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

    // now open destination file
    outFd = Open(file2, O_WRITE|O_CREATE);
    if (outFd < 0) {
        Printf ("error opening %s", file2);
  Exit();
    }

    for (read =0; read < stat.size; read += ret) {
        ret = Read(inFd, buffer, sizeof(buffer));
  if (ret < 0) {
      Printf("error reading file for copy\n");
      Exit();
  }

  ret = Write(outFd, buffer, ret);
  if (ret < 0) {
      Printf("error writing file for copy\n");
      Exit();
  }
    }

    Close(inFd);
    Close(outFd);

    return 0;
}

int Main( int argc , char ** argv )
{
  int fd1, fd2;
  int sink, source;
  int fdin;
  int pipe1, pipe2, pipe3;
  int ret;

  /*
    First Part, Testing Wrong Argument
  */

  ret = Format(1);
  Mount("/d", 1, "gosfs");
  if (ret < 0) {
     Printf("Format failed\n");
     Exit();
  }

  fd1 = MessageQueueCreate("cat");
  Printf("create mq returned %d\n", fd1);

  fd2 = MessageQueueCreate("cat");
  Printf("create mq returned %d\n", fd2);
  Close(fd2);
  Close(fd1);

  fdin = Open("/c/file1", O_READ);
  source = Spawn_Program ("/c/cat.exe", fdin, -1);
  Close(fdin);
  Wait(source);
  if (source == -1) {
    Printf("Part 1: arguments 1 test, Success.\n");
  }
  else {
    Printf("Part 1: arguments 1 test, Failed.\n");
  }

  /*
    Second Part, Standard Test ( a | b ) form
  */

  fd1 = MessageQueueCreate("cat");
  source = Spawn_Program ("/c/cat.exe /c/file1", 0, fd1);
  sink = Spawn_Program ("/c/wc.exe", fd1, 1);
  Close(fd1);
  Wait(source);
  Wait(sink);
  Printf("Part 2: if output 5 18 92 then, Success.\n");

  /*
    Third Part, a | b | c Form
  */

  /* cat | wc | wc */
  fd1 = MessageQueueCreate("dog1");
  fd2 = MessageQueueCreate("dog2");

  pipe1 = Spawn_Program ("/c/cat.exe /c/file1", 0, fd1);
  pipe2 = Spawn_Program ("/c/wc.exe", fd1, fd2);
  pipe3 = Spawn_Program ("/c/wc.exe", fd2, 1);
  Close(fd1);
  Close(fd2);

  Wait(pipe1);
  Wait(pipe2);
  Wait(pipe3);

  Printf("Part 3: if output 1 3 18 then, Success.\n");

  /* Part 4 */

  // copy file1
  ret = copyFile("/c/file1", "/d/file1");
  if (ret !=0) {
    Printf("error copy file1\n");
    Exit();
  }

  // copy file2 add acls for user 33 to read
  ret = copyFile("/c/file1", "/d/file2");
  if (ret !=0) {
    Printf("error copy file2\n");
    Exit();
  }

  ret = SetAcl("/d/file1", 32, O_READ|O_WRITE);
  if (ret !=0) {
    Printf("error SetAcl file2\n");
    Exit();
  }

  ret = SetAcl("/d/file1", 34, O_READ|O_WRITE);
  if (ret !=0) {
    Printf("error SetAcl file2\n");
    Exit();
  }

  ret = SetAcl("/d/file2", 23, O_READ|O_WRITE);
  if (ret !=0) {
    Printf("error SetAcl file2\n");
    Exit();
  }

  ret = SetAcl("/d/file2", 33, O_READ|O_WRITE);
  if (ret !=0) {
    Printf("error SetAcl file2\n");
    Exit();
  }

  // copy ls.exe
  ret = copyFile("/c/opentest.exe", "/d/ls.exe");
  if (ret !=0) {
    Printf("error copy ls.exe\n");
    Exit();
  }


  ret = SetAcl("/d/ls.exe", 33, O_READ);
  if (ret !=0) {
    Printf("error SetAcl ls\n");
    Exit();
  }

  // copy ls.exe with setuid enabled.
  ret = copyFile("/c/opentest.exe", "/d/lssetuid.exe");
  if (ret !=0) {
    Printf("error copy lssetuid.exe\n");
    Exit();
  }

  ret = SetAcl("/d/lssetuid.exe", 33, O_READ);
  if (ret !=0) {
    Printf("error SetAcl file2\n");
    Exit();
  }

  ret = SetSetUid("/d/lssetuid.exe", 1);
  if (ret !=0) {
    Printf("error setuid lssetuid.exe\n");
    Exit();
  }

  /* start of test */

  ret = SetEffectiveUid(33);
  ret = GetUid();
  if (ret != 33) {
    Printf("Part 4: SetEffectiveUid/GetUid 1, Failed.\n");
  } else {
    Printf("Part 4: SetEffectiveUid/GetUid 1, Success.\n");
  }

  ret = SetEffectiveUid(34);
  if (ret != -1) {
    Printf("Part 4: SetEffectiveUid/GetUid 2, Failed.\n");
  } else {
    Printf("Part 4: SetEffectiveUid/GetUid 2, Success.\n");
  }

  // open file1 - should fail
  ret = Open("/d/file1", O_READ);
  if (ret >= 0) {
    Printf("Part 4: Open check 1, Failed.\n");
  } else {
    Printf("Part 4: Open check 1, Success.\n");
  }

  // open file2 - should succeed
  ret = Open("/d/file2", O_READ);
  if (ret >= 0) {
    Printf("Part 4: Open check 2, Success.\n");
  } else {
    Printf("Part 4: Open check 2, Failed.\n");
  }

  // spawn ls w/o acl should fail
  ret = Spawn_Program("/d/ls.exe /d/file1", 0, 1);
  Wait(ret);
  if (ret < 0) {
    Printf("Part 5: SetSetUid 1, Failed.\n");
  }
  else {
    Printf("Part 5: SetSetUid 1, Success if output unable to open /d.\n");
  }

  ret = Spawn_Program("/d/lssetuid.exe /d/file1", 0, 1);
  Wait(ret);
  if (ret >= 0) {
    Printf("Part 5: SetSetUid 2, Success if printed that it opened /d.\n");
  } else {
    Printf("Part 5: SetSetUid 2, Failed.\n");
  }

  Printf("You should see this message at end!!!\n");

  return 0;
}

