CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fo"debug/" /Yuwindows.h /Fp"..\include\debug.pch" /Fd"debug\\" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib ..\debuglib\v5subs.lib /NODEFAULTLIB:LIBC.LIB /nologo /subsystem:windows /incremental:no /pdb:"debug\eventman.pdb" /debug /machine:I386 /out:"debug/eventman.exe"

RSC_PROJ=/l 0x409 /fo"debug\eventman.res" /d "NDEBUG"

debug\eventman.exe : debug\eventman.obj debug\eventman.res
    link $(LINK32_FLAGS) debug\eventman.obj ..\empty\debug\empty.obj debug\eventman.res
    copy debug\eventman.exe \v8\exes

debug\eventman.obj : eventman.cpp
    cl $(CPP_PROJ) eventman.cpp

debug\eventman.res : eventman.rc
    rc $(RSC_PROJ) eventman.rc
