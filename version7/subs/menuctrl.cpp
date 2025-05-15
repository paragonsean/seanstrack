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
#include "..\include\menuctrl.h"
#include "..\include\subs.h"

extern HINSTANCE    MainInstance;

static TCHAR DefaultLanguage[]  = TEXT( "ENGLISH" );
static TCHAR MenuClassName[]    = TEXT( "VTWMenuClass" );
static TCHAR SubMenuClassName[] = TEXT( "VTWSubMenuClass" );

static const COLORREF BorderColor     = RGB(  50,  50, 255 );
static const COLORREF RedColor        = RGB( 255,   0,   0 );
static const COLORREF GrayColor       = RGB( 127, 127, 127 );
static const COLORREF GreenColor      = RGB(   0, 255,   0 );
static const COLORREF BlackColor      = RGB(   0,   0,   0 );
static const COLORREF WhiteColor      = RGB( 255, 255, 255 );
static       COLORREF BackGroundColor = RGB( 255, 255, 255 );

static COLORREF CurrentColor[NOF_MENU_COLORS] = { WhiteColor, BlackColor, BlackColor, WhiteColor };

static FONT_LIST_CLASS FontList;

static int BorderHeight = 0;
static int BorderWidth  = 0;

/***********************************************************************
*                             TO_POINT                                 *
***********************************************************************/
void to_point( POINT & p, LPARAM lParam )
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

/**********************************************************************
*                           SHOW_SUBMENU                              *
**********************************************************************/
void MENU_CONTROL_ENTRY::show_submenu()
{
MENU_ITEM_ENTRY * m;
RECTANGLE_CLASS   r;
int h;
int w;
int x;
int y;

m = main + main_index;
h = m->submenu_height + 2*BorderHeight;
w = m->submenu_width  + 2*BorderWidth + 6;
mw.getrect( r );
x = r.left;
y = r.bottom + BorderHeight + 1;
if ( main_index > 0 )
    x += m->r.left;
sr.setsize( w, h );
sr.moveto( x, y );
sw.move( sr );
sw.show();
sw.refresh();
}

/***********************************************************************
*                         MENU_CONTROL_ENTRY                           *
*                         DO_LEFT_BUTTON_UP                            *
***********************************************************************/
void MENU_CONTROL_ENTRY::do_left_button_up( WPARAM wParam, LPARAM lParam )
{
WINDOW_CLASS w;
WPARAM       chosen_id;
HDC          dc;
MENU_ITEM_ENTRY * m;
MENU_ITEM_ENTRY * sm;

if ( main_index == NO_INDEX )
    return;

if ( sub_index == NO_INDEX )
    return;

m  = main + main_index;
sm = m->submenu + sub_index;
if ( sm->is_gray )
    return;

ReleaseCapture();

chosen_id = (WPARAM) sm->id;
sm->state = NO_ITEM_STATE;
sub_index = NO_INDEX;
sw.hide();

main_index = NO_INDEX;
state = NO_CONTROL_STATE;
m->state = NO_ITEM_STATE;
dc = mw.get_dc();
if ( dc )
    {
    m->draw( dc );
    mw.release_dc( dc );
    }

w = parent;
w.post( WM_VTW_MENU_CHOICE, chosen_id, 0 );
}

