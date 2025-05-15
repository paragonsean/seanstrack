#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\fontclas.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\structs.h"
#include "..\include\param_index_class.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "extern.h"
#include "resource.h"

/*
------------------
Main menu controls
------------------ */
#define VIEW_PARAMETERS_MENU   507

/*
-----------
My controls
----------- */
#define PARAMETER_LISTBOX  101

#define MY_FLOAT_LEN 9
#define MAXLINE     100

const TCHAR NullChar = TEXT('\0');
const TCHAR TabChar  = TEXT('\11');

extern HWND MainWindow;
extern HWND MainPlotWindow;
extern HWND ViewParametersDialogWindow;

void toggle_menu_view_state( BOOLEAN & state, UINT menu_choice_id );
BOOLEAN is_alarm( short parameter_index, float value );
BOOLEAN is_warning( short parameter_index, float value );

/***********************************************************************
*                   POSITION_VIEW_PARAMETERS_WINDOW                    *
***********************************************************************/
void position_view_parameters_window( void )
{
int  myheight;
int  mywidth;
RECT r;
RECT mr;

GetWindowRect( MainPlotWindow, &mr );
GetWindowRect( ViewParametersDialogWindow, &r );
mywidth  = r.right - r.left;
myheight = r.bottom - r.top;

MoveWindow( ViewParametersDialogWindow, (mr.left + mr.right - mywidth)/2, mr.top, mywidth, myheight, TRUE );
}

/***********************************************************************
*                     SIZE_VIEW_PARAMETERS_LISTBOX                     *
***********************************************************************/
static void size_view_parameters_listbox()
{
RECT lbr;
RECT vpr;
WINDOW_CLASS lb;
WINDOW_CLASS vp;

vp = ViewParametersDialogWindow;
lb = vp.control( VP_LISTBOX );

vp.get_client_rect( vpr );
lb.get_move_rect( lbr );
lbr.bottom = vpr.bottom - 1;
lb.move( lbr );
}

/***********************************************************************
*                             DRAW_ONE_ITEM                            *
***********************************************************************/
static void draw_one_item( LPDRAWITEMSTRUCT dip )
{
HDC     dc;
TCHAR * cp;
PARAMETER_ENTRY * p;
TEXTMETRIC tm;
COLORREF old_text_color;
COLORREF my_text_color;
int pixels_per_char;
int i;
int j;
int x;
int y;
int dx;
float value;

static const int a[NOF_ALARM_LIMIT_TYPES] =
    { WARNING_MIN, WARNING_MAX, ALARM_MIN, ALARM_MAX };

dc = dip->hDC;

GetTextMetrics( dc, &tm );

pixels_per_char = tm.tmAveCharWidth;

y  = (dip->rcItem.bottom + dip->rcItem.top - tm.tmHeight) / 2;
x  = pixels_per_char / 4;

dx = (PARAMETER_NAME_LEN + 5) * pixels_per_char;

i = dip->itemData;

if ( i < 0 || i >= MAX_PARMS )
    return;

i = CurrentSort.parameter_number( i );

p = &CurrentParam.parameter[i];

if ( p->input.type == NO_PARAMETER_TYPE )
    return;

TextOut( dc, x, y, p->name, lstrlen(p->name) );

x += dx;
dx = ( 11 ) * pixels_per_char;

TextOut( dc, x, y, p->target.text(), p->target.len() );
x += dx;

for ( j=0; j<NOF_ALARM_LIMIT_TYPES; j++ )
    {
    cp = ascii_float( p->limits[a[j]].value );
    TextOut( dc, x, y, cp, lstrlen(cp) );
    x += dx;
    }

my_text_color = DialogTextColor;
if ( p->vartype != TEXT_VAR )
    {
    value = Parms[i].float_value();

    if ( is_alarm(i, value) )
        my_text_color = AlarmColor;
    else if ( is_warning(i, value) )
        my_text_color = WarningColor;
    }

if ( my_text_color != 0 )
    old_text_color = SetTextColor( dc, my_text_color );

cp = Parms[i].text();
TextOut( dc, x, y, cp, lstrlen(cp) );
x += dx;

if ( my_text_color != 0 )
    SetTextColor( dc, old_text_color );

cp = units_name( p->units );
TextOut( dc, x, y, cp, lstrlen(cp) );
}

/***********************************************************************
*                     FILL_VIEW_PARAMETERS_LISTBOX                     *
***********************************************************************/
void fill_view_parameters_listbox( void )
{
int i;
int n;
int parameter;
LRESULT ti;

PARAMETER_ENTRY * p;

ti = SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, LB_GETTOPINDEX, 0, 0L );

SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, WM_SETREDRAW, 0,  0l );
SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, LB_RESETCONTENT, 0, 0L );

n = CurrentParam.count();

for ( i=0; i<n; i++ )
    {
    parameter = CurrentSort.parameter_number( i );
    p = &CurrentParam.parameter[parameter];

    if ( p->input.type == NO_PARAMETER_TYPE )
        continue;

    SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, LB_ADDSTRING, 0,(LPARAM) i );
    }

if ( ti != LB_ERR && ti < n )
    SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, LB_SETTOPINDEX, ti, 0L );

SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, WM_SETREDRAW, 1,  0l );
InvalidateRect( GetDlgItem(ViewParametersDialogWindow, PARAMETER_LISTBOX), NULL, TRUE );

}

/***********************************************************************
*                   SET_VIEW_PARAMETERS_SETUP_STRINGS                  *
***********************************************************************/
static void set_view_parameters_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { VP_PARAM_STATIC,      TEXT("PARAMETER_STRING") },
    { VP_WARN_LOW_STATIC,   TEXT("LOW_STRING") },
    { VP_WARN_HIGH_STATIC,  TEXT("HIGH_STRING") },
    { VP_ALARM_LOW_STATIC,  TEXT("LOW_STRING") },
    { VP_ALARM_HIGH_STATIC, TEXT("HIGH_STRING") },
    { VP_VALUE_STATIC,      TEXT("VALUE_STRING") },
    { VP_ALARM_STATIC,      TEXT("ALARM_STRING") },
    { VP_WARN_STATIC,       TEXT("WARNING_STRING") },
    { VP_TARGET_STATIC,     TEXT("TARGET_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("VIEW_PARAMS")) );
w.refresh();
}

/***********************************************************************
*                      VIEW_PARAMETERS_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK view_parameters_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
LPMEASUREITEMSTRUCT mp;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        set_view_parameters_setup_strings( hWnd );
        return FALSE;

    case WM_MEASUREITEM:
        mp = (LPMEASUREITEMSTRUCT) lParam;
        mp->itemHeight = 13;
        return TRUE;

    case WM_DRAWITEM:
        draw_one_item( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_SIZE:
        size_view_parameters_listbox();
        return TRUE;

    case WM_CLOSE:
        toggle_menu_view_state( ViewParametersDisplay, VIEW_PARAMETERS_MENU );
        ShowWindow( ViewParametersDialogWindow, SW_HIDE );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                SendMessage( hWnd, WM_CLOSE, 0, 0L );
                return TRUE;
            }
        break;
    }

return FALSE;
}
