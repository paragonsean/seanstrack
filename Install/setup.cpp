#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\array.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\rectclas.h"
#include "..\include\setpoint.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\wclass.h"
#include "..\include\menuctrl.h"
#include "..\include\statics.h"

#include "resource.h"

#define _MAIN_
#include "..\include\chars.h"

/*
----------------------------
Definitions from profile.cpp
---------------------------- */
#define BOARD_MONITOR_MENU        703
#define MONITOR_CONFIG_MENU       704
#define SETUPS_MENU               707

const DWORD NEW_SYSTEM_HELP    = 0x0001;
const DWORD UPDATE_SYSTEM_HELP = 0x0002;
const DWORD CONTENTS_HELP      = 0x0003;

const COLORREF RedColor = RGB( 255, 0, 0 );

static HBRUSH WhiteBrush = 0;

#define MY_MAX_INI_STRING_LEN 255

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS CurrentDialogWindow;
WINDOW_CLASS ChoiceRect;            /* Rectangle around machine, E20, M2, or M20 */
HWND         StatusTextWindow = 0;
HWND         MachineTextBox   = 0;
HWND         PartTextBox      = 0;

COMPUTER_CLASS    Computer;
FONT_LIST_CLASS   FontList;
STRINGTABLE_CLASS StringTable;

TCHAR BackupString[]       = TEXT( "backup" );
TCHAR CommonResourceFile[] = TEXT( "common.stbl" );
TCHAR CurrentLanguageKey[] = TEXT( "CurrentLanguage" );
TCHAR CurrentString[]      = TEXT( "current" );           /* c:\autoback\current */
TCHAR DataString[]         = TEXT( "data" );
TCHAR ExesString[]         = TEXT( "exes" );
TCHAR ResourcesString[]    = TEXT( "resources" );
TCHAR LevelTenString[]     = TEXT( "10" );
TCHAR LevelZeroString[]    = TEXT( "0" );
TCHAR MyName[]             = TEXT( "VisiSetup" );
TCHAR NewEnd[]             = TEXT( ";0" );
TCHAR NString[]            = TEXT( "N" );
TCHAR PreImpactWork[]      = TEXT( "Pre-Impact Work" );
TCHAR SetupResourceFile[]  = TEXT( "setup.stbl" );
TCHAR TempName[]           = TEMP_GRAPHLST_DB;
TCHAR UnknownString[]      = UNKNOWN;
TCHAR YString[]            = TEXT( "Y" );

STRING_CLASS CurrentLanguage;
NAME_CLASS   OriginalComputerName;
NAME_CLASS   NewComputerName;
NAME_CLASS   RestorePointPath;

TCHAR BackupDirectoryName[MAX_PATH+1] = TEXT( "" );
TCHAR DataDirectoryName[MAX_PATH+1]   = TEXT( "" );
TCHAR ExeDirectoryName[MAX_PATH+1]    = TEXT( "" );
TCHAR FloppyDirectoryName[MAX_PATH+1] = TEXT( "" );         /* Location of installation files */

TCHAR CopyOfV5[]               = TEXT( "C:\\copy_of_v5" );
TCHAR V5Path[]                 = TEXT( "C:\\v5" );
TCHAR ChooseWiresBmpFile[]     = TEXT( "Choose-Wires.bmp" );
TCHAR CurrentDir[]             = TEXT( "current" );           /* c:\autoback\current */
TCHAR DisplayIniFile[]         = TEXT( "display.ini" );
TCHAR EventmanExeFile[]        = TEXT( "eventman.exe" );
TCHAR EditPartIniFileName[]    = TEXT( "editpart.ini" );
TCHAR EditPartExeFile[]        = TEXT( "editpart.exe" );
TCHAR FasTrak2ExeFile[]        = TEXT( "ft2.exe"     );
TCHAR FasTrak2IniFileName[]    = TEXT( "ftii.ini" );
TCHAR OldFt2MenuFile[]         = TEXT( "oldft2.mnu" );
TCHAR Ft2MenuFile[]            = TEXT( "ft2.mnu" );
TCHAR OldFt2TerminalMenuFile[] = TEXT( "oldft2terminal.mnu" );
TCHAR Ft2TerminalMenuFile[]    = TEXT( "ft2terminal.mnu" );
TCHAR FtiiMessageFileName[]    = TEXT( "ftii_control.msg" );   /* ftii control.msg in the install directory, copy to \v5\exes\control.msg */
TCHAR ProfileExeFile[]         = TEXT( "profile.exe" );
TCHAR MonEditExeFile[]         = TEXT( "monedit.exe" );
TCHAR VisiTrakIniFileName[]    = TEXT( "visitrak.ini" );
TCHAR VisiSuFileName[]         = TEXT( "Su.exe" );
TCHAR MonallIniFileName[]      = TEXT( "monall.ini" );
TCHAR MonallExeFile[]          = TEXT( "monall.exe" );
TCHAR PkzipExeFile[]           = TEXT( "pkzip25.exe" );
TCHAR OldVisiEditMenuFile[]    = TEXT( "oldvisiedit.mnu" );
TCHAR VisiEditMenuFile[]       = TEXT( "visiedit.mnu" );
TCHAR OldProfileMenuFile[]     = TEXT( "oldprofile.mnu" );
TCHAR ProfileMenuFile[]        = TEXT( "profile.mnu" );
TCHAR SetupIniFile[]           = TEXT( "setup.ini" );
TCHAR StdAutoBackPath[]        = TEXT( "d:\\autoback\\current" );
TCHAR StdAutoBackRoot[]        = TEXT( "d:\\autoback" );
TCHAR StartupListFile[]        = TEXT( "startup.lst" );
TCHAR DefaultSetupFile[]       = SETUP_SHEET_FILE;
TCHAR StatusScreenConfigFile[] = TEXT( "status_screen_config.dat" );

TCHAR BackupSection[]          = TEXT( "Backup" );
TCHAR ConfigSection[]          = TEXT( "Config" );
TCHAR ButtonLevelsSection[]    = TEXT( "ButtonLevels" );

TCHAR AutoBackupDirKey[]       = TEXT( "AutoBackupDir" );
TCHAR CurrentChannelModeKey[]  = TEXT( "CurrentChannelMode" );
TCHAR DowntimePurgeButtonKey[] = TEXT( "DowntimePurgeButton" );
TCHAR EditOperatorsButtonKey[] = TEXT( "EditOperatorsButton" );
TCHAR EditControlButtonKey[]   = TEXT( "EditControlButton" );
TCHAR NoNetworkKey[]           = TEXT( "NoNetwork" );
TCHAR ParameterTabLevelKey[]   = TEXT( "ParameterTabEditLevel" );
TCHAR PfsvDefSmoothNKey[]      = TEXT( " PfsvDefSmoothN");
TCHAR PfsvDefSmoothMultiplierKey[] = TEXT( "PfsvDefSmoothMultiplier");

TCHAR HaveSureTrakKey[]        = TEXT( "HaveSureTrak" );
TCHAR StartSureTrakKey[]       = TEXT( "StartSureTrak" );
TCHAR ThisComputerKey[]        = TEXT( "ThisComputer" );
TCHAR V5[]                     = TEXT( "V5" );
TCHAR V5DS[]                   = TEXT( "V5DS" );
TCHAR VersionKey[]             = TEXT( "Version" );
TCHAR XkeyTogglesX1X4Key[]     = TEXT( "XKeyTogglesX1X4" );

TCHAR ChannelMode7String[]     = TEXT( "7" );
TCHAR ChanPreConfigKey[]       = TEXT( "ChanPreConfig" );
TCHAR ChanPostConfigKey[]      = TEXT( "ChanPostConfig" );
TCHAR Ft2ChanConfig[]          = TEXT( "1400" );

BOOLEAN NeedPartEditor            = FALSE;
BOOLEAN NeedMonitorConfig         = FALSE;
BOOLEAN NeedMonitorStartup        = FALSE;
BOOLEAN NeedStartMenuEntry        = FALSE;
BOOLEAN NeedToCopyFt2ControlFiles = FALSE;
BOOLEAN HaveSureTrak              = FALSE;
BOOLEAN NeedToConvertToSt2        = FALSE;
BOOLEAN IsNT                      = FALSE;
BOOLEAN NeedToCopy                = FALSE;
BOOLEAN Ft2ControlAllFileIsOld    = FALSE;
BOOLEAN Ft2ControlFilesAreMissing = FALSE;
BOOLEAN Ft2ControlFilesAreOld     = FALSE;
BOOLEAN UsingRestorePoint         = FALSE;

static TCHAR * SorcControlFiles[] = {
    { TEXT( "e20.prg" ) },
    { TEXT( "m2.prg" ) },
    { TEXT( "m20.prg" ) }
    };

static TCHAR * SorcMessageFiles[] = {
    { TEXT( "e20.msg" ) },
    { TEXT( "m2.msg" ) },
    { TEXT( "m20.msg" ) }
    };

static TCHAR * SorcParamFiles[] = {
    { TEXT( "e20.dat" ) },
    { TEXT( "m2.dat" ) },
    { TEXT( "m20.dat" ) }
    };

static TCHAR * SorcDirNames[] = {
    { TEXT( "e20" ) },
    { TEXT( "m2" ) },
    { TEXT( "m20" ) }
    };

static int32 StSorcIndex = 0;
static const int32 NofStSorcFiles = 3;

static TCHAR * Ft2ControlFile[] = {
    { TEXT("control4m.txt") },
    { TEXT("control4c.txt") },
    { TEXT("control5m.txt") },
    { TEXT("control5c.txt") },
    { TEXT("control5p.txt") },
    { TEXT("control5e.txt") },
    { TEXT("control_all.txt") },
    { TEXT("control_all.msg") }
    };
static int NofFt2ControlFiles = 8;

/*
---------------------------------
Main Version Number of old system
--------------------------------- */
int OldVersion = 5;

static int NO_COMPUTER_TYPE = 0;

#define MONITORING_COMPUTER_TYPE 1
#define DATA_ARCHIVER_TYPE       2
#define OFFICE_WORKSTATION_TYPE  3
#define NO_NETWORK_TYPE          4
#define NOF_COMPUTER_TYPES       5

static int ComputerType = NO_COMPUTER_TYPE;

static TCHAR LinkDesc[] = TEXT( "TrueTrak 2020 Monitoring System" );
static TCHAR LinkFile[] = TEXT( "TrueTrak 2020 Monitoring System.lnk" );

BOOLEAN menu_update( TCHAR * sorc_file_name, TCHAR * dest_file_name, TCHAR * directory_name );
BOOLEAN set_menu_password_level( TCHAR * dest_file_name, int32 id_to_change, int new_password_level );

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                            ST_MESSAGE_BOX                            *
***********************************************************************/
int st_message_box( TCHAR * msg_id, TCHAR * title_id, UINT boxtype )
{
STRING_CLASS msg;
STRING_CLASS title;

msg   = StringTable.find( msg_id );
title = StringTable.find( title_id );
return resource_message_box( msg.text(), title.text(), boxtype );
}

/***********************************************************************
*                             ST_MESSAGE_BOX                           *
***********************************************************************/
void st_message_box( TCHAR * msg_id, TCHAR * title_id )
{
STRING_CLASS msg;
STRING_CLASS title;

msg   = StringTable.find( msg_id );
title = StringTable.find( title_id );
resource_message_box( msg.text(), title.text() );
}

/***********************************************************************
*                         MY_GET_INI_STRING                            *
***********************************************************************/
TCHAR * my_get_ini_string( TCHAR * fname, TCHAR * section, TCHAR * key )
{
static TCHAR Buf[MY_MAX_INI_STRING_LEN+1];

NAME_CLASS s;

*Buf = 0;

s = ExeDirectoryName;
s.cat_path( fname );
GetPrivateProfileString( section, key, UnknownString, Buf, MY_MAX_INI_STRING_LEN, s.text() );

return Buf;
}

/***********************************************************************
*                          MY_PUT_INI_STRING                           *
***********************************************************************/
void my_put_ini_string( TCHAR * fname, TCHAR * section, TCHAR * key, TCHAR * sorc )
{
TCHAR path[MAX_PATH+1];

lstrcpy( path, ExeDirectoryName );
append_filename_to_path( path, fname );

WritePrivateProfileString( section, key, sorc, path );
}

/***********************************************************************
*                            WAIT_FOR_FILE                             *
*        Wait for a file to appear (e.g. after CopyFile).              *
***********************************************************************/
BOOLEAN wait_for_file( TCHAR * filename )
{
int count;

count = 10;
while ( true )
    {
    if ( file_exists(filename) )
        break;

    count--;
    if ( count == 0 )
        return FALSE;

    Sleep( 500 );
    }

return TRUE;
}

/***********************************************************************
*                            WAIT_FOR_FILE                             *
***********************************************************************/
BOOLEAN wait_for_file( NAME_CLASS & filename )
{
return wait_for_file( filename.text() );
}

/***********************************************************************
*                            IS_SURETRAK                               *
*        Get the HaveSureTrak value from the visitrak.ini file.        *
***********************************************************************/
static BOOLEAN is_suretrak( STRING_CLASS & computer )
{
STRING_CLASS s;
INI_CLASS    ini;
BOOLEAN      is_suretrak;

is_suretrak = FALSE;

s = computer;
s.cat_path( ExesString );
s.cat_path( VisiTrakIniFileName );

ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(HaveSureTrakKey) )
    is_suretrak = ini.get_boolean();

return is_suretrak;
}

/***********************************************************************
*                            COMPUTER_TYPE                             *
* Figure out what kind of computer this is: monitoring computer,       *
* office workstation, data archiver, or no network (local on DA).      *
***********************************************************************/
static int computer_type( STRING_CLASS & computer )
{
INI_CLASS               ini;
NAME_CLASS              s;
MACHINE_NAME_LIST_CLASS mlist;
DB_TABLE                t;
TEXT_LIST_CLASS         tlist;
TCHAR                 * cp;
int                     count;

if ( computer.contains(V5DS) )
    return DATA_ARCHIVER_TYPE;

count = 0;
cp = my_get_ini_string( VisiTrakIniFileName, ConfigSection, VersionKey );
if ( *cp == '6' )
    {
    s = computer;
    s.cat_path( DataString );
    tlist.get_directory_list( s.text() );
    if ( tlist.count() > 0 )
        {
        tlist.rewind();
        tlist.next();

        s.cat_path( tlist.text() );
        s.cat_path( MACHSET_DB );
        if ( t.open(s, MACHSET_RECLEN, PFL) )
            {
            count = t.nof_recs();
            t.close();
            }
        }
    }
else
    {
    mlist.add_computer( computer );
    count = mlist.count();
    }

if ( count < 1 )
    return OFFICE_WORKSTATION_TYPE;

s.get_exe_dir_file_name( computer, VisiTrakIniFileName );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.get_boolean(NoNetworkKey) )
    return NO_NETWORK_TYPE;

