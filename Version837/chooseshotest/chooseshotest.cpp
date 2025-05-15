#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\rectclas.h"
#include "..\include\strarray.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\wclass.h"

#define _MAIN_
#include "..\include\chars.h"
#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS BrouseWindow;
HWND         DirTc;

static TCHAR MyClassName[]        = "Test";
static TCHAR CommonResourceFile[] = TEXT( "common" );
static TCHAR ConfigSection[]      = TEXT( "Config" );
static TCHAR CurrentLanguageKey[] = TEXT( "CurrentLanguage" );
static TCHAR DisplayIniFile[]     = TEXT( "display.ini" );
static TCHAR MyMenuFile[]         = TEXT( "chooseshotest.mnu" );
static TCHAR MyResourceFile[]     = TEXT( "chooseshotest" );
static TCHAR RealtimeDirName[]    = TEXT( "Current" );

int CurrentPasswordLevel = 0;
STRINGTABLE_CLASS  StringTable;
MACHINE_NAME_LIST_CLASS MachList;

STRING_CLASS CurrentComputer;
STRING_CLASS CurrentMachine;
STRING_CLASS CurrentPart;

HFONT MyFont = INVALID_FONT_HANDLE;
FONT_LIST_CLASS FontList;

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                         FILL_MACHINE_LISTBOX                         *
***********************************************************************/
static void fill_machine_listbox()
{
LISTBOX_CLASS lb;

hourglass_cursor();

lb.init( BrouseWindow.handle(), CS_MA_LB );
lb.empty();
lb.redraw_off();

MachList.rewind();
while ( MachList.next() )
    lb.add( MachList.name().text() );

lb.redraw_on();

restore_cursor();
}

/***********************************************************************
*                           FILL_PART_LISTBOX                          *
***********************************************************************/
static void fill_part_listbox()
{
LISTBOX_CLASS lb;

lb.init( BrouseWindow.handle(), CS_MA_LB );
CurrentMachine = lb.selected_text();

fill_part_listbox( BrouseWindow.handle(), CS_PA_LB, CurrentComputer, CurrentMachine );
}

/***********************************************************************
*                   FILL_DIR_TC_WITH_ONE_DIRECTORY                     *
***********************************************************************/
static void fill_dir_tc_with_one_directory( HTREEITEM parent, STRING_CLASS & sorc )
{
BOOLEAN i_am_a_number;
BOOLEAN sa_is_a_number;
int     i;
int     sai;
bool    saved;
TCHAR * cp;
STRING_CLASS s;
STRING_CLASS dest;
STRING_ARRAY_CLASS sa;
HANDLE fh;
WIN32_FIND_DATA fdata;
TV_INSERTSTRUCT tvi;
HTREEITEM       item_handle;

tvi.hParent             = parent;
tvi.hInsertAfter        = TVI_LAST;
tvi.item.mask           = TVIF_TEXT;
tvi.item.hItem          = 0;
tvi.item.state          = 0;
tvi.item.stateMask      = 0;
tvi.item.pszText        = 0;
tvi.item.cchTextMax     = 0;
tvi.item.iImage         = 0;
tvi.item.iSelectedImage = 0;
tvi.item.cChildren      = 0;
tvi.item.lParam         = 0;

s = sorc;
s.cat_path( StarDotStar );

fh = FindFirstFile( s.text(), &fdata );
if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( true )
        {
        cp = fdata.cFileName;
        if ( *cp != PeriodChar )
            {
            if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                saved = false;
                sa.rewind();
                if ( strings_are_equal( cp, RealtimeDirName )
                    {
                    sa.insert( cp );
                    saved = true;
                    }
                else
                    {
                    i = 0;
                    i_am_a_number = is_numeric( cp );
                    if ( i_am_a_number )
                        {
                        i = (int) ascii_to_int32( cp );
                        if ( i < 1 || i > 2200 )
                            i_am_a_number = FALSE;
                        }

                    while ( sa.next() )
                        {
                        sa_is_a_number = sa.is_numeric();
                        if ( sa_is_a_number )
                            {
                            sai = (int) ascii_to_int32( cp );
                            if ( sai < 1 || sai > 2200 )
                                sa_is_a_number = FALSE;
                            }

                        if ( i_am_a_number )
                            {
                            if ( sa_is_a_number )
                                {
                                if ( i>1900 && i<2200 && i > sai )
                                    {
                                    sa.insert( cp );
                                    saved = true;
                                    break
                                    }

                                if ( i < sai )
                                    {
                                    sa.insert( cp );
                                    saved = true;
                                    break
                                    }
                                }
                            }
                        else
                            {
                            /*
                            ----------
                            i am alpha
                            ---------- */
                            if ( sa_is_a_number )
                                {
                                sa.insert( cp );
                                saved = true;
                                break
                                }
                            else if ( compare( cp, sa.text()) < 0 )
                                {
                                sa.insert( cp );
                                saved = true;
                                break;
                                }
                            }
                        }
                    if ( !saved )
                        sa.add( cp );
                    }
                }
            }
        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );

                tvi.item.pszText = fdata.cFileName;
                tvi.item.cchTextMax = lstrlen( fdata.cFileName );
                item_handle = TreeView_InsertItem( DirTc, &tvi );
                dest = sorc;
                dest.cat_path( fdata.cFileName );
                fill_dir_tc_with_one_directory( item_handle, dest );
                }
    }
}

/***********************************************************************
*                             FILL_DIR_TC                              *
***********************************************************************/
static void fill_dir_tc()
{
TCHAR    * cp;
NAME_CLASS s;
LISTBOX_CLASS lb;

lb.init( BrouseWindow.handle(), CS_PA_LB );
CurrentPart = lb.selected_text();
cp = CurrentPart.find( TabChar );
if ( cp )
    *cp = NullChar;
TreeView_DeleteAllItems( DirTc );

s.get_part_results_directory( CurrentComputer, CurrentMachine, CurrentPart );

fill_dir_tc_with_one_directory( (HTREEITEM) TVI_ROOT, s );
}

/***********************************************************************
*                        CHOOSE_SHOTS_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK choose_shots_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {

    case WM_DBINIT:
        fill_machine_listbox();
        return TRUE;

    case WM_INITDIALOG:
        BrouseWindow = hWnd;
        DirTc = GetDlgItem( hWnd, CS_DIR_TC );
        MainWindow.shrinkwrap( hWnd );
        BrouseWindow.post( WM_DBINIT );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CS_MA_LB:
                if ( cmd == LBN_SELCHANGE )
                    {
                    fill_part_listbox();
                    fill_dir_tc();
                    }
                return TRUE;

            case CS_PA_LB:
                if ( cmd == LBN_SELCHANGE )
                    fill_dir_tc();
                return TRUE;

            case IDOK:
            case IDCANCEL:
                MainWindow.close();
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
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        break;

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
    s = ini.get_string();
    StringTable.set_language( s );

s.get_stringtable_name( MyResourceFile );
StringTable.read( s );

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

c.startup();
CurrentComputer = c.whoami();
MachList.add_computer( CurrentComputer );

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

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    605, 300,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT( "CHOOSE_SHOTS_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) choose_shots_dialog_proc );

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
