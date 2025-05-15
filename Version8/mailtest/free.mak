CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /machine:I386 /out:"free/mailslot.exe"

RSC_PROJ=/l 0x409 /fo"free\mailslot.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=mailslot.rc
!ENDIF

free\mailslot.exe : free\mailslot.obj free\mailslot.res
    link $(LINK32_FLAGS) free\mailslot.obj ..\empty\free\empty.obj free\mailslot.res

free\mailslot.obj : mailslot.cpp
    cl $(CPP_PROJ) mailslot.cpp

free\mailslot.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
