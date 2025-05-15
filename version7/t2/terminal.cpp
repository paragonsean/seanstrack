#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\iniclass.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\rectclas.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#define _MAIN_
#include "resource.h"

const UINT TIMER_ID = 1;

HINSTANCE     MainInstance;
WINDOW_CLASS  MainWindow;
HWND          MainEbox = 0;
LISTBOX_CLASS MainListBox;

bool          ShuttingDown = false;
bool          Broadcasting = false;
bool          IsCyclone    = false;
bool          WaitingForVersion = false;
int           FilesSentToCyclone = 0;

/*
---------------------
File upload Variables
--------------------- */
static NAME_CLASS      UploadDir;
static TEXT_LIST_CLASS UploadFiles;

char  START_OF_UPLOAD[]  = "START_OF_UPLOAD";
char  END_OF_UPLOAD[]    = "END_OF_UPLOAD";
char  Turn_Off_Plusses[] = "P0\r";
char  Turn_On_Plusses[]  = "P1\r";
char  LockCycloneCmd[]   = "E)\r";
char  UnlockCycloneCmd[] = "E(\r";
char  RebootCycloneCmd[] = "RB\r";
char  EraseApexCmd[]     = "E108000\r";
char  GetVersionString[] = "OV\r";

static TCHAR MyIniFileName[]     = TEXT( "terminal.ini" );
static TCHAR ConfigSection[]     = TEXT( "Config" );
static TCHAR TcpAddressKey[]     = TEXT( "TcpIpAddress" );
static TCHAR TcpPortKey[]        = TEXT( "TcpIpPort" );
static TCHAR UploadFileNameKey[] = TEXT( "UploadFileName" );
static TCHAR FlashFilesDirKey[]  = TEXT( "FlashFilesDirectory" );
static TCHAR SingleFileDirKey[]  = TEXT( "SingleFileDirectory" );
static int CurrentRadioButtonId  = SELECT_7_RADIO;
static TCHAR StarDotStar[]       = TEXT( "*.*" );
static TCHAR StarDotFlash[]      = TEXT( "*.flash" );
static TCHAR DotFlash[]          = TEXT( ".flash" );
static TCHAR DotTxt[]            = TEXT( ".txt" );
static TCHAR V650FileName[]      = TEXT( "emc_6_50" );
static TCHAR * FileDirName[]   = { TEXT("FileSet4m"), TEXT("FileSet4c"), TEXT("FileSet5"), TEXT("FileSet6"), TEXT("FileSet7") };

static TCHAR AChar         = TEXT( 'A' );
static TCHAR HChar         = TEXT( 'H' );
static TCHAR VChar         = TEXT( 'V' );
static TCHAR CommaChar     = TEXT( ',' );
static TCHAR HatChar       = TEXT( '^' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR PlusChar      = TEXT( '+'  );
static TCHAR QuestionChar  = TEXT( '?'  );
static TCHAR PoundChar     = TEXT( '#'  );
static TCHAR SpaceChar     = TEXT( ' '  );
static TCHAR TabChar       = TEXT( '\t'  );
static TCHAR LfChar        = TEXT( '\n'  );
static TCHAR CrChar        = TEXT( '\r'  );
static TCHAR UnderscoreChar = TEXT( '_' );

static TCHAR MyClassName[] = "Terminal";
static TCHAR CommaString[] = ",";
static TCHAR CrString[] =    "\r";
static TCHAR UnderscoreString[] = "_";
static TCHAR EmptyString[] = "";
static STRING_CLASS StartOfPrefix;

static const char START_OF_POS_SAMPLES[]  = TEXT( "P_BEGIN" );
static const char START_OF_TIME_SAMPLES[] = TEXT( "T_BEGIN" );

/*
--------------------------------------------------------
Whenever I see a variable (V0 thru V511) I save it here.
-------------------------------------------------------- */
static const int NOF_BOARD_VARIABLES = 512;
static int BoardVariable[NOF_BOARD_VARIABLES];
static int NextBoardVariable;
static BOOLEAN SendingBoardVariableRequests;

/*
--------------------------------------------------------------
These are from the last shot and enable me so save a shot file
-------------------------------------------------------------- */
STRING_CLASS ShotDataFile = TEXT( "shot_data.pr2" );
STRING_CLASS ShotCsvFile  = TEXT( "shot_data.csv" );
int CurrentShotNumber                  = 0;
TIME_CLASS CurrentShotTime;
FTII_NET_DATA ShotData;

u_short TcpIpPort        = 20000;
char    TcpIpAddress[16] = "192.168.254.40";
WNDPROC OriEditControlProc;

char    BroadcastAddress[16] = "192.168.254.40";
char  * BroadcastCp;
int     BroadcastCount = 0;
STRING_CLASS BroadcastMessage;

bool begin_monitoring( WINDOW_CLASS & notification_window );
void stop_monitoring();
bool set_current_address( char * new_address, u_short new_port );
void set_socket_send_event();
bool send_broadcast_message( char * sorc, bool need_to_set_event );
bool send_socket_message( char * sorc, bool need_to_set_event );

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/**********************************************************************
*                             SET_WINDOW_TITLE                        *
**********************************************************************/
static void set_window_title()
{
STRING_CLASS s;
STRING_CLASS title;

s.get_ascii( TcpIpAddress );
title = resource_string( MAIN_WINDOW_TITLE_STRING );
title += TEXT( "  " );
title += s;
MainWindow.set_title( title.text() );
}

/***********************************************************************
*                              UPLOAD_FILE                             *
***********************************************************************/
void upload_file( STRING_CLASS filename )
{
char start_message[MAX_PATH];
char  * dest;
TCHAR * cp;
TCHAR * pp;
STRING_CLASS s;
FILE_CLASS f;
bool    is_flash_file;

if ( f.open_for_read(filename) )
    {
    is_flash_file = false;
    if ( filename.contains(DotFlash) )
        is_flash_file = true;
    cp = filename_from_path( filename.text() );
    strcpy( start_message, START_OF_UPLOAD );
    dest = start_message + strlen( start_message );
    unicode_to_char( dest, cp );

    send_socket_message( start_message, false );
    send_socket_message( Turn_On_Plusses, false );
    if ( is_flash_file )
        send_socket_message( UnlockCycloneCmd, true );

    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;

        /*
        ---------------------------------------------
        Don't send comments or spaces before comments
        --------------------------------------------- */
        pp = findchar( PoundChar, cp );
        if ( pp )
            {
            while ( pp > cp )
                {
                pp--;
                if ( *pp != SpaceChar && *pp != TabChar )
                    {
                    pp++;
                    break;
                    }
                }
            *pp = NullChar;
            }
        if ( *cp != NullChar )
            {
            s = cp;
            if ( s.len() > 0 )
                {
                s += CrString;
                send_socket_message( s.text(), false );
                }
            }
        }
    send_socket_message( END_OF_UPLOAD, false );
    send_socket_message( Turn_Off_Plusses, false );
    if ( is_flash_file )
        send_socket_message( LockCycloneCmd, true );
    set_socket_send_event();
    f.close();
    }
else
    {
    s = TEXT("File name =[" );
    s += filename;
    s += TEXT( "]" );
    MessageBox( 0, s.text(), TEXT("Cannot open file"), MB_OK );
    }
}

