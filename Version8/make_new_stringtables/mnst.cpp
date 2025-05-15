#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\multistring.h"
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
WINDOW_CLASS MainListbox;

const int   MAX_COLS      = 1000;
static TCHAR MyClassName[] = TEXT( "mnst");

/**********************************************************************
*                       STRIP_WHITESPACE_FROM_EOL                     *
**********************************************************************/
static void strip_whitespace_from_eol( TCHAR * sorc )
{
TCHAR * ep;

ep = sorc + lstrlen( sorc );
while ( ep > sorc )
    {
    ep--;
    if ( *ep == SpaceChar || *ep == SemiColonChar || *ep == TabChar )
        *ep = NullChar;
    else
        break;
    }
}

/***********************************************************************
*                            READ_ONE_FILE                             *
***********************************************************************/
static bool read_one_file( STRING_CLASS & path )
{
static wchar_t UnicodeIntroducer = L'\xFEFF';

TCHAR           * cp;
int               i;
FILE_CLASS        f;
MULTISTRING_CLASS ms;
int               nof_languages;
NAME_CLASS        s;

if ( !f.open_for_read(path) )
    return false;

cp = f.readline();

#ifdef UNICODE
if ( *cp == UnicodeIntroducer )
    cp++;
#endif

nof_languages = asctoint32( cp );

for ( i=0; i<nof_languages; i++ )
    {
    cp = f.readline();
    strip_whitespace_from_eol( cp );

    /*
    ------------------------------------------------------------
    If this is an excel file the line will be enclosed in quotes
    ------------------------------------------------------------ */
    remove_all_occurances( cp, DoubleQuoteChar );

    if ( replace_char_with_null(cp, CommaChar) )
        {
        ms.append( cp );
        s = path;
        s.remove_ending_filename();
        s.cat_path( ms );
        if ( !s.directory_exists() )
            s.create_directory();
        }
    }
f.close();
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
                    s.get_text( MyTbox.handle() );
                    read_one_file( s );
                    }
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

MainListbox = CreateWindow (
    TEXT("listbox"),
    "List Box",
    WS_CHILD | WS_VISIBLE | LBS_STANDARD,
    0, 22,
    649, 280,
    w,
    (HMENU) MY_LB,
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
