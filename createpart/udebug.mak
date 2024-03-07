CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"udebug\createpart.pch" /YX"windows.h" /Fo"udebug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib ..\udebuglib\v5subs.lib /nologo /debug /subsystem:windows /incremental:no /pdb:"udebug\createpart.pdb"  /NODEFAULTLIB:LIBCMT.LIB  /machine:I386 /out:"udebug/createpart.exe"

RSC_PROJ=/l 0x409 /fo"udebug\createpart.res"

!IFNDEF RCFILE
RCFILE=createpart.rc
!ENDIF

udebug\createpart.exe : udebug\createpart.obj udebug\touni.obj udebug\createpart.res
    link $(LINK32_FLAGS) udebug\createpart.obj udebug\touni.obj udebug\createpart.res
    copy udebug\createpart.exe \v8wide\exes

udebug\createpart.obj : createpart.cpp
    cl $(CPP_PROJ) createpart.cpp

udebug\touni.obj : touni.cpp
    cl $(CPP_PROJ) touni.cpp

udebug\createpart.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
