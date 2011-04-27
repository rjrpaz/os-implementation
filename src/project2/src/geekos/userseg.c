/*
 * Segmentation-based user mode implementation
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.23 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/ktypes.h>
#include <geekos/kassert.h>
#include <geekos/defs.h>
#include <geekos/mem.h>
#include <geekos/string.h>
#include <geekos/malloc.h>
#include <geekos/int.h>
#include <geekos/gdt.h>
#include <geekos/segment.h>
#include <geekos/tss.h>
#include <geekos/kthread.h>
#include <geekos/argblock.h>
#include <geekos/user.h>

/* ----------------------------------------------------------------------
 * Variables
 * ---------------------------------------------------------------------- */

//#define DEBUG 1
#undef DEBUG

#define DEFAULT_USER_STACK_SIZE 8192
#define EUSERCONTEXT    -20  /* Create_User_Context error */
#define ENOARGVPOINTER  -21  /* No se asgino puntero al bloque de argumentos */


void memDump(const void * src, size_t length);

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */


/*
 * Create a new user context of given size
 */

static struct User_Context* Create_User_Context(ulong_t size);

static bool Validate_User_Memory(struct User_Context* userContext,
    ulong_t userAddr, ulong_t bufSize)
{
    ulong_t avail;

    if (userAddr >= userContext->size)
        return false;

    avail = userContext->size - userAddr;
    if (bufSize > avail)
        return false;

    return true;
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Destroy a User_Context object, including all memory
 * and other resources allocated within it.
 */
void Destroy_User_Context(struct User_Context* userContext)
{
    /*
     * Hints:
     * - you need to free the memory allocated for the user process
     * - don't forget to free the segment descriptor allocated
     *   for the process's LDT
     */
//    TODO("Destroy a User_Context");

    /* you need to free the memory allocated for the user process */
    Free(userContext->memory);

    /*
     * don't forget to free the segment descriptor allocated
     * for the process's LDT
     */
    Free_Segment_Descriptor(userContext->ldtDescriptor);
    return;
}

/*
 * Load a user executable into memory by creating a User_Context
 * data structure.
 * Params:
 * exeFileData - a buffer containing the executable to load
 * exeFileLength - number of bytes in exeFileData
 * exeFormat - parsed ELF segment information describing how to
 *   load the executable's text and data segments, and the
 *   code entry point address
 * command - string containing the complete command to be executed:
 *   this should be used to create the argument block for the
 *   process
 * pUserContext - reference to the pointer where the User_Context
 *   should be stored
 *
 * Returns:
 *   0 if successful, or an error code (< 0) if unsuccessful
 */
int Load_User_Program(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat, const char *command,
    struct User_Context **pUserContext)
{
    unsigned long virtSize, argBlockSize, progSegmentSize;
    int i;
    ulong_t maxva = 0;
    unsigned int numArgs = 0, blockAddress = 0, stackAddress = 0;
    char *argBlock = NULL;

    /*
     * Hints:
     * - Determine where in memory each executable segment will be placed
     * - Determine size of argument block and where it memory it will
     *   be placed
     * - Copy each executable segment into memory
     * - Format argument block in memory
     * - In the created User_Context object, set code entry point
     *   address, argument block address, and initial kernel stack pointer
     *   address
     */

    /*
     * Determina dirección máxima de los segmentos del programa.
     * Inspirado del proyecto 1.
     */
    for (i = 0; i < exeFormat->numSegments; ++i) {
        struct Exe_Segment *segment = &exeFormat->segmentList[i];
        ulong_t topva = segment->startAddress + segment->sizeInMemory; 

        if (topva > maxva)
            maxva = topva;
    }

    /*
     * Determina tamaño del bloque de argumentos.
     */
    Get_Argument_Block_Size(command, &numArgs, &argBlockSize);
#ifdef DEBUG
    Print("Command: %s\n", command);
    Print("numArgs: %d\n", numArgs);
    Print("argBlockSize: %lu\n", argBlockSize);
#endif

    /*
     * Determina tamaño en memoria que ocupan: segmentos de
     * programa, bloques de argumento y stack.
     * Reserva memoria para almacenar todo.
     */
    progSegmentSize = Round_Up_To_Page(maxva);
    virtSize = progSegmentSize + Round_Up_To_Page(DEFAULT_USER_STACK_SIZE+argBlockSize);
    /* La reserva de memoria la hace en Create_User_Context */

    /*
     * Crea contexto de usuario.
     */
    *pUserContext = Create_User_Context(virtSize);
    if (!(pUserContext)) {
        Print("Create_User_Context error");
        return (EUSERCONTEXT);
    }

    /*
     * Copia los segmentos de programa en memoria.
     * Inspirado del proyecto 1.
     */
    for (i = 0; i < exeFormat->numSegments; ++i) {
        struct Exe_Segment *segment = &exeFormat->segmentList[i];
        memcpy((*pUserContext)->memory + segment->startAddress, exeFileData + segment->offsetInFile, segment->lengthInFile);
    }

    /*
     * - Format argument block in memory
     */
    blockAddress = progSegmentSize;
    stackAddress = virtSize;
#ifdef DEBUG
    Print("argBlock: %c\n", *argBlock);
    Print("argBlockSize: %lu\n", argBlockSize);
    // Print("Tamaño de 1 página: %lu\n", Round_Up_To_Page(1)); 4096 bytes = 2^12.
    // Print("Cantidad de segmentos de programa: %lu\n", Round_Up_To_Page(maxva)); // 4 Paginas
    // DEFAULT_USER_STACK_SIZE = 2 Paginas
    // Print("Cantidad de segmentos de argv y stack: %lu\n", Round_Up_To_Page(DEFAULT_USER_STACK_SIZE+argBlockSize)); // 3 Paginas
    Print("programAddress: %p\n", (*pUserContext)->memory);
    Print("blockAddress: %ul\n", blockAddress);
    Print("stackAddress: %ul\n", stackAddress);
#endif

    argBlock = (*pUserContext)->memory + progSegmentSize;
    Format_Argument_Block(argBlock, numArgs, blockAddress, command);
    if (argBlock == NULL) {
        Print("No se asigno puntero al bloque de argumentos.\n");
        return (ENOARGVPOINTER);
    }
#ifdef DEBUG
    memDump(argBlock, 25);
#endif

    /*
     * - In the created User_Context object, set code entry point
     *   address, argument block address, and initial kernel stack pointer
     *   address
     *  ______________________________________
     * | Seg. Programa        | Arg | Stack   |
     * |______________________|_____|_________|
     *
     * \---- N segmentos ----\\- 3 segmentos -\
     */
    (*pUserContext)->entryAddr = exeFormat->entryAddr;
    (*pUserContext)->argBlockAddr = blockAddress;
    (*pUserContext)->stackPointerAddr = stackAddress;

    return 0;
}

static struct User_Context* Create_User_Context(ulong_t size)
{
    int index = 0;
    struct User_Context *userContext = Malloc(sizeof(struct User_Context));

    /* Reserva memoria y blanquea contenido */
    userContext->memory = Malloc(size);
    memset(userContext->memory, '\0', size);
    userContext->size = size;

    /*
     * Crea una LDT para el proceso
     * "Inspirado en" ("Robado de") http://www.cs.umd.edu/~hollings/cs412/s02/hotNews/
     */
    /* Crea un local descriptor table para el proceso */
    userContext->ldtDescriptor = Allocate_Segment_Descriptor();
    /*
     * Inicializa la LDT en la GDT, o sea, agrega un descriptor en la GDT que
     * apunta a la ubicación del descriptor de la LDT dentro de la GDT.
     */
    Init_LDT_Descriptor(userContext->ldtDescriptor, userContext->ldt, NUM_USER_LDT_ENTRIES);
    /* Crea un selector que contiene la ubicación del LDT dentro de la GDT */
    index = Get_Descriptor_Index(userContext->ldtDescriptor);
    userContext->ldtSelector = Selector(KERNEL_PRIVILEGE, true, index);

    /*
     * Crea descriptores para los segmentos de código y datos del programa,
     * y agrega estos descriptores a la LDT. ¿Cantidad de páginas del segmento?
     */
    Init_Code_Segment_Descriptor(&userContext->ldt[0], (ulong_t)userContext->memory, size/PAGE_SIZE, USER_PRIVILEGE);
    Init_Data_Segment_Descriptor(&userContext->ldt[1], (ulong_t)userContext->memory, size/PAGE_SIZE, USER_PRIVILEGE);

    /*
     * Crea selectores que contienen la ubicación de los dos descriptores
     * dentro de la LDT.
     */
    userContext->csSelector = Selector(USER_PRIVILEGE, false, 0);
    userContext->dsSelector = Selector(USER_PRIVILEGE, false, 1);

    /* ¿refCount? */
    userContext->refCount = 0;
    return userContext;
}

/*
 * Copy data from user memory into a kernel buffer.
 * Params:
 * destInKernel - address of kernel buffer
 * srcInUser - address of user buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */
bool Copy_From_User(void* destInKernel, ulong_t srcInUser, ulong_t bufSize)
{
    /*
     * Hints:
     * - the User_Context of the current process can be found
     *   from g_currentThread->userContext
     * - the user address is an index relative to the chunk
     *   of memory you allocated for it
     * - make sure the user buffer lies entirely in memory belonging
     *   to the process
     */

    struct User_Context *userContext = g_currentThread->userContext;
    if (Validate_User_Memory(userContext, srcInUser, bufSize)) {
        memcpy(destInKernel, (userContext->memory)+srcInUser, bufSize);
        return true;
    } else {
        return false;
    }
}

/*
 * Copy data from kernel memory into a user buffer.
 * Params:
 * destInUser - address of user buffer
 * srcInKernel - address of kernel buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */
bool Copy_To_User(ulong_t destInUser, void* srcInKernel, ulong_t bufSize)
{
    /*
     * Hints: same as for Copy_From_User()
     */

    struct User_Context *userContext = g_currentThread->userContext;
    if (Validate_User_Memory(userContext, destInUser, bufSize)) {
        memcpy((userContext->memory)+destInUser, srcInKernel, bufSize);
        return true;
    } else {
        return false;
    }
}

/*
 * Switch to user address space belonging to given
 * User_Context object.
 * Params:
 * userContext - the User_Context
 */
void Switch_To_Address_Space(struct User_Context *userContext)
{
    /*
     * Hint: you will need to use the lldt assembly language instruction
     * to load the process's LDT by specifying its LDT selector.
     */
    __asm__ __volatile__ ("lldt %0" :: "a" (userContext->ldtSelector));
}

void memDump(const void * src, size_t length)
{
    char* address = (char*)src;
    int i = 0; //used to keep track of line lengths
    char *line = (char*)address; //used to print char version of data
    unsigned char ch; // also used to print char version of data

    Print("%p| ", address); //Print the address we are pulling from
    while (length-- > 0) {
        Print("%02X ", (unsigned char)*address++); //Print each char
        if (!(++i % 16) || (length == 0 && i % 16)) { //If we come to the end of a line...
            //If this is the last line, print some fillers.
            if (length == 0) { while (i++ % 16) { Print("__ "); } }
            Print("| ");
            while (line < address) {  // Print the character version
                ch = *line++;
                Print("%c", (ch < 33 || ch == 255) ? 0x2E : ch);
            }
            // If we are not on the last line, prefix the next line with the address.
            if (length > 0) { Print("\n%p| ", address); }
        }
    }
    Print("\n");
}
