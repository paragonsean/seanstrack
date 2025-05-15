#ifndef _SHOT_CLASS_
#define _SHOT_CLASS_

#ifndef _STRUCTS_
#include "..\include\structs.h"
#endif

#ifndef _TIME_CLASS_
#include "..\include\timeclas.h"
#endif

#ifndef _VDB_CLASS_
#include "..\include\vdbclass.h"
#endif

class SHOT_CLASS
    {
    public:
    TIME_CLASS time;
    STRING_CLASS name;

    SHOT_CLASS() {}
    ~SHOT_CLASS() {}
    void       clear() { name.null(); }
    void       operator=( const SHOT_CLASS & sorc );
    BOOLEAN    operator==( const SHOT_CLASS & sorc ) { return (time == sorc.time); }
    BOOLEAN    operator==( const TIME_CLASS & sorc ) { return (time == sorc); }
    BOOLEAN    operator==( const STRING_CLASS & sorc ) { return (name == sorc); }
    BOOLEAN    operator==( const TCHAR * sorc ) { return (name == sorc); }
    BOOLEAN    get_current( PART_NAME_ENTRY & p );
    BOOLEAN    get_from_graphlst( DB_TABLE & t );
    BOOLEAN    get_from_shotparm( VDB_CLASS & t );
    };

#endif

