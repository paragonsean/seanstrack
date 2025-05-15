CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /Yu"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib mpr.lib wsock32.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\mnst.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/mnst.exe"

RSC_PROJ=/l 0x409 /fo"debug\mnst.res"

debug\mnst.exe : debug\mnst.obj debug\mnst.res
    link $(LINK32_FLAGS) debug\mnst.obj ..\empty\debug\empty.obj debug\mnst.res

debug\mnst.obj : mnst.cpp
    cl $(CPP_PROJ) mnst.cpp

debug\mnst.res : mnst.rc
    rc $(RSC_PROJ) mnst.rc
