#ifndef FILEIO_H
#define FILEIO_H

#define FS_TYPE_NONE	0
#define FS_TYPE_PFAT	1
#define FS_TYPE_GOSFS	2

#define MAX_OPEN_FILES	10

#define O_CREATE        0x1
#define O_READ          0x2
#define O_WRITE         0x4

typedef struct {
    int size;
    int isDirectory:1;
} fileStat;

typedef struct {
    char name[64];
    fileStat stats;
} dirEntry;

typedef struct {
    char mountPoint[1024];
    int drive;
    char fsType[8];
} mountReq;


#endif
