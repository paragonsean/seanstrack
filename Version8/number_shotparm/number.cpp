#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
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

const int   MAX_COLS      = 1000;
static TCHAR MyClassName[] = TEXT( "Number");

/***********************************************************************
*                               ADD_LINES                              *
***********************************************************************/
static void add_lines()
{
const int32 SHOT_NAME_LEN = 6;

TCHAR      * cp;
STRING_CLASS d;
NAME_CLASS   destfile;
FILE_CLASS   df;
int          i;
STRING_CLASS s;
FILE_CLASS   sf;
TCHAR        shot_name[SHOT_NAME_LEN+1];
STRING_CLASS sorcfile;

sorcfile.get_text( MyTbox.handle() );
if ( sf.open_for_read(sorcfile) )
    {
    destfile = sorcfile;

    cp = destfile.find( PeriodChar );
    if ( !cp )
        {
        sf.close();
        MessageBox( 0, "No Extension", "Cannot Continue", MB_OK );
        return;
        }
    d = AChar;
    d += cp;
    *cp = NullChar;
    destfile += d;
    df.open_for_write( destfile );

    i = 0;
    while ( true )
        {
        s = sf.readline();
        if ( s.isempty() )
            break;
        i++;
        d = i;
        d += CommaChar;

        insalph( shot_name, (int32)i, SHOT_NAME_LEN, ZeroChar, DECIMAL_RADIX );
        shot_name[SHOT_NAME_LEN] = NullChar;
        d += shot_name;

        cp = s.text();
        cp = findchar( CommaChar, cp );
        if ( cp )
            {
            cp++;
            cp = findchar( CommaChar, cp );
            }
        if ( cp )
            {
            d += cp;
            df.writeline( d.text() );
            }
        }

    df.close();
    sf.close();
    }
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
    TEXT("Number SHOTPARM"),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    650, 300,                         // W,h
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
    600, 20,
    w,
    (HMENU) MY_TBOX,
    MainInstance,
    NULL
    );

OkButton = CreateWindow (
    TEXT("button"),
    "OK",
    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
    602, 0,
    40, 20,
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
