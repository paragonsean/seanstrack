CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib ..\freelib\dundas.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/emachine.exe"

RSC_PROJ=/l 0x409 /fo"free\emachine.res" /d "NDEBUG"

free\emachine.exe : free\editdiff.obj free\emachine.obj free\emachine.res
    link $(LINK32_FLAGS) free\editdiff.obj free\emachine.obj ..\empty\free\empty.obj free\emachine.res
    copy free\emachine.exe c:\v814\exes

free\editdiff.obj : editdiff.cpp
    cl $(CPP_PROJ) editdiff.cpp

free\emachine.obj : emachine.cpp
    cl $(CPP_PROJ) emachine.cpp

free\emachine.res : emachine.rc
    rc $(RSC_PROJ) emachine.rc
