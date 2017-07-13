tee.mod: tee.obj io412.obj klib.obj c0grh.obj
    tlink /v c0grh tee io412 klib , tee.mod ,nul , mathh ch

klib.obj:   klib.c klib.h 
    bcc -v -c -mh -w-par -w-rvl klib.c

tee.obj: tee.c io412.h klib.h    
    bcc -v -c -mh -w-par -w-rvl tee.c

io412.obj:   io412.c io412.h
    bcc -v -c -mh io412.c
