CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/dsrestor.exe"

RSC_PROJ=/l 0x409 /fo"free\dsrestor.res" /d "NDEBUG"

free\dsrestor.exe : free\dsrestor.obj free\proclass.obj free\dsrestor.res
    link $(LINK32_FLAGS) free\dsrestor.obj free\proclass.obj ..\empty\free\empty.obj free\dsrestor.res
    copy free\dsrestor.exe c:\v814\exes\dsrestor.exe

free\dsrestor.obj : dsrestor.cpp
    cl $(CPP_PROJ) dsrestor.cpp

free\proclass.obj : proclass.cpp
    cl $(CPP_PROJ) proclass.cpp

free\dsrestor.res : dsrestor.rc
    rc $(RSC_PROJ) dsrestor.rc