return MONITORING_COMPUTER_TYPE;
}

/***********************************************************************
*                SET_GLOBAL_COMPUTER_TYPE_FROM_BUTTONS                 *
* The computer type is now set by pressing one of four radio buttons.  *
***********************************************************************/
static void set_global_computer_type_from_buttons( HWND w )
{
if ( is_checked(w, MONITORING_COMPUTER_RADIO) )
    ComputerType = MONITORING_COMPUTER_TYPE;
else if ( is_checked(w,  DATA_ARCHIVER_RADIO) )
    ComputerType = DATA_ARCHIVER_TYPE;
else if ( is_checked(w, OFFICE_WORKSTATION_RADIO) )
    ComputerType = OFFICE_WORKSTATION_TYPE;
else if ( is_checked(w, NO_NETWORK_RADIO) )
    ComputerType = NO_NETWORK_TYPE;
else
    ComputerType = NO_COMPUTER_TYPE;
}

/***********************************************************************
*                                                                      *
*                             CREATE_LINK                              *
*                                                                      *
*  Uses the shell's IShellLink and IPersistFile interfaces             *
*  to create and store a shortcut to the specified object.             *
*                                                                      *
*  Returns the result of calling the member functions of the interfaces*
*                                                                      *
*  lpszPathObj - address of a buffer containing the path of the object *
*  lpszPathLink - address of a buffer containing the path where the    *
*    shell link is to be stored                                        *
*                                                                      *
*  lpszDesc - address of a buffer containing the description of the    *
*    shell link                                                        *
*                                                                      *
***********************************************************************/
HRESULT create_link( TCHAR * exe_file_name, TCHAR * link_file_name, TCHAR * link_description )
{
HRESULT        hres;
IPersistFile * ppf;
IShellLink   * psl;
WCHAR          ws[MAX_PATH];

/*
------------------------------------------
Get a pointer to the IShellLink interface.
------------------------------------------ */
hres = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**) &psl );
if ( SUCCEEDED(hres) )
    {

    /*
    -------------------------------------------------------------
    Set the path to the shortcut target, and add the description.
    ------------------------------------------------------------- */
    psl->SetPath( exe_file_name );
    psl->SetDescription( link_description );

    /*
    --------------------------------------------------------------
    Query IShellLink for the IPersistFile interface for saving the
    shortcut in persistent storage.
    -------------------------------------------------------------- */
    hres = psl->QueryInterface( IID_IPersistFile, (void**) &ppf );

    if ( SUCCEEDED(hres) )
        {

        /*
        -------------------------------
        Ensure that the string is ANSI.
        ------------------------------- */
        #ifdef UNICODE
            lstrcpy( ws, link_file_name );
        #else
            MultiByteToWideChar(CP_ACP, 0, link_file_name, -1, ws, MAX_PATH);
        #endif

        /*
        --------------------------------------------
        Save the link by calling IPersistFile::Save.
        -------------------------------------------- */
        hres = ppf->Save( ws, TRUE );
        ppf->Release();
        }

    psl->Release();
    }

return hres;
}

/***********************************************************************
*                             SHOW_STATUS                              *
***********************************************************************/
void show_status( TCHAR * id )
{
HWND w;

w = CurrentDialogWindow.handle();
if ( w )
    set_text( w, STATUS_MSG_TEXTBOX, StringTable.find(id) );
}

/***********************************************************************
*                          SHOW_UPDATE_STATUS                          *
***********************************************************************/
static void show_update_status( TCHAR * id )
{
set_text( StatusTextWindow, StringTable.find(id) );
}

/***********************************************************************
*                           IS_EXE_DIRECTORY                           *
***********************************************************************/
static BOOL is_exe_directory( TCHAR * exe_dir )
{
NAME_CLASS s;

s = exe_dir;
s.cat_path( VisiTrakIniFileName );
return s.file_exists();
}

/***********************************************************************
*                            UPDATE_DCURVE                             *
*   3. If there is no alumax work equation, add it to DCURVE.TXT.      *
***********************************************************************/
void update_dcurve()
{
NAME_CLASS s;
DB_TABLE   t;
int        highest_curve_number;
BOOLEAN    have_work;
TCHAR      desc[DCURVE_DESC_LEN+1];

have_work = FALSE;
highest_curve_number = 0;

s.get_data_dir_file_name( DCURVE_DB );
if ( t.open(s, DCURVE_RECLEN, PWL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        maxint( highest_curve_number, (int) t.get_short(DCURVE_CURVE_NUMBER_OFFSET) );
        t.get_alpha( desc, DCURVE_DESC_OFFSET, DCURVE_DESC_LEN );
        if ( lstrcmp(desc, PreImpactWork) == 0 )
            {
            have_work = TRUE;
            break;
            }
        }

    if ( !have_work )
        {
        t.put_short( DCURVE_CURVE_NUMBER_OFFSET, (short) (highest_curve_number + 1), DCURVE_NUMBER_LEN );
        t.put_alpha( DCURVE_DESC_OFFSET, PreImpactWork, DCURVE_DESC_LEN );
        t.put_float( DCURVE_HEAD_PRES_OFFSET, (float) 1.0, DCURVE_COEF_LEN );
        t.put_float( DCURVE_ROD_PRES_OFFSET,  (float) 1.0, DCURVE_COEF_LEN );
        t.rec_append();
        }

    t.close();
    }
}

/***********************************************************************
*                           UPDATE_PARMNAME                            *
*      If there is no PreImpact Work in PARMNAME.TXT, add it.          *
***********************************************************************/
void update_parmname()
{
NAME_CLASS s;
DB_TABLE   t;
short      parameter_number;

parameter_number = PRE_IMPACT_WORK + 1;

s.get_data_dir_file_name( PARMNAME_DB );
if ( t.open(s, PARMNAME_RECLEN, PWL) )
    {
    t.put_short( PARMNAME_NUMBER_OFFSET, parameter_number, PARAMETER_NUMBER_LEN );
    if ( !t.get_next_key_match( 1, NO_LOCK) )
        {
        t.put_long( PARMNAME_MACH_TYPE_OFFSET, long(COLD_CHAMBER_TYPE) | long(HOT_CHAMBER_TYPE), BITSETYPE_LEN );
        t.put_alpha( PARMNAME_NAME_OFFSET,     PreImpactWork,   PARAMETER_NAME_LEN );
        t.put_long( PARMNAME_TYPE_OFFSET,      WORK_VAR,                                         BITSETYPE_LEN );
        t.rec_append();
        }

    t.close();
    }
}

/***********************************************************************
*                             FILE_IS_NEWER                            *
***********************************************************************/
static bool file_is_newer( STRING_CLASS & current_file_name, STRING_CLASS & new_file_name )
{
FILETIME current_file_time;
FILETIME new_file_time;
FILE_CLASS f;

/*
------------------------------------------------------
If there is no current file then the new file is newer
------------------------------------------------------ */
if ( !f.open_for_read(current_file_name) )
    return true;

f.get_creation_time( &current_file_time );
f.close();

/*
------------------------------------------------------
If there is no new file then the current file is newer
------------------------------------------------------ */
if ( !f.open_for_read(new_file_name) )
    return false;
f.get_creation_time( &new_file_time );
f.close();

if ( CompareFileTime(&current_file_time, &new_file_time) < 0 )
    return true;

return false;
}

/***********************************************************************
*                             UPDATE_UNITS                             *
*      Copy the units.txt and vartype.txt if they are newer.           *
***********************************************************************/
void update_units()
{
NAME_CLASS current_file;
NAME_CLASS new_file;

current_file.get_data_dir_file_name( UNITS_DB );
new_file = FloppyDirectoryName;
new_file.cat_path( UNITS_DB );
if ( file_is_newer(current_file, new_file) )
    new_file.copyto( current_file );

current_file.get_data_dir_file_name( VARTYPE_DB );
new_file = FloppyDirectoryName;
new_file.cat_path( VARTYPE_DB );
if ( file_is_newer(current_file, new_file) )
    new_file.copyto( current_file );
}

/***********************************************************************
*                           UPDATE_RODPITCHES                          *
***********************************************************************/
static void update_rodpitches()
{
static TCHAR newpitch[] = TEXT( "1.6" );

DB_TABLE     dest;
DB_TABLE     sorc;
NAME_CLASS   s;
BOOLEAN      need_to_insert;

s.get_rodpitch_dbname();
if ( dest.open(s, RODPITCH_RECLEN, FL) )
    {
    dest.put_alpha( RODPITCH_NAME_OFFSET, newpitch, RODPITCH_NAME_LEN );
    dest.reset_search_mode();
    if ( !dest.get_next_key_match(1, FALSE) )
        {
        dest.reset_search_mode();
        if ( dest.goto_first_greater_than_record(1) )
            dest.rec_insert();
        else
            dest.rec_append();
        }
    dest.close();
    }


s.get_psensor_dbname();
if ( dest.open(s, PSENSOR_RECLEN, FL) )
    {
    s = FloppyDirectoryName;
    s.cat_path( PSENSOR_DB );
    if ( sorc.open(s, PSENSOR_RECLEN, FL) )
        {
        sorc.put_alpha( PSENSOR_RODPITCH_OFFSET, newpitch, RODPITCH_NAME_LEN );
        sorc.reset_search_mode();
        /*
        ------------------------------------------
        Copy any records not in the existing table
        ------------------------------------------ */
        while ( sorc.get_next_key_match(1, FALSE) )
            {
            dest.copy_rec( sorc );
            dest.rewind();
            dest.reset_search_mode();
            if ( !dest.get_next_key_match(4, FALSE) )
                {
                need_to_insert = dest.goto_first_greater_than_record( 4 );
                /*
                -----------------------------------------------------------------
                If there is a record I load it so I need to recopy the new record
                ----------------------------------------------------------------- */
                dest.copy_rec( sorc );
                if ( need_to_insert )
                    dest.rec_insert();
                else
                    dest.rec_append();
                }
            }
        sorc.close();
        }
    dest.close();
    }
}

/***********************************************************************
*                       CREATE_STARTUP_LIST_FILE                       *
***********************************************************************/
static void create_startup_list_file()
{
static TCHAR ft2_delay_string[] = TEXT( "2000" );
static TCHAR ms_delay_string[]  = TEXT( "1000" );

TCHAR   sm1[MAX_PATH+1];
TCHAR * s;
TCHAR * fname;
FILE_CLASS f;

/*
------------------------------------------------------
A minus at the start of the line means start minimized
------------------------------------------------------ */
*sm1 = TEXT( '-' );
s = sm1 + 1;

lstrcpy( s, ExeDirectoryName );
append_backslash_to_path( s );
fname = s + lstrlen( s );

lstrcpy( fname, StartupListFile );

if ( f.open_for_write(s) )
    {
    lstrcpy( fname, EventmanExeFile );
    f.writeline( s );

    f.writeline( ms_delay_string );

    lstrcpy( fname, ProfileExeFile );
    f.writeline( s );

    if ( NeedPartEditor )
        {
        lstrcpy( fname, EditPartExeFile );
        f.writeline( s );
        }

    if ( NeedMonitorConfig )
        {
        lstrcpy( fname, MonEditExeFile );
        f.writeline( s );
        }

    if ( NeedMonitorStartup && (ComputerType != OFFICE_WORKSTATION_TYPE) )
        {
        /*
        -------------------------------
        Give the FasTrak2 time to start
        ------------------------------- */
        f.writeline( ft2_delay_string );

        lstrcpy( fname, MonallExeFile );
        f.writeline( sm1 );
        }

    f.close();
    }
}

/***********************************************************************
*                  SET_CHANGE_TO_SETUP_STRINGS                    *
***********************************************************************/
static void set_change_to_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,                   TEXT("CHANGE_MA_TO_ST2") },
    { IDCANCEL,               TEXT("DO_NOTHING") },
    { IS_CTRL_STATIC,         TEXT("IS_CTRL") },
    { MARK_AS_CTRL_STATIC,    TEXT("MARK_CTRL") },
    { SHOULD_SAVE_ALL_STATIC, TEXT("SHOULD_SAVE_ALL") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, TEXT("MA_TYPE_CHANGE") );
}

/***********************************************************************
*                  CHANGE_TO_CONTROL_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK change_to_control_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_change_to_setup_strings( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                EndDialog( hWnd, 1 );
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      CREATE_STD_AUTO_BACKUP_DIR                      *
***********************************************************************/
static void create_std_auto_backup_dir()
{
NAME_CLASS s;
s = StdAutoBackRoot;
if ( !s.directory_exists() )
    s.create_directory();

s = StdAutoBackPath;
if ( !s.directory_exists() )
    s.create_directory();

if ( s.directory_exists() )
    my_put_ini_string( VisiTrakIniFileName, BackupSection, AutoBackupDirKey, StdAutoBackPath );
}

/***********************************************************************
*                    SET_AUTO_BU_SETUP_STRINGS                         *
***********************************************************************/
static void set_auto_bu_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK, TEXT("YES_STRING") },
    { IDCANCEL, TEXT("NO_STRING") },
    { AUTO_BU_STATIC, TEXT("AUTO_BU") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, NULL );
}

