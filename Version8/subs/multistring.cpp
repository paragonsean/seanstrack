#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\multistring.h"

/***********************************************************************
*                           MULTISTRING_CLASS                          *
***********************************************************************/
MULTISTRING_CLASS::MULTISTRING_CLASS()
{
a  = 0;
na = 0;
current_index = NO_INDEX;
}

/***********************************************************************
*                           MULTISTRING_CLASS                          *
***********************************************************************/
MULTISTRING_CLASS::MULTISTRING_CLASS( int new_n )
{
a  = 0;
na = 0;
current_index = NO_INDEX;
set_array_size( new_n );
}

/***********************************************************************
*                           ~MULTISTRING_CLASS                         *
***********************************************************************/
MULTISTRING_CLASS::~MULTISTRING_CLASS()
{
empty_array();
}

/***********************************************************************
*                                REWIND                                *
***********************************************************************/
bool MULTISTRING_CLASS::rewind()
{
if ( current_index >= 0 )
    {
    a[current_index].acquire( *this );
    current_index = NO_INDEX;
    }

return true;
}

/***********************************************************************
*                             EMPTY_ARRAY                              *
***********************************************************************/
bool MULTISTRING_CLASS::empty_array()
{
rewind();

if ( a )
    {
    na = 0;
    delete[] a;
    a = 0;
    }

return true;
}

/***********************************************************************
*                                 COPY                                 *
***********************************************************************/
bool MULTISTRING_CLASS::copy( MULTISTRING_CLASS & sorc )
{
empty_array();
if ( sorc.na > 0 )
    {
    if ( set_array_size(sorc.na) )
        {
        sorc.rewind();
        while ( sorc.next() )
            {
            next();
            STRING_CLASS::operator=( sorc );
            }
        rewind();
        goto_index( sorc.current_index );
        return true;
        }
    }

return false;
}

