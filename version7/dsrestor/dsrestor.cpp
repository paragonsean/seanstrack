#include <windows.h>
#include <process.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\alarmsum.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\fileclas.h"
#include "..\include\genlist.h"
#include "..\include\limit_switch_class.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\plookup.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\downtime.h"
#include "..\include\subs.h"
#include "..\include\warmupcl.h"

#include "proclass.h"
#include "resource.h"

const BITSETYPE BEGIN_RESTORE_TYPE = 1;
const BITSETYPE ABORT_RESTORE_TYPE = 2;

const int NO_ZIP_FILE_TYPE      = 0;
const int PART_ZIP_FILE_TYPE    = 1;
const int MACHINE_ZIP_FILE_TYPE = 2;

static TCHAR PartZipSuffix[]    = TEXT( ".paz" );
static TCHAR MachineZipSuffix[] = TEXT( ".maz" );
static TCHAR WildProfileName[]  = TEXT( "*.PR?" );
static TCHAR WildCsvFileName[]  = TEXT( "*.CSV" );
static TCHAR WildDatFileName[]  = TEXT( "*.DAT" );
static TCHAR WildTxtFileName[]  = TEXT( "*.TXT" );
const TCHAR NewDirName[]   = TEXT( "New" );
const TCHAR FinalDirName[] = TEXT( "Final" );
const TCHAR DataString[]   = TEXT( "Data" );

HWND      MainWindow;
HWND      MainDialogWindow;
HINSTANCE MainInstance;

TCHAR MyClassName[]   = TEXT( "DsRestor" );
TCHAR MyWindowTitle[] = TEXT( "Restore" );

NAME_CLASS NewDirectory;
NAME_CLASS RealDirectory;
NAME_CLASS FinalDirectory;
NAME_CLASS ZipDirectory;

PART_CLASS DefaultPart;

BOOLEAN   AbortRequested    = FALSE;
BITSETYPE RestoreButtonType = BEGIN_RESTORE_TYPE;

/*
------------------------------------------
Globals used when restoring each zip file;
The computer in RestorePart is whoami.
------------------------------------------ */
PART_NAME_ENTRY RestorePart;
BOOLEAN         IsVersion7        = FALSE;

static TCHAR DefaultRestoreDirectory[] = TEXT( "C:\\V5BACKUP" );
static TCHAR ResultsDirName[] = RESULTS_DIRECTORY_NAME;
static TCHAR AlarmSumFile[]    = TEXT( "alarmsum.txt" );
static TCHAR DowntimeFile[]    = TEXT( "downtime.txt" );
static TCHAR DsRestorIniFile[] = TEXT( "visitrak.ini" );
static TCHAR GraphLstFile[]    = TEXT( "graphlst.txt" );
static TCHAR MachinePartFile[] = TEXT( "machinepart.txt" );
static TCHAR ShotParmFile[]    = TEXT( "shotparm.txt" );
static TCHAR VisiTrakIniFile[] = TEXT( "visitrak.ini" );
static TCHAR ConfigSection[]   = TEXT( "Config" );
static TCHAR RestoreSection[]  = TEXT( "Restore" );
static TCHAR RestoreDirKey[]   = TEXT( "RestoreDir" );
static TCHAR RootDirKey[]      = TEXT( "RootDir" );
static TCHAR StartDirKey[]     = TEXT( "StartDir" );
static TCHAR Filter[]          = TEXT( "Visi-Trak Zip Files\0*.paz;*.maz\0" );
static TCHAR MachsetCsvName[]  = MACHSET_CSV;
static TCHAR PartsetCsvName[]  = PARTSET_CSV;
static TCHAR OldMachsetDbName[]  = MACHSET_DB;
static TCHAR OldPartsetDbName[]  = PARTS_DB;

/***********************************************************************
*                        RESOURCE_MESSAGE_BOX                          *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                          FIND_DEFAULT_PART                           *
***********************************************************************/
static BOOLEAN find_default_part()
{
COMPUTER_CLASS  c;
MACHINE_NAME_LIST_CLASS m;
BOOLEAN         status;
PART_NAME_ENTRY p;
TEXT_LIST_CLASS t;

status = FALSE;

m.add_computer( c.whoami() );
if ( m.count() < 1 )
    return status;

p.computer = c.whoami();

m.rewind();
while ( m.next() )
    {
    p.machine = m.name();
    if ( get_partlist(t, p.computer, p.machine) )
        {
        t.rewind();
        t.next();
        p.part = t.text();
        status = TRUE;
        }
    }

/*
---------------------------------
Load the part as the default part
--------------------------------- */
if ( status )
    status = DefaultPart.find( p.computer, p.machine, p.part );

return status;
}

/***********************************************************************
*                            ZIP_FILE_TYPE                             *
***********************************************************************/
int zip_file_type( STRING_CLASS & filename )
{
if ( filename.contains(PartZipSuffix) )
    return PART_ZIP_FILE_TYPE;

if ( filename.contains(MachineZipSuffix) )
    return MACHINE_ZIP_FILE_TYPE;

return NO_ZIP_FILE_TYPE;
}

