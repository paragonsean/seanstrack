#include <windows.h>
#include <commctrl.h>
#include <htmlhelp.h>
#include <winnetwk.h>   /* link to mpr.lib */
#include <shlobj.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\rectclas.h"
#include "..\include\strarray.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "msres.h"

#define _MAIN_
#include "..\include\chars.h"

HINSTANCE     MainInstance;
WINDOW_CLASS  MainWindow;
WINDOW_CLASS  MainDialog;
LISTBOX_CLASS StatusLb;
int           ShotsToSave = 0;

static TCHAR MyClassName[]    = TEXT( "CopyV5" );
static TCHAR MyIniFileName[]  = TEXT( "copyv5.ini" );
static TCHAR ConfigSection[]  = TEXT( "Config" );
static TCHAR DestDirKey[]     = TEXT( "DestDir" );
static TCHAR SorcDirKey[]     = TEXT( "SorcDir" );
static TCHAR ShotsToSaveKey[] = TEXT( "ShotsToSave" );

/**********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                              GET_DEFAULTS                            *
***********************************************************************/
static void get_defaults()
{
NAME_CLASS s;
INI_CLASS  ini;

s = exe_directory();
s.cat_path( MyIniFileName );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( ini.find(SorcDirKey) )
    {
    s = ini.get_string();
    s.set_text( MainDialog.handle(), SORC_DIR_EB );
    }

if ( ini.find(DestDirKey) )
    {
    s = ini.get_string();
    s.set_text( MainDialog.handle(), DEST_DIR_EB );
    }

if ( ini.find(ShotsToSaveKey) )
    {
    s = ini.get_string();
    s.set_text( MainDialog.handle(), SHOTS_TO_SAVE_EB );
    }
}

/***********************************************************************
*                             SAVE_DEFAULTS                            *
***********************************************************************/
static void save_defaults()
{
NAME_CLASS s;
INI_CLASS  ini;

s = exe_directory();
s.cat_path( MyIniFileName );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

s.get_text( MainDialog.handle(), SORC_DIR_EB );
if ( !s.isempty() )
    ini.put_string( SorcDirKey, s.text() );

s.get_text( MainDialog.handle(), DEST_DIR_EB );
if ( !s.isempty() )
    ini.put_string( DestDirKey, s.text() );

s.get_text( MainDialog.handle(), SHOTS_TO_SAVE_EB );
if ( !s.isempty() )
    ini.put_string( ShotsToSaveKey, s.text() );
}

/***********************************************************************
*                            BROWSE_FOR_DEST                           *
***********************************************************************/
static void browse_for_dest( HWND w )
{
TCHAR        buf[MAX_PATH];
BROWSEINFO   bi;
ITEMIDLIST * pidlBrowse;
ITEMIDLIST * network_neighborhood;
ITEMIDLIST ** np;
LPMALLOC     ip;

/*
--------------------------------------------------------------------
If this fails it will set the network neighborhood to NULL, which is
what I want.
-------------------------------------------------------------------- */
np = &network_neighborhood;
SHGetSpecialFolderLocation( NULL, CSIDL_DRIVES, np );

bi.hwndOwner      = w;
bi.pidlRoot       = network_neighborhood;
bi.pszDisplayName = buf;
bi.lpszTitle      = TEXT("Select the destination folder that you wish to copy to");
bi.ulFlags        = BIF_USENEWUI;       // Use new ui with create dir button (shlobj.h 0x0040)
bi.lpfn           = 0;
bi.lParam         = 0;
bi.iImage         = 0;

/*
-----------------------------------------------------------------------------
I don't actually use the returned choice. I do each v5 folder as it is chosen
but I need to delete the itemidlist returned by SHBrowseForFolder.
----------------------------------------------------------------------------- */
if ( SUCCEEDED(SHGetMalloc(&ip)) )
    {
    pidlBrowse = SHBrowseForFolder( &bi );
    if ( pidlBrowse != NULL )
        {
        if ( SHGetPathFromIDList(pidlBrowse, buf) )
            set_text( w, DEST_DIR_EB, buf );
        ip->Free( pidlBrowse );
        }

    if ( network_neighborhood != NULL )
        ip->Free( network_neighborhood );

    ip->Release();
    }
}