/***********************************************************************
*                       STD_AUTO_BACKUP_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK std_auto_backup_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_auto_bu_setup_strings( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                create_std_auto_backup_dir();
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                       CHECK_CONTROL_FILE_DATES                       *
***********************************************************************/
static void check_control_file_dates()
{
FILETIME current_file_time;
FILETIME new_file_time;
FILE_CLASS f;

int i;
TCHAR           sorc[MAX_PATH+1];
TCHAR           dest[MAX_PATH+1];
TCHAR         * sorcfile;
TCHAR         * destfile;

sorcfile = copystring( sorc, FloppyDirectoryName  );
destfile = copystring( dest, ExeDirectoryName );

Ft2ControlFilesAreMissing = FALSE;
Ft2ControlFilesAreOld     = FALSE;
Ft2ControlAllFileIsOld    = FALSE;

for ( i=0; i<NofFt2ControlFiles; i++ )
    {
    copystring( sorcfile, Ft2ControlFile[i] );
    copystring( destfile, sorcfile );
    if ( f.open_for_read(dest) )
        {
        f.get_creation_time( &current_file_time );
        f.close();
        if ( f.open_for_read(sorc) )
            {
            f.get_creation_time( &new_file_time );
            f.close();
            if ( CompareFileTime(&current_file_time, &new_file_time) < 0 )
                {
                Ft2ControlFilesAreOld = TRUE;
                if ( strings_are_equal(sorcfile, FT2_CONTROL_ALL_FILE))
                    Ft2ControlAllFileIsOld = TRUE;
                }
            }
        }
    else
        {
        Ft2ControlFilesAreMissing = TRUE;
        }
    }

/*
-------------------------------------------------------------------------
I used to turn this on if there were files to copy but now that is
optional and is only turned on if the copy ft2 files checkbox is checked.
------------------------------------------------------------------------- */
NeedToCopyFt2ControlFiles = FALSE;
}

/***********************************************************************
*                  UPDATE_DEFAULT_BINARY_VALVE_FILE                    *
***********************************************************************/
static void update_default_binary_valve_file()
{
NAME_CLASS current_file;
NAME_CLASS new_file;

current_file.get_default_hmi_settings_csvname();
new_file = FloppyDirectoryName;
new_file.cat_path( DEFAULT_HMI_SETTINGS_CSV );
if ( file_is_newer(current_file, new_file) )
    new_file.copyto( current_file );
}

/**********************************************************************
*                     copy_resources_directory                        *
**********************************************************************/
void copy_resources_directory()
{
static TCHAR WildStringtableName[] = TEXT("*") STRINGTABLE_NAME_SUFFIX;
NAME_CLASS s;
NAME_CLASS d;
TEXT_LIST_CLASS t;

/*
------------------------------------------
Make sure there is a destination directory
------------------------------------------ */
d = ExeDirectoryName;
d.cat_path( ResourcesString );
if ( !d.directory_exists() )
    d.create_directory();

/*
------------------------
Make a list of the files
------------------------ */
s = FloppyDirectoryName;
s.cat_path( ResourcesString );
t.get_file_list( s.text(), WildStringtableName );

t.rewind();
while ( t.next() )
    {
    d = ExeDirectoryName;
    d.cat_path( ResourcesString );
    d.cat_path( t.text() );
    s = FloppyDirectoryName;
    s.cat_path( ResourcesString );
    s.cat_path( t.text() );
    s.copyto( d );
    }
}

/***********************************************************************
*                             COPY_NEW_DBS                             *
*              This is only called by the update thread.               *
***********************************************************************/
void copy_new_dbs( int subversion )
{
NAME_CLASS      sorc;
NAME_CLASS      dest;
TCHAR         * sorcfile;
BOOLEAN         firstime;
BOOLEAN         need_downtime_db;
int             i;
int             board_number;
int             nof_machines;
INT_ARRAY_CLASS ft2boards;

STRING_CLASS computer;
STRING_CLASS machine;
STRING_CLASS part;
MACHINE_NAME_LIST_CLASS mlist;
SETPOINT_CLASS          sp;
TEXT_LIST_CLASS         partlist;
DB_TABLE t;
DB_TABLE p;

firstime = TRUE;  /* Used to ask for update to control */

sorc.init( MAX_PATH );
sorc = FloppyDirectoryName;

sorcfile = sorc.text() + sorc.len();

/*
----------------------------------
Make sure there is a password file
---------------------------------- */
dest.get_new_password_dbname();
if ( !dest.file_exists() )
    {
    dest.get_data_dir_file_name( PASSWORD_DB );
    if ( !dest.file_exists() )
        {
        copystring( sorcfile, PASSWORD_DB );
        sorc.copyto( dest );
        }
    }

/*
----------------
And a density db
---------------- */
dest.get_data_dir_file_name( DENSITY_DB );
if ( !dest.file_exists() )
    {
    copystring( sorcfile, DENSITY_DB );
    sorc.copyto( dest );
    }

/*
-----------------------------------------------------
Copy the FT2 Control Program Files (there are 5 now )
----------------------------------------------------- */
if ( NeedToCopyFt2ControlFiles )
    {
    for ( i=0; i<NofFt2ControlFiles; i++ )
        {
        copystring( sorcfile, Ft2ControlFile[i] );
        dest = ExeDirectoryName;
        dest.cat_path( Ft2ControlFile[i] );
        sorc.copyto( dest );
        }
    }

/*
--------------------------------------------------------------------------------------------
Make sure there is a SureTrak2 message file
The actual message file has the same name as the old message file but is in the exes folder.
I use a different name on the install directory because the old one is there too.
I always copy this now (as of V6.23).
-------------------------------------------------------------------------------------------- */
dest = ExeDirectoryName;
dest.cat_path( CONTROL_MESSAGE_FILE );
copystring( sorcfile, FtiiMessageFileName );
sorc.copyto( dest );

/*
--------------------------------------
Make sure there is a FasTrak2 ini file
I always copy this now.
-------------------------------------- */
copystring( sorcfile, FasTrak2IniFileName );

dest = ExeDirectoryName;
dest.cat_path( FasTrak2IniFileName );
sorc.copyto( dest );

computer = my_get_ini_string( VisiTrakIniFileName, ConfigSection, ThisComputerKey );

/*
--------------------------------------------------------------
Copy the default suretrak control messages if there is no file
-------------------------------------------------------------- */
dest.get_exe_dir_file_name( CONTROL_MESSAGE_FILE );
if ( !dest.file_exists() )
    {
    copystring( sorcfile, CONTROL_MESSAGE_FILE );
    sorc.copyto( dest );
    }

/*
----------------------------------------------------
Copy the ft2 variable names file if there is no file
---------------------------------------------------- */
dest.get_exe_dir_file_name( VARIABLE_LIST_CSV_FILE );
if ( !dest.file_exists() )
    {
    copystring( sorcfile, VARIABLE_LIST_CSV_FILE );
    sorc.copyto( dest );
    }

/*
---------------------------------------------------
Make sure there is a global suretrak parameter file
--------------------------------------------------- */
dest.get_stparam_file_name( computer );
if ( !dest.file_exists() )
    {
    copystring( sorcfile, STPARAM_FILE );
    sorc.copyto( dest );
    }
/*
----------------------------------
Make a list of the fastrak2 boards
---------------------------------- */
sorc.get_ft2_boards_dbname( computer );
if ( sorc.file_exists() )
    {
    if ( t.open(sorc, FTII_BOARDS_RECLEN, PFL) )
        {
        i = 0;
        while ( t.get_next_record(FALSE) )
            {
            ft2boards[i] = t.get_short( FTII_BOARDS_NUMBER_OFFSET );
            i++;
            }
        t.close();
        }
    }

mlist.add_computer( computer );
nof_machines = mlist.count();
mlist.rewind();
while ( mlist.next() )
    {
    need_downtime_db = FALSE;
    machine = mlist.name();
    dest.get_machset_csvname( computer, machine );
    sp.get( dest );
    if ( HaveSureTrak && (ComputerType==MONITORING_COMPUTER_TYPE) )
        {
        board_number = (int) sp[MACHSET_FT_BOARD_NO_INDEX].value.int_value();
        if ( ft2boards.contains(board_number) )
            {
            if ( !sp[MACHSET_ST_BOARD_INDEX].value.boolean_value() )
                {
                if ( firstime )
                    if ( DialogBox(MainInstance, TEXT("CHANGE_TO_CONTROL_DIALOG"), MainWindow.handle(), (DLGPROC) change_to_control_dialog_proc) == 1 )
                        NeedToConvertToSt2 = TRUE;
                firstime = FALSE;
                if ( NeedToConvertToSt2 )
                    {
                    sp[MACHSET_ST_BOARD_INDEX].value = (BOOLEAN) TRUE;
                    sp.put( dest );
                    }
                }
            }
        }

    /*
    -----------------------------------------------------
    Create a machine results directory if there isn't one
    ----------------------------------------------------- */
    dest.get_machine_results_directory( computer, machine );
    if ( !dest.directory_exists() )
        {
        dest.create_directory();
        need_downtime_db = TRUE;
        }

    if ( need_downtime_db )
        {
        dest.get_downtime_dbname( computer, machine );
        copystring( sorcfile, DOWNTIME_DB );
        sorc.copyto( dest );
        }

    /*
    ------------------------------------
    Make sure each part has suretrak dbs
    ------------------------------------ */
    if ( get_partlist(partlist, computer, machine) )
        {
        while ( partlist.next() )
            {
            part = partlist.text();

            /*
            ----------------------------------------------------------------
            If there is no part setup file then this is not a part directory
            ---------------------------------------------------------------- */
            dest.get_partset_csvname( computer, machine, part );
            if ( !dest.file_exists() )
                continue;

            /*
            ----------------------
            SureTrak profile steps
            ---------------------- */
            dest.get_ststeps_dbname( computer, machine, part );
            if ( !dest.file_exists() )
                {
                copystring( sorcfile, STSTEPS_DB );
                sorc.copyto( dest );
                }

            /*
            -------------------------------
            SureTrak limit switch positions
            ------------------------------- */
            dest.get_stlimits_dbname( computer, machine, part );
            if ( !dest.file_exists() )
                {
                copystring( sorcfile, STLIMITS_DB );
                sorc.copyto( dest );
                }
            }
        }
    }

create_startup_list_file();
}

/***********************************************************************
*                      UPDATE_SURETRAK_AND_VERSION                     *
***********************************************************************/
void update_suretrak_and_version( BOOLEAN updating )
{
BOOLEAN                 b;
INI_CLASS               ini;
INI_CLASS               esini;
MACHINE_NAME_LIST_CLASS mn;
MACHINE_CLASS           m;
NAME_CLASS              d;
NAME_CLASS              s;
FILE_CLASS              f;

s = ExeDirectoryName;
s.cat_path( VisiTrakIniFileName );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
ini.put_string( VersionKey, resource_string(CURRENT_VERSION_STRING) );

/*
---------------------------------------------------------------------------------------------
If I am updating from a monitor to a control I should change the current machine to a control
--------------------------------------------------------------------------------------------- */
if ( updating )
    {
    b = ini.get_boolean( HaveSureTrakKey );
    if ( HaveSureTrak & !b )
        {
        mn.add_computer( Computer.whoami() );
        mn.rewind();
        while ( mn.next() )
            {
            if ( m.find(mn.computer_name(), mn.name()) )
                {
                if ( (m.monitor_flags & MA_MONITORING_ON) && m.is_ftii )
                    {
                    if ( NeedToConvertToSt2 )
                        {
                        m.suretrak_controlled = TRUE;
                        m.monitor_flags |= MA_HAS_DIGITAL_SERVO;
                        m.save();
                        s.get_ft2_editor_settings_file_name( m.computer, m.name );
                        esini.set_file( s.text() );
                        esini.set_section( ConfigSection );
                        esini.put_string( CurrentChannelModeKey, ChannelMode7String );
                        s.get_auto_save_all_parts_file_name();
                        f.open_for_write( s );
                        f.writeline( TEXT("This file will be deleted by Part Setup the next time it runs.") );
                        f.close();
                        }
                    /*
                    --------------------------------------------------------------------------------------
                    Copy the following files in any case as they won't hurt anything if it is monitor only
                    -------------------------------------------------------------------------------------- */
                    d.get_ft2_editor_settings_file_name( m.computer, m.name );
                    if ( !d.file_exists() )
                        {
                        s = exe_directory();
                        s.cat_path( FTII_EDITOR_SETTINGS_FILE );
                        CopyFile( s.text(), d.text(), FALSE );
                        }
                    d.get_ft2_global_settings_file_name( m.computer, m.name );
                    if ( !d.file_exists() )
                        {
                        s = exe_directory();
                        s.cat_path( FTII_GLOBAL_SETTINGS_FILE );
                        CopyFile( s.text(), d.text(), FALSE );
                        }
                    }
                }
            }
        }
    }

if ( !HaveSureTrak )
    {
    /*
    --------------------------------------------
    Make sure no machines are marked as controls
    -------------------------------------------- */
    mn.add_computer( Computer.whoami() );
    mn.rewind();
    while ( mn.next() )
        {
        if ( m.find(mn.computer_name(), mn.name()) )
            {
            if ( m.suretrak_controlled )
                {
                m.suretrak_controlled = FALSE;
                m.save();
                }
            }
        }
    }

ini.put_boolean( HaveSureTrakKey, HaveSureTrak );
}

/***********************************************************************
*                         BOOT_DRIVE_LETTER                            *
***********************************************************************/
TCHAR boot_drive_letter()
{
TCHAR path[MAX_PATH+1];
int   slen;
TCHAR c;

c    = CChar;
slen = GetSystemDirectory( path, MAX_PATH );
if ( slen > 0 )
    c = *path;

return c;
}

/***********************************************************************
*                          COPY_HELP_FILE                              *
***********************************************************************/
static void copy_help_file()
{
const TCHAR help_file_name[]       = TEXT( "v5help" );
const TCHAR cnt_file_name[]        = TEXT( "v5help.cnt" );
const TCHAR gid_file_name[]        = TEXT( "v5help.gid" );
const TCHAR zip_extension[]        = TEXT( ".zip" );

TCHAR dest[MAX_PATH+1];
TCHAR s[MAX_PATH+1];
TCHAR * cp;

lstrcpy( s, FloppyDirectoryName );
lstrcat( s, help_file_name );

lstrcat( s, zip_extension );

show_status( TEXT("COPY_HELP") );

if ( !file_exists(s) )
    {
    MessageBox( 0, s, StringTable.find(TEXT("NO_FILE")), MB_OK | MB_SYSTEMMODAL );
    return;
    }

/*
--------------------------------
Delete the old cnt and gid files
-------------------------------- */
lstrcpy( s, ExeDirectoryName );
append_filename_to_path( s, cnt_file_name );
DeleteFile( s );

lstrcpy( s, ExeDirectoryName );
append_filename_to_path( s, gid_file_name );
DeleteFile( s );

/*
-----------------------
Unzip the new help file
----------------------- */
lstrcpy( s, ExeDirectoryName );
append_filename_to_path( s, TEXT("pkzip25 -ext -over=all ") );
cp = s + lstrlen(s);
lstrcpy( cp, FloppyDirectoryName );

lstrcat( s, help_file_name );

copystring( dest, ExeDirectoryName );
execute_and_wait( s, dest );
}

