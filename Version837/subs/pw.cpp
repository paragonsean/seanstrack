#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

 /***********************************************************************
*                            PASSWORD_LEVEL                            *
***********************************************************************/
short password_level( TCHAR * pw )
{
NAME_CLASS s;
DB_TABLE   t;
short      level;
TCHAR      name[PASSWORD_LEN+1];
short      reclen;

level = NO_PASSWORD_LEVEL;

s.get_data_dir_file_name( NEW_PASSWORD_DB );
if ( s.file_exists() )
    {
    reclen = NEW_PASSWORD_RECLEN;
    }
else
    {
    s.get_data_dir_file_name( PASSWORD_DB );
    reclen = PASSWORD_RECLEN;
    }

if ( t.open(s, reclen, PFL) )
    {
    while ( t.get_next_record(FALSE) )
        {
        t.get_alpha( name, PASSWORD_OFFSET, PASSWORD_LEN );
        if ( strings_are_equal(pw, name ) )
            {
            level = t.get_long( PASSWORD_LEVEL_OFFSET );
            break;
            }
        }

    t.close();
    }

return level;
}

/***********************************************************************
*                        HIGHEST_PASSWORD_LEVEL                        *
*                Get the highest defined password level                *
***********************************************************************/
short highest_password_level()
{
NAME_CLASS s;
DB_TABLE   t;
short      highest_level;
short      this_level;
short      reclen;

highest_level = NO_PASSWORD_LEVEL;

s.get_data_dir_file_name( NEW_PASSWORD_DB );
if ( s.file_exists() )
    {
    reclen = NEW_PASSWORD_RECLEN;
    }
else
    {
    s.get_data_dir_file_name( PASSWORD_DB );
    reclen = PASSWORD_RECLEN;
    }

if ( t.open(s, reclen, PFL) )
    {
    while ( t.get_next_record(FALSE) )
        {
        this_level = (short) t.get_long( PASSWORD_LEVEL_OFFSET );
        if ( this_level > highest_level )
            highest_level = this_level;
        }

    t.close();
    }

return highest_level;
}
