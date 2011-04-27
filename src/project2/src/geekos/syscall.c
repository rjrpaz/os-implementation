/*
 * System call handlers
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003,2004 David Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.59 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/syscall.h>
#include <geekos/errno.h>
#include <geekos/kthread.h>
#include <geekos/int.h>
#include <geekos/elf.h>
#include <geekos/malloc.h>
#include <geekos/screen.h>
#include <geekos/keyboard.h>
#include <geekos/string.h>
#include <geekos/user.h>
#include <geekos/timer.h>
#include <geekos/vfs.h>

// #define DEBUG 1
#undef DEBUG

/*
 * Null system call.
 * Does nothing except immediately return control back
 * to the interrupted user program.
 * Params:
 *  state - processor registers from user mode
 *
 * Returns:
 *   always returns the value 0 (zero)
 */
static int Sys_Null(struct Interrupt_State* state)
{
#ifdef DEBUG
    Print("Sys_Null ha sido invocado\n");
#endif
    return 0;
}

/*
 * Exit system call.
 * The interrupted user process is terminated.
 * Params:
 *   state->ebx - process exit code
 * Returns:
 *   Never returns to user mode!
 */
static int Sys_Exit(struct Interrupt_State* state)
{
    Exit(state->ebx);
    return 0;
}

/*
 * Print a string to the console.
 * Params:
 *   state->ebx - user pointer of string to be printed
 *   state->ecx - number of characters to print
 * Returns: 0 if successful, -1 if not
 */
static int Sys_PrintString(struct Interrupt_State* state)
{
    unsigned int largo = state->ecx;
    char *cadena = NULL;

    cadena = Malloc(largo);

    if (! Copy_From_User(cadena, state->ebx, largo)) {
        Print("Error al invocar Copy_From_User\n");
        Free(cadena);
        return -1;
    }
    Put_Buf(cadena, largo);
    Free(cadena);
    return 0;
}

/*
 * Get a single key press from the console.
 * Suspends the user process until a key press is available.
 * Params:
 *   state - processor registers from user mode
 * Returns: the key code
 */
static int Sys_GetKey(struct Interrupt_State* state)
{
    int keyCode = Wait_For_Key();
    return keyCode;
}

/*
 * Set the current text attributes.
 * Params:
 *   state->ebx - character attributes to use
 * Returns: always returns 0
 */
static int Sys_SetAttr(struct Interrupt_State* state)
{
    Set_Current_Attr(state->ebx);
    return 0;
}

/*
 * Get the current cursor position.
 * Params:
 *   state->ebx - pointer to user int where row value should be stored
 *   state->ecx - pointer to user int where column value should be stored
 * Returns: 0 if successful, -1 otherwise
 */
static int Sys_GetCursor(struct Interrupt_State* state)
{
    int y=0, x=0;
    Get_Cursor(&y, &x);
    if (! Copy_To_User(state->ebx, &y, sizeof(int))) {
        Print("Error al invocar Copy_To_User para almacenar la fila\n");
        return -1;
    }
    if (! Copy_To_User(state->ecx, &x, sizeof(int))) {
        Print("Error al invocar Copy_To_User para almacenar la columna\n");
        return -1;
    }
    return 0;
}

/*
 * Set the current cursor position.
 * Params:
 *   state->ebx - new row value
 *   state->ecx - new column value
 * Returns: 0 if successful, -1 otherwise
 */
static int Sys_PutCursor(struct Interrupt_State* state)
{
    if (Put_Cursor(state->ebx, state->ecx))
        return 0;
    else
        return -1;
}

/*
 * Create a new user process.
 * Params:
 *   state->ebx - user address of name of executable
 *   state->ecx - length of executable name
 *   state->edx - user address of command string
 *   state->esi - length of command string
 * Returns: pid of process if successful, error code (< 0) otherwise
 */
static int Sys_Spawn(struct Interrupt_State* state)
{
    char *pathname=NULL;
    char *command=NULL;
    struct Kernel_Thread *pThread = NULL;
    int pid=0;

    pathname=Malloc(state->ecx+1);
    memset(pathname, 0x0, state->ecx+1);
    if (! Copy_From_User(pathname, state->ebx, state->ecx)) {
        Print("Error al invocar Copy_From_User para el pathname\n");
        Free(pathname);
        return -1;
    }

    command=Malloc(state->esi+1);
    memset(command, 0x0, state->ecx+1);
    if (! Copy_From_User(command, state->edx, state->esi)) {
        Print("Error al invocar Copy_From_User para el comando\n");
        Free(pathname);
        Free(command);
        return -1;
    }

#ifdef DEBUG
    Print("PATH: %s\n", pathname);
    Print("COMM: %s\n", command);
#endif

//    Enable_Interrupts();
    pid = Spawn(pathname, command, &pThread);
//    Disable_Interrupts();
    Free(pathname);
    Free(command);
    return pid;
}

/*
 * Wait for a process to exit.
 * Params:
 *   state->ebx - pid of process to wait for
 * Returns: the exit code of the process,
 *   or error code (< 0) on error
 */
static int Sys_Wait(struct Interrupt_State* state)
{
    struct Kernel_Thread *wThread = Lookup_Thread(state->ebx);

    int exit_code = Join(wThread);
    return exit_code;
}

/*
 * Get pid (process id) of current thread.
 * Params:
 *   state - processor registers from user mode
 * Returns: the pid of the current thread
 */
static int Sys_GetPID(struct Interrupt_State* state)
{
    return(g_currentThread->pid);
}


/*
 * Global table of system call handler functions.
 */
const Syscall g_syscallTable[] = {
    Sys_Null,
    Sys_Exit,
    Sys_PrintString,
    Sys_GetKey,
    Sys_SetAttr,
    Sys_GetCursor,
    Sys_PutCursor,
    Sys_Spawn,
    Sys_Wait,
    Sys_GetPID,
};

/*
 * Number of system calls implemented.
 */
const int g_numSyscalls = sizeof(g_syscallTable) / sizeof(Syscall);










