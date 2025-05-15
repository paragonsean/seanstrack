#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\setpoint.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\machine.h"
#include "..\include\verrors.h"

static const TCHAR BackSlashChar     = TEXT( '\\' );
static const TCHAR EqualsChar        = TEXT( '='  );
static const TCHAR SevenChar         = TEXT( '7'  );
static const TCHAR ConfigSection[]   = TEXT( "Config" );
static const TCHAR FlashVersionKey[] = TEXT( "FlashVersion" );

static TCHAR * Description[] = {
    { TEXT("Machine Type") },
    { TEXT("Fast Trak Board Number") },
    { TEXT("Mux Channel Number (FT1 Only)") },
    { TEXT("Is SureTrak Control") },
    { TEXT("Current Part Name") },
    { TEXT("Rod Pitch") },
    { TEXT("Profile Skip Count") },
    { TEXT("Profile Acquire Count") },
    { TEXT("Profile Time Skip (Sec.)") },
    { TEXT("Profile Flags") },
    { TEXT("Profile Max Saved") },
    { TEXT("Param Skip Count") },
    { TEXT("Param Acquire Count") },
    { TEXT("Param Time Skip (Sec.)") },
    { TEXT("Param Flags") },
    { TEXT("Param Max Saved") },
    { TEXT("Monitor Flags") },
    { TEXT("Auto Shot Timeout (Sec.)") },
    { TEXT("Downtime Timeout (Sec.)") },
    { TEXT("Differential Curve Number") },
    { TEXT("Position Base Impact Points") },
    { TEXT("Time Base Impact Points") },
    { TEXT("Pre Impact Pressure Type") },
    { TEXT("Post Impact Pressure Type") },
    { TEXT("Operator Number") },
    { TEXT("Quadrature Divisor") },
    { TEXT("Binary Valve Pulse Time") },
    { TEXT("Binary Valve Devent Time") },
    { TEXT("Binary Valve Pulse Wire") },
    { TEXT("Valve Setting after Jog Shot") }
    };
static int NofDescriptions = sizeof(Description)/sizeof(TCHAR *);

/***********************************************************************
*                           INIT_SAVE_COUNTS                           *
***********************************************************************/
void init_save_counts( SAVE_COUNTS * s )
{
s->skip_count   = 0;
s->acquire_count = 0;
s->max_saved    = 0;
s->skip_seconds = 0;
s->flags = ACQUIRE_NONE_BIT;
}

/***********************************************************************
*                         MACHINE_CLASS::INIT                          *
***********************************************************************/
void MACHINE_CLASS::init()
{
init_save_counts( &profile );
init_save_counts( &params );
type                     = COLD_CHAMBER_TYPE;
ft_board_number          = NO_BOARD_NUMBER;
muxchan                  = NO_MUX_CHANNEL;
suretrak_controlled      = FALSE;
is_ftii                  = FALSE;
is_cyclone               = FALSE;
monitor_flags            = MA_NO_MONITORING;
autoshot_timeout_seconds = DEF_AUTO_SHOT_TIMEOUT;
cycle_timeout_seconds    = 0;
diff_curve_number        = NO_DIFF_CURVE_NUMBER;
quadrature_divisor       = 1;
pulse_time               = TEXT( "1.0" );
devent_time              = TEXT( ".2" );
pulse_wire               = TEXT( "27" );
impact_pres_type[PRE_IMPACT_INDEX]  = NO_CURVE_TYPE;
impact_pres_type[POST_IMPACT_INDEX] = NO_CURVE_TYPE;
impact_points[PRE_IMPACT_INDEX]     = 0;
impact_points[POST_IMPACT_INDEX]    = 0;
}

/***********************************************************************
*                     MACHINE_CLASS::MACHINE_CLASS                     *
***********************************************************************/
MACHINE_CLASS::MACHINE_CLASS()
{
init();
}

