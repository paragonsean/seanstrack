#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\verrors.h"
#include "..\include\dbclass.h"

struct POS_SENSOR_ENTRY
    {
    POS_SENSOR_ENTRY * next;
    TCHAR rod_pitch[RODPITCH_NAME_LEN+1];
    short dist_units;
    short vel_units;
    short mhz;
    POS_SENSOR_SETUP_DATA data;
    };

static POS_SENSOR_ENTRY    * PosSensorList    = 0;

/***********************************************************************
*                         FIND_POS_SENSOR                              *
***********************************************************************/
POS_SENSOR_SETUP_DATA * find_pos_sensor( const TCHAR * rod_pitch, short dist_units, short vel_units, short mhz )
{

POS_SENSOR_ENTRY * newpe;
POS_SENSOR_ENTRY * pe;
NAME_CLASS s;
DB_TABLE   t;

pe = PosSensorList;
while ( pe )
    {
    /*
    --------------------------------
    See if this is the sensor I want
    -------------------------------- */
    if ( lstrcmp( pe->rod_pitch, rod_pitch) == 0 && (pe->dist_units == dist_units) && (pe->vel_units == vel_units) && (pe->mhz == mhz) )
        return &pe->data;

    /*
    ------------------------------------------------
    Exit pointing to the last entry so I can add one
    ------------------------------------------------ */
    if ( !pe->next )
        break;

    pe = pe->next;
    }

/*
-------------------------------------------------------
I didn't find a match in my list, get it from the table
------------------------------------------------------- */
newpe = 0;
s.get_psensor_dbname();
if ( t.open(s, PSENSOR_RECLEN, PFL) )
    {
    t.put_alpha( PSENSOR_RODPITCH_OFFSET,   rod_pitch, RODPITCH_NAME_LEN );
    t.put_short( PSENSOR_DIST_UNITS_OFFSET, dist_units, UNITS_ID_LEN );
    t.put_short( PSENSOR_VEL_UNITS_OFFSET,  vel_units,  UNITS_ID_LEN );
    t.put_short( PSENSOR_MHZ_OFFSET,        mhz,        PSENSOR_MHZ_LEN );
    t.reset_search_mode();

    if ( t.get_next_key_match(4, NO_LOCK) )
        {
        newpe = new POS_SENSOR_ENTRY;
        if ( newpe )
            {
            lstrcpy( newpe->rod_pitch, rod_pitch);
            newpe->dist_units = dist_units;
            newpe->vel_units  = vel_units;
            newpe->mhz        = mhz;
            newpe->data.x4_pos_divisor = t.get_float( PSENSOR_X4_POS_DIVISOR_OFFSET );
            newpe->data.x1_pos_divisor = t.get_float( PSENSOR_X1_POS_DIVISOR_OFFSET );
            newpe->data.vel_dividend   = t.get_float( PSENSOR_VEL_FACTOR_OFFSET );
            newpe->data.min_vel        = t.get_float( PSENSOR_MIN_VEL_OFFSET );
            newpe->data.crystal_freq   = (float) mhz;
            newpe->next = 0;
            if ( pe )
                pe->next = newpe;
            else
                PosSensorList = newpe;
            }
        else
            {
            error_message( FIND_POSITION_SENSOR, NO_MEM_ALLOC_ERROR );
            }
        }

    t.close();
    }
if ( newpe )
    return &newpe->data;

return 0;
}

/***********************************************************************
*                        SENSOR_LIST_SHUTDOWN                          *
***********************************************************************/
void sensor_list_shutdown()
{
void                * next_struct;
POS_SENSOR_ENTRY    * pe;

pe = PosSensorList;
while( pe )
    {
    next_struct = pe->next;
    delete pe;
    pe = (POS_SENSOR_ENTRY *) next_struct;
    }

PosSensorList    = 0;
}

/***********************************************************************
*                        SENSOR_LIST_STARTUP                           *
***********************************************************************/
BOOLEAN sensor_list_startup()
{
PosSensorList    = 0;
return TRUE;
}
