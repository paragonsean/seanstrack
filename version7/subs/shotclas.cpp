#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\plookup.h"
#include "..\include\structs.h"
#include "..\include\timeclas.h"
#include "..\include\shotclas.h"
#include "..\include\subs.h"

/***********************************************************************
*                             SHOT_CLASS                               *
*                                  =                                   *
***********************************************************************/
void SHOT_CLASS::operator=( const SHOT_CLASS & sorc )
{
time   = sorc.time;
number = sorc.number;
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
    number = plookup.get_last_shot_number();
    plookup.get_time( time );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              SHOT_CLASS                              *
*                           GET_FROM_GRAPHLST                          *
***********************************************************************/
BOOLEAN SHOT_CLASS::get_from_graphlst( DB_TABLE & t )
{
SYSTEMTIME mytime;
BOOLEAN    status;

status = FALSE;
init_systemtime_struct( mytime );

number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );

if ( t.get_date(mytime, GRAPHLST_DATE_OFFSET) )
    {
    if ( t.get_time( mytime, GRAPHLST_TIME_OFFSET) )
        {
        time.set( mytime );
        status = TRUE;
        }
    }

return status;
}

/***********************************************************************
*                              SHOT_CLASS                              *
*                           GET_FROM_SHOTPARM                          *
***********************************************************************/
BOOLEAN SHOT_CLASS::get_from_shotparm( DB_TABLE & t )
{
SYSTEMTIME mytime;
BOOLEAN    status;

status = FALSE;
init_systemtime_struct( mytime );

number = t.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );

if ( t.get_date(mytime, SHOTPARM_DATE_OFFSET) )
    {
    if ( t.get_time( mytime, SHOTPARM_TIME_OFFSET) )
        {
        time.set( mytime );
        status = TRUE;
        }
    }

return status;
}

