#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\stparam.h"

static const TCHAR NullChar = TEXT( '\0' );

/***********************************************************************
*                         SURETRAK_PARAM_CLASS                         *
***********************************************************************/
SURETRAK_PARAM_CLASS::SURETRAK_PARAM_CLASS()
{
idata[LVDT_DAC_OFFSET_INDEX]        = 0;
idata[NULL_DAC_OFFSET_INDEX]        = 0;

velocity_dividend_value             = 4294967295UL;

fdata[ZERO_SPEED_CHECK_POS_INDEX]   = 22;
fdata[COUNTS_PER_UNIT_INDEX]        = 80;
fdata[JOG_VALVE_PERCENT_INDEX]      = 9.0;
fdata[RETRACT_VALVE_PERCENT_INDEX]  = -100.0;
fdata[FT_VALVE_PERCENT_INDEX]       = 12.0;
fdata[VEL_MULTIPLIER_INDEX]         = 2.86;
fdata[MIN_LOW_IMPACT_PERCENT_INDEX] = 5.0;
fdata[MAX_VELOCITY_INDEX]           = 100.0;
}

/***********************************************************************
*                         ~SURETRAK_PARAM_CLASS                        *
***********************************************************************/
SURETRAK_PARAM_CLASS::~SURETRAK_PARAM_CLASS()
{
}

/***********************************************************************
*                                FIND                                  *
***********************************************************************/
BOOLEAN SURETRAK_PARAM_CLASS::find( STRING_CLASS & computer, STRING_CLASS & machine )
{
int32       i;
FILE_CLASS  f;
TCHAR     * cp;
NAME_CLASS  s;

s.get_stparam_file_name( computer, machine );

if ( !s.file_exists() )
    {
    s.get_stparam_file_name( computer );
    if ( !s.file_exists() )
        return FALSE;
    }

if ( f.open_for_read(s) )
    {
    for ( i=0; i<NOF_INT_PARAMS; i++ )
        {
        cp = f.readline();
        if ( cp )
            idata[i] = asctoint32( cp );
        }

    cp = f.readline();
    if ( cp )
        velocity_dividend_value = asctoul( cp );

    for ( i=0; i<NOF_FLOAT_PARAMS; i++ )
        {
        cp = f.readline();
        if ( cp )
            fdata[i] = extfloat( cp );
        }

    f.close();
    }

return TRUE;
}

/***********************************************************************
*                                SAVE                                  *
***********************************************************************/
BOOLEAN SURETRAK_PARAM_CLASS::save( STRING_CLASS & computer, STRING_CLASS & machine )
{
int32       i;
FILE_CLASS  f;
NAME_CLASS  s;

s.get_stparam_file_name( computer, machine );
if ( !f.open_for_write(s) )
    return FALSE;

for ( i=0; i<NOF_INT_PARAMS; i++ )
    {
    s = idata[i];
    f.writeline( s );
    }
s = (unsigned int) velocity_dividend_value;
f.writeline( s );

for ( i=0; i<NOF_FLOAT_PARAMS; i++ )
    {
    s = (double) fdata[i];
    f.writeline( s );
    }

f.close();

return TRUE;
}
