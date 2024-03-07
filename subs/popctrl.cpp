#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\rectclas.h"
#include "..\include\textlist.h"
#include "..\include\wclass.h"
#include "..\include\popctrl.h"
#include "..\include\subs.h"

extern HINSTANCE    MainInstance;

static TCHAR DefaultLanguage[]  = TEXT( "ENGLISH" );
static TCHAR PopupClassName[]    = TEXT( "VTWPopupClass" );

static const COLORREF BorderColor     = RGB(  50,  50, 255 );
static const COLORREF RedColor        = RGB( 255,   0,   0 );
static const COLORREF GrayColor       = RGB( 127, 127, 127 );
static const COLORREF GreenColor      = RGB(   0, 255,   0 );
static const COLORREF BlackColor      = RGB(   0,   0,   0 );
static const COLORREF WhiteColor      = RGB( 255, 255, 255 );
static       COLORREF BackGroundColor = RGB( 255, 255, 255 );

static COLORREF CurrentColor[NOF_MENU_COLORS] = { WhiteColor, BlackColor, GrayColor, BlackColor, WhiteColor, GrayColor };

static FONT_LIST_CLASS FontList;

static int BorderHeight = 0;
static int BorderWidth  = 0;

/***********************************************************************
*                             TO_POINT                                 *
***********************************************************************/
static void to_point( POINT & p, LPARAM lParam )
{
short i;

i = LOWORD( lParam );
p.x = (long) i;

i = HIWORD( lParam );
p.y = (long) i;
}

/***********************************************************************
*                        DO_NOTHING_PAINT_ME                           *
***********************************************************************/
static void do_nothing_paint_me( HWND hWnd )
{
PAINTSTRUCT ps;

BeginPaint( hWnd, &ps );
EndPaint( hWnd, &ps );
}

/***********************************************************************
*                         POPUP_CONTROL_ENTRY                          *
*                             show_menu                                *
***********************************************************************/
void POPUP_CONTROL_ENTRY::show_menu( int x, int y )
{
mw.move( x, y, mr.width(), mr.height(), false );
mw.show();
SetCapture( mw.handle() );
}

/***********************************************************************
*                         POPUP_CONTROL_ENTRY                          *
*                         DO_LEFT_BUTTON_UP                            *
***********************************************************************/
void POPUP_CONTROL_ENTRY::do_left_button_up( WPARAM wParam, LPARAM lParam )
{
WINDOW_CLASS w;
WPARAM       chosen_id;
POPUP_ITEM_ENTRY * m;

chosen_id = 0;
ReleaseCapture();

if ( current_index != NO_INDEX )
    {
    m  = item + current_index;
    if ( !m->is_gray )
        {
        chosen_id = (WPARAM) m->id;
        current_index = NO_INDEX;
        m->state = NO_ITEM_STATE;
        }
    }

empty();
mw.hide();
if ( chosen_id > 0 )
    {
    w = parent;
    w.post( WM_VTW_MENU_CHOICE, chosen_id, 0 );
    }
}

/***********************************************************************
*                         POPUP_CONTROL_ENTRY                           *
*                           DO_MOUSEMOVE                               *
***********************************************************************/
void POPUP_CONTROL_ENTRY::do_mousemove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
int      i;
HDC      dc;
POINT    p;
POPUP_ITEM_ENTRY * m;
WINDOW_CLASS w;

p.x      = LOWORD(lParam);  // horizontal position of cursor
p.y      = HIWORD(lParam);  // vertical position of cursor

/*
------------------------------------------------------------
If I'm still in the current rectangle there is nothing to do
------------------------------------------------------------ */
if ( current_index > NO_INDEX )
    {
    m = item + current_index;
    if ( m->r.contains(p) )
        return;
    }
    
w = hWnd;
dc = w.get_dc();
if ( !dc )
    return;

/*
-------------------------------------
The current index is no longer valide
------------------------------------- */
if ( current_index > NO_INDEX )
    {
    m = item + current_index;
    m->state = NO_ITEM_STATE;
    m->draw( dc, *this );
    current_index = NO_INDEX;
    }

/*
---------------------------------
See if I am over a different item
--------------------------------- */
for ( i=0; i<nof_items; i++ )
    {
    if ( i == current_index )
        continue;
    m = item + i;
    if ( m->r.contains(p) )
        {
        current_index = i;
        m->state = HOVER_ITEM_STATE;
        m->draw( dc, *this );
        break;        
        }
    }

w.release_dc( dc );
}

