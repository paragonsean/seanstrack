#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"

/***********************************************************************
*                            MACHINE_EXISTS                            *
***********************************************************************/
BOOLEAN machine_exists( STRING_CLASS & computer, STRING_CLASS & machine )
{
NAME_CLASS s;

s.get_machset_csvname( computer, machine );
return s.file_exists();
}
