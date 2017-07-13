init.mod: init.obj io412.obj screen.obj klib.obj c0grh.obj
    tlink /v c0grh init io412 klib , init.mod ,nul , mathh ch

c0grh.obj:  c0grh.asm
    tasm /MX c0grh.asm

klib.obj:   klib.c klib.h queue.h kernel.h semaphor.h
    bcc -v -c -mh -w-par -w-rvl klib.c

init.obj: init.c queue.h io412.h kernel.h klib.h    
    bcc -v -c -mh -w-par -w-rvl init.c

io412.obj:   io412.c io412.h
    bcc -v -c -mh io412.c

screen.obj: screen.c screen.h
    bcc -v -c -mh screen.c


