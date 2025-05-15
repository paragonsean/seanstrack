#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\setpoint.h"
#include "..\include\subs.h"
#include "..\include\part.h"
#include "..\include\verrors.h"

static TCHAR YChar = TEXT( 'Y' );
static TCHAR BackSlashString[] = TEXT( "\\" );
static TCHAR CommaString[]     = TEXT( "," );

/***********************************************************************
*                         PART_CLASS::INIT                          *
***********************************************************************/
void PART_CLASS::init()
{
short i;

name.null();
computer.null();
machine.null();
sp.setsize(PARTSET_SETPOINT_COUNT);

distance_units       = NO_UNITS;
velocity_units       = NO_UNITS;
pressure_units       = NO_UNITS;
total_stroke_length  = 0.0;
min_stroke_length    = 0.0;
biscuit_time_delay   = 0;
eos_velocity         = 0.0;
sleeve_fill_distance = 0.0;
runner_fill_distance = 0.0;
csfs_min_velocity    = 0.0;
csfs_rise_velocity   = 0.0;
csfs_min_position    = 0.0;

for ( i=0; i<USER_VEL_COUNT; i++ )
    user_velocity_position[i] = 0.0;

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    user_avg_velocity_start[i] = 0.0;
    user_avg_velocity_end[i] = 0.0;
    }

plunger_diameter       = 0.0;
time_for_intens_pres   = 0;
pres_for_response_time = 0.0;
ms_per_time_sample     = 0;
nof_time_samples       = 0;
production_enabled     = FALSE;
head_pressure_channel  = 0;
rod_pressure_channel   = 0;

for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    analog_sensor[i] = NO_ANALOG_SENSOR_NUMBER;

x4_divisor          = 80.0;
velocity_multiplier =  1.0;
good_shot_wire      = NO_WIRE;
good_shot_requires_no_warnings = FALSE;
zero_speed_output_wire   = NO_WIRE;
zero_speed_output_enable = FALSE;
alarm_timeout_seconds    = 0;
}

/***********************************************************************
*                             PART_CLASS                               *
***********************************************************************/
PART_CLASS::PART_CLASS()
{
init();
}

/***********************************************************************
*                            ~PART_CLASS                               *
***********************************************************************/
PART_CLASS::~PART_CLASS()
{
}

/***********************************************************************
*                             OPERATOR=                                *
***********************************************************************/
void PART_CLASS::operator=( const PART_CLASS & sorc )
{
int i;

name     = sorc.name;
computer = sorc.computer;
machine  = sorc.machine;

velocity_units       = sorc.velocity_units;
distance_units       = sorc.distance_units;
pressure_units       = sorc.pressure_units;
total_stroke_length  = sorc.total_stroke_length;
min_stroke_length    = sorc.min_stroke_length;
biscuit_time_delay   = sorc.biscuit_time_delay;
eos_velocity         = sorc.eos_velocity;
sleeve_fill_distance = sorc.sleeve_fill_distance;
runner_fill_distance = sorc.runner_fill_distance;
csfs_min_velocity    = sorc.csfs_min_velocity;
csfs_rise_velocity   = sorc.csfs_rise_velocity;
csfs_min_position    = sorc.csfs_min_position;

for ( i=0; i<USER_VEL_COUNT; i++ )
    user_velocity_position[i] = sorc.user_velocity_position[i];

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    user_avg_velocity_start[i] = sorc.user_avg_velocity_start[i];

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    user_avg_velocity_end[i] = sorc.user_avg_velocity_end[i];

plunger_diameter       = sorc.plunger_diameter;
time_for_intens_pres   = sorc.time_for_intens_pres;
pres_for_response_time = sorc.pres_for_response_time;
ms_per_time_sample     = sorc.ms_per_time_sample;
nof_time_samples       = sorc.nof_time_samples;
production_enabled     = sorc.production_enabled;
head_pressure_channel  = sorc.head_pressure_channel;
rod_pressure_channel   = sorc.rod_pressure_channel;
sp                     = sorc.sp;

for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    analog_sensor[i] = sorc.analog_sensor[i];

x4_divisor                     = sorc.x4_divisor;
velocity_multiplier            = sorc.velocity_multiplier;
good_shot_wire                 = sorc.good_shot_wire;
good_shot_requires_no_warnings = sorc.good_shot_requires_no_warnings;
zero_speed_output_wire         = sorc.zero_speed_output_wire;
zero_speed_output_enable       = sorc.zero_speed_output_enable;
alarm_timeout_seconds          = sorc.alarm_timeout_seconds;
}

