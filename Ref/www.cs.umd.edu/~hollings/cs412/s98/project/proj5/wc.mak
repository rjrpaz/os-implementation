wc.mod: wc.obj io412.obj klib.obj c0grh.obj
    tlink /v c0grh wc io412 klib , wc.mod ,nul , mathh ch

klib.obj:   klib.c klib.h 
    bcc -v -c -mh -w-par -w-rvl klib.c

wc.obj: wc.c io412.h klib.h    
    bcc -v -c -mh -w-par -w-rvl wc.c

io412.obj:   io412.c io412.h
    bcc -v -c -mh io412.c
