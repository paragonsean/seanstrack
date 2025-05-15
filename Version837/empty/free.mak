CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yc"windows.h" /Fo"free/" /c /EHsc

free\empty.obj : empty.cpp
    cl $(CPP_PROJ) empty.cpp
