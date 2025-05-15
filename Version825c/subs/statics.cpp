#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fontclas.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\wclass.h"

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

static HFONT StaticsFont = INVALID_FONT_HANDLE;

/*
--------------------------------------------
These have to be defined in the main program
-------------------------------------------- */
extern FONT_LIST_CLASS   FontList;
extern STRINGTABLE_CLASS StringTable;

/***********************************************************************
*                           STATICS_STARTUP                            *
* Don't call this until after you have loaded the resources into the   *
* StringTable.                                                         *
***********************************************************************/
void statics_startup()
{
StaticsFont = FontList.get_handle( StringTable.fontclass() );
}

/***********************************************************************
*                           STATICS_SHUTDOWN                           *
***********************************************************************/
void statics_shutdown()
{
if ( StaticsFont != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( StaticsFont );
    StaticsFont = INVALID_FONT_HANDLE;
    }
}

/***********************************************************************
*                           SET_STATICS_FONT                           *
***********************************************************************/
void set_statics_font( WINDOW_CLASS & w )
{
static LPARAM lParam = MAKELPARAM( TRUE, 0 );
if ( StaticsFont != INVALID_FONT_HANDLE )
    w.post( WM_SETFONT, (WPARAM) StaticsFont, lParam );
}

/***********************************************************************
*                           SET_STATICS_FONT                           *
***********************************************************************/
void set_statics_font( HWND hWnd, UINT id )
{
WINDOW_CLASS w;
w = GetDlgItem( hWnd, id );
set_statics_font( w );
}

/**********************************************************************
*                            UPDATE_STATICS                           *
**********************************************************************/
void update_statics( HWND hWnd, RESOURCE_TEXT_ENTRY * rlist, int n )
{
int  i;
STRING_CLASS s;
WINDOW_CLASS w;

for ( i=0; i<n; i++ )
    {
    w = GetDlgItem( hWnd, rlist[i].id );
    set_statics_font( w );
    s = StringTable.find( rlist[i].s  );
    if ( !s.isempty() )
        w.set_title( s.text() );
    }
}

/***********************************************************************
*                             SET_SETUP_STRINGS                        *
***********************************************************************/
void set_setup_strings( HWND hWnd, RESOURCE_TEXT_ENTRY * rsn, int nof_rsn, UINT * box, int nof_boxes, TCHAR * title )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

if ( title )
    {
    w = hWnd;
    w.set_title( StringTable.find(title) );
    w.refresh();
    }
}