/***********************************************************************
*                           GET_ZIP_FILE_LIST                          *
***********************************************************************/
BOOLEAN get_zip_file_list()
{
const TCHAR default_start_dir[] = TEXT( "\\" );
const int32 buflen = 5000;
OPENFILENAME   fh;
TCHAR        * cp;
TCHAR        * fname;
LISTBOX_CLASS lb;
BOOLEAN       status;
NAME_CLASS    startdir;

cp = get_ini_string( DsRestorIniFile, RestoreSection, StartDirKey );
if ( !unknown(cp) )
    startdir = cp;
else
    startdir = default_start_dir;

status = FALSE;
fname = maketext( buflen );
if ( !fname )
    {
    resource_message_box( CANT_DO_THAT_STRING, UNABLE_TO_ALLOC_BUFFER_STRING );
    return FALSE;
    }

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow;
fh.hInstance         = 0;
fh.lpstrFilter       = Filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = fname;
fh.nMaxFile          = buflen;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = startdir.text();
fh.lpstrTitle        = TEXT("Choose Zip File to Load");
fh.Flags             = OFN_EXPLORER | OFN_ALLOWMULTISELECT ;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*fname = NullChar;

if ( GetOpenFileName( &fh ) )
    {
    lb.init( MainDialogWindow, RESTORE_FILES_LISTBOX );
    lb.empty();

    /*
    -----------------------------------------------------------
    If there is only one file, make it look like multiple files
    ----------------------------------------------------------- */
    if ( fh.nFileOffset < lstrlen(fname) )
        {
        if ( fh.nFileOffset > 0 )
            {
            cp = fname + fh.nFileOffset;
            cp--;
            *cp = NullChar;
            cp++;
            cp = nextstring( cp );
            *cp = NullChar;
            }
        }

    if ( fh.nFileOffset > lstrlen(fname) )
        {
        cp = fname + fh.nFileOffset;
        while ( *cp )
            {
            lb.add( cp );
            cp = nextstring( cp );
            }
        ZipDirectory = fname;
        put_ini_string( DsRestorIniFile, RestoreSection, StartDirKey, ZipDirectory.text() );
        }
    status = TRUE;
    }

delete[] fname;
return status;
}

/***********************************************************************
*                           GET_RESTORE_DIRECTORY                      *
***********************************************************************/
static void get_restore_directory()
{
COMPUTER_CLASS c;
TCHAR        * cp;
int32          slen;

/*
--------------------------------------------------------
Default to the drive that has the current root directory
-------------------------------------------------------- */
cp = get_ini_string( VisiTrakIniFile, ConfigSection, RootDirKey );
if ( !unknown(cp) )
    {
    *DefaultRestoreDirectory = *cp;   /* Replace the first character */
    }

cp = get_ini_string( DsRestorIniFile, RestoreSection, RestoreDirKey );
if ( unknown(cp) )
    {
    cp = DefaultRestoreDirectory;
    put_ini_string( DsRestorIniFile, RestoreSection, RestoreDirKey, cp );
    }

slen = lstrlen(cp);
if ( slen > 2 )
    {
    NewDirectory   = cp;
    NewDirectory.cat_path( NewDirName );
    NewDirectory.create_directory();

    FinalDirectory = cp;
    FinalDirectory.cat_path( FinalDirName );
    FinalDirectory.create_directory();
    return;
    }

resource_message_box( UNABLE_TO_CONTINUE_STRING, GET_RESTORE_FAILED_STRING );
PostMessage( MainWindow, WM_CLOSE, 0, 0L );
}

/***********************************************************************
*                         CHECK_FOR_EXISTANCE                          *
* I am assuming that the machset db has been unzipped into the         *
* NewDirectory.                                                        *
***********************************************************************/
BOOLEAN check_for_existance( PART_NAME_ENTRY & p )
{
BOOLEAN         status;
NAME_CLASS      s;
MACHINE_CLASS   dm;
MACHINE_CLASS   m;
PART_CLASS      pc;
PARAMETER_CLASS param;

/*
-----------------------
Copy the machine setups
----------------------- */
m.computer = p.computer;
m.name     = p.machine;

s = NewDirectory;
s.cat_path( MachsetCsvName );
if ( m.read_machset_db(s) )
    {
    if ( machine_exists(p.computer, p.machine) )
        {
        if ( dm.find(p.computer, p.machine) )
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
        }
    else
        {
        /*
        -------------------------------------------------
        This is a new machine, it shouldn't be monitored.
        ------------------------------------------------- */
        m.monitor_flags = MA_NO_MONITORING;
        }
    m.save();
    }

status = machine_exists( p.computer, p.machine );
if ( !status )
    resource_message_box( CANT_DO_THAT_STRING, MACHINE_CREATE_ERROR_STRING );

/*
--------------------
Copy the part setups
-------------------- */
if ( status && !p.part.isempty() && (p.part != NO_PART) )
    {
    if ( !p.exists() )
        copypart( p.computer, p.machine, p.part, DefaultPart.computer, DefaultPart.machine, DefaultPart.name, COPY_PART_ALL );

    pc.computer = p.computer;
    pc.machine  = p.machine;
    pc.name     = p.part;

    s = NewDirectory;
    s.cat_path( PartsetCsvName );
    status = pc.read_part_setup( s );
    if ( status )
        {
        s = NewDirectory;
        s.cat_path( PARMLIST_DB );
        param.computer = p.computer;
        param.machine  = p.machine;
        param.part     = p.part;
        status = param.read_param_db( s );
        }

    if ( status )
        {
        s = NewDirectory;
        s.cat_path( FTCHAN_DB );
        status = pc.read_ftchan_db( s );
        }

    if ( status )
        {
        pc.save();
        param.computer = pc.computer;
        param.machine  = pc.machine;
        param.part     = pc.name;
        param.save();
        }

    status = p.exists();
    if ( !status )
        resource_message_box( CANT_DO_THAT_STRING, PART_CREATE_ERROR_STRING );
    }

return status;
}

