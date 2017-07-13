
#ifndef GOSFS_H
#define GOSFS_H

#define MAX_FILES_PER_DIR	60

#include "fileio.h"
#include "vfs.h"

// do not modify this item
typedef struct {
    int name;				/* offset of name in name block */
    int size;				/* size of the file */
    unsigned int isUsed:1;		/* is entry active */
    unsigned int isDirectory:1;		/* is this file a directory */
    unsigned int isSetuid:1;		/* run with owner's uid */
    unsigned int freeNames:28;		/* For directories, first free block */
    int blocks[10];			/* 8 are for direct blocks, 1 for indirect and 1 for double indirect */
    aclEntry acls[4];			/* first entry is defined to be owner */
} GOSFSfileNode;

// do not modify this item
typedef struct {
    GOSFSfileNode files[MAX_FILES_PER_DIR];	/* all of the files */
} GOSFSdirectory;

typedef struct {
    // ADD your fields here
} GOSFSptr;

int GOSFS_Format(int drive);

GOSFSptr *GOSFS_Open(mountPoint *mp, char *name, int permissions);
int GOSFS_Read(GOSFSptr *f, char *buffer, int size);
int GOSFS_Write(GOSFSptr *f, char *buffer, int size);
int GOSFS_Stat(GOSFSptr *f, fileStat *stat);
int GOSFS_Close(GOSFSptr *f);
int GOSFS_CreateDirectory(mountPoint *mp, char *name);

int GOSFS_Seek(GOSFSptr *f, int offset);
int GOSFS_Delete(mountPoint *mp, char *file);
int GOSFS_SetAcl(mountPoint *mp, char *file, int uid, int permissions);

#endif
