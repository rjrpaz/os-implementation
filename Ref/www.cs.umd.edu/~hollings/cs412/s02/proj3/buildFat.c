#include "../pfat.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int roundToNextBlock(int x)
{
    if (x % 512 == 0) {
        return x;
    } else {
        return (x + 512 - (x % 512));
    }
}

int main(int argc, char *argv[])
{
    int i;
    int fd;
    int fd2;
    int ret;
    int *fat;
    int blocks;
    int fileSize;
    int fileCount;
    char *imageFile;
    int usableBlocks;
    struct stat sbuf;
    int firstFreeBlock;
    bootSector bSector;
    int totalFileBlocks = 0;
    directoryEntry *directory;

    if (argc <= 1) {
        printf("usage: makeFat <diskImage> <files>\n");
	exit(-1);
    }

    imageFile = argv[1];

    ret = stat(imageFile, &sbuf);
    if (ret) {
        perror("stat");
	printf("Does %s exist?\n", imageFile);
	exit(-1);
    }

    fileCount = argc - 2;
    fileSize = sbuf.st_size;
    if (fileSize % 512 != 0) {
        printf("image is not a multiple of 512 bytes\n");
	exit(-1);
    }

    blocks = fileSize / 512;

    bSector.magic = PFAT_MAGIC;
    bSector.fileAllocationOffset = 1;
    bSector.fileAllocationLength = roundToNextBlock(blocks)/512*4;
    fat = (int *) calloc(blocks, sizeof(int));
    bSector.rootDirectoryOffset = bSector.fileAllocationLength + 1;
    bSector.rootDirectoryCount = fileCount;

    fd = open(imageFile, O_WRONLY, 0);
    if (fd < 0) {
        perror("image File open:");
	exit(-1);
    }

    ret = write(fd, &bSector, sizeof(bSector));
    assert(ret == sizeof(bSector));

    firstFreeBlock = bSector.rootDirectoryOffset + 
        roundToNextBlock(sizeof(directoryEntry) * fileCount)/ 512;
    usableBlocks = blocks - firstFreeBlock;
    printf("first data blcoks is %d\n", firstFreeBlock);

    directory = (directoryEntry*) malloc(sizeof(directoryEntry) * fileCount);
    for (i=0; i < fileCount; i++) {
	int j;
	int numBlocks;

        strncpy(directory[i].fileName, argv[i+2], 13);
	directory[i].firstBlock = firstFreeBlock;

	ret = stat(directory[i].fileName, &sbuf);
	assert(ret == 0);
	numBlocks = roundToNextBlock(sbuf.st_size)/512;
	directory[i].fileSize = sbuf.st_size;

	for (j=0; j < numBlocks-1; j++) {
	    fat[firstFreeBlock] = ++firstFreeBlock;
	}
	fat[firstFreeBlock++] = FAT_ENTRY_EOF;

	lseek(fd, directory[i].firstBlock * 512, SEEK_SET);

	/* copy the file to the disk */
	fd2 = open(directory[i].fileName, O_RDONLY, 0);
	assert(fd2);
        totalFileBlocks += numBlocks;
	if (totalFileBlocks > usableBlocks) {
     	    printf("image file is too small to hold all blocks from requested files\n");
	    exit(-1);
	}
	for (j=0; j < numBlocks; j++) {
	    int ret2;
	    char buffer[512];

	    ret = read(fd2, buffer, 512);
	    assert(ret >= 0);
	    ret2 = write(fd, buffer, ret);
	    assert(ret2 == ret);
	}
	close(fd2);
    }

    lseek(fd, 512, SEEK_SET);
    write(fd, fat, sizeof(int) * blocks);

    lseek(fd, bSector.rootDirectoryOffset * 512, SEEK_SET);
    printf("putting the directory at sector %d\n", bSector.rootDirectoryOffset);
    write(fd, directory, sizeof(directoryEntry) * fileCount);

    close(fd);

    exit(0);
}
