#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\plookup.h"
#include "..\include\timeclas.h"

static TCHAR * Description[] = {
    { TEXT( "Last Shot Name" ) },
    { TEXT( "Last Shot Number" ) },
    { TEXT( "Date" ) },
    { TEXT( "Time" ) },
    { TEXT( "Update Status" ) }
    };

static TCHAR AChar     = TEXT( 'A' );
static TCHAR NullChar  = TEXT( '\0' );
static TCHAR SpaceChar = TEXT( ' ' );
static TCHAR ZChar     = TEXT( 'Z' );
static TCHAR ZeroChar  = TEXT( '0' );

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
int i;
int j;
int mycount;
SETPOINT_CLASS sp;

mycount = SETPOINT_CLASS::get( s );
if ( mycount < PLOOKUP_SETPOINT_COUNT )
    {
    i = 0;
    j = 0;
    sp = *this;
    setsize( PLOOKUP_SETPOINT_COUNT );
    if ( sp[j].desc != Description[PLOOKUP_LAST_SHOT_NAME_INDEX] )
        {
        operator[](i).desc = Description[PLOOKUP_LAST_SHOT_NAME_INDEX];
        operator[](i).value.null();
        i++;
        }
    while ( j < mycount )
        {
        operator[](i) = sp[j];
        i++;
        j++;
        }
    if ( i < (PLOOKUP_SETPOINT_COUNT-1) )
        {
        operator[](i).desc = Description[i];
        operator[](i).value = 0;
        }
    }

/*
-------------------------------------------------------
If this is an old plookup table the name will be empty.
------------------------------------------------------- */
if ( operator[](PLOOKUP_LAST_SHOT_NAME_INDEX).value.isempty() )
    {
    operator[](PLOOKUP_LAST_SHOT_NAME_INDEX).value = operator[](PLOOKUP_LAST_SHOT_NUMBER_INDEX).value;
    operator[](PLOOKUP_LAST_SHOT_NAME_INDEX).value.rjust( SHOT_LEN, ZeroChar );
    }
return count();
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

if ( operator[]( PLOOKUP_LAST_SHOT_NUMBER_INDEX ).value.isempty() )
    operator[]( PLOOKUP_LAST_SHOT_NUMBER_INDEX ).value = 0;

if ( operator[](PLOOKUP_LAST_SHOT_NAME_INDEX).value.isempty() )
    {
    operator[](PLOOKUP_LAST_SHOT_NAME_INDEX).value = operator[](PLOOKUP_LAST_SHOT_NUMBER_INDEX).value;
    operator[](PLOOKUP_LAST_SHOT_NAME_INDEX).value.rjust(SHOT_LEN);
    }

if ( operator[]( PLOOKUP_DATE_INDEX ).value.isempty() || operator[]( PLOOKUP_TIME_INDEX ).value.isempty() )
    {
    t.get_local_time();
    operator[]( PLOOKUP_DATE_INDEX ).value = t.mmddyyyy();
    operator[]( PLOOKUP_TIME_INDEX ).value = t.hhmmss();
    }

return SETPOINT_CLASS::put( s );
}

/***********************************************************************
*                         SET_LAST_SHOT_NAME                           *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::set_last_shot_name( STRING_CLASS & new_shot_name )
{
if ( count() > PLOOKUP_LAST_SHOT_NAME_INDEX )
    {
    operator[]( PLOOKUP_LAST_SHOT_NAME_INDEX ).value = new_shot_name;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                         SET_LAST_SHOT_NAME                           *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::set_last_shot_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, STRING_CLASS & new_shot_name )
{
init( computer, machine, part );
get();
set_last_shot_name( new_shot_name );
return put();
}

/***********************************************************************
*                           SET_LAST_SHOT_NUMBER                       *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::set_last_shot_number( int new_shot_number )
{
if ( count() > PLOOKUP_LAST_SHOT_NUMBER_INDEX )
    {
    operator[](PLOOKUP_LAST_SHOT_NUMBER_INDEX).value = new_shot_number;
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
*                           GET_LAST_SHOT_NAME                         *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::get_last_shot_name( STRING_CLASS & dest, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
if ( init(computer, machine, part) )
    {
    get();
    dest = operator[](PLOOKUP_LAST_SHOT_NAME_INDEX).value;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           GET_LAST_SHOT_NAME                         *
***********************************************************************/
BOOLEAN PLOOKUP_CLASS::get_last_shot_name( STRING_CLASS & dest )
{
if ( count() > PLOOKUP_LAST_SHOT_NAME_INDEX )
    {
    dest = operator[](PLOOKUP_LAST_SHOT_NAME_INDEX).value;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          GET_LAST_SHOT_NUMBER                        *
***********************************************************************/
int PLOOKUP_CLASS::get_last_shot_number( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
if ( init(computer, machine, part) )
    {
    get();
    return operator[](PLOOKUP_LAST_SHOT_NUMBER_INDEX).value.int_value();
    }

return NO_SHOT_NUMBER;
}

/***********************************************************************
*                           GET_LAST_SHOT_NUMBER                       *
***********************************************************************/
int PLOOKUP_CLASS::get_last_shot_number()
{
if ( count() > PLOOKUP_LAST_SHOT_NUMBER_INDEX )
    {
    return operator[](PLOOKUP_LAST_SHOT_NUMBER_INDEX).value.int_value();
    }

return NO_SHOT_NUMBER;
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
