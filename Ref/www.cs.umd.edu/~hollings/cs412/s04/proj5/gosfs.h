
#ifndef GOSFS_H
#define GOSFS_H


#include "fileio.h"
#include "vfs.h"


typedef struct {
    char name[64];			/* name of file */
    int size;      			/* size of the file */
    unsigned int isUsed:1;		/* is entry active */
    unsigned int isDirectory:1;		/* is this file a directory */
    int blocks[10];			/* 8 are for direct blocks, 1 for indirect and 1 for double indirect */
} GOSFSfileNode;

/* should fit in one block */
#define MAX_FILES_PER_DIR  (4096/(sizeof(GOSFSfileNode))) 


typedef struct {
    GOSFSfileNode files[MAX_FILES_PER_DIR];	/* all of the files */
} GOSFSdirectory;


typedef struct {
  //info about open file:
  unsigned int filePos;                          /* current position of filepointer for open file */
  unsigned int endPos;                           /* current position of last byte in file         */

  //location of filenode on disk:
  unsigned int blockNum;                         /* number of block containing filenode */
  unsigned int offset;                           /* offset of filenode within block     */
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


#endif
