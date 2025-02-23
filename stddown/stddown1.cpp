#include <windows.h>
#include "..\include\ugctrl.h"

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\textlen.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"
#include "..\include\v5help.h"

#include "..\editdown\category.h"
#include "resource.h"

#define _MAIN_
#include "..\include\chars.h"


HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;

STRING_CLASS CatString;

CATEGORY_CLASS Category;
SUBCAT_CLASS   SubCategory;

static TCHAR CommonResourceFile[]     = TEXT( "common" );
static TCHAR ConfigSection[]          = TEXT( "Config" );
static TCHAR CurrentLanguageKey[]     = TEXT( "CurrentLanguage" );
static TCHAR DisplayIniFile[]         = TEXT( "display.ini" );
static TCHAR StdDownResourceFile[]    = TEXT( "stddown" );

class GRID_CLASS : public CUGCtrl
{
private:
int     my_font_index;
void    createfont();
BOOLEAN is_editable();

public:

GRID_CLASS();
~GRID_CLASS();
void fill_grid();
void startup();
void save_changes();
void delete_row();
void insert_row();
void set_row_numbers();
void set_row_colors();
void make_droplist( int col, long row );
void remove_droplist( int col, long row );
void OnKeyDown( UINT *vcKey, BOOL processed );
void OnCharDown( UINT *vcKey, BOOL processed );
void OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed );
void OnCellChange( int oldcol, int newcol, long oldrow, long newrow );
int  OnEditContinue( int oldcol, long oldrow, int * newcol, long * newrow ) { return FALSE; }
};

GRID_CLASS   Grid;
const int32  NofCols  = 4;
bool         Updating = true;

TCHAR MyName[]      = TEXT( "StdDown" );
TCHAR CrString[]    = TEXT( "\n" );
TCHAR DefaultTimeString[] = TEXT( "00:00:00" );

FONT_LIST_CLASS   FontList;
STRINGTABLE_CLASS StringTable;

/***********************************************************************
*                               GRID_CLASS                             *
***********************************************************************/
GRID_CLASS::GRID_CLASS()
{
}

/***********************************************************************
*                              ~GRID_CLASS                             *
***********************************************************************/
GRID_CLASS::~GRID_CLASS()
{
RemoveFont( my_font_index );
}

/***********************************************************************
*                              GRID_CLASS                              *
*                              CREATEFONT                              *
***********************************************************************/
void GRID_CLASS::createfont()
{
FONT_CLASS lf;

lf = StringTable.fontclass();

ClearAllFonts();

my_font_index = AddFont(
    lf.lfHeight,
    lf.lfWidth,
    lf.lfEscapement,
    lf.lfOrientation,
    lf.lfWeight,
    lf.lfItalic,
    lf.lfUnderline,
    lf.lfStrikeOut,
    lf.lfCharSet,
    lf.lfOutPrecision,
    lf.lfClipPrecision,
    lf.lfQuality,
    lf.lfPitchAndFamily,
    lf.lfFaceName
    );

if ( SetDefFont(my_font_index) != UG_SUCCESS )
    MessageBox( 0, TEXT("SetDefFont"), TEXT("No Font at this index"), MB_OK );
}

/***********************************************************************
*                                GRID_CLASS                            *
*                                 Startup                              *
***********************************************************************/
void GRID_CLASS::startup()
{
CUGCell cell;
HDC     dc;
int     h;
HFONT   hf;
HFONT   oldfont;

static TCHAR mytext[] = TEXT( "0" );

createfont();

hf = StringTable.font();
dc = GetDC( m_ctrlWnd );
if ( dc != NULL )
    {
    oldfont = (HFONT) SelectObject( dc, hf );
    h = (int) character_height( dc );
    SelectObject( dc, oldfont );
    ReleaseDC( m_ctrlWnd, dc );
    }

h += 2;

SetDefRowHeight( h );

SetNumberCols( NofCols );
Set3DHeight( 0 );
fill_grid();
if ( GetNumberRows() > 0 )
    make_droplist( GetCurrentCol(), GetCurrentRow() );
}

