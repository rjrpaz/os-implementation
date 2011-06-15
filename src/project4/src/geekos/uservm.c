/*
 * Paging-based user mode implementation
 * Copyright (c) 2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.50 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/int.h>
#include <geekos/mem.h>
#include <geekos/paging.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/argblock.h>
#include <geekos/kthread.h>
#include <geekos/range.h>
#include <geekos/vfs.h>
#include <geekos/user.h>
#include <geekos/errno.h>
#include <geekos/paging.h>
#include <geekos/gdt.h>

#define DEFAULT_USER_STACK_SIZE 8192

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */

int debugVM = 1;
#define DebugVM(args...) if (debugVM) Print(args)

// TODO: Add private functions
/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */


/*
 * Destroy a User_Context object, including all memory
 * and other resources allocated within it.
 */
void Destroy_User_Context(struct User_Context* context)
{
    /*
     * Hints:
     * - Free all pages, page tables, and page directory for
     *   the process (interrupts must be disabled while you do this,
     *   otherwise those pages could be stolen by other processes)
     * - Free semaphores, files, and other resources used
     *   by the process
     */
    TODO("Destroy User_Context data structure after process exits");
}

/*
 * Load a user executable into memory by creating a User_Context
 * data structure.
 * Params:
 * exeFileData - a buffer containing the executable to load
 * exeFileLength - number of bytes in exeFileData
 * exeFormat - parsed ELF segment information describing how to
 *   load the executable's text and data segments, and the
 *   code entry point address
 * command - string containing the complete command to be executed:
 *   this should be used to create the argument block for the
 *   process
 * pUserContext - reference to the pointer where the User_Context
 *   should be stored
 *
 * Returns:
 *   0 if successful, or an error code (< 0) if unsuccessful
 */
int Load_User_Program(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat, const char *command,
    struct User_Context **pUserContext)
{
    /*
     * Hints:
     * - This will be similar to the same function in userseg.c
     * - Determine space requirements for code, data, argument block,
     *   and stack
     * - Allocate pages for above, map them into user address
     *   space (allocating page directory and page tables as needed)
     * - Fill in initial stack pointer, argument block address,
     *   and code entry point fields in User_Context
     */
//    TODO("Load user program into address space");

    KASSERT(exeFileData != NULL);
    KASSERT(command != NULL);
    KASSERT(exeFormat != NULL);
                
    /* Por Victor Rosales */
    int i = 0;
    int ret = 0;
    ulong_t maxva = 0;
    ulong_t argBlockSize = 0;
    ulong_t stackAddr = 0;
    unsigned numArgs = 0;
    unsigned long virtSize;
    struct User_Context *userContext = 0;

    int j = 0;
    uint_t pageNumber = 0, numberOfPages = 0;
    ulong_t vaddr = 0;

    /* Find maximum virtual address */
    for (i = 0; i < exeFormat->numSegments; ++i) {
        struct Exe_Segment *segment = &exeFormat->segmentList[i];
        ulong_t topva = segment->startAddress + segment->sizeInMemory;

        if (topva > maxva)
            maxva = topva;
    }

    virtSize = Round_Up_To_Page(maxva);

    /*
     * memoria para almacenar temporalmente los segmentos del programa,
     * antes de copiarlos a las p√°ginas.
     */
    char *mem = (char *) Malloc(virtSize);
    if (mem == NULL)
        goto error;

    /* Reset memory with zeros */
    memset(mem, '\0', virtSize);
 
    /* Copia segmentos en la memoria temporal */
    for (i = 0; i < exeFormat->numSegments; i++) {
        struct Exe_Segment *segment = &exeFormat->segmentList[i];

        memcpy(mem + segment->startAddress,
            exeFileData + segment->offsetInFile,
            segment->lengthInFile);
    }

    /* Pido memoria para el User_Context */
    userContext = Malloc(sizeof(struct User_Context));
    if (userContext ==  NULL)
        goto error;

    /* Copia Page Directory de kernel al proceso */
    userContext->pageDir = Alloc_Page();
    memcpy(userContext->pageDir, pageDirectory, PAGE_SIZE);

