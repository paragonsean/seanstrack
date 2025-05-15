CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"ufree/vtextrac.exe"

RSC_PROJ=/l 0x409 /fo"ufree\vtextrac.res" /d "NDEBUG"

ufree\vtextrac.exe : ufree\vtextrac.obj ufree\vtextrac.res
    link $(LINK32_FLAGS) ufree\vtextrac.obj ..\empty\ufree\empty.obj ufree\vtextrac.res
    copy ufree\vtextrac.exe c:\v814wide\exes

ufree\vtextrac.obj : vtextrac.cpp
    cl $(CPP_PROJ) vtextrac.cpp

ufree\vtextrac.res : vtextrac.rc
    rc $(RSC_PROJ) vtextrac.rc
