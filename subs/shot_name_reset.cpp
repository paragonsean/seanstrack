#include <windows.h>
#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\setpoint.h"
#include "..\include\shot_name_reset.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

static TCHAR * Description[] = {
    { TEXT( "Enable" ) },
    { TEXT( "Time" ) },
    { TEXT( "Purge On Reset" ) }
    };

static TCHAR * DefaultValue[] = {
    { TEXT( "N" )    },
    { TEXT( "00:00") },
    { TEXT( "N" )    }
    };

static TCHAR AChar             = TEXT( 'A' );
static TCHAR AsterixChar       = TEXT( '*' );
static TCHAR LeftCurlyBracket  = TEXT( '{' );
static TCHAR RightCurlyBracket = TEXT( '}' );
static TCHAR NullChar          = TEXT( '\0' );
static TCHAR SChar             = TEXT( 'S' );
static TCHAR SCharLow          = TEXT( 's' );
static TCHAR ZChar             = TEXT( 'Z' );
static TCHAR ZeroChar          = TEXT( '0' );
static STRING_CLASS MyEmptyString;

/***********************************************************************
*                         SHOT_NAME_RESET_CLASS                        *
***********************************************************************/
SHOT_NAME_RESET_CLASS::SHOT_NAME_RESET_CLASS()
{
}

/***********************************************************************
*                         ~SHOT_NAME_RESET_CLASS                       *
***********************************************************************/
SHOT_NAME_RESET_CLASS::~SHOT_NAME_RESET_CLASS()
{
}

/***********************************************************************
*                                  INIT                                *
***********************************************************************/
BOOLEAN SHOT_NAME_RESET_CLASS::init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
return s.get_shot_name_reset_csvname( computer, machine, part );
}

/***********************************************************************
*                                EXISTS                                *
***********************************************************************/
BOOLEAN SHOT_NAME_RESET_CLASS::exists()
{
return s.file_exists();
}

/***********************************************************************
*                             SET_DEFAULTS                             *
***********************************************************************/
void SHOT_NAME_RESET_CLASS::set_defaults()
{
int i;
int mycount;

mycount = SETPOINT_CLASS::count();
if ( mycount < SHOT_NAME_RESET_COUNT )
    setsize( SHOT_NAME_RESET_COUNT );

for ( i=0; i<mycount; i++ )
    {
    if ( operator[](i).desc.isempty() )
        operator[](i).desc = Description[i];
    if ( operator[](i).value.isempty() )
        operator[](i).value = DefaultValue[i];
    }

for ( i=mycount; i<MIN_SHOT_NAME_SETPOINT_COUNT; i++ )
    {
    operator[](i).desc = Description[i];
    operator[](i).value = DefaultValue[i];
    }

}
/***********************************************************************
*                                  GET                                 *
***********************************************************************/
int SHOT_NAME_RESET_CLASS::get()
{
int mycount;

mycount = SETPOINT_CLASS::get( s );
if ( mycount < MIN_SHOT_NAME_SETPOINT_COUNT )
    set_defaults();

return count();
}

/***********************************************************************
*                                  PUT                                 *
***********************************************************************/
BOOLEAN SHOT_NAME_RESET_CLASS::put()
{
if (SETPOINT_CLASS::count() < SHOT_NAME_RESET_COUNT )
    set_defaults();

return SETPOINT_CLASS::put( s );
}

/***********************************************************************
*                              IS_ENABLED                              *
***********************************************************************/
BOOLEAN  SHOT_NAME_RESET_CLASS::is_enabled()
{
if ( SETPOINT_CLASS::count() > SHOT_NAME_RESET_ENABLE_INDEX )
    return operator[](SHOT_NAME_RESET_ENABLE_INDEX).value.boolean_value();
return FALSE;
}

/***********************************************************************
*                             PURGE_ON_RESET                           *
***********************************************************************/
BOOLEAN  SHOT_NAME_RESET_CLASS::purge_on_reset()
{
if ( SETPOINT_CLASS::count() > SHOT_NAME_PURGE_ON_RESET_INDEX )
    return operator[](SHOT_NAME_PURGE_ON_RESET_INDEX).value.boolean_value();
return FALSE;
}

/***********************************************************************
*                              RESET_TIME                              *
***********************************************************************/
STRING_CLASS & SHOT_NAME_RESET_CLASS::reset_time()
{
if ( SETPOINT_CLASS::count() > SHOT_NAME_RESET_TIME_INDEX )
    return operator[](SHOT_NAME_RESET_TIME_INDEX).value;
return MyEmptyString;
}

/***********************************************************************
*                              SET_ENABLE                              *
***********************************************************************/
void SHOT_NAME_RESET_CLASS::set_enable( BOOLEAN reset_is_enabled )
{
if (SETPOINT_CLASS::count() < SHOT_NAME_RESET_COUNT )
    set_defaults();
operator[](SHOT_NAME_RESET_ENABLE_INDEX).value = reset_is_enabled;
}

/***********************************************************************
*                               SET_TIME                               *
***********************************************************************/
void SHOT_NAME_RESET_CLASS::set_time( STRING_CLASS & sorc )
{
if (SETPOINT_CLASS::count() < SHOT_NAME_RESET_COUNT )
    set_defaults();
operator[](SHOT_NAME_RESET_TIME_INDEX).value = sorc;
}

/***********************************************************************
*                          SET_PURGE_ON_RESET                          *
***********************************************************************/
void SHOT_NAME_RESET_CLASS::set_purge_on_reset( BOOLEAN purge_is_enabled )
{
if (SETPOINT_CLASS::count() < SHOT_NAME_RESET_COUNT )
    set_defaults();
operator[](SHOT_NAME_PURGE_ON_RESET_INDEX).value = purge_is_enabled;
}
