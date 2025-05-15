CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/eventman.exe"

RSC_PROJ=/l 0x409 /fo"free\eventman.res" /d "NDEBUG"

free\eventman.exe : free\eventman.obj free\mail.obj free\eventman.res
    link $(LINK32_FLAGS) free\eventman.obj free\mail.obj ..\empty\free\empty.obj free\eventman.res
    copy free\eventman.exe c:\v814\exes

free\eventman.obj : eventman.cpp
    cl $(CPP_PROJ) eventman.cpp

free\mail.obj : mail.cpp
    cl $(CPP_PROJ) mail.cpp

free\eventman.res : eventman.rc
    rc $(RSC_PROJ) eventman.rc
