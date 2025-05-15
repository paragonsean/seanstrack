CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/pickwire.exe"

RSC_PROJ=/l 0x409 /fo"free\pickwire.res" /d "NDEBUG"

free\pickwire.exe : free\pickwire.obj free\pickwire.res
    link $(LINK32_FLAGS) free\pickwire.obj free\pickwire.res
    copy free\pickwire.exe \v7\exes\pickwire.exe

free\pickwire.obj : pickwire.cpp
    cl $(CPP_PROJ) pickwire.cpp

free\pickwire.res : pickwire.rc
    rc $(RSC_PROJ) pickwire.rc