/***********************************************************************
*                            CLEAR_DIRECTORY                           *
***********************************************************************/
static void clear_directory( NAME_CLASS & dir )
{
if ( dir.directory_exists() )
    dir.empty_directory();
else
    dir.create_directory();
}

/***********************************************************************
*                             COPY_SETUPS                              *
* The setup files for the machine are in the NewDirectory. The         *
* setups for the part are in the part directory in NewDirectory.       *
*                                                                      *
* If this is a v6 restore the only machine file will be the machset.   *
***********************************************************************/
static void copy_setups()
{
NAME_CLASS      d;
NAME_CLASS      s;
SETPOINT_CLASS  sp;
unsigned int    monitor_flags;

if ( d.get_machine_directory(RestorePart.computer, RestorePart.machine) )
    {
    s = NewDirectory;
    copy_all_files( d.text(), s.text(), (BOOLEAN) WITHOUT_SUBS, (HWND) 0, (int) 0 );

    s.cat_path( RestorePart.part );
    d.cat_path( RestorePart.part );

    copy_all_files( d.text(), s.text(), (BOOLEAN) WITHOUT_SUBS, (HWND) 0, (int) 0 );

    s.get_machset_csvname( RestorePart.computer, RestorePart.machine );
    sp.get( s );
    monitor_flags = sp[MACHSET_MONITOR_FLAGS_INDEX].value.uint_value();
    monitor_flags &= ~MA_MONITORING_ON;
    sp[MACHSET_MONITOR_FLAGS_INDEX].value = monitor_flags;
    sp.put( s );
    }
}

