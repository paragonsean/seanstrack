CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/chooseshotest.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\chooseshotest.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/chooseshotest.exe"

RSC_PROJ=/l 0x409 /fo"debug\chooseshotest.res"

debug\chooseshotest.exe : debug\chooseshotest.obj debug\chooseshotest.res
    link $(LINK32_FLAGS) debug\chooseshotest.obj debug\chooseshotest.res
    copy debug\chooseshotest.exe \v7\exes

debug\chooseshotest.obj : chooseshotest.cpp
    cl $(CPP_PROJ) chooseshotest.cpp

debug\chooseshotest.res : chooseshotest.rc
    rc $(RSC_PROJ) chooseshotest.rc
