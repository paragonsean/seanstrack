#include <windows.h>
#include <htmlhelp.h>
#include <commctrl.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\rectclas.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"

#include "resource.h"

#include "backupcl.h"
#include "extern.h"

const int32 MEG_FOR_FULL_DISK = 50;

static HWND SaveOptionsWindow = 0;

HANDLE  MailSlotEvent;
BOOLEAN MailSlotIsRunning;
BOOLEAN NeedToZipAll    = FALSE;
BOOLEAN UpdatingListbox = FALSE;
BOOLEAN UpdatingBoxes   = FALSE;
BOOLEAN ShuttingDown;

TCHAR MyClassName[]   = "DataServer";
TCHAR MyWindowTitle[] = "Data Archiver";

static DS_BACKUP_CLASS SaveConfig;

BOOLEAN       DiskIsFull      = FALSE;

TIME_CLASS BackupTime[MAX_BACKUP_TIME_COUNT];
TCHAR *    BackupTimeIniName[MAX_BACKUP_TIME_COUNT] = {
           { TEXT( "BackupTime1" ) },
           { TEXT( "BackupTime2" ) },
           { TEXT( "BackupTime3" ) }
           };

int32      FreeDiskMegabytes = 0;
int32      NofBackupTimes = 0;

LISTBOX_CLASS StatusListBox;

static const DWORD MS_TO_WAIT     = 505;
static const TCHAR MailSlotName[] = "\\\\.\\mailslot\\v5\\eventman";

static TCHAR YString[]     = TEXT( "Y" );
static TCHAR NString[]     = TEXT( "N" );

static TCHAR BackupDirKey[]       = TEXT( "BackupDir" );
static TCHAR MaxZipCountKey[]     = TEXT( "MaxZipCount" );
static TCHAR UseMaxZipCountKey[]  = TEXT( "UseMaxZipCount" );
static TCHAR UseBackupTimeKey[]   = TEXT( "UseBackupTime" );
static TCHAR SkipDowntimeZipKey[] = TEXT( "SkipDowntimeZip" );

static TCHAR DefaultHaveAlarmShot[]   = TEXT( "A" );
static TCHAR DefaultHaveGoodShot[]    = TEXT( "G" );
static TCHAR DefaultHaveWarningShot[] = TEXT( "W" );

static TCHAR DsBackupHelpFile[]   = TEXT( "dsbackup.chm" );

TCHAR * resource_string( UINT resource_id );
void resource_message_box( UINT msg_id, UINT title_id );
BOOL CALLBACK global_file_settings_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

/***********************************************************************
*                             SET_INT_EBOX                             *
***********************************************************************/
void set_int_ebox( UINT id, int32 value )
{
set_text( SaveOptionsWindow, id, int32toasc(value) );
}

/***********************************************************************
*                          READ_NEXT_MESSAGE                           *
***********************************************************************/
static BOOLEAN read_next_message( HANDLE mailslot_handle, DWORD slen )
{
TCHAR    * s;
DWORD      bytes_read;
FILE_CLASS f;
NAME_CLASS name;
TIME_CLASS t;

if ( slen > 0 )
    {
    s = maketext( slen );
    if ( s )
        {
        if( ReadFile(mailslot_handle, s, slen, &bytes_read, NULL) )
            {
            *(s+bytes_read) = NullChar;
            if ( LogFileEnable )
                {
                name = LogFileDir;
                if ( !name.directory_exists() )
                    name.create_directory();
                name.cat_path( MailslotListLogFile );
                if ( f.open_for_append(name) )
                    {
                    t.get_local_time();
                    name = t.text();
                    name += TEXT(" - " );
                    name += s;
                    name.strip_crlf();
                    f.writeline( name );
                    f.close();
                    }
                }
            if ( !DiskIsFull )
                BList.process_mailslot_message( s );
            }
        delete[] s;
        }
    }

return TRUE;
}