    /* Crea las Page Tables del proceso */
    pde_t *ppde = userContext->pageDir + (NUM_PAGE_DIR_ENTRIES / 2);
    DebugVM("Direccion de la pagedir: @%lx\n", (unsigned long) ppde);
    numberOfPages = virtSize / PAGE_SIZE;
    DebugVM("Cant. de paginas necesarias para paginar: %d\n", numberOfPages);
    vaddr = 0x8000000;
    for (i=0; i<NUM_PAGE_DIR_ENTRIES/2; i++) {
//        DebugVM("Direccion de la entrada %i de la pagedir: @%lx\n", i, (unsigned long) ppde);
        if (pageNumber >= numberOfPages) {
            ppde->present = 0;
        } else {
            ppde->present = 1;
            ppde->flags = VM_READ | VM_WRITE | VM_USER;

            /* Reserva p√°gina para la tabla de p√°ginas */
            pte_t *ppte = (pte_t *)Alloc_Page();
            DebugVM("Direccion de la primer entrada en la tabla de paginas @%lx. PageNumber %d\n", (unsigned long) ppte, pageNumber);
            memset(ppte, 0x0, PAGE_SIZE);
            ppde->pageTableBaseAddr = PAGE_ALLIGNED_ADDR(ppte);
            for (j=0; j<NUM_PAGE_TABLE_ENTRIES; j++) {
                /* Evita posici√≥n 0 para NULL pointer */
                if ((i==0) && (j==0))
                    ppte->present = 0;
                else {
                    (pageNumber<numberOfPages) ? (ppte->present = 1) : (ppte->present = 0);
                    if (ppte->present == 1) {
                        ppte->flags = VM_READ | VM_WRITE | VM_USER;

                        void *page = Alloc_Pageable_Page(ppte, vaddr);
                        DebugVM("Direccion de la pagina nro. %d asignada al programa @%lx\n", pageNumber, (unsigned long) page);
                        ppte->pageBaseAddr = PAGE_ALLIGNED_ADDR(page);

                        /* Copia el programa desde la memoria temporal a la p√°gina */
                        memcpy(page, mem+(pageNumber*PAGE_SIZE), PAGE_SIZE);

                        pageNumber++;
                    }

                    ppte++;
                }
                vaddr += PAGE_SIZE;
            }
        }
        ppde++;
    }
    Free(mem);

    /* Crea entrada de tabla de p√°ginas para stack y argumentos */
    ppde = userContext->pageDir + NUM_PAGE_DIR_ENTRIES - 1; // Ultima entrada del Page Directory
    DebugVM("Ultima direccion de la tabla de paginas @%lx\n", (unsigned long) ppde);
    /* No est√° presente. La habilito */
    if (ppde->present == 0) {
        DebugVM("Ultima direccion no est· presente. La habilito\n");
        ppde->present = 1;
        ppde->flags = VM_READ | VM_WRITE | VM_USER;

        /* Reserva p√°gina para la tabla de p√°ginas */
        pte_t *ppte = (pte_t *)Alloc_Page();
        memset(ppte, 0x0, PAGE_SIZE);
        ppde->pageTableBaseAddr = PAGE_ALLIGNED_ADDR(ppte);
    }
    /* Obtengo direccion de la tabla de paginas de la √∫ltima entrada del page directory */
    ulong_t ultima = ppde->pageTableBaseAddr << 12;
    DebugVM("DirecciÛn de la tabla de p·ginas de la ˙ltima entrada del page directory @%lx @%lx\n", ultima, (unsigned long) ppde->pageTableBaseAddr);

    /* Obtengo pen˙ltima entrada del page table - STACK */
    pte_t *ppte = (pte_t *) (ultima) + NUM_PAGE_TABLE_ENTRIES - 2;
    DebugVM("DirecciÛn de penultima entrada en la tabla de p·ginas asociada a la ˙ltima entrada del page directory @%lx\n", (unsigned long) ppte);
    /* El programa es muy grande */
    KASSERT(ppte->present != 1);
    
    ppte->present = 1;
    ppte->flags = VM_READ | VM_WRITE | VM_USER;
    void *page = Alloc_Pageable_Page(ppte, 0xFFFFE000);
    DebugVM("Direccion de la pagina asignada al stack @%lx\n", (unsigned long) page);
    ppte->pageBaseAddr = PAGE_ALLIGNED_ADDR(page);

    /* Obtengo ˙ltima entrada del page table - ARGS */
    ppte++;
    DebugVM("DirecciÛn de ultima entrada en la tabla de p·ginas asociada a la ˙ltima entrada del page directory @%lx\n", (unsigned long) ppte);
    /* El programa es muy grande */
    KASSERT(ppte->present != 1);
    Get_Argument_Block_Size(command, &numArgs, &argBlockSize);
    KASSERT(argBlockSize <= PAGE_SIZE);
 