/***********************************************************************
*                          KILL_MONITOR_FILES                          *
* If this is an office workstation then I don't want the monitor exes  *
* at all.                                                              *
***********************************************************************/
static void kill_monitor_files()
{
TCHAR s[MAX_PATH+1];
TCHAR * cp;

copystring( s, ExeDirectoryName );
append_backslash_to_path( s );

cp = s + lstrlen(s);

copystring( cp, MonallExeFile );
DeleteFile( s );

copystring( cp, FasTrak2ExeFile );
DeleteFile( s );
}

/***********************************************************************
*                            CHOOSE_FTII                               *
***********************************************************************/
static void choose_ftii()
{
TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];
TCHAR * cp;

copystring( dest, ExeDirectoryName );
append_backslash_to_path( dest );

cp = dest + lstrlen(dest);

copystring( cp, MonallExeFile );
DeleteFile( dest );

copystring( sorc, ExeDirectoryName );
append_filename_to_path( sorc, FasTrak2ExeFile );
MoveFile( sorc, dest );
}

/***********************************************************************
*                           GET_FOLDER_PATH                            *
***********************************************************************/
bool get_folder_path( NAME_CLASS & dest, TCHAR * local_machine_key, TCHAR * current_user_key )
{
static TCHAR shell_folder_key[] = TEXT( "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders" );

HKEY  rh;
DWORD type;
ULONG buf_size;
bool  have_key;
TCHAR buf[MAX_PATH+1];

have_key = false;
*buf = NullChar;

if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, shell_folder_key, 0, KEY_QUERY_VALUE, &rh) == ERROR_SUCCESS )
    {
    buf_size = MAX_PATH * sizeof(TCHAR);
    if ( RegQueryValueEx( rh, local_machine_key, NULL, &type, (unsigned char *) buf, &buf_size) == ERROR_SUCCESS )
        have_key = true;
    RegCloseKey(rh);
    }

/*
-----------------------------------------
Win95 doesn't normally have the above key
----------------------------------------- */
if ( !have_key )
    {
    if ( RegOpenKeyEx(HKEY_CURRENT_USER, shell_folder_key, 0, KEY_QUERY_VALUE, &rh) == ERROR_SUCCESS )
        {
        buf_size = MAX_PATH * sizeof(TCHAR);
        if ( RegQueryValueEx( rh, current_user_key, NULL, &type, (unsigned char *) buf, &buf_size) == ERROR_SUCCESS )
            have_key = true;
        RegCloseKey(rh);
        }
    }

dest = buf;
return have_key;
}

/***********************************************************************
*                           GET_FOLDER_PATH                            *
*     It is assumed the dest size is MAX_PATH * sizeof(TCHAR)          *
***********************************************************************/
bool get_folder_path( TCHAR * dest, TCHAR * local_machine_key, TCHAR * current_user_key )
{
static TCHAR shell_folder_key[] = TEXT( "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders" );

HKEY  rh;
DWORD type;
ULONG buf_size;
bool  have_key;

have_key = false;

if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, shell_folder_key, 0, KEY_QUERY_VALUE, &rh) == ERROR_SUCCESS )
    {
    buf_size = MAX_PATH * sizeof(TCHAR);
    if ( RegQueryValueEx( rh, local_machine_key, NULL, &type, (unsigned char *) dest, &buf_size) == ERROR_SUCCESS )
        have_key = true;
    RegCloseKey(rh);
    }

/*
-----------------------------------------
Win95 doesn't normally have the above key
----------------------------------------- */
if ( !have_key )
    {
    if ( RegOpenKeyEx(HKEY_CURRENT_USER, shell_folder_key, 0, KEY_QUERY_VALUE, &rh) == ERROR_SUCCESS )
        {
        buf_size = MAX_PATH * sizeof(TCHAR);
        if ( RegQueryValueEx( rh, current_user_key, NULL, &type, (unsigned char *) dest, &buf_size) == ERROR_SUCCESS )
            have_key = true;
        RegCloseKey(rh);
        }
    }

return have_key;
}

/***********************************************************************
*                           COPY_RESULTS_DIR                           *
* The shotparm copy in this routine are for v6 results only and        *
* don't need to be updated.                                            *
***********************************************************************/
BOOLEAN copy_results_dir( TCHAR * destdir, TCHAR * sorcdir )
{
NAME_CLASS d;
NAME_CLASS s;
DB_TABLE   td;
DB_TABLE   t;
TCHAR      ascii_shot_number[SHOT_LEN+1];
int        shot_number;
DWORD      n;

/*
----------------------------------------------------
Return FALSE if this is not a part results directory
---------------------------------------------------- */
s = sorcdir;
s.cat_path( SHOTPARM_DB );
if ( !s.file_exists() )
    return FALSE;

s = sorcdir;
s.cat_path( CURRENT_MASTER_TRACE_NAME );
s += MASTER_TRACE_SUFFIX;

if ( !s.file_exists() )
    s.replace( MASTER_TRACE_SUFFIX, MASTER_TRACE_II_SUFFIX );
if ( s.file_exists() )
    {
    d = s;
    d.replace( V5Path, CopyOfV5 );
    s.copyto( d );
    }

s = sorcdir;
d = destdir;
s.cat_path( ALARMSUM_DB );
d.cat_path( ALARMSUM_DB );
s.copyto( d );

s = sorcdir;
d = destdir;
s.cat_path( SHOTPARM_DB );
d.cat_path( SHOTPARM_DB );

if ( s.file_exists() )
    {
    if ( t.open(s, SHOTPARM_RECLEN, PFL) )
        {
        n = t.nof_recs();
        if ( n > 50 )
            {
            n -= 50;
            n--;
            t.goto_record_number( n );
            t.get_current_record( FALSE );
            td.ensure_existance(d);
            td.open( d, SHOTPARM_RECLEN, WL  );
            while ( t.get_next_record(FALSE) )
                {
                td.copy_rec( t );
                td.rec_append();
                }
            td.close();
            }
        else
            {
            s.copyto( d );
            }
        t.close();
        }
    }

s = sorcdir;
d = destdir;
s.cat_path( GRAPHLST_DB );
d.cat_path( GRAPHLST_DB );

if ( s.file_exists() )
    {
    if ( t.open(s, GRAPHLST_RECLEN, PFL) )
        {
        n = t.nof_recs();
        if ( n > 50 )
            {
            n -= 50;
            n--;
            t.goto_record_number( n );
            t.get_current_record( FALSE );
            td.ensure_existance(d);
            td.open( d, GRAPHLST_RECLEN, WL  );
            while ( t.get_next_record(FALSE) )
                {
                td.copy_rec( t );
                td.rec_append();
                }
            td.close();
            }
        else
            {
            s.copyto( d );
            }
        t.close();
        }

    td.open( d, GRAPHLST_RECLEN, PFL  );
    while ( td.get_next_record(FALSE) )
        {
        shot_number = (int) td.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
        copy_ascii_shot_number( ascii_shot_number, shot_number, SHOT_LEN );
        s = sorcdir;
        s.cat_path( ascii_shot_number );
        s += PROFILE_NAME_SUFFIX;
        if ( !s.file_exists() )
            s.replace( PROFILE_NAME_SUFFIX, PROFILE_II_SUFFIX );
        if ( s.file_exists() )
            {
            d = s;
            d.replace( V5Path, CopyOfV5 );
            s.copyto( d );
            }
        }
    td.close();
    }

return TRUE;
}

/***********************************************************************
*                            UPDATE_THREAD                             *
***********************************************************************/
DWORD update_thread( int * notused )
{
TCHAR hide_menu_string[]           = TEXT( "HideMenu" );
TCHAR show_x1_points_only_string[] = TEXT( "ShowX1PointsOnly" );
TCHAR dde_param_delim_string[]     = TEXT( "DdeParamDelimiter" );
TCHAR dde_show_machine_string[]    = TEXT( "DdeParamShowMachine" );

TCHAR    * cp;
NAME_CLASS dest;
NAME_CLASS mycomputer;
NAME_CLASS s;
BOOLEAN    have_ft2_menu;
BOOLEAN    have_ft2terminal_menu;
BOOLEAN    have_profile_menu;
BOOLEAN    have_visiedit_menu;
int        subversion;

/*
---------------------------------------
See if this update has been done before
--------------------------------------- */
show_update_status( TEXT("BEGIN_UPDATE") );
cp = my_get_ini_string( VisiTrakIniFileName, ConfigSection, VersionKey );

OldVersion = asctoint32( cp );

cp = findchar( PeriodChar, cp, lstrlen(cp) );
subversion = 0;
if ( cp )
    {
    cp++;
    subversion = asctoint32( cp );
    }

/*
----------------------------------------------------------
Use the exe directory to make up the name of this computer
---------------------------------------------------------- */
mycomputer = ExeDirectoryName;
cp = mycomputer.find( ExesString );
if ( cp )
    {
    cp--;
    *cp = NullChar;
    }
Computer.startup( mycomputer );

copystring( DataDirectoryName, Computer.whoami().text() );
copystring( BackupDirectoryName, DataDirectoryName );

append_filename_to_path( DataDirectoryName, DataString );
append_filename_to_path( BackupDirectoryName, BackupString );

/*
-----------------------------------------------------------------
If this is version 7 (I will have just updated it if it was less)
then I have to convert the graphlst and shotparm files to use
shot names.
----------------------------------------------------------------- */
if ( OldVersion < 8 )
    {
    s = DataDirectoryName;
    v7_results_to_v8( s, MachineTextBox, PartTextBox );
    }

show_update_status( TEXT("UPDATE_DIFF_CURVES") );
update_dcurve();

show_update_status( TEXT("UPDATE_PARAM_NAMES") );
update_parmname();

show_update_status( TEXT("COPY_NEW_DBS") );
copy_new_dbs( subversion );

cp = my_get_ini_string( DisplayIniFile, ConfigSection, hide_menu_string );
if ( unknown(cp) )
    my_put_ini_string( DisplayIniFile, ConfigSection, hide_menu_string, YString );

cp = my_get_ini_string( DisplayIniFile, ConfigSection, show_x1_points_only_string );
if ( unknown(cp) )
    my_put_ini_string( DisplayIniFile, ConfigSection, show_x1_points_only_string, YString );

my_put_ini_string( DisplayIniFile, ConfigSection, XkeyTogglesX1X4Key, NString );
my_put_ini_string( DisplayIniFile, ConfigSection, CurrentLanguageKey, CurrentLanguage.text() );

cp = my_get_ini_string( MonallIniFileName, ConfigSection, dde_param_delim_string );
if ( unknown(cp) )
    my_put_ini_string( MonallIniFileName, ConfigSection, dde_param_delim_string, TEXT("\\t") );

cp = my_get_ini_string( MonallIniFileName, ConfigSection, dde_show_machine_string );
if ( unknown(cp) )
    {
    my_put_ini_string( MonallIniFileName, ConfigSection, dde_show_machine_string,     YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamShowPart"),    YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamShowDate"),    YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamShowTime"),    YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamShowShot"),    YString );
    my_put_ini_string( MonallIniFileName, ConfigSection, TEXT("DdeParamParmList"),    TEXT("1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60") );
    }

cp = my_get_ini_string( VisiTrakIniFileName, ButtonLevelsSection, DowntimePurgeButtonKey );
if ( unknown(cp) )
    my_put_ini_string( VisiTrakIniFileName, ButtonLevelsSection, DowntimePurgeButtonKey, LevelTenString );

cp = my_get_ini_string( VisiTrakIniFileName, ButtonLevelsSection, EditOperatorsButtonKey );
if ( unknown(cp) )
    my_put_ini_string( VisiTrakIniFileName, ButtonLevelsSection, EditOperatorsButtonKey, LevelTenString );

cp = my_get_ini_string( VisiTrakIniFileName, ButtonLevelsSection, EditControlButtonKey );
if ( unknown(cp) )
    my_put_ini_string( VisiTrakIniFileName, ButtonLevelsSection, EditControlButtonKey, LevelTenString );

/*
---------------------------------------------------------------------------------------------
If this is the local install on a Data Archiver I want to tell eventman to ignore the network
--------------------------------------------------------------------------------------------- */
cp = NString;
if ( ComputerType == NO_NETWORK_TYPE )
    cp = YString;
my_put_ini_string( VisiTrakIniFileName, ConfigSection, NoNetworkKey, cp );

cp = my_get_ini_string( EditPartIniFileName, ConfigSection, ParameterTabLevelKey );
if ( unknown(cp) )
     my_put_ini_string( EditPartIniFileName, ConfigSection, ParameterTabLevelKey, LevelZeroString );

/*
-----------------------------------------------------------------------------
Add the defaults for the Peak Fast Shot Velocity Settings if they are missing
----------------------------------------------------------------------------- */
cp = my_get_ini_string( EditPartIniFileName, ConfigSection, PfsvDefSmoothNKey );
if ( unknown(cp) )
     my_put_ini_string( EditPartIniFileName, ConfigSection, PfsvDefSmoothNKey, TEXT("3") );

cp = my_get_ini_string( EditPartIniFileName, ConfigSection, PfsvDefSmoothMultiplierKey );
if ( unknown(cp) )
     my_put_ini_string( EditPartIniFileName, ConfigSection, PfsvDefSmoothMultiplierKey, TEXT(".95") );

/*
--------------------------
Copy the new exes and mnus
-------------------------- */
dest = ExeDirectoryName;
dest.cat_path( ProfileMenuFile );
have_profile_menu = dest.file_exists();
if ( have_profile_menu )
    {
    s = ExeDirectoryName;
    s.cat_path( OldProfileMenuFile );
    if ( s.file_exists() )
        s.delete_file();
    dest.moveto( s );
    }

