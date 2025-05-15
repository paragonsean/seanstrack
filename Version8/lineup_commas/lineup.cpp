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
static TCHAR MyClassName[] = TEXT( "LineUp");

/***********************************************************************
*                               ADD_LINES                              *
***********************************************************************/
static void add_lines()
{
static TCHAR dest_end[] = TEXT( "-fixed.CSV" );
STRING_CLASS d;
FILE_CLASS   df;
STRING_CLASS s;
STRING_CLASS sorcfile;
NAME_CLASS   destfile;
FILE_CLASS   sf;
TCHAR      * cp;
TCHAR      * bp;
int          i;
int          my_width;
int          width[MAX_COLS];

for ( i=0; i<MAX_COLS; i++ )
    width[i] = 0;
sorcfile.get_text( MyTbox.handle() );
if ( sf.open_for_read(sorcfile) )
    {
    while ( true )
        {
        s = sf.readline();
        if ( s.isempty() )
            break;
        i = 0;
        cp = s.text();
        my_width = 0;
        while ( true )
            {
            if ( *cp == NullChar )
                {
                width[i] = my_width;
                break;
                }
            if ( *cp == CommaChar )
                {
                if ( my_width > width[i] )
                    width[i] = my_width;
                my_width = 0;
                i++;
                }
            else
                {
                my_width++;
                }
            cp++;
            }
        }

    sf.rewind();
    destfile = sorcfile;
    destfile.remove_extension();
    destfile += dest_end;
    df.open_for_write( destfile );
    while ( true )
        {
        d.null();
        s = sf.readline();
        if ( s.isempty() )
            break;
        i = 0;
        cp = s.text();
        bp = cp;
        my_width = 0;
        while ( true )
            {
            if ( *cp == NullChar )
                {
                while ( my_width < width[i] )
                    {
                    d += SpaceChar;
                    my_width++;
                    }
                d += bp;
                df.writeline( d.text() );
                break;
                }
            if ( *cp == CommaChar )
                {
                while ( my_width < width[i] )
                    {
                    d += SpaceChar;
                    my_width++;
                    }
                *cp = NullChar;
                d += bp;
                *cp = CommaChar;
                d += CommaChar;
                d += SpaceChar;
                bp = cp;
                bp++;
                my_width = 0;
                i++;
                }
            else
                {
                my_width++;
                }
            cp++;
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
    TEXT("Line Up Commas"),
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
