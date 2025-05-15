#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\vdbclass.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"
#include "fill.h"
#include "extern.h"
#include "save.h"

static HWND DDWindow = 0;

static LISTBOX_CLASS MachBox;
static LISTBOX_CLASS PartBox;
static LISTBOX_CLASS ShotBox;

struct SHOT_LIST_DB
    {
    BOOLEAN      is_finished;
    STRING_CLASS shot_name;
    STRING_CLASS shot_date;
    STRING_CLASS shot_time;
    int          db_type;
    int          name_index;
    int          date_index;
    int          time_index;
    DB_TABLE     db;
    VDB_CLASS    vb;
    };

/*
-----------------------------------
These are indexes and also db_types
----------------------------------- */
static const int PROFILE_INDEX   = 0;
static const int PARAMETER_INDEX = 1;

void st_resource_message_box( TCHAR * msg_id, TCHAR * title_id );
/***********************************************************************
*                             BACKUP_BITS                              *
***********************************************************************/
static BITSETYPE backup_bits()
{
int       i;
int       n;
BITSETYPE mask;

static int listbox_id[] = { DD_PROFILES_CHECKBOX, DD_PARAMETERS_CHECKBOX };

static BITSETYPE bitmask[] = { PROFILE_DATA, PARAMETER_DATA };

n = sizeof(listbox_id) / sizeof(int);

mask = NO_BACKUP;

for ( i=0; i<n; i++ )
    {
    if ( is_checked(DDWindow, listbox_id[i]) )
        mask |= bitmask[i];
    }

mask |= DO_DELETE;

return mask;
}

/***********************************************************************
*                             NEXT__RECORD                             *
***********************************************************************/
static void next_record( SHOT_LIST_DB & s )
{
bool have_rec;

if ( s.is_finished )
    return;

have_rec = false;

if ( s.db_type == PROFILE_INDEX )
    {
    if ( s.db.get_next_record(NO_LOCK) )
        {
        s.db.get_alpha( s.shot_name, s.name_index );
        s.db.get_alpha( s.shot_date, s.date_index );
        s.db.get_alpha( s.shot_time, s.time_index );
        have_rec = true;
        }
    }
else
    {
    if ( s.vb.get_next_record() )
        {
        s.shot_name = s.vb.ra[s.name_index];
        s.shot_date = s.vb.ra[s.date_index];
        s.shot_time = s.vb.ra[s.time_index];
        have_rec = true;
        }
    }

if ( !have_rec )
    s.is_finished = TRUE;
}

/***********************************************************************
*                              SHOW_SHOTS                              *
***********************************************************************/
static void show_shots()
{
STRING_CLASS buf;
TCHAR        c;
STRING_CLASS computer;
STRING_CLASS machine;
STRING_CLASS part;
NAME_CLASS   s;
STRING_CLASS my_shot_name;
SHOT_LIST_DB t[2];
int          i;
int          j;
HCURSOR      old_cursor;

ShotBox.empty();

if ( MachBox.select_count() != 1 )
    return;

if ( PartBox.select_count() != 1 )
    return;

machine = MachBox.selected_text();
if ( !MachList.find(machine) )
    return;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

computer = MachList.computer_name();
part     = PartBox.selected_text();

t[PROFILE_INDEX].is_finished     = TRUE;
t[PROFILE_INDEX].name_index = GRAPHLST_SHOT_NAME_INDEX;
t[PROFILE_INDEX].date_index = GRAPHLST_DATE_INDEX;
t[PROFILE_INDEX].time_index = GRAPHLST_TIME_INDEX;
t[PROFILE_INDEX].db_type    = PROFILE_INDEX;

if ( is_checked(DDWindow, DD_PROFILES_CHECKBOX) )
    {
    s.get_graphlst_dbname( computer, machine, part );
    if ( t[PROFILE_INDEX].db.open(s, AUTO_RECLEN, PFL) )
        {
        t[PROFILE_INDEX].is_finished = FALSE;
        next_record( t[PROFILE_INDEX] );
        }
    }

t[PARAMETER_INDEX].is_finished = TRUE;
t[PARAMETER_INDEX].name_index  = SHOTPARM_SHOT_NAME_INDEX;
t[PARAMETER_INDEX].date_index  = SHOTPARM_DATE_INDEX;
t[PARAMETER_INDEX].time_index  = SHOTPARM_TIME_INDEX;
t[PARAMETER_INDEX].db_type     = PARAMETER_INDEX;

if ( is_checked(DDWindow, DD_PARAMETERS_CHECKBOX) )
    {
    s.get_shotparm_csvname( computer, machine, part );
    if ( t[PARAMETER_INDEX].vb.open_for_read(s) )
        {
        t[PARAMETER_INDEX].is_finished = FALSE;
        next_record( t[PARAMETER_INDEX] );
        }
    }

while ( !(t[PROFILE_INDEX].is_finished && t[PARAMETER_INDEX].is_finished) )
    {
    if ( t[PROFILE_INDEX].is_finished )
        {
        i = PARAMETER_INDEX;
        }
    else if ( t[PARAMETER_INDEX].is_finished )
        {
        i = PROFILE_INDEX;
        }
    else if ( t[PROFILE_INDEX].shot_name <= t[PARAMETER_INDEX].shot_name )
        {
        i = PROFILE_INDEX;
        }
    else
        {
        i = PARAMETER_INDEX;
        }

    /*
    ------------------------------------------
    I is the primary index, set j to the other
    ------------------------------------------ */
    j = i ^ 1;

    my_shot_name = t[i].shot_name;
    buf = my_shot_name;
    buf += TabChar;
    buf.cat_w_char( t[i].shot_date, SpaceChar );
    buf.cat_w_char( t[i].shot_time, TabChar );

    /*
    -----------------------------------------
    Note: get_shotbox_list uses these offsets
    ----------------------------------------- */
    c = SpaceChar;
    if ( !t[PARAMETER_INDEX].is_finished && (t[PARAMETER_INDEX].shot_name == my_shot_name) )
        c= XChar;
    buf += c;
    buf += SpaceChar;

    c = SpaceChar;
    if ( !t[PROFILE_INDEX].is_finished && (t[PROFILE_INDEX].shot_name == my_shot_name) )
        c = XChar;
    buf += c;

    ShotBox.add( buf.text() );

    next_record( t[i] );
    if ( !t[j].is_finished && (t[j].shot_name == my_shot_name) )
        next_record( t[j] );
    }

t[PROFILE_INDEX].db.close();
t[PARAMETER_INDEX].vb.close();

SetCursor( old_cursor );
}

