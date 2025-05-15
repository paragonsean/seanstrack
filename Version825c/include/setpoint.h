#ifndef _SETPOINT_
#define _SETPOINT_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

struct SETPOINT_ENTRY
    {
    STRING_CLASS desc;
    STRING_CLASS value;

    void     operator=( const SETPOINT_ENTRY & sorc );
    int      int_value() { return value.int_value(); }
    unsigned uint_value() { return value.uint_value(); }
    double   real_value() { return value.real_value(); }
    BOOLEAN  boolean_value() { return value.boolean_value(); }
    };

class SETPOINT_CLASS
    {
    private:

    SETPOINT_ENTRY * array;
    int              n;

    public:

    SETPOINT_CLASS();
    ~SETPOINT_CLASS();

    SETPOINT_ENTRY & operator[]( int i );
    SETPOINT_ENTRY & operator[]( const TCHAR * sorc );
    SETPOINT_ENTRY & operator[]( STRING_CLASS & sorc );

    int     count() { return n; }
    void    cleanup();
    void    empty();
    void    operator=( const SETPOINT_CLASS & sorc );
    BOOLEAN remove( int index_to_remove );
    BOOLEAN setsize( int new_n );

    int     get( STRING_CLASS & path );
    int     getfrom( STRING_CLASS & path );
    BOOLEAN get_one_value( STRING_CLASS & dest, STRING_CLASS & path, int index_to_get );
    BOOLEAN put( STRING_CLASS & path );
    };

#endif