/***********************************************************************
*                         MENU_CONTROL_ENTRY                           *
*                         DO_LEFT_BUTTON_DOWN                          *
***********************************************************************/
void MENU_CONTROL_ENTRY::do_left_button_down( WPARAM wParam, LPARAM lParam )
{
HDC               dc;
int               i;
MENU_ITEM_ENTRY * m;
POINT             p;

/*
--------------------------------------------
Get the mouse position in client coordinates
-------------------------------------------- */
p.x = LOWORD( lParam );
p.y = HIWORD( lParam );

if ( main_index == NO_INDEX )
    return;

/*
-------------------------------------------------------------------------------
If there is a sub_index then I am in the sub menu list and I don't do anything
until I get a left button up message.
------------------------------------------------------------------------------- */
if ( sub_index > NO_INDEX )
    {
    return;
    }

/*
-----------------------------------------------------
See if the user is clicking outside my known universe
----------------------------------------------------- */
m = main + main_index;
if ( m->state != NO_ITEM_STATE )
    {
    for ( i=0; i<nof_main_items; i++ )
        {
        m = main + i;
        if ( m->r.contains(p) )
            break;
        }

    if ( i == nof_main_items )
        {
        ReleaseCapture();

        /*
        ------------------------------------------------------
        The user is clicking outside of the current selection.
        I need to unselect everyone.
        ------------------------------------------------------ */
        if ( state == SELECTED_CONTROL_STATE )
            sw.hide();

        m = main + main_index;
        m->state = NO_ITEM_STATE;

        main_index = NO_INDEX;
        state = NO_CONTROL_STATE;
        dc = mw.get_dc();
        if ( dc )
            {
            m->draw( dc );
            mw.release_dc( dc );
            }

        return;
        }
    }

dc = mw.get_dc();
if ( dc )
    {
    m = main + main_index;
    if ( state == SELECTED_CONTROL_STATE )
        {
        state = NO_CONTROL_STATE;
        m->state = HOVER_ITEM_STATE;
        sw.hide();
        }
    else
        {
        state = SELECTED_CONTROL_STATE;
        m->state = SELECTED_ITEM_STATE;
        }
    m->draw( dc );
    mw.release_dc( dc );
    if ( state == SELECTED_CONTROL_STATE )
        show_submenu();
    }
}

/***********************************************************************
*                         MENU_CONTROL_ENTRY                           *
*                           DO_MOUSEMOVE                               *
***********************************************************************/
void MENU_CONTROL_ENTRY::do_mousemove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
bool     over_main_item;
int      i;
int      previous_sub_index;
UINT     currently_selected_id;
HDC      dc;
HDC      sdc;
HFONT    oldfont;
unsigned keys;
POINT    p;
POINT    sp;
MENU_ITEM_ENTRY * m;
MENU_ITEM_ENTRY * sm;
WINDOW_CLASS w;

keys     = wParam;         // key flags
/*
---------------------------------------
Get the screen coordinates of the mouse
--------------------------------------- */
to_point( sp, lParam );
ClientToScreen( hWnd, &sp );

previous_sub_index  = NO_INDEX;

w = hWnd;
dc = w.get_dc();
if ( !dc )
    return;

if ( main_index > NO_INDEX )
    {
    m = main + main_index;

    /*
    -----------------------------------------------------------------
    Check to see if the current selection in the submenu has changed.
    ----------------------------------------------------------------- */
    if ( state == SELECTED_CONTROL_STATE )
        {
        sdc = sw.get_dc();
        oldfont = INVALID_FONT_HANDLE;
        if ( font_handle != INVALID_FONT_HANDLE )
            oldfont = (HFONT) SelectObject( sdc, font_handle );
        SetTextAlign( sdc, TA_TOP | TA_LEFT );
        SetBkMode( sdc, TRANSPARENT );
        if ( sr.contains(sp) )
            {
            p = sp;
            ScreenToClient( sw.handle(), &p );
            for ( i=0; i<m->nof_submenu_items; i++ )
                {
                sm = m->submenu + i;
                if ( sm->r.contains(p) )
                    {
                    if ( i != sub_index )
                        {
                        previous_sub_index = sub_index;
                        sub_index = i;
                        currently_selected_id = m->submenu[sub_index].id;
                        sm->draw_submenu_item( sdc, currently_selected_id );
                        }
                    }
                }
            if ( previous_sub_index != NO_INDEX )
                {
                sm = m->submenu + previous_sub_index;
                sm->draw_submenu_item( sdc, currently_selected_id );
                }
            }
        else
            {
            /*
            -------------------------------------------------------------------------
            The cursor has moved out of the submenu window. If something was selected
            I need to unselect it.
            ------------------------------------------------------------------------- */
            if ( sub_index != NO_INDEX )
                {
                sm = m->submenu + sub_index;
                sub_index = NO_INDEX;
                sm->draw_submenu_item( sdc, 0 );
                }
            }

        if ( oldfont != INVALID_FONT_HANDLE )
            SelectObject( sdc, oldfont );
        oldfont = INVALID_FONT_HANDLE;
        sw.release_dc( sdc );

        if ( sub_index > NO_INDEX )
            {
            w.release_dc( dc );
            return;
            }
        }
    }