dest = ExeDirectoryName;
dest.cat_path( VisiEditMenuFile );
have_visiedit_menu = dest.file_exists();
if ( have_visiedit_menu )
    {
    s = ExeDirectoryName;
    s.cat_path( OldVisiEditMenuFile );
    if ( s.file_exists() )
        s.delete_file();
    dest.moveto( s );
    }

dest = ExeDirectoryName;
dest.cat_path( Ft2MenuFile );
have_ft2_menu = dest.file_exists();
if ( have_ft2_menu )
    {
    s = ExeDirectoryName;
    s.cat_path( OldFt2MenuFile );
    if ( s.file_exists() )
        s.delete_file();
    dest.moveto( s );
    }

dest = ExeDirectoryName;
dest.cat_path( Ft2TerminalMenuFile );
have_ft2terminal_menu = dest.file_exists();
if ( have_ft2terminal_menu )
    {
    s = ExeDirectoryName;
    s.cat_path( OldFt2TerminalMenuFile );
    if ( s.file_exists() )
        s.delete_file();
    dest.moveto( s );
    }

show_update_status( TEXT("UNZIP_EXES") );

/*
---------------------------------------------------------------------------------
Copy the pkzip25 executable. Put quotes around the sorc in case there are spaces.
--------------------------------------------------------------------------------- */
s = FloppyDirectoryName;
s.cat_path( PkzipExeFile );

dest = ExeDirectoryName;
dest.cat_path( PkzipExeFile );

if ( !dest.file_exists() )
    {
    show_update_status( TEXT("COPYING_PKZIP") );
    s.copyto( dest );

    if ( !wait_for_file(dest) )
         st_message_box( TEXT("NO_FILE"), TEXT("PKZIP_COPY_FAIL") );

    show_update_status( TEXT("UNZIP_EXES") );
    }

/*
---------------------
Update the units file
--------------------- */
show_update_status( TEXT("UPDATE_UNITS") );
update_units();

/*
-------------------------------------
Update the rodpitch and psensor files
------------------------------------- */
show_update_status( TEXT("UPDATE_RODPITCH") );
update_rodpitches();

show_update_status( TEXT("UPDATE_DEF_BINVALVE") );
update_default_binary_valve_file();

/*
-------------------
Copy the Setup file
------------------- */
s = FloppyDirectoryName;
s.cat_path( DefaultSetupFile );

dest = ExeDirectoryName ;
dest.cat_path( DefaultSetupFile );

if ( !dest.file_exists() )
    {
    s.copyto( dest );
    wait_for_file( dest );
    }

/*
----------------------------------
Copy the Status Screen Config file
---------------------------------- */
s = FloppyDirectoryName;
s.cat_path( StatusScreenConfigFile );

dest = ExeDirectoryName;
dest.cat_path( StatusScreenConfigFile );
if ( !dest.file_exists() )
    {
    s.copyto( dest );
    wait_for_file( dest );
    }

/*
---------------------------------------
Copy the restricted part setup ini file
--------------------------------------- */
s = FloppyDirectoryName;
s.cat_path( RESTRICTED_PART_ENTRY_SCREEN_INI_FILE );

dest = ExeDirectoryName;
dest.cat_path( RESTRICTED_PART_ENTRY_SCREEN_INI_FILE );
if ( !dest.file_exists() )
    {
    s.copyto( dest );
    wait_for_file( dest );
    }

s = ExeDirectoryName;
s.add_ending_backslash();
s += TEXT( "pkzip25 -ext -dir -over=all " );
s += FloppyDirectoryName;
s += TEXT( "v5exes *.exe *.hlp *.pif *.swf *.bmp *.stbl ft2help.*" );

s += SpaceChar;
s += ProfileMenuFile;

s += SpaceChar;
s += VisiEditMenuFile;

s += SpaceChar;
s += Ft2MenuFile;

s += SpaceChar;
s += Ft2TerminalMenuFile;

dest = ExeDirectoryName;
dest.remove_ending_backslash();
execute_and_wait( s.text(), dest.text() );

menu_update( ProfileMenuFile,     OldProfileMenuFile,     ExeDirectoryName );
menu_update( VisiEditMenuFile,    OldVisiEditMenuFile,    ExeDirectoryName );
menu_update( Ft2MenuFile,         OldFt2MenuFile,         ExeDirectoryName );
menu_update( Ft2TerminalMenuFile, OldFt2TerminalMenuFile, ExeDirectoryName );

if ( ComputerType != MONITORING_COMPUTER_TYPE )
    {
    kill_monitor_files();
    }
else
    {
    choose_ftii();
    }

update_suretrak_and_version( TRUE );

/*
------------------
Copy the help file
------------------ */
copy_help_file();

s = ExeDirectoryName;
s.add_ending_backslash();
s += TEXT( "pkzip25 -ext -over=all " );
s += FloppyDirectoryName;
s +=  TEXT( "ft2help *.*" );

dest = ExeDirectoryName;
dest.remove_ending_backslash();

execute_and_wait( s.text(), dest.text() );

copy_resources_directory();

if ( ComputerType != DATA_ARCHIVER_TYPE )
    {
    CoInitialize(0);
    /*
    -----------------------------------------
    Make the full path to the startup program
    ----------------------------------------- */
    s = ExeDirectoryName;
    s.cat_path( VisiSuFileName );

    /*
    ----------------------------
    Put a link in the start menu
    ---------------------------- */
    get_folder_path(dest, TEXT("Common Startup"), TEXT("Startup") );
    if ( !dest.isempty() )
        {
        dest.cat_path( LinkFile );
        if ( NeedStartMenuEntry )
            create_link( s.text(), dest.text(), LinkDesc );
        else
            dest.delete_file();
        }

    CoUninitialize();
    }

CurrentDialogWindow.post( WM_DBCLOSE );
return 0;
}

/***********************************************************************
*                         START_UPDATE_THREAD                          *
***********************************************************************/
static void start_update_thread()
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                    /* no security attributes        */
    0,                                       /* use default stack size        */
    (LPTHREAD_START_ROUTINE) update_thread,  /* thread function       */
    0,                                       /* argument to thread function   */
    0,                                       /* use default creation flags    */
    &id );                                   /* returns the thread identifier */

if ( !h )
    st_message_box( TEXT("CREATE_FAIL"), TEXT("UPDATE_THREAD") );
}

/***********************************************************************
*                           FIND_DIRECTORIES                           *
***********************************************************************/
static void find_directories( HWND w )
{
static TCHAR firstry[]   = TEXT( "C:\\V5\\EXES" );
static TCHAR secondtry[] = TEXT( "C:\\V7\\EXES" );
TCHAR        s[MAX_PATH+1];
TCHAR        drivelistc[] = TEXT( "CDEFGH" );
TCHAR        drivelistd[] = TEXT( "DCEFGH" );
TCHAR      * cp;
TCHAR        c;

lstrcpy( s, firstry );
append_backslash_to_path( s );

/*
---------------------------------------------------------
Start looking on the boot drive (for now I assume C or D)
--------------------------------------------------------- */
c = boot_drive_letter();
if ( c == *drivelistc )
    cp = drivelistc;
else
    cp = drivelistd;

while ( TRUE )
    {
    if ( !(*cp) )
        break;

    *firstry   = *cp;
    *secondtry = *cp;
    cp++;

    if ( drive_exists(firstry) )
        {
        if ( directory_exists(firstry) )
            lstrcpy( s, firstry);
        else if ( directory_exists(secondtry) )
            lstrcpy( s, secondtry);
        else
            continue;
        }

    append_backslash_to_path( s );

    if ( is_exe_directory(s) )
        break;
    }

if ( !is_exe_directory(s) )
    {
    st_message_box( TEXT("NOT_FIND_EXES"), TEXT("INPUT_MANUALLY") );
    return;
    }

copystring( ExeDirectoryName, s );
set_text( w, EXE_DIR_EDITBOX, s );
}

/***********************************************************************
*                     REMOVE_SURETRAK_CONTROL_MENU                     *
***********************************************************************/
static void remove_suretrak_control_menu()
{
static TCHAR SURETRAK_CONTROL_MENU[] = TEXT( ",710," );
static TCHAR TEMP_MENU_FILE[] = TEXT( "tempmnu.txt" );

NAME_CLASS dfn;
NAME_CLASS sfn;
FILE_CLASS dest;
FILE_CLASS sorc;
TCHAR * cp;

sfn = ExeDirectoryName;
sfn.cat_path( ProfileMenuFile );

dfn = ExeDirectoryName;
dfn.cat_path( TEMP_MENU_FILE );

if ( sorc.open_for_read(sfn.text()) )
    {
    if ( dest.open_for_write(dfn.text()) )
        {
        while ( true )
            {
            cp = sorc.readline();
            if ( !cp )
                break;
            if ( !findstring(SURETRAK_CONTROL_MENU, cp) )
                dest.writeline( cp );
            }
        dest.close();
        sorc.close();
        sfn.delete_file();
        dfn.moveto( sfn );
        }
    }
}

/***********************************************************************
*                           INSTALL_THREAD                             *
***********************************************************************/
DWORD install_thread( int * notused )
{
static TCHAR backup_section[]          = TEXT( "Backup" );
static TCHAR backup_dir_key[]          = TEXT( "BackupDir" );
static TCHAR backup_zip_key[]          = TEXT( "BDZipName" );
static TCHAR * ftchanpath[] = {
    TEXT( "\\E20\\NEWPART\\" ),
    TEXT( "\\E20\\VISITRAK\\" ),
    TEXT( "\\M20\\NEWPART\\" ),
    TEXT( "\\M20\\VISITRAK\\" ),
    TEXT( "\\M2\\NEWPART\\" ),
    TEXT( "\\M2\\VISITRAK\\" )
    };
const int nof_ftchanpaths = 6;

static TCHAR default_backup_zip_file[] = TEXT( "backup.zip" );

TCHAR    dest[MAX_PATH+1];
TCHAR    sorc[MAX_PATH+1];
TCHAR    ini_file_name[MAX_PATH+1];

FILE_CLASS     f;
NAME_CLASS     s;
SETPOINT_CLASS sp;

TCHAR  * dest_file;
TCHAR  * sorc_file;
TCHAR  * cp;
BOOLEAN  b;
int32    i;

DB_TABLE t;

show_status( TEXT("CREATING_DIRS") );

lstrcpy( ExeDirectoryName,    NewComputerName.text() );
lstrcpy( DataDirectoryName,   ExeDirectoryName );
lstrcpy( BackupDirectoryName, ExeDirectoryName );

catpath( ExeDirectoryName,    ExesString   );
catpath( DataDirectoryName,   DataString   );
catpath( BackupDirectoryName, BackupString );

create_directory( ExeDirectoryName    );
create_directory( DataDirectoryName   );
create_directory( BackupDirectoryName );

/*
----------------------------------------------------------
Use the exe directory to make up the name of this computer
---------------------------------------------------------- */
s = NewComputerName;
s.remove_ending_backslash();
Computer.startup( s );

show_status( TEXT("COPYING_PKZIP")  );

/*
---------------------------
Copy the pkzip25 executable
--------------------------- */
lstrcpy( sorc, FloppyDirectoryName );
lstrcat( sorc, PkzipExeFile );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, PkzipExeFile );

CopyFile( sorc, dest, FALSE );

if ( !wait_for_file(dest) )
    {
    st_message_box( TEXT("NO_FILE"), TEXT("PKZIP_COPY_FAIL") );
    MessageBox( 0, sorc, dest, MB_OK );
    }

/*
-------------------
Copy the Setup file
------------------- */
lstrcpy( sorc, FloppyDirectoryName );
lstrcat( sorc, DefaultSetupFile );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, DefaultSetupFile );

if ( !file_exists(dest) )
    {
    CopyFile( sorc, dest, FALSE );
    wait_for_file(dest);
    }

/*
----------------------------------
Copy the Status Screen Config file
---------------------------------- */
lstrcpy( sorc, FloppyDirectoryName );
lstrcat( sorc, StatusScreenConfigFile );

lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, StatusScreenConfigFile );

if ( !file_exists(dest) )
    {
    CopyFile( sorc, dest, FALSE );
    wait_for_file(dest);
    }

/*
--------------------------------------------------------------------------------------------
Make sure there is a SureTrak2 message file
The actual message file has the same name as the old message file but is in the exes folder.
I use a different name on the install directory because the old one is there too.
-------------------------------------------------------------------------------------------- */
lstrcpy( dest, ExeDirectoryName );
append_filename_to_path( dest, CONTROL_MESSAGE_FILE );
if ( !file_exists(dest) )
    {
    lstrcpy( sorc, FloppyDirectoryName );
    lstrcat( sorc,  FtiiMessageFileName );
    CopyFile( sorc, dest, FALSE );
    }

/*
-------------------------
Unzip the v5data.zip file
------------------------- */
show_status( TEXT("COPYING_DATA")  );

if ( UsingRestorePoint )
    {
    copystring( sorc, RestorePointPath.text() );
    append_filename_to_path( sorc, DataString );
    copy_all_files( DataDirectoryName, sorc );
    }
else
    {
    lstrcpy( sorc, ExeDirectoryName );
    append_backslash_to_path( sorc );
    lstrcat( sorc, TEXT("pkzip25 -ext -dir ") );
    lstrcat( sorc, FloppyDirectoryName );
    lstrcat( sorc, TEXT("v5data") );

    execute_and_wait( sorc, DataDirectoryName );
    }
show_status( TEXT("UPDATING_DATA")  );

/*
--------------------------------------------------------------------
If I am installing a new FT2 system, replace the old boards.txt with
an empty boards.txt and new ftii_boards.txt
-------------------------------------------------------------------- */
if ( !UsingRestorePoint )
    {
    lstrcpy( sorc, FloppyDirectoryName );
    append_filename_to_path(  sorc, BOARDS_DB );

    lstrcpy( dest, DataDirectoryName );
    append_filename_to_path( dest, BOARDS_DB );
    CopyFile( sorc, dest, FALSE );

    lstrcpy( sorc, FloppyDirectoryName );
    append_filename_to_path(  sorc, FTII_BOARDS_DB );

    lstrcpy( dest, DataDirectoryName );
    append_filename_to_path( dest, FTII_BOARDS_DB );
    CopyFile( sorc, dest, FALSE );
    }

