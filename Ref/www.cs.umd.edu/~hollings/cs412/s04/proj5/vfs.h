#ifndef VFS_H
#define VFS_H

#include "fileio.h"


// fsType == FS_TYPE_NONE represents a free slot

typedef struct {
    int fsType;			// file system type
    int fsId;			// handle specified by file system
    int mode;			// read vs. write;
    unsigned int filePos;       // current position of filepointer for open file 
    unsigned int endPos;        // current position of last byte in file         
} openFile;

typedef struct {
    char *mountPoint;
    int drive;
    int fsType;
} mountPoint;

int Mount(char *name, int drive, char *fstype);
int Open(char *name, int permissions);
int Close(int fd);
int Delete(char *name);
int Read(int fd, char *buffer, int length);
int Write(int fd, char *buffer, int length);
int Stat(int fd, fileStat *stat);
int Seek(int fd, int offset);
int CreateDirectory(char *name);
int Format(int drive);

#endif
