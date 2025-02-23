CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib ..\freelib\v5subs.lib /map /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/ft2.exe"

RSC_PROJ=/l 0x409 /fo"free\ft2.res" /d "NDEBUG"

free\ft2.exe : free\ft2.obj free\board_monitor.obj free\shotsave.obj free\skip.obj free\upload_copy_class.obj free\ft2.res
    link $(LINK32_FLAGS) free\ft2.obj free\board_monitor.obj free\shotsave.obj free\skip.obj free\upload_copy_class.obj ..\empty\free\empty.obj free\ft2.res
    copy free\ft2.exe \v5\exes\monall.exe

free\ft2.obj : ft2.cpp ..\include\ftii.h resource.h extern.h
    cl $(CPP_PROJ) ft2.cpp

free\board_monitor.obj : board_monitor.cpp extern.h ..\include\ftii_shot_data.h ..\include\ftii.h shotsave.h
    cl $(CPP_PROJ) board_monitor.cpp

free\shotsave.obj : shotsave.cpp shotsave.h
    cl $(CPP_PROJ) shotsave.cpp

free\skip.obj : skip.cpp
    cl $(CPP_PROJ) skip.cpp

free\upload_copy_class.obj : upload_copy_class.cpp upload_copy_class.h
    cl $(CPP_PROJ) upload_copy_class.cpp

free\ft2.res : ft2.rc resource.h resrc1.h
    rc $(RSC_PROJ) ft2.rc