/***********************************************************************
*                           UNZIP_V6_FILES                             *
***********************************************************************/
static BOOLEAN unzip_v6_files( NAME_CLASS & fname )
{
static TCHAR unzip_command[]  = TEXT( "pkzip25 -ext -noz \"" );
static TCHAR files_to_unzip[] = TEXT( "\" *.*" );
const int nof_nocopyfiles = 3;
static TCHAR * nocopyfile[nof_nocopyfiles] = { AlarmSumFile, GraphLstFile, ShotParmFile };

NAME_CLASS      partdir;
NAME_CLASS      d;
NAME_CLASS      dir;
NAME_CLASS      s;
LIMIT_SWITCH_POS_CLASS pos;
SETPOINT_CLASS  sp;
DB_TABLE        t;
TEXT_LIST_CLASS tlist;
WARMUP_CLASS    wc;

int        i;
BOOLEAN    status;

NewDirectory.set_current_directory();

dir = NewDirectory;
dir.cat_path( ResultsDirName );
if ( !dir.create_directory() )
    return FALSE;






/*
--------------------------------------------------------------------------------------------------------------
I am going to convert the v6 files, which are all in one directory, into v7 files. NewDirectory is the machine
directory so I need results, partname, and partname\results directories.
-------------------------------------------------------------------------------------------------------------- */
dir = NewDirectory;
dir.cat_path( ResultsDirName );
if ( !dir.create_directory() )
    return FALSE;

dir = NewDirectory;
dir.cat_path( RestorePart.part );
if ( !dir.create_directory() )
    return FALSE;

partdir = dir;
dir.cat_path( ResultsDirName );
if ( !dir.create_directory() )
    return FALSE;

/*
---------------------------------------------------
Unzip all the files into the part results directory
--------------------------------------------------- */
dir.set_current_directory();

s.get_exe_dir_file_name( unzip_command );
s += fname;
s += files_to_unzip;

status = execute_and_wait( s.text(), dir.text() );
if ( !status )
    {
    resource_message_box( CANT_DO_THAT_STRING, UNZIP_ERROR_STRING );
    NewDirectory.set_current_directory();
    return FALSE;
    }
/*
---------------------------------
Convert the machset to a csv file
--------------------------------- */
s = dir;
s.cat_path( OldMachsetDbName );
if ( s.file_exists() )
    {
    if ( t.open(s, MACHSET_RECLEN, PFL) )
        {
        if ( t.get_next_record(NO_LOCK) )
            {
            sp.setsize( MACHSET_SETPOINT_COUNT );
            extract_setpoints_from_machset_db( sp, t );
            d = NewDirectory;
            d.cat_path( MachsetCsvName );
            sp.put( d );
            }
        t.close();
        }
    s.delete_file();
    }

/*
------------------------------------------------------
Convert the parts to a csv file in the parts directory
------------------------------------------------------ */
s = dir;
s.cat_path( OldPartsetDbName );
if ( s.file_exists() )
    {
    if ( t.open(s, PARTS_RECLEN, PFL) )
        {
        if ( t.get_next_record(NO_LOCK) )
            {
            extract_setpoints_from_parts_db( sp, pos, t );
            d = NewDirectory;
            d.cat_path( RestorePart.part );
            d.cat_path( PartsetCsvName );
            sp.put( d );
            }
        t.close();
        }
    s.delete_file();
    }

/*
----------------------------------------------------------------------------------------------
The warmup.txt file can't be easily converted until the actual part directory exists for sure.
---------------------------------------------------------------------------------------------- */

/*
-------------------------------------------------------------
Move the downtime.txt file into the machine results directory
------------------------------------------------------------- */
s = dir;
s.cat_path( DowntimeFile );
if ( s.file_exists() )
    {
    d = NewDirectory;
    d.cat_path( ResultsDirName );
    d.cat_path( DowntimeFile );
    s.moveto( d );
    }

/*
-------------------------------------------------------------------------------------------------------
Everything else that isn't a part results file is a part file and should be moved to the part directory
------------------------------------------------------------------------------------------------------- */
s = dir;
s.cat_path( WildTxtFileName );
tlist.get_file_list( s.text() );

s = dir;
s.cat_path( WildDatFileName );
tlist.get_file_list( s.text() );

s = dir;
s.cat_path( WildCsvFileName );
tlist.get_file_list( s.text() );

tlist.rewind();
while ( tlist.next() )
    {
    d = tlist.text();
    status = true;
    for ( i=0; i<nof_nocopyfiles; i++ )
        {
        if ( d.contains(nocopyfile[i]) )
            {
            status = false;
            break;
            }
        }

    if ( status )
        {
        s = dir;
        s.cat_path( tlist.text() );
        d = partdir;
        d.cat_path( tlist.text() );
        s.moveto( d );
        }
    }

NewDirectory.set_current_directory();
return TRUE;
}

/***********************************************************************
*                           UNZIP_V7_FILES                             *
***********************************************************************/
static BOOLEAN unzip_v7_files( NAME_CLASS & fname )
{
static TCHAR unzip_command[] = TEXT( "pkzip25 -ext -noz -dir \"" );
static TCHAR files_to_unzip[] = TEXT( "\" *.*" );

NAME_CLASS s;
BOOLEAN    status;

NewDirectory.set_current_directory();

s.get_exe_dir_file_name( TEXT("pkzip25 -ext -dir -noz \"") );
s += fname;
s += files_to_unzip;

status = execute_and_wait( s.text(), NewDirectory.text() );
if ( !status )
    resource_message_box( CANT_DO_THAT_STRING, UNZIP_ERROR_STRING );

return status;
}

/***********************************************************************
*                             UNZIP_FILES                              *
***********************************************************************/
static BOOLEAN unzip_files( NAME_CLASS & fname )
{
if ( IsVersion7 )
    return unzip_v7_files( fname );
else
    return unzip_v6_files( fname );
}

/***********************************************************************
*                             COPY_MASTER                              *
***********************************************************************/
static void copy_master()
{
const TCHAR MasterName[]     = CURRENT_MASTER_TRACE_NAME;
const TCHAR MasterSuffix[]   = MASTER_TRACE_SUFFIX;
const TCHAR MasterIISuffix[] = MASTER_TRACE_II_SUFFIX;

NAME_CLASS d;
NAME_CLASS s;

s.init( NewDirectory.len()   + MASTER_TRACE_NAME_LEN + MASTER_TRACE_SUFFIX_LEN + 1 );
d.init( FinalDirectory.len() + MASTER_TRACE_NAME_LEN + MASTER_TRACE_SUFFIX_LEN + 1 );

s = NewDirectory;
d = FinalDirectory;

d.cat_path( MasterName );
s.cat_path( MasterName );
s += MasterSuffix;

if ( s.file_exists() )
    {
    d += MasterSuffix;
    }
else
    {
    s.replace( MasterSuffix, MasterIISuffix );
    if ( !s.file_exists() )
        return;
    d += MasterIISuffix;
    }
if ( d.file_exists() )
    d.delete_file();

s.moveto( d );
}

