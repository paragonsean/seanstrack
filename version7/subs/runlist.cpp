#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\runlist.h"

static const TCHAR CommaChar    = TEXT( ',' );
static const TCHAR NullChar     = TEXT( '\0' );
static const TCHAR NChar        = TEXT( 'N' );
static const TCHAR LowerNChar   = TEXT( 'n' );
static       TCHAR ZeroString[] = TEXT( "0" );

static RUNLIST_ENTRY Empty;

/***********************************************************************
*                           RUNLIST_CLASS                              *
*                              cleanup                                 *
***********************************************************************/
void RUNLIST_CLASS::cleanup()
{
if ( list )
    {
    delete[] list;
    list = 0;
    }

n    = 0;
mask = 0;
}

/***********************************************************************
*                           RUNLIST_CLASS                              *
***********************************************************************/
RUNLIST_CLASS::RUNLIST_CLASS( const RUNLIST_CLASS & sorc )
{
list = 0;
n    = 0;

if ( sorc.n > 0 )
    {
    list = new RUNLIST_ENTRY[sorc.n];
    if ( list )
        {
        while ( n < sorc.n )
            {
            list[n].bits = sorc.list[n].bits;
            list[n].partname = sorc.list[n].partname;
            n++;
            }
        }
    }

mask = sorc.mask;
}

/***********************************************************************
*                          ~RUNLIST_CLASS                              *
***********************************************************************/
RUNLIST_CLASS::~RUNLIST_CLASS()
{
cleanup();
}

/***********************************************************************
*                           RUNLIST_CLASS                              *
*                                 []                                   *
***********************************************************************/
RUNLIST_ENTRY & RUNLIST_CLASS::operator[]( int i )
{
if ( i >= 0 && i < n )
    return list[i];

return Empty;
}

/***********************************************************************
*                            RUNLIST_CLASS                             *
*                          current_part_name                           *
***********************************************************************/
STRING_CLASS & RUNLIST_CLASS::current_part_name()
{
if ( n > 0 )
    return list[0].partname;
else
    return Empty.partname;
}

/***********************************************************************
*                            RUNLIST_CLASS                             *
*                              find_part                               *
***********************************************************************/
STRING_CLASS & RUNLIST_CLASS::find_part( unsigned int suretrak_bits )
{
int i;
unsigned int x;

if ( n < 1 )
    return Empty.partname;

x = (unsigned int) suretrak_bits;
x &= mask;

for ( i=0; i<n; i++ )
    {
    if ( list[i].bits == x )
        return list[i].partname;
    }

/*
---------------------------------------------------------------------
The zeroth entry is the current part. Return this if nothing matches.
--------------------------------------------------------------------- */
return list[0].partname;
}

/***********************************************************************
*                            RUNLIST_CLASS                             *
*                                GET                                   *
***********************************************************************/
BOOLEAN RUNLIST_CLASS::get( NAME_CLASS & fname )
{
FILE_CLASS   f;
STRING_CLASS s;
TCHAR      * cp;
int          nof_recs;

cleanup();

if ( fname.file_exists() )
    {
    if ( f.open_for_read(fname.text()) )
        {
        nof_recs = f.nof_lines();
        if ( nof_recs > 0 )
            {
            list = new RUNLIST_ENTRY[nof_recs];
            if ( list )
                {
                while ( true )
                    {
                    if ( n >= nof_recs )
                        break;
                    s = f.readline();
                    if ( s.isempty() )
                        break;
                    cp = s.find( CommaChar );
                    if ( cp )
                        {
                        *cp = NullChar;
                        cp++;
                        }
                    list[n].bits = hextoul( s.text() );
                    mask |= list[n].bits;
                    if ( cp )
                        list[n].partname = cp;
                    n++;
                    }
                }
            }
        f.close();
        }
    }

if ( list && n < 1 )
    cleanup();

if ( n > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            RUNLIST_CLASS                             *
*                                GET                                   *
***********************************************************************/
BOOLEAN RUNLIST_CLASS::get( STRING_CLASS & co, STRING_CLASS & ma )
{
NAME_CLASS s;
s.get_runlist_csvname( co, ma );
return get( s );
}
