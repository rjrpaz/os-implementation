# Unix/Windows makefile for GeekOS
# Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
# $Revision: 1.2.2.1 $

# This is free software.  You are permitted to use,
# redistribute, and modify it as specified in the file "COPYING".

# This makefile works (well, is intended to work) on both
# Unix (including Linux) and Windows platforms.  On Windows,
# you need a recent version of the cygwin toolkit, available
# at
#   http://sources.redhat.com/cygwin/

# On any platform, you need gcc/binutils with an i386 target
# (http://gcc.gnu.org/), Nasm (http://www.web-sites.co.uk/nasm/),
# and perl (http://www.perl.com/).

# GNU make is required for this makefile.

# ----------------------------------------------------------------------
# Configuration stuff
# ----------------------------------------------------------------------

# Try to figure out if we're compiling on Unix or Windows.
# You may want to override this.
ifeq ($(OS),Windows_NT)
OS := Windows
else
OS := Unix
endif

# Uncomment this if you're using the i386-elf cross compiler and
# binutils.
#CC_PREFIX := i386-elf-

# In theory, you should not need to modify anything beyond this point...

# ----------------------------------------------------------------------
# Tools and definitions
# ----------------------------------------------------------------------

# The C compiler used to compile the kernel.
CC := $(CC_PREFIX)gcc

# Compiler flags.
ifeq ($(OS),Windows)
EXTRA_CFLAGS := -DNEED_UNDERSCORE -DGNU_WIN32
endif
CFLAGS := -O -fno-builtin -Wall -Werror -DGEEKOS $(EXTRA_CFLAGS)
#CFLAGS := -Wall -Werror -DGEEKOS $(EXTRA_CFLAGS)

# GNU linker and GNU objcopy.  These are provided with the
# cygwin tools.
LD := $(CC_PREFIX)ld
OBJCOPY := $(CC_PREFIX)objcopy

# Objcopy flags.
# On Unix/ELF we get rid of some unnecessary sections.
ifeq ($(OS),Unix)
OBJCOPY_FLAGS := -R .dynamic -R .note -R .comment
endif

# Nasm.  I'm using version 0.98
ifeq ($(OS),Windows)
NASM := nasmw
NASMFLAGS := -f win32 -DNEED_UNDERSCORE
else
NASM := nasm
NASMFLAGS := -f elf
endif

# Kernel source and object files.
C_SRCS := screen.c idt.c int.c irq.c io.c keyboard.c floppy.c ide.c pfat.c mem.c string.c \
	gdt.c kthread.c thrqueue.c timer.c segment.c tss.c bget.c malloc.c \
	trap.c main.c elf.c 
ASM_SRCS := lowlevel.asm
OBJS := $(C_SRCS:.c=.o) $(ASM_SRCS:.asm=.o)

# Perl (needs to be version 5 or later).
PERL = perl

# Utility scripts
PAD := $(PERL) pad
NUMSECS := $(PERL) numsecs
ZEROFILE := $(PERL) zerofile
CAT := $(PERL) pcat
RM :=  rm -f

# Kernel entry point.
ifeq ($(OS),Windows)
ENTRY := _Main
else
ENTRY = Main
endif

# ----------------------------------------------------------------------
# Compilation rules
# ----------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .c .asm .o

.c.o:
	$(CC) $(CFLAGS) -c $<

.asm.o:
	$(NASM) $(NASMFLAGS) -o $*.o $<


# ----------------------------------------------------------------------
# Targets
# ----------------------------------------------------------------------

# Default target.
# Create floppy and hard drive image files needed to
# start bochs.
all : fd.img hd.img
	(cd buildFat; gmake)
	(cd userProgs; gmake)

fd.img : bootsect.bin setup.bin kernel.bin
	$(ZEROFILE) $@ 1024

# Create a nice 2MB hard disk for bochs to use.
hd.img :
	$(ZEROFILE) $@ 4096

# Create a nice 2MB hard disk for bochs to use.
diskd.img :
	$(ZEROFILE) $@ 4096

# Setup code (16 bit real mode), loaded by the bootsector.
# Its purpose is to set up the hardware sufficiently such that
# the kernel can start running.  Note that the kernel storage map
# is a prerequisite, since the setup code needs to know the address
# of the kernel entry point.
setup.bin : setup.asm defs.asm util.asm storage.txt
	$(NASM) -DENTRY_POINT=`perl findaddr storage.txt Main` \
		-f bin -o $@ setup.asm
	$(PAD) $@ 512

# Create a binary image of the kernel code and data.
# The file produced can be loaded into memory as-is and executed.
# The -S option stips out the symbol table.
kernel.bin: kernel.exe
	$(OBJCOPY) $(OBJCOPY_FLAGS) -S -O binary kernel.exe kernel.bin
	$(CC_PREFIX)nm kernel.exe > kernel.syms
	$(PAD) $@ 512

# Link the kernel.  Put the .text section (which is the first
# section in the executable) at address 0x10000 (64K).
kernel.exe storage.txt : $(OBJS)
	$(LD) -M -o kernel.exe -Ttext 0x00010000 -e $(ENTRY) $(OBJS) \
		> storage.txt

# Assemble the boot sector.
bootsect.bin : setup.bin kernel.bin bootsect.asm defs.asm util.asm
	$(NASM) -DNUM_SETUP_SECTORS=`$(NUMSECS) setup.bin` \
		-DNUM_KERN_SECTORS=`$(NUMSECS) kernel.bin` \
		-DDRIVE_NUMBER=0                        \
		-f bin -o $@ bootsect.asm

# Rules for generating user programs which live in the "test" directory.

dummy :

# Delete generated files.
clean :
	$(RM) *.img kernel.exe kernel.bin bootsect.bin setup.bin *.o \
		storage.txt bochs.out
	(cd buildFat; gmake clean) 
	(cd userProgs; gmake clean)

realclean : clean
	$(RM) hd.img depend.mak

# Generate header file dependencies.
depend : 
	$(CC) $(CFLAGS) -M $(C_SRCS) > depend.mak

depend.mak :
	echo dummy: > $@

include depend.mak
