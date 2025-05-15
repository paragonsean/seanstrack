CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /machine:I386 /out:"free/savepart.exe"

RSC_PROJ=/l 0x409 /fo"free\savepart.res" /d "NDEBUG"

free\savepart.exe : free\savepart.obj free\touni.obj free\savepart.res
    link $(LINK32_FLAGS) free\savepart.obj free\touni.obj ..\empty\free\empty.obj free\savepart.res
    copy free\savepart.exe c:\v814\exes

free\savepart.obj : savepart.cpp
    cl $(CPP_PROJ) savepart.cpp

free\touni.obj : touni.cpp
    cl $(CPP_PROJ) touni.cpp

free\savepart.res : savepart.rc
    rc $(RSC_PROJ) savepart.rc
