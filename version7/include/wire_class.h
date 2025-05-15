#ifndef _WIRE_CLASS_
#define _WIRE_CLASS_

#define ISW1_WIRE 1
#define ISW2_WIRE 2
#define ISW3_WIRE 3
#define ISW4_WIRE 4

#define OSW1_WIRE 1
#define OSW2_WIRE 2
#define OSW3_WIRE 3

class OUTPUT_WIRE_CLASS
    {
    public:
    unsigned type;
    unsigned number;

    OUTPUT_WIRE_CLASS();
    ~OUTPUT_WIRE_CLASS();
    unsigned bit_number();  /* [0, 31] */
    unsigned mask();        /* [0, 0xffff] */
    bool     isempty();
    bool     isset();
    void     operator=( unsigned new_number ); /* Opto:[1, 32], Virtual Monitor:[101, 132], Virtual Control:[200, 221] */
    void     operator=( OUTPUT_WIRE_CLASS & wc );
    void     operator++(int);
    bool     operator==( OUTPUT_WIRE_CLASS & wc );
    bool     operator!=( OUTPUT_WIRE_CLASS & wc );
    void     set_from_mask( unsigned new_type, unsigned new_mask );
    };

#endif
