#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\alarmsum.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\genlist.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\param.h"
#include "..\include\plookup.h"
#include "..\include\shotname.h"
#include "..\include\structs.h"
#include "..\include\vdbclass.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\shotclas.h"

#include "resource.h"
#include "backupcl.h"
#include "extern.h"

extern TCHAR *    BackupTimeIniName[];

static const int MAX_KEY_NAME_LEN = 40;
static const int32 ZIP_SUFFIX_LEN = 4;
static const DWORD ThirtyMinutesInMs = 1800000L;

static TCHAR BackupDirString[]        = TEXT( "BackupDir" );

static TCHAR CurrentPartName[]        = TEXT( "CurrentPartName" );
static TCHAR LastDownTime[]           = TEXT( "LastDownTime" );

static TCHAR LastExaminedShotName[] = TEXT( "LastExaminedShotName" );
static TCHAR LastExaminedShotTime[] = TEXT( "LastExaminedShotTime" );

static TCHAR ZipCount[]               = TEXT( "ZipCount" );
static TCHAR PartZipSuffix[]          = TEXT( ".PAZ" );
static TCHAR MachineZipSuffix[]       = TEXT( ".MAZ" );
static TCHAR DefaultPartFileName[]    = TEXT( "{m}_{p}_{nb}_{ne}" );
static TCHAR DefaultMachineFileName[] = TEXT( "{m}_{mm}_{dd}_{hh}_{mi}" );
static TCHAR DefaultBackupDirectoryName[] = TEXT( "v5backup" );

/*
-----------------------------------------------------
These tokens are used only for the part zip file name
----------------------------------------------------- */
static TCHAR FirstShotToken[]         = TEXT( "{nb}" );
static TCHAR LastShotToken[]          = TEXT( "{ne}" );

const TCHAR ColonSpaceString[]      = TEXT( ": " );

static const int StripPathNames = 1;

extern LISTBOX_CLASS StatusListBox;

/*
-------------------------------------------------------------------
If the global ParamLinesPerfile is greater than 0 I keep the number
of lines that I have written here .
------------------------------------------------------------------- */
static int ParamLinesWritten = 0;

/*
------------------------------------------------------------------
This is just a limit for the number of rows in the status list box
------------------------------------------------------------------ */
static int MaxStatusCount    = 200;

/*
------------------------------------------------------------------------
This is only used to show an error message if I can't save the part data
------------------------------------------------------------------------ */
static STRING_CLASS PartSaveBuf;

int32 MaxZipCount = 0;

void zip_callback( const char * name, int object_ratio, int job_ratio );

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                           RESOURCE_MESSAGE_BOX                       *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                        COPY_MACHINE_DIRECTORY                        *
* This copies a machine directory without copying any parts. All files *
* are copied except the semaphor.txt file, which is simply created.    *
* You should have a lock on the sorc semaphor file before doing this.  *
***********************************************************************/
bool copy_machine_directory( STRING_CLASS dco, STRING_CLASS dma, STRING_CLASS sco, STRING_CLASS sma )
{
NAME_CLASS      d;
NAME_CLASS      s;
FILE_CLASS      f;
TEXT_LIST_CLASS t;

/*
-------------------------------------
Make sure the source directory exists
------------------------------------- */
s.get_machine_directory( sco, sma );
if ( !s.directory_exists() )
    return false;

/*
----------------------------------------
Make sure the dest data directory exists
---------------------------------------- */
d.get_data_dir_file_name( dco, 0 );
if ( !d.directory_exists() )
    return false;

/*
-----------------------------------------
Create the machine directory if necessary
----------------------------------------- */
d.get_machine_directory( dco, dma );
if ( !d.directory_exists() )
    {
    if ( !d.create_directory() )
        return false;
    }

/*
--------------------
Make a list of files
-------------------- */
t.get_file_list( s.text() );
t.rewind();
while ( t.next() )
    {
    if ( !strings_are_equal( s.text(), SEMAPHOR_DB) )
        {
        s.get_machine_dir_file_name( sco, sma, s.text() );
        d.get_machine_dir_file_name( dco, dma, s.text() );
        s.copyto( d );
        }
    }

d.get_semaphor_dbname( dco, dma );
if ( !d.file_exists() )
    {
    f.open_for_write( d.text() );
    f.close();
    }

return true;
}

/***********************************************************************
*                           SAVE_TO_LOG_FILE                           *
***********************************************************************/
static void save_to_log_file( HWND w )
{
STRING_CLASS filename;
FILE_CLASS f;

filename.get_text( w, LOG_FILE_EBOX );

if ( f.open_for_write(filename.text()) )
    {
    f.writeline( PartSaveBuf.text() );
    f.writeline( BackupDirectory.text() );
    if ( IsWindowVisible(GetDlgItem(w,NO_PKZIP_TBOX)) )
        f.writeline( TEXT("pkzip not found") );
    f.close();
    }
}

/***********************************************************************
*                           INIT_PART_SAVE_DIALOG                      *
***********************************************************************/
static void init_part_save_dialog( HWND w )
{
static TCHAR defaultname[] = TEXT( "LOGFILE.TXT" );
NAME_CLASS s;

PartSaveBuf.set_text( w, PSERROR_CMD_TBOX );
BackupDirectory.set_text( w, PSERROR_BACKUP_PATH_TBOX );

s.get_exe_dir_file_name( TEXT("pkzip25.exe") );
if ( !s.file_exists() )
   ShowWindow( GetDlgItem(w,NO_PKZIP_TBOX), SW_SHOW );

s.get_exe_dir_file_name( defaultname );
s.set_text( w, LOG_FILE_EBOX );
}

/***********************************************************************
*                       PART_SAVE_ERROR_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK part_save_error_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int           id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        init_part_save_dialog( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_TO_LOG_PB:
                save_to_log_file( hWnd );
                return TRUE;

            case IDOK:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        MACHINE_DATA_ZIP_FILE_NAME                    *
*                           M01_12_31_20_45.MAZ                        *
***********************************************************************/
static NAME_CLASS & machine_data_zip_file_name( STRING_CLASS & sorc, PART_NAME_ENTRY & p )
{
static NAME_CLASS zipfile;

COMPUTER_CLASS c;
NAME_CLASS     s;
DB_TABLE       t;
TIME_CLASS     tc;
int32          n;
BOOLEAN        have_time;

if ( findchar( LeftCurlyBracket, sorc.text()) )
    {
    have_time = FALSE;
    s.get_downtime_dbname( c.whoami(), p.machine );

    if ( s.file_exists() )
        {
        if ( t.open(s, DOWNTIME_RECLEN, PFL) )
            {
            n = t.nof_recs();
            if ( n > 0 )
                {
                n--;
                if ( t.goto_record_number(n) )
                    if ( t.get_current_record(NO_LOCK) )
                        if ( get_time_class_from_db(tc, t, DOWNTIME_DATE_OFFSET) )
                            have_time = TRUE;
                }
            t.close();
            }
        }

    if ( !have_time )
        tc.get_local_time();
    zipfile = dynamic_file_name( tc, p, sorc.text() );
    }
else
    {
    zipfile = sorc;
    }

zipfile += MachineZipSuffix;

return zipfile;
}

