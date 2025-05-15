#ifndef _GENLIST_H
#include "..\include\genlist.h"
#endif

#ifndef _EXTERNAL_PARAMETER_SETTINGS_CLASS_
#include "..\include\external_parameter.h"
#endif

#define IS_SUCCESSFUL 0
#define FT3_SOCKET_CREATION_ERROR 1
#define FT3_SOCKET_CONNECT_ERROR  2
#define FT3_SOCKET_READ_ERROR     3
#define FT3_SOCKET_SEND_ERROR     4

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

    sockaddr_in     a;
    STRING_CLASS    address;
    unsigned        alarm_bits;              /* Output bits mapped to alarms */
    char          * binarybuf;
    unsigned        bits_currently_on;       /* Alarm + monitor wire + alarm_on_down */
    char          * bp;
    int             bytes_uploaded;
    int             channel_mode;
    int             chars_left_in_buffer;
    unsigned        config_word;
    unsigned        connect_state;
    unsigned        control_file_date;       /* V447 = low 31 bits of last write time */
    int             current_len;  /* Binary data only */
    int             current_packet_number;
    int             current_type;
    bool            do_not_upload;
    unsigned        do_not_upload_timeout;   /* ms = time when I can upload */
    int             down_state;
    int             down_timeout_seconds;
    unsigned        eos_bits_on;             /* Bits turned ON at eos (used by external parameters) */
    bool            ft3_is_connected;
    bool            ft3_read_thread_is_running;
    HANDLE          ft3_send_event;
    bool            ft3_send_thread_is_running;
    bool            has_alarm;
    bool            has_warning;
    bool            have_response;   /* Used during upload to choose error message */
    BOOLEAN         have_runlist;
    int             index;
    unsigned        input_bits;
    bool            is_ft3;
    bool            is_sending_op_data_packets;
    bool            monitoring_was_stopped;
    unsigned        ms_at_last_contact;
    int             nak_count;
    bool            needs_impact_position_report;
    int             number;
    STRING_CLASS    port;
    char            sample_type;    /* Type of binary data expected [P, T, C] */
    int             shot_number;
    SOCKET          mysock;
    bool            start_op_data_after_upload;
    BOARD_DATA      statusbits;
    int             sub_version;
    TIME_CLASS      time_of_last_shot;
    int             timer_frequency;
    unsigned        upload_copy_date;        /* V448 = low 31 bits of last write time 0xF0000000=> no file*/
    NAME_CLASS      upload_file_name;
    int             upload_state;            /* WAITING_FOR_INFO, UPLOAD_ALL, UPLOAD_ALL_VARIABLES, UPLOAD_NEW_VARIABLES */
    bool            uploading;
    int             version;
    STRING_CLASS    version_string;
    bool            waiting_for_ack;

    FIFO_CLASS             f;
    FTANALOG_CLASS         ftanalog;
    FTCALC_CLASS           ftcalc;
    MACHINE_CLASS          m;
    PART_CLASS             part;
    PARAMETER_CLASS        param;
    PRESSURE_CONTROL_CLASS pc;
    RUNLIST_CLASS          runlist;
    FTII_NET_DATA          shotdata;

    BOARD_EXTENDED_ANALOG_ARRAY       extended_analog;
    EXTERNAL_PARAMETER_SETTINGS_CLASS external_parameter_settings;

    FTII_BOARD_ENTRY();
    ~FTII_BOARD_ENTRY();

    /*
    --------------------
    Shot Event Functions
    -------------------- */
    BOOLEAN         read_shot_events();
    BOOLEAN         process_new_shot_event( int new_event );
    BOOLEAN         check_for_shot_event_timeouts( TIME_CLASS & now );

    void            add_binary_to_shotdata();
    void            check_for_extended_channel_value( TCHAR * sorc );
    void            check_inputs( TCHAR * sorc );
    void            clear_all_alarm_bits();
    void            clearwire( int wire_to_clear );
    int             ft3_connect();
    bool            ft3_disconnect();
    void            get_alarm_bits();
    void            get_config_word( TCHAR * cp );
    unsigned        get_parameter_alarm_bits();
    BOOLEAN         get_version_string();
    bool            is_monitoring();
    void            put_version_string();
    void            save_shot();
    void            send_bits( unsigned bits, bool set_bits );
    void            send_config_word();
    bool            send_socket_message( TCHAR * sorc, bool need_to_set_event );
    void            set_connect_state( unsigned new_state );
    void            set_extended_analog_bits();
    void            setwire( int wire_to_set );
    bool            upload_control_file();
    };
