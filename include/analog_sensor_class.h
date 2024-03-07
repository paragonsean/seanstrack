#ifndef _ANALOG_SENSOR_CLASS_
#define _ANALOG_SENSOR_CLASS_

/*
----------------
Max Volts Levels
---------------- */
#define NO_MAX_VOLTS             0
#define TWO_POINT_FIVE_MAX_VOLTS 1
#define FIVE_MAX_VOLTS           2
#define TEN_MAX_VOLTS            3



struct ANALOG_SENSOR_EDIT_CLASS
    {
    int          units;
    int          max_volts_level;
    unsigned     vartype;
    int          n;
    STRING_CLASS name;
    STRING_CLASS * volts;
    STRING_CLASS * value;


    ANALOG_SENSOR_EDIT_CLASS();
    ~ANALOG_SENSOR_EDIT_CLASS();
    ANALOG_SENSOR_EDIT_CLASS & operator=( const ANALOG_SENSOR_EDIT_CLASS & sorc );
    BOOLEAN operator==( const ANALOG_SENSOR_EDIT_CLASS & sorc );
    void    clear_points();
    BOOLEAN make_points( int nof_points );
    BOOLEAN get( TCHAR * sorc );
    BOOLEAN put( STRING_CLASS & dest );
    BOOLEAN add( STRING_CLASS & new_volts, STRING_CLASS & new_value );
    BOOLEAN remove( STRING_CLASS & volts_to_remove );
    BOOLEAN remove( int index );
    };

struct ANALOG_SENSOR_RUN_CLASS
    {
    int          units;
    unsigned     vartype;
    double       max_volts;
    int          n;
    double     * volts;
    double     * value;
    double     *  m;
    double     *  b;

    ANALOG_SENSOR_RUN_CLASS();
    ~ANALOG_SENSOR_RUN_CLASS();
    ANALOG_SENSOR_RUN_CLASS & operator=( const ANALOG_SENSOR_RUN_CLASS & sorc );
    ANALOG_SENSOR_RUN_CLASS & operator=( const ANALOG_SENSOR_EDIT_CLASS & sorc );
    BOOLEAN operator==( const ANALOG_SENSOR_RUN_CLASS & sorc );
    BOOLEAN update();
    double converted_value( double raw_analog_counts );
    double converted_value( unsigned raw_analog_counts );
    double raw_count( double x );
    int units_index() { return units; }
    double raw_count( double sorc_value );
    double volts( double sorc_value );
    int units_index() { return units; }
    };

class ANALOG_SENSOR_CLASS
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