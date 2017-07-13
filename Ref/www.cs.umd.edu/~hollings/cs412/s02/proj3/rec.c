// A test program for GeekOS user mode

#include "libuser.h"


void Recurse(int x)
{
    int stuff[512];

    if (x == 0) return;

    stuff[0] = x;
    Print("calling Recurse %d\n", x);
    Recurse(x-1);
}

void Main( void )
{
    // change recurse to 5-10 to see stack faults without page outs
    Recurse(512);
}