/***********************************************************************
*                             PART_CLASS                               *
*                           READ_PART_SETUP                            *
*                                                                      *
*The parts db used to be in the machine directory but is now in the    *
*part directory and is only for this part. It is a csv setpoint file.  *
***********************************************************************/
BOOLEAN PART_CLASS::read_part_setup( STRING_CLASS & fname )
{
if ( sp.get(fname) )
    {
    update();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             PART_CLASS                               *
*                         REFRESH_TIME_SAMPLE                          *
*                                                                      *
* This routine refreshes the ms_per_time_sample and the number         *
* of time samples. It is called by the part editor so that if the      *
* monitor setup has been changed, saving the part doesn't unchange it. *
*                                                                      *
***********************************************************************/
BOOLEAN PART_CLASS::refresh_time_sample()
{
NAME_CLASS s;
SETPOINT_CLASS mysp;

s.get_partset_csvname( computer, machine, name );
if ( s.file_exists() )
    {
    mysp.get( s );
    if ( mysp.count() == PARTSET_SETPOINT_COUNT )
        {
        sp[PARTSET_MS_PER_TIME_SAMPLE_INDEX].value = mysp[PARTSET_MS_PER_TIME_SAMPLE_INDEX].value;
        ms_per_time_sample         = (short) sp[PARTSET_MS_PER_TIME_SAMPLE_INDEX].value.int_value();

        sp[PARTSET_N_OF_TIME_SAMPLES_INDEX].value = mysp[PARTSET_N_OF_TIME_SAMPLES_INDEX].value;
        nof_time_samples           = (short) sp[PARTSET_N_OF_TIME_SAMPLES_INDEX].value.int_value();

        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                             PART_CLASS                               *
*                           READ_FTCHAN_DB                             *
***********************************************************************/
BOOLEAN PART_CLASS::read_ftchan_db( STRING_CLASS & fname )
{
short    i;
DB_TABLE t;
BOOLEAN status;

status = FALSE;

if ( t.open(fname, FTCHAN_RECLEN, PFL) )
    {
    /*
    -------------------------------------------------
    Clear all the sensors before loading the new ones
    ------------------------------------------------- */
    for ( i=0; i<MAX_FT2_CHANNELS; i++ )
        analog_sensor[i] = NO_ANALOG_SENSOR_NUMBER;

    while( t.get_next_record(NO_LOCK) )
        {
        i = t.get_short( FTCHAN_CHANNEL_NUMBER_OFFSET );
        if ( i > 0 && i <= MAX_FT2_CHANNELS )
            {
            i--;
            analog_sensor[i] = t.get_short( FTCHAN_SENSOR_NUMBER_OFFSET );
            }
        }
    status = TRUE;
    t.close();
    }

return status;
}

/***********************************************************************
*                           READ_X4_DIVISOR                            *
***********************************************************************/
BOOLEAN PART_CLASS::read_x4_divisor()
{
BOOLEAN status;
DB_TABLE t;
TCHAR rodpitch[RODPITCH_NAME_LEN+1];
NAME_CLASS s;
SETPOINT_CLASS mysp;

status = FALSE;
s.get_machset_csvname( computer, machine );
if ( mysp.get(s) )
    copystring( rodpitch, mysp[MACHSET_RODPITCH_INDEX].value.text() );
else
    return status;

s.get_psensor_dbname( computer );

/*
----------------------------------
Backups don't have a psensor table
---------------------------------- */
if ( !s.file_exists() )
    s.get_psensor_dbname();

if ( t.open(s, PSENSOR_RECLEN, PFL) )
    {
    t.put_alpha( PSENSOR_RODPITCH_OFFSET,   rodpitch, RODPITCH_NAME_LEN );
    t.put_short( PSENSOR_DIST_UNITS_OFFSET, distance_units, UNITS_ID_LEN      );
    t.reset_search_mode();

    if ( t.get_next_key_match(2, NO_LOCK) )
        {
        x4_divisor = t.get_double( PSENSOR_X4_POS_DIVISOR_OFFSET );
        status = TRUE;
        }
    t.close();
    }

return status;
}

/***********************************************************************
*                           PART_CLASS::FIND                           *
***********************************************************************/
BOOLEAN PART_CLASS::find( STRING_CLASS & computer_to_find, STRING_CLASS & machine_to_find, STRING_CLASS & name_to_find )
{
BOOLEAN        status;
SETPOINT_CLASS gsp;
NAME_CLASS     s;

s.get_partset_csvname( computer_to_find, machine_to_find, name_to_find );
if ( !s.file_exists() )
    return FALSE;

status = read_part_setup( s );
if ( status )
    {
    computer = computer_to_find;
    machine  = machine_to_find;
    name     = name_to_find;

    s.get_ftchan_dbname( computer, machine, name );
    status = read_ftchan_db( s );
    read_x4_divisor();
    good_shot_wire = NO_WIRE;
    good_shot_requires_no_warnings = FALSE;
    s.get_good_shot_setup_file_name( computer, machine, name );
    if ( gsp.get(s) > 1 )
        {
        good_shot_wire = gsp[0].value.uint_value();
        if ( good_shot_wire > HIGH_FT2_OUTPUT_WIRE )
            good_shot_wire = NO_WIRE;
        good_shot_requires_no_warnings = gsp[1].value.boolean_value();
        }
    }

return status;
}

/***********************************************************************
*                          PART_CLASS::SAVE                            *
***********************************************************************/
BOOLEAN PART_CLASS::save()
{
static TCHAR * description[] = {
    { TEXT("Distance Units") },
    { TEXT("Velocity Units") },
    { TEXT("Pressure Units") },
    { TEXT("Total Stroke Length") },
    { TEXT("Min Stroke Length") },
    { TEXT("Time Delay Biscuit") },
    { TEXT("End of Shot Velocity") },
    { TEXT("Sleeve Fill Distance") },
    { TEXT("Runner Fill Distance") },
    { TEXT("Min Velocity for CSFS") },
    { TEXT("Velocity for Rise CSFS") },
    { TEXT("Min CSFS Position") },
    { TEXT("User Vel 1") },
    { TEXT("User Vel 2") },
    { TEXT("User Vel 3") },
    { TEXT("User Vel 4") },
    { TEXT("User Avg St 1") },
    { TEXT("User Avg St 2") },
    { TEXT("User Avg St 3") },
    { TEXT("User Avg St 4") },
    { TEXT("User Avg End 1") },
    { TEXT("User Avg End 2") },
    { TEXT("User Avg End 3") },
    { TEXT("User Avg End 4") },
    { TEXT("Plunger Diameter") },
    { TEXT("Time for Intens Pres") },
    { TEXT("Pres for Response Time") },
    { TEXT("Ms per time sample") },
    { TEXT("N of Time Samples") },
    { TEXT("Production Enabled") },
    { TEXT("Head Pressure Channel") },
    { TEXT("Rod Pressure Channel") },
    { TEXT("Zero Speed Output Wire") },
    { TEXT("Zero Speed Output Enable") },
    { TEXT("Alarm Timeout") }
    };
static int NofDescriptions = sizeof(description)/sizeof(TCHAR *);

STRING_CLASS this_part;
short       i;
BOOLEAN     part_exists;
BOOLEAN     status;
DB_TABLE    t;
FILE_CLASS  f;
NAME_CLASS  s;
SETPOINT_CLASS gsp;

if ( NofDescriptions < PARTSET_SETPOINT_COUNT )
    MessageBox( 0, TEXT("NofDescriptions < PARTSET_SETPOINT_COUNT"), TEXT("PART_CLASS::save()"), MB_OK );

s.get_part_dir_file_name( computer, machine, name, 0 );
part_exists = s.directory_exists();
if ( !part_exists )
    {
    s.create_directory();
    s.get_part_results_dir_file_name( computer, machine, name, 0 );
    s.create_directory();
    }

/*
-------------------------------------
Add descriptions where there are none
------------------------------------- */
for ( i=0; i<PARTSET_SETPOINT_COUNT; i++ )
    {
    if ( sp[i].desc.isempty() )
        sp[i].desc = description[i];
    }

s.get_partset_csvname( computer, machine, name );
status = sp.put(s);
if ( status )
    {
    s.get_ftchan_dbname( computer, machine, name );
    t.ensure_existance( s );
    if ( t.open(s, FTCHAN_RECLEN, FL) )
        {
        t.empty();
        for ( i=0; i<MAX_FT2_CHANNELS; i++ )
            {
            t.put_short( FTCHAN_CHANNEL_NUMBER_OFFSET, i+1,              CHANNEL_NUMBER_LEN );
            t.put_short( FTCHAN_SENSOR_NUMBER_OFFSET,  analog_sensor[i], SENSOR_NUMBER_LEN );
            t.rec_append();
            }
        t.close();
        }
    s.get_good_shot_setup_file_name( computer, machine, name );
    gsp.setsize( 2 );
    gsp[0].desc  = GOOD_SHOT_WIRE_DESC;
    gsp[0].value = good_shot_wire;
    gsp[1].desc  = GOOD_SHOT_IF_NO_WARNINGS_DESC;
    gsp[1].value = boolean_to_ascii( good_shot_requires_no_warnings );
    gsp.put( s );
    }

return status;
}

/***********************************************************************
*                             PART_CLASS                               *
*                            plunger_area                              *
***********************************************************************/
double PART_CLASS::plunger_area()
{
double x;

x = plunger_diameter;
if ( not_zero(x) )
    {
    x *= x;
    x *= .78540;
    }
else
    x = 1.0;

return x;
}

/***********************************************************************
*                             PART_CLASS                               *
*                         set_distance_units                           *
***********************************************************************/
void PART_CLASS::set_distance_units( short new_distance_units )
{
distance_units = new_distance_units;
sp[PARTSET_DISTANCE_UNITS_INDEX].value = (int) distance_units;
velocity_multiplier = dups_to_vu( velocity_units, distance_units );
}

/***********************************************************************
*                             PART_CLASS                               *
*                          set_alarm_timeout                           *
***********************************************************************/
void PART_CLASS::set_alarm_timeout( int32 new_alarm_timeout_seconds )
{
alarm_timeout_seconds = new_alarm_timeout_seconds;
sp[PARTSET_ALARM_TIMEOUT_INDEX].value = new_alarm_timeout_seconds;
velocity_multiplier = dups_to_vu( velocity_units, distance_units );
}

/***********************************************************************
*                             PART_CLASS                               *
*                         set_distance_units                           *
***********************************************************************/
void PART_CLASS::set_distance_units( STRING_CLASS & new_distance_units )
{
distance_units = (short) new_distance_units.int_value();
sp[PARTSET_DISTANCE_UNITS_INDEX].value = new_distance_units;
velocity_multiplier = dups_to_vu( velocity_units, distance_units );
}

/***********************************************************************
*                             PART_CLASS                               *
*                         set_velocity_units                           *
***********************************************************************/
void PART_CLASS::set_velocity_units( short new_velocity_units )
{
velocity_units = new_velocity_units;
sp[PARTSET_VELOCITY_UNITS_INDEX].value = (int) velocity_units;
velocity_multiplier = dups_to_vu( velocity_units, distance_units );
}

/***********************************************************************
*                             PART_CLASS                               *
*                         set_velocity_units                           *
***********************************************************************/
void PART_CLASS::set_velocity_units( STRING_CLASS & new_velocity_units )
{
velocity_units = (short) new_velocity_units.int_value();
sp[PARTSET_VELOCITY_UNITS_INDEX].value = new_velocity_units;
velocity_multiplier = dups_to_vu( velocity_units, distance_units );
}

/***********************************************************************
*                             PART_CLASS                               *
*                         set_pressure_units                           *
***********************************************************************/
void PART_CLASS::set_pressure_units( short new_pressure_units )
{
pressure_units = new_pressure_units;
sp[PARTSET_PRESSURE_UNITS_INDEX].value = (int) pressure_units;
}

/***********************************************************************
*                             PART_CLASS                               *
*                         set_pressure_units                           *
***********************************************************************/
void PART_CLASS::set_pressure_units( STRING_CLASS & new_pressure_units )
{
pressure_units = (short) new_pressure_units.int_value();
sp[PARTSET_PRESSURE_UNITS_INDEX].value = new_pressure_units;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_total_stroke_length                       *
***********************************************************************/
void PART_CLASS::set_total_stroke_length( double new_total_stroke_length )
{
total_stroke_length = new_total_stroke_length;
sp[PARTSET_TOTAL_STROKE_LEN_INDEX].value = total_stroke_length;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_total_stroke_length                       *
***********************************************************************/
void PART_CLASS::set_total_stroke_length( STRING_CLASS & new_total_stroke_length )
{
total_stroke_length = new_total_stroke_length.real_value();
sp[PARTSET_TOTAL_STROKE_LEN_INDEX].value = new_total_stroke_length;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_min_stroke_length                         *
***********************************************************************/
void PART_CLASS::set_min_stroke_length( double new_min_stroke_length )
{
min_stroke_length = new_min_stroke_length;
sp[PARTSET_MIN_STROKE_LEN_INDEX].value = min_stroke_length;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_min_stroke_length                         *
***********************************************************************/
void PART_CLASS::set_min_stroke_length( STRING_CLASS & new_min_stroke_length )
{
min_stroke_length = new_min_stroke_length.real_value();
sp[PARTSET_MIN_STROKE_LEN_INDEX].value = new_min_stroke_length;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_biscuit_time_delay                        *
***********************************************************************/
void PART_CLASS::set_biscuit_time_delay( int32 new_biscuit_time_delay )
{
biscuit_time_delay = new_biscuit_time_delay;
sp[PARTSET_BISCUIT_TIME_DELAY_INDEX].value = (int) biscuit_time_delay;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_biscuit_time_delay                        *
***********************************************************************/
void PART_CLASS::set_biscuit_time_delay( STRING_CLASS & new_biscuit_time_delay )
{
biscuit_time_delay = (int32) new_biscuit_time_delay.int_value();
sp[PARTSET_BISCUIT_TIME_DELAY_INDEX].value = new_biscuit_time_delay;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_eos_velocity                              *
***********************************************************************/
void PART_CLASS::set_eos_velocity( double new_eos_velocity )
{
eos_velocity = new_eos_velocity;
sp[PARTSET_EOS_VEL_INDEX].value = eos_velocity;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_eos_velocity                              *
***********************************************************************/
void PART_CLASS::set_eos_velocity( STRING_CLASS & new_eos_velocity )
{
eos_velocity = new_eos_velocity.real_value();
sp[PARTSET_EOS_VEL_INDEX].value = new_eos_velocity;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_sleeve_fill_distance                      *
***********************************************************************/
void PART_CLASS::set_sleeve_fill_distance( double new_sleeve_fill_distance )
{
sleeve_fill_distance = new_sleeve_fill_distance;
sp[PARTSET_SLEEVE_FILL_DIST_INDEX].value = sleeve_fill_distance;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_sleeve_fill_distance                      *
***********************************************************************/
void PART_CLASS::set_sleeve_fill_distance( STRING_CLASS & new_sleeve_fill_distance )
{
sleeve_fill_distance = new_sleeve_fill_distance.real_value();
sp[PARTSET_SLEEVE_FILL_DIST_INDEX].value = new_sleeve_fill_distance;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_runner_fill_distance                      *
***********************************************************************/
void PART_CLASS::set_runner_fill_distance( double new_runner_fill_distance )
{
runner_fill_distance = new_runner_fill_distance;
sp[PARTSET_RUNNER_FILL_DIST_INDEX].value = runner_fill_distance;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_runner_fill_distance                      *
***********************************************************************/
void PART_CLASS::set_runner_fill_distance( STRING_CLASS & new_runner_fill_distance )
{
runner_fill_distance = new_runner_fill_distance.real_value();
sp[PARTSET_RUNNER_FILL_DIST_INDEX].value = new_runner_fill_distance;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_csfs_min_velocity                         *
***********************************************************************/
void PART_CLASS::set_csfs_min_velocity( double new_csfs_min_velocity )
{
csfs_min_velocity = new_csfs_min_velocity;
sp[PARTSET_MIN_CSFS_VEL_INDEX].value = csfs_min_velocity;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_csfs_min_velocity                         *
***********************************************************************/
void PART_CLASS::set_csfs_min_velocity( STRING_CLASS & new_csfs_min_velocity )
{
csfs_min_velocity = new_csfs_min_velocity.real_value();
sp[PARTSET_MIN_CSFS_VEL_INDEX].value = new_csfs_min_velocity;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_csfs_rise_velocity                        *
***********************************************************************/
void PART_CLASS::set_csfs_rise_velocity( double new_csfs_rise_velocity )
{
csfs_rise_velocity = new_csfs_rise_velocity;
sp[PARTSET_RISE_CSFS_VEL_INDEX].value = csfs_rise_velocity;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_csfs_rise_velocity                        *
***********************************************************************/
void PART_CLASS::set_csfs_rise_velocity( STRING_CLASS & new_csfs_rise_velocity )
{
csfs_rise_velocity = new_csfs_rise_velocity.real_value();
sp[PARTSET_RISE_CSFS_VEL_INDEX].value = new_csfs_rise_velocity;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_csfs_min_position                         *
***********************************************************************/
void PART_CLASS::set_csfs_min_position( double new_csfs_min_position )
{
csfs_min_position = new_csfs_min_position;
sp[PARTSET_MIN_CSFS_POS_INDEX].value = csfs_min_position;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_csfs_min_position                         *
***********************************************************************/
void PART_CLASS::set_csfs_min_position( STRING_CLASS & new_csfs_min_position )
{
csfs_min_position = new_csfs_min_position.real_value();
sp[PARTSET_MIN_CSFS_POS_INDEX].value = new_csfs_min_position;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_user_velocity_position                    *
***********************************************************************/
void PART_CLASS::set_user_velocity_position( int index, double new_user_velocity_position )
{
user_velocity_position[index] = new_user_velocity_position;
sp[PARTSET_USER_VEL_1_INDEX+index].value = new_user_velocity_position;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_user_velocity_position                    *
***********************************************************************/
void PART_CLASS::set_user_velocity_position( int index, STRING_CLASS & new_user_velocity_position )
{
user_velocity_position[index] = new_user_velocity_position.real_value();
sp[PARTSET_USER_VEL_1_INDEX+index].value = new_user_velocity_position;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_user_avg_velocity_start                   *
***********************************************************************/
void PART_CLASS::set_user_avg_velocity_start( int index, double new_avg_velocity_start )
{
user_avg_velocity_start[index] = new_avg_velocity_start;
sp[PARTSET_AVG_START_1_INDEX+index].value = new_avg_velocity_start;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_user_avg_velocity_start                   *
***********************************************************************/
void PART_CLASS::set_user_avg_velocity_start( int index, STRING_CLASS & new_avg_velocity_start )
{
user_avg_velocity_start[index] = new_avg_velocity_start.real_value();
sp[PARTSET_AVG_START_1_INDEX+index].value = new_avg_velocity_start;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_user_avg_velocity_end                     *
***********************************************************************/
void PART_CLASS::set_user_avg_velocity_end( int index, double new_avg_velocity_end )
{
user_avg_velocity_end[index] = new_avg_velocity_end;
sp[PARTSET_AVG_END_1_INDEX+index].value = new_avg_velocity_end;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_user_avg_velocity_end                     *
***********************************************************************/
void PART_CLASS::set_user_avg_velocity_end( int index, STRING_CLASS & new_avg_velocity_end )
{
user_avg_velocity_end[index] = new_avg_velocity_end.real_value();
sp[PARTSET_AVG_END_1_INDEX+index].value = new_avg_velocity_end;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_plunger_diameter                          *
***********************************************************************/
void PART_CLASS::set_plunger_diameter( double new_plunger_diameter )
{
plunger_diameter = new_plunger_diameter;
sp[PARTSET_PLUNGER_DIA_INDEX].value = plunger_diameter;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_plunger_diameter                          *
***********************************************************************/
void PART_CLASS::set_plunger_diameter( STRING_CLASS & new_plunger_diameter )
{
plunger_diameter = new_plunger_diameter.real_value();
sp[PARTSET_PLUNGER_DIA_INDEX].value = new_plunger_diameter;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_time_for_intens_pres                      *
***********************************************************************/
void PART_CLASS::set_time_for_intens_pres( int32 new_time_for_intens_pres )
{
time_for_intens_pres = new_time_for_intens_pres;
sp[PARTSET_TIME_FOR_INTENS_PRES_INDEX].value = (int) time_for_intens_pres;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_time_for_intens_pres                      *
***********************************************************************/
void PART_CLASS::set_time_for_intens_pres( STRING_CLASS & new_time_for_intens_pres )
{
time_for_intens_pres = (int32) new_time_for_intens_pres.int_value();
sp[PARTSET_TIME_FOR_INTENS_PRES_INDEX].value = new_time_for_intens_pres;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_pres_for_response_time                    *
***********************************************************************/
void PART_CLASS::set_pres_for_response_time( double new_pres_for_response_time )
{
pres_for_response_time = new_pres_for_response_time;
sp[PARTSET_PRES_FOR_RESP_TIME_INDEX].value = pres_for_response_time;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_pres_for_response_time                    *
***********************************************************************/
void PART_CLASS::set_pres_for_response_time( STRING_CLASS & new_pres_for_response_time )
{
pres_for_response_time = new_pres_for_response_time.real_value();
sp[PARTSET_PRES_FOR_RESP_TIME_INDEX].value = new_pres_for_response_time;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_ms_per_time_sample                        *
***********************************************************************/
void PART_CLASS::set_ms_per_time_sample( short new_ms_per_time_sample )
{
ms_per_time_sample = new_ms_per_time_sample;
sp[PARTSET_MS_PER_TIME_SAMPLE_INDEX].value = (int) ms_per_time_sample;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_ms_per_time_sample                        *
***********************************************************************/
void PART_CLASS::set_ms_per_time_sample( STRING_CLASS & new_ms_per_time_sample )
{
ms_per_time_sample = (short) new_ms_per_time_sample.int_value();
sp[PARTSET_MS_PER_TIME_SAMPLE_INDEX].value = new_ms_per_time_sample;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_nof_time_samples                          *
***********************************************************************/
void PART_CLASS::set_nof_time_samples( short new_nof_time_samples )
{
nof_time_samples = new_nof_time_samples;
sp[PARTSET_N_OF_TIME_SAMPLES_INDEX].value = (int) nof_time_samples;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_nof_time_samples                          *
***********************************************************************/
void PART_CLASS::set_nof_time_samples( STRING_CLASS & new_nof_time_samples )
{
nof_time_samples = (short) new_nof_time_samples.int_value();
sp[PARTSET_N_OF_TIME_SAMPLES_INDEX].value = new_nof_time_samples;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_production_enabled                        *
***********************************************************************/
void PART_CLASS::set_production_enabled( BOOLEAN new_production_enabled )
{
production_enabled = new_production_enabled;
sp[PARTSET_PRODUCTION_ENABLED_INDEX].value = production_enabled;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_head_pressure_channel                     *
***********************************************************************/
void PART_CLASS::set_head_pressure_channel( short new_head_pressure_channel )
{
head_pressure_channel = new_head_pressure_channel;
sp[PARTSET_HEAD_CHANNEL_INDEX].value = (int) head_pressure_channel;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_head_pressure_channel                     *
***********************************************************************/
void PART_CLASS::set_head_pressure_channel( STRING_CLASS & new_head_pressure_channel )
{
head_pressure_channel = (short) new_head_pressure_channel.int_value();
sp[PARTSET_HEAD_CHANNEL_INDEX].value = new_head_pressure_channel;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_rod_pressure_channel                      *
***********************************************************************/
void PART_CLASS::set_rod_pressure_channel( short new_rod_pressure_channel )
{
rod_pressure_channel = new_rod_pressure_channel;
sp[PARTSET_ROD_CHANNEL_INDEX].value = (int) rod_pressure_channel;
}

/***********************************************************************
*                             PART_CLASS                               *
*                        set_rod_pressure_channel                      *
***********************************************************************/
void PART_CLASS::set_rod_pressure_channel( STRING_CLASS & new_rod_pressure_channel )
{
rod_pressure_channel = (short) new_rod_pressure_channel.int_value();
sp[PARTSET_ROD_CHANNEL_INDEX].value = new_rod_pressure_channel;
}

/***********************************************************************
*                             PART_CLASS                               *
*                     SET_ZERO_SPEED_OUTPUT_ENABLE                     *
***********************************************************************/
void PART_CLASS::set_zero_speed_output_enable( BOOLEAN new_zero_speed_output_enable )
{
zero_speed_output_enable = new_zero_speed_output_enable;
sp[PARTSET_ZERO_SPEED_OUTPUT_ENABLE_INDEX].value = (BOOLEAN) zero_speed_output_enable;
}

/***********************************************************************
*                             PART_CLASS                               *
*                      SET_ZERO_SPEED_OUTPUT_WIRE                      *
***********************************************************************/
void PART_CLASS::set_zero_speed_output_wire( short new_wire_number )
{
zero_speed_output_wire = new_wire_number;
sp[PARTSET_ZERO_SPEED_OUTPUT_WIRE_INDEX].value = (int) zero_speed_output_wire;
}

/***********************************************************************
*                             PART_CLASS                               *
*                      SET_ZERO_SPEED_OUTPUT_WIRE                      *
***********************************************************************/
void PART_CLASS::set_zero_speed_output_wire( STRING_CLASS & new_wire_number )
{
zero_speed_output_wire = (short) new_wire_number.int_value();
sp[PARTSET_ZERO_SPEED_OUTPUT_WIRE_INDEX].value = (int) zero_speed_output_wire;
}

/***********************************************************************
*                               PART_CLASS                             *
*                                 UPDATE                               *
* This routine updates the numeric values with those converted from    *
* the strings in the setpoint class.                                   *
***********************************************************************/
BOOLEAN PART_CLASS::update()
{
int n;

n = sp.count();

if ( n < PARTSET_ZERO_SPEED_OUTPUT_WIRE_INDEX )
    return FALSE;

if ( n < PARTSET_SETPOINT_COUNT )
    {
    sp.setsize( PARTSET_SETPOINT_COUNT );
    if ( n <= PARTSET_ZERO_SPEED_OUTPUT_WIRE_INDEX )
        sp[PARTSET_ZERO_SPEED_OUTPUT_WIRE_INDEX].value = (int) NO_WIRE;
    if ( n <= PARTSET_ZERO_SPEED_OUTPUT_ENABLE_INDEX )
        sp[PARTSET_ZERO_SPEED_OUTPUT_ENABLE_INDEX].value = (BOOLEAN) FALSE;
    if ( n <= PARTSET_ALARM_TIMEOUT_INDEX )
        sp[PARTSET_ALARM_TIMEOUT_INDEX].value = (int) 0;
    }

velocity_units             = (short) sp[PARTSET_VELOCITY_UNITS_INDEX].value.int_value();
distance_units             = (short) sp[PARTSET_DISTANCE_UNITS_INDEX].value.int_value();
pressure_units             = (short) sp[PARTSET_PRESSURE_UNITS_INDEX].value.int_value();
total_stroke_length        = sp[PARTSET_TOTAL_STROKE_LEN_INDEX].value.real_value();
min_stroke_length          = sp[PARTSET_MIN_STROKE_LEN_INDEX].value.real_value();
biscuit_time_delay         = sp[PARTSET_BISCUIT_TIME_DELAY_INDEX].value.int_value();
eos_velocity               = sp[PARTSET_EOS_VEL_INDEX].value.real_value();
sleeve_fill_distance       = sp[PARTSET_SLEEVE_FILL_DIST_INDEX].value.real_value();
runner_fill_distance       = sp[PARTSET_RUNNER_FILL_DIST_INDEX].value.real_value();
csfs_min_velocity          = sp[PARTSET_MIN_CSFS_VEL_INDEX].value.real_value();
csfs_rise_velocity         = sp[PARTSET_RISE_CSFS_VEL_INDEX].value.real_value();
csfs_min_position          = sp[PARTSET_MIN_CSFS_POS_INDEX].value.real_value();
user_velocity_position[0]  = sp[PARTSET_USER_VEL_1_INDEX].value.real_value();
user_velocity_position[1]  = sp[PARTSET_USER_VEL_2_INDEX].value.real_value();
user_velocity_position[2]  = sp[PARTSET_USER_VEL_3_INDEX].value.real_value();
user_velocity_position[3]  = sp[PARTSET_USER_VEL_4_INDEX].value.real_value();
user_avg_velocity_start[0] = sp[PARTSET_AVG_START_1_INDEX].value.real_value();
user_avg_velocity_start[1] = sp[PARTSET_AVG_START_2_INDEX].value.real_value();
user_avg_velocity_start[2] = sp[PARTSET_AVG_START_3_INDEX].value.real_value();
user_avg_velocity_start[3] = sp[PARTSET_AVG_START_4_INDEX].value.real_value();
user_avg_velocity_end[0]   = sp[PARTSET_AVG_END_1_INDEX].value.real_value();
user_avg_velocity_end[1]   = sp[PARTSET_AVG_END_2_INDEX].value.real_value();
user_avg_velocity_end[2]   = sp[PARTSET_AVG_END_3_INDEX].value.real_value();
user_avg_velocity_end[3]   = sp[PARTSET_AVG_END_4_INDEX].value.real_value();
plunger_diameter           = sp[PARTSET_PLUNGER_DIA_INDEX].value.real_value();
time_for_intens_pres       = sp[PARTSET_TIME_FOR_INTENS_PRES_INDEX].value.int_value();
pres_for_response_time     = sp[PARTSET_PRES_FOR_RESP_TIME_INDEX].value.real_value();
ms_per_time_sample         = (short) sp[PARTSET_MS_PER_TIME_SAMPLE_INDEX].value.int_value();
nof_time_samples           = (short) sp[PARTSET_N_OF_TIME_SAMPLES_INDEX].value.int_value();
production_enabled         = sp[PARTSET_PRODUCTION_ENABLED_INDEX].value.boolean_value();
head_pressure_channel      = (short) sp[PARTSET_HEAD_CHANNEL_INDEX].value.int_value();
rod_pressure_channel       = (short) sp[PARTSET_ROD_CHANNEL_INDEX].value.int_value();
velocity_multiplier        = dups_to_vu( velocity_units, distance_units );
zero_speed_output_wire     = sp[PARTSET_ZERO_SPEED_OUTPUT_WIRE_INDEX].value.int_value();
zero_speed_output_enable   = sp[PARTSET_ZERO_SPEED_OUTPUT_ENABLE_INDEX].value.boolean_value();
alarm_timeout_seconds      = sp[PARTSET_ALARM_TIMEOUT_INDEX].value.int_value();

return TRUE;
}