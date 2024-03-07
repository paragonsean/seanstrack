#ifndef _SURETRAK_SETUP_CLASS_
#define _SURETRAK_SETUP_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

#ifndef _SETPOINT_
#include "..\include\setpoint.h"
#endif

#define MAX_ST_STEPS 8
#define NOF_GAIN_BREAKS 3

/*
-------------------------------------------------------------------------------
Firmware V7 only uses one break gain. GAIN_BREAK_GAIN 1 and 2 are replaced with
the integral and dirivative gain values. These are optional.
------------------------------------------------------------------------------- */
#define GAIN_BREAK_VEL_0_INDEX           0
#define GAIN_BREAK_VEL_1_INDEX           1
#define GAIN_BREAK_VEL_2_INDEX           2
#define GAIN_BREAK_GAIN_0_INDEX          3
#define GAIN_BREAK_GAIN_1_INDEX          4
#define GAIN_BREAK_GAIN_2_INDEX          5
#define INTEGRAL_GAIN_INDEX              4
#define DERIVATIVE_GAIN_INDEX            5
#define VELOCITY_LOOP_GAIN_INDEX         6
#define FT_STOP_POS_INDEX                7
#define VACUUM_LIMIT_SWITCH_NUMBER_INDEX 8
#define NOF_STSETUP_SETPOINTS            9

struct SURETRAK_STEP_ENTRY
    {
    TCHAR accel[STSTEPS_FLOAT_LEN+1];
    TCHAR vel[STSTEPS_FLOAT_LEN+1];
    TCHAR end_pos[STSTEPS_FLOAT_LEN+1];
    TCHAR low_impact[STSTEPS_FLOAT_LEN+1];
    BOOLEAN vacuum_wait;
    BOOLEAN low_impact_from_input;
    BOOLEAN accel_percent;
    BOOLEAN vel_percent;
    };

class SURETRAK_SETUP_CLASS
    {
    private:

    BOOLEAN             is_rewound;
    int                 current_index;
    int                 n;
    SURETRAK_STEP_ENTRY step[MAX_ST_STEPS];
    SETPOINT_CLASS      sp;
    public:

    SURETRAK_SETUP_CLASS();
    ~SURETRAK_SETUP_CLASS();
    void    empty();
    void    clear_steps();
    BOOLEAN find( PART_NAME_ENTRY & p );
    BOOLEAN find( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa );
    BOOLEAN save( PART_NAME_ENTRY & p );
    BOOLEAN save( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa );
    BOOLEAN append();

    void    rewind() { current_index = 0; is_rewound = TRUE; }
    BOOLEAN next();
    BOOLEAN gotostep( int step_wanted );  /* [1-6] */

    int   determine_nof_steps();
    int   nof_steps() { return n; }
    int   vacuum_limit_switch_index()  { return sp[VACUUM_LIMIT_SWITCH_NUMBER_INDEX].int_value()-1; }
    int   vacuum_limit_switch_number() { return sp[VACUUM_LIMIT_SWITCH_NUMBER_INDEX].int_value(); }

    TCHAR * accel_string()      { return step[current_index].accel;      }
    TCHAR * vel_string()        { return step[current_index].vel;        }
    TCHAR * end_pos_string()    { return step[current_index].end_pos;    }
    TCHAR * low_impact_string() { return step[current_index].low_impact; }
    double  accel_value();
    double  vel_value();
    double  end_pos_value();
    double  low_impact_value();
    BOOLEAN have_vacuum_wait()           { return step[current_index].vacuum_wait;           }
    BOOLEAN have_low_impact_from_input() { return step[current_index].low_impact_from_input; }
    BOOLEAN accel_is_percent()           { return step[current_index].accel_percent;         }
    BOOLEAN vel_is_percent()             { return step[current_index].vel_percent;           }

    STRING_CLASS & derivative_gain() { return sp[DERIVATIVE_GAIN_INDEX].value; }
    STRING_CLASS & gain_break_gain( int i );
    STRING_CLASS & gain_break_velocity( int i );
    STRING_CLASS & integral_gain() { return sp[INTEGRAL_GAIN_INDEX].value; }
    STRING_CLASS & ft_stop_pos() { return sp[FT_STOP_POS_INDEX].value; }
    STRING_CLASS & velocity_loop_gain() { return sp[VELOCITY_LOOP_GAIN_INDEX].value; }

    double derivative_gain_value() { return sp[DERIVATIVE_GAIN_INDEX].real_value(); }
    double integral_gain_value() { return sp[INTEGRAL_GAIN_INDEX].real_value(); }
    double gain_break_gain_value( int i );
    double gain_break_velocity_value( int i );
    double ft_stop_pos_value()        { return sp[FT_STOP_POS_INDEX].real_value();        }
    double velocity_loop_gain_value() { return sp[VELOCITY_LOOP_GAIN_INDEX].real_value(); }

    void    set_accel( TCHAR * sorc );
    void    set_vel( TCHAR * sorc );
    void    set_end_pos( TCHAR * sorc )    { lstrcpy( step[current_index].end_pos, sorc); }
    void    set_low_impact( TCHAR * sorc ) { lstrcpy( step[current_index].low_impact, sorc); }
    void    set_have_vacuum_wait( BOOLEAN sorc )           { step[current_index].vacuum_wait = sorc; }
    void    set_have_low_impact_from_input( BOOLEAN sorc ) { step[current_index].low_impact_from_input = sorc; }

    void    set_derivative_gain( STRING_CLASS & new_gain ) { sp[DERIVATIVE_GAIN_INDEX].value = new_gain; }
    void    set_ft_stop_pos( STRING_CLASS new_stop_pos )       { sp[FT_STOP_POS_INDEX].value = new_stop_pos; }
    void    set_gain_break_velocity( int i, STRING_CLASS & new_velocity );
    void    set_gain_break_gain( int i, STRING_CLASS & new_gain );
    void    set_integral_gain( STRING_CLASS & new_gain ) { sp[INTEGRAL_GAIN_INDEX].value = new_gain; }
    void    set_vacuum_limit_switch_number( int new_number )   { sp[VACUUM_LIMIT_SWITCH_NUMBER_INDEX].value = new_number; }
    void    set_vacuum_limit_switch_number( STRING_CLASS & new_number ) { sp[VACUUM_LIMIT_SWITCH_NUMBER_INDEX].value = new_number; }
    void    set_velocity_loop_gain( double new_value )         { sp[VELOCITY_LOOP_GAIN_INDEX].value         = new_value; }
    void    set_velocity_loop_gain( STRING_CLASS & new_value ) { sp[VELOCITY_LOOP_GAIN_INDEX].value         = new_value; }
    };

#endif
