#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

/***********************************************************************
*                           PURGE_SHOT_DATA                            *
***********************************************************************/
void purge_shot_data( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
static TCHAR all_profiles[] = TEXT( "*.pr?" );

NAME_CLASS    s;
DB_TABLE      t;

/*
------------------
Purge the profiles
------------------ */
s.get_part_results_dir_file_name( computer, machine, part, 0 );
empty_directory( s.text(), all_profiles );
s.get_graphlst_dbname( computer, machine, part );
s.delete_file();
t.create( s );

/*
--------------------
Purge the parameters
-------------------- */
s.get_shotparm_csvname( computer, machine, part );
s.delete_file();
t.create( s );
}