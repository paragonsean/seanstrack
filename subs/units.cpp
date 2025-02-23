#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"
#include "..\include\units.h"
#include "..\include\verrors.h"

struct UNITS_ENTRY
    {
    short id;
    BITSETYPE type;
    float sec_per_vel_unit;
    TCHAR name[UNITS_LEN+1];
    };

static TCHAR         EmptyString[] = TEXT( "" );
static UNITS_ENTRY   EmptyUnitsEntry;

static UNITS_ENTRY * UnitsArray = 0;
static short         NofUnits   = 0;

/***********************************************************************
*                              UNITS_CLASS                             *
***********************************************************************/
UNITS_CLASS::UNITS_CLASS()
{
x = 0;
rewound = true;
}

/***********************************************************************
*                              ~UNITS_CLASS                            *
***********************************************************************/
UNITS_CLASS::~UNITS_CLASS()
{
}

/***********************************************************************
*                           UNITS_CLASS::next                          *
***********************************************************************/
bool UNITS_CLASS::next()
{
if ( NofUnits < 1 )
    return false;

if ( rewound )
    {
    rewound = false;
    return true;
    }

if ( x < (NofUnits-1) )
    {
    x++;
    return true;
    }

return false;
}

/***********************************************************************
*                          UNITS_CLASS::count                          *
***********************************************************************/
int UNITS_CLASS::count()
{
return (int) NofUnits;
}

/***********************************************************************
*                          UNITS_CLASS::id                             *
***********************************************************************/
short UNITS_CLASS::id()
{
if ( NofUnits < 1 )
    return 0;

return UnitsArray[x].id;
}

/***********************************************************************
*                          UNITS_CLASS::type                           *
***********************************************************************/
BITSETYPE UNITS_CLASS::type()
{
if ( NofUnits < 1 )
    return 0;
return UnitsArray[x].type;
}


/***********************************************************************
*                          UNITS_CLASS::name                           *
***********************************************************************/
TCHAR * UNITS_CLASS::name()
{
static TCHAR noname[] = TEXT( "No Name" );

if ( NofUnits < 1 )
    return noname;

return UnitsArray[x].name;
}

/***********************************************************************
*                UNITS_CLASS::seconds_per_velocity_units               *
***********************************************************************/
float UNITS_CLASS::seconds_per_velocity_units()
{
if ( NofUnits < 1 )
    return (float) 0.0;

return UnitsArray[x].sec_per_vel_unit;
}

/***********************************************************************
*                          UNITS_CLASS::find                           *
***********************************************************************/
bool UNITS_CLASS::find( TCHAR * sorc )
{
rewind();
while ( next() )
    {
    if ( lstrcmp(sorc, UnitsArray[x].name) == 0 )
        return true;
    }

rewind();
return false;
}

/***********************************************************************
*                          UNITS_CLASS::find                           *
***********************************************************************/
bool UNITS_CLASS::find( short id_to_find )
{
rewind();
while ( next() )
    {
    if ( id_to_find == UnitsArray[x].id )
        return true;
    }

rewind();
return false;
}

/***********************************************************************
*                            UNITS_SHUTDOWN                            *
***********************************************************************/
void units_shutdown()
{
if ( UnitsArray )
    {
    delete[] UnitsArray;
    UnitsArray = 0;
    }
}

/***********************************************************************
*                            UNITS_STARTUP                             *
***********************************************************************/
BOOLEAN units_startup()
{
NAME_CLASS s;
DB_TABLE   t;
short      n;
BOOLEAN    status;

status = TRUE;
s.get_data_dir_file_name( UNITS_DB );
if ( t.open(s, UNITS_RECLEN, PFL) )
    {
    n = (short) t.nof_recs();
    if ( n > 0 )
        {
        NofUnits = 0;
        UnitsArray = new UNITS_ENTRY[n];
        if ( UnitsArray )
            {
            while ( t.get_next_record(NO_LOCK) && NofUnits < n )
                {
                UnitsArray[NofUnits].id   = t.get_short( UNITS_ID_OFFSET );
                UnitsArray[NofUnits].type = BITSETYPE(t.get_long(UNITS_VARIABLE_TYPE_OFFSET));
                UnitsArray[NofUnits].sec_per_vel_unit = t.get_float(UNITS_SEC_PER_VEL_OFFSET);
                t.get_alpha( UnitsArray[NofUnits].name, UNITS_NAME_OFFSET, UNITS_LEN );
                NofUnits++;
                }
            }
        else
            {
            NofUnits = 0;
            error_message( UNITS_STARTUP, NO_MEM_ALLOC_ERROR );
            status = FALSE;
            }

        }
    t.close();
    }

return status;
}

/***********************************************************************
*                          SHORT UNITS_INDEX                           *
***********************************************************************/
static short units_index( short uid )
{
short i;
short x;

x = 0;

for ( i=0; i<NofUnits; i++ )
    {
    if ( UnitsArray[i].id == uid )
        x = i;
    }

return x;
}

/***********************************************************************
*                              UNITS_NAME                              *
*                                                                      *
*  If you pass an invalid uid, this will return the 0th name, which    *
*  should be "NONE".                                                   *
*                                                                      *
***********************************************************************/
TCHAR * units_name( short uid )
{
short x;

x = units_index( uid );

if ( UnitsArray )
    return UnitsArray[x].name;
else
    return EmptyString;
}


/***********************************************************************
*                               UNITS_ID                               *
*  If you pass an invalid name, This will return a 0, which should be  *
*  the id of the "NONE" units.                                         *
***********************************************************************/
short units_id( TCHAR * sorc )
{
short i;
short x;

x = 0;

if ( UnitsArray )
    {
    for ( i=0; i<NofUnits; i++ )
        {
        if ( lstrcmp(sorc, UnitsArray[i].name) == 0 )
            {
            x = UnitsArray[i].id;
            break;
            }
        }
    }

return x;
}

/***********************************************************************
*                              UNITS_TYPE                              *
***********************************************************************/
BITSETYPE units_type( short uid )
{
short x;

x = units_index( uid );

if ( UnitsArray )
    return UnitsArray[x].type;
else
    return 0;
}

/***********************************************************************
*                           SEC_PER_VEL_UNIT                           *
***********************************************************************/
float sec_per_vel_unit( short uid )
{
short x;

x = units_index( uid );

if ( UnitsArray )
    return UnitsArray[x].sec_per_vel_unit;
else
    return 0;
}
