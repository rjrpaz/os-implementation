// A library of useful functions for user-mode programs.

#include <stdarg.h>
#include "libuser.h"

// Entry point.  Calls user program's Main() routine,
// then exits.  TODO: pass arguments to Main().
void _Entry( void )
{
    int i;
    int argc;
    char **argv;
    unsigned int register baseAddr;
    extern void Main( int argc, char *argv[]);

    __asm( "movl %%ebp, %0" : "=r" (baseAddr));

    baseAddr += sizeof(int);

    argc = *((unsigned int *) baseAddr);
    argv = (char **) (baseAddr + sizeof(int));

    baseAddr += sizeof(int) + sizeof(char *) * argc;
    for (i=0; i < argc; i++) {
        argv[i] += baseAddr;
    }
    Main(argc, argv);

    Exit();
}

// ----------------------------------------------------------------------
// Wrappers for system calls
// ----------------------------------------------------------------------

#define SYSCALL "int $0x90"

// system call numbers
#define SYS_NULL 0
#define SYS_EXIT 1
#define SYS_PRINTSTRING 2
#define SYS_GETKEY 3
#define SYS_SETATTR 4
#define SYS_SPAWN 5
#define SYS_WAIT 6
#define SYS_SET_SCHEDULING_POLICY 7
#define SYS_GETTIMEOFDAY 8
#define SYS_CREATESEMAPHORE 9
#define SYS_P 10
#define SYS_V 11
#define SYS_DESTROY_SEMAPHORE 12

// ....

int Set_Scheduling_Policy(int policy, int quantum)
{
    int num = SYS_SET_SCHEDULING_POLICY, rc;

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (policy), "c" (quantum)
    );

    return rc;
}

int Get_Time_Of_Day()
{
    int num = SYS_GETTIMEOFDAY, rc;

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num)
    );

    return rc;
}

int Create_Semaphore(char *name, int ival)
{
    int num = SYS_CREATESEMAPHORE, rc;
    size_t len = strlen( name );

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (name), "c" (len), "d" (ival)
    );

    return rc;
}

int P(int sem)
{
    int num = SYS_P, rc;

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (sem)
    );

    return rc;
}

int V(int sem)
{
    int num = SYS_V, rc;

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (sem)
    );

    return rc;
}

int Destroy_Semaphore(int sem)
{
    int num = SYS_DESTROY_SEMAPHORE, rc;

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)
	: "a" (num), "b" (sem)
    );

    return rc;
}

// ----------------------------------------------------------------------
// Useful functions
// ----------------------------------------------------------------------

static Boolean s_echo = TRUE;

void Echo( Boolean enable )
{
    s_echo = enable;
}

void Read_Line( char* buf, size_t bufSize )
{
    size_t n = 0;
    Keycode k;
    Boolean done = FALSE;

    bufSize--;
    do {
    again:
	k = Get_Key();
	if ( (k & KEY_SPECIAL_FLAG) || (k & KEY_RELEASE_FLAG) )
	    goto again;

	k &= 0xff;
	if ( k == '\r' )
	    k = '\n';

	if ( s_echo )
	    Put_Char( k );

	if ( k == '\n' )
	    done = TRUE;

	if ( n < bufSize ) {
	    *buf++ = k;
	    ++n;
	}
    }
    while ( !done );

    *buf = '\0';
}


void *Malloc_Atomic(unsigned int size) {
    return 0;
}

