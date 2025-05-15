#ifndef _SUBS_H_
#define _SUBS_H_

/*
-----------------------
Args for copy_all_files
----------------------- */
#define WITH_SUBS            TRUE
#define WITHOUT_SUBS         FALSE

inline float absolute_value( float x )
{
if ( x < 0.0 )
    x = -x;
return x;
}

inline int absolute_value( int x )
{
if ( x < 0.0 )
    x = -x;
return x;
}

inline bool boolean_to_bool( BOOLEAN b )
{
if ( b )
    return true;
return false;
}

inline void swap( float & x, float & y )
{
float z;
z = y;
y = x;
x = z;
}

inline void swap( int & x, int & y )
{
int z;
z = y;
y = x;
x = z;
}

inline void toggle( BOOLEAN & b )
{
if ( b )
    b = FALSE;
else
    b = TRUE;
}

double    adjust_for_units( short dest_units_id, double x, short sorc_units_id );
float     adjust_for_units( short dest_units_id, float sorc, short sorc_units_id );
BOOLEAN   alphadate( TCHAR * dest, const SYSTEMTIME & sorc );
TCHAR   * alphadate( const SYSTEMTIME & sorc );
BOOLEAN   alphatime( TCHAR * dest, const SYSTEMTIME & sorc );
TCHAR   * alphatime( const SYSTEMTIME & sorc );
#ifdef _STRING_CLASS_
BOOLEAN   alphadate( STRING_CLASS & dest, const SYSTEMTIME & sorc );
BOOLEAN   alphatime( STRING_CLASS & dest, const SYSTEMTIME & sorc );
#endif
TCHAR *   ascii_double( double sorc );
TCHAR   * ascii_float( float sorc );
bool      ascii_to_bool( TCHAR * cp );
BOOLEAN   ascii_to_boolean( TCHAR * cp );
TCHAR   * copy_ascii_shot_number( TCHAR * dest, int32 shot_number, int32 slen );
int       asctoint( const TCHAR * sorc );
int32     asctoint32( const TCHAR * sorc );
unsigned long asctoul( const TCHAR * sorc );

#ifdef _STRING_CLASS_
BOOLEAN   autoback_data_dir();
BOOLEAN   autoback_machine( STRING_CLASS & m );
BOOLEAN   autoback_part( STRING_CLASS & m, STRING_CLASS & p );
BOOLEAN   get_autoback_computer();
void      set_autoback_computer( STRING_CLASS & sorc );
#endif

bool      bool_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
void      bool_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, bool b );
BOOLEAN   boolean_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
TCHAR   * boolean_to_ascii( BOOLEAN b );
void      boolean_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, BOOLEAN b );
TCHAR   * bool_to_ascii( bool b );
TCHAR   * category_name( TCHAR * number_to_match );
TCHAR     char_to_tchar( char sorc );
void      char_to_tchar( TCHAR * dest, char * sorc );
void      char_to_tchar( TCHAR * dest, char * sorc, int max_len );
TCHAR   * char_to_tchar( char * sorc );
void      char_to_unicode( wchar_t * dest, TCHAR * sorc );
void      wchar_to_char( char * dest, wchar_t * sorc, int n );

