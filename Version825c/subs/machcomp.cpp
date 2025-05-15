#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\subs.h"

/***********************************************************************
*                         MACHINE_COMPUTER_NAME                        *
*                                                                      *
*  This routine searches the computers for a machine. If none is       *
*  found, I return a NULL.                                             *
*                                                                      *
***********************************************************************/
BOOLEAN machine_computer_name( STRING_CLASS & dest, STRING_CLASS & machine_name )
{
COMPUTER_CLASS c;

c.rewind();
while ( c.next() )
    {
    if ( c.is_present() )
        {
        if ( machine_exists(c.name(), machine_name) )
            {
            dest = c.name();
            return TRUE;
            }
        }
    }

return FALSE;
}
