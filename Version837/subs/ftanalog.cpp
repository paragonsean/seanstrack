#include <windows.h>

/***********************************************************************
*  Note: The ith entry is the one for the ith parameter.               *
*  There is room for all the parameters but, obviously, only the       *
*  ones that are ftanalog parameters are used for anything.            *
***********************************************************************/
#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"
#include "..\include\ftanalog.h"
#include "..\include\verrors.h"

static TCHAR CommaChar     = TEXT(',');
static TCHAR EmptyString[] = TEXT("");
static TCHAR CommaString[] = TEXT(",");
static TCHAR ZeroString[]  = TEXT("0");
static TCHAR ZeroPointZero[]       = TEXT("0.0");
static TCHAR OnePointZeroString[]  = TEXT("1.0");

/***********************************************************************
*                         FTANALOG_CLASS::INIT                         *
***********************************************************************/
void FTANALOG_CLASS::init( short i )
{
array[i].channel      = NO_FT_CHANNEL;
array[i].ind_var_type = POSITION_VAR;
array[i].result_type  = SINGLE_POINT_RESULT_TYPE;
array[i].ind_value    = EmptyString;
array[i].end_value    = EmptyString;
array[i].smooth_n          = ZeroString;
array[i].smooth_multiplier = OnePointZeroString;
}

/***********************************************************************
*                        FTANALOG_CLASS::REMOVE                        *
***********************************************************************/
BOOLEAN FTANALOG_CLASS::remove ( short index_to_remove )
{
short n;

if ( index_to_remove<0 || index_to_remove>=MAX_PARMS )
    return FALSE;

n = MAX_PARMS - 1;
while ( index_to_remove < n)
    {
    array[index_to_remove] = array[index_to_remove+1];
    index_to_remove++;
    }

init( index_to_remove );
return TRUE;
}

/***********************************************************************
*                                comma_count                           *
***********************************************************************/
static int comma_count( TCHAR * s )
{
int n;
n = 0;
while ( *s )
    {
    if ( *s == CommaChar )
        n++;
    s++;
    }

return n;
}

/***********************************************************************
*                          GET_FT_TARGET_LIST                          *
* Read the parmlist file (part dir) and make a list of the parameters  *
* that are FT_TARGET_INPUT parameters)                                 *
***********************************************************************/
static bool get_ft_target_list( bool * dest, STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
TCHAR    * cp;
NAME_CLASS s;
FILE_CLASS f;
int        i;

s.get_parmlist_csvname( co, ma, pa );
if ( !file_exists(s.text()) )
    return false;

for ( i=0; i<MAX_PARMS; i++ )
    dest[i] = false;

if ( f.open_for_read(s) )
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
                {
                if ( s.int_value() == FT_TARGET_INPUT )
                    dest[i] = true;
                }
            }
        }
    f.close();
    }

return true;
}

