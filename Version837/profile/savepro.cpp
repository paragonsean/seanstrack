#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "extern.h"

/***********************************************************************
*                    TOGGLE_PROFILE_SAVE_FLAGS                         *
*                                                                      *
*                                                                      *
*   If the profile is to be saved, turn off all saves else turn on     *
*   all saves.                                                         *
*                                                                      *
***********************************************************************/
void toggle_profile_save_flags( void )
{
const BITSETYPE flags = SAVE_PARM_DATA | SAVE_PROFILE_DATA;

short      field_length;
short      dif;
NAME_CLASS s;
DB_TABLE   t;
BITSETYPE  save_flags;

s.get_graphlst_dbname( ComputerName, MachineName, PartName );
if ( !t.open(s, 0, WL) )
    return;
dif = 0;
if ( t.is_alpha(GRAPHLST_SHOT_NAME_INDEX) )
    {
    field_length = t.field_length( GRAPHLST_SHOT_NAME_INDEX );
    t.put_alpha( GRAPHLST_SHOT_NUMBER_OFFSET, ShotName, field_length );
    dif = field_length + 2 - SHOT_LEN;
    }
else
    {
    t.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, ShotName.int_value(), SHOT_LEN );
    }
if ( t.get_next_key_match(1, FALSE) )
    {
    save_flags  = (BITSETYPE) t.get_short( GRAPHLST_SAVE_FLAGS_OFFSET+dif );
    if ( save_flags & SAVE_PROFILE_DATA )
        save_flags &= ~(flags);
    else
        save_flags |= flags;

    t.put_short( GRAPHLST_SAVE_FLAGS_OFFSET+dif, save_flags, GRAPHLST_SAVE_FLAGS_LEN );
    t.rec_update();
    }
t.close();
}
