#ifndef _EXTERNAL_PARAMETER_SETTINGS_CLASS_
#define _EXTERNAL_PARAMETER_SETTINGS_CLASS_

class EXTERNAL_PARAMETER_SETTINGS_CLASS
{
public:

bool       available_at_eos;
bool       enable;
NAME_CLASS file_name;

EXTERNAL_PARAMETER_SETTINGS_CLASS();
~EXTERNAL_PARAMETER_SETTINGS_CLASS();
void clear();
void operator=(const EXTERNAL_PARAMETER_SETTINGS_CLASS & sorc );
bool get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
bool put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
};

class EXTERNAL_PARAMETER_STATUS_CLASS
{
public:

NAME_CLASS file_name;
bool       is_enabled;
bool       need_to_check_alarms;
bool       waiting_for_file;

EXTERNAL_PARAMETER_STATUS_CLASS();
~EXTERNAL_PARAMETER_STATUS_CLASS();
void operator=(const EXTERNAL_PARAMETER_SETTINGS_CLASS & sorc );
void operator=(const EXTERNAL_PARAMETER_STATUS_CLASS   & sorc );
};

#endif
