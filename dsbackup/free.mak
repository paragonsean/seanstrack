CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib htmlhelp.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/dsbackup.exe"

RSC_PROJ=/l 0x409 /fo"free\dsbackup.res" /d "NDEBUG"

free\dsbackup.exe : free\global_file_settings.obj free\dsbackup.obj free\backupcl.obj free\dsbackup.res
    link $(LINK32_FLAGS) free\global_file_settings.obj free\dsbackup.obj free\backupcl.obj ..\empty\free\empty.obj free\dsbackup.res
    copy free\dsbackup.exe c:\v814ds\exes

free\dsbackup.obj : dsbackup.cpp backupcl.h
    cl $(CPP_PROJ) dsbackup.cpp

free\backupcl.obj : backupcl.cpp backupcl.h
    cl $(CPP_PROJ) backupcl.cpp

free\global_file_settings.obj : global_file_settings.cpp backupcl.h
    cl $(CPP_PROJ) global_file_settings.cpp

free\dsbackup.res : dsbackup.rc
    rc $(RSC_PROJ) dsbackup.rc
