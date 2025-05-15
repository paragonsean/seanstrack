#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\plookup.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

/***********************************************************************
*                                PURGE                                 *
***********************************************************************/
void purge( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
static TCHAR all_profiles[] = TEXT( "*.pr?" );

DB_TABLE t;
int32    i;
int32    shot_number;
int32    nof_profiles;
BOOL     need_plookup_update;
TCHAR  * cp;
PLOOKUP_CLASS plookup;
NAME_CLASS s;

if ( !(BackupType & WITH_PURGE) )
    return;

need_plookup_update = FALSE;

/*
------------------
Purge the profiles
------------------ */
if ( BackupType & SAVE_LAST_50 )
    {
    s.get_graphlst_dbname( computer, machine, part );
    if ( t.open( s, GRAPHLST_RECLEN, FL) )
        {
        nof_profiles = t.nof_recs();
        nof_profiles -= int32( HistoryShotCount );

        if ( nof_profiles > 0 )
            {
            for ( i=0; i<nof_profiles; i++ )
                {
                if ( t.get_next_record(NO_LOCK) )
                    {
                    shot_number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
                    s.get_profile_name( computer, machine, part, shot_number );
                    if ( !s.delete_file() )
                        {
                        cp = s.text();
                        cp += s.len();
                        cp--;
                        *cp = TwoChar;
                        s.delete_file();
                        }
                    }
                }
            }

        t.reset_search_mode();
        t.get_next_record(NO_LOCK);
        t.rec_delete( nof_profiles );

        t.close();
        }
    }
else
    {
    s.get_part_results_dir_file_name( computer, machine, part, 0 );
    empty_directory( s.text(), all_profiles );
    s.get_graphlst_dbname( computer, machine, part );
    s.delete_file();
    t.create( s );
    need_plookup_update = TRUE;
    }

/*
--------------------
Purge the parameters
-------------------- */
s.get_shotparm_dbname( computer, machine, part );
if ( BackupType & SAVE_LAST_50 )
    {
    if ( t.open( s, SHOTPARM_RECLEN, FL) )
        {
        nof_profiles = t.nof_recs();
        nof_profiles -= int32( HistoryShotCount );

        if ( nof_profiles > 0 )
            {
            t.get_next_record(NO_LOCK);
            t.rec_delete( nof_profiles );
            }
        t.close();
        }
    }
else
    {
    s.delete_file();
    t.create( s );
    }

if ( BackupType & ALARM_DATA )
    {
    s.get_alarmsum_dbname( computer, machine, part );
    if ( s.file_exists() )
        {
        t.open( s, ALARMSUM_RECLEN, FL );
        t.empty();
        t.close();
        }
    }

/*
--------------
Update plookup
-------------- */
if ( need_plookup_update )
    {
    i = 0;
    if ( NextShotNumber > 0 )
        i = NextShotNumber - 1;
    plookup.set_last_shot_number( computer, machine, part, i );
    }
}