    ppte->present = 1;
    ppte->flags = VM_READ | VM_WRITE | VM_USER;
    page = Alloc_Pageable_Page(ppte, 0xFFFFF000);
    DebugVM("Direccion de la pagina asignada a los args @%lx\n", (unsigned long) page);
    ppte->pageBaseAddr = PAGE_ALLIGNED_ADDR(page);

    Format_Argument_Block(page, numArgs, stackAddr, command);

    /* Create the user context */
    userContext->entryAddr          = exeFormat->entryAddr;
    userContext->argBlockAddr       = 0xFFFFF000;
    userContext->stackPointerAddr   = 0xFFFFE000;

    /* Completo la informaciÛn para segmentaciÛn, con los valores virtuales */

    /* Guardo el segment descriptor de la ldt en la gdt */
    struct Segment_Descriptor *ldt_desc = Allocate_Segment_Descriptor(); //LDT-Descriptor for the process
    if (ldt_desc == NULL)
        goto error;

    Init_LDT_Descriptor(ldt_desc, userContext->ldt, NUM_USER_LDT_ENTRIES);
    /* Creo un selector para el descriptor de ldt */
    ushort_t ldt_selector = Selector(KERNEL_PRIVILEGE, true, Get_Descriptor_Index(ldt_desc)); 

    /* Inicializo los segmentos */
    Init_Code_Segment_Descriptor(&(userContext->ldt[0]), (ulong_t)mem, 0x8000000/PAGE_SIZE, USER_PRIVILEGE);
    Init_Data_Segment_Descriptor(&(userContext->ldt[1]), (ulong_t)mem, 0x8000000/PAGE_SIZE, USER_PRIVILEGE);

    /* Creo los selectores */
    ushort_t cs_selector = Selector(USER_PRIVILEGE, false, 0);
    ushort_t ds_selector = Selector(USER_PRIVILEGE, false, 1);

    /* Asigno todo al userContext */
    userContext->ldtDescriptor = ldt_desc;
    userContext->ldtSelector = ldt_selector;
    userContext->csSelector = cs_selector;
    userContext->dsSelector = ds_selector;
    userContext->size = 0x80000000;
    userContext->memory = NULL;
    userContext->refCount = 0;

    if (userContext == NULL)
        ret = ENOMEM;

    *pUserContext = userContext;
    goto success;

error:
//    DebugVM("ERROR\n");
    if (mem != NULL)
        Free(mem);
    if (userContext != NULL)
        Free(userContext);
    return -1;

success:
//    DebugVM("SUCESS\n");
    return ret;
}

/*
 * Copy data from user buffer into kernel buffer.
 * Returns true if successful, false otherwise.
 */
bool Copy_From_User(void* destInKernel, ulong_t srcInUser, ulong_t numBytes)
{
    /*
     * Hints:
     * - Make sure that user page is part of a valid region
     *   of memory
     * - Remember that you need to add 0x80000000 to user addresses
     *   to convert them to kernel addresses, because of how the
     *   user code and data segments are defined
     * - User pages may need to be paged in from disk before being accessed.
     * - Before you touch (read or write) any data in a user
     *   page, **disable the PAGE_PAGEABLE bit**.
     *
     * Be very careful with race conditions in reading a page from disk.
     * Kernel code must always assume that if the struct Page for
     * a page of memory has the PAGE_PAGEABLE bit set,
     * IT CAN BE STOLEN AT ANY TIME.  The only exception is if
     * interrupts are disabled; because no other process can run,
     * the page is guaranteed not to be stolen.
     */
    TODO("Copy user data to kernel buffer");
}

/*
 * Copy data from kernel buffer into user buffer.
 * Returns true if successful, false otherwise.
 */
bool Copy_To_User(ulong_t destInUser, void* srcInKernel, ulong_t numBytes)
{
    /*
     * Hints:
     * - Same as for Copy_From_User()
     * - Also, make sure the memory is mapped into the user
     *   address space with write permission enabled
     */
    TODO("Copy kernel data to user buffer");
}

/*
 * Switch to user address space.
 */
void Switch_To_Address_Space(struct User_Context *userContext)
{
    /*
     * - If you are still using an LDT to define your user code and data
     *   segments, switch to the process's LDT
     * - 
     */
//    TODO("Switch_To_Address_Space() using paging");
    KASSERT(userContext != NULL);
    KASSERT(userContext->ldtSelector != 0);
    /*
     * Hint: you will need to use the lldt assembly language instruction
     * to load the process's LDT by specifying its LDT selector.
     */
    /* Load the task register */
    __asm__ __volatile__ (
        "lldt %0"
        :
        : "a" (userContext->ldtSelector)
    );

//    Set_PDBR(userContext->pageDir);
return;
}

