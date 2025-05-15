CPP_PROJ=/nologo /MT /W3 /Zp1 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"../include/debug.pch" /Yu"windows.h" /Fo"debug/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib mpr.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /machine:I386 /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/setup.exe" 

RSC_PROJ=/l 0x409 /fo"debug\setup.res"

!IFNDEF RCFILE
RCFILE=setup.rc
!ENDIF

debug\setup.exe : debug\setup.obj debug\setup.res
    link $(LINK32_FLAGS) debug\setup.obj ..\empty\debug\empty.obj debug\setup.res

debug\setup.obj : setup.cpp
    cl $(CPP_PROJ) setup.cpp

debug\setup.res : $(RCFILE)
    rc $(RSC_PROJ) $(RCFILE)
