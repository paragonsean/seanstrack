#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\limit_switch_class.h"
#include "..\include\machine.h"
#include "..\include\setpoint.h"
#include "..\include\structs.h"
#include "..\include\stsetup.h"
#include "..\include\plookup.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\warmupcl.h"

static int OLD_MAX_ST_LIMIT_SWITCHES = 4;

static TCHAR DataString[]      = TEXT( "data" );
static TCHAR SlashDataString[] = TEXT( "\\data" );
static TCHAR ExesString[]      = TEXT( "exes" );
static TCHAR ResultsString[]   = TEXT( "results" );
static TCHAR StarDotStar[]     = TEXT( "*.*" );
static TCHAR ZeroPointZero[]   = TEXT( "0.0" );
static TCHAR VisitrakIniFile[] = TEXT( "visitrak.ini" );
static TCHAR ConfigSection[]   = TEXT( "Config" );
static TCHAR ThisComputerKey[] = TEXT( "ThisComputer" );
static TCHAR RootDirKey[] = TEXT( "RootDir" );
static TCHAR HaveSureTrakKey[]   = TEXT( "HaveSureTrak" );
static TCHAR LimitSwitchPosFile[] = LIMIT_SWITCH_POS_FILE ;
static TCHAR VacuumLimitSwitchKey[] = TEXT( "VacuumLimitSwitch" );
static NAME_CLASS V5Directory;
static NAME_CLASS ComputerDirectory;
static NAME_CLASS DataDirectory;
static TEXT_LIST_CLASS MachineList;

static BOOLEAN HaveSureTrak = FALSE;

static TCHAR * MachineDescription[] = {
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
    { TEXT("Operator Number") }
    };
static int NofMachineDescriptions = sizeof(MachineDescription)/sizeof(TCHAR *);

static TCHAR * PartDescription[] = {
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
    };
static int NofPartDescriptions = sizeof(PartDescription)/sizeof(TCHAR *);

bool HaveNewAnalogSensor = false;
int  NewAnalogSensorNumber[] = { 23, 24, 25 };

/**********************************************************************
*                          IS_DEFAULT_SENSOR                          *
* When converting an old analog sensor table I compare the values     *
* and equation components to see if they are the same as the original *
* table. If so I assume I don't have to convert them.                 *
**********************************************************************/
static bool is_default_sensor( int i, double high, double low, double m, double s, double d, double a )
{
struct OLD_DEFAULT_SENSOR_ENTRY {
    double high_value;
    double low_value;
    double m;
    double s;
    double d;
    double a;
    };

/*
-------------------------------------------------------------------
High Value  Low Value    M             S            D             A
------------------------------------------------------------------- */
static OLD_DEFAULT_SENSOR_ENTRY ds[] = {

{ 10000.00,         0.0,10000.00,         0.0,    4095.000,         0.0 },
{ 5000.000,         0.0,5000.000,         0.0,    4095.000,         0.0 },
{ 932.0000,    32.00000,900.0000,    819.0000,    3276.000,    32.00000 },
{ 500.0000,         0.0,500.0000,    819.0000,    3276.000,         0.0 },
{ 1832.000,    32.00000,1800.000,    819.0000,    3276.000,    32.00000 },
{ 1000.000,         0.0,968.0000,    819.0000,    3276.000,    32.00000 },
{ 10.00000,         0.0,10.00000,         0.0,    4095.000,         0.0 },
{ 689.5000,         0.0,689.5000,         0.0,    4095.000,         0.0 },
{ 344.7500,         0.0,344.7500,         0.0,    4095.000,         0.0 },
{ 4095.000,         0.0,1.000000,         0.0,    1.000000,         0.0 },
{ 10000.00,         0.0,10000.00,    819.0000,    3276.000,         0.0 },
{ 5000.000,         0.0,5000.000,    819.0000,    3276.000,         0.0 },
{ 689.5000,         0.0,689.5000,    819.0000,    3276.000,         0.0 },
{ 344.7500,         0.0,344.7500,    819.0000,    3276.000,         0.0 },
{ 1400.000,         0.0,1400.000,    819.0000,    3276.000,         0.0 },
{ 500.0000,         0.0,500.0000,         0.0,    4095.000,         0.0 },
{ 20000.00,         0.0,20000.00,         0.0,    4095.000,         0.0 },
{ 1379.000,         0.0,1379.000,         0.0,    4095.000,         0.0 },
{ 150.0000,   -50.00000,200.0000,    819.0000,    3276.000,   -50.00000 },
{ 703.0700,         0.0,703.0700,    819.0000,    3276.000,         0.0 },
{ 302.0000,   -67.00000,369.0000,    819.0000,    3276.000,   -67.00000 },
{ 10000.00,         0.0,10000.00,         0.0,    4095.000,         0.0 },
{ 5000.000,         0.0,5000.000,         0.0,    4095.000,         0.0 },
{   10.000,         0.0,  10.000,         0.0,    4095.000,         0.0 },
{    5.000,         0.0,   5.000,         0.0,    4095.000,         0.0 },
{   10.000,       -10.0,  20.000,         0.0,    4095.000,       -10.0 }
};
const int nof_defaults = sizeof(ds)/sizeof(OLD_DEFAULT_SENSOR_ENTRY);

if ( i < 0 || i >= nof_defaults )
    return FALSE;

if ( not_zero(ds[i].high_value - high) )
    return FALSE;
if ( not_zero(ds[i].low_value - low ) )
    return FALSE;
if ( not_zero(ds[i].m - m) )
    return FALSE;
if ( not_zero(ds[i].s - s) )
    return FALSE;
if ( not_zero(ds[i].d - d) )
    return FALSE;
if ( not_zero(ds[i].a - a) )
    return FALSE;

return TRUE;
}

