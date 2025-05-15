#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"
#include "..\include\wire_class.h"

/***********************************************************************
*                           OUTPUT_WIRE_CLASS                          *
***********************************************************************/
OUTPUT_WIRE_CLASS::OUTPUT_WIRE_CLASS()
{
type   = NO_WIRE;
number = NO_WIRE;
}

/***********************************************************************
*                           ~OUTPUT_WIRE_CLASS                         *
***********************************************************************/
OUTPUT_WIRE_CLASS::~OUTPUT_WIRE_CLASS()
{
}

/***********************************************************************
*                                ISEMPTY                               *
***********************************************************************/
bool OUTPUT_WIRE_CLASS::isempty()
{
if ( number == NO_WIRE )
    return true;
return false;
}
/***********************************************************************
*                                 ISSET                                *
***********************************************************************/
bool OUTPUT_WIRE_CLASS::isset()
{
if ( number == NO_WIRE )
    return false;
return true;
}

/***********************************************************************
*                                  ++                                  *
***********************************************************************/
void OUTPUT_WIRE_CLASS::operator++(int)
{
if ( number == HIGH_OSW1_WIRE )
    {
    type   = OSW2_WIRE;
    number = LOW_OSW2_WIRE;
    }
else if ( number == HIGH_OSW2_WIRE )
    {
    type   = OSW3_WIRE;
    number = LOW_OSW3_WIRE;
    }
else
    {
    number++;
    }
}

/***********************************************************************
*                              BIT_NUMBER                              *
*                                [0-31]                                *
***********************************************************************/
unsigned OUTPUT_WIRE_CLASS::bit_number()
{
unsigned bn;
bn = NO_WIRE;

switch( type )
    {
    case OSW1_WIRE:
        bn = number - LOW_OSW1_WIRE;
        break;

    case OSW2_WIRE:
        bn = number - LOW_OSW2_WIRE;
        break;

    case OSW3_WIRE:
        bn = number - LOW_OSW3_WIRE;
        break;
    }

return bn;
}

/***********************************************************************
*                                 MASK                                 *
***********************************************************************/
unsigned OUTPUT_WIRE_CLASS::mask()
{
unsigned m;
unsigned bn;

m  = 1;
bn = bit_number();

if ( bn > 0 )
    m <<= bn;

return m;
}

/***********************************************************************
*                                  ==                                  *
***********************************************************************/
bool OUTPUT_WIRE_CLASS::operator==( OUTPUT_WIRE_CLASS & wc )
{
return ( number == wc.number );
}

/***********************************************************************
*                                  !=                                  *
***********************************************************************/
bool OUTPUT_WIRE_CLASS::operator!=( OUTPUT_WIRE_CLASS & wc )
{
return !(number == wc.number);
}

/***********************************************************************
*                                   =                                  *
***********************************************************************/
void OUTPUT_WIRE_CLASS::operator=( OUTPUT_WIRE_CLASS & wc )
{
type   = wc.type;
number = wc.number;
}

/***********************************************************************
*                                  =                                   *
***********************************************************************/
void OUTPUT_WIRE_CLASS::operator=( unsigned new_number )
{
number = new_number;

if  ( LOW_OSW1_WIRE <= number && number <= HIGH_OSW1_WIRE )
    {
    type = OSW1_WIRE;
    }
else if  ( LOW_OSW2_WIRE <= number && number <= HIGH_OSW2_WIRE )
    {
    type = OSW2_WIRE;
    }
else if  ( LOW_OSW3_WIRE <= number && number <= HIGH_OSW3_WIRE )
    {
    type = OSW3_WIRE;
    }
else
    {
    type = NO_WIRE;
    number = NO_WIRE;
    }
}

/***********************************************************************
*                            SET_FROM_MASK                             *
***********************************************************************/
void OUTPUT_WIRE_CLASS::set_from_mask( unsigned new_type, unsigned new_mask )
{
number = wire_from_mask( new_mask );
type   = new_type;
}
