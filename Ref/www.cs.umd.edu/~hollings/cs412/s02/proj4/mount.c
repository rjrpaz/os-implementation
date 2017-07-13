
// A simple mount program for GeekOS

#include "libuser.h"

void Main( int argc, char *argv[] )
{
    int ret;
    int drive;

    if (argc != 4) {
        Print("usage: mount <drive> <directory [pfat|gosfs]\n");
	Exit ();
    }

    drive = atoi(argv[1]);

    if (strcmp(argv[3], "pfat") && strcmp(argv[3], "gosfs")) {
        Print("mount of invalid filesystem type %s\n", argv[3]);
	Exit();
    }

    ret = Mount(argv[2], drive, argv[3]);
    if (ret != 0) {
        Print("mount failed\n");
	Exit ();
    }
}