/**********************************************************************
*                     CONVERT_ASENSOR_DB_TO_NEW_CSV                   *
**********************************************************************/
void convert_asensor_db_to_new_csv( NAME_CLASS & sorc )
{
const double MaxRawCount = 4095.0;

DB_TABLE  t;
double high_value;
double low_value;
double a;
double m;
double s;
double d;
double v;
int    gain;
int    i;
int    number;
int    n;
ANALOG_SENSOR as;

if ( as.count() == 0 )
    as.get();

if ( t.open(sorc,ASENSOR_RECLEN, PFL) )
    {
    n = (int) t.nof_recs();
    if ( n > as.count() )
        as.setsize( n );
    while ( t.get_next_record(FALSE) )
        {
        number     = (int) t.get_long( ASENSOR_NUMBER_OFFSET );
        high_value = t.get_double( ASENSOR_HIGH_VALUE_OFFSET );
        low_value  = t.get_double( ASENSOR_LOW_VALUE_OFFSET );
        gain       = (int) t.get_long( ASENSOR_GAIN_OFFSET );
        if ( gain == 2 )
            v = 5.0;
        else
            v = 10.0;
        m          = t.get_double( ASENSOR_MUL_OFFSET );
        s          = t.get_double( ASENSOR_SUB_OFFSET );
        d          = t.get_double( ASENSOR_DIV_OFFSET );
        a          = t.get_double( ASENSOR_ADD_OFFSET );
        if ( !is_default_sensor(number, high_value, low_value, m, s, d, a) )
            {
            if ( !is_zero(m) )
                {
                /*
                -----------------------------------------------------------------------
                If one of the standard sensors has been redefined then I need to create
                a new sensor and move it to there. Later I need to change the
                ftchan.txt file in each part.
                ----------------------------------------------------------------------- */
                if ( number >= 23 && number <= 25 )
                    {
                    HaveNewAnalogSensor = true;
                    n++;
                    as.setsize(n);
                    i = number - 23;
                    number = n - 1;
                    NewAnalogSensorNumber[i] = number;
                    }
                as[number].low_volts = v * (s + d * (low_value - a)/m)/MaxRawCount;
                as[number].high_volts = v * (s + d * (high_value - a)/m)/MaxRawCount;
                as[number].units      = (int) t.get_long( ASENSOR_UNITS_OFFSET );
                as[number].vartype    = (unsigned) t.get_long( ASENSOR_VARIABLE_TYPE_OFFSET );
                t.get_alpha( as[number].desc, ASENSOR_DESC_OFFSET, ASENSOR_DESC_LEN );
                as[number].low_value  = low_value;
                as[number].high_value = high_value;
                as[number].update();
                }
            }
        }

    t.close();
    as.put();
    }
}

/***********************************************************************
*                  GET_OLD_LIMIT_SWITCH_DESCRIPTIONS                   *
* The old system had machine based limit switch descriptions. Suretrak1*
* systems had the descriptions in the computer dir but it was assumed  *
* that only one machine existed. FT1 systems had none.                 *
*                                                                      *
* It is assumed that the MACHINE_CLASS m has loaded the machine in     *
* question.                                                            *
***********************************************************************/
static BOOLEAN get_old_limit_switch_descriptions( LIMIT_SWITCH_DESC_CLASS & desc, MACHINE_CLASS & m )
{
static TCHAR LsString[] = TEXT( "LS" );
static TCHAR Description[] = TEXT( "Description" );

BOOLEAN    have_labels;
FILE_CLASS f;
int        i;
INI_CLASS  ini;
NAME_CLASS s;

/*
-------------------
Make generic labels
------------------- */
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    s = LsString;
    s += SpaceChar;
    s += i+1;
    desc[i] = s;
    }

have_labels = FALSE;
if ( m.is_ftii )
    {
    s.get_ft2_editor_settings_file_name( m.computer, m.name );
    if ( s.file_exists() )
        {
        ini.set_file( s.text() );
        ini.set_section( ConfigSection );
        for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
            {
            s = LsString;
            s += i+1;
            s += Description;
            if ( ini.find(s.text()) )
                {
                s = ini.get_string();
                if ( !s.isempty() )
                    {
                    desc[i] = s;
                    have_labels = TRUE;
                    }
                }
            }
        }
    }

if ( !have_labels )
    {
    s.get_local_ini_file_name( TEXT("visitrak.ini") );
    ini.set_file( s.text() );
    if ( ini.get_boolean(TEXT("Config"), TEXT("HaveSureTrak")) )
        {
        /*
        ----------------------------------------------------------------------------------------------
        This is a suretrak 1 control. There are only four limit switches. The names are
        in the stparam.dat file. This might be in the machine dir but is probably in the computer dir.
        ---------------------------------------------------------------------------------------------- */
        s.get_stparam_file_name( m.computer, m.name );
        if ( !s.file_exists() )
            s.get_stparam_file_name( m.computer );
        if ( s.file_exists() )
            {
            if ( f.open_for_read(s) )
                {
                for ( i=0; i<11; i++ )
                    f.readline();
                for ( i=0; i<MAX_ST_LIMIT_SWITCHES; i++ )
                    {
                    s = f.readline();
                    if ( !s.isempty() )
                        desc[i] = s;
                    }
                f.close();
                }
            }
        }
    }

return TRUE;
}

