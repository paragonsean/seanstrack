#include <windows.h>
#include <shlobj.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"

HINSTANCE     MainInstance;
WINDOW_CLASS  MainWindow;

WINDOW_CLASS  BrouseWindow;
HWND          DestEb = 0;
LISTBOX_CLASS Lb;
static TCHAR  NullChar   = TEXT( '\0' );
static TCHAR  PeriodChar = TEXT( '.' );
static TCHAR  SpaceChar  = TEXT( ' ' );

static TCHAR        ConfigSection[]   = TEXT( "Config" );
static STRING_CLASS DestExesDir( TEXT("c:\\v5\\exes\\") );
static TCHAR        MyClassName[]     = TEXT( "Test" );
static TCHAR        VersionKey[]      = TEXT( "Version" );
static TCHAR        VisiTrakIniFile[] = TEXT( "c:\\v5\\exes\\visitrak.ini" );
static TCHAR        VisiTrakIniFileName[] = TEXT( "visitrak.ini" );

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                            WAIT_FOR_FILE                             *
*        Wait for a file to appear (e.g. after CopyFile).              *
***********************************************************************/
static boolean wait_for_file( TCHAR * filename )
{
int count;

count = 10;
while ( true )
    {
    if ( file_exists(filename) )
        break;

    count--;
    if ( count == 0 )
        return false;

    Sleep( 500 );
    }

return true;
}

/***********************************************************************
*                     SCROLL_LISTBOX_TO_LAST_ENTRY                     *
***********************************************************************/
static void scroll_listbox_to_last_entry()
{
int ti;

ti = Lb.count() - Lb.visible_row_count();

if ( ti > Lb.top_index() )
    Lb.set_top_index( ti );
}

/***********************************************************************
*                         CREATE_DESKTOP_LINK                          *
*  The title is what will appear on the desktop,                       *
*  e.g., Data Archive Restore                                          *
*                                                                      *
*  the link_name is the name of the link itself                        *
*  e.g. "data archive restore.lnk"                                     *
*                                                                      *
*  the exe_path is the full path to the executable                     *
*  e.g. C:\V5\EXES\DSRESTORE.EXE                                       *
***********************************************************************/
static void create_desktop_link( TCHAR * title, TCHAR * link_name, TCHAR * exe_path )
{
static TCHAR ShellFolderKey[] = TEXT( "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders" );

TCHAR buf[MAX_PATH];
WCHAR ws[MAX_PATH];

ULONG buf_size = MAX_PATH;
HKEY  hCU;
DWORD lpType;

HRESULT        result;
IShellLink   * psl;
IPersistFile * ppf;
bool           have_key;

/*
---------------------------
Get the path of the desktop
--------------------------- */
have_key = false;
if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, ShellFolderKey, 0, KEY_QUERY_VALUE, &hCU) == ERROR_SUCCESS )
    {
    buf_size = MAX_PATH * sizeof(TCHAR);
    if ( RegQueryValueEx( hCU, TEXT( "Common Desktop"), NULL, &lpType, (unsigned char *) &buf, &buf_size) == ERROR_SUCCESS )
        have_key = true;
    RegCloseKey(hCU);
    }

/*
-----------------------------------------
Win95 doesn't normally have the above key
----------------------------------------- */
if ( !have_key )
    {
    if ( RegOpenKeyEx(HKEY_CURRENT_USER, ShellFolderKey, 0, KEY_QUERY_VALUE, &hCU) == ERROR_SUCCESS )
        {
        buf_size = MAX_PATH * sizeof(TCHAR);
        if ( RegQueryValueEx( hCU, TEXT("Desktop"), NULL, &lpType, (unsigned char *) &buf, &buf_size) == ERROR_SUCCESS )
            have_key = true;
        RegCloseKey(hCU);
        }
    }

append_filename_to_path(  buf, link_name );

if ( CoInitialize(0) == S_OK )
    {
    /*
    ------------------------------------------
    Get a pointer to the IShellLink interface.
    ------------------------------------------ */
    result = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**) &psl );
    if (SUCCEEDED(result) )
        {
        /*
        ------------------------------------------------------------------------------------------------
        Query IShellLink for the IPersistFile interface for saving the shell link in persistent storage.
        ------------------------------------------------------------------------------------------------ */
        result = psl->QueryInterface( IID_IPersistFile, (void**) &ppf );
        if ( SUCCEEDED(result) )
            {
            /*
            --------------------------------------
            Set the path to the shell link target.
            -------------------------------------- */
            psl->SetPath( exe_path );

            /*
            --------------------------------------
            Set the description of the shell link.
            -------------------------------------- */
            psl->SetDescription( title );

            /*
            ----------------------
            Ensure string is ANSI.
            ---------------------- */
            #ifdef UNICODE
                lstrcpy( ws, buf );
            #else
                MultiByteToWideChar(CP_ACP, 0, buf, -1, ws, MAX_PATH);
            #endif

            /*
            ------------------------------------------------
            Save the link via the IPersistFile::Save method.
            ------------------------------------------------ */
            result = ppf->Save(ws, TRUE);

            /*
            --------------------------------
            Release pointer to IPersistFile.
            -------------------------------- */
            ppf->Release();
            }

        /*
        ------------------------------
        Release pointer to IShellLink.
        ------------------------------ */
        psl->Release();
        }

    CoUninitialize();
    }
}

