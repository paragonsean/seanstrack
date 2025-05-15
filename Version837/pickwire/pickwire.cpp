#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\ftii.h"
#include "..\include\iniclass.h"
#include "..\include\limit_switch_class.h"   // Included for choose_wire
#include "..\include\machine.h"
#include "..\include\part.h"                 // Included for choose_wire
#include "..\include\param.h"                // Included for choose_wire
#include "..\include\machname.h"
#include "..\include\setpoint.h"
#include "..\include\rectclas.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\wclass.h"
#include "..\include\menuctrl.h"

#define _MAIN_
#include "..\include\chars.h"
#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS BrouseWindow;

static TCHAR MyClassName[]        = "Test";
static TCHAR MainMenuWindowName[] = TEXT( "MainMenu" );
static TCHAR ChooseWireResourceFile[] = TEXT( "choose_wire" );
static TCHAR CommonResourceFile[] = TEXT( "common" );
static TCHAR ConfigSection[]      = TEXT( "Config" );
static TCHAR CurrentLanguageKey[] = TEXT( "CurrentLanguage" );
static TCHAR DisplayIniFile[]     = TEXT( "display.ini" );
static TCHAR MyMenuFile[]         = TEXT( "pickwire.mnu" );
static TCHAR MyResourceFile[]     = TEXT( "pickwire" );

int CurrentPasswordLevel = 0;
MENU_CONTROL_ENTRY MainMenu;
STRINGTABLE_CLASS  StringTable;

HFONT MyFont = INVALID_FONT_HANDLE;
FONT_LIST_CLASS FontList;

MACHINE_CLASS Ma;

/***********************************************************************
*                           BROUSE_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK brouse_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {

    case WM_DBINIT:
        return TRUE;

    case WM_INITDIALOG:
        BrouseWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        BrouseWindow.post( WM_DBINIT );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case IDCANCEL:
                MainWindow.close();
                return TRUE;

            case CHOOSE_WIRE_PB:
                choose_wire(GetDlgItem(hWnd, id), hWnd, BINARY_VALVE_PULSE_OUTPUT_TYPE, NO_INDEX, Ma.computer, Ma.name, Ma.current_part );
                return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                            LOAD_MAIN_MENU                            *
***********************************************************************/
static void load_main_menu()
{
INI_CLASS ini;
NAME_CLASS s;

s.get_exe_dir_file_name( MyMenuFile );
MainMenu.read( s, CurrentPasswordLevel );
MainMenu.resize();
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainMenu.create_windows( MainMenuWindowName, hWnd );
        load_main_menu();
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        return 0;

    case WM_COMMAND:
        switch (id)
            {
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
    MainMenu.language = ini.get_string();
    StringTable.set_language( MainMenu.language );
    }

s.get_exe_dir_file_name( MyMenuFile );
MainMenu.read( s, 0 );

s.get_stringtable_name( MyResourceFile );
StringTable.read( s );

s.get_stringtable_name( ChooseWireResourceFile );
StringTable.append( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );

MyFont = FontList.get_handle( StringTable.fontclass() );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
STRING_CLASS title;
WNDCLASS wc;
COMPUTER_CLASS c;

MENU_CONTROL_ENTRY::register_window();
c.startup();

Ma.computer = c.whoami();
Ma.name     = TEXT( "Prince34" );
Ma.find( Ma.computer, Ma.name );

load_resources();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );
register_choose_wire_control();

title = StringTable.find( TEXT("MAIN_WINDOW_TITLE") );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT( "BROUSE_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) brouse_dialog_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
COMPUTER_CLASS c;

c.shutdown();

if ( MyFont != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( MyFont );
    MyFont = INVALID_FONT_HANDLE;
    }
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

    if ( BrouseWindow.handle() )
        status = IsDialogMessage( BrouseWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
