#include <windows.h>
#include <cmath>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\ftanalog.h"
#include "..\include\ftcalc.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

static TCHAR AChar = TEXT( 'A' );
static TCHAR PChar = TEXT( 'P' );

extern TCHAR MonallIniFile[];
extern TCHAR ConfigSection[];

/*
------------------------------------------------------------
                   Setpoints for ftcalc
The 0th setpoint is for the Intensification Squeeze Time.
This is the time when the rod first reaches the max position

New parameters can effectively be added by adding them to
the SETPOINT_CLASS sp in ftcalc. You can then create a
parameter with the calculated parameter.
------------------------------------------------------------ */
static       TCHAR IstName[]            = IST_NAME;
static const int   IST_INDEX            = 0;
static const int   NOF_FTCALC_SETPOINTS = 1;

/***********************************************************************
*                              GET_POSITION                            *
* parameter is the array of parameters. Only those less than the       *
* current_parameter_index have been calculated at this time.           *
* The "sorc" is either the real value or a paramter name like P13. If  *
* it is "P13" then I use the value of parameter[13].                   *
* If the sorc is negative then it is subtracted from P3                *
***********************************************************************/
static double get_position( double * parameter, int current_parameter_index, double p3_position, STRING_CLASS & sorc )
{
TCHAR * cp;
int     i;
double  x;

x = 0.0;
cp = sorc.text();
if ( *cp == PChar )
    {
    /*
    ----------------------------------------------
    I want to use an already calculated parameter.
    ---------------------------------------------- */
    cp++;
    i = (int) asctoint32( cp );
    i--;
    if ( i>=0 && i < current_parameter_index )
        x = parameter[i];
    }
else
    {
    x = sorc.real_value();
    if ( x < 0.0 )
        x += p3_position;
    }

return x;
}

/***********************************************************************
*                           SET_SOURCE_DATA                            *
* This fills in the three values of a SOURCE_DATA struct using the     *
* data from a single measurement point, "index".                       *
***********************************************************************/
static void set_source_data( SOURCE_DATA * sd, int parameter_index, int index, FTII_PROFILE & f, PART_CLASS & part )
{
double position;
double time;
double velocity;
double x4_position;

x4_position = f.position[index];
position = part.dist_from_x4( x4_position );

time = f.ms(index);
velocity = f.interpolated_velocity( x4_position );
velocity = part.velocity_from_x4( velocity );
sd[parameter_index].set( position, time, velocity );
}

/***********************************************************************
*                           SET_SOURCE_DATA                            *
* This fills in the three values of a SOURCE_DATA struct using a real  *
* index which may have a fractional part, thus requiring interpolation *
* from the measurement points on either side.
***********************************************************************/
static void set_source_data( SOURCE_DATA * sd, int parameter_index, double index, FTII_PROFILE & f, PART_CLASS & part )
{
int    i1;
int    i2;
double d;
double position;
double time;
double velocity;
double x1;
double x2;

if ( index < 0 )
    {
    set_source_data( sd, parameter_index, 0, f, part );
    return;
    }

if ( index > f.n )
    {
    set_source_data( sd, parameter_index, f.n, f, part );
    return;
    }

x1 = floor( index );
x2 = ceil( index );
i1 = (int) x1;
i2 = (int) x2;

if ( double_are_equal(index, x1) )
    {
    set_source_data( sd, parameter_index, i1, f, part );
    return;
    }

if ( double_are_equal(index, x2) )
    {
    set_source_data( sd, parameter_index, i2, f, part );
    return;
    }

d  = index - x1;

position = f.position[i1] + d * (f.position[i2] - f.position[i1]);
position = part.dist_from_x4( position );

time = f.timer[i1] + d * (f.timer[i2] - f.timer[i1]);
time *= 1000.0;
time /= double( f.timer_frequency );

velocity = f.velocity[i1] + d * (f.velocity[i2]-f.velocity[i1]);
velocity = part.velocity_from_x4( velocity );

sd[parameter_index].set( position, time, velocity );
}