/***********************************************************************
*                          POPUP_CONTROL_PROC                          *
***********************************************************************/
LRESULT CALLBACK popup_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
POPUP_CONTROL_ENTRY * mce;

if ( msg == WM_CREATE )
    return 0;

mce = (POPUP_CONTROL_ENTRY *) GetWindowLong( hWnd, GWL_USERDATA );

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_LBUTTONUP:
        if ( mce )
            mce->do_left_button_up( wParam, lParam );
        return 0;

    case WM_MOUSEMOVE:
        if ( mce )
            mce->do_mousemove( hWnd, wParam, lParam );
        return 0;

    case WM_PAINT:
        if ( mce )
            mce->paint_me( hWnd );
        else
            do_nothing_paint_me( hWnd );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                            POPUP_ITEM_ENTRY                           *
***********************************************************************/
POPUP_ITEM_ENTRY::POPUP_ITEM_ENTRY()
{
id                            = 0;
state                         = NO_ITEM_STATE;
is_checked                    = FALSE;
is_gray                       = FALSE;

color[BACKGROUND]             = CurrentColor[BACKGROUND];
color[FOREGROUND]             = CurrentColor[FOREGROUND];
color[GRAY_FOREGROUND]        = CurrentColor[GRAY_FOREGROUND];
color[SELECT_BACKGROUND]      = CurrentColor[SELECT_BACKGROUND];
color[SELECT_FOREGROUND]      = CurrentColor[SELECT_FOREGROUND];
color[SELECT_GRAY_FOREGROUND] = CurrentColor[SELECT_GRAY_FOREGROUND];
}

/***********************************************************************
*                          ~POPUP_ITEM_ENTRY                           *
***********************************************************************/
POPUP_ITEM_ENTRY::~POPUP_ITEM_ENTRY()
{
}

/***********************************************************************
*                           POPUP_ITEM_ENTRY                           *
*                              DRAW_RECT                               *
***********************************************************************/
void POPUP_ITEM_ENTRY::draw_rect( HDC dc )
{
COLORREF c;
RECTANGLE_CLASS rc;

rc = r;
rc.left -= 2;
rc.right += 2;
if ( state == HOVER_ITEM_STATE )
    c = color[FOREGROUND];
else if ( state == SELECTED_ITEM_STATE )
    c = color[SELECT_BACKGROUND];
else
    c = color[BACKGROUND];

r.draw( dc, c );
}

/***********************************************************************
*                            POPUP_ITEM_ENTRY                           *
*                                 DRAW                                 *
***********************************************************************/
void POPUP_ITEM_ENTRY::draw( HDC dc, POPUP_CONTROL_ENTRY & owner )
{
COLORREF background_color;
COLORREF foreground_color;
HFONT   old_font;
int     h;
int     x;
int     y;

if ( state == NO_ITEM_STATE )
    {
    background_color = color[BACKGROUND];
    if ( is_gray )
        foreground_color = color[GRAY_FOREGROUND];
    else
        foreground_color = color[FOREGROUND];
    }
else
    {
    background_color = color[SELECT_BACKGROUND];
    if ( is_gray )
        foreground_color = color[SELECT_GRAY_FOREGROUND];
    else
        foreground_color = color[SELECT_FOREGROUND];
    }

r.fill( dc, background_color );

draw_rect( dc );

h = 0;
old_font = INVALID_FONT_HANDLE;
if ( owner.font_handle != INVALID_FONT_HANDLE )
    {
    old_font = (HFONT) SelectObject( dc, owner.font_handle );
    h = owner.font_height;
    }

if ( h == 0 )
    h = character_height( dc );

x = r.left + 3 + owner.checkmark_width;
y = r.top;

SetTextAlign( dc, TA_TOP | TA_LEFT );
SetTextColor( dc, foreground_color );
SetBkMode( dc, TRANSPARENT );
TextOut( dc, x, y, s.text(), s.len() );

if ( is_checked )
    {
    x = r.left + 3;
    TextOut( dc, x, y, owner.checkmark, lstrlen(owner.checkmark) );
    }

if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );
}

/***********************************************************************
*                           POPUP_CONTROL_ENTRY                         *
***********************************************************************/
POPUP_CONTROL_ENTRY::POPUP_CONTROL_ENTRY()
{
parent          = 0;
font_handle     = INVALID_FONT_HANDLE;
item            = 0;
nof_items       = 0;
current_index   = NO_INDEX;
menu_height     = 0;
menu_width      = 0;
font_height     = 0;
checkmark_width = 0;
checkmark[0]   = AsterixChar;
checkmark[1]   = SpaceChar;
checkmark[2]   = NullChar;
}

