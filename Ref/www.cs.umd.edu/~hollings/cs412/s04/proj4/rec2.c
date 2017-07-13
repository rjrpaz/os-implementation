// A test program for GeekOS user mode

#include "libuser.h"
#include "libio.h"

void Recurse(int x)
{
    int stuff[512];

    if (x == 0) return;

    stuff[0] = x;
    Printf("calling Recurse %d\n", x);
    Recurse(x-1);
}

int Main( int argc, char ** argv )
{
    // change recurse to 5-10 to see stack faults without page outs
    Recurse(3500);

   return 0;
}

