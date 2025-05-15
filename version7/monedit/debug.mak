CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/monedit.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib          ..\debuglib\dundas.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\monedit.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/monedit.exe"

RSC_PROJ=/l 0x409 /fo"debug\monedit.res" /d "NDEBUG"

debug\monedit.exe : debug\monedit.obj debug\monitor.obj debug\monedit.res
    link $(LINK32_FLAGS) debug\monedit.obj debug\monitor.obj debug\monedit.res
    copy debug\monedit.exe \v7\exes

debug\monedit.obj : monedit.cpp
    cl $(CPP_PROJ) monedit.cpp

debug\monitor.obj : monitor.cpp
    cl $(CPP_PROJ) monitor.cpp

debug\monedit.res : monedit.rc
    rc $(RSC_PROJ) monedit.rc

