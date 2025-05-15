#ifndef _WIRE_CLASS_
#define _WIRE_CLASS_

class WIRE_NAME_ENTRY
{
public:

STRING_CLASS  wire_number;
STRING_CLASS  input_connector_name;
STRING_CLASS  output_connector_name;
unsigned      wire_bit;

WIRE_NAME_ENTRY();
~WIRE_NAME_ENTRY();
};

class WIRE_CLASS
    {
    public:
    WIRE_NAME_ENTRY wirelist[32];

    WIRE_CLASS();
    ~WIRE_CLASS();

    BOOLEAN  get( STRING_CLASS & co, STRING_CLASS & ma );
    int      wire_number( STRING_CLASS & wire_name );
    unsigned wirebit( STRING_CLASS & wire_name );
    unsigned wirebit( int wire_number );
    };

#endif
