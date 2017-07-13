 
struct Kernel_Thread* Start_User_Program( const struct User_Program* program,
        Boolean detached , int argc, char *argv[], char *command, int commandLeng\
th)
{
    // your P1 code here

    // copy paramters
    argmemBase = &processMem[userContext->size-4096];
    memcpy(argmemBase, &argc, sizeof(int));

    // subtract base address
    for (i=0; i < argc; i++) argv[i] = (char *) (argv[i] - command);

    memcpy(&argmemBase[sizeof(int)], argv, sizeof(char *) * argc);
    memcpy(&argmemBase[sizeof(int) + sizeof(char *) * argc], command, commandLeng\
th);

    // Fire it up!
    return Start_User_Thread( userContext, program->entryAddr, detached );
}
