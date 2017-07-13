# test1.mak

all: init.mod test1i.mod test1a.mod

init.mod:    test1i.mod
        copy test1i.mod init.mod

test1i.mod:  test1i.obj io412.obj klib.obj c0grh.obj
        tlink /v c0grh test1i io412 klib, test1i.mod, nul, mathh ch

test1a.mod:  test1a.obj io412.obj klib.obj c0grh.obj
        tlink /v c0grh test1a io412 klib, test1a.mod, nul, mathh ch

test1i.obj:  test1i.c
        bcc -v -c -mh test1i.c

test1a.obj:  test1a.c
        bcc -v -c -mh test1a.c

klib.obj:    klib.c klib.h kernel.h
        bcc -v -c -mh klib.c

io412.obj:   io412.c io412.h
        bcc -v -c -mh io412.c


