#include <windows.h>

#include "..\include\stringcl.h"
#include "..\include\visiparm.h"
#include "..\include\subs.h"

static TCHAR ColonChar = TEXT( ':' );
static TCHAR CrChar    = TEXT( '\r' );
static TCHAR LfChar    = TEXT( '\n' );
static TCHAR NullChar  = TEXT( '\0' );
static TCHAR SpaceChar = TEXT( ' ' );
static TCHAR TabChar   = TEXT( '\t' );
static TCHAR ZeroChar  = TEXT( '0' );


/***********************************************************************
*                             FIX_SHOTNAME                             *
*           If this is an old shot name left pad with zeros.           *
***********************************************************************/
void fix_shotname( STRING_CLASS & s, int shot_number_length )
{
s.remove_leading_chars( SpaceChar );
/*
------------------------------------------------------------
Only old shot names will be less than the shot number length
------------------------------------------------------------ */
if ( s.len() < shot_number_length )
    s.rjust( shot_number_length, ZeroChar );
}

/***********************************************************************
*                             FIX_SHOTNAME                             *
***********************************************************************/
void fix_shotname( STRING_CLASS & s )
{
if ( s.isempty() )
    return;

s.strip();
if ( s.isempty() )
    return;

s.replace( TabChar, NullChar );
if ( s.isempty() )
    return;

if ( s.isnumeric() )
    {
    if ( s.len() < SHOT_LEN )
        s.rjust( SHOT_LEN, ZeroChar );
    }
}

/***********************************************************************
*                             FIX_DDE_NAME                             *
*                                                                      *
*  Dde names are terminated with a cr/lf. If the name is shorter than  *
*  the max name len then the cr/lf may be included as part of the      *
*  name. This strips them, ending the string at the first cr or lf.    *
***********************************************************************/
void fix_dde_name( TCHAR * s )
{

/*
--------------------------------------------------------
A carriage returns or line feed is the end of the string
-------------------------------------------------------- */
while ( *s != NullChar )
    {
    if ( *s == CrChar || *s == LfChar )
        {
        *s = NullChar;
        break;
        }
    s++;
    }
}

