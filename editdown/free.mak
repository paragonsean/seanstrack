CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/editdown.exe"

RSC_PROJ=/l 0x409 /fo"free\editdown.res" /d "NDEBUG"

free\editdown.exe : free\editdown.obj free\category.obj free\editdown.res
    link $(LINK32_FLAGS) free\editdown.obj free\category.obj ..\empty\free\empty.obj free\editdown.res
    copy free\editdown.exe c:\v814\exes

free\editdown.obj : editdown.cpp
    cl $(CPP_PROJ) editdown.cpp

free\category.obj : category.cpp
    cl $(CPP_PROJ) category.cpp

free\editdown.res : editdown.rc
    rc $(RSC_PROJ) editdown.rc
