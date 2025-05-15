CPP_PROJ=/nologo /MT /W3 /Zp1 /Od /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "_DEBUG" /Fp"..\include\udebug.pch" /Yu"windows.h" /Fo"udebug\\" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib wsock32.lib uuid.lib ..\udebuglib\v5subs.lib /NODEFAULTLIB:LIBC.LIB /NODEFAULTLIB:LIBCMT.LIB /nologo /debug /subsystem:windows,5.01 /incremental:no /machine:I386 /out:"udebug/eventman.exe"

RSC_PROJ=/l 0x409 /fo"udebug\eventman.res"

!IFNDEF RCFILE
RCFILE=eventman.rc
!ENDIF

udebug\eventman.exe : udebug\eventman.obj udebug\eventman.res
    link $(LINK32_FLAGS) udebug\eventman.obj ..\empty\udebug\empty.obj udebug\eventman.res
    copy udebug\eventman.exe c:\v8wide\exes

udebug\eventman.obj : eventman.cpp
    cl $(CPP_PROJ) eventman.cpp

udebug\eventman.res : eventman.rc
    rc $(RSC_PROJ) eventman.rc
