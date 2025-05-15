CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib wsock32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /machine:I386 /out:"free/datagetter.exe"

RSC_PROJ=/l 0x409 /fo"free\datagetter.res" /d "NDEBUG"

free\datagetter.exe : free\datagetter.obj free\datagetter.res
    link $(LINK32_FLAGS) free\datagetter.obj ..\empty\free\empty.obj free\datagetter.res
    copy free\datagetter.exe c:\v8\exes

free\datagetter.obj : datagetter.cpp
    cl $(CPP_PROJ) datagetter.cpp

free\datagetter.res : datagetter.rc
    rc $(RSC_PROJ) datagetter.rc
