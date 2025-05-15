#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\multistring.h"
#include "..\include\setpoint.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define  SET_CRT_DEBUG_FIELD(a) _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif

#include "resource.h"
#include "resrc1.h"

#define _MAIN_
#include "..\include\chars.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS OkButton;
WINDOW_CLASS MyTbox;
LISTBOX_CLASS MainListbox;

static TCHAR MyClassName[] = TEXT( "Test");

/***********************************************************************
*                               ADD_LINES                              *
***********************************************************************/
static void add_lines()
{
#ifdef _DEBUG
_CrtSetReportMode( _CRT_WARN,   _CRTDBG_MODE_FILE );
_CrtSetReportMode( _CRT_ERROR,  _CRTDBG_MODE_FILE );
_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
#endif

TCHAR sorc[] = TEXT( "c:\\sources\\version8\\test\\results\\shotparm.txt" );
STRING_CLASS s;
FILE_CLASS   f;
HANDLE       fh;

#ifdef _DEBUG
_CrtMemState s1, s2, s3;
#endif

f.open_for_write( "c:\\bozo\\memoryleaks.txt" );
f.writeline( "Hi you guys" );

fh = f.file_handle();

/*
----------------------------------------------------------
Store a memory checkpoint in the s1 memory-state structure
---------------------------------------------------------- */
s = sorc;
s += TEXT( "Hi you guys" );
s += TEXT( " and Hi you guys..." );
s += TEXT( " some more. " );
s += TEXT( "etc. eof" );

s.empty();

f.close();

}

/***********************************************************************
*                           CREATE_FILES                               *
************************************************************************/
static void create_files()
{
TCHAR sorc[] = TEXT( "c:\\sources\\version8\\test\\results\\000001.pr2" );
TCHAR myroot[] = TEXT( "c:\\sources\\version8\\test\\results" );
int i;

NAME_CLASS s;
NAME_CLASS sn;
NAME_CLASS d;
s = sorc;

for ( i=2; i<10001; i++)
    {
    sn = i;
    sn.rjust( 6, ZeroChar );
    sn += TEXT( ".pr2" );
    d = myroot;
    d.cat_path( sn );
    s.copyto( d );
    }
}

/***********************************************************************
*                              READ_FILES                              *
***********************************************************************/
static void read_files()
{
TCHAR sorc[] = TEXT( "c:\\sources\\version7\\test\\results\\000001.csv" );
TCHAR myroot[] = TEXT( "c:\\sources\\version7\\test\\results" );
int i;
DWORD t1;
DWORD t2;
SETPOINT_CLASS s;
NAME_CLASS sn;
NAME_CLASS d;

t1 = GetTickCount();
for ( i=1; i<1001; i++)
    {
    sn = i;
    sn.rjust( 6, ZeroChar );
    sn += TEXT( ".csv" );
    d = myroot;
    d.cat_path( sn );
    s.get( d );
    }
t2 = GetTickCount();

sn = (unsigned int) t1;
d  = (unsigned int) t2;
MessageBox( 0, d.text(), sn.text(), MB_OK );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
STRING_CLASS  s;
double x;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

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
            case IDOK:
                if ( cmd == BN_CLICKED )
                    {
                    s.get_text( hWnd, MY_TBOX);
                    x = s.real_value();
                    s = x;
                    s.set_text( hWnd, MY_TBOX );
                    }
                return 0;

            case CREATE_CHOICE:
                add_lines();
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
WNDCLASS wc;
HWND     w;


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
    TEXT("edit"),
    "0",
    WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
    0, 0,
    80, 20,
    w,
    (HMENU) MY_TBOX,
    MainInstance,
    NULL
    );

OkButton = CreateWindow (
    TEXT("button"),
    "OK",
    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
    81, 0,
    30, 20,
    w,
    (HMENU) IDOK,
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
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

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
