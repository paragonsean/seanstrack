# Microsoft Developer Studio Project File - Name="serialterm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=serialterm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "serialterm.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "serialterm.mak" CFG="serialterm - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "serialterm - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE "serialterm - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "serialterm - Win32 UFree" (based on "Win32 (x86) External Target")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "serialterm - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "serialterm.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Free"
# PROP Intermediate_Dir "Free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "free\serialterm.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "serialterm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f debug.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "serialterm.exe"
# PROP BASE Bsc_Name "serialterm.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "serialterm.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "serialterm - Win32 UFree"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ufree"
# PROP BASE Intermediate_Dir "ufree"
# PROP BASE Cmd_Line "nmake /f "ufree.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "serialterm.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ufree"
# PROP Intermediate_Dir "ufree"
# PROP Cmd_Line "nmake /f "ufree.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "serialterm.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF

# Begin Target

# Name "serialterm - Win32 Free"
# Name "serialterm - Win32 Debug"
# Name "serialterm - Win32 UFree"

!IF  "$(CFG)" == "serialterm - Win32 Free"

!ELSEIF  "$(CFG)" == "serialterm - Win32 Debug"

!ELSEIF  "$(CFG)" == "serialterm - Win32 UFree"

!ENDIF

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\serial_monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\serialterm.cpp
# End Source File
# Begin Source File

SOURCE=.\serialterm.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resrc1.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\serialterm.ico
# End Source File
# End Group
# End Target
# End Project