show_status( TEXT("UPDATE_DEF_BINVALVE") );
update_default_binary_valve_file();

show_status( TEXT("COPYING_EXES") );

/*
--------------
Unzip the exes
-------------- */
lstrcpy( sorc, ExeDirectoryName );
append_backslash_to_path( sorc );
lstrcat( sorc, TEXT("pkzip25 -ext -dir ") );
lstrcat( sorc, FloppyDirectoryName );
lstrcat( sorc, TEXT("v5exes") );

execute_and_wait( sorc, ExeDirectoryName );

/*
---------------------------------------
Default the channel display to 7 and 8.
--------------------------------------- */
cp = Ft2ChanConfig;

my_put_ini_string( DisplayIniFile, ConfigSection, ChanPreConfigKey,  cp );
my_put_ini_string( DisplayIniFile, ConfigSection, ChanPostConfigKey, cp );
my_put_ini_string( DisplayIniFile, ConfigSection, XkeyTogglesX1X4Key, NString );
my_put_ini_string( DisplayIniFile, ConfigSection, CurrentLanguageKey, CurrentLanguage.text() );

lstrcpy( ini_file_name, ExeDirectoryName );
append_filename_to_path(  ini_file_name, VisiTrakIniFileName );

/*
------------------------
Fix the backup directory
[Backup]
BackupDir=e:\v5\backup\
------------------------ */
lstrcpy( sorc, BackupDirectoryName );
append_backslash_to_path( sorc );
WritePrivateProfileString( backup_section, backup_dir_key, sorc, ini_file_name );

/*
---------------------------------
[Backup]
BDZipName=e:\v5\backup.zip
--------------------------------- */
lstrcpy( sorc, NewComputerName.text() );
append_filename_to_path( sorc, default_backup_zip_file );
WritePrivateProfileString( backup_section, backup_zip_key, sorc, ini_file_name );

cp = NString;
if ( ComputerType == NO_NETWORK_TYPE )
    cp = YString;
WritePrivateProfileString( ConfigSection, NoNetworkKey, cp, ini_file_name );

update_suretrak_and_version( FALSE );

/*
---------------------------------------------------------
If there is a suretrak control, the installer should have
chosen the type [E20, M2, M20]. Copy the corresponding
control, message, and parameter files (note, the new
computer name has not been set yet so I have to use
the original (c01).
--------------------------------------------------------- */
lstrcpy( sorc, DataDirectoryName );
append_backslash_to_path( sorc );

lstrcpy( dest, sorc );

dest_file = dest + lstrlen(dest);
sorc_file = sorc + lstrlen(sorc);

/*
----------------------------------------------------------------------
The NO_NETWORK_TYPE is the local install on the data archiver. Neither
of these is a monitoring computer and so they don't need monitor menus
---------------------------------------------------------------------- */
if ( ComputerType==OFFICE_WORKSTATION_TYPE || ComputerType==NO_NETWORK_TYPE )
    {
    s = ExeDirectoryName;
    s.cat_path( ProfileMenuFile );
    set_menu_password_level( s.text(), BOARD_MONITOR_MENU, 99 );
    set_menu_password_level( s.text(), MONITOR_CONFIG_MENU, 99 );
    set_menu_password_level( s.text(), SETUPS_MENU, 99 );
    }

if ( (ComputerType == OFFICE_WORKSTATION_TYPE) || (ComputerType==DATA_ARCHIVER_TYPE) )
    {
    /*
    ----------------------------------
    Remove all the machine directories
    ---------------------------------- */
    for ( i=0; i<NofStSorcFiles; i++ )
        {
        lstrcpy( sorc_file, SorcDirNames[i] );
        kill_directory( sorc );
        }
    }
else if ( HaveSureTrak )
    {
    lstrcpy( sorc_file, SorcMessageFiles[StSorcIndex] );
    lstrcpy( dest_file, CONTROL_MESSAGE_FILE );
    CopyFile( sorc, dest, FALSE );

    lstrcpy( sorc_file, SorcParamFiles[StSorcIndex] );
    lstrcpy( dest_file, STPARAM_FILE );
    CopyFile( sorc, dest, FALSE );

    /*
    ------------------------------------------------
    Delete the directories of the other two machines
    Delete this one as well if this is an office
    workstation.
    ------------------------------------------------ */
    if ( !UsingRestorePoint )
        {
        for ( i=0; i<NofStSorcFiles; i++ )
            {
            if ( i != StSorcIndex || (ComputerType == OFFICE_WORKSTATION_TYPE) )
                {
                lstrcpy( sorc_file, SorcDirNames[i] );
                kill_directory( sorc );
                }
            }
        }

    /*
    ------------------------------------------------------
    Make sure the machine is marked as a suretrak2 control
    ------------------------------------------------------ */
    lstrcpy( sorc_file, SorcDirNames[StSorcIndex] );
    append_filename_to_path( sorc, MACHSET_CSV );
    s.acquire( sorc );
    if ( sp.get(s) )
        {
        b = FALSE;
        if ( HaveSureTrak )
            b = TRUE;
        sp[MACHSET_ST_BOARD_INDEX].value = b;
        sp.put( s );
        }
    s.release();
    }

create_startup_list_file();

/*
---------------------------------------------------------
The new computer names do NOT have a backslash at the end
--------------------------------------------------------- */
s = NewComputerName;
s.remove_ending_backslash();

WritePrivateProfileString( ConfigSection, ThisComputerKey, s.text(), ini_file_name );

/*
----------------------
Update the computer db
---------------------- */
lstrcpy( sorc, DataDirectoryName );
append_filename_to_path( sorc, COMPUTER_CSV );

if ( f.open_for_write(sorc) )
    {
    f.writeline( s.text() );
    f.close();
    }

if ( ComputerType != MONITORING_COMPUTER_TYPE )
    kill_monitor_files();
else
    choose_ftii();

copy_resources_directory();

if ( ComputerType != DATA_ARCHIVER_TYPE )
    {
    CoInitialize(0);
    /*
    -----------------------------------------
    Make the full path to the startup program
    ----------------------------------------- */
    lstrcpy( sorc, ExeDirectoryName );
    append_filename_to_path(  sorc, VisiSuFileName );

    /*
    --------------------------
    Put an icon on the desktop
    -------------------------- */
    if ( get_folder_path(dest, TEXT("Common Desktop"), TEXT("Desktop")) )
        {
        append_filename_to_path( dest, LinkFile );
        create_link( sorc, dest, LinkDesc );
        }

    if ( NeedStartMenuEntry )
        {
        /*
        ----------------------------
        Put a link in the start menu
        ---------------------------- */
        if ( get_folder_path(dest, TEXT("Common Startup"), TEXT("Startup")) )
            {
            append_filename_to_path( dest, LinkFile );
            create_link( sorc, dest, LinkDesc );
            }
        }

    CoUninitialize();
    }

copy_help_file();

lstrcpy( sorc, ExeDirectoryName );
append_backslash_to_path( sorc );
lstrcat( sorc, TEXT("pkzip25 -ext -over=all ") );
lstrcat( sorc, FloppyDirectoryName );
lstrcat( sorc, TEXT("ft2help *.*") );

lstrcpy( dest, ExeDirectoryName );
remove_backslash_from_path( dest );

execute_and_wait( sorc, dest );

if ( UsingRestorePoint )
    {
    s = RestorePointPath;
    if ( !s.find(CurrentString) )
        {
        s.remove_ending_filename();
        s.cat_path( CurrentString );
        MessageBox( 0, s.text(), StringTable.find(TEXT("AUTOBACK_DIR_SET")), MB_OK );
        }
    my_put_ini_string( VisiTrakIniFileName, BackupSection, AutoBackupDirKey, s.text() );
    }
else
    {
    if ( drive_exists(StdAutoBackPath) )
        DialogBox( MainInstance, TEXT("STD_AUTO_BACKUP_DIALOG"), MainWindow.handle(), (DLGPROC) std_auto_backup_dialog_proc );
    }

CurrentDialogWindow.post( WM_DBCLOSE );
return 0;
}

/***********************************************************************
*                         START_INSTALL_THREAD                         *
***********************************************************************/
static void start_install_thread()
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                    /* no security attributes        */
    0,                                       /* use default stack size        */
    (LPTHREAD_START_ROUTINE) install_thread,  /* thread function       */
    0,                                       /* argument to thread function   */
    0,                                       /* use default creation flags    */
    &id );                                   /* returns the thread identifier */

if ( !h )
    st_message_box( TEXT("CREATE_FAIL"), TEXT("INSTALL_THREAD") );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( const DWORD context )
{
static TCHAR setup_help_file[] = TEXT( "setup.hlp" );
NAME_CLASS s;

s = FloppyDirectoryName;
s.cat_path( setup_help_file );
if ( s.file_exists() )
    WinHelp( MainWindow.handle(), s.text(), HELP_CONTEXT, context );
}

/***********************************************************************
*                  SET_FINISH_SETUP_STRINGS                    *
***********************************************************************/
static void set_finish_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,                TEXT("OK_STRING") },
    { INS_COMPLETE_STATIC, TEXT("FINISHED") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, NULL );
}

