#ifndef _PROFILE_MERGE_CLASS_
#define _PROFILE_MERGE_CLASS_

#ifndef _DB_CLASS_
#include "..\include\dbclass.h"
#endif

#ifndef _SHOT_CLASS_
#include "..\include\shotclas.h"
#endif

class PROFILE_MERGE_CLASS
    {
    private:

    NAME_CLASS dir;
    SHOT_CLASS current_shot;
    BOOLEAN    have_current_shot;

    DB_TABLE t;

    public:

    SHOT_CLASS min;
    SHOT_CLASS max;

    PROFILE_MERGE_CLASS();
    ~PROFILE_MERGE_CLASS();
    void    close() { t.close(); }
    BOOLEAN have_shot() { return have_current_shot; }
    BOOLEAN next();
    BOOLEAN get( PROFILE_MERGE_CLASS & sorc, STRING_CLASS & new_shot_name );
    BOOLEAN open_graphlst( NAME_CLASS & directory_name, short record_length, short table_lock_type );
    void    rewind();
    SHOT_CLASS & shot() { return current_shot; }
    BOOLEAN get_min_and_max();
    BOOLEAN operator<( const PROFILE_MERGE_CLASS & sorc );
    BOOLEAN operator<( const TIME_CLASS & sorc );
    BOOLEAN operator==( const PROFILE_MERGE_CLASS & sorc );
    BOOLEAN operator==( const TIME_CLASS & sorc );
    };

class SHOTPARM_MERGE_CLASS
    {
    private:

    NAME_CLASS dir;
    SHOT_CLASS current_shot;
    BOOLEAN    have_current_shot;

    VDB_CLASS t;

    public:

    SHOT_CLASS min;
    SHOT_CLASS max;

    SHOTPARM_MERGE_CLASS();
    ~SHOTPARM_MERGE_CLASS();
    void    close() { t.close(); }
    BOOLEAN have_shot() { return have_current_shot; }
    BOOLEAN next();
    BOOLEAN get( SHOTPARM_MERGE_CLASS & sorc, STRING_CLASS & new_shot_name );
    BOOLEAN open_shotparm( NAME_CLASS & directory_name, short table_lock_type );
    void    rewind();
    SHOT_CLASS & shot() { return current_shot; }
    BOOLEAN get_min_and_max();
    BOOLEAN operator<( const SHOTPARM_MERGE_CLASS & sorc );
    BOOLEAN operator<( const TIME_CLASS & sorc );
    BOOLEAN operator==( const SHOTPARM_MERGE_CLASS & sorc );
    BOOLEAN operator==( const TIME_CLASS & sorc );
    };

class DOWNTIME_MERGE_CLASS
    {
    private:

    DOWNTIME_ENTRY d;
    BOOLEAN        have_current_downtime;

    DB_TABLE t;

    public:

    DOWNTIME_MERGE_CLASS();
    ~DOWNTIME_MERGE_CLASS();
    void    close() { t.close(); }
    BOOLEAN have_downtime() { return have_current_downtime; }
    BOOLEAN next();
    BOOLEAN add( const DOWNTIME_MERGE_CLASS & sorc );
    BOOLEAN open( NAME_CLASS & directory_name, short table_lock_type );
    void    rewind();
    TIME_CLASS & time() { return d.time; }
    BOOLEAN operator<( const DOWNTIME_MERGE_CLASS & sorc );
    BOOLEAN operator<( const TIME_CLASS & sorc );
    BOOLEAN operator==( const DOWNTIME_MERGE_CLASS & sorc );
    BOOLEAN operator==( const TIME_CLASS & sorc );
    };

#endif
