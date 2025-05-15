#include <windows.h>
#include <cmath>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\ftanalog.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\stringtable.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\limit_switch_class.h"
#include "..\include\ftcalc.h"

#include "extern.h"

const int TIME_ONLY_DISPLAY = 3;    /* Defined in profile.cpp */
extern int CurrentDisplay;          /* Defined in profile.cpp */

static const float MaxFloatShort = float( 0xFFFF );
double * calculate_ftii_parameters( FTII_PROFILE & f, MACHINE_CLASS & machine, PART_CLASS & part, PARAMETER_CLASS & param, FTANALOG_CLASS & ftanalog, FTCALC_CLASS & ftclass, LIMIT_SWITCH_CLASS &limits );
float running_average( float * raw, int32 true_index, int32 final_index );

/***********************************************************************
*                              DAC_VOLTS                               *
***********************************************************************/
static double dac_volts( unsigned x )
{
double d;

d = double( x );
d /= 4095.0;
d *= 20.0;
d -= 10.0;
return round( d, 0.001 );
}

/***********************************************************************
*                           MAKE_DATE_STRING                           *
***********************************************************************/
static void make_date_string()
{
SYSTEMTIME st;

FileTimeToSystemTime( &Ph.time_of_shot, &st );

getday( DateString, st.wDayOfWeek );

copychars( DateString+DATESTRING_DATE_OFFSET, alphadate(st), ALPHADATE_LEN );
copychars( DateString+DATESTRING_TIME_OFFSET, alphatime(st), ALPHATIME_LEN );
}

/***********************************************************************
*                       FILL_DIFFERENTIAL_CURVE                        *
***********************************************************************/
BOOLEAN fill_differential_curve( MACHINE_CLASS machine, PART_CLASS & part )
{
int32   area;
float   ch;
float   cr;
float   n;
float * hp;
float * rp;
float * dp;

Ph.curves[DIFFERENTIAL_CURVE_INDEX].type = NO_CURVE_TYPE;

if ( part.head_pressure_channel == NO_FT_CHANNEL || part.rod_pressure_channel == NO_FT_CHANNEL )
    return FALSE;

/*
--------------------------------------------
Do nothing if there is no differential curve
-------------------------------------------- */
if ( machine.diff_curve_number == NO_DIFF_CURVE_NUMBER )
    return FALSE;

dp = Points[DIFFERENTIAL_CURVE_INDEX];

/*
----------------------------------------------------
Point hp at the beginning of the head pressure curve
---------------------------------------------------- */
hp = Points[HEAD_PRES_CURVE_INDEX];

/*
---------------------------------------------------
Point Rp at the beginning of the rod pressure curve
--------------------------------------------------- */
rp = Points[ROD_PRES_CURVE_INDEX];

/*
----------------------------------------------
Copy the units to the differential curve
(I assume they are the same for rod and head).
---------------------------------------------- */
Ph.curves[DIFFERENTIAL_CURVE_INDEX].units = CurrentPart.pressure_units;

/*
-------------------------
Fill the array with zeros
------------------------- */
n = Ph.n;
while ( n )
    {
    *dp++ = 0.0;
    n--;
    }

Ph.curves[DIFFERENTIAL_CURVE_INDEX].type  = DIFF_PRESSURE_CURVE;

/*
-----------------------------------------------
Get the coefficients for the differential curve
----------------------------------------------- */
get_dcurve_coefficients( ch, cr, machine.diff_curve_number );

/*
-------------------------------------------
Divide the coefficients by the plunger area
------------------------------------------- */
area = float( part.plunger_area() );
ch /= area;
cr /= area;

dp = Points[DIFFERENTIAL_CURVE_INDEX];

/*
--------------------------------------
Set the min and max to the first point
-------------------------------------- */
*dp = (*hp) * ch + (*rp) * cr;
Ph.curves[DIFFERENTIAL_CURVE_INDEX].max = *dp;
Ph.curves[DIFFERENTIAL_CURVE_INDEX].min = *dp;

n = Ph.n;
while ( n )
    {
    *dp = (*hp) * ch + (*rp) * cr;

    maxfloat( Ph.curves[DIFFERENTIAL_CURVE_INDEX].max, *dp );
    minfloat( Ph.curves[DIFFERENTIAL_CURVE_INDEX].min, *dp );

    dp++;
    hp++;
    rp++;
    n--;
    }

return TRUE;
}

/***********************************************************************
*                              FTII_VOLTS                              *
***********************************************************************/
static double ftii_volts( unsigned int count )
{
double x;

x = double( count );
x *= 5.0;
x /= 4095.0;

return x;
}

