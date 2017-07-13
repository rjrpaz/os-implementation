/*
 * ELF executable loading
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.29 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/kassert.h>
#include <geekos/ktypes.h>
#include <geekos/screen.h>  /* for debug Print() statements */
#include <geekos/pfat.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/user.h>
#include <geekos/elf.h>


/**
 * From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 *   and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength, struct Exe_Format *exeFormat)
{
//    TODO("Parse an ELF executable image");
	
	int i;

	//check wheater file has ELF format or not
	if(strncmp(exeFileData, "\x7f""ELF", 4) != 0)
	{
		Print("\nFormat of file to load does not match ELF format!!!\n");
		return ENOEXEC;		//EN0EXEC...invalid executeable format (see file errno.h)
	}


	//casting the exeFileData to (elfHeader*)
	//so the pointer elf_header points to the first bytes of exeFileData where the ELF Header is located
	elfHeader* elf_header = (elfHeader*) exeFileData;
	
	
	//output for DEBUG use only
/*
	for(i=0; i<16; i++)
	{
		Print("%02x\n",elf_header->ident[i]);
	}
	Print("  \n");
	Print("type: %hd\n",elf_header->type);
	Print("machine: %hd\n",elf_header->machine);
	Print("version: %d\n",elf_header->version);
	Print("entry: %d\n",elf_header->entry);
	Print("phoff: %d\n",elf_header->phoff);
	Print("sphoff: %d\n",elf_header->sphoff);
	Print("flags: %d\n",elf_header->flags);
	Print("ehsize: %hd\n",elf_header->ehsize); 
	Print("phentsize: %hd\n",elf_header->phentsize); 
	Print("phnum: %hd\n",elf_header->phnum);
	Print("shentsize: %hd\n",elf_header->shentsize); 
	Print("shnum: %hd\n",elf_header->shnum); 
	Print("shstrndx: %hd\n",elf_header->shstrndx); 
*/
	
	//set Number of Segments included in the ELF File
	exeFormat->numSegments = elf_header->phnum;
	//set Entry Address
	exeFormat->entryAddr = elf_header->entry;

	//creating the programHeader
	//elf_header->phoff ... Offset des Programmheaders in Byte
	programHeader* prog_header;
	prog_header = (programHeader*) (exeFileData + elf_header->phoff);


	for(i=0; i < exeFormat->numSegments; i++, prog_header++)
	{
		exeFormat->segmentList[i].offsetInFile = prog_header->offset;	 		/* Offset of segment in executable file */
		exeFormat->segmentList[i].lengthInFile = prog_header->fileSize;		/* Length of segment data in executable file */
		exeFormat->segmentList[i].startAddress = prog_header->vaddr;	 	/* Start address of segment in user memory */
		exeFormat->segmentList[i].sizeInMemory = prog_header->memSize;	/* Size of segment in memory */
		exeFormat->segmentList[i].protFlags = prog_header->flags;		 		/* VM protection flags; combination of VM_READ,VM_WRITE,VM_EXEC */
	}

	return 0;
}

