#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\stsetup.h"

static const TCHAR NullChar      = TEXT( '\0' );
static const TCHAR PercentChar   = TEXT( '%' );
static       TCHAR EmptyString[] = TEXT( "" );
static STRING_CLASS ZeroPointZero( TEXT( "0.0") );
static STRING_CLASS DefaultBreakVelocity( TEXT( "300") );
static STRING_CLASS DefaultBreakGain(     TEXT( "1") );

/***********************************************************************
*                              IS_PERCENT                              *
***********************************************************************/
static BOOLEAN is_percent( TCHAR * sorc )
{
if ( findchar(PercentChar, sorc ) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                             FLOAT_VALUE                              *
***********************************************************************/
static double float_value( TCHAR * sorc )
{
static TCHAR s[STSTEPS_FLOAT_LEN+1];
TCHAR * cp;
int32   i;

cp = s;
for ( i=0; i<STSTEPS_FLOAT_LEN; i++ )
    {
    if ( *sorc == PercentChar || *sorc == NullChar )
        break;

    *cp = *sorc;
    cp++;
    sorc++;
    }

*cp = NullChar;

/*
-------------------------------------------
If the string is empty of numbers, return 0
------------------------------------------- */
if ( cp == s )
    return 0.0;

return extdouble( s );
}

/***********************************************************************
*                            CLEAR_STEPS                               *
***********************************************************************/
void SURETRAK_SETUP_CLASS::clear_steps()
{
int32 i;

n             = 0;
current_index = 0;
is_rewound    = TRUE;

for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    *step[i].accel      = NullChar;
    *step[i].vel        = NullChar;
    *step[i].end_pos    = NullChar;
    *step[i].low_impact = NullChar;

    step[i].vacuum_wait           = FALSE;
    step[i].low_impact_from_input = FALSE;
    step[i].accel_percent         = FALSE;
    step[i].vel_percent           = FALSE;
    }
}

/***********************************************************************
*                              EMPTY                                   *
***********************************************************************/
void SURETRAK_SETUP_CLASS::empty()
{
int32 i;

clear_steps();

for ( i=0; i<NOF_GAIN_BREAKS; i++ )
    {
    sp[GAIN_BREAK_VEL_0_INDEX+i].value  = TEXT( "0.0" );
    sp[GAIN_BREAK_GAIN_0_INDEX+i].value = TEXT( "1.0" );
    }

if ( sp.count() >= NOF_STSETUP_SETPOINTS )
    {
    sp[VELOCITY_LOOP_GAIN_INDEX].value = TEXT( "300.0" );
    sp[FT_STOP_POS_INDEX].value.null();
    sp[VACUUM_LIMIT_SWITCH_NUMBER_INDEX].value = NO_WIRE;
    }
}

/***********************************************************************
*                         SURETRAK_SETUP_CLASS                         *
***********************************************************************/
SURETRAK_SETUP_CLASS::SURETRAK_SETUP_CLASS()
{
sp.setsize( NOF_STSETUP_SETPOINTS );
empty();
}

/***********************************************************************
*                         ~SURETRAK_SETUP_CLASS                        *
***********************************************************************/
SURETRAK_SETUP_CLASS::~SURETRAK_SETUP_CLASS()
{
}

/***********************************************************************
*                                FIND                                  *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::find( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
int          i;
DB_TABLE     t;
NAME_CLASS   s;

s.get_ststeps_dbname( co, ma, pa );
if ( s.file_exists() )
    {
    if ( t.open(s, STSTEPS_RECLEN, PFL) )
        {
        n = 0;
        while ( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( step[n].accel,      STSTEPS_ACCEL_OFFSET,      STSTEPS_FLOAT_LEN );
            strip( step[n].accel );
            t.get_alpha( step[n].vel,        STSTEPS_VEL_OFFSET,        STSTEPS_FLOAT_LEN );
            strip( step[n].vel );
            t.get_alpha( step[n].end_pos,    STSTEPS_END_POS_OFFSET,    STSTEPS_FLOAT_LEN );
            strip( step[n].end_pos );
            t.get_alpha( step[n].low_impact, STSTEPS_LOW_IMPACT_OFFSET, STSTEPS_FLOAT_LEN );
            strip( step[n].low_impact );

            step[n].vacuum_wait           = t.get_boolean( STSTEPS_VACUUM_WAIT_OFFSET );
            step[n].low_impact_from_input = t.get_boolean( STSTEPS_LOW_IMPACT_FROM_INPUT_OFFSET );
            step[n].accel_percent         = is_percent( step[n].accel );
            step[n].vel_percent           = is_percent( step[n].vel );

            n++;
            }
        t.close();
        }
    }

s.get_stsetup_csvname( co, ma, pa );
if ( s.file_exists() )
    {
    sp.getfrom( s );

    /*
    -----------------------------------------------------------------------------------
    We are no longer using integral and derivative gains. Check for a zero velocity and
    replace with vel=300, gain=1.
    ----------------------------------------------------------------------------------- */

    for ( i=0; i<NOF_BREAK_GAIN_SETTINGS; i++ )
        {
        if ( is_zero(gain_break_velocity_value(i)) )
            {
            set_gain_break_velocity( i, DefaultBreakVelocity );
            set_gain_break_gain(     i, DefaultBreakGain );
            }
        }
    }
