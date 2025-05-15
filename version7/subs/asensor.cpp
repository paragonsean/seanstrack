#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"
#include "..\include\chars.h"

static const double MAX_RAW_COUNTS = 4095.0;
static const int MIN_ANALOG_SENSOR_COUNT = DEFAULT_FT2_ANALOG_SENSOR + 1;

static ANALOG_SENSOR_ENTRY EmptySensor;

static ANALOG_SENSOR_ENTRY * SensorArray = 0;
static int                   NofSensors  = 0;

/***********************************************************************
*                          KILL_SENSOR_ARRAY                           *
***********************************************************************/
static void kill_sensor_array()
{
ANALOG_SENSOR_ENTRY * copy;

NofSensors  = 0;
if ( SensorArray )
    {
    copy = SensorArray;
    SensorArray = 0;
    delete[] copy;
    }
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
***********************************************************************/
ANALOG_SENSOR_ENTRY::ANALOG_SENSOR_ENTRY()
{
units           = VOLTS_UNITS;
max_volts_level = NO_MAX_VOLTS;
vartype         = NO_VARIABLE_TYPE;
max_volts       = 10.0;
m               = 1.0;
b               = 0.0;
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
***********************************************************************/
ANALOG_SENSOR_ENTRY::~ANALOG_SENSOR_ENTRY()
{
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                                 =                                    *
***********************************************************************/
ANALOG_SENSOR_ENTRY & ANALOG_SENSOR_ENTRY::operator=( const ANALOG_SENSOR_ENTRY & sorc )
{
units           = sorc.units;
max_volts_level = sorc.max_volts_level;
vartype         = sorc.vartype;
desc            = sorc.desc;
low_volts       = sorc.low_volts;
low_value       = sorc.low_value;
high_volts      = sorc.high_volts;
high_value      = sorc.high_value;
max_volts       = sorc.max_volts;
m               = sorc.m;
b               = sorc.b;

return (*this);
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                                ==                                    *
*           This checks everything except the description.             *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_ENTRY::operator==( const ANALOG_SENSOR_ENTRY & sorc )
{
if ( units != sorc.units )
    return FALSE;
if ( vartype != sorc.vartype )
    return FALSE;
if ( low_volts != sorc.low_volts )
    return FALSE;
if ( low_value != sorc.low_value )
    return FALSE;
if ( high_volts != sorc.high_volts )
    return FALSE;
if ( high_value != sorc.high_value )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                              update                                  *
*                  Update the max_volts and equation.                  *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_ENTRY::update()
{
double x1;
double x2;
double y1;
double y2;

x1 = low_volts.real_value();
x2 = high_volts.real_value();
y1 = low_value.real_value();
y2 = high_value.real_value();
if ( x2 > 5.001 )
    {
    max_volts_level = TEN_MAX_VOLTS;
    max_volts = 10.0;
    }
else if ( x2 > 2.501 )
    {
    max_volts_level = FIVE_MAX_VOLTS;
    max_volts = 5.0;
    }
else
    {
    max_volts_level = TWO_POINT_FIVE_MAX_VOLTS;
    max_volts = 2.5;
    }

/*
----------------------------------------------------------------
The equation is y = mx + b where x is the voltage in raw counts.
---------------------------------------------------------------- */
x1 *= MAX_RAW_COUNTS;
x1 /= max_volts;
x2 *= MAX_RAW_COUNTS;
x2 /= max_volts;
if ( x1 == x2 )
    {
    m = 1;
    b = 0;
    return FALSE;
    }
m = (y2 - y1) / (x2 - x1);
b = y1 - m*x1;

return TRUE;
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                                 get                                  *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_ENTRY::get( TCHAR * sorc )
{
if ( !sorc )
    return FALSE;
/*
------------------------------------------------------------------------------------------------
The first field is the number which is the same as the index. Skip this as it is only for humans
------------------------------------------------------------------------------------------------ */
replace_char_with_null( sorc, CommaChar );
sorc = nextstring( sorc );
if ( !sorc )
    return FALSE;
replace_char_with_null(sorc, CommaChar );
units = (int) asctoint32(sorc);

sorc = nextstring( sorc );
if ( !sorc )
    return FALSE;
replace_char_with_null(sorc, CommaChar );
vartype = (BITSETYPE) asctoul(sorc);

sorc = nextstring( sorc );
if ( !sorc )
    return FALSE;
replace_char_with_null(sorc, CommaChar );
desc = sorc;

sorc = nextstring( sorc );
if ( !sorc )
    return FALSE;
replace_char_with_null(sorc, CommaChar );
low_volts = sorc;

sorc = nextstring( sorc );
if ( !sorc )
    return FALSE;
replace_char_with_null(sorc, CommaChar );
low_value = sorc;

sorc = nextstring( sorc );
if ( !sorc )
    return FALSE;
replace_char_with_null(sorc, CommaChar );
high_volts = sorc;

sorc = nextstring( sorc );
if ( !sorc )
    return FALSE;
high_value = sorc;

return update();
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                                 put                                  *
*                                                                      *
* This appends the record. I assume the string currently has the       *
* index number and nothing else.                                       *
*                                                                      *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_ENTRY::put( STRING_CLASS & dest )
{
dest += CommaChar;
dest += units;
dest += CommaChar;
dest += vartype;
dest += CommaChar;
dest += desc;
dest += CommaChar;
dest += low_volts;
dest += CommaChar;
dest += low_value;
dest += CommaChar;
dest += high_volts;
dest += CommaChar;
dest += high_value;

return TRUE;
}


/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                            max_volts_text                            *
***********************************************************************/
TCHAR * ANALOG_SENSOR_ENTRY::max_volts_text()
{
static TCHAR * tp[] = { TEXT("2.5"), TEXT("5.0"), TEXT("10.0") };
int i;
i = max_volts_level - 1;
if ( i < 0 || i > 2 )
    i = 0;
return tp[i];
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                           converted_value                            *
***********************************************************************/
double ANALOG_SENSOR_ENTRY::converted_value( double raw_analog_counts )
{
double y;

y = raw_analog_counts;
y *= m;
y += b;

return y;
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                           converted_value                            *
***********************************************************************/
double ANALOG_SENSOR_ENTRY::converted_value( unsigned raw_analog_counts )
{
double y;

y = (double) raw_analog_counts;
return converted_value( y );
}


/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                               raw_count                              *
*                              r = (y - b)/m                           *
***********************************************************************/
double ANALOG_SENSOR_ENTRY::raw_count( double r )
{
r -= b;
if ( not_zero(m) )
    r /= m;

return r;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
***********************************************************************/
ANALOG_SENSOR::ANALOG_SENSOR()
{
x = NO_INDEX;
}

/***********************************************************************
*                            ~ANALOG_SENSOR                            *
***********************************************************************/
ANALOG_SENSOR::~ANALOG_SENSOR()
{
}

/***********************************************************************
*                            ANALOG_SENSOR                             *
*                               shutdown                               *
***********************************************************************/
void ANALOG_SENSOR::shutdown()
{
kill_sensor_array();
}

/***********************************************************************
*                            ANALOG_SENSOR                             *
*                                 get                                  *
* I always make an array of at least MIN_ANALOG_SENSOR_COUNT sensors.  *
* This means I can always use the default sensor.                      *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::get()
{
FILE_CLASS f;
int        i;
int        n;
NAME_CLASS s;
BOOLEAN    status;

kill_sensor_array();

status = TRUE;
i = 0;
s.get_data_dir_file_name( ASENSOR_CSV );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
        {
        n = f.nof_lines();
        if ( n > 0 )
            {
            if ( n < MIN_ANALOG_SENSOR_COUNT )
                n = MIN_ANALOG_SENSOR_COUNT;
            SensorArray = new ANALOG_SENSOR_ENTRY[n];
            if ( SensorArray )
                {
                i = 0;
                while ( i<n )
                    {
                    s = f.readline();
                    if ( s.isempty() )
                        break;
                    SensorArray[i].get( s.text() );
                    i++;
                    }
                }
            }
        f.close();
        }
    }
else
    {
    n = MIN_ANALOG_SENSOR_COUNT;
    SensorArray = new ANALOG_SENSOR_ENTRY[n];
    if ( !SensorArray )
        status = FALSE;
    }

if ( status )
    {
    while ( i<MIN_ANALOG_SENSOR_COUNT )
        {
        SensorArray[i].desc = TEXT( "Not Used" );
        i++;
        }
    NofSensors = i;
    }

return status;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                  put                                 *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::put()
{
NAME_CLASS s;
FILE_CLASS f;
int        i;

s.get_data_dir_file_name( ASENSOR_CSV );
if ( f.open_for_write(s) )
    {
    for ( i=0; i<NofSensors; i++ )
        {
        /*
        -----------------------------------------------------------------------
        Put the index number in the string. The entry put() will append to this
        ----------------------------------------------------------------------- */
        s = i;
        if ( SensorArray[i].put(s) )
            f.writeline( s );
        }
    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                               setsize                                *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::setsize( int new_n )
{
int n;
int i;
ANALOG_SENSOR_ENTRY * ap;

if ( new_n == NofSensors )
    return TRUE;

if ( new_n < MIN_ANALOG_SENSOR_COUNT )
    new_n = MIN_ANALOG_SENSOR_COUNT;

ap = new ANALOG_SENSOR_ENTRY[new_n];
if ( !ap )
    return FALSE;

n = new_n;
if ( n < NofSensors )
    n = NofSensors;

for ( i=0; i<n; i++ )
    ap[i] = SensorArray[i];

kill_sensor_array();
SensorArray = ap;
NofSensors  = new_n;
return TRUE;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                 count                                *
***********************************************************************/
int ANALOG_SENSOR::count()
{
return NofSensors;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                  []                                  *
***********************************************************************/
ANALOG_SENSOR_ENTRY & ANALOG_SENSOR::operator[]( int i )
{
if ( i>=0 && i<NofSensors )
    return SensorArray[i];

return EmptySensor;
}


/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                 index                                *
***********************************************************************/
int ANALOG_SENSOR::index( TCHAR * sorc_desc )
{
int i;
for ( i=0; i<NofSensors; i++ )
    {
    if ( SensorArray[i].desc == sorc_desc )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                 find                                 *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::find( TCHAR * sorc_desc )
{
int i;

i = index( sorc_desc );
if ( i == NO_INDEX )
    {
    x = DEFAULT_FT2_ANALOG_SENSOR;
    return FALSE;
    }

x = i;
return TRUE;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                 find                                 *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::find( int index_to_find )
{
if ( index_to_find>=0 && index_to_find<NofSensors )
    {
    x = index_to_find;
    return TRUE;
    }

x = DEFAULT_FT2_ANALOG_SENSOR;
return FALSE;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                 exists                               *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::exists( int index_to_find )
{
if ( index_to_find>=0 && index_to_find<NofSensors )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                 next                                 *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::next()
{
if ( x == NO_INDEX && NofSensors > 0 )
    {
    x = 0;
    return TRUE;
    }

if ( x < (NofSensors-1) )
    {
    x++;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                rewind                                *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::rewind()
{
x = NO_INDEX;
return TRUE;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                           converted_value                            *
***********************************************************************/
double ANALOG_SENSOR::converted_value( double raw_analog_counts )
{
if ( x != NO_INDEX )
    return SensorArray[x].converted_value( raw_analog_counts );

return 0.0;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                           converted_value                            *
***********************************************************************/
double ANALOG_SENSOR::converted_value( unsigned raw_analog_counts )
{
if ( x != NO_INDEX )
    return SensorArray[x].converted_value( raw_analog_counts );

return 0.0;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                               raw_count                              *
***********************************************************************/
double ANALOG_SENSOR::raw_count( double y )
{
if ( x != NO_INDEX )
    return SensorArray[x].raw_count( y );

return 0;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                              units_index                             *
***********************************************************************/
int ANALOG_SENSOR::units_index()
{
if ( x != NO_INDEX )
    return SensorArray[x].units;

return VOLTS_UNITS;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                vartype                               *
***********************************************************************/
unsigned ANALOG_SENSOR::vartype()
{
if ( x != NO_INDEX )
    return SensorArray[x].vartype;

return NO_VARIABLE_TYPE;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                               high_value                             *
***********************************************************************/
double ANALOG_SENSOR::high_value()
{
if ( x != NO_INDEX )
    return SensorArray[x].high_value.real_value();

return 1.0;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                low_value                             *
***********************************************************************/
double ANALOG_SENSOR::low_value()
{
if ( x != NO_INDEX )
    return SensorArray[x].low_value.real_value();

return 0.0;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                               max_volts                              *
***********************************************************************/
double ANALOG_SENSOR::max_volts()
{
if ( x != NO_INDEX )
    return SensorArray[x].max_volts;

return 5.0;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                desc                                  *
***********************************************************************/
TCHAR * ANALOG_SENSOR::desc()
{
if ( x != NO_INDEX )
    return SensorArray[x].desc.text();

return EmptyString;
}