p = sp;
ScreenToClient( mw.handle(), &p );
over_main_item = false;
if ( mr.contains(p) )
    {
    for ( i=0; i<nof_main_items; i++ )
        {
        m = main + i;
        if ( m->r.contains(p) )
            {
            over_main_item = true;
            if ( i == main_index )
                break;

            if ( main_index > NO_INDEX )
                {
                m = main + main_index;
                m->state = NO_ITEM_STATE;
                m->draw( dc );
                if ( state == SELECTED_CONTROL_STATE )
                    sw.hide();
                m = main + i;
                }

            main_index = i;
            if ( state == SELECTED_CONTROL_STATE )
                {
                m->state = SELECTED_ITEM_STATE;
                m->draw( dc );
                }
            else
                {
                m->state = HOVER_ITEM_STATE;
                m->draw_rect( dc );
                }
            SetCapture( w.handle() );
            if ( state == SELECTED_CONTROL_STATE )
                show_submenu();
            break;
            }
        }
    }

if ( !over_main_item && main_index > NO_INDEX)
    {
    m = main + main_index;
    if ( m->state == HOVER_ITEM_STATE )
        {
        m->state = NO_ITEM_STATE;
        m->draw_rect( dc );
        main_index = NO_INDEX;
        ReleaseCapture();
        }
    }

w.release_dc( dc );
}

/***********************************************************************
*                          MENU_CONTROL_PROC                           *
***********************************************************************/
LRESULT CALLBACK menu_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
MENU_CONTROL_ENTRY * mce;

if ( msg == WM_CREATE )
    return 0;

mce = (MENU_CONTROL_ENTRY *) GetWindowLong( hWnd, GWL_USERDATA );

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_SIZE:
        if ( mce )
            mce->resize();
        return 0;

    case WM_LBUTTONDOWN:
        if ( mce )
            mce->do_left_button_down( wParam, lParam );
        return 0;

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
*                          SUB_MENU_CONTROL_PROC                       *
***********************************************************************/
LRESULT CALLBACK sub_menu_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
MENU_CONTROL_ENTRY * mce;

if ( msg == WM_CREATE )
    return 0;

mce = (MENU_CONTROL_ENTRY *) GetWindowLong( hWnd, GWL_USERDATA );

