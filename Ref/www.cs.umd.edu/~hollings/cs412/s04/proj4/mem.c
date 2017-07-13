// Memory handling and allocation
// Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
// $Revision: 3.0.2.1 $

// This is free software.  You are permitted to use,
// redistribute, and modify it as specified in the file "COPYING".

#include "defs.h"
#include "kassert.h"
#include "bootinfo.h"
#include "gdt.h"
#include "screen.h"
#include "int.h"
#include "malloc.h"
#include "mem.h"
#include "string.h"
#include "paging.h"

// ----------------------------------------------------------------------
// Private data and functions
// ----------------------------------------------------------------------

// We use this constant to indicate an "invalid" page;
// for example, it terminates the freelist.
#define INVALID_PAGE (~0UL)

// We'll leave the page at address zero alone, and start allocation
// at the next page.  This will allow this us to treat a null pointer
// as "invalid".  Eventually we will have paged virtual memory,
// in which case we can *use* physical page 0.  (Of course, we won't
// *map* anything at address 0.)
#define FIRST_PAGE (1 * PAGE_SIZE)

// Address of first free page.
static unsigned long s_firstFreePage;

static int unsigned numPages;

struct Page* s_pageList;

// Initialize given page as being "already allocated",
// meaning it can be returned to the freelist with
// the Free_Page() function.
static void Add_Allocated_Page( unsigned long addr )
{
    struct Page* page = Get_Page( addr );
    page->flags = PAGE_ALLOCATED;
    page->next = INVALID_PAGE;
    page->addr = addr;
}

// Initialize given page as one that should be incorporated into
// the kernel free page list.  Returns TRUE if the page was successfully
// added, or FALSE otherwise (if it couldn't be used).
static Boolean Add_Avail_Page( unsigned long addr, unsigned long prev )
{
    struct Page* page = Get_Page( addr );

    KASSERT( addr != KERN_THREAD_OBJ && addr != KERN_STACK );

    page->flags = 0;
    page->next = INVALID_PAGE; // ensures that list will be terminated
    page->addr = addr;

    // Link the previous available page initialized to this one
    if ( prev != INVALID_PAGE ) {
	struct Page* prevPage = Get_Page( prev );
	prevPage->next = addr;
    }

    return TRUE;
}

// Initialize given page as reserved for kernel.
static void Add_Kernel_Page( unsigned long addr )
{
    struct Page* page = Get_Page( addr );
    page->flags = PAGE_KERN;
    page->next = INVALID_PAGE;
    page->addr = addr;
}

// Initialize given page as reserved for hardware (or BIOS, etc.)
static void Add_HW_Page( unsigned long addr )
{
    struct Page* page = Get_Page( addr );
    page->flags = PAGE_HW;
    page->next = INVALID_PAGE;
    page->addr = addr;
}

// Initialize a given page as belonging to the kernel heap.
static void Add_Heap_Page( unsigned long addr )
{
    struct Page* page = Get_Page( addr );
    page->flags = PAGE_HEAP;
    page->next = INVALID_PAGE;
    page->addr = addr;
}

// ----------------------------------------------------------------------
// Public functions
// ----------------------------------------------------------------------
//

// The linker defines this symbol to indicate the end of
// the executable image.
extern char end;

// The Main() function happens to be at the very beginning
// of the kernel image.
void Main( struct Boot_Info* );

unsigned freePageCount = 0;

