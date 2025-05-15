CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib netapi32.lib ole32.lib uuid.lib mpr.lib wsock32.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/lineup.exe"

RSC_PROJ=/l 0x409 /fo"free\lineup.res" /d "NDEBUG"

free\lineup.exe : free\lineup.obj free\lineup.res
    link $(LINK32_FLAGS) free\lineup.obj ..\empty\free\empty.obj free\lineup.res

free\lineup.obj : lineup.cpp
    cl $(CPP_PROJ) lineup.cpp

free\lineup.res : lineup.rc
    rc $(RSC_PROJ) lineup.rc
