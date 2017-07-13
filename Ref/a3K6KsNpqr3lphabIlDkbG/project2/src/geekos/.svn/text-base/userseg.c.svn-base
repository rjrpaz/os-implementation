/*
 * Segmentation-based user mode implementation
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.23 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/ktypes.h>
#include <geekos/kassert.h>
#include <geekos/defs.h>
#include <geekos/mem.h>
#include <geekos/string.h>
#include <geekos/malloc.h>
#include <geekos/int.h>
#include <geekos/gdt.h>
#include <geekos/segment.h>
#include <geekos/tss.h>
#include <geekos/kthread.h>
#include <geekos/argblock.h>
#include <geekos/user.h>

#include <geekos/errno.h>

/* ----------------------------------------------------------------------
 * Variables
 * ---------------------------------------------------------------------- */

#define DEFAULT_USER_STACK_SIZE 8192

#define USERSEGDEBUG 1


/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */


/*
 * Create a new user context of given size
 */

// TODO: Implement
static struct User_Context* Create_User_Context(ulong_t size)
{
//	TODO("Implement Create_User_Context Methode");

	struct User_Context* userContext = 0;
	
	Disable_Interrupts();
	//Malloc uses Function bget() which is defined in bget.h + *.c
	//Malloc returns a void* pointer to the address of the alocated memory if successfully and otherwise NULL
	userContext = (struct User_Context*) Malloc(sizeof(*userContext));		
	Enable_Interrupts();
	
	if(!userContext)
		return 0;
		
		
/*alocating mamory */
	userContext->memory=Malloc(size);
		
	if(!userContext->memory)
	{
		Free(userContext);
		return 0;
	}
	
	/* initialize the alocated memory with '\0', so everything is deleted*/
	memset((char *) userContext->memory, '\0', size);
	userContext->size=size;
/*end memory*/



/* Create an LDT descriptor by calling the routine Allocate_Segment_Descriptor(), which allocates an descriptor from the GDT. */
	userContext->ldtDescriptor = Allocate_Segment_Descriptor();
	if(!userContext->ldtDescriptor)
	{
			Free(userContext->memory);
			Free(userContext);
			return 0;	
	}
	
	Init_LDT_Descriptor(userContext->ldtDescriptor, userContext->ldt, NUM_USER_LDT_ENTRIES);
/*end ldtDescriptor */


/* ltdSelector, csSelector, dsSelector*/
	/*Create an LDT selector by calling the routine Selector()*/
	userContext->ldtSelector = Selector(KERNEL_PRIVILEGE, true, Get_Descriptor_Index(userContext->ldtDescriptor));

	/*Create a text segment descriptor by calling Init_Code_Segment_Descriptor() */
	Init_Code_Segment_Descriptor( &userContext->ldt[0], (ulong_t)userContext->memory, size/PAGE_SIZE, USER_PRIVILEGE);
	/*Create a data segment descriptor by calling Init_Data_Segment_Descriptor() */
	Init_Data_Segment_Descriptor( &userContext->ldt[1], (ulong_t)userContext->memory, size/PAGE_SIZE, USER_PRIVILEGE);
	
	/*Create an data segment selector by calling the routine Selector() */
	userContext->csSelector = Selector(USER_PRIVILEGE, false, 0);
	/*Create an text segment selector by calling the routine Selector() */
	userContext->dsSelector = Selector(USER_PRIVILEGE, false, 1);
/*end ltd-, cs-, dsSelector */

/*refCount: May use this in future to allow multiple threads in the same user context  */
	userContext->refCount = 0;
		
	return userContext;
}



