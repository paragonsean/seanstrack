CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"udebug\setup.pch" /YX"windows.h" /Fo"udebug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib ..\udebuglib\v5subs.lib /nologo /debug /subsystem:windows /incremental:no /pdb:"udebug\setup.pdb"  /NODEFAULTLIB:LIBCMT.LIB  /machine:I386 /out:"udebug/setup.exe"

RSC_PROJ=/l 0x409 /fo"udebug\setup.res"

!IFNDEF RCFILE
RCFILE=setup.rc
!ENDIF

udebug\setup.exe : udebug\setup.obj udebug\menupdat.obj udebug\setup.res
    link $(LINK32_FLAGS) udebug\setup.obj udebug\menupdat.obj udebug\setup.res

udebug\setup.obj : setup.cpp
    cl $(CPP_PROJ) setup.cpp

udebug\menupdat.obj : menupdat.cpp
    cl $(CPP_PROJ) menupdat.cpp

udebug\setup.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
