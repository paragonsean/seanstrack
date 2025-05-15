#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\stringtable.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"
#include "extern.h"
#include "fill.h"
#include "save.h"

static MACHINE_NAME_LIST_CLASS BupMachList;
static STRING_CLASS AutoBackupDir;
void load_zip_file();
void st_resource_message_box( TCHAR * msg_id, TCHAR * title_id );

/***********************************************************************
*                             BACKUP_BITS                              *
*                                                                      *
*    If the return value is DO_RESTORE, no boxes were checked.         *
*                                                                      *
***********************************************************************/
static BITSETYPE backup_bits( HWND w )
{
int i;
int n;
BITSETYPE mask;

static int listbox_id[] = { TR_SETUPS_CHECKBOX,  TR_PROFILE_CHECKBOX,
                            TR_PARAM_CHECKBOX,   TR_SCRAP_DOWN_CHECKBOX,
                            TR_ALARM_CHECKBOX,   TR_SURETRAK_CHECKBOX,
                            TR_ANALOGS_CHECKBOX, TR_BOARDS_CHECKBOX };

static BITSETYPE bitmask[] = { SETUP_DATA,         PROFILE_DATA,
                               PARAMETER_DATA,     DOWNTIME_DATA,
                               ALARM_DATA,         SURETRAK_GLOBAL_DATA,
                               ANALOG_SENSOR_DATA, FASTRAK_BOARDS_DATA };


n = sizeof(listbox_id) / sizeof(int);

mask = DO_RESTORE;

for ( i=0; i<n; i++ )
    if ( is_checked(w, listbox_id[i]) )
        mask |= bitmask[i];

return mask;
}

/***********************************************************************
*                        CHECK_FOR_DEST_MACHINE                        *
***********************************************************************/
void check_for_dest_machine( HWND w )
{
LISTBOX_CLASS lb;

lb.init( w, TR_MACHINE_LISTBOX );
if ( lb.select_count() == 1 )
    {
    lb.init( w, TR_DEST_LISTBOX );
    if ( lb.selected_index() != NO_INDEX )
        {
        if ( MachList.find(lb.selected_text()) )
            {
            DestMachine.set( MachList.computer_name(), MachList.name() );
            return;
            }
        }
    }

DestMachine.empty();
}

/***********************************************************************
*                         SORC_MACHINE_CHANGE                          *
***********************************************************************/
void sorc_machine_change( HWND w )
{
LISTBOX_CLASS sorc;
LISTBOX_CLASS dest;
BOOL is_enabled;

sorc.init( w, TR_MACHINE_LISTBOX );
dest.init( w, TR_DEST_LISTBOX    );

if ( sorc.select_count() == 1 )
    {
    is_enabled = TRUE;
    }
else
    {
    dest.unselect_all();
    is_enabled = FALSE;
    }

EnableWindow( dest.handle(), is_enabled );
if ( is_enabled )
    dest.setcursel( sorc.selected_text() );
}

