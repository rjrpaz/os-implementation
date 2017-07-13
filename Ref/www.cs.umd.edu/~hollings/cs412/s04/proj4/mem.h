// Memory handling and allocation
// Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
// $Revision: 3.0 $

// This is free software.  You are permitted to use,
// redistribute, and modify it as specified in the file "COPYING".

#ifndef MEM_H
#define MEM_H

#include "ktypes.h"
#include "paging.h"

struct Boot_Info;

// Page flags
#define PAGE_KERN      0x0001	// page used by kernel code or data
#define PAGE_HW        0x0002	// page used by hardware (e.g., ISA hole)
#define PAGE_ALLOCATED 0x0004	// page is allocated
#define PAGE_UNUSED    0x0008	// page is unused
#define PAGE_HEAP      0x0010	// page is in kernel heap
#define PAGE_PAGEABLE  0x0020	// page can be paged out

// x86 has 4096 byte pages
#define PAGE_POWER 12
#define PAGE_SIZE (1<<PAGE_POWER)
#define PAGE_MASK (~(0xffffffff << PAGE_POWER))

// PC memory map
#define ISA_HOLE_START 0x0A0000
#define ISA_HOLE_END   0x100000

// We reserve the two pages just after the ISA hole for the initial
// kernel thread's context object and stack.
#define HIGHMEM_START (ISA_HOLE_END + 8192)

// Each page of physical memory has one of these structures
// associated with it, to do allocation and bookkeeping.
struct Page {
    unsigned flags;
    unsigned long next;
    int clock;
    int addr;
    pageTableEntry *entry;		// who maps the page
};

void Init_Mem( struct Boot_Info* bootInfo );
void* Alloc_Page( void );
void* Alloc_Page_Atomic( void );
void* Alloc_Pageable_Page( void );

void Free_Page( void* pageAddr );
void Free_Page_Atomic( void* pageAddr );

// Determine if given address is a multiple of the page size.
static __inline__ Boolean Is_Page_Multiple( unsigned long addr )
{
    return addr == (addr & ~(PAGE_MASK));
}

// Round given address up to a multiple of the page size
static __inline__ unsigned long Round_Up_To_Page( unsigned long addr )
{
    if ( (addr & PAGE_MASK) != 0 ) {
	addr &= ~(PAGE_MASK);
	addr += PAGE_SIZE;
    }
    return addr;
}

// Get the index of the page in memory.
static __inline__ int Page_Index( unsigned long addr )
{
    return (int) (addr >> PAGE_POWER);
}

// Pointer to the table of Page structs.
extern struct Page* s_pageList;

// Get the Page struct associated with given address.
#define Get_Page(addr) (&s_pageList[Page_Index(addr)])

#endif // MEM_H
