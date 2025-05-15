#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

static TCHAR NullChar       = TEXT( '\0' );
static TCHAR UnderscoreChar = TEXT( '_' );
static TCHAR DashChar       = TEXT( '-' );

/***********************************************************************
*                            PART_NAME_COPY                            *
*                                                                      *
*         Copy the part name from a string that may be bigger.         *
*                                                                      *
***********************************************************************/
BOOLEAN part_name_copy( STRING_CLASS & dest, STRING_CLASS & sorc )
{

STRING_CLASS s;
TCHAR *cp;

if ( sorc.isempty() )
    return FALSE;

s = sorc;
if ( s.len() < 1 )
    return FALSE;

cp = s.text();
while ( (*cp) != NullChar )
    {
    /*
    -------------------------------------------
    Ok to copy if a is in { 0-9, A-Z, a-z, _, -  }
    ------------------------------------------- */
    if ( !IsCharAlphaNumeric(*cp) && *cp != UnderscoreChar && *cp != DashChar )
        break;

    cp++;
    }

*cp = NullChar;
dest = s;

return TRUE;
}
