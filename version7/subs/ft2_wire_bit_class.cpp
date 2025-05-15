#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\ft2_wire_bit_class.h"

/***********************************************************************
*                           FT2_OSW_BIT_CLASS                          *
***********************************************************************/
FT2_OSW_BIT_CLASS::FT2_OSW_BIT_CLASS()
{
bits[OSW1] = 0;
bits[OSW2] = 0;
bits[OSW3] = 0;
}

/***********************************************************************
*                           FT2_OSW_BIT_CLASS                          *
***********************************************************************/
FT2_OSW_BIT_CLASS::FT2_OSW_BIT_CLASS( unsigned a, unsigned b, unsigned c )
{
bits[OSW1] = a;
bits[OSW2] = b;
bits[OSW3] = c;
}

/***********************************************************************
*                           ~FT2_OSW_BIT_CLASS                         *
***********************************************************************/
FT2_OSW_BIT_CLASS::~FT2_OSW_BIT_CLASS()
{
}

/***********************************************************************
*                                CLEAR                                 *
***********************************************************************/
void FT2_OSW_BIT_CLASS::clear( OUTPUT_WIRE_CLASS & wire_to_clear )
{
switch( wire_to_clear.type )
    {
    case OSW1_WIRE:
        bits[OSW1] &= ~wire_to_clear.mask();
        break;
    case OSW2_WIRE:
        bits[OSW2] &= ~wire_to_clear.mask();
        break;
    case OSW3_WIRE:
        bits[OSW3] &= ~wire_to_clear.mask();
        break;
    }
}
/***********************************************************************
*                                CLEAR                                 *
***********************************************************************/
void FT2_OSW_BIT_CLASS::clear( unsigned wire_number_to_clear )
{
OUTPUT_WIRE_CLASS w;
w = wire_number_to_clear;
clear( w );
}

/***********************************************************************
*                                CLEAR                                 *
*                    Clear bits that are set in sorc                   *
***********************************************************************/
void FT2_OSW_BIT_CLASS::clear( FT2_OSW_BIT_CLASS & sorc )
{
bits[OSW1] &= ~sorc.bits[OSW1];
bits[OSW2] &= ~sorc.bits[OSW2];
bits[OSW3] &= ~sorc.bits[OSW3];
}

/***********************************************************************
*                               CLEARALL                               *
***********************************************************************/
void FT2_OSW_BIT_CLASS::clearall()
{
bits[OSW1] = 0;
bits[OSW2] = 0;
bits[OSW3] = 0;
}

/***********************************************************************
*                                ISEMPTY                               *
***********************************************************************/
bool FT2_OSW_BIT_CLASS::isempty()
{
int i;
for ( i=0; i<NOF_OSWS; i++ )
    {
    if ( bits[i] != 0 )
        return false;
    }
return true;
}

/***********************************************************************
*                                 ISSET                                *
*                 Return true of one or more bits are set              *
***********************************************************************/
bool FT2_OSW_BIT_CLASS::isset()
{
int i;
for ( i=0; i<NOF_OSWS; i++ )
    {
    if ( bits[i] != 0 )
        return true;
    }
return false;
}

/***********************************************************************
*                              OPERATOR[]                              *
***********************************************************************/
bool FT2_OSW_BIT_CLASS::operator[]( OUTPUT_WIRE_CLASS wire_to_check )
{
unsigned u;

switch( wire_to_check.type )
    {
    case OSW1_WIRE:
        u = bits[OSW1] & wire_to_check.mask();
        break;
    case OSW2_WIRE:
        u = bits[OSW2] & wire_to_check.mask();
        break;
    case OSW3_WIRE:
        u = bits[OSW3] & wire_to_check.mask();
        break;
    }

if ( u )
    return true;

return false;
}

/***********************************************************************
*                                   =                                  *
***********************************************************************/
void FT2_OSW_BIT_CLASS::operator=( FT2_OSW_BIT_CLASS & sorc )
{
bits[OSW1] = sorc.bits[OSW1];
bits[OSW2] = sorc.bits[OSW2];
bits[OSW3] = sorc.bits[OSW3];
}

/***********************************************************************
*                                  |=                                  *
***********************************************************************/
void FT2_OSW_BIT_CLASS::operator|=( FT2_OSW_BIT_CLASS & sorc )
{
bits[OSW1] |= sorc.bits[OSW1];
bits[OSW2] |= sorc.bits[OSW2];
bits[OSW3] |= sorc.bits[OSW3];
}

/***********************************************************************
*                                  &=                                  *
***********************************************************************/
void FT2_OSW_BIT_CLASS::operator&=( FT2_OSW_BIT_CLASS & sorc )
{
bits[OSW1] &= sorc.bits[OSW1];
bits[OSW2] &= sorc.bits[OSW2];
bits[OSW3] &= sorc.bits[OSW3];
}

/***********************************************************************
*                                  ^=                                  *
***********************************************************************/
void FT2_OSW_BIT_CLASS::operator^=( FT2_OSW_BIT_CLASS & sorc )
{
bits[OSW1] ^= sorc.bits[OSW1];
bits[OSW2] ^= sorc.bits[OSW2];
bits[OSW3] ^= sorc.bits[OSW3];
}

/***********************************************************************
*                                   ==                                 *
***********************************************************************/
bool FT2_OSW_BIT_CLASS::operator==( FT2_OSW_BIT_CLASS & sorc )
{
if ( bits[OSW1] != sorc.bits[OSW1] ) return false;
if ( bits[OSW2] != sorc.bits[OSW2] ) return false;
if ( bits[OSW3] != sorc.bits[OSW3] ) return false;
return true;
}

/***********************************************************************
*                                   !=                                 *
***********************************************************************/
bool FT2_OSW_BIT_CLASS::operator!=( FT2_OSW_BIT_CLASS & sorc )
{
if ( bits[OSW1] != sorc.bits[OSW1] ) return true;
if ( bits[OSW2] != sorc.bits[OSW2] ) return true;
if ( bits[OSW3] != sorc.bits[OSW3] ) return true;
return false;
}

/***********************************************************************
*                                 SET                                  *
***********************************************************************/
void FT2_OSW_BIT_CLASS::set( OUTPUT_WIRE_CLASS & wire_to_set )
{
switch( wire_to_set.type )
    {
    case OSW1_WIRE:
        bits[OSW1] |= wire_to_set.mask();
        break;
    case OSW2_WIRE:
        bits[OSW2] |= wire_to_set.mask();
        break;
    case OSW3_WIRE:
        bits[OSW3] |= wire_to_set.mask();
        break;
    }
}

/***********************************************************************
*                                 SET                                  *
***********************************************************************/
void FT2_OSW_BIT_CLASS::set( unsigned wire_number_to_set )
{
OUTPUT_WIRE_CLASS w;
w = wire_number_to_set;
set( w );
}