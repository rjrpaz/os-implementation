cat.mod: cat.obj io412.obj klib.obj c0grh.obj
    tlink /v c0grh cat io412 klib , cat.mod ,nul , mathh ch

klib.obj:   klib.c klib.h 
    bcc -v -c -mh -w-par -w-rvl klib.c

cat.obj: cat.c io412.h klib.h    
    bcc -v -c -mh -w-par -w-rvl cat.c

io412.obj:   io412.c io412.h
    bcc -v -c -mh io412.c
