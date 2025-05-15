#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\machine.h"
#include "..\include\plookup.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\verrors.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

#define SHOTPARM_TABLE_TYPE 1
#define GRAPHLST_TABLE_TYPE 2

extern HWND    ProgressWindow;
extern BOOLEAN HaveProgressCancel;

BOOLEAN set_last_shot_number( PART_NAME_ENTRY & pn );
void fill_machine_list( void );

/***********************************************************************
*                            SHOW_FILE_NAME                            *
***********************************************************************/
void show_file_name( TCHAR * sorc )
{
SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, sorc );
}

/***********************************************************************
*                            GET_SHOT_ENTRY                            *
***********************************************************************/
void get_shot_entry( SHOT_ENTRY & se, DB_TABLE & t, short table_type )
{
FIELDOFFSET shot_number_offset;
FIELDOFFSET date_offset;
FIELDOFFSET time_offset;

if ( table_type == SHOTPARM_TABLE_TYPE )
    {
    shot_number_offset = SHOTPARM_SHOT_NUMBER_OFFSET;
    date_offset        = SHOTPARM_TIME_OFFSET;
    time_offset        = SHOTPARM_DATE_OFFSET;
    }
else
    {
    shot_number_offset = GRAPHLST_SHOT_NUMBER_OFFSET;
    date_offset        = GRAPHLST_TIME_OFFSET;
    time_offset        = GRAPHLST_DATE_OFFSET;
    }

se.number = t.get_long( shot_number_offset );
t.get_time( se.time, time_offset );
t.get_date( se.time, date_offset );
}

/***********************************************************************
*                  GET_NEWEST_AND_OLDEST_SHOT_ENTRIES                  *
*                                                                      *
* Return the number of records of this type. 0 => entries not changed. *
***********************************************************************/
int32 get_newest_and_oldest_shot_entries( SHOT_RANGE_ENTRY & shot, PART_NAME_ENTRY & sorc, short table_type )
{
short      reclen;
int32      n;
NAME_CLASS s;
DB_TABLE   t;

if ( table_type == SHOTPARM_TABLE_TYPE )
    {
    s.get_shotparm_dbname( sorc.computer, sorc.machine, sorc.part );
    reclen = SHOTPARM_RECLEN;
    }
else
    {
    s.get_graphlst_dbname(sorc.computer, sorc.machine, sorc.part);
    reclen = GRAPHLST_RECLEN;
    }

n = 0;
if ( t.open( s, reclen, PFL) )
    {
    n = t.nof_recs();
    if ( n > 0 )
        {
        t.get_next_record( FALSE );
        get_shot_entry( shot.oldest, t, table_type );

        if ( n > 1 )
            {
            t.goto_record_number(n-1);
            t.get_rec();
            }

        get_shot_entry( shot.newest, t, table_type );
        }
    t.close();
    }

return n;
}

/***********************************************************************
*                              SHOT_RANGE                              *
*                                                                      *
*        Return the sum of parameter shots and profile shots           *
***********************************************************************/
int32 shot_range( SHOT_RANGE_ENTRY & dest, PART_NAME_ENTRY & sorc )
{
int32 n;
int32 np;
int32 ng;
SHOT_RANGE_ENTRY p;
SHOT_RANGE_ENTRY g;

np = get_newest_and_oldest_shot_entries( p, sorc, SHOTPARM_TABLE_TYPE );
ng = get_newest_and_oldest_shot_entries( g, sorc, GRAPHLST_TABLE_TYPE );

n = np + ng;
if ( n )
    {
    if ( np == 0 )
        {
        dest = g;
        }
    else if ( ng == 0 )
        {
        dest = p;
        }
    else
        {
        if ( seconds_difference(p.oldest.time, g.oldest.time) > 0 )
            dest.oldest = g.oldest;
        else
            dest.oldest = p.oldest;

        /*
        ---------------------------------
        if newest is greater, it is newer
        --------------------------------- */
        if ( seconds_difference(p.newest.time, g.newest.time) > 0 )
            dest.newest = p.newest;
        else
            dest.newest = g.newest;
        }

    }

return n;
}

