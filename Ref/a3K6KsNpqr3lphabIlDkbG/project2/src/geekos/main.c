/*
 * GeekOS C code entry point
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, Iulian Neamtiu <neamtiu@cs.umd.edu>
 * $Revision: 1.51 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/bootinfo.h>
#include <geekos/string.h>
#include <geekos/screen.h>
#include <geekos/mem.h>
#include <geekos/crc32.h>
#include <geekos/tss.h>
#include <geekos/int.h>
#include <geekos/kthread.h>
#include <geekos/trap.h>
#include <geekos/timer.h>
#include <geekos/keyboard.h>
#include <geekos/dma.h>
#include <geekos/ide.h>
#include <geekos/floppy.h>
#include <geekos/pfat.h>
#include <geekos/vfs.h>
#include <geekos/user.h>

#define MAINDEBUG 1


/*
 * Define this for a self-contained boot floppy
 * with a PFAT filesystem.  (Target "fd_aug.img" in
 * the makefile.)
 */
/*#define FD_BOOT*/

#ifdef FD_BOOT
#  define ROOT_DEVICE "fd0"
#  define ROOT_PREFIX "a"
#else
#  define ROOT_DEVICE "ide0"
#  define ROOT_PREFIX "c"
#endif

#define INIT_PROGRAM "/" ROOT_PREFIX "/shell.exe"


//start project0
void funct_eigen();
//end project0

static void Mount_Root_Filesystem(void);
static void Spawn_Init_Process(void);

/*
 * Kernel C code entry point.
 * Initializes kernel subsystems, mounts filesystems,
 * and spawns init process.
 */
void Main(struct Boot_Info* bootInfo)
{
    Init_BSS();
    Init_Screen();
    Init_Mem(bootInfo);
    Init_CRC32();
    Init_TSS();
    Init_Interrupts();
    Init_Scheduler();
    Init_Traps();
    Init_Timer();
    Init_Keyboard();
    Init_DMA();
    Init_Floppy();
    Init_IDE();
    Init_PFAT();

    Mount_Root_Filesystem();

    Set_Current_Attr(ATTRIB(BLACK, GREEN|BRIGHT));
    Print("Welcome to GeekOS!\n");
    Set_Current_Attr(ATTRIB(BLACK, GRAY));
	

//start project0
/*  TODO("Start a kernel thread to echo pressed keys and print counts");*/
//	Start_Kernel_Thread(&funct_eigen, 0, PRIORITY_NORMAL, false);
//end project0


    Spawn_Init_Process();

    /* Now this thread is done. */
    Exit(0);
}


//start poject0
void funct_eigen()
{
	Keycode c = 0;

	Print("Hallo from Franz!\n");

	do
	{
		c = Wait_For_Key();
		if((c&KEY_RELEASE_FLAG) == 0)			//Beim Taste auslassen nicht mehr ausgeben
		{
			Print("%c\n", c);
		}
	}
	while((c & KEY_SPECIAL_FLAG) == 1 || (c & KEY_CTRL_FLAG) == 0 || (char)c != 'd');

	Exit(0);
}
//end project0


static void Mount_Root_Filesystem(void)
{
    if (Mount(ROOT_DEVICE, ROOT_PREFIX, "pfat") != 0)
	Print("Failed to mount /" ROOT_PREFIX " filesystem\n");
    else
	Print("Mounted /" ROOT_PREFIX " filesystem!\n");

}



static void Spawn_Init_Process(void)
{
//start project1
/*
  // this thread will load&run  ELF files, see the rest in lprog.c
  Print("Starting the Spawner thread...\n");
  Start_Kernel_Thread(Spawner, 0, PRIORITY_NORMAL, true );
*/
//end peoject1

	
//	TODO("Spawn the init process");
	int pid;
    struct Kernel_Thread* Init_Process = 0;
    
	if(MAINDEBUG)
		Print("%s %d pid = %d\n", __FILE__ , __LINE__ , Init_Process->pid);


//	pid = Spawn("/c/b.exe","erstes zweites drittes viertes Argument",&Init_Process, 0);		//OK
//	pid = Spawn("/c/c.exe","",&Init_Process, 0);																		//OK
	
	//INIT_PROGRAM ... /c/shell.exe	
	pid = Spawn(INIT_PROGRAM,"",&Init_Process, 0);	

	if(pid < 0){
		Print("Spawn'ing process failed caused by error: %d\n",pid);
		return;
	}
	else
		Print("Got process with PID: %d\n",pid);
}
