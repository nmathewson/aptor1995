# APTDUMP :  DEBUGGER FOR APTOR

library=c:\bc\lib
include=c:\bc\include
#sw=-v -mc          # Debug
sw=-mc -G -O2      # Final

aptdump.exe: aptdump.obj nameref2.obj aptor.obj crypt.obj
  bcc $(sw) -L$(library) aptdump.obj crypt.obj nameref2.obj aptor.obj
      
aptdump.obj: aptdump.cpp aptor.h nameref2.h crypt.h
  bcc -c $(sw) -I$(include) aptdump.cpp

crypt.obj: crypt.cpp crypt.h
  bcc -c $(sw) -I$(include) crypt.cpp

nameref2.obj: nameref2.cpp nameref2.h
  bcc -c $(sw) -I$(include) nameref2.cpp

aptor.obj: aptor.cpp aptor.h crypt.h nameref2.h
  bcc -c $(sw) -I$(include) aptor.cpp

