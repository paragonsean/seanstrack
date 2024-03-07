#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"

#include "..\include\chars.h"

/***********************************************************************
*                        COUNT_SHOTS_IN_ONE_DIR                        *
***********************************************************************/
void count_shots_in_one_dir( int & count, STRING_CLASS & sorcdir )
{
TCHAR         * cp;
STRING_CLASS    d;
STRING_CLASS    s;
HANDLE          fh;
WIN32_FIND_DATA fdata;

s = sorcdir;
s.cat_path( StarDotStar );

fh = FindFirstFile( s.text(), &fdata );
if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( true )
        {
        cp = fdata.cFileName;
        if ( *cp != PeriodChar )
            {
            if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                d = sorcdir;
                d.cat_path( cp );
                count_shots_in_one_dir( count, d );
                }
            else
                {
                /*
                ---------------------------
                See if this is a shot file.
                --------------------------- */
                while ( *cp != PeriodChar && *cp != NullChar )
                    cp++;
                if ( *cp != NullChar )
                    {
                    cp++;
                    if ( *cp == PChar || *cp == PCharLow )
                        {
                        cp++;
                        if ( *cp == RChar || *cp == RCharLow )
                            {
                            cp++;
                            if ( *cp == TwoChar || *cp == OChar || *cp == OCharLow )
                                count++;
                            }
                        }
                    }
                }
            }
        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );
    }
}

/***********************************************************************
*                              SHOT_COUNT                              *
* Count the number of shot files in this directory and all subdirs.    *
***********************************************************************/
int shot_count( STRING_CLASS & sorcdir )
{
int count;

count = 0;
count_shots_in_one_dir( count, sorcdir );

return count;
}
