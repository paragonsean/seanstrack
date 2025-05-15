#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"
#include "extern.h"
#include "fill.h"
#include "save.h"

static       TCHAR BupresIniFile[] = TEXT( "bupres.ini" );
static       TCHAR ConfigSection[] = TEXT( "Config" );
static       TCHAR LastNKey[]      = TEXT( "LastN" );

void st_resource_message_box( TCHAR * msg_id, TCHAR * title_id );

/***********************************************************************
*                             BACKUP_BITS                              *
***********************************************************************/
BITSETYPE backup_bits( HWND w )
{
int i;
int n;
BITSETYPE mask;
static int listbox_id[] = { TB_SETUPS_CHECKBOX, TB_PROFILE_CHECKBOX,
                            TB_PARAM_CHECKBOX, TB_SCRAP_DOWN_CHECKBOX,
                            TB_PURGE_CHECKBOX, TB_LAST_50_CHECKBOX,
                            TB_PURGE_DOWNTIME_CHECKBOX, TB_ALARM_CHECKBOX,
                            TB_PURGE_ALARMS_CHECKBOX };

static BITSETYPE bitmask[] = { SETUP_DATA, PROFILE_DATA,
                               PARAMETER_DATA, DOWNTIME_DATA,
                               WITH_PURGE,     SAVE_LAST_50,
                               PURGE_DOWNTIME, ALARM_DATA,
                               PURGE_ALARM_SUMMARY };

n = sizeof(listbox_id) / sizeof(int);

mask = DO_BACKUP;

for ( i=0; i<n; i++ )
    if ( is_checked(w, listbox_id[i]) )
        mask |= bitmask[i];

return mask;
}

/***********************************************************************
*                            CHECK_ZIP_NAME                            *
***********************************************************************/
static BOOLEAN check_zip_name( HWND w )
{
TCHAR * cp;
TCHAR   zipname[MAX_PATH+1];
int32   slen;
bool    need_to_update_zipname;

need_to_update_zipname = false;

if ( !get_text(zipname, w, MAX_PATH) )
    {
    st_resource_message_box( TEXT("NEED_ZIP_NAME"), TEXT("CANNOT_COMPLY_STRING") );
    return FALSE;
    }

/*
-----------------------------------------------------------------------
Don't let them use the temp backup dir as I will then delete their file
----------------------------------------------------------------------- */
slen = BackupDir.len();
if ( slen > 0 )
    {
    if ( strings_are_equal(BackupDir.text(), zipname, slen) )
        {
        st_resource_message_box( TEXT("NO_SAVE_TO_BUP_DIR"), TEXT("CANNOT_COMPLY_STRING") );
        return FALSE;
        }
    }

/*
-------------------------------
Insert a backslash if necessary
------------------------------- */
if ( zipname[0] != BackSlashChar && zipname[1] == ColonChar && zipname[2] != BackSlashChar )
    {
    insert_char( zipname+2, BackSlashChar );
    need_to_update_zipname = true;
    }

cp = findchar( PeriodChar, zipname );
if ( !cp )
    {
    catstring( zipname, ZIP_SUFFIX );
    need_to_update_zipname = true;
    }

if ( need_to_update_zipname )
    set_text( w, zipname );

return TRUE;
}

/***********************************************************************
*                         GET_PROFILES_TO_COPY                         *
***********************************************************************/
int32 get_profiles_to_copy( HWND w )
{
int32 i;
BOOL  status;

i = 0;
if ( is_checked(w, TB_RECENT_XBOX) )
    {
    i = (int32) GetDlgItemInt(w, TB_RECENT_COUNT_EBOX, &status, TRUE );
    if ( status )
        {
        if ( i < 1 )
            i = 0;
        }
    else
        {
        i = 0;
        }
    }

return i;
}

