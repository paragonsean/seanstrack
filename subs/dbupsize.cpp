#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"

/***********************************************************************
*                       UPSIZE_TABLE_W_SHOT_NAME                       *
*                                                                      *
* This assumes you are upsizing a graphlst or shotparm table. The new  *
* table is assumed to have an ascii shot name. The shot name is        *
* assumed to be the first field.                                       *
* This assumes you have loaded chars.h in the main program.                                                                     *
***********************************************************************/
BOOLEAN upsize_table_w_shot_name( NAME_CLASS & sn, int new_shot_name_length )
{
const TCHAR  DelimiterChar = TEXT( ';' );
FILE_CLASS sorc;
FILE_CLASS dest;
NAME_CLASS dn;
int        difference;
int        new_record_length;
int        old_record_length;
int        old_shot_name_length;
STRING_CLASS s;
STRING_CLASS d;
TCHAR * bp;
TCHAR * cp;
bool    was_alpha;
bool    aborting;

dn = sn;
dn.remove_ending_filename();
dn.cat_path( TEMP_GRAPHLST_DB );

if ( !sorc.open_for_read( sn ) )
    return FALSE;

if ( !dest.open_for_write(dn) )
    {
    sorc.close();
    return FALSE;
    }

while ( true )
    {
    s = sorc.readline();
    if ( s.isempty() )
        break;
    old_record_length = s.len();
    /*
    ----------------------------------
    Get the shot name ready to extract
    ---------------------------------- */
    bp = s.text();
    was_alpha = false;
    if ( *bp == DoubleQuoteChar )
        {
        was_alpha = true;
        bp++;
        }
    cp = s.find( DelimiterChar );
    *cp = NullChar;
    if ( was_alpha )
        {
        *(cp-1) = NullChar;
        }
    else
        {
        s.replace_all( SpaceChar, ZeroChar );
        }


    /*
    ------------------------------------------------------------
    Get the difference between the new and old shot name lengths
    ------------------------------------------------------------ */
    old_shot_name_length = lstrlen( bp );
    difference = new_shot_name_length - old_shot_name_length;
    if ( difference < 0 )
        {
        aborting = true;
        break;
        }

    /*
    -------------------------------
    Calculate the new record length
    ------------------------------- */
    new_record_length = old_record_length + difference;
    if ( !was_alpha )
        new_record_length += 2;

    d.upsize( new_record_length );
    d = DoubleQuoteChar;
    while ( difference )
        {
        d += SpaceChar;
        difference--;
        }
    d += bp;
    d += DoubleQuoteChar;
    *cp = DelimiterChar;
    d += cp;
    dest.writeline( d );
    }

sorc.close();
dest.close();

if ( aborting )
    {
    dn.delete_file();
    }
else
    {
    sn.delete_file();
    dn.moveto( sn );
    }

return TRUE;
}