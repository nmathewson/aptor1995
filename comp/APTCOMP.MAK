# APTCOMP :  COMPILER FOR APTOR

library=c:\bc\lib
include=c:\bc\include;c:\code\aptor\shared
shared=c:\code\aptor\shared\           #
#sw=-v -mc          # Debug
sw=-mc -G -O2      # Final

aptcomp.exe: aptcomp.obj intrface.obj parsing.obj crypt.obj nameref.obj \
             compile.obj
  bcc $(sw) -L$(library) aptcomp.obj crypt.obj intrface.obj parsing.obj \
      nameref.obj compile.obj
      
aptcomp.obj: aptcomp.cpp intrface.h parsing.h common.h compile.h
  bcc -c $(sw) -I$(include) aptcomp.cpp

crypt.obj: $(shared)crypt.cpp $(shared)crypt.h
  bcc -c $(sw) -I$(include) $(shared)crypt.cpp

intrface.obj: intrface.cpp intrface.h common.h
  bcc -c $(sw) -I$(include) intrface.cpp

parsing.obj: parsing.cpp parsing.h common.h intrface.h
  bcc -c $(sw) -I$(include) parsing.cpp

nameref.obj: nameref.cpp nameref.h intrface.h
  bcc -c $(sw) -I$(include) nameref.cpp

compile.obj: compile.cpp compile.h intrface.h parsing.h $(shared)crypt.h \
             nameref.h
  bcc -c $(sw) -I$(include) compile.cpp

