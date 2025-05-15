#include <windows.h>
#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"
#include "..\include\setpoint.h"

static SETPOINT_ENTRY empty_entry;

static TCHAR CommaChar = TEXT( ','  );
static TCHAR NullChar  = TEXT( '\0' );

static TCHAR CommaString[] = TEXT( "," );

/***********************************************************************
*                      SETPOINT_ENTRY::OPERATOR=                       *
***********************************************************************/
void SETPOINT_ENTRY::operator=( const SETPOINT_ENTRY & sorc )
{
value = sorc.value;
desc  = sorc.desc;
}

/***********************************************************************
*                             empty                                    *
***********************************************************************/
void SETPOINT_CLASS::empty()
{
int i;
for ( i=0; i<n; i++ )
    {
    array[i].value.null();
    array[i].desc.null();
    }
}

/***********************************************************************
*                            cleanup                                   *
***********************************************************************/
void SETPOINT_CLASS::cleanup()
{
SETPOINT_ENTRY * copy;

if ( array )
    {
    copy  = array;
    n     = 0;
    array = 0;
    delete[] copy;
    }
}

/***********************************************************************
*                           SETPOINT_CLASS                             *
***********************************************************************/
SETPOINT_CLASS::SETPOINT_CLASS()
{
array = 0;
n     = 0;
}

/***********************************************************************
*                          ~SETPOINT_CLASS                             *
***********************************************************************/
SETPOINT_CLASS::~SETPOINT_CLASS()
{
cleanup();
}

/***********************************************************************
*                             OPERATOR=                                *
***********************************************************************/
void SETPOINT_CLASS::operator=( const SETPOINT_CLASS & sorc )
{
int i;

if ( setsize(sorc.n) )
    {
    for ( i=0; i<n; i++ )
        {
        array[i].value = sorc.array[i].value;
        array[i].desc  = sorc.array[i].desc;
        }
    }
}

/***********************************************************************
*                             SETSIZE                                  *
***********************************************************************/
BOOLEAN SETPOINT_CLASS::setsize( int new_n )
{
int              i;
SETPOINT_ENTRY * se;

if ( n == new_n )
    return TRUE;

if ( new_n <= 0 )
    {
    cleanup();
    return TRUE;
    }

se = new SETPOINT_ENTRY[new_n];
if ( se )
    {
    for ( i=0; i<n; i++ )
        {
        if ( i < new_n )
            {
            if ( !array[i].desc.isempty() )
                se[i].desc = array[i].desc;

            if ( !array[i].value.isempty() )
                se[i].value = array[i].value;
            }
        }

    cleanup();
    array = se;
    n = new_n;
    }

return n;
}

/***********************************************************************
*                              REMOVE                                  *
***********************************************************************/
BOOLEAN SETPOINT_CLASS::remove( int index_to_remove )
{
int              i;
int              j;
int              new_n;
SETPOINT_ENTRY * se;

if ( index_to_remove < 0 || index_to_remove >= n )
    return FALSE;

if ( n <= 1 )
    {
    cleanup();
    return TRUE;
    }

new_n = n;
new_n--;
se = new SETPOINT_ENTRY[new_n];
j = 0;
if ( se )
    {
    for ( i=0; i<n; i++ )
        {
        if ( j < new_n )
            {
            if ( i != index_to_remove )
                {
                se[j].value = array[i].value;
                se[j].desc  = array[i].desc;
                j++;
                }
            }
        }

    cleanup();
    array = se;
    n = new_n;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               GETFROM                                *
* I've changed this so I don't have to have descriptions. If there     *
* is no comma then I just read the value from the line and null the    *
* description (v7).                                                    *
* If the setpoint class already has enough entrys it will not be       *
* reallocated. This is a new function. Get still works as it did and   *
* always clears out the old data.                                      *
***********************************************************************/
int SETPOINT_CLASS::getfrom( STRING_CLASS & path )
{
FILE_CLASS f;
int        i;
TCHAR    * cp;
TCHAR    * s;

if ( f.open_for_read(path) )
    {
    i = f.nof_lines();
    if ( i > 0 )
        {
        if ( i > n )
            setsize( i );
        i = 0;
        while ( i < n )
            {
            s = f.readline();
            if ( !s )
                break;

            cp = findchar( CommaChar, s );

            if ( cp )
                {
                *cp = NullChar;
                array[i].desc = s;

                cp++;
                array[i].value = cp;

                i++;
                }
            else
                {
                array[i].desc.null();
                array[i].value = s;
                if ( !array[i].value.isempty() )
                    i++;
                }
            }
        n = i;
        }
    f.close();
    }

return n;
}

/***********************************************************************
*                                 GET                                  *
***********************************************************************/
int SETPOINT_CLASS::get( STRING_CLASS & path )
{
cleanup();

return getfrom( path );
}

/***********************************************************************
*                            GET_ONE_VALUE                             *
* This allows you to read one value from the file without allocating   *
* any extra strings. dest and path can be the same STRING_CLASS.       *
***********************************************************************/
BOOLEAN SETPOINT_CLASS::get_one_value( STRING_CLASS & dest, STRING_CLASS & path, int index_to_get )
{
FILE_CLASS       f;
int              i;
TCHAR          * cp;
TCHAR          * s;
BOOLEAN          status;

status = FALSE;
if ( f.open_for_read(path) )
    {
    i = 0;
    while ( true )
        {
        s = f.readline();
        if ( !s )
            break;

        if ( i == index_to_get )
            {
            cp = findchar( CommaChar, s );
            if ( cp )
                cp++;
            else
                cp = s;
            dest = cp;
            status = TRUE;
            break;
            }
        i++;
        }
    f.close();
    }

return status;
}

/***********************************************************************
*                                  []                                  *
***********************************************************************/
SETPOINT_ENTRY & SETPOINT_CLASS::operator[]( int i )
{
if ( i<0 || i >= n )
    return empty_entry;

return array[i];
}

/***********************************************************************
*                                  []                                  *
***********************************************************************/
SETPOINT_ENTRY & SETPOINT_CLASS::operator[]( STRING_CLASS & sorc )
{
int i;
for (i=0; i<n; i++ )
    {
    if ( array[i].desc == sorc )
        return array[i];
    }

return empty_entry;
}

/***********************************************************************
*                                  []                                  *
***********************************************************************/
SETPOINT_ENTRY & SETPOINT_CLASS::operator[]( const TCHAR * sorc )
{
int i;
if ( sorc )
    {
    for (i=0; i<n; i++ )
        {
        if ( array[i].desc == sorc )
            return array[i];
        }
    }

return empty_entry;
}

/***********************************************************************
*                                 PUT                                  *
***********************************************************************/
BOOLEAN SETPOINT_CLASS::put( STRING_CLASS & path )
{
FILE_CLASS   f;
STRING_CLASS s;
int i;

s.init( 50 );
if ( f.open_for_write(path.text()) )
    {
    for( i=0; i<n; i++ )
        {
        if ( array[i].desc.isempty() )
            {
            s = array[i].value;
            }
        else
            {
            s = array[i].desc;
            s += CommaString;
            s += array[i].value;
            }
        f.writeline( s.text() );
        }
    f.close();
    return TRUE;
    }

return FALSE;
}
