/* 
* ========================================================================
*                     Standard IO functions file 
* ========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dos.h>
#include <alloc.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "io412.h"

/*
 *  klib.h is to include prototypes to Put_char()
 */

#include "klib.h"


/*
* ========================================================================
* PROCEDURE Cprintf (interface function).
*
*      Prints the given string to the screen driver, using Put_char.
*   If you want to use it to the normal standard output, you will
*   need to change Put_char() to putch(), which should be built in.
*      Put_char() should now be a system call kept in klib.c.  Recall
*   this file consists of system calls whose body of code are
*   gen_interrupt's to System_service
* ========================================================================
 */

#define MAX_PRINT_LENGTH 255

int Cprintf(char *str, ...)
{
  int count,    /* loop index for printing the string */
	  old0x21;

  char outstring[MAX_PRINT_LENGTH]; 
			/* contains the "real" string (with */
			/* all variables included in it)    */
  va_list argptr;       /* will point to a (possibly empty) list of     */
			/* variables to be inserted into the string.    */
			/* (for more info, read about the 'C' ellipsis  */
			/* ('...') capabilities )                       */

  strset(outstring,'\0');

  /* point to the list of optional arguments */

  va_start(argptr,str);

  /* stuff the variables into the strings */

  old0x21 = inp(0x21);
  outp(0x21,old0x21|3);

/*
 * PUSHF and so on are used because Mod_int is now only accessible
 * by the kernel.   You could have defined something similar within
 * this file (and make it local to this file so that user programs
 * could not access the function).   PUSHF pushes flags onto the stack.
 */

  asm PUSHF;
  asm CLI;
  vsprintf(outstring,str,argptr);
  asm POPF;
  outp(0x21,old0x21);

  /* restore the stackpointers correctly */

  va_end(argptr);

  /* print the string on the screen */

  for( count=0; outstring[count] !='\0'; count++ )
    Put_char(outstring[count]);

  return( count );
}

/*  ========== END Cprintf()  */
