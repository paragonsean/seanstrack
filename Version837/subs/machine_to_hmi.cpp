#include <windows.h>
#include <math.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\fontclas.h"
#include "..\include\ftii.h"
#include "..\include\iniclass.h"
#include "..\include\limit_switch_class.h"
#include "..\include\machine.h"
#include "..\include\part.h"
#include "..\include\setpoint.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\stsetup.h"

STRING_CLASS DefLowImpactSetting;
STRING_CLASS DefHmiSettings[NOF_DEFAULT_HMI_SETTINGS];

/*
---------------------------------------------------------------------------------------------------
In order to use this function you must have the following defined somewhere and it must contain the
entries from the common.stbl
--------------------------------------------------------------------------------------------------- */
extern STRINGTABLE_CLASS StringTable;
extern TCHAR ConfigSection[];

/***********************************************************************
*                             PART_TO_HMI                              *
***********************************************************************/
static BOOLEAN part_to_hmi( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS part )
{
int    i;
double x;
LIMIT_SWITCH_CLASS   ls;
PART_CLASS           p;
SURETRAK_SETUP_CLASS s;

if ( !p.find(computer, machine, part) )
    return FALSE;
if ( !ls.get(computer, machine, part) )
    return FALSE;
if ( !s.find(computer, machine, part) )
    return FALSE;

i = s.nof_steps();
if ( i>1 )
    {
    /*
    --------------------------------------------------------
    Make sure there is a low impact setting in the last step
    -------------------------------------------------------- */
    s.gotostep( i );
    if ( is_empty(s.low_impact_string()) )
        {
        s.set_low_impact( DefLowImpactSetting.text() );
        s.save( computer, machine, part );
        }
    }

i = DefHmiSettings[DEF_HMI_PRE_FAST_LS_INDEX].uint_value();
if ( i > 0 )
    {
    i--;
    ls.desc(i) = StringTable.find( TEXT("PRE_FAST") );
    x = 3;
    if ( p.distance_units != INCH_UNITS )
        x = adjust_for_units( p.distance_units, x, INCH_UNITS );
    x *= double(-1.0);
    x += s.end_pos_value();
    ls.pos(i) = x;
    }

i = DefHmiSettings[DEF_HMI_FAST_LS_INDEX].uint_value();
if ( i > 0 )
    {
    i--;
    ls.desc(i) = StringTable.find( TEXT("FAST") );
    ls.pos(i)  = s.end_pos_string();
    }

i = DefHmiSettings[DEF_HMI_SHOT_FULL_FORWARD_LS_INDEX].uint_value();;
if ( i > 0 )
    {
    i--;
    ls.desc(i) = StringTable.find( TEXT("SHOT_FULL_FORWARD") );
    ls.pos(i)  = p.total_stroke_length;
    }

i = DefHmiSettings[DEF_HMI_INTENS_LS_INDEX].uint_value();
if ( i > 0 )
    {
    i--;
    ls.desc(i) = StringTable.find( TEXT("INTENSIFICATION") );
    ls.wire(i) = DefHmiSettings[DEF_HMI_LS_6_WIRE_INDEX].uint_value();
    }

ls.wire(4) =  DefHmiSettings[DEF_HMI_LS_5_WIRE_INDEX].uint_value();

return ls.put( computer, machine, part );
}

/***********************************************************************
*                            MACHINE_TO_HMI                            *
***********************************************************************/
BOOLEAN machine_to_hmi( MACHINE_CLASS & m, BOOLEAN need_to_update_parts )
{
TCHAR * cp;
int     i;
double  x;
INI_CLASS       ini;
TEXT_LIST_CLASS p;
NAME_CLASS      s;
FTII_VAR_ARRAY  va;
SETPOINT_CLASS  sp;

/*
-------------------------------------------
Use the min low impact setting as a default
------------------------------------------- */
s.get_ft2_editor_settings_file_name( m.computer, m.name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
DefLowImpactSetting = ini.get_string( MinLowImpactPercentKey );
if ( DefLowImpactSetting.isempty() )
    {
    DefLowImpactSetting = ZeroChar;
    }
else
    {
    cp = DefLowImpactSetting.text();
    while ( *cp == SpaceChar )
        cp++;
    if ( !is_numeric(*cp) )
        DefLowImpactSetting = ZeroChar;
    }
if ( !DefLowImpactSetting.contains(PercentChar) )
    DefLowImpactSetting += PercentChar;

s.get_default_hmi_settings_csvname();
sp.get( s );
for ( i=0; i<NOF_DEFAULT_HMI_SETTINGS; i++ )
    DefHmiSettings[i] = sp[i].value;
sp.cleanup();

s.get_ft2_machine_settings_file_name( m.computer, m.name );
if ( s.file_exists() )
    va.get( s );

va.update( VALVE_TYPES_1_VN, DefHmiSettings[DEF_HMI_VALVE_TYPE_INDEX].uint_value() );

x = DefHmiSettings[DEF_HMI_PULSE_TIME_INDEX].real_value();
x *= 1000000.0;
x = floor( x );
va.update( PULSE_TIME_VN, unsigned(x) );

x = DefHmiSettings[DEF_HMI_DEVENT_TIME_INDEX].uint_value();
x *= 1000000.0;
x = floor( x );
va.update( DEVENT_TIME_VN, unsigned(x) );

va.update( PULSE_WIRE_VN, DefHmiSettings[DEF_HMI_PULSE_WIRE_INDEX].uint_value() );
va.put( s );

if ( need_to_update_parts )
    {
    get_partlist( p, m.computer, m.name );
    p.rewind();
    while ( p.next() )
        part_to_hmi( m.computer, m.name, p.text() );
    }

return TRUE;
}

/***********************************************************************
*                        REMOVE_HMI_FROM_MACHINE                       *
***********************************************************************/
BOOLEAN remove_hmi_from_machine( STRING_CLASS & computer, STRING_CLASS & machine )
{
int             n;
NAME_CLASS      s;
FTII_VAR_ARRAY  va;

n = 0;
s.get_ft2_machine_settings_file_name( computer, machine );
if ( s.file_exists() )
    {
    va.get( s );
    n = va.count();
    }

if ( n == 0 )
    return TRUE;

if ( va.find(VALVE_TYPES_1_VN) )
    va.remove();

if ( va.find(PULSE_TIME_VN) )
    va.remove();

if ( va.find(DEVENT_TIME_VN) )
    va.remove();

if ( va.find(PULSE_WIRE_VN) )
    va.remove();

va.put( s );

return TRUE;
}
