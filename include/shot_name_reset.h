#ifndef _SHOT_NAME_RESET_CLASS_
#define _SHOT_NAME_RESET_CLASS_

#ifndef _SETPOINT_
#include "..\include\setpoint.h"
#endif

class SHOT_NAME_RESET_CLASS : public SETPOINT_CLASS
{
public:
NAME_CLASS s;

SHOT_NAME_RESET_CLASS();
~SHOT_NAME_RESET_CLASS();
BOOLEAN  init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN  exists();
int      get();
BOOLEAN  put();
BOOLEAN  is_enabled();
BOOLEAN  purge_on_reset();
STRING_CLASS & reset_time();
void     set_defaults();
void     set_enable( BOOLEAN reset_is_enabled );
void     set_time( STRING_CLASS & sorc );
void     set_purge_on_reset( BOOLEAN purge_is_enabled );
};

#endif