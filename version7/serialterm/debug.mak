CPP_PROJ=/nologo /MTd /W3 /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"debug/serialterm.pch" /YX"windows.h" /Fo"debug/" /c

LINK32_FLAGS= libcmtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib wsock32.lib ..\debuglib\v5subs.lib /nologo /subsystem:windows /incremental:no /pdb:"debug\serialterm.pdb" /NODEFAULTLIB:libcmt.lib /debug /machine:I386 /out:"debug/serialterm.exe"

RSC_PROJ=/l 0x409 /fo"debug\serialterm.res"

debug\serialterm.exe : debug\serialterm.obj debug\serial_monitor.obj debug\serialterm.res
    link $(LINK32_FLAGS) debug\serialterm.obj debug\serial_monitor.obj debug\serialterm.res

debug\serialterm.obj : serialterm.cpp
    cl $(CPP_PROJ) serialterm.cpp

debug\serial_monitor.obj : serial_monitor.cpp
    cl $(CPP_PROJ) serial_monitor.cpp

debug\serialterm.res : serialterm.rc
    rc $(RSC_PROJ) serialterm.rc
