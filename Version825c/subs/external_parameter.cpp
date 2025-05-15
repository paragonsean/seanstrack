#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\fileclas.h"
#include "..\include\external_parameter.h"
#include "..\include\subs.h"

/***********************************************************************
*                              CLEAR                                   *
***********************************************************************/
void EXTERNAL_PARAMETER_SETTINGS_CLASS::clear()
{
file_name.empty();
enable           = false;
available_at_eos = false;
}

/***********************************************************************
*                   EXTERNAL_PARAMETER_SETTINGS_CLASS                  *
***********************************************************************/
EXTERNAL_PARAMETER_SETTINGS_CLASS::EXTERNAL_PARAMETER_SETTINGS_CLASS()
{
enable           = false;
available_at_eos = false;
}

/***********************************************************************
*                  ~EXTERNAL_PARAMETER_SETTINGS_CLASS                  *
***********************************************************************/
EXTERNAL_PARAMETER_SETTINGS_CLASS::~EXTERNAL_PARAMETER_SETTINGS_CLASS()
{
}

/***********************************************************************
*                             OPERATOR =                               *
***********************************************************************/
void EXTERNAL_PARAMETER_SETTINGS_CLASS::operator=(const EXTERNAL_PARAMETER_SETTINGS_CLASS & sorc )
{
file_name        = sorc.file_name;
enable           = sorc.enable;
available_at_eos = sorc.available_at_eos;
}

/***********************************************************************
*                    EXTERNAL_PARAMETER_SETTINGS_CLASS                 *
*                                  GET                                 *
***********************************************************************/
bool EXTERNAL_PARAMETER_SETTINGS_CLASS::get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
FILE_CLASS f;
NAME_CLASS s;
bool       status;

clear();
s.get_external_parameter_settings_csvname( computer, machine, part );
if ( !s.file_exists() )
    return false;

status = false;
if ( f.open_for_read(s) )
    {
    s = f.readline();
    if ( !s.isempty() )
        {
        s.next_field();
        if ( s.next_field() )
           enable = s.bool_value();
        }

    s = f.readline();
    if ( !s.isempty() )
        {
        s.next_field();
        if ( s.next_field() )
            {
            available_at_eos = s.bool_value();
            status = true;
            }
        }

    s = f.readline();
    if ( !s.isempty() )
        {
        s.next_field();
        if ( s.next_field() )
            file_name = s;
        }

    f.close();
    }

return status;
}

/***********************************************************************
*                    EXTERNAL_PARAMETER_SETTINGS_CLASS                 *
*                                  PUT                                 *
***********************************************************************/
bool EXTERNAL_PARAMETER_SETTINGS_CLASS::put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
const TCHAR file_name_field_desc[]        = TEXT("File Name");
const TCHAR enable_field_desc[]           = TEXT("Enable");
const TCHAR available_at_eos_field_desc[] = TEXT("Available at EOS");

bool         changes_have_been_written;
FILE_CLASS   f;
STRING_CLASS new_record;
NAME_CLASS   s;

changes_have_been_written = false;
s.get_external_parameter_settings_csvname( computer, machine, part );
if ( f.open_for_write(s) )
    {
    new_record = enable_field_desc;
    new_record += CommaChar;
    new_record += enable;
    f.writeline( new_record );

    new_record = available_at_eos_field_desc;
    new_record += CommaChar;
    new_record += available_at_eos;
    f.writeline( new_record );

    new_record = file_name_field_desc;
    new_record += CommaChar;
    new_record += file_name;
    if ( f.writeline(new_record) )
        changes_have_been_written = true;

    f.close();
    }

return changes_have_been_written;
}

/***********************************************************************
*                    EXTERNAL_PARAMETER_STATUS_CLASS                   *
***********************************************************************/
EXTERNAL_PARAMETER_STATUS_CLASS::EXTERNAL_PARAMETER_STATUS_CLASS()
{
is_enabled           = false;
need_to_check_alarms = false;
waiting_for_file     = false;
}

/***********************************************************************
*                   ~EXTERNAL_PARAMETER_STATUS_CLASS                   *
***********************************************************************/
EXTERNAL_PARAMETER_STATUS_CLASS::~EXTERNAL_PARAMETER_STATUS_CLASS()
{
}

/***********************************************************************
*                             OPERATOR =                               *
***********************************************************************/
void EXTERNAL_PARAMETER_STATUS_CLASS::operator=(const EXTERNAL_PARAMETER_SETTINGS_CLASS & sorc )
{
is_enabled = sorc.enable;
file_name  = sorc.file_name;
}

/***********************************************************************
*                             OPERATOR =                               *
***********************************************************************/
void EXTERNAL_PARAMETER_STATUS_CLASS::operator=(const EXTERNAL_PARAMETER_STATUS_CLASS & sorc )
{
file_name            = sorc.file_name;
is_enabled           = sorc.is_enabled;
need_to_check_alarms = sorc.need_to_check_alarms;
waiting_for_file     = sorc.waiting_for_file;
}
