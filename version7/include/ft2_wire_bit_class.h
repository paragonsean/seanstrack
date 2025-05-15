/***********************************************************************
* This keeps track of the OSW1, OSW2, and OSW3 bits. It uses wire      *
* numbers for indexes. OSW1[1,32], OSW2[101,132], OSW3[201,232]        *
***********************************************************************/
#ifndef _FT2_OSW_BIT_CLASS_
#define _FT2_OSW_BIT_CLASS_

#ifndef _WIRE_CLASS_
#include ".\wire_class.h"
#endif

#define OSW1     0
#define OSW2     1
#define OSW3     2
#define NOF_OSWS 3

class FT2_OSW_BIT_CLASS
    {
    public:
    unsigned bits[NOF_OSWS];

    FT2_OSW_BIT_CLASS();
    FT2_OSW_BIT_CLASS( unsigned a, unsigned b, unsigned c );
    ~FT2_OSW_BIT_CLASS();
    void  clear( unsigned wire_number_to_clear );
    void  clear( OUTPUT_WIRE_CLASS & wire_to_clear );
    void  clear( FT2_OSW_BIT_CLASS & sorc ); // Clear bits that are set in sorc
    void  clearall();
    bool  isempty();
    bool  isset();
    void  set( unsigned wire_number_to_set );
    void  set( OUTPUT_WIRE_CLASS & wire_to_set );
    void  operator=( FT2_OSW_BIT_CLASS & sorc );
    bool  operator[]( OUTPUT_WIRE_CLASS wire_to_check );
    bool  operator==( FT2_OSW_BIT_CLASS & sorc );
    bool  operator!=( FT2_OSW_BIT_CLASS & sorc );
    void  operator|=( FT2_OSW_BIT_CLASS & sorc );
    void  operator&=( FT2_OSW_BIT_CLASS & sorc );
    void  operator^=( FT2_OSW_BIT_CLASS & sorc );
    };

#endif