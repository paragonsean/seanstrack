CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /Yu"windows.h" /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\debuglib\v5subs.lib ..\debuglib\dundas.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\createpart.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/createpart.exe"

RSC_PROJ=/l 0x409 /fo"debug\createpart.res"

debug\createpart.exe : debug\createpart.obj debug\createpart.res
    link $(LINK32_FLAGS) debug\createpart.obj ..\empty\debug\empty.obj debug\createpart.res
    copy debug\createpart.exe \v8\exes

debug\createpart.obj : createpart.cpp
    cl $(CPP_PROJ) createpart.cpp

debug\createpart.res : createpart.rc
    rc $(RSC_PROJ) createpart.rc