short     charlen( const TCHAR * sorc, short max_len );
#ifdef _STRING_CLASS_
BOOLEAN   choose_wire( HWND sorc, HWND parent, int output_type, int output_index, STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa );
#ifdef _VISI_PARAM_H_
#ifdef _MACHINE_CLASS_
#ifdef _LIMIT_SWITCH_CLASS_
BOOLEAN   choose_wire( HWND sorc, HWND parent, int output_type, int output_index, MACHINE_CLASS * ma, PART_CLASS * pa, PARAMETER_CLASS * pc, LIMIT_SWITCH_CLASS * sw );
#endif
#endif
#endif
#endif
short     compare( const TCHAR * s1, const TCHAR * s2 );
short     compare( const TCHAR * s1, const TCHAR * s2, short max_len );
int       compare_text( const void * sorc1, const void * sorc2 );
#ifdef _ANALOG_SENSOR_
float     convert_analog( BOARD_DATA raw_value, ANALOG_SENSOR & a );
#endif
void      copychars(   LPTSTR dest, const TCHAR * sorc, short n );
void      copyfromstr( LPTSTR dest, const TCHAR * sorc, short n );
TCHAR   * catstring(  TCHAR * dest, const TCHAR * sorc);
TCHAR   * copystring( TCHAR * dest, const TCHAR * sorc );
TCHAR   * copystring( TCHAR * dest, const TCHAR * sorc, int maxlen );
BOOLEAN   copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, TCHAR * wildname, BOOLEAN with_subs, HWND w, int id );
BOOLEAN   copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, BOOLEAN with_subs, HWND w, int id );
BOOLEAN   copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir, TCHAR * wildname, HWND w, int id );
BOOLEAN   copy_all_files( TCHAR * dest_dir, TCHAR * sorc_dir );
TCHAR   * copy_w_char( TCHAR * dest, TCHAR * sorc, TCHAR c );
int32     countchars( TCHAR c, const TCHAR * sorc );
BOOLEAN   create_directory( TCHAR * path );
#ifdef _STRING_CLASS_
BOOLEAN   current_part_name( STRING_CLASS & dest, STRING_CLASS & computer, STRING_CLASS & machine );
#endif
#ifdef _STRUCTS_
int32     current_shot_number( PART_NAME_ENTRY & p );
#endif
void      get_current_password_level( void );
long      current_sec_since1990( void );
int       day_of_the_week( int m, int d, int y );
WORD      days_in_month( WORD month, WORD year );
WORD      days_in_month( SYSTEMTIME & st );
#ifdef _NAME_CLASS_
BOOLEAN   db_to_vdb( NAME_CLASS & sn, int shot_name_index );
#endif
void      dcurve_shutdown();
BOOLEAN   dcurve_startup();

BOOLEAN   directory_exists( TCHAR * path );
WORD      dofyr( WORD month, WORD day, WORD year );
WORD      dofyr( const SYSTEMTIME & st );
short     dow_from_string( TCHAR * day );

bool      double_to_tchar( TCHAR * dest, double x, int max_decimal_places );

double    dups_to_vu( short velocity_units_id, short distance_units_id );
#ifdef _STRUCTS_
#ifdef _TIME_CLASS_
TCHAR *   dynamic_file_name( TIME_CLASS & tsorc, PART_NAME_ENTRY & pn, const TCHAR * sorc );
#endif
#endif
BOOLEAN   empty_directory( TCHAR * dirname );
BOOLEAN   empty_directory( TCHAR * dirname, TCHAR * fname );
void      error_message( short routine_number, short msg_number );
void      error_message( short routine_number, short msg_number, short n );
void      error_message( short routine_number, short msg_number, TCHAR * s );
void      error_message( TCHAR * s,            short msg_number );
void      error_message( TCHAR * s );

BOOLEAN   execute( TCHAR * command_line );
BOOLEAN   execute_and_wait( TCHAR * command_line, TCHAR * current_directory, DWORD ms_to_wait );
BOOLEAN   execute_and_wait( TCHAR * command_line, TCHAR * current_directory );
const TCHAR * exe_directory();

