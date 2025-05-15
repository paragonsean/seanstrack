CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib wsock32.lib uuid.lib mpr.lib htmlhelp.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/vtextrac.exe"

RSC_PROJ=/l 0x409 /fo"free\vtextrac.res" /d "NDEBUG"

free\vtextrac.exe : free\vtextrac.obj free\vtextrac.res
    link $(LINK32_FLAGS) free\vtextrac.obj ..\empty\free\empty.obj free\vtextrac.res
    copy free\vtextrac.exe \v8\exes

free\vtextrac.obj : vtextrac.cpp
     cl $(CPP_PROJ) vtextrac.cpp

free\vtextrac.res : vtextrac.rc
     rc $(RSC_PROJ) vtextrac.rc