id  = LOWORD( wParam );
switch ( msg )
    {
    case WM_PAINT:
        if ( mce )
            mce->paint_submenu( hWnd );
        else
            do_nothing_paint_me( hWnd );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                             READ_RECTANGLE                           *
***********************************************************************/
BOOLEAN read_rectangle( RECTANGLE_CLASS & r, FILE_CLASS & f )
{
const int BUFLEN = 50;
TCHAR buf[BUFLEN];
TCHAR * cp;

lstrcpy( buf, f.readline() );
cp = buf;

replace_char_with_null( cp, CommaChar );
r.left = (LONG) asctoint32( cp );

cp = nextstring( cp );
replace_char_with_null( cp, CommaChar );
r.top = (LONG) asctoint32( cp );

cp = nextstring( cp );
replace_char_with_null( cp, CommaChar );
r.right = (LONG) asctoint32( cp );

cp = nextstring( cp );
r.bottom = (LONG) asctoint32( cp );

return TRUE;
}

/***********************************************************************
*                            MENU_ITEM_ENTRY                           *
***********************************************************************/
MENU_ITEM_ENTRY::MENU_ITEM_ENTRY()
{
id                            = 0;
password_level                = 0;
state                         = NO_ITEM_STATE;
is_checked                    = FALSE;
is_gray                       = FALSE;
owner                         = 0;
submenu                       = 0;
nof_submenu_items             = 0;
submenu_height                = 0;
submenu_width                 = 0;
color[BACKGROUND]             = WhiteColor;
color[FOREGROUND]             = BlackColor;
color[GRAY_FOREGROUND]        = GrayColor;
color[SELECT_BACKGROUND]      = BlackColor;
color[SELECT_FOREGROUND]      = WhiteColor;
color[SELECT_GRAY_FOREGROUND] = GrayColor;
}

/***********************************************************************
*                            ~MENU_ITEM_ENTRY                           *
***********************************************************************/
MENU_ITEM_ENTRY::~MENU_ITEM_ENTRY()
{
MENU_ITEM_ENTRY * copy;
if ( submenu )
    {
    copy = submenu;
    submenu = 0;
    delete[] copy;
    }
}

/***********************************************************************
*                            MENU_ITEM_ENTRY                           *
*                              DRAW_RECT                               *
***********************************************************************/
void MENU_ITEM_ENTRY::draw_rect( HDC dc )
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
*                            MENU_ITEM_ENTRY                           *
*                                 DRAW                                 *
***********************************************************************/
void MENU_ITEM_ENTRY::draw( HDC dc )
{
COLORREF background_color;
COLORREF foreground_color;
HFONT   old_font;
int     h;
int     x;
int     y;

if ( state == NO_ITEM_STATE || state == HOVER_ITEM_STATE )
    {
    background_color = color[BACKGROUND];
    foreground_color = color[FOREGROUND];
    }
else
    {
    background_color = color[SELECT_BACKGROUND];
    foreground_color = color[SELECT_FOREGROUND];
    }

r.fill( dc, background_color );

draw_rect( dc );

h = 0;
old_font = INVALID_FONT_HANDLE;
if ( owner )
    {
    if ( owner->font_handle != INVALID_FONT_HANDLE )
        {
        old_font = (HFONT) SelectObject( dc, owner->font_handle );
        h = owner->font_height;
        }
    }
if ( h == 0 )
    h = character_height( dc );

x = r.left + r.right;
x /= 2;

y = r.bottom - r.top;
y -= h;
y /= 2;
y += r.top;
SetTextAlign( dc, TA_TOP | TA_CENTER );
SetTextColor( dc, foreground_color );

SetBkMode( dc, TRANSPARENT );

TextOut( dc, x, y, s.text(), s.len() );

if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );
}

/***********************************************************************
*                            MENU_ITEM_ENTRY                           *
*                           DRAW_SUBMENU_ITEM                          *
***********************************************************************/
void MENU_ITEM_ENTRY::draw_submenu_item( HDC dc, UINT currently_selected_id )
{
COLORREF background_color;
COLORREF foreground_color;
int x;
int y;

if ( id == currently_selected_id )
    {
    background_color = color[SELECT_BACKGROUND];
    if ( is_gray )
        foreground_color = color[SELECT_GRAY_FOREGROUND];
    else
        foreground_color = color[SELECT_FOREGROUND];
    }
else
    {
    background_color = color[BACKGROUND];
    if ( is_gray )
        foreground_color = color[GRAY_FOREGROUND];
    else
        foreground_color = color[FOREGROUND];
    }

r.fill( dc, background_color );

x = r.left + 3;
if ( owner )
    x += owner->checkmark_width;
y = r.top;

SetTextColor( dc, foreground_color );
TextOut( dc, x, y, s.text(), s.len() );

if ( is_checked && owner )
    {
    x = r.left + 3;
    TextOut( dc, x, y, owner->checkmark, lstrlen(owner->checkmark) );
    }
}

/***********************************************************************
*                         MENU_ITEM_ENTRY::GET                         *
*            1,Backup and Restore,Sichern und...,403,0                 *
*         Type,English,           German,        Id ,Password Level    *
***********************************************************************/
BOOLEAN MENU_ITEM_ENTRY::get( TCHAR * record )
{
STRING_CLASS buf;
int          i;

buf = record;

/*
---------------------------------------------------------
The first field is the record type. I don't use this here
--------------------------------------------------------- */
buf.next_field();

/*
----------------------------------
The next field is the display text
---------------------------------- */
for ( i=0; i<owner->nof_languages; i++ )
    {
    buf.next_field();
    if ( i == owner->language_index )
        s = buf;
    }

/*
------------------------
The next field is the id
------------------------ */
buf.next_field();
id = buf.int_value();

/*
--------------
Password Level
-------------- */
buf.next_field();
password_level = buf.int_value();

/*
----------------------
Get the current colors
---------------------- */
for ( i=0; i<NOF_MENU_COLORS; i++ )
    color[i] = CurrentColor[i];

return TRUE;
}

