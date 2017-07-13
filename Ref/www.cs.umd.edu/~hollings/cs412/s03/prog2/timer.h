// GeekOS timer interrupt support
// Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
// $Revision: 5.0.2.1 $

// This is free software.  You are permitted to use,
// redistribute, and modify it as specified in the file "COPYING".

#ifndef TIMER_H
#define TIMER_H

#define TIMER_IRQ 0

extern volatile unsigned long g_numTicks;

typedef void (*timerCallback)(int);

void Init_Timer( void );

typedef struct {
    int ticks;				// timer code decrements this
    int id;				// unqiue id for this timer even
    timerCallback callBack;		// Queue to wakeup on timer expire
    int origTicks;
} timerEvent;

int Start_Timer(int ticks, timerCallback);
int Get_Remaing_Timer_Ticks(int id);
int Cancel_Timer(int id);

#endif // TIMER_H
