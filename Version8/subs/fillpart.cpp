#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\vdbclass.h"

static TCHAR NullChar  = TEXT( '\0' );
static TCHAR SpaceChar = TEXT( ' ' );
static TCHAR TabChar   = TEXT( '\t' );

/***********************************************************************
*                            FILL_PART_LISTBOX                         *
***********************************************************************/
void fill_part_listbox( HWND w, int box, STRING_CLASS & computer, STRING_CLASS & machine )
{
HCURSOR  old_cursor;
LISTBOX_CLASS lb;
TEXT_LIST_CLASS t;

if ( !lb.init(w, box) )
    return;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

lb.redraw_off();
lb.empty();

if ( get_partlist(t, computer, machine) )
    {
    while ( t.next() )
        {
        lb.add( t.text() );
        }
    }

lb.setcursel( 0 );
lb.redraw_on();

SetCursor( old_cursor );
}

/***********************************************************************
*                      MAKE_PART_AND_COUNTS_STRING                     *
***********************************************************************/
BOOLEAN make_part_and_counts_string( STRING_CLASS & dest, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, TCHAR separator, BOOLEAN need_parms  )
{
int32        n;
STRING_CLASS buf;
NAME_CLASS   s;
DB_TABLE     t;
VDB_CLASS    v;

dest  = part;
dest += separator;

if ( need_parms )
    {
    n = 0;
    s.get_shotparm_csvname( computer, machine, part );
    if ( s.file_exists() )
        {
        if ( v.open_for_read(s) )
            {
            n = v.nof_recs();
            v.close();
            }
        }

    if ( separator == TabChar )
        {
        dest.cat_w_char( int32toasc(n), separator );
        }
    else
        {
        buf= n;
        if ( buf.len() < SHOT_LEN )
            buf.rjust( SHOT_LEN );
        dest.cat_w_char( buf, separator );
        }
    }

n = 0;
s.get_graphlst_dbname( computer, machine, part );
if ( s.file_exists() )
    {
    if ( t.open(s, 0, PFL) )
        {
        n = t.nof_recs();
        t.close();
        }
    }

if ( separator == TabChar )
    {
    dest += n;
    }
else
    {
    buf= n;
    if ( buf.len() < SHOT_LEN )
        buf.rjust( SHOT_LEN );
    dest += buf;
    }

return TRUE;
}

/***********************************************************************
*                     FILL_PARTS_AND_COUNTS_LISTBOX                    *
***********************************************************************/
static void fill_parts_and_counts_listbox( HWND w, int box, STRING_CLASS & computer, STRING_CLASS & machine, TCHAR separator, BOOLEAN need_parms )
{
STRING_CLASS    part;
STRING_CLASS    s;
HCURSOR         old_cursor;
LISTBOX_CLASS   lb;
TEXT_LIST_CLASS t;

if ( !lb.init(w, box) )
    return;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

lb.redraw_off();
lb.empty();

if ( get_partlist(t, computer, machine) )
    {
    while ( t.next() )
        {
        part = t.text();
        if ( make_part_and_counts_string(s, computer, machine, part, separator, need_parms) )
            lb.add( s.text() );
        }
    }

lb.setcursel( 0 );
lb.redraw_on();

SetCursor( old_cursor );
}

/***********************************************************************
*                    FILL_PARTS_AND_COUNTS_LISTBOX                     *
***********************************************************************/
void fill_parts_and_counts_listbox( HWND w, int box, STRING_CLASS & computer, STRING_CLASS & machine, TCHAR separator )
{
fill_parts_and_counts_listbox( w, box, computer, machine, separator, TRUE );
}

/***********************************************************************
*                     FILL_PARTS_AND_COUNTS_LISTBOX                    *
***********************************************************************/
void fill_parts_and_counts_listbox( HWND w, int box, STRING_CLASS & computer, STRING_CLASS & machine )
{
fill_parts_and_counts_listbox( w, box, computer, machine, SpaceChar, TRUE );
}

/***********************************************************************
*                   FILL_PARTS_AND_SHOT_COUNT_LISTBOX                  *
***********************************************************************/
void fill_parts_and_shot_count_listbox( HWND w, int box, STRING_CLASS & computer, STRING_CLASS & machine )
{
fill_parts_and_counts_listbox( w, box, computer, machine, TabChar, FALSE );
}
