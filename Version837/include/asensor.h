#ifndef _ANALOG_SENSOR_
#define _ANALOG_SENSOR_

#ifndef _NAME_CLASS_
#include "..\include\nameclas.h"
#endif

#include <vector>

/*
----------------
Max Volts Levels
---------------- */
#define NO_MAX_VOLTS             0
#define TWO_POINT_FIVE_MAX_VOLTS 1
#define FIVE_MAX_VOLTS           2
#define TEN_MAX_VOLTS            3

struct ANALOG_SENSOR_ENTRY
    {
    int          units;
    int          max_volts_level;
    double       max_volts;
    unsigned     vartype;
    STRING_CLASS desc;

	/*
    ---------------------------------------
    linear interpolation of sensor response
    --------------------------------------- */
    std::vector<STRING_CLASS> s_voltages;
    std::vector<STRING_CLASS> s_values;
    std::vector<double>       raw_counts;
    std::vector<double>       values;
    std::vector<double>       m;
    std::vector<double>       b;
	int                       nof_sensor_points;

    ANALOG_SENSOR_ENTRY();
    ~ANALOG_SENSOR_ENTRY();
	ANALOG_SENSOR_ENTRY & operator=( const ANALOG_SENSOR_ENTRY & sorc );

    BOOLEAN operator==( const ANALOG_SENSOR_ENTRY & sorc );
    BOOLEAN operator!=( const ANALOG_SENSOR_ENTRY & sorc );
    BOOLEAN find( NAME_CLASS & filename, STRING_CLASS & sensorname );
    BOOLEAN get( TCHAR * sorc );
    BOOLEAN put( STRING_CLASS & dest );
    BOOLEAN update();
    double  converted_value( double raw_analog_counts );
    double  converted_value( unsigned raw_analog_counts );
    double  high_value();
    double  low_value();
    double  raw_count( double x );
    int     units_index() { return units; }
    TCHAR * max_volts_text();
    };

class ANALOG_SENSOR
    {
    private:
    int x;

    public:

    ANALOG_SENSOR_ENTRY * SensorArray = 0;
    int                   NofSensors = 0;

    ANALOG_SENSOR_ENTRY DefaultSensor;
    ANALOG_SENSOR_ENTRY CMDSensor;
    int                 default_sensor_index;
    int                 cmd_sensor_index;

    ANALOG_SENSOR();
    ~ANALOG_SENSOR();
    void kill_sensor_array();
	BOOLEAN get();
	BOOLEAN get( NAME_CLASS & sensor_file );
    BOOLEAN put();
    BOOLEAN put( NAME_CLASS & sensor_file);
    BOOLEAN setsize( int new_n );
    int count();
    ANALOG_SENSOR_ENTRY & operator[]( int i );
    ANALOG_SENSOR & operator=( ANALOG_SENSOR & sorc );
    BOOLEAN find( int index_to_find );
    BOOLEAN find( TCHAR * sorc_desc );
    BOOLEAN exists( int index_to_find );
    BOOLEAN next();
    BOOLEAN rewind();
    double converted_value( double raw_analog_counts );
    double converted_value( unsigned raw_analog_counts );
	double raw_count( double x );
    int index( TCHAR * sorc_desc );
    int units_index();
    unsigned vartype();
    double high_value();
    double low_value();
    double max_volts();
    TCHAR * desc();
    ANALOG_SENSOR_ENTRY & current();
    BOOLEAN insert_default_sensor();
    };

#endif