/***********************************************************************
*                            COPY_ONE_FILE                             *
***********************************************************************/
BOOL copy_one_file( TCHAR * dest_dir, TCHAR * file_to_copy )
{
static TCHAR DotDotDot[] = TEXT( "..." );
static TCHAR Waiting[]   = TEXT( "Waiting for copy to finish..." );

TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];
TCHAR * cp;
BOOL copy_ok;

/*
--------------------------------
Tell the human what is happening
-------------------------------- */
cp = copystring( sorc, TEXT("Copying ") );
cp = copystring( cp,   file_to_copy );
copystring( cp, DotDotDot );
Lb.add( sorc );

/*
----------------------------------------------------
The source file is in the same dir as the executable
---------------------------------------------------- */
copystring( sorc, exe_directory() );
append_filename_to_path( sorc, file_to_copy );

copystring( dest, dest_dir );
append_filename_to_path( dest, file_to_copy );

copy_ok = CopyFile( sorc, dest, FALSE );

if ( copy_ok )
    {
    Lb.add( TEXT("Waiting for copy to finish...") );
    wait_for_file( dest );

    /*
    -----------------------------------
    Remove the read-only file attribute
    ----------------------------------- */
    SetFileAttributes( dest, FILE_ATTRIBUTE_NORMAL );
    }
else
    {
    Lb.add( TEXT("Copy FAILED!") );
    }

return copy_ok;
}

/***********************************************************************
*                                BEGIN                                 *
***********************************************************************/
static void begin()
{
static TCHAR RestoreDirPath[] = TEXT( "c:\\v5backup" );

static TCHAR RestoreTitle[]    = TEXT( "Data Archiver Restore" );
static TCHAR RestoreLinkName[] = TEXT( "Data Archiver Restore.lnk" );
static TCHAR RestoreExeFile[]  = TEXT( "dsrestor.exe" );

INI_CLASS  ini;
NAME_CLASS s;
TCHAR      sorc[MAX_PATH+1];
HWND       w;

DestExesDir.get_text( DestEb );

/*
----------------------
Copy the dsrestor file
---------------------- */
if ( copy_one_file(DestExesDir.text(), RestoreExeFile) )
    {
    copystring( sorc, DestExesDir.text() );
    append_filename_to_path( sorc, RestoreExeFile );
    Lb.add( TEXT("Creating a desktop link to the Data Archiver Restore Program") );
    create_desktop_link( RestoreTitle, RestoreLinkName, sorc );

    Lb.add( TEXT("Saving restore folder name in the Visitrak.ini file...") );
    ini.set_file( VisiTrakIniFile );
    ini.set_section( TEXT("Restore") );
    if ( !ini.find( TEXT("StartDir") ) )
        ini.put_string( TEXT("StartDir"),   RestoreDirPath );
    if ( !ini.find( TEXT("RestoreDir") ) )
        ini.put_string( TEXT("RestoreDir"), RestoreDirPath );
    }

Lb.add( TEXT("*** Finished ***") );

scroll_listbox_to_last_entry();
w = MainWindow.control( BEGIN_BUTTON );
set_text( w, TEXT("Exit") );
EnableWindow( w, TRUE );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static bool have_begun = false;
int id;
int cmd;
HWND begin_button;
HWND dest_static;
HWND w;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;

        dest_static = CreateWindow(
            TEXT("STATIC"),
            TEXT("Destination:"),
            WS_CHILD | BS_NOTIFY | BS_PUSHBUTTON | WS_VISIBLE,
            0, 0,
            100, 14,
            hWnd,
            (HMENU) DEST_DIR_STATIC,
            MainInstance,
            0
            );

        DestEb = CreateWindow(
            TEXT("EDIT"),
            TEXT(""),
            WS_CHILD | BS_NOTIFY | BS_PUSHBUTTON | WS_VISIBLE,
            101, 0,
            200, 14,
            hWnd,
            (HMENU) DEST_DIR_EB,
            MainInstance,
            0
            );

        DestExesDir.set_text( DestEb );

        begin_button = CreateWindow(
            TEXT("BUTTON"),
            TEXT("Begin"),
            WS_CHILD | BS_NOTIFY | BS_PUSHBUTTON | WS_VISIBLE,
            200, 470,
            100, 25,
            hWnd,
            (HMENU) BEGIN_BUTTON,
            MainInstance,
            0
            );

        w = CreateWindow(
            TEXT("LISTBOX"),
            0,
            WS_CHILD | LBS_NOSEL | WS_VISIBLE,
            3, 25,
            490, 450,
            hWnd,
            (HMENU) STATUS_LISTBOX,
            MainInstance,
            0
            );

        Lb.init( w );
        Lb.add( TEXT("Edit the above destination if necessary") );
        Lb.add( TEXT("Press 'Begin' to install the software") );
        SetFocus( begin_button );
        break;

    case WM_CHAR:
        if ( wParam == VK_ESCAPE )
            {
            MainWindow.close();
            return 0;
            }
        break;

    case WM_COMMAND:
        cmd  = HIWORD( wParam );
        switch (id)
            {
            case BEGIN_BUTTON:
                if ( cmd == BN_CLICKED )
                    {
                    if ( have_begun )
                        {
                        MainWindow.close();
                        }
                    else
                        {
                        have_begun = true;
                        EnableWindow( GetDlgItem(hWnd, BEGIN_BUTTON), FALSE );
                        MainWindow.set_focus();
                        begin();
                        }
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
STRING_CLASS title;
WNDCLASS wc;

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

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    505, 535,                         // W,h
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
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
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
