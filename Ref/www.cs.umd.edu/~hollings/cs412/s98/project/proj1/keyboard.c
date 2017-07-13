/*
 *  Sample skeleton outline for keyboard.c
 *   NOTE:  This is incomplete.  Fill in the rest yourself.
 */

#define INT_DISABLE 0
#define INT_ENABLE  0x200

/*
 *  This procedure is an interrupt subroutine which will temporarily
 *  substitute for the keyboard handler normally used (the substitution
 *  is made in main.c).    The normal handler is a BIOS keyboard device
 *  driver.   
 *
 *  Despite looking like a conventional function, the keyboard handler
 *  is not explicitly called in a program.    Instead it is "hooked"
 *  to interrupt 9h, which will execute the call.  Notice no arguments
 *  can be passed.  (There will be an "exception" to this in project 2).
 */

void interrupt Key_handler()
{ 
  int scancode,  /* keyboard data port value */
      cv,        /* keyboard control port value */
      intflag;   /* current state of the interrupt flag */

  asm CLI;       /* Disables interrupts using inline assembly */
  
  scancode = inp( 0x60 );   /* Read in the scan code from port 60 */

/* Reset the control port to read in the next character */
  cv = inp( 0x61 );   /* Read the value in the control port */

/* Reset to read in next character by switching the 7th bit on
 * and off on port 61
 */
  outp( 0x61, cv | 0x80 ); 
  outp( 0x61, cv ); 

/* 
 *   You fill in the code here to convert the scan code to ASCII
 *   and place it in the queue structure.
 */

     /*   FILL CODE HERE */
      
     /*  END OF YOUR CODE */

/*
 *  Send general end of interrupt to external interrupt controller.
 *  The interrupt controller is a device external to the CPU, and
 *  mediates incoming interrupts.   Until the CPU "clears" the
 *  controller, no further external interrupts will be received.
 *  This "outp" command will clear the controller.
 */

  outp(0x20, 0x20);

  asm STI;   /* This will re-enable the interrupts */
} /*  Key_handler */
