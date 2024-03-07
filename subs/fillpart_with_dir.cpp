#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"

#include "..\include\chars.h"

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
*                       COUNT_SHOT_PARMS_IN_ONE_DIR                    *
*    Count the shot parameters in this directory and all subdirs.      *
***********************************************************************/
void count_shot_parms_in_one_dir( int & count, STRING_CLASS & sorcdir )
{
TCHAR         * cp;
STRING_CLASS    d;
NAME_CLASS      s;
DB_TABLE        t;
HANDLE          fh;
WIN32_FIND_DATA fdata;

s = sorcdir;
s.cat_path( SHOTPARM_DB );

if ( s.file_exists() )
    {
    if ( t.open(s, 0, PFL) )
        {
        count += t.nof_recs();
        t.close();
        }
    }

s = sorcdir;
s.cat_path( StarDotStar );

fh = FindFirstFile( s.text(), &fdata );
if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( true )
        {
        cp = fdata.cFileName;
        if ( *cp != PeriodChar )
            {
            if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                d = sorcdir;
                d.cat_path( cp );
                count_shot_parms_in_one_dir( count, d );
                }
            }
        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );
    }
}

/***********************************************************************
*                            SHOTPARM_COUNT                            *
*    Count the shot parameters in this directory and all subdirs.      *
***********************************************************************/
int shotparm_count( STRING_CLASS & sorcdir )
{
int count;

count = 0;
count_shot_parms_in_one_dir( count, sorcdir );

return count;
}

/***********************************************************************
*                      MAKE_PART_AND_COUNTS_STRING                     *
***********************************************************************/
BOOLEAN make_part_and_counts_string( STRING_CLASS & dest, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, TCHAR separator, BOOLEAN need_parms  )
{
STRING_CLASS count;
NAME_CLASS   s;

dest  = part;
dest += separator;

s.get_part_results_directory( computer, machine, part );

if ( need_parms )
    {
    count = shotparm_count( s );
    if ( separator != TabChar )
        count.rjust( SHOT_LEN );
    dest.cat_w_char( count, separator );
    }

count = shot_count( s );
if ( separator != TabChar )
    count.rjust( SHOT_LEN );
dest += count;

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
