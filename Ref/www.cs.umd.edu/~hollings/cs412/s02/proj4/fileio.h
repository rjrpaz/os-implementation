
#ifndef FILEIO_H
#define FILEIO_H

#define O_CREATE        0x1
#define O_READ          0x2
#define O_WRITE         0x4

#define NULL		0x0

typedef struct {
    unsigned int uid:28;
    unsigned int permission:4;
} aclEntry;

typedef struct {
    int size;
    int isDirectory:1;
    int isSetuid:1;
    aclEntry acls[10];
} fileStat;

typedef struct {
    char name[1024];
    fileStat stats;
} dirEntry;

typedef struct {
    char mountPoint[1024];
    int drive;
    char fsType[8];
} mountReq;

#endif
