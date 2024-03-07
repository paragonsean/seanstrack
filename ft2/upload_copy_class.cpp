#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"

#include "upload_copy_class.h"

/***********************************************************************
*                            UPLOAD_COPY_CLASS                         *
*                                  init                                *
***********************************************************************/
void UPLOAD_COPY_CLASS::init()
{
int i;
for ( i=0; i<NOF_FT2_VARIABLES; i++ )
    a[i].null();
have_change = false;
}

/***********************************************************************
*                            UPLOAD_COPY_CLASS                         *
***********************************************************************/
UPLOAD_COPY_CLASS::UPLOAD_COPY_CLASS()
{
have_change = false;
}

/***********************************************************************
*                           ~UPLOAD_COPY_CLASS                         *
***********************************************************************/
UPLOAD_COPY_CLASS::~UPLOAD_COPY_CLASS()
{
}

/***********************************************************************
*                            UPLOAD_COPY_CLASS                         *
*                               is_changed                             *
***********************************************************************/
bool UPLOAD_COPY_CLASS::is_changed()
{
return have_change;
}

/***********************************************************************
*                            UPLOAD_COPY_CLASS                         *
*                                   []                                 *
***********************************************************************/
STRING_CLASS & UPLOAD_COPY_CLASS::operator[](int i )
{
static STRING_CLASS empty_string;

if ( i>=0 && i< NOF_FT2_VARIABLES )
    return a[i];

return empty_string;
}

/***********************************************************************
*                            UPLOAD_COPY_CLASS                         *
*                                  load                                *
* This returns the time of the last write the file (low 31 bits only). *
* If no file was opened it returns 0.                                  *
***********************************************************************/
unsigned UPLOAD_COPY_CLASS::load( STRING_CLASS co, STRING_CLASS ma, STRING_CLASS pa )
{
NAME_CLASS s;
FILE_CLASS f;
TCHAR    * cp;
int        i;
FILETIME   ft;
unsigned   wt;

init();
have_change = false;
wt = 0;

s.get_upload_copy_file_name( co, ma, pa );
if ( f.open_for_read(s) )
    {
    while ( true )
        {
        s = f.readline();
        if ( s.isempty() )
            break;

        cp = s.find( CommaChar );
        if ( cp )
            {
            *cp = NullChar;
            i = s.int_value();
            cp++;
            a[i] = cp;
            }
        }
    if ( f.get_creation_time(&ft) )
        {
        wt = (unsigned) ft.dwLowDateTime;
        wt &= 0x7FFFFFFF;
        }
    f.close();
    }

return wt;
}

/***********************************************************************
*                            UPLOAD_COPY_CLASS                         *
*                                  save                                *
* This returns the time of the last write the file (low 31 bits only). *
* If no file was opened it returns 0.                                  *
***********************************************************************/
unsigned UPLOAD_COPY_CLASS::save( STRING_CLASS co, STRING_CLASS ma, STRING_CLASS pa )
{
NAME_CLASS   s;
FILE_CLASS   f;
int          i;
FILETIME     ft;
unsigned     wt;

wt = 0;
s.get_upload_copy_file_name( co, ma, pa );
if ( f.open_for_write(s) )
    {
    for ( i=0; i<NOF_FT2_VARIABLES; i++ )
        {
        if ( !a[i].isempty() )
            {
            s = i;
            s += CommaChar;
            s += a[i];
            f.writeline(s);
            }
        }
    f.close();

    have_change = false;

    s.get_upload_copy_file_name( co, ma, pa );
    if ( f.open_for_read(s) )
        {
        if ( f.get_creation_time(&ft) )
            {
            wt = (unsigned) ft.dwLowDateTime;
            wt &= 0x7FFFFFFF;
            }
        f.close();
        }
    }

return wt;
}

/***********************************************************************
*                            UPLOAD_COPY_CLASS                         *
*                                 isnew                                *
***********************************************************************/
bool UPLOAD_COPY_CLASS::isnew( STRING_CLASS & sorc )
{
STRING_CLASS s;
TCHAR      * cp;
TCHAR      * ep;
int          i;

s = sorc;
if ( s.isempty() )
    return false;

cp = s.text();
if ( *cp != VChar )
    return false;
remove_char( cp );
ep = s.find( EqualsChar );
if ( !ep )
    return false;

*ep = NullChar;
i = s.int_value();
if ( i >= 0 && i < NOF_FT2_VARIABLES )
    {
    ep++;
    if ( a[i] != ep )
        {
        have_change = true;
        a[i] = ep;
        return true;
        }
    }

return false;
}