/***********************************************************************
*                           POPUP_CONTROL_ENTRY                        *
*                                EMPTY                                 *
***********************************************************************/
void POPUP_CONTROL_ENTRY::empty()
{
POPUP_ITEM_ENTRY * copy;

nof_items = 0;
if ( item )
    {
    copy = item;
    item = 0;
    delete[] copy;
    }
}

/***********************************************************************
*                          ~POPUP_CONTROL_ENTRY                        *
***********************************************************************/
POPUP_CONTROL_ENTRY::~POPUP_CONTROL_ENTRY()
{
destroy_windows();
if ( font_handle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( font_handle );
    font_handle = INVALID_FONT_HANDLE;
    }

empty();
}

/***********************************************************************
*                              DRAW_LINE                               *
***********************************************************************/
static void draw_line( HDC dc, int x1, int y1, int x2, int y2 )
{
if ( dc )
    {
    MoveToEx( dc, x1, y1, 0 );
    LineTo( dc, x2, y2 );
    }
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
void POPUP_CONTROL_ENTRY::paint_me( HWND hWnd )
{
static COLORREF gray_color  = RGB( 128, 128, 128 );
static COLORREF lightgray_color  = RGB( 192, 192, 192 );

PAINTSTRUCT ps;
HDC         dc;
int         i;
POPUP_ITEM_ENTRY * m;
HFONT       oldfont;

BeginPaint( hWnd, &ps );

dc = ps.hdc;

if ( ps.fErase )
    {
    i = nof_items - 1;
    mr.fill( dc, item[i].color[BACKGROUND] );
    }
    
oldfont = INVALID_FONT_HANDLE;
if ( font_handle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( dc, font_handle );

if ( item )
    {
    for ( i=0; i<nof_items; i++ )
        {
        m = item + i;
        m->draw( dc, *this );
        }
    }

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( dc, font_handle );

dc = 0;

EndPaint( hWnd, &ps );
}

/***********************************************************************
*                          POPUP_CONTROL_ENTRY                          *
*                            register_window                           *
*                            static function                           *
***********************************************************************/
void POPUP_CONTROL_ENTRY::register_window()
{
static bool firstime = true;
WNDCLASS wc;

if ( !firstime )
    return;

firstime = false;

wc.lpszClassName = PopupClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) popup_control_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = NULL;
wc.lpszMenuName  = NULL;
wc.hbrBackground = NULL;
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );
}

/***********************************************************************
*                          POPUP_CONTROL_ENTRY                          *
*                           DESTROY_WINDOWS                            *
***********************************************************************/
void POPUP_CONTROL_ENTRY::destroy_windows()
{
if ( mw.handle() )
    {
    mw.destroy();
    mw = 0;
    }

if ( font_handle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( font_handle );
    font_handle = INVALID_FONT_HANDLE;
    }
}

/***********************************************************************
*                           POPUP_CONTROL_ENTRY                         *
*                             CREATE_WINDOWS                           *
*     You sould register first, create window next, read last.         *
***********************************************************************/
BOOLEAN POPUP_CONTROL_ENTRY::create_windows( TCHAR * new_window_name, HWND new_parent )
{
DWORD mystyle;
RECTANGLE_CLASS pr;
STRING_CLASS    s;

/*
------------------------------------------------
Make sure I know the width and height of borders
------------------------------------------------ */
BorderWidth  = GetSystemMetrics( SM_CXBORDER );
BorderHeight = GetSystemMetrics( SM_CYBORDER );

destroy_windows();

mystyle =  WS_POPUP | WS_BORDER;
parent = new_parent;

mw = CreateWindow(
    PopupClassName,
    new_window_name,
    mystyle,
    0,
    10,
    20,
    20,
    parent,
    0,
    MainInstance,
    NULL
    );

SetWindowLong( mw.handle(), GWL_USERDATA, (LONG) this );

return TRUE;
}

/***********************************************************************
*                                 FIND                                 *
***********************************************************************/
POPUP_ITEM_ENTRY * POPUP_CONTROL_ENTRY::find( UINT id_to_find )
{
int i;
for ( i=0; i<nof_items; i++ )
    {
    if ( item[i].id == id_to_find )
        return (item + i);
    }

return 0;
}

/***********************************************************************
*                               IS_CHECKED                             *
***********************************************************************/
BOOLEAN POPUP_CONTROL_ENTRY::is_checked( UINT sorc_id )
{
POPUP_ITEM_ENTRY * m;
BOOLEAN           b;

b = FALSE;
m = find( sorc_id );
if ( m )
    b = m->is_checked;

return b;
}