// Initialize memory management data structures.
// Enables the use of Alloc_Page() and Free_Page() functions.
void Init_Mem( struct Boot_Info* bootInfo )
{
    // Memory looks like this:
    // 0 - start: available (might want to preserve BIOS data area)
    // start - end: kernel
    // end - ISA_HOLE_START: available
    // ISA_HOLE_START - ISA_HOLE_END: used by hardware (and ROM BIOS?)
    // ISA_HOLE_END - HIGHMEM_START: used by initial kernel thread
    // HIGHMEM_START - end of memory: available

    // We'll put the list of Page objects right after the end
    // of the kernel, and mark it as "kernel".  This will bootstrap
    // us sufficiently that we can start allocating pages and
    // keeping track of them.

    unsigned endOfMem;
    unsigned endOfHeap;
    unsigned sizeOfHeap;
    unsigned numPageListBytes;
    unsigned long kernStart, kernEnd;
    unsigned long addr, prev;

    if (bootInfo->memSizeKB == 0) {
        Print("unable to detect memory size, assuming 64 MB\n");
        bootInfo->memSizeKB = 64 * 1024;
    }

    numPages = bootInfo->memSizeKB >> 2;
    endOfMem = numPages * PAGE_SIZE;
    numPageListBytes = sizeof(struct Page) * numPages;

#define ADD_AVAIL_PAGE(addr,prev)		\
do {						\
    if ( Add_Avail_Page( addr, prev ) ) {	\
	prev = addr;				\
	++freePageCount;				\
    }						\
} while (0)

    // Before we do anything, switch from setup.asm's temporary GDT
    // to the kernel's permanent GDT.
    Init_GDT();

    kernStart = KERNEL_START_ADDR;
    KASSERT( (kernStart & PAGE_MASK) == 0 );

    s_pageList = (struct Page*) Round_Up_To_Page( (unsigned long) &end );
    kernEnd = Round_Up_To_Page( ((unsigned long) s_pageList) + numPageListBytes );
    Print("spage list runs from %x to %x\n", s_pageList, kernEnd);

    // Page 0 is not used.
    s_pageList[0].flags = PAGE_UNUSED;
    s_pageList[0].next = INVALID_PAGE;

    // The initial kernel thread context and stack pages are
    // "pre-allocated".
    Add_Allocated_Page( KERN_THREAD_OBJ );
    Add_Allocated_Page( KERN_STACK );

    // Add pages FIRST_PAGE - start of kernel
    prev = INVALID_PAGE;
    for ( addr = FIRST_PAGE; addr < kernStart; addr += PAGE_SIZE )
	ADD_AVAIL_PAGE( addr, prev );

    // Add pages start of kernel - end of kernel
    for ( addr = kernStart; addr < kernEnd; addr += PAGE_SIZE )
	Add_Kernel_Page( addr );

    // Add pages end of kernel - start of ISA hole
    for ( addr = kernEnd; addr < ISA_HOLE_START; addr += PAGE_SIZE )
	ADD_AVAIL_PAGE( addr, prev );

    // Add start of ISA hole - end of ISA hole
    for ( addr = ISA_HOLE_START; addr < ISA_HOLE_END; addr += PAGE_SIZE )
	Add_HW_Page( addr );

    // Decide how big to make the heap
    //    Goal is to make have equal sized heap and free list
    sizeOfHeap = Round_Up_To_Page((endOfMem - HIGHMEM_START - freePageCount*PAGE_SIZE)/2);
    endOfHeap = HIGHMEM_START + sizeOfHeap;

    // Create the kernel heap from the beginning of available high
    // memory to endOfHeap
    for ( addr = HIGHMEM_START; addr < endOfHeap; addr += PAGE_SIZE )
	Add_Heap_Page( addr );
    Init_Heap( HIGHMEM_START, endOfHeap - HIGHMEM_START );

    for ( addr = endOfHeap; addr < endOfMem; addr += PAGE_SIZE )
	ADD_AVAIL_PAGE( addr, prev );

    // Initialize freelist
    s_firstFreePage = FIRST_PAGE;

    Print( "%d pages of memory available...\n", freePageCount );
    Print( "%d pages of in kernel heap...\n", (endOfHeap - HIGHMEM_START)/PAGE_SIZE );

#undef ADD_AVAIL_PAGE
}

// Allocate a page of physical memory.
// Must be called with interrupts disabled!
void* Alloc_Page( void )
{
    unsigned long addr = s_firstFreePage;
    struct Page* page;

    KASSERT( !Interrupts_Enabled() );

    if ( addr == INVALID_PAGE )
	return 0;

    page = Get_Page( addr );
    page->flags |= PAGE_ALLOCATED;
    s_firstFreePage = page->next;

    freePageCount--;

    return (void*) addr;
}

// Allocate a page of physical memory; assumes that
// interrupts are currently enabled.
void* Alloc_Page_Atomic( void )
{
    void* addr;

    Disable_Interrupts();
    addr = Alloc_Page();
    Enable_Interrupts();

    return addr;
}

struct Page *FindPageToPageOut()
{
    // search for the page to page out based on algorithm in description
    return NULL;
}

// Allocate a page of physical memory
void* Alloc_Pageable_Page( void )
{
            KASSERT(0);
}

// Free a page of physical memory.
// Must be called with interrupts disabled!
void Free_Page( void* pageAddr )
{
    unsigned long addr = (unsigned long) pageAddr;
    struct Page* page;

    KASSERT( !Interrupts_Enabled() );

    KASSERT( Is_Page_Multiple( addr ) );

    page = Get_Page( addr );
    if ((page->flags & PAGE_ALLOCATED) == 0) {
        Print("Error freeing page %x\n", pageAddr);
        KASSERT( (page->flags & PAGE_ALLOCATED) != 0 );
    }

    page->flags &= ~(PAGE_ALLOCATED);
    page->next = s_firstFreePage;
    s_firstFreePage = addr;
    freePageCount++;
}

// Atomically free a page.
// Assumes that interrupts are enabled.
void Free_Page_Atomic( void* pageAddr )
{
    Disable_Interrupts();
    Free_Page( pageAddr );
    Enable_Interrupts();
}
