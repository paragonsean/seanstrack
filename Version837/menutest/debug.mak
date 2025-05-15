CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/menutest.pch" /YX"windows.h" /Fo"debug/" /Fd"debug\\" /c /EHsc

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\menutest.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/menutest.exe"

RSC_PROJ=/l 0x409 /fo"debug\menutest.res"

debug\menutest.exe : debug\menutest.obj debug\menutest.res
    link $(LINK32_FLAGS) debug\menutest.obj debug\menutest.res

debug\menutest.obj : menutest.cpp
    cl $(CPP_PROJ) menutest.cpp

debug\menutest.res : menutest.rc
    rc $(RSC_PROJ) menutest.rc
