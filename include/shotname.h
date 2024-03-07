#ifndef _SHOT_NAME_CLASS_
#define _SHOT_NAME_CLASS_

#ifndef _SETPOINT_
#include "..\include\setpoint.h"
#endif

class SHOT_NAME_CLASS : public SETPOINT_CLASS
{
public:
NAME_CLASS s;

SHOT_NAME_CLASS();
~SHOT_NAME_CLASS();
BOOLEAN  init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN  exists();
int      get();
BOOLEAN  put();
void     set_defaults();
int      shot_name_length();
int      shot_number_length();
BOOLEAN  get_shot_name_length( STRING_CLASS & dest );
BOOLEAN  set_shot_name_length( STRING_CLASS & new_length );
BOOLEAN  get_shot_name_template( STRING_CLASS & dest );
BOOLEAN  set_shot_name_template( STRING_CLASS & new_template );
};

class SHOT_NAME_PARTS_CLASS
{
public:
STRING_CLASS name;
STRING_CLASS s;
int          shot_number;
int          shot_number_length;
TCHAR        check_digit;

SHOT_NAME_PARTS_CLASS();
~SHOT_NAME_PARTS_CLASS();
void    clear();
void    set( STRING_CLASS & sorc, int shot_number_length );
void    make_name();
void    operator=(   SHOT_NAME_PARTS_CLASS & sorc );
BOOLEAN operator<(   SHOT_NAME_PARTS_CLASS & sorc );
BOOLEAN operator<=(  SHOT_NAME_PARTS_CLASS & sorc );
BOOLEAN operator>(   SHOT_NAME_PARTS_CLASS & sorc );
BOOLEAN operator>=(  SHOT_NAME_PARTS_CLASS & sorc );
BOOLEAN operator==(  SHOT_NAME_PARTS_CLASS & sorc );
BOOLEAN operator!=(  SHOT_NAME_PARTS_CLASS & sorc ) { if (operator==(sorc)) return FALSE; return TRUE; }
};

/*
---------------------------------------------------------------------------------------
The EXTERNAL_SHOT_NAME_CLASS gets all the information necessary to get a shot name from
an external file. "command" is the command to be sent to get the shot name.
I send the command when I get the cycle start message from the fastrak2 board. If this
is blank then the shot name file will be created without my help. "file" is the name
of the file that will contain the shot name. At the present I assume this will be
available when the shot is complete.
--------------------------------------------------------------------------------------- */
class EXTERNAL_SHOT_NAME_CLASS : public SETPOINT_CLASS
{
public:
NAME_CLASS fn;

EXTERNAL_SHOT_NAME_CLASS();
~EXTERNAL_SHOT_NAME_CLASS();
BOOLEAN  init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
STRING_CLASS & command();
STRING_CLASS & file();
int      get();
int      get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN  have_command();
BOOLEAN  is_enabled();
BOOLEAN  put();
void     set_defaults();
BOOLEAN  set_enable( BOOLEAN is_enabled );
};

#endif