#include "libuser.h"
#include "libio.h"
#include "atoi.h"
#include "../string.h"

int Main( int argc, char **argv )
{
    int ret;
    int drive;

    if (argc != 4) {
        Printf("usage: mount <drive> <directory [pfat|gosfs]\n");
	Exit ();
    }

    drive = atoi(argv[1]);

    if (strcmp(argv[3], "pfat") && strcmp(argv[3], "gosfs")) {
        Printf("mount of invalid filesystem type %s\n", argv[3]);
	Exit();
    }

    ret = Mount(argv[2], drive, argv[3]);
    if (ret != 0) {
        Printf("mount failed\n");
	Exit ();
    }
    return 0;
}
