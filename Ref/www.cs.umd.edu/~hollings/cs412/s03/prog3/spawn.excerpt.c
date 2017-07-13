
#define MAX_ARGS 10

static int Sys_Spawn( struct Interrupt_State* state )
{

    int i;
    int ret;
    int argc;
    char *argv[MAX_ARGS];
    struct Kernel_Thread *thread;
    struct User_Program *programPtr;
    const void* userPtr = (const void*) state->ebx;
    unsigned int length = state->ecx;
    unsigned char* buf;

    // Make sure buf is a reasonable size.
    if ( length > 1024 )
	return -1;

    buf = Malloc_Atomic( length + 1 );
    if ( buf == 0 )
	return -1;

    if ( !Copy_From_User( buf, userPtr, length ) ) {
	Free_Atomic( buf );
	return -1;
    }
    buf[ length ] = '\0';

    // convert buf to argc and argv;
    i = 0;
    argc = 0;
    argv[0] = buf;
    while (i < 1024 && buf[i]) {
         if (buf[i] == ' ') {
             buf[i] = '\0';
             i++;

             // skip sequence of white space
             while (i < 1024 && buf[i] == ' ') i++;

             if (i < 1024) {
                 ++argc;
                 if (argc == MAX_ARGS) return -1;
                 argv[argc] = &buf[i];
             }
         } else {
             i++;
         }
    }
    argc++;

    programPtr = loadElfProgram(argv[0]);
    if (programPtr) {
	ret = thread->pid;

        // setup argc and argv
        thread = Start_User_Program(programPtr, FALSE, argc, argv, buf, length);

        ret = thread->pid;

	if (programPtr->program) {
	    Free_Atomic( (char *) programPtr->executable );
	}
    } else {
        thread = 0;
	ret = -1;
    }

    KASSERT( Interrupts_Enabled() );

    return ret;
}