/***********************************************************************
*                            BROWSE_FOR_SORC                           *
***********************************************************************/
static void browse_for_sorc( HWND w )
{
TCHAR        buf[MAX_PATH];
BROWSEINFO   bi;
ITEMIDLIST * pidlBrowse;
ITEMIDLIST * network_neighborhood;
ITEMIDLIST ** np;
LPMALLOC     ip;

/*
--------------------------------------------------------------------
If this fails it will set the network neighborhood to NULL, which is
what I want.
-------------------------------------------------------------------- */
np = &network_neighborhood;
SHGetSpecialFolderLocation( NULL, CSIDL_DRIVES, np );

bi.hwndOwner      = w;
bi.pidlRoot       = network_neighborhood;
bi.pszDisplayName = buf;
bi.lpszTitle      = TEXT("Select the V5 folder that you wish to copy");
bi.ulFlags        = 0;
bi.lpfn           = 0;
bi.lParam         = 0;
bi.iImage         = 0;

/*
-----------------------------------------------------------------------------
I don't actually use the returned choice. I do each v5 folder as it is chosen
but I need to delete the itemidlist returned by SHBrowseForFolder.
----------------------------------------------------------------------------- */
if ( SUCCEEDED(SHGetMalloc(&ip)) )
    {
    pidlBrowse = SHBrowseForFolder( &bi );
    if ( pidlBrowse != NULL )
        {
        if ( SHGetPathFromIDList(pidlBrowse, buf) )
            set_text( w, SORC_DIR_EB, buf );
        ip->Free( pidlBrowse );
        }
    if ( network_neighborhood != NULL )
        ip->Free( network_neighborhood );

    ip->Release();
    }
}

/***********************************************************************
*                           COPY_RESULTS_DIR                           *
* The shotparm copy in this routine are for v6 results only and        *
* don't need to be updated.                                            *
***********************************************************************/
BOOLEAN copy_results_dir( STRING_CLASS & destdir, STRING_CLASS & sorcdir )
{
NAME_CLASS   d;
NAME_CLASS   s;
STRING_CLASS shot_name;
DB_TABLE     td;
DB_TABLE     t;
int          slen;
DWORD        n;

/*
----------------------------------------------------
Return FALSE if this is not a part results directory
---------------------------------------------------- */
s = sorcdir;
s.cat_path( SHOTPARM_DB );
if ( !s.file_exists() )
    return FALSE;

/*
---------------------
Copy the master trace
--------------------- */
s = sorcdir;
s.cat_path( CURRENT_MASTER_TRACE_NAME );
s += MASTER_TRACE_SUFFIX;

if ( !s.file_exists() )
    s.replace( MASTER_TRACE_SUFFIX, MASTER_TRACE_II_SUFFIX );
if ( s.file_exists() )
    {
    d = s;
    d.replace( sorcdir.text(), destdir.text() );
    s.copyto( d );
    }

/*
-------------------------
Copy the alarm summary db
------------------------- */
s = sorcdir;
d = destdir;
s.cat_path( ALARMSUM_DB );
d.cat_path( ALARMSUM_DB );
if ( s.file_exists() )
    s.copyto( d );
else
    td.ensure_existance( d );

/*
---------------------------------------------------------
Copy the last ShotsToSave parameters from the SHOTPARM db
--------------------------------------------------------- */
s = sorcdir;
d = destdir;
s.cat_path( SHOTPARM_DB );
d.cat_path( SHOTPARM_DB );

td.ensure_existance( d );
if ( ShotsToSave > 0 )
    {
    if ( s.file_exists() )
        {
        if ( t.open(s, SHOTPARM_RECLEN, PFL) )
            {
            n = t.nof_recs();
            if ( n > (DWORD) ShotsToSave )
                {
                n -= (DWORD) ShotsToSave;
                n--;
                t.goto_record_number( n );
                t.get_current_record( FALSE );
                td.open( d, SHOTPARM_RECLEN, WL  );
                while ( t.get_next_record(FALSE) )
                    {
                    td.copy_rec( t );
                    td.rec_append();
                    }
                td.close();
                }
            else
                {
                s.copyto( d );
                }
            t.close();
            }
        }
    }

/*
---------------------------------------------------
Copy the last ShotsToSave shots and the graphlst db
--------------------------------------------------- */
s = sorcdir;
d = destdir;
s.cat_path( GRAPHLST_DB );
d.cat_path( GRAPHLST_DB );

td.ensure_existance( d );
if ( ShotsToSave > 0 )
    {
    slen = 0;
    if ( s.file_exists() )
        {
        if ( t.open(s, GRAPHLST_RECLEN, PFL) )
            {
            n = t.nof_recs();
            if ( n > (DWORD) ShotsToSave )
                {
                n -= (DWORD) ShotsToSave;
                n--;
                t.goto_record_number( n );
                t.get_current_record( FALSE );
                td.open( d, GRAPHLST_RECLEN, WL  );
                while ( t.get_next_record(FALSE) )
                    {
                    td.copy_rec( t );
                    td.rec_append();
                    }
                td.close();
                }
            else
                {
                s.copyto( d );
                }
            t.close();
            }
        }
    }
return TRUE;
}

