/****************************************************************************

                      NAME

   This demo program consists of three parts:

   part 1 -  Inputs a string and an integer, then passes them to the
          assembly routine 'trnsfr'. 'trnsfr' will transfer the variables
          to another 'C' routine 'echo' that echoes them on the screen.

   part 2 -  Stuffs the interrupt vector for a 60h interrupt with 'asctab'
          which will access a table defined in the assembly code.

   part 3 -  A pointer to a 4-character string is passed to the assembly
          routine 'rvrse', which will reorganize the string.

                 TABLE OF CONTENTS:

   main()
   echo()
   asctab()
   response()
************************************************************************** 

#include "dos.h"
#include <conio.h>
#include <stdio.h>

/* *********************************************************************** 
 * define protoype for assembly routines: trnsfr and rvrse
   *********************************************************************** */
extern void trnsfr(int, char *);
extern void rvrse(char *);

/***************************************************************************
*  PROCEDURE echo is used to test the assembler-'C' parameter passing.     *
*            Echo prints out the passed parameters and exits.              *
*                                                                          *
*  calls:                                                                  *
*    functions : none                                                      *
*    procedures: none                                                      *
*  called by:                                                              *
*    functions : none                                                      *
*    procedures: trnsfr (assembler routine)                                *
*  parameters:                                                             *
*             i => an integer used for nothing                             *
*             a => a string    "    "     "                                *
*  variables:                                                       *
*    none                                                                *
***************************************************************************/
void echo (int i, char *a)
{
  cprintf ("Hi, this is echo. \n\r");
  cprintf ("The first argument is an integer '%d'. \n\r",i);
  cprintf ("The second argument is a string  '%s'. \n\r",a);
} /* end procedure echo */

/* *********************************************************************** */

/***************************************************************************
*  INTERRUPT asctab prints out a table of ASCII codes supplied by the      *
*         assembly file.                                            *
*                                                                          *
*  calls:                                                                  *
*    functions : none                                                      *
*    procedures: none                                                      *
*  called by: interrupt vector 0x60                                        *
*    functions : none                                                      *
*    procedures: none                                                      *
*  parameters:                                                             *
*       none                                                               *
*  variables:                                                       *
*    none                                                                *
***************************************************************************/
void interrupt asctab ()
{
extern char asccode[6][6]; /* asccode is written in the assembly routine */
int i,j;            /* loop variables used to print out the table */

  cprintf ("\n\r MAGIC TABLE: \n\r");
  for (i=0;i<=5;i++)
  {
    for (j=0;j<=5;j++)
    {
      putch(' ');           /* 'putch' sends one character to stdout
                         (normally defined to be the console) */
      if (asccode[i][j] != NULL)/* if the character is '<NULL>' print a ' '*/
     putch(asccode[i][j]);
      else
     putch(0x20);
    } /* end 'for j' loop */
    putch(13);  /* carriage return */
    putch(10);  /* linefeed */
  } /* end 'for i' loop */
} /* end procedure asctab */

/* *********************************************************************** */

/***************************************************************************
*  PROCEDURE response prints the given message on the console and waits    *
*         for the user to press the escape key.                    *
*                                                                          *
*  calls:                                                                  *
*    functions : none                                                      *
*    procedures: none                                                      *
*  called by:                                                              *
*    functions : none                                                      *
*    procedures: main                                                      *
*  parameters:                                                             *
*        partno => the message to be printed                               *
*  variables:                                                       *
*    none                                                                *
***************************************************************************/
void response (char *partno)
{
  cprintf ("\n\r press <Esc> to continue %s",partno);
  while (getch() != 0x1b); /* wait for escape keyin */
  cprintf ("\n\r");
} /* end procedure response */


main()
{

char a[80]; /* input buffer for 'echo'; holds up to 80 characters */
int i;      /* the integer input used in 'echo' */
static char str_4[5] = {'A','a','B','b'}; /* input for 'rvrse' */

static void interrupt (*old60h)();   /* saves the system interrupt vector */
void interrupt asctab();             /* predefinition of our interrupt */

  /*  PART 1  */

  cprintf ("\n\r ** Part 1 **");
  cprintf ("\n\r ** Enter a string: ");
  scanf ("%79s",a);  /* NB: remember, last location in buffer is reserved
                  for the end-of-string */
  cprintf ("\n\r ** Enter a number: ");
  scanf ("%d",&i);   /* NB: scanf uses the ADDRESS of the integer */
  cprintf ("\n\r\n");
  trnsfr(i,a);       /* pass i and a to C subroutine "echo" thru trnsfr */
  cprintf ("** transfer done **\n\r");

  /*  PART 2  */

  response("part 2");
  cprintf ("\n\r** Part 2 **\n\r");
  old60h = getvect(x60); /* save original system vector for int 60h */
  setvect(0x60,asctab);   /* tell BIOS to vector to subroutine 'asctab' when
                      an interrupt valued at 60h occurs */
  geninterrupt(0x60);  /* execute asctab by causing appropriate interrupt */
  setvect(0x60,old60h); /* restore original system vector location */
  cprintf ("\n\r ** end of table **\n\r");


  /*  PART 3  */

  response("part 3");
  cprintf ("\n\r** Part 3 ** \n\r");
  cprintf ("\n\r Old 4_string was '%3s';",str_4);
  rvrse (str_4);    /* reverse the order of the words in the string */
  cprintf ("\n\r new 4_string is '%3s'. ",str_4);
  cprintf ("\n\r\n\r ** End of sample program **");
} /* end MAIN */