double    extdouble( const TCHAR * sorc, short nof_chars );
double    extdouble( const TCHAR * sorc );
TCHAR   * extfield( TCHAR * dest, TCHAR * sorc, TCHAR delimiter, int32 maxlen );
float     extfloat(  const TCHAR * sorc, short nof_chars );
float     extfloat( const TCHAR * sorc );
#ifdef _VISIPARM_H
BOOLEAN   exthms( SYSTEMTIME & t, const TCHAR * s );
#endif
long      extlong( const TCHAR * sorc, short nof_chars );
long      extlong( const TCHAR * sorc, short nof_chars, int radix );
#ifdef _VISIPARM_H
BOOLEAN   extmdy( SYSTEMTIME & t, const TCHAR * s );
BOOLEAN   extmmddyy( SYSTEMTIME & t, const TCHAR * s );
#endif
short     extshort( const TCHAR * sorc, short nof_chars );
BOOLEAN   file_exists( TCHAR * path );
int32     file_size( TCHAR * path );
BOOLEAN   get_file_write_time( SYSTEMTIME & t,  TCHAR * fname );
BOOLEAN   get_file_write_time( FILETIME & dest, TCHAR * fname );
BOOLEAN   get_file_creation_time( FILETIME & dest, TCHAR * fname );
#ifdef _STRING_CLASS_
#ifdef _TEXT_LIST_CLASS_
BOOLEAN   get_partlist( TEXT_LIST_CLASS & dest, STRING_CLASS & computer, STRING_CLASS & machine );
#endif
BOOLEAN   get_shotsave_semaphore( STRING_CLASS & computer, STRING_CLASS & machine );
BOOLEAN   get_startup_part( STRING_CLASS & machine, STRING_CLASS & part );
#endif
#ifdef _TIME_CLASS_
#ifdef _DB_CLASS_
BOOLEAN   get_time_class_from_db( TIME_CLASS & tc, DB_TABLE & db, FIELDOFFSET offset );
#endif
#ifdef _VDB_CLASS_
BOOLEAN   get_time_class_from_vdb( TIME_CLASS & tc, VDB_CLASS & v, int column_index );
#endif
#endif
void      fillchars( TCHAR * dest, TCHAR c, short n );
TCHAR   * findchar( TCHAR c, TCHAR * sorc, short maxlen );
TCHAR   * findchar( TCHAR c, TCHAR * sorc );
const TCHAR * findchar( TCHAR c, const TCHAR * sorc );
const TCHAR * findchar( TCHAR c, const TCHAR * sorc, short maxlen );
TCHAR   * find_last_char( TCHAR c, TCHAR * sorc );
TCHAR   * find_last_string( const TCHAR * s_to_find, TCHAR * s );
TCHAR   * findstring( const TCHAR * s_to_find, TCHAR * s_to_search );
#ifdef _STRING_CLASS_
void      fill_part_listbox( HWND w, int box, STRING_CLASS & computer, STRING_CLASS & machine );
void      fill_parts_and_counts_listbox( HWND w, int box, STRING_CLASS & computer, STRING_CLASS & machine );
void      fill_parts_and_counts_listbox( HWND w, int box, STRING_CLASS & computer, STRING_CLASS & machine, TCHAR separator );
void      fill_parts_and_shot_count_listbox( HWND w, int box, STRING_CLASS & computer, STRING_CLASS & machine );
void      fill_shot_listbox( HWND w, int listbox_id, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
#endif
void      fix_alphatime( TCHAR * sorc );
void      fix_dde_name( TCHAR * s );
#ifdef _STRING_CLASS_
void      fix_shotname( STRING_CLASS & s );
void      fix_shotname( STRING_CLASS & s, int shot_number_length );
#endif
TCHAR   * fixed_string( double x, int32 left_places, int32 right_places );
TCHAR   * fixed_string( double x, int32 left_places, int32 right_places );
float     float_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
void      float_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, float x );
double    double_from_ini( const TCHAR * fname, const TCHAR * section, TCHAR * key );
void      double_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, double x );

