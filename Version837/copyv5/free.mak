CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib wsock32.lib uuid.lib mpr.lib htmlhelp.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/copyv5.exe"

RSC_PROJ=/l 0x409 /fo"free\copyv5.res" /d "NDEBUG"

free\copyv5.exe : free\copyv5.obj free\copyv5.res
    link $(LINK32_FLAGS) free\copyv5.obj ..\empty\free\empty.obj free\copyv5.res
    copy free\copyv5.exe c:\v8\exes

free\copyv5.obj : copyv5.cpp
    cl $(CPP_PROJ) copyv5.cpp

free\copyv5.res : copyv5.rc resource.h
    rc $(RSC_PROJ) copyv5.rc
