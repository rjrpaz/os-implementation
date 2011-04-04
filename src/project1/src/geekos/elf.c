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
#include <geekos/elf.h>

//#define DEBUG 1
#undef DEBUG
#define EV_CURRENT 1

/**
 * From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 *   and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat)
{
    unsigned int i = 0;
    ulong_t progtable = 0;
    elfHeader *cabecera = (elfHeader *) exeFileData;

#ifdef DEBUG
    Print("\n\n\n");
    Print("Tama#o del ejecutable: %ld\n", exeFileLength);
#endif

    /* Chequeo que exeFileData no sea NULL */
    if (exeFileData == NULL) {
        Print("Binario no es valido.");
        return -1;
    }

    /* Chequeo de integridad del ejecutable */
    if (((cabecera->ident[0]) != 0x7f) || ((cabecera->ident[1]) != 'E') || ((cabecera->ident[2]) != 'L') || ((cabecera->ident[3]) != 'F')) {
        Print("El archivo binario no cumple con el formato adecuado.");
        return -1;
    }

    if (!(((cabecera->ident[4]) >= 0) && ((cabecera->ident[4]) <= 2))) {
        Print("Clase de archivo erroneo.");
        return -1;
    }
#ifdef DEBUG
    Print("Clase de archivo: ");
    switch(cabecera->ident[4]) {
        case 2: Print("ELFCLASS64 (2)"); break;
        case 1: Print("ELFCLASS32 (1)"); break;
        default: Print("ELFCLASSNONE (0)"); break;
    }
    Print("\n");
#endif

    if (! (((cabecera->ident[5]) >= 0) && ((cabecera->ident[5]) <= 2))) {
        Print("Codificacion de datos erronea.");
        return -1;
    }
#ifdef DEBUG
    Print("Tipo de codificacion de los datos: ");
    switch(cabecera->ident[5]) {
        case 2: Print("ELFDATA2MSB (2)"); break;
        case 1: Print("ELFDATA2LSB (1)"); break;
        default: Print("ELFDATANONE (0)"); break;
    }
    Print("\n");
#endif

    if (!((cabecera->ident[6]) == EV_CURRENT)) {
        Print("Version de elf no soportado o erroneo.");
        return -1;
    }

    /*
     * No se si este chequeo es adecuado, ya que el kernel debería
     * cargar otro tipo de objetos y no solo ejecutables, aunque para
     * el caso de esta función en particular, si correponda el chequeo.
     */
    if (!((cabecera->type) == 2)) {
        Print("El archivo debe ser ejecutable.");
        return -1;
    }

    if (!(((cabecera->machine) >= 0) && ((cabecera->machine) <= 8) && ((cabecera->machine) != 6))) {
        Print("Archivo del archivo no reconocida.");
        return -1;
    }

#ifdef DEBUG
    Print("Arquitectura: ");
    switch(cabecera->machine) {
        case 1: Print("AT&T (1)"); break;
        case 2: Print("Sparc (2)"); break;
        case 3: Print("Intel 386 (3)"); break;
        case 4: Print("Motorola 68000 (4)"); break;
        case 5: Print("Motorola 88000 (5)"); break;
        case 7: Print("Intel 80860 (7)"); break;
        case 8: Print("MIPS (8)"); break;
        default: Print("No Machine (0)"); break;
    }
    Print("\n");
#endif

    if (!((cabecera->version) == EV_CURRENT)) {
        Print("Version de elf no soportado o erroneo.");
        return -1;
    }

    if (!(cabecera->machine <= EXE_MAX_SEGMENTS)) {
        Print("Cantidad de segmentos no es valida.");
        return -1;
    }

#ifdef DEBUG
    Print("e_entry: 0x%x\n", cabecera->entry);
    Print("e_phoff: %ld\n", cabecera->phoff);
    Print("e_shoff: %ld\n", cabecera->sphoff);
    Print("e_flags: 0x%x\n", cabecera->flags);
    Print("Tama#o de la cabecera: %d\n", cabecera->ehsize);
    Print("Tama#o de una entrada en la tabla de la cabecera: %d\n", cabecera->phentsize);
    Print("Cantidad de entradas en la tabla de la cabecera: %d. Tama#o de la tabla: %d\n", cabecera->phnum, (cabecera->phentsize) * (cabecera->phnum));
    Print("Tama#o de una cabecera de seccion: %d\n", cabecera->shentsize);
    Print("Cantidad de entradas en la tabla de cabecera de secciones: %d. Tama#o de la tabla: %d\n", cabecera->shnum, (cabecera->shentsize) * (cabecera->shnum));
    Print("Indice de la tabla de cabecera de seccion: %d\n", cabecera->shstrndx);
#endif

    /* Completo parte de la salida */
    exeFormat->numSegments = cabecera->phnum;
    exeFormat->entryAddr = cabecera->entry;
#ifdef DEBUG
    Print("Punto de entrada del codigo: 0x%x\n", exeFormat->entryAddr);
    Print("Cantidad de segmentos que contiene el ejecutable: %d\n", exeFormat->numSegments);
#endif

    /* Completo la lista de segmentos */
#ifdef DEBUG
    Print("Datos de los segmentos:\n");
    Print("\tOffset\tLargo\tDirMem\tTamMem\tFlags\n");
#endif
    progtable = cabecera->phoff;
    for (i=0; i<(exeFormat->numSegments); i++) {
        memcpy(&exeFormat->segmentList[i].offsetInFile, exeFileData + progtable + 4, 4);
        memcpy(&exeFormat->segmentList[i].lengthInFile, exeFileData + progtable + 16, 4);
        memcpy(&exeFormat->segmentList[i].startAddress, exeFileData + progtable + 12, 4);
        memcpy(&exeFormat->segmentList[i].sizeInMemory, exeFileData + progtable + 20, 4);
        memcpy(&exeFormat->segmentList[i].protFlags, exeFileData + progtable + 24, 4);

#ifdef DEBUG
    Print("\t0x%x\t0x%x\t0x%x\t0x%x\t%d\n", exeFormat->segmentList[i].offsetInFile, exeFormat->segmentList[i].lengthInFile, exeFormat->segmentList[i].startAddress, exeFormat->segmentList[i].sizeInMemory, exeFormat->segmentList[i].protFlags);
#endif
        progtable += (cabecera->phentsize);
    }


#ifdef DEBUG
    Print("\n\n\n");
#endif

    return 0;
}

