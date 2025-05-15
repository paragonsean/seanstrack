CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"udebug\savepart.pch" /YX"windows.h" /Fo"udebug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib ..\udebuglib\v5subs.lib /nologo /debug /subsystem:windows /incremental:no /pdb:"udebug\savepart.pdb"  /NODEFAULTLIB:LIBCMT.LIB  /machine:I386 /out:"udebug/savepart.exe"

RSC_PROJ=/l 0x409 /fo"udebug\savepart.res"

!IFNDEF RCFILE
RCFILE=savepart.rc
!ENDIF

udebug\savepart.exe : udebug\savepart.obj udebug\touni.obj udebug\savepart.res
    link $(LINK32_FLAGS) udebug\savepart.obj udebug\touni.obj udebug\savepart.res
    copy udebug\savepart.exe \v814wide\exes

udebug\savepart.obj : savepart.cpp
    cl $(CPP_PROJ) savepart.cpp

udebug\touni.obj : touni.cpp
    cl $(CPP_PROJ) touni.cpp

udebug\savepart.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