/***********************************************************************
*                             IS_EDITABLE                              *
***********************************************************************/
BOOLEAN GRID_CLASS::is_editable()
{
int  col;

col = GetCurrentCol();

if ( col < 2 )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                              GRID_CLASS                              *
*                              OnCharDown                              *
***********************************************************************/
void GRID_CLASS::OnCharDown( UINT *vcKey, BOOL processed )
{
if ( (*vcKey) == VK_TAB )
    return;

if( !processed )
    if ( is_editable() )
        StartEdit( *vcKey );
}

/***********************************************************************
*                              GRID_CLASS                              *
*                              OnKeyDown                               *
***********************************************************************/
void GRID_CLASS::OnKeyDown( UINT *vcKey, BOOL processed )
{
if ( (*vcKey) == VK_INSERT   )
    {
    insert_row();
    SetFocus( m_ctrlWnd );
    }

if ( (*vcKey) == VK_DELETE   )
    {
    delete_row();
    SetFocus( m_ctrlWnd );
    }
}

/***********************************************************************
*                              GRID_CLASS                              *
*                            make_droplist                             *
***********************************************************************/
void GRID_CLASS::make_droplist( int col, long row )
{
STRING_CLASS cat;
STRING_CLASS s;
CUGCell cell;
bool firstime;

if ( col > 1 )
    return;

GetCell( col, row, &cell );

if ( cell.GetCellType() == UGCT_DROPLIST )
    return;

if ( col == 1 )
    {
    cat = QuickGetText( 0, row );

    if ( Category.findname(cat.text()) )
        {
        cat = Category.cat();
        /*
        -----------------------------------------------------------------
        Make a string containing all the sub categories for this cagegory
        ----------------------------------------------------------------- */
        firstime = true;
        SubCategory.rewind();
        s.empty();
        while ( SubCategory.next() )
            {
            if ( cat == SubCategory.cat() )
                {
                if ( firstime )
                    firstime = false;
                else
                    s += CrString;

                s += SubCategory.name();
                }
            }
        cat = s;
        }
    else
        {
        cat = TEXT( "?" );
        }
    }
else
    {
    cat = CatString;
    }

s = cell.GetText();
cell.SetCellType( UGCT_DROPLIST );
cell.SetText( s.text() );
cell.SetLabelText( cat.text() );
SetCell( col, row, &cell );
}

/***********************************************************************
*                              GRID_CLASS                              *
*                            remove_droplist                           *
***********************************************************************/
void GRID_CLASS::remove_droplist( int col, long row )
{
STRING_CLASS s;
CUGCell cell;

GetCell( col, row, &cell );
if ( cell.GetCellType() == UGCT_DROPLIST )
    {
    s = cell.GetText();
    cell.SetLabelText( EmptyString );
    cell.SetCellType( UGCT_NORMAL );
    cell.SetText( s.text() );
    SetCell( col, row, &cell );
    }
}

/***********************************************************************
*                              GRID_CLASS                              *
*                              OnCellChange                            *
***********************************************************************/
void GRID_CLASS::OnCellChange( int oldcol, int newcol, long oldrow, long newrow )
{
if ( !Updating )
    {
    if ( oldcol == 0 || oldcol == 1 )
        remove_droplist( oldcol, oldrow );

    if ( newcol < 2 )
        make_droplist( newcol, newrow );
    }
}

/***********************************************************************
*                              GRID_CLASS                              *
*                              OnLClicked                              *
***********************************************************************/
void GRID_CLASS::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed )
{
CUGCell cell;
STRING_CLASS s;

if( updn && is_editable() )
    StartEdit();
}

/***********************************************************************
*                           SET_ROW_NUMBERS                            *
***********************************************************************/
void GRID_CLASS::set_row_numbers()
{
long row;
long n;
n = GetNumberRows();
for ( row=0; row<n; row++ )
    QuickSetText( -1, row, int32toasc(row) );
}

/***********************************************************************
*                            SET_ROW_COLORS                            *
***********************************************************************/
void GRID_CLASS::set_row_colors()
{
int  col;
long row;
long n;
COLORREF color;

n = GetNumberRows();
for ( row=0; row<n; row++ )
    {
    if ( (row/2)*2 == row )
        color = RGB( 252, 240, 190 );
    else
        color = RGB( 255, 255, 255 );

    for ( col=0; col<NofCols; col++ )
        QuickSetBackColor( col, row, color );
    }
}

