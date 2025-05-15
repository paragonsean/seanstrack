#include <windows.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\fifo.h"
#include "..\include\subs.h"

#include "resource.h"
#include "extern.h"

TCHAR MyClassName[]   = TEXT( "TrueTrakDataClient" );
TCHAR MyWindowTitle[] = TEXT( "Socket Data Test Client" );

static HWND MainDialogWindow = 0;
static HWND MessageListbox   = 0;

/*
----------
Data Types
---------- */
static const int DS_TCHAR = 1;

/*
---------
Constants
--------- */
static const int  Bytes_Per_Tchar = (int) sizeof( TCHAR );
static const WORD SocketVersion   = MAKEWORD(2,0);

/*
-------
Globals
------- */
static FIFO_CLASS Clients;
static FIFO_CLASS Data;
static FIFO_CLASS Responses;

static HANDLE NewClientEvent   = 0;
static HANDLE NewDataEvent     = 0;
static HANDLE NewResponseEvent = 0;

static bool   ListenThreadIsRunning      = false;
static bool   NewDataThreadIsRunning     = false;
static bool   NewResponseThreadIsRunning = false;
static bool   ReadThreadIsRunning        = false;
static bool   ShuttingDown               = false;

/*
-------------------------------------------------------------------
The listener socket must be global so I can close it from main_proc
------------------------------------------------------------------- */
SOCKET Listener = INVALID_SOCKET;

/*
-----------------
CLASS Definitions
----------------- */
class RECEIVED_SOCKET_CLASS
    {
    public:
    sockaddr address;
    int      address_len;
    SOCKET   client;
    void   * data;
    int      data_byte_len;
    int      data_type;
    RECEIVED_SOCKET_CLASS();
    ~RECEIVED_SOCKET_CLASS();
    void kill_data();
    };

/***********************************************************************
*             RECEIVED_SOCKET_CLASS::RECEIVED_SOCKET_CLASS             *
***********************************************************************/
RECEIVED_SOCKET_CLASS::RECEIVED_SOCKET_CLASS()
{
address_len   = 0;
client        = INVALID_SOCKET;
data          = 0;
data_byte_len = 0;
data_type     = DS_TCHAR;
}

/***********************************************************************
*            ~RECEIVED_SOCKET_CLASS::RECEIVED_SOCKET_CLASS             *
***********************************************************************/
RECEIVED_SOCKET_CLASS::~RECEIVED_SOCKET_CLASS()
{
if ( client != INVALID_SOCKET )
    {
    closesocket( client );
    client = INVALID_SOCKET;
    }

kill_data();
}

/***********************************************************************
*                   RECEIVED_SOCKET_CLASS::KILL_DATA                   *
***********************************************************************/
void RECEIVED_SOCKET_CLASS::kill_data()
{
TCHAR * ds_tchar_copy;

if ( data )
    {
    if ( data_type == DS_TCHAR )
        {
        ds_tchar_copy = (TCHAR *) data;
        data = 0;
        delete[] ds_tchar_copy;
        ds_tchar_copy = 0;
        }
    }
}

/***********************************************************************
*                         NEW_RESPONSE_THREAD                          *
***********************************************************************/
void new_response_thread( void * notused )
{
const DWORD wait_ms = 2000;

int                     status;
RECEIVED_SOCKET_CLASS * rsp;

NewResponseThreadIsRunning = true;
while ( true )
    {
    if ( WaitForSingleObject(NewResponseEvent, wait_ms) == WAIT_FAILED )
        ShuttingDown = true;
    ResetEvent( NewResponseEvent );

    while ( Responses.count() )
        {
        rsp = (RECEIVED_SOCKET_CLASS *) Responses.pop();
        rsp->client = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        if ( rsp->client != INVALID_SOCKET )
            {
            status = connect( rsp->client, &rsp->address, rsp->address_len );
            if ( status != SOCKET_ERROR )
                {
                status = send( rsp->client, (char *)rsp->data, rsp->data_byte_len, 0 );
                }
            closesocket( rsp->client );
            rsp->client = INVALID_SOCKET;
            }
        delete rsp;
        rsp = 0;
        }

    if ( ShuttingDown )
        break;
    }

if ( NewResponseEvent )
    {
    CloseHandle( NewResponseEvent );
    NewResponseEvent = 0;
    }

while ( Responses.count() )
    {
    rsp = (RECEIVED_SOCKET_CLASS *) Responses.pop();
    rsp->client = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( rsp->client != INVALID_SOCKET )
        {
        closesocket( rsp->client );
        rsp->client = INVALID_SOCKET;
        }
    delete rsp;
    rsp = 0;
    }
NewResponseThreadIsRunning = false;

}

/***********************************************************************
*                             DO_NEW_DATA                              *
***********************************************************************/
static void do_new_data( RECEIVED_SOCKET_CLASS * rsp )
{
static TCHAR REQUEST_1[] = TEXT( "GET_DATA_1" );

STRING_CLASS s;

s.acquire( (TCHAR *) rsp->data );
rsp->data = 0;

if ( s == REQUEST_1 )
    {
    s = "HI YOU GUYS 1";
    rsp->data = (void *) s.release();
    Responses.push( rsp );
    SetEvent( NewResponseEvent );
    }
else
    {
    delete rsp;
    }

rsp = 0;
}

