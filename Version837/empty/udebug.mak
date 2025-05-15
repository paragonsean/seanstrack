CPP_PROJ=/nologo /MTd /W3 /Od /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/udebug.pch" /Yc"windows.h" /Fo"udebug/" /c /EHsc

udebug\empty.obj : empty.cpp
    cl $(CPP_PROJ) empty.cpp

