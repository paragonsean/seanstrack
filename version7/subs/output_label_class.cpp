#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\output_label_class.h"
#include "..\include\subs.h"

static STRING_CLASS my_empty_string;

/***********************************************************************
*                          OUTPUT_LABEL_CLASS                          *
***********************************************************************/
OUTPUT_LABEL_CLASS::OUTPUT_LABEL_CLASS()
{
int i;

has_been_initialized = FALSE;
for ( i=0; i<HIGH_FT2_OUTPUT_WIRE; i++ )
    {
    wires[i] = i+1;
    labels[i] = wires[i];
    }
}

/***********************************************************************
*                                 INIT                                 *
* The file is c:\v5\data\output_wire_labels.csv. There should be 32    *
* lines like                                                           *
* Output Wire 1,1                                                      *
* ...                                                                  *
* Output Wire 32,62B                                                   *
* Only the part after the comma is currently used                      *
***********************************************************************/
BOOLEAN OUTPUT_LABEL_CLASS::init()
{
int        i;
FILE_CLASS f;
NAME_CLASS s;

s.get_output_wire_labels_csvname();
if ( f.open_for_read(s) )
    {
    for ( i=0; i<HIGH_FT2_OUTPUT_WIRE; i++ )
        {
        s = f.readline();
        s.next_field();
        s.next_field();
        s.strip();
        labels[i] = s;
        }
    f.close();
    has_been_initialized = TRUE;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                              WIRE_NUMBER                             *
***********************************************************************/
int OUTPUT_LABEL_CLASS::wire_number( STRING_CLASS & label_to_find )
{
int i;

for ( i=0; i<HIGH_FT2_OUTPUT_WIRE; i++ )
    {
    if ( label_to_find == labels[i] )
        return i+1;
    }

i = label_to_find.int_value();
if ( 0<i && i<HIGH_FT2_OUTPUT_WIRE )
    return i;

return NO_WIRE;
}

/***********************************************************************
*                                  WIRE                                *
***********************************************************************/
STRING_CLASS & OUTPUT_LABEL_CLASS::wire( STRING_CLASS & label_to_find )
{
int i;

for ( i=0; i<HIGH_FT2_OUTPUT_WIRE; i++ )
    {
    if ( label_to_find == labels[i] )
        return wires[i];
    }
return my_empty_string;;
}

/***********************************************************************
*                                 LABEL                                *
***********************************************************************/
STRING_CLASS & OUTPUT_LABEL_CLASS::label( STRING_CLASS & wire_to_find )
{
int i;

for ( i=0; i<HIGH_FT2_OUTPUT_WIRE; i++ )
    {
    if ( wire_to_find == wires[i] )
        return labels[i];
    }

return my_empty_string;;
}

/***********************************************************************
*                                  []                                  *
***********************************************************************/
STRING_CLASS & OUTPUT_LABEL_CLASS::operator[]( int i )
{
if ( 0 <= i && i < HIGH_FT2_OUTPUT_WIRE )
    return labels[i];

return my_empty_string;;
}
