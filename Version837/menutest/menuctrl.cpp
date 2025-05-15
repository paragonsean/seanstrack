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
#include "..\include\subs.h"

#include "menuctrl.h"

extern WINDOW_CLASS MainWindow;
extern HINSTANCE    MainInstance;

static TCHAR DefaultLanguage[]  = TEXT( "ENGLISH" );
static TCHAR MenuClassName[]    = TEXT( "VTWMenuClass" );
static TCHAR SubMenuClassName[] = TEXT( "VTWSubMenuClass" );

const COLORREF BorderColor     = RGB(  50,  50, 255 );
const COLORREF RedColor        = RGB( 255,   0,   0 );
const COLORREF GreenColor      = RGB(   0, 255,   0 );
const COLORREF BlackColor      = RGB(   0,   0,   0 );
const COLORREF WhiteColor      = RGB( 255, 255, 255 );
      COLORREF BackGroundColor = RGB( 255, 255, 255 );

COLORREF CurrentColor[NOF_COLORS] = { WhiteColor, BlackColor, BlackColor, WhiteColor };

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
int h;
int w;

m = main + main_index;
h = m->submenu_height + 2*BorderHeight;
w = m->submenu_width  + 2*BorderWidth + 6;

sr.setsize( w, h );
sr.moveto( m->r.left+BorderWidth+2, m->r.bottom+BorderHeight+3 );
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

if ( sub_index == NO_INDEX )
    return;
w = parent;
w.send( WM_VTW_MENU_CHOICE, (WPARAM) main[main_index].submenu[sub_index].id, 0 );
}

/***********************************************************************
*                         MENU_CONTROL_ENTRY                           *
*                         DO_LEFT_BUTTON_DOWN                          *
***********************************************************************/
void MENU_CONTROL_ENTRY::do_left_button_down( WPARAM wParam, LPARAM lParam )
{
HDC               dc;
MENU_ITEM_ENTRY * m;

if ( main_index == NO_INDEX )
    {
    if ( state == WAS_SELECTED_CONTROL_STATE )
        state = NO_CONTROL_STATE;
    return;
    }
/*
-------------------------------------------------------------------------------
If there is a sub_index then I am in the sub menu list and I don't do anything
until I get a left button up message.
------------------------------------------------------------------------------- */
if ( sub_index > NO_INDEX )
    return;

dc = mw.get_dc();
if ( dc )
    {
    state = SELECTED_CONTROL_STATE;
    m = main + main_index;
    m->state = SELECTED_ITEM_STATE;
    m->draw( dc );
    show_submenu();
    mw.release_dc( dc );
    }
}

/***********************************************************************
*                         MENU_CONTROL_ENTRY                           *
*                           DO_MOUSEMOVE                               *
***********************************************************************/
void MENU_CONTROL_ENTRY::do_mousemove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
int      i;
int      previous_main_index;
int      previous_sub_index;
UINT     currently_selected_id;
HDC      dc;
HDC      sdc;
HFONT    oldfont;
unsigned old_state;
unsigned keys;
POINT    p;
RECTANGLE_CLASS   r;
MENU_ITEM_ENTRY * m;
MENU_ITEM_ENTRY * sm;
WINDOW_CLASS w;

keys     = wParam;         // key flags
p.x      = LOWORD(lParam);  // horizontal position of cursor
p.y      = HIWORD(lParam);  // vertical position of cursor

previous_main_index = NO_INDEX;
previous_sub_index  = NO_INDEX;
w = hWnd;
dc = w.get_dc();
if ( !dc )
    return;

if ( main_index > NO_INDEX )
    {
    m = main + main_index;
    if ( m->r.contains(p) )
        {
        w.release_dc( dc );
        return;
        }

    if ( state == SELECTED_CONTROL_STATE )
        {
        r = sr;
        r.top = m->r.bottom;
        sdc = sw.get_dc();
        oldfont = INVALID_FONT_HANDLE;
        if ( font_handle != INVALID_FONT_HANDLE )
            oldfont = (HFONT) SelectObject( sdc, font_handle );
        SetTextAlign( sdc, TA_TOP | TA_LEFT );
        SetBkMode( sdc, TRANSPARENT );
        if ( r.contains(p) )
            {
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

            if ( oldfont != INVALID_FONT_HANDLE )
                SelectObject( sdc, oldfont );

            sw.release_dc( sdc );
            w.release_dc( dc );
            return;
            }
        else if ( sub_index != NO_INDEX )
            {
            sm = m->submenu + sub_index;
            sub_index = NO_INDEX;
            sm->draw_submenu_item( sdc, 0 );
            sw.release_dc( sdc );
            }

        if ( oldfont != INVALID_FONT_HANDLE )
            SelectObject( sdc, oldfont );
        oldfont = INVALID_FONT_HANDLE;
        }

    /*
    -------------------------------------------------------------
    The mouse has moved out of the rectangle of the current item.
    ------------------------------------------------------------- */
    if ( state == SELECTED_CONTROL_STATE )
        {
        state = WAS_SELECTED_CONTROL_STATE;
        sw.hide();
        }

    old_state = m->state;
    m->state = NO_ITEM_STATE;
    if ( old_state & SELECTED_ITEM_STATE )
        m->draw( dc );
    else
        m->draw_rect( dc );
    previous_main_index = main_index;
    main_index = NO_INDEX;
    ReleaseCapture();
    }

