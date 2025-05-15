#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

static TCHAR SpaceChar = TEXT( ' ' );

/***********************************************************************
*                            FILL_SHOT_LISTBOX                         *
***********************************************************************/
void fill_shot_listbox( HWND w, int listbox_id, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
int          max_shot_name_len;
HCURSOR      old_cursor;
NAME_CLASS   s;
DB_TABLE     t;
STRING_CLASS temp;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

max_shot_name_len = shot_name_length( computer, machine, part );
SendDlgItemMessage( w, listbox_id, LB_RESETCONTENT, TRUE, 0L );
SendDlgItemMessage( w, listbox_id, WM_SETREDRAW, 0,  0l );

s.get_graphlst_dbname( computer, machine, part );
if ( s.file_exists() )
    {
    if ( t.open(s, 0, PFL) )
        {
        while ( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( s, GRAPHLST_SHOT_NAME_INDEX );
            s.rjust( max_shot_name_len );
            s += SpaceChar;

            t.get_alpha( temp, GRAPHLST_DATE_INDEX );
            s += temp;
            s += SpaceChar;

            t.get_alpha( temp, GRAPHLST_TIME_INDEX );
            s += temp;

            SendDlgItemMessage( w, listbox_id, LB_ADDSTRING, 0, (LPARAM) s.text() );
            }
        t.close();
        }
    }

SendDlgItemMessage( w, listbox_id, LB_SETCURSEL, 0, 0L);
SendDlgItemMessage( w, listbox_id, WM_SETREDRAW, 1,  0l );
InvalidateRect( GetDlgItem(w, listbox_id), NULL, TRUE );

SetCursor( old_cursor );
}
