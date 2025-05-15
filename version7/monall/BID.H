struct BOARD_INITIALIZATION_DATA
    {
    short nof_time_samples;
    double total_stroke_len;
    double min_stroke_len;
    double biscuit_time_delay;
    double eos_velocity;
    double sleeve_fill_distance;
    double runner_fill_distance;
    double min_csfs_vel;
    double rise_csfs_vel;
    double min_csfs_pos;
    double limits[LIMIT_SWITCH_COUNT];
    double ms_per_time_based_sample;
    double auto_shot_timeout_seconds;
    };

BOOLEAN get_board_initialization_data( BOARD_INITIALIZATION_DATA & dest, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
