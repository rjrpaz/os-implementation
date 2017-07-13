// GeekOS timer interrupt support
// Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
// $Revision: 5.0.2.1 $

// This is free software.  You are permitted to use,
// redistribute, and modify it as specified in the file "COPYING".

#include "io.h"
#include "int.h"
#include "irq.h"
#include "kthread.h"
#include "timer.h"

#define MAX_TIMER_EVENTS	100

static int timerDebug = 0;
static int timeEventCount;
static int nextEventID;
timerEvent pendingTimerEvents[MAX_TIMER_EVENTS];

// global tick counter
volatile unsigned long g_numTicks;

// Maximum number of ticks a thread can use before
// we suspend it and choose another.
#define MAX_TICKS 4

int g_Quantum = MAX_TICKS;

// ----------------------------------------------------------------------
// Private functions
// ----------------------------------------------------------------------

static void Timer_Interrupt_Handler( struct Interrupt_State* state )
{
    int i;
    struct Kernel_Thread* current = g_currentThread;

    Begin_IRQ( state );

    // Update global and per-thread number of ticks
    ++g_numTicks;
    ++current->numTicks;

    // update timer events
    for (i=0; i < timeEventCount; i++) {
	if (pendingTimerEvents[i].ticks == 0) {
	    if (timerDebug) Print("timer: event %d expired (%d ticks)\n", 
	        pendingTimerEvents[i].id, pendingTimerEvents[i].origTicks);
	    (pendingTimerEvents[i].callBack)(pendingTimerEvents[i].id);
	} else {
	    pendingTimerEvents[i].ticks--;
	}
    }
    // If thread has been running for MAX_TICKS,
    // inform the interrupt return code that we want
    // to choose a new thread.
    if ( current->numTicks >= g_Quantum ) {
	g_needReschedule = TRUE;
        if (current->currentReadyQueue < (maxQueueLevel-1)) {
            // Print("process %d moved to ready queue %d\n", current->pid, current->currentReadyQueue);
            current->currentReadyQueue++;
        }

    }

    End_IRQ( state );
}

// ----------------------------------------------------------------------
// Public functions
// ----------------------------------------------------------------------

void Init_Timer( void )
{
    // TODO: reprogram the timer to set the frequency.
    // In bochs, it defaults to 18Hz, which is actually pretty
    // reasonable.

    unsigned short irqMask;

    Print( "Initializing timer...\n" );

    // configure for default clock
    Out_Byte(0x43, 0x36);
    Out_Byte(0x40, 0x00);
    Out_Byte(0x40, 0x00);

    // Install an interrupt handler for the timer IRQ
    Install_IRQ( TIMER_IRQ, &Timer_Interrupt_Handler );

    // Enable the IRQ in the IRQ mask
    irqMask = Get_IRQ_Mask();
    irqMask &= ~(1 << TIMER_IRQ);
    Set_IRQ_Mask( irqMask );
}

int Start_Timer(int ticks, timerCallback cb)
{
    int ret;

    KASSERT(!Interrupts_Enabled());

    if (timeEventCount == MAX_TIMER_EVENTS) {
	return -1;
    } else {
	ret = nextEventID++;
	pendingTimerEvents[timeEventCount].id = ret;
	pendingTimerEvents[timeEventCount].callBack = cb;
	pendingTimerEvents[timeEventCount].ticks = ticks;
	pendingTimerEvents[timeEventCount].origTicks = ticks;
	timeEventCount++;

	return ret;
    }
}

int Get_Remaing_Timer_Ticks(int id)
{
    int i;

    KASSERT(!Interrupts_Enabled());
    for (i=0; i < timeEventCount; i++) {
	if (pendingTimerEvents[i].id == id) {
	    return pendingTimerEvents[i].ticks;
	}
    }

    return -1;
}

int Cancel_Timer(int id)
{
    int i;
    KASSERT(!Interrupts_Enabled());
    for (i=0; i < timeEventCount; i++) {
	if (pendingTimerEvents[i].id == id) {
	    pendingTimerEvents[i] = pendingTimerEvents[timeEventCount-1];
	    timeEventCount--;
	    return 0;
	}
    }

    Print("timer: unable to find timer id %d to cancel it\n", id);
    return -1;
}


