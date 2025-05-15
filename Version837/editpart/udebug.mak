CPP_PROJ=/nologo /MTd /W3 /Od /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"..\include\udebug.pch" /Yu"windows.h" /Fo"udebug/" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\udebuglib\v5subs.lib ..\udebuglib\dundas.lib /nologo /debug /subsystem:windows,5.01 /incremental:no /pdb:"udebug\editpart.pdb"  /NODEFAULTLIB:LIBCMT.LIB  /machine:I386 /out:"udebug/editpart.exe"

RSC_PROJ=/l 0x409 /fo"udebug\editpart.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=editpart.rc
!ENDIF

udebug\editpart.exe : udebug\editpart.obj udebug\partdata.obj udebug\edparam.obj udebug\drawstr.obj \
    udebug\calc.obj udebug\editpart.res
    link $(LINK32_FLAGS) udebug\editpart.obj udebug\partdata.obj ..\empty\udebug\empty.obj udebug\edparam.obj \
    udebug\drawstr.obj udebug\calc.obj udebug\editpart.res
    copy udebug\editpart.exe c:\v8wide\exes

udebug\editpart.obj : editpart.cpp
    cl $(CPP_PROJ) editpart.cpp

udebug\partdata.obj : partdata.cpp
    cl $(CPP_PROJ) partdata.cpp

udebug\edparam.obj  : edparam.cpp
    cl $(CPP_PROJ) edparam.cpp

udebug\drawstr.obj  : drawstr.obj
    cl $(CPP_PROJ) drawstr.cpp

udebug\calc.obj : calc.cpp
    cl $(CPP_PROJ) calc.cpp

udebug\editpart.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
