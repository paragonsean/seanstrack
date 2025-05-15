#ifndef _MULTIPART_RUNLIST_CLASSES_
#define _MULTIPART_RUNLIST_CLASSES_

#define NO_PART_SELECTION_NUMBER -1

class MULTIPART_RUNLIST_SETTINGS_CLASS
    {
    public:

    bool         enable;
    unsigned     mask;
    int          n;
    STRING_CLASS wires;

    MULTIPART_RUNLIST_SETTINGS_CLASS();
    ~MULTIPART_RUNLIST_SETTINGS_CLASS();
    void operator=( const MULTIPART_RUNLIST_SETTINGS_CLASS & sorc );
    bool get( STRING_CLASS & co, STRING_CLASS & ma );
    void init();
    int  part_selection_number( STRING_CLASS & wirelist );
    bool part_to_run( STRING_CLASS & dest, STRING_CLASS & co, STRING_CLASS & ma, unsigned int wire_bits );
    bool put( STRING_CLASS & co, STRING_CLASS & ma );
    bool get_wirelist( STRING_CLASS & dest, int part_selection_number );
    int  max_part_selection_number();
    };

class MULTIPART_RUNLIST_WIRES_CLASS
    {
    public:
    int          part_selection_number;
    unsigned int bits;
    STRING_CLASS wires;

    MULTIPART_RUNLIST_WIRES_CLASS();
    ~MULTIPART_RUNLIST_WIRES_CLASS();
    void cleanup();
    bool isempty();
    void operator=( const MULTIPART_RUNLIST_WIRES_CLASS & sorc );
    bool get( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa );
    bool put( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa );
    };
/*
-------------------------------------------------------------------
Use the following if you just need to get the part selection number
------------------------------------------------------------------- */
int get_part_selection_number( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa );

/*
----------------------------------------------------------------------------------
Use the following to get the wire bits from comma delimited string of wire numbers
---------------------------------------------------------------------------------- */
unsigned bits_from_wirelist( STRING_CLASS & wirelist );

#endif