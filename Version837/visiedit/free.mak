CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c /EHsc

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib wsock32.lib uuid.lib ..\freelib\v5subs.lib /nologo /subsystem:windows,5.01 /incremental:no /MANIFEST:EMBED /machine:I386 /out:"free/visiedit.exe"

RSC_PROJ=/l 0x409 /fo"free\visiedit.res" /d "NDEBUG"

free\visiedit.exe : free\visiedit.obj free\visiedit.res
    link $(LINK32_FLAGS) free\visiedit.obj ..\empty\free\empty.obj free\visiedit.res
    copy free\visiedit.exe \v8\exes

free\visiedit.obj : visiedit.cpp
    cl $(CPP_PROJ) visiedit.cpp

free\visiedit.res : visiedit.rc
    rc $(RSC_PROJ) visiedit.rc