/***********************************************************************
*                               LOAD_TCP                               *
***********************************************************************/
static void load_tcp()
{
STRING_CLASS s;
INI_CLASS    ini;

s = exe_directory();
s.cat_path( MyIniFileName );

ini.set_file( s.text() );
if ( ini.exists() )
    {
    ini.set_section( ConfigSection );

    if ( ini.find(TcpAddressKey) )
        lstrcpy( TcpIpAddress, ini.get_string() );

    if ( ini.find(TcpPortKey) )
        TcpIpPort = ini.get_ul();
    }

set_current_address( TcpIpAddress, TcpIpPort );
}

/***********************************************************************
*                               SAVE_TCP                               *
***********************************************************************/
static void save_tcp()
{
STRING_CLASS s;
INI_CLASS    ini;

s = exe_directory();
s.cat_path( MyIniFileName );

ini.set_file( s.text() );
ini.set_section( ConfigSection );

ini.put_string( TcpAddressKey, TcpIpAddress );
ini.put_ul( TcpPortKey, TcpIpPort );
}

/***********************************************************************
*                     SCROLL_LISTBOX_TO_LAST_ENTRY                     *
***********************************************************************/
static void scroll_listbox_to_last_entry()
{
int ti;

ti = MainListBox.count() - MainListBox.visible_row_count();

if ( ti > MainListBox.top_index() )
    MainListBox.set_top_index( ti );
}

/***********************************************************************
*                             close_callback                           *
***********************************************************************/
void close_callback()
{
MainListBox.set_current_index( MainListBox.add( TEXT("Disconected") ) );
scroll_listbox_to_last_entry();
}

/***********************************************************************
*                          TCPIP_SETUP_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK tcpip_setup_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;

switch (msg)
    {
    case WM_INITDIALOG:
        set_text( hWnd, TCP_IP_ADDRESS_EBOX, TcpIpAddress );
        set_text( hWnd, TCP_IP_PORT_EBOX,    ultoascii( (unsigned long) TcpIpPort) );
        return FALSE;

    case WM_COMMAND:
        id  = LOWORD( wParam );
        cmd = HIWORD( wParam );
        if ( cmd == BN_CLICKED )
            {
            if ( id == IDOK )
                {
                STRING_CLASS s;
                s.get_text( hWnd, TCP_IP_ADDRESS_EBOX );
                copystring( TcpIpAddress, s.text() );
                s.get_text( hWnd, TCP_IP_PORT_EBOX );
                TcpIpPort = s.uint_value();
                set_window_title();
                set_current_address( TcpIpAddress, TcpIpPort );
                save_tcp();
                }

            if ( id == IDOK || id == IDCANCEL )
                EndDialog( hWnd, 0 );
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          NEED_REBOOT_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK need_reboot_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        if ( id == IDOK || id == IDCANCEL )
            EndDialog( hWnd, 0 );
        break;
    }

return FALSE;
}

/***********************************************************************
*                        SAVE_UPLOAD_FILE_NAME                         *
***********************************************************************/
void save_upload_file_name( STRING_CLASS & sorc )
{
INI_CLASS    ini;
STRING_CLASS dest_file_name;

dest_file_name = exe_directory();
dest_file_name.cat_path( MyIniFileName );

ini.put_string( dest_file_name.text(), ConfigSection, UploadFileNameKey, sorc.text() );
}