/***********************************************************************
*                      CALCULATE_FTII_PARAMETERS                       *
* This returns an array of MAX_PARMS double values, which it is the    *
* responsibility of the calling program to delete.                     *
***********************************************************************/
double * calculate_ftii_parameters( FTII_PROFILE & f, MACHINE_CLASS & machine, PART_CLASS & part, PARAMETER_CLASS & param, ANALOG_SENSOR & asensor, FTANALOG_CLASS & ftanalog, FTCALC_CLASS & ftcalc )
{
const int ABOVE_TARGET      = 1;
const int BELOW_TARGET      = 2;
const int TARGET_TRACK_INIT = 0;

static bool firstime = true;
static int  epi_skip_count = 5;

int         a;
int         ah;
int         ar;
int         c;
int         current_position;
int         csfs_velocity;
int         differential_curve_number;
int         eos_velocity;
int         hi;
int         i;
int         j;
int         i1;
int         i2;
int         last_point_side;
int         last_time_based_point;
int         max_analog_index;
int         max_epi_position;
int         max_position;
int         max_position_index;
int         min_analog_index;
int         min_stroke_length;
int         n;
int         p3_index;
int         p;
int         ri;
int         target_index;
int         v1;
int         v2;
int       * vi;
bool        have_p3;
BOOLEAN     need_head_pressure;
BOOLEAN     need_rod_pressure;
BOOLEAN     start_is_pos_end_is_time;
double      area;
double      ch;
double      cr;
double      csfs_position;
double    * dest;
SOURCE_DATA idata[MAX_INTERNAL_PARMS];
double      iparm[MAX_INTERNAL_PARMS];
double      impact_time;
double      max_analog_value;
double      max_position_time;
double      max_position_value;
double      min_analog_value;
double      sum;
double      p1_position;
double      p2_position;
double      p3_position;
double    * pressure;
double      target;
double      x;
double      y;
unsigned    mask;
unsigned    u;
unsigned    wire;
unsigned  * up;
BITSETYPE   variable_type;
BITSETYPE   parameter_pressure_type;

if ( firstime )
    {
    epi_skip_count = int32_from_ini( MonallIniFile, ConfigSection, TEXT("EffPeakSkipCount") );
    maxint( epi_skip_count, 5 );
    firstime = false;
    }

for ( i=0; i<MAX_INTERNAL_PARMS; i++ )
    iparm[i] = 0.0;

/*
---------------------------------------
Initialize the setpoint class in ftcalc
--------------------------------------- */
ftcalc.sp.setsize( NOF_FTCALC_SETPOINTS );

/*
---------------------------------
Clear all the source data points
(pos,time,vel for each parameter)
--------------------------------- */
ftcalc.clear_source_data();

/*
---------------------
Find the max position
--------------------- */
n = f.n;
/*
------------------------------------------------------------------------
If the time to measure biscuit is set the the number of useful points is
this point plus 1.
------------------------------------------------------------------------ */
if ( machine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT )
    {
    n = (int) part.biscuit_time_delay; /* ms */

    i  = f.us_per_time_sample;
    i /= 1000;
    if ( i < 1 )
        i = 1;
    n /= i; /* = time sample index */
    n += f.last_position_based_point;
    /*
    --------------------------------------------------------------------
    N is now the index of the point I should use to measure the biscuit.
    Increment this to make n the number of useful points.
    -------------------------------------------------------------------- */
    n++;
    if (  n > f.n )
        n = f.n;
    }

/*
----------------------------------------------------------------------------------
Now search through the post impact points for the one with the max position value.
If more than one point are found at the max position remember the highest index.
---------------------------------------------------------------------------------- */

/*
---------------------------------------------------------------------------------
Calculate the number of samples in 1/4 second. If the sample time is greater than
1/4 second then start looking at 1/8 of a second after impact.
--------------------------------------------------------------------------------- */
i2  = f.us_per_time_sample;
i2 *= 250;
i2 /= 1000;
if ( n > (f.last_position_based_point + i2) )
    i2 /= 2;
else
    i2 = 1;
i = f.last_position_based_point + i2;

max_position_index = i;
max_position = 0;
while ( i < n )
    {
    if ( f.position[i] > max_position )
        {
        max_position_index = i;
        max_position = f.position[i];
        }
    i++;
    }
max_position_value = part.dist_from_x4( double(max_position) );

iparm[BISCUIT_SIZE] = 0.0;
for ( i=0; i<NOF_FTII_FILE_PARAMETERS; i++ )
    {
    if ( f.parameter[i].index == CYCLE_TIME_PARM )
        {
        iparm[CYCLE_TIME_PARM] = f.parameter[i].ivalue;
        iparm[CYCLE_TIME_PARM] /= 1000;
        }
    else if ( f.parameter[i].index == BISCUIT_SIZE )
        {
        iparm[BISCUIT_SIZE] = part.dist_from_x4( double(f.parameter[i].ivalue) );
        }
    }

/*
--------------------------------------------------------------------------------------------
If this machine uses an absolute time to measure the biscuit then I must calculate it myself
(note that the biscuit time delay is stored as ms if it is an absolute time).
I now calculate the biscuit size every time unless the real biscuit time delay is not zero.
-------------------------------------------------------------------------------------------- */
if ( machine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT || part.biscuit_time_delay == 0 )
    iparm[BISCUIT_SIZE] = (double) part.total_stroke_length - max_position_value;

set_source_data( idata, BISCUIT_SIZE, max_position_index, f, part );

/*
------------------------------------------------------------------------------------
Eos = P3 position, first check to see if there is a value for the end_of_fs_velocity
that is greater than the eos_velocity
------------------------------------------------------------------------------------ */
x = double( part.eos_velocity );

/*
-------------------------------------------------------------------------------
This is code for using the end of fast shot velocity to calculate the p3 point.
y = part.setpoint[0].real_value();
if ( y > x )
    x = y;
------------------------------------------------------------------------------- */
x = part.x4_from_velocity( x );
eos_velocity = (int) round( x, 1.0 );

min_stroke_length = part.x4_from_dist( double(part.min_stroke_length) );

vi = f.velocity;
i2 = f.last_position_based_point + 1;
p3_index = i2;
i1 = i2 - 1;
x = (double) i1;

/*
------------------------------------------------------------------------------------------------------
Try to do something intelligent if the min stroke length is greater than the last position based point
------------------------------------------------------------------------------------------------------ */
if ( i1 < min_stroke_length )
    {
    min_stroke_length = i1 - 10;
    if ( min_stroke_length < 0 )
        min_stroke_length = i1;
    }

have_p3 = false;

/*
--------------------------------------------------------------------------------------------------------
This is the normal way to find the p3 point. Start at the last position based point and look back toward
the min stroke length.
-------------------------------------------------------------------------------------------------------- */
while ( i1 >= min_stroke_length )
    {
    v1 = vi[i1];
    v2 = vi[i2];
    if ( v2 < 0 )
        v2 = 0;

    if ( v1 >= eos_velocity && eos_velocity >= v2 )
        {
        if ( v1 == v2 )
            {
            x = (double) f.position[i2];
            }
        else
            {
            y = (double)( f.position[i2] - f.position[i1] );
            if ( y > 0.0 && (vi[i1] != v2) )
                {
                /*
                -----------------------------
                Interpolate between i1 and i2
                ----------------------------- */
                x = (double) ( vi[i1] - eos_velocity );
                x /= (double) ( vi[i1] - v2 );
                x *= y;
                x += (double) f.position[i1];
                }
            else
                {
                x = (double) f.position[i1];
                }
            }
        /*
        ----------------------------------------------------
        P3_index is the index of the first post-impact point
        ---------------------------------------------------- */
        p3_index = i2;
        have_p3 = true;
        break;
        }

    i1--;
    i2--;
    }

/*
----------------------------------------------------------------------------------------------
If I did not find p3 between the min stroke length and the end of position based sampling then
the board must have switched to time based sampling at a velocity greater than the
eos_velocity. Look for P3 moving forward
---------------------------------------------------------------------------------------------- */
last_time_based_point = n - 1;
if ( !have_p3 )
    {
    i1 = f.last_position_based_point+1;
    i2 = i1 + 1;

    while ( true )
        {
        v1 = vi[i1];
        v2 = vi[i2];

        if ( v1 >= eos_velocity && eos_velocity >= v2 )
            {
            if ( v1 == v2 )
                {
                x = (double) f.position[i2];
                }
            else
                {
                y = (double)( f.position[i2] - f.position[i1] );
                if ( y > 0.0 )
                    {
                    /*
                    -----------------------------
                    Interpolate between i1 and i2
                    ----------------------------- */
                    x = (double) ( v1 - eos_velocity );
                    x /= (double) ( v1 - v2 );
                    x *= y;
                    x += (double) f.position[i1];
                    }
                else
                    {
                    x = (double) f.position[i1];
                    }
                }
            /*
            ----------------------------------------------------
            P3_index is the index of the first post-impact point
            ---------------------------------------------------- */
            p3_index = i2;
            have_p3 = true;
            break;
            }
        if ( i2 == last_time_based_point )
            {
            i1 = f.last_position_based_point+1;
            i2 = i1 + 1;
            x = (double) f.position[i2];
            break;
            }
        i1++;
        i2++;
        }
    }

/*
--------------------------------------------------------------------------------
x is now the double value of the index. If I interpolated then there is probably
a fractional part. Convert this to the current distance units. This will not
work if FPM are used.
-------------------------------------------------------------------------------- */
p3_position        = x;
x                  = part.dist_from_x4( p3_position );
iparm[P3_POS_PARM] = x;
set_source_data( idata,  P3_POS_PARM, p3_position, f, part );

p2_position        = x - double( part.runner_fill_distance );
iparm[P2_POS_PARM] = p2_position;
p2_position        = part.x4_from_dist( p2_position );
set_source_data( idata,  P2_POS_PARM, p2_position, f, part );

p1_position        = x - double( part.sleeve_fill_distance );
iparm[P1_POS_PARM] = p1_position;
p1_position        = part.x4_from_dist( p1_position );
set_source_data( idata,  P1_POS_PARM, p1_position, f, part );

iparm[FILL_TIME] = f.ms_between_positions( p2_position, p3_position );

/*
-----------------------------
Calculated Start of Fast Shot
----------------------------- */
x =  part.x4_from_dist( double(part.csfs_min_position) );
i1 = (int) round( x, 1.0 );

if ( i1 >= f.last_position_based_point )
    i1 = 0;

x = part.x4_from_velocity( double(part.csfs_min_velocity) );
csfs_velocity = (int) round( x, 1.0 );
vi = f.velocity;

if ( vi[i1] >= csfs_velocity )
    {
    x = (double) i1;
    }
else
    {
    i2 = i1;
    while ( true )
        {
        i2++;
        if ( i2 > f.last_position_based_point )
            {
            x = 0.0;
            break;
            }
        if ( vi[i2] == csfs_velocity )
            {
            x = (double) f.position[i2];
            break;
            }
        if ( vi[i2] > csfs_velocity )
            {
            /*
            -----------------------------
            Interpolate between i1 and i2
            ----------------------------- */
            x = 0.0;
            if ( vi[i2] != vi[i1] )
                {
                x  = (double) csfs_velocity - vi[i1];
                x /= (double) ( vi[i2] - vi[i1] );
                }
            x += (double) i1;
            break;
            }
        i1++;
        }
    }

csfs_position = x;
iparm[CSFS] = part.dist_from_x4( csfs_position );
set_source_data( idata,  CSFS, csfs_position, f, part );

iparm[SLOW_SHOT_TIME]  = f.ms_between_positions(           0.0, csfs_position );
iparm[FAST_SHOT_TIME]  = f.ms_between_positions( csfs_position, p3_position   );
iparm[TOTAL_SHOT_TIME] = f.ms_between_positions(           0.0, p3_position   );

x = part.x4_from_velocity( part.csfs_rise_velocity );
j = (int) round( x, 1.0 );

i1 = (int) floor( csfs_position );
i2 = i1 + 1;

while ( i2 <= f.last_position_based_point )
    {
    if ( f.velocity[i1] <= j && j <= f.velocity[i2] )
        {
        x = f.interpolated_postion_from_velocity( j, i1, i2 );
        x                = f.ms_between_positions( csfs_position, x );
        iparm[RISE_TIME] = x;
        break;
        }

    i1++;
    i2++;
    }

/*
----------------------------------------------------------------------------------------------
Create pressure curve
This is used to calculate the internal parameters. Then, if it is not already the differential
equation it is reloaded for calculating the user defined parameters.
---------------------------------------------------------------------------------------------- */
pressure = new double[f.n];
if ( !pressure )
    return 0;

differential_curve_number = int( machine.diff_curve_number );

ch = 1.0;
cr = 1.0;

hi = part.head_pressure_channel - 1;
ri = part.rod_pressure_channel - 1;
need_head_pressure = TRUE;
need_rod_pressure  = FALSE;

if ( hi < 0 || hi >= f.nof_analogs )
    {
    hi = NO_INDEX;
    need_head_pressure = FALSE;
    need_rod_pressure  = TRUE;
    }

if ( ri < 0 || ri >= f.nof_analogs )
    {
    ri = NO_INDEX;
    need_rod_pressure = FALSE;
    }

ah = DEFAULT_ANALOG_SENSOR;
if ( hi != NO_INDEX )
    ah = part.analog_sensor[hi];

ar = DEFAULT_ANALOG_SENSOR;
if ( ri != NO_INDEX )
    ar = part.analog_sensor[ri];

/*
-----------------------------------------------
The default is to copy the head pressure values
----------------------------------------------- */
if ( need_head_pressure )
    need_rod_pressure = FALSE;

/*
-------------------------------------------------------------------------------
Although the machine has entries for pre and post impact pressure types
only the post is used at this time so I use this for calculating the pre impact
points as well. Internal parameters that use this are all post-impact.
------------------------------------------------------------------------------- */
parameter_pressure_type = machine.impact_pres_type[POST_IMPACT_INDEX];

if ( parameter_pressure_type & HEAD_PRESSURE_CURVE )
    {
    if ( hi != NO_INDEX )
        {
        ch = 1.0;
        need_head_pressure = TRUE;
        need_rod_pressure  = FALSE;
        }
    }
else if ( parameter_pressure_type & ROD_PRESSURE_CURVE )
    {
    if ( ri != NO_INDEX )
        {
        cr = 1.0;
        need_head_pressure = FALSE;
        need_rod_pressure  = TRUE;
        }
    }
else if ( parameter_pressure_type & DIFF_PRESSURE_CURVE )
    {
    if ( ri != NO_INDEX && hi != NO_INDEX )
        {
        if ( differential_curve_number >= LOW_METAL_PRESSURE_DIFF_CURVE )
            {
            get_dcurve_coefficients( ch, cr, differential_curve_number  );
            area = part.plunger_area();
            ch /= area;
            cr /= area;
            need_rod_pressure = TRUE;
            }
        }
    }

j = 0;
for ( i=0; i<f.np; i++ )
    {
    pressure[j] = 0.0;

    if ( need_head_pressure )
        pressure[j] += ch * asensor[ah].converted_value( f.analog[hi][j]  );

    if ( need_rod_pressure )
        pressure[j] += cr * asensor[ar].converted_value( f.analog[ri][j] );
    j++;
    }

if ( parameter_pressure_type & HEAD_PRESSURE_CURVE )
    {
    if ( hi != NO_INDEX )
        {
        ch = 1.0;
        need_head_pressure = TRUE;
        need_rod_pressure  = FALSE;
        }
    }

else if ( parameter_pressure_type & ROD_PRESSURE_CURVE )
    {
    if ( ri != NO_INDEX )
        {
        cr = 1.0;
        need_head_pressure = FALSE;
        need_rod_pressure  = TRUE;
        }
    }
else if ( parameter_pressure_type & DIFF_PRESSURE_CURVE )
    {
    if ( ri != NO_INDEX && hi != NO_INDEX )
        {
        if ( differential_curve_number != NO_DIFF_CURVE_NUMBER )
            {
            get_dcurve_coefficients( ch, cr, differential_curve_number  );
            area = part.plunger_area();
            ch /= area;
            cr /= area;
            need_head_pressure = TRUE;
            need_rod_pressure  = TRUE;
            }
        }
    }

for ( i=0; i<f.nt; i++ )
    {
    pressure[j] = 0.0;

    if ( need_head_pressure )
        pressure[j] += ch * asensor[ah].converted_value( f.analog[hi][j] );

    if ( need_rod_pressure )
        pressure[j] += cr * asensor[ar].converted_value( f.analog[ri][j] );
    j++;
    }

/*
----------------------------------------------------------------------
                   Intensification Pressure
The time_for_intens_pres is set, in ms, in the part setup. I just
divide this by the number of ms per post impact sample, index from the
last position based point, and read the rod pressure.
---------------------------------------------------------------------- */
i2 = part.time_for_intens_pres;
i  = f.us_per_time_sample;
i /= 1000;
if ( i < 1 )
    i = 1;
i2 /= i; /* = time sample index */
i2 += f.last_position_based_point;
if ( i2 >= f.n )
    i2 = f.n-1;

iparm[INTENS_PRESSURE] = pressure[i2];
set_source_data( idata, INTENS_PRESSURE, i2, f, part );

/*
-----------------------------------------------------------------------------
Effective Peak Intens (PSI,BAR)
The pressure recorded at the maximum position reached during intensification.
Skip EpiSkipCount points before starting to look so I don't get confused
by overshoot on the ram.
----------------------------------------------------------------------------- */
i = f.last_position_based_point + 1;
i += epi_skip_count;
if ( i >= f.n )
    i = f.n - 1;

max_epi_position = f.position[i];
max_analog_value = pressure[i];
i++;

while ( i < f.n )
    {
    current_position = f.position[i];
    x = pressure[i];
    if ( current_position > max_epi_position || (current_position == max_epi_position && x > max_analog_value) )
        {
        max_epi_position = current_position;
        max_analog_value = x;
        }
    i++;
    }

iparm[EFF_PEAK_INTENS] = max_analog_value;
set_source_data( idata, EFF_PEAK_INTENS, max_epi_position, f, part );

/*
--------------------
Get the ms at impact
-------------------- */
impact_time = f.interpolated_ms( p3_position );

/*
--------------------------------------------------
Time of first max position point in ms past impact
-------------------------------------------------- */
max_position_time = f.ms(max_position_index) - impact_time;
ftcalc.sp[IST_INDEX].desc  = IstName;
ftcalc.sp[IST_INDEX].value = max_position_time;

/*
-----------------------------
Intensification Response Time
----------------------------- */
target = (double) part.pres_for_response_time;
last_point_side = TARGET_TRACK_INIT;

for ( i=p3_index; i<f.n; i++ )
    {
    x = pressure[i];
    if ( x > target )
        {
        if ( last_point_side == BELOW_TARGET )
            break;
        last_point_side = ABOVE_TARGET;
        }
    else if ( x < target )
        {
        if ( last_point_side == ABOVE_TARGET )
            break;
        last_point_side = BELOW_TARGET;
        }
    else  /* x == target */
        {
        break;
        }
    }

if ( i >= f.n )
    x = 0.0;
else
    x = f.ms(i) - impact_time;

iparm[INTENS_RESPONSE_TIME] = x;
if ( i < f.n )
    set_source_data( idata, INTENS_RESPONSE_TIME, i, f, part );

/*
-----------------------
Impact pressure to peak
----------------------- */
i = p3_index - int( machine.impact_points[PRE_IMPACT_INDEX] );
maxint( i, 0 );
max_analog_value = pressure[i];
i++;
while ( i<p3_index )
    {
    x = pressure[i];
    if ( x > max_analog_value )
        max_analog_value = x;
    i++;
    }

/*
------------------------------
Check the n post impact points
------------------------------ */
i = p3_index + int( machine.impact_points[POST_IMPACT_INDEX] );
minint( i, f.n-1 );
max_analog_value = pressure[i];
max_analog_index = i;
while ( i >= p3_index  )
    {
    x = pressure[i];
    if ( x > max_analog_value )
        {
        max_analog_value = x;
        max_analog_index = i;
        }
    i--;
    }

iparm[IMPACT_PRES_TO_PEAK] = max_analog_value;
set_source_data( idata, IMPACT_PRES_TO_PEAK, max_analog_index, f, part );

/*
--------------------------------
Intensification Squeeze Distance
-------------------------------- */
x = (double) max_position;
x -= p3_position;
iparm[INTENS_SQUEEZE_DIST] = part.dist_from_x4( x );

/*
-------------------------------------------------------
Sleeve Full Velocity (IPS,FPM,CMPS,MPS)
The plunger velocity measured at P1 (Shot Sleeve Full).
------------------------------------------------------- */
x = f.interpolated_velocity( p1_position );
iparm[SLEEVE_FULL_VELOCITY] = part.velocity_from_x4( x );
set_source_data( idata, SLEEVE_FULL_VELOCITY, p1_position, f, part );

/*
--------------------
Runner Full Velocity
-------------------- */
x = f.interpolated_velocity( p2_position );
iparm[RUNNER_FULL_VELOCITY] = part.velocity_from_x4( x );
set_source_data( idata, RUNNER_FULL_VELOCITY, p2_position, f, part );

/*
------------------------------------------------------------------------
Avg Fill Velocity (IPS,FPM,CMPS,MPS)
The average plunger velocity measured between position P2 and the End of
Shot Velocity (EOSV) position P3.
------------------------------------------------------------------------ */
x = f.velocity_between_positions( p2_position, p3_position );
iparm[AVG_FILL_VELOCITY] = part.velocity_from_x4( x );

/*
--------------------------------------------------------------
User Velocity Positions One - Four (IPS,FPM,CMPS,MPS)
The velocity(ies) measured at the Positions at Which to Record
Velocities set in Part Setup.
-------------------------------------------------------------- */
j = USER_VEL_1;
for ( i=0; i<USER_VEL_COUNT; i++ )
    {
    x = part.user_velocity_position[i];
    if ( not_zero(x) )
        {
        x = part.x4_from_dist( x );
        set_source_data( idata, j, x, f, part );

        x = f.interpolated_velocity( x );
        iparm[j] = part.velocity_from_x4( x );
        }
    else
        {
        iparm[j] = 0.0;
        }
    j++;
    }

/*
-----------------------------------------------------------------------------
User Avg Vel Range One to Four (IPS,FPM,CMPS,MPS)
The average plunger velocity recorded between 2 setpoints entered by the user
in Position Ranges Over Which to Record Average Velocities in Part Setup.
----------------------------------------------------------------------------- */

j = USER_AVG_VEL_1;
for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    x = double( part.user_avg_velocity_start[i] );
    y = double( part.user_avg_velocity_end[i] );
    if ( y > x )
        {
        x = part.x4_from_dist( x );
        y = part.x4_from_dist( y );
        x = f.velocity_between_positions( x, y );
        iparm[j] = part.velocity_from_x4( x );
        }
    else
        {
        iparm[j] = 0.0;
        }
    j++;
    }

