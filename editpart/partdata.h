struct MACHINE_ENTRY
    {
    STRING_CLASS    name;
    STRING_CLASS    computer;
    TEXT_LIST_CLASS partlist;
    };

void            cleanup_machines_and_parts();
MACHINE_ENTRY * find_machine_entry( STRING_CLASS & machine );
BOOLEAN         find_part_entry( MACHINE_ENTRY * m, STRING_CLASS & part );
int             machine_count( STRING_CLASS & computer_to_search );
BOOLEAN         machine_is_local( STRING_CLASS & ma );
BOOLEAN         read_machines_and_parts();
BOOLEAN         reload_partlist( STRING_CLASS & machine );

