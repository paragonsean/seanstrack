CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /Yu"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib ..\debuglib\dundas.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\StdDown.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/StdDown.exe"

RSC_PROJ=/l 0x409 /fo"debug\StdDown.res"

debug\StdDown.exe : debug\StdDown.obj debug\StdDown.res
    link $(LINK32_FLAGS) debug\StdDown.obj ..\editdown\debug\category.obj ..\empty\debug\empty.obj debug\StdDown.res
    copy debug\stddown.exe \v8\exes


debug\StdDown.obj : StdDown.cpp
    cl $(CPP_PROJ) StdDown.cpp

debug\StdDown.res : StdDown.rc
    rc $(RSC_PROJ) StdDown.rc
