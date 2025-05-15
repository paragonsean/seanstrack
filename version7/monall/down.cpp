#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\events.h"
#include "..\include\setpoint.h"
#include "..\include\subs.h"

#include "extern.h"

/***********************************************************************
*                           CURRENT_OPERATOR                           *
***********************************************************************/
TCHAR * current_operator( STRING_CLASS & computer, STRING_CLASS & machine )
{
static TCHAR   opnumber[OPERATOR_NUMBER_LEN+1];
NAME_CLASS     s;
SETPOINT_CLASS sp;

lstrcpy( opnumber, NO_OPERATOR_NUMBER );

s.get_machset_csvname( computer, machine );
if ( sp.get(s) )
    copystring( opnumber, sp[MACHSET_OPERATOR_NUMBER_INDEX].value.text(), OPERATOR_NUMBER_LEN );

return opnumber;
}

/***********************************************************************
*                        ISSUE_DOWNTIME_EVENT                          *
***********************************************************************/
void issue_downtime_event( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat )
{
TCHAR * cp;
TCHAR * worker;

worker = current_operator( computer, machine );

cp = make_downtime_event_string( machine, part, st, cat, subcat, worker );
if ( cp )
    PostMessage( MainWindow, WM_POKEMON, (WPARAM) DOWN_DATA_INDEX, (LPARAM) cp );
}

/***********************************************************************
*                        APPEND_DOWNTIME_RECORD                        *
***********************************************************************/
BOOLEAN append_downtime_record( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat )
{
NAME_CLASS s;
DB_TABLE   t;
TCHAR * worker;
BOOLEAN status;

status = FALSE;

worker = current_operator( computer, machine );

s.get_downtime_dbname( computer, machine );
if ( t.open(s, DOWNTIME_RECLEN, PWL) )
    {
    make_downtime_record( t, st, cat, subcat, worker, part );
    t.rec_append();
    t.close();
    status = TRUE;
    }

issue_downtime_event( computer, machine, part, st, cat, subcat );

return status;
}