/***********************************************************************
*                           MAILSLOT_THREAD                            *
***********************************************************************/
DWORD mailslot_thread( int * notused )
{
BOOL   status;
int32  i;
HANDLE mailslot_handle;
DWORD  next_msg_size;
DWORD  nof_msg;
TIME_CLASS tc;

mailslot_handle = CreateMailslot(
        MailSlotName,
        0,                             // no maximum message size
        MAILSLOT_WAIT_FOREVER,         // no time-out for operations
        (LPSECURITY_ATTRIBUTES) NULL); // no security attributes

if ( mailslot_handle == INVALID_HANDLE_VALUE)
    {
    resource_message_box( MAILSLOT_STRING, CREATE_FAILED_STRING );
    return 0;
    }

MailSlotIsRunning = TRUE;

while ( TRUE )
    {
    if ( ShuttingDown )
        break;

    if ( NeedToReloadZipPaths )
        {
        NeedToReloadZipPaths = FALSE;
        BList.reload_zip_paths();
        }

    if ( BackupAtTime )
        {
        tc.get_local_time();
        for ( i=0; i<NofBackupTimes; i++ )
            {
            if ( tc > BackupTime[i] )
                {
                NeedToZipAll = TRUE;
                break;
                }
            }

        for ( i=0; i<NofBackupTimes; i++ )
            {
            while ( tc > BackupTime[i] )
                {
                BackupTime[i] += SEC_PER_DAY;
                }
            }
        }

    if ( !BackupDirectory.isempty() )
        {
        if ( *BackupDirectory.text() != BackSlashChar )
            {
            i = free_megabytes( BackupDirectory.text() );
            if ( FreeDiskMegabytes != i )
                {
                FreeDiskMegabytes = i;
                set_text( MainDialogWindow, K_FREE_TBOX, int32toasc(i) );
                if ( i <= MEG_FOR_FULL_DISK && !DiskIsFull )
                    {
                    DiskIsFull = TRUE;
                    ShowWindow( GetDlgItem(MainDialogWindow, DISK_FULL_TBOX), SW_SHOW );
                    }

                if ( i > MEG_FOR_FULL_DISK && DiskIsFull )
                    {
                    DiskIsFull = FALSE;
                    ShowWindow( GetDlgItem(MainDialogWindow, DISK_FULL_TBOX), SW_HIDE );
                    }
                }
            }
        }

    if ( NeedToZipAll )
        {
        NeedToZipAll = FALSE;
        BList.zip_all_parts();
        }

    status = GetMailslotInfo(
        mailslot_handle,
        (LPDWORD) NULL,               // no maximum message size
        &next_msg_size,               // size of next message
        &nof_msg,                     // number of messages
        (LPDWORD) NULL);              // no read time-out

    if ( status )
        {
        if ( next_msg_size != MAILSLOT_NO_MESSAGE )
            read_next_message( mailslot_handle, next_msg_size );
        }

    if ( WaitForSingleObject(MailSlotEvent, MS_TO_WAIT) == WAIT_FAILED )
        resource_message_box( UNABLE_TO_CONTINUE_STRING, MAILSLOT_WAIT_FAIL_STRING );
    ResetEvent( MailSlotEvent );
    }

CloseHandle( mailslot_handle );

MailSlotIsRunning = FALSE;
PostMessage( MainWindow, WM_CLOSE, 0, 0L );
return 0;
}

/***********************************************************************
*                         UPDATE_ZIP_TIME                              *
***********************************************************************/
static void update_zip_time()
{
TCHAR      buf[ALPHATIME_LEN+1];
TIME_CLASS t;
UINT       id;
int32      i;
INI_CLASS  ini;

NofBackupTimes = 0;

ini.set_file( VisiTrakIniPath.text() );
ini.set_section( ConfigSection );

for ( id = TIME_1_EBOX; id <= TIME_3_EBOX; id++ )
    {
    if ( get_text(buf, MainDialogWindow, id, ALPHATIME_LEN) )
        {
        if ( is_time(buf) )
            {
            t.get_local_time();
            BackupTime[NofBackupTimes] = t;
            BackupTime[NofBackupTimes].set_time( buf );
            if ( t > BackupTime[NofBackupTimes] )
                BackupTime[NofBackupTimes] += SEC_PER_DAY;
            ini.put_string( BackupTimeIniName[NofBackupTimes], buf );
            NofBackupTimes++;
            }
        }
    }

for ( i=NofBackupTimes; i<MAX_BACKUP_TIME_COUNT; i++ )
    ini.put_string( BackupTimeIniName[NofBackupTimes], UNKNOWN );
}

