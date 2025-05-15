CPP_PROJ=/nologo /MTd /W3 /Zp1 /Od /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "DEBUG" /Fp"..\include/debug.pch" /Yu"windows.h" /Fo"debug\\" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib wsock32.lib uuid.lib ..\debuglib\v5subs.lib /NODEFAULTLIB:LIBC.LIB /nologo /subsystem:windows,5.01 /incremental:no /machine:I386 /out:"debug/dataserver.exe"

RSC_PROJ=/l 0x409 /fo"debug\dataserver.res"

debug\dataserver.exe : debug\dataserver.obj debug\dataserver.res
    link $(LINK32_FLAGS) debug\dataserver.obj ..\empty\debug\empty.obj debug\dataserver.res
    copy debug\dataserver.exe c:\v8\exes

debug\dataserver.obj : dataserver.cpp
    cl $(CPP_PROJ) dataserver.cpp

debug\dataserver.res : dataserver.rc
    rc $(RSC_PROJ) dataserver.rc
