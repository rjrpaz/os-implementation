// A test program for GeekOS user mode

#include "libuser.h"

void Main( int argc, char *argv[] )
{
    if (argc !=2) {
         Print("Usage: format <drive num>\n");
	 Exit();
    }
    Format(atoi(argv[1]));
}
