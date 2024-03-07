#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\plookup.h"
#include "..\include\structs.h"
#include "..\include\timeclas.h"
#include "..\include\shotclas.h"
#include "..\include\subs.h"
#include "..\include\vdbclass.h"

/***********************************************************************
*                             SHOT_CLASS                               *
*                                  =                                   *
***********************************************************************/
void SHOT_CLASS::operator=( const SHOT_CLASS & sorc )
{
time = sorc.time;
name = sorc.name;
}

/***********************************************************************
*                             SHOT_CLASS                               *
*                             GET_CURRENT                              *
***********************************************************************/
BOOLEAN SHOT_CLASS::get_current( PART_NAME_ENTRY & p )
{
PLOOKUP_CLASS plookup;
BOOLEAN    status;

status      = FALSE;

plookup.init( p.computer, p.machine, p.part );
if ( plookup.get() )
    {
    plookup.get_last_shot_name( name );
    plookup.get_time( time );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              SHOT_CLASS                              *
*                           GET_FROM_GRAPHLST                          *
* This assumes that graphlst has been opened with a zero reclen so     *
* that it calculates the field offsets.                                *
***********************************************************************/
BOOLEAN SHOT_CLASS::get_from_graphlst( DB_TABLE & t )
{
SYSTEMTIME mytime;
BOOLEAN    status;
FIELDOFFSET fo;

status = FALSE;
init_systemtime_struct( mytime );

if ( t.get_alpha(name, GRAPHLST_SHOT_NAME_INDEX) )
    {
    if ( !t.is_alpha(GRAPHLST_SHOT_NAME_INDEX) )
        fix_shotname( name );
    fo = t.field_offset( GRAPHLST_DATE_INDEX );
    if ( t.get_date(mytime, fo) )
        {
        fo = t.field_offset( GRAPHLST_TIME_INDEX );
        if ( t.get_time( mytime, fo) )
            {
            time.set( mytime );
            status = TRUE;
            }
        }
    }

return status;
}

/***********************************************************************
*                              SHOT_CLASS                              *
*                           GET_FROM_SHOTPARM                          *
***********************************************************************/
BOOLEAN SHOT_CLASS::get_from_shotparm( VDB_CLASS & t )
{
SYSTEMTIME mytime;

init_systemtime_struct( mytime );

name = t.ra[SHOTPARM_SHOT_NAME_INDEX];
if ( name.isempty() )
    return FALSE;

t.get_date( mytime, SHOTPARM_DATE_INDEX );
t.get_time( mytime, SHOTPARM_TIME_INDEX );
time.set( mytime );

return TRUE;
}