/***********************************************************************
*                            FINISH_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK finish_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        set_finish_setup_strings( hWnd );
        CurrentDialogWindow.set_focus( IDOK );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( CONTENTS_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                  SET_UPDATE_STATUS_SETUP_STRINGS                    *
***********************************************************************/
static void set_update_status_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDCANCEL,          TEXT("CANCEL") },
    { US_MACHINE_STATIC, TEXT("MACHINE_STRING") },
    { US_PART_STATIC,    TEXT("PART_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    STATUS_MSG_TEXTBOX,
    STATUS_MACHINE_TB,
    STATUS_PART_TB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                        UPDATE_STATUS_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK update_status_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        set_update_status_setup_strings( hWnd );
        StatusTextWindow = GetDlgItem( hWnd, STATUS_MSG_TEXTBOX );
        MachineTextBox   = GetDlgItem( hWnd, STATUS_MACHINE_TB );
        PartTextBox      = GetDlgItem( hWnd, STATUS_PART_TB );
        set_text( MachineTextBox, EmptyString );
        set_text( PartTextBox,    EmptyString );
        start_update_thread();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( UPDATE_SYSTEM_HELP );
         return TRUE;

    case WM_DBCLOSE:
        StatusTextWindow = 0;
        CurrentDialogWindow = 0;
        CreateDialog(
            MainInstance,
            TEXT("FINISH_DIALOG"),
            MainWindow.handle(),
            (DLGPROC) finish_dialog_proc );
        DestroyWindow( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          FILENAME_IN_STRING                          *
***********************************************************************/
static BOOLEAN filename_in_string( TCHAR * s, TCHAR * filename )
{
int32 slen;
int32 filename_len;

filename_len = lstrlen( filename );
slen         = lstrlen( s );

if ( slen >= filename_len )
    {
    s += slen;
    s -= filename_len;
    if ( lstrcmpi(s, filename) == 0 )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        ENABLE_UPDATE_CONTROLS                        *
* If the type of computer or the type of board (ft1 or ft2, control    *
* or monitor only) the options change.                                 *
***********************************************************************/
static void enable_update_controls()
{
struct WINDOW_INFO_ENTRY
    {
    int id;
    BOOLEAN is_enabled[2*NOF_COMPUTER_TYPES];
    };
static WINDOW_INFO_ENTRY winfo[] = {
                            /* ----------- Control ------------   --------- Monitor Only ---------  */
                            /* NoCo   Mon    DA     WkSta  NoNet  NoCo   Mon    DA     WkSta  NoNet */
    START_MONITOR_CHECKBOX,    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    SURETRAK_CONTROL_CHECKBOX, FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,
    MONITOR_SETUPS_CHECKBOX,   FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    PART_EDITOR_CHECKBOX,      FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE,
    START_AT_BOOT_XBOX,        FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    AUTO_MONITOR_CHECKBOX,     FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    };

int NOF_CONTROLS = sizeof(winfo)/sizeof(WINDOW_INFO_ENTRY);

HWND w;
int  c;
int  i;

w = CurrentDialogWindow.handle();
set_global_computer_type_from_buttons( w );

c = ComputerType;
if ( !HaveSureTrak )
    c += NOF_COMPUTER_TYPES;

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    w = CurrentDialogWindow.control( winfo[i].id );
    EnableWindow( w, winfo[i].is_enabled[c] );
    }
}

/***********************************************************************
*                       INIT_UPDATE_CHECKBOXES                         *
***********************************************************************/
static void init_update_checkboxes( HWND w )
{
STRING_CLASS computer;
NAME_CLASS   s;
INI_CLASS    ini;
TCHAR      * cp;
BOOLEAN      need_part_editor;
BOOLEAN      need_monitor_setup;
BOOLEAN      need_board_monitor;
FILE_CLASS   f;
int          id;

need_part_editor   = FALSE;
need_monitor_setup = FALSE;
need_board_monitor = FALSE;
s.upsize( MAX_PATH );
if ( computer.get_text(w, EXE_DIR_EDITBOX) )
    {
    cp = computer.find( ExesString );
    if ( cp )
        {
        cp--;
        *cp = NullChar;
        }

    ComputerType = computer_type( computer );
    id = MONITORING_COMPUTER_RADIO;
    if ( ComputerType != NO_COMPUTER_TYPE )
        id += ComputerType - 1;
    CheckRadioButton( w, MONITORING_COMPUTER_RADIO, NO_NETWORK_RADIO, id );

    s.get_exe_dir_file_name( computer, VisiTrakIniFileName );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );

    HaveSureTrak = ini.get_boolean( HaveSureTrakKey );
    set_checkbox( w, SURETRAK_CONTROL_CHECKBOX, HaveSureTrak );

    check_control_file_dates();

    if ( Ft2ControlAllFileIsOld )
       s = StringTable.find( TEXT("NEW_STRING") );
    else
       s.null();
    s.set_text( w, NEW_FT2_CONTROL_FILE_TB );

    s.get_exe_dir_file_name( computer, StartupListFile );
    f.open_for_read( s.text() );
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;

        if ( !need_part_editor && filename_in_string(cp, EditPartExeFile) )
            need_part_editor = TRUE;

        if ( !need_monitor_setup && filename_in_string(cp, MonEditExeFile) )
            need_monitor_setup = TRUE;

        if ( !need_board_monitor && filename_in_string(cp, MonallExeFile) )
            need_board_monitor = TRUE;
        }

    f.close();

    set_checkbox( w, PART_EDITOR_CHECKBOX,    need_part_editor   );
    set_checkbox( w, AUTO_MONITOR_CHECKBOX,   need_board_monitor );
    set_checkbox( w, MONITOR_SETUPS_CHECKBOX, need_monitor_setup );
    }

CoInitialize(0);

if ( get_folder_path(s.text(), TEXT("Common Startup"), TEXT("Startup")) )
    {
    s.cat_path( LinkFile );
    if ( s.file_exists() )
        {
        NeedStartMenuEntry = TRUE;
        set_checkbox( w, START_AT_BOOT_XBOX, TRUE );
        }
    }

CoUninitialize();

enable_update_controls();
}

/***********************************************************************
*                            HAVE_CHECKMARK                            *
***********************************************************************/
static BOOLEAN have_checkmark( int id )
{
HWND w;

w = CurrentDialogWindow.control( id );
if ( !IsWindowEnabled(w ) )
    return FALSE;

return is_checked( w );
}

/***********************************************************************
*                  SET_FIND_DIR_SETUP_STRINGS                          *
***********************************************************************/
static void set_find_dir_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { TT_EXES_STATIC,              TEXT("TT_EXES") },
    { IDOK,                        TEXT("CONTINUE") },
    { IDCANCEL,                    TEXT("CANCEL") },
    { PART_EDITOR_CHECKBOX,        TEXT("START_PART_SETUPS") },
    { MONITOR_SETUPS_CHECKBOX,     TEXT("START_MON_SETUPS") },
    { AUTO_MONITOR_CHECKBOX,       TEXT("START_FT2") },
    { SURETRAK_CONTROL_CHECKBOX,   TEXT("ST2_INSTALLED") },
    { START_AT_BOOT_XBOX,          TEXT("START_AT_BOOT") },
    { MONITORING_COMPUTER_RADIO,   TEXT("MON_CO") },
    { DATA_ARCHIVER_RADIO,         TEXT("DA_CO") },
    { NO_NETWORK_RADIO,            TEXT("LOCAL_INS") },
    { OFFICE_WORKSTATION_RADIO,    TEXT("OFWRK") },
    { CO_TYPE_STATIC,              TEXT("CO_TYPE") },
    { COPY_FT2_CONTROL_FILES_XBOX, TEXT("COPY_FT2") },
    { DIR_OF_EXES_STATIC,          TEXT("EXES_DIR") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    EXE_DIR_EDITBOX,
    NEW_FT2_CONTROL_FILE_TB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, TEXT("") );
}

/***********************************************************************
*                       FIND_DIRECTORY_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK find_directory_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        find_directories( hWnd );
        set_find_dir_setup_strings( hWnd );
        init_update_checkboxes( hWnd );
        CurrentDialogWindow.set_focus( IDOK );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( UPDATE_SYSTEM_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SURETRAK_CONTROL_CHECKBOX:
                 HaveSureTrak = is_checked( hWnd, SURETRAK_CONTROL_CHECKBOX );
            case DATA_ARCHIVER_RADIO:
            case MONITORING_COMPUTER_RADIO:
            case OFFICE_WORKSTATION_RADIO:
            case NO_NETWORK_RADIO:
                enable_update_controls();
                return TRUE;

            case COPY_FT2_CONTROL_FILES_XBOX:
                if ( !is_checked(hWnd, COPY_FT2_CONTROL_FILES_XBOX) )
                    {
                    if ( Ft2ControlFilesAreMissing )
                        st_message_box( TEXT("COPY_BY_HAND"), TEXT("MISSING_CONTROL_FILES") );
                    }
                return TRUE;

            case IDOK:
                NeedPartEditor      = have_checkmark( PART_EDITOR_CHECKBOX );
                NeedMonitorConfig   = have_checkmark( MONITOR_SETUPS_CHECKBOX );
                NeedMonitorStartup  = have_checkmark( AUTO_MONITOR_CHECKBOX );
                NeedStartMenuEntry  = have_checkmark( START_AT_BOOT_XBOX );
                HaveSureTrak        = have_checkmark( SURETRAK_CONTROL_CHECKBOX );

                NeedToCopyFt2ControlFiles = is_checked( hWnd, COPY_FT2_CONTROL_FILES_XBOX );
                set_global_computer_type_from_buttons( hWnd );

                if ( ComputerType==OFFICE_WORKSTATION_TYPE || ComputerType==NO_NETWORK_TYPE )
                    {
                    NeedMonitorStartup  = FALSE;
                    NeedMonitorConfig   = FALSE;
                    }

                get_text( ExeDirectoryName, hWnd, EXE_DIR_EDITBOX, MAX_PATH+1 );
                if ( !is_exe_directory(ExeDirectoryName) )
                    {
                    st_message_box( TEXT("NO_VISITRAK_INI_STRING"), TEXT("UNABLE_TO_CONTINUE") );
                    return TRUE;
                    }
                SetCurrentDirectory( ExeDirectoryName );

                CurrentDialogWindow = 0;
                CreateDialog(
                    MainInstance,
                    TEXT("UPDATE_STATUS_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) update_status_dialog_proc );
                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                  SET_install_status_SETUP_STRINGS                    *
***********************************************************************/
static void set_install_status_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDCANCEL, TEXT("CANCEL") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    STATUS_MSG_TEXTBOX,
    FILENAME_TEXTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                       INSTALL_STATUS_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK install_status_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        set_install_status_setup_strings( hWnd );
        CurrentDialogWindow.set_focus( IDCANCEL );
        CurrentDialogWindow.post( WM_DBINIT );
        return TRUE;

    case WM_DBINIT:
        show_status( TEXT("BEGIN_INSTALL") );
        start_install_thread();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( NEW_SYSTEM_HELP );
         return TRUE;

    case WM_DBCLOSE:
        CurrentDialogWindow = 0;
        CreateDialog(
            MainInstance,
            TEXT("FINISH_DIALOG"),
            MainWindow.handle(),
            (DLGPROC) finish_dialog_proc );
        DestroyWindow( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        RENAME_ROOT_DIRECTORY                         *
***********************************************************************/
static boolean rename_root_directory( HWND w )
{
static TCHAR oldname[] = TEXT( "OldV5" );

int     i;
NAME_CLASS d;
NAME_CLASS dest;
NAME_CLASS s;
NAME_CLASS sorc;
TCHAR * cp;
TEXT_LIST_CLASS t;

sorc = NewComputerName;

i = 1;
while ( true )
    {
    dest = sorc;
    cp = dest.text();
    cp += dest.len();
    cp--;
    if ( *cp == BackSlashChar )
        cp--;

    /*
    ---------------------
    Replace V5 with OldV5
    --------------------- */
    while ( cp > dest.text() )
        {
        if ( *cp == BackSlashChar )
            {
            cp++;
            *cp = NullChar;
            dest += oldname;
            break;
            }
        cp--;
        }
    /*
    ------------
    Add a number
    ------------ */
    dest += i;

    if ( !dest.directory_exists() )
        break;
    i++;
    }

s = StringTable.find( TEXT("OLD_DIR_RENAME") );
s += dest;
i = MessageBox( w, s.text(), StringTable.find(TEXT("INSTALL_DIR_EXISTS")), MB_OKCANCEL | MB_ICONEXCLAMATION | MB_APPLMODAL );
if ( i != IDOK )
    return false;

if ( !dest.create_directory() )
    {
    st_message_box( TEXT("CANT_RENAME_DIR"), TEXT("CANNOT_COMPLY_STRING") );
    return false;
    }

t.get_directory_list( sorc.text() );
t.rewind();
while ( t.next() )
    {
    s = sorc;
    s.cat_path( t.text() );
    d = dest;
    d.cat_path( t.text() );
    s.moveto( d );
    }

return true;
}

/***********************************************************************
*                      INIT_ORIGINAL_COMPUTER_EBOX                     *
***********************************************************************/
static void init_original_computer()
{
OriginalComputerName = resource_string( DEFAULT_INSTALL_DIRECTORY_STRING );
*OriginalComputerName.text() = boot_drive_letter();
}

/***********************************************************************
*                       MAKE_SURE_I_AM_ON_SCREEN                       *
***********************************************************************/
static void make_sure_i_am_on_screen()
{
RECT dr;
RECT r;
long dy;

dr = desktop_client_rect();
if ( MainWindow.getrect(r) )
    {
    if ( r.bottom > dr.bottom )
        {
        dy = dr.bottom - r.bottom;
        MainWindow.offset( 0, dy );
        }
    }
}

/***********************************************************************
*                       ENABLE_INSTALL_CONTROLS                        *
* If the type of computer or the type of board (ft1 or ft2, control    *
* or monitor only) the options change.                                 *
***********************************************************************/
static void enable_install_controls()
{
struct WINDOW_INFO_ENTRY
    {
    int id;
    BOOLEAN is_enabled[2*NOF_COMPUTER_TYPES];
    };
static WINDOW_INFO_ENTRY winfo[] = {
                            /* ----------- Control ------------   --------- Monitor Only ---------  */
                            /* NoCo   Mon    DA     WkSta  NoNet  NoCo   Mon    DA     WkSta  NoNet */
    START_MONITOR_CHECKBOX,    FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    SURETRAK_CONTROL_CHECKBOX, FALSE, TRUE,  FALSE, TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  TRUE,
    MONITOR_SETUPS_CHECKBOX,   FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    PART_EDITOR_CHECKBOX,      FALSE, TRUE,  FALSE, TRUE,  FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE,
    START_AT_BOOT_XBOX,        FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    AUTO_MONITOR_CHECKBOX,     FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    BROWSE_FOR_RESTORE_PB,     FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    RESTORE_PATH_EBOX,         FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    RESTORE_PATH_GBOX,         FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE,
    E20_RADIO,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    M2_RADIO,                  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    M20_RADIO,                 FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    RADIO_GROUPBOX,            FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    DIST_UNITS_TBOX,           FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    VEL_UNITS_TBOX,            FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    ROD_PITCH_TBOX,            FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
    };

int NOF_CONTROLS = sizeof(winfo)/sizeof(WINDOW_INFO_ENTRY);
int E20_RADIO_INDEX = 11;

HWND w;
int  c;
int  i;
BOOLEAN b;
BOOLEAN using_restore_point;
STRING_CLASS s;

w = CurrentDialogWindow.handle();
set_global_computer_type_from_buttons( w );

using_restore_point = FALSE;
b = FALSE;
w = CurrentDialogWindow.control( USE_RESTORE_POINT_XBOX );
if ( ComputerType == MONITORING_COMPUTER_TYPE )
    {
    s.get_text( CurrentDialogWindow.control(RESTORE_PATH_EBOX) );
    if ( !s.isempty() )
        {
        b = TRUE;
        using_restore_point = is_checked( w );
        }
    }
EnableWindow( w, b );

c = ComputerType;
if ( !HaveSureTrak )
    c += NOF_COMPUTER_TYPES;

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    w = CurrentDialogWindow.control( winfo[i].id );
    if ( using_restore_point && i>=E20_RADIO_INDEX )
        EnableWindow( w, FALSE );
    else
        EnableWindow( w, winfo[i].is_enabled[c] );
    }
}

/***********************************************************************
*                           BROWSE_FOR_RESTORE                         *
***********************************************************************/
void browse_for_restore( HWND hWnd )
{
TCHAR        buf[MAX_PATH];
BROWSEINFO   bi;
ITEMIDLIST * idlist;
LPMALLOC     ip;
NAME_CLASS   s;
WINDOW_CLASS w;

bi.hwndOwner      = hWnd;
bi.pidlRoot       = 0;
bi.pszDisplayName = buf;
bi.lpszTitle      = TEXT("Choose the folder to restore ( e.g., d:\\autoback\\current )");
bi.ulFlags        = 0;
bi.lpfn           = 0;
bi.lParam         = 0;
bi.iImage         = 0;

if ( SUCCEEDED(SHGetMalloc(&ip)) )
    {
    idlist = SHBrowseForFolder( &bi );
    if ( idlist != NULL )
        {
        if ( SHGetPathFromIDList(idlist, buf) )
            {
            set_text( hWnd, RESTORE_PATH_EBOX, buf );
            w = GetDlgItem( hWnd, USE_RESTORE_POINT_XBOX );
            s = buf;
            s.cat_path( DataString );
            if ( s.directory_exists() )
                {
                w.enable();
                set_checkbox( w.handle(), TRUE );
                s = buf;
                HaveSureTrak = is_suretrak( s );
                set_checkbox( hWnd, SURETRAK_CONTROL_CHECKBOX, HaveSureTrak );
                }
            else
                {
                set_checkbox( w.handle(), FALSE );
                w.disable();
                st_message_box( TEXT("NOT_A_RESTORE_PT"), TEXT("CHOSEN_DIR") );
                }
            enable_install_controls();
            }
        ip->Free( idlist );
        }
    ip->Release();
    }
}

/***********************************************************************
*                         INIT_STARTUP_DIALOG                          *
***********************************************************************/
static void init_startup_dialog( HWND hWnd )
{
WINDOW_CLASS w;

RestorePointPath = TEXT( "d:\\autoback\\current" );
if ( RestorePointPath.directory_exists() )
    {
    RestorePointPath.set_text( hWnd, RESTORE_PATH_EBOX );
    }
else
    {
    w = GetDlgItem( hWnd, USE_RESTORE_POINT_XBOX );
    w.disable();
    }
}

/***********************************************************************
*                  SET_STARTUP_SETUP_STRINGS                    *
***********************************************************************/
static void set_startup_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { WILL_INSTALL_STATIC,       TEXT("WILL_INSTALL") },
    { IDOK,                      TEXT("CONTINUE") },
    { IDCANCEL,                  TEXT("CANCEL") },
    { PART_EDITOR_CHECKBOX,      TEXT("START_PART_SETUPS") },
    { MONITOR_SETUPS_CHECKBOX,   TEXT("START_MON_SETUPS") },
    { START_MONITOR_CHECKBOX,    TEXT("START_FT2") },
    { SURETRAK_CONTROL_CHECKBOX, TEXT("ST2_INSTALLED") },
    { RADIO_GROUPBOX,            TEXT("CTL_SETUP") },
    { E20_RADIO,                 TEXT("IN_UNITS") },
    { M2_RADIO,                  TEXT("MM_UNITS") },
    { M20_RADIO,                 TEXT("MM_UNITS") },
    { E20_VEL_TB,                TEXT("IPS") },
    { M2_VEL_TB,                 TEXT("MPS") },
    { M20_VEL_TB,                TEXT("MPS") },
    { E20_PITCH_TB,              TEXT("20P") },
    { M2_PITCH_TB,               TEXT("2MM") },
    { M20_PITCH_TB,              TEXT("20P") },
    { START_AT_BOOT_XBOX,        TEXT("START_AT_BOOT") },
    { DIST_UNITS_TBOX,           TEXT("DIST_UNITS") },
    { VEL_UNITS_TBOX,            TEXT("VEL_UNITS") },
    { ROD_PITCH_TBOX,            TEXT("ROD_PITCH") },
    { MONITORING_COMPUTER_RADIO, TEXT("MON_CO") },
    { DATA_ARCHIVER_RADIO,       TEXT("DA_CO") },
    { NO_NETWORK_RADIO,          TEXT("LOCAL_INS") },
    { OFFICE_WORKSTATION_RADIO,  TEXT("OFWRK") },
    { USE_RESTORE_POINT_XBOX,    TEXT("USE_RESTORE") },
    { BROWSE_FOR_RESTORE_PB,     TEXT("BROWSE_FOR_RES") },
    { RESTORE_PATH_GBOX,         TEXT("RES_FROM") },
    { CO_TYPE_STATIC,            TEXT("CO_TYPE") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    THIS_COMPUTER_EBOX,
    RESTORE_PATH_EBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                         POSITION_CHOICE_RECT                         *
***********************************************************************/
void position_choice_rect( LPARAM lParam )
{
WINDOW_CLASS button;
RECT button_rect;
RECT choose_rect;
int  height;

button = (HWND) lParam;
button.get_move_rect( button_rect );
ChoiceRect.get_move_rect( choose_rect );
height = choose_rect.bottom - choose_rect.top;

choose_rect.top = button_rect.top - 1;
choose_rect.bottom = choose_rect.top + height;

ChoiceRect.move( choose_rect.left, choose_rect.top, TRUE );
InvalidateRect( CurrentDialogWindow.handle(), &choose_rect, TRUE );
}

/***********************************************************************
*                         STARTUP_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK startup_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
static HWND computer_ebox = 0;

cmd = HIWORD( wParam );
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        ChoiceRect = GetDlgItem( hWnd, CHOICE_RECT );
        MainWindow.shrinkwrap( hWnd );
        set_startup_setup_strings( hWnd );
        make_sure_i_am_on_screen();
        MainWindow.set_title( StringTable.find(TEXT("STARTUP_TITLE")) );
        CurrentDialogWindow.set_focus( IDOK );
        CurrentDialogWindow.post( WM_DBINIT );
        computer_ebox = GetDlgItem( hWnd, THIS_COMPUTER_EBOX );
        CheckRadioButton( hWnd, E20_RADIO, M20_RADIO, E20_RADIO );
        CheckRadioButton( hWnd, MONITORING_COMPUTER_RADIO, NO_NETWORK_RADIO, MONITORING_COMPUTER_RADIO );
        init_startup_dialog( hWnd );
        enable_install_controls();
        return TRUE;

    case WM_DBINIT:
        OriginalComputerName.set_text( computer_ebox );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( NEW_SYSTEM_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SURETRAK_CONTROL_CHECKBOX:
                HaveSureTrak = is_checked( hWnd, SURETRAK_CONTROL_CHECKBOX );
            case USE_RESTORE_POINT_XBOX:
            case MONITORING_COMPUTER_RADIO:
            case OFFICE_WORKSTATION_RADIO:
            case NO_NETWORK_RADIO:
            case DATA_ARCHIVER_RADIO:
                enable_install_controls();
                return TRUE;

            case BROWSE_FOR_RESTORE_PB:
                browse_for_restore( hWnd );
                return TRUE;

            case E20_RADIO:
            case M2_RADIO:
            case M20_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    position_choice_rect( lParam );
                    return TRUE;
                    }
                return FALSE;

            case IDOK:
                NewComputerName.get_text( computer_ebox );
                if ( NewComputerName.len() < 3 )
                    {
                    st_message_box( TEXT("NO_CO_NAME"), TEXT("UNABLE_TO_CONTINUE") );
                    OriginalComputerName.set_text( computer_ebox );
                    return TRUE;
                    }
                NewComputerName.add_ending_backslash();
                NeedPartEditor = have_checkmark( PART_EDITOR_CHECKBOX );

                set_global_computer_type_from_buttons( hWnd );

                NeedStartMenuEntry = have_checkmark( START_AT_BOOT_XBOX );
                NeedMonitorConfig  = have_checkmark( MONITOR_SETUPS_CHECKBOX );

                if ( is_checked(hWnd, USE_RESTORE_POINT_XBOX) )
                    {
                    RestorePointPath.get_text( hWnd, RESTORE_PATH_EBOX );
                    if ( RestorePointPath.directory_exists() )
                        {
                        UsingRestorePoint = TRUE;
                        }
                    else
                        {
                        UsingRestorePoint = FALSE;
                        st_message_box( TEXT("BAD_RESTORE_PT"), TEXT("UNABLE_TO_CONTINUE") );
                        return TRUE;
                        }
                    }

                HaveSureTrak        = have_checkmark( SURETRAK_CONTROL_CHECKBOX );

                /*
                ---------------------------
                Get the suretrak units type
                --------------------------- */
                StSorcIndex = 0;
                if ( is_checked(hWnd, M2_RADIO) )
                    StSorcIndex = 1;
                if ( is_checked(hWnd, M20_RADIO) )
                    StSorcIndex = 2;


                NeedMonitorStartup = have_checkmark( START_MONITOR_CHECKBOX );

                if ( ComputerType==OFFICE_WORKSTATION_TYPE || ComputerType==NO_NETWORK_TYPE )
                    {
                    NeedMonitorStartup  = FALSE;
                    NeedMonitorConfig   = FALSE;
                    }

                if ( NewComputerName.directory_exists() )
                    {
                    if ( !rename_root_directory(hWnd) )
                        return TRUE;
                    }
                else if ( !NewComputerName.create_directory() )
                    {
                    st_message_box( TEXT("CANT_CREATE_DIR"), TEXT("UNABLE_TO_CONTINUE") );
                    return TRUE;
                    }

                CurrentDialogWindow = 0;

                CreateDialog(
                    MainInstance,
                    TEXT("INSTALL_STATUS_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) install_status_dialog_proc );

                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                  SET_UPDATE_STARTUP_SETUP_STRINGS                    *
***********************************************************************/
static void set_update_startup_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,               TEXT("CONTINUE") },
    { IDCANCEL,           TEXT("CANCEL") },
    { WILL_UPDATE_STATIC, TEXT("WILL_UPDATE") },
    { EXIT_ALL_STATIC,    TEXT("EXIT_ALL") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, NULL );
}

/***********************************************************************
*                  UPDATE_STARTUP_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK update_startup_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        MainWindow.set_title( StringTable.find(TEXT("UPDATE_STARTUP_TITLE")) );
        set_update_startup_setup_strings( hWnd );
        CurrentDialogWindow.set_focus( IDOK );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( UPDATE_SYSTEM_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;

            case IDOK:
                CurrentDialogWindow = 0;
                CreateDialog(
                    MainInstance,
                    TEXT("FIND_DIRECTORY_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) find_directory_dialog_proc );

                DestroyWindow( hWnd );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                THERE_ARE_SPACES_IN_THE_DIRECTORY_PATH                *
***********************************************************************/
BOOLEAN there_are_spaces_in_the_directory_path()
{
if ( findchar(SpaceChar, FloppyDirectoryName) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                  SET_INSTALL_OR_UPDATE_SETUP_STRINGS                    *
***********************************************************************/
static void set_install_or_update_setup_strings( HWND hWnd )
{
STRING_CLASS s;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { INSTALL_NEW_BUTTON,     TEXT("INSTALL_NEW") },
    { UPDATE_EXISTING_BUTTON, TEXT("UPDATE_EXISTING") },
    { IDCANCEL,               TEXT("CANCEL") },
    { IU_HELP,                TEXT("HELP_STRING") },
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, NULL );
s = StringTable.find( TEXT("IU_TITLE") );
MainWindow.set_title( s.text() );
}

/***********************************************************************
*                       CHOOSE_INSTALL_OR_UPDATE_PROC                  *
***********************************************************************/
BOOL CALLBACK choose_install_or_update_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );

        if ( there_are_spaces_in_the_directory_path() )
            {
            st_message_box( TEXT("SPACES_IN_PATH"), TEXT("UNABLE_TO_CONTINUE") );
            MainWindow.close();
            }
        set_install_or_update_setup_strings( hWnd );
        CurrentDialogWindow.set_focus( INSTALL_NEW_BUTTON );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( CONTENTS_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case INSTALL_NEW_BUTTON:
                CurrentDialogWindow = 0;
                CreateDialog(
                    MainInstance,
                    TEXT("STARTUP_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) startup_dialog_proc );
                DestroyWindow( hWnd );
                return TRUE;

            case UPDATE_EXISTING_BUTTON:
                CurrentDialogWindow = 0;
                CreateDialog(
                    MainInstance,
                    TEXT("UPDATE_STARTUP_DIALOG"),
                    MainWindow.handle(),
                    (DLGPROC) update_startup_dialog_proc );
                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;

            case IU_HELP:
                 gethelp( CONTENTS_HELP );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/**********************************************************************
*                           LOAD_RESOURCES                            *
**********************************************************************/
static void load_resources()
{
NAME_CLASS s;

StringTable.set_language( CurrentLanguage );

/*
-----------------------------------------------------
Read my file first so I can override the common names
----------------------------------------------------- */
s = FloppyDirectoryName;
s.cat_path( ResourcesString );
s.cat_path( SetupResourceFile );
StringTable.read( s );

s = FloppyDirectoryName;
s.cat_path( ResourcesString );
s.cat_path( CommonResourceFile );
StringTable.append( s );

statics_startup();
}

/**********************************************************************
*                          do_language_selection                      *
**********************************************************************/
void do_language_selection( HWND hWnd )
{
INI_CLASS     ini;
NAME_CLASS    s;
LISTBOX_CLASS lb;

lb.init( hWnd, LANGUAGE_LB );
s = lb.selected_text();
if ( !s.isempty() )
    {
    CurrentLanguage = s;
    load_resources();
    }
}

/**********************************************************************
*                            fill_languages                           *
**********************************************************************/
void fill_languages( HWND hWnd )
{
static wchar_t UnicodeIntroducer = L'\xFEFF';

TCHAR       * cp;
FILE_CLASS    f;
int           i;
LISTBOX_CLASS lb;
int           nof_languages;
NAME_CLASS    s;

lb.init( hWnd, LANGUAGE_LB );

s = FloppyDirectoryName;
s.cat_path( ResourcesString );
s.cat_path( SetupResourceFile );

if ( f.open_for_read(s) )
    {
    /*
    ----------------------------
    Read the number of languages
    ---------------------------- */
    cp = f.readline();

    #ifdef UNICODE
    if ( *cp == UnicodeIntroducer )
        cp++;
    #endif
    excel_to_csv( cp );
    nof_languages = asctoint32( cp );

    /*
    ---------------------------------
    For each language, read the font.
    --------------------------------- */
    for ( i=0; i<nof_languages; i++ )
        {
        cp = f.readline();
        excel_to_csv( cp );
        if ( replace_char_with_null(cp, CommaChar) )
            lb.add( cp );
        }
    f.close();
    }
}

/***********************************************************************
*                          CHOOSE_LANGUAGE_PROC                        *
***********************************************************************/
BOOL CALLBACK choose_language_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;

cmd = HIWORD( wParam );
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentDialogWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        fill_languages( hWnd );
        CurrentDialogWindow.set_focus( LANGUAGE_LB );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( CONTENTS_HELP );
         return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case LANGUAGE_LB:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_language_selection( hWnd );
                    CurrentDialogWindow = 0;
                    CreateDialog(
                        MainInstance,
                        TEXT("INSTALL_OR_UPDATE_DIALOG"),
                        MainWindow.handle(),
                        (DLGPROC) choose_install_or_update_proc );
                    DestroyWindow( hWnd );
                    return TRUE;
                    }
                break;

            case IDCANCEL:
                MainWindow.post( WM_CLOSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
switch (msg)
    {
    case WM_CREATE:
        MainWindow = hWnd;
        WhiteBrush = CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
        return 0;

    case WM_DESTROY:
        if ( WhiteBrush )
            {
            DeleteObject( WhiteBrush );
            WhiteBrush = 0;
            }
        PostQuitMessage( 0 );
        return 0;

    case WM_SETFOCUS:
        CurrentDialogWindow.set_focus();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp( CONTENTS_HELP );
         return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}


/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( HINSTANCE this_instance, int cmd_show )
{
static TCHAR MAIN_TITLE[] = TEXT("True-Trak 2020� Installation");
WNDCLASS wc;

init_original_computer();

/*
-------------------------------------------------------
Get the install directory and append a backslash to it.
------------------------------------------------------- */
get_exe_directory( FloppyDirectoryName );
append_backslash_to_path( FloppyDirectoryName );

MainInstance = this_instance;

wc.lpszClassName = MyName;
wc.hInstance     = this_instance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

CreateWindow(
    MyName,                         // window class name
    MAIN_TITLE,                     // window title
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,   // X,y
    405, 70,                        // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT("CHOOSE_LANGUAGE_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) choose_language_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL  status;

CoInitialize(0);

init( this_instance, cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( CurrentDialogWindow.handle() )
        status = IsDialogMessage( CurrentDialogWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

statics_shutdown();
CoUninitialize();
return(msg.wParam);
}