/***********************************************************************
*                           MENU_CONTROL_ENTRY                         *
***********************************************************************/
MENU_CONTROL_ENTRY::MENU_CONTROL_ENTRY()
{
id              = 0;
parent          = 0;
state           = NO_CONTROL_STATE;
font_handle     = INVALID_FONT_HANDLE;
main            = 0;
nof_main_items  = 0;
main_index      = NO_INDEX;
sub_index       = NO_INDEX;
language        = DefaultLanguage;
nof_languages   = 0;
language_index  = 0;
font_height     = 0;
checkmark_width = 0;
checkmark[0]   = AsterixChar;
checkmark[1]   = SpaceChar;
checkmark[2]   = NullChar;
}

/***********************************************************************
*                          ~MENU_CONTROL_ENTRY                         *
***********************************************************************/
MENU_CONTROL_ENTRY::~MENU_CONTROL_ENTRY()
{
MENU_ITEM_ENTRY * copy;

destroy_windows();
if ( font_handle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( font_handle );
    font_handle = INVALID_FONT_HANDLE;
    }
if ( main )
    {
    copy = main;
    main = 0;
    delete[] copy;
    }
}

/***********************************************************************
*                              DRAW_LINE                               *
***********************************************************************/
void draw_line( HDC dc, int x1, int y1, int x2, int y2 )
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
void MENU_CONTROL_ENTRY::paint_me( HWND hWnd )
{
static COLORREF gray_color  = RGB( 128, 128, 128 );
static COLORREF lightgray_color  = RGB( 192, 192, 192 );

PAINTSTRUCT ps;
HDC         dc;
int         i;
MENU_ITEM_ENTRY * m;
HFONT       oldfont;

BeginPaint( hWnd, &ps );

dc = ps.hdc;

if ( ps.fErase )
    {
    i = nof_main_items - 1;
    mr.fill( dc, main[i].color[BACKGROUND] );
    }

oldfont = INVALID_FONT_HANDLE;
if ( font_handle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( dc, font_handle );

if ( main )
    {
    for ( i=0; i<nof_main_items; i++ )
        {
        m = main + i;
        m->draw( dc );
        }
    }

if ( oldfont != INVALID_FONT_HANDLE )
    SelectObject( dc, font_handle );

dc = 0;

EndPaint( hWnd, &ps );
}

/***********************************************************************
*                            PAINT_SUBMENU                             *
***********************************************************************/
void MENU_CONTROL_ENTRY::paint_submenu( HWND hWnd )
{
PAINTSTRUCT ps;
UINT        currently_selected_id;
HDC         dc;
int         i;
MENU_ITEM_ENTRY * m;
MENU_ITEM_ENTRY * sm;
HFONT      old_font;

currently_selected_id = 0;

BeginPaint( hWnd, &ps );

if ( main_index > NO_INDEX )
    {
    m = main + main_index;
    dc = ps.hdc;
    if ( sub_index > NO_INDEX )
        currently_selected_id = m->submenu[sub_index].id;
    old_font = INVALID_FONT_HANDLE;
    if ( m->owner )
        {
        if ( m->owner->font_handle != INVALID_FONT_HANDLE )
            old_font = (HFONT) SelectObject( dc, m->owner->font_handle );
        }

    SetTextAlign( dc, TA_TOP | TA_LEFT );
    SetBkMode( dc, TRANSPARENT );

    for ( i=0; i<m->nof_submenu_items; i++ )
        {
        sm = m->submenu + i;
        sm->draw_submenu_item( dc, currently_selected_id );
        }

    if ( old_font != INVALID_FONT_HANDLE )
        SelectObject( dc, old_font );
    }

EndPaint( hWnd, &ps );
}

/***********************************************************************
*                          MENU_CONTROL_ENTRY                          *
*                            register_window                           *
*                            static function                           *
***********************************************************************/
void MENU_CONTROL_ENTRY::register_window()
{
static bool firstime = true;
WNDCLASS wc;

if ( !firstime )
    return;

firstime = false;

wc.lpszClassName = MenuClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) menu_control_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = NULL;
wc.lpszMenuName  = NULL;
wc.hbrBackground = NULL;
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

wc.lpszClassName = SubMenuClassName;
wc.lpfnWndProc   = (WNDPROC) sub_menu_control_proc;
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
*                          MENU_CONTROL_ENTRY                          *
*                           DESTROY_WINDOWS                            *
***********************************************************************/
void MENU_CONTROL_ENTRY::destroy_windows()
{
if ( !mw.handle() && !sw.handle() )
    return;

mw.destroy();
mw = 0;

sw.destroy();
sw = 0;

if ( font_handle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( font_handle );
    font_handle = INVALID_FONT_HANDLE;
    }
}

/***********************************************************************
*                           MENU_CONTROL_ENTRY                         *
*                             CREATE_WINDOWS                           *
*     You sould register first, create window next, read last.         *
***********************************************************************/
BOOLEAN MENU_CONTROL_ENTRY::create_windows( TCHAR * new_window_name, HWND new_parent )
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

mystyle =  WS_CHILD;
name    = new_window_name;
parent  = new_parent;

mw = CreateWindow(
    MenuClassName,
    new_window_name,
    mystyle,
    0,
    0,
    100,
    10,
    new_parent,
    0,
    MainInstance,
    NULL
    );

SetWindowLong( mw.handle(), GWL_USERDATA, (LONG) this );

mystyle =  WS_POPUP | WS_BORDER;
s       = new_window_name;
s      += TEXT("SubMenu");
parent  = new_parent;

sw = CreateWindow(
    SubMenuClassName,
    s.text(),
    mystyle,
    0,
    10,
    20,
    20,
    new_parent,
    0,
    MainInstance,
    NULL
    );

SetWindowLong( sw.handle(), GWL_USERDATA, (LONG) this );

return TRUE;
}

