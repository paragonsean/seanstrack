#include <windows.h>
#include <process.h>

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

static TCHAR MyClassName[]      = TEXT( "PipeTest" );
static TCHAR MyIniFile[]        = TEXT( "pipetest.ini" );
static TCHAR ConfigSection[]    = TEXT( "Config" );
static TCHAR LocalPipePrefix[]  = TEXT( "\\\\.\\pipe\\" );
static TCHAR ReadPathKey[]      = TEXT( "ReadPath" );
static TCHAR SendPathKey[]      = TEXT( "SendPath" );
static TCHAR SendMessageKey[]   = TEXT( "SendMessage" );
static TCHAR PipeTest_Closed[]  = TEXT( "Closed" );
static TCHAR PipeTest_Open[]    = TEXT( "Open" );

static STRING_CLASS ClosePipeMessage( TEXT("Close Pipe") );
static STRING_CLASS LocalPipeName;
STRING_CLASS        MyWindowTitle;
static HANDLE       ReadHandle = INVALID_HANDLE_VALUE;

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
*                          SHOW_READ_MESSAGE                           *
***********************************************************************/
static void show_read_message( STRING_CLASS & sorc )
{
STRING_CLASS s;
s = TEXT("Message = [");
s += sorc;
s += "]";
show_message( s.text() );
}

/***********************************************************************
*                           PIPE_SERVER_THREAD                         *
***********************************************************************/
static void pipe_server_thread( void * notused )
{
const DWORD BYTES_TO_READ = 2048;
DWORD        bytes_read;
TCHAR      * cp;
unsigned int error_value;
STRING_CLASS s;

if ( ReadHandle != INVALID_HANDLE_VALUE )
    {
    show_message( TEXT("The pipe is already opened") );
    return;
    }

s.get_text( ReadPathEb );
LocalPipeName = LocalPipePrefix;
LocalPipeName += s.findlast( BackSlashChar );

ReadHandle = CreateNamedPipe(
        s.text(),
        PIPE_ACCESS_INBOUND,           // dwOpenMode
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, // dwPipeMode
        1,                             // nMaxInstances
        0,                             // nOutBufferSize,
        2048,                          // nInBufferSize,
        0,                             // nDefaultTimeOut,
        (LPSECURITY_ATTRIBUTES) NULL); // no security attributes

if ( ReadHandle == INVALID_HANDLE_VALUE)
    {
    error_value = (unsigned int) GetLastError();
    set_text( StatusTb, PipeTest_Closed );
    show_message( TEXT("Unable to create pipe.") );
    s = TEXT("Error = ");
    s += error_value;
    show_message( s.text() );
    return;
    }

show_message( TEXT("Pipe Created Successfully") );
set_text( StatusTb, PipeTest_Open );

s.upsize( BYTES_TO_READ );

while ( true )
    {
    if ( ConnectNamedPipe(ReadHandle, NULL) )
        {
        s.null();
        bytes_read = 0;
        if ( ReadFile(ReadHandle, s.text(), BYTES_TO_READ, &bytes_read, 0) )
            {
            cp = s.text() + bytes_read;
            *cp = NullChar;
            }
        DisconnectNamedPipe( ReadHandle );
        if ( !s.isempty() )
            {
            show_read_message( s );
            if ( s == ClosePipeMessage )
                break;
            }
        }
    }
CloseHandle( ReadHandle );
ReadHandle = INVALID_HANDLE_VALUE;
}

/***********************************************************************
*                      START_PIPE_SERVER_THREAD                        *
***********************************************************************/
void start_pipe_server_thread()
{
unsigned long h;

h = _beginthread( pipe_server_thread, 0, NULL );
if ( !h )
    show_message( TEXT("Unable to create Pipe Server Thread") );
}

/***********************************************************************
*                            SEND_PIPE_MESSAGE                         *
***********************************************************************/
static void send_pipe_message( STRING_CLASS & pipename, STRING_CLASS & message_to_send )
{
DWORD        bytes_written;
unsigned int error_value;
STRING_CLASS s;
HANDLE       send_handle;
int          slen;
BOOL         was_sent;

s = TEXT( "send_pipe_message: pipe= [" );
s += pipename;
s += TEXT( "] message = [" );
s += message_to_send;
s += TEXT( "]" );
show_message( s.text() );

slen = message_to_send.len() + 1;
send_handle = CreateFile( pipename.text(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
if ( send_handle == INVALID_HANDLE_VALUE)
    {
    error_value = (unsigned int) GetLastError();
    show_message( TEXT("Could not open the destination file for writing.") );
    s = TEXT("Error = ");
    s += error_value;
    show_message( s.text() );
    return;
    }

was_sent = WriteFile( send_handle, message_to_send.text(), (DWORD) slen, &bytes_written, (LPOVERLAPPED) NULL );
error_value = (unsigned int) GetLastError();
CloseHandle( send_handle );
if ( was_sent )
    {
    show_message( TEXT("Message Sent") );
    }
else
    {
    show_message( TEXT("Unable to send the message") );
    s = TEXT("Error = ");
    s += error_value;
    show_message( s.text() );
    }
}

/***********************************************************************
*                           DO_CLOSE_BUTTON                            *
***********************************************************************/
void do_close_button()
{
if ( ReadHandle != INVALID_HANDLE_VALUE )
    send_pipe_message( LocalPipeName, ClosePipeMessage  );
set_text( StatusTb, PipeTest_Closed );
}

/***********************************************************************
*                            DO_SEND_BUTTON                            *
***********************************************************************/
void do_send_button()
{
STRING_CLASS message_to_send;
STRING_CLASS pipename;

pipename.get_text( SendPathEb );
message_to_send.get_text( SendMessageEb );
send_pipe_message( pipename, message_to_send );
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
        set_text( StatusTb, PipeTest_Closed );
        load_default_strings();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case OPEN_BUTTON:
                start_pipe_server_thread();
                return TRUE;

            case SEND_BUTTON:
                do_send_button();
                return TRUE;

            case CLOSE_BUTTON:
                do_close_button();
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
        do_close_button();
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

MyWindowTitle = "Pipe Test Program";

if ( is_previous_instance(MyClassName, 0) )
    return 0;

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
