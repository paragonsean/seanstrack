CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /YX"windows.h" /Fo"debug/" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib mpr.lib wsock32.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\test.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/test.exe"

RSC_PROJ=/l 0x409 /fo"debug\test.res"

debug\test.exe : debug\test.obj debug\test.res
    link $(LINK32_FLAGS) debug\test.obj ..\empty\debug\empty.obj debug\test.res
    copy debug\test.exe \v8\exes

debug\test.obj : test.cpp
    cl $(CPP_PROJ) test.cpp

debug\test.res : test.rc
    rc $(RSC_PROJ) test.rc