/***********************************************************************
*                          GET_CURRENT_COLORS                          *
***********************************************************************/
static void get_current_colors( TCHAR * record )
{
STRING_CLASS buf;
int          i;
BYTE         red;
BYTE         green;
BYTE         blue;

buf = record;

/*
---------------------------------------------------------
The first field is the record type. I don't use this here
--------------------------------------------------------- */
buf.next_field();

for ( i=0; i<NOF_MENU_COLORS; i++ )
    {
    buf.next_field();
    red = (BYTE) buf.uint_value();
    buf.next_field();
    green = (BYTE) buf.uint_value();
    buf.next_field();
    blue = (BYTE) buf.uint_value();

    CurrentColor[i] = RGB( red, green, blue );
    }

return;
}

/***********************************************************************
*                                 FIND                                 *
***********************************************************************/
MENU_ITEM_ENTRY * MENU_CONTROL_ENTRY::find( UINT id_to_find )
{
int i;
int j;
for ( i=0; i<nof_main_items; i++ )
    {
    for ( j=0; j<main[i].nof_submenu_items; j++ )
        {
        if ( main[i].submenu[j].id == id_to_find )
            return (main[i].submenu + j);
        }
    }

return 0;
}

/***********************************************************************
*                               IS_CHECKED                             *
***********************************************************************/
BOOLEAN MENU_CONTROL_ENTRY::is_checked( UINT sorc_id )
{
MENU_ITEM_ENTRY * m;
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
BOOLEAN MENU_CONTROL_ENTRY::set_checkmark( UINT dest_id, BOOLEAN new_checked_state )
{
MENU_ITEM_ENTRY * m;

m = find( dest_id );
if ( m )
    {
    m->is_checked = new_checked_state;
    sw.refresh();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                                IS_GRAY                               *
***********************************************************************/
BOOLEAN MENU_CONTROL_ENTRY::is_gray( UINT sorc_id )
{
MENU_ITEM_ENTRY * m;
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
BOOLEAN MENU_CONTROL_ENTRY::set_gray( UINT dest_id, BOOLEAN new_gray_state )
{
MENU_ITEM_ENTRY * m;

m = find( dest_id );
if ( m )
    {
    m->is_gray = new_gray_state;
    sw.refresh();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           MENU_CONTROL_ENTRY                         *
*                                RESIZE                                *
*                   mr is the client rectangle of mw.                  *
***********************************************************************/
BOOLEAN MENU_CONTROL_ENTRY::resize( int x )
{
long            h;
RECTANGLE_CLASS pr;
WINDOW_CLASS    pw;

pw = parent;
pw.get_client_rect( pr );
pr.left = x;
h = font_height + 1 + BorderHeight * 2;;
mr.setsize( pr.width(), h );
mr.moveto( x, 0 );
mw.move( mr );
mw.show();

return TRUE;
}

/***********************************************************************
*                           MENU_CONTROL_ENTRY                         *
*                                RESIZE                                *
*                   mr is the client rectangle of mw.                  *
***********************************************************************/
BOOLEAN MENU_CONTROL_ENTRY::resize()
{
return resize( 0 );
}

/**********************************************************************
*                            PASSWORDLEVEL                            *
* The password level is a number at the end of the line (after the    *
* last comma.                                                         *
**********************************************************************/
static int passwordlevel( TCHAR * sorc )
{
TCHAR * cp;
cp = sorc + lstrlen(sorc);
while ( *cp != CommaChar && cp > sorc )
    cp--;
if ( *cp == CommaChar )
    {
    cp++;
    if ( *cp != NullChar )
       return (int) asctoint32( cp );
    }

return 0;
}

/**********************************************************************
*                            EXCEL_TO_CSV                             *
* After Excel has mangled it, this restores the line to csv           *
**********************************************************************/
void excel_to_csv( TCHAR * sorc )
{
TCHAR * s;
TCHAR replacement;

replacement = NullChar;

s = sorc + lstrlen(sorc);
while ( s > sorc )
    {
    s--;
    if ( *s == TabChar )
        *s = replacement;
    else
        replacement = CommaChar;
    }
}

/***********************************************************************
*                           MENU_CONTROL_ENTRY                         *
*                                  READ                                *
*     Don't do this until after you have created the window.           *
***********************************************************************/
BOOLEAN MENU_CONTROL_ENTRY::read( NAME_CLASS & s, int current_password_level )
{
static wchar_t UnicodeIntroducer = L'\xFEFF';
static TCHAR READ_COMPLETE = MAIN_MENU_ITEM_TYPE;

TCHAR           * cp;
HDC               dc;
FILE_CLASS        f;
int               ch;
int               cw;
int               i;
int               j;
int               n;
MENU_ITEM_ENTRY * m;
MENU_ITEM_ENTRY * sm;
int               w;
int               x;
int               y;
HFONT             new_font_handle;
HFONT             old_font;
bool              ok_to_read;
bool              main_item_is_valid;

t.empty();

if ( !f.open_for_read(s) )
    return FALSE;

/*
----------------------------
Read the number of languages
---------------------------- */
cp = f.readline();

#ifdef UNICODE
if ( *cp == UnicodeIntroducer )
    cp++;
#endif

excel_to_csv( cp );
nof_languages = asctoint32( cp );

/*
---------------------------------
For each language, read the font.
--------------------------------- */
for ( i=0; i<nof_languages; i++ )
    {
    cp = f.readline();
    excel_to_csv( cp );
    if ( replace_char_with_null(cp, CommaChar) )
        {
        if ( language == cp )
            {
            language_index = i;
            cp = nextstring( cp );
            /*
            ----------------------------
            Get a handle to the new font
            ---------------------------- */
            lf.get( cp );
            new_font_handle = FontList.get_handle( lf );

            /*
            ---------------------
            Remove the old handle
            --------------------- */
            if ( font_handle != INVALID_FONT_HANDLE )
                {
                FontList.free_handle( font_handle );
                font_handle = INVALID_FONT_HANDLE;
                }

            font_handle = new_font_handle;
            //resize();
            }
        }
    }

/*
-----------------------------------------------------------------
Make a list of the menu items and keep a count of main menu items
Only add valid lines (pw <= current pw).
----------------------------------------------------------------- */
n = 0;
while ( true )
    {
    cp = f.readline();
    if ( !cp )
        break;
    excel_to_csv( cp );
    ok_to_read = true;
    if ( *cp != COLOR_DEF_TYPE )
        ok_to_read = (passwordlevel(cp) <= current_password_level);
    if ( *cp == MAIN_MENU_ITEM_TYPE )
        {
        main_item_is_valid = ok_to_read;
        if ( main_item_is_valid )
            n++;
        }
    else if ( *cp == SUB_MENU_ITEM_TYPE )
        {
        if ( !main_item_is_valid )
            ok_to_read = false;
        }

    if ( ok_to_read )
        t.append( cp );
    }
f.close();

/*
----------------------------------
n is the number of main menu items
---------------------------------- */
main = new MENU_ITEM_ENTRY[n];
nof_main_items = n;
for ( i=0; i<n; i++ )
    main[i].owner = this;

/*
----------------------------------------------
Fill the main list and count the submenu items
---------------------------------------------- */
i = -1;
n = 0;
t.rewind();
while ( true )
    {
    if ( t.next() )
        cp = t.text();
    else
        cp = &READ_COMPLETE;

    if ( *cp == MAIN_MENU_ITEM_TYPE )
        {
        /*
        ----------------------------------------------------
        Allocate the submenu for the previous main menu item
        ---------------------------------------------------- */
        if ( (i >= 0) && (n > 0) )
            {
            main[i].submenu = new MENU_ITEM_ENTRY[n];
            main[i].nof_submenu_items = n;
            for ( j=0; j<n; j++ )
                main[i].submenu[j].owner = this;
            }

        i++;
        if ( i >= nof_main_items )
            break;

        main[i].get( cp );
        n = 0;
        }
    else if ( *cp == SUB_MENU_ITEM_TYPE )
        {
        n++;
        }
    else if ( *cp == COLOR_DEF_TYPE )
        {
        get_current_colors( cp );
        }
    }

/*
-----------------------------------------
Calculate the size of each main rectangle
----------------------------------------- */
dc = mw.get_dc();
old_font = INVALID_FONT_HANDLE;
if ( font_handle != INVALID_FONT_HANDLE )
    old_font = (HFONT) SelectObject( dc, font_handle );

font_height = character_height( dc );
cw = average_character_width( dc );
x = 0;
for ( i=0; i<nof_main_items; i++ )
    {
    m = main + i;
    m->r.top    = 1;
    m->r.bottom = font_height + 1 + BorderHeight * 2;
    m->r.left   = x;
    x += pixel_width( dc, m->s.text() );
    x += cw*2;
    m->r.right = x;
    x++;
    }

if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );

old_font = INVALID_FONT_HANDLE;
mw.release_dc( dc );

dc = sw.get_dc();
old_font = INVALID_FONT_HANDLE;
if ( font_handle != INVALID_FONT_HANDLE )
    old_font = (HFONT) SelectObject( dc, font_handle );

checkmark_width = pixel_width( dc, checkmark );

j = font_height + 1;
for ( i=0; i<nof_main_items; i++ )
    {
    main[i].submenu_height = main[i].nof_submenu_items * j;
    main[i].submenu_width = 0;
    }

/*
-------------------------------------
Go thru one more time to get submenus
------------------------------------- */
i = -1;
j = 0;
t.rewind();
while ( t.next() )
    {
    s = t.text();
    cp = s.text();
    if ( *cp == MAIN_MENU_ITEM_TYPE )
        {
        i++;
        j = 0;
        }
    else if ( *cp == SUB_MENU_ITEM_TYPE )
        {
        main[i].submenu[j].get( cp );
        w = pixel_width( dc, main[i].submenu[j].s.text() );
        w += cw + checkmark_width;
        maxint( main[i].submenu_width, w );
        j++;
        }
    else if ( *cp == COLOR_DEF_TYPE )
        {
        get_current_colors( cp );
        }
    }

ch = font_height + 1;
for ( i=0; i<nof_main_items; i++ )
    {
    y = 0;
    m = main + i;
    for ( j=0; j<m->nof_submenu_items; j++ )
        {
        sm = m->submenu + j;
        sm->r.top = y;
        y += ch;
        sm->r.bottom = y;
        sm->r.left = 0;
        sm->r.right = m->submenu_width + 2*BorderWidth + 6;

        }
    }

if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );

sw.release_dc( dc );
mw.refresh();
return TRUE;
}