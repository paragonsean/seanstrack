#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\plookup.h"
#include "..\include\timeclas.h"

static TCHAR * Description[] = {
    { TEXT( "Last Shot Number" ) },
    { TEXT( "Date" ) },
    { TEXT( "Time" ) }
    };

static TCHAR SpaceChar = TEXT( ' ' );

/***********************************************************************
*                              PLOOKUP_CLASS                           *
***********************************************************************/
PLOOKUP_CLASS::PLOOKUP_CLASS()
{
}

/***********************************************************************
*                              ~PLOOKUP_CLASS                          *
***********************************************************************/
PLOOKUP_CLASS::~PLOOKUP_CLASS()
{
}

/***********************************************************************
*                              PLOOKUP_CLASS                           *
*                                  INIT                                *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
return s.get_plookup_csvname( computer, machine, part );
}

/***********************************************************************
*                              PLOOKUP_CLASS                           *
*                                EXISTS                                *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::exists()
{
return s.file_exists();
}

/***********************************************************************
*                              PLOOKUP_CLASS                           *
*                                  GET                                 *
***********************************************************************/
int PLOOKUP_CLASS::get()
{
return SETPOINT_CLASS::get( s );
}

/***********************************************************************
*                              PLOOKUP_CLASS                           *
*                                  PUT                                 *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::put()
{
int        i;
TIME_CLASS t;

setsize( PLOOKUP_SETPOINT_COUNT );
for( i=0; i<PLOOKUP_SETPOINT_COUNT; i++ )
    {
    if ( operator[](i).desc.isempty() )
        operator[](i).desc = Description[i];
    }

if ( operator[]( PLOOKUP_LAST_SHOT_INDEX ).value.isempty() )
    operator[]( PLOOKUP_LAST_SHOT_INDEX ).value = 0;

if ( operator[]( PLOOKUP_DATE_INDEX ).value.isempty() || operator[]( PLOOKUP_TIME_INDEX ).value.isempty() )
    {
    t.get_local_time();
    operator[]( PLOOKUP_DATE_INDEX ).value = t.mmddyyyy();
    operator[]( PLOOKUP_TIME_INDEX ).value = t.hhmmss();
    }

return SETPOINT_CLASS::put( s );
}

/***********************************************************************
*                         SET_LAST_SHOT_NUMBER                         *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::set_last_shot_number( int new_shot_number )
{
if ( count() > PLOOKUP_LAST_SHOT_INDEX )
    {
    operator[]( PLOOKUP_LAST_SHOT_INDEX ).value = new_shot_number;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                         SET_LAST_SHOT_NUMBER                         *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::set_last_shot_number( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, int new_shot_number )
{
init( computer, machine, part );
get();
set_last_shot_number( new_shot_number );
return put();
}

/***********************************************************************
*                          GET_LAST_SHOT_NUMBER                        *
***********************************************************************/
int PLOOKUP_CLASS::get_last_shot_number( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
if ( init(computer, machine, part) )
    {
    get();
    return operator[](PLOOKUP_LAST_SHOT_INDEX).value.int_value();
    }
return 0;
}

/***********************************************************************
*                          GET_LAST_SHOT_NUMBER                        *
***********************************************************************/
int PLOOKUP_CLASS::get_last_shot_number()
{
if ( count() > PLOOKUP_LAST_SHOT_INDEX )
    return operator[](PLOOKUP_LAST_SHOT_INDEX).value.int_value();

return 0;
}

/***********************************************************************
*                                SET_TIME                              *
***********************************************************************/
BOOLEAN  PLOOKUP_CLASS::set_time( TIME_CLASS & sorc )
{
if ( count() > PLOOKUP_TIME_INDEX )
    {
    operator[]( PLOOKUP_DATE_INDEX ).value = sorc.mmddyyyy();
    operator[]( PLOOKUP_TIME_INDEX ).value = sorc.hhmmss();
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                                SET_TIME                              *
***********************************************************************/
BOOLEAN  PLOOKUP_CLASS::set_time( SYSTEMTIME & sorc )
{
TCHAR s[ALPHADATE_LEN+1];
if ( count() > PLOOKUP_TIME_INDEX )
    {
    alphadate( s, sorc );
    operator[]( PLOOKUP_DATE_INDEX ).value = s;

    alphatime( s, sorc );
    operator[]( PLOOKUP_TIME_INDEX ).value = s;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                                GET_TIME                              *
***********************************************************************/
BOOLEAN  PLOOKUP_CLASS::get_time( TIME_CLASS & dest )
{
TCHAR   s[ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 + 20];
TCHAR * cp;

if ( count() > PLOOKUP_TIME_INDEX )
    {
    cp = copy_w_char( s, operator[]( PLOOKUP_DATE_INDEX ).value.text(), SpaceChar );
    copystring( cp, operator[]( PLOOKUP_TIME_INDEX ).value.text() );
    dest = s;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                                GET_TIME                              *
***********************************************************************/
BOOLEAN  PLOOKUP_CLASS::get_time( SYSTEMTIME & dest )
{
if ( count() > PLOOKUP_TIME_INDEX )
    {
    extmdy( dest, operator[]( PLOOKUP_DATE_INDEX ).value.text() );
    exthms( dest, operator[]( PLOOKUP_TIME_INDEX ).value.text() );
    return TRUE;
    }

return FALSE;
}