int32     free_megabytes( const TCHAR * root );
void      free_shotsave_semaphore();
void      ftoasc( TCHAR * dest, double x );
void      ftoasc( TCHAR * dest, float x  );
void      getday( TCHAR * dest, short day );
BOOLEAN   get_dcurve_coefficients( float & dest_head_coef, float & dest_rod_coef, short curve_number );
BOOLEAN   get_dcurve_coefficients( double & dest_head_coef, double & dest_rod_coef, int curve_number );
BOOLEAN   get_exe_directory( TCHAR * dest );
TCHAR   * get_ini_string( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
TCHAR   * get_long_ini_string( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
void      put_ini_string( const TCHAR * fname, const TCHAR * section, const TCHAR * key, const TCHAR * sorc );
unsigned long hextoul( const TCHAR * sorc );
short     highest_password_level();
TCHAR   * hhmm( int32 hours, int32 minutes );
#ifdef _STRING_CLASS_
BOOLEAN   increment_shot_name( STRING_CLASS & shot_name, int shot_number_length, int shot_increment );
#endif
void      init_systemtime_struct( SYSTEMTIME & t );
void      insalph( LPTSTR dest, int32 n,  int32 slen, TCHAR c, int32 radix );
void      insalph( LPTSTR dest, double x, int32 slen, TCHAR c );
void      insalph( LPTSTR dest, float x,  int32 slen, TCHAR c );
void      insert_char( TCHAR * sorc, TCHAR c );
void      int32toasc(  LPTSTR dest, int32 n, int32 radix );
TCHAR   * int32toasc( int32 n );
int32     int32_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );
void      int32_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, int32 i );
void      int_to_ascii( TCHAR * dest, int n );
void      int_to_ascii( TCHAR * dest, int n, int decimal_places );
BOOLEAN   is_numeric( TCHAR c );
BOOLEAN   is_numeric( const TCHAR * s );
BOOLEAN   is_hex_char( TCHAR c );
BOOLEAN   is_real_char( TCHAR c );
BOOLEAN   is_removable( TCHAR * path );
BOOLEAN   is_time( TCHAR * s );
int       itou( int sorc );
BOOLEAN   kill_directory( TCHAR * dirname );
void      killtext( TCHAR * s );
void      lower_case( TCHAR & c );
void      lower_case( TCHAR * s );
#ifdef _STRING_CLASS_
BOOLEAN   log_file_string( STRING_CLASS & dest, TCHAR * format, TCHAR * owner, STRING_CLASS & machine, STRING_CLASS & old_part, STRING_CLASS & new_part );
#endif
TCHAR   * maketext( int32 n );
TCHAR   * maketext( const TCHAR * sorc, int32 chars_to_add );
TCHAR   * maketext( const TCHAR * sorc );
TCHAR   * maketext_from_char( char * sorc );
TCHAR   * maketext_from_char( char * sorc, int len );
#ifdef _STRING_CLASS_
BOOLEAN   machine_computer_name( STRING_CLASS & dest, STRING_CLASS & machine_name );
BOOLEAN   machine_exists( STRING_CLASS & computer, STRING_CLASS & machine );
#endif
#ifdef _MACHINE_CLASS_
BOOLEAN   machine_to_hmi( MACHINE_CLASS & m, BOOLEAN need_to_update_parts );
#endif
void      matchcase( TCHAR * dest, const TCHAR * sorc );
BOOLEAN   monitor_is_running( void );
#ifdef _STRING_CLASS_
BOOLEAN   part_exists( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & name );
#endif
#ifdef _STRUCTS_
BOOLEAN   part_exists( PART_NAME_ENTRY & p );
#endif

#ifdef _STRING_CLASS_
TCHAR   * make_downtime_event_string( STRING_CLASS & machine, STRING_CLASS & part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat, TCHAR * worker );
#endif

#ifdef _DB_CLASS_
BOOLEAN   make_downtime_record( DB_TABLE & t, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat, TCHAR * op, STRING_CLASS & part );
#endif

void      maxdouble( double & x, const double y );
void      maxfloat( float & x, const float y );
void      maxint(     int & x, const   int y );
void      maxunsigned( unsigned & x, const unsigned y );
void      maxlong(   long & x, const  long y );
void      maxshort( short & x, const short y );

void      mindouble( double & x, const double y );
void      minfloat( float & x, const float y );
void      minint(     int & x, const   int y );
void      minunsigned( unsigned & x, const unsigned y );
void      minlong(   long & x, const  long y );
void      minshort( short & x, const short y );

#ifdef _VISIPARM_H
void      maxint32( int32 & x, const int32 y );
void      minint32( int32 & x, const int32 y );
#endif

void      move_string_forward( TCHAR * s, int n );
TCHAR   * nextstring( TCHAR * s );

