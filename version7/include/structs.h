#ifndef _STRUCTS_

#define _STRUCTS_

struct MINMAX
    {
    float min;
    float max;
    };

struct POS_SENSOR_SETUP_DATA
    {
    float x4_pos_divisor;    /* Position units per unit measure */
    float x1_pos_divisor;    /* Marks per unit measure */
    float vel_dividend;      /* Divide by count to obtain velocity */
    float min_vel;           /* Min vel in units for rod pitch */
    float crystal_freq;      /* FasTrak crystal freq. in MHz. */
    };

struct CURVE_DATA
    {
    BITSETYPE type;
    float     max;
    float     min;
    short     units;
    short     ft_channel_number();
    BITSETYPE ft_channel_type();
    };

#define _PROFILE_H_

struct PROFILE_HEADER
    {
    TCHAR      machine_name[OLD_MACHINE_NAME_LEN+1];
    TCHAR      part_name[OLD_PART_NAME_LEN+1];
    int32      shot_number;
    FILETIME   time_of_shot;      /* 8 bytes */
    short      n;
    short      last_pos_based_point; /* NofPosBasedPoints - 1 */
    CURVE_DATA curves[MAX_CURVES];
    };

#ifdef _STRING_CLASS_

struct MACHINE_NAME_ENTRY
    {
    STRING_CLASS computer;
    STRING_CLASS machine;

    MACHINE_NAME_ENTRY();
    void    empty();
    BOOLEAN is_empty();
    BOOLEAN is_equal( const STRING_CLASS & cn, const STRING_CLASS & mn );
    BOOLEAN operator==(const MACHINE_NAME_ENTRY & sorc ) { return is_equal(sorc.computer, sorc.machine); }
    void    operator=(const MACHINE_NAME_ENTRY & sorc );
    inline void set( STRING_CLASS & cn, STRING_CLASS & mn ) { computer = cn; machine = mn; }
    inline void set( STRING_CLASS & mn ) { machine = mn; }
    };

struct PART_NAME_ENTRY
    {
    STRING_CLASS computer;
    STRING_CLASS machine;
    STRING_CLASS part;

    BOOLEAN exists();
    BOOLEAN is_equal( const STRING_CLASS & cn, const STRING_CLASS & mn, const STRING_CLASS & pn );
    void set( STRING_CLASS & cn, STRING_CLASS & mn );
    inline void set( STRING_CLASS & cn, STRING_CLASS & mn, STRING_CLASS & pn ) { computer=cn; machine=mn; part=pn; }
    inline void set( STRING_CLASS & pn ) { part = pn; }
    PART_NAME_ENTRY();
    BOOLEAN operator==(const PART_NAME_ENTRY & sorc );
    void    operator=(const PART_NAME_ENTRY & sorc );
    };

struct PROFILE_NAME_ENTRY
    {
    STRING_CLASS computer;
    STRING_CLASS machine;
    STRING_CLASS part;
    STRING_CLASS shot;
    void  set( const STRING_CLASS & cn, const STRING_CLASS & mn, const STRING_CLASS & pn, const STRING_CLASS & sn );
    void  set( const STRING_CLASS & cn, const STRING_CLASS & mn, const STRING_CLASS & pn, const TCHAR * sn );
    void  operator=(const PROFILE_NAME_ENTRY & sorc );
    void  operator=(const PART_NAME_ENTRY & sorc );
    };

#endif

struct SHOT_ENTRY
    {
    SYSTEMTIME time;
    int32      number;
    };

struct SHOT_RANGE_ENTRY
    {
    SHOT_ENTRY newest;
    SHOT_ENTRY oldest;
    };

#endif
