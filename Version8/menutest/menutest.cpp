#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\rectclas.h"
#include "..\include\textlist.h"
#include "..\include\wclass.h"
#include "..\include\menuctrl.h"
#include "..\include\subs.h"

#define _MAIN_
#include "..\include\chars.h"

#include "resource.h"

const int EXIT_MENU_CHOICE = 108;
const int SHOW_REF_TRACE_CHOICE = 506;
const int SAVE_REF_TRACE_CHOICE = 116;

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;

MENU_CONTROL_ENTRY MyMenu;

static TCHAR MyClassName[] = "MenuTest";
static HDC   MyDc          = 0;
static HFONT MyFont        = 0;

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                             CREATE_MENU                              *
***********************************************************************/
static void create_menu( HWND hWnd )
{
NAME_CLASS s;
s = TEXT( "C:\\sources\\version7\\menutest\\free\\mymenu.mnu" );
MyMenu.create_windows( TEXT("JaysWindow"), hWnd );
MyMenu.read( s );
MyMenu.resize();
MainWindow.shrinkwrap( MyMenu.mw.handle() );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
BOOLEAN b;
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        create_menu( hWnd );
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case EXIT_CHOICE:
                MainWindow.close();
                return 0;
            }
        break;

    case WM_VTW_MENU_CHOICE:
        id = (int) wParam;
        switch ( id )
            {
            case EXIT_MENU_CHOICE:
                MainWindow.close();
                return 0;
            
            case SHOW_REF_TRACE_CHOICE:
                b = MyMenu.is_checked( id );
                if ( b )
                    b = FALSE;
                else
                    b = TRUE;
                MyMenu.set_checkmark( id, b );
                return 0;
            
            case SAVE_REF_TRACE_CHOICE:
                MyMenu.set_gray( id, TRUE );
                return 0;
            }
        return 0;
            
    case WM_SIZE:
        MyMenu.resize();
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
STRING_CLASS title;
WNDCLASS wc;

MENU_CONTROL_ENTRY::register_window();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = 0;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_POPUP | WS_VISIBLE | WS_THICKFRAME,  //WS_OVERLAPPEDWINDOW,
    0, 0,                             // X,y
    1280, 40,                         // W,h
    NULL,
    NULL,
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

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage(  &msg );
    }

shutdown();
return( msg.wParam );
}
