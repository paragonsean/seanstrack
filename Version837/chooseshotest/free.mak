CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/chooseshotest.exe"

RSC_PROJ=/l 0x409 /fo"free\chooseshotest.res" /d "NDEBUG"

free\chooseshotest.exe : free\chooseshotest.obj free\chooseshotest.res
    link $(LINK32_FLAGS) free\chooseshotest.obj free\chooseshotest.res
    copy free\chooseshotest.exe \v8\exes\chooseshotest.exe

free\chooseshotest.obj : chooseshotest.cpp
    cl $(CPP_PROJ) chooseshotest.cpp

free\chooseshotest.res : chooseshotest.rc
    rc $(RSC_PROJ) chooseshotest.rc
