#include "gosfs.h"
#include "string.h"


// You will implement these for project 5

int GOSFS_Format(int drive)
{
  return -1;
}

GOSFSptr *GOSFS_Open(mountPoint *mp, char *name, int permissions)
{
  return NULL;
}

int GOSFS_Read(GOSFSptr *f, char *buffer, int size)
{
  return -1;
}

int GOSFS_Write(GOSFSptr *f, char *buffer, int size)
{
  return -1;
}

int GOSFS_Stat(GOSFSptr *f, fileStat *stat)
{
  return -1;
}

int GOSFS_Close(GOSFSptr *f)
{
  return -1;
}

int GOSFS_CreateDirectory(mountPoint *mp, char *name)
{
  return -1;
}

int GOSFS_Seek(GOSFSptr *f, int offset)
{
  return -1;
}

int GOSFS_Delete(mountPoint *mp, char *file)
{
  return -1;
}

