
#ifndef VFS_H
#define VFS_H

#include "fileio.h"

#define FS_TYPE_NONE	0
#define FS_TYPE_PFAT	1
#define FS_TYPE_GOSFS	2

#define MAX_OPEN_FILES	10

typedef struct {
    unsigned int filePos;	// file offset
    unsigned int endPos;	// last byte of file
    int fsType;			// file system type
    int fsId;			// handle specified by file system
    int mode;			// read vs. write;
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
int CreateDirectory(char *buffer);
int SetAcl(char *fileName, int uid, int permissions);

int SetSetUid(char *name, int mode);
int SetEffectiveUid(int uid);


#endif
