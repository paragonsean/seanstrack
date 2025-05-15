CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /machine:I386 /out:"free/v8tov7.exe"

RSC_PROJ=/l 0x409 /fo"free\v8tov7.res" /d "NDEBUG"

free\v8tov7.exe : free\v8tov7.obj free\v8tov7.res
    link $(LINK32_FLAGS) free\v8tov7.obj ..\empty\free\empty.obj free\v8tov7.res
    copy free\v8tov7.exe \v8\exes

free\v8tov7.obj : v8tov7.cpp
    cl $(CPP_PROJ) v8tov7.cpp

free\v8tov7.res : v8tov7.rc
    rc $(RSC_PROJ) v8tov7.rc
