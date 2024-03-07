#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\iniclass.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"

static TCHAR AutoBackupDirKey[] = TEXT( "AutoBackupDir" );
static TCHAR BackupSection[]    = TEXT( "Backup" );
static TCHAR VisitrakIniFile[]  = TEXT( "visitrak.ini" );

/***********************************************************************
*                            AUTO_BACKUP_CLASS                         *
***********************************************************************/
AUTO_BACKUP_CLASS::AUTO_BACKUP_CLASS()
{
HaveAutoBackupComputer = FALSE;
}

/***********************************************************************
*                           ~AUTO_BACKUP_CLASS                         *
***********************************************************************/
AUTO_BACKUP_CLASS::~AUTO_BACKUP_CLASS()
{
}

/***********************************************************************
*                                   DIR                                *
***********************************************************************/
STRING_CLASS & AUTO_BACKUP_CLASS::dir()
{
return AutoBackupComputer;
}

/***********************************************************************
*                                 EXISTS                               *
***********************************************************************/
BOOLEAN AUTO_BACKUP_CLASS::exists()
{
return HaveAutoBackupComputer;
}

/***********************************************************************
*                              GET_BACKUP_DIR                          *
*           The autoback directory can be used as a computer.          *
***********************************************************************/
BOOLEAN AUTO_BACKUP_CLASS::get_backup_dir()
{
INI_CLASS  ini;
NAME_CLASS s;

HaveAutoBackupComputer = FALSE;
s.get_local_ini_file_name( VisitrakIniFile );
ini.set_file( s.text() );
ini.set_section( BackupSection );
if ( ini.find(AutoBackupDirKey) )
    {
    AutoBackupComputer = ini.get_string();
    if ( !AutoBackupComputer.isempty() )
        HaveAutoBackupComputer = TRUE;
    }

return HaveAutoBackupComputer;
}

/***********************************************************************
*                              PUT_BACKUP_DIR                          *
***********************************************************************/
BOOLEAN AUTO_BACKUP_CLASS::put_backup_dir()
{
INI_CLASS  ini;
NAME_CLASS s;

s.get_local_ini_file_name( VisitrakIniFile );
ini.set_file( s.text() );
ini.set_section( BackupSection );
if ( HaveAutoBackupComputer )
    return ini.put_string( AutoBackupDirKey, AutoBackupComputer.text() );
else
    ini.remove_key( AutoBackupDirKey );

return TRUE;
}

/***********************************************************************
*                            SET_BACKUP_DIR                            *
* Normally you use the current autobackup directory but bupres needs   *
* to be able to set the autobackup dir to a restore point.             *
***********************************************************************/
void AUTO_BACKUP_CLASS::set_backup_dir( STRING_CLASS & sorc )
{
if ( sorc.isempty() )
    {
    HaveAutoBackupComputer = FALSE;
    AutoBackupComputer.null();
    }
else
    {
    HaveAutoBackupComputer = TRUE;
    AutoBackupComputer = sorc;
    }
}

/***********************************************************************
*                           BACKUP_DATA_DIR                            *
* Check to make sure all the files in the data directory are backed    *
* up to the autobackup directory.                                      *
***********************************************************************/
BOOLEAN AUTO_BACKUP_CLASS::backup_data_dir()
{
NAME_CLASS d;
NAME_CLASS s;

if ( !HaveAutoBackupComputer )
    return FALSE;

s.get_data_dir_file_name( 0 );
d.get_data_dir_file_name( AutoBackupComputer, 0 );

if ( !d.directory_exists() )
    d.create_directory();

return copy_all_files( d.text(), s.text(), StarDotStar, WITHOUT_SUBS, 0, 0 );
}

/***********************************************************************
*                            BACKUP_EXE_DIR                            *
* Check to make sure all the user files in the exe directory are       *
* backed up to the autobackup directory.                               *
***********************************************************************/
BOOLEAN AUTO_BACKUP_CLASS::backup_exe_dir()
{
const int NOF_EXTENSIONS = 3;
TCHAR * extension[NOF_EXTENSIONS] = { TEXT("*.dat"), TEXT("*.csv"), TEXT("*.ini") };

BOOLEAN    b;
int        i;
NAME_CLASS d;
NAME_CLASS s;

if ( !HaveAutoBackupComputer )
    return FALSE;

s.get_exe_dir_file_name( 0 );
d.get_exe_dir_file_name( AutoBackupComputer, 0 );

if ( !d.directory_exists() )
    d.create_directory();
for ( i=0; i<NOF_EXTENSIONS; i++ )
    {
    b = copy_all_files( d.text(), s.text(), extension[i], WITHOUT_SUBS, 0, 0 );
    if ( !b )
        break;
    }

return b;
}

/***********************************************************************
*                            BACKUP_MACHINE                            *
***********************************************************************/
BOOLEAN AUTO_BACKUP_CLASS::backup_machine( STRING_CLASS & m )
{
COMPUTER_CLASS c;
NAME_CLASS     d;
NAME_CLASS     s;
DB_TABLE       t;

if ( !HaveAutoBackupComputer )
    return FALSE;

/*
---------------------------------------------------
If this machine is not on this computer, do nothing
--------------------------------------------------- */
s.get_machine_directory( c.whoami(), m );
if ( !s.directory_exists() )
    return FALSE;

d.get_machine_directory( AutoBackupComputer, m );
if ( !d.directory_exists() )
    d.create_directory();

if ( copy_all_files(d.text(), s.text(), StarDotStar, WITHOUT_SUBS, 0, 0) )
    {
    d.get_machine_results_directory( AutoBackupComputer, m );
    if ( !d.directory_exists() )
        {
        d.create_directory();
        d.get_downtime_dbname( AutoBackupComputer, m );
        t.ensure_existance( d );
        }
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             BACKUP_PART                              *
***********************************************************************/
BOOLEAN AUTO_BACKUP_CLASS::backup_part( STRING_CLASS & m, STRING_CLASS & p )
{
COMPUTER_CLASS c;
NAME_CLASS     d;
NAME_CLASS     s;
DB_TABLE       t;

if ( !HaveAutoBackupComputer )
    return FALSE;

/*
-------------------------------------------------------
If this part doesn't exist on this computer, do nothing
------------------------------------------------------- */
s.get_part_directory( c.whoami(), m, p );
if ( !s.directory_exists() )
    return FALSE;

d.get_part_directory( AutoBackupComputer, m, p );
if ( !d.directory_exists() )
    d.create_directory();

if ( !copy_all_files(d.text(), s.text(), StarDotStar, WITHOUT_SUBS, 0, 0) )
    return FALSE;

d.get_part_results_directory( AutoBackupComputer, m, p );
if ( !d.directory_exists() )
    d.create_directory();

d.get_alarmsum_dbname( AutoBackupComputer, m,  p );
t.ensure_existance( d );

d.get_graphlst_dbname( AutoBackupComputer, m,  p );
t.ensure_existance( d );

d.get_shotparm_csvname( AutoBackupComputer, m,  p );
t.ensure_existance( d );

s.get_master_name( c.whoami(), m, p );
if ( s.file_exists() )
    {
    d.get_master_name( AutoBackupComputer, m, p );
    s.copyto( d );
    }

s.get_master_ii_name( c.whoami(), m, p );
if ( s.file_exists() )
    {
    d.get_master_ii_name( AutoBackupComputer, m, p );
    s.copyto( d );
    }

return TRUE;
}
