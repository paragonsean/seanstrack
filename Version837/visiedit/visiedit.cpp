#include <windows.h>
#include <richedit.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\rectclas.h"
#include "..\include\stringtable.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"
#include "..\include\menuctrl.h"

#include "visiedit.h"

#define _MAIN_
#include "..\include\chars.h"

HINSTANCE    MainInstance;
HWND         MainWindow;
HWND         EditWindow;
HMENU        EditWindowId = (HMENU) 1;
HMODULE      RichEditLibHandle = 0;

STRING_CLASS MyLanguage;
STRING_CLASS MyWindowTitle;
TCHAR        FileName[MAX_PATH+1];
TCHAR        MainHelpFileName[]  = TEXT( "v5help.hlp" );

FONT_LIST_CLASS    FontList;
MENU_CONTROL_ENTRY MainMenu;
STRINGTABLE_CLASS  StringTable;

TCHAR MyClassName[]       = TEXT( "VTWEditor" );
TCHAR FileNameDelimiter[] = TEXT( "   " );
/*
--------------
Ini file names
-------------- */
TCHAR CommonResourceFile[] = TEXT( "common" );
TCHAR ConfigSection[]      = TEXT( "Config" );
TCHAR CurrentLanguageKey[] = TEXT( "CurrentLanguage" );
TCHAR DisplayIniFile[]     = TEXT( "display.ini" );
TCHAR MainMenuWindowName[] = TEXT( "VisiEditMenu" );
TCHAR MyMenuFile[]         = TEXT( "visiedit.mnu" );
TCHAR MyResourceFile[]     = TEXT( "visiedit" );
TCHAR MyTitleID[]          = TEXT( "MY_TITLE" );

/***********************************************************************
*                        STRINGTABLE_MESSAGE_BOX                       *
***********************************************************************/
int stringtable_message_box( TCHAR * msg_id, TCHAR * title_id, UINT boxtype )
{
STRING_CLASS msg;
STRING_CLASS title;

StringTable.find( msg,   msg_id );
StringTable.find( title, title_id );

boxtype |= MB_SETFOREGROUND;

return MessageBox( NULL, msg.text(), title.text(), boxtype );
}

/***********************************************************************
*                        STRINGTABLE_MESSAGE_BOX                       *
***********************************************************************/
void stringtable_message_box( TCHAR * msg_id, TCHAR * title_id )
{
const UINT boxtype = MB_OK | MB_SYSTEMMODAL;

stringtable_message_box( msg_id, title_id, boxtype );
}