/***********************************************************************
*                           FTANALOG_CLASS::LOAD                       *
***********************************************************************/
BOOLEAN FTANALOG_CLASS::load( STRING_CLASS & computer_to_find, STRING_CLASS & machine_to_find, STRING_CLASS & part_to_find )
{
int          comma_count;
int          i;
TCHAR        c;
STRING_CLASS s;
FILE_CLASS   f;
DB_TABLE     t;
NAME_CLASS   fn;
BITSETYPE    vartype;
bool         is_ft_target[MAX_PARMS];

get_ft_target_list( is_ft_target, computer_to_find, machine_to_find, part_to_find );

fn.get_ftanalog_csvname( computer_to_find, machine_to_find, part_to_find );
if ( fn.file_exists() )
    {
    if ( f.open_for_read(fn.text()) )
        {
        while ( true )
            {
            s = f.readline();
            if ( s.isempty() )
                break;
            comma_count = s.countchars( CommaChar );
            if ( comma_count >= FTANALOG_V7_COMMA_COUNT )
                {
                i = 0;
                s.next_field();
                i = s.int_value();
                if ( i > 0 && i <= MAX_PARMS )
                    {
                    i--;
                    s.next_field();
                    array[i].channel = s.int_value();

                    s.next_field();
                    array[i].result_type = (BITSETYPE) s.uint_value();

                    s.next_field();

                    vartype = (BITSETYPE) s.uint_value();
                    array[i].ind_var_type = vartype;

                    s.next_field();
                    array[i].ind_value = s;

                    s.next_field();
                    array[i].end_value = s;

                    /*
                    -----------------------------------------------------------
                    If this is an FT_TARGET_INPUT and the end value is "0" then
                    this is an old file that use the MSL for the start position
                    (I use 0.0 for zero now).
                    ----------------------------------------------------------- */
                    if ( is_ft_target[i] && s == ZeroString )
                        array[i].end_value = TEXT( "MSL" );

                    if ( comma_count >= FTANALOG_COMMA_COUNT )
                        {
                        s.next_field();
                        array[i].smooth_n = s;

                        s.next_field();
                        array[i].smooth_multiplier = s;
                        }
                    else
                        {
                        array[i].smooth_n          = ZeroString;
                        array[i].smooth_multiplier = OnePointZeroString;
                        }
                    }
                }
            }
        f.close();
        return TRUE;
        }
    }
else
    {
    fn.get_ftanalog_dbname( computer_to_find, machine_to_find, part_to_find );
    if ( fn.file_exists() )
        {
        if ( t.open(fn.text(), FTANALOG_RECLEN, PFL) )
            {
            while( t.get_next_record(NO_LOCK) )
                {
                i = (int) t.get_short( FTANALOG_INPUT_NUMBER_OFFSET );
                if ( i > 0 && i <= MAX_PARMS )
                    {
                    i--;
                    array[i].channel = t.get_short( FTANALOG_CHANNEL_NUMBER_OFFSET );
                    c                = t.get_char(  FTANALOG_SAMPLE_TYPE_OFFSET );
                    if ( c == ASCII_POS_IND_VAR )
                        array[i].ind_var_type = POSITION_VAR;
                    else if ( c == ASCII_TIME_IND_VAR )
                        array[i].ind_var_type = TIME_VAR;
                    else if ( c == ASCII_GREATER_THAN_VAR )
                        array[i].ind_var_type = TRIGGER_WHEN_GREATER;
                    else if ( c == ASCII_SMALLER_THAN_VAR )
                        array[i].ind_var_type = TRIGGER_WHEN_SMALLER;
                    else
                        array[i].ind_var_type = NO_VARIABLE_TYPE;
                    array[i].result_type = SINGLE_POINT_RESULT_TYPE;

                    array[i].ind_value = ascii_float( t.get_float(FTANALOG_SAMPLE_VALUE_OFFSET) );
                    array[i].end_value = EmptyString;
                    array[i].smooth_n  = ZeroString;
                    array[i].smooth_multiplier = OnePointZeroString;
                    }
                }

            t.close();
            if ( t.get_global_error() == VS_SUCCESS )
                return TRUE;
            }
        }
    }

return FALSE;
}

/***********************************************************************
*                          FTANALOG_CLASS::SAVE                        *
***********************************************************************/
BOOLEAN FTANALOG_CLASS::save( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
int          i;
STRING_CLASS s;
FILE_CLASS   f;
NAME_CLASS   fn;
BITSETYPE    vartype;
bool         is_ft_target[MAX_PARMS];

get_ft_target_list( is_ft_target, computer, machine, part );

s.upsize( 100 );
fn.get_ftanalog_csvname( computer, machine, part );
if ( f.open_for_write(fn.text()) )
    {
    for ( i=0; i<MAX_PARMS; i++ )
        {
        /*
        ----------------------------------------
        if ( array[i].channel == NO_FT_CHANNEL )
            continue;
        ---------------------------------------- */
        s = EmptyString;

        s += i+1;
        s += CommaString;

        s += (int) array[i].channel;
        s += CommaString;

        s += (unsigned) array[i].result_type;
        s += CommaString;

        vartype = array[i].ind_var_type;
        s += (unsigned) vartype;
        s += CommaString;

        s.cat_w_char( array[i].ind_value, CommaChar );

        /*
        ---------------------------------------------------------------
        The FT_TARGET_INPUT end_value is the position to start looking.
        If this is actually zero I save it as "0.0" so I will know
        is not an old record the next time I read it.
        I am assuming that the parmlist.csv has already been
        saved.
        ------------------------------------------------------ */
        if ( is_ft_target[i] && array[i].end_value == ZeroString )
            array[i].end_value = ZeroPointZero;

        s.cat_w_char( array[i].end_value, CommaChar );
        s.cat_w_char( array[i].smooth_n, CommaChar );
        s += array[i].smooth_multiplier;
        f.writeline( s.text() );
        }
    f.close();
    return TRUE;
    }

return FALSE;
}
