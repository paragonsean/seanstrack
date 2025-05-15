#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"
#include "..\include\chars.h"

static const double MAX_RAW_COUNTS = 4095.0;
static const int MIN_ANALOG_SENSOR_COUNT = DEFAULT_FT2_ANALOG_SENSOR + 1;

static ANALOG_SENSOR_RUN_CLASS EmptySensor;

static ANALOG_SENSOR_RUN_CLASS * SensorArray = 0;
static int                   NofSensors  = 0;

/***********************************************************************
*                       ANALOG_SENSOR_EDIT_CLASS                       *
                              clear_points
***********************************************************************/
void ANALOG_SENSOR_EDIT_CLASS::clear_points()
{
STRING_CLASS * myvolts;
STRING_CLASS * myvalue;

myvolts = volts;
myvalue = value;

n     = 0;
volts = 0;
value = 0;

if ( myvolts )
    delete[] myvolts;

if ( value )
    delete[] myvalue;
}

/***********************************************************************
*                       ANALOG_SENSOR_EDIT_CLASS                       *
*                             make_points                              *
***********************************************************************/
bool ANALOG_SENSOR_EDIT_CLASS::make_points( int nof_points )
{
int i;

if ( n != nof_points )
    {
    clear_points();
    if ( nof_points > 0 )
        {
        volts = new STRING_CLASS[nof_points];
        value = new STRING_CLASS[nof_points];
        n = nof_points;
        }
    }
else if ( nof_points > 0 )
    {
    for ( i=0; i<n; i++ )
        {
        volts[i].empty();
        value[i].empty();
        }
    }
}

/***********************************************************************
*                       ANALOG_SENSOR_EDIT_CLASS                       *
***********************************************************************/
ANALOG_SENSOR_EDIT_CLASS::ANALOG_SENSOR_EDIT_CLASS()
{
units           = VOLTS_UNITS;
max_volts_level = NO_MAX_VOLTS;
vartype         = NO_VARIABLE_TYPE;
n               = 0;
volts           = 0;
value           = 0;
}

/***********************************************************************
*                       ANALOG_SENSOR_EDIT_CLASS                       *
***********************************************************************/
ANALOG_SENSOR_EDIT_CLASS::~ANALOG_SENSOR_EDIT_CLASS()
{
clear_points();
}

/***********************************************************************
*                       ANALOG_SENSOR_EDIT_CLASS                       *
*                                 =                                    *
***********************************************************************/
ANALOG_SENSOR_EDIT_CLASS & ANALOG_SENSOR_EDIT_CLASS::operator=( const ANALOG_SENSOR_EDIT_CLASS & sorc )
{
int i;

units           = sorc.units;
max_volts_level = sorc.max_volts_level;
vartype         = sorc.vartype;
name            = sorc.name;

make_points( sorc.n );

for ( i=0; i<n; i++ )
    {
    volts[i] = sorc.volts[i];
    value[i] = sorc.value[i];
    }

return (*this);
}

/***********************************************************************
*                       ANALOG_SENSOR_EDIT_CLASS                       *
*                                ==                                    *
*               This checks everything except the name.                *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_EDIT_CLASS::operator==( const ANALOG_SENSOR_EDIT_CLASS & sorc )
{
int i;

if ( units != sorc.units )
    return FALSE;
if ( vartype != sorc.vartype )
    return FALSE;
if ( max_volts_level != sorc.max_volts_level )
    return FALSE;
if ( n != sorc.n )
    return FALSE;

if ( n == 0 )
    return TRUE;

for ( i=0; i<n; i++ )
    {
    if ( volts[i] != sorc.volts[i] )
        return FALSE;
    if ( value[i] != sorc.value[i] )
        return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                       ANALOG_SENSOR_EDIT_CLASS                       *
*                                 get                                  *
*    name, units, variable type, max volts, x1,y1,x2,y2,...xn,yn       *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_EDIT_CLASS::get( TCHAR * sorc )
{
STRING_CLASS s;
int i;
int nof_points;

if ( !sorc )
    return FALSE;

s = sorc;
nof_points = s.count_chars( CommaChar );
if ( nof_points < 7 )
    return FALSE;
nof_points -=  3;
nof_points /= 2;

/*
---------------
Make the arrays
--------------- */
if ( !make_points( nof_points );
    return FALSE;

if ( s.next_field() )
    {
    name = s;

    s.next_field();
    units = s.int_value();

    s.next_field();
    vartype = s.uint_value();

    s.next_field();
    max_volts_level = s.int_value();

    for ( i=0; i<n; i++ )
        {
        s.next_field();
        volts[i] = s;
        s.next_field();
        value[i] = s;
        }
    }
}

/***********************************************************************
*                       ANALOG_SENSOR_EDIT_CLASS                       *
*                                 put                                  *
*                                                                      *
* This appends the record. I assume the string currently has the       *
* index number and nothing else.                                       *
*                                                                      *
***********************************************************************/
bool ANALOG_SENSOR_EDIT_CLASS::put( STRING_CLASS & dest )
{
int i;

dest = name;
dest += CommaChar;
dest += units;
dest += CommaChar;
dest += vartype;
dest += CommaChar;
dest += max_volts_level;
for ( i=0; i<n; i++ )
    {
    dest += CommaChar;
    dest += volts[i];
    dest += CommaChar;
    dest += value[i];
    }
return TRUE;
}