/***********************************************************************
*                    GET_OLD_LIMIT_SWITCH_POSITIONS                    *
* It is assumed that the machine class has been loaded and that the    *
* four position values from that old part table have been put into     *
* the first four entries in pos.                                       *
***********************************************************************/
static BOOLEAN get_old_limit_switch_positions( LIMIT_SWITCH_POS_CLASS & pos, MACHINE_CLASS & m, STRING_CLASS & p )
{
const TCHAR temp_file_name[] = TEXT( "KILLME.DAT" );

TCHAR     * cp;
int         i;
int         n;
BOOLEAN     status;
FILE_CLASS  f;
FIELDOFFSET fo;
NAME_CLASS  s;
DB_TABLE    t;

status = FALSE;
if ( m.is_ftii && m.suretrak_controlled )
    {
    /*
    ------------------------------------------------------------------------------------
    The four positions from the old part.txt file are already in the first four strings.
    If this is a suretrak2 system these need to be moved up (only the first two are
    valid) and the first four values read from the old stlimits file.
    ------------------------------------------------------------------------------------ */
    pos[4] = pos[0];
    pos[5] = pos[1];
    s.get_stlimits_datname( m.computer, m.name, p );
    if ( s.file_exists() )
        {
        if ( f.open_for_read(s) )
            {
            n = f.nof_lines();
            if ( n >= 12 )
                {
                f.rewind();
                for ( i=0; i<OLD_MAX_ST_LIMIT_SWITCHES; i++ )
                    {
                    cp = f.readline();
                    if ( cp )
                        pos[i] = cp;
                    }
                status = TRUE;
                }
            f.close();
            }
        }
    else
        {
        s.get_stlimits_dbname( m.computer, m.name, p );
        if ( s.file_exists() )
            {
            if ( t.open(s, STLIMITS_RECLEN, PFL) )
                {
                fo = 0;
                if ( t.get_next_record(NO_LOCK) )
                    {
                    for ( i=0; i<OLD_MAX_ST_LIMIT_SWITCHES; i++ )
                        {
                        t.get_alpha( pos[i], fo, STLIMITS_FLOAT_LEN );
                        fo += STLIMITS_FLOAT_LEN + ALPHA_ADDED_CHARS;
                        }
                    status = TRUE;
                    }
                t.close();
                }
            }
        }

    if ( !status )
        {
        /*
        ---------------------------------------------------------
        If neither stlimits file exists, just keep the old limits
        --------------------------------------------------------- */
        pos[4].null();
        pos[5].null();
        }
    }

return status;
}

/***********************************************************************
*                     CREATE_STSETUP_SETPOINT_FILE                     *
* This creates a part based stsetup.csv file using the old             *
***********************************************************************/
static void create_stsetup_setpoint_file( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa, MACHINE_CLASS & m )
{
int        i;
FILE_CLASS f;
INI_CLASS  ini;
NAME_CLASS s;
SURETRAK_SETUP_CLASS sc;

/*
---------------
Read the steps.
--------------- */
sc.find( co, ma, pa );

s.get_stlimits_datname( co, ma, pa );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
        {
        f.rewind();
        for ( i=0; i<OLD_MAX_ST_LIMIT_SWITCHES; i++ )
            f.readline();

        for ( i=0; i<NOF_GAIN_BREAKS; i++ )
            {
            s = f.readline();
            sc.set_gain_break_velocity( i, s );
            }

        for ( i=0; i<NOF_GAIN_BREAKS; i++ )
            {
            s = f.readline();
            sc.set_gain_break_gain( i, s );
            }

        s = f.readline();
        sc.set_velocity_loop_gain( s );

        s = f.readline();
        sc.set_ft_stop_pos( s );
        f.close();
        }
    }

if ( m.is_ftii )
    {
    s.get_ft2_editor_settings_file_name( co, ma );
    if ( s.file_exists() )
        {
        ini.set_file( s.text() );
        if ( ini.find(ConfigSection, VacuumLimitSwitchKey) )
            {
            s = ini.get_string();
            /*
            ---------------------------------------------------
            In v6 the vacuum limit switch was saved as an index
            --------------------------------------------------- */
            i = s.int_value();
            if ( i >=0 && i<MAX_FTII_LIMIT_SWITCHES )
                {
                i++;
                s = i;
                }

            sc.set_vacuum_limit_switch_number( s );
            }
        }
    }
else
    {
    /*
    ------------------------------------------------------------
    The vacuum limit switch used to be kept in the stparam table
    ------------------------------------------------------------ */
    s.get_stparam_file_name( co, ma );
    if ( !s.file_exists() )
        s.get_stparam_file_name( co );
    if ( s.file_exists() )
        {
        if ( f.open_for_read(s) )
            {
            for ( i=0; i<15; i++ )
                f.readline();
            s = f.readline();
            sc.set_vacuum_limit_switch_number( s );
            f.close();
            }
        }
    }

sc.save( co, ma, pa );
}

/***********************************************************************
*                     CONVERT_DOWNTIME_FROM_5_TO_7                     *
* S is the name of the source db, which is assumed to be a version     *
* 5 db. This will be converted to a version 7 db, which involves       *
* increasing the size of the part name from 8 to 50                    *
***********************************************************************/
BOOLEAN convert_downtime_from_5_to_7( NAME_CLASS & s )
{
STRING_CLASS buf;
NAME_CLASS   d;
SYSTEMTIME   st;
DB_TABLE     t;
DB_TABLE     td;

d = s;
d.replace( DOWNTIME_DB, TEMP_DOWNTIME_DB );

if ( s.file_exists() )
    {
    if ( t.open(s, OLD_DOWNTIME_RECLEN, PFL) )
        {
        td.ensure_existance(d);
        if ( td.open(d, DOWNTIME_RECLEN, FL) )
            {
            td.empty();
            while ( t.get_next_record(NO_LOCK) )
                {
                t.get_date( st, DOWNTIME_DATE_OFFSET );
                t.get_time( st, DOWNTIME_TIME_OFFSET );
                td.put_date( DOWNTIME_DATE_OFFSET, st );
                td.put_time( DOWNTIME_TIME_OFFSET, st );

                t.get_alpha( buf, DOWNTIME_CAT_OFFSET, DOWNCAT_NUMBER_LEN );
                td.put_alpha( DOWNTIME_CAT_OFFSET, buf, DOWNCAT_NUMBER_LEN );

                t.get_alpha( buf, DOWNTIME_SUBCAT_OFFSET, DOWNCAT_NUMBER_LEN );
                td.put_alpha( DOWNTIME_SUBCAT_OFFSET, buf, DOWNCAT_NUMBER_LEN );

                t.get_alpha( buf, DOWNTIME_OPERATOR_OFFSET, OPERATOR_NUMBER_LEN );
                td.put_alpha( DOWNTIME_OPERATOR_OFFSET, buf, OPERATOR_NUMBER_LEN );

                t.get_alpha( buf, DOWNTIME_PART_NAME_OFFSET, OLD_PART_NAME_LEN );
                td.put_alpha( DOWNTIME_PART_NAME_OFFSET, buf, DOWNTIME_PART_NAME_LEN );
                td.rec_append();
                }
            td.close();
            }
        t.close();
        }
    s.delete_file();
    d.moveto( s );
    }

return TRUE;
}

