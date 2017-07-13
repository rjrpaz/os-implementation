// Synchronization primitives
// Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
// $Revision: 4.0 $

// This is free software.  You are permitted to use,
// redistribute, and modify it as specified in the file "COPYING".

#include "kthread.h"
#include "int.h"
#include "kassert.h"
#include "screen.h"
#include "synch.h"

// ----------------------------------------------------------------------
// Private functions
// ----------------------------------------------------------------------

#define ASSERT_HELD(mutex) 				\
do {							\
    KASSERT( (mutex)->state == MUTEX_LOCKED );		\
    KASSERT( (mutex)->owner == g_currentThread );	\
} while ( 0 )

static __inline__ void Mutex_Lock_Imp( struct Mutex* mutex )
{
    // make sure we're not already holding the mutex
    KASSERT( mutex->owner != g_currentThread );

    // wait until the mutex is in an unlocked state
    while ( mutex->state == MUTEX_LOCKED ) {
	Wait( &mutex->waitQueue );
    }

    // now it's ours!
    mutex->state = MUTEX_LOCKED;
    mutex->owner = g_currentThread;
}

static __inline__ void Mutex_Unlock_Imp( struct Mutex* mutex )
{
    // make sure it's ours
    ASSERT_HELD( mutex );

    // unlock the mutex
    mutex->state = MUTEX_UNLOCKED;
    mutex->owner = 0;

    // Wake up a single waiter
    Wake_Up_One( &mutex->waitQueue );
}

// ----------------------------------------------------------------------
// Public functions
// ----------------------------------------------------------------------

// Initialize given mutex.
void Mutex_Init( struct Mutex* mutex )
{
    mutex->state = MUTEX_UNLOCKED;
    mutex->owner = 0;
    Clear_Thread_Queue( &mutex->waitQueue );
}

// Lock given mutex.
void Mutex_Lock( struct Mutex* mutex )
{
    // prevent other threads from being scheduled
    Disable_Interrupts();

    Mutex_Lock_Imp( mutex );

    // resume scheduling
    Enable_Interrupts();
}

// Unlock given mutex.
void Mutex_Unlock( struct Mutex* mutex )
{
    // prevent scheduling
    Disable_Interrupts();

    Mutex_Unlock_Imp( mutex );

    // resume scheduling
    Enable_Interrupts();
}

// Initialize given condition.
void Cond_Init( struct Condition* cond )
{
    Clear_Thread_Queue( &cond->waitQueue );
}

// Wait on given condition (protected by given mutex).
void Cond_Wait( struct Condition* cond, struct Mutex* mutex )
{
    // this should only be done while holding the mutex
    ASSERT_HELD( mutex );

    Disable_Interrupts(); // prevent scheduling

    Mutex_Unlock_Imp( mutex );
    Wait( &cond->waitQueue );
    Mutex_Lock_Imp( mutex );

    Enable_Interrupts(); // resume scheduling
}

// Wake up one thread waiting on the given condition.
// The mutex guarding the condition should be held!
void Cond_Signal( struct Condition* cond )
{
    Disable_Interrupts(); // prevent scheduling
    Wake_Up_One( &cond->waitQueue );
    Enable_Interrupts(); // resume scheduling
}

// Wake up all threads waiting on the given condition.
// The mutex guarding the condition should be held!
void Cond_Broadcast( struct Condition* cond )
{
    Disable_Interrupts(); // prevent scheduling
    Wake_Up( &cond->waitQueue );
    Enable_Interrupts(); // resume scheduling
}
