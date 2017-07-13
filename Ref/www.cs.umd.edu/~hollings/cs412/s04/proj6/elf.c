#include "pfat.h"
#include "screen.h"
#include "malloc.h"
#include "string.h"
#include "kthread.h"
#include "elf.h"
#include "lprog.h"
#include "vfs.h"

#include "mem.h"

int elfDebug = 0;

struct Loadable_Program *Read_Elf_Executable(char *programName)
{
    char *program;
    int length;
    int ret;
    struct Loadable_Program *lp;

    int i;
    int size;
    int maxSize;
    int zeroSize;
    elfHeader *hdr;
    programHeader *phdr;

    int fd;
    fileStat stat;    

    if (elfDebug) 
      Print("reading %s...\n", programName);

    if (!g_currentThread->userContext) {
        // special case for first program which must be on pfat filesystem
	ret = ReadFileAndAllocate(programName, &program, &length);
	if (ret) {
	    return 0;
	}
    } else {
	// First open the program
	fd = Open(programName, O_READ);
	if (fd < 0) return 0;
	ret = Stat(fd, &stat);
	if (ret < 0) return 0;

	length = stat.size;
	program = Malloc_Atomic(length);
	ret = Read(fd, program, length);
	if (ret != length) {
	    Free_Atomic(program);
	    return 0;
	}
	Close(fd);
    }

    if (elfDebug) 
      Print("reading %s done\n", programName);

    lp = (struct Loadable_Program *) Malloc_Atomic(sizeof(struct  Loadable_Program));

    /* here you should put your code from project 1
    ........
    ........
     */

    hdr = (elfHeader *) program;
    phdr = (programHeader *) (program + hdr->phoff);

    maxSize = 0;
    for (i=0; i < hdr->phnum; i++) {
	if (elfDebug) {
	    Print ("Segment %d: runs from %d to %d, offset %d\n", i, phdr[i].vaddr, 
	        phdr[i].vaddr + phdr[i].memSize, phdr[i].offset);
	    Print ("The pheader type is %d\n", phdr[i].type);
	}
	size = phdr[i].vaddr + phdr[i].memSize;
	if (size > maxSize) maxSize = size;
    }

    zeroSize = phdr[1].memSize - phdr[1].fileSize;
    lp->imageSize = Round_Up_To_Page(maxSize + STACK_SIZE);
    lp->entryPoint = hdr->entry;

    lp->image = Malloc_Atomic(lp->imageSize);
    memset((char *) lp->image, '\0', sizeof(lp->image));

    for (i = 0; i < hdr->phnum; i++)
    {
       memcpy((char *)(lp->image + phdr[i].vaddr), (char *)(program + phdr[i].offset), phdr[i].memSize);
    }

    Free_Atomic(program);

    if (elfDebug) {
	Print ("user program loaded\n");
	Print (" size = %d\n", lp->imageSize);
	Print (" entry = %d\n", lp->entryPoint);
    }

    return lp;
}
