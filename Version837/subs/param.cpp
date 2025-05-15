#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\vdbclass.h"
#include "..\include\subs.h"
#include "..\include\param.h"
#include "..\include\verrors.h"

static TCHAR CommaChar = TEXT( ',' );
static TCHAR PChar     = TEXT( 'P' );
static TCHAR PCharLow  = TEXT( 'p' );
static TCHAR NullChar  = TEXT( '\0' );

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
    limits[i].svalue      = sorc.limits[i].svalue;
    limits[i].value       = sorc.limits[i].value;
    limits[i].wire_number = sorc.limits[i].wire_number;
    }
static_text = sorc.static_text;
vartype     = sorc.vartype;
units       = sorc.units;
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

if ( parameter[i].input.type == FT_STATIC_TEXT_PARAMETER )
    parameter[i].static_text.clear();
parameter[i].input.type   = NO_PARAMETER_TYPE;
parameter[i].input.number = 0;
parameter[i].vartype      = NO_VARIABLE_TYPE;
parameter[i].units        = NO_UNITS;
for ( w=0; w<NOF_ALARM_LIMIT_TYPES; w++ )
    {
    parameter[i].limits[w].svalue.empty();
    parameter[i].limits[w].value = 0.0;
    parameter[i].limits[w].wire_number = NO_WIRE;
    }
parameter[i].target.null();
}

