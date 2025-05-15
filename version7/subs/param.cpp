#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"
#include "..\include\param.h"
#include "..\include\verrors.h"

static TCHAR PChar = TEXT( 'P' );
static TCHAR PCharLow = TEXT( 'p' );
static TCHAR NullChar = TEXT( '\0' );

/***********************************************************************
*                          PARAMETER_ENTRY                             *
*                                =                                     *
***********************************************************************/
void PARAMETER_ENTRY::operator=( const PARAMETER_ENTRY & sorc )
{
int i;

lstrcpy( name, sorc.name );
input.type   = sorc.input.type;
input.number = sorc.input.number;

for ( i=0; i<NOF_ALARM_LIMIT_TYPES; i++ )
    {
    limits[i].value       = sorc.limits[i].value;
    limits[i].wire_number = sorc.limits[i].wire_number;
    }

vartype = sorc.vartype;
units   = sorc.units;
}

/***********************************************************************
*                          PARAMETER_ENTRY                             *
*                         HAS_WARNING_LIMITS                           *
***********************************************************************/
BOOLEAN PARAMETER_ENTRY::has_warning_limits()
{
return not_float_zero(limits[WARNING_MAX].value-limits[WARNING_MIN].value);
}

/***********************************************************************
*                          PARAMETER_ENTRY                             *
*                          HAS_ALARM_LIMITS                            *
***********************************************************************/
BOOLEAN PARAMETER_ENTRY::has_alarm_limits()
{
return not_float_zero(limits[ALARM_MAX].value-limits[ALARM_MIN].value);
}

