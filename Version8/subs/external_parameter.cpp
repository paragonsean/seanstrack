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
enable           = FALSE;
available_at_eos = FALSE;
}

/***********************************************************************
*                   EXTERNAL_PARAMETER_SETTINGS_CLASS                  *
***********************************************************************/
EXTERNAL_PARAMETER_SETTINGS_CLASS::EXTERNAL_PARAMETER_SETTINGS_CLASS()
{
enable           = FALSE;
available_at_eos = FALSE;
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
BOOLEAN EXTERNAL_PARAMETER_SETTINGS_CLASS::get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
FILE_CLASS f;
NAME_CLASS s;
BOOLEAN    status;

status = FALSE;
clear();
s.get_external_parameter_settings_csvname( computer, machine, part );
if ( !s.file_exists() )
    return status;

if ( f.open_for_read(s) )
    {
    s = f.readline();
    if ( !s.isempty() )
        {
        s.next_field();
        if ( s.next_field() )
           enable = s.boolean_value();
        }

    s = f.readline();
    if ( !s.isempty() )
        {
        s.next_field();
        if ( s.next_field() )
            {
            available_at_eos = s.boolean_value();
            status = TRUE;
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
BOOLEAN EXTERNAL_PARAMETER_SETTINGS_CLASS::put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
const TCHAR file_name_field_desc[]        = TEXT("File Name");
const TCHAR enable_field_desc[]           = TEXT("Enable");
const TCHAR available_at_eos_field_desc[] = TEXT("Available at EOS");

BOOLEAN      changes_have_been_written;
FILE_CLASS   f;
STRING_CLASS new_record;
NAME_CLASS   s;

changes_have_been_written = FALSE;
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
        changes_have_been_written = TRUE;

    f.close();
    }

return changes_have_been_written;
}
