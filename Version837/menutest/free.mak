CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/menutest.exe"

RSC_PROJ=/l 0x409 /fo"free\menutest.res" /d "NDEBUG"

free\menutest.exe : free\menutest.obj free\menutest.res
    link $(LINK32_FLAGS) free\menutest.obj free\menutest.res

free\menutest.obj : menutest.cpp
    cl $(CPP_PROJ) menutest.cpp

free\menutest.res : menutest.rc
    rc $(RSC_PROJ) menutest.rc

