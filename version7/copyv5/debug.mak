CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/copyv5.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib mpr.lib htmlhelp.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\copyv5.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/copyv5.exe"

RSC_PROJ=/l 0x409 /fo"debug\copyv5.res"

debug\copyv5.exe : debug\copyv5.obj debug\copyv5.res
    link $(LINK32_FLAGS) debug\copyv5.obj debug\copyv5.res

debug\copyv5.obj : copyv5.cpp
    cl $(CPP_PROJ) copyv5.cpp

debug\copyv5.res : copyv5.rc resource.h
    rc $(RSC_PROJ) copyv5.rc
