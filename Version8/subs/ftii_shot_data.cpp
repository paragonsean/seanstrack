#include <windows.h>
#include <limits.h>
#include <cmath>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\asensor.h"
#include "..\include\fileclas.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\ftii_shot_data.h"

const int CHANNEL_MODE_1  =  1;  /* This is really defined in ftii.h */
const int CHANNEL_MODE_2  =  2;  /* This is really defined in ftii.h */
const int CHANNEL_MODE_3  =  3;  /* This is really defined in ftii.h */
const int CHANNEL_MODE_4  =  4;  /* This is really defined in ftii.h */
const int CHANNEL_MODE_5  =  5;  /* This is really defined in ftii.h */

const int NOF_FILE_COUNTS =  7;
const int NOF_CURVES      = NOF_FTII_FILE_CURVES;

const unsigned MAX_12_BIT_VALUE = 4095;
const unsigned CHANNEL_MASK     = 0xF000;
const unsigned VALUE_MASK       = 0x0FFF;
const unsigned TIMER_MASK       = 0x3FFFFFFF; /* Hi bit is direction, next is error */
const unsigned ERROR_MASK       = 0x40000000; /* Quadrature error signal */
const unsigned DIRECTION_MASK   = 0x80000000; /* */

static TCHAR CommaChar     = TEXT( ',' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR YChar         = TEXT( 'Y' );
static TCHAR CommaString[] = TEXT( "," );
static TCHAR EmptyString[] = TEXT( "" );

/***********************************************************************
*                     EXTRACT_CHANNEL_FROM_ANALOG                      *
                               [0,15]
***********************************************************************/
static int extract_channel_from_analog( unsigned short analog_value )
{
unsigned x;

x = (unsigned) analog_value;
x &= CHANNEL_MASK;
x >>= 12;

return (int) x;
}

/***********************************************************************
*                        TEXT_PARAMETER_ENTRY                      *
*                                 READ                                 *
***********************************************************************/
BOOLEAN TEXT_PARAMETER_ENTRY::read( FILE_CLASS & f )
{
int nof_tchars;
int nof_bytes;

f.readbinary( &parameter_number, sizeof(int) );
f.readbinary( &nof_tchars, sizeof(int) );
if ( nof_tchars > 0 )
    {
    value.upsize( nof_tchars );
    nof_bytes = nof_tchars * sizeof( TCHAR );
    f.readbinary( value.text(), nof_bytes );
    }
else
    {
    value.null();
    }

return TRUE;
}

/***********************************************************************
*                        TEXT_PARAMETER_ENTRY                      *
*                                WRITE                                 *
***********************************************************************/
BOOLEAN TEXT_PARAMETER_ENTRY::write( FILE_CLASS & f )
{
int nof_tchars;
int nof_bytes;

f.writebinary( &parameter_number, sizeof(int) );
nof_tchars = value.len() + 1;
f.writebinary( &nof_tchars, sizeof(int) );
if ( nof_tchars > 0 )
    {
    nof_bytes = nof_tchars * sizeof( TCHAR );
    f.writebinary( value.text(), nof_bytes );
    }

return TRUE;
}

/***********************************************************************
*                         FTII_OP_STATUS_DATA                          *
***********************************************************************/
FTII_OP_STATUS_DATA::FTII_OP_STATUS_DATA()
{
dac[0]         = 0;
dac[1]         = 0;
dac[2]         = 0;
dac[3]         = 0;
analog[0]      = 0;
analog[1]      = 0;
analog[2]      = 0;
analog[3]      = 0;
analog[4]      = 0;
analog[5]      = 0;
analog[6]      = 0;
analog[7]      = 0;
analog[8]      = 0;
analog[9]      = 0;
analog[10]     = 0;
analog[11]     = 0;
analog[12]     = 0;
analog[13]     = 0;
analog[14]     = 0;
analog[15]     = 0;
analog[16]     = 0;
analog[17]     = 0;
analog[18]     = 0;
analog[19]     = 0;
pos            = 0;
vel            = 0;
isw1           = 0;
isw4           = 0;
osw1           = 0;
monitor_status = 0;
status_word1   = 0;
config_word1   = 0;
warning        = 0;
fatal_error    = 0;
blk_no         = 0;
}

/***********************************************************************
*                         FTII_OP_STATUS_DATA                          *
***********************************************************************/
bool FTII_OP_STATUS_DATA::operator==( const FTII_OP_STATUS_DATA & sorc )
{
if ( dac[0]!=sorc.dac[0]             )return false;
if ( dac[1]!=sorc.dac[1]             )return false;
if ( dac[2]!=sorc.dac[2]             )return false;
if ( dac[3]!=sorc.dac[3]             )return false;
if ( analog[0] != sorc.analog[0]     )return false;
if ( analog[1] != sorc.analog[1]     )return false;
if ( analog[2] != sorc.analog[2]     )return false;
if ( analog[3] != sorc.analog[3]     )return false;
if ( analog[4] != sorc.analog[4]     )return false;
if ( analog[5] != sorc.analog[5]     )return false;
if ( analog[6] != sorc.analog[6]     )return false;
if ( analog[7] != sorc.analog[7]     )return false;
if ( analog[8] != sorc.analog[8]     )return false;
if ( analog[9] != sorc.analog[9]     )return false;
if ( analog[10] != sorc.analog[10]   )return false;
if ( analog[11] != sorc.analog[11]   )return false;
if ( analog[12] != sorc.analog[12]   )return false;
if ( analog[13] != sorc.analog[13]   )return false;
if ( analog[14] != sorc.analog[14]   )return false;
if ( analog[15] != sorc.analog[15]   )return false;
if ( analog[16] != sorc.analog[16]   )return false;
if ( analog[17] != sorc.analog[17]   )return false;
if ( analog[18] != sorc.analog[18]   )return false;
if ( analog[19] != sorc.analog[19]   )return false;
if ( pos!=sorc.pos                   )return false;
if ( vel!=sorc.vel                   )return false;
if ( isw1!=sorc.isw1                 )return false;
if ( isw4!=sorc.isw4                 )return false;
if ( osw1!=sorc.osw1                 )return false;
if ( monitor_status!=sorc.monitor_status)return false;
if ( status_word1!=sorc.status_word1 )return false;
if ( config_word1!=sorc.config_word1 )return false;
if ( warning!=sorc.warning           )return false;
if ( fatal_error!=sorc.fatal_error   )return false;
if ( blk_no!=sorc.blk_no             )return false;
return true;
}

/***********************************************************************
*                         FTII_OP_STATUS_DATA                          *
*                                 =                                    *
***********************************************************************/
void FTII_OP_STATUS_DATA::operator=( const FTII_OP_STATUS_DATA & sorc )
{
dac[0]         = sorc.dac[0];
dac[1]         = sorc.dac[1];
dac[2]         = sorc.dac[2];
dac[3]         = sorc.dac[3];
analog[0]      = sorc.analog[0];
analog[1]      = sorc.analog[1];
analog[2]      = sorc.analog[2];
analog[3]      = sorc.analog[3];
analog[4]      = sorc.analog[4];
analog[5]      = sorc.analog[5];
analog[6]      = sorc.analog[6];
analog[7]      = sorc.analog[7];
analog[8]      = sorc.analog[8];
analog[9]      = sorc.analog[9];
analog[10]     = sorc.analog[10];
analog[11]     = sorc.analog[11];
analog[12]     = sorc.analog[12];
analog[13]     = sorc.analog[13];
analog[14]     = sorc.analog[14];
analog[15]     = sorc.analog[15];
analog[16]     = sorc.analog[16];
analog[17]     = sorc.analog[17];
analog[18]     = sorc.analog[18];
analog[19]     = sorc.analog[19];
pos            = sorc.pos;
vel            = sorc.vel;
isw1           = sorc.isw1;
isw4           = sorc.isw4;
osw1           = sorc.osw1;
monitor_status = sorc.monitor_status;
status_word1   = sorc.status_word1;
config_word1   = sorc.config_word1;
warning        = sorc.warning;
fatal_error    = sorc.fatal_error;
blk_no         = sorc.blk_no;
}

/***********************************************************************
*                          FTII_PARAMETERS                             *
*                                =                                     *
***********************************************************************/
void FTII_PARAMETERS::operator=( const FTII_PARAMETERS & sorc )
{
int i;

cycle_time   = sorc.cycle_time;
biscuit_size = sorc.biscuit_size;
eos_pos      = sorc.eos_pos;

for ( i=0; i<NOF_STATIC_ANALOGS; i++ )
    static_analog[i] = sorc.static_analog[i];
};

/***********************************************************************
*                          FTII_PARAMETERS                             *
*                                =                                     *
***********************************************************************/
void FTII_PARAMETERS::operator=( const VERSION_1_FTII_PARAMETERS & sorc )
{
int i;
cycle_time   = sorc.cycle_time;
biscuit_size = sorc.biscuit_size;
eos_pos      = sorc.eos_pos;
for ( i=0; i<NOF_STATIC_ANALOGS; i++ )
    static_analog[i] = 0;
};

/***********************************************************************
*                        FTII_POSITION_DATA                            *
*                                =                                     *
***********************************************************************/
void FTII_POSITION_DATA::operator=( const FTII_SAMPLE & sorc )
{
int i;

for ( i=0; i<NOF_FTII_ANALOGS; i++ )
    analog[i] = sorc.analog[i];

for ( i=0; i<X4_COUNTS_PER_MARK; i++ )
    velcount[i] = sorc.velcount[i];

isw1     = sorc.isw1;
osw1     = sorc.osw1;
isw4     = unsigned short( sorc.isw4 ); // Only 6 bits are significant
position = short( sorc.position );
}

/***********************************************************************
*                           FTII_TIME_DATA                             *
*                                  =                                   *
***********************************************************************/
void FTII_TIME_DATA::operator=( const FTII_SAMPLE & sorc )
{
int i;

for ( i=0; i<NOF_FTII_ANALOGS; i++ )
    analog[i] = sorc.analog[i];

isw1     = sorc.isw1;
osw1     = sorc.osw1;
isw4     = unsigned short( sorc.isw4 ); // Only 6 bits are significant
position = short( sorc.position );
}

/***********************************************************************
*                             FTII_NET_DATA                            *
***********************************************************************/
FTII_NET_DATA::FTII_NET_DATA()
{
timer_frequency      = 16666666;
np                   = 0;
nt                   = 0;
us_per_time_sample   = 1000;
internal_shot_number = 0;
position_sample      = 0;
time_sample          = 0;
}

/***********************************************************************
*                             FTII_NET_DATA                            *
*                                 clear                                *
***********************************************************************/
void FTII_NET_DATA::clear()
{
np = 0;
nt = 0;
if ( position_sample  )
    {
    delete[] (char *) position_sample;
    position_sample = 0;
    }

if ( time_sample )
    {
    delete[] (char *) time_sample;
    time_sample = 0;
    }
}

/***********************************************************************
*                            ~FTII_NET_DATA                            *
***********************************************************************/
FTII_NET_DATA::~FTII_NET_DATA()
{
clear();
}

/***********************************************************************
*                            FTII_NET_DATA                             *
*                            SET_SHOT_TIME                             *
***********************************************************************/
void FTII_NET_DATA::set_shot_time( FILETIME & t )
{
shot_time = t;
}

/***********************************************************************
*                            FTII_NET_DATA                             *
*                        SET_TIME_BASED_POINTS                         *
***********************************************************************/
void FTII_NET_DATA::set_time_based_points( FTII_SAMPLE * p, int nof_samples )
{
if ( time_sample )
    {
    delete[] (char *) time_sample;
    time_sample = 0;
    nt          = 0;
    }

if ( nof_samples > 0 )
    {
    time_sample = p;
    nt          = nof_samples;
    }
}

/***********************************************************************
*                            FTII_NET_DATA                             *
*                        SET_TIME_BASED_POINTS                         *
***********************************************************************/
void FTII_NET_DATA::set_time_based_points( NEW_FTII_DATA & nd )
{
set_time_based_points( (FTII_SAMPLE *) nd.buf, nd.len );
nd.buf = 0;
nd.len = 0;
}

/***********************************************************************
*                            FTII_NET_DATA                             *
*                      SET_POSITION_BASED_POINTS                       *
***********************************************************************/
void FTII_NET_DATA::set_position_based_points( FTII_SAMPLE * p, int nof_samples )
{
if ( position_sample )
    {
    delete[] (char *) position_sample;
    position_sample = 0;
    np              = 0;
    }

if ( nof_samples > 0 )
    {
    position_sample = p;
    np              = nof_samples;
    }
}

/***********************************************************************
*                            FTII_NET_DATA                             *
*                      SET_POSITION_BASED_POINTS                       *
***********************************************************************/
void FTII_NET_DATA::set_position_based_points( NEW_FTII_DATA & nd )
{
set_position_based_points( (FTII_SAMPLE *) nd.buf, nd.len );
nd.buf = 0;
nd.len = 0;
}

/***********************************************************************
*                            FTII_NET_DATA                             *
*                            SET_PARAMETERS                            *
***********************************************************************/
void FTII_NET_DATA::set_parameters( NEW_FTII_DATA & nd )
{
FTII_PARAMETERS           * p;
VERSION_1_FTII_PARAMETERS * p1;

if ( nd.len > sizeof(VERSION_1_FTII_PARAMETERS) )
    {
    p = (FTII_PARAMETERS *) nd.buf;

    parameter = *p;
    }
else
    {
    p1 = (VERSION_1_FTII_PARAMETERS *) nd.buf;

    parameter = *p1;
    }

/*
----------------------------------------
I am responsible for deleting the buffer
---------------------------------------- */
delete[] (char *) nd.buf;
nd.buf = 0;
nd.len = 0;
}

/***********************************************************************
*                            FTII_NET_DATA                             *
*                            SET_PARAMETERS                            *
***********************************************************************/
void FTII_NET_DATA::set_parameters( FTII_PARAMETERS * p )
{
parameter = *p;
delete[] (char *) p;
}

/***********************************************************************
*                            MAKE_CSV_LINE                             *
***********************************************************************/
static void make_csv_line( STRING_CLASS & s, FTII_SAMPLE & sample )
{
int j;

s = sample.position;
s += CommaChar;

s += sample.one_ms_timer;
s += CommaChar;

for ( j=0; j<4; j++ )
    {
    s += sample.velcount[j];
    s += CommaChar;
    }

s += sample.isw1;
s += CommaChar;

s += sample.isw4;
s += CommaChar;

s += sample.osw1;
s += CommaChar;

for ( j=0; j<8; j++ )
    {
    s += (unsigned int) sample.analog[j];
//  u &= 0xfff;
    if ( j<7 )
        s += CommaChar;
    }
}

/***********************************************************************
*                             FTII_NET_DATA                            *
*                               write_csv                              *
***********************************************************************/
BOOLEAN FTII_NET_DATA::write_csv( STRING_CLASS & dest_file_path )
{
static TCHAR title[] = TEXT( "Pos,Time,Q1,Q2,Q3,Q4,ISW1,ISW4,OSW1,A1,A2,A3,A4,A5,A6,A7,A8" );
FILE_CLASS f;
STRING_CLASS s;
int i;

if ( !f.open_for_write( dest_file_path.text()) )
    return FALSE;

f.writeline( title );

for ( i=0; i<np; i++ )
    {
    make_csv_line( s, position_sample[i] );
    f.writeline( s.text() );
    }

for ( i=0; i<nt; i++ )
    {
    make_csv_line( s, time_sample[i] );
    f.writeline( s.text() );
    }

f.close();
return TRUE;
}

/***********************************************************************
*                             FTII_FILE_DATA                           *
***********************************************************************/
FTII_FILE_DATA::FTII_FILE_DATA()
{
channel_mode            = CHANNEL_MODE_1;
timer_frequency         = 16666666;
np                      = 0;
nt                      = 0;
cs_to_point_1_ms        = 0;
us_per_time_sample      = 1000;
internal_shot_number    = 0;
position_sample         = 0;
time_sample             = 0;
parameter[0]            = 0;
parameter[1]            = 0;
parameter[2]            = 0;
parameter[3]            = 0;
parameter[4]            = 0;
parameter[5]            = 0;
parameter[6]            = 0;
parameter[7]            = 0;
parameter[8]            = 0;
parameter[9]            = 0;
parameter[10]           = 0;
nof_extended_analogs    = 0;
extended_analog         = 0;
nof_text_parameters     = 0;
text_parameter      = 0;
}

/***********************************************************************
*                             FTII_FILE_DATA                           *
*                                 clear                                *
***********************************************************************/
void FTII_FILE_DATA::clear()
{
np = 0;
nt = 0;
if ( position_sample  )
    {
    delete[] position_sample;
    position_sample = 0;
    }

if ( time_sample )
    {
    delete[] time_sample;
    time_sample = 0;
    }

nof_extended_analogs = 0;
if ( extended_analog )
    {
    delete[] extended_analog;
    extended_analog = 0;
    }

nof_text_parameters = 0;
if ( text_parameter )
    {
    delete[] text_parameter;
    text_parameter = 0;
    }
}

/***********************************************************************
*                            ~FTII_FILE_DATA                           *
***********************************************************************/
FTII_FILE_DATA::~FTII_FILE_DATA()
{
clear();
}

/***********************************************************************
*                             FTII_FILE_DATA                           *
*                                  move                                *
*            Transfer the data from one instance to another.           *
***********************************************************************/
void FTII_FILE_DATA::move( FTII_FILE_DATA & sorc )
{
int i;

clear();

channel_mode         = sorc.channel_mode;
timer_frequency      = sorc.timer_frequency;
np                   = sorc.np;
nt                   = sorc.nt;
cs_to_point_1_ms     = sorc.cs_to_point_1_ms;
us_per_time_sample   = sorc.us_per_time_sample;
internal_shot_number = sorc.internal_shot_number;
shot_time            = sorc.shot_time;

for ( i=0; i<NOF_FTII_FILE_CURVES; i++ )
    curvedata[i] = sorc.curvedata[i];

position_sample = sorc.position_sample;
time_sample     = sorc.time_sample;

if ( sorc.nof_extended_analogs > 0 )
    {
    extended_analog      = sorc.extended_analog;
    nof_extended_analogs = sorc.nof_extended_analogs;
    sorc.nof_extended_analogs = 0;
    sorc.extended_analog      = 0;
    }

if ( sorc.nof_text_parameters > 0 )
    {
    text_parameter = sorc.text_parameter;
    nof_text_parameters = sorc.nof_text_parameters;
    sorc.text_parameter = 0;
    sorc.nof_text_parameters = 0;
    }

sorc.position_sample = 0;
sorc.time_sample     = 0;
sorc.clear();

for ( i=0; i<NOF_FTII_FILE_PARAMETERS; i++ )
    parameter[i] = sorc.parameter[i];

}

/***********************************************************************
*                        RESIZE_TEXT_PARAMETERS                        *
***********************************************************************/
BOOLEAN FTII_FILE_DATA::resize_text_parameters( int new_count, BOOLEAN copy_old_recs )
{
int                        i;
int                        n;
TEXT_PARAMETER_ENTRY * new_text_parameter;
int                        old_count;
TEXT_PARAMETER_ENTRY * old_text_parameter;

if ( new_count == nof_text_parameters )
    {
    if ( !copy_old_recs )
        {
        for ( i=0; i<new_count; i++ )
            {
            text_parameter[i].parameter_number = NO_PARAMETER_NUMBER;
            text_parameter[i].value.empty();
            }
        }
    return TRUE;
    }

if ( new_count > 0 )
    {
    new_text_parameter = new TEXT_PARAMETER_ENTRY[new_count];
    if ( !new_text_parameter )
        return FALSE;

    if ( copy_old_recs )
        {
        n = new_count;
        if ( nof_text_parameters < n )
            n = nof_text_parameters;
        for ( i=0; i<n; i++ )
            new_text_parameter[i] = text_parameter[i];
        }
    }

old_count = nof_text_parameters;
old_text_parameter = text_parameter;

nof_text_parameters = new_count;
text_parameter      = new_text_parameter;

for ( i=0; i<old_count; i++ )
    old_text_parameter[i].value.empty();
delete[] old_text_parameter;
return TRUE;
}

/***********************************************************************
*                             FTII_FILE_DATA                           *
*                                   =                                  *
***********************************************************************/
void FTII_FILE_DATA::operator=( const FTII_NET_DATA & sorc )
{
int ai;
int i;
unsigned u;

clear();

timer_frequency      = sorc.timer_frequency;
np                   = sorc.np;
nt                   = sorc.nt;
cs_to_point_1_ms     = sorc.position_sample[1].one_ms_timer;
us_per_time_sample   = sorc.us_per_time_sample;
internal_shot_number = sorc.internal_shot_number;
shot_time            = sorc.shot_time;

if ( (np > 0) && sorc.position_sample )
    {
    position_sample = new FTII_POSITION_DATA[np];
    if ( position_sample )
        {
        for ( i=0; i<np; i++ )
            position_sample[i] = sorc.position_sample[i];
        }
    else
        {
        np = 0;
        }
    }

curvedata[0].curve_type = FTII_POSITION_TYPE;
curvedata[0].imin = 0;
curvedata[0].imax = np - 1;

if ( (nt > 0) && sorc.time_sample )
    {
    time_sample = new FTII_TIME_DATA[nt];
    if ( time_sample )
        {
        for ( i=0; i<nt; i++ )
            {
            time_sample[i] = sorc.time_sample[i];
            maxint( curvedata[0].imax, (int) time_sample[i].position );
            }
        }
    else
        {
        nt = 0;
        }
    }

parameter[0] = int( sorc.parameter.cycle_time   );
parameter[1] = int( sorc.parameter.biscuit_size );
parameter[2] = int( sorc.parameter.eos_pos      );

ai = 0;
for ( i=3; i<NOF_FTII_FILE_PARAMETERS; i++ )
    {
    u = (unsigned) sorc.parameter.static_analog[ai];
    parameter[i] = (int) u;
    ai++;
    }

nof_text_parameters = 0;
if ( text_parameter )
    {
    delete[] text_parameter;
    text_parameter = 0;
    }
}

/***********************************************************************
*                             FTII_FILE_DATA                           *
*                                  get                                 *
***********************************************************************/
BOOLEAN FTII_FILE_DATA::get( STRING_CLASS & sorc_file_path )
{
FILE_CLASS f;
int        i;
int        counts[NOF_FILE_COUNTS];
BOOLEAN    status;

clear();

status = FALSE;
if ( f.open_for_read(sorc_file_path.text())  )
    {
    f.get_creation_time( &shot_time );
    f.readbinary( counts, sizeof(counts) );

    np                   = counts[1];
    nt                   = counts[2];
    timer_frequency      = counts[4];
    us_per_time_sample   = counts[5];
    internal_shot_number = counts[6];

    f.readbinary( curvedata,  sizeof(curvedata) );

    /*
    ------------------------------------
    Figure out what the channel mode was
    ------------------------------------ */
    channel_mode = CHANNEL_MODE_1;
    i = FIRST_ANALOG_CURVE_INDEX + 4;
    i = curvedata[i].curve_type;
    if ( i == FTII_ANALOG9_TYPE )
        channel_mode = CHANNEL_MODE_2;
    else if ( i == FTII_ANALOG13_TYPE )
        channel_mode = CHANNEL_MODE_3;
    else if ( i == FTII_ANALOG17_TYPE )
        channel_mode = CHANNEL_MODE_4;
    else
        {
        i = FIRST_ANALOG_CURVE_INDEX + 6;
        i = curvedata[i].curve_type;
        if ( i == FTII_ANALOG17_TYPE )
            channel_mode = CHANNEL_MODE_5;
        }

    if ( np > 0 )
        position_sample = new FTII_POSITION_DATA[np];

    if ( nt > 0 )
        time_sample     = new FTII_TIME_DATA[nt];

    if ( position_sample && time_sample )
        {
        f.readbinary( position_sample, np*sizeof(FTII_POSITION_DATA) );
        f.readbinary( time_sample,     nt*sizeof(FTII_TIME_DATA) );

        if ( counts[0] > 1 )
            {
            f.readbinary( parameter, sizeof(parameter) );
            }
        else
            {
            f.readbinary( parameter, sizeof(VERSION_1_FTII_PARAMETERS) );
            for ( i=3; i<NOF_STATIC_ANALOGS; i++ )
                parameter[i] = 0;
            }

        if ( counts[0] > 2 )
            {
            f.readbinary( &nof_extended_analogs, sizeof(int) );
            if ( nof_extended_analogs > 0 )
                {
                extended_analog = new EXTENDED_ANALOG_ENTRY[nof_extended_analogs];
                if ( extended_analog )
                    f.readbinary( extended_analog, nof_extended_analogs * sizeof(EXTENDED_ANALOG_ENTRY) );
                else
                    nof_extended_analogs = 0;
                }
            }

        /*
        ---------------------------------------------------------------------------------------------
        I use the text parameters to store all text parameters (static_text_parameters for instance).
        --------------------------------------------------------------------------------------------- */
        if ( counts[0] > 3 )
            {
            f.readbinary( &nof_text_parameters, sizeof(int) );
            if ( nof_text_parameters > 0 )
                {
                text_parameter = new TEXT_PARAMETER_ENTRY[nof_text_parameters];
                if ( text_parameter )
                    {
                    for ( i=0; i<nof_text_parameters; i++ )
                        text_parameter[i].read( f );
                    }
                else
                    nof_text_parameters = 0;
                }
            }
        status = TRUE;
        }

    f.close();
    }

/*
---------------------------------------------------------------------------------
Sometimes the i/o bits and position of the first time based point are zeroed out.
Remove this code when Greg fixes this.
--------------------------------------------------------------------------------- */
if ( nt > 10 )
    {
    for ( i=9; i>=0; i-- )
        {
        if ( time_sample[i].position == 0 )
            {
            time_sample[i].isw1     = time_sample[i+1].isw1;
            time_sample[i].osw1     = time_sample[i+1].osw1;
            time_sample[i].isw4     = time_sample[i+1].isw4;
            time_sample[i].position = time_sample[i+1].position;
            break;
            }
        }
    }

return status;
}

/***********************************************************************
*                             FTII_FILE_DATA                           *
*                                  put                                 *
*                                                                      *
*    struct FTII_MIN_MAX_DATA {  // 10 bytes total                     *
*       short curve_type;                                              *
*       int   min;                                                     *
*       int   max;                                                     *
*       };                                                             *
*                                                                      *
*  New file structure. 21-May-03                                       *
*  Bytes                                                               *
*     4    version                                                     *
*     4    np, the number of position based points                     *
*     4    nt, the number of time based points                         *
*     4    ms from cs to point 1                                       *
*     4    timer frequency                                             *
*     4    n of microseconds per time based point;                     *
*     4    internal shot number (currently zero)                       *
*    10    curve 1 FTII_MIN_MAX_DATA                                   *
*    10    curve 2          "                                          *
*    ...                                                               *
*    10    curve 10         "                                          *
*          position_data np * sizeof(FT_POSITION_DATA)                 *
*          time data     nt * sizeof(FT_TIME_DATA)                     *
*    12    parameters (3)                                              *
*    32    eos extended analogs (8) ( added version 2 )                *
*     4    nof_extended_analogs (added version 3 )                     *
*     4*nof_extended_analogs extended_analog_values (added version 3 ) *
*     4    nof_text_parameters (added version 4)
           nof_text_parameters data
                                                                       *
* The data will be in the same form as the original data except        *
* that I leave out the non-relavent info for the different types.      *
*                                                                      *
* This is done by the two classes FT_POSITION_DATA and FT_TIME_DATA.   *
*                                                                      *
***********************************************************************/
BOOLEAN FTII_FILE_DATA::put( STRING_CLASS & dest_file_path )
{
FILE_CLASS f;
int      counts[NOF_FILE_COUNTS];
int      channel;
int      curve_number;
int      i;
int      j;
unsigned x;
__int64  x64; /* Microsoft Specific Code */

counts[0] = 4; //Version 4 Nov 2014
counts[1] = np;
counts[2] = nt;
counts[3] = cs_to_point_1_ms; // This is the ms since the cycle start
counts[4] = timer_frequency;
counts[5] = us_per_time_sample;
counts[6] = internal_shot_number;  // This will be zero for now

/*
--------
Position
-------- */
curvedata[0].curve_type = FTII_POSITION_TYPE;
curvedata[0].imin = 0;
curvedata[0].imax = np - 1;

for ( i=0; i<nt; i++ )
    maxint( curvedata[0].imax, time_sample[i].position );

/*
------------------------------------------------------------------------
                             Timer Counts
The counts in the zeroth point are garbage. I must check all four counts
in the first point as the zeroth count is not necessarily the smallest.
------------------------------------------------------------------------ */
curvedata[1].curve_type = FTII_TIMER_COUNT_TYPE;
curvedata[1].umin       = position_sample[1].velcount[0];
for ( i=1; i<X4_COUNTS_PER_MARK; i++ )
    {
    if ( position_sample[1].velcount[i] < curvedata[1].umin )
         curvedata[1].umin = position_sample[0].velcount[i];
    }

/*
---------------------------------------------------------------------------------
The max timer count is the max velcount plus the post impact time in timer counts
Thus the total shot time is umax - umin plus the unknown amount of time between
the zeroth point and the first point.
--------------------------------------------------------------------------------- */
j = np - 1;
curvedata[1].umax = position_sample[j].velcount[0];
for ( i=1; i<X4_COUNTS_PER_MARK; i++ )
    {
    if ( position_sample[j].velcount[i] > curvedata[1].umax )
         curvedata[1].umax = position_sample[j].velcount[i];
    }

/*
----------------------------------------
Add the post impact time in timer counts
---------------------------------------- */
x64 = timer_frequency;
x64 *= nt;
x64 *= us_per_time_sample;
x64 /= 1000000;
curvedata[1].umax += unsigned( x64 );

/*
-------
Analogs
------- */
curve_number = FIRST_ANALOG_CURVE_INDEX;

for ( i=0; i<NOF_FTII_ANALOGS; i++ )
    {
    /*
    ------------------------------------------------------------------------------------
    I use the third point because sometimes the board is confused about the second point
    ------------------------------------------------------------------------------------ */
    channel = extract_channel_from_analog( position_sample[2].analog[i] );
    if ( channel > 11 )
        {
        if ( channel == 12 && i == 6 )
            channel = 16;
        else if ( channel == 13 && i == 7 )
            channel = 17;
        else if ( channel_mode == CHANNEL_MODE_4 )
            channel += 4;
        }
    curvedata[curve_number].curve_type = FTII_ANALOG1_TYPE + channel;
    curvedata[curve_number].umax = 0;
    curvedata[curve_number].umin = MAX_12_BIT_VALUE;
    /*
    -----------------------------------------
    The first point is not measured. Skip it.
    ----------------------------------------- */
    for ( j=1; j<np; j++ )
        {
        x = position_sample[j].analog[i];
        x &= VALUE_MASK;
        if ( x > curvedata[curve_number].umax )
            curvedata[curve_number].umax = x;

        if ( x < curvedata[curve_number].umin )
            curvedata[curve_number].umin = x;
        }

    for ( j=0; j<nt; j++ )
        {
        x = time_sample[j].analog[i];
        x &= VALUE_MASK;
        if ( x > curvedata[curve_number].umax )
            curvedata[curve_number].umax = x;

        if ( x < curvedata[curve_number].umin )
            curvedata[curve_number].umin = x;
        }

    curve_number++;
    }

if ( position_sample && time_sample )
    {
    f.open_for_write( dest_file_path.text() );
    f.writebinary( counts,          sizeof(counts)                );
    f.writebinary( curvedata,       sizeof(curvedata)             );
    f.writebinary( position_sample, np*sizeof(FTII_POSITION_DATA) );
    f.writebinary( time_sample,     nt*sizeof(FTII_TIME_DATA)     );
    f.writebinary( parameter,       sizeof(parameter)             );
    f.writebinary( &nof_extended_analogs, sizeof(int)             );
    if ( nof_extended_analogs > 0 )
        {
        f.writebinary( extended_analog, nof_extended_analogs*sizeof(EXTENDED_ANALOG_ENTRY) );
        }
    f.writebinary( &nof_text_parameters, sizeof(int)          );
    for ( i=0; i<nof_text_parameters; i++ )
        text_parameter[i].write( f );

    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             FTII_FILE_DATA                           *
*                        set_nof_extended_analogs                      *
***********************************************************************/
void FTII_FILE_DATA::set_nof_extended_analogs( int n )
{
if ( n == nof_extended_analogs )
    return;
if ( extended_analog )
    {
    nof_extended_analogs = 0;
    delete[] extended_analog;
    }
extended_analog = new EXTENDED_ANALOG_ENTRY[n];
if ( extended_analog )
    nof_extended_analogs = n;
}

/***********************************************************************
*                            MAKE_CSV_LINE                             *
***********************************************************************/
static void make_csv_line( STRING_CLASS & s, FTII_POSITION_DATA & sample )
{
int j;

s = sample.position;
s += CommaChar;

for ( j=0; j<4; j++ )
    {
    s += sample.velcount[j];
    s += CommaChar;
    }

s += sample.isw1;
s += CommaChar;

s += sample.isw4;
s += CommaChar;

s += sample.osw1;
s += CommaChar;

for ( j=0; j<8; j++ )
    {
    s += (unsigned int) sample.analog[j];
    if ( j<7 )
        s += CommaChar;
    }
}

/***********************************************************************
*                            MAKE_CSV_LINE                             *
***********************************************************************/
static void make_csv_line( STRING_CLASS & s, FTII_TIME_DATA & sample )
{
int j;

s = sample.position;
s += TEXT(",,,," );

s += sample.isw1;
s += CommaChar;

s += sample.isw4;
s += CommaChar;

s += sample.osw1;
s += CommaChar;

for ( j=0; j<8; j++ )
    {
    s += (unsigned int) sample.analog[j];
    if ( j<7 )
        s += CommaChar;
    }
}

/***********************************************************************
*                             FTII_FILE_DATA                           *
*                               write_csv                              *
***********************************************************************/
BOOLEAN FTII_FILE_DATA::write_csv( STRING_CLASS & dest_file_path )
{
static TCHAR title1[] = TEXT("N Pos, N Time,Pt 1 ms,Timer Freq,Us per Sample,Int Shot No.,Shot Time" );
static TCHAR title2[] = TEXT( "Pos,Q1,Q2,Q3,Q4,ISW1,ISW4,OSW1,A1,A2,A3,A4,A5,A6,A7,A8" );
FILE_CLASS f;
int      i;

STRING_CLASS s;
TIME_CLASS   t;
TCHAR hex[MAX_INTEGER_LEN+1];

if ( !f.open_for_write( dest_file_path.text() ) )
    return FALSE;

f.writeline( title1 );

s = EmptyString;
s += np;
s += CommaString;

s += nt;
s += CommaString;

s += cs_to_point_1_ms;
s += CommaString;

s += timer_frequency;
s += CommaString;

s += us_per_time_sample;
s += CommaString;

s += internal_shot_number;
s += CommaString;

t.set( shot_time );
s += t.text();

f.writeline( s.text() );

f.writeline( title2 );

for ( i=0; i<np; i++ )
    {
    make_csv_line( s, position_sample[i] );
    f.writeline( s.text() );
    }

for ( i=0; i<nt; i++ )
    {
    make_csv_line( s, time_sample[i] );
    f.writeline( s.text() );
    }

for ( i=0; i<nof_extended_analogs; i++ )
    {
    s = EmptyString;
    s += i;
    s += CommaString;
    s += extended_analog[i].parameter_number;
    s += TEXT( ",0x" );
    hex[0] = NullChar;
    ultoascii( hex, (unsigned long) extended_analog[i].value, HEX_RADIX );
    s += hex;
    f.writeline( s.text() );
    }

f.close();
return TRUE;
}

/***********************************************************************
*                             FTII_FILE_DATA                           *
*                         write_raw_positions_csv                      *
***********************************************************************/
BOOLEAN FTII_FILE_DATA::write_raw_positions_csv( STRING_CLASS & dest_file_path )
{
static TCHAR title1[] = TEXT("N Pos, N Time,Pt 1 ms,Timer Freq,Int Shot No.,Shot Time" );
static TCHAR title2[] = TEXT( "Pos,Dir,Err,Quad,Count,Delta Count,Avg Count,Delta Avg,Std Dev" );

unsigned     avg;
FILE_CLASS   f;
int          i;
int          j;
int          k;
int          last_sample;
int          min_index;
int          n;
unsigned     old_avg;
int          pos;
STRING_CLASS s;
TIME_CLASS   t;
unsigned     x;
double       y;
double       sd;
double       sumx2;
double       sumx;

n           = np * X4_COUNTS_PER_MARK;
last_sample = X4_COUNTS_PER_MARK - 1;


/*
-------------------------------------------------------------
Find the first set of points with no errors
Start at i=1 because the zeroth set is invalid by definition.
------------------------------------------------------------- */
i = 1;
while ( i < np )
    {
    x = position_sample[i].velcount[0];
    for ( j=1; j<X4_COUNTS_PER_MARK; j++ )
        x |= position_sample[i].velcount[j];
    if ( (x & ERROR_MASK) == 0  )
        break;
    i++;
    }

x &= ERROR_MASK;

/*
-----------------------------------------------------------------
Get the index of the column containing the minimum counts per row
i still contains the index of the first valid set of vel counts.
----------------------------------------------------------------- */
min_index = 0;

/*
----------------------------------------------------------------------------------
If there was at least one good set of points, get the column with the lowest count
---------------------------------------------------------------------------------- */
if ( !x )
    {
    x = position_sample[i].velcount[0];
    for ( j=1; j<X4_COUNTS_PER_MARK; j++ )
        {
        if ( position_sample[i].velcount[j] < x )
            {
            x = position_sample[i].velcount[j];
            min_index = j;
            }
        }
    }

if ( !f.open_for_write( dest_file_path.text() ) )
    return FALSE;

f.writeline( title1 );

s = np;
s += CommaString;

s += cs_to_point_1_ms;
s += CommaString;

s += timer_frequency;
s += CommaString;

s += us_per_time_sample;
s += CommaString;

s += internal_shot_number;
s += CommaString;

t.set( shot_time );
s += t.text();

f.writeline( s.text() );

f.writeline( title2 );

old_avg = 0;
pos     = 0;
for ( i=0; i<np; i++ )
    {
    avg  = 0;
    k    = min_index;
    sumx = 0.0;
    sumx2 = 0.0;
    for ( j=0; j<X4_COUNTS_PER_MARK; j++ )
        {
        pos++;
        s = pos;
        s += CommaString;

        x = position_sample[i].velcount[k];

        if ( x & DIRECTION_MASK )
            s += YChar;
        s += CommaString;

        if ( x & ERROR_MASK )
            s += YChar;
        s += CommaString;

        s += k+1;
        s += CommaString;

        x &= TIMER_MASK;
        avg   += x;

        s += (unsigned int) x;
        s += CommaString;

        if ( i > 1 )
            {
            x -= position_sample[i-1].velcount[k] & TIMER_MASK;
            s += (unsigned int) x;
            y = double( x );
            sumx  += y;
            sumx2 += y * y;
            }
        s += CommaString;

        if ( j == last_sample )
            s += avg/X4_COUNTS_PER_MARK;
        s += CommaString;

        if ( j == last_sample )
            {
            x = avg - old_avg;
            s += x/X4_COUNTS_PER_MARK;
            old_avg = avg;
            sd = sqrt( (4.0 * sumx2 - sumx*sumx) / 12.0 );
            s += CommaString;
            s += sd;
            }

        f.writeline( s.text() );

        k++;
        if ( k >= X4_COUNTS_PER_MARK )
            k = 0;
        }
    }

f.close();
return TRUE;
}

/***********************************************************************
*                            FTII_ARRAY_DATA                           *
***********************************************************************/
FTII_ARRAY_DATA::FTII_ARRAY_DATA()
{
int i;

timer_frequency           = 16666666;
last_position_based_point = 0;
np = 0;
nt = 0;
n  = 0;
position  = 0;
velocity  = 0;
timer     = 0;
isw1      = 0;
isw4      = 0;
osw1      = 0;

for ( i=0; i<NOF_FTII_ANALOGS; i++ )
    {
    analog[i]  = 0;
    channel[i] = 0;
    }
}

/***********************************************************************
*                            FTII_ARRAY_DATA                           *
*                                 clear                                *
***********************************************************************/
void FTII_ARRAY_DATA::clear()
{
int i;

last_position_based_point = 0;
np = 0;
nt = 0;
n  = 0;
if ( position )
    {
    delete[] position;
    position = 0;
    }

if ( velocity )
    {
    delete[] velocity;
    velocity = 0;
    }

if ( timer )
    {
    delete[] timer;
    timer = 0;
    }

if ( isw1 )
    {
    delete[] isw1;
    isw1 = 0;
    }

if ( isw4 )
    {
    delete[] isw4;
    isw4 = 0;
    }

if ( osw1 )
    {
    delete[] osw1;
    osw1 = 0;
    }

for ( i=0; i<NOF_FTII_ANALOGS; i++ )
    {
    if ( analog[i] )
        {
        delete[] analog[i];
        analog[i] = 0;
        }
    channel[i] = 0;
    }
}

/***********************************************************************
*                           ~FTII_ARRAY_DATA                           *
***********************************************************************/
FTII_ARRAY_DATA::~FTII_ARRAY_DATA()
{
FTII_ARRAY_DATA::clear();
}

/***********************************************************************
*                           FTII_ARRAY_DATA                            *
*                              write_csv                               *
***********************************************************************/
void FTII_ARRAY_DATA::write_csv( TCHAR * destfile  )
{
static TCHAR title[] = TEXT( "Type,Sample,Pos,Timer,Velocity,Inputs,Outputs,A1,A2,A3,A4,A5,A6,A7,A8" );
FILE_CLASS f;
STRING_CLASS s;
const int last_channel = NOF_FTII_ANALOGS - 1;
int i;
int c;
TCHAR hex[MAX_INTEGER_LEN+1];

hex[0] = NullChar;

if ( !f.open_for_write(destfile) )
    return;

f.writeline( title );

for ( i=0; i<n; i++ )
    {
    if ( i < np )
        s = TEXT( "P," );
    else
        s = TEXT( "T," );
    s += i;
    s += CommaChar;

    s += position[i];
    s += CommaChar;

    s += timer[i];
    s += CommaChar;

    s += velocity[i];
    s += CommaChar;

    ultoascii( hex, isw1[i], HEX_RADIX );
    s.cat_w_char( hex, CommaChar );

    ultoascii( hex, osw1[i], HEX_RADIX );
    s.cat_w_char( hex, CommaChar );

    for ( c=0; c<NOF_FTII_ANALOGS; c++ )
        {
        s += analog[c][i];
        if ( c < last_channel )
            s += CommaString;
        }
    f.writeline( s );
    }

f.close();
}

/***********************************************************************
*                            FTII_ARRAY_DATA                           *
*                                  =                                   *
***********************************************************************/
void FTII_ARRAY_DATA::operator=( const FTII_FILE_DATA & sorc )
{
int analog_index;
bool * bad; /* true if timer[i] is invalid */
int first_good_index;
int i;
int j;
int k;
int min_index;
int na;
int ti;
int x1;
int x2;
int y;
int y1;
int y2;
unsigned sum;
unsigned x;
unsigned * dest;
__int64  tf64; /* Microsoft Specific Code */
__int64  x64;  /* Microsoft Specific Code */
__int64  dt;   /* Microsoft Specific Code */
__int64  us;   /* Microsoft Specific Code */

FTII_ARRAY_DATA::clear();

if ( !sorc.np || !sorc.nt )
    return;

timer_frequency = unsigned( sorc.timer_frequency );
if ( timer_frequency == 0 )
    timer_frequency = 16666666;

np = 1 + (sorc.np-1)*X4_COUNTS_PER_MARK;
nt = sorc.nt;
last_position_based_point = np - 1;
n = np + nt;

isw1     = new unsigned[n];
isw4     = new unsigned[n];
osw1     = new unsigned[n];

for ( i=0; i<NOF_FTII_ANALOGS; i++ )
    analog[i] = new unsigned[n];

/*
---------
Positions
--------- */
position = new int[n];
for ( i=0; i<np; i++ )
    position[i] = i;

j = np;
for ( i=0; i<nt; i++ )
    {
    position[j] = sorc.time_sample[i].position;
    j++;
    }

/*
------------
Timer counts
------------ */
timer = new unsigned[n];
bad   = new bool[n];

/*
----------------------------------------------------------
The zeroth point and the first set of points are not valid
Zero them now. They will be interpolated later.
---------------------------------------------------------- */
i = 0;
while ( i<=X4_COUNTS_PER_MARK )
    {
    timer[i] = 0;
    bad[i] = true;
    i++;
    }

while ( i < n )
    {
    bad[i] = false;
    i++;
    }

/*
-------------------------------------------------------------
Find the first set of points with no errors
Start at i=1 because the zeroth set is invalid by definition.
------------------------------------------------------------- */
i = 1;
while ( i < sorc.np )
    {
    x = sorc.position_sample[i].velcount[0];
    for ( j=1; j<X4_COUNTS_PER_MARK; j++ )
        x |= sorc.position_sample[i].velcount[j];
    if ( (x & ERROR_MASK) == 0  )
        break;
    i++;
    }

/*
----------------------------------
If all the points are bad, give up
---------------------------------- */
if ( x & ERROR_MASK )
    {
    delete[] bad;
    bad = 0;
    return;
    }

/*
-----------------------------------------------------------------
Get the index of the column containing the minimum counts per row
i still contains the index of the first valid set of vel counts.
----------------------------------------------------------------- */
min_index = 0;

x = sorc.position_sample[i].velcount[0];
for ( j=1; j<X4_COUNTS_PER_MARK; j++ )
    {
    if ( sorc.position_sample[i].velcount[j] < x )
        {
        x = sorc.position_sample[i].velcount[j];
        min_index = j;
        }
    }

/*
-------------------------------------------------------
Skip the first X4COUNTS_PER_MARK points.
The program will skip one more before writing anything.
These were zeroed and marked bad already.
------------------------------------------------------- */
ti = X4_COUNTS_PER_MARK;

/*
-------------------------------------------
Copy the velocity counts to the timer array
and mark any that are bad.
------------------------------------------- */
for ( i=1; i<sorc.np; i++ )
    {
    k = min_index;
    for ( j=0; j<X4_COUNTS_PER_MARK; j++ )
        {
        ti++;
        timer[ti] = sorc.position_sample[i].velcount[k];
        if ( timer[ti] & ERROR_MASK )
            bad[ti] = true;
        timer[ti] &= TIMER_MASK;
        k++;
        if ( k >= X4_COUNTS_PER_MARK )
            k = 0;
        }
    }

first_good_index = 0;

/*
-----------------------------------------------------
Subtract the quadrature value from the previous point
----------------------------------------------------- */
i = last_position_based_point;
j = i - 4;
while ( i>4 )
    {
    if ( !bad[i] && !bad[j] )
        {
        timer[i] -= timer[j];
        if ( timer[i] != 0 )
            first_good_index = i;
        }
    else
        {
        bad[i] = true;
        }
    j--;
    i--;
    }

/*
-------------------------------------------------
Make a smooth line from 0 to the first good point
------------------------------------------------- */
x = timer[first_good_index];
for ( i=1; i<first_good_index; i++ )
    {
    j = first_good_index - i;
    j *= 2;
    timer[i] = unsigned(j) * x;
    bad[i] = false;
    }

/*
------------------------------------------------
Replace any zero values with interpolated values
------------------------------------------------ */
for ( j=first_good_index+1; j<np; j++ )
    {
    if ( timer[j] == 0 || bad[j] )
        {
        x1 = j - 1;
        y1 = (int) timer[i];
        x2 = j+1;
        while ( x2 < np )
            {
            if ( timer[x2] != 0 && !bad[x2] )
                {
                y2 = (int) timer[x2];
                for ( i=j; i<x2; i++ )
                    {
                    y = (i - x1) * (y2 - y1);
                    y /= (x2 - x1);
                    y += y1;
                    timer[i] = (unsigned) y;
                    bad[i]   = false;
                    }
                break;
                }
            else if ( k == (np - 1) )
                {
                for ( i=j; i<np; i++ )
                    {
                    timer[i] = y1;
                    bad[i]   = false;
                    }
                break;
                }
            x2++;
            }
        }
    /*
    ------------------------------------------------------------------------------
    If it is still zero there is something wrong but at least don't leave it zero.
    ------------------------------------------------------------------------------ */
    if ( timer[j] == 0 )
        timer[j] = 1;
    }

/*
---------------------------------------------------------------
The velocity is calculated from the differences so do these now
--------------------------------------------------------------- */
velocity = new int[n];

sum = (unsigned) sorc.timer_frequency;
sum *= 4;
velocity[0] = 0;
for ( i=1; i<np; i++ )
    velocity[i] = int( sum / timer[i] );

/*
------------------------------------------------------------------------
Add them back up so that each represents the time from the start of shot
------------------------------------------------------------------------ */
x64 = 0;
for ( i=1; i<np; i++ )
    {
    x64 += (__int64) timer[i];
    timer[i] = unsigned( x64/4 );
    }

/*
----------------------------------------------------------------------------------------
Fill in the rest of the array. Each is timer_frequency*nof_us_per_sample/1000000 counts.
Use a 64 bit counter and multiply by 1000000 to maintain precision.
---------------------------------------------------------------------------------------- */
tf64 =  (__int64) sorc.timer_frequency;
tf64 *= (__int64) sorc.us_per_time_sample;

x64  = (__int64) timer[last_position_based_point];
x64  *= 1000000;

for ( i=np; i<n; i++ )
    {
    x64 += tf64;
    timer[i] = unsigned( x64/1000000 );
    }

/*
--------------------------------------------------------------------------
                                Velocity
The velocity of the time base  points are just dx/dt. if dx is zero I keep
going and average the velocity when I get a dx > 0;
-------------------------------------------------------------------------- */
us   =  (__int64) sorc.us_per_time_sample;
dt   =  0;
i = last_position_based_point;
for ( j=np; j<n; j++ )
    {
    x64 =  position[j];
    x64 -= position[i];
    dt += us;
    if ( x64 != 0 || j==(n-1) )
        {
        if ( x64 != 0 )
            {
            x64 *= 1000000;
            x64 /= dt;
            }
        while ( i < j )
            {
            i++;
            velocity[i] = (int) x64;
            }
        dt = 0;
        }
    }

//write_csv();

/*
----------
I/O Arrays
---------- */
isw1[0] = sorc.position_sample[1].isw1;
isw4[0] = unsigned( sorc.position_sample[1].isw4 );
osw1[0] = sorc.position_sample[1].osw1;
k = 1;
for ( i=1; i<sorc.np; i++ )
    {
    for ( j=0; j<X4_COUNTS_PER_MARK; j++ )
        {
        isw1[k] = sorc.position_sample[i].isw1;
        isw4[k] = unsigned( sorc.position_sample[i].isw4 );
        osw1[k] = sorc.position_sample[i].osw1;
        k++;
        }
    }

for ( i=0; i<sorc.nt; i++ )
    {
    isw1[k] = sorc.time_sample[i].isw1;
    isw4[k] = unsigned( sorc.time_sample[i].isw4 );
    osw1[k] = sorc.time_sample[i].osw1;
    k++;
    }

/*
-------------
Analog Arrays
------------- */
for ( analog_index=0; analog_index<NOF_FTII_ANALOGS; analog_index++ )
    {
    /*
    --------------------------------------------------------------
    First copy the points into an array so I can fix any mistakes.
    -------------------------------------------------------------- */
    dest = analog[analog_index];
    k=0;
    for ( i=1; i<sorc.np; i++ )
        {
        dest[k++] = sorc.position_sample[i].analog[analog_index] & VALUE_MASK;
        }

    for ( i=0; i<sorc.nt; i++ )
        dest[k++] = sorc.time_sample[i].analog[analog_index] & VALUE_MASK;

    na = k;

    /*
    ------------------------------------------------------------------
    Expand the points, making four copies of each position based point
    ------------------------------------------------------------------ */
    k = n - 1;
    j = na - 1;

    for ( i=0; i<sorc.nt; i++ )
        dest[k--] = dest[j--];

    for ( i=1; i<sorc.np; i++ )
        {
        dest[k--] = dest[j];
        dest[k--] = dest[j];
        dest[k--] = dest[j];
        dest[k--] = dest[j--];
        }
    }

delete[] bad;
bad = 0;
}

/***********************************************************************
*                           FTII_ARRAY_DATA                            *
*                        interpolated_timer_count                      *
***********************************************************************/
unsigned FTII_ARRAY_DATA::interpolated_timer_count( double p )
{
unsigned counts;
int i1;
int i2;

counts = 0;
i1 = (int) floor( p );
i2 = (int) ceil( p );

if ( i2 > last_position_based_point )
    i1 = i2 = last_position_based_point;

if ( i1 >= 0 && i2 <= last_position_based_point )
    {
    if ( i1 == i2 )
        {
        counts = timer[i1];
        }
    else
        {
        counts = timer[i2] - timer[i1];
        p -= i1;    /* I dont have to divide because i2 = i1 + 1 */
        p *= counts;
        counts = timer[i1] + (int) p;
        }
    }

return counts;
}

/***********************************************************************
*                           FTII_ARRAY_DATA                            *
*                        interpolated_velocity                         *
*                               x4/sec                                 *
***********************************************************************/
double FTII_ARRAY_DATA::interpolated_velocity( double p )
{
int i1;
int i2;
double t1;
double t2;
double t;

i1 = (int) floor( p );
if ( i1 < 0 )
    i1 = 0;
else if ( i1 >= n )
    i1 = n-1;

i2 = (int) ceil( p );
if ( i2 < 0 )
    i2 = 0;
else if ( i2 >= n )
    i2 = n-1;

if ( i1 == 0 || i1 == i2 )
    return double( velocity[i2] );
else if ( i2 >- last_position_based_point )
    return double( velocity[i1] );

t1 = double( timer[i1] ) - double( timer[i1-1] );
t2 = double( timer[i2] ) - double( timer[i1]  );

t = t1 + (p-double(i1)) * (t2-t1);

if ( not_zero(t) )
    t = timer_frequency/t;

return t;
}

/***********************************************************************
*                           FTII_ARRAY_DATA                            *
*                        velocity_between_positions                    *
*                               x4/sec                                 *
***********************************************************************/
double FTII_ARRAY_DATA::velocity_between_positions( double p1, double p2 )
{
unsigned u;
double   x;

/*
-----------------------------------------------------------------------
The following code restricts the calculations to position based points.
This is necessary because the timer_counts routine does this. 4/10/08
----------------------------------------------------------------------- */
x = (double) last_position_based_point;
if ( x < p2 )
    p2 = x;

if ( p1 >= p2 )
    p1 = (double) (last_position_based_point - 1);

u = timer_counts_between_positions( p1, p2 );
x = p2 - p1;
if ( u )
    {
    x *= double( timer_frequency );
    x /= double(u);
    }

return x;
}

/***********************************************************************
*                           FTII_ARRAY_DATA                            *
*                         max_average_velocity                         *
*                               x4/sec                                 *
***********************************************************************/
double FTII_ARRAY_DATA::max_average_velocity( int & max_index, int start_index, int end_index, int nof_points_on_each_side, double multiplier )
{
double max_velocity;
double x;
double vsum;
double vdiv;
int    count;
int    i;

max_velocity = 0.0;

if ( start_index < 0 )
    start_index = 0;
if ( end_index > last_position_based_point )
    end_index =  last_position_based_point;

max_index = start_index;
for ( i=start_index; i<=end_index; i++ )
    {
    if ( (nof_points_on_each_side <= 0) || (i < 2) || (i == last_position_based_point) )
        {
        x = double( velocity[i] );
        }
    else
        {
        count = i-1;
        if ( count > nof_points_on_each_side )
            count = last_position_based_point - i;

        if ( count > nof_points_on_each_side )
            count = nof_points_on_each_side;

        vsum = 0.0;
        vdiv = 0.0;
        while ( count > 0 )
            {
            vsum += velocity[i-count];
            vsum += velocity[i+count];
            vsum *= multiplier;

            vdiv += 2.0;
            vdiv *= multiplier;

            count--;
            }
        vsum += velocity[i];
        vdiv += 1.0;
        x = vsum/vdiv;
        }

    if ( x > max_velocity )
        {
        max_velocity = x;
        max_index    = i;
        }
    }

return max_velocity;
}

/***********************************************************************
*                           FTII_ARRAY_DATA                            *
*                         index_from_position (x4 counts)              *
*                             pre impact                               *
***********************************************************************/
int FTII_ARRAY_DATA::index_from_position( double p )
{
int i;

i = (int) round( p, 1.0 );

if ( i < 0 )
    i = 0;
if ( i >= np )
    i = np - 1;

return i;
}

/***********************************************************************
*                           FTII_ARRAY_DATA                            *
*                           index_from_time                            *
*                             post impact                              *
***********************************************************************/
int FTII_ARRAY_DATA::index_from_time( double ms_past_impact )
{
int i;
int j;
unsigned t;

ms_past_impact *= double( timer_frequency );
ms_past_impact /= 1000.0;
t = unsigned( round(ms_past_impact, 1.0) );
t += timer[last_position_based_point];

j=last_position_based_point;
for ( i=0; i<nt-1; i++ )
    {
    if ( timer[j] <= t && timer[j+1] > t )
        break;
    j++;
    }

return j;
}

/***********************************************************************
*                             FTII_PROFILE                             *
*                                clear                                 *
***********************************************************************/
void FTII_PROFILE::clear()
{
timer_frequency    = 16666666;
nof_analogs        = 0;
nof_curves         = 0;
us_per_time_sample = 1000;

if ( curvedata )
    {
    delete[] curvedata;
    curvedata = 0;
    }

if ( parameter )
    {
    delete[] parameter;
    parameter = 0;
    }

nof_extended_analogs = 0;
if ( extended_analog )
    {
    delete[] extended_analog;
    extended_analog = 0;
    }

nof_text_parameters = 0;
if ( text_parameter )
    {
    delete[] text_parameter;
    text_parameter = 0;
    }

FTII_ARRAY_DATA::clear();
}

/***********************************************************************
*                             FTII_PROFILE                             *
***********************************************************************/
FTII_PROFILE::FTII_PROFILE()
{
nof_analogs        = 0;
nof_curves         = 0;
us_per_time_sample = 1000;
timer_frequency    = 16666666;
parameter          = 0;
curvedata          = 0;
nof_extended_analogs    = 0;
extended_analog         = 0;
nof_text_parameters = 0;
text_parameter      = 0;
}

/***********************************************************************
*                            ~FTII_PROFILE                             *
***********************************************************************/
FTII_PROFILE::~FTII_PROFILE()
{
FTII_PROFILE::clear();
}

/***********************************************************************
*                             FTII_PROFILE                             *
*                              curve_index                             *
***********************************************************************/
int FTII_PROFILE::curve_index( int curve_type_to_find )
{
int i;

if ( curvedata )
    {
    for ( i=0; i<nof_curves; i++ )
        {
        if ( curvedata[i].curve_type == curve_type_to_find )
            return i;
        }
    }

return NO_INDEX;
}

/***********************************************************************
*                            FTII_PROFILE                              *
*                         ms_between_positions                         *
*                   p1 and p2 are positions in x4 counts               *
***********************************************************************/
double FTII_PROFILE::ms_between_positions( double p1, double p2 )
{
double m;

m = (double) timer_frequency;
m /= 1000;
m = timer_counts_between_positions( p1, p2 ) / m;

return m;
}

/***********************************************************************
*                            FTII_PROFILE                              *
*                  INTERPOLATED_POSITION_FROM_VELOCITY                 *
*           target_velocity units are x4 counts per second             *
*              i1 is starting index; i2 is ending index                *
***********************************************************************/
double FTII_PROFILE::interpolated_postion_from_velocity( int target_velocity, int i1, int i2 )
{
double x;

x  = (double) (velocity[i2] - target_velocity);
x /= (double) (velocity[i2] - velocity[i1]);
x *= (double) (position[i2] - position[i1]);
x += (double) position[i1];

return x;
}

/***********************************************************************
*                            WRITE_TO_CSV                              *
***********************************************************************/
void FTII_PROFILE::write_to_csv( TCHAR * filename )
{
FILE_CLASS   f;
STRING_CLASS s;
int i;
int j;

if ( f.open_for_write(filename) )
    {
    f.writeline( TEXT("Index,Pos,Velocity,Timer Count,Isw1,Isw4,Osw1,A1,A2,A3,A4,A5,A6,A7,A8") );

    for ( i=0; i<n; i++ )
        {
        s = int32toasc(i);
        s += TEXT(",");
        s += position[i];
        s += TEXT(",");
        s += velocity[i];
        s += TEXT(",");
        s += timer[i];
        s += TEXT(",");
        s += isw1[i];
        s += TEXT(",");
        s += isw4[i];
        s += TEXT(",");
        s += osw1[i];
        s += TEXT(",");
        for ( j=0; j<nof_analogs; j++ )
            {
            s += analog[j][i];
            if ( j < (nof_analogs-1) )
                s += TEXT(",");
            }

        f.writeline( s.text() );
        }

    f.close();
    }
}

/***********************************************************************
*                             FTII_PROFILE                             *
*                                  =                                   *
***********************************************************************/
void FTII_PROFILE::operator=( const FTII_FILE_DATA & sorc )
{
int i;

FTII_ARRAY_DATA::operator=(sorc);

nof_analogs        = NOF_FTII_ANALOGS;
nof_curves         = NOF_FTII_FILE_CURVES;
shot_time          = sorc.shot_time;
us_per_time_sample = sorc.us_per_time_sample;

/*
--------------------------------------------------------
Nof_curves is constant so I don't have to realocate this
-------------------------------------------------------- */
if ( !curvedata )
    curvedata = new FTII_MIN_MAX_DATA[nof_curves];

if ( curvedata )
    {
    for ( i=0; i<nof_curves; i++ )
        curvedata[i] = sorc.curvedata[i];
    }

if ( !parameter )
    parameter = new FTII_PARAMETER_ENTRY[NOF_FTII_FILE_PARAMETERS];

if ( parameter )
    {
    parameter[0].index  = CYCLE_TIME_PARM;
    parameter[0].ivalue = sorc.parameter[0];

    parameter[1].index  = BISCUIT_SIZE;
    parameter[1].ivalue = sorc.parameter[1];

    parameter[2].index  = P3_POS_PARM;
    parameter[2].ivalue = sorc.parameter[2];

    for ( i=3; i<NOF_FTII_FILE_PARAMETERS; i++ )
        {
        parameter[i].index  = STATIC_ANALOG_PARAMETER;
        parameter[i].ivalue = sorc.parameter[i];
        }
    }

if ( nof_extended_analogs > 0 && nof_extended_analogs != sorc.nof_extended_analogs )
    {
    nof_extended_analogs = 0;
    delete[] extended_analog;
    extended_analog = 0;
    }

if ( sorc.nof_extended_analogs > 0 )
    {
    if ( !nof_extended_analogs )
        extended_analog = new EXTENDED_ANALOG_ENTRY[sorc.nof_extended_analogs];

    if ( extended_analog )
        {
        nof_extended_analogs = sorc.nof_extended_analogs;
        for ( i=0; i<nof_extended_analogs; i++ )
            extended_analog[i] = sorc.extended_analog[i];
        }
    }

if ( nof_text_parameters > 0 && nof_text_parameters != sorc.nof_text_parameters )
    {
    nof_text_parameters = 0;
    delete[] text_parameter;
    text_parameter = 0;
    }

if ( sorc.nof_text_parameters > 0 )
    {
    if ( !nof_text_parameters )
        text_parameter = new TEXT_PARAMETER_ENTRY[sorc.nof_text_parameters];

    if ( text_parameter )
        {
        nof_text_parameters = sorc.nof_text_parameters;
        for ( i=0; i<nof_text_parameters; i++ )
            text_parameter[i] = sorc.text_parameter[i];
        }
    }
}

/***********************************************************************
*                             FTII_PROFILE                             *
*                                  get                                 *
***********************************************************************/
BOOLEAN FTII_PROFILE::get( STRING_CLASS & sorc_file_path )
{
int            i;
FTII_FILE_DATA f;

FTII_PROFILE::clear();

if ( !f.get(sorc_file_path) )
    return FALSE;

if ( !f.np || !f.nt )
    return FALSE;

(*this) = f;

if ( nof_extended_analogs > 0 && nof_extended_analogs != f.nof_extended_analogs )
    {
    nof_extended_analogs = 0;
    delete[] extended_analog;
    extended_analog = 0;
    }

if ( f.nof_extended_analogs > 0 )
    {
    if ( !nof_extended_analogs )
        extended_analog = new EXTENDED_ANALOG_ENTRY[f.nof_extended_analogs];

    if ( extended_analog )
        {
        nof_extended_analogs = f.nof_extended_analogs;
        for ( i=0; i<nof_extended_analogs; i++ )
            extended_analog[i] = f.extended_analog[i];
        }
    }

if ( nof_text_parameters > 0 && nof_text_parameters != f.nof_text_parameters )
    {
    nof_text_parameters = 0;
    delete[] text_parameter;
    text_parameter = 0;
    }

if ( f.nof_text_parameters > 0 )
    {
    if ( !nof_text_parameters )
        text_parameter = new TEXT_PARAMETER_ENTRY[f.nof_text_parameters];

    if ( text_parameter )
        {
        nof_text_parameters = f.nof_text_parameters;
        for ( i=0; i<nof_text_parameters; i++ )
            text_parameter[i] = f.text_parameter[i];
        }
    }

/*
------------------------------------------------------
If there is no velocity array, I did not read any data
------------------------------------------------------ */
if ( !velocity )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                             FTII_PROFILE                             *
*                                  ms                                  *
***********************************************************************/
double FTII_PROFILE::ms( int timer_index )
{
double x;

x = double( timer[timer_index] );
x *= 1000.0;
x /= double( timer_frequency );

return x;
}

/***********************************************************************
*                             FTII_PROFILE                             *
*                           interpolated_ms                            *
***********************************************************************/
double FTII_PROFILE::interpolated_ms( double timer_index )
{
double x;

x = interpolated_timer_count( timer_index );
x *= 1000.0;
x /= double( timer_frequency );

return x;
}