/***********************************************************************
*                         UPDATE_MAX_ZIP_COUNT                         *
***********************************************************************/
static void update_max_zip_count()
{
TCHAR buf[MAX_INTEGER_LEN+1];
INI_CLASS ini;

ini.set_file( VisiTrakIniPath.text() );
if ( get_text(buf, MainDialogWindow, ZIP_COUNT_EBOX, MAX_INTEGER_LEN) )
    {
    if ( is_checked( MainDialogWindow, ZIP_COUNT_XBOX) )
        BList.set_max_zip_count( asctoint32(buf) );
    else
        BList.clear_max_zip_count();

    ini.put_string( ConfigSection, MaxZipCountKey, buf );
    }
}

/***********************************************************************
*                           INIT_DIALOG_CONTROLS                       *
***********************************************************************/
static void init_dialog_controls( HWND w )
{
static TCHAR zero[] = TEXT( "0" );

int32   i;
TCHAR * cp;
BOOLEAN need_check;
TIME_CLASS t;
INI_CLASS ini;

ini.set_file( DsBackupIniPath.text() );
ini.set_section( ConfigSection );
HaveMirror = ini.get_boolean( HaveMirrorKey );
if ( HaveMirror )
    {
    MirrorRoot = ini.get_string( MirrorRootKey );
    if ( MirrorRoot.len() < 2 )
        {
        HaveMirror = FALSE;
        }
    else
        {
        cp = MirrorRoot.text();
        cp++;
        if ( *cp != ColonChar && *cp != BackSlashChar )
            HaveMirror = FALSE;
        }

    if ( !HaveMirror )
        MessageBox( w, TEXT("Not a Root Directory"), TEXT("Error in Mirror Drive"), MB_OK );
    }

ini.set_section( DefaultSection );
Using_Machine_Based_Zip_Paths = ini.get_boolean( MachineBasedZipPathsKey );
DefaultMachineZipPath = ini.get_string( BackupMachinePathString );
DefaultPartZipPath    = ini.get_string( BackupPartPathString );

/*
----------------------
Max zip count controls
---------------------- */
ini.set_file( VisiTrakIniPath.text() );
ini.set_section( ConfigSection );
need_check = ini.get_boolean( UseMaxZipCountKey );
set_checkbox( w, ZIP_COUNT_XBOX, need_check );

SkipDowntimeZip = ini.get_boolean( SkipDowntimeZipKey );
set_checkbox( w, IGNORE_DOWNTIME_XBOX, SkipDowntimeZip );

cp = zero;
if ( ini.find( MaxZipCountKey) )
    cp = ini.get_string();
set_text( w, ZIP_COUNT_EBOX, cp );

if ( need_check )
    BList.set_max_zip_count( asctoint32(cp) );

/*
-------------------------
Backup at a time controls
------------------------- */
BackupAtTime = ini.get_boolean( UseBackupTimeKey );
NofBackupTimes = 0;
for ( i=0; i<MAX_BACKUP_TIME_COUNT; i++ )
    {
    if ( !ini.find(BackupTimeIniName[i]) )
        break;
    cp = ini.get_string();
    t.get_local_time();
    BackupTime[NofBackupTimes] = t;
    BackupTime[NofBackupTimes].set_time( cp );
    if ( t > BackupTime[NofBackupTimes] )
        BackupTime[NofBackupTimes] += SEC_PER_DAY;
    set_text( w, TIME_1_EBOX+NofBackupTimes, cp );
    NofBackupTimes++;
    }

/*
----------------------------------
Blank out the boxes I am not using
---------------------------------- */
i = NofBackupTimes;
while ( i < MAX_BACKUP_TIME_COUNT )
    {
    set_text( w, TIME_1_EBOX+i, EmptyString );
    i++;
    }

set_checkbox( w, ZIP_TIME_XBOX, BackupAtTime );
set_checkbox( w, LOG_FILE_ENABLE_XB, LogFileEnable );
}

