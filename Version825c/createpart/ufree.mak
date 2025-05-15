CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /MAP /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"ufree/createpart.exe"

RSC_PROJ=/l 0x409 /fo"ufree\createpart.res" /d "NDEBUG"

ufree\createpart.exe : ufree\createpart.obj ufree\touni.obj ufree\createpart.res
    link $(LINK32_FLAGS) ufree\createpart.obj ufree\touni.obj ..\empty\ufree\empty.obj ufree\createpart.res
    copy ufree\createpart.exe c:\v5wide\exes

ufree\createpart.obj : createpart.cpp
    cl $(CPP_PROJ) createpart.cpp

ufree\touni.obj : touni.cpp
    cl $(CPP_PROJ) touni.cpp

ufree\createpart.res : createpart.rc
    rc $(RSC_PROJ) createpart.rc
