
int Mount(char *name, int drive, char *fstype)
{
    mountReq req;
    int num = SYS_MOUNT, rc;

    strcpy(req.mountPoint, name);
    req.drive = drive;
    strcpy(req.fsType, fstype);

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (&req)
    );

    return rc;
}

int Open(char *name, int permissions)
{
    int num = SYS_OPEN, rc;
    size_t len = strlen( name );

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (name), "c" (len), "d" (permissions)
    );

    return rc;
}

int Close(int fd)
{
    int num = SYS_CLOSE, rc;

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (fd)
    );

    return rc;
}

int Delete(char *name)
{
    int num = SYS_DELETE, rc;
    size_t len = strlen( name );

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (name), "c" (len)
    );

    return rc;
}

int Read(int fd, char *buffer, int length)
{
    int num = SYS_READ, rc;
    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (fd), "c" (buffer), "d" (length)
    );

    return rc;
}

int Write(int fd, char *buffer, int length)
{
    int num = SYS_WRITE, rc;
    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (fd), "c" (buffer), "d" (length)
    );

    return rc;
}

int Stat(int fd, fileStat *stat)
{
    int num = SYS_STAT, rc;
    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (fd), "c" (stat)
    );

    return rc;
}

int Seek(int fd, int offset)
{
    int num = SYS_SEEK, rc;
    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (fd), "c" (offset)
    );

    return rc;
}

int CreateDirectory(char *name)
{
    int num = SYS_CREATE_DIRECTORY, rc;
    size_t len = strlen( name );

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (name), "c" (len)
    );

    return rc;
}

int Format(int drive)
{
    int num = SYS_FORMAT, rc;

    Print("libuser in format\n");
    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (drive)
    );

    return rc;
}

