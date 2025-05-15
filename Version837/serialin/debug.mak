CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/serialin.pch" /YX"windows.h" /Fo"debug/" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\serialin.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/serialin.exe"

RSC_PROJ=/l 0x409 /fo"debug\serialin.res"

debug\serialin.exe : debug\serialin.obj debug\serialin.res
    link $(LINK32_FLAGS) debug\serialin.obj debug\serialin.res
    copy debug\serialin.exe \v5\exes

debug\serialin.obj : serialin.cpp
    cl $(CPP_PROJ) serialin.cpp

debug\serialin.res : serialin.rc
    rc $(RSC_PROJ) serialin.rc
