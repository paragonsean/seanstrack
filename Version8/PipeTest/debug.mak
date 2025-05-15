CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug\v5setups.pch" /YX"windows.h" /Fo"debug/" /Fd"debug\\" /c
CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug\mailslot.pch" /YX"windows.h" /Fo"debug/" /Fd"debug\\" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib         ..\debuglib\v5subs.lib /NODEFAULTLIB:LIBC.LIB /nologo /subsystem:windows /incremental:no /pdb:"debug\mailslot.pdb" /debug /machine:I386 /out:"debug/mailslot.exe"

RSC_PROJ=/l 0x409 /fo"debug\mailslot.res" /d "NDEBUG"

debug\mailslot.exe : debug\mailslot.obj debug\mailslot.res
    link $(LINK32_FLAGS) debug\mailslot.obj debug\mailslot.res

debug\mailslot.obj : mailslot.cpp
    cl $(CPP_PROJ) mailslot.cpp

debug\mailslot.res : mailslot.rc
    rc $(RSC_PROJ) mailslot.rc
