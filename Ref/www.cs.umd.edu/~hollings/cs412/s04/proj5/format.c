#include "libuser.h"
#include "libio.h"
#include "atoi.h"

int Main( int argc, char **argv )
{
    if (argc !=2) {
         Printf("Usage: format <drive num>\n");
	 Exit();
    }
    Format(atoi(argv[1]));
    return 0;
}