/***********************************************************************
*                              IS_OUT                                  *
***********************************************************************/
static BOOLEAN is_out( float value, float low, float high )
{
/*
--------------------------------------
If the two limits are equal do nothing
-------------------------------------- */
if ( is_float_zero(high-low) )
    return FALSE;

/*
--------------------------------------
If I equal the limit I am NOT in alarm
-------------------------------------- */
if ( is_float_zero(value-low) || is_float_zero(high-value) )
    return FALSE;

if ( value < low || value > high )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                               IS_ALARM                               *
***********************************************************************/
BOOLEAN PARAMETER_ENTRY::is_alarm( float value )
{
return is_out( value, limits[ALARM_MIN].value, limits[ALARM_MAX].value );
}

/***********************************************************************
*                              IS_WARNING                              *
***********************************************************************/
BOOLEAN PARAMETER_ENTRY::is_warning( float value )
{
return is_out( value, limits[WARNING_MIN].value, limits[WARNING_MAX].value );
}

/***********************************************************************
*                        PARAMETER_CLASS::COUNT                        *
***********************************************************************/
short PARAMETER_CLASS::count()
{
short i;

i = MAX_PARMS -1;
while ( i >= 0 )
    {
    if ( parameter[i].input.type != NO_PARAMETER_TYPE )
        return i+1;
    i--;
    }

return 0;
}

/**********************************************************************
*                        get_type_and_units                           *
* This is for FT_CALC_INPUT parameters only. The units are assumed    *
* to be the same as the units of the first parameter in the command   *
* string.                                                             *
**********************************************************************/
BOOLEAN PARAMETER_CLASS::get_type_and_units( short i, TCHAR * cmd )
{
static TCHAR IstName[] = IST_NAME;
short   sorc_index;
BOOLEAN have_change;

have_change = FALSE;

if ( parameter[i].input.type != FT_CALC_INPUT )
    return FALSE;

while ( true )
    {
    if ( *cmd == NullChar )
        break;

    if ( *cmd == PChar || *cmd == PCharLow )
        {
        cmd++;
        sorc_index = (short) asctoint32( cmd );
        if ( sorc_index > 0 && sorc_index <= MAX_PARMS )
            {
            sorc_index--;
            if ( parameter[i].vartype != parameter[sorc_index].vartype )
                {
                parameter[i].vartype = parameter[sorc_index].vartype;
                have_change = TRUE;
                }
            if ( parameter[i].units != parameter[sorc_index].units )
                {
                parameter[i].units = parameter[sorc_index].units;
                have_change = TRUE;
                }
            return have_change;
            }
        }
    else if ( start_chars_are_equal(IstName, cmd) )
        {
        if ( parameter[i].vartype != TIME_VAR )
            {
            parameter[i].vartype = TIME_VAR;
            have_change = TRUE;
            }

        if ( parameter[i].units != MS_UNITS )
            {
            parameter[i].units   = MS_UNITS;
            have_change = TRUE;
            }
        return have_change;
        }
    cmd++;
    }
return FALSE;
}

/***********************************************************************
*                        PARAMETER_CLASS::INIT                         *
***********************************************************************/
void PARAMETER_CLASS::init( short i )
{
int w;

if ( i<0 || i>=MAX_PARMS )
    return;


lstrcpy( parameter[i].name, NO_PARAMETER_NAME );
parameter[i].input.type   = NO_PARAMETER_TYPE;
parameter[i].input.number = 0;
parameter[i].vartype      = NO_VARIABLE_TYPE;
parameter[i].units        = NO_UNITS;
for ( w=0; w<NOF_ALARM_LIMIT_TYPES; w++ )
    {
    parameter[i].limits[w].value = 0.0;
    parameter[i].limits[w].wire_number = NO_WIRE;
    }
}

/***********************************************************************
*                    PARAMETER_CLASS::REMOVE                           *
***********************************************************************/
BOOLEAN PARAMETER_CLASS::remove( short i )
{
short n;

if ( i<0 || i>=MAX_PARMS )
    return FALSE;

n = MAX_PARMS - 1;
while ( i < n)
    {
    parameter[i] = parameter[i+1];
    i++;
    }

init( i );
return TRUE;
}

/***********************************************************************
*                   PARAMETER_CLASS::PARAMETER_CLASS                   *
***********************************************************************/
PARAMETER_CLASS::PARAMETER_CLASS()
{
short i;

for ( i=0; i<MAX_PARMS; i++ )
    init( i );
}

/***********************************************************************
*                      PARAMETER_CLASS::READ_PARAM_DB                  *
***********************************************************************/
BOOLEAN PARAMETER_CLASS::read_param_db( STRING_CLASS & filename )
{
short    a;
short    i;
FIELDOFFSET offset;
DB_TABLE t;

for ( i=0; i<MAX_PARMS; i++ )
    init( i );

if ( !file_exists(filename.text()) )
    return FALSE;

if ( t.open( filename, PARMLIST_RECLEN, PFL) )
    {
    while( t.get_next_record(NO_LOCK) )
        {
        i = t.get_short( PARMLIST_PARM_NUMBER_OFFSET );
        if ( i > 0 && i <= MAX_PARMS )
            {
            i--;
            t.get_alpha( parameter[i].name, PARMLIST_DESC_OFFSET, PARAMETER_NAME_LEN );
            parameter[i].input.type   = t.get_short( PARMLIST_INPUT_TYPE_OFFSET );
            parameter[i].input.number = t.get_short( PARMLIST_INPUT_NUMBER_OFFSET );
            parameter[i].vartype      = t.get_long(  PARMLIST_VARIABLE_TYPE_OFFSET );
            parameter[i].units        = t.get_short( PARMLIST_UNITS_OFFSET );
            for ( a=0; a<NOF_ALARM_LIMIT_TYPES; a++ )
                {
                switch ( a )
                    {
                    case ALARM_MIN:
                        offset = PARMLIST_LOW_ALARM_LIMIT_OFFSET;
                        break;
                    case ALARM_MAX:
                        offset = PARMLIST_HIGH_ALARM_LIMIT_OFFSET;
                        break;
                    case WARNING_MIN:
                        offset = PARMLIST_LOW_WARN_LIMIT_OFFSET;
                        break;
                    default:
                        offset = PARMLIST_HIGH_WARN_LIMIT_OFFSET;
                        break;
                    }

                parameter[i].limits[a].value = t.get_float( offset );
                offset += PARMLIST_FLOAT_LEN + 1;
                parameter[i].limits[a].wire_number = t.get_short( offset );
                }
            }
        }

    t.close();

    if ( t.get_global_error() == VS_SUCCESS )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              SHOT_GOODNESS                           *
* Db points to a DB_TABLE class that contains the record of the        *
* shot you are interested in. I will look at the values and return     *
* ALARM_SHOT if at least one parameter is in alarm, WARNING_SHOT if at *
* least one parameter is in warning and there are no alarms, and       *
* GOOD_SHOT if all parameters are within spec.                         *
***********************************************************************/
int PARAMETER_CLASS::shot_goodness( DB_TABLE & db )
{
int   i;
int   type;
float x;
FIELDOFFSET offset;

type = GOOD_SHOT;
offset = SHOTPARM_PARAM_1_OFFSET;
for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( parameter[i].input.type == NO_PARAMETER_TYPE )
        break;
    x = db.get_float( offset );
    if ( parameter[i].is_alarm(x) )
        return ALARM_SHOT;
    if ( parameter[i].is_warning(x) )
        type = WARNING_SHOT;
    offset += SHOTPARM_FLOAT_LEN+1;
    }

return type;
}

/***********************************************************************
*                         PARAMETER_CLASS::FIND                        *
***********************************************************************/
BOOLEAN PARAMETER_CLASS::find( STRING_CLASS & computer_to_find, STRING_CLASS & machine_to_find, STRING_CLASS & part_to_find )
{
NAME_CLASS s;
computer = computer_to_find;
machine  = machine_to_find;
part     = part_to_find;

s.get_parmlist_dbname( computer_to_find, machine_to_find, part_to_find );
return read_param_db( s );
}

/***********************************************************************
*                          PARAMETER_CLASS::SAVE                       *
***********************************************************************/
BOOLEAN PARAMETER_CLASS::save()
{
short       a;
short       i;
FIELDOFFSET offset;
DB_TABLE    t;
NAME_CLASS  tn;

tn.get_parmlist_dbname( computer, machine, part );
t.ensure_existance( tn );

if ( t.open(tn, PARMLIST_RECLEN, FL) )
    {
    t.empty();
    for ( i=0; i<MAX_PARMS; i++ )
        {
        if ( parameter[i].input.type == NO_PARAMETER_TYPE )
            continue;

        t.put_short( PARMLIST_PARM_NUMBER_OFFSET,   i+1, PARAMETER_NUMBER_LEN );
        t.put_short( PARMLIST_INPUT_TYPE_OFFSET,    parameter[i].input.type, PARMLIST_INPUT_TYPE_LEN );
        t.put_short( PARMLIST_INPUT_NUMBER_OFFSET,  parameter[i].input.number, INPUT_NUMBER_LEN );
        t.put_long(  PARMLIST_VARIABLE_TYPE_OFFSET, parameter[i].vartype, BITSETYPE_LEN );
        t.put_short( PARMLIST_UNITS_OFFSET,         parameter[i].units,   UNITS_ID_LEN );
        t.put_alpha( PARMLIST_DESC_OFFSET,          parameter[i].name,    PARAMETER_NAME_LEN );
        for ( a=0; a<NOF_ALARM_LIMIT_TYPES; a++ )
            {
            switch ( a )
                {
                case ALARM_MIN:
                    offset = PARMLIST_LOW_ALARM_LIMIT_OFFSET;
                    break;
                case ALARM_MAX:
                    offset = PARMLIST_HIGH_ALARM_LIMIT_OFFSET;
                    break;
                case WARNING_MIN:
                    offset = PARMLIST_LOW_WARN_LIMIT_OFFSET;
                    break;
                default:
                    offset = PARMLIST_HIGH_WARN_LIMIT_OFFSET;
                    break;
                }

            t.put_float( offset, parameter[i].limits[a].value, PARMLIST_FLOAT_LEN );
            offset += PARMLIST_FLOAT_LEN + 1;
            t.put_short( offset, parameter[i].limits[a].wire_number, WIRE_LEN );
            }
        t.rec_append();
        }
    t.close();
    }

if ( t.get_global_error() == VS_SUCCESS )
    return TRUE;

MessageBox( 0, tn.text(), TEXT("Part Setup: Unable to save"), MB_OK | MB_APPLMODAL );
return FALSE;
}
