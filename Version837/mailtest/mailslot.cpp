#include <windows.h>
#include <ddeml.h>
#include <commctrl.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\subs.h"
#include "..\include\iniclass.h"

#include "resource.h"

#define MINIMIZE_CHOICE     106
#define CLEAR_EVENTS_CHOICE 107
#define EXIT_CHOICE         108
#define WM_USER_INITIATE    WM_USER+0x101

HINSTANCE MainInstance;
HWND      MainWindow;

/*
---------------------------------------------------
Data source types for the add_new_computer function
--------------------------------------------------- */
HWND      MainDialogWindow = 0;
HWND      MessageListbox   = 0;
HWND      ReadPathEb       = 0;
HWND      SendPathEb       = 0;
HWND      SendMessageEb    = 0;
HWND      StatusTb         = 0;

static TCHAR MyClassName[] = TEXT("MailslotTest" );
static TCHAR MyIniFile[]   = TEXT("mailslot.ini");
static TCHAR ConfigSection[] = TEXT("Config" );
static TCHAR ReadPathKey[]    = TEXT("ReadPath");
static TCHAR SendPathKey[]    = TEXT("SendPath");
static TCHAR SendMessageKey[] = TEXT("SendMessage");
static TCHAR Mailslot_Closed[]= TEXT("Closed");
static TCHAR Mailslot_Open[]  = TEXT("Open");

STRING_CLASS MyWindowTitle;
static HANDLE  ReadHandle = INVALID_HANDLE_VALUE;

/***********************************************************************
*                             SHOW_MESSAGE                             *
***********************************************************************/
void show_message( const TCHAR * sorc )
{
if ( MessageListbox )
    SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM) sorc );
}