/***********************************************************************
*                           SMOOTH_ONE_CURVE                           *
***********************************************************************/
static void smooth_one_curve( float * yp, int n )
{
static float * buf = 0;
int32 last_index;
int32 i;

if ( !buf )
    buf = new float[MAX_FT2_POINTS];

last_index = n-1;

for ( i=0; i<n; i++ )
    buf[i] = running_average( yp+i, i, last_index );

for ( i=0; i<n; i++ )
    yp[i] = buf[i];
}

/***********************************************************************
*                            READ_FTII_PROFILE                         *
***********************************************************************/
static BOOL read_ftii_profile( TCHAR * fname )
{
static BITSETYPE ft_analog_curve_type[NOF_FTII_ANALOGS] =
    {
    CHAN_1_ANALOG_CURVE, CHAN_2_ANALOG_CURVE, CHAN_3_ANALOG_CURVE, CHAN_4_ANALOG_CURVE,
    CHAN_5_ANALOG_CURVE, CHAN_6_ANALOG_CURVE, CHAN_7_ANALOG_CURVE, CHAN_8_ANALOG_CURVE
    };

STRING_CLASS s;
FTII_PROFILE fp;
float      * yp;
double     * dp;
float        mymin;
double       x;
double       real_timer_frequency;
unsigned     * ap;
unsigned     umax;
unsigned     ux;
int          i;
int          j;
int          k;
int          last_point;
int          last_pos_based_point;
LIMIT_SWITCH_CLASS limits;
int          first_time_based_point;
int          imax;
int          imin;
int          ix;
int          curve_index;
int          ftii_curve_index;
int          analog_index;
BITSETYPE    my_type;
BITSETYPE    current_plot_types;
bool         is_dac;
short        x4_multiplier;

/*
-----------------------------------------------------
Get the current types of curves plotted on the screen
----------------------------------------------------- */
current_plot_types = PreImpactPlotCurveTypes;
current_plot_types |= PostImpactPlotCurveTypes;

s = fname;

if ( !fp.get(s) )
    return FALSE;

FileTimeToLocalFileTime( &fp.shot_time, &Ph.time_of_shot );

if ( ShowX1PointsOnly )
    x4_multiplier = X4_COUNTS_PER_MARK;
else
    x4_multiplier = 1;

Ph.n = (short) fp.n;
if ( ShowX1PointsOnly )
    Ph.n /= x4_multiplier;

Ph.last_pos_based_point = (short) fp.last_position_based_point;
if ( ShowX1PointsOnly )
    Ph.last_pos_based_point /= x4_multiplier;

last_pos_based_point = Ph.last_pos_based_point;
first_time_based_point = last_pos_based_point + 1;

NofPoints = Ph.n;
last_point = NofPoints - 1;

/*
--------------
Position Curve
-------------- */
yp = Points[POSITION_CURVE_INDEX];
yp[0] = 0;
for (i=1; i<NofPoints; i++ )
    {
    x = CurrentPart.dist_from_x4( (double) fp.position[i*x4_multiplier] );
    yp[i] = (float) x;
    }
i = fp.curve_index( FTII_POSITION_TYPE );
Ph.curves[POSITION_CURVE_INDEX].type  = POSITION_CURVE;
Ph.curves[POSITION_CURVE_INDEX].units = CurrentPart.distance_units;
x = (double) fp.curvedata[i].imax;
x = CurrentPart.dist_from_x4( x );
Ph.curves[POSITION_CURVE_INDEX].max   = float( x );
Ph.curves[POSITION_CURVE_INDEX].min   = 0.0;
PosCurve = POSITION_CURVE_INDEX;

/*
----------
Time Curve
---------- */
real_timer_frequency = (double) fp.timer_frequency;
umax = fp.timer[0];
yp = Points[TIME_CURVE_INDEX];
for (i=0; i<NofPoints; i++ )
    {
    ux = fp.timer[i*x4_multiplier];
    maxunsigned( umax, ux );
    x = (double) ux;
    x /= real_timer_frequency;
    yp[i] = (float) x;
    }

i = fp.curve_index( FTII_TIMER_COUNT_TYPE );
Ph.curves[TIME_CURVE_INDEX].type      = TIME_CURVE;
Ph.curves[TIME_CURVE_INDEX].units     = SECOND_UNITS;
x = umax;
x /= real_timer_frequency;
Ph.curves[TIME_CURVE_INDEX].max       = float( x );
Ph.curves[TIME_CURVE_INDEX].min       = 0.0;
TimeCurve = TIME_CURVE_INDEX;

/*
--------------
Velocity Curve
-------------- */
imax = fp.velocity[0];
imin = imax;

yp = Points[VELOCITY_CURVE_INDEX];
yp[0] = 0.0;
for ( i=1; i<NofPoints; i++ )
    {
    if ( ShowX1PointsOnly )
        {
        if ( (i == last_point) || (i == last_pos_based_point) || (i == first_time_based_point) )
            {
            ix = fp.velocity[i*x4_multiplier];
            x = (double) ix;
            }
        else
            {
            j = i*x4_multiplier - NofX4PointsToBackup;
            x = 0;
            for ( k=0; k<x4_multiplier; k++ )
                x += (double) fp.velocity[j++];
            x /= (double)x4_multiplier;
            ix = floor(x);
            }
        }
    else
        {
        ix = fp.velocity[i];
        x = (double) ix;
        }
    maxint( imax, ix );
    minint( imin, ix );
    x = CurrentPart.velocity_from_x4( x );
    yp[i] = (float) x;
    }

if ( Smoothing && SmoothingVelocity )
    smooth_one_curve( yp, NofPoints );

Ph.curves[VELOCITY_CURVE_INDEX].type  = VELOCITY_CURVE;
Ph.curves[VELOCITY_CURVE_INDEX].units = CurrentPart.velocity_units;

x = (double) imax;
x = CurrentPart.velocity_from_x4( x );
Ph.curves[VELOCITY_CURVE_INDEX].max = float( x );

x = (double) imin;
x = CurrentPart.velocity_from_x4( x );
Ph.curves[VELOCITY_CURVE_INDEX].min = float( x );

/*
-----------------------------------------------------------------------------
                               Analog Channels
The analog_index is the channel number - 1
I only have room for 4 curves so I load the four that are currently displayed
----------------------------------------------------------------------------- */
analog_index     = 0;
curve_index      = CHAN_1_ANALOG_CURVE_INDEX;
ftii_curve_index = fp.curve_index( FTII_ANALOG1_TYPE );

while ( curve_index <= CHAN_4_ANALOG_CURVE_INDEX )
    {
    if ( analog_index >= fp.nof_analogs )
        break;

    /*
    -------------------------------------------------
    Convert the ftii curve type to an ft channel type
    ------------------------------------------------- */
    my_type = ft_analog_curve_type[analog_index];

    i = analog_index + 1;
    if ( i == CurrentPart.head_pressure_channel )
        my_type |= HEAD_PRESSURE_CURVE;
    else if ( i == CurrentPart.rod_pressure_channel )
        my_type |= ROD_PRESSURE_CURVE;

    if ( my_type & current_plot_types )
        {
        Ph.curves[curve_index].type = my_type;

        /*
        --------------------------------------
        The actual type may be channels 17-20.
        -------------------------------------- */
        if ( fp.curvedata[ftii_curve_index].curve_type >= FTII_ANALOG17_TYPE )
            is_dac = true;
        else
            is_dac = false;

        if ( is_dac )
            {
            Ph.curves[curve_index].units = VOLTS_UNITS;
            Ph.curves[curve_index].max   = dac_volts( fp.curvedata[ftii_curve_index].umax );
            Ph.curves[curve_index].min   = dac_volts( fp.curvedata[ftii_curve_index].umin );
            }
        else
            {
            Ph.curves[curve_index].units = CurrentPart.analog_sensor[analog_index].units_index();
            Ph.curves[curve_index].max   = CurrentPart.analog_sensor[analog_index].converted_value(fp.curvedata[ftii_curve_index].umax);
            Ph.curves[curve_index].min   = CurrentPart.analog_sensor[analog_index].converted_value(fp.curvedata[ftii_curve_index].umin);
            if ( Ph.curves[curve_index].max < Ph.curves[curve_index].min )
                {
                mymin = Ph.curves[curve_index].max;
                Ph.curves[curve_index].max = Ph.curves[curve_index].min;
                Ph.curves[curve_index].min = mymin;
                }
            }

        yp = Points[curve_index];
        ap = fp.analog[analog_index];
        for (i=0; i<NofPoints; i++ )
            {
            if ( is_dac )
                yp[i] = (float) dac_volts( ap[i*x4_multiplier] );
            else
                yp[i] = (float)CurrentPart.analog_sensor[analog_index].converted_value(ap[i*x4_multiplier]);
            }

        if ( analog_index >=0 && analog_index < NOF_DYNAMIC_CHANNELS )
            {
            if ( Smoothing && SmoothingChannel[analog_index] )
                smooth_one_curve( yp, NofPoints );
            }

        curve_index++;
        }

    ftii_curve_index++;
    analog_index++;
    }

while ( curve_index < MAX_CURVES )
    {
    Ph.curves[curve_index].type = NO_CURVE_TYPE;
    curve_index++;
    }

/*
------------------------------------------------------------------------------
Do the head and rod pressure curves for the differential pressure calculations
------------------------------------------------------------------------------ */
i = CurrentPart.head_pressure_channel;
if ( i > 0 )
    {
    analog_index = i - 1;
    yp = Points[HEAD_PRES_CURVE_INDEX];
    ap = fp.analog[analog_index];
    if (analog_index < MAX_FT_CHANNELS)
        for (i=0; i<NofPoints; i++ )
            yp[i] = (float) CurrentPart.analog_sensor[analog_index].converted_value( ap[i*x4_multiplier] );

    if ( analog_index >=0 && analog_index < NOF_DYNAMIC_CHANNELS )
        {
        if ( Smoothing && SmoothingChannel[analog_index] )
            smooth_one_curve( yp, NofPoints );
        }
    }

i = CurrentPart.rod_pressure_channel;
if ( i > 0 )
    {
    analog_index = i - 1;
    yp = Points[ROD_PRES_CURVE_INDEX];
    ap = fp.analog[analog_index];
    if ( analog_index < NOF_DYNAMIC_CHANNELS )
        for (i=0; i<NofPoints; i++ )
            yp[i] = (float) CurrentPart.analog_sensor[analog_index].converted_value( ap[i*x4_multiplier] );

    if ( analog_index >=0 && analog_index < NOF_DYNAMIC_CHANNELS )
        {
        if ( Smoothing && SmoothingChannel[analog_index] )
            smooth_one_curve( yp, NofPoints );
        }
    }

/*
----------
Parameters
---------- */
NofParms = CurrentParam.count();
dp = calculate_ftii_parameters( fp, CurrentMachine, CurrentPart, CurrentParam, CurrentFtAnalog, CurrentFtCalc, limits );
if ( dp )
    {
    StringTable.find( s, TEXT("NONE_STRING") );
    for ( i=0; i<MAX_PARMS; i++ )
        {
        if ( CurrentParam.parameter[i].vartype == TEXT_VAR )
            {
            if ( CurrentParam.parameter[i].input.type == FT_STATIC_TEXT_PARAMETER || CurrentParam.parameter[i].input.type == FT_EXTERNAL_PARAMETER )
                /*
                ------------------------------------------------------------
                I'm loading a shot now so I need to empty the static text in
                preparation to loading from the shot file.
                ------------------------------------------------------------ */
                Parms[i] = s;
            }
        else
            {
            Parms[i] = dp[i];
            }
        }
    delete[] dp;
    dp = 0;
    }
else
    {
    for ( i=0; i<MAX_PARMS; i++ )
        Parms[i] = ZeroPointZero;
    }

for ( i=0; i<fp.nof_text_parameters; i++ )
    {
    j = fp.text_parameter[i].parameter_number - 1;
    if ( j >= 0 && j < MAX_PARMS )
        Parms[j] = fp.text_parameter[i].value;
    }

return TRUE;
}