for ( i=0; i<nof_main_items; i++ )
    {
    if ( i == previous_main_index )
        continue;
    m = main + i;
    if ( m->r.contains(p) )
        {
        main_index = i;
        if ( m->state == NO_ITEM_STATE )
            {
            if ( state == WAS_SELECTED_CONTROL_STATE )
                {
                state = SELECTED_CONTROL_STATE;
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
            }
        break;
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
id                       = 0;
password_level           = 0;
state                    = NO_ITEM_STATE;
owner                    = 0;
submenu                  = 0;
nof_submenu_items        = 0;
submenu_height           = 0;
submenu_width            = 0;
color[BACKGROUND]        = WhiteColor;
color[FOREGROUND]        = BlackColor;
color[SELECT_BACKGROUND] = BlackColor;
color[SELECT_FOREGROUND] = WhiteColor;
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
if ( state == NO_ITEM_STATE )
    c = color[BACKGROUND];
else
    c = color[FOREGROUND];

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

old_font = INVALID_FONT_HANDLE;
if ( owner )
    {
    if ( owner->font_handle != INVALID_FONT_HANDLE )
        old_font = (HFONT) SelectObject( dc, owner->font_handle );
    }

x = r.left + r.right;
x /= 2;

y = r.bottom - r.top;
y -= character_height( dc );
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
    foreground_color = color[SELECT_FOREGROUND];
    }
else
    {
    background_color = color[BACKGROUND];
    foreground_color = color[FOREGROUND];
    }

r.fill( dc, background_color );

x = r.left + 3;
y = r.top;

SetTextColor( dc, foreground_color );
TextOut( dc, x, y, s.text(), s.len() );
}

/***********************************************************************
*                         MENU_ITEM_ENTRY::GET                         *
*                      1,&Backup and Restore,403,0                     *
*                   Type, Display Text      ,Id ,Password Level        *
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
buf.next_field();
s = buf;

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
for ( i=0; i<4; i++ )
    color[i] = CurrentColor[i];

return TRUE;
}

/***********************************************************************
*                           MENU_CONTROL_ENTRY                         *
***********************************************************************/
MENU_CONTROL_ENTRY::MENU_CONTROL_ENTRY()
{
id             = 0;
parent         = 0;
state          = NO_CONTROL_STATE;
font_handle    = INVALID_FONT_HANDLE;
main           = 0;
nof_main_items = 0;
main_index     = NO_INDEX;
sub_index      = NO_INDEX;
language       = DefaultLanguage;
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
int         x;
int         y;
int         cw;
MENU_ITEM_ENTRY * m;
HPEN        newpen;
HPEN        oldpen;
HFONT       oldfont;

BeginPaint( hWnd, &ps );

dc = ps.hdc;

y = mr.bottom - 2;
newpen  = CreatePen( PS_SOLID, 1, gray_color );
oldpen = (HPEN) SelectObject( dc, newpen );
draw_line( dc, mr.left, y, mr.right, y );
SelectObject( dc, oldpen );
DeleteObject( newpen );

y++;
newpen = (HPEN) GetStockObject( WHITE_PEN );
oldpen = (HPEN) SelectObject( dc, newpen );
draw_line( dc, mr.left, y, mr.right, y );
SelectObject( dc, oldpen );

oldfont = INVALID_FONT_HANDLE;
if ( font_handle != INVALID_FONT_HANDLE )
    oldfont = (HFONT) SelectObject( dc, font_handle );

cw = average_character_width( dc );
x  = cw / 2;
if ( main )
    {
    for ( i=0; i<nof_main_items; i++ )
        {
        m = main + i;
        m->r.top    = mr.top + 1;
        m->r.bottom = mr.bottom - 3;
        m->r.left   = x - 1;
        y = mr.height() - character_height( dc );
        y /= 2;
        x += pixel_width( dc, m->s.text() );
        m->r.right = x + 1;
        m->draw( dc );
        x += cw;
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
HDC         dc;
int         i;
int         y;
int         ch;
MENU_ITEM_ENTRY * m;
MENU_ITEM_ENTRY * sm;
HFONT      old_font;

BeginPaint( hWnd, &ps );

if ( main_index > NO_INDEX )
    {
    m = main + main_index;
    dc = ps.hdc;

    old_font = INVALID_FONT_HANDLE;
    if ( m->owner )
        {
        if ( m->owner->font_handle != INVALID_FONT_HANDLE )
            old_font = (HFONT) SelectObject( dc, m->owner->font_handle );
        }

    SetTextAlign( dc, TA_TOP | TA_LEFT );
    SetBkMode( dc, TRANSPARENT );

    ch = character_height( dc ) + 1;
    y  = 0;
    for ( i=0; i<m->nof_submenu_items; i++ )
        {
        sm = m->submenu + i;
        sm->r.top = y;
        y += ch;
        sm->r.bottom = y;
        sm->r.left = 0;
        sm->r.right = sr.width();
        sm->draw_submenu_item( dc, m->submenu[sub_index].id );
        }

    if ( old_font != INVALID_FONT_HANDLE )
        SelectObject( dc, old_font );

    dc = 0;
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
HFONT system_font_handle;

if ( !mw.handle() && !sw.handle() )
    return;
/*
---------------------------------------
Deselect the current font, just in case
--------------------------------------- */
system_font_handle = (HFONT) GetStockObject( OEM_FIXED_FONT );
mw.send( WM_SETFONT, (WPARAM) system_font_handle, TRUE );
sw.send( WM_SETFONT, (WPARAM) system_font_handle, TRUE );

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
    MainWindow.handle(),
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
    MainWindow.handle(),
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

for ( i=0; i<NOF_COLORS; i++ )
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
*                           MENU_CONTROL_ENTRY                         *
*                                RESIZE                                *
***********************************************************************/
BOOLEAN MENU_CONTROL_ENTRY::resize()
{
HDC             dc;
long            h;
HFONT           oldfont;
RECTANGLE_CLASS pr;
WINDOW_CLASS    pw;

pw = parent;
pw.get_client_rect( pr );

dc = mw.get_dc();
oldfont = (HFONT) SelectObject( dc, font_handle );
h = character_height( dc );
SelectObject( dc, oldfont );
mw.release_dc( dc );

h += h/2;
mr.setsize( pr.width(), h );
mr.top = 0;
mr.left = 0;

mw.move( mr );
mw.show();

return TRUE;
}

/***********************************************************************
*                           MENU_CONTROL_ENTRY                         *
*                                  READ                                *
*     Don't do this until after you have created the window.           *
***********************************************************************/
BOOLEAN MENU_CONTROL_ENTRY::read( NAME_CLASS & s )
{
TCHAR      * cp;
HDC          dc;
FILE_CLASS   f;
int          i;
int          j;
int          n;
int          w;
HFONT        new_font_handle;
HFONT        old_font;

t.empty();

if ( !f.open_for_read(s) )
    return FALSE;

/*
----------------------------
Read the number of languages
---------------------------- */
cp = f.readline();
n = asctoint32( cp );
/*
---------------------------------
For each language, read the font.
--------------------------------- */
for ( i=0; i<n; i++ )
    {
    cp = f.readline();
    if ( replace_char_with_null(cp, CommaChar) )
        {
        if ( language == cp )
            {
            cp = nextstring( cp );
            /*
            ----------------------------
            Get a handle to the new font
            ---------------------------- */
            lf.get( cp );
            new_font_handle = FontList.get_handle( lf );

            /*
            -------------------------
            Select this on my control
            ------------------------- */
            if ( mw.handle() != 0 )
                mw.send( WM_SETFONT, (WPARAM) new_font_handle, TRUE );

            if ( sw.handle() != 0 )
                sw.send( WM_SETFONT, (WPARAM) new_font_handle, TRUE );

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
            resize();
            }
        }
    }

/*
-----------------------------------------------------------------
Make a list of the menu items and keep a count of main menu items
----------------------------------------------------------------- */
n = 0;
while ( true )
    {
    cp = f.readline();
    if ( !cp )
        break;
    if ( *cp == ZeroChar )
        n++;
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
while ( t.next() )
    {
    cp = t.text();
    if ( *cp == MAIN_MENU_ITEM_TYPE )
        {
        if ( (i >= 0) && (n > 0) )
            {
            main[i].submenu = new MENU_ITEM_ENTRY[n];
            main[i].nof_submenu_items = n;
            }
        i++;
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
if ( (i >= 0) && (n > 0) )
    {
    main[i].submenu   = new MENU_ITEM_ENTRY[n];
    main[i].nof_submenu_items = n;
    }

dc = sw.get_dc();
old_font = INVALID_FONT_HANDLE;
if ( font_handle != INVALID_FONT_HANDLE )
    old_font = (HFONT) SelectObject( dc, font_handle );

j  = character_height( dc ) + 1;
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
        maxint( main[i].submenu_width, w );
        j++;
        }
    else if ( *cp == COLOR_DEF_TYPE )
        {
        get_current_colors( cp );
        }
    }

if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );

return TRUE;
}