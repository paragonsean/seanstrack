CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib ..\freelib\dundas.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/editpart.exe"

RSC_PROJ=/l 0x409 /fo"free\editpart.res" /d "NDEBUG"

free\editpart.exe : free\editpart.obj free\partdata.obj free\edparam.obj  free\drawstr.obj \
    free\calc.obj free\editpart.res
    link $(LINK32_FLAGS) free\editpart.obj free\partdata.obj ..\empty\free\empty.obj free\edparam.obj \
    free\drawstr.obj free\calc.obj free\editpart.res
    copy free\editpart.exe c:\v5\exes

free\editpart.obj : editpart.cpp
    cl $(CPP_PROJ) editpart.cpp

free\partdata.obj : partdata.cpp
    cl $(CPP_PROJ) partdata.cpp

free\edparam.obj  : edparam.cpp
    cl $(CPP_PROJ) edparam.cpp

free\drawstr.obj  : drawstr.obj
    cl $(CPP_PROJ) drawstr.cpp

free\calc.obj : calc.cpp
    cl $(CPP_PROJ) calc.cpp

free\editpart.res : editpart.rc
    rc $(RSC_PROJ) editpart.rc
