#include <windows.h>
#include <process.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"
#include "extern.h"
#include "fill.h"
#include "save.h"

static TCHAR BupresIniFile[] = TEXT( "bupres.ini" );
static TCHAR ConfigSection[] = TEXT( "Config" );
static TCHAR LastNKey[]      = TEXT( "LastN" );

static AUTO_BACKUP_CLASS AutoBack;
static LISTBOX_CLASS     AutoBackLb;  /* Used by copy_all_thread */
static bool              CopyAllThreadIsRunning = false;

void st_resource_message_box( TCHAR * msg_id, TCHAR * title_id );

/***********************************************************************
*                      FILL_DEFAULT_RESTORE_NAME                       *
***********************************************************************/
static void fill_default_restore_name( HWND w )
{
STRING_CLASS s;
NAME_CLASS   path;
TIME_CLASS   t;
TCHAR      * cp;
int          i;

t.get_local_time();
s = t.yyyy();
s += MinusChar;
s += t.mm();
s += MinusChar;
s += t.dd();
i = 0;
while ( true )
    {
    path = AutoBack.dir();
    path.remove_ending_filename();
    path.cat_path( s );
    if ( !path.directory_exists() )
        break;
    i++;
    cp = s.find( UnderScoreChar );
    if ( cp )
        *cp = NullChar;
    s += UnderScoreChar;
    s += i;
    }

s.set_text( w, AB_RESTORE_NAME_EBOX );
}

/***********************************************************************
*                          COPY_ALL_THREAD                             *
***********************************************************************/
void copy_all_thread( void * notused )
{
COMPUTER_CLASS  c;
NAME_CLASS      s;
STRING_CLASS    part;
TEXT_LIST_CLASS t;
MACHINE_NAME_LIST_CLASS m;

s.get_data_dir_file_name( c.whoami(), 0 );

AutoBackLb.empty();
AutoBackLb.add( s.text() );

AutoBack.backup_data_dir();
AutoBack.backup_exe_dir();

/*
-----------------------------------------
Get the list of machines at this computer
----------------------------------------- */
m.add_computer( c.whoami() );
m.rewind();
while ( m.next() )
    {
    StringTable.find( s, TEXT("MA_INTRO") );
    s += m.name();
    AutoBackLb.add( s.text() );
    AutoBack.backup_machine( m.name() );
    /*
    -------------------------
    Copy the part directories
    ------------------------- */
    get_partlist( t, c.whoami(), m.name() );
    t.rewind();
    while ( t.next() )
        {
        part = t.text();
        StringTable.find( s, TEXT("PA_INTRO") );
        s += part;
        AutoBackLb.add( s.text() );
        AutoBack.backup_part( m.name(), part );
        }
    t.empty();
    }

AutoBackLb.add( StringTable.find( TEXT("FINISHED") ) );
AutoBackLb.scroll_to_end();
CopyAllThreadIsRunning = false;
}

/***********************************************************************
*                             AB_COPY_ALL                              *
***********************************************************************/
static BOOLEAN ab_copy_all( HWND w )
{
if ( CopyAllThreadIsRunning )
    return FALSE;

if ( !AutoBack.exists() )
    return FALSE;

AutoBackLb.init( w, AB_HELP_LB );

_beginthread( copy_all_thread, 0, NULL );

return TRUE;
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( HWND w )
{
AutoBack.get_backup_dir();
AutoBack.dir().set_text( w, AB_AUTOBACKUPDIR_EBOX );
fill_default_restore_name( w );
}

/***********************************************************************
*                          SAVE_AUTOBACKUPDIR                          *
***********************************************************************/
static void save_autobackupdir( HWND w, int id )
{
AUTO_BACKUP_CLASS ab;
BOOLEAN    b;
INI_CLASS  ini;
NAME_CLASS backupdir;
NAME_CLASS s;
LISTBOX_CLASS lb;

lb.init( w, AB_HELP_LB );
lb.empty();

backupdir.get_text( w, AB_AUTOBACKUPDIR_EBOX );
s = backupdir;
s.remove_ending_backslash();
if ( s.countchars(BackSlashChar) < 1 )
    {
    st_resource_message_box( TEXT("NO_ROOT"), TEXT("CANNOT_COMPLY_STRING") );
    return;
    }
s.remove_ending_filename();

if ( id == AB_CREATE_PB )
    {
    /*
    ------------------------------------------------------------------
    I am creating a new restore point. Use the restore point directory
    instead of the current auto backup directory.
    ------------------------------------------------------------------ */
    backupdir = s;
    s.get_text( w, AB_RESTORE_NAME_EBOX );
    backupdir.cat_path( s );
    }

lb.add( backupdir.text() );

b = TRUE;
if ( !backupdir.directory_exists() )
    {
    lb.add( StringTable.find(TEXT("CREATING_DIR")) );
    b = backupdir.create_directory();

    if ( b )
        lb.add( StringTable.find( TEXT("FINISHED")) );
    else
        lb.add( StringTable.find( TEXT("DIR_CREATE_FAIL")) );
    }

if ( b )
    {
    lb.add( StringTable.find(TEXT("COPYING_SETUPS")) );
    AutoBack.set_backup_dir( backupdir );
    ab_copy_all( w );
    if ( id == AB_SAVE_PB )
        AutoBack.put_backup_dir();
    else
        fill_default_restore_name( w );
    }
}

/***********************************************************************
*                  SET_AUTO_BACKUP_SETUP_STRINGS                    *
***********************************************************************/
static void set_auto_backup_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { AB_SAVE_PB,             TEXT("SAVE") },
    { AB_CREATE_NEW_STATIC,   TEXT("NEW_RES_PT") },
    { AB_CREATE_PB,           TEXT("CREATE") },
    { AB_DIR_STATIC,          TEXT("AB_DIR") },
    { AB_PRESS_SAVE_STATIC,   TEXT("PRESS_SAVE") },
    { AB_PRESS_CREATE_STATIC, TEXT("PRESS_CREATE") },
    { AB_DEF_PATH_STATIC,     TEXT("DEF_PATH") },
    { AB_EXAMPLE_STATIC,      TEXT("EXAMPLE:") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    AB_RESTORE_NAME_EBOX,
    AB_AUTOBACKUPDIR_EBOX,
    AB_HELP_LB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, TEXT("") );
}

/***********************************************************************
*                           AUTO_BACKUP_PROC                           *
***********************************************************************/
BOOL CALLBACK auto_backup_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;
NAME_CLASS zipname;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        startup( w );
        set_auto_backup_setup_strings( w );
        return TRUE;

    case WM_DBINIT:
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;

            case AB_CREATE_PB:
            case AB_SAVE_PB:
                save_autobackupdir( w, id );
                return TRUE;

            case IDOK:
                return TRUE;

            }
        break;
    }

return FALSE;
}
