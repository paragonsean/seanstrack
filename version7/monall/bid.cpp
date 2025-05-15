#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\ft.h"
#include "..\include\limit_switch_class.h"
#include "..\include\setpoint.h"
#include "..\include\subs.h"
#include "..\include\verrors.h"

#include "bid.h"

/***********************************************************************
*                    GET_BOARD_INITIALIZATION_DATA                     *
***********************************************************************/
BOOLEAN get_board_initialization_data( BOARD_INITIALIZATION_DATA & dest, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
int                    i;
LIMIT_SWITCH_POS_CLASS pos;
NAME_CLASS             s;
SETPOINT_CLASS         sp;
BOOLEAN                status;

status = FALSE;
s.get_machset_csvname( computer, machine );
if ( sp.get(s) )
    {
    dest.auto_shot_timeout_seconds = sp[MACHSET_AUTO_SHOT_TIMEOUT_INDEX].real_value();
    status = TRUE;
    }

if ( !status )
    {
    error_message( GET_BOARD_INITIALIZATION_DATA, MACHINE_NOT_FOUND, machine.text() );
    return FALSE;
    }

status = FALSE;
s.get_partset_csvname( computer, machine, part );
if ( sp.get(s) )
    {
    dest.nof_time_samples          = sp[PARTSET_N_OF_TIME_SAMPLES_INDEX].int_value();
    dest.total_stroke_len          = sp[PARTSET_TOTAL_STROKE_LEN_INDEX].real_value();
    dest.min_stroke_len            = sp[PARTSET_MIN_STROKE_LEN_INDEX].real_value();
    dest.biscuit_time_delay        = sp[PARTSET_BISCUIT_TIME_DELAY_INDEX].real_value();
    dest.eos_velocity              = sp[PARTSET_EOS_VEL_INDEX].real_value();
    dest.sleeve_fill_distance      = sp[PARTSET_SLEEVE_FILL_DIST_INDEX].real_value();
    dest.runner_fill_distance      = sp[PARTSET_RUNNER_FILL_DIST_INDEX].real_value();
    dest.min_csfs_vel              = sp[PARTSET_MIN_CSFS_VEL_INDEX].real_value();
    dest.rise_csfs_vel             = sp[PARTSET_RISE_CSFS_VEL_INDEX].real_value();
    dest.min_csfs_pos              = sp[PARTSET_MIN_CSFS_POS_INDEX].real_value();
    dest.ms_per_time_based_sample  = sp[PARTSET_MS_PER_TIME_SAMPLE_INDEX].real_value();
    status = TRUE;
    }

if ( status )
    {
    pos.get( computer, machine, part );
    for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
        {
        dest.limits[i] = pos.real_pos(i);
        }
    status = TRUE;
    }

if ( !status )
    error_message( GET_BOARD_INITIALIZATION_DATA, PART_NOT_FOUND, part.text() );

return status;
}
