CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib wsock32.lib uuid.lib ..\ufreelib\v5subs.lib ..\ufreelib\dundas.lib /nologo /MAP /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"ufree/editpart.exe"

RSC_PROJ=/l 0x409 /fo"ufree\editpart.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=editpart.rc
!ENDIF

ufree\editpart.exe : ufree\editpart.obj ufree\partdata.obj ufree\edparam.obj ufree\drawstr.obj \
    ufree\calc.obj ufree\editpart.res
    link $(LINK32_FLAGS) ufree\editpart.obj ufree\partdata.obj ..\empty\ufree\empty.obj ufree\edparam.obj \
    ufree\drawstr.obj ufree\calc.obj ufree\editpart.res
    copy ufree\editpart.exe c:\v8wide\exes

ufree\editpart.obj : editpart.cpp
    cl $(CPP_PROJ) editpart.cpp

ufree\partdata.obj : partdata.cpp
    cl $(CPP_PROJ) partdata.cpp

ufree\edparam.obj  : edparam.cpp
    cl $(CPP_PROJ) edparam.cpp

ufree\drawstr.obj  : drawstr.obj
    cl $(CPP_PROJ) drawstr.cpp

ufree\calc.obj : calc.cpp
    cl $(CPP_PROJ) calc.cpp

ufree\editpart.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