/***********************************************************************
*                      CONVERT_RUNLIST_FROM_5_TO_7                     *
*   s is the name of the source db ( runlist.txt ).                    *
*   This will be converted to a version 7 csv file (runlist.csv)       *
***********************************************************************/
BOOLEAN convert_runlist_from_5_to_7( NAME_CLASS & s )
{
NAME_CLASS d;
FILE_CLASS f;
DB_TABLE   t;
STRING_CLASS buf;
STRING_CLASS part;

d = s;
d.replace( RUNLIST_DB, RUNLIST_CSV );

if ( s.file_exists() )
    {
    if ( f.open_for_write(d.text()) )
        {
        if ( t.open(s, RUNLIST_RECLEN, PFL) )
            {
            while ( t.get_next_record(NO_LOCK) )
                {
                t.get_alpha( buf, RUNLIST_BITS_OFFSET, MAX_HEX_LEN );
                t.get_alpha( part, RUNLIST_PART_NAME_OFFSET, OLD_PART_NAME_LEN   );
                buf += CommaChar;
                buf += part;
                f.writeline( buf.text() );
                }
            t.close();
            }
        f.close();
        }
    s.delete_file();
    }

return TRUE;
}

/***********************************************************************
*                   EXTRACT_SETPOINTS_FROM_PARTS_DB                    *
***********************************************************************/
BOOLEAN extract_setpoints_from_parts_db( SETPOINT_CLASS & sp, LIMIT_SWITCH_POS_CLASS & sw, DB_TABLE & t )
{
int i;

if ( sp.count() != PARTSET_SETPOINT_COUNT )
    sp.setsize( PARTSET_SETPOINT_COUNT );

for ( i=0; i<NofPartDescriptions; i++ )
    sp[i].desc  = PartDescription[i];

sp[PARTSET_DISTANCE_UNITS_INDEX].value       = (int) t.get_short( PARTS_DISTANCE_UNITS_OFFSET );
sp[PARTSET_VELOCITY_UNITS_INDEX].value       = (int) t.get_short( PARTS_VELOCITY_UNITS_OFFSET );
sp[PARTSET_PRESSURE_UNITS_INDEX].value       = (int) t.get_short( PARTS_PRESSURE_UNITS_OFFSET );
sp[PARTSET_TOTAL_STROKE_LEN_INDEX].value     = t.get_double( PARTS_TOTAL_STROKE_LEN_OFFSET );
sp[PARTSET_MIN_STROKE_LEN_INDEX].value       = t.get_double( PARTS_MIN_STROKE_LEN_OFFSET );
sp[PARTSET_BISCUIT_TIME_DELAY_INDEX].value   = (int) t.get_long( PARTS_BISCUIT_TIME_DELAY_OFFSET );
sp[PARTSET_EOS_VEL_INDEX].value              = t.get_double( PARTS_EOS_VEL_OFFSET );
sp[PARTSET_SLEEVE_FILL_DIST_INDEX].value     = t.get_double( PARTS_SLEEVE_FILL_DIST_OFFSET );
sp[PARTSET_RUNNER_FILL_DIST_INDEX].value     = t.get_double( PARTS_RUNNER_FILL_DIST_OFFSET );
sp[PARTSET_MIN_CSFS_VEL_INDEX].value         = t.get_double( PARTS_MIN_CSFS_VEL_OFFSET );
sp[PARTSET_RISE_CSFS_VEL_INDEX].value        = t.get_double( PARTS_RISE_CSFS_VEL_OFFSET );
sp[PARTSET_MIN_CSFS_POS_INDEX].value         = t.get_double( PARTS_MIN_CSFS_POS_OFFSET );
sp[PARTSET_USER_VEL_1_INDEX].value           = t.get_double( PARTS_USER_VEL_1_OFFSET );
sp[PARTSET_USER_VEL_2_INDEX].value           = t.get_double( PARTS_USER_VEL_2_OFFSET );
sp[PARTSET_USER_VEL_3_INDEX].value           = t.get_double( PARTS_USER_VEL_3_OFFSET );
sp[PARTSET_USER_VEL_4_INDEX].value           = t.get_double( PARTS_USER_VEL_4_OFFSET );
sp[PARTSET_AVG_START_1_INDEX].value          = t.get_double( PARTS_AVG_START_1_OFFSET );
sp[PARTSET_AVG_START_2_INDEX].value          = t.get_double( PARTS_AVG_START_2_OFFSET );
sp[PARTSET_AVG_START_3_INDEX].value          = t.get_double( PARTS_AVG_START_3_OFFSET );
sp[PARTSET_AVG_START_4_INDEX].value          = t.get_double( PARTS_AVG_START_4_OFFSET );
sp[PARTSET_AVG_END_1_INDEX].value            = t.get_double( PARTS_AVG_END_1_OFFSET );
sp[PARTSET_AVG_END_2_INDEX].value            = t.get_double( PARTS_AVG_END_2_OFFSET );
sp[PARTSET_AVG_END_3_INDEX].value            = t.get_double( PARTS_AVG_END_3_OFFSET );
sp[PARTSET_AVG_END_4_INDEX].value            = t.get_double( PARTS_AVG_END_4_OFFSET );
sp[PARTSET_PLUNGER_DIA_INDEX].value          = t.get_double( PARTS_PLUNGER_DIA_OFFSET );
sp[PARTSET_TIME_FOR_INTENS_PRES_INDEX].value = (int) t.get_long( PARTS_TIME_FOR_INTENS_PRES_OFFSET );
sp[PARTSET_PRES_FOR_RESP_TIME_INDEX].value   = t.get_double( PARTS_PRES_FOR_RESP_TIME_OFFSET );
sp[PARTSET_MS_PER_TIME_SAMPLE_INDEX].value   = (int) t.get_short( PARTS_MS_PER_TIME_SAMPLE_OFFSET );
sp[PARTSET_N_OF_TIME_SAMPLES_INDEX].value    = (int) t.get_short( PARTS_N_OF_TIME_SAMPLES_OFFSET );
sp[PARTSET_PRODUCTION_ENABLED_INDEX].value   = (int) t.get_boolean( PARTS_PRODUCTION_ENABLED );
sp[PARTSET_HEAD_CHANNEL_INDEX].value         = (int) t.get_short( PARTS_HEAD_CHANNEL_OFFSET );
sp[PARTSET_ROD_CHANNEL_INDEX].value          = (int) t.get_short( PARTS_ROD_CHANNEL_OFFSET );

/*
---------------------------------------------------------------
The limit switch positions are no longer kept in the part file.
--------------------------------------------------------------- */
sw[0] = t.get_double( PARTS_LIMIT_SW_1_OFFSET );
sw[1] = t.get_double( PARTS_LIMIT_SW_2_OFFSET );
sw[2] = t.get_double( PARTS_LIMIT_SW_3_OFFSET );
sw[3] = t.get_double( PARTS_LIMIT_SW_4_OFFSET );

return TRUE;
}

