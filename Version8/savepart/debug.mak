CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /Yu"windows.h" /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\debuglib\v5subs.lib ..\debuglib\dundas.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\savepart.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/savepart.exe"

RSC_PROJ=/l 0x409 /fo"debug\savepart.res"

debug\savepart.exe : debug\savepart.obj debug\touni.obj debug\savepart.res
    link $(LINK32_FLAGS) debug\savepart.obj debug\touni.obj ..\empty\debug\empty.obj debug\savepart.res
    copy debug\savepart.exe \v7\exes

debug\savepart.obj : savepart.cpp
    cl $(CPP_PROJ) savepart.cpp

debug\touni.obj : touni.cpp
    cl $(CPP_PROJ) touni.cpp

debug\savepart.res : savepart.rc
    rc $(RSC_PROJ) savepart.rc
