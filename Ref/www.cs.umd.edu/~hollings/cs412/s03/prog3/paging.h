
#ifndef PAGING_H
#define PAGING_H

#define NUM_PAGE_TABLE_ENTRIES	1024
#define NUM_PAGE_DIR_ENTRIES	1024

#define PAGE_DIRECTORY_INDEX(x)	(((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x)	(((x) >> 12) & 0x3ff)

typedef struct {
    unsigned int present:1;
    unsigned int writeable:1;
    unsigned int userMode:1;
    unsigned int writeThrough:1;
    unsigned int cacheDisabled:1;
    unsigned int accesed:1;
    unsigned int reserved:1;
    unsigned int largePages:1;
    unsigned int globalPage:1;
    unsigned int kernelInfo:3;
    unsigned int pageTableBaseAddr:20;
} pageDirectoryEntry;

typedef struct {
    unsigned int present:1;
    unsigned int writeable:1;
    unsigned int userMode:1;
    unsigned int writeThrough:1;
    unsigned int cacheDisabled:1;
    unsigned int accesed:1;
    unsigned int dirty:1;
    unsigned int pteAttribute:1;
    unsigned int globalPage:1;
    unsigned int kernelInfo:3;
    unsigned int pageBaseAddr:20;
} pageTableEntry;

// contents of error code pushed by hardware.
typedef struct {
    unsigned int protectionViolation:1;
    unsigned int writeFault:1;
    unsigned int userModeFault:1;
    unsigned int reservedBitFault:1;
    unsigned int reserved:28;
} pageFaultErrorCode;

// bits used in the kernelInfo field of the PTE's
#define KINFO_PAGE_ON_DISK	0x7

extern void flushTLB();
extern void SetPDBR(pageDirectoryEntry *);
extern void Enable_Paging(pageDirectoryEntry *);

#endif