BOOLEAN   is_empty( const TCHAR * s );
bool      is_zero( double x );
BOOLEAN   is_float_zero( float x );
BOOLEAN   not_float_zero( float x );
bool      not_zero( double x );
bool      double_are_equal( double x1, double x2 );

BOOLEAN   parmdata_startup( void );
#ifdef _VISIPARM_H
BITSETYPE parmdata_type( short parm_number );
#endif
short     password_level( TCHAR * pw );
void      pixel_equation( float & a, float & b, int32 min_pixel, int32 max_pixel, float min_value, float max_value );
void      plot_data( HDC dc, RECT & r, float min_x, float max_x, float min_y, float max_y, float * xp, float * yp, int32 n );
void      post_impact_seconds( TCHAR * dest, short n, short ms_per_sample );
void      post_impact_n_ms( short * dest_n, short * dest_ms, TCHAR * sorc );
short     post_impact_n( short true_ms, TCHAR * sorc );
#ifdef _STRING_CLASS_
void      purge_shot_data( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
#endif
short     readline( TCHAR * dest, HANDLE fh, short buflen );
void      register_choose_wire_control();
void      remove_char( TCHAR * sorc );
void      remove_all_occurances( TCHAR * sorc, TCHAR offending_char );
#ifdef _STRING_CLASS_
BOOLEAN   remove_hmi_from_machine( STRING_CLASS & computer, STRING_CLASS & machine );
#endif
BOOLEAN   replace_char_with_null( TCHAR * cp, TCHAR c );
BOOLEAN   replace_char_with_null( TCHAR * cp, const TCHAR * cset );
BOOLEAN   replace_tab_with_null( TCHAR * cp );
void      reverse( LPTSTR s );
void      rjust( LPTSTR s, short n, TCHAR c );
double    round( double sorc, double smallest_increment );
bool      rounded_double_to_tchar( TCHAR * dest, double x );
BOOLEAN   samedate( const SYSTEMTIME & s1, const SYSTEMTIME & s2 );
int32     seconds_difference( SYSTEMTIME & t1, SYSTEMTIME & t2 );
int32     seconds_difference( FILETIME & t1,   FILETIME & t2 );
float     sec_per_vel_unit( short uid );
long      sec_since1990( SYSTEMTIME & t );
BOOLEAN   sensor_list_startup();
void      sensor_list_shutdown();
BOOLEAN   set_exe_directory( TCHAR * new_exe_directory );

#ifdef _STRUCTS_
BOOLEAN   set_current_part_name( PART_NAME_ENTRY & p );
BOOLEAN   set_last_shot_number( PART_NAME_ENTRY & p, int32 new_shot_number );
#endif
#ifdef _STRING_CLASS_
BOOLEAN   set_current_part_name( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa );
//BOOLEAN   set_last_shot_number( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, int32 new_shot_number );
#endif

void      get_shift_number_and_date( short * shift, SYSTEMTIME * shift_date, const SYSTEMTIME & true_date );
short     shift_number( short * shift_dow, const SYSTEMTIME & now );
void      shifts_shutdown();
BOOLEAN   shifts_startup();
SYSTEMTIME & start_of_shift( const SYSTEMTIME & now );

#ifdef _STRING_CLASS_
#ifdef _STRUCTS_
BOOLEAN shot_name( STRING_CLASS & dest, PART_NAME_ENTRY & pn, int shot_number );
#endif
BOOLEAN shot_name( STRING_CLASS & dest, STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa, int shot_number );
int     shot_name_length( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa );
int     shot_count_between_shots( STRING_CLASS & low_shot_name, STRING_CLASS & high_shot_name, int shot_number_length );
#ifdef _STRUCTS_
int     shot_name_length( PART_NAME_ENTRY & pn );
int     shot_name_length( PROFILE_NAME_ENTRY & pn );
#endif
int     shot_number_from_shot_name( STRING_CLASS & name, int shot_number_length );
#endif

#ifdef _STRING_CLASS_
int32     max_parameters_saved( STRING_CLASS & machine );
int32     max_profiles_saved( STRING_CLASS & machine );

#ifdef _VISIPARM_H
BITSETYPE skip_new_shot_notify( STRING_CLASS & machine, FILETIME time_of_shot );
#endif
BOOLEAN   skip_reset_machine( STRING_CLASS & machine );
#endif

void      skip_shutdown( void );
BOOLEAN   skip_startup( void );
bool      start_chars_are_equal( const TCHAR * sorc, const TCHAR * s );
BOOLEAN   strings_are_equal( const TCHAR * s1, const TCHAR * s2, short max_len );
BOOLEAN   strings_are_equal( const TCHAR * s1, const TCHAR * s2 );
void      strip( TCHAR * s );
TCHAR   * subcategory_name( TCHAR * cat_to_match, TCHAR * subcat_to_match );
int       tchar_to_int( TCHAR sorc_tchar );
void      trim( TCHAR * s );

unsigned long digit_to_ul( TCHAR c );
void      ultoascii( TCHAR * dest, unsigned long n, unsigned long radix );
TCHAR   * ultoascii( unsigned long n, unsigned long radix );
TCHAR   * ultoascii( unsigned long n );
void      ultohex( TCHAR * dest,  unsigned long n );
TCHAR   * ultohex( unsigned long n );
char      ul_to_char( unsigned long sorc );
void      ul_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, unsigned long i );
unsigned long ul_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key );

