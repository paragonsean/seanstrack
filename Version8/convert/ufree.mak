CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib uuid.lib mpr.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"ufree/convert.exe"

RSC_PROJ=/l 0x409 /fo"ufree\convert.res" /d "NDEBUG"

ufree\convert.exe : ufree\convert.obj ufree\convert.res
    link $(LINK32_FLAGS) ufree\convert.obj ..\empty\ufree\empty.obj ufree\convert.res
    copy ufree\convert.exe c:\v814wide\exes

ufree\convert.obj : convert.cpp
    cl $(CPP_PROJ) convert.cpp

ufree\convert.res : convert.rc
    rc $(RSC_PROJ) convert.rc
