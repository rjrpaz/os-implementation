
/**********************************************************************

	    Project 5: Sample KLIB 


**********************************************************************/

/* Library Function Declaration */
typedef void (* procptr) ( int , char ** ) ;

int  		Proc_start ( procptr , int , char **, int , int , int ) ;
void 		Proc_term () ;
void 		Yield () ;
void 		P ( int ) ;
void 		V ( int ) ;
int  		Create_semaphore ( char* name, int ival) ;
void far * 	Kmalloc ( int ) ;
void 		Kfree ( void far * ) ;
procptr 	load_module ( char * ) ;
void 		close_module ( char * ) ;

/* message passing */
int 		MQ_Create(char *name);
int 		MQ_Send(int id, void *msg, int len);
int 		MQ_Receive(int id, void *msg, int len);
int		MQ_Destroy(int fd);

/* waiting for other process */
int 		waitpid(int pid);

/*********************************************************************/
/*                   End of File klib.h                              */
/*********************************************************************/
