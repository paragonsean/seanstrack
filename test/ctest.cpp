#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"

/***********************************************************************
*                               NEWLINE                                *
***********************************************************************/
void newline()
{
_cprintf( "\n\r" );
}

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
TCHAR * getline( void )
{
static TCHAR buf[103];
buf[0] = 100;

_cgets( buf );

return buf+2;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
STRING_CLASS s;

newline();
_cprintf( "Enter a position or press <Enter> to exit " );
while ( true )
    {
    s = getline();
    if ( s.isempty() )
        break;
    newline();
    _cprintf( "[%s]\n\r", s.text() );
    }

_cprintf( "\n\rDone, press <Enter> to exit..." );

return 0;
}