/***********************************************************************
*                         LOAD_AUTOBACKUP_DIR                          *
* Copy one of the autobackup restore points to the backup directory.   *
***********************************************************************/
static BOOLEAN load_autobackup_dir( HWND w )
{
NAME_CLASS d;
NAME_CLASS s;
STRING_CLASS subdir;
LISTBOX_CLASS lb;

d = BackupDir;
d.remove_ending_backslash();

/*
-----------------------------------
Remove the current backup directory
----------------------------------- */
if ( d.directory_exists() )
    d.delete_directory();

d.create_directory();

if ( d.directory_exists() )
    {
    s = AutoBackupDir;
    s.remove_ending_filename();
    lb.init( w, TR_AUTOBACKUPDIR_LISTBOX );
    subdir = lb.selected_text();
    if ( !subdir.isempty() )
        {
        lb.init( w, TR_MACHINE_LISTBOX );
        lb.empty();
        lb.init( w, TR_PART_LISTBOX );
        lb.empty();
        s.cat_path( subdir );
        hourglass_cursor();
        copy_all_files( d.text(), s.text() );
        restore_cursor();
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( HWND w )
{
static int title_ids[] = { TR_PARAMS_STATIC, TR_SHOTS_STATIC };
const int nof_title_ids = sizeof(title_ids)/sizeof(int);

set_listbox_tabs_from_title_positions( w, TR_PART_LISTBOX, title_ids, nof_title_ids );
}

/***********************************************************************
*                        TOGGLE_ALL_CHECKBOXES                         *
***********************************************************************/
static void toggle_all_checkboxes( HWND parent )
{
static int id[] = {
    TR_SETUPS_CHECKBOX,
    TR_PROFILE_CHECKBOX,
    TR_PARAM_CHECKBOX,
    TR_SCRAP_DOWN_CHECKBOX,
    TR_ALARM_CHECKBOX,
    TR_SURETRAK_CHECKBOX,
    TR_ANALOGS_CHECKBOX,
    TR_BOARDS_CHECKBOX
    };
const int nof_ids = sizeof(id)/sizeof(int);

int     i;
BOOLEAN need_check;
HWND    w[nof_ids];

need_check = FALSE;

for ( i=0; i<nof_ids; i++ )
    {
    w[i] = GetDlgItem( parent, id[i] );
    if ( !is_checked(w[i]) )
        need_check = TRUE;
    }

for ( i=0; i<nof_ids; i++ )
    {
    set_checkbox( w[i], need_check );
    }
}

/***********************************************************************
*                         FILL_BACKUP_LISTBOXES                        *
***********************************************************************/
static void fill_backup_listboxes( HWND w )
{
BupMachList.empty();
fill_startup( w, TR_MACHINE_LISTBOX, TR_PART_LISTBOX );

/*
-------------------------------------------------------------
Fill the destination listbox with a list of existing machines
------------------------------------------------------------- */
if ( BackupDir.directory_exists() )
    {
    BupMachList.add_computer( BackupComputer );
    fill_machines( BupMachList );
    fill_parts( BupMachList, DO_RESTORE );
    }
}

/***********************************************************************
*                            INIT_LISTBOXES                            *
***********************************************************************/
static void init_listboxes( HWND w )
{
AUTO_BACKUP_CLASS ab;
COMPUTER_CLASS    c;
LISTBOX_CLASS     lb;
NAME_CLASS        s;
TEXT_LIST_CLASS   t;

c.add( BackupComputer );
fill_backup_listboxes( w );
fill_machine_listbox( w, TR_DEST_LISTBOX, MachList );

/*
---------------------------
Fill the autobackup listbox
--------------------------- */
if ( ab.get_backup_dir() )
    {
    lb.init( w, TR_AUTOBACKUPDIR_LISTBOX );
    lb.empty();
    AutoBackupDir = ab.dir();
    s = AutoBackupDir;
    s.remove_ending_filename();
    s.remove_ending_backslash();
    t.get_directory_list( s.text() );
    t.rewind();
    while ( t.next() )
        lb.add( t.text() );
    }

sorc_machine_change( w );
}

/***********************************************************************
*                  SET_RESTORE_SETUP_STRINGS                    *
***********************************************************************/
static void set_restore_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { TR_RESTORE_STATIC,       TEXT("RESTORE") },
    { TR_MA_STATIC,            TEXT("MACHINE_STRING") },
    { TR_PA_STATIC,            TEXT("PART_STRING") },
    { TR_PARAMS_STATIC,        TEXT("PARAMS_STRING") },
    { TR_SHOTS_STATIC,         TEXT("SHOTS_STRING") },
    { IDOK,                    TEXT("BEGIN") },
    { TR_SETUPS_CHECKBOX,      TEXT("SETUPS") },
    { TR_SCRAP_DOWN_CHECKBOX,  TEXT("SCRAP_DOWN") },
    { TR_PROFILE_CHECKBOX,     TEXT("SHOT_PROS") },
    { TR_PARAM_CHECKBOX,       TEXT("SHOT_PARMS") },
    { TR_ALARM_CHECKBOX,       TEXT("ALARM_DATA") },
    { TR_SURETRAK_CHECKBOX,    TEXT("ST_GLOBALS") },
    { TR_RESTORE_ALL_BUTTON,   TEXT("ALL_STRING") },
    { TR_ALL_MACHINES_BUTTON,  TEXT("ALL_STRING") },
    { TR_LOAD_ZIP_FILE_BUTTON, TEXT("LOAD_ZIP") },
    { TR_DEST_MA_STATIC,       TEXT("DEST_MA") },
    { TR_ANALOGS_CHECKBOX,     TEXT("ASENSOR_LIST") },
    { TR_BOARDS_CHECKBOX,      TEXT("FT_BD_LIST") },
    { TR_AUTOBACKUPDIR_TBOX,   TEXT("AUTO_BUPS") },
    { TR_CLICK_AUTO_STATIC,    TEXT("CLICK_AUTO") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    TR_MACHINE_LISTBOX,
    TR_PART_LISTBOX,
    TR_DEST_LISTBOX,
    TR_AUTOBACKUPDIR_LISTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                             RESTORE_PROC                             *
***********************************************************************/
BOOL CALLBACK restore_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
BITSETYPE b;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        startup( w );
        set_restore_setup_strings( w );
        DestMachine.empty();
        return TRUE;

    case WM_DBINIT:
        BOOL is_enabled;
        BupMachList.empty();
        init_listboxes( w );
        if ( HaveSureTrakControl )
            is_enabled = TRUE;
        else
            is_enabled = FALSE;
        EnableWindow( GetDlgItem(w, TR_SURETRAK_CHECKBOX), is_enabled );
        return TRUE;

    case WM_DB_SAVE_DONE:
        /*
        ----------------------------------------------------------------
        Refill the destination listbox in case a new machine was created
        ---------------------------------------------------------------- */
        fill_machine_listbox( w, TR_DEST_LISTBOX, MachList );
        sorc_machine_change( w );
        return TRUE;

    case WM_DBCLOSE:
        SendDlgItemMessage( w, TR_MACHINE_LISTBOX, LB_RESETCONTENT, TRUE, 0L );
        SendDlgItemMessage( w, TR_PART_LISTBOX,    LB_RESETCONTENT, TRUE, 0L );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case TR_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    sorc_machine_change( w );
                    fill_parts( BupMachList, DO_RESTORE );
                    }
                return TRUE;

            case TR_AUTOBACKUPDIR_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    load_autobackup_dir( w );
                    fill_backup_listboxes( w );
                    sorc_machine_change( w );
                    }
                return TRUE;

            case TR_PART_LISTBOX:
                return TRUE;

            case TR_ALL_MACHINES_BUTTON:
                lb_select_all( w, TR_MACHINE_LISTBOX );
                sorc_machine_change( w );
                fill_parts( BupMachList, DO_RESTORE );
                return TRUE;

            case TR_LOAD_ZIP_FILE_BUTTON:
                load_zip_file();
                PostMessage( w, WM_DBINIT, 0, 0 );
                return TRUE;

            case TR_RESTORE_ALL_BUTTON:
                toggle_all_checkboxes( w );
                return TRUE;

            case IDOK:
                b = backup_bits( w );
                if ( b == DO_RESTORE )
                    {
                    st_resource_message_box( TEXT("NOTHING_TO_DO"), TEXT("NOTE_STRING") );
                    SetFocus( GetDlgItem(w, TR_SETUPS_CHECKBOX) );
                    }
                else
                    {
                    check_for_dest_machine( w );
                    save( b, w, TR_MACHINE_LISTBOX, TR_PART_LISTBOX );
                    }
                return TRUE;
            }
        break;
    }

return FALSE;
}