current_index = 0;
is_rewound    = TRUE;

return TRUE;
}

/***********************************************************************
*                                FIND                                  *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::find( PART_NAME_ENTRY & p )
{
return find( p.computer, p.machine, p.part );
}

/***********************************************************************
*                                SAVE                                  *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::save( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
static TCHAR * description[NOF_STSETUP_SETPOINTS] = {
    { TEXT("Gain Break Velocity 1") },
    { TEXT("Gain Break Velocity 2") },
    { TEXT("Gain Break Velocity 3") },
    { TEXT("Gain Break Gain 1") },
    { TEXT("Gain Break 2 or Integral Gain") },
    { TEXT("Gain Break 3 or Derivative Gain") },
    { TEXT("Velocity Loop Gain") },
    { TEXT("Follow Through Stop Position") },
    { TEXT("Vacuum Limit Switch Number") }
    };

int32      i;
NAME_CLASS s;
DB_TABLE   t;

s.get_ststeps_dbname( co, ma, pa );
if ( !s.file_exists() )
    {
    if ( !t.ensure_existance(s) )
        return FALSE;
    }

if ( !t.open(s, STSTEPS_RECLEN, FL) )
    return FALSE;

t.empty();
for ( i=0; i<n; i++ )
    {
    t.put_alpha(   STSTEPS_ACCEL_OFFSET,          step[i].accel,      STSTEPS_FLOAT_LEN );
    t.put_alpha(   STSTEPS_VEL_OFFSET,            step[i].vel,        STSTEPS_FLOAT_LEN );
    t.put_alpha(   STSTEPS_END_POS_OFFSET,        step[i].end_pos,    STSTEPS_FLOAT_LEN );
    t.put_alpha(   STSTEPS_LOW_IMPACT_OFFSET,     step[i].low_impact, STSTEPS_FLOAT_LEN );
    t.put_boolean( STSTEPS_VACUUM_WAIT_OFFSET,    step[i].vacuum_wait );
    t.put_boolean( STSTEPS_LOW_IMPACT_FROM_INPUT_OFFSET, step[i].low_impact_from_input );
    t.rec_append();
    }

t.close();

for ( i=0; i<NOF_STSETUP_SETPOINTS; i++ )
    {
    if ( sp[i].desc.isempty() )
        sp[i].desc = description[i];
    }
s.get_stsetup_csvname( co, ma, pa );
sp.put( s );

return TRUE;
}

/***********************************************************************
*                                SAVE                                  *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::save( PART_NAME_ENTRY & p )
{
return save( p.computer, p.machine, p.part );
}

/***********************************************************************
*                               APPEND                                 *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::append()
{
if ( n < MAX_ST_STEPS )
    {
    n++;
    current_index = n - 1;
    is_rewound    = FALSE;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          DETERMINE_NOF_STEPS                         *
*              Determine n based on the content. Return n.             *
***********************************************************************/
int SURETRAK_SETUP_CLASS::determine_nof_steps()
{
int     i;

rewind();
n = 0;

for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    if ( !is_numeric(step[i].accel) && !is_numeric(step[i].vel) && !is_numeric(step[i].end_pos) )
        break;
    n++;
    }

return n;
}

