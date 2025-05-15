CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib netapi32.lib ole32.lib uuid.lib mpr.lib wsock32.lib ..\ufreelib\v5subs.lib /nologo /MAP /subsystem:windows /incremental:no /machine:I386 /out:"ufree/test.exe"

RSC_PROJ=/l 0x409 /fo"ufree\test.res" /d "NDEBUG"

ufree\test.exe : ufree\test.obj ufree\test.res
    link $(LINK32_FLAGS) ufree\test.obj ufree\test.res

ufree\test.obj : test.cpp
    cl $(CPP_PROJ) test.cpp

ufree\test.res : test.rc
    rc $(RSC_PROJ) test.rc
