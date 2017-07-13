#include "user.h"
#include "pfat.h"
#include "screen.h"
#include "malloc.h"
#include "string.h"
#include "kthread.h"

int elfDebug;

typedef struct {
	unsigned  char	ident[16];
	unsigned  short	type;
	unsigned  short	machine;
	unsigned  int	version;
	unsigned  int	entry;
	unsigned  int	phoff;
	unsigned  int	sphoff;
	unsigned  int	flags;
	unsigned  short	ehsize;
	unsigned  short	phentsize;
	unsigned  short	phnum;
	unsigned  short	shentsize;
	unsigned  short	shnum;
	unsigned  short	shstrndx;
} elfHeader;

typedef struct {
   	unsigned  int   type;
   	unsigned  int   offset;
   	unsigned  int   vaddr;
   	unsigned  int   paddr;
   	unsigned  int   fileSize;
   	unsigned  int   memSize;
   	unsigned  int   flags;
   	unsigned  int   alignment;
} programHeader;

struct User_Program *loadElfProgram(char *programName)
{
    int i;
    int fd;
    int ret;
    int size;
    int length;
    int maxSize;
    int zeroSize;
    char *program;
    fileStat stat;
    elfHeader *hdr;
    programHeader *phdr;
    struct User_Program *uProgram;

    if (elfDebug) Print("about to read %s\n", programName);

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

    hdr = (elfHeader *) program;
    phdr = (programHeader *) (program + hdr->phoff);

    maxSize = 0;
    for (i=0; i < hdr->phnum; i++) {
	if (elfDebug) {
	    Print ("Segment %d: runs from %d to %d, offset %d\n", i, phdr[i].vaddr, 
	        phdr[i].vaddr + phdr[i].memSize, phdr[i].offset);
	}
	size = phdr[i].vaddr + phdr[i].memSize;
	if (size > maxSize) maxSize = size;
    }

    zeroSize = phdr[1].memSize - phdr[1].fileSize;
    uProgram = (struct User_Program *) Malloc_Atomic(sizeof(struct User_Program));
    uProgram->name = strdup(programName);
    uProgram->size = maxSize;
    uProgram->entryAddr = hdr->entry;

    if (elfDebug) Print("copied %d bytes into region %d bytes long\n", length, length+zeroSize);
    uProgram->executable = Malloc_Atomic(length+zeroSize);
    memset((char *) uProgram->executable, '\0', length+zeroSize);
    memcpy((char *) uProgram->executable, program, length);
    Free_Atomic(program);

    /* XXX - check these constants */
    uProgram->program = uProgram->executable + phdr[0].offset; 
    uProgram->programSize = phdr[0].memSize; 

    uProgram->data = uProgram->executable + phdr[1].offset; 
    uProgram->dataOffset = phdr[1].vaddr;
    uProgram->dataSize = phdr[1].memSize; 

    if (elfDebug) {
	Print ("user program loaded\n");
	Print (" name = %s\n", uProgram->name);
	Print (" size = %d\n", uProgram->size);
	Print (" entry = %d\n", uProgram->entryAddr);
	Print (" program = %x\n", uProgram->program);
	Print (" programSize = %x\n", uProgram->programSize);
	Print (" data = %x\n", uProgram->data);
	Print (" dataSize = %x\n", uProgram->dataSize);
    }

    return uProgram;
}
