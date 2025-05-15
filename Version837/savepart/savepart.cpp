#include <windows.h>
#include <math.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\asensor.h"
#include "..\include\chaxis.h"
#include "..\include\color.h"
#include "..\include\dbclass.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\computer.h"
#include "..\include\limit_switch_class.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\mrunlist.h"
#include "..\include\param.h"
#include "..\include\param_index_class.h"
#include "..\include\part.h"
#include "..\include\marks.h"
#include "..\include\fileclas.h"
#include "..\include\ftanalog.h"
#include "..\include\stparam.h"
#include "..\include\stpres.h"
#include "..\include\stsetup.h"
#include "..\include\runlist.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\v5help.h"
#include "..\include\warmupcl.h"
#include "..\include\wclass.h"
#include "..\include\workclas.h"

#include "..\include\ftii.h"
#include "..\include\ftii_file.h"

#define _MAIN_
#include "savepart.h"
#include "resource.h"

#include "..\include\chars.h"
#include "..\include\event_client.h"

EVENT_CLIENT EventClient;

ANALOG_SENSOR CurrentAnalog;

HINSTANCE MainInstance;
HWND      MainWindow                  = 0;

/*
--------
SureTrak
-------- */
static BOOLEAN                 SureTrakHasChanged = FALSE;
static BOOLEAN                 HaveSureTrakPlot = FALSE;
static WINDOW_CLASS            SureTrakPlotWindow;
static SURETRAK_SETUP_CLASS    SureTrakSetup;
       SURETRAK_PARAM_CLASS    SureTrakParam;
static PRESSURE_CONTROL_CLASS  PressureControl;
static WARMUP_CLASS            Warmup;

/*
--------------
Ftii Constants
-------------- */
static const int FTII_VACUUM_WAIT_INPUT = 58;
static const int FTII_LOW_IMPACT_INPUT  = 60;

/*
----
Bits
---- */
static const unsigned Bit31 = 0x80000000;
static const unsigned Bit30 = 0x40000000;

/*
-----------------
Multipart runlist
----------------- */
static BOOLEAN                HaveMultipartRunlist = FALSE;

static const COLORREF BlackColor = RGB( 0,   0, 0 );
static const COLORREF RedColor   = RGB( 255, 0, 0 );

/*
--------------------------------
These are initialized in startup
-------------------------------- */
static       COLORREF EboxBackgroundColor = 0;
static       HBRUSH   EboxBackgroundBrush = 0;

static const int32    ST_POS_AXIS_ID  = 1;
static const int32    ST_VEL_AXIS_ID  = 2;
static const int32    ST_TIME_AXIS_ID = 1;
static const int32    ST_PRES_AXIS_ID = 2;

static HFONT MyFont;

HWND      TabControl;

TCHAR     MyClassName[]                 = TEXT( "SavePart" );
TCHAR     EditPartIniFile[]             = TEXT( "editpart.ini" );
TCHAR     SavePartIniFile[]             = TEXT( "savepart.ini" );
TCHAR     SureTrakIniFile[]             = TEXT( "suretrak.ini" );
TCHAR     VisiTrakIniFile[]             = TEXT( "visitrak.ini" );
TCHAR     ConfigSection[]               = TEXT( "Config" );
TCHAR     MultipartSetupSection[]       = TEXT( "Multipart" );
TCHAR     CsvSuffix[]                   = TEXT( ".csv" );
TCHAR     CsvHomeDirectoryKey[]         = TEXT( "CsvHomeDirectory" );
TCHAR     CurrentTabKey[]               = TEXT( "CurrentTab" );
TCHAR     HavePressureControlKey[]      = TEXT( "HavePressureControl" );
TCHAR     HaveSureTrakKey[]             = TEXT( "HaveSureTrak" );
TCHAR     HaveMultipartRunlistKey[]     = TEXT( "HaveMultipartRunlist" );
TCHAR     LogFileKey[]                  = TEXT( "LogFile" );
TCHAR     UseLogFileKey[]               = TEXT( "UseLogFile" );

STRING_CLASS       NewPartName;   /* If this is not empty then I am to create a new part with this name */
STRING_CLASS       CsvHomeDirectory;
NAME_CLASS         LogFile;
MACHINE_CLASS      CurrentMachine;
PART_CLASS         CurrentPart;
PARAMETER_CLASS    CurrentParam;
PARAM_INDEX_CLASS  CurrentSort;
MARKS_CLASS        CurrentMarks;
FTANALOG_CLASS     CurrentFtAnalog;
LIMIT_SWITCH_CLASS CurrentLimitSwitch;

BOOLEAN         HaveCurrentPart     = FALSE;
BOOLEAN         HavePressureControl = FALSE;
BOOLEAN         HaveSureTrakControl = FALSE;
BOOLEAN         UseLogFile          = FALSE;
static int      AlarmIndex[NOF_ALARM_LIMIT_TYPES] = { WARNING_MIN, WARNING_MAX, ALARM_MIN, ALARM_MAX };

static MULTIPART_RUNLIST_CLASS MultipartRunlist;
static BOOLEAN                 MultipartRunlistHasChanged = FALSE;
static STRING_CLASS            NotUsedString;                         /* Used by Multipart Runlist load/save */

struct ADD_MARKS_ENTRY
    {
    TCHAR       parameter_type;
    short       parameter_number;
    };

static  ADD_MARKS_ENTRY AddMarksList[MAX_MARKS];
static  short   NofAddMarks;

static TCHAR LowerXString[]  = TEXT("x");
static TCHAR PercentString[] = TEXT("%");
static TCHAR OneString[]     = TEXT("1");
static TCHAR ZeroString[]    = TEXT("0");

BOOLEAN touni( const TCHAR * dest, const TCHAR * sorc );
static void load_extended_channels();

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                               WRITELOG                               *
***********************************************************************/
static bool writelog( TCHAR * name, TCHAR * sorc )
{
FILE_CLASS   f;
STRING_CLASS s;
TIME_CLASS   t;

if ( !UseLogFile )
    return false;

if ( !f.open_for_append(LogFile) )
    return false;
t.get_local_time();
s = t.text();
s += CommaChar;
s += name;
s += CommaChar;
s += sorc;
f.writeline( s );
f.close();
return true;
}

