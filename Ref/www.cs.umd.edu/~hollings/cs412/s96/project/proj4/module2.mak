module2.mod: module2.obj io412.obj screen.obj klib.obj c0grh.obj
    tlink /v c0grh module2 io412 klib , module2.mod ,nul , mathh ch

klib.obj:   klib.c klib.h queue.h kernel.h semaphor.h
    bcc -v -c -mh -w-par -w-rvl klib.c

module2.obj: module2.c queue.h io412.h kernel.h klib.h    
    bcc -v -c -mh -w-par -w-rvl module2.c

io412.obj:   io412.c io412.h
    bcc -v -c -mh io412.c

screen.obj: screen.c screen.h
    bcc -v -c -mh screen.c