/***********************************************************************
*                         GET_UPLOAD_FILE_NAME                         *
***********************************************************************/
void get_upload_file_name( HWND hWnd )
{
STRING_CLASS s;
INI_CLASS    ini;
HWND         w;

s = exe_directory();
s.cat_path( MyIniFileName );

ini.set_file( s.text() );
if ( ini.exists() )
    {
    ini.set_section( ConfigSection );

    if ( ini.find(UploadFileNameKey) )
        {
        s = ini.get_string();
        w = GetDlgItem( hWnd, UPLOAD_FILE_NAME_EBOX );
        s.set_text( w );
        PostMessage( w, EM_SETSEL, 0, -1 ); /* Select all text */
        }
    }
}

/**********************************************************************
*                        get_dir_from_radio                           *
*       Each button corresponds to a different subdirectory.          *
**********************************************************************/
static BOOLEAN get_dir_from_radio( NAME_CLASS & dest, HWND hWnd )
{
int id;

for ( id=SELECT_4M_RADIO; id<=SELECT_7_RADIO; id++ )
    {
    if ( is_checked(hWnd, id) )
        {
        dest = exe_directory();
        dest.cat_path( FileDirName[id-SELECT_4M_RADIO] );
        return TRUE;
        }
    }
return FALSE;
}

/***********************************************************************
*                          SET_BUTTON_LABELS                           *
* This used to set the labels on pushbuttons but now the file list     *
* is in the FILE_LB listbox.                                           *
***********************************************************************/
static void set_button_labels( HWND hWnd )
{
LISTBOX_CLASS   lb;
NAME_CLASS      s;
TEXT_LIST_CLASS t;

lb.init( hWnd, FILES_LB );
lb.empty();

if ( get_dir_from_radio(s, hWnd) )
    {
    t.get_file_list( s.text(), StarDotStar );
    t.rewind();
    while ( t.next() )
        lb.add( t.text() );
    }
}

/***********************************************************************
*                           UPLOAD_ALL_FILES                           *
* The number of files to send is... int             FilesSentToCyclone *
* The source directory is.......... NAME_CLASS      UploadDir          *
* The list of files is............. TEXT_LIST_CLASS UploadFiles        *
***********************************************************************/
static void upload_all_files()
{
NAME_CLASS s;
bool       have_flash;

have_flash = false;

FilesSentToCyclone = UploadFiles.count();
if ( FilesSentToCyclone < 1 )
    return;

UploadFiles.rewind();
while ( UploadFiles.next() )
    {
    if ( findstring( DotFlash, UploadFiles.text()) )
        {
        have_flash = true;
        break;
        }
    }

if ( !have_flash )
    FilesSentToCyclone = 0;

/*
--------------------------------------------------------------------------------------------
If these are text files I don't want to unprotect the board so I set FilesSentToCyclone to 0
-------------------------------------------------------------------------------------------- */
if ( FilesSentToCyclone && !IsCyclone )
    {
    /*
    -------------------------------------------------------------------------------------
    If I'm updating an Apex board to 6_50 I need to erase the program and variables first
    ------------------------------------------------------------------------------------- */
    UploadFiles.rewind();
    while ( UploadFiles.next() )
        {
        if ( strings_are_equal(V650FileName, UploadFiles.text()) )
            {
            send_socket_message( EraseApexCmd, true );
            break;
            }
        }
    }

MainListBox.add( TEXT("Files to Upload...") );

UploadFiles.rewind();
while ( UploadFiles.next() )
    MainListBox.add( UploadFiles.text() );

UploadFiles.rewind();
while ( UploadFiles.next() )
    {
    s = UploadDir;
    s.cat_path( UploadFiles.text() );
    upload_file( s );
    }
}

/***********************************************************************
*                     GET_VERSION_TO_BEGIN_UPLOAD                      *
***********************************************************************/
static void get_version_to_begin_upload()
{
MainListBox.add( TEXT("Waiting for version data... ") );
WaitingForVersion = true;
send_socket_message( GetVersionString, true );
}

/***********************************************************************
*                    UPLOAD_ALL_FILES_IN_FLASH_DIR                     *
* Source directory is UploadDir.                                       *
***********************************************************************/
static void upload_all_files_in_flash_dir()
{
UploadFiles.empty();

UploadFiles.get_file_list( UploadDir.text(), StarDotStar );

get_version_to_begin_upload();
}

/***********************************************************************
*                        ALL_STANDARD_FILES_UPLOAD                     *
***********************************************************************/
static void all_standard_files_upload( HWND hWnd )
{
if ( get_dir_from_radio(UploadDir, hWnd) )
    upload_all_files_in_flash_dir();
}

/***********************************************************************
*                         ONE_STANDARD_FILE_UPLOAD                     *
***********************************************************************/
static void one_standard_file_upload( HWND hWnd )
{
NAME_CLASS filename;

UploadFiles.empty();
if ( get_dir_from_radio(UploadDir, hWnd) )
    {
    filename.get_text( hWnd, UPLOAD_FILE_NAME_EBOX );
    if ( !filename.isempty() )
        {
        UploadFiles.append( filename.text() );
        get_version_to_begin_upload();
        }
    }
}

