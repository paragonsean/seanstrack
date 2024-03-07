CPP_PROJ=/nologo /MTd /W3 /Zp1 /Od /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /Fp"../include/debug.pch" /Yc"windows.h" /Fo"debug/" /c

debug\empty.obj : empty.cpp
    cl $(CPP_PROJ) empty.cpp