/***********************************************************************
*                       ANALOG_SENSOR_EDIT_CLASS                       *
*                                add                                   *
***********************************************************************/
BOOLEAN add( STRING_CLASS & new_volts, STRING_CLASS & new_value );
{
int i;
int oldi;
int oldn;
STRING_CLASS * oldvolts;
STRING_CLASS * oldvalue;

/*
-----------------------------------------------------
Check to see if this is the same as an existing point
----------------------------------------------------- */
for ( i=0; i<n; i++ )
    {
    if ( new_volts == volts[i] )
        {
        value[i] = new_value;
        return TRUE;
        }
    }

/*
----------------------
Move the existing data
---------------------- */
oldn = n;
oldvolts = volts;
oldvalue = value;

/*
------------------------
Null the class pointers.
------------------------ */
n = 0;
volts = 0;
value = 0;

/*
-------------------
Allocate new arrays
------------------- */
make_points( oldn + 1 );

/*
-------------------------------------------------------
Copy the old points, inserting the new point by voltage
------------------------------------------------------- */
oldi = 0;
for ( i=0; i<n; i++ )
    {
    if ( new_volts.real_value() < oldvolts[i].real_value() )
        {
        volts[i] = new_volts;
        value[i] = new_value;
        }
    else
        {
        volts[i] = oldvolts[i];
        value[i] = oldvalue[i];
        oldi++;
        }
    }
}

/***********************************************************************
*                         ANALOG_SENSOR_RUN_CLASS                          *
***********************************************************************/
ANALOG_SENSOR_RUN_CLASS::ANALOG_SENSOR_RUN_CLASS()
{
units     = VOLTS_UNITS;
max_volts = 0.0;
vartype   = NO_VARIABLE_TYPE;
n         = 0;
volts     = 0;
value     = 0;
m         = 0;
b         = 0;
}

/***********************************************************************
*                         ANALOG_SENSOR_RUN_CLASS                          *
***********************************************************************/
ANALOG_SENSOR_RUN_CLASS::~ANALOG_SENSOR_RUN_CLASS()
{
}

/***********************************************************************
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                                 =                                    *
***********************************************************************/
ANALOG_SENSOR_RUN_CLASS & ANALOG_SENSOR_RUN_CLASS::operator=( const ANALOG_SENSOR_RUN_CLASS & sorc )
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
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                                 =                                    *
***********************************************************************/
ANALOG_SENSOR_RUN_CLASS & ANALOG_SENSOR_RUN_CLASS::operator=( const ANALOG_SENSOR_EDIT_CLASS & sorc )
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




/***********************************************************************
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                                ==                                    *
*           This checks everything except the description.             *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_RUN_CLASS::operator==( const ANALOG_SENSOR_RUN_CLASS & sorc )
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
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                              update                                  *
*                  Update the max_volts and equation.                  *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_RUN_CLASS::update()
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
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                                 get                                  *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_RUN_CLASS::get( TCHAR * sorc )
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
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                                 put                                  *
*                                                                      *
* This appends the record. I assume the string currently has the       *
* index number and nothing else.                                       *
*                                                                      *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_RUN_CLASS::put( STRING_CLASS & dest )
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
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                            max_volts_text                            *
***********************************************************************/
TCHAR * ANALOG_SENSOR_RUN_CLASS::max_volts_text()
{
static TCHAR * tp[] = { TEXT("2.5"), TEXT("5.0"), TEXT("10.0") };
int i;
i = max_volts_level - 1;
if ( i < 0 || i > 2 )
    i = 0;
return tp[i];
}

/***********************************************************************
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                           converted_value                            *
***********************************************************************/
double ANALOG_SENSOR_RUN_CLASS::converted_value( double raw_analog_counts )
{
double y;

y = raw_analog_counts;
y *= m;
y += b;

return y;
}

/***********************************************************************
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                           converted_value                            *
***********************************************************************/
double ANALOG_SENSOR_RUN_CLASS::converted_value( unsigned raw_analog_counts )
{
double y;

y = (double) raw_analog_counts;
return converted_value( y );
}


/***********************************************************************
*                         ANALOG_SENSOR_RUN_CLASS                          *
*                               raw_count                              *
*                              r = (y - b)/m                           *
***********************************************************************/
double ANALOG_SENSOR_RUN_CLASS::raw_count( double r )
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
            SensorArray = new ANALOG_SENSOR_EDIT_CLASS[n];
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
    SensorArray = new ANALOG_SENSOR_EDIT_CLASS[n];
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
ANALOG_SENSOR_EDIT_CLASS * ap;

if ( new_n == NofSensors )
    return TRUE;

if ( new_n < MIN_ANALOG_SENSOR_COUNT )
    new_n = MIN_ANALOG_SENSOR_COUNT;

ap = new ANALOG_SENSOR_EDIT_CLASS[new_n];
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
ANALOG_SENSOR_EDIT_CLASS & ANALOG_SENSOR::operator[]( int i )
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
