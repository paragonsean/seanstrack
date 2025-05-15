#include <windows.h>
#include <richedit.h>
#include <conio.h>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <regex>
#include <shlobj.h>

#include "..\..\include\visiparm.h"
#include "..\..\include\nameclas.h"
#include "..\..\include\asensor.h"
#include "..\..\include\dbclass.h"
#include "..\..\include\listbox.h"
#include "..\..\include\multistring.h"
#include "..\..\include\setpoint.h"
#include "..\..\include\vdbclass.h"
#include "..\..\include\wclass.h"
#include "..\..\include\subs.h"
#include "..\..\include\event_client.h"

#include "resource.h"
#include "resrc1.h"

#define _MAIN_
#include "..\..\include\chars.h"

extern TCHAR MonallIniFile[] = TEXT( "Monall.ini" );
extern TCHAR ConfigSection[] = TEXT( "Config" );

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS OkButton;
WINDOW_CLASS MyTbox;
WINDOW_CLASS ResultBox;
LISTBOX_CLASS MainListbox;
NAME_CLASS TestDataDir;
static TCHAR MyClassName[] = TEXT( "Test");

EVENT_CLIENT EventClient;

/***********************************************************************
*                             CONNECT_DLG_PROC                         *
***********************************************************************/
BOOL CALLBACK connect_dlg_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int  id;
    id = LOWORD(wParam);
    STRING_CLASS sa;
    STRING_CLASS sp;
    static TCHAR buf[1025];
    TCHAR path[MAX_PATH + 1];

    switch (msg)
    {
    case WM_INITDIALOG:
        GetPrivateProfileString(
            TEXT("Config"),
            TEXT("LocalIpAddress"),
            TEXT("UNKNOWN"),
            buf,
            MAX_LONG_INI_STRING_LEN,
            TEXT("visitrak.ini") );
        sa = buf;
        sa.set_text(hWnd, IDC_IPADDRESS);
        break;

    case WM_COMMAND:
        switch (id)
        {
        case ID_CONNECT_BTN:
            sa.get_text(hWnd, IDC_IPADDRESS);
            WritePrivateProfileString(
                TEXT("Config"),
                TEXT("LocalIpAddress"),
                sa.text(),
                TEXT("visitrak.ini"));
            
            /*
            -------------------
            Connect to eventman
            ------------------- */
            EventClient.set_ip_addr(sa.text());
            EventClient.set_port(sp.int_value());
            EventClient.connect_to_server();

            EndDialog(hWnd, 0);
            break;

        case IDCANCEL:
            EndDialog(hWnd, 0);
            return TRUE;
        }
        break;
    }

    return FALSE;
}


/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
STRING_CLASS command;
STRING_CLASS result;
std::string path;
NAME_CLASS path_str;
std::string box_msg;
REGEX cmp_split_ex( TEXT( "(.*)\\\\data\\\\(.*)\\\\(.*)" ) );
REGMATCH sm;

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
                return 0;

            case ID_CONNECT_MENU:
                DialogBox(MainInstance, MAKEINTRESOURCE(ID_CONNECT_DLG), MainWindow.handle(), connect_dlg_proc);
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
    605, 400,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MyTbox = CreateWindow(
    TEXT("edit"),
    TEXT("0"),
    WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT | ES_MULTILINE,
    //WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
    0, 0,
    500, 200,
    w,
    (HMENU) MY_TBOX,
    MainInstance,
    NULL
    );

OkButton = CreateWindow (
    TEXT("button"),
    TEXT("Send"),
    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
    501, 0,
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

TCHAR exe_path[500];
GetModuleFileName(NULL, exe_path, 500);
TestDataDir = exe_path;
TestDataDir.remove_ending_filename();
TestDataDir.remove_ending_filename();

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
