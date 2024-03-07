#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\chars.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\structs.h"
#include "..\include\strarray.h"

#include "..\include\param_index_class.h"

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
***********************************************************************/
void PARAM_INDEX_CLASS::init()
{
int i;
for ( i=0; i<MAX_PARMS; i++ )
    x[i] = i;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
***********************************************************************/
PARAM_INDEX_CLASS::PARAM_INDEX_CLASS()
{
init();
}

/***********************************************************************
*                           ~PARAM_INDEX_CLASS                         *
***********************************************************************/
PARAM_INDEX_CLASS::~PARAM_INDEX_CLASS()
{
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                               OPERATOR[]                             *
***********************************************************************/
int & PARAM_INDEX_CLASS::operator[](int i )
{
static int empty_int = 0;

if ( i>=0 && i< MAX_PARMS )
    return x[i];

return empty_int;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                            parameter_number                          *
***********************************************************************/
int PARAM_INDEX_CLASS::parameter_number( int i )
{
if ( i>=0 && i< MAX_PARMS )
    return x[i];

return 0;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                 index                                *
***********************************************************************/
int PARAM_INDEX_CLASS::index( int parameter_number )
{
int i;

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( x[i] == parameter_number )
        return i;
    }

return 0;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  move                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::move( int new_index,  int current_index )
{
int i;
int copy;

if ( new_index<0 || new_index>=MAX_PARMS )
    return false;

if ( current_index<0 || current_index>=MAX_PARMS )
    return false;

copy = x[new_index];

if ( new_index > current_index )
    {
    for ( i=new_index; i>current_index; i-- )
        x[i] = x[i-1];
    }
else
    {
    for ( i=new_index; i<current_index; i++ )
        x[i] = x[i+1];
    }

x[current_index] = copy;

return true;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                 remove                               *
***********************************************************************/
bool PARAM_INDEX_CLASS::remove( int parameter_index_to_remove )
{
int i;
int n;
int sort_index;

if ( parameter_index_to_remove<0 || parameter_index_to_remove>=MAX_PARMS )
    return false;

sort_index = index( parameter_index_to_remove );

/*
---------------------------------------------------------------
I only need to change parameters that are not in standard order
--------------------------------------------------------------- */
n = MAX_PARMS;
while ( n > 0 )
    {
    n--;
    if ( x[n] != n )
        break;
    }

/*
---------------------------------------------------------------
If the parameter that I am removing is in the standard position
and all parameters above are as well, there is nothing to do.
--------------------------------------------------------------- */
if ( n < sort_index )
    return true;

for ( i=sort_index; i<n; i++ )
    x[i] = x[i+1];
x[n] = n;

/*
-----------------------------------------------------------
Parameters above the one being removed are renumbered down.
----------------------------------------------------------- */
for ( i=0; i<n; i++ )
    {
    if ( x[i]>parameter_index_to_remove )
        x[i]--;
    }

return true;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  load                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::load( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
int                i;
STRING_ARRAY_CLASS s;
NAME_CLASS         fn;
FILE_CLASS         f;

init();

fn.get_parameter_sort_order_file_name( co, ma, pa );
if ( fn.file_exists() )
    {
    if ( f.open_for_read(fn) )
        {
        s = f.readline();
        f.close();
        s.split( CommaString );
        s.rewind();
        i = 0;
        while ( s.next() )
            {
            x[i] = s.int_value();
            i++;
            if ( i == MAX_PARMS )
                break;
            }
        }

    }
return false;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  load                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::load( PART_NAME_ENTRY & pn )
{
return load( pn.computer, pn.machine, pn.part );
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  save                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::save( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
const int SLEN = MAX_PARMS * 3;
const int LAST_INDEX = MAX_PARMS - 1;
int          i;
STRING_CLASS s;
NAME_CLASS   fn;
FILE_CLASS   f;

s.init( SLEN );
for ( i=0; i<MAX_PARMS; i++ )
    {
    s += x[i];
    if ( i<LAST_INDEX )
        s += CommaChar;
    }

fn.get_parameter_sort_order_file_name( co, ma, pa );
if ( f.open_for_write(fn) )
    {
    f.writeline(s);
    f.close();
    return true;
    }

return false;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  save                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::save( PART_NAME_ENTRY & pn )
{
return save( pn.computer, pn.machine, pn.part );
}