/***********************************************************************
*                          SAVE_TO_FILE                                *
***********************************************************************/
static void save_to_file()
{
HANDLE  fh;
TCHAR * s;
DWORD   slen;
DWORD   bytes_to_write;
DWORD   bytes_written;

const DWORD access_mode  = GENERIC_WRITE;
const DWORD share_mode   = 0;
const DWORD create_flags = CREATE_ALWAYS;

if ( *FileName == NullChar )
    {
    stringtable_message_box( TEXT("NOTHING_TO_SAVE"), TEXT("CANNOT_COMPLY_STRING") );
    return;
    }

slen = GetWindowTextLength( EditWindow );
s    = maketext( slen+1 );
if ( s )
    {
    if ( get_text(s, EditWindow, slen) )
        {
        fh = CreateFile( FileName, access_mode, share_mode, NULL, create_flags, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
        if ( fh != INVALID_HANDLE_VALUE )
            {
            *(s+slen) = NullChar;
            slen = lstrlen( s );
            bytes_to_write = slen * sizeof(TCHAR);
            WriteFile( fh, s, bytes_to_write, &bytes_written, 0 );
            delete[] s;
            CloseHandle( fh );
            SendMessage( EditWindow, EM_SETMODIFY, FALSE, 0L );
            }
        }
    }
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
TCHAR path[MAX_PATH+1];

if ( get_exe_directory(path) )
    {
    append_filename_to_path( path, MainHelpFileName );
    WinHelp( MainWindow, path, helptype, context );
    }
}

/***********************************************************************
*                          LOAD_FROM_FILE                              *
***********************************************************************/
static void load_from_file()
{
static int32 max_file_size = 30000;
HANDLE  fh;
TCHAR * s;
int32   n;
DWORD   slen;
DWORD   bytes_to_read;
DWORD   bytes_read;
BOOL    status;

const DWORD access_mode  = GENERIC_READ;
const DWORD share_mode   = FILE_SHARE_READ | FILE_SHARE_WRITE;
const DWORD create_flags = OPEN_EXISTING;

n = file_size( FileName );
if ( n > max_file_size )
    {
    max_file_size = 2 * n;
    SendMessage( EditWindow, EM_EXLIMITTEXT, 0, (LPARAM) max_file_size );
    }

fh = CreateFile( FileName, access_mode, share_mode, NULL, create_flags, FILE_FLAG_SEQUENTIAL_SCAN, 0 );

if ( fh != INVALID_HANDLE_VALUE )
    {
    bytes_to_read = GetFileSize( fh, 0 );
    slen = bytes_to_read/sizeof(TCHAR);

    s = maketext( slen+1 );
    if ( s )
        {
        status = ReadFile( fh, s, bytes_to_read, &bytes_read, 0 );
        if ( status && bytes_read > 0 )
            {
            *(s+slen) = NullChar;
            if (!set_text(EditWindow, s) )
                {
                stringtable_message_box( TEXT("ERROR_LOADING_FILE"), TEXT("CANNOT_COMPLY_STRING") );
                *FileName = NullChar;
                }
            }
        delete[] s;
        }
    CloseHandle( fh );
    SendMessage( EditWindow, EM_SETMODIFY, FALSE, 0L );
    StringTable.find( MyWindowTitle, MyTitleID );
    if ( *FileName != NullChar )
        {
        MyWindowTitle += FileNameDelimiter;
        MyWindowTitle += FileName;
        }
    set_text( MainWindow, MyWindowTitle.text() );
    }
}

/***********************************************************************
*                         CHOOSE_FILE_TO_LOAD                          *
***********************************************************************/
static void choose_file_to_load( void )
{
static TCHAR Filter[] = TEXT( "Visitrak Files\0*.TXT;*.DAT;*.MSG;*.CSV;*.INI;*.PRG;*.MNU;*.LST\0" );

OPENFILENAME fh;

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow;
fh.hInstance         = 0;
fh.lpstrFilter       = Filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = FileName;
fh.nMaxFile          = MAX_PATH;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = 0;
fh.lpstrTitle        = StringTable.find( TEXT("CHOOSE_FILE") );
fh.Flags             = OFN_EXPLORER;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*FileName = NullChar;

if ( GetOpenFileName(&fh) )
    load_from_file();

}

/***********************************************************************
*                            CHECK_FOR_SAVE                            *
*                                                                      *
*  Check to see if the text has changed and, if so, whether the        *
*  operator wishes to save the changes.                                *
*                                                                      *
***********************************************************************/
static int check_for_save( void )
{
int i;

i = IDNO;

if ( EditWindow && (*FileName != NullChar) )
    {
    if ( SendMessage( EditWindow, EM_GETMODIFY, 0, 0L) )
        {
        i = stringtable_message_box( TEXT("WISH_TO_SAVE"), TEXT("CHANGES_MADE"), MB_YESNOCANCEL );
        if ( i == IDYES )
            save_to_file();
        }
    }

return i;
}
/***********************************************************************
*                              LOAD_MENU                               *
***********************************************************************/
static void load_menu()
{
NAME_CLASS   s;
WINDOW_CLASS w;

MainMenu.create_windows( MainMenuWindowName, MainWindow );
MainMenu.language = MyLanguage;
s.get_exe_dir_file_name( MyMenuFile );
MainMenu.read( s, 0 );

w = EditWindow;
set_statics_font( w );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        MainWindow = hWnd;
        EditWindow = CreateWindow(
            TEXT("RichEdit"),
            NULL,
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            0, 0,
            0, 0,
            MainWindow,
            EditWindowId,
            MainInstance,
            NULL
            );

        load_menu();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_DBINIT:
        choose_file_to_load();
		SetFocus( EditWindow );
        return 0;

//    case WM_SETFOCUS:
//        SetFocus( EditWindow );
//        return 0 ;

    case WM_SIZE :
        MainMenu.resize();
        MoveWindow( EditWindow, 0, MainMenu.mr.bottom, LOWORD(lParam), HIWORD(lParam)-MainMenu.mr.bottom, TRUE );
        return 0 ;

    case WM_HELP:
        gethelp( HELP_CONTEXT, TEXT_EDITOR_HELP );
        return 0;

    case WM_VTW_MENU_CHOICE:
        switch (id)
            {
            case OPEN_MENU_CHOICE:
                if ( check_for_save() != IDCANCEL )
                    choose_file_to_load();
                SetFocus( EditWindow );
                return 0;

            case SAVE_MENU_CHOICE:
                save_to_file();
                SetFocus( EditWindow );
                return 0;

            case MINIMIZE_MENU_CHOICE:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_MENU_CHOICE:
                if ( check_for_save() != IDCANCEL )
                    PostMessage( hWnd, WM_CLOSE, 0, 0L );
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
    MyLanguage = ini.get_string();
    StringTable.set_language( MyLanguage );
    }

s.get_exe_dir_file_name( MyMenuFile );

StringTable.find( MyWindowTitle,  MyTitleID );

statics_startup();
}

/***********************************************************************
*                         MY_PREVIOUS_INSTANCE                         *
***********************************************************************/
BOOLEAN my_previous_instance()
{
if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return TRUE;

return FALSE;
}
/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static bool init( int cmd_show )
{
COMPUTER_CLASS c;
TCHAR path[MAX_PATH+1];
WNDCLASS wc;

*FileName = NullChar;

MENU_CONTROL_ENTRY::register_window();
c.startup();

load_resources();
if ( my_previous_instance() )
    return false;

/*
----------------------------------------------
I need to make sure the richedit dll is loaded
---------------------------------------------- */
GetSystemDirectory( path, MAX_PATH );
append_filename_to_path( path, TEXT("RICHED32.DLL") );
HMODULE RichEditLibHandle = (HMODULE) LoadLibrary( path );

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MAKEINTRESOURCE(VISIEDIT_ICON));
wc.lpszMenuName  = 0; //TEXT("MainMenu");
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
    0, 0,                             // X,y
    750, 550,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
return true;
}

/***********************************************************************
*                            VOID SHUTDOWN                             *
***********************************************************************/
static void shutdown()
{
if ( RichEditLibHandle )
    {
    FreeLibrary( RichEditLibHandle );
    RichEditLibHandle = 0;
    }
statics_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;

MainInstance = this_instance;

if ( !init(cmd_show) )
    return 0;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    }

shutdown();
return(msg.wParam);
}
