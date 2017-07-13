// A library of useful functions for user-mode programs.

#include "libuser.h"

// ----------------------------------------------------------------------
// Wrappers for system calls
// ----------------------------------------------------------------------

#define SYSCALL "int $0x90"

// system call numbers
#define SYS_NULL 0

int Null( void )
{
    int num = SYS_NULL, rc;

    __asm__ __volatile__ (
	SYSCALL
	: "=a" (rc)	// system call returns value in eax
	: "a" (num)	// system call number specified in eax
    );

    return rc;
}
