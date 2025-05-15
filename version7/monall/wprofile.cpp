#pragma warning( disable : 4018 )   // No warning on signed/unsigned compare

#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "extern.h"
#include "shotsave.h"

/***********************************************************************
*                            WRITE_PROFILE                             *
***********************************************************************/
BOOLEAN write_profile( SHOT_BUFFER_ENTRY * b, BITSETYPE save_flags )
{
int     i;
NAME_CLASS s;
HANDLE  fh;
DWORD   access_mode;
DWORD   share_mode;
DWORD   bytes_written;
DWORD   bytes_to_write;
BOOL    status;
SYSTEMTIME shot_time;
DB_TABLE   t;
int32      n;

status = FALSE;

if ( !FileTimeToSystemTime( &b->ph.time_of_shot, &shot_time ) )
    MessageBox( NULL, TEXT("Bad Convert"), TEXT("FileTimeToSystemTime"), MB_OK );

s.get_profile_name( ComputerName, b->machine, b->part, b->ph.shot_number );

access_mode = GENERIC_WRITE;
share_mode  = 0;

fh = CreateFile( s.text(), access_mode, share_mode, 0, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh != INVALID_HANDLE_VALUE )
    {
    status = WriteFile( fh, &b->ph, sizeof(PROFILE_HEADER), &bytes_written, 0 );
    if ( status )
        status = WriteFile( fh, &b->nof_parameters, sizeof(short), &bytes_written, 0 );

    if ( status && b->nof_parameters > 0 )
        status = WriteFile( fh, b->parameters, b->nof_parameters*sizeof(float), &bytes_written, 0 );

    if ( status )
        {
        bytes_to_write = b->ph.n * sizeof( unsigned short );

        for ( i=0; i<MAX_CURVES; i++ )
            {
            if ( b->ph.curves[i].type == NO_CURVE_TYPE )
                break;

            status = WriteFile( fh, b->points[i], bytes_to_write, &bytes_written, 0 );
            if ( !status )
                break;
            }
        }
    CloseHandle( fh );
    }

if ( status )
    {
    /*
    ---------------------
    Update the graph list
    --------------------- */
    s.get_graphlst_dbname( ComputerName, b->machine, b->part );
    if ( t.open(s, GRAPHLST_RECLEN, PWL) )
        {
        t.put_long(  GRAPHLST_SHOT_NUMBER_OFFSET, b->ph.shot_number, SHOT_LEN );
        t.put_date(  GRAPHLST_DATE_OFFSET, shot_time );
        t.put_time(  GRAPHLST_TIME_OFFSET, shot_time );
        t.put_short( GRAPHLST_SAVE_FLAGS_OFFSET, (short) save_flags, GRAPHLST_SAVE_FLAGS_LEN );
        t.rec_append();

        /*
        ----------------------------------------------------------------------
        Check back 51 shots and see if I should delete the record and profile.
        ---------------------------------------------------------------------- */
        n = t.nof_recs();
        if ( n > HistoryShotCount )
            {
            n -= HistoryShotCount;
            /*
            --------------------------------------------------
            Subtract one more because the last record is n - 1
            -------------------------------------------------- */
            n--;

            t.goto_record_number(n);
            t.get_rec();

            /*
            ------------------------------------
            See if I need to delete this profile
            ------------------------------------ */
            save_flags = (BITSETYPE) t.get_short( GRAPHLST_SAVE_FLAGS_OFFSET );
            if ( !(save_flags & SAVE_PROFILE_DATA) )
                {
                n = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
                t.rec_delete();

                s.get_profile_name( ComputerName, b->machine, b->part, n );
                s.delete_file();
                }

            n = max_profiles_saved( b->machine );
            if ( n > 0 && n < (int32) t.nof_recs() )
                {
                t.goto_record_number(0);
                t.get_rec();
                n = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
                t.rec_delete();

                s.get_profile_name( ComputerName, b->machine, b->part, n );
                s.delete_file();
                }

            }

        t.close();
        }
    }

return status;
}
