#include <windows.h>
#include "..\include\ugctrl.h"

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\dbclass.h"
#include "..\include\computer.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"
#include "extern.h"

static const TCHAR NullChar  = TEXT( '\0' );
static const TCHAR SpaceChar = TEXT( ' ' );
static TCHAR DefaultFontName[] = TEXT( "Arial" );
static HWND  MyWindow = 0;

static const int NOF_COLS = 3;
static const int LAST_COL = 2;

class EDITOP_GRID_CLASS : public CUGCtrl
{
private:
static HFONT my_default_font;

public:

EDITOP_GRID_CLASS();
~EDITOP_GRID_CLASS();

void    createfont();
void    OnSetup();
void    delete_highlighted_rows();
void    select_row( long row );
void    insert_row();
BOOLEAN is_row_selected( int row );
void    refresh_row_labels();
void    OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, BOOL processed );
void    OnCharDown( UINT *vcKey, int processed );
void    OnKeyDown( UINT *vcKey, BOOL processed);
int     OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag );
void    OnSH_LClicked( int col, long row, int updn, RECT *rect, POINT *point, BOOL processed );
};

HFONT EDITOP_GRID_CLASS::my_default_font = 0;

static EDITOP_GRID_CLASS Grid;
static int32 MaxChars;

bool resource_string( STRING_CLASS & dest, UINT resource_id );
void resource_message_box( UINT msg_id, UINT title_id );