/***********************************************************************
*                        SET_SHOT_LISTBOX_TABS                         *
***********************************************************************/
static void set_shot_listbox_tabs()
{
static int title_ids[] = { DD_DATE_STATIC   , DD_PROFILE_STATIC };
const int  nof_title_ids = sizeof(title_ids)/sizeof(int);

set_listbox_tabs_from_title_positions( DDWindow, DD_SHOT_LISTBOX, title_ids, nof_title_ids );
}

/***********************************************************************
*                  SET_DELETE_DATA_SETUP_STRINGS                    *
***********************************************************************/
static void set_delete_data_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { DD_DATA_TYPES_STATIC,   TEXT("DATA_TYPES") },
    { DD_MA_STATIC,           TEXT("MACHINE_STRING") },
    { DD_PA_STATIC,           TEXT("PART_STRING") },
    { DD_PARAMETERS_CHECKBOX, TEXT("PARAMETERS") },
    { DD_PROFILES_CHECKBOX,   TEXT("PROFILES") },
    { IDOK,                   TEXT("BEGIN") },
    { DD_ALL_SHOTS_BUTTON,    TEXT("ALL_STRING") },
    { DD_SHOT_STATIC,         TEXT("SHOT_STRING") },
    { DD_DATE_STATIC,         TEXT("DATE_STRING") },
    { DD_PARAM_STATIC,        TEXT("PARAM") },
    { DD_PROFILE_STATIC,      TEXT("PROFILE") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    DD_MACHINE_LISTBOX,
    DD_PART_LISTBOX,
    DD_SHOT_LISTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL  );
}

/***********************************************************************
*                           DELETE_DATA_PROC                           *
***********************************************************************/
BOOL CALLBACK delete_data_proc( HWND w, UINT msg, WPARAM wparam, LPARAM lparam )
{
int id;
int cmd;

id  = LOWORD( wparam );
cmd = HIWORD( wparam );

switch (msg)
    {
    case WM_INITDIALOG:
        DDWindow = w;
        MachBox.init( w, DD_MACHINE_LISTBOX );
        PartBox.init( w, DD_PART_LISTBOX );
        ShotBox.init( w, DD_SHOT_LISTBOX );
        set_shot_listbox_tabs();
        set_delete_data_setup_strings( w );
        return TRUE;

    case WM_DBINIT:
        fill_startup( w, DD_MACHINE_LISTBOX, DD_PART_LISTBOX );
        fill_machines( MachList );
        return TRUE;

    case WM_DB_SAVE_DONE:
        show_shots();
        return TRUE;

    case WM_DBCLOSE:
        MachBox.empty();
        PartBox.empty();
        ShotBox.empty();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save( backup_bits(), w, DD_MACHINE_LISTBOX, DD_PART_LISTBOX, DD_SHOT_LISTBOX );
                return TRUE;

            case DD_ALL_SHOTS_BUTTON:
                ShotBox.select_all();
                return TRUE;

            case DD_PARAMETERS_CHECKBOX:
            case DD_PROFILES_CHECKBOX:
                show_shots();
                return TRUE;

            case DD_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    fill_parts( MachList, DO_DELETE );
                    show_shots();
                    }
                return TRUE;

            case DD_PART_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    show_shots();
                    }
                return TRUE;

            }
        break;
    }

return FALSE;
}
