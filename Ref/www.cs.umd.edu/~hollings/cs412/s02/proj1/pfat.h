
/*
 * Header file for the pseudo-fat filesystem.
 *
 */

typedef struct {
    int magic;			/* id to tell the type of filesystem */
    int fileAllocationOffset;	/* where is the file allocation table */
    int fileAllocationLength;	/* length of allocation table */
    int rootDirectoryOffset;	/* offset in sectors of root directory */
    int rootDirectoryCount;	/* number of items in the directory */
} bootSector;

typedef struct {
    char fileName[8+4];

    /* attribute bits */
    char readOnly:1;
    char hidden:1;
    char systemFile:1;
    char volumeLabel:1;
    char directory:1;

    short time;
    short date;
    int firstBlock;
    int fileSize;
} directoryEntry;

#define FAT_ENTRY_FREE		0
#define FAT_ENTRY_EOF		1

/* magic number to indicate its a PFAT disk */
#define PFAT_MAGIC		0x78320000

void Init_PFAT();
int ReadFileAndAllocate(char *fileName, char **buffer);