/***********************************************************************
*                         SHOT_DATA_ZIP_FILE_NAME                      *
*                         M01_VISITRAK_1_1000.PAZ                      *
***********************************************************************/
static NAME_CLASS & shot_data_zip_file_name( STRING_CLASS & sorc, PART_NAME_ENTRY & p )
{
static NAME_CLASS zipfile;

COMPUTER_CLASS c;
NAME_CLASS   s;
DB_TABLE     t;
int32        n;
FIELDOFFSET  fo;
SHOT_CLASS   firstshot;
SHOT_CLASS   lastshot;
TIME_CLASS   tc;
BOOLEAN      have_time;

have_time = FALSE;

s.get_graphlst_dbname( c.whoami(), p.machine, p.part );
if ( s.file_exists() )
    {
    if ( t.open(s, AUTO_RECLEN, PFL) )
        {
        fo = t.field_offset( GRAPHLST_DATE_INDEX );
        if ( t.get_next_record(NO_LOCK) )
            {
            firstshot.get_from_graphlst( t );
            lastshot = firstshot;
            n = t.nof_recs() - 1;
            if ( n > 0 )
                {
                if ( t.goto_record_number(n) )
                    if ( t.get_current_record(NO_LOCK) )
                        {
                        lastshot.get_from_graphlst( t );
                        if ( get_time_class_from_db(tc, t, fo) )
                            have_time = TRUE;
                        }
                }
            }
        t.close();
        }
    }

s = sorc;

/*
------------------------------------------------------------------------------
I need to replace the shot tokens with the strings, as the dynamic_file_name
routine doesn't know how to do this. {nb} is the first shot, {ne} is the last.
------------------------------------------------------------------------------ */
s.replace( FirstShotToken, firstshot.name.text() );
s.replace( LastShotToken,  lastshot.name.text()  );

/*
-------------------------------------------------------------
If there are any curly brackets left then there is more to do
------------------------------------------------------------- */
if ( findchar( LeftCurlyBracket, s.text()) )
    {
    if ( !have_time )
        tc.get_local_time();
    s = dynamic_file_name(tc, p, s.text() );
    }

zipfile = s;
zipfile += PartZipSuffix;

return zipfile;
}

