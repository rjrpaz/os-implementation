
/*
 * pseudo-fat filesystem.
 *
 */
#include "screen.h"
#include "string.h"
#include "malloc.h"
#include "ide.h"
#include "floppy.h"
#include "pfat.h"

int debugPFAT = 0;

#define HARD_DISK	0
#define FLOPPY_DISK	1

// one cached FAT per drive
int *cachedFAT[2];

int roundToNextBlock(int x)
{
    if (x % 512 == 0) {
        return x;
    } else {
	return (x + 512 - (x % 512));
    }
}

int Block_Read(int floppyDrive, int drive, int block, char *buffer)
{
    if (floppyDrive == 1) {
        // floppy 
	if (debugPFAT) Print("calling block read for floppy drive\n");
	return Floppy_Read(drive, block, buffer);
    } else {
	if (debugPFAT) Print("calling block read for IDE drive\n");
	return IDE_Read(drive, block, buffer);
    }
}

int readDirectory(char *path, int *itemCount, directoryEntry** items)
{
    int i;
    int ret;
    int drive;
    int dirBytes;
    char *dirPtr;
    int floppyDrive;
    char buffer[512];
    bootSector *bootSectorPtr;

    if (debugPFAT) Print ("About to check for root directory for %s\n", path);
    if (!strcmp(path, "/a")) {
	if (debugPFAT) Print ("looking on floppy drive 0 \n");
	floppyDrive = 1;
        drive = 0;
    } else if (!strcmp(path, "/c")) {
	if (debugPFAT) Print ("looking on drive 0\n");
	floppyDrive = 0;
        drive = 0;
    } else {
	Print("invalid path: %s\n", path);
        return -1;
    }

    if (debugPFAT) Print ("About to read root directory\n");
    ret = Block_Read(floppyDrive, drive, 0, buffer);
    if (ret != IDE_ERROR_NO_ERROR) {
	Print("Read of boot sector failed\n");
        return -1;
    }

    bootSectorPtr = (bootSector *) &buffer[PFAT_BOOT_RECORD_OFFSET];
    if (bootSectorPtr->magic != PFAT_MAGIC) {
        Print("Magic number for filesystem is %x, not %x\n",
	    bootSectorPtr->magic, PFAT_MAGIC);
	return -1;
    }

    *itemCount = bootSectorPtr->rootDirectoryCount;
    if (debugPFAT) Print ("Got %d directory Items\n", *itemCount);

    dirBytes = roundToNextBlock(sizeof(directoryEntry) * bootSectorPtr->rootDirectoryCount);
    *items = (directoryEntry *) dirPtr = Malloc_Atomic( dirBytes );

    for (i=0; i < dirBytes; i += 512) {
	ret = Block_Read(floppyDrive, drive, bootSectorPtr->rootDirectoryOffset + i/512, &dirPtr[i]);
	if (ret != IDE_ERROR_NO_ERROR) {
	    Free_Atomic (items);
	    itemCount = 0;
	    return -1;
	}
    }

    if (!cachedFAT[floppyDrive]) {
	cachedFAT[floppyDrive] = Malloc_Atomic( bootSectorPtr->fileAllocationLength * 512 );
	for (i=0; i < bootSectorPtr->fileAllocationLength; i++) {
	    ret = Block_Read(floppyDrive, drive, bootSectorPtr->fileAllocationOffset+i, (char *) &cachedFAT[floppyDrive][i*512/4]);
	    if (ret != IDE_ERROR_NO_ERROR) {
		Free_Atomic (items);
		itemCount = 0;
		return -1;
	    }
	}
    }

    return 0;
}

/* 
 * lookup a file, allocate memory  for it and read it into memory.
 */
int ReadFileAndAllocate(char *fileName, char **buffer, int *length)
{
    int i;
    int ret;
    int count;
    int currBlock;
    int blockCount;
    int floppyDrive;
    char dirPart[3];
    char fullPath[256];
    directoryEntry *items;

    dirPart[0] = '/';
    dirPart[1] = fileName[1];
    dirPart[2] = '\0';

    floppyDrive = 0;
    if (fileName[1] == 'a') floppyDrive = 1;

    ret = readDirectory(dirPart, &count, &items);
    if (ret < 0) return -1;
    for (i=0; i < count; i++) {
	memset(fullPath, '\0', 256);
	strcpy(fullPath, dirPart);
	strcat(fullPath, "/");
	strcat(fullPath, items[i].fileName);
	if (debugPFAT) Print("lookup comparing: %s to %s\n", fullPath, fileName);
	if (!strcmp(fullPath, fileName)) {
	    break;
	}
    }

    if (i == count) {
	Print ("file %s not found\n", fileName);
        return -1;
    }

    if (debugPFAT) Print("at ReadFileAndAllocate size = %d\n", items[i].fileSize);
    *length = items[i].fileSize;
    blockCount = roundToNextBlock(items[i].fileSize) / 512;
    *buffer = Malloc_Atomic( blockCount * 512 );
    currBlock = items[i].firstBlock;
    if (debugPFAT) Print("Reading blocks:");
    for (i=0; i < blockCount; i++) {
	if (debugPFAT) Print ("%d ", currBlock);
        Block_Read(floppyDrive, 0, currBlock, &((*buffer)[i*512]));
	currBlock = cachedFAT[floppyDrive][currBlock];
    }
    if (debugPFAT) Print ("\n");

    Free_Atomic (items);
    return 0;
}

//
// Find the paging file on the disk (/c/swapfile).  If it is exists, verify
//    that it's blocks on contiguous, and return the first block and size
//    of the file (in 512 byte units).
//
int getPagingFileInfo(int *firstBlock, int *size)
{
    int i;
    int ret;
    int count;
    int currBlock;
    int blockCount;
    directoryEntry *items= NULL;
    
    ret = readDirectory("/c", &count, &items);
    if (ret < 0) {
	Print ("swapfile not found\n");
        if (items) Free_Atomic( items);
        return -1;
    }

    for (i=0; i < count; i++) {
      if (debugPFAT) Print("lookup comparing: swapfile to %s\n", items[i].fileName);
      if (!strcmp("swapfile", items[i].fileName)) {
          break;
      }
    }

    if (i == count) {
        Print ("swapfile not found\n");
        Free_Atomic( items);
        return -1;
    }

    // now verify its contiguous
    *size = blockCount = roundToNextBlock(items[i].fileSize) / 512;
    *firstBlock = currBlock = items[i].firstBlock;
    for (i=0; i < blockCount; i++) {
      if (currBlock+1 != cachedFAT[HARD_DISK][currBlock]) {
          Print ("swap file is not contiguous at block %d\n", i);
          Free_Atomic( items);
          return -1;
      }
    }

    Free_Atomic( items);
    return 0;
}

void Init_PFAT()
{
    int i;
    int ret;
    int count;
    directoryEntry *items;

    ret = readDirectory("/a", &count, &items);
    if (ret) {
	Print ("ERROR: Pseudo-FAT filesystem unable to locate root directory\n");
	return;
    }

    Print ("Pseudo-FAT filesystem initialized\n");
    Print ("File     Size\n");
    for (i=0; i < count; i++) {
        Print ("%s    ", items[i].fileName);
        Print ("%d\n", items[i].fileSize);
    }

    Free_Atomic (items);
}
