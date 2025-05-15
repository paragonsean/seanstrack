#ifndef _OUTPUT_LABEL_CLASS_
#define _OUTPUT_LABEL_CLASS_

class OUTPUT_LABEL_CLASS
    {
    private:
    BOOLEAN      has_been_initialized;
    STRING_CLASS labels[HIGH_FT2_OUTPUT_WIRE];
    STRING_CLASS wires[HIGH_FT2_OUTPUT_WIRE];

    public:

    OUTPUT_LABEL_CLASS();
    ~OUTPUT_LABEL_CLASS(){};
    BOOLEAN init();
    BOOLEAN is_initialized() { return has_been_initialized; }
    int            wire_number( STRING_CLASS & label_to_find );
    STRING_CLASS & wire( STRING_CLASS & label_to_find );
    STRING_CLASS & label( STRING_CLASS & wire_to_find );
    STRING_CLASS & operator[]( int index );
    };

#endif