/***********************************************************************
*                        CHECK_ZIP_FILE_ENABLE                         *
***********************************************************************/
static void check_zip_file_enable( HWND w )
{
static int control[] = { TB_RECENT_XBOX, TB_RECENT_COUNT_EBOX, TB_ZIP_NAME_EDITBOX };
const int nof_controls = sizeof(control)/sizeof(int);

static int id[] = { TB_SETUPS_CHECKBOX, TB_SCRAP_DOWN_CHECKBOX, TB_PROFILE_CHECKBOX, TB_PARAM_CHECKBOX, TB_ALARM_CHECKBOX };
const int nof_ids = sizeof(id)/sizeof(int);

int i;
BOOLEAN need_zip_file;
BOOL    is_enabled;

need_zip_file = FALSE;

for ( i=0; i<nof_ids; i++ )
    {
    if ( is_checked(w, id[i]) )
        {
        need_zip_file = TRUE;
        break;
        }
    }

is_enabled = IsWindowEnabled( GetDlgItem(w, TB_ZIP_NAME_EDITBOX) );
if ( need_zip_file && !is_enabled )
    is_enabled = TRUE;
else if ( !need_zip_file && is_enabled )
    is_enabled = FALSE;
else
    return;

for ( i=0; i<nof_controls; i++ )
    EnableWindow( GetDlgItem(w, control[i]), is_enabled );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( HWND w )
{
static int title_ids[] = { TB_PARAMS_STATIC, TB_SHOTS_STATIC };
const int  nof_title_ids = sizeof(title_ids)/sizeof(int);

int32      i;
NAME_CLASS s;
INI_CLASS  ini;

set_listbox_tabs_from_title_positions( w, TB_PART_LISTBOX, title_ids, nof_title_ids );

s.get_exe_dir_file_name( BupresIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
i = ini.get_int( LastNKey );
if ( i > 0 )
    {
    set_checkbox( w, TB_RECENT_XBOX, TRUE );
    set_text( w, TB_RECENT_COUNT_EBOX, int32toasc(i) );
    }
}

/***********************************************************************
*                        TOGGLE_ALL_CHECKBOXES                         *
***********************************************************************/
static void toggle_all_checkboxes( HWND parent )
{
static int id[] = {
    TB_SETUPS_CHECKBOX,
    TB_PROFILE_CHECKBOX,
    TB_PARAM_CHECKBOX,
    TB_SCRAP_DOWN_CHECKBOX,
    TB_ALARM_CHECKBOX
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
*                     ENABLE_NEXT_SHOT_NUMBER_EBOX                     *
***********************************************************************/
static void enable_next_shot_number_ebox( HWND w )
{
BOOL is_enabled;
HWND purge_xbox;
HWND last_50_xbox;
HWND next_shot_number_static;
HWND next_shot_number_ebox;

purge_xbox   = GetDlgItem( w, TB_PURGE_CHECKBOX );
last_50_xbox = GetDlgItem( w, TB_LAST_50_CHECKBOX );
next_shot_number_static = GetDlgItem( w, TB_NEXT_SHOT_NUMBER_STATIC );
next_shot_number_ebox   = GetDlgItem( w, TB_NEXT_SHOT_NUMBER_EBOX   );

is_enabled = FALSE;
if ( is_checked(purge_xbox) && !is_checked(last_50_xbox) )
    is_enabled = TRUE;
EnableWindow( next_shot_number_static, is_enabled );
EnableWindow( next_shot_number_ebox, is_enabled );
}

/***********************************************************************
*                     SET_BACKUP_SETUP_STRINGS                         *
***********************************************************************/
static void set_backup_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { TB_BACKUP_STATIC,           TEXT("BACKUP") },
    { TB_SETUPS_CHECKBOX,         TEXT("SETUPS") },
    { TB_SCRAP_DOWN_CHECKBOX,     TEXT("SCRAP_DOWN") },
    { TB_PROFILE_CHECKBOX,        TEXT("SHOT_PROS") },
    { TB_PARAM_CHECKBOX,          TEXT("SHOT_PARMS") },
    { TB_ALARM_CHECKBOX,          TEXT("ALARM_DATA") },
    { TB_BACKUP_ALL_BUTTON,       TEXT("ALL_STRING") },
    { TB_ZIP_FILE_STATIC,         TEXT("BUP_ZIP_FILE") },
    { TB_MA_STATIC,               TEXT("MACHINE_STRING") },
    { TB_PA_STATIC,               TEXT("PART_STRING") },
    { TB_PARAMS_STATIC,           TEXT("PARAMS_STRING") },
    { TB_SHOTS_STATIC,            TEXT("SHOTS_STRING") },
    { TB_AND_PARAMS_STATIC,       TEXT("AND_PARMS") },
    { TB_SHOTS_2_STATIC,          TEXT("SHOTS_STRING") },
    { TB_PURGE_OPT_STATIC,        TEXT("PURGE_OPT") },
    { TB_PURGE_CHECKBOX,          TEXT("PURGE_DATA") },
    { TB_LAST_50_CHECKBOX,        TEXT("LAST50") },
    { TB_PURGE_DOWNTIME_CHECKBOX, TEXT("PURGE_DOWN") },
    { TB_PURGE_ALARMS_CHECKBOX,   TEXT("PURGE_ALARMS") },
    { TB_ALL_MACHINES_BUTTON,     TEXT("ALL_STRING") },
    { TB_RECENT_XBOX,             TEXT("MOST_RECENT") },
    { TB_ZIP_FILE_2_STATIC,       TEXT("EX_ZIP_PATH") },
    { TB_NEXT_SHOT_NUMBER_STATIC, TEXT("NEXT_SHOT") },
    { IDOK,                       TEXT("BEGIN") },
    { TB_ALL_TEXT_FILES_PB,       TEXT("DEBUG_BUP") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    TB_MACHINE_LISTBOX,
    TB_PART_LISTBOX,
    TB_RECENT_COUNT_EBOX,
    TB_ZIP_NAME_EDITBOX,
    TB_NEXT_SHOT_NUMBER_EBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, TEXT("") );
}

/***********************************************************************
*                              BACKUP_PROC                             *
***********************************************************************/
BOOL CALLBACK backup_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;
BITSETYPE b;
NAME_CLASS zipname;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        startup( w );
        set_backup_setup_strings( w );
        return TRUE;

    case WM_DBINIT:
        fill_startup( w, TB_MACHINE_LISTBOX, TB_PART_LISTBOX );
        fill_machines( MachList );
        fill_parts( MachList, DO_BACKUP );
        check_zip_file_enable( w );
        enable_next_shot_number_ebox( w );
        w = GetDlgItem( w, TB_SETUPS_CHECKBOX );
        SetFocus(w);
        return TRUE;

    case WM_DB_SAVE_DONE:
        return TRUE;

    case WM_DBCLOSE:
        SendDlgItemMessage( w, TB_MACHINE_LISTBOX, LB_RESETCONTENT, TRUE, 0L );
        SendDlgItemMessage( w, TB_PART_LISTBOX,    LB_RESETCONTENT, TRUE, 0L );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case TB_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    fill_parts( MachList, DO_BACKUP );
                return TRUE;

            case TB_PART_LISTBOX:
                return TRUE;

            case TB_LAST_50_CHECKBOX:
            case TB_PURGE_CHECKBOX:
                enable_next_shot_number_ebox( w );
                return TRUE;

            case TB_ALL_MACHINES_BUTTON:
                lb_select_all( w, TB_MACHINE_LISTBOX );

                fill_parts( MachList, DO_BACKUP );
                return TRUE;

            case TB_BACKUP_ALL_BUTTON:
                toggle_all_checkboxes( w );
                check_zip_file_enable( w );
                return TRUE;

            case TB_ALL_TEXT_FILES_PB:
                if ( check_zip_name(GetDlgItem(w, TB_ZIP_NAME_EDITBOX)) )
                    {
                    if ( zipname.get_text(w, TB_ZIP_NAME_EDITBOX) )
                        save_all_text_files( zipname.text()  );
                    }
                return TRUE;

            case TB_SETUPS_CHECKBOX:
            case TB_SCRAP_DOWN_CHECKBOX:
            case TB_PROFILE_CHECKBOX:
            case TB_PARAM_CHECKBOX:
            case TB_ALARM_CHECKBOX:
                if ( cmd == BN_CLICKED )
                    {
                    check_zip_file_enable( w );
                    return TRUE;
                    }
                break;

            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;

            case IDOK:
                if ( check_zip_name(GetDlgItem(w, TB_ZIP_NAME_EDITBOX)) )
                    {
                    ProfilesToCopy = get_profiles_to_copy( w );
                    int32_to_ini( BupresIniFile, ConfigSection, LastNKey, ProfilesToCopy );

                    NextShotNumber = 0;
                    if ( zipname.get_text(w, TB_ZIP_NAME_EDITBOX) )
                        {
                        b = backup_bits(w);
                        if ( b == DO_BACKUP )
                            {
                            st_resource_message_box( TEXT("NOTHING_TO_DO"), TEXT("NOTE") );
                            SetFocus( GetDlgItem(w, TB_SETUPS_CHECKBOX) );
                            }
                        else
                            {
                            if ( IsWindowEnabled(GetDlgItem(w, TB_NEXT_SHOT_NUMBER_EBOX)) )
                                NextShotNumber = GetDlgItemInt( w, TB_NEXT_SHOT_NUMBER_EBOX, 0, FALSE );
                            save( zipname, b, w, TB_MACHINE_LISTBOX, TB_PART_LISTBOX );
                            NextShotNumber = 0;
                            }
                        }
                    else
                        return FALSE;
                    }
                else
                    {
                    SetFocus( GetDlgItem(w, TB_ZIP_NAME_EDITBOX) );
                    }
                fill_parts( MachList, DO_BACKUP );
                return TRUE;

            }
        break;
    }


return FALSE;
}

