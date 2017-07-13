# test3.mak

all: init.mod test3i.mod test3a.mod test3b.mod test3c.mod

init.mod:    test3i.mod
        copy test3i.mod init.mod

test3i.mod:  test3i.obj io412.obj klib.obj c0grh.obj
        tlink /v c0grh test3i io412 klib, test3i.mod, nul, mathh ch

test3i.obj:  test3i.c
        bcc -v -c -mh test3i.c

test3a.mod:  test3a.obj io412.obj klib.obj c0grh.obj
        tlink /v c0grh test3a io412 klib, test3a.mod, nul, mathh ch

test3a.obj:  test3a.c
        bcc -v -c -mh test3a.c

test3b.mod:  test3b.obj io412.obj klib.obj c0grh.obj
        tlink /v c0grh test3b io412 klib, test3b.mod, nul, mathh ch

test3b.obj:  test3b.c
        bcc -v -c -mh test3b.c

test3c.mod:  test3c.obj io412.obj klib.obj c0grh.obj
        tlink /v c0grh test3c io412 klib, test3c.mod, nul, mathh ch

test3c.obj:  test3c.c
        bcc -v -c -mh test3c.c

klib.obj:    klib.c klib.h kernel.h
        bcc -v -c -mh klib.c

io412.obj:   io412.c io412.h
        bcc -v -c -mh io412.c


