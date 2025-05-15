#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR PeriodChar    = TEXT( '.' );

/***********************************************************************
*                    READ_PARAMETERS_FROM_PARMLIST                     *
***********************************************************************/
int32 read_parameters_from_parmlist( float * dest, STRING_CLASS & filename )
{
NAME_CLASS s;
TCHAR    * cp;

FIELDOFFSET  offset;
DB_TABLE     t;
int32        i;
int32        n;
int32        shot_number;

/*
-------------------------------------------------------------------------
Copy the name of the profile plus some extra space for the shotparm name.
------------------------------------------------------------------------- */
s.init( filename.len()+8 );
s = filename;
n = s.len();

if ( n < 1 )
    return 0;

/*
------------------------------------------------------------
Backup past the shot number, nulling out the .pro on the way
------------------------------------------------------------ */
cp = s.text();
cp += n;
while ( *cp != BackslashChar )
    {
    cp--;

    if ( *cp == PeriodChar )
        *cp = NullChar;
    n--;
    if ( n < 1 )
        break;
    }

n = 0;
if ( *cp == BackslashChar )
    {
    cp++;

    /*
    ----------------------------------------
    I'm pointing to the shot number in ascii
    ---------------------------------------- */
    shot_number = asctoint32( cp );

    /*
    -------------------------------------------------------------------
    Replace the shot number with the name of the shotparm database file
    ------------------------------------------------------------------- */
    copystring( cp, SHOTPARM_DB );

    if ( t.open(s, SHOTPARM_RECLEN, PFL) )
        {
        t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, shot_number, SHOT_LEN );
        t.reset_search_mode();
        if ( t.get_next_key_match(1, FALSE) )
            {
            offset = SHOTPARM_PARAM_1_OFFSET;
            for ( i=0; i<MAX_PARMS; i++ )
                {
                dest[i] = t.get_float( offset );
                offset += SHOTPARM_FLOAT_LEN+1;
                n++;
                }
            }
        t.close();
        }
    }

return n;
}

/***********************************************************************
*                     READ_PARAMETERS_FROM_PROFILE                     *
***********************************************************************/
short read_parameters_from_profile( float * parameters, STRING_CLASS & fname )
{
HANDLE         fh;
DWORD          access_mode;
DWORD          bytes_read;
DWORD          share_mode;
BOOL           status;
DWORD          seek_status;
long           offset;
short          nof_parameters;

access_mode = GENERIC_READ;
share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
status      = FALSE;
offset      = sizeof(PROFILE_HEADER);

fh = CreateFile( fname.text(), access_mode, share_mode, 0, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0 );
if ( fh != INVALID_HANDLE_VALUE )
    {
    seek_status = SetFilePointer( fh, offset, 0, FILE_BEGIN );
    if ( seek_status != 0xFFFFFFFF )
        {
        status = ReadFile( fh, &nof_parameters, sizeof(short), &bytes_read, 0 );

        if ( nof_parameters > MAX_PARMS )
            nof_parameters = MAX_PARMS;

        if ( status && nof_parameters > 0 )
            status = ReadFile( fh, parameters, nof_parameters*sizeof(float), &bytes_read, 0 );
        }
    CloseHandle( fh );
    }

if ( !status )
    nof_parameters = 0;

return nof_parameters;
}

/***********************************************************************
*                    CHANGE_SHOT_NUMBER_IN_PROFILE                     *
***********************************************************************/
BOOLEAN change_shot_number_in_profile( STRING_CLASS & fname, int32 new_shot_number )
{
HANDLE         fh;
DWORD          access_mode;
DWORD          bytes_copied;
DWORD          share_mode;
BOOLEAN        status;
DWORD          seek_status;
long           offset;

access_mode = GENERIC_WRITE | GENERIC_READ;
share_mode  = FILE_SHARE_READ;
status      = FALSE;
offset      = sizeof( TCHAR[OLD_MACHINE_NAME_LEN+1] ) + sizeof( TCHAR[OLD_PART_NAME_LEN+1] );

fh = CreateFile( fname.text(), access_mode, share_mode, 0, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0 );
if ( fh != INVALID_HANDLE_VALUE )
    {
    seek_status = SetFilePointer( fh, offset, 0, FILE_BEGIN );
    if ( seek_status != 0xFFFFFFFF )
        {
        WriteFile( fh, &new_shot_number, sizeof(new_shot_number), &bytes_copied, 0 );
        status = TRUE;
        }
    CloseHandle( fh );
    }

return status;
}

/***********************************************************************
*                             COPY_PROFILE                             *
***********************************************************************/
BOOLEAN copy_profile( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc, int32 shot_number, int32 shot_increment, BOOLEAN need_to_check_for_symphony )
{
const BOOL OVERWRITE_EXISTING = FALSE;
NAME_CLASS dest_profile;
NAME_CLASS sorc_profile;
NAME_CLASS s;
bool       is_ftii_profile;

is_ftii_profile = false;
sorc_profile.get_profile_name( sorc.computer, sorc.machine, sorc.part, shot_number );
if ( !sorc_profile.file_exists() )
    {
    sorc_profile.get_profile_ii_name( sorc.computer, sorc.machine, sorc.part, shot_number );
    is_ftii_profile = true;
    }

shot_number += shot_increment;

if ( is_ftii_profile )
    dest_profile.get_profile_ii_name(dest.computer, dest.machine, dest.part, shot_number );
else
    dest_profile.get_profile_name(dest.computer, dest.machine, dest.part, shot_number );

sorc_profile.copyto( dest_profile );

if ( shot_increment && !is_ftii_profile )
    change_shot_number_in_profile( dest_profile, shot_number );

if ( need_to_check_for_symphony )
    {
    s.get_symphony_directory( dest.computer );
    if ( s.directory_exists() )
        {
        s.get_symphony_machine_directory( dest.computer, dest.machine );
        if ( !s.directory_exists() )
            s.create_directory();
        s.cat_path( filename_from_path(dest_profile.text()) );
        dest_profile.copyto( s );
        }
    }

return TRUE;
}

/***********************************************************************
*                             COPY_PROFILE                             *
***********************************************************************/
BOOLEAN copy_profile( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc, int32 shot_number, int32 shot_increment )
{
return copy_profile( dest, sorc, shot_number, shot_increment, FALSE );
}