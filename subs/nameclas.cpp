#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"

static TCHAR BackSlashChar     = TEXT( '\\' );
static TCHAR ForwardSlashChar  = TEXT( '/' );
static TCHAR NullChar          = TEXT( '\0' );
static TCHAR PeriodChar        = TEXT( '.' );
static TCHAR SpaceChar         = TEXT( ' ' );
static TCHAR ZeroChar          = TEXT( '0' );
static TCHAR Data[]            = TEXT( "data" );
static TCHAR Display[]         = TEXT( "display" );
static TCHAR Log[]             = TEXT( "log" );
static TCHAR Exes[]            = TEXT( "exes" );
static TCHAR Results[]         = TEXT( "results" );
static TCHAR Resources[]       = TEXT( "resources" );
static TCHAR VisiTrakIniFile[] = TEXT( "visitrak.ini" );
static TCHAR ConfigSection[]   = TEXT( "Config" );
static TCHAR ThisComputerKey[] = TEXT( "ThisComputer" );

static STRING_CLASS EmptyString;
static COMPUTER_CLASS MyComputer;

/***********************************************************************
*                        ENSURE_FILE_EXISTANCE                         *
***********************************************************************/
BOOLEAN NAME_CLASS::ensure_file_existance()
{
static SECURITY_ATTRIBUTES sa = {sizeof(sa), 0, FALSE };

DWORD  access_mode;
HANDLE fh;
DWORD  share_mode;

if ( isempty() )
    return FALSE;

if ( !file_exists() )
    {
    access_mode = GENERIC_READ | GENERIC_WRITE;
    share_mode  = 0;
    fh = CreateFile( text(), access_mode, share_mode, &sa, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, 0 );
    if ( !fh )
        return FALSE;
    CloseHandle( fh );
    fh = 0;
    }

return TRUE;
}

