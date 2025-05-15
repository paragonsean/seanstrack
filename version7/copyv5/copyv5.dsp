# Microsoft Developer Studio Project File - Name="copyv5" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=copyv5 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "copyv5.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "copyv5.mak" CFG="copyv5 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "copyv5 - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "copyv5 - Win32 Free" (based on "Win32 (x86) External Target")
!MESSAGE "copyv5 - Win32 Ufree" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "copyv5 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f copyv5.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "copyv5.exe"
# PROP BASE Bsc_Name "copyv5.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "debug.mak""
# PROP Rebuild_Opt ""
# PROP Target_File "debug\copyv5.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "copyv5 - Win32 Free"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Free"
# PROP BASE Intermediate_Dir "Free"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt ""
# PROP BASE Target_File "free\copyv5.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Free"
# PROP Intermediate_Dir "Free"
# PROP Cmd_Line "nmake /f "free.mak""
# PROP Rebuild_Opt ""
# PROP Target_File "free\copyv5.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "copyv5 - Win32 Ufree"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ufree"
# PROP BASE Intermediate_Dir "ufree"
# PROP BASE Cmd_Line "nmake /f "free.mak""
# PROP BASE Rebuild_Opt ""
# PROP BASE Target_File "ufree\copyv5.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ufree"
# PROP Intermediate_Dir "ufree"
# PROP Cmd_Line "nmake /f "ufree.mak""
# PROP Rebuild_Opt ""
# PROP Target_File "ufree\copyv5.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "copyv5 - Win32 Debug"
# Name "copyv5 - Win32 Free"
# Name "copyv5 - Win32 Ufree"

!IF  "$(CFG)" == "copyv5 - Win32 Debug"

!ELSEIF  "$(CFG)" == "copyv5 - Win32 Free"

!ELSEIF  "$(CFG)" == "copyv5 - Win32 Ufree"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\copyv5.cpp
# End Source File
# Begin Source File

SOURCE=.\copyv5.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\msres.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\copyv5.ico
# End Source File
# End Group
# End Target
# End Project
