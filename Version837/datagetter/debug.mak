CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"../include/debug.pch" /Yu"windows.h" /Fo"debug/" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib wsock32.lib uuid.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /pdb:"debug\datagetter.pdb"  /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/datagetter.exe"

RSC_PROJ=/l 0x409 /fo"debug\datagetter.res" /d "NDEBUG"

debug\datagetter.exe : debug\datagetter.obj debug\datagetter.res
    link $(LINK32_FLAGS) debug\datagetter.obj ..\empty\debug\empty.obj debug\datagetter.res
    copy debug\datagetter.exe c:\v8\exes

debug\datagetter.obj : datagetter.cpp
    cl $(CPP_PROJ) datagetter.cpp

debug\datagetter.res : datagetter.rc
    rc $(RSC_PROJ) datagetter.rc
