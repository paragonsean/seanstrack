#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\fileclas.h"


/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
FILE_CLASS sorc;
FILE_CLASS dest;

STRING_CLASS s;

sorc.open_for_read( "c:\\bozo\\Book2.csv" );
dest.open_for_write( "c:\\bozo\\Book3.csv" );

while ( true )
    {
    s = sorc.readline();
    if ( s.isempty() )
        break;
    while ( s.next_field() )
        dest.writeline( s );
    }


sorc.close();
dest.close();

return( 0 );
}
