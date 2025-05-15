CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/serialterm.exe"

RSC_PROJ=/l 0x409 /fo"free\serialterm.res" /d "NDEBUG"

!IFNDEF RCFILE
RCFILE=serialterm.rc
!ENDIF

free\serialterm.exe : free\serialterm.obj free\serial_monitor.obj free\serialterm.res
    link /MAP $(LINK32_FLAGS) free\serialterm.obj free\serial_monitor.obj free\serialterm.res

free\serialterm.obj : serialterm.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h
    cl $(CPP_PROJ) serialterm.cpp

free\serial_monitor.obj : serial_monitor.cpp
    cl $(CPP_PROJ) serial_monitor.cpp

free\serialterm.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
