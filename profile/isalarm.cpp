#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

#include "extern.h"

/***********************************************************************
*                           GET_ALARM_STATE                            *
***********************************************************************/
int get_alarm_state()
{
const float zero_difference = (float) ZERO_DIF;

DWORD             file_attributes;
NAME_CLASS        fn;
float             high;
int               i;
bool              ignore_external_parameters;
bool              this_is_an_external_parameter;
float             low;
int               n;
int               state;
PARAMETER_ENTRY * p;
float             v;

ignore_external_parameters = false;
fn.get_profile_ii_name( ComputerName, MachineName, PartName, ShotName );
file_attributes = GetFileAttributes( fn.text() );

if ( !(file_attributes & FILE_ATTRIBUTE_ARCHIVE) )
    ignore_external_parameters = true;

state = NO_ALARM_STATE;
n = CurrentParam.count();

for ( i=0; i<n; i++ )
    {
    p = &CurrentParam.parameter[i];
    this_is_an_external_parameter = false;
    if ( p->input.type == FT_EXTERNAL_PARAMETER )
        this_is_an_external_parameter = true;
    if ( (p->input.type != NO_PARAMETER_TYPE) && (p->vartype != TEXT_VAR) && !(ignore_external_parameters && this_is_an_external_parameter) )
        {
        v = Parms[i].float_value();
        high = p->limits[ALARM_MAX].value;
        low  = p->limits[ALARM_MIN].value;
        if ( not_float_zero(high - low) )
            {
            high += zero_difference;
            low  -= zero_difference;
            if ( v < low || v > high )
                {
                state = ALARM_STATE;
                break;
                }
            }

        high = p->limits[WARNING_MAX].value;
        low  = p->limits[WARNING_MIN].value;
        if ( not_float_zero(high - low) )
            {
            high += zero_difference;
            low  -= zero_difference;
            if ( v < low || v > high )
                state = WARNING_STATE;
            }
        }
    }

return state;
}

/***********************************************************************
*                              IS_OUT                                  *
***********************************************************************/
static BOOLEAN is_out( short parameter_index, float value, short min_index, short max_index )
{
PARAMETER_ENTRY * p;
float low;
float high;

p = &CurrentParam.parameter[parameter_index];
if ( p->input.type != NO_PARAMETER_TYPE && p->vartype != TEXT_VAR )
    {
    high = p->limits[max_index].value;
    low  = p->limits[min_index].value;
    if ( not_float_zero(high - low) )
        {
        if ( is_float_zero(value-low) || is_float_zero(high-value) )
            return FALSE;
        if ( value < low || value > high )
            return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                           IS_ALARM                                   *
***********************************************************************/
BOOLEAN is_alarm( short parameter_index, float value )
{
return is_out( parameter_index, value, ALARM_MIN, ALARM_MAX );
}

/***********************************************************************
*                              IS_WARNING                              *
***********************************************************************/
BOOLEAN is_warning( short parameter_index, float value )
{
return is_out( parameter_index, value, WARNING_MIN, WARNING_MAX );
}
