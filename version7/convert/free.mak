CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/convert.exe"

RSC_PROJ=/l 0x409 /fo"free\convert.res" /d "NDEBUG"

free\convert.exe : free\convert.obj free\convert.res
    link $(LINK32_FLAGS) free\convert.obj free\convert.res
    copy free\convert.exe \v7\exes

free\convert.obj : convert.cpp
    cl $(CPP_PROJ) convert.cpp

free\convert.res : convert.rc
    rc $(RSC_PROJ) convert.rc