/***********************************************************************
*                            MERGE_DOWNTIMES                           *
***********************************************************************/
BOOLEAN merge_downtimes( NAME_CLASS & fname )
{
TIME_CLASS mintime;
NAME_CLASS d;
NAME_CLASS newdir;
NAME_CLASS s;

DOWNTIME_MERGE_CLASS sorc1;
DOWNTIME_MERGE_CLASS sorc2;
DOWNTIME_MERGE_CLASS dest;

clear_directory( NewDirectory );
clear_directory( FinalDirectory );

RealDirectory.get_machine_results_dir_file_name( RestorePart.computer, RestorePart.machine, 0 );

if ( !unzip_files(fname) )
    return FALSE;

if ( !check_for_existance(RestorePart) )
    return FALSE;

newdir = NewDirectory;
newdir.cat_path( RESULTS_DIRECTORY_NAME );

sorc1.open( RealDirectory,   PFL );
sorc2.open( newdir,          PFL );
dest.open(  FinalDirectory,  WL  );

sorc1.next();
sorc2.next();

while ( TRUE )
    {
    if ( sorc1.have_downtime() )
        mintime = sorc1.time();
    else if ( sorc2.have_downtime() )
        mintime = sorc2.time();
    else
        break;

    if ( sorc2 < mintime )
        mintime = sorc2.time();

    if ( sorc2 == mintime )
        {
        dest.add( sorc2 );
        if ( sorc1 == sorc2 )
            sorc1.next();
        sorc2.next();
        }
    else if ( sorc1 == mintime )
        {
        dest.add( sorc1 );
        sorc1.next();
        }
    }

sorc1.close();
sorc2.close();
dest.close();

s = FinalDirectory;
s.cat_path( DOWNTIME_DB );
d.get_downtime_dbname( RestorePart.computer, RestorePart.machine );
s.copyto( d );

return TRUE;
}

/***********************************************************************
*                           MAKE_ALARMSUM_PATH                         *
***********************************************************************/
static void make_alarmsum_path( NAME_CLASS & dest, NAME_CLASS & dir )
{
dest = dir;
dest.cat_path( ALARMSUM_DB );
}

/***********************************************************************
*                             MERGE_ALARMS                             *
***********************************************************************/
BOOLEAN merge_alarms()
{
ALARM_SUMMARY_CLASS sum;
SYSTEMTIME st;

NAME_CLASS dest;
NAME_CLASS newdir;
NAME_CLASS sorc;
TIME_CLASS stime;
TIME_CLASS dtime;
int32      myshift;
int32      last_shift;
DB_TABLE   t;

/*
---------------------------------------------------------------------
I maintain the directories now. NewDirectory is the machine directory
--------------------------------------------------------------------- */
newdir = NewDirectory;
newdir.cat_path( RestorePart.part );
newdir.cat_path( RESULTS_DIRECTORY_NAME );

make_alarmsum_path( sorc, RealDirectory );

if ( !sorc.file_exists() )
    {
    /*
    ----------------------------------------------------------------
    There is no old file (or I don't want it), just copy the new one
    ---------------------------------------------------------------- */
    make_alarmsum_path( sorc, newdir );
    make_alarmsum_path( dest, FinalDirectory );
    if ( sorc.file_exists() )
        sorc.copyto( dest );
    return TRUE;
    }

make_alarmsum_path( dest, newdir );
if ( !dest.file_exists() )
    {
    /*
    -------------------------------------------
    There is no new file, just copy the old one
    ------------------------------------------- */
    make_alarmsum_path( sorc, RealDirectory );
    make_alarmsum_path( dest, FinalDirectory );
    sorc.copyto( dest );
    return TRUE;
    }

/*
------------------------------------------
Copy the older file to the final directory
------------------------------------------ */
get_file_write_time( st, sorc.text() );
stime.set( st );

get_file_write_time( st, dest.text() );
dtime.set( st );

if ( dtime < stime )
    make_alarmsum_path( sorc, newdir );

make_alarmsum_path( dest, FinalDirectory );
sorc.copyto( dest );

/*
--------------------------------------
Put the name of the other file in sorc
-------------------------------------- */
if ( dtime < stime )
    make_alarmsum_path( sorc, RealDirectory );
else
    make_alarmsum_path( sorc, newdir );

init_systemtime_struct( st );
dtime.set( st );

last_shift = NO_SHIFT_NUMBER;

t.open( sorc, ALARMSUM_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    t.get_date( st, ALARMSUM_DATE_OFFSET );
    stime.set( st );
    myshift = t.get_long( ALARMSUM_SHIFT_OFFSET );

    if ( stime != dtime || myshift != last_shift )
        {
        sum.get_counts( st, myshift, sorc.text() );
        sum.save( st, myshift, TRUE, dest );
        dtime      = stime;
        last_shift = myshift;
        }
    if ( AbortRequested )
        break;
    }
t.close();
return TRUE;
}

