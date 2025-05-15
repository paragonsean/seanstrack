#ifndef _WARMUP_CLASS_
#define _WARMUP_CLASS_

#ifndef _STRUCTS_
#include "..\include\structs.h"
#endif

class WARMUP_CLASS
    {
    public:

    BOOLEAN      is_enabled;
    STRING_CLASS partname;
    BOOLEAN      run_if_on;
    TCHAR        wire[WARMUP_WIRE_LEN+1];

    WARMUP_CLASS();
    WARMUP_CLASS( const WARMUP_CLASS & sorc );
    ~WARMUP_CLASS() {}

    BOOLEAN get( STRING_CLASS & cn, STRING_CLASS & mn, STRING_CLASS & pn );
    BOOLEAN get( PART_NAME_ENTRY & pn ) { return get(pn.computer, pn.machine, pn.part); }

    BOOLEAN put( STRING_CLASS & cn, STRING_CLASS & mn, STRING_CLASS & pn );
    BOOLEAN put( PART_NAME_ENTRY & pn ) { return put(pn.computer, pn.machine, pn.part); }

    BOOLEAN create_runlist( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa );
    };

#endif

