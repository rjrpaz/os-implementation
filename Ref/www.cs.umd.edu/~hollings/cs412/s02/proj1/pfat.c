
/*
 * pseudo-fat filesystem.
 *
 */
#include "screen.h"
#include "string.h"
#include "malloc.h"
#include "ide.h"
#include "pfat.h"

int debugPFAT;

int *cachedFAT;

int roundToNextBlock(int x)
{
    if (x % 512 == 0) {
        return x;
    } else {
	return (x + 512 - (x % 512));
    }
}

int readDirectory(char *path, int *itemCount, directoryEntry** items)
{
    int ret;
    int drive;
    char buffer[512];
    bootSector *bootSectorPtr;

    if (debugPFAT) Print ("About to check for root directory for %s\n", path);
    if (!strcmp(path, "/c")) {
	if (debugPFAT) Print ("looking on drive 0\n");
        drive = 0;
    } else {
	Print("invalid path: %s\n", path);
        return -1;
    }

    if (debugPFAT) Print ("About to read root directory\n");
    ret = IDE_Read(drive, 0, buffer);
    if (ret != IDE_ERROR_NO_ERROR) {
	Print("Read of boot sector failed\n");
        return -1;
    }

    bootSectorPtr = (bootSector *) buffer;
    if (bootSectorPtr->magic != PFAT_MAGIC) {
        Print("Magic number for filesystem is %x, not %x\n",
	    bootSectorPtr->magic, PFAT_MAGIC);
    }

    *itemCount = bootSectorPtr->rootDirectoryCount;
    if (debugPFAT) Print ("Got %d directory Items\n", *itemCount);

    *items = Malloc_Atomic( roundToNextBlock( sizeof(directoryEntry) * *itemCount ));

    ret = IDE_Read(drive, bootSectorPtr->rootDirectoryOffset, (char *) *items);
    if (ret != IDE_ERROR_NO_ERROR) {
        Free_Atomic (items);
	itemCount = 0;
	return -1;
    }

    if (!cachedFAT) {
	cachedFAT = Malloc_Atomic( bootSectorPtr->fileAllocationLength * 512 );
	ret = IDE_Read(drive, bootSectorPtr->fileAllocationOffset, (char *) cachedFAT);
	if (ret != IDE_ERROR_NO_ERROR) {
	    Free_Atomic (items);
	    itemCount = 0;
	    return -1;
	}
    }

    return 0;
}

/* 
 * lookup a file, allocate memory  for it and read it into memory.
 */
int ReadFileAndAllocate(char *fileName, char **buffer)
{
    int i;
    int ret;
    int count;
    int currBlock;
    int blockCount;
    char fullPath[256];
    directoryEntry *items;

    ret = readDirectory("/c", &count, &items);
    for (i=0; i < count; i++) {
	memset(fullPath, '\0', 256);
	strcpy(fullPath, "/c/");
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

    blockCount = roundToNextBlock(items[i].fileSize) / 512;
    *buffer = Malloc_Atomic( blockCount * 512 );
    currBlock = items[i].firstBlock;
    if (debugPFAT) Print("Reading blocks:");
    for (i=0; i < blockCount; i++) {
	if (debugPFAT) Print ("%d ", currBlock);
        IDE_Read(0, currBlock, &((*buffer)[i*512]));
	currBlock = cachedFAT[currBlock];
    }
    if (debugPFAT) Print ("\n");

    Free_Atomic (items);
    return 0;
}

void Init_PFAT()
{
    int i;
    int ret;
    int count;
    directoryEntry *items;

    ret = readDirectory("/c", &count, &items);
    if (ret) {
	Print ("ERROR: Pseudo-FAT filesystem unable to locate root directory\n");
    }

    Print ("Pseudo-FAT filesystem initialized\n");
    Print ("File     Size\n");
    for (i=0; i < count; i++) {
        Print ("%s    ", items[i].fileName);
        Print ("%d\n", items[i].fileSize);
    }

    Free_Atomic (items);
}
