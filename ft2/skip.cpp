#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\setpoint.h"
#include "..\include\subs.h"
#include "..\include\genlist.h"
#include "..\include\verrors.h"

#include "extern.h"

/*
----------
State bits
---------- */
#define ACQUIRING      1
#define NOT_ACQUIRING  2
#define SKIPPING       4

struct SKIP_ENTRY
    {
    BITSETYPE state;
    BITSETYPE config;
    int32     max_saved;
    int32     skip_count;    /* Minutes or shots */
    int32     acquire_count;
    int32     current_count;
    FILETIME  last_time;
    };

struct MACH_SKIP_ENTRY
    {
    STRING_CLASS machine;
    SKIP_ENTRY parm;
    SKIP_ENTRY profile;
    };

GENERIC_LIST_CLASS SkipList;

/***********************************************************************
*                         FILL_SKIP_ENTRY                              *
*                  i is the index of the skip_count.                   *
***********************************************************************/
static void fill_skip_entry( SKIP_ENTRY * se, SETPOINT_CLASS & sp, int i )
{
SYSTEMTIME st;

se->acquire_count = sp[i + ACQUIRE_COUNT_OFFSET].int_value();
se->config        = (BITSETYPE) sp[i + FLAGS_OFFSET].uint_value();
se->max_saved     = sp[i + MAX_SAVED_OFFSET].int_value();
se->current_count = se->acquire_count;

GetLocalTime(&st);
SystemTimeToFileTime( &st, &se->last_time );

se->state = NOT_ACQUIRING;

if ( se->config & ACQUIRE_ALL_BIT )
    se->state = ACQUIRING;

else if ( se->config & (SKIP_TIME_BIT | SKIP_COUNT_BIT) )
    {
    if ( se->config & SKIP_TIME_BIT )
        se->skip_count = sp[i+ TIME_SKIP_OFFSET].int_value();
    else
        se->skip_count = sp[i].int_value();

    if ( se->acquire_count > 0 )
        se->state = ACQUIRING;
    }
}

