#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\asensor.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"

#define DEFAULT_SENSOR_DESC TEXT("0 to 5 Volts")

static const double MAX_RAW_COUNTS = 4095.0;

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
***********************************************************************/
ANALOG_SENSOR_ENTRY::ANALOG_SENSOR_ENTRY()
{
units           = VOLTS_UNITS;
max_volts_level = NO_MAX_VOLTS;
vartype         = NO_VARIABLE_TYPE;
max_volts       = 10.0;
nof_sensor_points = 0;
}

/***********************************************************************
*                        ~ANALOG_SENSOR_ENTRY                          *
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
desc = sorc.desc;
units             = sorc.units;
vartype           = sorc.vartype;
nof_sensor_points = sorc.nof_sensor_points;
raw_counts        = sorc.raw_counts;
values            = sorc.values;
s_voltages        = sorc.s_voltages;
s_values          = sorc.s_values;
m                 = sorc.m;
b                 = sorc.b;

return (*this);
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                                ==                                    *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_ENTRY::operator==( const ANALOG_SENSOR_ENTRY & sorc )
{
int i;

if ( desc != sorc.desc )
    return FALSE;

if ( units != sorc.units )
    return FALSE;

if ( vartype != sorc.vartype )
    return FALSE;

if ( nof_sensor_points != sorc.nof_sensor_points )
    return FALSE;

for ( i = 0; i < nof_sensor_points; i++ )
{
    if ( s_voltages[i] != sorc.s_voltages[i] )
        return FALSE;
    if ( values[i] != sorc.values[i] )
        return FALSE;
    if ( raw_counts[i] != sorc.raw_counts[i] )
        return FALSE;
}

return TRUE;
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                                !=                                    *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_ENTRY::operator!=( const ANALOG_SENSOR_ENTRY & sorc )
{
if ( operator==(sorc) )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                              update                                  *
*                      Update the max_volts                            *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_ENTRY::update()
{
    double x1;
    double x2;
    double y1;
    double y2;
    int n;
    int i;

    n = s_voltages.size();
    x2 = s_voltages[n - 1].real_value();

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
    ------------------------------
    Convert voltages to raw counts
    ------------------------------ */
    raw_counts.clear();
    for ( i = 0; i < n; i++ )
    {
        raw_counts.push_back( s_voltages[i].real_value() * MAX_RAW_COUNTS / max_volts );
    }

    /*
    ----------------------------------------------------------------
    The equation is y = mx + b where x is the voltage in raw counts.
    ---------------------------------------------------------------- */
    m.clear();
    b.clear();
    for ( i = 0; i < n - 1; i++ )
    {
        x1 = raw_counts[i];
        x2 = raw_counts[i + 1];
        y1 = values[i];
        y2 = values[i + 1];

        if (x1 == x2)
        {
            m.push_back( 1.0 );
            b.push_back( 0.0 );
            return FALSE;
        }

        m.push_back( (y2 - y1) / (x2 - x1) );
        b.push_back( y1 - m[i]*x1 );
    }

    return TRUE;
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                                 get                                  *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_ENTRY::get( TCHAR * sorc )
{
    STRING_CLASS s;

    if ( !sorc )
        return FALSE;

    s = sorc;
    if ( s.isempty() )
        return FALSE;

    s.next_field();
    if ( s.isnumeric() && !(s == DEFAULT_SENSOR_DESC) )
    {
        /*
        -------------------------------
        This is an old asensor.csv file
        ------------------------------- */
        s.next_field();
        units = s.int_value();

        s.next_field();
        vartype = s.uint_value();

        s.next_field();
        desc = s;

        /*
        ---------
        Low point
        --------- */
        s.next_field();
        s_voltages.push_back( s );

        s.next_field();
        values.push_back( s.real_value() );
        s_values.push_back( s );

        /*
        ----------
        High point
        ---------- */
        s.next_field();
        s_voltages.push_back( s );

        s.next_field();
        values.push_back( s.real_value() );
        s_values.push_back( s );
    }
    else
    {
        desc = s;

        s.next_field();
        units = s.int_value();

        s.next_field();
        vartype = s.uint_value();

        nof_sensor_points = 0;
        while ( s.next_field() && !s.isempty() )
        {
            s_voltages.push_back(s);

            s.next_field();
            values.push_back( s.real_value() );
            s_values.push_back( s );

            nof_sensor_points++;
        }
    }

    if ( nof_sensor_points > 0 )
        return update();
    else
        return FALSE;
}

/***********************************************************************
*                         ANALOG_SENSOR_ENTRY                          *
*                                 FIND                                 *
* This is used to load a single sensor, such as for the                *
* PRESSURE_CONTROL_CLASS, which only needs one sensor.                 *
* filename   = name of asensor.csv to read from                        *
* sensorname = name of the sensor to find                              *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_ENTRY::find( NAME_CLASS & filename, STRING_CLASS & sensorname )
{
FILE_CLASS   f;
STRING_CLASS s;
int          i;

if ( filename.file_exists() )
    {
        if ( f.open_for_read(filename) )
        {
            if ( sensorname.isnumeric() )
            {
                /*
                -----------------------
                Old style numeric index
                ----------------------- */
                for ( i = 0; i <= sensorname.int_value(); i++ )
                {
                    s = f.readline();
                    if ( s.isempty() )
                        return FALSE;
                }
                get( s.text() );
                f.close();
                return TRUE;
            }
            else
            {
                /*
                --------------------
                New style name index
                -------------------- */
                while ( true )
                {
                    s = f.readline();
                    if ( s.isempty() )
                        break;
                    get( s.text() );
                    if ( desc == sensorname )
                    {
                        f.close();
                        return TRUE;
                    }
                }
                f.close();
            }
        }
    }