/***********************************************************************
*                              GRID_CLASS                              *
*                             save_changes                             *
***********************************************************************/
void GRID_CLASS::save_changes()
{
long     n;
long     row;

NAME_CLASS   s;
DB_TABLE     t;
STRING_CLASS cat;
STRING_CLASS sub;

n = GetNumberRows();
s.get_stddown_dbname();
if ( t.open(s, STDDOWN_RECLEN, FL) )
    {
    t.empty();
    for ( row=0; row<n; row++ )
        {
        cat = QuickGetText( 0, row );
        if ( Category.findname(cat.text()) )
            {
            cat = Category.cat();
            t.put_alpha( STDDOWN_CAT_NUMBER_OFFSET, cat.text(), DOWNCAT_NUMBER_LEN );
            sub = QuickGetText( 1, row );

            if ( SubCategory.findname(cat.text(), sub.text()) )
                {
                sub = SubCategory.subcat();
                t.put_alpha( STDDOWN_SUB_NUMBER_OFFSET, sub.text(), DOWNCAT_NUMBER_LEN );
                t.put_alpha( STDDOWN_START_TIME_OFFSET, QuickGetText(2,row), ALPHATIME_LEN );
                t.put_alpha( STDDOWN_END_TIME_OFFSET, QuickGetText(3,row), ALPHATIME_LEN );
                t.rec_append();
                }
            }
        }
    t.close();
    }
}

/***********************************************************************
*                              GRID_CLASS                              *
*                               FILL_GRID                              *
***********************************************************************/
void GRID_CLASS::fill_grid()
{
HDC     dc;
int     col;
long    row;
long    n;
int     maxwide;
int     w;
int     id;
HFONT   hf;
HFONT   oldfont;
RECT    r;
CUGCell cell;
DB_TABLE t;
BOOLEAN  status;
TCHAR    buf[ALPHATIME_LEN];
TCHAR    cat[DOWNCAT_NUMBER_LEN+1];
TCHAR    sub[DOWNCAT_NUMBER_LEN+1];
TCHAR    name[DOWNCAT_NAME_LEN+1];
TCHAR  * cp;
TEXT_LEN_CLASS tmax;
NAME_CLASS     s;

TCHAR *column_name[] = {
    { TEXT("CATEGORY") },
    { TEXT("SUB_CAT") },
    { TEXT("START_TIME") },
    { TEXT("END_TIME") }
    };
int nof_col_names = 4;

Grid.GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );

status = FALSE;

s.get_stddown_dbname();

if ( !s.file_exists() )
    t.create( s );

if ( t.open(s, STDDOWN_RECLEN, PFL) )
    {
    n = t.nof_recs();
    if ( n > 0 )
        {
        SetNumberRows( n, FALSE );

        row = 0;
        while( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( cat, STDDOWN_CAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );

            if ( Category.find(cat) )
                lstrcpy( name, Category.name() );
            else
                lstrcpy( name, EmptyString );

            cell.SetText( name );
            SetCell( 0, row, &cell );

            t.get_alpha( sub, STDDOWN_SUB_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );

            if ( SubCategory.find(cat, sub) )
                lstrcpy( name, SubCategory.name() );
            else
                lstrcpy( name, EmptyString );

            cell.SetText( name );
            SetCell( 1, row, &cell );

            t.get_alpha( buf, STDDOWN_START_TIME_OFFSET, ALPHATIME_LEN );
            cell.SetText( buf );
            SetCell( 2, row, &cell );

            t.get_alpha( buf, STDDOWN_END_TIME_OFFSET, ALPHATIME_LEN );
            cell.SetText( buf );
            SetCell( 3, row, &cell );

            row++;
            if ( row >= n )
                break;
            }
        }
    t.close();
    }

if ( GetNumberRows() > 0 )
    set_row_colors();

cell.SetText( EmptyString );

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN | UG_BDR_LTHIN );
SetCell( -1, -1, &cell );

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN );
id = 0;
for ( col=0; col<NofCols; col++ )
    {
    cp = StringTable.find( column_name[id] );
    //resource_message_box( cp, "Column Name" );
    cell.SetText( cp );
    id++;
    SetCell( col, -1, &cell );
    }

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_LTHIN );
n = GetNumberRows();
for ( row=0; row<n; row++ )
    {
    cell.SetText( int32toasc(row) );
    SetCell( -1, row, &cell );
    }

