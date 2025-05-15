#include <windows.h>
static TCHAR NullChar = TEXT('0');

const TCHAR * findchar( TCHAR c, const TCHAR * sorc );

/***********************************************************************
*                               IS_EMPTY                               *
***********************************************************************/
BOOLEAN is_empty( const TCHAR * s )
{
if ( s )
    {
    while ( *s )
        {
        if ( *s != TEXT( ' ' ) )
            return FALSE;
        s++;
        }
    }

return TRUE;
}

/***********************************************************************
*                              IS_NUMERIC                              *
*  Return TRUE if the string begins with a number "  1" " +.0", etc.   *
***********************************************************************/
BOOLEAN is_numeric( const TCHAR * s )
{
const TCHAR startchars[] = TEXT(" +-." );
const TCHAR numerics[]   = TEXT("0123456789");
const TCHAR * cp;
TCHAR   c;

if ( !s )
    return FALSE;

cp = s;
while ( *cp != NullChar )
    {
    c = *cp;
    if ( findchar(c, numerics) )
        return TRUE;
    if ( !findchar(c, startchars) )
        return FALSE;
    cp++;
    }

return FALSE;
}
