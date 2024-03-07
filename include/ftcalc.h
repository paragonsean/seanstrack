#ifndef _FTCALC_CLASS_
#define _FTCALC_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

#ifndef _SETPOINT_
#include "..\include\setpoint.h"
#endif

/*
--------------------------------------------------------------------------------
A SOURCE_POINT has the pos, time, and velocity at the point on the
shot where the parameter was calculated or measured. Most of the time
it is just the measured point but sometimes, like when finding the max
pressure over a range of positions, it isn't so obvious. For external parameters
it has no meaning. Position is the position in the current distance units, time
is the time in ms from start of shot, velocity is in the current velocity units.
-------------------------------------------------------------------------------- */
class SOURCE_DATA
    {
    public:
    double   position;
    double   velocity;
    double   time;
    bool     is_set;

    SOURCE_DATA();
    ~SOURCE_DATA();
    void clear();
    void operator=( const SOURCE_DATA & sorc );
    void set( double new_position, double new_time, double new_velocity );
    };

class FTCALC_CLASS
    {
    public:

    STRING_CLASS   cmd[MAX_PARMS];
    SOURCE_DATA    sourcedata[MAX_PARMS];
    SETPOINT_CLASS sp;

    FTCALC_CLASS();
    ~FTCALC_CLASS(){}
    void       cleanup();
    void       clear_source_data();
    double     value( STRING_CLASS & command, double * parameters, PART_CLASS & part );
    double     value( int dest_parameter_number, double * parameters, PART_CLASS & part );
    BOOLEAN    load( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    BOOLEAN    remove( int index_to_remove );
    BOOLEAN    save( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    };

#endif
