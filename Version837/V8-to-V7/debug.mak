CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /Yu"windows.h" /Fo"debug/" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /pdb:"debug\v8tov7.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/v8tov7.exe"

RSC_PROJ=/l 0x409 /fo"debug\v8tov7.res"

debug\v8tov7.exe : debug\v8tov7.obj debug\v8tov7.res
    link $(LINK32_FLAGS) debug\v8tov7.obj ..\empty\debug\empty.obj debug\v8tov7.res
    copy debug\v8tov7.exe \v8\exes

debug\v8tov7.obj : v8tov7.cpp
    cl $(CPP_PROJ) v8tov7.cpp

debug\v8tov7.res : v8tov7.rc
    rc $(RSC_PROJ) v8tov7.rc
