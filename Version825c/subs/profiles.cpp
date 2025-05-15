#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\multistring.h"
#include "..\include\shotname.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\vdbclass.h"

static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR PeriodChar    = TEXT( '.' );
static TCHAR ZeroString[]  = TEXT( "0.0" );

/***********************************************************************
*                    READ_PARAMETERS_FROM_PARMLIST                     *
*           filename is the full path name of the shot profile         *
***********************************************************************/
int32 read_parameters_from_parmlist( MULTISTRING_CLASS & dest, STRING_CLASS & filename )
{
const int ONE_KEY_FIELD = 1;

int32        i;
int32        n;
NAME_CLASS   s;
NAME_CLASS   shot_name;
VDB_CLASS    v;

shot_name.get_file_name( filename );

s = filename;
s.remove_ending_filename();
s.cat_path( SHOTPARM_CSV );

n = 0;
if ( v.open_for_read(s) )
    {
    v.sa[SHOTPARM_SHOT_NAME_INDEX] = shot_name;

    v.rewind();
    if ( v.get_next_key_match(ONE_KEY_FIELD) )
        {
        dest.rewind();
        for ( i=0; i<MAX_PARMS; i++ )
            {
            dest.next();
            dest = v.ra[SHOTPARM_PARAM_1_INDEX + i];
            n++;
            }
        }

    v.close();
    }

if ( n == 0 )
    {
    dest.rewind();
    for ( i=0; i<MAX_PARMS; i++ )
        {
        dest.next();
        dest = ZeroString;
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