/***********************************************************************
*                       UPLOAD_FILE_NAME_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK upload_file_name_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static STRING_CLASS s;
static LISTBOX_CLASS lb;

int  id;
int  cmd;

switch (msg)
    {
    case WM_INITDIALOG:
        CheckRadioButton( hWnd, SELECT_4M_RADIO, SELECT_7_RADIO, CurrentRadioButtonId );
        set_button_labels( hWnd );
        set_checkbox( hWnd, UPLOAD_ALL_FILES_XBOX, TRUE );
        return FALSE;

    case WM_COMMAND:
        id  = LOWORD( wParam );
        cmd = HIWORD( wParam );

        if ( id == FILES_LB && cmd == LBN_SELCHANGE )
            {
            lb.init( hWnd, FILES_LB );
            s = lb.selected_text();
            s.set_text( hWnd, UPLOAD_FILE_NAME_EBOX );
            break;
            }

        if ( cmd == BN_CLICKED )
            {
            if ( id == IDOK )
                {
                if ( is_checked(hWnd, UPLOAD_ALL_FILES_XBOX) )
                    all_standard_files_upload( hWnd );
                else
                    one_standard_file_upload( hWnd );
                }

            if ( id == IDOK || id == IDCANCEL )
                {
                EndDialog( hWnd, 0 );
                }

            switch( id )
                {
                case SELECT_4M_RADIO:
                case SELECT_4C_RADIO:
                case SELECT_5_RADIO:
                case SELECT_6_RADIO:
                case SELECT_7_RADIO:
                    set_button_labels( hWnd );
                    break;
                }
            }
        break;
    }

return FALSE;
}


/***********************************************************************
*                        GET_FLASH_FILE_DIR                            *
***********************************************************************/
BOOLEAN get_flash_file_dir()
{
static TCHAR filter[] = TEXT( "Upload Files\0*.*\0" );
const int32 buflen = 5000;
OPENFILENAME   fh;
TCHAR        * fname;
BOOLEAN       status;
NAME_CLASS    startdir;
STRING_CLASS s;
INI_CLASS    ini;

/*
----------------------------------------------------------------------------
If I've done this before the directory I used before will be in the ini file
---------------------------------------------------------------------------- */
s = exe_directory();
s.cat_path( MyIniFileName );

ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(FlashFilesDirKey) )
    startdir = ini.get_string();
else
    startdir = exe_directory();

status = FALSE;
fname = maketext( buflen );
if ( !fname )
    return FALSE;

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow.handle();
fh.hInstance         = 0;
fh.lpstrFilter       = filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = fname;
fh.nMaxFile          = buflen;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = startdir.text();
fh.lpstrTitle        = TEXT("Choose Any Flash File to Load Them All");
fh.Flags             = 0;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*fname = NullChar;

if ( GetOpenFileName( &fh ) )
    {
    if ( dir_from_filename(fname) )
        {
        UploadDir = fname;
        upload_all_files_in_flash_dir();
        ini.put_string( FlashFilesDirKey, fname );
        status = TRUE;
        }
    }

delete[] fname;
return status;
}

/***********************************************************************
*                       BROWSE_FOR_SINGLE_FILE                         *
***********************************************************************/
BOOLEAN browse_for_single_file()
{
static TCHAR filter[] = TEXT( "All Files\0*.*\0" );
const int32 buflen = 5000;
OPENFILENAME   fh;
TCHAR        * fname;
BOOLEAN       status;
NAME_CLASS    s;
INI_CLASS     ini;

/*
----------------------------------------------------------------------------
If I've done this before the directory I used before will be in the ini file
---------------------------------------------------------------------------- */
s = exe_directory();
s.cat_path( MyIniFileName );

ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(SingleFileDirKey) )
    s = ini.get_string();
else
    s = exe_directory();

status = FALSE;
fname = maketext( buflen );
if ( !fname )
    return FALSE;

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow.handle();
fh.hInstance         = 0;
fh.lpstrFilter       = filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = fname;
fh.nMaxFile          = buflen;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = s.text();
fh.lpstrTitle        = TEXT("Choose One File to Load");
fh.Flags             = 0;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*fname = NullChar;

if ( GetOpenFileName( &fh ) )
    {
    s = filename_from_path( fname );
    if ( dir_from_filename(fname) )
        {
        ini.put_string( SingleFileDirKey, fname );
        UploadDir = fname;
        UploadFiles.empty();
        UploadFiles.append( s.text() );
        get_version_to_begin_upload();
        status = TRUE;
        }
    }

delete[] fname;
return status;
}

/***********************************************************************
*                         DO_HAVE_NEW_VERSION                          *
***********************************************************************/
static void do_have_new_version()
{
if  ( !WaitingForVersion )
    return;
WaitingForVersion = false;
upload_all_files();
}

/***********************************************************************
*                        SEND_BROADCAST_MESSAGE                        *
***********************************************************************/
void send_broadcast_message( char * sorc )
{
lstrcpy( BroadcastAddress, TcpIpAddress );
BroadcastCp = BroadcastAddress + lstrlen(BroadcastAddress);
while ( BroadcastCp > BroadcastAddress )
    {
    if ( *BroadcastCp == '.' )
        {
        BroadcastCp++;
        break;
        }
    BroadcastCp--;
    }

Broadcasting = true;
BroadcastCount = 0;
BroadcastMessage = sorc;

SetTimer( MainWindow.handle(), TIMER_ID, 250, 0 );
}

