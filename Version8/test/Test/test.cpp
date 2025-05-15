#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\multistring.h"
#include "..\include\setpoint.h"
#include "..\include\vdbclass.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

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
TCHAR sorc[] = TEXT( "c:\\sources\\version8\\test\\results\\shotparm.txt" );
FIELDOFFSET fo;
long     i;
int      pi;
DB_TABLE t;
NAME_CLASS s;
SYSTEMTIME   st;

s = sorc;

if ( t.open(s, SHOTPARM_RECLEN, PWL) )
    {
    t.empty();
    for ( i=0; i<100001; i++ )
        {
        GetLocalTime( &st );
        t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, i, SHOT_LEN );
        t.put_date( SHOTPARM_DATE_OFFSET, st );
        t.put_time( SHOTPARM_TIME_OFFSET, st );
        t.put_short( SHOTPARM_SAVE_FLAGS_OFFSET, (short) 3, SHOTPARM_SAVE_FLAGS_LEN );
        fo = SHOTPARM_PARAM_1_OFFSET;
        for ( pi=0; pi<MAX_PARMS; pi++ )
            {
            t.put_double( fo, 0.0, SHOTPARM_FLOAT_LEN );
            fo += SHOTPARM_FLOAT_LEN+1;
            }

        t.rec_append();
        }
    t.close();
    }
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
*                                 BOZO                                 *
static void bozo()
{
VDB_CLASS v;

if ( v.open_for_rw(TEXT(".\\results\\SHOTPARM.csv")) )
    {
    if ( v.goto_record_number(593) )
        {
        v.rec_delete();
        }
    v.close();
    }
}
************************************************************************/

/***********************************************************************
*                                 BOZO                                 *
************************************************************************/
static void bozo()
{
NAME_CLASS s;
s = TEXT( "C:\\v8\\data\\Prince34\\Ford+Chevy+Pontiac\\results\\SHOTPARM.TXT" );
db_to_vdb( s, 0 );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
STRING_CLASS  s;

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
                    cmd = GetDlgItemInt( hWnd, MY_TBOX, 0, 0);
                    bozo();
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
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
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
