/*
    CMSC412.H
    Header file for CMSC412.C
*/

/* Prototypes */

/* Initialized memory routines */
int Init_malloc(void);

/* Substitute for malloc */
void *Safe_malloc(int);

/* Substitute for free */
void Safe_free(void *);

/* Frees memory allocated by memory routines (call just before quitting) */
void Free_malloc(void);

/* A printf routine, which uses your putch, and the memory routines */
int Cprintf(char *str, ...);

/* Changes the state of the interrupt flag */
int Mod_int(int what);