/***********************************************************************
*                         FILL_BACKUP_LISTBOX                          *
***********************************************************************/
static void fill_backup_listbox( HWND w )
{
LISTBOX_CLASS lb;
FILE_CLASS    f;
NAME_CLASS    s;
TCHAR       * cp;

lb.init( w, BACKUP_LIST_LISTBOX );
lb.empty();

s.get_backup_list_csvname();
if ( !s.file_exists() )
    return;

if ( f.open_for_read(s.text()) )
    {
    while ( true )
        {
        s = f.readline();
        if ( s.countchars(CommaChar) == 3  )
            {
            /*
            -------------------------------------------------
            Replace all the commas except the first with tabs
            changing this to a two field record.
            ------------------------------------------------- */
            cp = s.find( CommaChar );
            cp++;
            while ( *cp != NullChar )
                {
                if ( *cp == CommaChar )
                    *cp = TabChar;
                cp++;
                }
            /*
            ---------------------------------------
            Skip the first field, the computer name
            --------------------------------------- */
            s.next_field();
            s.next_field();
            lb.add( s.text() );
            }
        else
            {
            break;
            }
        }
    f.close();
    }
}

/***********************************************************************
*                    SET_BACKUP_LIST_SETUP_STRINGS                     *
***********************************************************************/
static void set_backup_list_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,                  TEXT("BEGIN") },
    { BL_ZIP_FILE_STATIC,    TEXT("BUP_ZIP_FILE") },
    { BL_SHOTS_STATIC,       TEXT("FOLLOWING_SHOTS") },
    { BL_MA_STATIC,          TEXT("MACHINE_STRING") },
    { BACKUP_LIST_PART_TBOX, TEXT("PART_STRING") },
    { BACKUP_LIST_SHOT_TBOX, TEXT("SHOT_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    TB_ZIP_NAME_EDITBOX,
    BACKUP_LIST_LISTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                            BACKUP_LIST_PROC                          *
***********************************************************************/
BOOL CALLBACK backup_list_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int title_ids[] = {
    BACKUP_LIST_PART_TBOX,
    BACKUP_LIST_SHOT_TBOX
    };
const int nof_title_ids = sizeof(title_ids)/sizeof(int);

static int id;
static TCHAR zipname[MAX_PATH+1];
static BITSETYPE b = DO_BACKUP | SETUP_DATA | PROFILE_DATA;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_listbox_tabs_from_title_positions( w, BACKUP_LIST_LISTBOX, title_ids, nof_title_ids );
        set_backup_list_setup_strings( w );
        return TRUE;

    case WM_DBINIT:
        fill_backup_listbox(w);
        SetFocus( GetDlgItem(w, TB_ZIP_NAME_EDITBOX) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                return TRUE;

            case IDOK:
                if ( check_zip_name(GetDlgItem(w, TB_ZIP_NAME_EDITBOX)) )
                    {
                    if ( get_text(zipname, w, TB_ZIP_NAME_EDITBOX, MAX_PATH+1) )
                        {
                        save_backup_list( zipname, b );
                        }
                    }
                return TRUE;
            }
        break;
    }

return FALSE;
}
