// Synchronization primitives
// Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
// $Revision: 4.0 $

// This is free software.  You are permitted to use,
// redistribute, and modify it as specified in the file "COPYING".

#ifndef SYNCH_H
#define SYNCH_H

#include "thrqueue.h"

// mutex states
enum { MUTEX_UNLOCKED, MUTEX_LOCKED };

struct Mutex {
    int state;
    struct Kernel_Thread* owner;
    struct Thread_Queue waitQueue;
};

#define MUTEX_INITIALIZER { MUTEX_UNLOCKED, 0, THREAD_QUEUE_INITIALIZER }

struct Condition {
    struct Thread_Queue waitQueue;
};

void Mutex_Init( struct Mutex* mutex );
void Mutex_Lock( struct Mutex* mutex );
void Mutex_Unlock( struct Mutex* mutex );

void Cond_Init( struct Condition* cond );
void Cond_Wait( struct Condition* cond, struct Mutex* mutex );
void Cond_Signal( struct Condition* cond );
void Cond_Broadcast( struct Condition* cond );

#endif // SYNCH_H
