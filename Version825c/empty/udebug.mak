CPP_PROJ=/nologo /MTd /W3 /Zp1 /Od /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/udebug.pch" /Yc"windows.h" /Fo"udebug/" /c


udebug\empty.obj : empty.cpp
    cl $(CPP_PROJ) empty.cpp

