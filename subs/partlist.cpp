#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"

TCHAR ResultsString[]   = TEXT( "results" );

/***********************************************************************
*                             GET_PARTLIST                             *
***********************************************************************/
BOOLEAN get_partlist( TEXT_LIST_CLASS & dest, STRING_CLASS & computer, STRING_CLASS & machine )
{
NAME_CLASS s;
s.get_machine_directory( computer, machine );
if ( !s.directory_exists() )
    return FALSE;

dest.empty();
dest.get_directory_list( s.text() );
dest.rewind();
while ( dest.next() )
    {
    if ( strings_are_equal(dest.text(), ResultsString) )
        {
        dest.remove();
        break;
        }
    }

if ( dest.count() > 0 )
    {
    dest.rewind();
    return TRUE;
    }

return FALSE;
}