/***********************************************************************
*                             LOAD_PROFILE                             *
***********************************************************************/
void load_profile( TCHAR * fname )
{
static int MySemaphor = 0;

if ( MySemaphor )
    return;

MySemaphor++;

if ( read_ftii_profile(fname) )
    {
    HaveHeader = TRUE;
    fill_differential_curve( CurrentMachine, CurrentPart );
    make_date_string();
    }

MySemaphor--;
}

/***********************************************************************
*                             LOAD_PROFILE                             *
***********************************************************************/
void load_profile()
{
NAME_CLASS s;
s.get_profile_ii_name( ComputerName, MachineName, PartName, ShotName );
load_profile( s.text() );
}

/***********************************************************************
*                             LOAD_PROFILE                             *
***********************************************************************/
void load_profile( PROFILE_NAME_ENTRY * p )
{
NAME_CLASS s;
s.get_profile_ii_name( p->computer, p->machine, p->part, p->shot );
load_profile( s.text() );
}

/***********************************************************************
*                      GET_REFERENCE_TRACE_NAME                        *
* Bupres backed up current.ma2 as current.mas for a long time. If it   *
* is an ma2 file the first two bytes are int 2 or 3 whereas if it is   *
* really an mas the first byte (or int if unicode) is a character      *
* >= '0'. Pass me the name of the ma2 file.                            *
***********************************************************************/
static BOOLEAN get_reference_trace_name( NAME_CLASS & dest )
{
NAME_CLASS sorc;
FILE_CLASS f;
int        i;
TCHAR    * cp;

if ( dest.file_exists() )
    return TRUE;

sorc = dest;
cp = sorc.text();
cp += sorc.len();
cp--;
*cp = TEXT( 'S' );

if ( sorc.file_exists() )
    {
    if ( f.open_for_read(sorc.text())  )
        {
        f.readbinary( &i, sizeof(i) );
        f.close();

        if ( i==2 || i==3 )
            {
            sorc.moveto( dest );
            return TRUE;
            }
        }
    }

return FALSE;
}

/***********************************************************************
*                         LOAD_REFERENCE_TRACE                         *
***********************************************************************/
BOOLEAN load_reference_trace()
{
NAME_CLASS fname;

fname.get_master_ii_name( ComputerName, MachineName, PartName );

if ( !get_reference_trace_name(fname) )
    return FALSE;

load_profile( fname.text() );
return TRUE;
}