/*
-----------------------------------------------------------------
Avg Slow Shot Velocity (IPS,CMPS,MPS)
The average velocity recorded between the initial movement of the
plunger and the Calculated Start of Fast Shot.
----------------------------------------------------------------- */
x = f.velocity_between_positions( 0.0, csfs_position );
iparm[AVG_SLOW_SHOT_VELOCITY] = part.velocity_from_x4( x );

/*
-----------------------------------------------------------------------
Avg Fast Shot Velocity (IPS,FPM,CMPS,MPS)
The average velocity recorded between the Calculated Start of Fast Shot
position to the End of Shot Velocity position.
----------------------------------------------------------------------- */
x = f.velocity_between_positions( csfs_position, p3_position );
iparm[AVG_FAST_SHOT_VELOCITY] = part.velocity_from_x4( x );

/*
----------------------------------------------------------------------
Avg Gate Velocity
The average velocity of metal thru the gate(s). This is equal to the
plunger velocity times the ratio of plunger area over total gate area.
---------------------------------------------------------------------- */
x = f.velocity_between_positions( p2_position, p3_position );
iparm[AVG_GATE_VELOCITY]  = part.velocity_from_x4( x );

iparm[FAST_SHOT_DISTANCE] = part.dist_from_x4( p3_position - csfs_position );

