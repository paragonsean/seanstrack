#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\multistring.h"
#include "..\include\shotname.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR PeriodChar    = TEXT( '.' );

/***********************************************************************
*                    READ_PARAMETERS_FROM_PARMLIST                     *
***********************************************************************/
int32 read_parameters_from_parmlist( MULTISTRING_CLASS & dest, STRING_CLASS & filename )
{
NAME_CLASS s;
TCHAR    * cp;

DB_TABLE     t;
int32        i;
int32        n;
STRING_CLASS shot_name;
short        slen;

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
    shot_name = cp;

    /*
    -------------------------------------------------------------------
    Replace the shot number with the name of the shotparm database file
    ------------------------------------------------------------------- */
    copystring( cp, SHOTPARM_DB );

    if ( t.open(s, 0, PFL) )
        {
        slen = t.field_length( SHOTPARM_SHOT_NAME_INDEX );
        t.put_alpha( SHOTPARM_SHOT_NUMBER_OFFSET, shot_name, slen );
        t.reset_search_mode();
        if ( t.get_next_key_match(1, FALSE) )
            {
            for ( i=0; i<MAX_PARMS; i++ )
                {
                t.get_alpha( dest[i], SHOTPARM_PARAM_1_INDEX + i );
                n++;
                }
            }
        t.close();
        }
    }

return n;
}

/***********************************************************************
*                    CHANGE_SHOT_NAME_IN_PROFILE                     *
***********************************************************************/
BOOLEAN change_shot_name_in_profile( STRING_CLASS & fname, STRING_CLASS & new_shot_name )
{
HANDLE         fh;
DWORD          access_mode;
DWORD          bytes_copied;
DWORD          share_mode;
BOOLEAN        status;
DWORD          seek_status;
long           offset;
int32          new_shot_number;
access_mode = GENERIC_WRITE | GENERIC_READ;
share_mode  = FILE_SHARE_READ;
status      = FALSE;
offset      = sizeof( TCHAR[OLD_MACHINE_NAME_LEN+1] ) + sizeof( TCHAR[OLD_PART_NAME_LEN+1] );

new_shot_number = (int32) new_shot_name.int_value();
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
BOOLEAN copy_profile( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc, STRING_CLASS & shot_name )
{
NAME_CLASS dest_profile;
NAME_CLASS sorc_profile;

sorc_profile.get_profile_ii_name( sorc.computer, sorc.machine, sorc.part, shot_name );
dest_profile.get_profile_ii_name(dest.computer, dest.machine, dest.part, shot_name );
sorc_profile.copyto( dest_profile );

return TRUE;
}