/***********************************************************************
*                            SET_CHECKMARK                             *
***********************************************************************/
BOOLEAN POPUP_CONTROL_ENTRY::set_checkmark( UINT dest_id, BOOLEAN new_checked_state )
{
POPUP_ITEM_ENTRY * m;

m = find( dest_id );
if ( m )
    {
    m->is_checked = new_checked_state;
    mw.refresh();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                                IS_GRAY                               *
***********************************************************************/
BOOLEAN POPUP_CONTROL_ENTRY::is_gray( UINT sorc_id )
{
POPUP_ITEM_ENTRY * m;
BOOLEAN           b;

b = FALSE;
m = find( sorc_id );
if ( m )
    b = m->is_gray;

return b;
}

/***********************************************************************
*                               SET_GRAY                               *
***********************************************************************/
BOOLEAN POPUP_CONTROL_ENTRY::set_gray( UINT dest_id, BOOLEAN new_gray_state )
{
POPUP_ITEM_ENTRY * m;

m = find( dest_id );
if ( m )
    {
    m->is_gray = new_gray_state;
    mw.refresh();
    return TRUE;
    }

return FALSE;
}

/**********************************************************************
*                          CALCULATE_RECTANGLES                       *
**********************************************************************/
BOOLEAN POPUP_CONTROL_ENTRY::calculate_rectangles()
{
HDC dc;
int ch;
int cw;
int i;
int width;
int y;
HFONT old_font;
POPUP_ITEM_ENTRY * m;

dc = mw.get_dc();
old_font = INVALID_FONT_HANDLE;
if ( font_handle != INVALID_FONT_HANDLE )
    old_font = (HFONT) SelectObject( dc, font_handle );

ch = font_height + 1;
cw = average_character_width( dc );
checkmark_width = pixel_width( dc, checkmark );

menu_height = nof_items * ch;
menu_width = 0;

y = BorderHeight;
for ( i=0; i<nof_items; i++ )
    {
    m = item + i;
    width = pixel_width( dc, m->s.text() );
    width += cw + checkmark_width;
    maxint( menu_width, width );
    m->r.top = y;
    y += ch;
    m->r.bottom = y;
    m->r.left = BorderWidth;
    m->r.right = BorderWidth + menu_width;
    }

for ( i=0; i<nof_items; i++ )
    item[i].r.right = BorderWidth + menu_width;

if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );
mr.left = 0;
mr.right = menu_width + 5 * BorderWidth;
mr.top = 0;
mr.bottom = menu_height + 5 * BorderHeight;
mw.release_dc( dc );
return TRUE;
}

/**********************************************************************
*                         POPUP_CONTROL_ENTRY                         *
*                              SET_FONT                               *
**********************************************************************/
BOOLEAN POPUP_CONTROL_ENTRY::set_font( FONT_CLASS & new_font )
{
HDC dc;
HFONT old_font;

if ( font_handle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( font_handle );
    font_handle = INVALID_FONT_HANDLE;
    }
font_handle = FontList.get_handle( new_font );
if ( font_handle != INVALID_FONT_HANDLE )
    {
    dc = mw.get_dc();
    old_font = (HFONT) SelectObject( dc, font_handle );
    font_height = character_height( dc );
    SelectObject( dc, old_font );
    mw.release_dc( dc );
    return TRUE;
    }
    
return FALSE;
}

/***********************************************************************
*                          POPUP_CONTROL_ENTRY                         *
*                                  ADD                                 *
*     Don't do this until after you have created the window.           *
***********************************************************************/
BOOLEAN POPUP_CONTROL_ENTRY::add( TCHAR * s_to_add, UINT id_to_add )
{
int               i;
int               n;
POPUP_ITEM_ENTRY * m;
POPUP_ITEM_ENTRY * copy;

n = nof_items + 1;
m = new POPUP_ITEM_ENTRY[n];
for ( i=0; i<nof_items; i++ )
    m[i] = item[i];
i = n - 1;
m[i].s = s_to_add;
m[i].id = id_to_add;

copy = item;
item = m;
nof_items = n;
if ( copy )
    delete[] copy;

calculate_rectangles();
return TRUE;
}

/***********************************************************************
*                          POPUP_CONTROL_ENTRY                         *
*                               SET_COLORS                             *
***********************************************************************/
void POPUP_CONTROL_ENTRY::set_colors( COLORREF * new_color )
{
int i;

for ( i=0; i<NOF_MENU_COLORS; i++ )
    CurrentColor[i] = new_color[i];
    
}