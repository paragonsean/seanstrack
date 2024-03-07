#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

#include "fixaxis.h"

/***********************************************************************
*                          FIXED_AXIS_ENTRY                            *
*                               WRITE                                  *
***********************************************************************/
BOOLEAN FIXED_AXIS_ENTRY::write( DB_TABLE & dest )
{
dest.put_boolean( FIXAXIS_ENABLED_OFFSET, enabled );
dest.put_float( FIXAXIS_MIN_OFFSET, min, FIXAXIS_FLOAT_LEN );
dest.put_float( FIXAXIS_MAX_OFFSET, max, FIXAXIS_FLOAT_LEN );
return TRUE;
}

/***********************************************************************
*                          FIXED_AXIS_ENTRY                            *
*                                READ                                  *
***********************************************************************/
BOOLEAN FIXED_AXIS_ENTRY::read( DB_TABLE & sorc )
{
enabled = sorc.get_boolean( FIXAXIS_ENABLED_OFFSET );
min     = sorc.get_float(   FIXAXIS_MIN_OFFSET );
max     = sorc.get_float(   FIXAXIS_MAX_OFFSET );

return TRUE;
}

/***********************************************************************
*                           FIXED_AXIS_ENTRY                           *
*                                  =                                   *
***********************************************************************/
void FIXED_AXIS_ENTRY::operator=( const FIXED_AXIS_ENTRY & sorc )
{
enabled = sorc.enabled;
min     = sorc.min;
max     = sorc.max;
}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                EMPTY                                 *
***********************************************************************/
void FIXED_AXIS_CLASS::empty()
{
n = 0;

if ( f )
    {
    delete[] f;
    f = 0;
    }
}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                               setsize                                *
***********************************************************************/
BOOLEAN FIXED_AXIS_CLASS::setsize( int32 nof_entries )
{
FIXED_AXIS_ENTRY * fe;
int32              nmin;
int32              i;

if ( nof_entries > 0 )
    {
    fe = new FIXED_AXIS_ENTRY[nof_entries];
    if ( fe )
        {
        nmin = nof_entries;
        if ( n < nmin )
            nmin = n;
        if ( nmin > 0 && f )
            {
            for ( i=0; i<nmin; i++ )
                fe[i] = f[i];
            empty();
            }
        n = nof_entries;
        f = fe;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                 INIT                                 *
***********************************************************************/
BOOLEAN FIXED_AXIS_CLASS::init( int32 nof_entries )
{
empty();
return setsize( nof_entries );
}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                  []                                  *
***********************************************************************/
FIXED_AXIS_ENTRY & FIXED_AXIS_CLASS::operator[]( int32 i )
{
static FIXED_AXIS_ENTRY dummy;

if ( i >= 0 && i < n )
    return f[i];

return dummy;
}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                 READ                                 *
***********************************************************************/
BOOLEAN FIXED_AXIS_CLASS::read( TCHAR * filename )
{
int32    i;
int32    nr;
DB_TABLE t;
BOOLEAN  status;

empty();
status = FALSE;

if ( file_exists(filename) )
    {
    if ( t.open(filename, FIXAXIS_RECLEN, PFL) )
        {
        nr = (int32) t.nof_recs();
        if ( nr > 0 )
            {
            if ( init(nr) )
                {
                i = 0;
                while ( t.get_next_record(NO_LOCK) && i < nr )
                    {
                    f[i].read( t );
                    i++;
                    }
                status = TRUE;
                }
            }
        t.close();
        }
    }

return status;
}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                WRITE                                 *
***********************************************************************/
BOOLEAN FIXED_AXIS_CLASS::write( TCHAR * filename )
{
int32 i;
DB_TABLE t;

if ( !file_exists(filename ) )
    t.create( filename );

if ( t.open(filename, FIXAXIS_RECLEN, FL) )
    {
    t.empty();
    for ( i=0; i<n; i++ )
        {
        f[i].write( t );
        t.rec_append();
        }

    t.close();
    return TRUE;
    }

return FALSE;
}
