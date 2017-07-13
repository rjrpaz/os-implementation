sampleC.exe: sampleC.obj  sampleS.obj
    bcc -mk sampleC.obj sampleS.obj

sampleC.obj: sampleC.c  
     bcc -c -mh sampleC.c

sampleS.obj: sampleS.asm
     tasm sampleS /MX


