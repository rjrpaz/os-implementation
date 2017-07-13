proj2.exe:   kernel.obj proc.obj keyboard.obj \
             queue.obj screen.obj main.obj cmsc412.obj
    tlink /v c0h main kernel proc keyboard queue screen cmsc412,proj2,proj2,\
             emu mathh ch

keyboard.obj: keyboard.c keyboard.h queue.h cmsc412.h codes.h
    bcc -v -c -mh keyboard.c

queue.obj: queue.c screen.h keyboard.h queue.h cmsc412.h codes.h     
    bcc -v -c -mh queue.c

kernel.obj: kernel.c kernel.h queue.h cmsc412.h 
    bcc -v -c -mh kernel.c

proc.obj: proc.c cmsc412.h
    bcc -v -c -mh  proc.c

screen.obj: screen.c screen.h keyboard.h queue.h cmsc412.h codes.h     
    bcc -v -c -mh screen.c

main.obj: main.c screen.h keyboard.h queue.h cmsc412.h kernel.h    
    bcc -v -c -mh main.c

cmsc412.obj: cmsc412.c cmsc412.h codes.h     
    bcc -v -c -mh cmsc412.c