/***********************************************************************
*                   SET_INPUT_ERROR_SETUP_STRINGS                      *
***********************************************************************/
static void set_input_error_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,           TEXT("OK_STRING") },
    { NO_MORE_STATIC, TEXT("NO_MORE_THAN") },
    { CHARS_STATIC,   TEXT("CHARACTERS") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    INPUT_ERROR_MAX_CHARS_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    StringTable.find( s, rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.refresh();
}

/***********************************************************************
*                          INPUT_ERROR_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK input_error_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_input_error_setup_strings( hWnd );
        set_text( hWnd, INPUT_ERROR_MAX_CHARS_TBOX, int32toasc(MaxChars) );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          EDIT0P_GRID_CLASS                           *
***********************************************************************/
void EDITOP_GRID_CLASS::createfont()
{
LOGFONT    lf;

lf.lfHeight          = 14;
lf.lfWidth           = 0;
lf.lfEscapement      = 0;
lf.lfOrientation     = 0;
lf.lfWeight          = FW_DONTCARE;
lf.lfItalic          = FALSE;
lf.lfUnderline       = FALSE;
lf.lfStrikeOut       = FALSE;
lf.lfCharSet         = ANSI_CHARSET;
lf.lfOutPrecision    = 0;
lf.lfClipPrecision   = 0;
lf.lfQuality         = DEFAULT_QUALITY;
lf.lfPitchAndFamily  = 0;
lstrcpy( lf.lfFaceName, DefaultFontName );

if (PRIMARYLANGID(GetUserDefaultLangID())==LANG_JAPANESE)
    {
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lstrcpy(lf.lfFaceName,TEXT("MS –¾’©"));
    }

my_default_font = CreateFontIndirect( &lf );
}

/***********************************************************************
*                          EDITOP_GRID_CLASS                           *
***********************************************************************/
EDITOP_GRID_CLASS::EDITOP_GRID_CLASS()
{
if ( my_default_font == 0 )
    createfont();
}

/***********************************************************************
*                         ~EDITOP_GRID_CLASS                           *
***********************************************************************/
EDITOP_GRID_CLASS::~EDITOP_GRID_CLASS()
{

if ( my_default_font != 0 )
    {
    DeleteObject( my_default_font );
    my_default_font = 0;
    }
}

/***********************************************************************
*                           EDITOP_GRID_CLASS                          *
*                              SELECT_ROW                              *
***********************************************************************/
void EDITOP_GRID_CLASS::select_row( long row )
{

if ( !IsSelected(0, row) || !IsSelected(LAST_COL, row) )
    SelectRange( 0, row, LAST_COL, row );
}

/***********************************************************************
*                          EDITOP_GRID_CLASS                           *
*                          REFRESH_ROW_LABELS                          *
***********************************************************************/
void EDITOP_GRID_CLASS::refresh_row_labels()
{
CUGCell  cell;
long row;
long nof_rows;

nof_rows = GetNumberRows();

GetCell( -1, 0, &cell );
for ( row=0; row<nof_rows; row++ )
    {
    cell.SetText( int32toasc(row+1) );
    SetCell( -1, row, &cell );
    }

}

/***********************************************************************
*                          EDITOP_GRID_CLASS                           *
*                             INSERT_ROW                               *
***********************************************************************/
void EDITOP_GRID_CLASS::insert_row()
{
CUGCell  cell;
long current_row;
long old_row;
int  col;

current_row = GetCurrentRow();
old_row     = current_row + 1;
Grid.InsertRow( current_row );

/*
---------------------
Duplicate the old row
--------------------- */
for ( col=0; col<NOF_COLS; col++ )
    {
    Grid.GetCell( col, old_row,     &cell );
    Grid.SetCell( col, current_row, &cell );
    }

refresh_row_labels();
}

/***********************************************************************
*                         EDITOP_GRID_CLASS                            *
*                          IS_ROW_SELECTED                             *
***********************************************************************/
BOOLEAN EDITOP_GRID_CLASS::is_row_selected( int row )
{
if ( IsSelected(0, row) && IsSelected(LAST_COL, row) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                       DELETE_HIGHLIGHTED_ROWS                        *
***********************************************************************/
void EDITOP_GRID_CLASS::delete_highlighted_rows()
{
long nof_rows;
long row;
BOOLEAN have_select;
BOOLEAN one_row_remains;

have_select     = FALSE;
one_row_remains = FALSE;
nof_rows = GetNumberRows();

for ( row = 0; row<nof_rows; row++ )
    {
    if ( is_row_selected(row) )
        have_select = TRUE;
    else
        one_row_remains = TRUE;
    }

if ( !have_select )
    {
    resource_message_box( SELECT_ROW_STRING, NOTHING_SELECTED_STRING );
    return;
    }

if ( !one_row_remains )
    {
    resource_message_box( NO_DEL_ALL_ROWS_STRING, ATTEMPT_DEL_ALL_ROWS_STRING );
    return;
    }

row = nof_rows-1;
while ( row >= 0 )
    {
    if ( is_row_selected(row) )
        DeleteRow( row );
    row--;
    }

ClearSelections();
refresh_row_labels();
RedrawAll();
SetFocus( Grid.m_ctrlWnd );
}


/***********************************************************************
*                          EDITOP_GRID_CLASS                           *
*                             OnEditFinish                             *
***********************************************************************/
int EDITOP_GRID_CLASS::OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag )
{
int32 n;

const int max_length[] = { OPERATOR_NUMBER_LEN, OPERATOR_FIRST_NAME_LEN, OPERATOR_LAST_NAME_LEN };

if ( !MyWindow )
    return TRUE;

n = lstrlen( string );

if ( n > max_length[col] )
    {
    MaxChars = max_length[col];
    DialogBox( MainInstance, TEXT("INPUT_ERROR_DIALOG"), MyWindow, (DLGPROC) input_error_dialog_proc );
    Grid.GotoCell( col, row );
    return FALSE;
    }

if ( n < 1 )
    {
    resource_message_box( FIELD_TOO_SHORT_STRING, CANT_DO_THAT_STRING );
    Grid.GotoCell( col, row );
    return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                          EDITOP_GRID_CLASS                           *
*                                OnSetup                               *
***********************************************************************/
void EDITOP_GRID_CLASS::OnSetup()
{
CUGCell cell;
static TCHAR mytext[] = TEXT( "0" );

SetNumberRows( 1 );
SetNumberCols( 1 );

GetCell(0,0,&cell);
if ( my_default_font )
    cell.SetFont( my_default_font );

cell.SetText( mytext );
SetCell(0,0,&cell);
SetMultiSelectMode(UG_MULTISELECT_ROW);
}

/***********************************************************************
*                          EDITOP_GRID_CLASS                           *
*                             OnLClicked                               *
***********************************************************************/
void EDITOP_GRID_CLASS::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed )
{

if( updn )
    StartEdit();

}

/***********************************************************************
*                            EDITOP_GRID_CLASS                         *
*                              OnSH_LClicked                           *
***********************************************************************/
void EDITOP_GRID_CLASS::OnSH_LClicked( int col, long row, int updn, RECT *rect, POINT *point, BOOL processed )
{
BOOLEAN shift_key_down   = FALSE;
BOOLEAN control_key_down = FALSE;

if ( updn )
    {
    if ( !is_row_selected(row) )
        {
        if ( GetKeyState(VK_CONTROL) < 0 )
            control_key_down = TRUE;

        if ( GetKeyState(VK_SHIFT) < 0 )
            shift_key_down = TRUE;

        if ( !shift_key_down && !control_key_down )
            ClearSelections();

        if ( shift_key_down )
            m_GI->m_multiSelect->EndBlock( LAST_COL, row );
        else
            select_row( row );

        Update();
        }
    }
}

/***********************************************************************
*                          EDITOP_GRID_CLASS                           *
*                              OnCharDown                              *
***********************************************************************/
void EDITOP_GRID_CLASS::OnCharDown( UINT *vcKey, int processed )
{
if( !processed )
    StartEdit( *vcKey );
}

/***********************************************************************
*                          EDITOP_GRID_CLASS                           *
*                              OnKeyDown                               *
***********************************************************************/
void EDITOP_GRID_CLASS::OnKeyDown( UINT *vcKey, BOOL processed )
{

if ( *vcKey == VK_INSERT )
    {
    insert_row();
    RedrawAll();
    }

}

/***********************************************************************
*                              LOAD_GRID                               *
***********************************************************************/
static void load_grid()
{
CUGCell    cell;
int32      n;
RECT       r;
int32      remaining_width;
NAME_CLASS s;
DB_TABLE   t;
int32      width;

s.get_operator_dbname();
if ( t.open(s, OPERATOR_RECLEN, PFL) )
    {
    n = t.nof_recs();
    if ( n < 1 )
        n = 1;

    Grid.GetCell( 0, 0, &cell );

    Grid.SetNumberCols( NOF_COLS, FALSE );

    resource_string( s, OPERATOR_ABREVIATION_STRING );
    cell.SetText( s.text() );
    Grid.SetCell( 0,-1, &cell );

    resource_string( s, OPERATOR_FIRST_NAME_STRING );
    cell.SetText( s.text() );
    Grid.SetCell( 1,-1, &cell );

    resource_string( s, OPERATOR_LAST_NAME_STRING );
    cell.SetText( s.text() );
    Grid.SetCell( 2,-1, &cell );

    Grid.SetNumberRows( n, FALSE );

    GetClientRect( Grid.m_CUGGrid->m_gridWnd, &r );
    remaining_width = r.right - GetSystemMetrics(SM_CXVSCROLL) - 1;
    width           = remaining_width / 3;

    for ( n=0; n<2; n++ )
        {
        Grid.SetColWidth( n, width);
        remaining_width -= width;
        }

    Grid.SetColWidth( 2, remaining_width);

    n = 0;
    while ( t.get_next_record(NO_LOCK) )
        {
        cell.SetText( int32toasc(n+1) );
        Grid.SetCell( -1, n, &cell );

        t.get_alpha( s, OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
        s.strip();
        cell.SetText( s.text() );
        Grid.SetCell( 0, n, &cell );

        t.get_alpha( s, OPERATOR_FIRST_NAME_OFFSET, OPERATOR_FIRST_NAME_LEN );
        cell.SetText( s.text() );
        Grid.SetCell( 1, n, &cell );

        t.get_alpha( s, OPERATOR_LAST_NAME_OFFSET,  OPERATOR_LAST_NAME_LEN );
        cell.SetText( s.text() );
        Grid.SetCell( 2, n, &cell );

        n++;
        }
    t.close();
    }
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
BOOLEAN save_changes()
{
CUGCell    cell;
AUTO_BACKUP_CLASS ab;
NAME_CLASS s;
DB_TABLE   t;
long       n;
long       row;
long       i;
long       j;
long       nsorted;
STRING_CLASS this_number_string;
struct OP_NUMBER_INDEX
    {
    long x;
    STRING_CLASS number_string;
    };

OP_NUMBER_INDEX * op;

this_number_string.init( OPERATOR_NUMBER_LEN );

n = Grid.GetNumberRows();
if ( n <= 0 )
    return FALSE;

/*
--------------------------------------------------
Make a sorted list of indexes and operator numbers
-------------------------------------------------- */
op = new OP_NUMBER_INDEX[n];
if ( !op )
    {
    resource_message_box( NO_MEM_FOR_SORT_STRING, CANNOT_COMPLY_STRING );
    return FALSE;
    }

for ( i=0; i<n; i++ )
    {
    op[i].x = NO_INDEX;
    op[i].number_string.init( OPERATOR_NUMBER_LEN );
    }

nsorted = 0;
for ( row=0; row<n; row++ )
    {
    if ( Grid.GetCell(0, row, &cell) == UG_SUCCESS )
        {
        this_number_string = cell.GetText();
        this_number_string.rjust( OPERATOR_NUMBER_LEN );
        for ( i=0; i<nsorted; i++ )
            {
            if ( op[i].number_string > this_number_string )
                break;
            }

        /*
        ------------------------------------------
        If I am inserting, move bigger guys up one
        ------------------------------------------ */
        if ( i < nsorted )
            {
            for ( j=nsorted; j>i; j-- )
                {
                op[j].x = op[j-1].x;
                op[j].number_string = op[j-1].number_string;
                }
            }

        op[i].x = row;
        op[i].number_string = this_number_string;

        nsorted++;
        }
    }

/*
---------------------------
Save using the sorted order
--------------------------- */
s.get_operator_dbname();
if ( t.open(s, OPERATOR_RECLEN, FL) )
    {
    t.empty();
    for ( i=0; i<nsorted; i++ )
        {
        row = op[i].x;

        if ( Grid.GetCell(0, row, &cell) == UG_SUCCESS )
            {
            this_number_string = cell.GetText();
            this_number_string.rjust( OPERATOR_NUMBER_LEN );
            t.put_alpha( OPERATOR_NUMBER_OFFSET, this_number_string, OPERATOR_NUMBER_LEN );
            }

        if ( Grid.GetCell(1, row, &cell) == UG_SUCCESS )
            t.put_alpha( OPERATOR_FIRST_NAME_OFFSET, cell.GetText(), OPERATOR_FIRST_NAME_LEN );

        if ( Grid.GetCell(2, row, &cell) == UG_SUCCESS )
            t.put_alpha( OPERATOR_LAST_NAME_OFFSET, cell.GetText(), OPERATOR_FIRST_NAME_LEN );

        t.rec_append();
        }

    t.close();
    }

delete[] op;
ab.get_backup_dir();
ab.backup_data_dir();
return TRUE;
}

/***********************************************************************
*                  SET_EDIT_OPERATORS_SETUP_STRINGS                    *
***********************************************************************/
static void set_edit_operators_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { EDITOP_SAVE_CHANGES_BUTTON, TEXT("SAVE_CHANGES") },
    { INSERT_OPERATOR_BUTTON,     TEXT("INSERT") },
    { DELETE_OPERATOR_BUTTON,     TEXT("DELETE_STRING") },
    { EDITOP_CANCEL_BUTTON,       TEXT("CANCEL") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    StringTable.find( s, rsn[i].s );
    s.set_text( w.handle() );
    }

StringTable.find( s, TEXT("EDIT_OPERATORS") );
s.set_text( hWnd );

w = hWnd;
w.refresh();
}

/***********************************************************************
*                        EDIT_OPERATORS_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK edit_operators_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MyWindow = hWnd;
        set_edit_operators_setup_strings( hWnd );
        Grid.AttachGrid( hWnd, GRID_POSITION_BORDER );
        load_grid();
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case EDITOP_CANCEL_BUTTON:
            case IDCANCEL:
                MyWindow = 0;
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;

            case INSERT_OPERATOR_BUTTON:
                Grid.insert_row();
                Grid.RedrawAll();
                return TRUE;

            case DELETE_OPERATOR_BUTTON:
                Grid.delete_highlighted_rows();
                Grid.RedrawAll();
                return TRUE;

            case EDITOP_SAVE_CHANGES_BUTTON:
                if ( save_changes() )
                    {
                    MyWindow = 0;
                    DialogIsActive = FALSE;
                    EndDialog( hWnd, 0 );
                    }
                return TRUE;

            }
        break;

    case WM_CLOSE:
        if ( MyWindow )
            {
            MyWindow = 0;
            DialogIsActive = FALSE;
            EndDialog( hWnd, 0 );
            }
        return TRUE;

    }

return FALSE;
}
