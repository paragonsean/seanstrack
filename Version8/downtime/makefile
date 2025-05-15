OPS = -c -v -tWM -WE

downtime.exe : downtime.obj category.obj downtime.res
  TLink32 @&&|
   -Tpe -aa -c -x +
\APPS\BC5\LIB\c0w32.obj+
downtime.obj+
category.obj
downtime.exe

..\lib\v5subs.lib+
\APPS\BC5\LIB\import32.lib+
\APPS\BC5\LIB\cw32mt.lib

|
   brc32 downtime.res downtime.exe
   copy downtime.exe \v5\exes\downtime.exe

downtime.obj :  downtime.cpp
  Bcc32 $(OPS) downtime.cpp

category.obj :  category.cpp
  Bcc32 $(OPS) category.cpp

downtime.res : downtime.rc
   brcc32 downtime.rc