/***********************************************************************
*                               COPYDIR                                *
***********************************************************************/
static void copydir( STRING_CLASS & destdir, STRING_CLASS & sorcdir )
{
static TCHAR resultstring[] = TEXT( "RESULTS" );
static TCHAR backupstring[] = TEXT( "BACKUP" );

TEXT_LIST_CLASS t;
NAME_CLASS      sorc;
NAME_CLASS      dest;
BOOLEAN         have_copy;
BOOLEAN         copy_this_dir;
int             rows_in_status_listbox;
int             i;

rows_in_status_listbox = StatusLb.visible_row_count();

t.get_directory_list( sorcdir.text() );
t.rewind();

while ( t.next() )
    {
    sorc = sorcdir;
    sorc.cat_path( t.text() );

    dest = destdir;
    dest.cat_path( t.text() );

    i = (int) StatusLb.add( sorc.text() );
    if ( i > rows_in_status_listbox )
        StatusLb.scroll_to_end();
    dest.create_directory();

    have_copy = FALSE;
    copy_this_dir = TRUE;

    if ( strings_are_equal(t.text(), resultstring) )
        {
        have_copy = copy_results_dir( dest, sorc );
        }
    else if ( strings_are_equal(t.text(), backupstring) )
        {
        have_copy = TRUE;  // Don't copy the backup directory
        copy_this_dir = FALSE;
        }
    if ( !have_copy )
        copy_all_files( dest.text(), sorc.text(), StarDotStar, WITHOUT_SUBS, 0, 0 );

    if ( copy_this_dir )
        copydir( dest, sorc );
    }
}

/***********************************************************************
*                             BEGIN_BACKUP                             *
***********************************************************************/
static void begin_backup()
{
int          i;
STRING_CLASS droot;
STRING_CLASS sroot;

sroot.get_text( MainDialog.control(SHOTS_TO_SAVE_EB) );
ShotsToSave = sroot.int_value();
if ( ShotsToSave < 0 )
    ShotsToSave = 0;

sroot.get_text( MainDialog.control(SORC_DIR_EB) );
droot.get_text( MainDialog.control( DEST_DIR_EB) );
if ( sroot.isempty() )
    {
    MessageBox( MainDialog.handle(), TEXT("No Source Directory"), TEXT("Cannot Comply"), MB_OK );
    }
else if ( droot.isempty() )
    {
    MessageBox( MainDialog.handle(), TEXT("No Destination Directory"), TEXT("Cannot Comply"), MB_OK );
    }
else
    {
    StatusLb.add( TEXT("Begin Copying...") );
    copydir( droot, sroot );
    i = (int) StatusLb.add( TEXT("*** Copy Complete ***") );
    if ( i > StatusLb.visible_row_count() )
        StatusLb.scroll_to_end();
    }
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MainDialog = w;
        MainWindow.shrinkwrap( w );
        StatusLb.init( w, STATUS_LB );
        get_defaults();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case BEGIN_PB:
                begin_backup();
                return TRUE;

            case BROWSE_FOR_DEST_PB:
                browse_for_dest(w);
                return TRUE;
            case BROWSE_FOR_SORC_PB:
                browse_for_sorc(w);
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
static RECTANGLE_CLASS r;

switch (msg)
    {
    case WM_CREATE:
        MainWindow = hWnd;
        break;

    case WM_CLOSE:
        save_defaults();
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

CoInitialize(0);

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("COPYV5_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title   = resource_string( MAIN_WINDOW_TITLE_STRING );

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
    TEXT("MAIN_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) main_dialog_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
CoUninitialize();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL status;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialog.handle() )
        status = IsDialogMessage( MainDialog.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
