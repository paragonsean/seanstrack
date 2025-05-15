CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib ole32.lib uuid.lib ..\ufreelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"ufree/copyv5.exe"

RSC_PROJ=/l 0x409 /fo"ufree/copyv5.res" /d "NDEBUG"

ufree\copyv5.exe : ufree\copyv5.obj ufree\copyv5.res
    link $(LINK32_FLAGS) ufree\copyv5.obj ..\empty\ufree\empty.obj ufree\copyv5.res
    copy ufree\copyv5.exe c:\v814wide\exes

ufree\copyv5.obj : copyv5.cpp
    cl $(CPP_PROJ) copyv5.cpp

ufree\copyv5.res : copyv5.rc
    rc $(RSC_PROJ) copyv5.rc
