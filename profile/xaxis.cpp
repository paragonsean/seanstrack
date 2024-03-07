#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\color.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

const COLORREF DEFAULT_COLOR = RGB( 255, 255, 255 );

#include "extern.h"
#include "xaxis.h"

void draw_rectangle( HDC dc, RECT & r, COLORREF color );
void fill_rectangle( HDC dc, RECT & r, COLORREF color );
void get_text_size( int32 & dest_width, int32 & dest_height, HDC dc, const TCHAR * sorc );

/***********************************************************************
*                          SHOT_ARRAY_CLASS                            *
*                               CLEANUP                                *
***********************************************************************/
void SHOT_ARRAY_CLASS::cleanup()
{
if ( p )
    delete[] p;
p    = 0;
n    = 0;
nset = 0;
x    = 0;
rewound = FALSE;
}

/***********************************************************************
*                          SHOT_ARRAY_CLASS                            *
*                                INIT                                  *
***********************************************************************/
BOOLEAN SHOT_ARRAY_CLASS::init( int32 nof_points )
{
cleanup();

if ( nof_points > 0 )
    {
    n = 0;
    p = new SHOT_DATA_ENTRY[nof_points];
    if ( p )
        {
        n = nof_points;
        rewound = TRUE;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                          SHOT_ARRAY_CLASS                            *
*                            LEFT_SHIFT                                *
***********************************************************************/
int32 SHOT_ARRAY_CLASS::left_shift( SHOT_DATA_ENTRY & sorc )
{
int32 i;

if ( !p )
    return 0;

if ( n <= 0 )
    return 0;

if ( nset < n )
    {
    i = nset;
    p[nset] = sorc;
    nset++;
    }
else
    {
    for ( i=0; i<(n-1); i++ )
        p[i] = p[i+1];

    p[n-1] = sorc;
    i = n-1;
    }

return i;
}

/***********************************************************************
*                          SHOT_ARRAY_CLASS                            *
*                            LEFT_SHIFT                                *
***********************************************************************/
int32 SHOT_ARRAY_CLASS::left_shift( STRING_CLASS & new_shot_name, SYSTEMTIME & st )
{
SHOT_DATA_ENTRY s;

s.set( new_shot_name, st );
return left_shift( s );
}

/***********************************************************************
*                          SHOT_ARRAY_CLASS                            *
*                                SET                                   *
***********************************************************************/
BOOLEAN SHOT_ARRAY_CLASS::set( STRING_CLASS & new_shot_name, SYSTEMTIME & st )
{
if ( p && x >= 0 && x < n )
    {
    p[x].shot_name = new_shot_name;
    p[x].shot_time.set( st );
    if ( x >= nset )
        nset = x + 1;
    return TRUE;
    }
return FALSE;
}


/***********************************************************************
*                          SHOT_ARRAY_CLASS                            *
*                            NEXT_SET                                  *
***********************************************************************/
BOOLEAN SHOT_ARRAY_CLASS::next_set()
{

if ( rewound )
    {
    rewound = FALSE;
    }
else
    {
    x++;
    if ( x >= nset )
        {
        x--;
        return FALSE;
        }
    }

return TRUE;
}

/***********************************************************************
*                             SHOT_ARRAY_CLASS                         *
*                                  []                                  *
***********************************************************************/
SHOT_DATA_ENTRY & SHOT_ARRAY_CLASS::operator[]( int32 i )
{
static SHOT_DATA_ENTRY empty_shot;

if ( p && i>=0 && i< n )
    return p[i];

return empty_shot;
}

/***********************************************************************
*                          SHOT_ARRAY_CLASS                            *
*                               NEXT                                   *
***********************************************************************/
BOOLEAN SHOT_ARRAY_CLASS::next()
{

if ( rewound )
    {
    rewound = FALSE;
    }
else
    {
    x++;
    if ( x >= n )
        {
        x--;
        return FALSE;
        }
    }

return TRUE;
}
/***********************************************************************
*                             SHOT_ARRAY_CLASS                         *
*                              MAX_SHOT_NAME                           *
***********************************************************************/
BOOLEAN SHOT_ARRAY_CLASS::max_shot_name( STRING_CLASS & dest )
{
int32 i;

if ( !p )
    return FALSE;

dest = p[0].shot_name;

for ( i=1; i<nset; i++ )
    if ( p[i].shot_name > dest )
        dest = p[i].shot_name;

return TRUE;
}

/***********************************************************************
*                            SHOT_ARRAY_CLASS                          *
*                             MIN_SHOT_NANE                            *
***********************************************************************/
BOOLEAN SHOT_ARRAY_CLASS::min_shot_name( STRING_CLASS & dest )
{
int32 i;

if ( !p )
    return FALSE;

dest = p[0].shot_name;

for ( i=1; i<nset; i++ )
    if ( p[i].shot_name < dest )
        dest = p[i].shot_name;

return TRUE;
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
***********************************************************************/
X_AXIS_CLASS::X_AXIS_CLASS()
{
dc          = 0;
intercept   = 0;
type        = NO_AXIS_TYPE;
nof_tics    = 0;
axis_color  = DEFAULT_COLOR;
label_color = DEFAULT_COLOR;
title_color = DEFAULT_COLOR;
r.top       = 0;
r.bottom    = 0;
r.left      = 0;
r.right     = 0;
firstime    = TRUE;
min_pixel   = 0;
max_pixel   = 0;
pixel_range = 0;

max         = 0;
min         = 0;
}

/***********************************************************************
*                           ~X_AXIS_CLASS                              *
***********************************************************************/
X_AXIS_CLASS::~X_AXIS_CLASS()
{
}

/***********************************************************************
*                            SET_BITMAP_SIZE                           *
***********************************************************************/
void X_AXIS_CLASS::set_bitmap_size( int shot_name_length )
{
const TCHAR sample_end[]    = TEXT( "  00/00/0000 00:00:0000" );
const int sample_end_length = sizeof(sample_end)/sizeof(TCHAR);

HFONT old_font;
HDC   dc;
int32 h;
int   i;
int32 w;
STRING_CLASS s;

firstime = TRUE;

s.init( shot_name_length + sample_end_length );
for ( i=0; i<shot_name_length; i++ )
    s += ZeroChar;

s += sample_end;

/*
-----------------
Create the bitmap
----------------- */
dc  = GetDC( MainPlotWindow );

old_font = (HFONT) SelectObject( dc, GetStockObject(SYSTEM_FONT) );
get_text_size( w, h, dc, s.text() );
SelectObject( dc, old_font );

w  += 4;
h  += 4;

bitmap.init( dc, w, h );
ReleaseDC( MainPlotWindow, dc );
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                                INIT                                  *
***********************************************************************/
BOOLEAN X_AXIS_CLASS::init( int32 nof_ints )
{
return SHOT_ARRAY_CLASS::init( nof_ints );
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                             LEFT_SHIFT                               *
***********************************************************************/
void X_AXIS_CLASS::left_shift( STRING_CLASS & new_shot_name, SYSTEMTIME & st )
{
BOOLEAN need_max;
BOOLEAN need_min;
STRING_CLASS my_shot_name;

need_max = FALSE;
need_min = FALSE;

/* if ( set_count() == count() )*/
if ( nset == n )
    {
    my_shot_name = (*this)[0].shot_name;
    if ( my_shot_name == min )
        need_min = TRUE;

    if ( my_shot_name == max )
        need_max = TRUE;
    }

SHOT_ARRAY_CLASS::left_shift( new_shot_name, st );

if ( new_shot_name < min )
    {
    min = new_shot_name;
    need_min = FALSE;
    }

if ( new_shot_name > max )
    {
    max = new_shot_name;
    need_max = FALSE;
    }

if ( need_min )
    min_shot_name( min );

if ( need_max )
    max_shot_name( max );
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                                 SET                                  *
***********************************************************************/
void X_AXIS_CLASS::set( STRING_CLASS & new_shot_name, SYSTEMTIME & st )
{
if ( new_shot_name < min || firstime )
    min = new_shot_name;

if ( new_shot_name > max || firstime )
    max = new_shot_name;

firstime = FALSE;

SHOT_ARRAY_CLASS::set( new_shot_name, st );
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                              DRAW_LINE                               *
***********************************************************************/
void X_AXIS_CLASS::draw_line( int x1, int y1, int x2, int y2 )
{
MoveToEx( dc, x1, y1, 0 );
LineTo( dc, x2, y2 );
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                               PAINT                                  *
***********************************************************************/
BOOLEAN X_AXIS_CLASS::paint( HDC dest_device_context )
{

int  char_width;
int  i;
int  nt;
int  v;
int  x;
int  y;
int  y2;
int  line_height;
int  tic_height;
HPEN mypen;
HPEN oldpen;
SIZE label_size;
LONG & label_width  = label_size.cx;
LONG & label_height = label_size.cy;
STRING_CLASS s;

if ( r.left >= r.right )
    return FALSE;

if ( !GetTextExtentPoint32(dest_device_context, TEXT("0"), 1, &label_size) )
    return FALSE;

char_width = (int) label_width;

s = max;

if ( !GetTextExtentPoint32(dest_device_context, s.text(), s.len(), &label_size) )
    return FALSE;

tic_height  = label_height / 2;
line_height = label_height; /* * 3 / 2; */

dc = dest_device_context;

SetTextAlign( dc, TA_TOP | TA_CENTER );
SetTextColor( dc, label_color );
SetBkMode(    dc, TRANSPARENT );

mypen  = (HPEN) CreatePen( PS_SOLID, 1, axis_color );
if ( mypen )
    {
    oldpen = (HPEN) SelectObject( dc, mypen );

    /*
    -----------------------
    Leave room for the tics
    ----------------------- */
    y = r.bottom - tic_height;

    /*
    -------------------------------------
    Leave room for one line of tic labels
    ------------------------------------- */
    y -= line_height;

    /*
    ----------------------------------------
    If there is a title, leave room for this
    ---------------------------------------- */
    if ( title.len() > 0 )
        y -= line_height;

    /*
    ---------------
    Draw the x axis
    --------------- */
    draw_line( r.left, y, r.right, y );

    /*
    ------------------------
    Save the intercept value
    ------------------------ */
    intercept = y;

    pixel_range = (r.right - r.left) - label_width/2;
    nt = nof_tics;
    if ( nt <= 0 )
        {
        nt = pixel_range;
        nt /= label_width + 2 * char_width;

        if ( type == INTEGER_AXIS_TYPE )
            minint( nt, 1 + nset );
        }

    /*
    ---------------------------------------
    Save the x positions of the min and max
    --------------------------------------- */
    min_pixel   = r.left;
    max_pixel   = r.left + pixel_range;

    /*
    -------------
    Draw the tics
    ------------- */
    y2 = y + tic_height;
    for ( i=0; i<nt; i++ )
        {
        v = nset * i;
        if ( nt > 1 )
            v /= nt-1;
        if ( v == nset )
            v--;
        if ( v >= 0 && v<nset )
            {
            x = x_pos( v );

            draw_line( x, y, x, y2 );

            s = (*this)[v].shot_name;
            s.remove_leading_chars( ZeroChar );
            TextOut( dc, x, y2, s.text(), s.len() );
            }
        }

    if ( title.len() > 0 )
        {
        SetTextAlign( dc, TA_BOTTOM | TA_CENTER );
        x = (r.left + r.right)/2;
        y = r.bottom;
        TextOut( dc, x, y, title.text(), title.len() );
        }

    SelectObject( dc, oldpen );
    DeleteObject( mypen );
    }

dc = 0;
return TRUE;
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                                X_POS                                 *
*         Given an index, return the x position to plot it at.         *
***********************************************************************/
int32 X_AXIS_CLASS::x_pos( int32 i )
{
if ( nset == 0 )
    return min_pixel;

if ( i <= 0 )
    return min_pixel;

if ( i >= nset )
    return max_pixel;

i *= pixel_range;
if ( nset > 0 )
    i /= nset;
i += min_pixel;

return i;
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                              X_INDEX                                 *
***********************************************************************/
int32 X_AXIS_CLASS::x_index( int32 x_pixel_pos )
{
int32 i;
int32 mindif;
int32 mindex;
int32 d;

if ( x_pixel_pos <= min_pixel || pixel_range == 0 )
    return 0;

if ( x_pixel_pos >= max_pixel )
    return set_count() - 1;

i = x_pixel_pos - min_pixel;
i *= nset;
i /= pixel_range;

rewind();
mindif = nset;
mindex = 0;

while ( next_set() )
    {
    d = i - x;
    if ( d < 0 )
        d = -d;

    if ( d < mindif )
        {
        mindif = d;
        mindex = x;
        }
    }

return mindex;
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                          X_POS_FROM_INDEX                            *
***********************************************************************/
int32  X_AXIS_CLASS::x_pos_from_index( int32 i )
{
if ( i< 0 )
    return min_pixel;

if ( i >= nset )
    return max_pixel;

return x_pos( i );
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                        INFO_BOX_X_POSITION                           *
***********************************************************************/
int32 X_AXIS_CLASS::info_box_x_position( int32 x )
{
int32 midpoint;

midpoint = r.right - 2 - bitmap.width();

if ( x < midpoint )
    x += 2;
else
    x -= bitmap.width() + 2;

return x;
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                          RESTORE_INFO_BOX                            *
***********************************************************************/
BOOLEAN X_AXIS_CLASS::restore_info_box( HDC dc )
{
return bitmap.restore( dc );
}

/***********************************************************************
*                            X_AXIS_CLASS                              *
*                           SHOW_INFO_BOX                              *
***********************************************************************/
BOOLEAN X_AXIS_CLASS::show_info_box( HDC dc, int32 cursor_x, int32 i, BOOLEAN need_bitmap )
{
const static COLORREF black = RGB( 0, 0, 0 );
const static COLORREF white = RGB( 255, 255, 255 );
const static TCHAR    space[] = TEXT( " " );
const static TCHAR    twospaces[] = TEXT( "  " );

int32 x;
int32 y;
HFONT old_font;
RECT  nr;
STRING_CLASS s;

x = info_box_x_position( cursor_x );
y = intercept + 2;

/*
-----------------------------------------
Copy the rectangle I am going to cover up
----------------------------------------- */
if ( need_bitmap )
    bitmap.get( dc, x, y );

/*
-----------------
Paint a rectangle
----------------- */
nr.left = x;
nr.top  = y;
nr.right  = x + bitmap.width() - 1;
nr.bottom = y + bitmap.height() - 1;

fill_rectangle( dc, nr, white );
draw_rectangle( dc, nr, black );

if ( x > cursor_x )
    {
    x += 2;
    SetTextAlign( dc, TA_TOP | TA_LEFT );
    }
else
    {
    x = nr.right - 2;
    SetTextAlign( dc, TA_TOP | TA_RIGHT );
    }

y += 2;

old_font = (HFONT) SelectObject( dc, GetStockObject(SYSTEM_FONT) );

SetTextColor( dc, black );
SetBkMode( dc, TRANSPARENT );

if ( p && i >= 0 && i < n )
    {
    s = p[i].shot_name;
    s.remove_leading_chars( ZeroChar );
    s += twospaces;
    s += alphadate( p[i].shot_time.system_time() );
    s += space;
    s += alphatime( p[i].shot_time.system_time() );
    TextOut( dc, x, y, s.text(), s.len() );
    }

SelectObject( dc, old_font );
return TRUE;
}

