CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/createpart.exe"

RSC_PROJ=/l 0x409 /fo"free\createpart.res" /d "NDEBUG"

free\createpart.exe : free\createpart.obj free\createpart.res
    link $(LINK32_FLAGS) free\createpart.obj ..\empty\free\empty.obj free\createpart.res
    copy free\createpart.exe c:\v814\exes

free\createpart.obj : createpart.cpp
    cl $(CPP_PROJ) createpart.cpp

free\createpart.res : createpart.rc
    rc $(RSC_PROJ) createpart.rc
