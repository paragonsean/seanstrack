#include <windows.h>
#include <winuser.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"
#include "resrc1.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MyTbox;

static TCHAR MyClassName[]            = "Test";

const TCHAR BackSlashChar     = TEXT( '\\' );
const TCHAR BackSlashString[] = TEXT( "\\" );
const TCHAR ColonChar         = TEXT( ':' );
const TCHAR CrChar            = TEXT( '\r' );
const TCHAR MinusChar         = TEXT( '-' );
const TCHAR NullChar          = TEXT( '\0' );
const TCHAR PlusChar          = TEXT( '+' );
const TCHAR SpaceChar         = TEXT( ' ' );
const TCHAR TabChar           = TEXT( '\t' );

const TCHAR UnknownString[]   = UNKNOWN;
const TCHAR EmptyString[]     = TEXT("");

const UINT TIMER_ID           = 1;

/***********************************************************************
*                           CHECK_FOR_HUMAN                            *
***********************************************************************/
static void check_for_human()
{
STRING_CLASS  s;
LASTINPUTINFO info;
DWORD         now;

info.cbSize = sizeof( LASTINPUTINFO );

if ( GetLastInputInfo(&info) )
    {
    now = GetTickCount();
    if ( now > info.dwTime )
        {
        now -= info.dwTime;
        s = (unsigned int) now;
        }
    else
        {
        s = "Error in GetLastInputInfo.";
        }
    }
s.set_text( MyTbox.handle() );
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
        SetTimer( hWnd, TIMER_ID, 1000, NULL );
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case CREATE_CHOICE:
                return 0;

            case DELETE_CHOICE:
                return 0;

            case EXIT_CHOICE:
                MainWindow.close();
                return 0;
            }

        break;

    case WM_TIMER:
        check_for_human();
        return 0;

    case WM_DESTROY:
        KillTimer( hWnd, TIMER_ID );
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
WNDCLASS wc;
HWND     w;

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
