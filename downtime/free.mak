CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/downtime.exe"

RSC_PROJ=/l 0x409 /fo"free\downtime.res" /d "NDEBUG"

free\downtime.exe : free\downtime.obj free\category.obj free\downtime.res
    link $(LINK32_FLAGS) free\downtime.obj free\category.obj ..\empty\free\empty.obj free\downtime.res
    copy free\downtime.exe c:\v814\exes

free\downtime.obj : downtime.cpp
    cl $(CPP_PROJ) downtime.cpp

free\category.obj : category.cpp
    cl $(CPP_PROJ) category.cpp

free\downtime.res : downtime.rc
    rc $(RSC_PROJ) downtime.rc
