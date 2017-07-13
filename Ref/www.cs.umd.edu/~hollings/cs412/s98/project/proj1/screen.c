/*
 *  CMSC 412 
 *
 *  You should include screen manipulating functions within this
 *  file, as well as create screen.h 
 */


void Init_screen () /* Read system configuration byte, initialize
                       cursor, .. etc.                            */
{
   char far *ptr ;
   int i ;

   ptr = (char *)0x00000410L ;  /* address of system config. byte */
   if ( (*ptr & 0x30) == 0x30 )
        charbase = (char *) 0xB0000000L ;  /* monochrome board */
   else
        charbase = (char *) 0xB8000000L ;  /* color board */

   _AX = 3 ;
   geninterrupt (0x10) ;  /* Call BIOS function 10h to set screen attr */
   _AX = 0x0100 ;  /* set cursor attribute code */
   _CX = 0xFF00 ;  /* make cursor invisible */
   geninterrupt (0x10) ;

/*
 *  Add any more code that you might need.
 */
}
