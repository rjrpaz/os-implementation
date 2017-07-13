all: init.mod test0a.mod test0b.mod

init.mod: test0i.obj io412.obj screen.obj klib.obj c0grh.obj
    tlink /v c0grh test0i io412 klib , init.mod ,nul , mathh ch

# c0grh.obj:  c0grh.asm
#     tasm /MX c0grh.asm

klib.obj:   klib.c klib.h queue.h kernel.h # semaphor.h
    bcc -v -c -mh -w-par -w-rvl klib.c

test0i.obj: test0i.c queue.h io412.h kernel.h klib.h    
    bcc -v -c -mh -w-par -w-rvl test0i.c

io412.obj:   io412.c io412.h
    bcc -v -c -mh io412.c

screen.obj: screen.c screen.h
    bcc -v -c -mh screen.c

test0a.mod: test0a.obj io412.obj screen.obj klib.obj c0grh.obj
    tlink /v c0grh test0a io412 klib , test0a.mod ,nul , mathh ch

test0a.obj: test0a.c queue.h io412.h kernel.h klib.h    
    bcc -v -c -mh -w-par -w-rvl test0a.c

test0b.mod: test0b.obj io412.obj screen.obj klib.obj c0grh.obj
    tlink /v c0grh test0b io412 klib , test0b.mod ,nul , mathh ch

test0b.obj: test0b.c queue.h io412.h kernel.h klib.h    
    bcc -v -c -mh -w-par -w-rvl test0b.c
