#ifndef _FTCALC_CLASS_
#define _FTCALC_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

#ifndef _SETPOINT_
#include "..\include\setpoint.h"
#endif

#ifndef _LIMIT_SWITCH_CLASS_
#include "..\include\limit_switch_class.h"
#endif

#ifndef _FTII_SHOT_DATA_
#include "..\include\ftii_shot_data.h"
#endif

#ifndef _VISI_PARAM_H_
#include "..\include\param.h"
#endif

#include <stack>
#include <queue>
#include <string>
#include <map>

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

    STRING_CLASS       cmd[MAX_PARMS];
    PART_CLASS         current_part;
    double             gate_area;
    double             hydraulic_cyl_dia;
    double             hydraulic_cyl_area;
    LIMIT_SWITCH_CLASS limit_switch;
    double           * parameter_list;
    SOURCE_DATA        sourcedata[MAX_PARMS];
    SETPOINT_CLASS     sp;
    std::map <STRING, STRING> vars;

    FTCALC_CLASS();
    ~FTCALC_CLASS(){}
    void    cleanup();
    void    clear_source_data();
    double  value( STRING_CLASS & command, double * parameters, PART_CLASS & part, LIMIT_SWITCH_CLASS & limts, FTII_PROFILE & profile );
    double  value( int dest_parameter_number, double * parameters, PART_CLASS & part, LIMIT_SWITCH_CLASS & limts, FTII_PROFILE & profile );
    BOOLEAN load( PARAMETER_CLASS & params, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    BOOLEAN remove( int index_to_remove );
    BOOLEAN save( PARAMETER_CLASS & params, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    
    unsigned int parse_operand( const STRING input, int i, STRING & operand );
    int          infix_string_to_queue( STRING infixStr, std::queue<STRING> & infixQ );
    STRING       parse_lines( TCHAR * text, FTII_PROFILE & profile, PART_CLASS & part );
    BOOLEAN      parse_assignment( STRING line );
    void         parse_functions( STRING & line, FTII_PROFILE & profile, PART_CLASS & part );
    void         evaluate_variable( STRING_CLASS & value, STRING_CLASS & name );
    void         evaluate_arg( STRING_CLASS & value, STRING_CLASS & arg );
};

#endif
