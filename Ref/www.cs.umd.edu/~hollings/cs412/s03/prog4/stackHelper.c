
void ProtectKernelPage(unsigned int addr)
{
    int pageDirectoryIndex;
    int pageTableIndex;
    pageTableEntry *tab;
    pageDirectoryEntry *dir, *pdbr;

    pageDirectoryIndex = PAGE_DIRECTORY_INDEX(addr);
    pageTableIndex = PAGE_TABLE_INDEX(addr);

     pdbr = NULL;
     __asm__ __volatile__ (
         "movl %%cr3, %0"
         : "=a" (pdbr)
         :
     );
    dir = &pdbr[pageDirectoryIndex];

    KASSERT(dir->present);
    tab = (pageTableEntry *) (dir->pageTableBaseAddr << 12);
    KASSERT(tab);
    tab[pageTableIndex].present = 0;
    flushTLB();
}


int UnProtectKernelPage(unsigned int addr)
{
    int ret;
    int pageDirectoryIndex;
    int pageTableIndex;
    pageTableEntry *tab;
    pageDirectoryEntry *dir, *pdbr;

    KASSERT(addr < 0x80000000);

    pageDirectoryIndex = PAGE_DIRECTORY_INDEX(addr);
    pageTableIndex = PAGE_TABLE_INDEX(addr);

     pdbr = NULL;
     __asm__ __volatile__ (
         "movl %%cr3, %0"
         : "=a" (pdbr)
         :
     );

    KASSERT(pdbr);
    dir = &pdbr[pageDirectoryIndex];

    KASSERT(dir->present);
    tab = (pageTableEntry *) (dir->pageTableBaseAddr << 12);
    ret = tab->present;
    tab[pageTableIndex].present = 1;
    flushTLB();

    if (ret == 0)
        Print("unprotected page at addr = %x\n", addr);

    return ret;
}

#define PAGE_LIMIT      20


//
// allocate a stack page and the page before it (and unmap the
//   page before it.
//
void* Alloc_Stack_Page( void )
{
    int i;
    struct Page* lag;
    struct Page* page;
    unsigned long addr;
    unsigned long temp;
    int uselessPages[PAGE_LIMIT];
    unsigned long targetAddr;


    KASSERT( !Interrupts_Enabled() );

    for (i=0; i < PAGE_LIMIT; i++) {
	targetAddr = s_firstFreePage;

	page = Get_Page( targetAddr );
	s_firstFreePage = page->next;
	KASSERT(!(page->flags & PAGE_ALLOCATED));
	page->flags |= PAGE_ALLOCATED;

	addr = s_firstFreePage;
	lag = NULL;
	while (addr != INVALID_PAGE) {
	    page = Get_Page( addr );
	    if ((addr == (targetAddr + 4096)) ||
		(addr == (targetAddr - 4096))) {
	       // found a page before or after the first free one

	       // mark it as used
	       if (lag) 
		   lag->next = page->next;
	       else
		   s_firstFreePage = page->next;
	       KASSERT(!(page->flags & PAGE_ALLOCATED));
	       page->flags |= PAGE_ALLOCATED;
	       break;
	    }
	    addr = page->next;
	    lag = page;
	}
	if (addr != INVALID_PAGE) break;
	uselessPages[i] = targetAddr;
    }

    if (addr == INVALID_PAGE) {
	Print("unable to protect page after kernel page\n");
    } else {
	Print("found page %x on the %d try\n", targetAddr, i);
	if (addr > targetAddr) {
	     Print("flipping %x and %x\n", addr, targetAddr);
	     temp = targetAddr;
	     targetAddr = addr;
	     addr = temp;
	}
	// get kernel page table for this address
	ProtectKernelPage(addr);
    }

    // free extra pages
#ifdef notdef
    while (i > 0) {
        Free_Page(uselessPages[--i]); 
    }
#endif

    return (void *) targetAddr;
}

void Free_Stack_Page( void *pageAddr)
{
    unsigned int addr;

    Free_Page(pageAddr);

    addr = (unsigned int) pageAddr - 4096;
    if (!UnProtectKernelPage(addr)) {
        // it was un-mapped so put it on the free list
        Free_Page((void *) addr);
    }
}
