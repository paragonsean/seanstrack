#ifndef _STATIC_TEXT_PARAMETER_
#define _STATIC_TEXT_PARAMETER_

class STATIC_TEXT_PARAMETER_CLASS
{
public:
NAME_CLASS   fn;
int          parameter_number;
STRING_CLASS value;
BOOLEAN      popup_on_create;
BOOLEAN      popup_on_monitor;

STATIC_TEXT_PARAMETER_CLASS();
~STATIC_TEXT_PARAMETER_CLASS();
void     clear();
void     operator=( const STATIC_TEXT_PARAMETER_CLASS & sorc );
BOOLEAN  init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN  exists();
BOOLEAN  get( int new_parameter_number );
BOOLEAN  put( int new_parameter_number );
};
#endif
