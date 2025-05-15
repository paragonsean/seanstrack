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
HWND         ShotTc;

static TCHAR MyClassName[]        = "Test";
static TCHAR CommonResourceFile[] = TEXT( "common" );
static TCHAR ConfigSection[]      = TEXT( "Config" );
static TCHAR CurrentLanguageKey[] = TEXT( "CurrentLanguage" );
static TCHAR DisplayIniFile[]     = TEXT( "display.ini" );
static TCHAR MyMenuFile[]         = TEXT( "chooseshotest.mnu" );
static TCHAR MyResourceFile[]     = TEXT( "chooseshotest" );

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

fill_parts_and_shot_count_listbox( BrouseWindow.handle(), CS_PA_LB, CurrentComputer, CurrentMachine );
}

/***********************************************************************
*                             FILL_SHOT_TC                             *
***********************************************************************/
static void fill_shot_tc_with_one_directory( HTREEITEM parent, STRING_CLASS & sorc )
{
TCHAR * cp;
STRING_CLASS s;
STRING_CLASS dest;
HANDLE fh;
WIN32_FIND_DATA fdata;
TV_INSERTSTRUCT tvi;
HTREEITEM       item_handle;
bool            is_dir;
bool            is_shot_file;

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
            is_shot_file = false;
            is_dir       = false;
            if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                is_dir = true;
                }
            else
                {
                /*
                ---------------------------
                See if this is a shot file.
                --------------------------- */
                while ( *cp != PeriodChar && *cp != NullChar )
                    cp++;
                if ( *cp != NullChar )
                    {
                    cp++;
                    if ( *cp == PChar || *cp == PCharLow )
                        {
                        cp++;
                        if ( *cp == RChar || *cp == RCharLow )
                            {
                            cp++;
                            if ( *cp == TwoChar || *cp == OChar || *cp == OCharLow )
                                is_shot_file = true;
                            }
                        }
                    }
                }
            if ( is_dir || is_shot_file )
                {
                tvi.item.pszText = fdata.cFileName;
                tvi.item.cchTextMax = lstrlen( fdata.cFileName );
                item_handle = TreeView_InsertItem( ShotTc, &tvi );
                if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    {
                    dest = sorc;
                    dest.cat_path( fdata.cFileName );
                    fill_shot_tc_with_one_directory( item_handle, dest );
                    }
                }
            }
        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );
    }
}

/***********************************************************************
*                          INIT_PART_LISTBOX                           *
***********************************************************************/
static void init_part_listbox()
{
LISTBOX_CLASS lb;

lb.init( BrouseWindow.handle(), CS_PA_LB );

lb.set_tabs( CS_PART_STATIC, CS_SHOTS_STATIC );
}

/***********************************************************************
*                             FILL_SHOT_TC                             *
***********************************************************************/
static void fill_shot_tc()
{
TCHAR    * cp;
NAME_CLASS s;
LISTBOX_CLASS lb;

lb.init( BrouseWindow.handle(), CS_PA_LB );
CurrentPart = lb.selected_text();
cp = CurrentPart.find( TabChar );
if ( cp )
    *cp = NullChar;
TreeView_DeleteAllItems( ShotTc );

s.get_part_results_directory( CurrentComputer, CurrentMachine, CurrentPart );

fill_shot_tc_with_one_directory( (HTREEITEM) TVI_ROOT, s );
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
        init_part_listbox();
        ShotTc = GetDlgItem( hWnd, CS_SHOT_TC );
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
                    fill_shot_tc();
                    }
                return TRUE;

            case CS_PA_LB:
                if ( cmd == LBN_SELCHANGE )
                    fill_shot_tc();
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
