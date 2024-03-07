#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\machine.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\vdbclass.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

#define PART_LISTBOX_LEN (PART_NAME_LEN + SHOT_LEN + SHOT_LEN + 3)
#define MAX_ZIP_CMD_LEN 150

extern HWND              ProgressWindow;
extern BOOLEAN           HaveProgressCancel;
extern STRINGTABLE_CLASS StringTable;
void st_resource_message_box( TCHAR * msg_id, TCHAR * title_id );
int st_resource_message_box( TCHAR * msg_id, TCHAR * title_id, UINT boxtype );

/***********************************************************************
*                             BACKUP_PART                              *
***********************************************************************/
static BOOLEAN backup_part( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
static TCHAR profile_suffix[] = TEXT("*") PROFILE_BOTH_SUFFIXES;
NAME_CLASS   dest;
NAME_CLASS   sorc;
DB_TABLE     t;
DB_TABLE     d;
VDB_CLASS    tv;
VDB_CLASS    dv;
int32        i;
int32        n;
int32        ntocopy;
short        field_length;
short        record_length;
NAME_CLASS   dest_profile;
NAME_CLASS   sorc_profile;
STRING_CLASS s;
DWORD        last_tick_count;
DWORD        this_tick_count;
BOOLEAN      is_alpha;
BOOLEAN      status;

status = TRUE;

part.set_text( ProgressWindow, SAVING_PART_TEXT_BOX );

/*
----------------------
Always save the setups
---------------------- */
status = copypart( BackupComputer, machine, part, computer, machine, part, COPY_PART_ALL );

if ( BackupType & PARAMETER_DATA )
    {
    SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, SHOTPARM_CSV );

    sorc.get_shotparm_csvname(computer,       machine, part );
    dest.get_shotparm_csvname(BackupComputer, machine, part );

    if ( tv.open_for_read(sorc) )
        {
        n = tv.nof_recs();
        if ( ProfilesToCopy > 0 && ProfilesToCopy<n )
            {
            n -= ProfilesToCopy;
            n--;
            tv.get_rec( n );
            dv.open_for_rw( dest );
            while ( tv.get_next_record() )
                {
                dv.copy_rec( tv );
                dv.rec_append();
                }
            dv.close();
            }
        else
            {
            sorc.copyto( dest );
            }
        tv.close();
        }
    else
        {
        status = FALSE;
        }
    }

if ( BackupType & PROFILE_DATA )
    {
    SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, GRAPHLST_DB );

    /*
    --------------------------
    Copy the graphlst database
    -------------------------- */
    sorc.get_graphlst_dbname(computer,       machine, part );
    dest.get_graphlst_dbname(BackupComputer, machine, part );

    last_tick_count = 0;
    if ( t.open(sorc, AUTO_RECLEN, PFL) )
        {
        n = t.nof_recs();
        field_length  = t.field_length( GRAPHLST_SHOT_NAME_INDEX );
        is_alpha      = t.is_alpha(     GRAPHLST_SHOT_NAME_INDEX );
        record_length = t.record_length();
        if ( BackupListCount > 0 )
            {
            d.open( dest, record_length, WL  );
            for ( i=0; i<BackupListCount; i++ )
                {
                if ( BackupList[i].machine == machine )
                    {
                    if ( BackupList[i].part == part )
                        {
                        s = BackupList[i].shot;
                        if ( is_alpha )
                            t.put_alpha( GRAPHLST_SHOT_NUMBER_OFFSET, s, field_length );
                        else
                            t.put_long(  GRAPHLST_SHOT_NUMBER_OFFSET, s.int_value(), SHOT_LEN );
                        if ( t.get_next_key_match(1, NO_LOCK) )
                            {
                            d.copy_rec( t );
                            d.rec_append();

                            sorc_profile.get_profile_ii_name( computer, machine, part, s );
                            if ( sorc_profile.file_exists() )
                                {
                                dest_profile.get_profile_ii_name( BackupComputer, machine, part, s );
                                }
                            else
                                {
                                sorc_profile.get_profile_name( computer,       machine, part, s );
                                dest_profile.get_profile_name( BackupComputer, machine, part, s );
                                }
                            this_tick_count = GetTickCount();
                            if ( last_tick_count>this_tick_count || (this_tick_count-last_tick_count) > 4 )
                               {
                               last_tick_count = this_tick_count;
                               SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, filename_from_path(sorc_profile.text()) );
                               }
                            CopyFile( sorc_profile.text(), dest_profile.text(), OVERWRITE_EXISTING );
                            }
                        }
                    }
                if ( HaveProgressCancel )
                    break;
                }
            d.close();
            }
        else if ( ProfilesToCopy >= 0  )
            {
            ntocopy = ProfilesToCopy;
            if ( ntocopy > n )
                ntocopy = n;
            /*
            ------------------------------------
            Copy the last ProfilesToCopy records
            ------------------------------------ */
            if ( ntocopy>0 && ntocopy<n )
                {
                n -= ntocopy;
                n--;
                t.goto_record_number( n );
                t.get_current_record( FALSE );
                }

            d.open( dest, record_length, WL  );
            while ( t.get_next_record(FALSE) )
                {
                /*
                ------------------
                Copy the .pr2 file
                ------------------ */
                t.get_alpha( s, GRAPHLST_SHOT_NAME_INDEX );
                fix_shotname( s );
                sorc_profile.get_profile_ii_name( computer, machine, part, s );
                if ( sorc_profile.file_exists() )
                    {
                    dest_profile.get_profile_ii_name( BackupComputer, machine, part, s );

                    this_tick_count = GetTickCount();
                    if ( last_tick_count>this_tick_count || (this_tick_count-last_tick_count) > 4 )
                        {
                        last_tick_count = this_tick_count;
                        SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, filename_from_path(sorc_profile.text()) );
                        }

                    sorc_profile.copyto( dest_profile );

                    /*
                    ------------------------
                    Copy the graphlst record
                    ------------------------ */
                    d.copy_rec( t );
                    d.rec_append();
                    }

                if ( HaveProgressCancel )
                    break;
                }
            d.close();
            }
        t.close();
        }
    else
        {
        status = FALSE;
        }

    /*
    -----------------------
    Copy the plookup record
    ----------------------- */
    sorc.get_plookup_csvname( computer, machine, part );
    dest.get_plookup_csvname( BackupComputer, machine, part );

    sorc.copyto( dest );
    }