/***********************************************************************
*                            EDIT_CONTROL_PROC                         *
***********************************************************************/
LRESULT CALLBACK edit_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static STRING_CLASS lastcommand;
static STRING_CLASS s;
TCHAR * cp;
BOOLEAN is_broadcast;

is_broadcast = FALSE;

if ( msg == WM_CHAR && LOWORD(wParam) == VK_RETURN )
    {
    s.get_text( MainEbox );
    if ( !s.isempty() )
        {
        s.uppercase();
        lastcommand = s;
        cp = s.text() + s.len()-1;
        if ( *cp == TEXT('?') )
            {
            *cp = NullChar;
            is_broadcast = TRUE;
            }
        MainListBox.set_current_index( MainListBox.add( s.text()) );
        }
    s += CrString;
    if ( is_broadcast )
        send_broadcast_message( s.text());
    else
        send_socket_message( s.text(), true );
    s = EmptyString;
    s.set_text( MainEbox );
    SetFocus( MainEbox );
    return 0;
    }
else if ( (msg == WM_KEYUP || msg == WM_KEYDOWN)  && LOWORD(wParam) == VK_UP )
    {
    if ( msg == WM_KEYUP )
        {
        if ( !lastcommand.isempty() )
            {
            lastcommand.set_text( MainEbox );
            SetFocus( MainEbox );
            }
        }
    }
else
    {
    return CallWindowProc( OriEditControlProc, hWnd, msg, wParam, lParam );
    }

return 0;
}

/***********************************************************************
*                   POST_OP_STATUS_LINES_FROM_BINARY                   *
***********************************************************************/
static void post_op_status_lines_from_binary( FTII_OP_STATUS_DATA & sorc )
{
STRING_CLASS s;
int          i;
double       x;

for ( i=0; i<4; i++ )
    {
    x = sorc.dac[i];
    x /= 1000.0;
    s = "Dac ";
    s += i+1;
    s += " Volts = ";
    s += ascii_double( x );
    MainListBox.add( s.text() );
    }

s = "Position (x4) = ";
s += sorc.pos;
MainListBox.add( s.text() );

s = "Velocity (x1/sec) = ";
s += sorc.vel;
MainListBox.add( s.text() );

for ( i=0; i<20; i++ )
    {
    s = "Analog ";
    s += i+1;
    s += " = ";
    s += ultohex( (unsigned long) sorc.analog[i] );
    MainListBox.add( s.text() );
    }

s = "Isw1 = ";
s += ultohex( (unsigned long) sorc.isw1 );
MainListBox.add( s.text() );

s = "Osw1 = ";
s += ultohex( (unsigned long) sorc.osw1 );
MainListBox.add( s.text() );

s = "Monitor Status = ";
s += ultohex( (unsigned long) sorc.monitor_status );
MainListBox.add( s.text() );

s = "Control Status = ";
s += ultohex( (unsigned long) sorc.status_word1 );
MainListBox.add( s.text() );

s = "Warning = ";
s += sorc.warning;
MainListBox.add( s.text() );

s = "Fatal = ";
s += sorc.fatal_error;
MainListBox.add( s.text() );

s = "Current Block Number = ";
s += sorc.blk_no;
MainListBox.add( s.text() );
}

/***********************************************************************
*                 POST_OSCILLOSCOPE_LINES_FROM_BINARY                  *
***********************************************************************/
static void post_oscilloscope_lines_from_binary( FTII_OSCILLOSCOPE_DATA & sorc )
{
static STRING_CLASS s;
int i;
int j;
for ( i=0; i<5; i++ )
    {
    s = TEXT( "Os Data[" );
    s += i;
    s += TEXT( "] = " );
    for ( j=0; j<4; j++ )
        {
        s += sorc.ch_17_20[i][j];
        if ( j<3 )
            s += TEXT( ", " );
        }
    MainListBox.add( s.text() );
    }
}

/***********************************************************************
*                  POST_SAMPLE_LINES_FROM_BINARY                       *
***********************************************************************/
static void post_sample_lines_from_binary( int point_number, FTII_SAMPLE & sorc )
{
const int X4_COUNTS_PER_POINT = 4;
static STRING_CLASS prefix;
static STRING_CLASS s;
int i;

prefix = StartOfPrefix;
prefix += point_number;
prefix += CommaString;

/*
-------------------------------------------------------------
                         First line
P_7_2,MS506P8Q12155743931Q22155801944Q32155860755Q42155918824
------------------------------------------------------------- */
s =  prefix;
s += "MS";
s += sorc.one_ms_timer;
s += "P";
s += sorc.position;
for ( i=0; i<4; i++ )
    {
    s += "Q";
    s += i+1;
    s += sorc.velcount[i];
    }

MainListBox.add( s.text() );

/*
-----------------------------------------------------------
                         Second Line
P_7_2,CH14CH21174CH32004CH4341dCH54004CH65003CH76003CH87003
----------------------------------------------------------- */
s = prefix;
for ( i=0; i<8; i++ )
    {
    s += "CH";
    s += i+1;
    // s += unsigned int( 0xFFF & sorc.analog[i] );
    s += ultohex( (unsigned long) sorc.analog[i] );
    }
MainListBox.add( s.text() );

/*
------------------------------------------------
                  Third Line
P_7_2,ISW1ffffffffISW20ISW30ISW42OSW10OSW20OSW30
------------------------------------------------ */
s = prefix;
s += "ISW1";
s += ultohex( (unsigned long) sorc.isw1 );
s += "ISW20ISW30ISW4";
s += ultohex( (unsigned long) sorc.isw4 );
s += "OSW1";
s += ultohex( (unsigned long) sorc.osw1 );
s += "OSW20OSW30";
MainListBox.add( s.text() );
}

