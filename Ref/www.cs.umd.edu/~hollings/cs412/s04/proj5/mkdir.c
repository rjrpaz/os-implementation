#include "libuser.h"
#include "libio.h"

int Main( int argc, char **argv )
{
    if (argc != 2) {
        Printf("Usage: mkdir <directory>\n");
	Exit();
    }

    CreateDirectory(argv[1]);
    return 0;
}