/***********************************************************************
*                             MERGE_SHOTS                              *
***********************************************************************/
BOOLEAN merge_shots( NAME_CLASS & fname )
{
NAME_CLASS           newdir;
PROFILE_MERGE_CLASS  gnew;
PROFILE_MERGE_CLASS  gori;
PROFILE_MERGE_CLASS  gfinal;
SHOTPARM_MERGE_CLASS pnew;
SHOTPARM_MERGE_CLASS pori;
SHOTPARM_MERGE_CLASS pfinal;
SHOT_CLASS           minshot;
SHOT_CLASS           nextshot;
TCHAR              * s;
TCHAR              * shotname;

clear_directory( NewDirectory );
clear_directory( FinalDirectory );

RealDirectory.get_part_results_directory( RestorePart.computer, RestorePart.machine, RestorePart.part );

if ( !unzip_files(fname) )
    return FALSE;

if ( !check_for_existance(RestorePart) )
    return FALSE;

s = maketext( resource_string(MainInstance, MERGING_SHOT_STRING), SHOT_LEN );
shotname = 0;
if ( s )
    shotname = s + lstrlen(s);

gori.open_graphlst( RealDirectory, PFL );
pori.open_shotparm( RealDirectory, PFL );

/*
-----------------------------------------------------------------------------------------------------
I am maintaining the directory structure now so I need to point to the part results dir for the shots
----------------------------------------------------------------------------------------------------- */
newdir = NewDirectory;
newdir.cat_path( RestorePart.part );
newdir.cat_path( RESULTS_DIRECTORY_NAME );
gnew.open_graphlst( newdir, PFL );
pnew.open_shotparm( newdir, PFL );

gfinal.open_graphlst( FinalDirectory, WL );
pfinal.open_shotparm( FinalDirectory, WL );

gori.rewind();
gori.next();

gnew.rewind();
gnew.next();

pnew.rewind();
pnew.next();

pori.rewind();
pori.next();

nextshot.number = NO_SHOT_NUMBER;

while ( TRUE )
    {
    /*
    ------------------------------
    Get the min of the current lot
    ------------------------------ */
    if ( gori.have_shot() )
        minshot = gori.shot();
    else if ( gnew.have_shot() )
        minshot = gnew.shot();
    else if ( pori.have_shot() )
        minshot = pori.shot();
    else if ( pnew.have_shot() )
        minshot = pnew.shot();
    else
        break;

    /*
    -------------------------------------------------
    If gori had a shot, minshot will be equal to that
    so I don't have to test that instance.
    ------------------------------------------------- */
    if ( gnew < minshot.time )
        minshot = gnew.shot();

    if ( pori < minshot.time )
        minshot = pori.shot();

    if ( pnew < minshot.time )
        minshot = pnew.shot();

    nextshot.time = minshot.time;

    if ( minshot.number > nextshot.number || nextshot.number == NO_SHOT_NUMBER )
        nextshot.number = minshot.number;

    if ( shotname )
        {
        lstrcpy( shotname, int32toasc(nextshot.number) );
        set_text( MainDialogWindow, DS_STATUS_TBOX, s );
        }

    if ( gnew == nextshot.time )
        {
        gfinal.get( gnew, nextshot.number );
        if ( gori == gnew )
            gori.next();
        gnew.next();
        }
    else if ( gori == nextshot.time )
        {
        gfinal.get( gori, nextshot.number );
        gori.next();
        }

    if ( pnew == nextshot.time )
        {
        pfinal.get( pnew, nextshot.number );
        if ( pori == pnew )
            pori.next();
        pnew.next();
        }
    else if ( pori == nextshot.time )
        {
        pfinal.get( pori, nextshot.number );
        pori.next();
        }

    if ( AbortRequested )
        break;
    nextshot.number++;
    }

gori.close();
gnew.close();
gfinal.close();
pori.close();
pnew.close();
pfinal.close();

if ( s )
    delete[] s;


if ( !AbortRequested )
    {
    copy_master();
    merge_alarms();
    }

RealDirectory.delete_directory();
MoveFile( FinalDirectory.text(), RealDirectory.text() );

copy_setups();

clear_directory( NewDirectory );

set_text( MainDialogWindow, DS_STATUS_TBOX, resource_string(MainInstance, FILE_COMPLETE_STRING) );

return TRUE;
}

/***********************************************************************
*                             SET_BUTTON_TYPE                          *
***********************************************************************/
static void set_button_type( BITSETYPE new_button_type )
{
UINT id;

RestoreButtonType = new_button_type;

if ( new_button_type & BEGIN_RESTORE_TYPE )
    id = BEGIN_RESTORE_STRING;
else
    id = ABORT_RESTORE_STRING;

set_text( MainDialogWindow, BEGIN_RESTORE_BUTTON, resource_string(MainInstance, id) );
}

/***********************************************************************
*                           SET_BUTTON_STATE                           *
***********************************************************************/
static void set_button_state( int id, BOOL is_enabled )
{
EnableWindow( GetDlgItem(MainDialogWindow, id), is_enabled );
}

/***********************************************************************
*                        TURN_OFF_BEGIN_BUTTON                         *
***********************************************************************/
static void turn_off_begin_button()
{
set_button_state( BEGIN_RESTORE_BUTTON, FALSE );
}