/***********************************************************************
*                  POST_PARAMETER_LINES_FROM_BINARY                       *
***********************************************************************/
static void post_parameter_lines_from_binary( FTII_PARAMETERS & sorc )
{
static STRING_CLASS s;
int i;


/*
----------------------------------
           First Line
C_7_1,293 #Biscuit size, X4 counts
---------------------------------- */
s = StartOfPrefix;
s += "1,";
s += (unsigned int) sorc.biscuit_size;
s += " #Biscuit size, X4 counts";
MainListBox.add( s.text() );

/*
------------------------------
          Second Line
C_7_2,702915 #Cycle time in mS
------------------------------ */
s = StartOfPrefix;
s += "2,";
s += (unsigned int) sorc.cycle_time;
s += " #Cycle time in mS";
MainListBox.add( s.text() );

/*
------------------------------------
           Third Line
C_7_3,126 #Eos position in X1 counts
------------------------------------ */
s = StartOfPrefix;
s += "3,";
s += (int ) sorc.eos_pos;
s += " #Eos position in X1 counts";
MainListBox.add( s.text() );

for ( i=0; i<NOF_STATIC_ANALOGS; i++ )
    {
    s = TEXT( "Static Analog " );
    s += i;
    s += TEXT( " H" );
    s += ultohex( (unsigned long) sorc.static_analog[i] );
    MainListBox.add( s.text() );
    }
}

/***********************************************************************
*                           CHECK_FOR_VERSION                          *
***********************************************************************/
static void check_for_version( char * sorc )
{
static const char APEX_RESPONSE[]    = TEXT( "R:6#emc_" );
static const char CYCLONE_RESPONSE[] = TEXT( "R:6#cyc_" );  /* Cyclone II board returns this */
bool have_version;

have_version = false;
if ( findstring(APEX_RESPONSE, sorc) )
    {
    IsCyclone = false;
    have_version = true;
    }
else if ( findstring(CYCLONE_RESPONSE, sorc) )
    {
    IsCyclone = true;
    have_version = true;
    }

if ( have_version )
    do_have_new_version();
}

/***********************************************************************
*                      CHECK_FOR_START_OF_POINTS                       *
*                           P_BEGIN_12_7112                            *
***********************************************************************/
static void check_for_start_of_points( char * sorc )
{
static const char START_OF_POS_SAMPLES[]  = TEXT( "P_BEGIN_" );
static const char START_OF_TIME_SAMPLES[] = TEXT( "T_BEGIN_" );
static const char START_OF_PARAMETERS[]   = TEXT( "C_BEGIN_" );
static const int  SLEN = 8;
char * cp;
char * up;
char sample_type;

cp = findstring( START_OF_POS_SAMPLES, sorc );

if ( !cp )
    cp = findstring( START_OF_TIME_SAMPLES, sorc );

if ( !cp )
    cp = findstring( START_OF_PARAMETERS, sorc );

if ( cp )
    {
    *(cp+2) = NullChar;
    StartOfPrefix = cp;
    *(cp+2) = 'B';

    /*
    ------------------------------------
    Remember what kind of sample this is
    ------------------------------------ */
    sample_type = *cp;

    cp += SLEN;
    up = findstring( UnderscoreString, cp );
    if ( up )
        {
        *up = NullChar;
        StartOfPrefix += cp;
        StartOfPrefix += UnderscoreString;

        if ( sample_type == 'P' )
            {
            /*
            -----------------------------------------------------------
            This is starting a new shot, get the shot number and delete
            any data from the last shot
            ----------------------------------------------------------- */
            CurrentShotTime.get_local_time();
            CurrentShotNumber = asctoint32( cp );
            }

        *up = *UnderscoreString;
        }
    }
}

/**********************************************************************
*                     request_next_board_variable                     *
**********************************************************************/
static void request_next_board_variable()
{
static STRING_CLASS s;

if ( !SendingBoardVariableRequests )
    {
    NextBoardVariable = 0;
    SendingBoardVariableRequests = TRUE;
    }
else
    {
    NextBoardVariable++;
    }

if ( NextBoardVariable >= NOF_BOARD_VARIABLES )
    {
    SendingBoardVariableRequests = FALSE;
    return;
    }

s = VChar;
s += NextBoardVariable;
send_socket_message( s.text(), true );
}

