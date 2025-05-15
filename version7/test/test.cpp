#include <windows.h>
#include <commctrl.h>
#include <winnetwk.h>   /* link to mpr.lib */
#include <shlobj.h>
#include <lm.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\rectclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"
#include "resrc1.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MyTbox;
LISTBOX_CLASS MainListbox;

static TCHAR MyClassName[]            = "Test";

const TCHAR BackSlashChar     = TEXT( '\\' );
const TCHAR BackSlashString[] = TEXT( "\\" );
const TCHAR CommaChar         = TEXT( ',' );
const TCHAR ColonChar         = TEXT( ':' );
const TCHAR CrChar            = TEXT( '\r' );
const TCHAR MinusChar         = TEXT( '-' );
const TCHAR NullChar          = TEXT( '\0' );
const TCHAR PlusChar          = TEXT( '+' );
const TCHAR SpaceChar         = TEXT( ' ' );
const TCHAR TabChar           = TEXT( '\t' );

const TCHAR UnknownString[]   = UNKNOWN;
const TCHAR EmptyString[]     = TEXT("");

/***********************************************************************
*                              VOID BOZO                               *
***********************************************************************/
static void bozo()
{
DB_TABLE t;
STRING_CLASS s;
int    gain;
int    id;
int    number;  /* Sensor number */
int    units;   /* Units index */
int    vartype;
double adder;
double subtrahend;
double multiplier;
double divisor;
double high;
double low;
double max_volts;
double x1;
double x2;
RECTANGLE_CLASS r;
WINDOW_CLASS w;
FILE_CLASS f;
TCHAR  desc[ASENSOR_DESC_LEN+1];

if ( !MainListbox.handle() )
    {
    MainWindow.get_client_rect( r );

    id = MAIN_LISTBOX_ID;
    MainListbox.init( CreateWindow(TEXT ("listbox"),
                  NULL,
                  WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOSEL,
                  0, 0, r.width(), r.height(),                       // x, y, w, h
                  MainWindow.handle(),
                  (HMENU) id,
                  MainInstance,
                  NULL)
                );

    w = MainListbox.handle();
    w.show();
    }

f.open_for_write( "c:\\sources\\version7\\test\\asensor.csv" );
if ( t.open("c:\\sources\\version7\\test\\asensor.txt", ASENSOR_RECLEN, PFL ) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        number  = t.get_long( ASENSOR_NUMBER_OFFSET );
        units   = t.get_long( ASENSOR_UNITS_OFFSET );
        gain    = t.get_long( ASENSOR_GAIN_OFFSET  );
        vartype = t.get_long( ASENSOR_VARIABLE_TYPE_OFFSET );
        t.get_alpha( desc, ASENSOR_DESC_OFFSET, ASENSOR_DESC_LEN );
        high       = t.get_double(ASENSOR_HIGH_VALUE_OFFSET);
        low        = t.get_double(ASENSOR_LOW_VALUE_OFFSET);
        multiplier = t.get_double( ASENSOR_MUL_OFFSET );
        subtrahend = t.get_double( ASENSOR_SUB_OFFSET );
        divisor    = t.get_double( ASENSOR_DIV_OFFSET );
        adder      = t.get_double( ASENSOR_ADD_OFFSET );
        max_volts = 5.0;
        if ( gain == 1 )
            max_volts = 10.0;
        x1 = 0.0;
        x2 = 0.0;
        if ( not_zero(multiplier)  )
            {
            x1 = (low - adder) * divisor / multiplier + subtrahend;
            x1 *= max_volts;
            x1 /= 4095.0;
            x2 = (high - adder) * divisor / multiplier + subtrahend;
            x2 *= max_volts;
            x2 /= 4095.0;
            }
        s = number;
        s += CommaChar;
        s += units;
        s += CommaChar;
        s += vartype;
        s += CommaChar;
        s += desc;
        s += CommaChar;
        s += x1;
        s += CommaChar;
        s += low;
        s += CommaChar;
        s += x2;
        s += CommaChar;
        s += high;
        MainListbox.add( s.text() );
        f.writeline( s );
        }

    t.close();
    }
f.close();
}


/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
STRING_CLASS  s;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case CREATE_CHOICE:
                bozo();
                return 0;

            case DELETE_CHOICE:
                return 0;

            case EXIT_CHOICE:
                MainWindow.close();
                return 0;
            }

        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
const WORD SocketVersion = MAKEWORD(2,0);

WNDCLASS wc;
WSADATA  wsadata;
HWND     w;

WSAStartup( SocketVersion, &wsadata );

if ( CoInitialize(0) != S_OK )
    MessageBox( 0, "Opps", "CoInitialize Failed", MB_OK );

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

w = CreateWindow(
    MyClassName,
    TEXT("Main Window Title"),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 300,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MyTbox = CreateWindow (
        TEXT("static"),
        "mytbox",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        0, 0,
        400, 20,
        w,
        (HMENU) MY_TBOX,
        MainInstance,
        NULL
        );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
CoUninitialize();
WSACleanup();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