/***********************************************************************
*                         TURN_ON_BEGIN_BUTTON                         *
***********************************************************************/
static void turn_on_begin_button()
{
set_button_state( BEGIN_RESTORE_BUTTON, TRUE );
}

/***********************************************************************
*                        TURN_OFF_CHOOSE_BUTTON                        *
***********************************************************************/
static void turn_off_choose_button()
{
set_button_state( CHOOSE_FILES_BUTTON, FALSE );
}

/***********************************************************************
*                        TURN_ON_CHOOSE_BUTTON                         *
***********************************************************************/
static void turn_on_choose_button()
{
set_button_state( CHOOSE_FILES_BUTTON, TRUE );
}

/***********************************************************************
*                         PURGE_MACHINE_DATA                           *
***********************************************************************/
static void purge_machine_data()
{
DB_TABLE   t;
NAME_CLASS s;

s.get_downtime_dbname( RestorePart.computer, RestorePart.machine );
s.delete_file();
t.ensure_existance( s );
}

/***********************************************************************
*                           PURGE_PART_DATA                            *
***********************************************************************/
static void purge_part_data()
{
DB_TABLE      t;
NAME_CLASS    s;
PLOOKUP_CLASS plookup;

if ( !machine_exists(RestorePart.computer, RestorePart.machine) )
    return;

if ( !part_exists(RestorePart) )
    return;

/*
------------------
Purge the profiles
------------------ */
s.get_graphlst_dbname( RestorePart.computer, RestorePart.machine, RestorePart.part );
s.delete_file();
t.ensure_existance( s );

s.get_part_results_dir_file_name( RestorePart.computer, RestorePart.machine, RestorePart.part, 0 );
s.empty_directory( WildProfileName );

/*
--------------------
Purge the parameters
-------------------- */
s.get_shotparm_dbname( RestorePart.computer, RestorePart.machine, RestorePart.part );
s.delete_file();
t.ensure_existance( s );

/*
----------------
Purge the alarms
---------------- */
s.get_alarmsum_dbname( RestorePart.computer, RestorePart.machine, RestorePart.part );
s.delete_file();
t.ensure_existance( s );

plookup.init( RestorePart.computer, RestorePart.machine, RestorePart.part );
plookup.set_last_shot_number( 0 );
}

/***********************************************************************
*                          PART_FROM_ZIP_FILE                          *
* I extract the machset and part files to the NewDiirectory,           *
* read the machine and part, and then delete them. I put the part      *
* name information into the global RestorePart.                        *
***********************************************************************/
static bool part_from_zip_file( STRING_CLASS & zipfile )
{
static TCHAR pkzip_command[] = TEXT( "pkzip25 -ext -noz \"" );
static TCHAR files_to_unzip[] = TEXT( " ") MACHSET_DB TEXT(" ") PARTS_DB;

COMPUTER_CLASS c;
DB_TABLE       t;
FILE_CLASS     f;
NAME_CLASS     s;
bool           status;

status = false;

NewDirectory.set_current_directory();
RestorePart.computer = c.whoami();
RestorePart.machine.null();
RestorePart.part.null();

/*
--------------------------------------------------------------
Make sure the file I'm trying to restore doesn't exist already
-------------------------------------------------------------- */
DeleteFile( MachinePartFile );
DeleteFile( OldMachsetDbName );
DeleteFile( OldPartsetDbName );

/*
--------------------------------------------------------------------------------------------------------------
Make up the unzip command to unzip all the files. Only the MachinePartFile will exist if this is a V7 archive.
pkzip25 -ext -noz "d:\\v5backup\20_visitrak_1_10.paz" machinepart.txt machset.txt parts.txt
-------------------------------------------------------------------------------------------------------------- */
s.get_exe_dir_file_name( pkzip_command );
s += zipfile;
s += DoubleQuoteChar;
s += SpaceChar;
s += MachinePartFile;
s += files_to_unzip;

IsVersion7 = FALSE;
if ( execute_and_wait(s.text(), NewDirectory.text()) )
    {
    if ( f.open_for_read(MachinePartFile) )
        {
        IsVersion7 = TRUE;
        RestorePart.machine = f.readline();
        RestorePart.part    = f.readline();
        f.close();
        DeleteFile( MachinePartFile );
        status = true;
        }
    else
        {
        if ( t.open(OldMachsetDbName, MACHSET_RECLEN, PFL) )
            {
            if ( t.get_next_record(NO_LOCK) )
                t.get_alpha( RestorePart.machine, MACHSET_MACHINE_NAME_OFFSET, OLD_MACHINE_NAME_LEN );
            t.close();
            }
        DeleteFile( OldMachsetDbName );

        if ( t.open(OldPartsetDbName, PARTS_RECLEN, PFL) )
            {
            if ( t.get_next_record(NO_LOCK) )
                t.get_alpha( RestorePart.part, PARTS_PART_NAME_OFFSET, OLD_PART_NAME_LEN );
            t.close();
            status = true;
            }
        DeleteFile( OldPartsetDbName );
        }
    }
else
    {
    resource_message_box( CANT_DO_THAT_STRING, UNZIP_ERROR_STRING );
    }

return status;
}

