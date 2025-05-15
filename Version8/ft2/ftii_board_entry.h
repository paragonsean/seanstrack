#ifndef _GENLIST_H
#include "..\include\genlist.h"
#endif

#ifndef _EXTERNAL_PARAMETER_SETTINGS_CLASS_
#include "..\include\external_parameter.h"
#endif

struct BOARD_EXTENDED_ANALOG_ENTRY {
    unsigned              mask;
    int                   channel;
    unsigned              trigger_type;
    BOOLEAN               waiting_for_value;
    EXTENDED_ANALOG_ENTRY ae;
    BOARD_EXTENDED_ANALOG_ENTRY() { mask=0; channel=NO_FT_CHANNEL; trigger_type=TRIGGER_WHEN_GREATER; waiting_for_value=FALSE; }
    };

class BOARD_EXTENDED_ANALOG_ARRAY {
    public:
    int n;
    BOARD_EXTENDED_ANALOG_ENTRY * array;

    BOARD_EXTENDED_ANALOG_ARRAY() { n = 0; array=0; }
    ~BOARD_EXTENDED_ANALOG_ARRAY();
    void clear();
    BOOLEAN init( int new_n );
    BOARD_EXTENDED_ANALOG_ENTRY & operator[]( int i );
    };

class FTII_BOARD_ENTRY {
    public:

    int             number;
    int             index;
    int             version;
    int             sub_version;
    int             shot_number;
    int             timer_frequency;
    bool            has_alarm;
    bool            has_warning;
    bool            needs_impact_position_report;

    unsigned        alarm_bits;              /* Output bits mapped to alarms */
    unsigned        bits_currently_on;       /* Alarm + monitor wire + alarm_on_down */
    unsigned        eos_bits_on;             /* Bits turned ON at eos (used by external parameters) */
    unsigned        config_word;
    unsigned        connect_state;
    unsigned        ms_at_last_contact;
    unsigned        do_not_upload_timeout;   /* ms = time when I can upload */
    unsigned        input_bits;

    unsigned        control_file_date;       /* V447 = low 31 bits of last write time */
    unsigned        upload_copy_date;        /* V448 = low 31 bits of last write time 0xF0000000=> no file*/
    int             upload_state;            /* WAITING_FOR_INFO, UPLOAD_ALL, UPLOAD_ALL_VARIABLES, UPLOAD_NEW_VARIABLES */

    int             down_state;
    int             down_timeout_seconds;
    TIME_CLASS      time_of_last_shot;
    BOARD_DATA      statusbits;

    STRING_CLASS    address;
    STRING_CLASS    port;
    STRING_CLASS    version_string;
    NAME_CLASS      upload_file_name;
    char            sample_type;    /* Type of binary data expected [P, T, C] */

    int             current_type;
    int             current_len;  /* Binary data only */
    int             chars_left_in_buffer;
    int             current_packet_number;
    char          * bp;
    char          * binarybuf;
    sockaddr_in     a;
    int             channel_mode;
    bool            do_not_upload;
    bool            have_response;   /* Used during upload to choose error message */
    bool            monitoring_was_stopped;
    bool            need_to_check_for_shot_complete_timeout;
    bool            waiting_for_ack;
    bool            uploading;
    bool            start_op_data_after_upload;
    bool            is_sending_op_data_packets;
    int             nak_count;
    int             bytes_uploaded;
    FIFO_CLASS      f;

    MACHINE_CLASS   m;
    PART_CLASS      part;
    PARAMETER_CLASS param;
    PRESSURE_CONTROL_CLASS pc;
    FTANALOG_CLASS  ftanalog;
    FTCALC_CLASS    ftcalc;
    BOOLEAN         have_runlist;
    RUNLIST_CLASS   runlist;
    FTII_NET_DATA   shotdata;
    EXTERNAL_PARAMETER_SETTINGS_CLASS external_parameter_settings;
    BOARD_EXTENDED_ANALOG_ARRAY extended_analog;

    FTII_BOARD_ENTRY();
    ~FTII_BOARD_ENTRY();

    void            check_inputs( TCHAR * sorc );
    void            check_for_extended_channel_value( TCHAR * sorc );
    void            clearwire( int wire_to_clear );
    void            get_alarm_bits();
    unsigned        get_parameter_alarm_bits();
    BOOLEAN         get_version_string();
    void            put_version_string();
    void            clear_all_alarm_bits();
    bool            upload_control_file();
    void            set_connect_state( unsigned new_state );
    void            set_extended_analog_bits();
    void            save_shot();
    void            add_binary_to_shotdata();
    void            get_config_word( TCHAR * cp );
    void            setwire( int wire_to_set );
    void            send_bits( unsigned bits, bool set_bits );
    void            send_config_word();
    bool            send_socket_message( TCHAR * sorc, bool need_to_set_event );
    bool            is_monitoring();
    };