/***********************************************************************
*                         CHECK_FOR_LOCAL_PART                         *
* Make sure I have a machine and part to copy to. If I create one I    *
* need to assure the plookup file is set to 0.                         *
***********************************************************************/
static BOOLEAN check_for_local_part( PART_NAME_ENTRY & sorc )
{
COMPUTER_CLASS c;
BOOLEAN        status;
NAME_CLASS     s;

status = machine_exists( c.whoami(), sorc.machine );
if ( !status )
    status = update_machine( c.whoami(), sorc.machine, sorc.computer, sorc.machine );

if ( status )
    {
    status = part_exists( c.whoami(), sorc.machine, sorc.part );
    if ( !status )
        status = update_part( c.whoami(), sorc.machine, sorc.part, sorc.computer, sorc.machine, sorc.part );
    }

return status;
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
***********************************************************************/
DS_BACKUP_ENTRY::DS_BACKUP_ENTRY()
{
zip_count                 = 0;
}

/***********************************************************************
*                            ~DS_BACKUP_ENTRY                          *
***********************************************************************/
DS_BACKUP_ENTRY::~DS_BACKUP_ENTRY()
{
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                             START_NEW_PART                           *
***********************************************************************/
void DS_BACKUP_ENTRY::start_new_part( TCHAR * newpart )
{
NAME_CLASS s;
DB_TABLE   t;
int32      n;

name.part = newpart;

zip_count = 0;

last_examined_shot.clear();

s.get_graphlst_dbname( name.computer, name.machine, name.part );
if ( !s.file_exists() )
    return;

/*
-----------------------------------------------
I want to start saving at the next to last shot
----------------------------------------------- */
if ( t.open(s, AUTO_RECLEN, PFL) )
    {
    n = t.nof_recs();
    if ( n > 10 )
        {
        n -= 2;
        if ( t.goto_record_number(n) )
            {
            if ( t.get_current_record(NO_LOCK) )
                {
                last_examined_shot.get_from_graphlst( t );
                }
            }
        }
    t.close();
    }
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                                   =                                  *
***********************************************************************/
void DS_BACKUP_ENTRY::operator=( const DS_BACKUP_ENTRY & sorc )
{
name                    = sorc.name;
last_downtime           = sorc.last_downtime;
last_examined_shot      = sorc.last_examined_shot;
zip_count               = sorc.zip_count;
part_zip_file_path      = sorc.part_zip_file_path;
machine_zip_file_path   = sorc.machine_zip_file_path;
}

/***********************************************************************
*                          GET_ZIP_FILE_PATH                           *
* The section is either the machine name or "Default".                 *
* the path type is PART_PATH (0) or MACHINE_PATH (1).                  *
***********************************************************************/
void get_zip_file_path( NAME_CLASS & dest, const TCHAR * section, int path_type )
{
const TCHAR * key;
const TCHAR * default_file_name;
COMPUTER_CLASS c;
INI_CLASS      ini;
STRING_CLASS   s;

ini.set_file( DsBackupIniPath.text() );
ini.set_section( section );

if ( path_type == PART_PATH )
    {
    key = BackupPartPathString;
    default_file_name = DefaultPartFileName;
    }
else
    {
    key = BackupMachinePathString;
    default_file_name = DefaultMachineFileName;
    }

if ( ini.find(key) )
    {
    dest = ini.get_string();
    return;
    }

/*
-------------------------------------------------------------
See if this is an old file that hasn't been converted before,
in which case there will be a backup directory string.
------------------------------------------------------------- */
if ( ini.find(BackupDirString) )
    {
    dest = ini.get_string();
    /*
    ---------------------------------------------------------
    This is the first time this part has been loaded. Replace
    the BackupDirString entry with two new equivalent entries
    --------------------------------------------------------- */
    dest.cat_path( DefaultPartFileName );
    ini.put_string( BackupPartPathString, dest.text() );

    dest = ini.get_string( BackupDirString );
    dest.cat_path( DefaultMachineFileName );
    ini.put_string( BackupMachinePathString, dest.text() );

    /*
    ----------------------
    Make the return string
    ---------------------- */
    dest = ini.get_string( BackupDirString );
    dest.cat_path( default_file_name );

    /*
    ---------------------------------------------------------
    Delete the entry for the BackupDirString for this machine
    --------------------------------------------------------- */
    ini.remove_key( BackupDirString );
    }
else
    {
    /*
    --------------------
    Get the default name
    -------------------- */
    ini.set_section( DefaultSection );
    dest = ini.get_string( key );
    if ( ini.find(key) )
        {
        dest = ini.get_string();
        return;
        }

    /*
    -----------------------
    Get the old default dir
    ----------------------- */
    if ( ini.find(BackupDirString) )
        {
        dest = ini.get_string();
        }
    else
        {
        dest = c.whoami();
        dest.cat_path( DefaultBackupDirectoryName );
        }

    /*
    ------------------------------------------------------------------------------
    If I've gotten here then there are no default strings and I need to make some.
    ------------------------------------------------------------------------------ */
    s = dest;
    s.cat_path( DefaultPartFileName );
    ini.put_string( BackupPartPathString, s.text() );

    s = dest;
    s.cat_path( DefaultMachineFileName );
    ini.put_string( BackupMachinePathString, s.text() );

    /*
    -------------------------------
    The old key is no longer needed
    ------------------------------- */
    ini.remove_key( BackupDirString );

    dest.cat_path( default_file_name );
    }
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                             READ_ZIP_PATHS                           *
***********************************************************************/
BOOLEAN DS_BACKUP_ENTRY::read_zip_paths()
{
get_zip_file_path( part_zip_file_path,    name.machine.text(), PART_PATH    );
get_zip_file_path( machine_zip_file_path, name.machine.text(), MACHINE_PATH );
return TRUE;
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                                 READ                                 *
***********************************************************************/
BOOLEAN DS_BACKUP_ENTRY::read()
{
INI_CLASS ini;

ini.set_file( DsBackupIniPath.text() );
ini.set_section( name.machine.text() );

if ( !ini.find(CurrentPartName) )
    {
    /*
    ---------------------------------------------
    Just fill the zip file names with the default
    --------------------------------------------- */
    get_zip_file_path( part_zip_file_path,    DefaultSection, PART_PATH    );
    get_zip_file_path( machine_zip_file_path, DefaultSection, MACHINE_PATH );
    return FALSE;
    }

name.part = ini.get_string();
last_downtime = ini.get_string( LastDownTime );
last_examined_shot.name = ini.get_string( LastExaminedShotName );
last_examined_shot.time = ini.get_string( LastExaminedShotTime );

zip_count = ini.get_int( ZipCount );

read_zip_paths();

return TRUE;
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                             READ_PART_CONFIG                         *
***********************************************************************/
BOOLEAN DS_BACKUP_ENTRY::read_part_config()
{
COMPUTER_CLASS c;
INI_CLASS      ini;
NAME_CLASS     s;

s.get_part_dir_file_name( c.whoami(), name.machine, name.part, DsBackupIniFile );

ini.set_file( s.text() );
ini.set_section( ConfigKey );

if ( !ini.find(CurrentPartName) )
    {
    /*
    ---------------------------------------------
    Just fill the zip file names with the default
    --------------------------------------------- */
    get_zip_file_path( part_zip_file_path,    DefaultSection, PART_PATH    );
    get_zip_file_path( machine_zip_file_path, DefaultSection, MACHINE_PATH );
    return FALSE;
    }

last_downtime           = ini.get_string( LastDownTime         );
last_examined_shot.name = ini.get_string( LastExaminedShotName );
last_examined_shot.time = ini.get_string( LastExaminedShotTime );

zip_count = ini.get_int( ZipCount );

read_zip_paths();

return TRUE;
}


/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                             WRITE_ZIP_PATHS                          *
***********************************************************************/
void DS_BACKUP_ENTRY::write_zip_paths()
{
put_ini_string( DsBackupIniFile, name.machine.text(), BackupPartPathString,    part_zip_file_path.text()    );
put_ini_string( DsBackupIniFile, name.machine.text(), BackupMachinePathString, machine_zip_file_path.text() );
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                                 WRITE                                *
***********************************************************************/
void DS_BACKUP_ENTRY::write()
{
INI_CLASS ini;

ini.set_file( DsBackupIniPath.text() );
ini.set_section( name.machine.text() );

ini.put_string( CurrentPartName, name.part.text()     );
ini.put_string( LastDownTime,    last_downtime.text() );

ini.put_string( BackupPartPathString,    part_zip_file_path.text()    );
ini.put_string( BackupMachinePathString, machine_zip_file_path.text() );

ini.put_string( LastExaminedShotName, last_examined_shot.name.text() );
ini.put_string( LastExaminedShotTime, last_examined_shot.time.text() );

ini.put_int( ZipCount, zip_count );
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                            WRITE_PART_CONFIG                         *
***********************************************************************/
void DS_BACKUP_ENTRY::write_part_config()
{
COMPUTER_CLASS c;
INI_CLASS      ini;
NAME_CLASS     s;

s.get_part_dir_file_name( c.whoami(), name.machine, name.part, DsBackupIniFile );

ini.set_file( s.text() );
ini.set_section( ConfigKey );

ini.put_string( LastDownTime,    last_downtime.text() );

ini.put_string( BackupPartPathString,    part_zip_file_path.text()    );
ini.put_string( BackupMachinePathString, machine_zip_file_path.text() );

ini.put_string( LastExaminedShotName, last_examined_shot.name.text() );
ini.put_string( LastExaminedShotTime, last_examined_shot.time.text() );

ini.put_int( ZipCount, zip_count );
}

/***********************************************************************
*                        EMPTY_BACKUP_DIRECTORY                        *
***********************************************************************/
void empty_backup_directory()
{
if ( BackupDirectory.directory_exists() )
    {
    BackupDirectory.empty_directory();
    }
else
    {
    if ( !BackupDirectory.create_directory() )
        resource_message_box( DIR_CREATE_FAIL, CANT_DO_THAT_STRING );
    }
}

/***********************************************************************
*                     COPY_SETUP_FILES_FOR_ONE_PART                    *
***********************************************************************/
BOOLEAN DS_BACKUP_ENTRY::copy_setup_files_for_one_part()
{
COMPUTER_CLASS c;
NAME_CLASS     d;
NAME_CLASS     s;

/*
-------------------------------------------------------
If the remote computer is online, update the local copy
------------------------------------------------------- */
d.get_part_directory( name.computer, name.machine, name.part );
if ( !d.directory_exists() )
    return FALSE;

update_machine( c.whoami(), name.machine,            name.computer, name.machine            );
update_part(    c.whoami(), name.machine, name.part, name.computer, name.machine, name.part );

/*
-----------------
Copy master trace
----------------- */
s.get_master_ii_name( name.computer, name.machine, name.part );
d.get_master_ii_name( c.whoami(), name.machine, name.part );

s.copyto( d );

return TRUE;
}

/***********************************************************************
*                        CHANGE_TO_MIRROR_PATH                         *
* A Mirror path assumes that I an going to replace my root path        *
* like C: or \\backup-computer with E: or \\new-backup-computer, etc.  *
* I will replace ether the X: or \\xxxxxx with the new string. The     *
* MirrorRoot should be X: or \\xxxxxxxx without a trailing backslash.  *
***********************************************************************/
static BOOLEAN change_to_mirror_path( NAME_CLASS & dest, NAME_CLASS & sorc )
{
TCHAR * cp;
int     slen;

if ( !HaveMirror )
    return FALSE;

slen = sorc.len();
if ( slen > 2 )
    {
    slen += MirrorRoot.len();
    dest.upsize( slen );

    dest = MirrorRoot;
    cp = sorc.text() + 2;
    cp = findchar( BackSlashChar, cp );
    if ( cp )
        {
        dest += cp;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                     ENSURE_ZIP_FILE_DIRECTORIES                      *
* I am passed the name of the zip file. I extract the directory from   *
* it and make sure the directory exists and also the mirror directory  *
* if there is one.                                                     *
***********************************************************************/
bool ensure_zip_file_directories( NAME_CLASS s )
{
NAME_CLASS d;
NAME_CLASS m;

d = s;
if ( !dir_from_filename(d.text()) )
    return false;

if ( !d.directory_exists() )
    d.create_directory();

if ( change_to_mirror_path(m, d) )
    {
    if ( !m.directory_exists() )
        m.create_directory();
    }

return true;
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                              ZIP_SHOT_DATA                           *
*                                                                      *
* I'm going to zip relative to the machine directory. All the machine  *
* files, all the part files, and all the results files.                *
*                                                                      *
* Before doing this I'm going to create a text file with the name of   *
* the machine and the name of the part so I can read, but not restore, *
* this later.                                                          *
*                                                                      *
* I don't use the backup directory any more.                           *
***********************************************************************/
void DS_BACKUP_ENTRY::zip_shot_data()
{
COMPUTER_CLASS c;
BOOLEAN        status;
FILE_CLASS     f;
NAME_CLASS     buf;
NAME_CLASS     filename;
NAME_CLASS     s;
DB_TABLE       db;

if ( name.part.isempty() )
    return;

if ( part_zip_file_path.isempty() )
    return;

s.get_graphlst_dbname( c.whoami(), name.machine, name.part );

if ( !s.file_exists() )
    return;

if ( s.file_size() < 1 )
    return;

copy_setup_files_for_one_part();

/*
---------------------------------------
Make the full path name of the zip file
--------------------------------------- */
if ( Using_Machine_Based_Zip_Paths )
    filename = shot_data_zip_file_name( part_zip_file_path, name );
else
    filename = shot_data_zip_file_name( DefaultPartZipPath, name );

/*
----------------------------------------
Make sure the zip file directories exist
---------------------------------------- */
if ( !ensure_zip_file_directories(filename) )
    return;

/*
------------------------------------------------------------
Create a text file with the name of the machine and the part
------------------------------------------------------------ */
s.get_machine_dir_file_name( c.whoami(), name.machine, MachinePartFile );
if ( f.open_for_write(s.text()) )
    {
    f.writeline( name.machine.text() );
    f.writeline( name.part.text()    );
    f.close();
    }
else
    {
    return;
    }

/*
-------------------------------------------------------------------------------------------------------------
Pkzip25 -add -noz -path=current "e:\v5backup\shots1_10.zip" *.* "Visitrak Part\*.*" "Visitrak Part\results\*.*"
------------------------------------------------------------------------------------------------------------- */
buf.get_exe_dir_file_name( TEXT("pkzip25 -add -noz -path=current \"") );
buf += filename;
buf += TEXT( "\" *.* \"" );
buf += name.part;
buf += TEXT( "\\*.*\" \"" );
buf += name.part;
buf += TEXT ( "\\results\\*.*\"" );

/*
------------------------------------------------------------------------
The machine directory will be the current directory of the pkzip process
------------------------------------------------------------------------ */
s.get_machine_directory( c.whoami(), name.machine );
status = execute_and_wait( buf.text(), s.text() );
if ( status )
    {
    if ( change_to_mirror_path(s, filename) )
        filename.copyto( s );
    }
else
    {
    PartSaveBuf = buf;
    DialogBox(
        MainInstance,
        "PART_SAVE_ERROR_DIALOG",
        MainDialogWindow,
        (DLGPROC) part_save_error_dialog_proc );
    PartSaveBuf = 0;
    }

/*
-------------------------------------
Delete the machine and part name file
------------------------------------- */
s.get_machine_dir_file_name( c.whoami(), name.machine, MachinePartFile );
s.delete_file();

/*
------------------------
Delete all the shot data
------------------------ */
s.get_part_results_directory( c.whoami(), name.machine, name.part );
s.empty_directory();

s.get_alarmsum_dbname( c.whoami(), name.machine, name.part );
db.create( s);

s.get_graphlst_dbname( c.whoami(), name.machine, name.part );
db.create( s);

s.get_shotparm_csvname( c.whoami(), name.machine, name.part );
db.create( s);

/*
--------------------------------------------
Zero the count of the number of files zipped
-------------------------------------------- */
zip_count = 0;
}

/***********************************************************************
*                            DS_BACKUP_ENTRY                           *
*                            ZIP_MACHINE_DATA                          *
***********************************************************************/
void DS_BACKUP_ENTRY::zip_machine_data()
{
COMPUTER_CLASS c;
BOOLEAN        status;
NAME_CLASS     buf;
NAME_CLASS     filename;
NAME_CLASS     s;
DB_TABLE       db;

if ( SkipDowntimeZip )
    return;

s.get_downtime_dbname( c.whoami(), name.machine );
if ( !s.file_exists() )
    return;

if ( s.file_size() < 1 )
    return;

copy_setup_files_for_one_part();

/*
------------------------------------------------------------------------------------------------
Make the zip file name and make sure the directory and, if necessary, the mirror directory exist
------------------------------------------------------------------------------------------------ */
if ( Using_Machine_Based_Zip_Paths )
    filename = machine_data_zip_file_name( machine_zip_file_path, name );
else
    filename = machine_data_zip_file_name( DefaultMachineZipPath, name );

if ( !ensure_zip_file_directories(filename) )
    return;

/*
---------------------------------------------------------------------------
Pkzip25 -add -noz -path=current "e:\v5backup\shots1_10.zip" *.* results\*.*
--------------------------------------------------------------------------- */
buf.get_exe_dir_file_name( TEXT("pkzip25 -add -noz -path=current \"") );
buf += filename;
buf += TEXT( "\" *.* results\\*.*" );

/*
------------------------------------------------------------------------
The machine directory will be the current directory of the pkzip process
------------------------------------------------------------------------ */
s.get_machine_directory( c.whoami(), name.machine );

status = execute_and_wait( buf.text(), s.text() );
if ( status )
    {
    if ( change_to_mirror_path(s, filename) )
        filename.copyto( s );
    }
else
    {
    resource_message_box( MainInstance, CANT_DO_THAT_STRING, MAZ_ZIP_ERROR_STRING );
    }

/*
------------------------
Empty the downtime table
------------------------ */
s.get_downtime_dbname( c.whoami(), name.machine );
if ( s.file_exists() )
    s.delete_file();
db.create( s );
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                               ZIP_ALL_DATA                           *
***********************************************************************/
void DS_BACKUP_ENTRY::zip_all_data()
{
if ( name.part.isempty() || name.part == NO_PART )
    return;

zip_shot_data();
zip_machine_data();
}

/***********************************************************************
*                        SAVE_NETWORK_SHOTPARM                         *
***********************************************************************/
static void save_network_shotparm( PART_NAME_ENTRY & pn, VDB_CLASS & v )
{
static TCHAR TmpExtension[] = TEXT( ".tmp" );
TCHAR        buf[2048];
TCHAR      * pattern;
TCHAR      * p;
TCHAR      * dirp;
TCHAR      * dp;
TCHAR      * filename;
NAME_CLASS   d;
NAME_CLASS   extension;
NAME_CLASS   s;
TIME_CLASS   t;
FILE_CLASS   f;
PARAMETER_CLASS param;
int          i;

GetPrivateProfileString( ShotparmSection, RecordFormatKey, unknown_string(), buf, 512, DsBackupIniPath.text() );
if ( unknown(buf) )
    return;

pattern = maketext( buf );

/*
------------------
Make the file name
------------------ */
GetPrivateProfileString( ShotparmSection, FileNameKey, unknown_string(), buf, 512, DsBackupIniPath.text() );
if ( unknown(buf) )
    {
    delete[] pattern;
    pattern = 0;
    return;
    }

/*
----------------------------------------------
Extract the shot time from the shotparm record
---------------------------------------------- */
get_time_class_from_vdb( t, v, SHOTPARM_DATE_INDEX );

if ( findchar( LeftCurlyBracket, buf)  )
    copystring( buf, dynamic_file_name(t, pn, buf) );

filename = maketext( buf );

/*
------------------------
Construct the new record
------------------------ */
p = pattern;
dp = buf;
while ( replace_char_with_null(p, LeftCurlyBracket) )
    {
    dp = copystring( dp, p );
    p = nextstring( p );
    if ( replace_char_with_null(p, RightCurlyBracket) )
        {
        if (strings_are_equal(p, TEXT("dd"), 2) )
            {
            dp = copystring( dp, t.dd() );
            }
        else if (strings_are_equal(p, TEXT("mmm"), 3) )
            {
            dp = copystring( dp, t.mmm() );
            }
        else if (strings_are_equal(p, TEXT("mm"), 2) )
            {
            dp = copystring( dp, t.mm() );
            }
        else if (strings_are_equal(p, TEXT("yyyy"), 4) )
            {
            dp = copystring( dp, t.yyyy() );
            }
        else if (strings_are_equal(p, TEXT("yy"), 2) )
            {
            dp = copystring( dp, t.yy() );
            }
        else if (strings_are_equal(p, TEXT("hh"), 2) || strings_are_equal(p, TEXT("mi"), 2) || strings_are_equal(p, TEXT("ss"), 2) )
            {
            TCHAR * hhmmss();
            if (strings_are_equal(p, TEXT("hh"), 2) )
                i = 0;
            else if ( strings_are_equal(p, TEXT("mi"), 2) )
                i = 3;
            else if ( strings_are_equal(p, TEXT("ss"), 2) )
                i = 6;
            copychars( dp, t.hhmmss()+i, 2 );
            dp += 2;
            *dp = NullChar;
            }
        else if (strings_are_equal(p, TEXT("c"), 1) )
            {
            dp = copystring( dp, pn.computer.text() );
            }
        else if (strings_are_equal(p, TEXT("g"), 1) )
            {
            if ( param.find(pn.computer, pn.machine, pn.part) )
                {
                i = param.shot_goodness(v);
                if ( i == ALARM_SHOT )
                    dp = copystring( dp, HaveAlarmShot );
                else if ( i == WARNING_SHOT )
                    dp = copystring( dp, HaveWarningShot );
                else
                    dp = copystring( dp, HaveGoodShot );
                }
            }
        else if (strings_are_equal(p, TEXT("m"), 1) )
            {
            dp = copystring( dp, pn.machine.text() );
            }
        else if (strings_are_equal(p, TEXT("p"), 1) )
            {
            dp = copystring( dp, pn.part.text() );
            }
        else if (strings_are_equal(p, TEXT("n"), 1) )
            {
            dp = copystring( dp, v.ra[SHOTPARM_SHOT_NAME_INDEX].text() );
            }
        else if ( is_numeric(*p) )
            {
            i = asctoint32( p );
            /*
            ----------------------------------------
            Parameter numbers are for humans, [1,60]
            so I need to subtract 1 for the index.
            ---------------------------------------- */
            if ( i > 0 )
                i--;
            i += SHOTPARM_PARAM_1_INDEX;
            dp = copystring( dp, v.ra[i].text() );
            }

        p = nextstring( p );
        }
    }

copystring( dp, p );

/*
-----------------------
Create dir if necessary
----------------------- */
dirp = maketext( filename );
if ( dir_from_filename(dirp) )
    create_directory(dirp);
delete[] dirp;

s = filename;
if ( ParamLinesPerFile > 0 )
    {
    extension.get_extension( filename );
    s.replace( extension.text(), TmpExtension );
    }

/*
------------------------
Create file if necessary
------------------------ */
if ( f.open_for_append(s) )
    {
    f.writeline( buf );
    f.close();
    if ( ParamLinesPerFile > 0 )
        {
        /*
        ----------------------------------------------------------------------------------
        If I've written ParamLinesPerFile then I want to rename the file to the real name2
        ---------------------------------------------------------------------------------- */
        ParamLinesWritten++;
        if ( ParamLinesWritten >= ParamLinesPerFile )
            {
            ParamLinesWritten = 0;
            d = filename;
            /*
            -------------------------------------------------------------------
            Remove the extension from filename so I can build names dynamically
            ------------------------------------------------------------------- */
            dp = findstring( extension.text(), filename );
            if ( dp )
                *dp = NullChar;

            i = 0;
            while ( d.file_exists() )
                {
                d = filename;
                i++;
                d += i;
                d += extension;
                }
            s.moveto(d);
            }
        }
    }

delete[] filename;
delete[] pattern;
}

/***********************************************************************
*                            COPY_PROFILES                             *
***********************************************************************/
void DS_BACKUP_ENTRY::copy_profiles( SHOT_CLASS & current_shot )
{
SHOT_CLASS      record_shot;
COMPUTER_CLASS  c;
PART_NAME_ENTRY dest;
SYSTEMTIME      st;
DB_TABLE        s;
DB_TABLE        d;
NAME_CLASS      tn;
FIELDOFFSET     dif;
FIELDOFFSET     fo;
short           dest_record_length;
short           dest_shot_name_length;
short           save_flags;

dest = name;
dest.computer = c.whoami();

tn.get_graphlst_dbname( name.computer, name.machine, name.part );
if ( !tn.file_exists() )
    return;

if ( !s.open(tn, AUTO_RECLEN, PFL) )
    return;

dest_shot_name_length = shot_name_length( name.computer, name.machine, name.part );
dest_record_length = GRAPHLST_RECLEN + 2 + dest_shot_name_length - SHOT_LEN;
dif = dest_record_length - GRAPHLST_RECLEN;
tn.get_graphlst_dbname( dest.computer, dest.machine, dest.part );
if ( !tn.file_exists() )
    d.create( tn );

if ( d.open(tn, dest_record_length, WL) )
    {
    s.put_alpha( GRAPHLST_SHOT_NAME_INDEX,  last_examined_shot.name );
    if ( s.goto_first_greater_than_record(1) )
        {
        s.get_current_record( NO_LOCK );
        while ( TRUE )
            {
            record_shot.get_from_graphlst(s);
            if ( record_shot.name > current_shot.name )
                break;

            copy_profile( dest, name, record_shot.name );
            zip_count++;

            fo = s.field_offset( GRAPHLST_SAVE_FLAGS_INDEX );
            save_flags = (BITSETYPE) s.get_short( fo );

            /*
            ----------------------------------------------------------------------------------
            Because the record lengths may be different I need to copy the fields individually
            ---------------------------------------------------------------------------------- */
            d.put_alpha( GRAPHLST_SHOT_NUMBER_OFFSET, record_shot.name, dest_shot_name_length );
            st = record_shot.time.system_time();
            fo = (FIELDOFFSET) GRAPHLST_DATE_OFFSET + dif;
            d.put_date( fo, st );
            fo = (FIELDOFFSET) GRAPHLST_TIME_OFFSET + dif;
            d.put_time( fo, st );
            fo = (FIELDOFFSET) GRAPHLST_SAVE_FLAGS_OFFSET + dif;
            d.put_short( fo, (short) save_flags, GRAPHLST_SAVE_FLAGS_LEN );
            d.rec_append();

            if ( !s.get_next_record(FALSE) )
                break;
            }
        }

    d.close();
    }

s.close();
}

/***********************************************************************
*                           COPY_PARAMETERS                            *
***********************************************************************/
void DS_BACKUP_ENTRY::copy_parameters( SHOT_CLASS & current_shot )
{
SHOT_CLASS      record_shot;
COMPUTER_CLASS  c;
PART_NAME_ENTRY dest;
VDB_CLASS       d;
VDB_CLASS       s;
SYSTEMTIME      st;
NAME_CLASS      tn;
STRING_CLASS    sc;
TIME_CLASS      t;
FILE_CLASS      f;

dest = name;
dest.computer = c.whoami();
if ( LogFileEnable )
    {
    tn = LogFileDir;
    tn.cat_path( ParameterLogFile );
    if ( f.open_for_append(tn) )
        {
        t.get_local_time();
        sc = t.text();
        sc += CommaChar;
        sc += name.computer;
        sc += CommaChar;
        sc += name.machine;
        sc += CommaChar;
        sc += name.part;
        sc += CommaChar;
        sc += current_shot.name;
        f.writeline( sc );
        f.close();
        }
    }
/*
---------------------
Open the source table
--------------------- */
tn.get_shotparm_csvname( name.computer, name.machine, name.part );
if ( !tn.file_exists() )
    return;

if ( !s.open_for_read(tn) )
    return;

tn.get_shotparm_csvname( dest.computer, dest.machine, dest.part );
if ( !tn.file_exists() )
    d.create( tn );

if ( d.open_for_rw(tn) )
    {
    s.sa[SHOTPARM_SHOT_NAME_INDEX] =  last_examined_shot.name;

    if ( s.get_first_greater_than_key_match(1) )
        {
        while ( TRUE )
            {
            record_shot.get_from_shotparm( s );
            if ( record_shot.name > current_shot.name )
                break;
            save_network_shotparm( name, s );
            d.copy_rec( s );
            d.ra[SHOTPARM_SHOT_NAME_INDEX] = record_shot.name;
            st = record_shot.time.system_time();
            d.put_date( SHOTPARM_DATE_INDEX, st );
            d.put_time( SHOTPARM_TIME_INDEX, st );
            d.rec_append();
            if ( LogFileEnable )
                {
                tn = LogFileDir;
                tn.cat_path( ParameterLogFile );
                if ( f.open_for_append(tn) )
                    {
                    t.get_local_time();
                    sc = t.text();
                    d.ra.rewind();
                    while ( d.ra.next() )
                        {
                        sc += CommaChar;
                        sc += d.ra;
                        }
                    f.writeline( sc );
                    f.close();
                    }
                }

            if ( !s.get_next_record() )
                break;
            }
        }

    d.close();
    }

s.close();
}

/***********************************************************************
*                             COPY_ALARMS                              *
***********************************************************************/
void DS_BACKUP_ENTRY::copy_alarms( SHOT_CLASS & shot )
{
COMPUTER_CLASS      c;
PART_NAME_ENTRY     dest;
ALARM_SUMMARY_CLASS a;
short               i;
int32               shift;
SYSTEMTIME          shift_date;

get_shift_number_and_date( &i, &shift_date, shot.time.system_time() );
shift = (int32) i;

a.set_part( name );
if ( a.get_counts(shift_date, shift) )
    {
    a.set_part( c.whoami(), name.machine, name.part );
    a.save( shift_date, shift, TRUE );
    }
}

/***********************************************************************
*                         SHOW_STATUS_MESSAGE                          *
***********************************************************************/
static void show_status_message( STRING_CLASS & s )
{
LISTBOX_CLASS lb;
lb.init( MainDialogWindow, DS_STATUS_LB );
if ( lb.count() > MaxStatusCount )
    {
    lb.remove(1);
    }

lb.add( s.text() );
lb.scroll_to_end();
}

/***********************************************************************
*                           SHOW_SEMAPHORE_ERROR                       *
***********************************************************************/
static void show_semaphore_error( PART_NAME_ENTRY & name )
{
STRING_CLASS s;

s = name.machine;
s += SpaceChar;
s += name.part;
s += SpaceChar;
s += resource_string( CANT_GET_SEMAPHORE_STRING );

show_status_message( s );
}

/***********************************************************************
*                             SHOW_RECEIVED_SHOT                       *
***********************************************************************/
static void show_received_shot( PART_NAME_ENTRY & name, SHOT_CLASS & shot )
{
STRING_CLASS s;
s = name.machine;
s += SpaceChar;
s += name.part;
s += SpaceChar;
s += resource_string( SHOT_LABEL_STRING );
s += ColonSpaceString;
s += shot.name;

show_status_message( s );
}

/***********************************************************************
*                                SHOW_NO_SHOT                          *
***********************************************************************/
static void show_no_shot( PART_NAME_ENTRY & name  )
{
STRING_CLASS s;
s = name.machine;
s += SpaceChar;
s += name.part;
s += SpaceChar;
s += resource_string( NOT_FOUND_STRING );

show_status_message( s );
}

/***********************************************************************
*                         SHOW_ZERO_ZIPPED_SHOTS                       *
***********************************************************************/
static void show_zero_zipped_shots( PART_NAME_ENTRY & name )
{
STRING_CLASS s;
LISTBOX_CLASS lb;

s = name.machine;
s += TabChar;
s += name.part;
s += TabChar;
s += ZeroChar;

lb.init( MainDialogWindow, ZIP_COUNT_LISTBOX );
if ( lb.findprefix(name.machine.text()) )
    lb.replace( s.text() );
else
    lb.add( s.text() );
}

/***********************************************************************
*                            SHOW_ZIPPED_SHOT                          *
***********************************************************************/
static void show_zipped_shot( PART_NAME_ENTRY & name, SHOT_CLASS & shot, int32 zip_count )
{
STRING_CLASS s;
LISTBOX_CLASS lb;

s = name.machine;
s += TabChar;
s += name.part;
s += TabChar;
s += int32toasc( zip_count );
s += TabChar;
s += shot.time.text();

lb.init( MainDialogWindow, ZIP_COUNT_LISTBOX );
if ( lb.findprefix(name.machine.text()) )
    lb.replace( s.text() );
else
    lb.add( s.text() );
}

/***********************************************************************
*                     SHOT_NAME_LENGTH_HAS_CHANGED                     *
* This checks to see if the shot name length in shot_name.csv          *
* has changed. It also checks to see if my shot_name.csv file is       *
* missing. If it is I create a default one for {s6}                    *
***********************************************************************/
static bool shot_name_has_changed( PART_NAME_ENTRY & pn )
{
COMPUTER_CLASS c;
SHOT_NAME_CLASS s;
NAME_CLASS dest;
NAME_CLASS sorc;
bool       has_changed;
int my_length;
int new_length;

has_changed = false;

sorc.get_shot_name_csvname( pn.computer, pn.machine, pn.part );
dest.get_shot_name_csvname( c.whoami(),  pn.machine, pn.part );
if ( !dest.file_exists() )
    {
    if ( sorc.file_exists() )
        {
        sorc.copyto( dest );
        }
    else
        {
        s.init( c.whoami(), pn.machine, pn.part );
        s.set_defaults();
        s.put();
        }
    has_changed = true;
    }
else if ( s.init(c.whoami(), pn.machine, pn.part) )
    {
    s.get();
    my_length = s.shot_name_length();
    s.init( pn.computer, pn.machine, pn.part );
    if ( s.get() )
        {
        new_length = s.shot_name_length();
        if ( new_length != my_length )
            {
            sorc.copyto( dest );
            has_changed = true;
            }
        }
    }

return has_changed;
}

/***********************************************************************
*                           CHECK_FOR_NEW_SHOTS                        *
***********************************************************************/
BOOLEAN DS_BACKUP_ENTRY::check_for_new_shots( PART_NAME_ENTRY & name, BOOLEAN forced )
{
PLOOKUP_CLASS plookup;
PLOOKUP_CLASS myplookup;
MACHINE_CLASS ma;

PART_NAME_ENTRY myname;

TIME_CLASS    shot_time;
TIME_CLASS    my_shot_time;

STRING_CLASS  shotname;
STRING_CLASS  myshotname;

int           record_length;
int           my_record_length;
int           record_number;
NAME_CLASS    s;

SHOT_CLASS      record_shot;
COMPUTER_CLASS  c;
PART_NAME_ENTRY dest;
SYSTEMTIME      st;

DB_TABLE        graphlst;
DB_TABLE        mygraphlst;

BOOLEAN         need_to_zip_all;

TIME_CLASS backup_time[MAX_BACKUP_TIME_COUNT];
int        nof_backup_times = 0;

myname = name;
myname.computer = c.whoami();

/*
---------------------------------------------------------------------------------
The first section determines whether I need to create a new part on the Archiver.
I only create a new part on the archiver if this is the current part or if the
operator is forcing a backup of this part.
--------------------------------------------------------------------------------- */
plookup.init( name.computer, name.machine, name.part );
if ( !plookup.exists() )
    return;

plookup.get();

ma.find( name.computer, name.machine );

myplookup.init( c.whoami(), name.machine, name.part );
if ( myplookup.exists() )
    {
    myplookup.get();
    }
else if ( forced || (name.part == ma.current_part) )
    {
    /*
    ---------------------------------------------------------------------
    In that case I use the same plookup, meaning I will backup no old
    shots by default.
    --------------------------------------------------------------------- */
    start_new_part( name );
    myplookup.get();

    /*
    -------------------------------------------------------------------------
    If this is a forced part that was not present on the archiver before then
    I assume I am supposed to back up all shots on that part.
    ------------------------------------------------------------------------- */
    if ( forced )
        {
        s.empty();
        myplookup.set_last_shot_name( s );
        myplookup.set_last_shot_number( 0 );
        myplookup.put();
        last_examined_shot.clear();
        }
    }
else
    {
    return TRUE;
    }

myplookup.get_last_shot_name( myshotname );
plookup.get_last_shot_name( shotname );

/*
---------------------------------------------------------------------------
See if the record length of the sorc file is different than that of my copy
--------------------------------------------------------------------------- */
record_length = 0;
my_record_length = 0;
if ( waitfor_shotsave_semaphor(name.computer, name.machine) )
    {
    s.get_graphlst_dbname( name.computer, name.machine, name.part );
    if ( s.file_exists() )
        {
        if ( t.open(s, AUTO_RECLEN, PFL) )
            {
            record_length = t.record_length();
            t.close();
            s.get_graphlst_dbname( c.whoami(), name.machine, name.part );
            if ( s.file_exists() )
                {
                if ( t.open(s, AUTO_RECLEN, PFL) )
                    {
                    my_record_length = t.record_length();
                    t.close();
                    }
                }
            }
        }
    free_shotsave_semaphore();
    }
else
    {
    return FALSE;
    }

if ( record_length == 0 )
    return TRUE;

if ( my_record_length > 0 && my_record_length != record_length )
    zip_shot_data();

/*
--------------
Copy new shots
-------------- */
if ( waitfor_shotsave_semaphor(name.computer, name.machine) )
    {
    s.get_graphlst_dbname( name.computer, name.machine, name.part );
    if ( !graphlst.open(s, AUTO_RECLEN, PFL) )
        return FALSE;

    dif = record_length - GRAPHLST_RECLEN;

    s.get_graphlst_dbname( c.whoami(), name.machine, name.part );
    if ( !s.file_exists() )
        s.create( s );

    if ( mygraphlst.open(s, record_length, WL) )
        {
        if ( !last_examined_shot.name.isempty() )
            {
            graphlst.put_alpha( GRAPHLST_SHOT_NAME_INDEX, last_examined_shot.name );
            graphlst.goto_first_greater_than_record(1)
            }

        graphlst.get_current_record( NO_LOCK );

        while ( TRUE )
            {
            record_shot.get_from_graphlst( graphlst );

            if ( record_shot.name > shotname )
                break;

            need_to_zip_all = FALSE;

            if ( BackupAtTime )
                {
                for ( i=0; i<NofBackupTimes; i++ )
                    {
                    if ( record_shot.time > BackupTime[i] )
                        {
                        need_to_zip_all = TRUE;
                        break;
                        }
                    }

                for ( i=0; i<NofBackupTimes; i++ )
                    {
                    while ( record_shot.time > BackupTime[i] )
                        {
                        BackupTime[i] += SEC_PER_DAY;
                        }
                    }
                }

            zip_count++;
            if ( MaxZipCount > 0 && zip_count >= MaxZipCount )
                need_to_zip_all = TRUE;

            if ( need_to_zip_all )
                {
                graphlst_record_number = graphlst.current_record_number();
                graphlst.close;
                copy_parameters( record_shot );
                copy_alarms( record_shot );

                free_shotsave_semaphore();

                zip_all_data();

                if ( waitfor_shotsave_semaphor(name.computer, name.machine) )
                    {
                    s.get_graphlst_dbname( name.computer, name.machine, name.part );
                    if ( !graphlst.open(s, AUTO_RECLEN, PFL) )
                        {
                        free_shotsave_semaphore();
                        return FALSE;
                        }
                    graphlst.goto_record_number( record_number );
                    graphlst.get_current_record( NO_LOCK );
                    }
                else
                    {
                    show_semaphore_error( name );
                    return FALSE;
                    }
                }

            copy_profile( myname, name, record_shot.name );
            mygraphlst.copy_rec( graphlst );
            mygraphlst.rec_append();

            last_examined_shot = record_shot;

            if ( !graphlst.get_next_record(FALSE) )
                break;
            }

        mygraphlst.close();
        }

    graphlst.close();
    free_shotsave_semaphore();
    }

return TRUE;
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                            CHECK_FOR_NEW_SHOT                        *
***********************************************************************/
void DS_BACKUP_ENTRY::check_for_new_shot( TCHAR * current_part )
{
bool       shot_was_found;
SHOT_CLASS shot;

shot_was_found = false;
if ( name.part.isempty() || (name.part == NO_PART) )
    start_new_part( current_part );

if ( name.part != current_part )
    {
    zip_all_data();
    start_new_part( current_part );
    }
else if ( shot_name_has_changed(name) )
    {
    zip_all_data();
    }

if ( check_for_local_part(name) )
    {
    if ( shot.get_current(name) )
        {
        shot_was_found = true;
        show_received_shot( name, shot );

        if ( shot.name < last_examined_shot_name )
            {
            zip_shot_data();
            last_examined_shot.clear();
            }
        }
    }

if ( shot_was_found )
    {
    if  ( waitfor_shotsave_semaphor(name.computer, name.machine) )
        {
        copy_profiles( shot );
        copy_parameters( shot );
        copy_alarms( shot );
        free_shotsave_semaphore();
        if ( MaxZipCount > 0 && zip_count >= MaxZipCount )
            {
            zip_all_data();
            set_text( MainDialogWindow, DS_STATUS_TBOX, resource_string(ZIP_COMPLETE_STRING) );
            }
        show_zipped_shot( name, shot, zip_count );
        last_examined_shot = shot;
        }
    else
        {
        show_semaphore_error( name );
        }
    }
else
    {
    show_no_shot( name );
    }
}

/***********************************************************************
*                             DS_BACKUP_ENTRY                          *
*                          CHECK_FOR_NEW_DOWNTIME                      *
***********************************************************************/
void DS_BACKUP_ENTRY::check_for_new_downtime()
{
COMPUTER_CLASS  c;
NAME_CLASS      tn;
PART_NAME_ENTRY dest;
DB_TABLE        s;
DB_TABLE        d;
SYSTEMTIME      st;
TIME_CLASS      t;

dest = name;
dest.computer = c.whoami();

tn.get_downtime_dbname( name.computer, name.machine );

if ( !tn.file_exists() )
    return;

t = last_downtime;

if ( s.open(tn, DOWNTIME_RECLEN, PFL) )
    {
    st = last_downtime.system_time();
    s.put_date(  DOWNTIME_DATE_OFFSET, st );
    s.put_time(  DOWNTIME_TIME_OFFSET, st );

    if ( s.goto_first_greater_than_record(2) )
        {
        s.get_current_record( NO_LOCK );

        tn.get_machine_results_directory( c.whoami(), name.machine );
        if ( !tn.directory_exists() )
            tn.create_directory();

        tn.get_downtime_dbname( c.whoami(), name.machine );
        if ( !tn.file_exists() )
            d.create( tn );

        if ( tn.file_exists() )
            {
            if ( d.open(tn, DOWNTIME_RECLEN, WL) )
                {
                while ( TRUE )
                    {
                    get_time_class_from_db( t, s, DOWNTIME_DATE_OFFSET );
                    d.copy_rec( s );
                    d.rec_append();

                    if ( !s.get_next_record(NO_LOCK) )
                        break;
                    }
                d.close();
                }
            }
        }
    s.close();
    }

if ( t > last_downtime )
    {
    last_downtime = t;
    put_ini_string( DsBackupIniFile, name.machine.text(), LastDownTime, last_downtime.text() );
    }
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
***********************************************************************/
DS_BACKUP_CLASS::DS_BACKUP_CLASS()
{
current_entry = 0;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                 EMPTY                                *
***********************************************************************/
void  DS_BACKUP_CLASS::empty()
{
backuplist.rewind();
while ( TRUE )
    {
    current_entry = (DS_BACKUP_ENTRY *) backuplist.next();
    if ( !current_entry )
        break;
    delete current_entry;
    }

backuplist.remove_all();
}

/***********************************************************************
*                            ~DS_BACKUP_CLASS                          *
***********************************************************************/
DS_BACKUP_CLASS::~DS_BACKUP_CLASS()
{
empty();
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                 REWIND                               *
***********************************************************************/
void DS_BACKUP_CLASS::rewind()
{
backuplist.rewind();
current_entry = 0;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                  NEXT                                *
***********************************************************************/
BOOLEAN DS_BACKUP_CLASS::next()
{
current_entry = (DS_BACKUP_ENTRY *)backuplist.next();
if ( current_entry )
    return TRUE;
return FALSE;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                   =                                  *
***********************************************************************/
void DS_BACKUP_CLASS::operator=( DS_BACKUP_CLASS & sorc )
{
DS_BACKUP_ENTRY * be;

empty();
sorc.rewind();
while ( sorc.next() )
    {
    be = new DS_BACKUP_ENTRY;
    if ( be )
        {
        *be = *(sorc.current_entry);
        if ( !add( be ) )
            delete( be );
        }
    }
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                  ADD                                 *
***********************************************************************/
BOOLEAN DS_BACKUP_CLASS::add( DS_BACKUP_ENTRY * b )
{
short status;

rewind();

while ( next() )
    {
    status = b->name.machine.compare( current_entry->name.machine );
    /*
    --------------------------
    Only one entry per machine
    -------------------------- */
    if ( status == 0 )
        return FALSE;
    /*
    --------------------
    Sort by machine name
    -------------------- */
    if ( status < 0 )
        return backuplist.insert( b );
    }

return backuplist.append( b );
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                  LOAD                                *
***********************************************************************/
void DS_BACKUP_CLASS::load()
{
COMPUTER_CLASS    c;
DS_BACKUP_ENTRY * be;
BOOLEAN           status;
MACHINE_NAME_LIST_CLASS t;
NAME_CLASS        root;
NAME_CLASS        s;
FILE_CLASS        f;
TCHAR           * cp;

set_text( MainDialogWindow, DS_STATUS_TBOX, TEXT("Loading machines") );

empty();
c.rewind();
while ( c.next() )
    {
    /*
    --------------------
    Ignore this computer
    -------------------- */
    if ( c.is_this_computer() )
        continue;

    root.get_root_name( c.name() );
    s = TEXT( "Loading machines at computer " );
    s += root;

    s.set_text( MainDialogWindow, DS_STATUS_TBOX );

    /*
    ------------------------------------------------------------------------------------
    Make the machine list file name for this computer [c:\v5\exes\m02_machine_list.txt].
    ------------------------------------------------------------------------------------ */
    s.get_exe_dir_file_name( root.text() );
    s += TEXT( "_machine_list.txt" );

    if ( c.is_present() )
        {
        t.add_computer( c.name() );
        t.rewind();
        while ( t.next() )
            {
            be = new DS_BACKUP_ENTRY;
            if ( be )
                {
                be->name.computer = c.name();
                be->name.machine  = t.name();
                status = add( be );
                if ( !status )
                    delete be;
                }
            }

        if ( t.count() > 0 )
            {
            t.rewind();
            if ( f.open_for_write(s.text()) )
                {
                while ( t.next() )
                    f.writeline( t.name().text() );
                f.close();
                }
            }
        t.empty();
        }
    else
        {
        /*
        -----------------------------------------------------------------------------
        If the computer isn't online then see if I have saved the machine list before
        ----------------------------------------------------------------------------- */
        if ( f.open_for_read(s.text()) )
            {
            while ( true )
                {
                cp = f.readline();
                if ( !cp )
                    break;
                if ( *cp != NullChar )
                    {
                    be = new DS_BACKUP_ENTRY;
                    if ( be )
                        {
                        be->name.computer = c.name();
                        be->name.machine  = cp;
                        if ( !add(be) )
                            delete be;
                        }
                    }
                }
            f.close();
            }
        }
    }

set_text( MainDialogWindow, DS_STATUS_TBOX, EmptyString );

rewind();
while ( next() )
    current_entry->read();
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                  FIND                                *
***********************************************************************/
BOOLEAN DS_BACKUP_CLASS::find( TCHAR * machine_to_find )
{
if ( current_entry )
    if( current_entry->name.machine == machine_to_find )
        return TRUE;

rewind();
while ( next() )
    if ( current_entry->name.machine == machine_to_find )
        return TRUE;

return FALSE;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                              ZIP_ALL_PARTS                           *
***********************************************************************/
void DS_BACKUP_CLASS::zip_all_parts()
{
rewind();
while ( next() )
    {
    if ( !current_entry->last_examined_shot_name.isempty() )
        {
        current_entry->zip_all_data();
        show_zero_zipped_shots( current_entry->name );
        }
    }

if ( MainDialogWindow )
    {
    set_text( MainDialogWindow, DS_STATUS_TBOX, resource_string(ZIP_COMPLETE_STRING) );
    }
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                         PROCESS_MAILSLOT_MESSAGE                     *
*                       MON,DS_NOTIFY,M01,VISITRAK,DT                  *
*                    MON,DS_NOTIFY,M01,VISITRAK,NS,1395                *
***********************************************************************/
void DS_BACKUP_CLASS::process_mailslot_message( TCHAR * msg )
{
TCHAR * cp;
TCHAR * p;

/*
-----------------------
Skip the topic and item
----------------------- */
cp = findchar( CommaChar, msg );
if ( cp )
    {
    cp++;
    cp = findchar( CommaChar, cp );
    }

if ( cp )
    {
    /*
    ---------------------------------------------------------------
    Point the start of the message at the start of the machine name
    --------------------------------------------------------------- */
    cp++;
    msg = cp;

    /*
    -----------------------------------------
    Look for the comma after the machine name
    ----------------------------------------- */
    cp = findchar( CommaChar, cp );
    }

if ( cp )
    {
    *cp = NullChar;
    /*
    ----------------------------
    Find this machine in my list
    ---------------------------- */
    if ( find(msg) )
        {
        cp++;
        p = cp;
        cp = findchar( CommaChar, cp );
        if ( cp )
            {
            *cp = NullChar;
            cp++;
            if ( *cp == NChar )
                current_entry->check_for_new_shot( p );
            else
                current_entry->check_for_new_downtime();
            }
        }
    }
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                                SHUTDOWN                              *
***********************************************************************/
void DS_BACKUP_CLASS::shutdown()
{
rewind();
while ( next() )
    current_entry->write();
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                           CLEAR_MAX_ZIP_COUNT                        *
***********************************************************************/
void DS_BACKUP_CLASS::clear_max_zip_count()
{
MaxZipCount = 0;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                            SET_MAX_ZIP_COUNT                         *
***********************************************************************/
void DS_BACKUP_CLASS::set_max_zip_count( int32 new_count )
{
MaxZipCount = new_count;
}

/***********************************************************************
*                             DS_BACKUP_CLASS                          *
*                           RELOAD_ZIP_PATHS                           *
***********************************************************************/
void DS_BACKUP_CLASS::reload_zip_paths()
{
rewind();
while ( next() )
    current_entry->read_zip_paths();
}
