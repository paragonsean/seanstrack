#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

/***********************************************************************
*                            FILL_SHOT_LISTBOX                         *
***********************************************************************/
void fill_shot_listbox( HWND w, int listbox_id, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
SYSTEMTIME st;
NAME_CLASS tn;
DB_TABLE   t;
TCHAR    * cp;
TCHAR      s[SHOT_LEN+1+ALPHADATE_LEN+1+ALPHATIME_LEN+1];
HCURSOR    old_cursor;
long       shot_number;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

SendDlgItemMessage( w, listbox_id, LB_RESETCONTENT, TRUE, 0L );
SendDlgItemMessage( w, listbox_id, WM_SETREDRAW, 0,  0l );

tn.get_graphlst_dbname( computer, machine, part );
if ( tn.file_exists() )
    {
    if ( t.open(tn, GRAPHLST_RECLEN, PFL) )
        {
        while ( t.get_next_record(NO_LOCK) )
            {
            cp = s;

            shot_number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
            insalph( cp, shot_number, SHOT_LEN, TEXT('0'), DECIMAL_RADIX );
            cp += SHOT_LEN;
            *cp = TEXT( ' ' );
            cp++;

            t.get_date( st, GRAPHLST_DATE_OFFSET );
            copychars( cp, alphadate(st), ALPHADATE_LEN );
            cp += ALPHADATE_LEN;
            *cp = TEXT( ' ' );
            cp++;

            t.get_time( st, GRAPHLST_TIME_OFFSET );
            copychars( cp, alphatime(st), ALPHATIME_LEN );
            cp += ALPHATIME_LEN;
            *cp = TEXT( '\0');
            SendDlgItemMessage( w, listbox_id, LB_ADDSTRING, 0, (LPARAM) s );
            }
        t.close();
        }
    }

SendDlgItemMessage( w, listbox_id, LB_SETCURSEL, 0, 0L);
SendDlgItemMessage( w, listbox_id, WM_SETREDRAW, 1,  0l );
InvalidateRect( GetDlgItem(w, listbox_id), NULL, TRUE );

SetCursor( old_cursor );
}
