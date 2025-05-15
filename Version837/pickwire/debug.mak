CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/pickwire.pch" /YX"windows.h" /Fo"debug/" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\pickwire.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/pickwire.exe"

RSC_PROJ=/l 0x409 /fo"debug\pickwire.res"

debug\pickwire.exe : debug\pickwire.obj debug\pickwire.res
    link $(LINK32_FLAGS) debug\pickwire.obj debug\pickwire.res
    copy debug\pickwire.exe \v7\exes

debug\pickwire.obj : pickwire.cpp
    cl $(CPP_PROJ) pickwire.cpp

debug\pickwire.res : pickwire.rc
    rc $(RSC_PROJ) pickwire.rc
