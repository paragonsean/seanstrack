#ifndef _MACHINE_NAME_LIST_CLASS_
#define _MACHINE_NAME_LIST_CLASS_

#ifndef _GENLIST_H
#include "..\include\genlist.h"
#endif

#define SORT_BY_COMPUTER 1
#define SORT_BY_MACHINE  2
#define NO_SORT          3

struct MACHINE_NAME_LIST_ENTRY
    {
    STRING_CLASS computer;
    STRING_CLASS machine;
    STRING_CLASS hostname;
    STRING_CLASS ip_address;
    };

class MACHINE_NAME_LIST_CLASS
    {
    private:
    MACHINE_NAME_LIST_ENTRY * current_entry;
    GENERIC_LIST_CLASS        mlist;

    public:
    MACHINE_NAME_LIST_CLASS();
    ~MACHINE_NAME_LIST_CLASS();
    BOOLEAN    acquire( MACHINE_NAME_LIST_CLASS & sorc, BITSETYPE sort_type );
    BOOLEAN    add_computer( STRING_CLASS & computer );
    BOOLEAN    add_computer( STRING_CLASS & computer, STRING_CLASS & hostname, STRING_CLASS & ip_addr );
    BOOLEAN    add( MACHINE_NAME_LIST_ENTRY * mne, BITSETYPE sort_type );
    BOOLEAN    add( STRING_CLASS & computer, STRING_CLASS & machine, BITSETYPE sort_type );
    int        count() { return (int) mlist.count(); }
    BOOLEAN    get_computer_name( STRING_CLASS & destco, STRING_CLASS & sorcma );
    void       empty();
    void       remove();
    void       rewind() { mlist.rewind(); current_entry = 0; }
    BOOLEAN    next();
    BOOLEAN    find( TCHAR * name_to_find );
    BOOLEAN    find( STRING_CLASS & machine_name );
    STRING_CLASS & name();
    STRING_CLASS & computer_name();
    STRING_CLASS & hostname();
    STRING_CLASS & ip_address();
    };

#endif