/***********************************************************************
*                                 FIND                                 *
* Return the index of the first matching string or -1 if not found     *
***********************************************************************/
int MULTISTRING_CLASS::find( STRING_CLASS & s_to_find )
{
int i;

for ( i=0; i<na; i++ )
    {
    if ( a[i] == s_to_find )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                              GOTO_INDEX                              *
***********************************************************************/
bool MULTISTRING_CLASS::goto_index( int dest_index )
{
if ( dest_index < 0 || dest_index >= na )
    return false;

if ( current_index >= 0 )
    a[current_index].acquire( *this );

current_index = dest_index;
this->acquire( a[current_index] );
return true;
}

/***********************************************************************
*                                 NEXT                                 *
***********************************************************************/
bool MULTISTRING_CLASS::next()
{
if ( !a )
    return false;

if ( (current_index+1) == na )
    return false;

if ( current_index >= 0 )
    a[current_index].acquire( *this );

current_index++;

this->acquire( a[current_index] );
return true;
}

/***********************************************************************
*                            SET_ARRAY_SIZE                            *
***********************************************************************/
bool MULTISTRING_CLASS::set_array_size( int new_n )
{
STRING_CLASS * array;
int      i;
int      previous_index;

if ( new_n <= 0 )
    return false;

if ( new_n == na )
    return true;

array = new STRING_CLASS[new_n];
if ( !array )
    return false;

previous_index = current_index;
if ( na > 0 )
    rewind();

i = 0;
while ( i<na && i<new_n )
    {
    array[i].acquire( a[i] );
    i++;
    }
if ( a )
    empty_array();

a  = array;
na = new_n;

if ( previous_index == NO_INDEX )
    {
    if ( !STRING_CLASS::isempty() )
        current_index = 0;
    else
        current_index = NO_INDEX;
    }
else if ( previous_index < na )
    {
    goto_index( previous_index );
    }

return true;
}

/***********************************************************************
*                                SPLIT                                 *
***********************************************************************/
bool MULTISTRING_CLASS::split( const TCHAR separator )
{
STRING_CLASS * array;
int n;
int i;

n = this->countchars( separator );
n++;
if ( n < 1 )
    return false;

array = new STRING_CLASS[n];
if ( !array )
    return false;

i = 0;
while ( this->next_field(separator) )
    {
    array[i] = *this;
    i++;
    }

empty_array();
a = array;
na = n;
return true;
}

/***********************************************************************
*                                 JOIN                                 *
***********************************************************************/
bool MULTISTRING_CLASS::join( TCHAR separator )
{
int i;
int n;

if ( na < 1 )
    return false;

/*
----------------------------------------
Make sure I don't own any of the strings
---------------------------------------- */
rewind();

/*
------------------------------
Calculate how big I have to be
------------------------------ */
n = a[0].len();
i = 1;
while ( i<na )
    {
    n++; /* Separator */
    n += a[i].len();
    i++;
    }

STRING_CLASS::upsize( n );

STRING_CLASS::operator=( a[0] );
i = 1;
while ( i<na )
    {
    STRING_CLASS::operator+=( separator );
    STRING_CLASS::operator+=( a[i] );
    i++;
    }
empty_array();
set_array_size( 1 );
current_index = 0;
return true;
}

/***********************************************************************
*                                 JOIN                                 *
***********************************************************************/
bool MULTISTRING_CLASS::join()
{
TCHAR c = TEXT( ',' );
return join( c );
}

/***********************************************************************
*                                APPEND                                *
***********************************************************************/
bool MULTISTRING_CLASS::append( const TCHAR * sorc )
{
int i;

i = na;
set_array_size( na + 1 );
a[i] = sorc;
goto_index( i );
return true;
}

/***********************************************************************
*                                APPEND                                *
***********************************************************************/
bool MULTISTRING_CLASS::append( const STRING_CLASS & sorc )
{
int i;

i = na;
set_array_size( na + 1 );
a[i] = sorc;
goto_index( i );
return true;
}

/***********************************************************************
*                                INSERT                                *
***********************************************************************/
bool MULTISTRING_CLASS::insert( const TCHAR * sorc )
{
int i;
int previous_index;

previous_index = current_index;
if ( previous_index < 0 )
    previous_index = 0;
rewind();
i = na;
set_array_size( na + 1 );
while ( i > previous_index )
    {
    a[i].acquire( a[i-1] );
    i--;
    }

a[previous_index] = sorc;
goto_index( previous_index );
return true;
}

/***********************************************************************
*                                INSERT                                *
***********************************************************************/
bool MULTISTRING_CLASS::insert( const STRING_CLASS & sorc )
{
int i;
int previous_index;

previous_index = current_index;
if ( previous_index < 0 )
    previous_index = 0;
rewind();
i = na;
set_array_size( na + 1 );
while ( i > previous_index )
    {
    a[i].acquire( a[i-1] );
    i--;
    }

a[previous_index] = sorc;
goto_index( previous_index );
return true;
}

/***********************************************************************
*                                  []                                  *
***********************************************************************/
MULTISTRING_CLASS & MULTISTRING_CLASS::operator[]( int i )
{
goto_index( i );
return *this;
}

/***********************************************************************
*                                REMOVE                                *
***********************************************************************/
bool MULTISTRING_CLASS::remove( int n_to_remove )
{
STRING_CLASS * array;
int            i;
int            new_n;
int            nleft;
int            old_n;
int            old_index;

if ( na < 1 )
    return false;

old_index = current_index;

if ( old_index < 0 )
    old_index = 0;

if ( old_index > na )
    old_index = na - 1;

nleft = na - old_index;
nleft--;
if ( nleft < n_to_remove )
    n_to_remove = nleft;

new_n = na - n_to_remove;
if ( new_n > 0 )
    {
    array = a;
    old_n = na;
    old_index = current_index;
    a = 0;
    na = 0;
    current_index = NO_INDEX;
    set_array_size( new_n );
    rewind();
    for ( i=0; i<old_index; i++ )
        {
        next();
        this->acquire( array[i] );
        }
    i += n_to_remove;
    while ( i < old_n )
        {
        next();
        this->acquire( array[i] );
        i++;
        }
    rewind();
    goto_index( old_index );
    delete[] array;
    }
else
    {
    empty_array();
    }

return true;
}
