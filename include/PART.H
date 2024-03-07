#ifndef _PART_CLASS_
#define _PART_CLASS_

#ifndef _SETPOINT_CLASS_
#include "..\include\setpoint.h"
#endif

#ifndef _MULTIPART_RUNLIST_CLASSES_
#include "..\include\multipart_runlist.h"
#endif

class PART_CLASS
    {
    public:

    STRING_CLASS computer;
    STRING_CLASS machine;
    STRING_CLASS name;

    unsigned                      alarm_shot_wire;
    int32                         alarm_timeout_seconds;
    short                         analog_sensor[MAX_FT2_CHANNELS];
    int32                         biscuit_time_delay;
    double                        csfs_min_position;
    double                        csfs_min_velocity;
    double                        csfs_rise_velocity;
    short                         distance_units;
    double                        eos_velocity;
    BOOLEAN                       good_shot_requires_no_warnings;
    unsigned                      good_shot_wire;
    short                         head_pressure_channel;
    double                        min_stroke_length;
    short                         ms_per_time_sample;
    MULTIPART_RUNLIST_WIRES_CLASS multipart_runlist_wires;
    short                         nof_time_samples;
    double                        plunger_diameter;
    double                        pres_for_response_time;
    short                         pressure_units;
    BOOLEAN                       production_enabled;
    short                         rod_pressure_channel;
    double                        runner_fill_distance;
    int                           shot_complete_timeout_seconds;
    unsigned                      shot_complete_wire;
    unsigned                      shot_save_complete_wire;
    double                        sleeve_fill_distance;
    SETPOINT_CLASS                sp;
    unsigned                      st2_program_abort_wire;
    unsigned                      thirty_second_timeout_wire;
    int32                         time_for_intens_pres;
    double                        total_stroke_length;
    double                        user_avg_velocity_start[USER_AVG_VEL_COUNT];
    double                        user_avg_velocity_end[USER_AVG_VEL_COUNT];
    double                        user_velocity_position[USER_VEL_COUNT];
    double                        velocity_multiplier;
    short                         velocity_units;
    unsigned                      warning_shot_wire;
    double                        x4_divisor;
    short                         zero_speed_output_wire;
    BOOLEAN                       zero_speed_output_enable;

    PART_CLASS();
    ~PART_CLASS();
    void    init();
    double  plunger_area();
    BOOLEAN read_part_setup( STRING_CLASS & fname );
    BOOLEAN read_ftchan_db( STRING_CLASS & fname );
    BOOLEAN read_x4_divisor();
    BOOLEAN refresh_time_sample();
    BOOLEAN find( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & name );
    BOOLEAN save();
    double  dist_from_x4(     double x4_value ){ return x4_value/x4_divisor; }
    double  velocity_from_x4( double x4_value );
    double  x4_from_dist(     double dist_value     ){ return dist_value*x4_divisor; }
    double  x4_from_velocity( double velocity_value ){ return velocity_value * x4_divisor / velocity_multiplier;}
    void    operator=( const PART_CLASS & sorc );
    void    set_alarm_timeout( int32 new_alarm_timeout_seconds );
    void    set_velocity_units( short new_velocity_units );
    void    set_velocity_units( STRING_CLASS & new_velocity_units );
    void    set_distance_units( short new_distance_units );
    void    set_distance_units( STRING_CLASS & new_distance_units );
    void    set_pressure_units( short new_pressure_units );
    void    set_pressure_units( STRING_CLASS & new_pressure_units );
    void    set_total_stroke_length( double new_total_stroke_length );
    void    set_total_stroke_length( STRING_CLASS & new_total_stroke_length );
    void    set_min_stroke_length( double new_min_stroke_length );
    void    set_min_stroke_length( STRING_CLASS & new_min_stroke_length );
    void    set_biscuit_time_delay( int32 new_biscuit_time_delay );
    void    set_biscuit_time_delay( STRING_CLASS & new_biscuit_time_delay );
    void    set_eos_velocity( double new_eos_velocity );
    void    set_eos_velocity( STRING_CLASS & new_eos_velocity );
    void    set_sleeve_fill_distance( double new_sleeve_fill_distance );
    void    set_sleeve_fill_distance( STRING_CLASS & new_sleeve_fill_distance );
    void    set_runner_fill_distance( double new_runner_fill_distance );
    void    set_runner_fill_distance( STRING_CLASS & new_runner_fill_distance );
    void    set_csfs_min_velocity( double new_csfs_min_velocity );
    void    set_csfs_min_velocity( STRING_CLASS & new_csfs_min_velocity );
    void    set_csfs_rise_velocity( double new_csfs_rise_velocity );
    void    set_csfs_rise_velocity( STRING_CLASS & new_csfs_rise_velocity );
    void    set_csfs_min_position( double new_csfs_min_position );
    void    set_csfs_min_position( STRING_CLASS & new_csfs_min_position );
    void    set_user_velocity_position( int index, double new_user_velocity_position ) ;
    void    set_user_velocity_position( int index, STRING_CLASS & new_user_velocity_position ) ;
    void    set_user_avg_velocity_start( int index, double new_avg_velocity_start );
    void    set_user_avg_velocity_start( int index, STRING_CLASS & new_avg_velocity_start );
    void    set_user_avg_velocity_end( int index, double new_avg_velocity_end );
    void    set_user_avg_velocity_end( int index, STRING_CLASS & new_avg_velocity_end );
    void    set_plunger_diameter( double new_plunger_diameter );
    void    set_plunger_diameter( STRING_CLASS & new_plunger_diameter );
    void    set_time_for_intens_pres( int32 new_time_for_intens_pres );
    void    set_time_for_intens_pres( STRING_CLASS & new_time_for_intens_pres );
    void    set_pres_for_response_time( double new_pres_for_response_time );
    void    set_pres_for_response_time( STRING_CLASS & new_pres_for_response_time );
    void    set_ms_per_time_sample( short new_ms_per_time_sample );
    void    set_ms_per_time_sample( STRING_CLASS & new_ms_per_time_sample );
    void    set_nof_time_samples( short new_nof_time_samples );
    void    set_nof_time_samples( STRING_CLASS & new_nof_time_samples );
    void    set_production_enabled( BOOLEAN new_production_enabled );
    void    set_head_pressure_channel( short new_head_pressure_channel );
    void    set_head_pressure_channel( STRING_CLASS & new_head_pressure_channel );
    void    set_rod_pressure_channel( short new_rod_pressure_channel );
    void    set_rod_pressure_channel( STRING_CLASS & new_rod_pressure_channel );
    void    set_zero_speed_output_enable( BOOLEAN new_zero_speed_output_enable );
    void    set_zero_speed_output_wire( short new_wire_number );
    void    set_zero_speed_output_wire( STRING_CLASS & new_wire_number );

    BOOLEAN update(); /* update numbers from the setpoint values */
    };

#endif
