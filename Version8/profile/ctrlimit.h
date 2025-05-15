#ifndef _CONTROL_LIMIT_CLASS_
#define _CONTROL_LIMIT_CLASS_

#ifndef _DB_CLASS_
#include "..\include\dbclass.h"
#endif

class CONTROL_LIMIT_ENTRY
    {
    public:

    int32 number;
    float low;
    float high;

    CONTROL_LIMIT_ENTRY() { number = NO_PARAMETER_NUMBER; low = 0.0; high = 0.0; }
    ~CONTROL_LIMIT_ENTRY() {}
    BOOLEAN read( DB_TABLE & t );
    BOOLEAN write( DB_TABLE & t );
    };

class CONTROL_LIMIT_CLASS
    {
    private:

    CONTROL_LIMIT_ENTRY * current_entry;
    GENERIC_LIST_CLASS list;

    public:

    CONTROL_LIMIT_CLASS();
    ~CONTROL_LIMIT_CLASS();

    int32   parameter_number( void ) { if ( current_entry ) return current_entry->number;else return NO_PARAMETER_NUMBER; }
    float   low_limit( void ) { if ( current_entry ) return current_entry->low;  else return 0.0; }
    float   high_limit( void ){ if ( current_entry ) return current_entry->high; else return 0.0; }
    void    empty( void );
    BOOLEAN add( int32 new_number, float new_low_limit, float new_high_limit );
    BOOLEAN find( int32 number_to_find );
    BOOLEAN next( void );
    void    rewind( void ) { list.rewind(); }
    BOOLEAN read( TCHAR * filename );
    BOOLEAN read( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    BOOLEAN read( PART_NAME_ENTRY & p );
    BOOLEAN write( TCHAR * filename );
    BOOLEAN write( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    BOOLEAN write( PART_NAME_ENTRY & p );
    };

#endif