iparm[PRE_IMPACT_WORK]    = 0.0;

dest = new double[MAX_PARMS];
for ( p=0; p<MAX_PARMS; p++ )
    dest[p] = 0.0;

/*
-----------------------------------------------
Copy the internal parameters to the global list
----------------------------------------------- */
for ( p=0; p<MAX_PARMS; p++ )
    {
    if ( param.parameter[p].input.type == INTERNAL_PARAMETER )
        {
        i = param.parameter[p].input.number - 1;
        if ( i >= 0 && i < MAX_INTERNAL_PARMS )
            {
            dest[p] = iparm[i];
            ftcalc.sourcedata[p] = idata[i];
            }
        }

    }

/*
--------------------------------------------------------------------------------------
I am through using the pressure curve. Now I am going to put the differential curve
values into the pressure curve so they can be used by the user defined parameters.
If the differential curve number is a metal pressure number then I will put the values
into the pre impact as well.
-------------------------------------------------------------------------------------- */
if ( differential_curve_number != NO_DIFF_CURVE_NUMBER )
    {
    if ( !(parameter_pressure_type & DIFF_PRESSURE_CURVE) && (ri != NO_INDEX) && (hi != NO_INDEX) )
        {
        get_dcurve_coefficients( ch, cr, differential_curve_number  );
        area = part.plunger_area();
        ch /= area;
        cr /= area;

        if ( differential_curve_number >= LOW_METAL_PRESSURE_DIFF_CURVE )
            i = 0;
        else
            i = f.np;

        while ( i<f.n )
            {
            pressure[i] =  ch * asensor[ah].converted_value( f.analog[hi][i]  );
            pressure[i] += cr * asensor[ar].converted_value( f.analog[ri][i] );
            i++;
            }
        }
    }

