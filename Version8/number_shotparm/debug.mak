CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /Yu"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib mpr.lib wsock32.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\lineup.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/lineup.exe"

RSC_PROJ=/l 0x409 /fo"debug\lineup.res"

debug\lineup.exe : debug\lineup.obj debug\lineup.res
    link $(LINK32_FLAGS) debug\lineup.obj ..\empty\debug\empty.obj debug\lineup.res

debug\lineup.obj : lineup.cpp
    cl $(CPP_PROJ) lineup.cpp

debug\lineup.res : lineup.rc
    rc $(RSC_PROJ) lineup.rc
