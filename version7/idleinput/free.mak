CPP_PROJ=/nologo /MD /W3 /Zp1 /O1 /D "WIN32" /D "_USRDLL" /D "_WINDOWS" /D "NDEBUG" /D "IDLEINPUT_EXPORTS" /Fo"free/" /c

LINK32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /dll /incremental:no /machine:I386 /out:"free/idleinput.dll"

free\idleinput.dll : free\idleinput.obj
    link $(LINK32_FLAGS) free\idleinput.obj

free\idleinput.obj : idleinput.cpp
    cl $(CPP_PROJ) idleinput.cpp

