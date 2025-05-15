#include <windows.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\fifo.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\subs.h"

#include "resource.h"

TCHAR MyClassName[]   = TEXT( "TrueTrakDataServer" );
TCHAR MyWindowTitle[] = TEXT( "True-Trak 2020(tm) Socket Data Server" );

static HWND          MainDialogWindow = 0;
static HINSTANCE     MainInstance;
static HWND          MainWindow = 0;
static LISTBOX_CLASS MessageLB;
static HWND          MyAddressEB;
static HWND          MyPortEB;
/*
----------
Data Types
---------- */
static const int DS_BINARY = 1;
static const int DS_TCHAR  = 2;

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
TCHAR ConfigSection[]     = TEXT( "Config" );
TCHAR MyAddressKey[]      = TEXT( "MyAddress" );
TCHAR MyPortKey[]         = TEXT( "MyPort" );
TCHAR DataServerIniFile[] = TEXT( "dataserver.ini" );

/*
-------
Globals
------- */
static FIFO_CLASS   Data;
static SOCKET       Listener         = INVALID_SOCKET;
static STRING_CLASS MyAddress;
static int          MyPort           = 0;
static HANDLE       NewDataEvent     = 0;

static bool   NewDataThreadIsRunning     = false;
static bool   ShuttingDown               = false;

/*
-----------------
CLASS Definitions
----------------- */
class RECEIVED_MESSAGE_CLASS
    {
    public:
    sockaddr address;
    int      address_len;
    int      buflen;
    char   * buf;

    RECEIVED_MESSAGE_CLASS();
    RECEIVED_MESSAGE_CLASS( int new_len );
    ~RECEIVED_MESSAGE_CLASS();
    bool upsize( int new_len );
    };

/***********************************************************************
*                          RECEIVED_MESSAGE_CLASS                      *
***********************************************************************/
RECEIVED_MESSAGE_CLASS::RECEIVED_MESSAGE_CLASS()
{
address_len = 0;
buf         = 0;
buflen      = 0;
}

/***********************************************************************
*                          RECEIVED_MESSAGE_CLASS                      *
***********************************************************************/
RECEIVED_MESSAGE_CLASS::RECEIVED_MESSAGE_CLASS( int new_len )
{
address_len = 0;
buf         = 0;
buflen      = 0;
upsize( new_len );
}

/***********************************************************************
*                         ~RECEIVED_MESSAGE_CLASS                      *
***********************************************************************/
RECEIVED_MESSAGE_CLASS::~RECEIVED_MESSAGE_CLASS()
{
if ( buf )
    delete[] buf;
buf    = 0;
buflen = 0;
}

/***********************************************************************
*                          RECEIVED_MESSAGE_CLASS                      *
*                                UPSIZE                                *
***********************************************************************/
bool RECEIVED_MESSAGE_CLASS::upsize( int new_len )
{
char * dest;
int    n;
char * np;
char * sorc;

if ( new_len <= buflen )
    return true;

np = new char[new_len+2];
if ( !np )
    return false;

if ( buflen > 0 )
    {
    sorc = buf;
    dest = np;
    n    = buflen+2;
    while ( n )
        {
        *dest++ = *sorc++;
        n--;
        }

    delete[] buf;
    }

buflen = new_len;
buf    = np;

/*
-----------------------------
Put a UNICODE null at the end
----------------------------- */
dest    = buf + buflen;
*dest++ = 0;
*dest   = 0;

return true;
}