/***********************************************************************
*                            SET_TAB_STOPS                             *
***********************************************************************/
static void set_tab_stops( HWND w )
{
LISTBOX_CLASS lb;

lb.init( w, ZIP_COUNT_LISTBOX );
lb.set_tabs( MAIN_PART_STATIC, MAIN_LAST_SHOT_STATIC );
}

/***********************************************************************
*                         SAVE_LOG_FILE_ENABLE                         *
***********************************************************************/
static void save_log_file_enable()
{
INI_CLASS  ini;
NAME_CLASS s;

ini.set_file( DsBackupIniPath.text() );
ini.set_section( ConfigSection );
ini.put_boolean( LogFileEnableKey, LogFileEnable );
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOOLEAN HaveNewZipCount = FALSE;
static BOOLEAN HaveNewZipTime  = FALSE;

int     id;
int     cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        init_dialog_controls( hWnd );
        set_tab_stops( hWnd );
        StatusListBox.init( hWnd, DS_STATUS_LB );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( HaveNewZipCount )
                    {
                    update_max_zip_count();
                    HaveNewZipCount = FALSE;
                    }

                if ( HaveNewZipTime )
                    {
                    update_zip_time();
                    HaveNewZipTime = FALSE;
                    }
                return TRUE;

            case ZIP_TIME_XBOX:
                if ( is_checked( MainDialogWindow, ZIP_TIME_XBOX) )
                    {
                    update_zip_time();
                    BackupAtTime = TRUE;
                    }
                else
                    {
                    BackupAtTime  = FALSE;
                    }
                boolean_to_ini( VisiTrakIniFile, ConfigSection, UseBackupTimeKey, BackupAtTime );
                return TRUE;

            case LOG_FILE_ENABLE_XB:
                LogFileEnable = is_checked( hWnd, id );
                save_log_file_enable();
                return TRUE;

            case TIME_1_EBOX:
            case TIME_2_EBOX:
            case TIME_3_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    HaveNewZipTime = TRUE;
                    return TRUE;
                    }

                if ( cmd == EN_KILLFOCUS )
                    {
                    if ( HaveNewZipTime )
                        {
                        update_zip_time();
                        HaveNewZipTime = FALSE;
                            }
                    }

                return TRUE;

            case ZIP_COUNT_XBOX:
                update_max_zip_count();
                HaveNewZipCount = FALSE;
                boolean_to_ini( VisiTrakIniFile, ConfigSection, UseMaxZipCountKey, is_checked(MainDialogWindow, ZIP_COUNT_XBOX) );
                return TRUE;

            case IGNORE_DOWNTIME_XBOX:
                if ( is_checked( MainDialogWindow, IGNORE_DOWNTIME_XBOX) )
                    SkipDowntimeZip = TRUE;
                else
                    SkipDowntimeZip = FALSE;
                boolean_to_ini( VisiTrakIniFile, ConfigSection, SkipDowntimeZipKey, SkipDowntimeZip );
                return TRUE;

            case ZIP_COUNT_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    HaveNewZipCount = TRUE;
                    return TRUE;
                    }

                if ( cmd == EN_KILLFOCUS )
                    {
                    if ( HaveNewZipCount )
                        {
                        update_max_zip_count();
                        HaveNewZipCount = FALSE;
                        }
                    return TRUE;
                    }
                break;

            case ZIP_NOW_BUTTON:
                if ( MailSlotEvent && !NeedToZipAll )
                    {
                    NeedToZipAll = TRUE;
                    SetEvent( MailSlotEvent );
                    }
                return TRUE;

            default:
                break;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                        START_MAILSLOT_THREAD                         *
