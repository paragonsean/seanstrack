CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/bupres.exe"

RSC_PROJ=/l 0x409 /fo"free/bupres.res" /d "NDEBUG"

free/bupres.exe : free/ab.obj free/backup.obj free/bupres.obj free/dd.obj free/delete.obj \
    free/fill.obj free/loadzip.obj free/purge.obj free/restore.obj \
    free/save.obj free/tb.obj free/tr.obj free/bupres.res
    link $(LINK32_FLAGS) free/ab.obj free/backup.obj free/bupres.obj free/dd.obj free/delete.obj \
    free/fill.obj free/loadzip.obj free/purge.obj free/restore.obj \
    free/save.obj free/tb.obj free/tr.obj ..\empty\free\empty.obj free/bupres.res
    copy free\bupres.exe c:\v814\exes

free/ab.obj : ab.cpp
    cl $(CPP_PROJ) ab.cpp

free/backup.obj : backup.cpp
    cl $(CPP_PROJ) backup.cpp

free/bupres.obj : bupres.cpp
    cl $(CPP_PROJ) bupres.cpp

free/dd.obj : dd.cpp
    cl $(CPP_PROJ) dd.cpp

free/delete.obj : delete.cpp
    cl $(CPP_PROJ) delete.cpp

free/fill.obj : fill.cpp
    cl $(CPP_PROJ) fill.cpp

free/loadzip.obj : loadzip.cpp
    cl $(CPP_PROJ) loadzip.cpp

free/purge.obj : purge.cpp
    cl $(CPP_PROJ) purge.cpp

free/restore.obj : restore.cpp
    cl $(CPP_PROJ) restore.cpp

free/save.obj : save.cpp
    cl $(CPP_PROJ) save.cpp

free/tb.obj : tb.cpp
    cl $(CPP_PROJ) tb.cpp

free/tr.obj : tr.cpp
    cl $(CPP_PROJ) tr.cpp

free/bupres.res : bupres.rc
    rc $(RSC_PROJ) bupres.rc