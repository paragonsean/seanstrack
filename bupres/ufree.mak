CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"ufree/bupres.exe"

RSC_PROJ=/l 0x409 /fo"ufree/bupres.res" /d "NDEBUG"

ufree/bupres.exe : ufree/ab.obj ufree/backup.obj ufree/bupres.obj ufree/dd.obj ufree/delete.obj \
    ufree/fill.obj ufree/loadzip.obj ufree/purge.obj ufree/restore.obj \
    ufree/save.obj ufree/tb.obj ufree/tr.obj ufree/bupres.res
    link $(LINK32_FLAGS) ufree/ab.obj ufree/backup.obj ufree/bupres.obj ufree/dd.obj ufree/delete.obj \
    ufree/fill.obj ufree/loadzip.obj ufree/purge.obj ufree/restore.obj \
    ufree/save.obj ufree/tb.obj ufree/tr.obj ..\empty\ufree\empty.obj ufree/bupres.res
    copy ufree\bupres.exe c:\v814wide\exes

ufree/ab.obj : ab.cpp
    cl $(CPP_PROJ) ab.cpp

ufree/backup.obj : backup.cpp
    cl $(CPP_PROJ) backup.cpp

ufree/bupres.obj : bupres.cpp
    cl $(CPP_PROJ) bupres.cpp

ufree/dd.obj : dd.cpp
    cl $(CPP_PROJ) dd.cpp

ufree/delete.obj : delete.cpp
    cl $(CPP_PROJ) delete.cpp

ufree/fill.obj : fill.cpp
    cl $(CPP_PROJ) fill.cpp

ufree/loadzip.obj : loadzip.cpp
    cl $(CPP_PROJ) loadzip.cpp

ufree/purge.obj : purge.cpp
    cl $(CPP_PROJ) purge.cpp

ufree/restore.obj : restore.cpp
    cl $(CPP_PROJ) restore.cpp

ufree/save.obj : save.cpp
    cl $(CPP_PROJ) save.cpp

ufree/tb.obj : tb.cpp
    cl $(CPP_PROJ) tb.cpp

ufree/tr.obj : tr.cpp
    cl $(CPP_PROJ) tr.cpp

ufree/bupres.res : bupres.rc
    rc $(RSC_PROJ) bupres.rc
