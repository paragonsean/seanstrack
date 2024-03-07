#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\setpoint.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

/***********************************************************************
*                           CURRENT_PART_NAME                          *
***********************************************************************/
BOOLEAN current_part_name( STRING_CLASS & dest, STRING_CLASS & computer, STRING_CLASS & machine )
{
NAME_CLASS s;
BOOLEAN    status;
SETPOINT_CLASS sp;

status = FALSE;
s.get_machset_csvname( computer, machine );

if ( s.file_exists() )
    {
    if ( sp.get(s) )
        {
        dest = sp[MACHSET_CURRENT_PART_INDEX].value;
        status = TRUE;
        }
    }

if ( !status )
    dest.null();

return status;
}

/***********************************************************************
*                        SET_CURRENT_PART_NAME                         *
***********************************************************************/
BOOLEAN set_current_part_name( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
NAME_CLASS s;
BOOLEAN    status;
SETPOINT_CLASS sp;

status = FALSE;

s.get_machset_csvname( co, ma );

if ( s.file_exists() )
    {
    if ( sp.get(s) )
        {
        sp[MACHSET_CURRENT_PART_INDEX].value = pa;
        sp.put(s);
        status = TRUE;
        }
    }

return status;
}

/***********************************************************************
*                        SET_CURRENT_PART_NAME                         *
***********************************************************************/
BOOLEAN set_current_part_name( PART_NAME_ENTRY & p )
{
return set_current_part_name( p.computer, p.machine, p.part);
}