dc = GetDC( m_ctrlWnd );
if ( dc != NULL )
    {
    oldfont = (HFONT) SelectObject( dc, hf );
    maxwide = pixel_width( dc, TEXT("000") );
    tmax.init( dc );
    id = 2;
    tmax.check( StringTable.find(column_name[id]) );
    id++;
    tmax.check( StringTable.find(column_name[id]) );
    tmax.check( TEXT( "00:00:00") );

    SelectObject( dc, oldfont );
    ReleaseDC( m_ctrlWnd, dc );

    w = tmax.width();
    w += 2;
    SetColWidth( 2, w );
    SetColWidth( 3, w );

    if ( maxwide > 0 )
        {
        maxwide += 2;
        SetSH_Width( maxwide );
        }

    GetWindowRect( Grid.m_ctrlWnd, &r );
    r.right -= GetVS_Width();
    r.left  += maxwide;
    w *= 2;
    w = r.right - r.left - w - 1;
    if ( w > 0 )
        {
        w /= 2;
        for ( col=0; col<2; col++ )
            SetColWidth( col, w );
        }

    AdjustComponentSizes();
    }

Updating = false;
}
/***********************************************************************
*                              DELETE_ROW                              *
***********************************************************************/
void GRID_CLASS::delete_row()
{
if ( GetNumberRows() > 0 )
    DeleteRow( GetCurrentRow() );

if ( GetNumberRows() > 0 )
    {
    set_row_colors();
    set_row_numbers();
    }
RedrawAll();
}

/***********************************************************************
*                              INSERT_ROW                              *
***********************************************************************/
void GRID_CLASS::insert_row()
{
CUGCell cell;
int  col;
long n;
long row;
STRING_CLASS s[NofCols];

n = GetNumberRows();

if ( n > 0 )
    {
    row = GetCurrentRow();
    col = GetCurrentCol();
    if ( col < 2 )
        remove_droplist( col, row );

    for ( col=0; col<NofCols; col++ )
        s[col] = QuickGetText( col, row );
    }
else
    {
    row = 0;
    s[2] = DefaultTimeString;
    s[3] = DefaultTimeString;
    }

InsertRow( row );

GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );

for ( col=0; col<NofCols; col++ )
    {
    cell.SetText( s[col].text() );
    SetCell( col, row, &cell );
    }

set_row_colors();
set_row_numbers();

col = GetCurrentCol();
if ( col < 2 )
    make_droplist( col, row );

RedrawAll();
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( UINT helptype, DWORD context )
{
NAME_CLASS s;

if ( s.get_exe_dir_file_name(TEXT("v5help.hlp")) )
    WinHelp( MainWindow.handle(), s.text(), helptype, context );
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
    case WM_COMMAND:
        switch (id)
            {
            case SAVE_CHANGES:
                Grid.save_changes();
                return 0;

            case SAVE_AND_EXIT:
                Grid.save_changes();

            case EXIT_WO_SAVE:
                MainWindow.close();
                return 0;

            case INSERT_CHOICE:
                Grid.insert_row();
                SetFocus( Grid.m_ctrlWnd );
                return 0;

            case DELETE_CHOICE:
                Grid.delete_row();
                SetFocus( Grid.m_ctrlWnd );
                return 0;
            }
        break;

    case WM_DBINIT:
        return 0;

    case WM_HELP:
        gethelp( HELP_CONTEXT, STANDARD_DOWNTIMES_HELP );
        return 0;

    case WM_CREATE:
        MainWindow = hWnd;
        RECT r;
        GetClientRect(hWnd,&r);
        Grid.CreateGrid( WS_CHILD|WS_VISIBLE, r, hWnd, GRID_ID );
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                            LOAD_CATSTRING                            *
***********************************************************************/
static void load_catstring()
{
static bool firstime = true;

Category.rewind();
while ( Category.next() )
    {
    if ( firstime )
        firstime = false;
    else
        CatString += CrString;
    CatString += Category.name();
    }
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
s.get_stringtable_name( StdDownResourceFile );
StringTable.read( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup()
{
WNDCLASS wc;
STRING_CLASS title;
COMPUTER_CLASS c;

c.startup();
load_resources();
statics_startup();

Category.read();
load_catstring();
SubCategory.read();

Grid.startup();

wc.lpszClassName = MyName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MyName );
wc.lpszMenuName  = TEXT("MainMenu");
wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

title = StringTable.find( TEXT("STD_DOWNTIME_SETUP") );

RegisterClass( &wc );

CreateWindowEx(
    WS_EX_APPWINDOW,
    MyName,
    title.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    450, 500,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainWindow.show( SW_SHOW );

UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
statics_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

startup();

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
return(msg.wParam);
}
