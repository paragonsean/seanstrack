#include <windows.h>
#include <ddeml.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\genlist.h"
#include "..\include\events.h"

HINSTANCE MainInstance;
HWND      MainWindow;

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch ( msg )
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        PostMessage( hWnd, WM_DBCLOSE, 0, 0L );
        return 0;

    case WM_DBCLOSE:
        poke_data( DDE_EXIT_TOPIC,  ItemList[NEW_MACH_STATE_INDEX].name, UNKNOWN );
        PostMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static void init( int cmd_show )
{
static TCHAR MyClassName[] = TEXT( "VtwShutdown" );
WNDCLASS wc;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = NULL;
wc.lpszMenuName  = NULL;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyClassName,
    WS_POPUP | WS_VISIBLE,
    0, 0,                             // X,y
    100, 100,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
}

/***********************************************************************
*                                SHUTDOWN                              *
***********************************************************************/
static void shutdown()
{
client_dde_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;

MainInstance = this_instance;

init( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
    }

shutdown();
return msg.wParam;
}
