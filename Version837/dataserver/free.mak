CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib wsock32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /machine:I386 /out:"free/dataserver.exe"

RSC_PROJ=/l 0x409 /fo"free\dataserver.res" /d "NDEBUG"

free\dataserver.exe : free\dataserver.obj free\dataserver.res
    link $(LINK32_FLAGS) free\dataserver.obj ..\empty\free\empty.obj free\dataserver.res
    copy free\dataserver.exe c:\v8\exes

free\dataserver.obj : dataserver.cpp
    cl $(CPP_PROJ) dataserver.cpp

free\dataserver.res : dataserver.rc
    rc $(RSC_PROJ) dataserver.rc
