#ifndef _VISI_PARAM_H_
#define _VISI_PARAM_H_

#ifndef _VDB_CLASS_
#include "..\include\vdbclass.h"
#endif

#ifndef _STATIC_TEXT_PARAMETER_CLASS_
#include "..\include\static_text_parameter.h"
#endif

/*
---------------------
Shot goodness returns
--------------------- */
#define GOOD_SHOT    0
#define WARNING_SHOT 1
#define ALARM_SHOT   2

#define NO_PARAMETER_NAME TEXT("NOPARAMETER")

struct WIRE_ENTRY
    {
    STRING_CLASS svalue;
    float        value;
    short        wire_number;
    };

struct INPUT_ENTRY
    {
    short type;
    short number;
    };

struct PARAMETER_ENTRY
    {
    TCHAR        name[PARAMETER_NAME_LEN+1];
    STRING_CLASS target;
    INPUT_ENTRY  input;
    BITSETYPE    vartype;
    short        units;
    WIRE_ENTRY   limits[NOF_ALARM_LIMIT_TYPES];
    STATIC_TEXT_PARAMETER_CLASS static_text;

    void         operator=( const PARAMETER_ENTRY & sorc );
    BOOLEAN      has_warning_limits();
    BOOLEAN      has_alarm_limits();
    BOOLEAN      has_any_limits() { if ( has_warning_limits() || has_alarm_limits() ) return TRUE; else return FALSE; }
    BOOLEAN      is_alarm( float value );
    BOOLEAN      is_warning( float value );
    };

class PARAMETER_CLASS
    {
    public:

    STRING_CLASS    computer;
    STRING_CLASS    machine;
    STRING_CLASS    part;
    PARAMETER_ENTRY parameter[MAX_PARMS];

    PARAMETER_CLASS();
    short      count();
    BOOLEAN    find( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    int        get_parameter_index( TCHAR * name );
    BOOLEAN    get_type_and_units( short i, TCHAR * cmd );  /* FtCalc only */
    void       init( short i );
    BOOLEAN    remove( short index_to_remove );
    BOOLEAN    read_param_csv( STRING_CLASS & filename );
    BOOLEAN    read_param_db( STRING_CLASS & filename );
    BOOLEAN    save();
    int        shot_goodness( VDB_CLASS & v );
    };

#endif