/***********************************************************************
*                    PARAMETER_CLASS::REMOVE                           *
***********************************************************************/
BOOLEAN PARAMETER_CLASS::remove( short index_to_remove )
{
short n;

if ( index_to_remove<0 || index_to_remove>=MAX_PARMS )
    return FALSE;

n = MAX_PARMS - 1;
while ( index_to_remove < n)
    {
    parameter[index_to_remove] = parameter[index_to_remove+1];
    index_to_remove++;
    }

init( index_to_remove );
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
TCHAR    text_value[PARMLIST_FLOAT_LEN+1];

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

                t.get_alpha( text_value, offset, PARMLIST_FLOAT_LEN );
                parameter[i].limits[a].svalue = text_value;
                parameter[i].limits[a].value = parameter[i].limits[a].svalue.float_value();;

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
*                             READ_PARAM_CSV                           *
***********************************************************************/
BOOLEAN PARAMETER_CLASS::read_param_csv( STRING_CLASS & filename )
{
int    a;
int    i;
TCHAR    * cp;
FILE_CLASS f;
STRING_CLASS s;

for ( i=0; i<MAX_PARMS; i++ )
    init( i );

if ( !file_exists(filename.text()) )
    return FALSE;

if ( f.open_for_read(filename) )
    {
    while ( true )
        {
        cp = f.readline();
        if ( !cp )
            break;
        s = cp;
        s.next_field();

        i = s.int_value();
        if ( i > 0 && i <= MAX_PARMS )
            {
            i--;
            if ( s.next_field() )
                parameter[i].input.type = (short) s.int_value();
            if ( s.next_field() )
                parameter[i].input.number = (short) s.int_value();
            if ( s.next_field() )
                parameter[i].vartype = (BITSETYPE) s.uint_value();
            if ( s.next_field() )
                parameter[i].units = (short) s.int_value();
            if ( s.next_field() )
                copystring( parameter[i].name, s.text(), PARAMETER_NAME_LEN );
            for ( a=0; a<NOF_ALARM_LIMIT_TYPES; a++ )
                {
                if ( s.next_field() )
                    {
                    parameter[i].limits[a].svalue = s;
                    parameter[i].limits[a].value = s.float_value();
                    }
                if ( s.next_field() )
                    parameter[i].limits[a].wire_number = (short) s.int_value();
                }
            if ( s.next_field() )
                parameter[i].target = s;

            if ( parameter[i].input.type == FT_STATIC_TEXT_PARAMETER )
                {
                parameter[i].static_text.init( computer, machine, part );
                parameter[i].static_text.get( i+1 );
                }
            }
        }

    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              SHOT_GOODNESS                           *
* Db points to a VDB_CLASS class that contains the record of the       *
* shot you are interested in. I will look at the values and return     *
* ALARM_SHOT if at least one parameter is in alarm, WARNING_SHOT if at *
* least one parameter is in warning and there are no alarms, and       *
* GOOD_SHOT if all parameters are within spec.                         *
***********************************************************************/
int PARAMETER_CLASS::shot_goodness( VDB_CLASS & v )
{
int   column_index;
int   i;
int   type;
float x;

type = GOOD_SHOT;
column_index = SHOTPARM_PARAM_1_INDEX;
for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( parameter[i].input.type == NO_PARAMETER_TYPE )
        break;
    x = v.ra[column_index].float_value();
    if ( parameter[i].is_alarm(x) )
        return ALARM_SHOT;
    if ( parameter[i].is_warning(x) )
        type = WARNING_SHOT;
    column_index++;
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

s.get_parmlist_csvname( computer_to_find, machine_to_find, part_to_find );
if ( s.file_exists() )
    return read_param_csv( s );

s.get_parmlist_dbname( computer_to_find, machine_to_find, part_to_find );
if ( read_param_db(s) )
    {
    if ( save() )
        s.delete_file();
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                          PARAMETER_CLASS::SAVE                       *
***********************************************************************/
BOOLEAN PARAMETER_CLASS::save()
{
int          a;
int          i;
FILE_CLASS   f;
NAME_CLASS   filename;
STRING_CLASS s;

filename.get_parmlist_csvname( computer, machine, part );

if ( f.open_for_write(filename) )
    {
    for ( i=0; i<MAX_PARMS; i++ )
        {
        if ( parameter[i].input.type == NO_PARAMETER_TYPE )
            continue;

        /*
        ----------------
        Parameter Number
        ---------------- */
        s = i+1;
        s += CommaChar;

        /*
        ---------------------
        Input type and number
        --------------------- */
        s += (int) parameter[i].input.type;
        s += CommaChar;
        s += (int) parameter[i].input.number;
        s += CommaChar;

        /*
        -------------
        Variable type
        ------------- */
        s += (unsigned int) parameter[i].vartype;
        s += CommaChar;

        /*
        -----
        Units
        ----- */
        s += (int) parameter[i].units;
        s += CommaChar;

        /*
        -----------------------------
        Parameter Name (User Defined)
        ----------------------------- */
        s.cat_w_char( parameter[i].name, CommaChar );

        /*
        ----------------
        Limits and wires
        ---------------- */
        for ( a=0; a<NOF_ALARM_LIMIT_TYPES; a++ )
            {
            s += parameter[i].limits[a].svalue;
            s += CommaChar;
            s += (int) parameter[i].limits[a].wire_number;
            s += CommaChar;
            }

        /*
        ------
        Target
        ------ */
        s += parameter[i].target;

        f.writeline( s );
        }
    f.close();

    for ( i=0; i<MAX_PARMS; i++ )
        {
        if ( parameter[i].input.type == FT_STATIC_TEXT_PARAMETER )
            {
            parameter[i].static_text.init( computer, machine, part );
            parameter[i].static_text.put( i+1 );
            }
        }

    return TRUE;
    }

MessageBox( 0, filename.text(), TEXT("Part Setup: Unable to save"), MB_OK | MB_APPLMODAL );
return FALSE;
}

/***********************************************************************
*                 PARAMETER_CLASS::GET_PARAMETER_INDEX                 *
***********************************************************************/
int PARAMETER_CLASS::get_parameter_index(TCHAR * name)
{
    int i;
    int n;
    STRING_CLASS s;
    STRING_CLASS p;

    n = count();
    p = name;

    for ( i = 0; i < n; i++ )
    {
        s = parameter[i].name;
        if ( s == p )
        {
            return i;
        }
    }

    return -1;
}