return FALSE;
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
    int i;

    dest = desc;
    dest += CommaChar;
    dest += units;
    dest += CommaChar;
    dest += vartype;

    /*
    -----------------------------------------------------
    Force voltages and values vectors to be the same size
    ----------------------------------------------------- */
    nof_sensor_points = s_voltages.size();
    minint( nof_sensor_points, values.size() );

    for ( i = 0; i < nof_sensor_points; i++ )
    {
        dest += CommaChar;
        dest += s_voltages[i];
        dest += CommaChar;
        dest += s_values[i];
    }

    return TRUE;
}

/***********************************************************************
*                          ANALOG_SENSOR_ENTRY                         *
*                               high_value                             *
***********************************************************************/
double ANALOG_SENSOR_ENTRY::high_value()
{
if ( nof_sensor_points > 0 )
    return values[nof_sensor_points-1];

return 1.0;
}

/***********************************************************************
*                          ANALOG_SENSOR_ENTRY                         *
*                               low_value                              *
***********************************************************************/
double ANALOG_SENSOR_ENTRY::low_value()
{
if ( nof_sensor_points > 0 )
    return values[0];

return 0.0;
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
*                              y = mx + b                              *
***********************************************************************/
double ANALOG_SENSOR_ENTRY::converted_value( double raw_analog_counts )
{
    int    i;

    if ( nof_sensor_points == 0 )
        return 0.0;

    if ( nof_sensor_points == 1 )
        return values[0];

    /*
    ----------------------------------------------
    Find which line segment to use
    Assume voltages are sorted by increasing order
    ---------------------------------------------- */
    for ( i = 1; i < nof_sensor_points; i++ )
    {
        if ( raw_analog_counts <= raw_counts[i] )
        {
            i--;
            return m[i] * raw_analog_counts + b[i];
        }
    }

    return values[nof_sensor_points - 1];
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
*                              x = (y - b)/m                           *
***********************************************************************/
double ANALOG_SENSOR_ENTRY::raw_count( double r )
{
    int    i;

    if ( nof_sensor_points == 0 )
        return 0.0;

    if ( nof_sensor_points == 1 )
        return raw_counts[0];

    /*
    ----------------------------------------------
    Find which line segment to use assumming
    monotonically increasing values
    ---------------------------------------------- */
    if ( m[0] > 0 )
    {
        if ( r >= values[0] )
        {
            for ( i = 0; i < nof_sensor_points - 1; i++ )
            {
                if ( r <= values[i + 1] )
                {
                    return (r - b[i]) / m[i];
                }
            }

            return raw_counts[nof_sensor_points - 1];
        }
        else
            return raw_counts[0];
    }

    /*
    ----------------------------------------------
    Find which line segment to use assumming
    monotonically decreasing values
    ---------------------------------------------- */
    else
    {
        if (r <= values[0])
        {
            for ( i = 0; i < nof_sensor_points - 1; i++ )
            {
                if ( r >= values[i + 1] )
                {
                    return (r - b[i]) / m[i];
                }
            }

            return raw_counts[nof_sensor_points - 1];
        }
        else
            return raw_counts[0];
    }
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
***********************************************************************/
ANALOG_SENSOR::ANALOG_SENSOR()
{
    x = NO_INDEX;
    default_sensor_index = NO_INDEX;
    cmd_sensor_index = NO_INDEX;

    DefaultSensor.desc = DEFAULT_SENSOR_DESC;
    DefaultSensor.units = VOLTS_UNITS;
    DefaultSensor.vartype = NO_VARIABLE_TYPE;
    DefaultSensor.max_volts_level = FIVE_MAX_VOLTS;
    DefaultSensor.max_volts = 5.0;
    DefaultSensor.nof_sensor_points = 2;
    DefaultSensor.values.resize(2);
    DefaultSensor.s_voltages.push_back( TEXT("0.0") );
    DefaultSensor.values[0] = 0.0;
    DefaultSensor.s_voltages.push_back( TEXT("5.0") );
    DefaultSensor.values[1] = 5.0;
    DefaultSensor.update();

    CMDSensor.desc = TEXT("CMD Volts [-10 to +10]");
    CMDSensor.units = VOLTS_UNITS;
    CMDSensor.vartype = NO_VARIABLE_TYPE;
    CMDSensor.max_volts_level = TEN_MAX_VOLTS;
    CMDSensor.max_volts = 10.0;
    CMDSensor.nof_sensor_points = 2;
    CMDSensor.values.resize(2);
    CMDSensor.s_voltages.push_back( TEXT("0.0") );
    CMDSensor.values[0] = -10.0;
    CMDSensor.s_voltages.push_back( TEXT("10.0") );
    CMDSensor.values[1] = 10.0;
    CMDSensor.update();
}

/***********************************************************************
*                            ~ANALOG_SENSOR                            *
***********************************************************************/
ANALOG_SENSOR::~ANALOG_SENSOR()
{
kill_sensor_array();
}

/***********************************************************************
*                            ANALOG_SENSOR                             *
*                                 get                                  *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::get()
{
NAME_CLASS s;
s.get_asensor_csvname();
return get( s );
}

/***********************************************************************
*                            ANALOG_SENSOR                             *
*                                 get                                  *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::get( NAME_CLASS & sensor_file )
{
FILE_CLASS f;
int        i;
int        n;
NAME_CLASS s = sensor_file;
BOOLEAN    status;
BOOLEAN    found_default_sensor = FALSE;
BOOLEAN    found_cmd_sensor = FALSE;

kill_sensor_array();

status = FALSE;
i = 0;

if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
        {
        n = f.nof_lines();
        if ( n > 0 )
            {
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

                    if ( SensorArray[i].desc == DefaultSensor.desc )
                    {
                        default_sensor_index = i;
                        found_default_sensor = TRUE;
                    }

                    if ( SensorArray[i].desc == CMDSensor.desc )
                    {
                        cmd_sensor_index = i;
                        found_cmd_sensor = TRUE;
                    }

                    i++;
                    }
                NofSensors = i;
                }
            }
        f.close();
        }
    }

if ( found_default_sensor == FALSE )
    {
    /*
    -------------------------------------------
    Could not find the default sensor so add it
    ------------------------------------------- */
    setsize( NofSensors + 1 );
    SensorArray[NofSensors -1] = DefaultSensor;
    }

if ( found_cmd_sensor == FALSE )
    {
    /*
    --------------------------------
    Could not find the CMD so add it
    -------------------------------- */
    setsize ( NofSensors + 1 );
    SensorArray[NofSensors - 1] = CMDSensor;
    }

if ( NofSensors > 0 )
    status = TRUE;

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

    s.get_asensor_csvname();
    return put( s );
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                  put                                 *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::put( NAME_CLASS & sensor_file)
{   
    STRING_CLASS s;
    FILE_CLASS   f;
    int          i;

    if ( f.open_for_write(sensor_file) )
    {
        for ( i = 0; i<NofSensors; i++ )
        {
            s = EmptyString;
            if (SensorArray[i].put(s) )
                f.writeline(s);
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

ap = new ANALOG_SENSOR_ENTRY[new_n];
if ( !ap )
    return FALSE;

n = new_n;
minint( n, NofSensors );

for ( i=0; i<n; i++ )
    ap[i] = SensorArray[i];

kill_sensor_array();
SensorArray = ap;
NofSensors  = new_n;

if ( x > NofSensors )
    x = NO_INDEX;

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

return DefaultSensor;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                   =                                  *
***********************************************************************/
ANALOG_SENSOR & ANALOG_SENSOR::operator=( ANALOG_SENSOR & sorc )
{
    int i;

    kill_sensor_array();
    NofSensors = sorc.NofSensors;
    SensorArray = new ANALOG_SENSOR_ENTRY[NofSensors];

    for ( i = 0; i < NofSensors; i++ )
    {
        SensorArray[i] = sorc[i];
    }

    return (*this);
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

    if (i == NO_INDEX)
    {
        x = default_sensor_index;
        return FALSE;

    }
    else
    {
        x = i;
        return TRUE;
    }
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                 find                                 *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::find( int index_to_find )
{
    if ( index_to_find >= 0 && index_to_find < NofSensors )
    {
        x = index_to_find;
        return TRUE;
    }
    else
    {
        x = NO_INDEX;
        return FALSE;
    }
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                 exists                               *
***********************************************************************/
BOOLEAN ANALOG_SENSOR::exists( int index_to_find )
{
if ( index_to_find >= 0 && index_to_find < NofSensors )
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
    int i;

    if ( x != NO_INDEX )
    {
        i = SensorArray[x].nof_sensor_points - 1;
        if ( i >= 0 )
            return SensorArray[x].values[i];
    }

    return 1.0;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                low_value                             *
***********************************************************************/
double ANALOG_SENSOR::low_value()
{
if ( x != NO_INDEX )
    return SensorArray[x].values[0];

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

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                                CURRENT                               *
***********************************************************************/
ANALOG_SENSOR_ENTRY & ANALOG_SENSOR::current()
{
    if (x != NO_INDEX)
        return SensorArray[x];
    else
        return DefaultSensor;
}

/***********************************************************************
*                             ANALOG_SENSOR                            *
*                           KILL_SENSOR_ARRAY                          *
***********************************************************************/
void ANALOG_SENSOR::kill_sensor_array()
{
    ANALOG_SENSOR_ENTRY * copy;

    NofSensors = 0;
    if ( SensorArray )
    {
        copy = SensorArray;
        SensorArray = 0;
        delete[] copy;
    }
}
