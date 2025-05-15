#include <windows.h>
#include <ddeml.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"

static TCHAR CommaChar          = TEXT( ',' );
static TCHAR NullChar           = TEXT( '\0' );
static TCHAR SpaceChar          = TEXT( ' ' );

/***********************************************************************
*                           GET_STARTUP_PART                           *
***********************************************************************/
BOOLEAN get_startup_part( STRING_CLASS & machine, STRING_CLASS & part )
{
TCHAR * commandline;

TCHAR * cp;
TCHAR * ep;
TCHAR * mp;
int     slen;

commandline = GetCommandLine();

cp = findchar( SpaceChar, commandline, lstrlen(commandline) );
if ( cp )
    {
    while ( *cp == SpaceChar )
        cp++;
    ep = findchar( CommaChar, cp, lstrlen(cp) );
    if ( ep )
        {
        slen = ep - cp;
        if ( slen > 0 )
            {
            machine.upsize( slen );
            mp = machine.text();
            copychars( mp, cp, slen );
            mp += slen;
            *(mp) = NullChar;
            ep++;
            slen = lstrlen( ep );
            if ( slen > 0 )
                {
                part = ep;
                return TRUE;
                }
            }
        }
    }

machine.null();
part.null();
return FALSE;
}