/***********************************************************************
*                     SEND_NEW_MONITOR_SETUP_EVENT                     *
***********************************************************************/
static void send_new_monitor_setup_event()
{
STRING_CLASS  s;
RUNLIST_CLASS r;

/*
---------------------------------------------------------------------------
See if this is the current part. Check the runlist first as the warmup part
might be current.
--------------------------------------------------------------------------- */
if ( r.get(CurrentMachine.computer, CurrentMachine.name) )
    s = r[0].partname;
else
    s = CurrentMachine.current_part;

if ( s == CurrentPart.name )
    {
    if ( HaveMultipartRunlist )
        {
        MultipartRunlist.create_runlist( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        }
    else
        {
        Warmup.create_runlist( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        }
    }

/*
---------------------------------------------------------------------------------------
Always make this the current part. This has the added benifit of making
sure an alternate part has not been left in the current part field of the machset table
--------------------------------------------------------------------------------------- */
current_part_name( s, CurrentPart.computer, CurrentPart.machine );
if ( s != CurrentPart.name )
    {
    writelog( TEXT("Set Current Part"), CurrentPart.name.text() );
    set_current_part_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    if ( HaveMultipartRunlist )
        {
        MultipartRunlist.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        MultipartRunlist.create_runlist( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        }
    else
        {
        Warmup.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        Warmup.create_runlist( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        }

    }

if ( CurrentMachine.monitor_flags & MA_MONITORING_ON )
    EventClient.poke_data( MONITOR_SETUP_INDEX, CurrentMachine.name.text() );
}

/***********************************************************************
*                               EBOX_FLOAT                             *
***********************************************************************/
float ebox_float( HWND w, int editbox_id )
{
UINT  n;
TCHAR s[31];

n = GetDlgItemText( w, editbox_id, s, 30 );
if ( n > 0 )
    return extfloat(s, short(n));

return 0.0;
}

/***********************************************************************
*                              MARK_NAME                               *
***********************************************************************/
TCHAR * mark_name( TCHAR type, short number )
{
static TCHAR s[PARAMETER_NAME_LEN+1];
short i;
int string_id;

lstrcpy( s, UNKNOWN );
i = number;
i--;
if ( type == PARAMETER_MARK )
    {
    if ( i >= 0 && i < CurrentParam.count() )
        lstrcpy( s, CurrentParam.parameter[i].name );
    }
else
    {
    if ( type == START_POS_MARK )
        string_id = START_POS_MARK_STRING + i;
    else if ( type == END_POS_MARK )
        string_id = END_POS_MARK_STRING + i;
    else
        string_id = VEL_POS_MARK_STRING + i;

    LoadString( MainInstance, string_id, s, sizeof(s) );
    }

return s;
}

/***********************************************************************
*                              MARK_NAME                               *
***********************************************************************/
TCHAR * mark_name( MARKLIST_ENTRY & m )
{
return mark_name( m.parameter_type, m.parameter_number );
}

/***********************************************************************
*                              CAT_W_TAB                               *
***********************************************************************/
static void cat_w_tab( TCHAR * dest, const TCHAR * sorc )
{
lstrcat( dest, sorc );
lstrcat( dest, TEXT("\t") );
}
/***********************************************************************
*                         mv_from_PERCENT                              *
*    Converts to signed mv from percent of range [-10v, 10v]           *
***********************************************************************/
static int mv_from_percent( STRING_CLASS & sorc )
{
double   x;
int      i;

if ( sorc.contains(PeriodChar) )
    {
    x = sorc.real_value();
    x *= 100.0;
    i = (int) round( x, 1.0 );
    }
else
    {
    i = sorc.int_value();
    i *= 100;
    }

return i;
}

/***********************************************************************
*                        ALL_STEPS_ARE_PERCENT                         *
***********************************************************************/
static BOOLEAN all_steps_are_percent()
{
SureTrakSetup.rewind();

while ( SureTrakSetup.next() )
    {
    if ( !SureTrakSetup.vel_is_percent() )
        return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                              SET_PERCENT                             *
*    Add or remove a percent sign, if necessary, from a string.        *
*    Do nothing if the string is empty. Return TRUE if the string      *
*    has changed.                                                      *
***********************************************************************/
static BOOLEAN set_percent( STRING_CLASS & s, BOOLEAN need_percent )
{
TCHAR * cp;

if ( s.isempty() )
    return FALSE;

cp = s.find( PercentChar );
if ( need_percent )
    {
    if ( !cp )
        {
        s += PercentChar;
        return TRUE;
        }
    }
else
    {
    if ( cp )
        {
        *cp = NullChar;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                          CHECK_LOW_IMPACT_VALUES                     *
***********************************************************************/
BOOLEAN check_low_impact_values()
{
TCHAR      * cp;
BOOLEAN      status;
float        x;
float        xmin;
STRING_CLASS s;

status = TRUE;
SureTrakSetup.rewind();
while ( SureTrakSetup.next() )
    {
    cp = SureTrakSetup.low_impact_string();
    if ( !is_empty(cp) )
        {
        x = extfloat( cp );
        xmin = SureTrakParam.min_low_impact_percent();
        if ( x < xmin )
            {
            if ( not_float_zero(xmin - x) )
                {
                status = FALSE;
                s = ascii_float(xmin);
                s += PercentChar;
                SureTrakSetup.set_low_impact( s.text() );
                SureTrakHasChanged = TRUE;
                }
            }
        }
    }

return status;
}

/***********************************************************************
*               UPDATE_SURETRAK_PARAMS_WITH_FT2_SETTINGS               *
***********************************************************************/
static void update_suretrak_params_with_ft2_settings()
{
INI_CLASS  ini;
NAME_CLASS s;

s.get_ft2_editor_settings_file_name( CurrentPart.computer, CurrentPart.machine );
if ( !s.file_exists() )
    return;

ini.set_file( s.text() );
ini.set_section( ConfigSection );

SureTrakParam.set_min_low_impact_percent( (float) ini.get_double( MinLowImpactPercentKey) );
SureTrakParam.set_max_velocity( (float) ini.get_double( MaxVelocityKey) );
}

/***********************************************************************
*                            GET_CURRENT_PART                          *
***********************************************************************/
static void get_current_part()
{
ANALOG_SENSOR a;
NAME_CLASS    s;

s.get_ftchan_dbname( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

if ( !CurrentPart.find(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
    return;

HaveCurrentPart = TRUE;

CurrentFtAnalog.load( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
CurrentLimitSwitch.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
----------------
Parameter Limits
---------------- */
CurrentSort.load( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

CurrentParam.find(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) ;

CurrentMarks.load(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) ;

SureTrakParam.find( CurrentPart.computer, CurrentPart.machine );
if ( CurrentMachine.is_ftii )
    {
    /*
    ----------------------------------------------------------------------
    This is a ft2 machine, get the settings that I need from the ft2 files
    ---------------------------------------------------------------------- */
    update_suretrak_params_with_ft2_settings();
    }

SureTrakSetup.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
PressureControl.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
Warmup.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
SureTrakHasChanged = FALSE;
}

static short DestUnitsId = NO_UNITS;
static short SorcUnitsId = NO_UNITS;

/***********************************************************************
*                             FLOAT_ADJUST                             *
***********************************************************************/
static float float_adjust( float x )
{
return (float) adjust_for_units( DestUnitsId, x, SorcUnitsId );
}

/***********************************************************************
*                             FLOAT_ADJUST                             *
***********************************************************************/
static double float_adjust( double x )
{
return adjust_for_units( DestUnitsId, x, SorcUnitsId );
}

/***********************************************************************
*                     GET_NEW_VACUUM_SWITCH_NUMBER                     *
***********************************************************************/
static void get_new_vacuum_switch_number()
{
int i;
int box;
HWND w;

w = 0;

i = NO_WIRE;
for ( box=VAC_SWITCH_1_RADIO; box<=VAC_SWITCH_6_RADIO; box++ )
    {
    if ( is_checked(w, box) )
        {
        i = 1 + box - VAC_SWITCH_1_RADIO;
        break;
        }
    }

if ( i != SureTrakSetup.vacuum_limit_switch_number() )
    {
    SureTrakSetup.set_vacuum_limit_switch_number(i);
    SureTrakHasChanged = TRUE;
    }
}

/***********************************************************************
*                      GET_NEW_SURETRAK_STEPS                          *
***********************************************************************/
static void get_new_suretrak_steps()
{
int          box;
TCHAR        buf[MAX_FLOAT_LEN+1];
int32        i;
STRING_CLASS s;
HWND         w;

w = 0; //WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

box = ACCEL_1_EDITBOX;
SureTrakSetup.clear_steps();
for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    SureTrakSetup.append();

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_accel( buf );
    box++;

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_vel( buf );
    box++;

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_end_pos( buf );
    box++;

    SureTrakSetup.set_have_vacuum_wait( is_checked(w, box) );
    box++;

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_low_impact( buf );
    box++;

    SureTrakSetup.set_have_low_impact_from_input( is_checked(w, box) );
    box++;

    if ( SureTrakSetup.accel_is_percent() && SureTrakSetup.vel_is_percent() )
        resource_message_box( MainInstance, NO_PERCENT_PERCENT_STRING, INVALID_ENTRY_STRING, MB_OK );
    }

get_new_vacuum_switch_number();

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    CurrentLimitSwitch.desc(i).get_text( w, LS1_DESC_EB+i );
    CurrentLimitSwitch.pos(i).get_text( w, LS1_POS_EB+i );
    CurrentLimitSwitch.wire(i).get_text( w, LS1_WIRE_PB+i );
    }

SureTrakHasChanged = TRUE;
}

/***********************************************************************
*                            UPDATE_ST_PARAMS                          *
***********************************************************************/
static void update_st_params( HWND w )
{
SureTrakSetup.gain_break_velocity(0).get_text( w, BREAK_VEL_1_EBOX );
SureTrakSetup.gain_break_velocity(1).get_text( w, BREAK_VEL_2_EBOX );
SureTrakSetup.gain_break_velocity(2).get_text( w, BREAK_VEL_3_EBOX );
SureTrakSetup.gain_break_gain(0).get_text( w, BREAK_GAIN_1_EBOX );
SureTrakSetup.gain_break_gain(1).get_text( w, BREAK_GAIN_2_EBOX );
SureTrakSetup.gain_break_gain(2).get_text( w, BREAK_GAIN_3_EBOX );
SureTrakSetup.velocity_loop_gain().get_text( w, VEL_LOOP_GAIN_EBOX );
SureTrakSetup.ft_stop_pos().get_text( w, FT_STOP_POS_EBOX );

SureTrakHasChanged = TRUE;
}

/***********************************************************************
*                   RECORD_MULTIPART_RUNLIST_CHANGES                   *
***********************************************************************/
static void record_multipart_runlist_changes( HWND w )
{
int           i;
STRING_CLASS  s;

for ( i=0; i<MAX_MULTIPART_RUNLIST; i++ )
    {
    if ( s == NotUsedString )
        s = unknown_string();
    MultipartRunlist.partname[i] = s;
    }

MultipartRunlistHasChanged = TRUE;
}

/***********************************************************************
*                             GET_PERCENTS                             *
***********************************************************************/
static void get_percents()
{
BOOLEAN ramp_contains_percent;
int i;
PRES_CONTROL_STEP_ENTRY * se;

for ( i=0; i<3; i++ )
    {
    se = &PressureControl.step[i];
    if ( PressureControl.proportional_valve || se->pressure.contains(PercentChar) )
        se->is_percent = TRUE;
    else
        se->is_percent = FALSE;

    ramp_contains_percent = se->ramp.contains( PercentChar );

    if ( !se->is_goose_step && ramp_contains_percent )
        {
        se->is_goose_step = TRUE;
        }

    se->is_goose_percent = FALSE;
    if ( se->is_goose_step )
        {
        if ( se->is_percent || ramp_contains_percent )
            se->is_goose_percent = TRUE;
        }
    }
}

/***********************************************************************
*                    GET_NEW_PRESSURE_CONTROL_STEPS                    *
static void get_new_pressure_control_steps()
{
int32 i;
PRES_CONTROL_STEP_ENTRY * se;

PressureControl.sensor_number = 0;//lb.selected_index();

for ( i=0; i<3; i++ )
    {
    if ( is_checked(w,  PC_2_VOLT_RANGE_RADIO+i) )
        {
        PressureControl.voltage_output_range = i;
        break;
        }
    }

PressureControl.retctl_arm_pres.get_text( w, RETCTL_ARM_PRES_EBOX );
PressureControl.integral_gain.get_text( w, PC_INTEGRAL_GAIN_EBOX );
PressureControl.derivative_gain.get_text( w, PC_DERIVATIVE_GAIN_EBOX );
PressureControl.pressure_loop_gain.get_text( w, PC_LOOP_GAIN_EBOX );
PressureControl.retctl_pres.get_text(w, RETCTL_PRES_EBOX );
PressureControl.retract_volts.get_text(w, RETRACT_PRES_EBOX );
PressureControl.park_volts.get_text(w, PARK_PRES_EBOX );
PressureControl.is_enabled = is_checked( w, PRES_CONTROL_ENABLE_XBOX );
PressureControl.use_only_positive_voltages = is_checked( w, POSITIVE_ONLY_XBOX );
PressureControl.retctl_is_enabled = is_checked( w, RETCTL_ENABLE_XBOX );
PressureControl.proportional_valve = is_checked( w, PROPORTIONAL_VALVE_XBOX );

PressureControl.arm_intensifier_ls_number = 0;//lb.selected_index() + 1;

se = &PressureControl.step[0];
se->ramp.get_text( w, RAMP_1_EBOX );
goose_button = GetDlgItem( w, PC_GOOSE_1_XBOX );
se->is_goose_step = is_checked( goose_button );
se->pressure.get_text( w, PRES_1_EBOX );
get_percents( goose_button, se );
se->is_open_loop = is_checked( w, PC_OPEN_LOOP_1_XBOX );
se->holdtime.get_text( w, HOLD_TIME_1_EBOX  );

se = &PressureControl.step[1];
se->ramp.get_text( w, RAMP_2_EBOX );
goose_button = GetDlgItem( w, PC_GOOSE_2_XBOX );
se->is_goose_step = is_checked( goose_button );
se->pressure.get_text( w, PRES_2_EBOX );
get_percents( goose_button, se );
se->is_open_loop = is_checked( w, PC_OPEN_LOOP_2_XBOX );
se->holdtime.get_text( w, HOLD_TIME_2_EBOX  );

se = &PressureControl.step[2];
se->ramp.get_text(     w, RAMP_3_EBOX );
goose_button = GetDlgItem( w, PC_GOOSE_3_XBOX );
se->is_goose_step = is_checked( goose_button );
se->pressure.get_text( w, PRES_3_EBOX );
get_percents( goose_button, se );
se->is_open_loop = is_checked( w, PC_OPEN_LOOP_3_XBOX );
se->holdtime.get_text( w, HOLD_TIME_3_EBOX  );

PressureControl.nof_steps = 0;
for ( i=0; i<MAX_PRESSURE_CONTROL_STEPS; i++ )
    {
    if ( !PressureControl.step[i].pressure.isempty() )
        PressureControl.nof_steps = i+1;
    }
}
***********************************************************************/

/***********************************************************************
*                            ADD_NEW_MARKS                             *
***********************************************************************/
static void add_new_marks()
{
short i;

for ( i=0; i<NofAddMarks; i++ )
    {
        CurrentMarks.add( AddMarksList[i].parameter_type, AddMarksList[i].parameter_number );
    }
}

/***********************************************************************
*                         REPLACE_DISTANCE_VALUE                       *
***********************************************************************/
static STRING_CLASS & replace_distance_value( TCHAR * sorc, float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
i = round( x, 1.0 );

return replace_value( sorc, i );
}

/***********************************************************************
*                       REPLACE_X1_DISTANCE_VALUE                      *
***********************************************************************/
static STRING_CLASS & replace_x1_distance_value( TCHAR * sorc, float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
x /= 4.0;
i = round( x, 1.0 );

return replace_value( sorc, i );
}

/***********************************************************************
*                         REPLACE_VELOCITY_VALUE                       *
***********************************************************************/
static STRING_CLASS & replace_velocity_value( TCHAR * sorc, float new_value, BOOLEAN need_bit_30 )
{
static TCHAR setbit30[] = TEXT( "|H40000000" );
const int SLEN = sizeof(setbit30)/sizeof(TCHAR);

TCHAR s[MAX_INTEGER_LEN + SLEN + 1];
TCHAR * cp;
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_velocity( x );
x /= 4.0;
i = round( x, 1.0 );

cp = copystring( s, int32toasc((int32) i) );

if ( need_bit_30 )
    copystring( cp, setbit30 );

return replace_value( sorc, s );
}

/***********************************************************************
*                         REPLACE_VELOCITY_VALUE                       *
***********************************************************************/
static STRING_CLASS & replace_velocity_value( TCHAR * sorc, float new_value )
{
return replace_velocity_value( sorc, new_value, (BOOLEAN) FALSE );
}

/***********************************************************************
*                         X4_FROM_DISTANCE_VALUE                       *
***********************************************************************/
static unsigned x4_from_distance_value( double new_value )
{
int    i;

new_value = CurrentPart.x4_from_dist( new_value );
i = (int) round( new_value, 1.0 );
return (unsigned) i;
}

/***********************************************************************
*                         X4_FROM_DISTANCE_VALUE                       *
***********************************************************************/
static unsigned x4_from_distance_value( float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
i = (int) round( x, 1.0 );
return (unsigned) i;
}

/***********************************************************************
*                         X1_FROM_DISTANCE_VALUE                       *
***********************************************************************/
static unsigned x1_from_distance_value( float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
x /= 4.0;
i = (int) round( x, 1.0 );
return (unsigned) i;
}

/***********************************************************************
*                              X1_VELOCITY                             *
***********************************************************************/
static unsigned x1_velocity( double x )
{
int    i;

x = CurrentPart.x4_from_velocity( x );
x /= 4.0;
i = round( x, 1.0 );

return (unsigned) i;
}

/***********************************************************************
*                              X1_VELOCITY                             *
***********************************************************************/
static unsigned x1_velocity( float new_value )
{
return x1_velocity( (double) new_value );
}

/***********************************************************************
*                              FTII_PERCENT                            *
***********************************************************************/
static unsigned ftii_percent( double d )
{
unsigned u;

d *= 100.0;
u = (unsigned) round( d, 1.0 );
u |= Bit31;

return u;
}

/***********************************************************************
*                              FTII_PERCENT                            *
***********************************************************************/
static unsigned ftii_percent( float new_value )
{
return ftii_percent( (double) new_value );
}

/***********************************************************************
*                    UPDATE_SURETRAK_STEP_VALUES                       *
***********************************************************************/
static void update_suretrak_step_value( FTII_VAR_ARRAY & va, int vi )
{
int      step_number;
int      vn;
bool     have_low_impact;
double   d;
unsigned u;

SureTrakSetup.rewind();

vn              = ST_ACCEL_1_VN;
have_low_impact = false;

step_number = 0;
while ( true )
    {
    if ( step_number >= MAX_FTII_CONTROL_STEPS )
        break;

    if ( SureTrakSetup.next() )
        {
        if ( !have_low_impact )
            {
            if ( not_float_zero(SureTrakSetup.low_impact_value()) )
                have_low_impact = true;
            }

        /*
        -----
        Accel
        ----- */
        d = (double) SureTrakSetup.accel_value();
        if ( SureTrakSetup.accel_is_percent() )
            u = ftii_percent( d );
        else
            u = x1_from_distance_value( d );

        va[vi].set( vn, u );
        vi++;
        vn++;

        /*
        --------
        Velocity
        -------- */
        d = (double) SureTrakSetup.vel_value();
        if ( SureTrakSetup.vel_is_percent() )
            u = ftii_percent( d );
        else
            u = x1_velocity( d );

        if ( SureTrakSetup.have_vacuum_wait() )
            u |= Bit30;

        va[vi].set( vn, u );
        vi++;
        vn++;

        /*
        ------------
        End Position
        ------------ */
        u = x4_from_distance_value( SureTrakSetup.end_pos_value() );

        va[vi].set( vn, u );
        vi++;
        vn++;

        /*
        -------------
        Input to test
        ------------- */
        u = 0;
        if ( SureTrakSetup.have_vacuum_wait() )
            u = (unsigned) FTII_VACUUM_WAIT_INPUT;
        else if ( SureTrakSetup.have_low_impact_from_input() )
            u = (unsigned) FTII_LOW_IMPACT_INPUT;

        va[vi].set( vn, u );
        vi++;
        vn++;

        if ( have_low_impact )
            {
            va[vi++].set( vn++, (unsigned) 0 );
            d = (double) SureTrakSetup.low_impact_value();
            u = ftii_percent( d );
            va[vi++].set( vn++, (unsigned) u );
            va[vi++].set( vn++, (unsigned) 0 );
            va[vi++].set( vn++, (unsigned) 0 );
            step_number++;
            }
        }
    else
        {
        va[vi++].set( vn++, (unsigned) 0 );
        va[vi++].set( vn++, (unsigned) 0x7FFF );
        va[vi++].set( vn++, (unsigned) 0 );
        va[vi++].set( vn++, (unsigned) 0 );
        }
    step_number++;
    }
}

/***********************************************************************
*                     PRESSURE_TO_VOLTS_SETPOINT                       *
*             Convert a pressure setpoint to volts * 1000              *
* The setpoint can be a 16 bit positive or negative number which       *
* is then zero extended to 32 bits.                                    *
***********************************************************************/
unsigned pressure_to_volts_setpoint( STRING_CLASS & sorc )
{
ANALOG_SENSOR a;
double max_pressure;
double min_pressure;
double max_volts;
double min_volts;
double v;
unsigned short us;

a.find( PressureControl.analog_sensor.desc.text() );
max_pressure = a.high_value();
min_pressure = - max_pressure;
if ( max_pressure <= min_pressure )
    max_pressure = min_pressure + 1.0;
max_volts = a.max_volts();
/*
---------------------------------------------------------------------------------------
The voltage output range does not apply here, I need the actual voltage range,
which I dont have yet. For now (i.e., for Diecast Press), assume the voltage range is
5 volts, as in 0-5 volts = 0-10000 psi).

if ( PressureControl.voltage_output_range == 0 )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == 2 )
    max_volts = 10.0;
-------------------------------------------------------------------------------------- */
min_volts = -max_volts;

v = sorc.real_value();
if ( v < min_pressure )
    v = min_pressure;
else if ( v > max_pressure )
    v = max_pressure;
v -= min_pressure;
v *= (max_volts - min_volts);
v *= 1000.0;
v /= (max_pressure - min_pressure);
v += min_volts*1000.0;

us = (unsigned short) v;

return (unsigned) us;
}
/***********************************************************************
*                     PRESSURE_TO_VOLTS_16BIT                          *
*             Convert a pressure setpoint to volts * 1000              *
* The setpoint can be a 16 bit positive or negative number which       *
* is then zero extended to 32 bits.                                    *
***********************************************************************/
unsigned pressure_to_volts_16bit( STRING_CLASS & sorc, double max_volts )
{
ANALOG_SENSOR a;
double max_pressure;
double min_pressure;
double min_volts;
double v;
union { unsigned u; unsigned short us; };
int nof_sensor_points;
int index;

u = 0;

if ( sorc.contains(PercentChar) )
    {
    max_pressure = 100.0;
    min_pressure = -100.0;
    }
else
    {   
    nof_sensor_points = PressureControl.analog_sensor.nof_sensor_points;
    max_pressure = PressureControl.analog_sensor.values[nof_sensor_points];
    min_pressure = - max_pressure;
    if ( max_pressure <= min_pressure )
        max_pressure = min_pressure + 1.0;
    }

min_volts = -max_volts;

v = sorc.real_value();
if ( v < min_pressure )
    v = min_pressure;
else if ( v > max_pressure )
    v = max_pressure;
v -= min_pressure;
v *= (max_volts - min_volts);
v *= 1000.0;
v /= (max_pressure - min_pressure);
v += min_volts*1000.0;

us = (unsigned short) v;

return u;
}

/***********************************************************************
*                     PRESSURE_TO_VOLTS_16BIT                          *
*             Convert a pressure setpoint to volts * 1000              *
* The setpoint can be a 16 bit positive or negative number which       *
* is then zero extended to 32 bits.                                    *
***********************************************************************/
unsigned pressure_to_volts_16bit( STRING_CLASS & sorc )
{
double max_volts;

/*
-------------------------------------------------------------------------------------
The voltage output range of the sensor does not apply here, I need the actual voltage
range, which is set by the radio buttons under the sensor drop down list box.
------------------------------------------------------------------------------------- */
max_volts = 5.0;

if ( PressureControl.voltage_output_range == PC_2_VOLT_RANGE )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == PC_10_VOLT_RANGE )
    max_volts = 10.0;

return pressure_to_volts_16bit( sorc, max_volts );
}

/***********************************************************************
*                      PERCENT_TO_UNSIGNED_VOLTS                       *
* This converts a string percent to an unsigned volts*1000.            *
* All 32 bits are used, so for +/- 10 volts -.01% = -.001 volts        *
* = -1 = 0xFFFFFFFF.                                                   *
***********************************************************************/
static unsigned percent_to_unsigned_volts( STRING_CLASS & sorc, double max_volts )
{
double d;

d = sorc.real_value();
d *= 100.0;
if ( (10.0 - max_volts) > 1.0 )
    {
    d *= max_volts;
    d /= 10.0;
    }

return (unsigned) round( d, 1.0 );
}

/***********************************************************************
*                      PERCENT_TO_UNSIGNED_VOLTS                       *
*            This uses the selected voltage output range.              *
***********************************************************************/
static unsigned percent_to_unsigned_volts( STRING_CLASS & sorc )
{
double max_volts;

max_volts = 5.0;

if ( PressureControl.voltage_output_range == PC_2_VOLT_RANGE )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == PC_10_VOLT_RANGE )
    max_volts = 10.0;

return percent_to_unsigned_volts( sorc, max_volts );
}

/***********************************************************************
*                           PERCENT_TO_VOLTS                           *
* Converts percent to volts using the current voltage output range     *
***********************************************************************/
double percent_to_volts( STRING_CLASS & sorc )
{
const double max_percent   =  100.0;
const double percent_range =  200.0;
double max_volts;
double volt_range;
double v;

/*
-------------------------------------------------------------------------------------
The voltage output range of the sensor does not apply here, I need the actual voltage
range, which is set by the radio buttons under the sensor drop down list box.
------------------------------------------------------------------------------------- */
max_volts = 5.0;

if ( PressureControl.voltage_output_range == PC_2_VOLT_RANGE )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == PC_10_VOLT_RANGE )
    max_volts = 10.0;

volt_range = 2 * max_volts;

v = sorc.real_value();
v += max_percent;
v *= volt_range;
v /= percent_range;
v -= max_volts;

return v;
}

/***********************************************************************
*                     UPDATE_FTII_CONTROL_PROGRAM                      *
* The limit_switch_mask is an array of 6 masks for the limit switches  *
* if this is NOT a suretrak2.                                          *
***********************************************************************/
static void update_ftii_control_program()
{
static int TSL_INDEX               = 0;
static int MSL_INDEX               = 1;
static int ESV_INDEX               = 2;
static int LS1_INDEX               = 3;
static int LS2_INDEX               = 4;
static int LS3_INDEX               = 5;
static int LS4_INDEX               = 6;
static int LS5_INDEX               = 7;
static int LS6_INDEX               = 8;
static int MY_VEL_LOOP_GAIN_INDEX  = 9;
static int MY_FT_STOP_POS_INDEX    = 10;
static int BREAK_VEL_1_INDEX       = 11;
static int BREAK_VEL_2_INDEX       = 12;
static int BREAK_VEL_3_INDEX       = 13;
static int BREAK_GAIN_1_INDEX      = 14;
static int BREAK_GAIN_2_INDEX      = 15;
static int BREAK_GAIN_3_INDEX      = 16;
static int TIME_INTERVAL_MS_INDEX  = 17;
static int NOF_TIME_SAMPLES_INDEX  = 18;
static int BISCUIT_DELAY_MS_INDEX  = 19;
static int PC_CONFIG_WORD_INDEX    = 20;
static int PC_PRES_LOOP_GAIN_INDEX = 21;
static int PC_RETRACT_VOLTS_INDEX    = 22;
static int PC_PARK_VOLTS_INDEX       = 23;
static int PC_RAMP_1_INDEX           = 24;
static int PC_PRES_1_INDEX           = 25;
static int PC_HOLDTIME_1_INDEX       = 26;
static int PC_RAMP_2_INDEX           = 27;
static int PC_PRES_2_INDEX           = 28;
static int PC_HOLDTIME_2_INDEX       = 29;
static int PC_RAMP_3_INDEX           = 30;
static int PC_PRES_3_INDEX           = 31;
static int PC_HOLDTIME_3_INDEX       = 32;
static int PC_INTEGRAL_GAIN_INDEX    = 33;
static int PC_DERIVATIVE_GAIN_INDEX  = 34;
static int PC_RETCTL_DAC_INDEX       = 35; /* Retctl must be last */
static int PC_RETCTL_ARM_PRES_INDEX  = 36;
static int PC_RETCTL_PRES_INDEX      = 37;

static int vars[] = { TSL_VN, MSL_VN, ESV_VN,
 LS1_VN, LS2_VN, LS3_VN, LS4_VN, LS5_VN, LS6_VN,
 VEL_LOOP_GAIN_VN,
 FT_STOP_POS_VN,
 BREAK_VEL_1_VN,  BREAK_VEL_2_VN,  BREAK_VEL_3_VN,
 BREAK_GAIN_1_VN, BREAK_GAIN_2_VN, BREAK_GAIN_3_VN,
 TIME_INTERVAL_MS_VN, NOF_TIME_SAMPLES_VN,
 BISCUIT_DELAY_MS_VN,
 PC_CONFIG_WORD_VN,
 PC_PRES_LOOP_GAIN_VN,
 PC_RETRACT_VOLTS_VN,
 PC_PARK_VOLTS_VN,
 PC_RAMP_1_VN,
 PC_PRES_1_VN,
 PC_HOLDTIME_1_VN,
 PC_RAMP_2_VN,
 PC_PRES_2_VN,
 PC_HOLDTIME_2_VN,
 PC_RAMP_3_VN,
 PC_PRES_3_VN,
 PC_HOLDTIME_3_VN,
 PC_INTEGRAL_GAIN_VN,
 PC_DERIVATIVE_GAIN_VN,
 PC_RETCTL_DAC_VN,
 PC_RETCTL_ARM_PRES_VN,
 PC_RETCTL_PRES_VN
 };

const int nof_vars = sizeof(vars)/sizeof(int);

static int normal_gain_vars[]  = { BREAK_GAIN_1_VN, BREAK_GAIN_2_VN,  BREAK_GAIN_3_VN    };
static int cyclone_gain_vars[] = { BREAK_GAIN_1_VN, INTEGRAL_GAIN_VN, DERIVATIVE_GAIN_VN };
const unsigned USE_ONLY_POSITIVE_VOLTAGES_MASK = 0x80;
BOOLEAN        all_percent;
BOOLEAN        is_cyclone;
int            vi;
int            i;
int            n;
unsigned       u;
unsigned       v;
double         x;
NAME_CLASS     s;
FTII_VAR_ARRAY va;

is_cyclone = CurrentMachine.is_cyclone;
n = nof_vars;
i = n;
if ( CurrentMachine.suretrak_controlled )
    i += ST_LAST_STEP_VN + 1;
va.upsize( i );
va[TSL_INDEX].set( TSL_VN, x4_from_distance_value(CurrentPart.total_stroke_length) );
va[MSL_INDEX].set( MSL_VN, x4_from_distance_value(CurrentPart.min_stroke_length) );
va[ESV_INDEX].set( ESV_VN, x1_velocity(CurrentPart.eos_velocity) );
vi = LS1_INDEX;
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    x = CurrentLimitSwitch.real_pos(i);
    u  = 0x7FFF;
    if ( not_zero(x) )
        u = x4_from_distance_value( x );
    va[vi].set( vars[vi], u );
    vi++;
    }

/*
------------------
Velocity Loop Gain
------------------ */
x = SureTrakSetup.velocity_loop_gain_value();
x = round( x, 1.0 );
u = (unsigned) x;
va[MY_VEL_LOOP_GAIN_INDEX].set( VEL_LOOP_GAIN_VN, u );

/*
----------------------------
Follow Through Stop Position
---------------------------- */
x = SureTrakSetup.ft_stop_pos_value();
u  = 0x7FFF;
if ( not_zero(x) )
    u = x4_from_distance_value( x );
va[MY_FT_STOP_POS_INDEX].set( FT_STOP_POS_VN, u );

va[BREAK_VEL_1_INDEX].set( BREAK_VEL_1_VN, x1_velocity(SureTrakSetup.gain_break_velocity_value(0)) );
if ( is_cyclone )
    {
    va[BREAK_VEL_2_INDEX].set( BREAK_VEL_2_VN, unsigned(6000) );
    va[BREAK_VEL_3_INDEX].set( BREAK_VEL_3_VN, unsigned(6000) );
    }
else
    {
    va[BREAK_VEL_2_INDEX].set( BREAK_VEL_2_VN, x1_velocity(SureTrakSetup.gain_break_velocity_value(1)) );
    va[BREAK_VEL_3_INDEX].set( BREAK_VEL_3_VN, x1_velocity(SureTrakSetup.gain_break_velocity_value(2)) );
    }

vi = BREAK_GAIN_1_INDEX;
for (i=0; i<3; i++ )
    {
    if ( is_cyclone )
        {
        vars[vi] = cyclone_gain_vars[i];
        }
    else
        {
        vars[vi] = normal_gain_vars[i];
        }

    x = SureTrakSetup.gain_break_gain_value(i);
    if ( i > 0 && is_cyclone )
        {
        if ( i==1 && is_zero(x) )
            u = 1;
        else
            u = (unsigned) round( x, 1.0 );
        }
    else
        {
        x *= 1000.0;
        u = (unsigned) round( x, 1.0 );
        }
    va[vi].set( vars[vi], u );
    vi++;
    }

va[TIME_INTERVAL_MS_INDEX].set( TIME_INTERVAL_MS_VN, (unsigned) CurrentPart.ms_per_time_sample );
va[NOF_TIME_SAMPLES_INDEX].set( NOF_TIME_SAMPLES_VN, (unsigned) CurrentPart.nof_time_samples   );

/*
-----------------------------------------------------------------------------------
See if this machine uses biscuit time delay or absolute time to measure the biscuit
if absolute then zero the time delay as I am going to calculate the biscuit size.
----------------------------------------------------------------------------------- */
if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT )
    u = 0;
else
    u = (unsigned) CurrentPart.biscuit_time_delay*1000;
va[BISCUIT_DELAY_MS_INDEX].set( BISCUIT_DELAY_MS_VN, u );

/*
----------------
Pressure Control
---------------- */
if ( HavePressureControl )
    {
    u = 0;
    all_percent = PressureControl.proportional_valve;
    if ( PressureControl.is_enabled )
        {
        u = 1;
        v = PressureControl.voltage_output_range;
        v <<= 4;
        u |= v;
        if ( PressureControl.use_only_positive_voltages )
            u |= USE_ONLY_POSITIVE_VOLTAGES_MASK;
        /*
        --------------------------------------------------------
        The ls that arms the pressure control goes in bits 1,2,3
        Range = [LS1 = 0 to LS6 = 5]
        -------------------------------------------------------- */
        v = PressureControl.arm_intensifier_ls_number;
        v--;
        if ( v >= MAX_FTII_LIMIT_SWITCHES )
            v = 0;
        else
            v <<= 1;
        u |= v;
        }

    va[PC_CONFIG_WORD_INDEX].set(    PC_CONFIG_WORD_VN,    u );
    va[PC_PRES_LOOP_GAIN_INDEX].set( PC_PRES_LOOP_GAIN_VN, PressureControl.pressure_loop_gain.uint_value() );
    va[PC_INTEGRAL_GAIN_INDEX].set( PC_INTEGRAL_GAIN_VN, PressureControl.integral_gain.uint_value() );
    va[PC_DERIVATIVE_GAIN_INDEX].set( PC_DERIVATIVE_GAIN_VN, PressureControl.derivative_gain.uint_value() );
    va[PC_RETRACT_VOLTS_INDEX].set( PC_RETRACT_VOLTS_VN, unsigned(0) );

    if ( PressureControl.proportional_valve )
        {
        x = percent_to_volts( PressureControl.retract_volts );
        }
    else
        {
        x = PressureControl.retract_volts.real_value();
        if ( x < -10.0 || x > 10.0 )
            x /= 1000.0;
        }
    va[PC_RETRACT_VOLTS_INDEX] = x;

    va[PC_PARK_VOLTS_INDEX].set( PC_PARK_VOLTS_VN, unsigned(0) );
    if ( PressureControl.proportional_valve )
        {
        x = percent_to_volts( PressureControl.park_volts );
        }
    else
        {
        x = PressureControl.park_volts.real_value();
        if ( x < -10.0 || x > 10.0 )
            x /= 1000.0;
        }
    va[PC_PARK_VOLTS_INDEX] = x;

    vi = PC_RAMP_1_INDEX;
    for ( i=0; i<PressureControl.nof_steps; i++ )
        {
        if ( PressureControl.step[i].is_goose_step )
            {
            s = PressureControl.step[i].ramp;
            if ( PressureControl.step[i].is_goose_percent || all_percent )
                set_percent( s, TRUE );
            u = pressure_to_volts_16bit( s );
            u |= Bit31;
            if ( PressureControl.step[i].is_goose_percent || all_percent )
                u |= Bit30;
            }
        else
            {
            u = PressureControl.step[i].ramp.uint_value();
            }
        va[vi].set( vars[vi], u );
        vi++;

        s = PressureControl.step[i].pressure;
        if ( PressureControl.step[i].is_percent || all_percent )
            set_percent( s, TRUE );
        u = pressure_to_volts_16bit( s );
        if ( PressureControl.step[i].is_open_loop )
            u |= Bit31;
        if ( PressureControl.step[i].is_percent || all_percent )
            u |= Bit30;
        va[vi].set( vars[vi], u );
        vi++;

        u = PressureControl.step[i].holdtime.uint_value();
        va[vi].set( vars[vi], u );
        vi++;
        }

    while( i<MAX_PRESSURE_CONTROL_STEPS )
        {
        u = 0;
        va[vi].set( vars[vi], u );
        vi++;

        u = 0x7FFF;
        va[vi].set( vars[vi], u );
        vi++;

        u = 0;
        va[vi].set( vars[vi], u );
        vi++;
        i++;
        }
    }
else
    {
    u = 0;
    va[PC_CONFIG_WORD_INDEX].set( PC_CONFIG_WORD_VN, u );
    }

/*
---------------
Retract Control
--------------- */
va[PC_RETCTL_DAC_INDEX].set( PC_RETCTL_DAC_VN, unsigned(4) );  /* DAC channel to be used for retract control */
va[PC_RETCTL_ARM_PRES_INDEX].set( PC_RETCTL_ARM_PRES_VN, unsigned(0) );

/*
---------------------------------------------------------
Setting the pressure to zero disables the retract control
--------------------------------------------------------- */
va[PC_RETCTL_PRES_INDEX].set( PC_RETCTL_PRES_VN, unsigned(0) );

if ( PressureControl.retctl_is_enabled )
    {
    /*
    ------------------------------------------------------------------------------
    The Retract control arm pressure is stored as a voltage * 1000 but the type is
    just a number so I have to convert to an unsigned value myself.
    ------------------------------------------------------------------------------ */
    va[PC_RETCTL_ARM_PRES_INDEX] = percent_to_unsigned_volts( PressureControl.retctl_arm_pres, 10.0 );

    /*
    -------------------------------------------------------------------------------------------------
    At the present time the only pressure control is for a proportional valve, which will always be %
    ------------------------------------------------------------------------------------------------- */
    s = PressureControl.retctl_pres;
    set_percent( s, TRUE );
    u = pressure_to_volts_16bit( s, 10.0 );
    u |= Bit31;
    va[PC_RETCTL_PRES_INDEX] = u;
    }

/*
----------------------------------------------------------------------------
I only allocated memory for the suretrak steps if this machine is controlled
---------------------------------------------------------------------------- */
if ( CurrentMachine.suretrak_controlled )
    update_suretrak_step_value( va, n );
s.get_ft2_part_settings_file_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
va.put( s );
writelog( TEXT("Write FT2 Part Settings"), s.text() );
}

/***********************************************************************
*                               SAVE_CHANGES                           *
***********************************************************************/
static void save_changes()
{
AUTO_BACKUP_CLASS ab;
int               i;

CurrentPart.save();
CurrentParam.save();
CurrentMarks.save();
CurrentFtAnalog.save(CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
CurrentLimitSwitch.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
if ( CurrentMachine.suretrak_controlled )
    SureTrakSetup.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

if ( HaveMultipartRunlist )
    MultipartRunlist.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
else
    Warmup.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

if ( HavePressureControl )
    {
    PressureControl.nof_steps = 0;
    for ( i=0; i<MAX_PRESSURE_CONTROL_STEPS; i++ )
        {
        if ( !PressureControl.step[i].pressure.isempty() )
            PressureControl.nof_steps = i+1;
        }
    PressureControl.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    }

update_ftii_control_program();

send_new_monitor_setup_event();

ab.get_backup_dir();
ab.backup_part( CurrentPart.machine, CurrentPart.name );
}

/***********************************************************************
*                      SAVE_ONE_PART_BASED_GLOBAL_LINE                 *
***********************************************************************/
static void save_one_part_based_global_line( SETPOINT_ENTRY & sorc )
{
FTII_VAR_ARRAY globals;
NAME_CLASS     s;
int            vn;

if ( sorc.desc == TEXT("FT VALVE SETTING") )
    vn = FT_SETTING_VN;
else if ( sorc.desc == TEXT("JOG SHOT VALVE SETTING") )
    vn = JOG_SHOT_SETTING_VN;
else
    return;


s.get_ft2_global_settings_file_name( CurrentPart.computer, CurrentPart.machine );
globals.get( s );

if ( globals.find(vn) )
    {
    globals = mv_from_percent( sorc.value );
    globals.put( s );
    }
}

/***********************************************************************
*                            SAVE_ONE_LINE                             *
***********************************************************************/
static void save_one_line( SETPOINT_ENTRY & sorc )
{
BOOLEAN  b;
TCHAR  * cp;
int      i;
int      n;
unsigned u;
double   y;

cp = sorc.desc.text();
if ( strings_are_equal(cp, TEXT("Param "), 6) )
    {
    /*
    ----------------------------------------------------------------------------------
    The parameter number is in the description. Extract and convert to parameter index
    ---------------------------------------------------------------------------------- */
    cp += 6;
    n = asctoint32(cp);
    n--;
    /*
    -------------------------------------------------------------------------------
    Move the pointer past the parameter number to the start rest of the description
    ------------------------------------------------------------------------------- */
    cp += 3;

    /*
    ----------------------------------------------------------------------------
    Match the rest of the description so I know what to change on this parameter
    ---------------------------------------------------------------------------- */
    for ( i=0; i<NofParamDescriptions; i++ )
        {
        if ( strings_are_equal(cp, ParamDescList[i].desc) )
            {
            switch( ParamDescList[i].index )
                {
                case SP_PARAM_DESC_INDEX:
                    copystring( CurrentParam.parameter[n].name, sorc.value.text(), PARAMETER_NAME_LEN );
                    break;
                case SP_PARAM_INPUT_INDEX:
                    CurrentParam.parameter[n].input.type = sorc.int_value();
                    break;
                case SP_PARAM_INPUT_NUMBER_INDEX:
                    CurrentParam.parameter[n].input.number = sorc.int_value();
                    break;
                case SP_PARAM_VAR_TYPE_INDEX:
                    CurrentParam.parameter[n].vartype = sorc.uint_value();
                    break;
                case SP_PARAM_UNITS_INDEX:
                    CurrentParam.parameter[n].units = sorc.int_value();
                    break;
                case SP_PARAM_LOW_WARN_LIMIT_INDEX:
                    CurrentParam.parameter[n].limits[WARNING_MIN].value = (float) sorc.real_value();
                    break;
                case SP_PARAM_LOW_WARN_WIRE_INDEX:
                    CurrentParam.parameter[n].limits[WARNING_MIN].wire_number = sorc.int_value();
                    break;
                case SP_PARAM_HIGH_WARN_LIMIT_INDEX:
                    CurrentParam.parameter[n].limits[WARNING_MAX].value = (float) sorc.real_value();
                    break;
                case SP_PARAM_HIGH_WARN_WIRE_INDEX:
                    CurrentParam.parameter[n].limits[WARNING_MAX].wire_number = sorc.int_value();
                    break;
                case SP_PARAM_LOW_ALARM_LIMIT_INDEX:
                    CurrentParam.parameter[n].limits[ALARM_MIN].value = (float) sorc.real_value();
                    break;
                case SP_PARAM_LOW_ALARM_WIRE_INDEX:
                    CurrentParam.parameter[n].limits[ALARM_MIN].wire_number = sorc.int_value();
                    break;
                case SP_PARAM_HIGH_ALARM_LIMIT_INDEX:
                    CurrentParam.parameter[n].limits[ALARM_MAX].value = (float) sorc.real_value();
                    break;
                case SP_PARAM_HIGH_ALARM_WIRE_INDEX:
                    CurrentParam.parameter[n].limits[ALARM_MAX].wire_number = sorc.int_value();
                    break;
                case SP_PARAM_ANALOG_CHAN_INDEX:
                    CurrentFtAnalog.array[n].channel = sorc.int_value();
                    break;
                case SP_PARAM_RESULT_TYPE_INDEX:
                    CurrentFtAnalog.array[n].result_type = sorc.uint_value();
                    break;
                case SP_PARAM_IVAR_TYPE_INDEX:
                    CurrentFtAnalog.array[n].ind_var_type = sorc.uint_value();
                    break;
                case SP_PARAM_IVAR_VALUE_INDEX:
                    CurrentFtAnalog.array[n].ind_value = sorc.value;
                    break;
                case SP_PARAM_END_VALUE_INDEX:
                    CurrentFtAnalog.array[n].end_value = sorc.value;
                    break;
                }
            return;
            }
        }
    }
else if ( strings_are_equal( sorc.desc.text(), TEXT("Mark "), 5) )
    {
    /*
    -----------------------------------------------------------------------------
    The parameter number is in the description. Extract and convert to mark index
    ----------------------------------------------------------------------------- */
    cp += 5;
    n = asctoint32(cp);
    n--;
    /*
    --------------------------------------------------------------------------
    Move the pointer past the mark number to the start rest of the description
    -------------------------------------------------------------------------- */
    cp += 3;

    /*
    -----------------------------------------------------------------------
    Match the rest of the description so I know what to change on this mark
    ----------------------------------------------------------------------- */
    for ( i=0; i<NofMarkDescriptions; i++ )
        {
        if ( strings_are_equal(cp, MarkDescList[i].desc) )
            {
            switch( MarkDescList[i].index )
                {
                case SP_MARK_TYPE_INDEX:
                    break;
                case SP_MARK_NUMBER_INDEX:
                    break;
                case SP_MARK_SYMBOL_INDEX:
                    break;
                case SP_MARK_COLOR_INDEX:
                    break;
                case SP_MARK_LABEL_INDEX:
                    break;
                case SP_MARK_ENABLE_INDEX:
                    break;
                }
            return;
            }
        }
    }
else
    {
    for ( i=0; i<NofDescriptions; i++ )
        {
        if ( strings_are_equal(cp, DescList[i].desc) )
            {
            switch( DescList[i].index )
                {
                case SP_DISTANCE_UNITS_INDEX:
                    CurrentPart.set_distance_units( sorc.value );
                    break;
                case SP_VELOCITY_UNITS_INDEX:
                    CurrentPart.set_velocity_units( sorc.value );
                    break;
                case SP_TSL_INDEX:
                    CurrentPart.set_total_stroke_length( sorc.value );
                    break;
                case SP_MSL_INDEX:
                    CurrentPart.set_min_stroke_length( sorc.value );
                    break;
                case SP_EOS_VEL_INDEX:
                    CurrentPart.set_eos_velocity( sorc.value );
                    break;
                case SP_PLUNGER_DIAMETER_INDEX:
                    CurrentPart.set_plunger_diameter( sorc.value );
                    break;
                case SP_SLEEVE_FILL_INDEX:
                    CurrentPart.set_sleeve_fill_distance( sorc.value );
                    break;
                case SP_RUNNER_FILL_INDEX:
                    CurrentPart.set_runner_fill_distance( sorc.value );
                    break;
                case SP_MIN_CSFS_POS_INDEX:
                    CurrentPart.set_csfs_min_position( sorc.value );
                    break;
                case SP_MIN_CSFS_VEL_INDEX:
                    CurrentPart.set_csfs_min_velocity( sorc.value );
                    break;
                case SP_RISE_CSFS_VEL_INDEX:
                    CurrentPart.set_csfs_rise_velocity( sorc.value );
                    break;
                case SP_TIME_FOR_INTENS_PRES_INDEX:
                    CurrentPart.sp[PARTSET_TIME_FOR_INTENS_PRES_INDEX].value = sorc.value;
                    CurrentPart.set_time_for_intens_pres( sorc.value );
                    break;
                case SP_PRES_FOR_RESP_TIME_INDEX:
                    CurrentPart.sp[PARTSET_PRES_FOR_RESP_TIME_INDEX].value = sorc.value;
                    CurrentPart.set_pres_for_response_time( sorc.value );
                    break;
                case SP_TIME_DELAY_BISCUIT_INDEX:
                    y = sorc.real_value();
                    if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT )
                        y *= 1000.0;
                    i = round( y, 1.0 );
                    CurrentPart.set_biscuit_time_delay( i );
                    break;
                case SP_USER_VEL_1_INDEX:
                    CurrentPart.set_user_velocity_position( 0, sorc.value );
                    break;
                case SP_USER_VEL_2_INDEX:
                    CurrentPart.set_user_velocity_position( 1, sorc.value );
                    break;
                case SP_USER_VEL_3_INDEX:
                    CurrentPart.set_user_velocity_position( 2, sorc.value );
                    break;
                case SP_USER_VEL_4_INDEX:
                    CurrentPart.set_user_velocity_position( 3, sorc.value );
                    break;
                case SP_USER_AVG_START_1_INDEX:
                    CurrentPart.set_user_avg_velocity_start( 0, sorc.value );
                    break;
                case SP_USER_AVG_START_2_INDEX:
                    CurrentPart.set_user_avg_velocity_start( 1, sorc.value );
                    break;
                case SP_USER_AVG_START_3_INDEX:
                    CurrentPart.set_user_avg_velocity_start( 2, sorc.value );
                    break;
                case SP_USER_AVG_START_4_INDEX:
                    CurrentPart.set_user_avg_velocity_start( 3, sorc.value );
                    break;
                case SP_USER_AVG_END_1_INDEX:
                    CurrentPart.set_user_avg_velocity_end( 0, sorc.value );
                    break;
                case SP_USER_AVG_END_2_INDEX:
                    CurrentPart.set_user_avg_velocity_end( 1, sorc.value );
                    break;
                case SP_USER_AVG_END_3_INDEX:
                    CurrentPart.set_user_avg_velocity_end( 2, sorc.value );
                    break;
                case SP_USER_AVG_END_4_INDEX:
                    CurrentPart.set_user_avg_velocity_end( 3, sorc.value );
                    break;
                case SP_GOOD_SHOT_WIRE_INDEX:
                    u = sorc.uint_value();
                    if ( u > HIGH_FT2_OUTPUT_WIRE )
                        u = NO_WIRE;
                    CurrentPart.good_shot_wire = u;
                    break;
                case SP_GOOD_SHOT_USE_WARN_INDEX:
                      CurrentPart.good_shot_requires_no_warnings = sorc.boolean_value();
                    break;
                case SP_ALARM_TIMEOUT_INDEX:
                    break;
                case SP_CHANNEL_01_SENSOR_INDEX:                   
                    CurrentPart.analog_sensor[0] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_02_SENSOR_INDEX:                 
                    CurrentPart.analog_sensor[1] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_03_SENSOR_INDEX:                   
                    CurrentPart.analog_sensor[2] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_04_SENSOR_INDEX:                  
                    CurrentPart.analog_sensor[3] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_05_SENSOR_INDEX:                  
                    CurrentPart.analog_sensor[4] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_06_SENSOR_INDEX:                   
                    CurrentPart.analog_sensor[5] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_07_SENSOR_INDEX:                  
                    CurrentPart.analog_sensor[6] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_08_SENSOR_INDEX:                   
                    CurrentPart.analog_sensor[7] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_09_SENSOR_INDEX:                  
                    CurrentPart.analog_sensor[8] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_10_SENSOR_INDEX:                
                    CurrentPart.analog_sensor[9] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_11_SENSOR_INDEX:                
                    CurrentPart.analog_sensor[10] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_12_SENSOR_INDEX:                  
                    CurrentPart.analog_sensor[11] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_13_SENSOR_INDEX:                 
                    CurrentPart.analog_sensor[12] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_14_SENSOR_INDEX:                 
                    CurrentPart.analog_sensor[13] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_15_SENSOR_INDEX:                
                    CurrentPart.analog_sensor[14] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_CHANNEL_16_SENSOR_INDEX:                
                    CurrentPart.analog_sensor[15] = CurrentAnalog[sorc.int_value()];
                    break;
                case SP_HEAD_PRES_CHANNEL_INDEX:
                    CurrentPart.set_head_pressure_channel( (short) sorc.int_value() );
                    break;
                case SP_ROD_PRES_CHANNEL_INDEX:
                    CurrentPart.set_rod_pressure_channel( (short) sorc.int_value() );
                    break;
                case SP_SC_ACCEL_1_INDEX:
                    if ( SureTrakSetup.gotostep(1) )
                        SureTrakSetup.set_accel( sorc.value.text() );
                    break;
                case SP_SC_VEL_1_INDEX:
                    if ( SureTrakSetup.gotostep(1) )
                        SureTrakSetup.set_vel( sorc.value.text() );
                    break;
                case SP_SC_END_POS_1_INDEX:
                    if ( SureTrakSetup.gotostep(1) )
                        SureTrakSetup.set_end_pos( sorc.value.text() );
                    break;
                case SP_SC_VAC_WAIT_1_INDEX:
                    if ( SureTrakSetup.gotostep(1) )
                        SureTrakSetup.set_have_vacuum_wait( sorc.boolean_value() );
                    break;
                case SP_LOW_IMPACT_1_INDEX:
                    if ( SureTrakSetup.gotostep(1) )
                        SureTrakSetup.set_low_impact( sorc.value.text() );
                    break;
                case SP_LOW_IMPACT_FROM_INPUT_1_INDEX:
                    if ( SureTrakSetup.gotostep(1) )
                        SureTrakSetup.set_have_low_impact_from_input( sorc.boolean_value() );
                    break;
                case SP_SC_ACCEL_2_INDEX:
                    if ( SureTrakSetup.gotostep(2) )
                        SureTrakSetup.set_accel( sorc.value.text() );
                    break;
                case SP_SC_VEL_2_INDEX:
                    if ( SureTrakSetup.gotostep(2) )
                        SureTrakSetup.set_vel( sorc.value.text() );
                    break;
                case SP_SC_END_POS_2_INDEX:
                    if ( SureTrakSetup.gotostep(2) )
                        SureTrakSetup.set_end_pos( sorc.value.text() );
                    break;
                case SP_SC_VAC_WAIT_2_INDEX:
                    if ( SureTrakSetup.gotostep(2) )
                        SureTrakSetup.set_have_vacuum_wait( sorc.boolean_value() );
                    break;
                case SP_LOW_IMPACT_2_INDEX:
                    if ( SureTrakSetup.gotostep(2) )
                        SureTrakSetup.set_low_impact( sorc.value.text() );
                    break;
                case SP_LOW_IMPACT_FROM_INPUT_2_INDEX:
                    if ( SureTrakSetup.gotostep(2) )
                        SureTrakSetup.set_have_low_impact_from_input( sorc.boolean_value() );
                    break;
                case SP_SC_ACCEL_3_INDEX:
                    if ( SureTrakSetup.gotostep(3) )
                        SureTrakSetup.set_accel( sorc.value.text() );
                    break;
                case SP_SC_VEL_3_INDEX:
                    if ( SureTrakSetup.gotostep(3) )
                        SureTrakSetup.set_vel( sorc.value.text() );
                    break;
                case SP_SC_END_POS_3_INDEX:
                    if ( SureTrakSetup.gotostep(3) )
                        SureTrakSetup.set_end_pos( sorc.value.text() );
                    break;
                case SP_SC_VAC_WAIT_3_INDEX:
                    if ( SureTrakSetup.gotostep(3) )
                        SureTrakSetup.set_have_vacuum_wait( sorc.boolean_value() );
                    break;
                case SP_LOW_IMPACT_3_INDEX:
                    if ( SureTrakSetup.gotostep(3) )
                        SureTrakSetup.set_low_impact( sorc.value.text() );
                    break;
                case SP_LOW_IMPACT_FROM_INPUT_3_INDEX:
                    if ( SureTrakSetup.gotostep(3) )
                        SureTrakSetup.set_have_low_impact_from_input( sorc.boolean_value() );
                    break;
                case SP_SC_ACCEL_4_INDEX:
                    if ( SureTrakSetup.gotostep(4) )
                        SureTrakSetup.set_accel( sorc.value.text() );
                    break;
                case SP_SC_VEL_4_INDEX:
                    if ( SureTrakSetup.gotostep(4) )
                        SureTrakSetup.set_vel( sorc.value.text() );
                    break;
                case SP_SC_END_POS_4_INDEX:
                    if ( SureTrakSetup.gotostep(4) )
                        SureTrakSetup.set_end_pos( sorc.value.text() );
                    break;
                case SP_SC_VAC_WAIT_4_INDEX:
                    if ( SureTrakSetup.gotostep(4) )
                        SureTrakSetup.set_have_vacuum_wait( sorc.boolean_value() );
                    break;
                case SP_LOW_IMPACT_4_INDEX:
                    if ( SureTrakSetup.gotostep(4) )
                        SureTrakSetup.set_low_impact( sorc.value.text() );
                    break;
                case SP_LOW_IMPACT_FROM_INPUT_4_INDEX:
                    if ( SureTrakSetup.gotostep(4) )
                        SureTrakSetup.set_have_low_impact_from_input( sorc.boolean_value() );
                    break;
                case SP_SC_ACCEL_5_INDEX:
                    if ( SureTrakSetup.gotostep(5) )
                        SureTrakSetup.set_accel( sorc.value.text() );
                    break;
                case SP_SC_VEL_5_INDEX:
                    if ( SureTrakSetup.gotostep(5) )
                        SureTrakSetup.set_vel( sorc.value.text() );
                    break;
                case SP_SC_END_POS_5_INDEX:
                    if ( SureTrakSetup.gotostep(5) )
                        SureTrakSetup.set_end_pos( sorc.value.text() );
                    break;
                case SP_SC_VAC_WAIT_5_INDEX:
                    if ( SureTrakSetup.gotostep(5) )
                        SureTrakSetup.set_have_vacuum_wait( sorc.boolean_value() );
                    break;
                case SP_LOW_IMPACT_5_INDEX:
                    if ( SureTrakSetup.gotostep(5) )
                        SureTrakSetup.set_low_impact( sorc.value.text() );
                    break;
                case SP_LOW_IMPACT_FROM_INPUT_5_INDEX:
                    if ( SureTrakSetup.gotostep(5) )
                        SureTrakSetup.set_have_low_impact_from_input( sorc.boolean_value() );
                    break;
                case SP_SC_ACCEL_6_INDEX:
                    if ( SureTrakSetup.gotostep(6) )
                        SureTrakSetup.set_accel( sorc.value.text() );
                    break;
                case SP_SC_VEL_6_INDEX:
                    if ( SureTrakSetup.gotostep(6) )
                        SureTrakSetup.set_vel( sorc.value.text() );
                    break;
                case SP_SC_END_POS_6_INDEX:
                    if ( SureTrakSetup.gotostep(6) )
                        SureTrakSetup.set_end_pos( sorc.value.text() );
                    break;
                case SP_SC_VAC_WAIT_6_INDEX:
                    if ( SureTrakSetup.gotostep(6) )
                        SureTrakSetup.set_have_vacuum_wait( sorc.boolean_value() );
                    break;
                case SP_LOW_IMPACT_6_INDEX:
                    if ( SureTrakSetup.gotostep(6) )
                        SureTrakSetup.set_low_impact( sorc.value.text() );
                    break;
                case SP_LOW_IMPACT_FROM_INPUT_6_INDEX:
                    if ( SureTrakSetup.gotostep(6) )
                        SureTrakSetup.set_have_low_impact_from_input( sorc.boolean_value() );
                    break;
                case SP_VAC_LS_INDEX:
                    SureTrakSetup.set_vacuum_limit_switch_number( sorc.value );
                    break;
                case SP_LS1_POS_INDEX:
                    CurrentLimitSwitch.pos(0) = sorc.value;
                    break;
                case SP_LS2_POS_INDEX:
                    CurrentLimitSwitch.pos(1) = sorc.value;
                    break;
                case SP_LS3_POS_INDEX:
                    CurrentLimitSwitch.pos(2) = sorc.value;
                    break;
                case SP_LS4_POS_INDEX:
                    CurrentLimitSwitch.pos(3) = sorc.value;
                    break;
                case SP_LS5_POS_INDEX:
                    CurrentLimitSwitch.pos(4) = sorc.value;
                    break;
                case SP_LS6_POS_INDEX:
                    CurrentLimitSwitch.pos(5) = sorc.value;
                    break;
                case SP_LS1_DESC_INDEX:
                    CurrentLimitSwitch.desc(0) = sorc.value;
                    break;
                case SP_LS2_DESC_INDEX:
                    CurrentLimitSwitch.desc(1) = sorc.value;
                    break;
                case SP_LS3_DESC_INDEX:
                    CurrentLimitSwitch.desc(2) = sorc.value;
                    break;
                case SP_LS4_DESC_INDEX:
                    CurrentLimitSwitch.desc(3) = sorc.value;
                    break;
                case SP_LS5_DESC_INDEX:
                    CurrentLimitSwitch.desc(4) = sorc.value;
                    break;
                case SP_LS6_DESC_INDEX:
                    CurrentLimitSwitch.desc(5) = sorc.value;
                    break;
                case SP_LS1_WIRE_INDEX:
                    CurrentLimitSwitch.wire(0) = sorc.value;
                    break;
                case SP_LS2_WIRE_INDEX:
                    CurrentLimitSwitch.wire(1) = sorc.value;
                    break;
                case SP_LS3_WIRE_INDEX:
                    CurrentLimitSwitch.wire(2) = sorc.value;
                    break;
                case SP_LS4_WIRE_INDEX:
                    CurrentLimitSwitch.wire(3) = sorc.value;
                    break;
                case SP_LS5_WIRE_INDEX:
                    CurrentLimitSwitch.wire(4) = sorc.value;
                    break;
                case SP_LS6_WIRE_INDEX:
                    CurrentLimitSwitch.wire(5) = sorc.value;
                    break;
                case SP_VEL_LOOP_GAIN_INDEX:
                    SureTrakSetup.set_velocity_loop_gain( sorc.value );
                    break;
                case SP_BREAK_VEL_1_INDEX:
                    SureTrakSetup.set_gain_break_velocity( 0, sorc.value );
                    break;
                case SP_BREAK_GAIN_1_INDEX:
                    SureTrakSetup.set_gain_break_gain( 0, sorc.value );
                    break;
                case SP_INTEGRAL_GAIN_INDEX:
                    SureTrakSetup.set_integral_gain( sorc.value );
                    break;
                case SP_DERIVATIVE_GAIN_INDEX:
                    SureTrakSetup.set_derivative_gain( sorc.value );
                    break;
                case SP_FT_STOP_POS_INDEX:
                    SureTrakSetup.set_ft_stop_pos( sorc.value );
                    break;
                case SP_WU_ENABLE_INDEX:
                    Warmup.is_enabled = sorc.boolean_value();
                    break;
                case SP_WU_PART_INDEX:
                    Warmup.partname = sorc.value;
                    break;
                case SP_WU_WIRE_INDEX:
                    copystring( Warmup.wire, sorc.value.text(), WARMUP_WIRE_LEN );
                    break;
                case SP_WU_WIRE_STATE_INDEX:
                    Warmup.run_if_on = sorc.boolean_value();
                    break;

                case SP_PC_ENABLE_INDEX:
                    PressureControl.is_enabled = sorc.boolean_value();
                    break;
                case SP_PC_POSITIVE_ONLY_INDEX:
                    PressureControl.use_only_positive_voltages = sorc.boolean_value();
                    break;
                case SP_RC_ENABLE_INDEX:
                    PressureControl.retctl_is_enabled = sorc.boolean_value();
                    break;
                case SP_PC_HARDWARE_CLOSED_LOOP_INDEX:
                    b = sorc.boolean_value();
                    for ( i=0; i<3; i++ )
                        PressureControl.step[i].is_open_loop = b;
                    break;
                case SP_PC_ALL_PERCENT_INDEX:
                    PressureControl.proportional_valve = sorc.boolean_value();
                    break;
                case SP_PC_RAMP_1_INDEX:
                    PressureControl.step[0].ramp = sorc.value;
                    break;
                case SP_PC_GOOSE_1_INDEX:
                    PressureControl.step[0].is_goose_step = sorc.boolean_value();
                    break;
                case SP_PC_PRES_1_INDEX:
                    PressureControl.step[0].pressure = sorc.value;
                    break;
                case SP_PC_HOLD_TIME_1_INDEX:
                    PressureControl.step[0].holdtime = sorc.value;
                    break;
                case SP_PC_RAMP_2_INDEX:
                    PressureControl.step[1].ramp = sorc.value;
                    break;
                case SP_PC_GOOSE_2_INDEX:
                    PressureControl.step[1].is_goose_step = sorc.boolean_value();
                    break;
                case SP_PC_PRES_2_INDEX:
                    PressureControl.step[1].pressure = sorc.value;
                    break;
                case SP_PC_HOLD_TIME_2_INDEX:
                    PressureControl.step[1].holdtime = sorc.value;
                    break;
                case SP_PC_RAMP_3_INDEX:
                    PressureControl.step[2].ramp = sorc.value;
                    break;
                case SP_PC_GOOSE_3_INDEX:
                    PressureControl.step[2].is_goose_step = sorc.boolean_value();
                    break;
                case SP_PC_PRES_3_INDEX:
                    PressureControl.step[2].pressure = sorc.value;
                    break;
                case SP_PC_HOLD_TIME_3_INDEX:
                    PressureControl.step[2].holdtime = sorc.value;
                    break;
                case SP_PC_SENSOR_INDEX:
                    //PressureControl.sensor_number = sorc.int_value();
                    break;
                case SP_PC_VOLT_RANGE_INDEX:
                    PressureControl.voltage_output_range = sorc.int_value();
                    break;
                case SP_PC_ARM_LS_INDEX:
                    PressureControl.arm_intensifier_ls_number = sorc.int_value();
                    break;
                case SP_PC_LOOP_GAIN_INDEX:
                    PressureControl.pressure_loop_gain = sorc.value;
                    break;
                case SP_PC_INTEGRAL_GAIN_INDEX:
                    PressureControl.integral_gain = sorc.value;
                    break;
                case SP_PC_DERIVATIVE_GAIN_INDEX:
                    PressureControl.derivative_gain = sorc.value;
                    break;
                case SP_PC_RETRACT_VOLTS_INDEX:
                    PressureControl.retract_volts = sorc.value;
                    break;
                case SP_PC_PARK_VOLTS_INDEX:
                    PressureControl.park_volts = sorc.value;
                    break;
                case SP_RC_ARM_PERCENT_INDEX:
                    PressureControl.retctl_arm_pres = sorc.value;
                    break;
                case SP_RC_RETRACT_PERCENT_INDEX:
                    PressureControl.retctl_pres = sorc.value;
                    break;
                }
            return;
            }
        }
    }

save_one_part_based_global_line( sorc );
}

/***********************************************************************
*                              SAVE_PART                               *
***********************************************************************/
static void save_part()
{
#ifdef UNICODE
NAME_CLASS d;
#endif
NAME_CLASS s;
SETPOINT_CLASS mysp;
SETPOINT_CLASS sp;
int i;

/*
------------------
Load existing part
------------------ */
get_current_part();

/*
---------
Copy part
--------- */
if ( !NewPartName.isempty() )
    {
    copypart( CurrentPart.computer, CurrentPart.machine, NewPartName, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, COPY_PART_ALL );
    CurrentPart.name = NewPartName;
    get_current_part();
    }

/*
--------------------------
Read changes from csv file
-------------------------- */
s = CsvHomeDirectory;
s.cat_path( CurrentPart.name );
s += CsvSuffix;

#ifdef UNICODE
d = CsvHomeDirectory;
d.cat_path( CurrentPart.name );
d += TEXT( "unicode" );
d += CsvSuffix;

if ( d.file_exists() )
    d.delete_file();
#endif

if ( s.file_exists() )
    {
    writelog( TEXT("CSV File Found"), s.text() );
#ifdef UNICODE
    /*
    -------------------------------------------------------------
    Zitai uses an ascii file, convert to a temporary unicode file
    ------------------------------------------------------------- */
    touni( d.text(), s.text() );
    sp.get( d );
#else
    sp.get( s );
#endif
    for ( i=0; i<sp.count(); i++ )
        {
        save_one_line( sp[i] );
        }
    SureTrakSetup.determine_nof_steps();
    if ( HavePressureControl )
        get_percents();
    }


#ifdef UNICODE
if ( d.file_exists() )
    d.delete_file();
#endif

save_changes();
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_DBINIT:
        save_part();
        PostMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                           EXTRACT_PART_NAME                          *
* Pass the string with the part name in it. It is either contiguous    *
* or is delimited with quotes. Extract the name and put it into dest   *
* and return a pointer to the next string or to a null character.      *
***********************************************************************/
TCHAR * extract_part_name( STRING_CLASS & dest, TCHAR * sorc )
{
TCHAR * ep;
TCHAR   oldchar;

if ( *sorc == DoubleQuoteChar )
    {
    sorc++;
    ep = sorc;
    while ( (*ep != DoubleQuoteChar) && (*ep != NullChar) )
        ep++;
    if ( *ep == DoubleQuoteChar )
        {
        *ep = NullChar;
        dest = sorc;
        *ep = DoubleQuoteChar;
        ep++;
        if ( *ep == SpaceChar )
            ep++;
        }
    }
else
    {
    ep = sorc;
    while ( (*ep != SpaceChar) && (*ep != NullChar) )
        ep++;
    oldchar = *ep;
    if ( *ep == SpaceChar )
        *ep = NullChar;
    dest = sorc;
    *ep = oldchar;
    if ( *ep != NullChar )
        ep++;
    }
return ep;
}

/***********************************************************************
*                          PARSE_COMMAND_LINE                          *
* The command line is the part name to save or the sorc name and       *
* dest name of part to create from an existing part.                   *
***********************************************************************/
BOOLEAN parse_command_line( TCHAR * cmd_line )
{
STRING_CLASS s;
TCHAR      * cp;

cp = findstring( MyClassName, cmd_line );
if ( cp )
    cp = findchar( SpaceChar, cp );

if ( cp )
    cp++; /* skip space */
else
    cp = cmd_line;

s = cp;
s.strip();
cp = extract_part_name( CurrentPart.name, s.text() );
if ( *cp != NullChar )
    extract_part_name( NewPartName, cp );

if ( CurrentPart.name.isempty() )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                              STARTUP                                 *
***********************************************************************/
BOOLEAN startup( TCHAR * cmd_line )
{
COMPUTER_CLASS          c;
WNDCLASS                wc;
MACHINE_NAME_LIST_CLASS m;
NAME_CLASS              s;
INI_CLASS               ini;


if ( !parse_command_line(cmd_line) )
    return FALSE;

EventClient.set_ip_addr( EventClient.get_local_ip_addr() );
EventClient.set_port( EventClient.get_local_port() );
EventClient.connect_to_server();

c.startup();
CurrentMachine.computer = c.whoami();
CurrentPart.computer    = c.whoami();

CurrentAnalog.get();
units_startup();

/*
-------------------------------------------------------------
Get the name of the directory where I will find the csv files
------------------------------------------------------------- */
s.get_local_ini_file_name( SavePartIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(CsvHomeDirectoryKey) )
    CsvHomeDirectory = ini.get_string();

UseLogFile = FALSE;
if ( ini.find(LogFileKey) )
    {
    LogFile = ini.get_string();
    if ( LogFile == UNKNOWN )
        LogFile.empty();
    if ( LogFile.isempty() )
        UseLogFile = FALSE;
    else
        UseLogFile = ini.get_boolean( UseLogFileKey );
    }
else
    {
    LogFile = exe_directory();
    LogFile.cat_path( TEXT("savepart.log") );
    ini.put_string( LogFileKey, LogFile.text() );
    UseLogFile = TRUE;
    ini.put_boolean( UseLogFileKey, UseLogFile );
    }

if ( UseLogFile )
    writelog( TEXT("Command Line"), cmd_line );

/*
---------------------------------------------
At the present time there is only one machine
--------------------------------------------- */
m.add_computer( CurrentMachine.computer );
m.rewind();
if ( m.next() )
    {
    CurrentMachine.name = m.name();
    CurrentPart.machine = m.name();
    }
else
    {
    return FALSE;
    }

CurrentMachine.find( CurrentMachine.computer, CurrentMachine.name );

/*
---------------------------------------------
See if this computer uses a multipart runlist
--------------------------------------------- */
HaveMultipartRunlist = boolean_from_ini( VisiTrakIniFile, ConfigSection, HaveMultipartRunlistKey );

/*
------------------------------------------------------------------------
Load a string to be used by the multipart runlist load and save routines
------------------------------------------------------------------------ */
NotUsedString = resource_string( NOT_USED_STRING );

HaveSureTrakControl = boolean_from_ini( VisiTrakIniFile, ConfigSection, HaveSureTrakKey );
HavePressureControl = boolean_from_ini( VisiTrakIniFile, ConfigSection, HavePressureControlKey );


wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MAKEINTRESOURCE(EDITPART_ICON) );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );
s = resource_string( MAIN_WINDOW_TITLE_STRING );

MainWindow = CreateWindow(
    MyClassName,
    s.text(),
    WS_POPUPWINDOW | WS_CAPTION,
    0, 0,                             // X,y
    640, 290,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, SW_HIDE );
UpdateWindow( MainWindow );
return TRUE;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
units_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;
TCHAR * my_cmd_line;

MainInstance = this_instance;

my_cmd_line = GetCommandLine();
if ( !startup(my_cmd_line) )
    return 0;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
    }

shutdown();
return(msg.wParam);
}
