# Makefile for usermode programs


CC = $(CC_PREFIX)gcc
CFLAGS = -Wall -fno-builtin

LD = $(CC_PREFIX)ld
OBJCOPY = $(CC_PREFIX)objcopy
RANLIB = $(CC_PREFIX)ranlib
MKUPROG = perl ../mkuprog

# FIXME: need better way to figure out unix vs. windows
ifeq ($(OS),Windows_NT)
ENTRY = __Entry
else
ENTRY = Entry
endif

KERN_DIR = ..

PROGS = shell.exe semtest.exe setsched.exe ping.exe pong.exe long.exe workload.exe

all: $(PROGS)
	$(KERN_DIR)/buildFat/buildFat $(KERN_DIR)/hd.img $(PROGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.exe : %.o libuser.a
	$(LD) -o $@ -Ttext 0x0 -e $(ENTRY) $*.o libuser.a

libuser.a : atoi.o libio.o ../string.o libuser.o
	ar ruv $@ libuser.o atoi.o libio.o ../string.o
	$(RANLIB) $@ || true

../string.o : dummy
	cd .. && $(MAKE) string.o

clean :
	rm -f *.exe *.o *.a *.bin $(PROGS)

dummy:
