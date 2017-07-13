// User mode context
// Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
// $Revision: 1.1.2.1 $

// This is free software.  You are permitted to use,
// redistribute, and modify it as specified in the file "COPYING".

#ifndef USER_H
#define USER_H

#include "ktypes.h"
#include "segment.h"

// We need one LDT entry each for user code and data segments.
#define NUM_USER_LDT_ENTRIES 2

// A user mode context which can be attached to a Kernel_Thread,
// to allow it to execute in user mode (ring 3).
struct User_Context {
    // Each user context contains a local descriptor table with
    // just enough room for one code and one data segment
    // describing the process's memory.
    struct Segment_Descriptor ldt[NUM_USER_LDT_ENTRIES];// offset 0
    struct Segment_Descriptor* ldtDescriptor;		// offset 16

    // The memory space used by the process.
    void* memory;					// offset 20
    unsigned long size;					// offset 24

    // May use this in future to allow multiple threads
    // in the same user context
    int refCount;					// offset 28

    // Selector for the LDT's descriptor in the GDT
    unsigned short ldtSelector;				// offset 32

    // Selectors for the user context's code and data segments
    // (which reside in its LDT)
    unsigned short csSelector;				// offset 34
    unsigned short dsSelector;				// offset 36

    int *semaphores;
};

// A convenient data structure for describing a user program
// linked into the kernel.
struct User_Program {
    const char* name;

    unsigned int size;
    unsigned int dataSize;
    unsigned int programSize;

    unsigned int dataOffset;

    unsigned long entryAddr;
    const unsigned char*data;		/* pointer to start of init data */
    const unsigned char*program;	/* pointer to start of code in file */
    const unsigned char*executable;	/* pointer to entire executable file */

};

struct Kernel_Thread;
struct Interrupt_State;

struct User_Context* Create_User_Context( unsigned long size );
void Destroy_User_Context( struct User_Context* context );
void Attach_User_Context(
    struct Kernel_Thread* kthread, struct User_Context* context );
void Detach_User_Context( struct Kernel_Thread* kthread );
void Switch_User_Context(
    struct Kernel_Thread* kthread, struct Interrupt_State* state );

struct Kernel_Thread* Start_User_Program( const struct User_Program* program,
	Boolean detached , char *command, int commandLength);

Boolean Validate_User_Memory( struct User_Context* userContext,
    void* userPtr, unsigned long bufSize );

static __inline__ void*
User_To_Kernel( struct User_Context* userContext, void* userPtr )
{
    unsigned char* userBase = (unsigned char*) userContext->memory;
    unsigned long offset = (unsigned long) userPtr;

    return (void*) userBase + offset;
}

Boolean Copy_From_User( void* destInKernel, const void* srcInUser,
	unsigned long bufSize );
Boolean Copy_To_User( void* destInUser, const void* srcInKernel,
	unsigned long bufSize );

// Table of all User_Programs linked into the kernel.
extern const struct User_Program* g_userProgramTable[];
extern const unsigned g_numUserPrograms;

#endif // USER_H