/***********************************************************************
*                           RESTORE_THREAD                             *
* The IsVersion7 global is set by the part_from_zip_file routine and   *
* everyone else can read it thereafter.                                *
* The name of the part being restored is in the global RestorePart.    *
***********************************************************************/
void restore_thread( void * notused )
{
int32           i;
int32           n;
LISTBOX_CLASS   lb;
NAME_CLASS      filename;
NAME_CLASS      s;
int             ziptype;

lb.init( MainDialogWindow, RESTORE_FILES_LISTBOX );
n = lb.count();
for ( i=0; i<n; i++ )
    {
    lb.setcursel( i );
    filename = lb.selected_text();

    s = ZipDirectory;
    s.cat_path( filename );
    if ( !part_from_zip_file(s) )
        continue;

    ziptype = zip_file_type( filename );
    if ( is_checked(MainDialogWindow, PURGE_BEFORE_RESTORE_RADIOBUTTON) )
        {
        if ( ziptype == PART_ZIP_FILE_TYPE )
            purge_part_data();
        else if (ziptype == MACHINE_ZIP_FILE_TYPE )
            purge_machine_data();
        }

    if ( AbortRequested )
        break;

    if ( ziptype == PART_ZIP_FILE_TYPE )
        merge_shots( s );
    else if (ziptype == MACHINE_ZIP_FILE_TYPE )
        merge_downtimes( s );

    if ( AbortRequested )
        break;
    }

AbortRequested = FALSE;

set_text( MainDialogWindow, DS_STATUS_TBOX, resource_string(MainInstance, COPY_COMPLETE_STRING) );
set_button_type( BEGIN_RESTORE_TYPE );
turn_on_choose_button();
}

/***********************************************************************
*                         START_RESTORE_THREAD                         *
***********************************************************************/
static void start_restore_thread()
{
unsigned long h;

h = _beginthread( restore_thread, 0, NULL );
if ( h == -1 )
   resource_message_box( CREATE_FAILED_STRING, RESTORE_THREAD_STRING );
}

/***********************************************************************
*                              ABOUT_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK about_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int           id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        break;
    }

return FALSE;
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        MainDialogWindow = hWnd;
        turn_off_begin_button();
        turn_on_choose_button();
        CheckRadioButton( hWnd, PURGE_BEFORE_RESTORE_RADIOBUTTON, MERGE_WITH_EXISTING_RADIOBUTTON, PURGE_BEFORE_RESTORE_RADIOBUTTON );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case BEGIN_RESTORE_BUTTON:
                if ( RestoreButtonType == ABORT_RESTORE_TYPE )
                    {
                    AbortRequested = TRUE;
                    }
                else
                    {
                    AbortRequested = FALSE;
                    set_button_type( ABORT_RESTORE_TYPE );
                    turn_off_choose_button();
                    start_restore_thread();
                    }
                return TRUE;

            case CHOOSE_FILES_BUTTON:
                turn_off_choose_button();
                if ( get_zip_file_list() )
                    turn_on_begin_button();
                turn_on_choose_button();
                return TRUE;

            default:
                break;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
BOOL CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_DBINIT :
        shrinkwrap( MainWindow, MainDialogWindow );
        get_restore_directory();
        if ( !find_default_part() )
            {
            resource_message_box( CANT_DO_THAT_STRING, DEFAULT_PART_ERROR_STRING );
            PostMessage( hWnd, WM_CLOSE, 0, 0L );
            }
        return 0;

    case WM_COMMAND:
        switch ( id )
            {
            case ABOUT_MENU:
                DialogBox(
                    MainInstance,
                    "ABOUT_DIALOG",
                    hWnd,
                    (DLGPROC) about_dialog_proc );
                return 0;

            case MINIMIZE_MENU:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_MENU:
                PostMessage( hWnd, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOL init( HINSTANCE this_instance )
{
WNDCLASS wc;
COMPUTER_CLASS c;

MainInstance = this_instance;
c.startup();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("DATA_SERVER_RESTORE_ICON") );
wc.lpszMenuName  = (LPSTR) "MainMenu";
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle,
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    279, 244,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainWindow )
    return FALSE;

MainDialogWindow  = CreateDialog(
    MainInstance,
    "MainDialog",
    MainWindow,
    (DLGPROC) main_dialog_proc );

ShowWindow( MainWindow, SW_SHOW );
UpdateWindow( MainWindow );

return TRUE;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;
BOOL status;

if ( is_previous_instance(MyClassName, MyWindowTitle) )
    return 0;

if ( !init(this_instance) )
    return FALSE;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialogWindow )
        status = IsDialogMessage( MainDialogWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        }
    }

shutdown();
return msg.wParam;
}