/***********************************************************************
*                            MACHINE_CLASS                             *
*                               ISEMPTY                                *
***********************************************************************/
BOOLEAN MACHINE_CLASS::isempty()
{
return name.isempty();
}

/***********************************************************************
*              GET_BINARY_VALVE_SETTINGS_FROM_CONTROL_ALL              *
* If there is no binary valve data in the machset file I should get    *
* the information from the controll_all.txt file on the same computer. *
***********************************************************************/
static void get_binary_valve_settings_from_control_all( MACHINE_CLASS & m )
{
static TCHAR DEVENT_TIME_INTRO[] = TEXT( "V417" );
static TCHAR PULSE_TIME_INTRO[]  = TEXT( "V416" );
static TCHAR PULSE_WIRE_INTRO[]  = TEXT( "V395" );
TCHAR        * cp;
int            i;
double         x;
FILE_CLASS     f;
NAME_CLASS     s;
SETPOINT_CLASS sp;
bool have_devent_time;
bool have_pulse_time;
bool have_pulse_wire;
bool have_a_match;

have_devent_time = false;
have_pulse_time  = false;
have_pulse_wire  = false;

s.get_exe_dir_file_name( m.computer, FT2_CONTROL_ALL_FILE );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
        {
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;
            if ( *cp == TEXT('#') )
                continue;
            have_a_match = false;
            if ( !have_devent_time )
                {
                if ( start_chars_are_equal(DEVENT_TIME_INTRO, cp) )
                    {
                    cp += 5;
                    i = asctoint32( cp );
                    x = i;
                    x /= 1000000.0;
                    m.devent_time = x;
                    have_devent_time = true;
                    have_a_match = true;
                    }
                }
            if ( !have_a_match && !have_pulse_time )
                {
                if ( start_chars_are_equal(PULSE_TIME_INTRO, cp) )
                    {
                    cp += 5;
                    i = asctoint32( cp );
                    x = i;
                    x /= 1000000.0;
                    m.pulse_time = x;
                    have_pulse_time = true;
                    have_a_match = true;
                    }
                }
            if ( !have_a_match && !have_pulse_wire )
                {
                if ( start_chars_are_equal(PULSE_WIRE_INTRO, cp) )
                    {
                    cp += 6;
                    m.pulse_wire = cp;
                    have_pulse_wire = true;
                    }
                }
            if ( have_devent_time && have_pulse_time && have_pulse_wire )
                break;
            }
        f.close();
        }
    }

/*
-----------------------------------------------------------------------------------
The above assumes that this machine had the values in the control_all file. This is
only true if the machine was used before my program knew about these variables.

If the values are zero then the control_all file did not have any settings and I
should get them from the default_hmi_settings.csv file in \v5\data.
----------------------------------------------------------------------------------- */
if ( m.devent_time.int_value() == 0 )
    {
    s.get_default_hmi_settings_csvname();
    sp.get( s );
    m.devent_time = sp[DEF_HMI_DEVENT_TIME_INDEX].value;
    m.pulse_time  = sp[DEF_HMI_PULSE_TIME_INDEX].value;
    m.pulse_wire  = sp[DEF_HMI_PULSE_WIRE_INDEX].value;
    sp.cleanup();
    }
}