/***********************************************************************
*                             RESTORE_DATA                             *
***********************************************************************/
BOOLEAN restore_data( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc )
{
DB_TABLE         d;
DB_TABLE         s;
int32            nd;
int32            ns;
int32            shot_number;
int32            shot_increment;
SHOT_RANGE_ENTRY drange;
SHOT_RANGE_ENTRY srange;
NAME_CLASS       fn;
PLOOKUP_CLASS    plookup;
TCHAR            progress_name[SHOT_LEN+5];

/*
-----------------------
Copy alarm summary data
----------------------- */
if ( BackupType & ALARM_DATA )
    {
    fn.get_alarmsum_dbname( sorc.computer, sorc.machine, sorc.part );
    if ( fn.file_exists() )
        {
        if ( s.open(fn, ALARMSUM_RECLEN, PFL) )
            {
            fn.get_alarmsum_dbname( dest.computer, dest.machine, dest.part );
            d.ensure_existance( fn );

            if ( d.open( fn, ALARMSUM_RECLEN, WL)  )
                {
                while ( s.get_next_record(FALSE) )
                    {
                    d.copy_rec( s );
                    d.rec_append();
                    }

                d.close();
                }
            s.close();
            }
        }
    }

/*
----------------------------------
Get the range of shots to be added
---------------------------------- */
ns = shot_range( srange, sorc );

/*
---------------------------------------------------------
If there are no entries in the source directory I am done
--------------------------------------------------------- */
if ( !ns )
    return TRUE;

/*
---------------------------
Get the existing shot range
--------------------------- */
nd = shot_range( drange, dest );

/*
---------------------------------------------------------------------
If there are no entries in the destination directory I need only copy
--------------------------------------------------------------------- */
shot_increment = 0;

if ( nd > 0 )
    {
    /*
    ----------------------------------------------
    For now I am only going to append the new data
    ---------------------------------------------- */
    if ( srange.oldest.number <= drange.newest.number )
        shot_increment = drange.newest.number + 1 - srange.oldest.number;
    }

lstrcpy( progress_name + SHOT_LEN, PROFILE_NAME_SUFFIX );

/*
-------------
Copy profiles
------------- */
if ( BackupType & PROFILE_DATA )
    {
    fn.get_graphlst_dbname( sorc.computer, sorc.machine, sorc.part );
    if ( s.open(fn, GRAPHLST_RECLEN, PFL) )
        {
        fn.get_graphlst_dbname( dest.computer, dest.machine, dest.part );
        if ( d.open(fn, GRAPHLST_RECLEN, WL) )
            {
            while ( s.get_next_record(FALSE) )
                {
                shot_number = s.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
                copy_profile( dest, sorc, shot_number, shot_increment );

                insalph( progress_name, shot_number, SHOT_LEN, ZeroChar, DECIMAL_RADIX );
                show_file_name( progress_name );

                /*
                ------------------------
                Copy the graphlst record
                ------------------------ */
                d.copy_rec( s );
                if ( shot_increment )
                    {
                    shot_number += shot_increment;
                    d.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, shot_number, SHOT_LEN );
                    }
                d.rec_append();
                }
            d.close();
            }
        }
    s.close();
    }

/*
---------------
Copy parameters
--------------- */
if ( BackupType & PARAMETER_DATA )
    {
    fn.get_shotparm_dbname( sorc.computer, sorc.machine, sorc.part );
    if ( s.open(fn, SHOTPARM_RECLEN, PFL) )
        {
        fn.get_shotparm_dbname( dest.computer, dest.machine, dest.part );
        if ( d.open(fn, SHOTPARM_RECLEN, WL) )
            {
            while ( s.get_next_record(FALSE) )
                {
                d.copy_rec( s );
                if ( shot_increment )
                    {
                    shot_number = d.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );
                    shot_number += shot_increment;
                    d.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, shot_number, SHOT_LEN );
                    }
                d.rec_append();
                }

            d.close();
            }
        s.close();
        }
    }

nd = shot_range( drange, dest );
if ( nd <= 0 )
    {
    drange.newest.number = 0L;
    GetSystemTime( &drange.newest.time );
    }

plookup.init( dest.computer, dest.machine, dest.part );
plookup.get();
plookup.set_last_shot_number( drange.newest.number );
plookup.set_time( drange.newest.time );
plookup.put();

return TRUE;
}

/***********************************************************************
*                            SHOW_PROGRESS                             *
***********************************************************************/
void show_progress( PART_NAME_ENTRY sorc )
{
sorc.machine.set_text( ProgressWindow, SAVING_MACHINE_TEXT_BOX );
sorc.part.set_text( ProgressWindow, SAVING_PART_TEXT_BOX );
SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
}