for ( p=0; p<MAX_PARMS; p++ )
    {
    if ( param.parameter[p].input.type == FT_ANALOG_INPUT || param.parameter[p].input.type == FT_TARGET_INPUT )
        {
        c = ftanalog.array[p].channel;
        if ( c != FT_ANALOG_VELOCITY_CHANNEL )
            c--;
        i = c;

        /*
        ---------------------------------------------------------------------------------
        The  channel is set to MAX_FT_CHANNELS if the differential channel is to be used.
        --------------------------------------------------------------------------------- */
        if ( i >= MAX_FT_CHANNELS && i != FT_ANALOG_VELOCITY_CHANNEL )
            {
            if ( hi != NO_INDEX )
                i = hi;
            else
                i = ri;
            }

        /*
        ---------------------------------------------------------------
        I have to check for -1 because I may be looking for pos or time
        --------------------------------------------------------------- */
        a = 0;
        if ( i >= 0 && i != FT_ANALOG_VELOCITY_CHANNEL )
            a = part.analog_sensor[i];
        variable_type = ftanalog.array[p].ind_var_type;

        x = ftcalc.value( ftanalog.array[p].ind_value, dest, part );

        /*
        -------------------------------------------------------------------------------
        In the old code I would use an end value of 10000 if the command began with "A"
        I do not remember why I did that. It won't work any more but was not documented
        do I don't expect that anyone ever used it.
        ------------------------------------------------------------------------------- */
        y = ftcalc.value( ftanalog.array[p].end_value, dest, part );

        start_is_pos_end_is_time = FALSE;
        if ( (variable_type & TIME_VAR) && (x < 0.0) )
            start_is_pos_end_is_time = TRUE;
        if ( param.parameter[p].input.type == FT_ANALOG_INPUT )
            {
            i = -1;
            if ( variable_type & POSITION_VAR || start_is_pos_end_is_time )
                {
                x = part.x4_from_dist(x);
                if ( x < 0.0 )
                    x += p3_position;
                i = f.index_from_position( x );

                if ( variable_type & POSITION_VAR )
                    {
                    if ( ftanalog.array[p].result_type != SINGLE_POINT_RESULT_TYPE )
                        {
                        y = part.x4_from_dist(y);
                        if ( y < 0.0 )
                            y += p3_position;
                        j = f.index_from_position( y );
                        }
                    else
                        {
                        j = i;
                        }
                    }
                }
            if ( variable_type & TIME_VAR )
                {
                if ( !start_is_pos_end_is_time )
                    i = f.index_from_time( x );
                if ( ftanalog.array[p].result_type != SINGLE_POINT_RESULT_TYPE )
                    j = f.index_from_time( y );
                else
                    j = i;
                }

            if ( i >= 0 )
                {
                /*
                ------------------------------------------------------------------------
                If I am taking the difference then just subtract the start from the end.
                It doesn't matter which position is less.
                ------------------------------------------------------------------------ */
                if ( ftanalog.array[p].result_type & SUB_RESULT_TYPE )
                    {
                    if ( c == NO_INDEX )
                        {
                        if ( variable_type & TIME_VAR )
                            {
                            x = part.dist_from_x4( double(f.position[i]) );
                            y = part.dist_from_x4( double(f.position[j]) );
                            }
                        else
                            {
                            x = f.ms(i);
                            y = f.ms(j);
                            }
                        }
                    else if ( 0 <= c && c < MAX_FT_CHANNELS )
                        {
                        x = asensor[a].converted_value( f.analog[c][i] );
                        y = asensor[a].converted_value( f.analog[c][j] );
                        }
                    else if ( c == FT_ANALOG_VELOCITY_CHANNEL )
                        {
                        x = f.velocity[i];
                        x = part.velocity_from_x4( x );
                        y = f.velocity[j];
                        y = part.velocity_from_x4( y );
                        }
                    else
                        {
                        x = pressure[i];
                        y = pressure[j];
                        }

                    dest[p] = y - x;
                    }
                else
                    {
                    if ( j < i )
                        {
                        n = i;
                        i = j;
                        j = n;
                        }
                    if ( c == NO_INDEX )
                        {
                        if ( variable_type & TIME_VAR )
                            max_analog_value = part.dist_from_x4( double(f.position[i]) );
                        else
                            max_analog_value = f.ms(i);
                        }
                    else if ( 0 <= c && c < MAX_FT_CHANNELS )
                        {
                        max_analog_value = asensor[a].converted_value( f.analog[c][i]  );
                        }
                    else if ( c == FT_ANALOG_VELOCITY_CHANNEL )
                        {
                        max_analog_value = 0.0;
                        }
                    else
                        {
                        max_analog_value = pressure[i];
                        }

                    min_analog_index = i;
                    max_analog_index = i;
                    min_analog_value = max_analog_value;
                    sum = 0.0;
                    n = 1 + j - i;
                    while (i <= j)
                        {
                        if ( c == NO_INDEX )
                            {
                            if ( variable_type & TIME_VAR )
                                x = part.dist_from_x4( double(f.position[i]) );
                            else
                                x = f.ms(i);
                            }
                        else if ( 0 <= c && c < MAX_FT_CHANNELS )
                            {
                            x = asensor[a].converted_value( f.analog[c][i]  );
                            }
                        else if ( c == FT_ANALOG_VELOCITY_CHANNEL )
                            {
                            x = f.velocity[i];
                            x = part.velocity_from_x4( x );
                            }
                        else
                            {
                            x = pressure[i];
                            }
                        sum += x;
                        if ( x < min_analog_value )
                            {
                            min_analog_value = x;
                            min_analog_index = i;
                            }

                        if ( x > max_analog_value )
                            {
                            max_analog_value = x;
                            max_analog_index = i;
                            }
                        i++;
                        }
                    sum /= double(n);
                    if ( ftanalog.array[p].result_type & MAX_RESULT_TYPE )
                        {
                        dest[p] = max_analog_value;
                        set_source_data( ftcalc.sourcedata, p, max_analog_index, f, part );
                        }
                    else if ( ftanalog.array[p].result_type & MIN_RESULT_TYPE )
                        {
                        dest[p] = min_analog_value;
                        set_source_data( ftcalc.sourcedata, p, min_analog_index, f, part );
                        }
                    else
                        dest[p] = sum;
                    }
                }
            }
        else
            {
            /*
            --------------------------------------
            Fast shot target position
            The end_value is where I start looking
            -------------------------------------- */
            target = asensor[a].raw_count( x );
            x = ftcalc.value( ftanalog.array[p].end_value, dest, part );
            x = part.x4_from_dist( x );
            i = f.index_from_position( x );

            if ( i <=0 || i > f.last_position_based_point )
                i = f.last_position_based_point / 2;

            up = f.analog[c];
            up += i;

            while ( i < f.np )
                {
                x = double( *up );
                if ( variable_type & TRIGGER_WHEN_SMALLER )
                    {
                    if ( x <= target )
                        break;
                    }
                else
                    {
                    if ( x >= target )
                        break;
                    }
                i++;
                up++;
                }

            if ( i > f.last_position_based_point )
                i = f.last_position_based_point;

            dest[p] = part.dist_from_x4( f.position[i] );
            set_source_data( ftcalc.sourcedata, p, i, f, part );
            }
        }
    else if ( param.parameter[p].input.type == FT_DIGITAL_INPUT )
        {
        wire = (unsigned ) param.parameter[p].input.number;
        mask = mask_from_wire( wire );
        i = f.n;
        i--;
        if ( f.isw1[i] & mask )
            dest[p] = 1.0;
        else
            dest[p] = 0.0;
        }
    else if ( param.parameter[p].input.type == FT_ON_OFF_INPUT )
        {
        wire = (unsigned ) param.parameter[p].input.number;
        mask = mask_from_wire( wire );

        variable_type = ftanalog.array[p].ind_var_type;
        /*
        -------------------------------------------
        I need to look for the opposite level first
        ------------------------------------------- */
        if ( variable_type & TRIGGER_WHEN_GREATER )
            u = 0;
        else
            u = mask;

        /*
        -------------------------------------
        Set the default transition point at 0
        ------------------------------------- */
        i1 = 0;

        for ( i=0; i<f.np; i++ )
            {
            if ( (f.isw1[i] & mask) == u )
                break;
            }

        u ^= mask;
        i++;
        while ( i < f.np )
            {
            if ( (f.isw1[i] & mask) == u )
                {
                i1 = i;
                break;
                }
            i++;
            }

        dest[p] = part.dist_from_x4( f.position[i1] );
        }
    else if ( param.parameter[p].input.type == FT_EXTENDED_ANALOG_INPUT )
        {
        c = ftanalog.array[p].channel - 1;
        if ( c < 0 || c >= MAX_FT2_CHANNELS )
            c = 0;
        dest[p] = 0.0;
        u = 0;
        wire = (unsigned) ftanalog.array[p].ind_value.int_value();

        if ( wire == 0 )
            {
            /*
            ------------------------------------------------------------------------------------
            Wire 0 means take the sample from the end of the shot. If it is
            an extended channel [8-16] then this is in the static analogs in the ftii_parameters
            otherwise I need to get it from the last sample point.
            ------------------------------------------------------------------------------------ */
            if ( c >= MAX_FT_CHANNELS )
                {
                i = c - MAX_FT_CHANNELS + FIRST_STATIC_ANALOG_INDEX;
                if ( i < NOF_FTII_FILE_PARAMETERS )
                    u = f.parameter[i].uvalue;
                }
            else
                {
                i = f.n;
                i--;
                u = f.analog[c][i];
                }
            }
        else
            {
            for ( i=0; i<f.nof_extended_analogs; i++ )
                {
                if ( f.extended_analog[i].parameter_number == p+1 )
                    {
                    u = (unsigned) f.extended_analog[i].value;
                    break;
                    }
                }
            }

        u &= 0xFFF;
        a = part.analog_sensor[c];
        dest[p] = asensor[a].converted_value( u );
        }
    else if ( param.parameter[p].input.type == FT_CALC_INPUT )
        {
        dest[p] = ftcalc.value( p+1, dest, part );
        }
    else if ( param.parameter[p].input.type == FT_PEAK_FS_VELOCITY )
        {
        x = get_position( dest, p, p3_position, ftanalog.array[p].ind_value );
        x = part.x4_from_dist(x);
        i = f.index_from_position( x );

        y = get_position( dest, p, p3_position, ftanalog.array[p].end_value );
        y = part.x4_from_dist(y);
        j = f.index_from_position( y );
        x = f.max_average_velocity( target_index, i, j, ftanalog.array[p].smooth_n.real_value(), ftanalog.array[p].smooth_multiplier.real_value() );
        dest[p] = part.velocity_from_x4( x );
        set_source_data( ftcalc.sourcedata, p, target_index, f, part );
        }
    else if ( param.parameter[p].input.type == FT_EXTERNAL_PARAMETER )
        {
        /*
        --------------------------------------------------------------------
        I try to convert all external variables.
        STRING_CLASS real_value() returns 0 for strings that are not numbers
        -------------------------------------------------------------------- */
        for ( i=0; i<f.nof_text_parameters; i++ )
            {
            j = f.text_parameter[i].parameter_number - 1;
            if ( p == j )
                {
                dest[p] = f.text_parameter[i].value.real_value();
                break;
                }
            }
        }
    }

delete[] pressure;

return dest;
}
