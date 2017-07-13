
static int Sys_Mount( struct Interrupt_State* state )
{
    char *ptr;
    struct User_Context *u;
    mountReq *req;
    
    ptr = (char *) state->ebx;
    u = g_currentThread->userContext;
    KASSERT(u);

    // validate pointer 
    if (!Validate_User_Memory(u, ptr, sizeof(mountReq))) {
        Print("invalid memory %x to %x\n", ptr, ptr+sizeof(mountReq));
	return -1;
    }
    req =  (mountReq *) (ptr + 0x80000000);
    
    return Mount(req->mountPoint, req->drive, req->fsType);
}

static int Sys_Open( struct Interrupt_State* state )
{
    char fileName[1024];
    int mode = state->edx;

    const void* userPtr = (const void*) state->ebx;
    unsigned int length = state->ecx;

    // Make sure buf is a reasonable size.
    if ( length > 1024 )
	return -1;

    if ( !Copy_From_User( fileName, userPtr, length ) ) {
	return -1;
    }
    fileName[ length ] = '\0';

    return Open(fileName, mode);
}

static int Sys_Close( struct Interrupt_State* state )
{
     int fd = state->ebx;

     return Close(fd);

}

static int Sys_Delete( struct Interrupt_State* state )
{
    char fileName[1024];

    const void* userPtr = (const void*) state->ebx;
    unsigned int length = state->ecx;

    // Make sure buf is a reasonable size.
    if ( length > 1024 )
	return -1;

    if ( !Copy_From_User( fileName, userPtr, length ) ) {
	return -1;
    }
    fileName[ length ] = '\0';

    return Delete(fileName);
}

static int Sys_Read( struct Interrupt_State* state )
{
    int fd = state->ebx;
    char* buffer = (char*) state->ecx;
    unsigned int length = state->edx;
    struct User_Context *u;

    u = g_currentThread->userContext;
    KASSERT(u);

    // validate pointer 
    if (!Validate_User_Memory(u, buffer, length))
	return -1;
    
    buffer += 0x80000000;

    return Read(fd, buffer, length);
}

static int Sys_Write( struct Interrupt_State* state )
{
    int fd = state->ebx;
    char* buffer = (char*) state->ecx;
    unsigned int length = state->edx;
    struct User_Context *u;

    u = g_currentThread->userContext;
    KASSERT(u);

    // validate pointer 
    if (!Validate_User_Memory(u, buffer, length)) {
	Print("Write failed: invalid buffer %x to %x\n", buffer, buffer+length);
	return -1;
    }
    buffer += 0x80000000;
    
    return Write(fd, buffer, length);
}

static int Sys_Sync( struct Interrupt_State* state )
{
    extern int GOFS_Sync();

    return (GOFS_Sync());
}

static int Sys_CreateDirectory( struct Interrupt_State* state )
{
    char fileName[4096];
    const void* userPtr = (const void*) state->ebx;
    unsigned int length = state->ecx;

    // Make sure buf is a reasonable size.
    if ( length > 1024 )
	return -1;

    if ( !Copy_From_User( fileName, userPtr, length ) ) {
	return -1;
    }
    fileName[ length ] = '\0';

    return CreateDirectory(fileName);
}

static int Sys_Stat( struct Interrupt_State* state )
{
    fileStat *stat;
    struct User_Context *u;
    unsigned int fd = state->ebx;
    char *ptr = (char *) state->ecx;

    u = g_currentThread->userContext;
    KASSERT(u);

    if (!Validate_User_Memory(u, ptr, sizeof(fileStat)))
        return -1;
    ptr += 0x80000000;
    stat = (fileStat *) ptr;

    return Stat(fd, stat);
}

static int Sys_Seek( struct Interrupt_State* state )
{
    unsigned int fd = state->ebx;
    unsigned int offset = state->ecx;

    return Seek(fd, offset);
}

static int Sys_Format( struct Interrupt_State* state )
{
    int drive = state->ebx;
    struct Kernel_Thread* current = g_currentThread;

    Print("in syscall for format\n");
    if (!current->userContext || current->userContext->uid != 0) {
        return -1;
    } else {
        return GOSFS_Format(drive);
    }
}

