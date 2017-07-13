#include "int.h"
#include "paging.h"
#include "kthread.h"
#include "kassert.h"

// flag to indcate if debugging paging code
int debugFaults;


//
// find a free bit of disk on the paging file for this page
//
unsigned int findSpaceOnPagingFile()
{
     KASSERT(0);
}


//
// write the memory starting at memory to the disk block starting at diskPage
//
void writeToPagingFile(int memory, int diskPage)
{
     KASSERT(0);
}


void printFaultInfo(unsigned int address, pageFaultErrorCode *faultCode)
{
    extern unsigned int freePageCount;

    Print( "Pid %d, Page Fault received, at address %x (%d pages free)\n",
        g_currentThread->pid, address, freePageCount);
    if (faultCode->protectionViolation)
        Print ("   Protection Violation, ");
    else
        Print ("   Non-present page, ");
    if (faultCode->writeFault)
        Print ("Write Fault, ");
    else
        Print ("Read Fault, ");
    if (faultCode->userModeFault)
        Print ("in User Mode\n");
    else
        Print ("in Supervisior Mode\n");
}

// Handler for page faults
void PageFault_Handler( struct Interrupt_State* state )
{
    unsigned long address;
    pageFaultErrorCode faultCode;

    KASSERT( !Interrupts_Enabled() );

    /* get the address */
    __asm__("movl %%cr2,%0":"=r" (address));

    faultCode = *((pageFaultErrorCode *) &(state->errorCode));

    /// rest of your handling code here
    Print ("Unexpected Page Fault received\n");
    printFaultInfo(address, &faultCode);
    Dump_Interrupt_State( state );
    // user faults just kill the process
    if (!faultCode.userModeFault) KASSERT(0);
    Detach_Thread( g_currentThread ); // send the thread to the reaper
    g_killCurrentThread = TRUE;     // don't make the thread runnable again
    g_needReschedule = TRUE;        // pick a new thread

}


//
// lots more of your code here
//

