CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib netapi32.lib ole32.lib uuid.lib mpr.lib wsock32.lib ..\..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/test_asensor.exe"

RSC_PROJ=/l 0x409 /fo"free\test_asensor.res" /d "NDEBUG"

free\test_asensor.exe : free\test_asensor.obj free\test_asensor.res
    link $(LINK32_FLAGS) free\test_asensor.obj ..\..\empty\free\empty.obj ..\..\subs\free\asensor.obj free\test_asensor.res
    copy free\test_asensor.exe \v8\exes

free\test_asensor.obj : test_asensor.cpp
    cl $(CPP_PROJ) test_asensor.cpp

free\test_asensor.res : test_asensor.rc
    rc $(RSC_PROJ) test_asensor.rc
