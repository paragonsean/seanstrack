CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib netapi32.lib ole32.lib uuid.lib mpr.lib wsock32.lib ..\..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/test_eventman.exe"

RSC_PROJ=/l 0x409 /fo"free\test_eventman.res" /d "NDEBUG"

free\test_eventman.exe : free\test_eventman.obj free\test_eventman.res
    link $(LINK32_FLAGS) free\test_eventman.obj ..\..\empty\free\empty.obj ..\..\subs\free\asensor.obj free\test_eventman.res
    copy free\test_eventman.exe \v8\exes

free\test_eventman.obj : test_eventman.cpp
    cl $(CPP_PROJ) test_eventman.cpp

free\test_eventman.res : test_eventman.rc
    rc $(RSC_PROJ) test_eventman.rc
