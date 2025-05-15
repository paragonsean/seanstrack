CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"..\include\debug.pch" /Yu"windows.h" /Fo"debug/" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib  ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\visiedit.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/visiedit.exe"

RSC_PROJ=/l 0x409 /fo"debug\visiedit.res"

debug\visiedit.exe : debug\visiedit.obj debug\visiedit.res
    link $(LINK32_FLAGS) debug\visiedit.obj ..\empty\debug\empty.obj debug\visiedit.res
    copy debug\visiedit.exe \v8\exes

debug\visiedit.obj : visiedit.cpp
    cl $(CPP_PROJ) visiedit.cpp

debug\visiedit.res : visiedit.rc
    rc $(RSC_PROJ) visiedit.rc
