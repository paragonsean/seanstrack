CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/newshotname.exe"

RSC_PROJ=/l 0x409 /fo"free\newshotname.res" /d "NDEBUG"

free\newshotname.exe : free\newshotname.obj free\newshotname.res
    link $(LINK32_FLAGS) free\newshotname.obj free\newshotname.res
    copy free\newshotname.exe \v8\exes

free\newshotname.obj : newshotname.cpp
    cl $(CPP_PROJ) newshotname.cpp

free\newshotname.res : newshotname.rc
    rc $(RSC_PROJ) newshotname.rc