/***********************************************************************
*                              RESET                                   *
***********************************************************************/
static BOOLEAN reset( MACH_SKIP_ENTRY * mse )
{
NAME_CLASS     s;
SETPOINT_CLASS sp;

s.get_machset_csvname( ComputerName, mse->machine );
if ( sp.get(s) )
    {
    fill_skip_entry( &mse->profile, sp, MACHSET_PROFILE_SKIP_COUNT_INDEX );
    fill_skip_entry( &mse->parm,    sp, MACHSET_PARAM_SKIP_COUNT_INDEX );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              FIND                                    *
***********************************************************************/
static MACH_SKIP_ENTRY * find( STRING_CLASS & machine_to_find )
{

static MACH_SKIP_ENTRY * mse = 0;

/*
-------------------------------
Find the entry for this machine
------------------------------- */
if ( !mse || (mse->machine != machine_to_find) )
    {
    SkipList.rewind();
    while ( TRUE )
        {
        mse = (MACH_SKIP_ENTRY *) SkipList.next();
        if ( !mse )
            break;
        if ( mse->machine == machine_to_find )
            break;
        }
    }

/*
-------------------------------------
If I didn't find an entry, create one
------------------------------------- */
if ( !mse )
    {
    mse = new MACH_SKIP_ENTRY;
    if ( mse )
        {
        mse->machine = machine_to_find;
        reset( mse );
        if ( !SkipList.append( mse ) )
            {
            delete mse;
            mse = 0;
            error_message( SKIP_FIND, NO_MEM_ALLOC_ERROR );
            }
        }
    }

if ( !mse )
    error_message( SKIP_FIND, NO_MEM_ALLOC_ERROR );

return mse;
}

/***********************************************************************
*                         SKIP_RESET_MACHINE                           *
***********************************************************************/
BOOLEAN skip_reset_machine( STRING_CLASS & machine )
{
MACH_SKIP_ENTRY * mse;

mse = find( machine );
if ( mse )
    return reset( mse );

return FALSE;
}

/***********************************************************************
*                            SKIP_STARTUP                              *
***********************************************************************/
BOOLEAN skip_startup( void )
{
return TRUE;
}

/***********************************************************************
*                            SKIP_SHUTDOWN                             *
***********************************************************************/
void skip_shutdown( void )
{

MACH_SKIP_ENTRY * mse;

SkipList.rewind();
while ( TRUE )
    {
    mse = (MACH_SKIP_ENTRY *) SkipList.next();
    if ( !mse )
        break;
    delete mse;
    }

SkipList.remove_all();
}

/***********************************************************************
*                       INCREMENT_SHOT_ENTRY                           *
***********************************************************************/
static BOOLEAN increment_shot_entry( SKIP_ENTRY * se, FILETIME time_of_shot )
{
BOOLEAN return_status;
int32    time_dif;
return_status = FALSE;

if ( se->state & ACQUIRING )
    {
    /*
    -----------------------
    I always save this shot
    ----------------------- */
    return_status = TRUE;

    /*
    ---------------------------------------
    See if it it time to switch to skipping
    --------------------------------------- */
    if ( !(se->config & ACQUIRE_ALL_BIT) )
        {
        if ( se->config & (SKIP_COUNT_BIT | SKIP_TIME_BIT) && se->skip_count > 0 )
            {
            se->current_count--;
            if ( !se->current_count )
                {
                se->current_count = se->skip_count;
                se->last_time     = time_of_shot;
                se->state &= ~BITSETYPE(ACQUIRING);
                se->state |= SKIPPING;
                }
            }
        }
    }
else if ( se->state & SKIPPING )
    {
    /*
    ---------------------------------------------------
    Leave return FALSE so I will skip this shot. See if
    it is time to switch to acquiring.
    --------------------------------------------------- */
    if ( se->config & SKIP_COUNT_BIT )
        {
        se->current_count--;
        }
    else if ( se->config & SKIP_TIME_BIT )
        {
        time_dif = seconds_difference( se->last_time, time_of_shot );

        if ( time_dif < 0 )
            time_dif *= -1;

        if ( time_dif > (long) se->skip_count )
            se->current_count = 0;
        }

    if ( !se->current_count )
        {
        se->current_count = se->acquire_count;
        se->state &= ~BITSETYPE(SKIPPING);
        se->state |= ACQUIRING;
        }
    }

return return_status;
}

/***********************************************************************
*                       SKIP_NEW_SHOT_NOTIFY                           *
*                                                                      *
*  Call this to notify me that a new shot has been received. I will    *
*  return with SAVE_PARM_DATA set if the parameter data should be      *
*  saved and with SAVE_PROFILE_DATA set if the profile should be       *
*  saved.                                                              *
*                                                                      *
***********************************************************************/
BITSETYPE skip_new_shot_notify( STRING_CLASS & machine, FILETIME time_of_shot )
{
MACH_SKIP_ENTRY * mse;
BITSETYPE return_bits;

return_bits = 0;
mse = find( machine );

if ( mse )
    {
    if ( increment_shot_entry(&mse->parm,   time_of_shot) )
        return_bits |= SAVE_PARM_DATA;

    if ( increment_shot_entry(&mse->profile, time_of_shot) )
        return_bits |= SAVE_PROFILE_DATA;
    }

return return_bits;
}

/***********************************************************************
*                         MAX_PARAMETERS_SAVED                         *
***********************************************************************/
int32 max_parameters_saved( STRING_CLASS & machine )
{
MACH_SKIP_ENTRY * mse;

mse = find( machine );

if ( mse )
    return mse->parm.max_saved;

return 0;
}

/***********************************************************************
*                          MAX_PROFILES_SAVED                          *
***********************************************************************/
int32 max_profiles_saved( STRING_CLASS & machine )
{
MACH_SKIP_ENTRY * mse;

mse = find( machine );

if ( mse )
    return mse->profile.max_saved;

return 0;
}
