# test4.mak

all: init.mod test4i.mod 

init.mod:    test4i.mod
        copy test4i.mod init.mod

test4i.mod:  test4i.obj io412.obj klib.obj c0grh.obj
        tlink /v c0grh test4i io412 klib, test4i.mod, nul, mathh ch

test4i.obj:  test4i.c
        bcc -v -c -mh test4i.c

klib.obj:    klib.c klib.h kernel.h
        bcc -v -c -mh klib.c

io412.obj:   io412.c io412.h
        bcc -v -c -mh io412.c


