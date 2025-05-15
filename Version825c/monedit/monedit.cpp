#include <windows.h>
#include <ddeml.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\computer.h"
#include "..\include\machine.h"
#include "..\include\part.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#define MINIMIZE_MENU_CHOICE        103
#define EXIT_CHOICE                 108
#define MONITOR_CONFIG_CHOICE       502

#define _MAIN_
#include "extern.h"
#include "..\include\chars.h"
#include "..\include\events.h"
#include "resource.h"

static TCHAR HmiCurrentString[] = TEXT( "CURRENT" );
const  int   HmiCurrentLen      = sizeof(HmiCurrentString)/sizeof(TCHAR);

TCHAR   MyClassName[]   = TEXT( "MonEdit" );
STRING_CLASS MyWindowTitle;

static TCHAR ConfigSection[]          = TEXT( "Config" );
static TCHAR CommonResourceFile[]     = TEXT( "common" );
static TCHAR DisplayIniFile[]         = TEXT( "display.ini" );
static TCHAR MoneditResourceFile[]    = TEXT( "monedit" );
static TCHAR CurrentLanguageKey[]     = TEXT( "CurrentLanguage" );

FONT_LIST_CLASS   FontList;
STRINGTABLE_CLASS StringTable;

BOOL CALLBACK MonitorProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

void monitor_proc_cleanup();

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
NAME_CLASS s;

if ( s.get_exe_dir_file_name(TEXT("v5help.hlp")) )
    {
    WinHelp( MainWindow, s.text(), helptype, context );
    }
}

/***********************************************************************
*                      NEW_MACHINE_SETUP_CALLBACK                      *
***********************************************************************/
void new_machine_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
TCHAR * cp;
DWORD   bytes_copied;
DWORD   bytes_to_copy;
int     n;

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_to_copy = DdeGetData( edata, NULL, 0, 0 );
if ( bytes_to_copy == 0 )
    return;

n = (int) bytes_to_copy;
n /= sizeof( TCHAR );
n++;
if ( !NewSetupMachine.upsize(n) )
    return;

cp = NewSetupMachine.text();

bytes_copied = DdeGetData( edata, (LPBYTE) cp, bytes_to_copy, 0 );
if ( bytes_copied > 0 )
    {
    *(cp+n) = NullChar;
    fix_dde_name( cp );
    SendMessage( MonitorWindow, WM_NEWSETUP, 0, 0L );
    }
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
void paint_me()
{
PAINTSTRUCT ps;

BeginPaint( MainWindow, &ps );
EndPaint( MainWindow, &ps );
}

/***********************************************************************
*                              PLOT_MIN_CALLBACK                       *
***********************************************************************/
void plot_min_callback( TCHAR * topic, short item, HDDEDATA edata )
{
if ( !IsIconic(MainWindow) )
    CloseWindow( MainWindow );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_DBINIT :
        client_dde_startup( hWnd );
        register_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX, new_machine_setup_callback );
        register_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX,   plot_min_callback          );
        return 0;

    case WM_EV_SHUTDOWN:
        SendMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, MONITOR_CONFIG_1_MACH_HELP );
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case MINIMIZE_MENU_CHOICE:
                CloseWindow( MainWindow );
                return 0;

            case MONITOR_CONFIG_CHOICE:
                return 0;

            case EXIT_CHOICE:
                SendMessage( hWnd, WM_CLOSE, 0, 0L );
                return 0;
            }

    case WM_PAINT:
        paint_me();
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/**********************************************************************
*                           LOAD_RESOURCES                            *
**********************************************************************/
static void load_resources()
{
INI_CLASS ini;
NAME_CLASS s;

s.get_local_ini_file_name( DisplayIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(CurrentLanguageKey) )
    {
    s = ini.get_string();
    StringTable.set_language( s );
    }

/*
-----------------------------------------------------
Read my file first so I can override the common names
----------------------------------------------------- */
s.get_stringtable_name( MoneditResourceFile );
StringTable.read( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );

statics_startup();
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static bool init( int cmd_show )
{
WNDCLASS wc;
COMPUTER_CLASS c;

c.startup();
ComputerName = c.whoami();
load_resources();

MyWindowTitle = StringTable.find( TEXT("MONITOR_SETUP") );

if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return false;

ShuttingDown = FALSE;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MAKEINTRESOURCE(IDI_MONEDIT) );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    515, 179,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );

MonitorWindow  = CreateDialog(
    MainInstance,
    TEXT("MONITOR_LIST"),
    MainWindow,
    (DLGPROC) MonitorProc );

return true;
}

/***********************************************************************
*                        CHECK_FOR_HMI_MESSAGE                         *
* 1. CURRENT,new part name                                             *
void check_for_hmi_message( LPSTR cmd_line )
{
WINDOW_CLASS w;
NAME_CLASS s;

have_hmi_message = FALSE;
if ( strings_are_equal(HmiCreateString, cmd_line, HmiCreateLen) || strings_are_equal(HmiCurrentString, cmd_line, HmiCurrentLen) )
    {
    }
}
***********************************************************************/

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown()
{
MyWindowTitle.empty();

monitor_proc_cleanup();
unregister_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX );
unregister_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX );
client_dde_shutdown();
statics_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL  status;

MainInstance  = this_instance;

if ( !init(cmd_show) )
    return 0;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MonitorWindow )
        status = IsDialogMessage( MonitorWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }


shutdown();
return(msg.wParam);
}