char      unicode_to_char( TCHAR sorc );
void      unicode_to_char( char * dest, TCHAR * sorc );

void      units_shutdown();
#ifdef _VISIPARM_H
BOOLEAN   units_startup();
#endif
TCHAR *   units_name( short uid );
short     units_id( TCHAR * sorc );
#ifdef _VISIPARM_H
BITSETYPE units_type( short uid );
#endif

BOOLEAN   unknown( TCHAR * cp );
TCHAR   * unknown_string();
void      upper_case( TCHAR & c );
void      upper_case( TCHAR * s );
int       utoi( unsigned int sorc );

#ifdef _NAME_CLASS_
void convert_asensor_db_to_new_csv( NAME_CLASS & sorc );
BOOLEAN convert_downtime_from_5_to_7( NAME_CLASS & s );
BOOLEAN convert_runlist_from_5_to_7( NAME_CLASS & s );
#ifdef _DB_CLASS_
#ifdef _SETPOINT_
BOOLEAN extract_setpoints_from_machset_db( SETPOINT_CLASS & sp, DB_TABLE & t );
#ifdef _LIMIT_SWITCH_CLASS_
BOOLEAN extract_setpoints_from_parts_db( SETPOINT_CLASS & sp, LIMIT_SWITCH_POS_CLASS & sw, DB_TABLE & t );
#endif
#endif
#endif
#endif

#ifdef _STRING_CLASS_
BOOLEAN convert_v5_data_dir_to_v7( STRING_CLASS & sorcdatadir );
BOOLEAN v5tov7( STRING_CLASS & computer, BOOLEAN need_to_convert_asensor_db );
BOOLEAN v7_results_to_v8( STRING_CLASS & datadir, HWND ma_tb, HWND pa_tb );
BOOLEAN waitfor_shotsave_semaphor( STRING_CLASS & computer, STRING_CLASS & machine );
#endif

int32     whole_places( float x );
unsigned  mask_from_wire( unsigned wire );
unsigned  wire_from_mask( unsigned mask );
BOOL      writeline( HANDLE fh, TCHAR * sorc );

/*
-------------
Path routines
------------- */
void    append_backslash_to_path( TCHAR * s );
void    append_directory_to_path( TCHAR * dest, const TCHAR * sorc );
void    append_filename_to_path(  TCHAR * dest, const TCHAR * sorc );
TCHAR * catpath( TCHAR * dest, const TCHAR * sorc );
#ifdef _STRING_CLASS_
BOOLEAN root_from_path( STRING_CLASS & s );
#endif
void    remove_backslash_from_path( TCHAR * s );
BOOLEAN drive_exists( TCHAR * path );
BOOLEAN disk_is_in_drive( TCHAR * path );
BOOLEAN path_is_drive_only( TCHAR * s );
BOOLEAN dir_from_command_line( TCHAR * s );    /* Assumes c:\...something.exe... */
BOOLEAN dir_from_filename( TCHAR * s );
TCHAR * filename_from_path( TCHAR * path );