/**********************************************************************
*                             save_variables                          *
**********************************************************************/
void save_variables( TCHAR * sorc )
{
TCHAR * cp;
TCHAR * ep;
int     i;
int     vn;

if ( *sorc != VChar )
    return;

ep = sorc;
ep++;
cp = ep;
for ( i = 0; i<4; i++ )
    {
    if ( *ep == UnderscoreChar )
        {
        *ep = NullChar;
        vn = asctoint32( cp );
        if ( vn>= 0 && vn<NOF_BOARD_VARIABLES )
            {
            *ep = UnderscoreChar;
            ep++;
            BoardVariable[vn] = asctoint32( ep );
            if ( SendingBoardVariableRequests )
                request_next_board_variable();
            }
        break;
        }
    ep++;
    }
}

/***********************************************************************
*                               DO_NEW_DATA                            *
***********************************************************************/
void do_new_data( LPARAM lParam )
{
char * cp;
int                      i;
NEW_FTII_DATA          * nd;
FTII_SAMPLE            * samples;
FTII_PARAMETERS        * parameters;
FTII_OP_STATUS_DATA    * op_status;
FTII_OSCILLOSCOPE_DATA * oscilloscope;

nd = (NEW_FTII_DATA *) lParam;
if ( nd )
    {
    /*
    ------------------------------------------------------------
    If this is not ascii data I am going to add many lines
    and need to turn off the refresh on the listbox before I do.
    ------------------------------------------------------------ */
    if ( nd->type != ASCII_TYPE  )
        MainListBox.redraw_off();

    switch ( nd->type )
        {
        case ASCII_TYPE:
            cp = (char *) nd->buf;
            check_for_start_of_points( cp );
            check_for_version( cp );
            i = nd->len;
            if ( i > 0 )
                {
                i--;
                if ( cp[i] == LfChar )
                    cp[i] = NullChar;
                if ( !Broadcasting || strings_are_equal(cp, "# N2", 4) )
                    MainListBox.add( cp );
                save_variables( cp );
                }
            delete[] cp;
            break;

        case SAMPLE_TYPE:
            samples = (FTII_SAMPLE *) nd->buf;
            nd->len /= sizeof( FTII_SAMPLE );
            for ( i=0; i<nd->len; i++ )
                post_sample_lines_from_binary( i+1, samples[i] );

            cp = StartOfPrefix.text();
            if ( *cp == 'P' )
                {
                ShotData.set_position_based_points( *nd );
                }
            else
                {
                ShotData.set_time_based_points( *nd );
                }

            samples = 0;
            break;

        case PARAMETERS_TYPE:
            MainListBox.redraw_off();

            parameters = (FTII_PARAMETERS *) nd->buf;
            post_parameter_lines_from_binary( *parameters );

            parameters = 0;
            ShotData.set_parameters( *nd );
            break;

        case OP_STATUS_TYPE:
            op_status = (FTII_OP_STATUS_DATA *) nd->buf;
            post_op_status_lines_from_binary( *op_status );

            delete op_status;
            op_status = 0;
            break;

        case OSCILLOSCOPE_TYPE:
            oscilloscope = (FTII_OSCILLOSCOPE_DATA *) nd->buf;
            post_oscilloscope_lines_from_binary( *oscilloscope );
            delete oscilloscope;
            oscilloscope = 0;
            break;
        }

    if ( nd->type != ASCII_TYPE  && nd->type != SOCKET_ERROR_TYPE )
        MainListBox.redraw_on();
    scroll_listbox_to_last_entry();

    delete nd;
    }
}

/***********************************************************************
*                           SIZE_MAIN_CONTROLS                         *
***********************************************************************/
static void size_main_controls()
{
RECTANGLE_CLASS r;
int y;

GetClientRect( MainWindow.handle(), &r );
y = r.height() - 20;
SetWindowPos( MainEbox, 0, 0, y, r.width(), 20, SWP_NOZORDER );

SetWindowPos( MainListBox.handle(), 0, 0, 0, r.width(), y, SWP_NOZORDER );
}

/***********************************************************************
*                             SAVE_LISTBOX                             *
***********************************************************************/
static void save_listbox()
{
FILE_CLASS f;
int i;
int n;

n = MainListBox.count();
if ( n > 0 )
    {
    f.open_for_write( "listbox.txt" );

    for ( i=0; i<n; i++ )
        f.writeline( MainListBox.item_text(i) );

    f.close();
    }
}

/***********************************************************************
*                         write_board_variables_to_csv                 *
***********************************************************************/
static void write_board_variables_to_csv()
{
union { unsigned long u; int i; };
FILE_CLASS f;
int j;
STRING_CLASS s;

u = 0;
s.init( 50 );

f.open_for_write( "board_variables.csv" );

for ( j=0; j<NOF_BOARD_VARIABLES; j++ )
    {
    s = j;
    s += CommaChar;
    i = BoardVariable[j];
    s += i;
    s += CommaChar;
    s += HChar;
    s += ultohex( u );
    f.writeline( s );
    }
f.close();
}



/***********************************************************************
*                             SAVE_PROFILE                             *
***********************************************************************/
static void save_profile()
{
FTII_FILE_DATA f;

f = ShotData;
f.put( ShotDataFile );
}

