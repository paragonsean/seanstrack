#ifndef _FTCALC_CLASS_
#define _FTCALC_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

#ifndef _SETPOINT_
#include "..\include\setpoint.h"
#endif

class FTCALC_CLASS
    {
    public:
    STRING_CLASS   cmd[MAX_PARMS];
    SETPOINT_CLASS sp;
    FTCALC_CLASS(){}
    ~FTCALC_CLASS(){}
    void       cleanup();
    double     value( int dest_parameter_number, double * parameters );
    BOOLEAN    load( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    BOOLEAN    save( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    };

#endif

