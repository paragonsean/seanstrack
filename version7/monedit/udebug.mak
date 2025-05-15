CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"udebug\monedit.pch" /YX"windows.h" /Fo"udebug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib             ..\udebuglib\dundas.lib ..\udebuglib\v5subs.lib      /nologo /subsystem:windows /incremental:no /pdb:"udebug\monedit.pdb" /NODEFAULTLIB:libcmt.lib /DEBUG /machine:I386 /out:"udebug/monedit.exe"

RSC_PROJ=/l 0x409 /fo"udebug\monedit.res" /d "NDEBUG"

udebug\monedit.exe : udebug\monedit.obj udebug\monitor.obj udebug\monedit.res
    link $(LINK32_FLAGS) udebug\monedit.obj udebug\monitor.obj udebug\monedit.res
    copy udebug\monedit.exe \v7wide\exes

udebug\monedit.obj : monedit.cpp
    cl $(CPP_PROJ) monedit.cpp

udebug\monitor.obj : monitor.cpp
    cl $(CPP_PROJ) monitor.cpp

udebug\monedit.res : monedit.rc
    rc $(RSC_PROJ) monedit.rc

