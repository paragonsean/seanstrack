CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib htmlhelp.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"ufree/dsbackup.exe"

RSC_PROJ=/l 0x409 /fo"ufree\dsbackup.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=dsbackup.rc
!ENDIF

ufree\dsbackup.exe : ufree\global_file_settings.obj ufree\dsbackup.obj ufree\backupcl.obj ufree\dsbackup.res
    link $(LINK32_FLAGS) ufree\global_file_settings.obj ufree\dsbackup.obj ufree\backupcl.obj ..\empty\ufree\empty.obj ufree\dsbackup.res
    copy ufree\dsbackup.exe c:\v8dswide\exes

ufree\dsbackup.obj : dsbackup.cpp backupcl.h
    cl $(CPP_PROJ) dsbackup.cpp

ufree\backupcl.obj : backupcl.cpp backupcl.h
    cl $(CPP_PROJ) backupcl.cpp

ufree\global_file_settings.obj : global_file_settings.cpp backupcl.h
    cl $(CPP_PROJ) global_file_settings.cpp

ufree\dsbackup.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
