#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\fileclas.h"
#include "..\include\multistring.h"
#include "..\include\subs.h"
#include "..\include\vdbclass.h"

/***********************************************************************
*                                db_to_vdb                             *
* Convert a fixed length semicolon delimited data base file to a       *
* variable length comma delimited database.                            *
************************************************************************/
BOOLEAN db_to_vdb( NAME_CLASS & sn, int shot_name_index )
{
const TCHAR  DelimiterChar = TEXT( ';' );
FILE_CLASS sorc;
VDB_CLASS  dest;
NAME_CLASS dn;
int        i;
int        n;
BOOLEAN    was_alpha;

dn = sn;
dn.replace_extension( CSV_SUFFIX );

if ( !sorc.open_for_read( sn ) )
    return FALSE;

if ( !dest.open_for_write(dn) )
    {
    sorc.close();
    return FALSE;
    }

while ( true )
    {
    dest.ra = sorc.readline();
    if ( dest.ra.isempty() )
        break;

    dest.ra.split( SemiColonChar );
    n = dest.ra.count();
    if ( n < 1 )
        continue;
    dest.set_nof_cols( n );
    dest.ra.rewind();
    for ( i=0; i<n; i++ )
        {
        dest.ra.next();
        was_alpha = dest.ra.strip_double_quotes();
        dest.ra.strip();
        /*
        ------------------------------------------------------------------------
        If there is a shot name field, check to see if it has quotes arround it.
        If not then this is a numeric field and I have to expand it to a 6 
        character field by padding it with zeros on the left.
        ------------------------------------------------------------------------ */
        if ( i == shot_name_index )
            {
            if ( !was_alpha )
                dest.ra.rjust( SHOT_LEN, ZeroChar );
            }
        }
    dest.rec_append();
    }

sorc.close();
dest.close();

sn.delete_file();
return TRUE;
}
