# test2.mak

all: init.mod test2i.mod test2a.mod

init.mod:    test2i.mod
        copy test2i.mod init.mod

test2i.mod:  test2i.obj io412.obj klib.obj c0grh.obj
        tlink /v c0grh test2i io412 klib, test2i.mod, nul, mathh ch

test2a.mod:  test2a.obj io412.obj klib.obj c0grh.obj
        tlink /v c0grh test2a io412 klib, test2a.mod, nul, mathh ch

test2i.obj:  test2i.c
        bcc -v -c -mh test2i.c

test2a.obj:  test2a.c
        bcc -v -c -mh test2a.c

klib.obj:    klib.c klib.h kernel.h
        bcc -v -c -mh klib.c

io412.obj:   io412.c io412.h
        bcc -v -c -mh io412.c


