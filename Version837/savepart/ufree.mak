CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib wsock32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /MAP /subsystem:windows,5.01 /incremental:no /machine:I386 /out:"ufree/savepart.exe"

RSC_PROJ=/l 0x409 /fo"ufree\savepart.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=savepart.rc
!ENDIF

ufree\savepart.exe : ufree\savepart.obj ufree\touni.obj ufree\savepart.res
    link $(LINK32_FLAGS) ufree\savepart.obj ufree\touni.obj ..\empty\ufree\empty.obj ufree\savepart.res
    copy ufree\savepart.exe \v8wide\exes

ufree\savepart.obj : savepart.cpp
    cl $(CPP_PROJ) savepart.cpp

ufree\touni.obj : touni.cpp
    cl $(CPP_PROJ) touni.cpp

ufree\savepart.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
