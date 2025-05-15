#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\machname.h"
#include "..\include\setpoint.h"
#include "..\include\subs.h"
#include "..\include\structs.h"
#include "..\include\wclass.h"

#include "extern.h"
#include "resource.h"

/***********************************************************************
*                        GET_CURRENT_PART_NAME                         *
* This assumes the computer name has already been put into             *
* SureTrakPart.computer.                                               *
***********************************************************************/
BOOLEAN get_suretrak_part_name()
{
MACHINE_NAME_LIST_CLASS mlist;
NAME_CLASS     s;
SETPOINT_CLASS sp;
DB_TABLE   t;
bool       status;
bool       is_monitor;
HWND       w;
STRING_CLASS rod_pitch;

status     = false;
is_monitor = false;

/*
-------------------------------------
Find the currently monitored machine.
------------------------------------- */
mlist.add_computer( SureTrakPart.computer );
mlist.rewind();
while ( mlist.next() )
    {
    s.get_machset_csvname( SureTrakPart.computer, mlist.name() );
    if ( sp.get(s) )
        {
        if ( MA_MONITORING_ON & sp[MACHSET_MONITOR_FLAGS_INDEX].uint_value() )
            is_monitor = true;

        /*
        ---------------------------------------------------------------------------
        Load the first machine or load the currently monitored machine if different
        --------------------------------------------------------------------------- */
        if ( !status || is_monitor )
            {
            SureTrakPart.machine = mlist.name();

            SureTrakPart.part = sp[MACHSET_CURRENT_PART_INDEX].value;
            rod_pitch = sp[MACHSET_RODPITCH_INDEX].value;
            status = true;
            }

        if ( is_monitor )
            break;
        }
    t.close();
    }

s.get_partset_csvname( SureTrakPart.computer, SureTrakPart.machine, SureTrakPart.part );
if ( sp.get(s) )
    {
    DistanceUnits = sp[PARTSET_DISTANCE_UNITS_INDEX].int_value();
    VelocityUnits = sp[PARTSET_VELOCITY_UNITS_INDEX].int_value();
    }

s.get_psensor_dbname();
if ( t.open(s, PSENSOR_RECLEN, PFL) )
    {
    t.put_alpha( PSENSOR_RODPITCH_OFFSET,   rod_pitch,        RODPITCH_NAME_LEN );
    t.put_short( PSENSOR_DIST_UNITS_OFFSET, DistanceUnits,    UNITS_ID_LEN );
    t.put_short( PSENSOR_VEL_UNITS_OFFSET,  VelocityUnits,    UNITS_ID_LEN );
    t.put_short( PSENSOR_MHZ_OFFSET,        DEF_SURETRAK_MHZ, PSENSOR_MHZ_LEN );
    t.reset_search_mode();

    if ( t.get_next_key_match(4, NO_LOCK) )
        PosSensorVelocityDividend = t.get_double( PSENSOR_VEL_FACTOR_OFFSET );

    t.close();
    }

if ( status )
    {
    w = MainScreenWindow.handle();
    if ( w )
        {
        SureTrakPart.computer.set_text( w, COMPUTER_NAME_TBOX );
        SureTrakPart.machine.set_text(  w, MACHINE_NAME_TBOX  );
        SureTrakPart.part.set_text(     w, PART_NAME_TBOX     );
        }
    }

return status;
}