/***********************************************************************
*                           RESTORE_DOWNTIME                           *
***********************************************************************/
static void restore_downtime( STRING_CLASS & computer, STRING_CLASS & machine )
{
NAME_CLASS dest;
NAME_CLASS sorc;

sorc.get_downtime_dbname( BackupComputer, machine );
dest.get_downtime_dbname( computer,       machine );

if ( sorc.file_exists() )
    sorc.copyto( dest );
}

/***********************************************************************
*                       RESTORE_SURETRAK_GLOBALS                       *
***********************************************************************/
static void restore_suretrak_globals( STRING_CLASS & computer, STRING_CLASS & machine )
{
NAME_CLASS dest;
NAME_CLASS sorc;

/*
--------------------------
Global suretrak parameters
-------------------------- */
sorc.get_stparam_file_name( BackupComputer, machine );
dest.get_stparam_file_name( computer, machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

/*
------------------
Valve test profile
------------------ */
sorc.get_valve_test_master_file_name( BackupComputer, machine );
dest.get_valve_test_master_file_name( computer, machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

/*
--------
FasTrak2
-------- */
sorc.get_ft2_editor_settings_file_name( BackupComputer, machine );
dest.get_ft2_editor_settings_file_name( computer,       machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

sorc.get_ft2_global_settings_file_name( BackupComputer, machine );
dest.get_ft2_global_settings_file_name( computer,       machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

sorc.get_ft2_ctrl_prog_steps_file_name( BackupComputer, machine );
dest.get_ft2_ctrl_prog_steps_file_name( computer,       machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

sorc.get_ft2_machine_settings_file_name( BackupComputer, machine );
dest.get_ft2_machine_settings_file_name( computer,       machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );
}

/***********************************************************************
*                           RESTORE_ASENSOR                            *
* The asensor table is copied to the machine directory of the backup   *
***********************************************************************/
static void restore_asensor( STRING_CLASS & sorc_machine )
{
NAME_CLASS d;
NAME_CLASS s;

s.get_machine_dir_file_name( BackupComputer, sorc_machine, ASENSOR_DB );
if ( s.file_exists() )
    {
    convert_asensor_db_to_new_csv( s );
    }
else
    {
    s.get_machine_dir_file_name( BackupComputer, sorc_machine, ASENSOR_CSV );
    if ( s.file_exists() )
        {
        d.get_asensor_csvname();
        d.delete_file();
        s.moveto( d );
        }
    }
}

/***********************************************************************
*                           RESTORE_BOARDS                             *
* The boards tables are copied to the machine directory of the backup  *
* Note that the boards file is in the data dir but when I back up a    *
* machine I copy the boards file into the machine dir.                 *
***********************************************************************/
static void restore_boards( STRING_CLASS & sorc_machine )
{
COMPUTER_CLASS c;
NAME_CLASS dest;
NAME_CLASS sorc;

sorc.get_machine_dir_file_name( BackupComputer, sorc_machine, BOARDS_DB );
dest.get_boards_dbname();
if ( sorc.file_exists() )
    sorc.copyto( dest );

sorc.get_machine_dir_file_name( BackupComputer, sorc_machine, FTII_BOARDS_DB );
dest.get_ft2_boards_dbname( c.whoami() );
if ( sorc.file_exists() )
    sorc.copyto( dest );
}

/***********************************************************************
*                         GET_FT_TYPE                                  *
* When I restore a machine I can't tell if it is an ft2 machine        *
* unless I have the original board lists. These are in the backup      *
* dir for the machine in question. If the board is not in the ftii     *
* board list then the machine is not ft2.                              *
***********************************************************************/
static void get_ft_type( MACHINE_CLASS & m )
{
NAME_CLASS s;
DB_TABLE   t;

m.is_ftii = FALSE;
s.get_machine_dir_file_name( BackupComputer, m.name, FTII_BOARDS_DB );
if ( s.file_exists() )
    {
    t.open( s, FTII_BOARDS_RECLEN, PFL );
    while ( t.get_next_record(FALSE) )
        {
        if ( m.ft_board_number == t.get_short(FTII_BOARDS_NUMBER_OFFSET) )
            {
            m.is_ftii = TRUE;
            break;
            }
        }
    t.close();
    }
}

/***********************************************************************
*                           IS_WORKSTATION_ZERO                        *
***********************************************************************/
BOOLEAN is_workstation_zero()
{
COMPUTER_CLASS    c;
TCHAR * cp;
STRING_CLASS ws0_name;

cp = get_ini_string( VisiTrakIniFile, ConfigSection, TEXT("WorkstationZero") );
if ( unknown(cp) )
    ws0_name = DEFAULT_WS0_NAME;
else
    ws0_name = cp;

return ( c.whoami() == ws0_name );
}

/***********************************************************************
*                      GET_BOARD_FOR_NEW_MACHINE                       *
***********************************************************************/
static void get_board_for_new_machine( MACHINE_CLASS & m )
{
NAME_CLASS s;
DB_TABLE   t;
short      new_board_number;
short      bn;

new_board_number = NO_BOARD_NUMBER;
bn               = NO_BOARD_NUMBER;
if ( m.is_ftii )
    {
    s.get_ft2_boards_dbname( m.computer );
    if ( s.file_exists() )
        {
        if ( t.open(s, FTII_BOARDS_RECLEN, PFL) )
            {
            while ( t.get_next_record(FALSE) )
                {
                bn = t.get_short( FTII_BOARDS_NUMBER_OFFSET );

                if ( new_board_number == NO_BOARD_NUMBER )
                    new_board_number = bn;

                if ( bn == m.ft_board_number )
                    {
                    new_board_number = bn;
                    break;
                    }
                }
            t.close();
            }
        }
    }
else
    {
    s.get_boards_dbname( m.computer );
    if ( s.file_exists() )
        {
        if ( t.open(s, BOARDS_RECLEN, PFL) )
            {
            while ( t.get_next_record(FALSE) )
                {
                bn = t.get_short( BOARDS_NUMBER_OFFSET );

                if ( new_board_number == NO_BOARD_NUMBER )
                    new_board_number = bn;

                if ( bn == m.ft_board_number )
                    {
                    new_board_number = bn;
                    break;
                    }
                }
            t.close();
            }
        }
    }

bn = new_board_number;
if ( bn != m.ft_board_number )
    {
    m.ft_board_number = bn;

    if ( bn == NO_BOARD_NUMBER )
        m.muxchan = NO_MUX_CHANNEL;
    else
        m.muxchan = MIN_FT_CHANNEL;
    }
}

/***********************************************************************
*                            RESTORE_DATA                              *
***********************************************************************/
void restore_data()
{
int     i;
int     choice;
BOOLEAN need_setup;
BOOLEAN have_dest_machine;
BOOLEAN i_own_the_semaphor;

PART_NAME_ENTRY dest;
PART_NAME_ENTRY sorc;

NAME_CLASS        dn;
NAME_CLASS        sn;

COMPUTER_CLASS    c;
MACHINE_CLASS     m;
MACHINE_CLASS     dm;
DB_TABLE          t;
SAVE_LIST_ENTRY * s;

if ( NtoSave < 1 )
    return;

show_progress( dest );

for ( i=0; i<NtoSave; i++ )
    {
    if ( HaveProgressCancel )
        break;

    s = SaveList+i;

    sorc.computer = BackupComputer;
    sorc.machine  = s->name.machine;
    sorc.part     = EmptyString;

    if ( !m.find(sorc.computer, sorc.machine) )
        {
        resource_message_box( MainInstance, CANT_LOAD_MACHINE_STRING, CANT_DO_THAT_STRING );
        break;
        }

    get_ft_type( m );

    /*
    ---------------------------------------------------------------------------
    The analog sensor file and the boards file are backed up in the machine dir
    of the backup computer and restored to the local computer.
    --------------------------------------------------------------------------- */
    if ( BackupType & ANALOG_SENSOR_DATA  )
        restore_asensor( sorc.machine );

    if ( BackupType & FASTRAK_BOARDS_DATA )
        restore_boards( sorc.machine );

    /*
    ---------------------------------------------
    Set the destination computer and machine name
    --------------------------------------------- */
    if ( NtoSave == 1 && !DestMachine.is_empty() )
        {
        dest.set( DestMachine.computer, DestMachine.machine );
        }
    else
        {
        if ( MachList.find(sorc.machine) )
            dest.set( MachList.computer_name(), sorc.machine );
        else
            dest.set( c.whoami(), sorc.machine );
        }

    show_progress( dest );

    have_dest_machine = dm.find( dest.computer, dest.machine );

    c.find( dest.computer );

    if ( c.is_this_computer() && is_workstation_zero() )
        {
        resource_message_box( MainInstance, NOT_ON_WS0_STRING, DEST_NOT_FOUND_STRING );
        continue;
        }

    if ( !c.is_this_computer() && !is_workstation_zero() )
        {
        resource_message_box( MainInstance, MUST_RESTORE_THERE_STRING, MACHINE_ON_OTHER_STRING );
        continue;
        }

    if ( have_dest_machine )
        {
        if ( s->part.find(dm.current_part.text()) )
            {
            dest.part = dm.current_part;
            show_progress( dest );
            choice = IDYES;
            if ( dm.monitor_flags & MA_MONITORING_ON )
                choice = resource_message_box( MainInstance, MONITOR_OFF_STRING, WILL_RESTOR_CURRENT_STRING, MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL );
            dest.part = EmptyString;
            if ( choice != IDYES )
                break;
            }
        }

    show_progress( dest );

    m.computer = dest.computer;
    m.name     = dest.machine;

    /*
    -------------------------------------
    Preserve the current monitoring setup
    ------------------------------------- */
    if ( have_dest_machine )
        {
        m.current_part        = dm.current_part;
        m.human               = dm.human;
        m.monitor_flags       = dm.monitor_flags;
        m.ft_board_number     = dm.ft_board_number;
        m.muxchan             = dm.muxchan;
        m.is_ftii             = dm.is_ftii;
        m.suretrak_controlled = dm.suretrak_controlled;
        m.profile             = dm.profile;
        m.params              = dm.params;
        }
    else
        {
        m.monitor_flags &= ~MA_MONITORING_ON;
        get_board_for_new_machine( m );
        }
    /*
    ------------------------------------------------------------------
    If this machine already exists then I must own the sempahor before
    making any changes
    ------------------------------------------------------------------ */
    i_own_the_semaphor = FALSE;
    if ( have_dest_machine )
        {
        i_own_the_semaphor = get_shotsave_semaphore( dest.computer, dest.machine );
        if ( !i_own_the_semaphor )
            {
            resource_message_box( MainInstance, CANT_DO_THAT_STRING, SEMAPHOR_FAIL_STRING );
            continue;
            }
        }

    /*
    -------------------------------------------
    Check to see that the current part is valid
    (only if this is a new machine).
    ------------------------------------------- */
    if ( !have_dest_machine && !s->part.find(m.current_part.text()) )
        {
        s->part.rewind();
        if ( s->part.next() )
            m.current_part = s->part.text();
        }

    /*
    -----------------------------------------------------------
    Save the machine setup, creating a new machine if necessary
    ----------------------------------------------------------- */
    m.save();
    if ( !have_dest_machine )
        {
        dn.get_machset_csvname( dest.computer, dest.machine );
        sn.get_machset_csvname( sorc.computer, sorc.machine );
        sn.copyto( dn );
        }

    /*
    -------------------------------------------------------------------
    If I just created a new machine, refill the machine list. I sort by
    computer because that's the way the add_computer loads initially.
    ------------------------------------------------------------------- */
    if ( !have_dest_machine )
        {
        MachList.add( dest.computer, dest.machine, SORT_BY_COMPUTER );
        CreatedNewMachine = TRUE;
        }

    if ( BackupType & DOWNTIME_DATA )
        restore_downtime( dest.computer, dest.machine );

    if ( BackupType & SURETRAK_GLOBAL_DATA )
        restore_suretrak_globals( dest.computer, dest.machine );

    s->part.rewind();
    while ( s->part.next() )
        {
        if ( HaveProgressCancel )
            break;

        dest.part = s->part.text();
        sorc.part = s->part.text();
        show_progress( dest );

        need_setup = FALSE;
        if ( BackupType & SETUP_DATA )
            need_setup = TRUE;
        else
            {
            if ( !part_exists(dest.computer, dest.machine, dest.part) )
                need_setup = TRUE;
            }

        if ( need_setup )
            copypart( dest.computer, dest.machine, dest.part, sorc.computer, sorc.machine, sorc.part, COPY_PART_ALL );

        restore_data( dest, sorc );
        }

    if ( i_own_the_semaphor )
        free_shotsave_semaphore();
    }
}