/***********************************************************************
*                  EXTRACT_SETPOINTS_FROM_MACHSET_DB                   *
* t contains a record from a machset db. sp is a SETPOINT_CLASS with   *
* size set to sp.setsize( MACHSET_SETPOINT_COUNT );                    *
***********************************************************************/
BOOLEAN extract_setpoints_from_machset_db( SETPOINT_CLASS & sp, DB_TABLE & t )
{
if ( sp.count() != MACHSET_SETPOINT_COUNT )
    sp.setsize( MACHSET_SETPOINT_COUNT );

sp[MACHSET_MACHINE_TYPE_INDEX].value = (unsigned) t.get_long( MACHSET_MACHINE_TYPE_OFFSET );
sp[MACHSET_MACHINE_TYPE_INDEX].desc  = MachineDescription[ MACHSET_MACHINE_TYPE_INDEX];

sp[MACHSET_FT_BOARD_NO_INDEX].value = (int) t.get_short( MACHSET_FT_BOARD_NO_OFFSET );
sp[MACHSET_FT_BOARD_NO_INDEX].desc  = MachineDescription[ MACHSET_FT_BOARD_NO_INDEX];

sp[MACHSET_MUX_CHAN_INDEX].value = (int) t.get_short( MACHSET_MUX_CHAN_OFFSET );
sp[MACHSET_MUX_CHAN_INDEX].desc  = MachineDescription[ MACHSET_MUX_CHAN_INDEX];

sp[MACHSET_ST_BOARD_INDEX].value = t.get_boolean( MACHSET_ST_BOARD_OFFSET );
sp[MACHSET_ST_BOARD_INDEX].desc  = MachineDescription[ MACHSET_ST_BOARD_INDEX];

t.get_alpha( sp[MACHSET_CURRENT_PART_INDEX].value, MACHSET_CURRENT_PART_OFFSET,  OLD_PART_NAME_LEN );
sp[MACHSET_CURRENT_PART_INDEX].desc  = MachineDescription[ MACHSET_CURRENT_PART_INDEX];

t.get_alpha( sp[MACHSET_RODPITCH_INDEX].value, MACHSET_RODPITCH_OFFSET, RODPITCH_NAME_LEN );
sp[MACHSET_RODPITCH_INDEX].desc  = MachineDescription[ MACHSET_RODPITCH_INDEX];

sp[MACHSET_PROFILE_SKIP_COUNT_INDEX].value = (int) t.get_long( MACHSET_PROFILE_SKIP_COUNT_OFFSET );
sp[MACHSET_PROFILE_SKIP_COUNT_INDEX].desc  = MachineDescription[ MACHSET_PROFILE_SKIP_COUNT_INDEX];

sp[MACHSET_PROFILE_ACQUIRE_COUNT_INDEX].value = (int) t.get_long( MACHSET_PROFILE_ACQUIRE_COUNT_OFFSET );
sp[MACHSET_PROFILE_ACQUIRE_COUNT_INDEX].desc  = MachineDescription[ MACHSET_PROFILE_ACQUIRE_COUNT_INDEX];

sp[MACHSET_PROFILE_TIME_SKIP_INDEX].value = (int) t.get_long( MACHSET_PROFILE_TIME_SKIP_OFFSET );
sp[MACHSET_PROFILE_TIME_SKIP_INDEX].desc  = MachineDescription[ MACHSET_PROFILE_TIME_SKIP_INDEX];

sp[MACHSET_PROFILE_FLAGS_INDEX].value = (unsigned) t.get_long( MACHSET_PROFILE_FLAGS_OFFSET );
sp[MACHSET_PROFILE_FLAGS_INDEX].desc  = MachineDescription[ MACHSET_PROFILE_FLAGS_INDEX];

sp[MACHSET_PROFILE_MAX_SAVED_INDEX].value = (int) t.get_long( MACHSET_PROFILE_MAX_SAVED_OFFSET );
sp[MACHSET_PROFILE_MAX_SAVED_INDEX].desc  = MachineDescription[ MACHSET_PROFILE_MAX_SAVED_INDEX];

sp[MACHSET_PARAM_SKIP_COUNT_INDEX].value = (int) t.get_long( MACHSET_PARAM_SKIP_COUNT_OFFSET );
sp[MACHSET_PARAM_SKIP_COUNT_INDEX].desc  = MachineDescription[ MACHSET_PARAM_SKIP_COUNT_INDEX];

sp[MACHSET_PARAM_ACQUIRE_COUNT_INDEX].value = (int) t.get_long( MACHSET_PARAM_ACQUIRE_COUNT_OFFSET );
sp[MACHSET_PARAM_ACQUIRE_COUNT_INDEX].desc  = MachineDescription[ MACHSET_PARAM_ACQUIRE_COUNT_INDEX];

sp[MACHSET_PARAM_TIME_SKIP_INDEX].value = (int) t.get_long( MACHSET_PARAM_TIME_SKIP_OFFSET );
sp[MACHSET_PARAM_TIME_SKIP_INDEX].desc  = MachineDescription[ MACHSET_PARAM_TIME_SKIP_INDEX];

sp[MACHSET_PARAM_FLAGS_INDEX].value = (unsigned) t.get_long( MACHSET_PARAM_FLAGS_OFFSET );
sp[MACHSET_PARAM_FLAGS_INDEX].desc  = MachineDescription[ MACHSET_PARAM_FLAGS_INDEX];

sp[MACHSET_PARAM_MAX_SAVED_INDEX].value = (int) t.get_long( MACHSET_PARAM_MAX_SAVED_OFFSET );
sp[MACHSET_PARAM_MAX_SAVED_INDEX].desc  = MachineDescription[ MACHSET_PARAM_MAX_SAVED_INDEX];

sp[MACHSET_MONITOR_FLAGS_INDEX].value = (unsigned) t.get_long( MACHSET_MONITOR_FLAGS_OFFSET );
sp[MACHSET_MONITOR_FLAGS_INDEX].desc  = MachineDescription[ MACHSET_MONITOR_FLAGS_INDEX];

sp[MACHSET_AUTO_SHOT_TIMEOUT_INDEX].value = (int) t.get_short( MACHSET_AUTO_SHOT_TIMEOUT_OFFSET );
sp[MACHSET_AUTO_SHOT_TIMEOUT_INDEX].desc  = MachineDescription[ MACHSET_AUTO_SHOT_TIMEOUT_INDEX];

sp[MACHSET_CYCLE_TIMEOUT_INDEX].value = (int) t.get_short( MACHSET_CYCLE_TIMEOUT_OFFSET );
sp[MACHSET_CYCLE_TIMEOUT_INDEX].desc  = MachineDescription[ MACHSET_CYCLE_TIMEOUT_INDEX];

sp[MACHSET_DIFF_CURVE_NUMBER_INDEX].value = (int) t.get_short( MACHSET_DIFF_CURVE_NUMBER_OFFSET );
sp[MACHSET_DIFF_CURVE_NUMBER_INDEX].desc  = MachineDescription[ MACHSET_DIFF_CURVE_NUMBER_INDEX];

sp[MACHSET_POSITION_IMPACT_PTS_INDEX].value = (int) t.get_short( MACHSET_POSITION_IMPACT_PTS_OFFSET );
sp[MACHSET_POSITION_IMPACT_PTS_INDEX].desc  = MachineDescription[ MACHSET_POSITION_IMPACT_PTS_INDEX];

sp[MACHSET_TIME_IMPACT_PTS_INDEX].value = (int) t.get_short( MACHSET_TIME_IMPACT_PTS_OFFSET );
sp[MACHSET_TIME_IMPACT_PTS_INDEX].desc  = MachineDescription[ MACHSET_TIME_IMPACT_PTS_INDEX];

sp[MACHSET_PRE_PRES_TYPE_INDEX].value = (unsigned) t.get_long( MACHSET_PRE_PRES_TYPE_OFFSET );
sp[MACHSET_PRE_PRES_TYPE_INDEX].desc  = MachineDescription[ MACHSET_PRE_PRES_TYPE_INDEX];

sp[MACHSET_POST_PRES_TYPE_INDEX].value = (unsigned) t.get_long( MACHSET_POST_PRES_TYPE_OFFSET );
sp[MACHSET_POST_PRES_TYPE_INDEX].desc  = MachineDescription[ MACHSET_POST_PRES_TYPE_INDEX];

t.get_alpha( sp[MACHSET_OPERATOR_NUMBER_INDEX].value, MACHSET_OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
if ( !sp[MACHSET_OPERATOR_NUMBER_INDEX].value.isempty() )
    trim( sp[MACHSET_OPERATOR_NUMBER_INDEX].value.text() );
sp[MACHSET_OPERATOR_NUMBER_INDEX].desc  = MachineDescription[ MACHSET_OPERATOR_NUMBER_INDEX];

return TRUE;
}

/***********************************************************************
*                      CONVERT_V5_DATA_DIR_TO_V7                       *
*                                                                      *
* This assumes that the machset.txt file is in the data directory as   *
* are all of the machine directories. Sorcdatadir is the data          *
* directory.                                                           *
* It isn't the standard data directory, just the one set up for this   *
* purpose such as c:\v5\backup\data.                                   *
***********************************************************************/
BOOLEAN convert_v5_data_dir_to_v7( STRING_CLASS & sorcdatadir )
{
NAME_CLASS              computer;
NAME_CLASS              d;
NAME_CLASS              datadir;
NAME_CLASS              s;
NAME_CLASS              s1;
MACHINE_CLASS           m;
SETPOINT_CLASS          sp;
DB_TABLE                t;
PLOOKUP_CLASS           p;
TEXT_LIST_CLASS         mlist;
TEXT_LIST_CLASS         plist;
STRING_CLASS            machine;
STRING_CLASS            part;
LIMIT_SWITCH_DESC_CLASS desc;
LIMIT_SWITCH_POS_CLASS  pos;
TIME_CLASS              mytime;
WARMUP_CLASS            wc;
int                     analog_sensor_number;
int                     i;

datadir = sorcdatadir;
datadir.remove_ending_backslash();

/*
--------------------------
Convert to parent dir name
-------------------------- */
computer = datadir;
if ( !computer.remove_ending_filename() )
    return FALSE;

/*
-------------------------------------------
The machset db is now in the data directory
------------------------------------------- */
s = datadir;
s.cat_path( MACHSET_DB );
if ( t.open(s, MACHSET_RECLEN, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        extract_setpoints_from_machset_db( sp, t );
        t.get_alpha( machine, MACHSET_MACHINE_NAME_OFFSET, OLD_MACHINE_NAME_LEN );
        d = datadir;
        d.cat_path( machine );
        d.cat_path( MACHSET_CSV );
        sp.put( d );

        /*
        -----------------------
        Keep a list of machines
        ----------------------- */
        mlist.append( machine.text() );
        }
    t.close();
    }

/*
------------------------------------------------------
I've copied all the information from the machset file.
------------------------------------------------------ */
s.delete_file();

mlist.rewind();
while ( mlist.next() )
    {
    machine = mlist.text();
    m.find( computer, machine );
    s = datadir;
    s.cat_path( machine );
    s.cat_path( PARTS_DB );
    plist.empty();
    if ( t.open(s, PARTS_RECLEN, PFL) )
        {
        while ( t.get_next_record(NO_LOCK) )
            {
            extract_setpoints_from_parts_db( sp, pos, t );
            t.get_alpha( part, PARTS_PART_NAME_OFFSET, OLD_PART_NAME_LEN );
            d = datadir;
            d.cat_path( machine );
            d.cat_path( part );
            d.cat_path( PARTSET_CSV );
            sp.put( d );
            get_old_limit_switch_descriptions( desc, m );
            get_old_limit_switch_positions( pos, m, part );
            s1.get_part_dir_file_name( computer, machine, part, OLD_FTII_LIMIT_SWITCH_WIRES_FILE );
            if ( s1.file_exists() )
                {
                d.get_limit_switch_wires_file_name( computer, machine, part );
                s1.moveto( d );
                }
            create_stsetup_setpoint_file( computer, machine, part, m );
            desc.put( computer, machine, part );
            pos.put( computer, machine, part );
            plist.append( part.text() );
            }
        t.close();
        }
    s.delete_file();

    /*
    ------
    Warmup
    ------ */
    plist.rewind();
    while ( plist.next() )
        {
        part = plist.text();
        s = datadir;
        s.cat_path( machine );
        s.cat_path( part );
        s.cat_path( WARMUP_DB );
        if ( s.file_exists() )
            {
            if ( t.open(s, WARMUP_RECLEN, PFL) )
                {
                if ( t.get_next_record(NO_LOCK) )
                    {
                    wc.is_enabled = t.get_boolean( WARMUP_ENABLED_OFFSET );
                    wc.run_if_on  = t.get_boolean( WARMUP_RUN_IF_ON_OFFSET );
                    t.get_alpha( wc.wire, WARMUP_WIRE_OFFSET, WARMUP_WIRE_LEN );
                    t.get_alpha( wc.partname, WARMUP_PART_NAME_OFFSET, OLD_PART_NAME_LEN );
                    wc.put( computer, machine, part );
                    }
                t.close();
                }
            s.delete_file();
            }
        /*
        --------------------------------------------------------------------------------
        If this part was using a redefined sensor in [23, 25] I need to change it to use
        the new sensor (which is the same as the old but a different number).
        -------------------------------------------------------------------------------- */
        if ( HaveNewAnalogSensor )
            {
            s = datadir;
            s.cat_path( machine );
            s.cat_path( part );
            s.cat_path( FTCHAN_DB );
            if ( s.file_exists() )
                {
                if ( t.open(s, FTCHAN_RECLEN, FL) )
                    {
                    while ( t.get_next_record(NO_LOCK) )
                        {
                        analog_sensor_number = (int) t.get_long( FTCHAN_SENSOR_NUMBER_OFFSET );
                        if ( analog_sensor_number >= 23 && analog_sensor_number <= 25 )
                            {
                            i = analog_sensor_number - 23;
                            analog_sensor_number = NewAnalogSensorNumber[i];
                            t.put_long( FTCHAN_SENSOR_NUMBER_OFFSET, (long) analog_sensor_number, SENSOR_NUMBER_LEN );
                            t.rec_update();
                            }
                        }
                    t.close();
                    }
                }
            }
        }
    }

mlist.rewind();
while ( mlist.next() )
    {
    /*
    -------
    Plookup
    ------- */
    machine = mlist.text();
    s = datadir;
    s.cat_path( machine );
    s.cat_path( PLOOKUP_DB );
    if ( t.open(s, PLOOKUP_RECLEN, PFL) )
        {
        while ( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( part, PLOOKUP_PART_NAME_OFFSET, OLD_PART_NAME_LEN );
            p.init( computer, machine, part );
            i = (int) t.get_long(PLOOKUP_LAST_SHOT_OFFSET );
            s1 = i;
            p.set_last_shot_name( s1 );
            get_time_class_from_db( mytime, t, PLOOKUP_DATE_OFFSET );
            p.set_time( mytime );
            p.put();
            }
        t.close();
        }
    s.delete_file();

    /*
    -----
    Dstat
    ----- */
    s = datadir;
    s.cat_path( machine );
    s.cat_path( DSTAT_DB );
    t.ensure_existance( s );

    /*
    -------
    Runlist
    ------- */
    s = datadir;
    s.cat_path( machine );
    s.cat_path( RUNLIST_DB );
    convert_runlist_from_5_to_7( s );

    /*
    --------
    Downtime
    -------- */
    s = datadir;
    s.cat_path( machine );
    s.cat_path( ResultsString );
    s.cat_path( DOWNTIME_DB );
    convert_downtime_from_5_to_7( s );
    }

return TRUE;
}

/***********************************************************************
*                                V5TOV7                                *
* This program converts a v5 directory to a v7. It moves the machset   *
* table to the machine directories, the parts and plookup tables to    *
* the part directories, converts the runlist and warmup to csv files,  *
* and changes the part name field length of any downtime tables from   *
* 8 to 50. The "computer" argument is the v5 directory to be converted.*
***********************************************************************/
BOOLEAN v5tov7( STRING_CLASS & computer, BOOLEAN need_to_convert_asensor_db )
{
int                     i;
FILE_CLASS              f;
FILE_CLASS              fd;
INI_CLASS               ini;
MACHINE_CLASS           m;
SETPOINT_CLASS          sp;
DB_TABLE                t;
DB_TABLE                td;
PLOOKUP_CLASS           p;
TEXT_LIST_CLASS         plist;
TEXT_LIST_CLASS         tlist;
NAME_CLASS              d;
NAME_CLASS              s;
NAME_CLASS              s1;
STRING_CLASS            machine;
STRING_CLASS            part;
STRING_CLASS            buf;
TCHAR                 * cp;

V5Directory = computer;
DataDirectory = computer;
DataDirectory.cat_path( DataString );

/*
------------
Visitrak.ini
------------ */
s = computer;
s.cat_path( ExesString );
s.cat_path( VisitrakIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
ini.remove_key( RootDirKey );
ini.put_string( ThisComputerKey, computer.text() );

/*
--------------------------------------------------------------------------------
I need to know if this was a suretrak install in case it is a SureTrak 1 Control
which will not have the suretrak_controlled set in the machine setup.
SureTrak 1 controls only had one machine so the global setting was enough.
-------------------------------------------------------------------------------- */
HaveSureTrak = ini.get_boolean( HaveSureTrakKey );

/*
------------
Computer.csv
------------ */
d = DataDirectory;
d.cat_path( TEMP_COMPUTER_DB );
if ( d.file_exists() )
    d.delete_file();
/*
-------------------------------------------------------------------------
Use i to keep track of the number of lines so I know I wrote at least one
------------------------------------------------------------------------- */
i = 0;
s = DataDirectory;
s.cat_path( COMPUTER_CSV );
if ( s.file_exists() )
    {
    if (f.open_for_read(s.text()) )
        {
        fd.open_for_write( d.text() );
        while( true )
            {
            buf = f.readline();
            if ( buf.isempty() )
                break;
            cp = buf.find( DataString );
            if ( cp )
                {
                if ( cp > buf.text() )
                    {
                    cp--;
                    *cp = NullChar;
                    cp = buf.find( CommaChar );
                    if ( cp )
                        {
                        cp++;
                        fd.writeline( cp );
                        i++;
                        }
                    }
                }
            }
        fd.close();
        f.close();
        }
    s.delete_file();
    }
else
    {
    s = DataDirectory;
    s.cat_path( COMPUTER_DB );
    if ( s.file_exists() )
        {
        if ( t.open(s, COMPUTER_RECLEN, PFL) )
            {
            fd.open_for_write( d.text() );
            while ( t.get_next_record(NO_LOCK) )
                {
                t.get_alpha( buf, COMPUTER_DIR_OFFSET, OLD_COMPUTER_DIR_LEN );
                cp = buf.find( DataString );
                if ( cp )
                    {
                    if ( cp > buf.text() )
                        {
                        cp--;
                        *cp = NullChar;
                        fd.writeline( buf.text() );
                        i++;
                        }
                    }
                }
            fd.close();
            t.close();
            }
        s.delete_file();
        }
    }

/*
-------------------------------------
If I created an empty file, delete it
------------------------------------- */
if ( i == 0 )
    {
    if ( d.file_exists() )
        d.delete_file();
    }

/*
------------------------------------
If there is no file, make one for me
------------------------------------ */
if ( !d.file_exists() )
    {
    f.open_for_write( d.text() );
    f.writeline( computer.text() );
    f.close();
    }

s = DataDirectory;
s.cat_path( COMPUTER_CSV );
d.moveto( s );

s = DataDirectory;
s.cat_path( COMPUTER_DB );
if ( s.file_exists() )
    s.delete_file();

/*
-----------------------------------------------------
Convert the old analog sensor table to the new format
----------------------------------------------------- */
if ( need_to_convert_asensor_db )
    {
    s = DataDirectory;
    s.cat_path( ASENSOR_DB );
    if ( s.file_exists() )
        convert_asensor_db_to_new_csv( s );
    }

/*
----------------------------------------------------------
There should only be one computer directory, usually "C01"
---------------------------------------------------------- */
tlist.get_directory_list( DataDirectory.text() );
if ( tlist.count() > 1 )
    return FALSE;

tlist.rewind();
tlist.next();

ComputerDirectory = DataDirectory;
ComputerDirectory.cat_path( tlist.text() );

/*
--------------------------------------------------------------------
Move all the files from the computer directory to the data directory
-------------------------------------------------------------------- */
tlist.empty();
tlist.get_file_list( ComputerDirectory.text(), StarDotStar );
tlist.rewind();
while ( tlist.next() )
    {
    s = ComputerDirectory;
    s.cat_path( tlist.text() );
    d = DataDirectory;
    d.cat_path( tlist.text() );
    s.moveto( d );
    }

/*
----------------------------------------------------------------------------------
Move all the machine directories from the computer directory to the data directory
---------------------------------------------------------------------------------- */
tlist.empty();
tlist.get_directory_list( ComputerDirectory.text() );
tlist.rewind();
while ( tlist.next() )
    {
    s = ComputerDirectory;
    s.cat_path( tlist.text() );
    d = DataDirectory;
    d.cat_path( tlist.text() );
    s.moveto( d );
    }

ComputerDirectory.delete_directory();
ComputerDirectory.empty();

convert_v5_data_dir_to_v7( DataDirectory );

return TRUE;
}

/***********************************************************************
*                           v7_results_to_v8                           *
* This routine looks through the results directories. If it finds a    *
* shotparm.txt file it converts that to a shotparm.csv file and        *
* converts the graphlst.txt file to one with an alpha shot name.       *
***********************************************************************/
BOOLEAN v7_results_to_v8( STRING_CLASS & datadir, HWND ma_tb, HWND pa_tb )
{
TEXT_LIST_CLASS machines;
TEXT_LIST_CLASS parts;
NAME_CLASS      s;
NAME_CLASS      shotparm;
NAME_CLASS      graphlst;

machines.get_directory_list( datadir.text() );
if ( machines.count() < 1 )
    return FALSE;

machines.rewind();
while ( machines.next() )
    {
    if ( ma_tb )
        set_text( ma_tb, machines.text() );
    if ( pa_tb )
        set_text( pa_tb, EmptyString );
    s = datadir;
    s.cat_path( machines.text() );
    parts.get_directory_list( s.text() );
    parts.rewind();
    while ( parts.next() )
        {
        if ( strings_are_equal(parts.text(), ResultsString) )
            continue;
        if ( pa_tb )
            set_text( pa_tb, parts.text() );
        shotparm = s;
        shotparm.cat_path( parts.text() );
        shotparm.cat_path( ResultsString );
        graphlst = shotparm;
        shotparm.cat_path( SHOTPARM_DB );
        if  ( shotparm.file_exists() )
            {
            upsize_table_w_shot_name( shotparm, SHOT_LEN );
            db_to_vdb( shotparm, 0 );
            graphlst.cat_path( GRAPHLST_DB );
            upsize_table_w_shot_name( graphlst, SHOT_LEN );
            }
        }
    }

if ( ma_tb )
    set_text( ma_tb, EmptyString );
if ( pa_tb )
    set_text( pa_tb, EmptyString );
return TRUE;
}