***********************************************************************/
static void start_mailslot_thread()
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                       /* no security attributes        */
    0,                                          /* use default stack size        */
    (LPTHREAD_START_ROUTINE) mailslot_thread,   /* thread function       */
    0,                                          /* argument to thread function   */
    0,                                          /* use default creation flags    */
    &id );                                      /* returns the thread identifier */

if ( !h )
    resource_message_box( CREATE_FAILED_STRING, MAILSLOT_THREAD_STRING );
}

/***********************************************************************
*                           GET_BACKUP_COMPUTER                        *
*  The backup computer is normally c:\v5\backup. I used to add this    *
*  to the list of computers but now the computer name is just the      *
*  v5 directory path w/o an ending backslash so I don't need to add    *
*  it to the official list.                                            *
***********************************************************************/
static void add_backup_computer()
{
INI_CLASS      ini;
COMPUTER_CLASS c;

ini.set_file( VisiTrakIniPath.text() );
ini.set_section( BackupSection );

if ( ini.find(BackupDirKey) )
    {
    BackupDirectory = ini.get_string();
    }
else
    {
    BackupDirectory = c.whoami();
    BackupDirectory.cat_path( TEXT("backup") );
    ini.put_string( BackupDirKey, BackupDirectory.text() );
    }
}

/***********************************************************************
*                              ABOUT_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK about_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        break;
    }

return FALSE;
}

/***********************************************************************
*                    POSITION_MACHINE_LIST_LISTBOX                     *
***********************************************************************/
static void position_machine_list_listbox( HWND hWnd )
{
RECTANGLE_CLASS r;
RECTANGLE_CLASS rc;
WINDOW_CLASS w;

w = hWnd;
w.get_client_rect( r );

w = w.control( MACHINE_LIST_LB );
w.get_move_rect( rc );
rc.left = r.left + 4;
rc.right = r.right - 4;
rc.bottom = r.bottom - 4;
w.move(rc);
}

/***********************************************************************
*                       MAKE_A_LIST_OF_MACHINES                        *
***********************************************************************/
static void make_a_list_of_machines( HWND hWnd )
{
COMPUTER_CLASS c;
LISTBOX_CLASS  lb;
MACHINE_NAME_LIST_CLASS m;
STRING_CLASS   s;

c.rewind();
while ( c.next() )
    {
    if ( c.is_present() && !c.is_this_computer() )
        m.add_computer( c.name() );
    }

lb.init( hWnd, MACHINE_LIST_LB );
lb.set_tabs( COMPUTER_DIR_STATIC, COMPUTER_DIR_STATIC );

if ( m.count() < 1 )
    {
    lb.add( resource_string(NO_MACHINES_FOUND_STRING) );
    return;
    }

s.upsize( MAX_PATH );
m.rewind();
while ( m.next() )
    {
    c.find( m.computer_name() );
    s = m.name();
    s += TabChar;
    s += c.name();
    lb.add( s.text() );
    }
}

