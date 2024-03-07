#ifndef _MACHINE_CLASS_
#define _MACHINE_CLASS_

#ifndef _DB_CLASS_
#include "..\include\dbclass.h"
#endif

#ifndef _MULTIPART_RUNLIST_CLASSES_
#include "..\include\multipart_runlist.h"
#endif

#define PRE_IMPACT_INDEX    0
#define POST_IMPACT_INDEX   1
#define MAX_SHOT_REGION     2

struct SAVE_COUNTS
    {
    int       skip_count;
    int       acquire_count;
    int       max_saved;
    int       skip_seconds;
    BITSETYPE flags;
    };

class MACHINE_CLASS
    {
    private:

    BOOLEAN MACHINE_CLASS::open_readonly( DB_TABLE & t, STRING_CLASS & tname );

    public:
    STRING_CLASS                     computer;
    STRING_CLASS                     name;

    int                              autoshot_timeout_seconds;
    STRING_CLASS                     current_part;
    int                              cycle_timeout_seconds;
    STRING_CLASS                     devent_time;
    int                              diff_curve_number;
    int                              ft_board_number;
    STRING_CLASS                     human;
    int                              impact_points[MAX_SHOT_REGION];
    BITSETYPE                        impact_pres_type[MAX_SHOT_REGION];
    BOOLEAN                          is_cyclone;
    BOOLEAN                          is_ftii;
    BITSETYPE                        monitor_flags;
    MULTIPART_RUNLIST_SETTINGS_CLASS multipart_runlist_settings;
    int                              muxchan;
    SAVE_COUNTS                      params;
    SAVE_COUNTS                      profile;
    STRING_CLASS                     pulse_time;
    STRING_CLASS                     pulse_wire;
    int                              quadrature_divisor;
    STRING_CLASS                     rodpitch;
    BOOLEAN                          suretrak_controlled;
    BITSETYPE                        type;
    STRING_CLASS                     valve_setting_after_jog_shot;

    MACHINE_CLASS();
    void       init();
    BOOLEAN    has_hmi();
    BOOLEAN    has_binary_valve();
    BOOLEAN    isempty();
    BOOLEAN    find( STRING_CLASS & computer, STRING_CLASS & name );
    BOOLEAN    ls_off_at_eos();
    void       operator=( const MACHINE_CLASS & sorc );
    BOOLEAN    read_machset_db( STRING_CLASS & fname );
    BOOLEAN    refresh_monitor_flags();
    BOOLEAN    save();
    void       set_has_hmi( BOOLEAN b );
    void       set_has_binary_valve( BOOLEAN b );
    void       set_ls_off_at_eos( BOOLEAN b );
    };

#endif
