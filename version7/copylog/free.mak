CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib htmlhelp.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/copylog.exe"

RSC_PROJ=/l 0x409 /fo"free\copylog.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=copylog.rc
!ENDIF

free\copylog.exe : free\copylog.obj free\copylog.res
    link $(LINK32_FLAGS) free\copylog.obj free\copylog.res
    copy free\copylog.exe \v7\exes

free\copylog.obj : copylog.cpp
    cl $(CPP_PROJ) copylog.cpp

free\copylog.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