/***********************************************************************
*                         CREATE_MAIN_CONTROLS                         *
***********************************************************************/
static void create_main_controls()
{
RECTANGLE_CLASS r;
int id;
int y;

GetClientRect( MainWindow.handle(), &r );
y = r.height() - 20;

id = MAIN_EBOX;
MainEbox = CreateWindow( TEXT("EDIT"),
              NULL,
              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
              0, y, r.width(), 20,
              MainWindow.handle(),
              (HMENU) id,
              MainInstance,
              NULL );

OriEditControlProc = (WNDPROC) SetWindowLong( MainEbox, GWL_WNDPROC, (LONG) edit_control_proc );

id = MAIN_LISTBOX_ID;
MainListBox.init( CreateWindow(TEXT ("listbox"),
                  NULL,
                  WS_CHILD | WS_BORDER | WS_VSCROLL | WS_VISIBLE | LBS_NOSEL,
                  0, 0, r.width(), y,                         // x, y, w, h
                  MainWindow.handle(),
                  (HMENU) id,
                  MainInstance,
                  NULL)
                );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static TCHAR already_connected[] = TEXT( "Already Connected" );
static TCHAR connected[]         = TEXT( "Connected..." );
static TCHAR fail_to_connect[]   = TEXT( "Connect Failure" );

static bool I_am_active = true;
int id;
int cmd;
id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        set_window_title();
        create_main_controls();
        begin_monitoring( MainWindow );
        break;

    case WM_DB_SAVE_DONE:
        if ( FilesSentToCyclone > 0 )
            {
            FilesSentToCyclone--;
            if ( FilesSentToCyclone == 0 )
                {
                if ( IsCyclone )
                    {
                    if ( IDYES == MessageBox(hWnd, "Reboot the FasTrak2 Board?", "Upload Complete", MB_YESNO | MB_SYSTEMMODAL) )
                        send_socket_message( RebootCycloneCmd, true );
                    }
                else
                    {
                    DialogBox(
                        MainInstance,
                        TEXT( "NEED_REBOOT_DIALOG" ),
                        MainWindow.handle(),
                        (DLGPROC) need_reboot_dialog_proc
                         );
                    }
                }
            }
        return 0;

    case WM_NEW_DATA:
        do_new_data( lParam );
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case EXIT_CHOICE:
                MainWindow.close();
                return 0;

            case SET_ADDRESS_CHOICE:
                DialogBox(
                    MainInstance,
                    TEXT( "TCPIP_SETUP_DIALOG" ),
                    MainWindow.handle(),
                    (DLGPROC) tcpip_setup_dialog_proc
                     );
                return 0;

            case UPLOAD_FILE_CHOICE:
                DialogBox(
                    MainInstance,
                    TEXT( "UPLOAD_FILE_NAME_DIALOG" ),
                    MainWindow.handle(),
                    (DLGPROC) upload_file_name_dialog_proc
                     );
                return 0;

            case BROWSE_FOR_FLASH_DIR_CHOICE:
                get_flash_file_dir();
                return 0;

            case BROWSE_FOR_SINGLE_FILE_CHOICE:
                browse_for_single_file();
                return 0;
            case CLEAR_LISTBOX_CHOICE:
                MainListBox.empty();
                return 0;

            case WRITE_LISTBOX_CHOICE:
                save_listbox();
                return 0;

            case WRITE_PROFILE_CHOICE:
                save_profile();
                return 0;

            case WRITE_CSV_CHOICE:
                ShotData.write_csv( ShotCsvFile );
                return 0;

            case SAVE_BOARD_VARIABLES_CHOICE:
                write_board_variables_to_csv();
                return 0;

            case REQUEST_ALL_BOARD_VARIABLES_CHOICE:
                request_next_board_variable();
                return 0;
            }

        break;

    case WM_TIMER:
        BroadcastCount++;
        if ( BroadcastCount > 254 )
            {
            KillTimer( hWnd, TIMER_ID );
            Broadcasting = false;
            set_window_title();
            set_current_address( TcpIpAddress, TcpIpPort );
            set_text( MainEbox, "Finished" );
            }
        else
            {
            lstrcpy( BroadcastCp, int32toasc(BroadcastCount) );
            set_current_address( BroadcastAddress, TcpIpPort );
            send_socket_message( BroadcastMessage.text(), true );
            set_text( MainEbox, BroadcastAddress );
            }
        break;

    case WM_SETFOCUS:
        SetFocus( MainEbox );
        break;

    case WM_CLOSE:
        ShuttingDown = true;
        set_socket_send_event();
        break;

    case WM_SIZE:
        size_main_controls();
        break;

    case WM_DESTROY:
        KillTimer( hWnd, TIMER_ID );
        PostQuitMessage( 0 );
        return 0;

    case WM_ACTIVATE:
        cmd = HIWORD( wParam );
        if ( id == WA_INACTIVE || cmd != 0 )
           I_am_active = false;
        else
           I_am_active = true;
        break;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
STRING_CLASS title;
WNDCLASS wc;
int      i;

/*
------------------------------
Initialize the board variables
------------------------------ */
for ( i=0; i<NOF_BOARD_VARIABLES; i++ )
    BoardVariable[i] = 0;

load_tcp();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    0, 0,                             // X,y
    750, 550,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
stop_monitoring();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage(  &msg );
    }

shutdown();
return( msg.wParam );
}