/***********************************************************************
*                                NEXT                                  *
*  If I have just rewound, point to the 0th record and return true     *
*  if there is one. Otherwise, if there is another record, point to    *
*  it and return TRUE.                                                 *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::next()
{
if ( is_rewound )
    {
    is_rewound = FALSE;
    if ( n > 0 )
        return TRUE;
    }
else if ( current_index < (n-1) )
    {
    current_index++;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                                GOTOSTEP                              *
* Go directly to a step. Step numbers are 1 thru 6.                    *
***********************************************************************/
BOOLEAN SURETRAK_SETUP_CLASS::gotostep( int step_wanted )
{
int i;

step_wanted--;

if ( step_wanted < 0 || step_wanted >=MAX_ST_STEPS )
    return FALSE;


if ( step_wanted >= n )
    {
    for ( i=n; i<=step_wanted; i++ )
        append();
    }

is_rewound = FALSE;
current_index = step_wanted;

return TRUE;
}

/***********************************************************************
*                             ACCEL_VALUE                              *
***********************************************************************/
double SURETRAK_SETUP_CLASS::accel_value()
{
return float_value( step[current_index].accel );
}

/***********************************************************************
*                               VEL_VALUE                              *
***********************************************************************/
double SURETRAK_SETUP_CLASS::vel_value()
{
return float_value( step[current_index].vel );
}

/***********************************************************************
*                           END_POS_VALUE                              *
***********************************************************************/
double SURETRAK_SETUP_CLASS::end_pos_value()
{
return float_value( step[current_index].end_pos );
}

/***********************************************************************
*                         GAIN_BREAK_VELOCITY                          *
***********************************************************************/
STRING_CLASS & SURETRAK_SETUP_CLASS::gain_break_velocity( int i )
{
if ( i < 0 || i >= NOF_GAIN_BREAKS )
    return ZeroPointZero;

return sp[GAIN_BREAK_VEL_0_INDEX+i].value;
}

/***********************************************************************
*                           GAIN_BREAK_GAIN                            *
***********************************************************************/
STRING_CLASS & SURETRAK_SETUP_CLASS::gain_break_gain( int i )
{
if ( i < 0 || i >= NOF_GAIN_BREAKS )
    return ZeroPointZero;

return sp[GAIN_BREAK_GAIN_0_INDEX+i].value;
}

/***********************************************************************
*                      GAIN_BREAK_VELOCITY_VALUE                       *
***********************************************************************/
double SURETRAK_SETUP_CLASS::gain_break_velocity_value( int i )
{
if ( i < 0 || i >= NOF_GAIN_BREAKS )
    return 0.0;

return sp[GAIN_BREAK_VEL_0_INDEX+i].real_value();
}

/***********************************************************************
*                        GAIN_BREAK_GAIN_VALUE                         *
***********************************************************************/
double SURETRAK_SETUP_CLASS::gain_break_gain_value( int i )
{
if ( i < 0 || i >= NOF_GAIN_BREAKS )
    return 0.0;

return sp[GAIN_BREAK_GAIN_0_INDEX+i].real_value();
}

/***********************************************************************
*                          LOW_IMPACT_VALUE                            *
***********************************************************************/
double SURETRAK_SETUP_CLASS::low_impact_value()
{
return float_value( step[current_index].low_impact );
}

/***********************************************************************
*                               SET_ACCEL                              *
***********************************************************************/
void SURETRAK_SETUP_CLASS::set_accel( TCHAR * sorc )
{
lstrcpy( step[current_index].accel, sorc);
step[current_index].accel_percent = is_percent( sorc );
}

/***********************************************************************
*                       SET_GAIN_BREAK_VELOCITY                        *
***********************************************************************/
void SURETRAK_SETUP_CLASS::set_gain_break_velocity( int i,  STRING_CLASS & new_velocity )
{
if ( i >= 0 && i < NOF_GAIN_BREAKS )
    sp[GAIN_BREAK_VEL_0_INDEX+i].value = new_velocity;
}

/***********************************************************************
*                         SET_GAIN_BREAK_GAIN                          *
***********************************************************************/
void SURETRAK_SETUP_CLASS::set_gain_break_gain( int i, STRING_CLASS & new_gain )
{
if ( i >= 0 && i < NOF_GAIN_BREAKS )
    sp[GAIN_BREAK_GAIN_0_INDEX+i].value = new_gain;
}

/***********************************************************************
*                                 SET_VEL                              *
***********************************************************************/
void SURETRAK_SETUP_CLASS::set_vel( TCHAR * sorc )
{
lstrcpy( step[current_index].vel, sorc);
step[current_index].vel_percent = is_percent( sorc );
}
