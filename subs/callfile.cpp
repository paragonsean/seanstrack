#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\chars.h"
#include "..\include\subs.h"

static BOOL    OVERWRITE_EXISTING  = FALSE;

/***********************************************************************
*                            COPY_ALL_FILES                            *
***********************************************************************/
BOOLEAN copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, TCHAR * wildname, BOOLEAN with_subs, HWND w, int id )
{

TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];

int dest_len;
int sorc_len;

WIN32_FIND_DATA fdata;
HANDLE          fh;

/*
-------------------------------------------------------
Copy the sorc and dest dirs and get the length of each.
------------------------------------------------------- */
lstrcpy( sorc, sorc_dir );
lstrcpy( dest, dest_dir );

append_backslash_to_path( sorc );
append_backslash_to_path( dest );

sorc_len = lstrlen( sorc );
dest_len = lstrlen( dest );

lstrcat( sorc, wildname );

fh = FindFirstFile( sorc, &fdata );

/*
---------------------------------------------------------------
If the handle is invalid, I assume (for now) there are no files
--------------------------------------------------------------- */
if ( fh == INVALID_HANDLE_VALUE )
    return TRUE;

while ( TRUE )
    {
    if ( *fdata.cFileName != PeriodChar )
        {
        lstrcpy( sorc+sorc_len, fdata.cFileName );
        lstrcpy( dest+dest_len, fdata.cFileName );

        if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
            if ( with_subs )
                {
                if ( !directory_exists(dest) )
                    create_directory( dest );
                copy_all_files( dest, sorc, wildname,  with_subs, w, id );
                }
            }
        else
            {
            if ( w )
                SetDlgItemText( w, id, fdata.cFileName );
            CopyFile( sorc, dest, OVERWRITE_EXISTING );
            }
        }

    if ( !FindNextFile(fh, &fdata) )
        break;
    }

FindClose( fh );
return TRUE;
}

/***********************************************************************
*                            COPY_ALL_FILES                            *
***********************************************************************/
BOOLEAN copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, BOOLEAN with_subs, HWND w, int id )
{
return copy_all_files( dest_dir, sorc_dir, StarDotStar, with_subs, w, id );
}

/***********************************************************************
*                            COPY_ALL_FILES                            *
***********************************************************************/
BOOLEAN copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir )
{
return copy_all_files( dest_dir, sorc_dir, StarDotStar, WITH_SUBS, 0, 0 );
}
