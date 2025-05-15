#include <windows.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\iniclass.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "resource.h"

TCHAR MyClassName[]   = TEXT( "TrueTrakCatcher" );
TCHAR MyWindowTitle[] = TEXT( "True-Trak 2020(tm) Socket Catcher" );

static HWND         MainDialogWindow = 0;
static HINSTANCE    MainInstance;

static WINDOW_CLASS ErrorCountTB;
static WINDOW_CLASS MainWindow;
static WINDOW_CLASS MessageCountTB;
static WINDOW_CLASS On_Off_PB;
#define TIMER_ID 1

/*
---------
Constants
--------- */
static const int  Bytes_Per_Tchar = (int) sizeof( TCHAR );
static const WORD SocketVersion   = MAKEWORD(2,0);

/*
---------
Ini Files
--------- */
TCHAR ConfigSection[]  = TEXT( "Config" );
TCHAR MyAddressKey[]   = TEXT( "MyAddress" );
TCHAR MyPortKey[]      = TEXT( "MyPort" );

/*
-------
Globals
------- */
static int          DisplayedErrorCount   = -1;
static int          DisplayedMessageCount = -1;
static int          ErrorCount            = 0;
static SOCKET       Listener              = INVALID_SOCKET;
static STRING_CLASS Message;
static int          MessageCount          = 0;
static STRING_CLASS MyAddress;
static int          MyPort                = 0;
static bool         ShuttingDown          = false;

/***********************************************************************
*                              RECV_THREAD                             *
***********************************************************************/
void recv_thread( void * notused )
{
const int   buflen  = 1024;

char               buf[buflen+1];
int                bytes_received;
struct sockaddr_in my_addr;
SOCKET             receiver = INVALID_SOCKET;
int                status;

Listener = socket( PF_INET, SOCK_STREAM, 0 );
if ( Listener == INVALID_SOCKET )
    return;

my_addr.sin_family      = PF_INET;
my_addr.sin_addr.s_addr = inet_addr( MyAddress.text() );
my_addr.sin_port        = htons( MyPort );

status = bind( Listener, (struct sockaddr FAR *)&my_addr, sizeof(my_addr) );
if ( status != SOCKET_ERROR )
    status = listen( Listener, SOMAXCONN );

if ( status == SOCKET_ERROR )
    {
    closesocket( Listener );
    Listener = INVALID_SOCKET;
    return;
    }

while ( true )
    {
    receiver = accept( Listener, NULL, NULL );
    if ( receiver == INVALID_SOCKET )
        {
        if ( Listener != INVALID_SOCKET )
            {
            closesocket( Listener );
            Listener = INVALID_SOCKET;
            }
        break;
        }

    while ( true )
        {
        bytes_received = recv( receiver, buf, buflen, 0 );
        if ( bytes_received == 0 || bytes_received == SOCKET_ERROR )
            {
            closesocket( receiver );
            receiver = INVALID_SOCKET;
            if ( bytes_received == 0 )
                MessageCount++;
            else
                ErrorCount++;
            break;
            }
        }
    }
}

/***********************************************************************
*                             GET_DEFAULTS                             *
***********************************************************************/
static void get_defaults()
{
INI_CLASS    ini;
STRING_CLASS s;

s = exe_directory();
s.cat_path( TEXT("catcher.ini") );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( ini.find(MyAddressKey) )
    {
    s = ini.get_string();
    s.set_text( MainDialogWindow, MY_ADDRESS_EB );
    }

if ( ini.find(MyPortKey) )
    {
    s = ini.get_string();
    s.set_text( MainDialogWindow, MY_PORT_EB );
    }
}

/***********************************************************************
*                             PUT_DEFAULTS                             *
***********************************************************************/
static void put_defaults()
{
INI_CLASS    ini;
STRING_CLASS s;

s = exe_directory();
s.cat_path( TEXT("catcher.ini") );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

s.get_text( MainDialogWindow, MY_ADDRESS_EB );
if ( !s.isempty() )
    ini.put_string( MyAddressKey, s.text() );

s.get_text( MainDialogWindow, MY_PORT_EB );
if ( !s.isempty() )
    ini.put_string( MyPortKey, s.text() );
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static STRING_CLASS s;

int    cmd;
HANDLE h;
int    id;
DWORD  thread_id;

cmd = HIWORD( wParam );
id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        MainWindow.shrinkwrap( hWnd );
        ErrorCountTB   = GetDlgItem( hWnd, ERROR_COUNT_TB   );
        MessageCountTB = GetDlgItem( hWnd, MESSAGE_COUNT_TB );
        On_Off_PB      = GetDlgItem( hWnd, ON_OFF_PB );
        get_defaults();
        SetTimer( hWnd, TIMER_ID, 1000, NULL );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        get_defaults();
        return TRUE;

    case WM_CLOSE:
        if ( Listener != INVALID_SOCKET )
            {
            closesocket( Listener );
            Listener = INVALID_SOCKET;
            }
        MainWindow.post( WM_CLOSE );
        return TRUE;

    case WM_DESTROY:
        put_defaults();
        KillTimer( hWnd, TIMER_ID );
        break;

    case WM_TIMER:
        if ( DisplayedErrorCount != ErrorCount )
            {
            s = ErrorCount;
            s.set_text( ErrorCountTB.handle() );
            DisplayedErrorCount = ErrorCount;
            }
        if ( DisplayedMessageCount != MessageCount )
            {
            s = MessageCount;
            s.set_text( MessageCountTB.handle() );
            DisplayedMessageCount = MessageCount;
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case ON_OFF_PB:
                if ( Listener == INVALID_SOCKET )
                    {
                    h = CreateThread(
                        NULL,                                     /* no security attributes        */
                        0,                                        /* use default stack size        */
                        ( LPTHREAD_START_ROUTINE )recv_thread,    /* thread function               */
                        ( void * ) NULL,                          /* argument to thread function   */
                        0,                                        /* use default creation flags    */
                        &thread_id );                             /* returns the tLhread identifier */
                    On_Off_PB.set_title( "Disconnect" );
                    }
                else
                    {
                    closesocket( Listener );
                    Listener = INVALID_SOCKET;
                    On_Off_PB.set_title( "Connect" );
                    }
                return TRUE;

            case CLEAR_COUNTS_PB:
                if ( cmd == BN_CLICKED )
                    {
                    MessageCount = 0;
                    ErrorCount   = 0;
                    }
                return TRUE;

            case MY_ADDRESS_EB:
                if ( cmd == EN_CHANGE )
                    MyAddress.get_text( hWnd, MY_ADDRESS_EB );
                return TRUE;

            case MY_PORT_EB:
                if ( cmd == EN_CHANGE )
                    {
                    s.get_text( hWnd, MY_PORT_EB );
                    MyPort = s.int_value();
                    }
                return TRUE;
            }
        break;
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

switch ( msg )
    {
    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 STARTUP                              *
***********************************************************************/
static bool startup()
{
int            status;
WNDCLASS       wc;
WSADATA        wsadata;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MyClassName );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle,
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    378, 490,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainWindow.handle() )
    return FALSE;

MainDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("MainDialog"),
    MainWindow.handle(),
    (DLGPROC) main_dialog_proc );

MainWindow.show();
UpdateWindow( MainWindow.handle() );

status = WSAStartup( SocketVersion, &wsadata );

return true;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
WSACleanup();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;

MainInstance = this_instance;

if ( !startup() )
    return FALSE;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
    }

shutdown();
return msg.wParam;
}
