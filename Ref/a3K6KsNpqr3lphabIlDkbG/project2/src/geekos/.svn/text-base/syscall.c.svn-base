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

#define SYSCALLDEBUG 1

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
	if(SYSCALLDEBUG)
		Print("Sys_Null SystemCall has been called!\n");
		
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
    //TODO("Exit system call");
    Exit(state->ebx);
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
    //TODO("PrintString system call");

	char *msg;
	uint_t length = state->ecx;

/*	if(length>1024)
	{
		if(SYSCALLDEBUG)
			Print("Sys_PrintString SystemCall has been called!\n");
	
		return -1;
	}
*/
	if((msg = Malloc(sizeof(char) * length + 1)) == NULL)		//Malloc() returns void*, which is NULL if there was an error
	{
		if(SYSCALLDEBUG)
			Print("Sys_PrintString SystemCall Error %d\n", ENOMEM);	

		return -1;
	}

	if(!Copy_From_User(msg, state->ebx, length + 1))
	{
		Free(msg);
		return -1;
	}
	
	Print("%s",msg);
	Free(msg);
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
    //TODO("GetKey system call");
    
    /*see project0 -> File: main.c function: void funct_eigen() */ 
	Keycode c;
	c = Wait_For_Key();
	return c;    
}

/*
 * Set the current text attributes.
 * Params:
 *   state->ebx - character attributes to use
 * Returns: always returns 0
 */
static int Sys_SetAttr(struct Interrupt_State* state)
{
    //TODO("SetAttr system call");
    
    Set_Current_Attr((uchar_t) state->ebx);
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
    //TODO("GetCursor system call");
    int row, col;
    
    //Get_Cursor declared in screen.h + *.c
    Get_Cursor(&row, &col);
    
	if(  (Copy_To_User(state->ebx, &row, sizeof(int)))  &&  (Copy_To_User(state->ecx, &col, sizeof(int)))  )
		return 0;	
	
	else
		return -1;    
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
    //TODO("PutCursor system call");
    
    int row = state->ebx;
    int col = state->ecx;
    
    //Put_Cursor declared in screen.h + *.c
    if(Put_Cursor(row, col))
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
    //TODO("Spawn system call");
    
	if(SYSCALLDEBUG){
		Print("Sys_Spawn SystemCall has been called!\n");
	}

	struct Kernel_Thread *user_process;
	int pid;

	char *program;
	int plength = state->ecx;
	if(SYSCALLDEBUG)
		Print("Sys_Spawn plength: %d\n", plength);

	char *command;
	int clength = state->esi;
	if(SYSCALLDEBUG)
		Print("Sys_Spawn clength: %d\n",clength);


	int detached=0;
	detached = state->edi;

	if (clength > VFS_MAX_PATH_LEN)
		return ENAMETOOLONG; //== -8

	program = Malloc(sizeof(char) * plength + 1);
	command = Malloc(sizeof(char) * clength + 1);
	strcpy(program,"");
	strcpy(command,"");

	if ( (!Copy_From_User(program, state->ebx, plength + 1)) || (!Copy_From_User(command, state->edx, clength + 1)) ){
		Free(program);
		Free(command);
		return -1;
	}
	
	Enable_Interrupts();	

	//Function int Spawn(const char *program, const char *command, struct Kernel_Thread **pThread, bool detached)
	//declared in user.h + *.c	 if it was successful return value is pid otherwise an error code is returned
	pid = Spawn(program, command, &user_process, detached);
	
	Disable_Interrupts();	
	
	if (SYSCALLDEBUG && (pid<0))
		Print("Sys_Spawn error with code %d\n", pid);

	Free(program);
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
    //TODO("Wait system call");
    
	if(SYSCALLDEBUG)
		Print("Sys_Wait SystemCall has been called!\n");

	//need to look at the "int Join (struct Kernel_Thread *bla)" in kthread.c
	//Join returns exit code (int)
	
	int exit_code;
	struct Kernel_Thread *kthread = Lookup_Thread(state->ebx);

	if (kthread != 0 && kthread->owner != g_currentThread)
		return -1;

	Enable_Interrupts();
	exit_code = Join (kthread);
	Disable_Interrupts();

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
    //TODO("GetPID system call");
    
    return g_currentThread->pid;
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
