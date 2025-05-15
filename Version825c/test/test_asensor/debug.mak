CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /YX"windows.h" /Fo"debug/" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib mpr.lib wsock32.lib ..\..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\test_asensor.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/test_asensor.exe"

RSC_PROJ=/l 0x409 /fo"debug\test_asensor.res"

debug\test_asensor.exe : debug\test_asensor.obj debug\test_asensor.res
    link $(LINK32_FLAGS) debug\test_asensor.obj ..\..\empty\debug\empty.obj ..\..\subs\debug\asensor.obj debug\test_asensor.res
    copy debug\test_asensor.exe \v8\exes

debug\test_asensor.obj : test_asensor.cpp
    cl $(CPP_PROJ) test_asensor.cpp

debug\test_asensor.res : test_asensor.rc
    rc $(RSC_PROJ) test_asensor.rc
