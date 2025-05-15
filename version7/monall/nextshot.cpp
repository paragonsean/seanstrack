#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\plookup.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "extern.h"
#include "shotsave.h"

/***********************************************************************
*                             NEXTSHOT                                 *
***********************************************************************/
BOOLEAN nextshot( SHOT_BUFFER_ENTRY * b )
{
PLOOKUP_CLASS plookup;
SYSTEMTIME    shot_time;

FileTimeToSystemTime( &b->ph.time_of_shot, &shot_time );

/*
--------------------------------------------
If there is no plookup record, start at one.
-------------------------------------------- */
b->ph.shot_number = 0;
plookup.init( ComputerName, b->machine, b->part );
plookup.setsize( PLOOKUP_SETPOINT_COUNT );
if ( plookup.get() )
    b->ph.shot_number = plookup.get_last_shot_number();

b->ph.shot_number++;
plookup.set_last_shot_number( b->ph.shot_number );
plookup.set_time( shot_time );
return plookup.put();
}
