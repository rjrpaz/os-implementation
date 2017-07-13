/*
    base412.H
    Header file for base412.C
*/

/* Offsets of .exe file header fields, in words */

#define EXEHEADERSEGS	4

/* Prototypes */

/* Initialized memory routines */
int Init_malloc(void);

/* Substitute for malloc */
void *Safe_malloc(int);

/* Substitute for free */
void Safe_free(void *);

/* Frees memory allocated by memory routines (call just before quitting) */
void Free_malloc(void);

/* Changes the state of the interrupt flag */
int Mod_int(int what);

/* File management functions */
int Open_exe(char *pathname, long *hsize);
int Read_exe(int fd, unsigned pos, unsigned len, void *buf);
int Close_exe(int fd);


