#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\machine.h"
#include "..\include\plookup.h"
#include "..\include\shotname.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\vdbclass.h"
#include "..\include\verrors.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

#define SHOTPARM_TABLE_TYPE 1
#define GRAPHLST_TABLE_TYPE 2

extern HWND    ProgressWindow;
extern BOOLEAN HaveProgressCancel;

void st_resource_message_box( TCHAR * msg_id, TCHAR * title_id );
int  st_resource_message_box( TCHAR * msg_id, TCHAR * title_id, UINT boxtype );

/***********************************************************************
*                        GET_GRAPHLST_SHOT_ENTRY                       *
***********************************************************************/
void get_graphlst_shot_entry( SHOT_ENTRY & se, DB_TABLE & t )
{
t.get_alpha( se.name, t.field_offset(GRAPHLST_SHOT_NAME_INDEX) );
t.get_time(  se.time, t.field_offset(GRAPHLST_TIME_INDEX) );
t.get_date(  se.time, t.field_offset(GRAPHLST_DATE_INDEX) );
}

/***********************************************************************
*              GET_NEWEST_AND_OLDEST_GRAPHLST_SHOT_ENTRIES             *
*                                                                      *
* Return the number of records of this type. 0 => entries not changed. *
***********************************************************************/
int32 get_newest_and_oldest_graphlst_shot_entries( SHOT_RANGE_ENTRY & shot, PART_NAME_ENTRY & sorc )
{
int32      n;
NAME_CLASS s;
DB_TABLE   t;

s.get_graphlst_dbname( sorc.computer, sorc.machine, sorc.part );

n = 0;
if ( t.open( s, AUTO_RECLEN, PFL) )
    {
    n = t.nof_recs();
    if ( n > 0 )
        {
        t.get_next_record( FALSE );

        get_graphlst_shot_entry( shot.oldest, t );

        if ( n > 1 )
            {
            t.goto_record_number(n-1);
            t.get_rec();
            }

        get_graphlst_shot_entry( shot.newest, t );
        }
    t.close();
    }

return n;
}

/***********************************************************************
*                       GET_SHOTPARM_SHOT_ENTRY                        *
***********************************************************************/
void get_shotparm_shot_entry( SHOT_ENTRY & se, VDB_CLASS & t )
{
se.name = t.ra[SHOTPARM_SHOT_NAME_INDEX];
t.get_time( se.time, SHOTPARM_TIME_INDEX );
t.get_date( se.time, SHOTPARM_DATE_INDEX );
}

