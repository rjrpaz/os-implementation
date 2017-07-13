/****************************************************************/
/*
  This header file contains the scan codes and some ascii codes of
  keys on the keyboard.
*/
/****************************************************************/

#define LASTCOL    79   /* Last column on screen(numbered 0-79) */
#define LASTROW    24   /* Last row on screen(0-24) */
#define ESC 27     /* ASCII escape */

/* Scancodes for shift and capslock keys: */

#define LSHIFT_PRESSED 0x2A          /* Scan codes for shift press, */
#define LSHIFT_RELEASED 0xAA        /* shift release and capslock  */
#define RSHIFT_PRESSED 0x36        /* keys.                       */
#define RSHIFT_RELEASED 0xB6
#define CAPSLOCK 0x3A
#define CAPSLOCK_RELEASED 0xBA 

/* scancodes for arrow keys: */

#define CODE_UP    72
#define CODE_DOWN  80
#define CODE_LEFT  75
#define CODE_RIGHT 77

/* scancodes for function keys (for project 5) */

#define F1 0x3B
#define F2 0x3C

/* ASCII codes to use for arrow keys: */

#define ASC_UP    '\200'
#define ASC_DOWN  '\201'
#define ASC_LEFT  '\202'
#define ASC_RIGHT '\203'

#define BACKSPACE  8       /* Ascii codes for Backspace, Tab and enter */
#define TAB        9      /* keys.                                    */
#define ENTER_KEY  13

