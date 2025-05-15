#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"

/***********************************************************************
*                             PART_EXISTS                              *
***********************************************************************/
BOOLEAN part_exists( STRING_CLASS & computer_to_find, STRING_CLASS & machine_to_find, STRING_CLASS & name_to_find )
{
NAME_CLASS s;

s.get_partset_csvname( computer_to_find, machine_to_find, name_to_find );

return s.file_exists();
}

/***********************************************************************
*                             PART_EXISTS                              *
***********************************************************************/
BOOLEAN part_exists( PART_NAME_ENTRY & p )
{
return part_exists( p.computer, p.machine, p.part );
}