static bool Validate_User_Memory(struct User_Context* userContext,
    ulong_t userAddr, ulong_t bufSize)
{
    ulong_t avail;

    if (userAddr >= userContext->size)
        return false;

    avail = userContext->size - userAddr;
    if (bufSize > avail)
        return false;

    return true;
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Destroy a User_Context object, including all memory
 * and other resources allocated within it.
 */
void Destroy_User_Context(struct User_Context* userContext)
{
    /*
     * Hints:
     * - you need to free the memory allocated for the user process
     * - don't forget to free the segment descriptor allocated
     *   for the process's LDT
     */
    //TODO("Destroy a User_Context");
    
	if(USERSEGDEBUG) {
		Print("Destroying User_Context!\n");
	}

	Free_Segment_Descriptor(userContext->ldtDescriptor);

	Disable_Interrupts();

	Free(userContext->memory);
	userContext->memory = 0;
	Free(userContext);

	Enable_Interrupts();    
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
     * - Determine where in memory each executable segment will be placed
     * - Determine size of argument block and where it memory it will
     *   be placed
     * - Copy each executable segment into memory
     * - Format argument block in memory
     * - In the created User_Context object, set code entry point
     *   address, argument block address, and initial kernel stack pointer
     *   address
     */
    //TODO("Load a user executable into a user memory space using segmentation");
    
	int i=0;
	
	unsigned numArgs=0;  	//number of arguments for the programm
	ulong_t argBlockSize=0; 	//size of the argument block space
		
	ulong_t temp_vaddr_segment=0;	//temporary Segment Address used for calculations of max_vaddr_segment		

	ulong_t max_vaddr_segment=0;		//theoretical the highest possible vaddress of a segment
	ulong_t start_vaddr_argblock=0;		//start Address of Argument Block space
	ulong_t max_vaddr_argblock=0;		//end Address of Argument Block space
	ulong_t max_vaddr=0;	//sum of  last_segment_vaddr/memsize -> gives us highest possible vaddress !!without argblock!!


/*	struct User_Context *userContext=0; */	
	struct Exe_Segment *segment;

/*get Segment Address space*/
	/* get max_vaddr_segment */
	/*for (i = 0; i < exeFormat->numSegments; ++i)*/ 
	for (i = 0; i < exeFormat->numSegments; i++)	
	{
		segment = &(exeFormat->segmentList[i]);
		temp_vaddr_segment = segment->startAddress + segment->sizeInMemory;

		if (temp_vaddr_segment > max_vaddr_segment)
			max_vaddr_segment = temp_vaddr_segment;
	}
	
/* roud up the calculated Segment Address Space */
	max_vaddr = Round_Up_To_Page(max_vaddr_segment);
	
/*get argument block space size*/	
	Get_Argument_Block_Size(command, &numArgs, &argBlockSize); 
	start_vaddr_argblock = max_vaddr;

/*round up the argument block space size*/
	max_vaddr += Round_Up_To_Page(argBlockSize);
	max_vaddr_argblock = max_vaddr;
	
/* add address space required for the stack*/	
	max_vaddr += DEFAULT_USER_STACK_SIZE;	

	

	/*create the user_context for our user-thread*/
/*	userContext = Create_User_Context(max_vaddr);
	if(!userContext)
		return ENOMEM;
*/
	(*pUserContext) = Create_User_Context(max_vaddr);
	if(!(*pUserContext))
		return ENOMEM;
	

/*copy segment-data into memory */	
	for (i = 0; i < exeFormat->numSegments; i++)
	{
		segment = &exeFormat->segmentList[i];
		memcpy((*pUserContext)->memory + segment->startAddress, exeFileData + segment->offsetInFile, segment->lengthInFile);
	}


/*copy Argument block space into memory */	
	Format_Argument_Block((*pUserContext)->memory + start_vaddr_argblock, numArgs, start_vaddr_argblock , command);
/*	*pUserContext = userContext;*/	
	

	
     /* - In the created User_Context object, set code entry point
     *   address, argument block address, and initial kernel stack pointer
     *   address
	 */
	(*pUserContext)->entryAddr = exeFormat->entryAddr;
	(*pUserContext)->argBlockAddr = start_vaddr_argblock;
	(*pUserContext)->stackPointerAddr = max_vaddr_argblock;	
	

	
	/* debug stuff delete afterwards */
	if(USERSEGDEBUG) {
		Print("    command:\t\t%s numArgs:\t%d argBlockSize:\t%li\n", command, numArgs, argBlockSize);
		Print("    seg_size:\t\t%li\n",max_vaddr_segment); 
		Print("    seg pages:\t\t%li\n\n",(max_vaddr_segment)/PAGE_SIZE); 
		Print("    arg_size:\t\t%li\n",argBlockSize); 
		
		Print("    entry_addr:\t\t%li\n",(*pUserContext)->entryAddr);
		Print("    stack_addr:\t\t%li\n", (*pUserContext)->stackPointerAddr);
		Print("    argb_addr:\t\t%li\n", (*pUserContext)->argBlockAddr);
		Print("    max_vaddr:\t\t%li\n\n",max_vaddr);
		
		Print("    mem address:\t%ld\n\n", (ulong_t)(*pUserContext)->memory);
		Print("    size:\t\t%ld\n", (*pUserContext)->size);
		Print("    pages used:\t\t%lu\n", (*pUserContext)->size/PAGE_SIZE);
		Print("    LDT Descriptor:\t%d\n", (*pUserContext)->ldtSelector);
		Print("    code selector:\t%d\n", (*pUserContext)->csSelector);
		Print("    data selector:\t%d\n", (*pUserContext)->dsSelector);
	}	
	
	
	return 0;    
}

/*
 * Copy data from user memory into a kernel buffer.
 * Params:
 * destInKernel - address of kernel buffer
 * srcInUser - address of user buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */
bool Copy_From_User(void* destInKernel, ulong_t srcInUser, ulong_t bufSize)
{
    /*
     * Hints:
     * - the User_Context of the current process can be found
     *   from g_currentThread->userContext
     * - the user address is an index relative to the chunk
     *   of memory you allocated for it
     * - make sure the user buffer lies entirely in memory belonging
     *   to the process
     */
    //TODO("Copy memory from user buffer to kernel buffer");
    //Validate_User_Memory(NULL,0,0); /* delete this; keeps gcc happy */
    
	if(Validate_User_Memory(g_currentThread->userContext, srcInUser, bufSize))
	{
		//Function memcopy declared in ../common/string.h + *.c
		memcpy(destInKernel, (const void *)(g_currentThread->userContext->memory + srcInUser), bufSize);
		return true;
	}    		
	else 
		return false;
}

/*
 * Copy data from kernel memory into a user buffer.
 * Params:
 * destInUser - address of user buffer
 * srcInKernel - address of kernel buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */
bool Copy_To_User(ulong_t destInUser, void* srcInKernel, ulong_t bufSize)
{
    /*
     * Hints: same as for Copy_From_User()
     */
    //TODO("Copy memory from kernel buffer to user buffer");
    
    if(Validate_User_Memory(g_currentThread->userContext, destInUser, bufSize))
	{
		//Function memcopy declared in ../common/string.h + *.c
		memcpy((void *)(g_currentThread->userContext->memory + destInUser), srcInKernel, bufSize);
		return true;
	}    		
	else 
		return false;
}

/*
 * Switch to user address space belonging to given
 * User_Context object.
 * Params:
 * userContext - the User_Context
 */
void Switch_To_Address_Space(struct User_Context *userContext)
{
    /*
     * Hint: you will need to use the lldt assembly language instruction
     * to load the process's LDT by specifying its LDT selector.
     */
    //TODO("Switch to user address space using segmentation/LDT");
    
    //this was done like in the function "Load_Task_Register(void)" in File tss.c
    __asm__ __volatile__ (
    "lldt %0"	
    : 
    : "a" (userContext->ldtSelector) 
    );
}

