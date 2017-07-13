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

    inFd = Open(file1, O_READ);
    if (inFd < 0) {
        Printf ("unable to open %s\n", file1);
Exit();
    }

    ret = Stat(inFd, &stat);
    if (ret != 0) {
        Printf ("error stating file\n");
Exit();
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
  int ret;
  int points = 0;

  /*
    First Part, Testing Wrong Argument
  */

  ret = Format(1);
  Mount("/d", 1, "gosfs");
  if (ret < 0) {
     Printf("Format failed\n");
     Exit();
  }

  // copy test1
  ret = copyFile("/c/test1", "/d/test1");
  if (ret !=0) {
    Printf("error copy file1\n");
Exit();
  }

  // copy file2 add acls for user 33 to read
  ret = copyFile("/c/test1", "/d/test2");
  if (ret !=0) {
    Printf("error copy test2\n");
    Exit();
  }

  ret = SetAcl("/d/test1", 32, O_READ|O_WRITE);
  if (ret !=0) {
    Printf("error SetAcl test2\n");
Exit();
  }

  ret = SetAcl("/d/test1", 34, O_READ|O_WRITE);
  if (ret !=0) {
    Printf("error SetAcl test2\n");
Exit();
  }

  ret = SetAcl("/d/test2", 23, O_READ|O_WRITE);
  if (ret !=0) {
    Printf("error SetAcl test2\n");
Exit();
  }

  ret = SetAcl("/d/test2", 33, O_READ|O_WRITE);
  if (ret !=0) {
    Printf("error SetAcl test2\n");
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
    Printf("error SetAcl lssetuid\n");
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
  } else {
    points+= 10;

  }

  ret = SetEffectiveUid(34);
  if (ret != -1) {
  } else {
    points+= 10;
  }

  // open test1 - should fail
  ret = Open("/d/test1", O_READ);
  if (ret >= 0) {
  } else {
    points+= 10;
  }

  // open file2 - should succeed
  ret = Open("/d/test2", O_READ);
  if (ret >= 0) {
    points+= 10;
  } else {
  }

  // spawn ls w/o acl should fail
  ret = Spawn_Program("/d/ls.exe /d/test1", 0, 1);
  Wait(ret);
  if (ret < 0) {
  }
  else {
    Printf("********If output 'unable to open' + 10*******\n");
  }

  ret = Spawn_Program("/d/lssetuid.exe /d/test1", 0, 1);
  Wait(ret);
  if (ret >= 0) {
    Printf("*********If output 'success' + 10********\n");
  } else {
  }

  Printf("******Points so far=%d (add in visual output points)*****\n", points);

  return 0;
}