/***********************************************************************
*                             GET_DEFAULTS                             *
***********************************************************************/
static void get_defaults()
{
INI_CLASS  ini;
NAME_CLASS s;

s.get_exe_dir_file_name( DataServerIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( ini.find(MyAddressKey) )
    {
    MyAddress = ini.get_string();
    MyAddress.set_text( MyAddressEB );
    }

if ( ini.find(MyPortKey) )
    {
    s = ini.get_string();
    s.set_text( MyPortEB );
    }
}

/***********************************************************************
*                             PUT_DEFAULTS                             *
***********************************************************************/
static void put_defaults()
{
INI_CLASS  ini;
NAME_CLASS s;

s.get_exe_dir_file_name( DataServerIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

s.get_text( MyAddressEB );
if ( !s.isempty() )
    ini.put_string( MyAddressKey, s.text() );

s.get_text( MyPortEB );
if ( !s.isempty() )
    ini.put_string( MyPortKey, s.text() );
}

/***********************************************************************
*                            GET_NEXT_FIELD                            *
***********************************************************************/
static bool get_next_field( STRING_CLASS & dest, TCHAR * bp )
{
TCHAR * cp;

cp = bp;
while ( *cp != CommaChar && *cp != NullChar )
    cp++;

if ( bp == cp )
    {
    dest.null();
    return false;
    }

*cp  = NullChar;
dest = bp;
bp   = cp;
bp++;

return true;
}

/***********************************************************************
*                             DO_NEW_DATA                              *
***********************************************************************/
static void do_new_data( RECEIVED_MESSAGE_CLASS * rmp )
{
static TCHAR GET_FILE_REQUEST[] = TEXT( "get_file" );

STRING_CLASS command;
STRING_CLASS file_name;
STRING_CLASS length;
STRING_CLASS msg_id;
NAME_CLASS   s;

TCHAR            * bp;
char             * dest;
FILE_CLASS         f;
int                file_size;
int                n;
u_short            port;
int                previous_length_len;
SOCKET             send_socket = INVALID_SOCKET;
struct sockaddr_in * server_addr;
int                status;

bp = (TCHAR *) rmp->buf;
get_next_field( length,    bp );
get_next_field( msg_id,    bp );
get_next_field( command,   bp );
get_next_field( file_name, bp );

if ( command == GET_FILE_REQUEST )
    {
    s.get_data_dir_file_name( file_name.text() );
    if ( s.file_exists() )
        {
        file_size = s.file_size();
        if ( file_size > 0 )
            {
            n = file_size;
            n += msg_id.len() * Bytes_Per_Tchar;
            n += 2 * Bytes_Per_Tchar; //commas
            length = n;
            previous_length_len = length.len();
            n += previous_length_len * Bytes_Per_Tchar;
            length = n;
            if ( length.len() > previous_length_len )
                n += Bytes_Per_Tchar;
            dest = new char[n];
            if ( dest )
                {
                bp    = (TCHAR *) dest;
                bp    = copystring( bp, length.text() );
                *bp++ = CommaChar;
                bp    = copystring( bp, msg_id.text() );
                *bp++ = CommaChar;
                if ( f.open_for_read(s) )
                    {
                    f.readbinary( (void *)bp, file_size );
                    f.close();
                    }
                else
                    {
                    memset( (void *) bp, 0, (size_t) file_size );
                    }
                send_socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
                server_addr = (sockaddr_in *) &rmp->address;

                /*
                ----------------------------------------------------------------
                The port that I send to is one more than the one I received from
                ---------------------------------------------------------------- */
                port = ntohs( server_addr->sin_port );
                port++;
                server_addr->sin_port = htons( port );
                status = connect( send_socket, (struct sockaddr FAR *) &rmp->address, rmp->address_len );
                if ( status != SOCKET_ERROR )
                    {
                    send( send_socket, dest, n, 0 );
                    closesocket( send_socket );
                    send_socket = INVALID_SOCKET;
                    }
                }
            }
        }
    }

delete rmp;
rmp = 0;
}

/***********************************************************************
*                           NEW_DATA_THREAD                            *
***********************************************************************/
void new_data_thread( void * notused )
{
const DWORD wait_ms = 2000;

STRING_CLASS s;
RECEIVED_MESSAGE_CLASS * rmp;

NewDataThreadIsRunning = true;

while ( true )
    {
    if ( WaitForSingleObject(NewDataEvent, wait_ms) == WAIT_FAILED )
        ShuttingDown = true;
    ResetEvent( NewDataEvent );

    while ( Data.count() )
        {
        rmp = (RECEIVED_MESSAGE_CLASS *) Data.pop();
        do_new_data( rmp );
        }

    if ( ShuttingDown )
        break;
    }

while ( Data.count() )
    {
    rmp = (RECEIVED_MESSAGE_CLASS *) Data.pop();
    delete rmp;
    }

if ( NewDataEvent )
    {
    CloseHandle( NewDataEvent );
    NewDataEvent = 0;
    }

NewDataThreadIsRunning = false;
}

/***********************************************************************
*                             LISTEN_THREAD                            *
***********************************************************************/
void listen_thread( void * unused )
{
const int                DEFAULT_BUFLEN = 1024;

sockaddr_in              a;
char                   * bp;
char                     buf[TCP_ADDRESS_LEN+1];
int                      bytes_left_in_buffer;
int                      total_bytes_received;
int                      bytes_received;
TCHAR                  * cp;
TCHAR                  * dest;
TCHAR                  * ep;
int                      message_size;
int                      n;
SOCKET                   receiver       = INVALID_SOCKET;
RECEIVED_MESSAGE_CLASS * rmp            = 0;
int                      status;

Listener = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
if ( Listener == INVALID_SOCKET )
    return;

a.sin_family = AF_INET;
a.sin_port   = htons( MyPort );

unicode_to_char( buf, MyAddress.text() );
a.sin_addr.S_un.S_addr = inet_addr( buf );

status = bind( Listener, (sockaddr*)&a, sizeof(a) );
if ( status != SOCKET_ERROR )
    status = listen( Listener, SOMAXCONN );

if ( status != SOCKET_ERROR )
    {
    while ( true )
        {
        receiver = accept( Listener, &rmp->address, &rmp->address_len );
        if ( receiver == INVALID_SOCKET )
            break;

        rmp = new RECEIVED_MESSAGE_CLASS( DEFAULT_BUFLEN );

        dest                 = rmp->buf;
        message_size         = 0;
        total_bytes_received = 0;
        bytes_left_in_buffer = rmp->buflen;
        while ( true )
            {
            bytes_received = recv( receiver, dest, bytes_left_in_buffer, 0 );
            if ( bytes_received == 0 || bytes_received == SOCKET_ERROR || (message_size > 0 && bytes_received == bytes_left_in_buffer) )
                {
                closesocket( receiver );
                receiver = INVALID_SOCKET;
                break;
                }
            dest += bytes_received;
            total_bytes_received += bytes_received;
            bytes_left_in_buffer -= bytes_received;
            if ( message_size = 0 )
                {
                cp = (TCHAR *) rmp->buf;
                n  = rmp->buflen / Bytes_Per_Tchar;
                ep = findchar( CommaChar, cp, n );
                if ( ep )
                    {
                    *ep = NullChar;
                    message_size = asctoint( cp );
                    *ep = CommaChar;
                    if ( message_size > rmp->buflen )
                        {
                        rmp->upsize( message_size );
                        bytes_left_in_buffer = message_size - total_bytes_received;
                        }
                    }
                }
            }

        if ( total_bytes_received == message_size )
            {
            /*
            -------------------------------------------------------------
            Null the end of the message. There are two extra bytes at the
            end just for this purpose.
            ------------------------------------------------------------- */
            bp = rmp->buf + message_size;
            *bp++ = 0;
            *bp   = 0;
            Data.push( (void *) rmp );
            SetEvent( NewDataEvent );
            }
        else
            {
            delete rmp;
            }

        rmp = 0;
        }
    }

if ( Listener != INVALID_SOCKET )
    {
    closesocket( Listener );
    Listener = INVALID_SOCKET;
    }
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
        MessageLB.init( hWnd, MAIN_LISTBOX );
        MyAddressEB = GetDlgItem( hWnd, MY_ADDRESS_EB );
        MyPortEB    = GetDlgItem( hWnd, MY_PORT_EB    );
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
            if ( NewDataThreadIsRunning )
                return 0;
            break;
            }

        ShuttingDown = true;
        if ( Listener != INVALID_SOCKET )
            {
            closesocket( Listener );
            Listener = INVALID_SOCKET;
            }

        SetEvent( NewDataEvent     );
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

NewDataEvent     = CreateEvent( NULL, FALSE, FALSE, NULL );

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
COMPUTER_CLASS c;
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