if ( BackupType & ALARM_DATA )
    {
    SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, ALARMSUM_DB );

    sorc.get_alarmsum_dbname( computer, machine, part );
    dest.get_alarmsum_dbname( BackupComputer, machine, part );

    if ( sorc.file_exists() )
        {
        if ( t.open(sorc, ALARMSUM_RECLEN, PFL) )
            {
            sorc.copyto( dest );
            t.close();
            }
        }
    }

SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
return status;
}

/***********************************************************************
*                            PURGE_DOWNTIME                            *
***********************************************************************/
static void purge_downtime( STRING_CLASS & computer, STRING_CLASS & machine )
{
DB_TABLE t;
int32    n;
NAME_CLASS s;

if ( BackupType & PURGE_DOWNTIME )
    {
    s.get_downtime_dbname( computer, machine );
    if ( s.file_exists() )
        {
        if ( t.open( s, DOWNTIME_RECLEN, FL) )
            {
            n = t.nof_recs();
            if ( n > 0 )
                {
                t.goto_record_number( n-1 );
                t.get_current_record( NO_LOCK );
                }
            t.empty();
            if ( n > 0 )
                t.rec_append();
            t.close();
            }
        }
    }
}

/***********************************************************************
*                       BACKUP_SURETRAK_GLOBALS                        *
*       Copy the suretrak global setups to the MACHINE directory       *
***********************************************************************/
static void backup_suretrak_globals( STRING_CLASS & computer, STRING_CLASS & machine )
{
NAME_CLASS dest;
NAME_CLASS sorc;

/*
--------------------------
Global suretrak parameters
-------------------------- */
sorc.get_stparam_file_name( computer, machine );
if ( !sorc.file_exists() )
    sorc.get_stparam_file_name( computer );
dest.get_stparam_file_name( BackupComputer, machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

/*
------------------
Valve test profile
------------------ */
sorc.get_valve_test_master_file_name( computer, machine );
if ( !sorc.file_exists() )
    sorc.get_valve_test_master_file_name( computer );
dest.get_valve_test_master_file_name( BackupComputer, machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

/*
-------------------------------------
Copy the FasTrak2 files if they exist
------------------------------------- */
sorc.get_ft2_editor_settings_file_name( computer, machine );
dest.get_ft2_editor_settings_file_name( BackupComputer, machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

sorc.get_ft2_global_settings_file_name( computer, machine );
dest.get_ft2_global_settings_file_name( BackupComputer, machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

sorc.get_ft2_ctrl_prog_steps_file_name( computer, machine );
dest.get_ft2_ctrl_prog_steps_file_name( BackupComputer, machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );

sorc.get_ft2_machine_settings_file_name( computer, machine );
dest.get_ft2_machine_settings_file_name( BackupComputer, machine );
if ( sorc.file_exists() )
    sorc.copyto( dest );
}

/***********************************************************************
*                             BACKUP_DATA                              *
***********************************************************************/
void backup_data()
{
const BITSETYPE backup_mask = SETUP_DATA | PROFILE_DATA | DOWNTIME_DATA | PARAMETER_DATA | ALARM_DATA;
const DWORD TEN_MINUTES = 600000L;
const DWORD THIRTY_SECONDS = 30000L;
int     i;

NAME_CLASS dest;
NAME_CLASS sorc;

STRING_CLASS computer;
STRING_CLASS machine;
STRING_CLASS part;

COMPUTER_CLASS c;
DB_TABLE       t;
BOOLEAN        backing_up;
SAVE_LIST_ENTRY * s;
BOOLEAN        backup_ok;

dest.init( MAX_PATH+MAX_ZIP_CMD_LEN );

if ( BackupAllTextFiles )
    {
    if ( BackupZipFile.file_exists() )
        BackupZipFile.delete_file();

    dest = c.whoami();
    if ( dest.directory_exists() )
        {
        SetCurrentDirectory( dest.text() );

        dest.get_exe_dir_file_name( c.whoami(), TEXT("pkzip25 -add -dir -lev=9 -excl=graphlst.txt -excl=shotparm.txt -excl=alarmsum.txt -excl=downtime.txt ") );

        dest += BackupZipFile;
        dest += TEXT( " data\\*.ini data\\*.txt data\\*.dat data\\*.csv data\\*.msg data\\*.prg exes\\*.ini exes\\*.txt display\\*.*" );

        execute_and_wait( dest.text(), NULL, THIRTY_SECONDS );

        if ( dest.get_exe_dir_file_name(c.whoami(), 0) )
            SetCurrentDirectory( dest.text() );
        }
    else
        {
        st_resource_message_box( TEXT("NO_EXES_DIR"), TEXT("CANNOT_COMPLY_STRING") );
        }
    return;
    }

if ( NtoSave < 0 )
    return;

/*
------------------------------------------------
See if I am backing something up or just purging
------------------------------------------------ */
backing_up = TRUE;
backup_ok = TRUE;
if ( (BackupType & backup_mask) == 0 )
    backing_up = FALSE;

if ( backing_up )
    {
    while ( !disk_is_in_drive(BackupZipFile.text()) )
        {
        i = st_resource_message_box( TEXT("TRY_AGAIN"), TEXT("NO_DISK"), MB_YESNO );
        if ( i != IDYES )
            return;
        }

    dest = BackupZipFile;
    if ( dir_from_filename(dest.text()) )
        {
        if ( !dest.directory_exists() )
            {
            if ( !dest.create_directory() )
                {
                sorc = StringTable.find( TEXT("NO_ZIP_DIR") );
                MessageBox( 0, BackupZipFile.text(), sorc.text(), MB_OK | MB_SYSTEMMODAL );
                return;
                }
            }
        }
    }

/*
-----------------------------------------------------
Create the root directory if it doesn't exist already
----------------------------------------------------- */
if ( backing_up && !path_is_drive_only(BackupDir.text()) )
    {
    dest = BackupDir;
    dest.remove_ending_backslash();

    /*
    --------------------------------------------
    Empty the destination directory if it exists
    -------------------------------------------- */
    if ( dest.directory_exists() )
        dest.delete_directory();

    if ( !dest.create_directory() )
        {
        sorc = StringTable.find( TEXT("DIR_CREATE_FAIL") );
        MessageBox( 0, dest.text(), sorc.text(), MB_OK );
        return;
        }
    }

for ( i=0; i<NtoSave; i++ )
    {
    if ( HaveProgressCancel )
        break;

    s = SaveList+i;

    computer = s->name.computer;
    machine  = s->name.machine;

    if ( !machine.isempty() )
        {
        machine.set_text( ProgressWindow, SAVING_MACHINE_TEXT_BOX  );
        set_text( ProgressWindow, SAVING_PART_TEXT_BOX, EmptyString );
        set_text( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );

        if ( HaveProgressCancel )
            break;

        if ( machine_exists(computer, machine) )
            {
            if ( !waitfor_shotsave_semaphor(computer, machine) )
                {
                st_resource_message_box( TEXT("CANNOT_COMPLY_STRING"), TEXT("SEMAPHOR_FAIL_STRING") );
                continue;
                }

            if ( backing_up )
                {
                sorc.get_machine_directory( computer, machine );
                dest.get_machine_directory( BackupComputer, machine );
                dest.create_directory();

                if ( !copy_all_files(dest.text(), sorc.text(), StarDotStar, WITHOUT_SUBS, 0, 0) )
                    backup_ok = FALSE;

                dest.get_machine_results_directory( BackupComputer, machine );
                dest.create_directory();

                /*
                ------------------------------------------------------------------------------------------
                Make a copy of the boards and analogs tables in the machine folder since everything is
                being saved as one backup computer and the machines could all be from different computers.
                ------------------------------------------------------------------------------------------ */
                sorc.get_boards_dbname( computer );
                dest.get_machine_dir_file_name( BackupComputer, machine, BOARDS_DB );
                if ( sorc.file_exists() )
                    sorc.copyto( dest );

                sorc.get_ft2_boards_dbname( computer );
                dest.get_machine_dir_file_name( BackupComputer, machine, FTII_BOARDS_DB );
                if ( sorc.file_exists() )
                    sorc.copyto( dest );

                sorc.get_data_dir_file_name( computer, DCURVE_DB );
                dest.get_machine_dir_file_name( BackupComputer, machine, DCURVE_DB );
                if ( sorc.file_exists() )
                    sorc.copyto( dest );

                /*
                ---------------------------------------------------------------------
                The analog sensor table is in the root directory of the sorc computer
                --------------------------------------------------------------------- */
                sorc.get_asensor_csvname( computer );
                dest.get_machine_dir_file_name( BackupComputer, machine, ASENSOR_CSV );
                if ( sorc.file_exists() )
                    sorc.copyto( dest );

                /*
                --------
                Downtime
                -------- */
                sorc.get_downtime_dbname( computer, machine );
                dest.get_downtime_dbname( BackupComputer, machine );
                if ( BackupType & DOWNTIME_DATA )
                    {
                    if ( sorc.file_exists() )
                        sorc.copyto( dest );
                    }

                /*
                ------------------------------------------------------------------
                If I'm not backing up the downtimes I need to create an empty file
                ------------------------------------------------------------------ */
                t.ensure_existance( dest );

                /*
                --------------------------------------------------------------------------
                I used to do this only with suretrak machines but now I do it for everyone
                as the ftii files span both types.
                -------------------------------------------------------------------------- */
                if ( BackupType & SETUP_DATA )
                    backup_suretrak_globals( computer, machine );
                }

            s->part.rewind();
            while ( s->part.next() )
                {
                if ( HaveProgressCancel )
                    break;

                part = s->part.text();

                if ( backing_up )
                    {
                    if ( !backup_part(computer, machine, part) )
                        backup_ok = FALSE;
                    }
                }

            free_shotsave_semaphore();
            }
        }
    }

if ( HaveProgressCancel )
    return;

if ( backing_up )
    {
    if ( BackupZipFile.file_exists() )
        BackupZipFile.find_unique_name();

    dest = BackupDir;
    dest.remove_ending_backslash();
    SetCurrentDirectory( dest.text() );

    dest.get_exe_dir_file_name( c.whoami(), TEXT("pkzip25 -add -dir ") );
    dest += BackupZipFile;
    dest += TEXT( " *.* " );

    if ( !execute_and_wait(dest.text(), NULL, TEN_MINUTES) )
        {
        dest = StringTable.find( TEXT("NO_BUP") );
        MessageBox( 0, dest.text(), BackupZipFile.text(), MB_OK | MB_SYSTEMMODAL );
        backup_ok = FALSE;
        }

    if ( dest.get_exe_dir_file_name(c.whoami(), 0) )
        SetCurrentDirectory( dest.text() );

    if ( !BackupZipFile.file_exists() )
        {
        dest = StringTable.find( TEXT("ZIP_CREATE_FAIL") );
        MessageBox( 0, dest.text(), BackupZipFile.text(), MB_OK | MB_SYSTEMMODAL );
        backup_ok = FALSE;
        }
    }

if ( HaveProgressCancel )
    return;

if ( !backup_ok )
    return;

/*
-----
Purge
----- */

if ( !(BackupType & WITH_PURGE) && !(BackupType & PURGE_DOWNTIME) && !(BackupType & PURGE_ALARM_SUMMARY) )
    return;

sorc = StringTable.find( TEXT("PURGING...") );
sorc.set_text( ProgressWindow );

for ( i=0; i<NtoSave; i++ )
    {
    if ( HaveProgressCancel )
        break;

    s = SaveList+i;

    computer = s->name.computer;
    machine  = s->name.machine;

    if ( !machine.isempty() )
        {
        if ( waitfor_shotsave_semaphor(computer, machine) )
            {
            machine.set_text( ProgressWindow, SAVING_MACHINE_TEXT_BOX  );
            set_text( ProgressWindow, SAVING_PART_TEXT_BOX, EmptyString );
            set_text( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );

            if ( !HaveProgressCancel )
                {
                purge_downtime( computer, machine );

                s->part.rewind();
                while ( s->part.next() )
                    {
                    if ( HaveProgressCancel )
                        break;

                    part = s->part.text();

                    purge( computer, machine, part );

                    if ( BackupType & PURGE_ALARM_SUMMARY )
                        {
                        dest.get_alarmsum_dbname( computer, machine, part );
                        if ( dest.file_exists() )
                            dest.delete_file();
                        }
                    }
                }
            free_shotsave_semaphore();
            }
        }
    }
}