/***********************************************************************
*                   ENSURE_FILE_DIRECTORY_EXISTANCE                    *
***********************************************************************/
BOOLEAN NAME_CLASS::ensure_file_directory_existance()
{
NAME_CLASS d;

d = *this;

if ( !d.remove_ending_filename() )
    return FALSE;

if ( !d.directory_exists() )
    {
    if ( !d.create_directory() )
        return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                        GET_CURRENT_DIRECTORY                         *
***********************************************************************/
BOOLEAN NAME_CLASS::get_current_directory()
{
int n;

n = (int) GetCurrentDirectory( 0, 0 );
if ( n > len() )
    {
    if ( !upsize(n) )
        return FALSE;
    }

n = (int) GetCurrentDirectory( n, text() );
if ( n > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                        SET_CURRENT_DIRECTORY                         *
***********************************************************************/
BOOLEAN NAME_CLASS::set_current_directory()
{
BOOL status;
status = FALSE;
if ( !isempty() )
    status = SetCurrentDirectory( text() );

if ( status )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                         GET_LAST_WRITE_TIME                          *
***********************************************************************/
BOOLEAN NAME_CLASS::get_last_write_time( FILETIME & dest )
{
FILE_CLASS f;
BOOLEAN    status;

if ( !file_exists() )
    return FALSE;

status = FALSE;
if ( f.open_for_read(text()) )
    {
    status = f.get_creation_time( &dest );   /* This is actually get_last_write_time */
    f.close();
    }

return status;
}

/***********************************************************************
*                               COPYTO                                 *
***********************************************************************/
BOOLEAN NAME_CLASS::copyto( STRING_CLASS & dest )
{
if ( CopyFile(text(), dest.text(), FALSE) )
    return TRUE;
return FALSE;
}

/***********************************************************************
*                               MOVETO                                 *
***********************************************************************/
BOOLEAN NAME_CLASS::moveto( STRING_CLASS & dest )
{
if ( MoveFile(text(), dest.text()) )
    return TRUE;
return FALSE;
}

/***********************************************************************
*                         ADD_ENDING_BACKSLASH                         *
***********************************************************************/
void NAME_CLASS::add_ending_backslash()
{
TCHAR * cp;
int     n;
n = len();
if ( n < 1 )
    return;
cp = text() + n -1;
if ( *cp != BackSlashChar )
    STRING_CLASS::operator+=( BackSlashChar );
}

/***********************************************************************
*                   NAME_CLASS::CONVERT_TO_DATA_DIR                    *
* This has to be an exe dir, or a data or display directory or         *
* subdirectory thereof.                                                *
***********************************************************************/
BOOLEAN NAME_CLASS::convert_to_data_dir()
{
TCHAR * cp;

cp = STRING_CLASS::find( Data );

if ( !cp )
    cp = STRING_CLASS::find( Exes );

if ( !cp )
    cp = STRING_CLASS::find( Display );

if ( cp )
    {
    copystring( cp, Data );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            GET_ROOT_NAME                             *
* For a network path, this makes the name of the computer, e.g.        *
* \\W01\v7 becomes "W01".                                              *
* A local computer becomes "C:".                                       *
***********************************************************************/
BOOLEAN NAME_CLASS::get_root_name( STRING_CLASS & computer )
{
TCHAR * cp;

if ( computer.isempty() )
    return FALSE;

cp = computer.text();
while ( *cp == BackSlashChar )
    cp++;

if ( *cp == NullChar )
    return FALSE;

STRING_CLASS::operator=( cp );

cp = text();
while ( true )
    {
    if ( *cp == BackSlashChar )
        *cp = NullChar;

    if ( *cp == NullChar )
        break;

    cp++;
    }

return TRUE;
}

/***********************************************************************
*                             FIND_EXTENSION                           *
*   This finds the file name extension from a path name. If there is   *
*   no extension a zero is returned.                                   *
***********************************************************************/
TCHAR * NAME_CLASS::find_extension()
{
TCHAR * bp;
TCHAR * cp;
int     n;

n = len();
if ( n < 2 )
    return FALSE;

bp = text();
cp = bp + n - 1;

while ( cp > bp )
    {
    cp--;
    if ( *cp == PeriodChar )
        return cp;

    if ( *cp == BackSlashChar )
        break;
    }

return 0;
}

/***********************************************************************
*                             FIND_UNIQUE_NAME                         *
* If the file exists, increase the ending number until it is unique.   *
***********************************************************************/
BOOLEAN NAME_CLASS::find_unique_name()
{
TCHAR      * cp;
NAME_CLASS   d;
STRING_CLASS extension;
STRING_CLASS root;
int          n;

if ( !file_exists() )
    return TRUE;

n = 1;
cp = find_extension();
if ( cp )
    {
    extension = cp;
    *cp = NullChar;
    }
root = text();

while ( true )
    {
    d = root;
    d += n;
    d += extension;
    if ( !d.file_exists() )
        {
        STRING_CLASS::operator=( d );
        return TRUE;
        }
    n++;
    }
}

/***********************************************************************
*                              GET_EXTENSION                           *
*   This extracts the extension from a path name. If there is no       *
*   extension the string is nullified. Extension includes '.'          *
***********************************************************************/
BOOLEAN NAME_CLASS::get_extension( TCHAR * sorc )
{
TCHAR * cp;

if ( sorc )
    {
    cp = sorc + lstrlen(sorc);
    while ( cp > sorc )
        {
        cp--;
        if ( *cp == PeriodChar )
            {
            STRING_CLASS::operator=( cp );
            return TRUE;
            }

        if ( *cp == BackSlashChar )
            break;
        }
    }

null();
return FALSE;
}

/***********************************************************************
*                              GET_EXTENSION                           *
***********************************************************************/
BOOLEAN NAME_CLASS::get_extension( STRING_CLASS & sorc )
{
return get_extension( sorc.text() );
}

/***********************************************************************
*                             GET_FILE_NAME                            *
*   This extracts the file name from a path name. If there is no       *
*   file name the string is nullified. The file name does not          *
*   include the extension.                                             *
***********************************************************************/
BOOLEAN NAME_CLASS::get_file_name( TCHAR * sorc )
{
TCHAR * cp;
TCHAR * dest;
TCHAR * ep;
int     n;

if ( sorc )
    {
    cp = sorc + lstrlen( sorc );
    ep = cp;

    while ( cp > sorc )
        {
        cp--;
        if ( *cp == PeriodChar )
            {
            ep = cp;
            }
        else if ( *cp == BackSlashChar )
            {
            cp++;
            if ( cp == ep )
                break;
            n = ep - cp;
            STRING_CLASS::upsize( n );
            dest = text();
            while ( cp < ep )
                {
                *dest = *cp;
                dest++;
                cp++;
                }
            *dest = NullChar;
            return TRUE;
            }
        }
    }

null();
return FALSE;
}

/***********************************************************************
*                              GET_FILE_NAME                           *
***********************************************************************/
BOOLEAN NAME_CLASS::get_file_name( STRING_CLASS & sorc )
{
return get_file_name( sorc.text() );
}

/***********************************************************************
*                           remove_extension                           *
***********************************************************************/
BOOLEAN NAME_CLASS::remove_extension()
{
TCHAR * cp;
cp = find_extension();
if ( cp )
    {
    *cp = NullChar;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           replace_extension                           *
***********************************************************************/
void NAME_CLASS::replace_extension( TCHAR * new_extension )
{
remove_extension();
if ( *new_extension != PeriodChar )
    STRING_CLASS::operator+=( PeriodChar );
STRING_CLASS::operator+=( new_extension );
}

/***********************************************************************
*                       REMOVE_ENDING_BACKSLASH                        *
***********************************************************************/
void NAME_CLASS::remove_ending_backslash()
{
TCHAR * cp;
int     n;
n = len();
if ( n < 1 )
    return;
cp = text() + n -1;
if ( *cp == BackSlashChar )
    *cp = NullChar;
}

/***********************************************************************
*                  NAME_CLASS::REMOVE_ENDING_FILENAME                  *
***********************************************************************/
BOOLEAN NAME_CLASS::remove_ending_filename()
{
if ( !isempty() )
    return dir_from_filename( text() );

return FALSE;
}

/***********************************************************************
*                       REMOVE_DIR_FROM_FILE_PATH                      *
*        Remove the directory path leaving just the file name.         *
***********************************************************************/
BOOLEAN NAME_CLASS::remove_dir_from_file_path()
{
TCHAR * bp;
TCHAR * cp;
if ( isempty() )
    return FALSE;

bp = text();
cp = last_char();

while ( cp > bp )
    {
    if ( *cp == BackSlashChar || *cp == ForwardSlashChar )
        {
        cp++;
        if ( *cp == NullChar )
            return FALSE;
        while ( true )
            {
            *bp = *cp;
            if ( *cp == NullChar )
                return TRUE;
            bp++;
            cp++;
            }
        }
    cp--;
    }

/*
-------------------------------
It was already just a file name
------------------------------- */
return TRUE;
}

/***********************************************************************
*                        GET_EXE_DIR_FILE_NAME                         *
*                 \\data-archiver\v5\exes\visitrak.ini                 *
***********************************************************************/
BOOLEAN NAME_CLASS::get_exe_dir_file_name( STRING_CLASS & computer, const TCHAR * file_name )
{
if ( computer.isempty() )
    return FALSE;

STRING_CLASS::operator=( computer );
cat_path( Exes );

if ( file_name )
    cat_path( file_name );

return TRUE;
}

/***********************************************************************
*                        GET_EXE_DIR_FILE_NAME                         *
*                        c:\v5\exes\visitrak.ini                       *
***********************************************************************/
BOOLEAN NAME_CLASS::get_exe_dir_file_name( const TCHAR * file_name )
{
STRING_CLASS::operator=( MyComputer.whoami() );
cat_path( Exes );

if ( file_name )
    cat_path( file_name );

return TRUE;
}

/***********************************************************************
*                        GET_LOG_DIR_FILE_NAME                         *
*                 \\data-archiver\v5\log\visitrak.ini                  *
***********************************************************************/
BOOLEAN NAME_CLASS::get_log_dir_file_name( STRING_CLASS & computer, const TCHAR * file_name )
{
if ( computer.isempty() )
    return FALSE;

STRING_CLASS::operator=( computer );
cat_path( Log );

if ( file_name )
    cat_path( file_name );

return TRUE;
}

/***********************************************************************
*                        GET_LOG_DIR_FILE_NAME                         *
*                        c:\v5\log\visitrak.ini                        *
***********************************************************************/
BOOLEAN NAME_CLASS::get_log_dir_file_name( const TCHAR * file_name )
{
STRING_CLASS::operator=( MyComputer.whoami() );
cat_path( Log );

if ( file_name )
    cat_path( file_name );

return TRUE;
}

/***********************************************************************
*                       GET_STRINGTABLE_NAME                           *
*                 \\w01\v5\exes\resources\profile.stbl                 *
***********************************************************************/
BOOLEAN NAME_CLASS::get_stringtable_name( STRING_CLASS & computer, const TCHAR * file_name )
{
if ( computer.isempty() )
    return FALSE;

STRING_CLASS::operator=( computer );

cat_path( Exes );
cat_path( Resources );
if ( file_name )
    {
    cat_path( file_name );
    STRING_CLASS::operator+=( STRINGTABLE_NAME_SUFFIX );
    }

return TRUE;
}

/***********************************************************************
*                       GET_STRINGTABLE_NAME                           *
*                    c:\v5\exes\resources\profile.stbl                 *
***********************************************************************/
BOOLEAN NAME_CLASS::get_stringtable_name( const TCHAR * file_name )
{
STRING_CLASS::operator=( MyComputer.whoami() );

cat_path( Exes );
cat_path( Resources );
if ( file_name )
    {
    cat_path( file_name );
    STRING_CLASS::operator+=( STRINGTABLE_NAME_SUFFIX );
    }

return TRUE;
}

/***********************************************************************
*                      GET_DISPLAY_DIR_FILE_NAME                       *
*                 \\data-archiver\v5\display\display.ini               *
***********************************************************************/
BOOLEAN NAME_CLASS::get_display_dir_file_name( STRING_CLASS & computer, const TCHAR * file_name )
{
if ( computer.isempty() )
    return FALSE;

STRING_CLASS::operator=( computer );
cat_path( Display );

if ( file_name )
    cat_path( file_name );

return TRUE;
}

/***********************************************************************
*                      GET_DISPLAY_DIR_FILE_NAME                       *
*                 \\data-archiver\v5\display\display.ini               *
***********************************************************************/
BOOLEAN NAME_CLASS::get_display_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, const TCHAR * file_name )
{
if ( computer.isempty() )
    return FALSE;

STRING_CLASS::operator=( computer );

cat_path( Display );

if ( !machine.isempty() )
    {
    cat_path( machine );
    if ( file_name )
        cat_path( file_name );
    }

return TRUE;
}

/***********************************************************************
*                        GET_DATA_DIR_FILE_NAME                        *
*                        \\w01\v5\data\file_name                       *
***********************************************************************/
BOOLEAN NAME_CLASS::get_data_dir_file_name( STRING_CLASS & computer, const TCHAR * file_name )
{
if ( computer.isempty() )
    return FALSE;

STRING_CLASS::operator=( computer );
cat_path( Data );
if ( file_name )
    cat_path( file_name );

return TRUE;
}

/***********************************************************************
*                        GET_DATA_DIR_FILE_NAME                        *
*                        \\w01\v5\data\file_name                       *
***********************************************************************/
BOOLEAN NAME_CLASS::get_data_dir_file_name( const TCHAR * file_name )
{
return get_data_dir_file_name( MyComputer.whoami(), file_name );
}

/***********************************************************************
*                      GET_MACHINE_DIR_FILE_NAME                       *
***********************************************************************/
BOOLEAN NAME_CLASS::get_machine_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, const TCHAR * file_name )
{
if ( get_data_dir_file_name(computer, 0) )
    {
    if ( !machine.isempty() )
        {
        cat_path( machine );
        if ( file_name )
            cat_path( file_name );
        }
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      GET_MACHINE_LIST_FILE_NAME                      *
* This file is saved in the local exe directory and contains a list    *
* of the machines at the sorc_computer the last time it was online.    *
* c:\v5\exes\m02_machine_list.txt                                      *
***********************************************************************/
BOOLEAN NAME_CLASS::get_machine_list_file_name( STRING_CLASS & sorc_computer )
{
STRING_CLASS root;

root = sorc_computer;
if ( root_from_path(root) )
    {
    get_exe_dir_file_name( root.text() );
    STRING_CLASS::operator+=( TEXT("_machine_list.txt") );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        GET_PART_DIR_FILE_NAME                        *
***********************************************************************/
BOOLEAN NAME_CLASS::get_part_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, const TCHAR * file_name )
{
if ( get_machine_dir_file_name(computer, machine, 0) )
    {
    if ( !part.isempty() )
        {
        cat_path( part );
        if ( file_name )
            cat_path( file_name );
        }
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    GET_PART_RESULTS_DIR_FILE_NAME                    *
***********************************************************************/
BOOLEAN NAME_CLASS::get_part_results_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, const TCHAR * file_name )
{

if ( get_part_dir_file_name(computer, machine, part, 0) )
    {
    cat_path( Results );
    if ( file_name )
        cat_path( file_name );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                  GET_MACHINE_RESULTS_DIR_FILE_NAME                   *
***********************************************************************/
BOOLEAN NAME_CLASS::get_machine_results_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, const TCHAR * file_name )
{
if ( get_machine_dir_file_name(computer, machine, 0) )
    {
    cat_path( Results );
    if ( file_name )
        cat_path( file_name );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           GET_PROFILE_NAME                           *
***********************************************************************/
BOOLEAN NAME_CLASS::get_profile_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, const TCHAR * shot_name, const TCHAR * suffix )
{
STRING_CLASS shot_file;

shot_file = shot_name;
if ( shot_file.isnumeric() )
    {
    if ( shot_file.len() < SHOT_LEN )
        shot_file.rjust( SHOT_LEN, ZeroChar );
    else
        shot_file.replace_all( SpaceChar, ZeroChar );
    }

if ( *suffix != PeriodChar )
    shot_file += PeriodChar;
shot_file += suffix;

return get_part_results_dir_file_name( computer, machine, part, shot_file.text() );
}

/***********************************************************************
*                           GET_PROFILE_NAME                           *
***********************************************************************/
BOOLEAN NAME_CLASS::get_profile_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, STRING_CLASS & shot_name, const TCHAR * suffix )
{
return get_profile_name( computer, machine, part, shot_name.text(), suffix );
}

/***********************************************************************
*                   GET_STATIC_TEXT_PARAMETERS_CSVNAME                 *
***********************************************************************/
BOOLEAN NAME_CLASS::get_static_text_parameters_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
return get_part_dir_file_name( computer, machine, part, STATIC_TEXT_PARAMETERS_CSV );
}