/***********************************************************************
*                            MACHINE_CLASS                             *
*                           READ_MACHSET_DB                            *
***********************************************************************/
BOOLEAN MACHINE_CLASS::read_machset_db( STRING_CLASS & fname )
{
static TCHAR inifile[] = TEXT( "\\" ) FTII_EDITOR_SETTINGS_FILE;

int        n;
TCHAR      c;
INI_CLASS  ini;
NAME_CLASS s;
DB_TABLE   t;
SETPOINT_CLASS sp;
unsigned int u;

if ( sp.get(fname) )
    {
    n                   = sp.count();
    type                = (BITSETYPE) sp[MACHSET_MACHINE_TYPE_INDEX].value.uint_value();

    /*
    --------------------------------------------------------------
    Make sure that a machine with a binary valve is an hmi machine
    -------------------------------------------------------------- */
    u = sp[MACHSET_MONITOR_FLAGS_INDEX].value.uint_value();
    if ( u & MA_HAS_BINARY_VALVE )
        u |= MA_HAS_HMI;
    monitor_flags       = (BITSETYPE) u;

    ft_board_number     = (short) sp[MACHSET_FT_BOARD_NO_INDEX].value.int_value();
    muxchan             = (short) sp[MACHSET_MUX_CHAN_INDEX].value.int_value();
    suretrak_controlled = sp[MACHSET_ST_BOARD_INDEX].value.boolean_value();
    current_part        = sp[MACHSET_CURRENT_PART_INDEX].value;
    rodpitch            = sp[MACHSET_RODPITCH_INDEX].value;

    profile.skip_count    = sp[MACHSET_PROFILE_SKIP_COUNT_INDEX].value.int_value();
    profile.acquire_count = sp[MACHSET_PROFILE_ACQUIRE_COUNT_INDEX].value.int_value();
    profile.skip_seconds  = sp[MACHSET_PROFILE_TIME_SKIP_INDEX].value.int_value();
    profile.flags         = (BITSETYPE) sp[MACHSET_PROFILE_FLAGS_INDEX].value.uint_value();
    profile.max_saved     = sp[MACHSET_PROFILE_MAX_SAVED_INDEX].value.int_value();

    params.skip_count     = sp[MACHSET_PARAM_SKIP_COUNT_INDEX].value.int_value();
    params.acquire_count  = sp[MACHSET_PARAM_ACQUIRE_COUNT_INDEX].value.int_value();
    params.skip_seconds   = sp[MACHSET_PARAM_TIME_SKIP_INDEX].value.int_value();
    params.flags          = (BITSETYPE) sp[MACHSET_PARAM_FLAGS_INDEX].value.uint_value();
    params.max_saved      = sp[MACHSET_PARAM_MAX_SAVED_INDEX].value.int_value();

    autoshot_timeout_seconds = (short) sp[MACHSET_AUTO_SHOT_TIMEOUT_INDEX].value.int_value();
    cycle_timeout_seconds    = (short) sp[MACHSET_CYCLE_TIMEOUT_INDEX].value.int_value();
    diff_curve_number        = (short) sp[MACHSET_DIFF_CURVE_NUMBER_INDEX].value.int_value();

    impact_points[PRE_IMPACT_INDEX]  = (short) sp[MACHSET_POSITION_IMPACT_PTS_INDEX].value.int_value();
    impact_points[POST_IMPACT_INDEX] = (short) sp[MACHSET_TIME_IMPACT_PTS_INDEX].value.int_value();

    impact_pres_type[PRE_IMPACT_INDEX]  = (BITSETYPE) sp[MACHSET_PRE_PRES_TYPE_INDEX].value.uint_value();
    impact_pres_type[POST_IMPACT_INDEX] = (BITSETYPE) sp[MACHSET_POST_PRES_TYPE_INDEX].value.uint_value();

    human = sp[MACHSET_OPERATOR_NUMBER_INDEX].value;
    quadrature_divisor = 1;
    if ( n > MACHSET_QUADRATURE_DIVISOR_INDEX )
        {
        quadrature_divisor = sp[MACHSET_QUADRATURE_DIVISOR_INDEX].value.int_value();
        }
    else
        {
        quadrature_divisor = 1;
        /*
        -----------------------------------------------------------------------------------------
        The Prince encoder puts out a 100 pitch signal. Divide by 5 and treat like a 20 pitch rod
        ----------------------------------------------------------------------------------------- */
        if ( rodpitch == PRINCE_RODPITCH )
            quadrature_divisor = 5;
        }

    if ( n <= MACHSET_PULSE_WIRE_INDEX )
        {
        get_binary_valve_settings_from_control_all( *this );
        }
    else
        {
        devent_time = sp[MACHSET_DEVENT_TIME_INDEX].value;
        pulse_time  = sp[MACHSET_PULSE_TIME_INDEX].value;
        pulse_wire  = sp[MACHSET_PULSE_WIRE_INDEX].value;
        }

    if ( n <= MACHSET_VALVE_SETTING_AFTER_JOG_SHOT_INDEX )
        valve_setting_after_jog_shot.null();
    else
        valve_setting_after_jog_shot = sp[MACHSET_VALVE_SETTING_AFTER_JOG_SHOT_INDEX].value;

    /*
    ------------------------------------------------------------
    If my board is in the ftii table then this is a ftii machine
    ------------------------------------------------------------ */
    is_ftii = FALSE;
    s.upsize( fname.len() + 10 );
    s = fname;
    s.convert_to_data_dir();
    s.cat_path( FTII_BOARDS_DB );
    if ( s.file_exists() )
        {
        if ( t.open(s, FTII_BOARDS_RECLEN, PFL) )
            {
            while ( t.get_next_record(FALSE) )
                {
                if ( ft_board_number == t.get_short(FTII_BOARDS_NUMBER_OFFSET) )
                    {
                    is_ftii = TRUE;
                    break;
                    }
                }
            t.close();
            }
        }
    is_cyclone = FALSE;
    if ( is_ftii )
        {
        s.replace( FTII_BOARDS_DB, name.text() );
        s += inifile;
        if ( s.file_exists() )
            {
            ini.set_file( s.text() );
            if ( ini.find(ConfigSection, FlashVersionKey) )
                {
                c = *ini.get_string();
                if ( c == SevenChar )
                    is_cyclone = TRUE;
               }
            }
        }
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         MACHINE_CLASS::FIND                          *
***********************************************************************/
BOOLEAN MACHINE_CLASS::find( STRING_CLASS & computer_to_find, STRING_CLASS & name_to_find )
{
NAME_CLASS s;

computer = computer_to_find;
name     = name_to_find;

s.get_machset_csvname( computer, name );
return read_machset_db( s );
}

/***********************************************************************
*                 MACHINE_CLASS::REFRESH_MONITOR_FLAGS                 *
***********************************************************************/
BOOLEAN MACHINE_CLASS::refresh_monitor_flags()
{
NAME_CLASS s;
SETPOINT_CLASS sp;

if ( computer.isempty() || name.isempty() )
    return FALSE;

s.get_machset_csvname( computer, name );
if ( sp.get_one_value(s, s, MACHSET_MONITOR_FLAGS_INDEX) )
    {
    monitor_flags = (BITSETYPE) s.uint_value();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             has_binary_valve                         *
***********************************************************************/
BOOLEAN MACHINE_CLASS::has_binary_valve()
{
if ( monitor_flags & MA_HAS_BINARY_VALVE )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                          set_has_binary_valve                        *
***********************************************************************/
void MACHINE_CLASS::set_has_binary_valve( BOOLEAN b )
{
const BITSETYPE mask = ~MA_HAS_BINARY_VALVE;

if ( b )
    monitor_flags |= MA_HAS_BINARY_VALVE;
else
    monitor_flags &= mask;
}

/***********************************************************************
*                                   has_hmi                            *
***********************************************************************/
BOOLEAN MACHINE_CLASS::has_hmi()
{
if ( monitor_flags & MA_HAS_HMI )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                                set_has_hmi                           *
***********************************************************************/
void MACHINE_CLASS::set_has_hmi( BOOLEAN b )
{
const BITSETYPE mask = ~MA_HAS_HMI;

if ( b )
    monitor_flags |= MA_HAS_HMI;
else
    monitor_flags &= mask;
}

/***********************************************************************
*                              ls_off_at_eos                           *
***********************************************************************/
BOOLEAN MACHINE_CLASS::ls_off_at_eos()
{
if ( monitor_flags & MA_LS_OFF_AT_EOS )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           set_ls_off_at_eos                          *
***********************************************************************/
void MACHINE_CLASS::set_ls_off_at_eos( BOOLEAN b )
{
const BITSETYPE mask = ~MA_LS_OFF_AT_EOS;

if ( b )
    monitor_flags |= MA_LS_OFF_AT_EOS;
else
    monitor_flags &= mask;
}

/***********************************************************************
*                         MACHINE_CLASS::SAVE                          *
***********************************************************************/
BOOLEAN MACHINE_CLASS::save()
{

int            i;
int            old_n;
NAME_CLASS     s;
SETPOINT_CLASS sp;
DB_TABLE       t;

if ( NofDescriptions < MACHSET_SETPOINT_COUNT )
    MessageBox( 0, TEXT("NofDescriptions < MACHSET_SETPOINT_COUNT"), TEXT("MACHINE_CLASS::save()"), MB_OK );

/*
----------------------------
Create the machine directory
---------------------------- */
s.get_machine_dir_file_name( computer, name, 0 );
if ( !s.directory_exists() )
    s.create_directory();

s.get_dstat_dbname( computer, name );
t.ensure_existance( s );

s.get_semaphor_dbname( computer, name );
t.ensure_existance( s );

/*
------------------------------------
Create the machine results directory
------------------------------------ */
s.get_machine_results_dir_file_name( computer, name, 0 );
if ( !s.directory_exists() )
    s.create_directory();

s.cat_path( DOWNTIME_DB );
t.ensure_existance( s );

old_n = MACHSET_SETPOINT_COUNT;
s.get_machset_csvname( computer, name );
if ( !s.file_exists() )
    {
    /*
    ----------------------------------------------------------------------------------
    The file should already exist but if it does not I should do something intelligent
    ---------------------------------------------------------------------------------- */
    sp.setsize( MACHSET_SETPOINT_COUNT );

    sp[MACHSET_FT_BOARD_NO_INDEX].value = 1;
    sp[MACHSET_MUX_CHAN_INDEX].value    = 1;

    sp[MACHSET_PROFILE_SKIP_COUNT_INDEX].value    = 10;
    sp[MACHSET_PROFILE_ACQUIRE_COUNT_INDEX].value = 5;
    sp[MACHSET_PROFILE_TIME_SKIP_INDEX].value     = 600;
    sp[MACHSET_PROFILE_FLAGS_INDEX].value         = ACQUIRE_ALL_BIT;
    sp[MACHSET_PROFILE_MAX_SAVED_INDEX].value     = 5000;

    sp[MACHSET_PARAM_SKIP_COUNT_INDEX].value    = 10;
    sp[MACHSET_PARAM_ACQUIRE_COUNT_INDEX].value = 5;
    sp[MACHSET_PARAM_TIME_SKIP_INDEX].value     = 600;
    sp[MACHSET_PARAM_FLAGS_INDEX].value         = ACQUIRE_ALL_BIT;
    sp[MACHSET_PARAM_MAX_SAVED_INDEX].value     = 5000;
    sp[MACHSET_QUADRATURE_DIVISOR_INDEX].value  = 1;
    }
else
    {
    /*
    ---------------------------------------------------------
    I need to get the current values because I don't save the
    monitor setup setpoints or the ft board number.
    --------------------------------------------------------- */
    sp.get( s );
    old_n = sp.count();
    if ( old_n < MACHSET_SETPOINT_COUNT )
        sp.setsize( MACHSET_SETPOINT_COUNT );
    }

for ( i=0; i<MACHSET_SETPOINT_COUNT; i++ )
    {
    if ( sp[i].desc.isempty() )
        sp[i].desc = Description[i];
    }

sp[MACHSET_MACHINE_TYPE_INDEX].value  = (unsigned int) type;
sp[MACHSET_MONITOR_FLAGS_INDEX].value = (unsigned int) monitor_flags;
sp[MACHSET_ST_BOARD_INDEX].value      = suretrak_controlled;
sp[MACHSET_CURRENT_PART_INDEX].value  = current_part;
sp[MACHSET_RODPITCH_INDEX].value      = rodpitch;

sp[MACHSET_AUTO_SHOT_TIMEOUT_INDEX].value = autoshot_timeout_seconds;
sp[MACHSET_CYCLE_TIMEOUT_INDEX].value     = cycle_timeout_seconds;
sp[MACHSET_DIFF_CURVE_NUMBER_INDEX].value = diff_curve_number;

sp[MACHSET_POSITION_IMPACT_PTS_INDEX].value = impact_points[PRE_IMPACT_INDEX];
sp[MACHSET_TIME_IMPACT_PTS_INDEX].value     = impact_points[POST_IMPACT_INDEX];

sp[MACHSET_PRE_PRES_TYPE_INDEX].value  = (unsigned int) impact_pres_type[PRE_IMPACT_INDEX];
sp[MACHSET_POST_PRES_TYPE_INDEX].value = (unsigned int) impact_pres_type[POST_IMPACT_INDEX];

sp[MACHSET_OPERATOR_NUMBER_INDEX].value = human;
sp[MACHSET_QUADRATURE_DIVISOR_INDEX].value = quadrature_divisor;
sp[MACHSET_PULSE_TIME_INDEX].value  = pulse_time;
sp[MACHSET_DEVENT_TIME_INDEX].value = devent_time;
sp[MACHSET_PULSE_WIRE_INDEX].value  = pulse_wire;

sp[MACHSET_VALVE_SETTING_AFTER_JOG_SHOT_INDEX].desc = Description[MACHSET_VALVE_SETTING_AFTER_JOG_SHOT_INDEX];
sp[MACHSET_VALVE_SETTING_AFTER_JOG_SHOT_INDEX].value = valve_setting_after_jog_shot;

return sp.put( s );
}

/***********************************************************************
*                             MACHINE_CLASS                            *
*                                  =                                   *
***********************************************************************/
void MACHINE_CLASS::operator=( const MACHINE_CLASS & sorc )
{
name               = sorc.name;
computer           = sorc.computer;
current_part       = sorc.current_part;
rodpitch           = sorc.rodpitch;
quadrature_divisor = sorc.quadrature_divisor;
human              = sorc.human;

type = sorc.type;
ft_board_number = sorc.ft_board_number;
muxchan = sorc.muxchan;
is_ftii = sorc.is_ftii;
suretrak_controlled = sorc.suretrak_controlled;

profile.skip_count    = sorc.profile.skip_count;
profile.acquire_count = sorc.profile.acquire_count;
profile.max_saved     = sorc.profile.max_saved;
profile.skip_seconds  = sorc.profile.skip_seconds;
profile.flags         = sorc.profile.flags;

params.skip_count    = sorc.params.skip_count;
params.acquire_count = sorc.params.acquire_count;
params.max_saved     = sorc.params.max_saved;
params.skip_seconds  = sorc.params.skip_seconds;
params.flags         = sorc.params.flags;

monitor_flags            = sorc.monitor_flags;
autoshot_timeout_seconds = sorc.autoshot_timeout_seconds;
cycle_timeout_seconds    = sorc.cycle_timeout_seconds;
diff_curve_number        = sorc.diff_curve_number;

impact_pres_type[PRE_IMPACT_INDEX]  = sorc.impact_pres_type[PRE_IMPACT_INDEX];
impact_pres_type[POST_IMPACT_INDEX] = sorc.impact_pres_type[POST_IMPACT_INDEX];

impact_points[PRE_IMPACT_INDEX]     = sorc.impact_points[PRE_IMPACT_INDEX];
impact_points[POST_IMPACT_INDEX]    = sorc.impact_points[POST_IMPACT_INDEX];

pulse_time               = sorc.pulse_time;
devent_time              = sorc.devent_time;
pulse_wire               = sorc.pulse_wire;
valve_setting_after_jog_shot = sorc.valve_setting_after_jog_shot;
}