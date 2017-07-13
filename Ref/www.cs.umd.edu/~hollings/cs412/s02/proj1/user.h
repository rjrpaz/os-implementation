// A convenient data structure for describing a user program
struct User_Program {
    const char* name;

    unsigned int size;
    unsigned int dataSize;
    unsigned int programSize;

    unsigned int dataOffset;

    unsigned long entryAddr;
    const unsigned char*data;		/* pointer to start of init data */
    const unsigned char*program;	/* pointer to start of code in file */
    const unsigned char*executable;	/* pointer to entire executable file */
};

Boolean Copy_From_User( void* destInKernel, const void* srcInUser,
	unsigned long bufSize );
Boolean Copy_To_User( void* destInUser, const void* srcInKernel,
	unsigned long bufSize );