/***********************************************************************
*                        MACHINE_LIST_DIALOG_PROC                      *
* A modal dialog that displays a list of the machines I know about     *
* and the computers and directories associated with them.              *
***********************************************************************/
BOOL CALLBACK machine_list_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        make_a_list_of_machines( hWnd );
        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_SIZE:
        position_machine_list_listbox( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
            case IDOK:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                                GETHELP                               *
***********************************************************************/
void gethelp( HWND w )
{
NAME_CLASS s;

s.get_exe_dir_file_name( DsBackupHelpFile );

HtmlHelp( w, s.text(), HH_DISPLAY_TOPIC, NULL );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
BOOL CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch (msg)
    {

    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_DBINIT :
        shrinkwrap( MainWindow, MainDialogWindow );
        shifts_startup();
        BList.load();
        /*
        --------------------------------------------------------------
        Once I've loaded the backup list I can add the backup computer
        -------------------------------------------------------------- */
        add_backup_computer();
        start_mailslot_thread();
        return 0;

    case WM_HELP:
        gethelp( hWnd );
        return 0;

    case WM_COMMAND:

        switch ( id )
            {
            case HELP_MENU:
                gethelp( hWnd );
                return 0;

            case ABOUT_MENU:
                DialogBox(
                    MainInstance,
                    "ABOUT_DIALOG",
                    hWnd,
                    (DLGPROC) about_dialog_proc );
                return 0;

            case GLOBAL_FILE_SETTINGS_MENU:
                DialogBox(
                    MainInstance,
                    "GLOBAL_FILE_SETTINGS_DIALOG",
                    hWnd,
                    (DLGPROC) global_file_settings_dialog_proc );
                return 0;

            case MACHINE_LIST_MENU:
                DialogBox(
                    MainInstance,
                    "MACHINE_LIST_DIALOG",
                    hWnd,
                    (DLGPROC) machine_list_dialog_proc );
                return 0;

            case MINIMIZE_MENU:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_MENU:
                PostMessage( hWnd, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_CLOSE:
        if ( MailSlotIsRunning )
            {
            if ( !ShuttingDown )
                {
                ShuttingDown = TRUE;
                BList.shutdown();

                if ( MailSlotEvent )
                    {
                    SetEvent( MailSlotEvent );
                    return 0;
                    }
                }
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOL init( HINSTANCE this_instance )
{
WNDCLASS wc;
COMPUTER_CLASS c;
INI_CLASS      ini;

MainInstance = this_instance;

ShuttingDown = FALSE;
c.startup();

/*
------------------------------------------------
Get the path to the ini file that everyone uses.
------------------------------------------------ */
DsBackupIniPath.get_exe_dir_file_name( DsBackupIniFile );
VisiTrakIniPath.get_exe_dir_file_name( VisiTrakIniFile );

ini.set_file( DsBackupIniPath.text() );
ini.set_section( ShotparmSection );
if ( ini.find(ParamLinesPerFileKey) )
    ParamLinesPerFile = ini.get_int();
if ( ini.find(HaveAlarmShotKey) )
    HaveAlarmShot = maketext( ini.get_string() );
else
    HaveAlarmShot = maketext( DefaultHaveAlarmShot );

if ( ini.find(HaveGoodShotKey) )
    HaveGoodShot = maketext( ini.get_string() );
else
    HaveGoodShot = maketext( DefaultHaveGoodShot );

if ( ini.find(HaveWarningShotKey) )
    HaveWarningShot = maketext( ini.get_string() );
else
    HaveWarningShot = maketext( DefaultHaveWarningShot );

/*
--------
Log File
-------- */
ini.set_section( ConfigSection );
LogFileEnable = ini.get_boolean( LogFileEnableKey );
if ( ini.find(LogFileDirKey) )
    LogFileDir = ini.get_string();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(this_instance, TEXT("DATA_SERVER_ICON") );
wc.lpszMenuName  = (LPSTR) "MainMenu";
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle,
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    279, 244,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainWindow )
    return FALSE;

/*
---------------------------------------
Create an event for the mailslot thread
--------------------------------------- */
MailSlotEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !MailSlotEvent )
    resource_message_box( UNABLE_TO_CONTINUE_STRING, MAILSLOT_EVENT_FAIL_STRING );

MainDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("MainDialog"),
    MainWindow,
    (DLGPROC) main_dialog_proc );

ShowWindow( MainWindow, SW_SHOW );
UpdateWindow( MainWindow );

return TRUE;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
shifts_shutdown();
killtext( HaveAlarmShot );
killtext( HaveGoodShot  );
killtext( HaveWarningShot );
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;
BOOL status;

if ( is_previous_instance(MyClassName, MyWindowTitle) )
    return 0;

if ( !init(this_instance) )
    return FALSE;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialogWindow )
        status = IsDialogMessage( MainDialogWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        }
    }

shutdown();
return msg.wParam;
}
