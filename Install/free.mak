CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/setup.exe"

RSC_PROJ=/l 0x409 /fo"free\setup.res" /d "NDEBUG"

free\setup.exe : free\setup.obj free\menupdat.obj free\setup.res
    link $(LINK32_FLAGS) free\setup.obj free\menupdat.obj ..\empty\free\empty.obj free\setup.res
    copy free\setup.exe \install\install8.25\setup.exe

free\setup.obj : setup.cpp
    cl $(CPP_PROJ) setup.cpp

free\menupdat.obj : menupdat.cpp
    cl $(CPP_PROJ) menupdat.cpp

free\setup.res : setup.rc
    rc $(RSC_PROJ) setup.rc
