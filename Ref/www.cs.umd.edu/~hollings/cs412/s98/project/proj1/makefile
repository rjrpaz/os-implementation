proj1.exe:   keyboard.obj queue.obj screen.obj main.obj cmsc412.obj
    tlink /v c0h main keyboard queue screen cmsc412,proj1,proj1,\
             emu mathh ch

keyboard.obj: keyboard.c keyboard.h queue.h cmsc412.h codes.h
    bcc -v -c -mh keyboard.c

queue.obj: queue.c screen.h keyboard.h queue.h cmsc412.h codes.h     
    bcc -v -c -mh queue.c

screen.obj: screen.c screen.h keyboard.h queue.h cmsc412.h codes.h     
    bcc -v -c -mh screen.c

main.obj: main.c screen.h keyboard.h queue.h cmsc412.h codes.h     
    bcc -v -c -mh main.c

cmsc412.obj: cmsc412.c cmsc412.h codes.h     
    bcc -v -c -mh cmsc412.c



