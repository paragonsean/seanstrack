#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\fileclas.h"
#include "..\include\static_text_parameter.h"
#include "..\include\subs.h"

/***********************************************************************
*                              CLEAR                                   *
***********************************************************************/
void STATIC_TEXT_PARAMETER_CLASS::clear()
{
fn.empty();
value.empty();
parameter_number = NO_PARAMETER_NUMBER;
popup_on_create  = FALSE;
popup_on_monitor = FALSE;
}

/***********************************************************************
*                       STATIC_TEXT_PARAMETER_CLASS                    *
***********************************************************************/
STATIC_TEXT_PARAMETER_CLASS::STATIC_TEXT_PARAMETER_CLASS()
{
parameter_number = NO_PARAMETER_NUMBER;
popup_on_create  = FALSE;
popup_on_monitor = FALSE;
}

/***********************************************************************
*                      ~STATIC_TEXT_PARAMETER_CLASS                    *
***********************************************************************/
STATIC_TEXT_PARAMETER_CLASS::~STATIC_TEXT_PARAMETER_CLASS()
{
}

/***********************************************************************
*                             OPERATOR =                               *
***********************************************************************/
void STATIC_TEXT_PARAMETER_CLASS::operator=(const STATIC_TEXT_PARAMETER_CLASS & sorc )
{
fn               = sorc.fn;
parameter_number = sorc.parameter_number;
value            = sorc.value;
popup_on_create  = sorc.popup_on_create;
popup_on_monitor = sorc.popup_on_monitor;
}

/***********************************************************************
*                      STATIC_TEXT_PARAMETER_CLASS                     *
*                                  INIT                                *
***********************************************************************/
BOOLEAN STATIC_TEXT_PARAMETER_CLASS::init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
return fn.get_static_text_parameters_csvname( computer, machine, part );
}

/***********************************************************************
*                      STATIC_TEXT_PARAMETER_CLASS                     *
*                                EXISTS                                *
***********************************************************************/
BOOLEAN STATIC_TEXT_PARAMETER_CLASS::exists()
{
return fn.file_exists();
}

/***********************************************************************
*                      STATIC_TEXT_PARAMETER_CLASS                     *
*                                  GET                                 *
***********************************************************************/
BOOLEAN STATIC_TEXT_PARAMETER_CLASS::get( int new_parameter_number )
{
FILE_CLASS   f;
STRING_CLASS s;
BOOLEAN      status;

parameter_number = new_parameter_number;

status = FALSE;
if ( f.open_for_read(fn) )
    {
    while ( true )
        {
        s = f.readline();
        if ( s.isempty() )
            break;
        if ( s.next_field() )
            {
            if ( new_parameter_number == s.int_value() )
                {
                if ( s.next_field() )
                    value = s;
                if ( s.next_field() )
                    popup_on_create = s.boolean_value();
                if ( s.next_field() )
                    popup_on_monitor = s.boolean_value();
                status = TRUE;
                break;
                }
            }
        }
    f.close();
    }

return status;
}

/***********************************************************************
*                       STATIC_TEXT_PARAMETER_CLASS                    *
*                                  PUT                                 *
***********************************************************************/
BOOLEAN STATIC_TEXT_PARAMETER_CLASS::put( int new_parameter_number )
{
FILE_CLASS   dest;
FILE_CLASS   sorc;
NAME_CLASS   destname;
NAME_CLASS   s;
STRING_CLASS copy;
STRING_CLASS new_record;
BOOLEAN      changes_have_been_written;

changes_have_been_written = FALSE;

new_record = parameter_number;
new_record += CommaChar;
new_record.cat_w_char( value.text(), CommaChar );
new_record += popup_on_create;
new_record += CommaChar;
new_record += popup_on_monitor;

destname = fn;
destname.replace_extension( TEXT(".TMP") );
if ( !dest.open_for_write(destname) )
    return FALSE;

if ( sorc.open_for_read(fn) )
    {
    while ( true )
        {
        s = sorc.readline();
        if ( s.isempty() )
            break;
        copy = s;
        if ( s.next_field() )
            {
            if ( !changes_have_been_written && (new_parameter_number <= s.int_value()) )
                {
                dest.writeline( new_record );
                changes_have_been_written = TRUE;
                if ( new_parameter_number != s.int_value() )
                    dest.writeline( copy );
                }
            else
                {
                dest.writeline( copy );
                }
            }
        sorc.close();
        }
    }
if ( !changes_have_been_written )
    dest.writeline( new_record );
dest.close();
fn.delete_file();
destname.moveto( fn );

return TRUE;
}