/***********************************************************************
*               GET_NEWEST_AND_OLDEST_SHOTPARM_SHOT_ENTRIES            *
*                                                                      *
* Return the number of records of this type. 0 => entries not changed. *
***********************************************************************/
int32 get_newest_and_oldest_shotparm_shot_entries( SHOT_RANGE_ENTRY & shot, PART_NAME_ENTRY & sorc )
{
int32      n;
NAME_CLASS s;
VDB_CLASS  t;

s.get_shotparm_csvname( sorc.computer, sorc.machine, sorc.part );

n = 0;
if ( t.open_for_read(s) )
    {
    n = t.nof_recs();
    if ( n > 0 )
        {
        t.get_next_record();
        get_shotparm_shot_entry( shot.oldest, t );

        if ( n > 1 )
            t.get_rec(n-1);

        get_shotparm_shot_entry( shot.newest, t );
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

np = get_newest_and_oldest_shotparm_shot_entries( p, sorc );
ng = get_newest_and_oldest_graphlst_shot_entries( g, sorc );

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
*                       CHECK_GRAPHLST_FOR_RESIZE                      *
*                                                                      *
* Check to see if the record length of the dest is less than the sorc. *
* If it is, resize all the records in the dest table.                  *
*                                                                      *
***********************************************************************/
short check_graphlst_for_resize( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc )
{
NAME_CLASS dn;
NAME_CLASS sn;
DB_TABLE   db;
short      dest_field_length   = 0;
BOOLEAN    dest_is_alpha       = FALSE;
short      dest_record_length  = 0;
short      final_field_length  = 0;
short      final_record_length = 0;
short      sorc_field_length   = 0;
BOOLEAN    sorc_is_alpha       = FALSE;
short      sorc_record_length  = 0;

sn.get_graphlst_dbname( sorc.computer, sorc.machine, sorc.part );
if ( sn.file_exists() )
    {
    if ( db.open(sn, AUTO_RECLEN, PFL) )
        {
        sorc_field_length  = (int) db.field_length( GRAPHLST_SHOT_NAME_INDEX );
        sorc_is_alpha      = db.is_alpha( GRAPHLST_SHOT_NAME_INDEX );
        sorc_record_length = db.record_length();
        db.close();
        final_field_length  = sorc_field_length;
        final_record_length = sorc_record_length;
        if ( !sorc_is_alpha )
            final_record_length += 2;
        }
    }

dn.get_graphlst_dbname( dest.computer, dest.machine, dest.part );
if ( dn.file_exists() )
    {
    if ( db.open(dn, AUTO_RECLEN, PFL) )
        {
        dest_field_length  = (int) db.field_length( GRAPHLST_SHOT_NAME_INDEX );
        dest_is_alpha      = db.is_alpha( GRAPHLST_SHOT_NAME_INDEX );
        dest_record_length = db.record_length();
        db.close();
        if ( dest_record_length >= final_record_length )
            {
            final_record_length = dest_record_length;
            if ( !dest_is_alpha )
                final_record_length += 2;
            if ( dest_field_length > final_field_length )
                final_field_length = dest_field_length;
            }
        }
    }

if ( final_record_length > sorc_record_length && sorc_record_length > 0 )
    upsize_table_w_shot_name( sn, final_field_length );

if ( final_record_length > dest_record_length && dest_record_length > 0 )
    upsize_table_w_shot_name( dn, final_field_length );

return final_record_length;
}

/***********************************************************************
*                         MERGE_GRAPHLST_FILES                         *
***********************************************************************/
static BOOLEAN merge_graphlst_files( PART_NAME_ENTRY & destpa, PART_NAME_ENTRY & sorcpa )
{
int              nd;
int              ns;
short            record_length;
DB_TABLE         dest;
DB_TABLE         sorc;
DB_TABLE         temp;
STRING_CLASS     dest_shot_name;
STRING_CLASS     sorc_shot_name;
NAME_CLASS       fn;
BOOLEAN          replace_existing_files = TRUE;
BOOLEAN          have_sorc_record;
BOOLEAN          have_dest_record;

nd = 0;
ns = 0;
have_dest_record = FALSE;
have_sorc_record = FALSE;
record_length = check_graphlst_for_resize( destpa, sorcpa );

fn.get_part_results_dir_file_name( destpa.computer, destpa.machine, destpa.part, TEMP_GRAPHLST_DB );
temp.create( fn );
if ( temp.open(fn, record_length, WL) )
    {
    fn.get_graphlst_dbname( sorcpa.computer, sorcpa.machine, sorcpa.part );
    if ( sorc.open(fn, record_length, PFL) )
        {
        ns = (int) sorc.nof_recs();
        /*
        ------------------------------------------
        If there is nothing to do, exit gracefully
        ------------------------------------------ */
        if ( ns < 1 )
            {
            sorc.close();
            temp.close();
            return TRUE;
            }
        sorc.determine_column_info();
        fn.get_graphlst_dbname( destpa.computer, destpa.machine, destpa.part );
        if ( dest.open(fn, record_length, PFL) )
            {
            nd = dest.nof_recs();
            if ( nd > 0 )
                dest.determine_column_info();

            while ( true )
                {
                if ( ns > 0 && !have_sorc_record )
                    {
                    have_sorc_record = sorc.get_next_record( FALSE );
                    if ( have_sorc_record )
                        {
                        sorc.get_alpha( sorc_shot_name, GRAPHLST_SHOT_NAME_INDEX );
                        fix_shotname( sorc_shot_name );
                        ns--;
                        }
                    else
                        {
                        ns = 0;
                        }
                    }
                if ( nd > 0 && !have_dest_record )
                    {
                    have_dest_record =  dest.get_next_record( FALSE );
                    if ( have_dest_record )
                        {
                        dest.get_alpha( dest_shot_name, GRAPHLST_SHOT_NAME_INDEX );
                        fix_shotname( dest_shot_name );
                        nd--;
                        }
                    else
                        {
                        nd = 0;
                        }
                    }

                if ( !have_sorc_record && !have_dest_record )
                    break;

                if ( have_sorc_record && have_dest_record )
                    {
                    if ( sorc_shot_name == dest_shot_name )
                        {
                        if ( replace_existing_files )
                            {
                            temp.copy_rec( sorc );
                            temp.rec_append();
                            copy_profile( destpa, sorcpa, sorc_shot_name );
                            }
                        else
                            {
                            temp.copy_rec( dest );
                            temp.rec_append();
                            }
                        have_dest_record = FALSE;
                        have_sorc_record = FALSE;
                        }
                    else if ( sorc_shot_name < dest_shot_name )
                        {
                        temp.copy_rec( sorc );
                        temp.rec_append();
                        copy_profile( destpa, sorcpa, sorc_shot_name );
                        have_sorc_record = FALSE;
                        }
                    else
                        {
                        temp.copy_rec( dest );
                        temp.rec_append();
                        have_dest_record = FALSE;
                        }
                    }
                else if ( have_sorc_record )
                    {
                    temp.copy_rec( sorc );
                    temp.rec_append();
                    copy_profile( destpa, sorcpa, sorc_shot_name );
                    have_sorc_record = FALSE;
                    }
                else
                    {
                    temp.copy_rec( dest );
                    temp.rec_append();
                    have_dest_record = FALSE;
                    }
                }
            dest.close();
            }
        sorc.close();
        }
    temp.close();
    }

/*
-----------------------------------------------------------------------
Use the dest_shot_name to hold the name of the official graphlst dbname
----------------------------------------------------------------------- */
fn.get_graphlst_dbname( destpa.computer, destpa.machine, destpa.part );
dest_shot_name = fn;
fn.delete_file();
fn.get_part_results_dir_file_name( destpa.computer, destpa.machine, destpa.part, TEMP_GRAPHLST_DB );
fn.moveto( dest_shot_name );
return TRUE;
}

/***********************************************************************
*                         MERGE_SHOTPARM_FILES                         *
***********************************************************************/
static BOOLEAN merge_shotparm_files( PART_NAME_ENTRY & destpa, PART_NAME_ENTRY & sorcpa )
{
int              nd;
int              ns;
VDB_CLASS        dest;
VDB_CLASS        sorc;
VDB_CLASS        temp;
STRING_CLASS     dest_shot_name;
STRING_CLASS     sorc_shot_name;
NAME_CLASS       fn;
BOOLEAN          replace_existing_records = TRUE;
BOOLEAN          have_sorc_record;
BOOLEAN          have_dest_record;

nd = 0;
ns = 0;
have_dest_record = FALSE;
have_sorc_record = FALSE;

fn.get_part_results_dir_file_name( destpa.computer, destpa.machine, destpa.part, TEMP_SHOTPARM_CSV );
temp.create( fn );
if ( temp.open_for_write(fn) )
    {
    fn.get_shotparm_csvname( sorcpa.computer, sorcpa.machine, sorcpa.part );
    if ( sorc.open_for_read(fn) )
        {
        ns = (int) sorc.nof_recs();
        /*
        ------------------------------------------
        If there is nothing to do, exit gracefully
        ------------------------------------------ */
        if ( ns < 1 )
            {
            sorc.close();
            temp.close();
            return TRUE;
            }
        fn.get_shotparm_csvname( destpa.computer, destpa.machine, destpa.part );
        if ( dest.open_for_read(fn) )
            {
            nd = dest.nof_recs();
            while ( true )
                {
                if ( ns > 0 && !have_sorc_record )
                    {
                    have_sorc_record = sorc.get_next_record();
                    if ( have_sorc_record )
                        {
                        sorc_shot_name = sorc.ra[SHOTPARM_SHOT_NAME_INDEX];
                        fix_shotname( sorc_shot_name );
                        ns--;
                        }
                    else
                        {
                        ns = 0;
                        }
                    }
                if ( nd > 0 && !have_dest_record )
                    {
                    have_dest_record =  dest.get_next_record();
                    if ( have_dest_record )
                        {
                        dest_shot_name = dest.ra[SHOTPARM_SHOT_NAME_INDEX];
                        fix_shotname( dest_shot_name );
                        nd--;
                        }
                    else
                        {
                        nd = 0;
                        }
                    }

                if ( !have_sorc_record && !have_dest_record )
                    break;

                if ( have_sorc_record && have_dest_record )
                    {
                    if ( sorc_shot_name == dest_shot_name )
                        {
                        if ( replace_existing_records )
                            {
                            temp.copy_rec( sorc );
                            temp.rec_append();
                            }
                        else
                            {
                            temp.copy_rec( dest );
                            temp.rec_append();
                            }
                        have_dest_record = FALSE;
                        have_sorc_record = FALSE;
                        }
                    else if ( sorc_shot_name < dest_shot_name )
                        {
                        temp.copy_rec( sorc );
                        temp.rec_append();
                        have_sorc_record = FALSE;
                        }
                    else
                        {
                        temp.copy_rec( dest );
                        temp.rec_append();
                        have_dest_record = FALSE;
                        }
                    }
                else if ( have_sorc_record )
                    {
                    temp.copy_rec( sorc );
                    temp.rec_append();
                    have_sorc_record = FALSE;
                    }
                else
                    {
                    temp.copy_rec( dest );
                    temp.rec_append();
                    have_dest_record = FALSE;
                    }
                }
            dest.close();
            }
        sorc.close();
        }
    temp.close();
    }

/*
------------------------------------------------------------------------
Use the dest_shot_name to hold the name of the official shotparm csvname
------------------------------------------------------------------------ */
fn.get_shotparm_csvname( destpa.computer, destpa.machine, destpa.part );
dest_shot_name = fn;
fn.delete_file();
fn.get_part_results_dir_file_name( destpa.computer, destpa.machine, destpa.part, TEMP_SHOTPARM_CSV );
fn.moveto( dest_shot_name );
return TRUE;
}

/***********************************************************************
*                             RESTORE_DATA                             *
***********************************************************************/
BOOLEAN restore_data( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc )
{
DB_TABLE         d;
DB_TABLE         s;
VDB_CLASS        dv;
VDB_CLASS        sv;
int32            nd;
int              shot_number_length;
STRING_CLASS     shot_name;
SHOT_NAME_CLASS  snc;
SHOT_RANGE_ENTRY drange;
SHOT_RANGE_ENTRY srange;
NAME_CLASS       fn;
PLOOKUP_CLASS    plookup;

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
-------------
Copy profiles
------------- */
if ( BackupType & PROFILE_DATA )
    merge_graphlst_files( dest, sorc );

/*
---------------
Copy parameters
--------------- */
if ( BackupType & PARAMETER_DATA )
    merge_shotparm_files( dest, sorc );

/*
-------------------
Get the newest shot
------------------- */
nd = shot_range( drange, dest );
if ( nd <= 0 )
    {
    drange.newest.name.null();
    GetSystemTime( &drange.newest.time );
    }

/*
-----------------------------------------------------------------
Get the number of digits used by the shot number in the shot name
----------------------------------------------------------------- */
snc.init( sorc.computer, sorc.machine, sorc.part );
snc.get();
shot_number_length = snc.shot_number_length();

plookup.init( dest.computer, dest.machine, dest.part );
plookup.get();
plookup.set_last_shot_name( drange.newest.name  );
nd = shot_number_from_shot_name( drange.newest.name, shot_number_length );
plookup.set_last_shot_number( (int) nd );
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
*                    RESTORE_MISSING_ANALOG_SENSORS                    *
* I don't want to restore the asensor.csv file from the backup but     *
* some of the parts need sensors that do not exist in the current file.*
* I have to copy just these records from the backup to the current     *
* asensor.csv file.                                                    *
***********************************************************************/
static void restore_missing_analog_sensors( int highest_analog_sensor_number, STRING_CLASS & sorc_machine )
{
NAME_CLASS d;
NAME_CLASS s;
FILE_CLASS df;
FILE_CLASS sf;
int        i;
int        n;

d.get_asensor_csvname();
if ( d.file_exists() )
    {
    if ( df.open_for_read(d) )
        {
        n = df.nof_lines();
        df.close();
        if ( highest_analog_sensor_number >= n )
            {
            s.get_machine_dir_file_name( BackupComputer, sorc_machine, ASENSOR_CSV );
            if ( s.file_exists() )
                {
                sf.open_for_read(s);
                for ( i=0; i<n; i++ )
                    sf.readline();
                df.open_for_append(d);
                while ( i<=highest_analog_sensor_number )
                    {
                    s = sf.readline();
                    if ( s.isempty() )
                        break;
                    df.writeline( s );
                    i++;
                    }
                df.close();
                sf.close();
                }
            }
        }
    }
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
*                  get_highest_analog_sensor_number                    *
***********************************************************************/
static void get_highest_analog_sensor_number( int & highest_analog_sensor_number, PART_NAME_ENTRY & p )
{
int    i;
DB_TABLE t;
NAME_CLASS s;

s.get_ftchan_dbname( p.computer, p.machine, p.part );

if ( t.open(s, FTCHAN_RECLEN, PFL) )
    {
    while( t.get_next_record(NO_LOCK) )
        {
        i = t.get_long( FTCHAN_SENSOR_NUMBER_OFFSET );
        if ( i > highest_analog_sensor_number )
            highest_analog_sensor_number = i;
        }
    }

t.close();
}

/***********************************************************************
*                            RESTORE_DATA                              *
***********************************************************************/
void restore_data()
{
const BITSETYPE COPY_BITS_MASK  = MA_MONITORING_ON | MA_ALARM_MONITORING_ON | MA_USE_TIME_TO_MEASURE_BISCUIT;
const BITSETYPE CLEAR_BITS_MASK = ~COPY_BITS_MASK;

int     i;
int     choice;
int     highest_analog_sensor_number;
BITSETYPE flags;

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
highest_analog_sensor_number = 0;

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
        st_resource_message_box( TEXT("CANT_LOAD_MA"), TEXT("CANNOT_COMPLY_STRING") );
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
        st_resource_message_box( TEXT("NO_MA_ON_WS"), TEXT("NO_DEST") );
        continue;
        }

    if ( !c.is_this_computer() && !is_workstation_zero() )
        {
        st_resource_message_box( TEXT("MUST_BE_AT_CO"), TEXT("MA_ON_OTHER_CO") );
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
                choice = st_resource_message_box( TEXT("MON_OFF"), TEXT("RESTORING_CUR_PA"), MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL );
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

        /*
        -------------------------------------
        I only want to copy the montior flags
        ------------------------------------- */
        flags = dm.monitor_flags;
        flags &= COPY_BITS_MASK;;
        m.monitor_flags &= CLEAR_BITS_MASK;
        m.monitor_flags |= flags;

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
            st_resource_message_box( TEXT("CANNOT_COMPLY_STRING"), TEXT("SEMAPHOR_FAIL_STRING") );
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
            {
            copypart( dest.computer, dest.machine, dest.part, sorc.computer, sorc.machine, sorc.part, COPY_PART_ALL );
            if ( !(BackupType & ANALOG_SENSOR_DATA)  )
                get_highest_analog_sensor_number( highest_analog_sensor_number, dest );
            }
        restore_data( dest, sorc );

        }

    if ( i_own_the_semaphor )
        free_shotsave_semaphore();

   if ( !(BackupType & ANALOG_SENSOR_DATA)  )
       restore_missing_analog_sensors( highest_analog_sensor_number, sorc.machine );
   }
}
