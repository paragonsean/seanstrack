CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /YX"windows.h" /Fo"debug/" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib mpr.lib wsock32.lib ..\..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\test_eventman.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/test_eventman.exe"

RSC_PROJ=/l 0x409 /fo"debug\test_eventman.res"

debug\test_eventman.exe : debug\test_eventman.obj debug\test_eventman.res
    link $(LINK32_FLAGS) debug\test_eventman.obj ..\..\empty\debug\empty.obj ..\..\subs\debug\asensor.obj debug\test_eventman.res
    copy debug\test_eventman.exe \v8\exes

debug\test_eventman.obj : test_eventman.cpp
    cl $(CPP_PROJ) test_eventman.cpp

debug\test_eventman.res : test_eventman.rc
    rc $(RSC_PROJ) test_eventman.rc
