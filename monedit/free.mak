CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\dundas.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/monedit.exe"

RSC_PROJ=/l 0x409 /fo"free\monedit.res" /d "NDEBUG"

free\monedit.exe : free\monedit.obj free\monitor.obj free\monedit.res
    link $(LINK32_FLAGS) free\monedit.obj free\monitor.obj ..\empty\free\empty.obj free\monedit.res
    copy free\monedit.exe c:\v814\exes

free\monedit.obj : monedit.cpp
    cl $(CPP_PROJ) monedit.cpp

free\monitor.obj : monitor.cpp
    cl $(CPP_PROJ) monitor.cpp

free\monedit.res : monedit.rc
    rc $(RSC_PROJ) monedit.rc

