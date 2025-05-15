#ifndef _ANALOG_SENSOR_
#define _ANALOG_SENSOR_

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
    unsigned     vartype;
    STRING_CLASS desc;
    STRING_CLASS low_volts;
    STRING_CLASS low_value;
    STRING_CLASS high_volts;
    STRING_CLASS high_value;

    double       max_volts;
    double       m;
    double       b;

    ANALOG_SENSOR_ENTRY();
    ~ANALOG_SENSOR_ENTRY();
    ANALOG_SENSOR_ENTRY & operator=( const ANALOG_SENSOR_ENTRY & sorc );
    BOOLEAN operator==( const ANALOG_SENSOR_ENTRY & sorc );
    BOOLEAN get( TCHAR * sorc );
    BOOLEAN put( STRING_CLASS & dest );
    BOOLEAN update();
    double converted_value( double raw_analog_counts );
    double converted_value( unsigned raw_analog_counts );
    double raw_count( double x );
    int units_index() { return units; }
    TCHAR * max_volts_text();
    };

class ANALOG_SENSOR
    {
    private:
    int x;

    public:
    ANALOG_SENSOR();
    ~ANALOG_SENSOR();
    static void shutdown();
    static BOOLEAN get();
    static BOOLEAN put();
    static BOOLEAN setsize( int new_n );
    int count();
    ANALOG_SENSOR_ENTRY & operator[]( int i );
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
    };

#endif