/*
----------------
Profile routines
---------------- */
#ifdef _STRING_CLASS_
BOOLEAN change_shot_name_in_profile( STRING_CLASS & fname, STRING_CLASS & new_shot_name );
#endif

#ifdef _STRUCTS_
BOOLEAN copy_profile( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc, STRING_CLASS & shot_name );
#endif
#ifdef _MULTISTRING_CLASS_
int32   read_parameters_from_parmlist( MULTISTRING_CLASS & dest, STRING_CLASS & filename );
#endif

/*
----------------------
Network info functions
---------------------- */
void   network_get_ip_addr( TCHAR *dest, int type );
int    network_get_ip_port( int type );
void   network_get_ip_addr_from_computername( TCHAR *dest, TCHAR *computername );
int    network_get_computer_type();
void  *socket_to_void_ptr( SOCKET s );
SOCKET void_ptr_to_socket( void *ptr );

/*
--------------
Copypart flags
-------------- */
#define COPY_PART_PARAMETERS      0x0001
#define COPY_PART_CHANNELS        0x0002
#define COPY_PART_MARKS           0x0004
#define COPY_SUBPART_NAMES        0x0008
#define COPY_PART_BASIC           0x0010
#define COPY_PART_ADVANCED        0x0020
#define COPY_PART_USER_POSITIONS  0x0040
#define COPY_PART_REFERENCE_TRACE 0x0080
#define COPY_PART_SURETRAK_SETUP  0x0100
#define COPY_PART_PARAM_NAMES     0x0200
#define COPY_PART_WARMUP_PART     0x0400
#define COPY_PART_DISPLAY_SETUP   0x0800
#define COPY_PART_CHAXIS_LABEL    0x1000
#define COPY_PART_PARAM_SORT      0x2000
#define COPY_PART_LS_DESC         0x4000
#define COPY_PART_LS_POS          0x8000
#define COPY_PART_LS_WIRES        0x10000
#define COPY_PART_PRES_CONTROL    0x20000
#define COPY_PART_SHOT_NAME       0x40000
#define COPY_PART_SHOT_NAME_RESET 0x80000
#define COPY_PART_ALL             0xFFFFF

#ifdef _STRING_CLASS_
#ifdef _VISIPARM_H
BOOLEAN copypart( STRING_CLASS & dest_computer, STRING_CLASS & dest_machine, STRING_CLASS & dest_part, STRING_CLASS & sorc_computer, STRING_CLASS & sorc_machine, STRING_CLASS & sorc_part, unsigned flags );
#endif

#ifdef _MACHINE_CLASS_
BOOLEAN copymachine( STRING_CLASS & computer, STRING_CLASS & machine, MACHINE_CLASS & sorc );
#endif

BOOLEAN update_machine( STRING_CLASS dest_computer, STRING_CLASS dest_machine, STRING_CLASS sorc_computer, STRING_CLASS sorc_machine );
BOOLEAN update_part( STRING_CLASS dest_computer, STRING_CLASS dest_machine, STRING_CLASS dest_part, STRING_CLASS sorc_computer, STRING_CLASS sorc_machine, STRING_CLASS sorc_part );
#endif

#ifdef _NAME_CLASS_
BOOLEAN upsize_table_w_shot_name( NAME_CLASS & sn, int new_shot_name_length );
#endif

#ifdef _STRUCTS_
BOOLEAN copy_part_by_files( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc );
#endif

#ifdef _SERVICE_LOG_CLASS_
void setup_service_log( SERVICE_LOG &log );
void get_time_delta_string( SYSTEMTIME &start, SYSTEMTIME &end, STRING_CLASS &dest );
#endif

/*
----------------
Subs for windows
---------------- */
int32   average_character_width( HDC dc );
int32   character_height( HDC dc );
HFONT   create_font( HDC dc, long width, long height, TCHAR * name );
RECT    desktop_client_rect( void );
void    draw_rectangle( HDC dc, RECT & r, COLORREF color );