/***********************************************************************
*                           SIZE_MAIN_DIALOG                           *
***********************************************************************/
void size_main_dialog()
{
RECT  r;

GetClientRect( MainWindow, &r);

MoveWindow( MainDialogWindow, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                           CREATE_MAILSLOT                            *
***********************************************************************/
static void create_mailslot()
{
STRING_CLASS s;
unsigned int error_value;

if ( ReadHandle != INVALID_HANDLE_VALUE )
    {
    show_message( TEXT("The mailslot is already opened") );
    return;
    }

s.get_text( ReadPathEb );

ReadHandle = CreateMailslot(
        s.text(),
        0,                             // no maximum message size
        MAILSLOT_WAIT_FOREVER,         // read timeout (use only 0 or MAILSLOT_WAIT_FOREVER )
        (LPSECURITY_ATTRIBUTES) NULL); // no security attributes

if ( ReadHandle == INVALID_HANDLE_VALUE)
    {
    error_value = (unsigned int) GetLastError();
    set_text( StatusTb, Mailslot_Closed );
    show_message( TEXT("Unable to create mailbox.") );
    s = TEXT("Error = ");
    s += error_value;
    show_message( s.text() );
    return;
    }
else
    {
    show_message( TEXT("Mailslot Created Successfully") );
    set_text( StatusTb, Mailslot_Open );
    }
}

/***********************************************************************
*                            SEND_MAILSLOT                             *
***********************************************************************/
static void send_mailslot()
{
STRING_CLASS mailslot_name;
STRING_CLASS message_to_send;
STRING_CLASS s;

HANDLE       send_handle;
BOOL         was_sent;
int          slen;
DWORD        bytes_written;
unsigned int error_value;

mailslot_name.get_text( SendPathEb );
message_to_send.get_text( SendMessageEb );
slen = message_to_send.len() + 1;
send_handle = CreateFile(
    mailslot_name.text(),
    GENERIC_WRITE,
    FILE_SHARE_READ,              /* required to write to a mailslot */
    (LPSECURITY_ATTRIBUTES) NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    (HANDLE) NULL);

error_value = (unsigned int) GetLastError();

if ( send_handle != INVALID_HANDLE_VALUE)
    {
    was_sent = WriteFile( send_handle, message_to_send.text(), (DWORD) slen, &bytes_written, (LPOVERLAPPED) NULL );
    error_value = (unsigned int) GetLastError();
    CloseHandle( send_handle );
    if ( was_sent )
        show_message( TEXT("Message Sent") );
    else
        {
        show_message( TEXT("Unable to send the message") );
        s = TEXT("Error = ");
        s += error_value;
        show_message( s.text() );
        }
    }
else
    {
    show_message( TEXT("Could not open the destination file for writing.") );
    s = TEXT("Error = ");
    s += error_value;
    show_message( s.text() );
    }
}

/***********************************************************************
*                            READ_MAILSLOT                             *
***********************************************************************/
static void read_mailslot()
{
TCHAR      * cp;
int          slen;
DWORD        bytes_read;
DWORD        next_msg_size;
DWORD        nof_msg;
BOOL         status;
STRING_CLASS s;

if ( ReadHandle == INVALID_HANDLE_VALUE )
    return;

status = GetMailslotInfo(
    ReadHandle,
    (LPDWORD) NULL,               // current maximum message size
    &next_msg_size,               // size of next message
    &nof_msg,                     // number of messages qued up
    (LPDWORD) NULL );             // current read time-out seting

if ( !status )
    {
    show_message( "Unable to get Mailslot Info" );
    return;
    }

if ( next_msg_size == MAILSLOT_NO_MESSAGE )
    {
    show_message( "Nothing to read." );
    }
else if ( next_msg_size > 0 )
    {
    slen = (int) next_msg_size;
    s = TEXT( "The message has " );
    s += slen;
    s += TEXT( " characters" );
    show_message( s.text() );
    cp = maketext( slen );

    status     = FALSE;
    bytes_read = 0;
    if ( cp )
        {
        status = ReadFile( ReadHandle, cp, next_msg_size, &bytes_read, NULL );
        if ( !status )
            {
            show_message( "Unable to read from the mailslot." );
            delete[] cp;
            return;
            }

        *(cp+slen) = NullChar;

        s = TEXT("Message = [");
        s += cp;
        s += "]";
        show_message( s.text() );
        }
    }
}

/***********************************************************************
*                         LOAD_DEFAULT_STRINGS                         *
***********************************************************************/
static void load_default_strings()
{
INI_CLASS  ini;
NAME_CLASS s;

s = exe_directory();
s.cat_path( MyIniFile );

ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.exists() )
    {
    if ( ini.find(ReadPathKey) )
        {
        s = ini.get_string();
        s.set_text( ReadPathEb );
        }
    if ( ini.find(SendPathKey) )
        {
        s = ini.get_string();
        s.set_text( SendPathEb );
        }
    if ( ini.find(SendMessageKey) )
        {
        s = ini.get_string();
        s.set_text( SendMessageEb );
        }
    }
}

/***********************************************************************
*                         SAVE_DEFAULT_STRINGS                         *
***********************************************************************/
static void save_default_strings()
{
INI_CLASS  ini;
NAME_CLASS s;

s = exe_directory();
s.cat_path( MyIniFile );

ini.set_file( s.text() );
ini.set_section( ConfigSection );

s.get_text( ReadPathEb );
ini.put_string( ReadPathKey, s.text() );

s.get_text( SendPathEb );
ini.put_string( SendPathKey, s.text() );

s.get_text( SendMessageEb );
ini.put_string( SendMessageKey, s.text() );
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        MessageListbox = GetDlgItem( hWnd, MESSAGE_LB );
        ReadPathEb     = GetDlgItem( hWnd, SORC_PATH_EB );
        SendPathEb     = GetDlgItem( hWnd, DEST_PATH_EB );
        SendMessageEb  = GetDlgItem( hWnd, DEST_MSG_EB );
        StatusTb       = GetDlgItem( hWnd, STATUS_TB );
        set_text( StatusTb, Mailslot_Closed );
        load_default_strings();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case OPEN_BUTTON:
                create_mailslot();
                return TRUE;

            case READ_BUTTON:
                read_mailslot();
                return TRUE;

            case SEND_BUTTON:
                send_mailslot();
                return TRUE;

            case CLOSE_BUTTON:
                if ( ReadHandle != INVALID_HANDLE_VALUE )
                    {
                    set_text( StatusTb, Mailslot_Closed );
                    CloseHandle( ReadHandle );
                    ReadHandle = INVALID_HANDLE_VALUE;
                    }
                return TRUE;
            }
    }

return FALSE;
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
    case WM_SIZE:
        size_main_dialog();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CLEAR_EVENTS_CHOICE:
                SendMessage( MessageListbox, LB_RESETCONTENT, 0, 0L );
                return 0;

            case MINIMIZE_CHOICE:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_CHOICE:
                PostMessage( MainWindow, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_CLOSE:
        save_default_strings();
        if ( ReadHandle != INVALID_HANDLE_VALUE )
            {
            CloseHandle( ReadHandle );
            ReadHandle = INVALID_HANDLE_VALUE;
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
BOOL init()
{
WNDCLASS wc;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MyClassName );
wc.lpszMenuName  = TEXT("MainMenu");
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    800, 560,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainWindow )
    return FALSE;

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
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;
BOOL status;

MainInstance = this_instance;

MyWindowTitle = "Mailslot Test Program";

if ( is_previous_instance(MyClassName, 0) )
    return 0;

InitCommonControls();

if ( !init() )
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

return msg.wParam;
}
