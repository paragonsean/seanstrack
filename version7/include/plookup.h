#ifndef _PLOOKUP_
#define _PLOOKUP_

#ifndef _SETPOINT_
#include "..\include\setpoint.h"
#endif

#ifndef _TIME_CLASS_
#include "..\include\timeclas.h"
#endif

class PLOOKUP_CLASS : public SETPOINT_CLASS
{
public:
NAME_CLASS s;

PLOOKUP_CLASS();
~PLOOKUP_CLASS();
BOOLEAN  init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN  exists();
int      get();
BOOLEAN  put();
BOOLEAN  set_last_shot_number( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, int new_shot_number );
BOOLEAN  set_last_shot_number( int new_shot_number );
int      get_last_shot_number( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
int      get_last_shot_number();
BOOLEAN  set_time( TIME_CLASS & sorc );
BOOLEAN  set_time( SYSTEMTIME & sorc );
BOOLEAN  get_time( TIME_CLASS & dest );
BOOLEAN  get_time( SYSTEMTIME & dest );
};
#endif
