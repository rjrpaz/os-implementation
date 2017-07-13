module3.mod: module3.obj io412.obj screen.obj klib.obj c0grh.obj
    tlink /v c0grh module3 io412 klib , module3.mod ,nul , mathh ch

klib.obj:   klib.c klib.h queue.h kernel.h semaphor.h
    bcc -v -c -mh -w-par -w-rvl klib.c

module3.obj: module3.c queue.h io412.h kernel.h klib.h    
    bcc -v -c -mh -w-par -w-rvl module3.c

io412.obj:   io412.c io412.h
    bcc -v -c -mh io412.c

screen.obj: screen.c screen.h
    bcc -v -c -mh screen.c