int     eb_get_caret_pos( HWND w );
void    eb_select_all( HWND w );
void    eb_select_all( HWND w, int id );
void    eb_set_caret_pos( HWND w, int new_pos );

void    fill_rectangle( HDC dc, RECT & r, COLORREF color );

short   get_lb_count( HWND w );
short   get_lb_count( HWND w, int box );

int32   pixel_width( HDC dc, TCHAR * s );
int32   pixel_width( HDC dc, TCHAR * s, int32 nof_chars );

void    hourglass_cursor( void );

void    lb_select_all( HWND w );
void    lb_select_all( HWND w, int box );

BOOLEAN get_text( TCHAR * dest, HWND w, int max_len );
BOOLEAN get_text( TCHAR * dest, HWND w, int box, int max_len );

double  get_double_from_ebox( HWND w );
double  get_double_from_ebox( HWND w, int box );

int     get_int_from_ebox( HWND w );
int     get_int_from_ebox( HWND w, int box );

void    limit_text( HWND w, int nof_chars );
void    limit_text( HWND w, int box, int nof_chars );

void    restore_cursor( void );

BOOLEAN set_text( HWND w, const TCHAR * sorc );
BOOLEAN set_text( HWND w, int box, const TCHAR * sorc );

BOOLEAN set_current_cb_item( HWND w, TCHAR * item_to_find );
BOOLEAN set_current_cb_item( HWND w, int box, TCHAR * item_to_find );

BOOLEAN set_current_lb_item( HWND w, TCHAR * item_to_find );
BOOLEAN set_current_lb_item( HWND w, int box, TCHAR * item_to_find );

BOOLEAN is_a_listbox( HWND w );
BOOLEAN is_an_editbox( HWND w );
BOOLEAN is_checked( HWND w );
BOOLEAN is_checked( HWND w, int button_id );
BOOLEAN is_previous_instance( TCHAR * class_name, TCHAR * window_title );
void    add_to_menu( UINT id, TCHAR * name );
BOOL    load_menu( HWND w, TCHAR * menuname, short current_password_level, const UINT * bad_ids, int nof_bad_ids );
BOOL    load_menu( HWND w, TCHAR * menuname, short current_password_level );
void    modify_menu_string( UINT id, TCHAR * newname );
void    remove_from_menu( UINT id );

int     resource_message_box( TCHAR * msg, TCHAR * title );
int     resource_message_box( TCHAR * msg, TCHAR * title, UINT boxtype );
int     resource_message_box( HWND w, HINSTANCE instance, UINT msg_id, UINT title_id, UINT boxtype );
int     resource_message_box( HINSTANCE instance, UINT msg_id, UINT title_id, UINT boxtype );
void    resource_message_box( HINSTANCE instance, UINT msg_id, UINT title_id );
TCHAR * resource_string( HINSTANCE instance, UINT resource_id );
BOOLEAN is_changed( HWND w, int id ); /* Edit Box */
BOOLEAN is_changed( HWND w );
void    set_checkbox( HWND w, BOOLEAN need_check );
void    set_checkbox( HWND w, int id, BOOLEAN need_check );
void    set_int_in_ebox( HWND w, int sorc );
void    set_int_in_ebox( HWND w, int box, int sorc );
void    set_listbox_tabs_from_title_positions( HWND parent_window, UINT listbox_id, int * id, int n );
void    set_menu_view_state( BOOLEAN & state, HWND w, UINT menu_choice_id );
void    show_window( HWND w, BOOLEAN is_visible );
void    show_window( HWND w, int id, BOOLEAN is_visible );
void    shrinkwrap( HWND main_window, HWND dialog_window );
void    toggle_menu_view_state( BOOLEAN & state, HWND w, UINT menu_choice_id );

void center_temp_message();
void kill_temp_message();
int  start_winsock();
void temp_message( TCHAR * message, int ms );
void temp_message( TCHAR * message );

#endif

