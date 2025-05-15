CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\freelib\v5subs.lib /nologo /subsystem:windows /incremental:no /machine:I386 /out:"free/serialin.exe"

RSC_PROJ=/l 0x409 /fo"free\serialin.res" /d "NDEBUG"

free\serialin.exe : free\serialin.obj free\serialin.res
    link $(LINK32_FLAGS) free\serialin.obj free\serialin.res

free\serialin.obj : serialin.cpp
    cl $(CPP_PROJ) serialin.cpp

free\serialin.res : serialin.rc
    rc $(RSC_PROJ) serialin.rc