/***********************************************************************
*                           NEW_DATA_THREAD                            *
***********************************************************************/
void new_data_thread( void * notused )
{
const DWORD wait_ms = 2000;

STRING_CLASS s;
RECEIVED_SOCKET_CLASS * rsp;

while ( true )
    {
    if ( WaitForSingleObject(NewDataEvent, wait_ms) == WAIT_FAILED )
        ShuttingDown = true;
    ResetEvent( NewDataEvent );

    while ( Data.count() )
        {
        rsp = (RECEIVED_SOCKET_CLASS *) Data.pop();
        do_new_data( rsp );
        }

    if ( ShuttingDown )
        break;
    }

while ( Data.count() )
    {
    rsp = (RECEIVED_SOCKET_CLASS *) Data.pop();
    delete rsp;
    }

if ( NewDataEvent )
    {
    CloseHandle( NewDataEvent );
    NewDataEvent = 0;
    }

NewDataThreadIsRunning = false;
}

/***********************************************************************
*                               READ_THREAD                            *
***********************************************************************/
void read_thread( void * notused )
{
const int   buflen          = 1024;
const DWORD wait_ms         = 2000;

int                     buf_byte_len;
int                     bytes_received;
SOCKET                * cp;
int                     n;
RECEIVED_SOCKET_CLASS * rsp;
STRING_CLASS            s;
TCHAR                   tbuf[buflen+1];

buf_byte_len = buflen * Bytes_Per_Tchar;

while ( true )
    {
    if ( WaitForSingleObject(NewClientEvent, wait_ms) == WAIT_FAILED )
        ShuttingDown = true;
    ResetEvent( NewClientEvent );

    while ( Clients.count() )
        {
        s.null();
        rsp = (RECEIVED_SOCKET_CLASS *) Clients.pop();

        while ( true )
            {
            bytes_received = recv( rsp->client, (char *) tbuf, buf_byte_len, 0 );
            if ( bytes_received > 0 )
                {
                n = bytes_received;
                n /= Bytes_Per_Tchar;;
                tbuf[n+1] = NullChar;
                s += tbuf;
                }
            else
                {
                break;
                }
            }
        /*
        -----------------------------------------------------------------------------------------------------
        I have received the data request and then a null message indicating that the client socket has closed
        ----------------------------------------------------------------------------------------------------- */
        closesocket( rsp->client );
        rsp->client = INVALID_SOCKET;

        if ( s.len() )
            {
            rsp->data_type     = DS_TCHAR;
            rsp->data_byte_len = s.len() * Bytes_Per_Tchar;
            rsp->data          = (void *) s.release();
            Data.push( (void *) rsp );
            SetEvent( NewDataEvent );
            }
        }

    if ( ShuttingDown )
        break;
    }

while ( Clients.count() )
    {
    cp = (SOCKET *) Clients.pop();
    delete cp;
    }

if ( NewClientEvent )
    {
    CloseHandle( NewClientEvent );
    NewClientEvent = 0;
    }

ReadThreadIsRunning = false;
}

/***********************************************************************
*                             LISTEN_THREAD                            *
***********************************************************************/
void listen_thread( void * unused )
{
static TCHAR ip_addr[] = TEXT( "127.0.0.1" );
static u_short ip_port = 27099;

sockaddr_in             a;
char                    cbuf[TCP_ADDRESS_LEN + 1];
int                     result;
RECEIVED_SOCKET_CLASS * rsp = 0;

Listener = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
if ( Listener == INVALID_SOCKET )
    return;

a.sin_family = AF_INET;
a.sin_port   = htons( ip_port );

unicode_to_char( cbuf, ip_addr );
a.sin_addr.S_un.S_addr = inet_addr( cbuf );

result = bind( Listener, (sockaddr*)&a, sizeof(a) );
if ( result == SOCKET_ERROR )
    {
    closesocket( Listener );
    return;
    }

while ( true )
    {
    rsp = new RECEIVED_SOCKET_CLASS;
    rsp->client = accept( Listener, &rsp->address, &rsp->address_len );
    if ( rsp->client != INVALID_SOCKET )
        Clients.push( (void *) rsp );
    else
        delete rsp;
    rsp = 0;

    if ( ShuttingDown )
        break;
    }

if ( Listener != INVALID_SOCKET )
    {
    closesocket( Listener );
    Listener = INVALID_SOCKET;
    }

ListenThreadIsRunning = false;
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        MessageListbox = GetDlgItem( hWnd, MAIN_LISTBOX );
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

switch (msg)
    {
    case WM_CLOSE:
        if ( ShuttingDown )
            {
            if ( ListenThreadIsRunning || NewDataThreadIsRunning || NewResponseThreadIsRunning || ReadThreadIsRunning )
                return 0;
            break;
            }
        ShuttingDown = true;
        if ( Listener != INVALID_SOCKET )
            {
            closesocket( Listener );
            Listener = INVALID_SOCKET;
            }
        SetEvent( NewClientEvent   );
        SetEvent( NewDataEvent     );
        SetEvent( NewResponseEvent );
        return 0;

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
COMPUTER_CLASS c;
DWORD          id;
HANDLE         h;
int            status;
WNDCLASS       wc;
WSADATA        wsadata;

//MENU_CONTROL_ENTRY::register_window();
c.startup();

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
    279, 244,                         // W,h
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

status = WSAStartup( SocketVersion, &wsadata );

NewClientEvent   = CreateEvent( NULL, FALSE, FALSE, NULL );
NewDataEvent     = CreateEvent( NULL, FALSE, FALSE, NULL );
NewResponseEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

h = CreateThread(
    NULL,                                       /* no security attributes        */
    0,                                          /* use default stack size        */
    ( LPTHREAD_START_ROUTINE )listen_thread,    /* thread function               */
    ( void * ) NULL,                            /* argument to thread function   */
    0,                                          /* use default creation flags    */
    &id );                                       /* returns the tLhread identifier */

return true;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
c.shutdown();
WSACleanup();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;
BOOL status;

MainInstance = this_instance;

if ( is_previous_instance(MyClassName, NULL) )
    return 0;

if ( !startup() )
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
