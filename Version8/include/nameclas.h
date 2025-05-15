#ifndef _NAME_CLASS_
#define _NAME_CLASS_

#ifndef _SUBS_H_
BOOLEAN create_directory( TCHAR * path );
BOOLEAN directory_exists( TCHAR * path );
BOOLEAN empty_directory( TCHAR * dirname );
BOOLEAN empty_directory( TCHAR * dirname, TCHAR * fname );
BOOLEAN file_exists( TCHAR * path );
int32   file_size( TCHAR * path );
BOOLEAN kill_directory( TCHAR * dirname );
#endif

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

class NAME_CLASS : public STRING_CLASS {
public:
/*
------------------------------------------------
There is no backslash at the end of any of these
------------------------------------------------ */
void    add_ending_backslash();
BOOLEAN convert_to_data_dir();
BOOLEAN copyto( STRING_CLASS & dest );
TCHAR * find_extension();
BOOLEAN find_unique_name();
BOOLEAN ensure_file_directory_existance();
BOOLEAN get_current_directory();
BOOLEAN get_data_dir_file_name( const TCHAR * file_name );
BOOLEAN get_data_dir_file_name( STRING_CLASS & computer, const TCHAR * file_name );
BOOLEAN get_display_dir_file_name( STRING_CLASS & computer, const TCHAR * file_name );
BOOLEAN get_display_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, const TCHAR * file_name );
BOOLEAN get_extension( TCHAR * sorc );
BOOLEAN get_extension( STRING_CLASS & sorc );
BOOLEAN get_file_name( TCHAR * sorc );
BOOLEAN get_file_name( STRING_CLASS & sorc );
BOOLEAN get_machine_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, const TCHAR * file_name );
BOOLEAN get_machine_list_file_name( STRING_CLASS & sorc_computer );
BOOLEAN moveto( STRING_CLASS & dest );
BOOLEAN get_part_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, const TCHAR * file_name );
BOOLEAN get_part_results_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, const TCHAR * file_name );
BOOLEAN get_profile_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, const TCHAR * shot_name, const TCHAR * suffix );
BOOLEAN get_profile_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, STRING_CLASS & shot_name, const TCHAR * suffix );
BOOLEAN get_last_write_time( FILETIME & dest );
BOOLEAN get_machine_results_dir_file_name( STRING_CLASS & computer, STRING_CLASS & machine, const TCHAR * file_name );
BOOLEAN get_exe_dir_file_name( const TCHAR * file_name );
BOOLEAN get_exe_dir_file_name( STRING_CLASS & computer, const TCHAR * file_name );
BOOLEAN get_root_name( STRING_CLASS & computer );
BOOLEAN get_stringtable_name( const TCHAR * file_name );
BOOLEAN get_stringtable_name( STRING_CLASS & computer, const TCHAR * file_name );
BOOLEAN get_symphony_directory( STRING_CLASS & computer );
BOOLEAN get_symphony_machine_directory( STRING_CLASS & computer, STRING_CLASS & machine );

void    remove_ending_backslash();
BOOLEAN remove_ending_filename();
BOOLEAN set_current_directory();
BOOLEAN remove_dir_from_file_path();
BOOLEAN remove_extension();
void    replace_extension( TCHAR * new_extension );

inline void operator=( TCHAR sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( const TCHAR * sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( const NAME_CLASS & sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( const STRING_CLASS & sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( int sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( unsigned int sorc ) { STRING_CLASS::operator=( sorc ); }
inline void operator=( double sorc ) { STRING_CLASS::operator=( sorc ); }

inline void operator+=( const TCHAR   sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( const TCHAR * sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( const NAME_CLASS & sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( const STRING_CLASS & sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( int sorc ) { STRING_CLASS::operator+=( sorc ); }
inline void operator+=( unsigned int sorc ) { STRING_CLASS::operator+=( sorc ); }

inline BOOLEAN operator<( const NAME_CLASS & sorc )   { return STRING_CLASS::operator<( sorc ); }
inline BOOLEAN operator<( const STRING_CLASS & sorc ) { return STRING_CLASS::operator<( sorc ); }
inline BOOLEAN operator<( const TCHAR * sorc )        { return STRING_CLASS::operator<( sorc ); }
inline BOOLEAN operator>( const NAME_CLASS & sorc )   { return STRING_CLASS::operator>( sorc ); }
inline BOOLEAN operator>( const STRING_CLASS & sorc ) { return STRING_CLASS::operator>( sorc ); }
inline BOOLEAN operator>( const TCHAR * sorc )        { return STRING_CLASS::operator>( sorc ); }

inline BOOLEAN operator==(const NAME_CLASS & sorc ) { return STRING_CLASS::operator==( sorc ); }
inline BOOLEAN operator==(const STRING_CLASS & sorc ) { return STRING_CLASS::operator==( sorc ); }
inline BOOLEAN operator==(const TCHAR * sorc ) { return STRING_CLASS::operator==( sorc ); }

inline BOOLEAN operator!=(const NAME_CLASS & sorc ) { return STRING_CLASS::operator!=( sorc ); }
inline BOOLEAN operator!=(const STRING_CLASS & sorc ) { return STRING_CLASS::operator!=( sorc ); }
inline BOOLEAN operator!=(const TCHAR * sorc ) { return STRING_CLASS::operator!=( sorc ); }

inline BOOLEAN create_directory() { return ::create_directory(text());  }
inline BOOLEAN delete_file() { if (::DeleteFile(text())) return TRUE; else return FALSE; }
inline BOOLEAN directory_exists() { return ::directory_exists(text());  }
inline BOOLEAN empty_directory() { return ::empty_directory(text());    }
inline BOOLEAN empty_directory(TCHAR * pattern) { return ::empty_directory(text(), pattern); }
inline BOOLEAN file_exists() { return ::file_exists(text());  }
inline int     file_size() { return (int) ::file_size(text());  }
inline BOOLEAN delete_directory() { return kill_directory(text()); }
inline BOOLEAN get_alarmsum_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_results_dir_file_name( computer, machine, part, ALARMSUM_DB ); }
inline BOOLEAN get_asensor_csvname() { return get_data_dir_file_name( ASENSOR_CSV ); }
inline BOOLEAN get_asensor_csvname( STRING_CLASS & computer) { return get_data_dir_file_name(computer, ASENSOR_CSV); }
inline BOOLEAN get_asensor_dbname() { return get_data_dir_file_name( ASENSOR_DB ); }
inline BOOLEAN get_asensor_dbname( STRING_CLASS & computer) { return get_data_dir_file_name(computer, ASENSOR_DB); }
inline BOOLEAN get_auto_save_all_parts_file_name() { return get_exe_dir_file_name(AUTO_SAVE_ALL_PARTS_FILE);  }
inline BOOLEAN get_boards_dbname() { return get_data_dir_file_name( BOARDS_DB ); }
inline BOOLEAN get_boards_dbname( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, BOARDS_DB ); }
inline BOOLEAN get_backup_list_csvname() { return get_exe_dir_file_name( BACKUP_LIST_CSV );  }
inline BOOLEAN get_calc_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, CALC_FILENAME ); }
inline BOOLEAN get_chaxis_label_datname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, CHAXIS_LABEL_DATA_FILE ); }
inline BOOLEAN get_control_message_file_name( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, CONTROL_MESSAGE_FILE );   }
inline BOOLEAN get_control_program_file_name( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, CONTROL_PROGRAM_FILE );   }
inline BOOLEAN get_ctrlimit_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part,  CTRLIMIT_DB ); }
inline BOOLEAN get_default_hmi_settings_csvname() { return get_data_dir_file_name( DEFAULT_HMI_SETTINGS_CSV ); }
inline BOOLEAN get_downcat_dbname() { return get_data_dir_file_name( DOWNCAT_DB ); }
inline BOOLEAN get_downcat_dbname( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, DOWNCAT_DB ); }
inline BOOLEAN get_downscat_dbname() { return get_data_dir_file_name( DOWNSCAT_DB ); }
inline BOOLEAN get_downscatc_dbname() { return get_data_dir_file_name( DOWNSCATC_DB ); }
inline BOOLEAN get_downscatc_dbname( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, DOWNSCATC_DB ); }
inline BOOLEAN get_downtime_dbname( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_results_dir_file_name( computer, machine, DOWNTIME_DB ); }
inline BOOLEAN get_dstat_dbname( STRING_CLASS & computer, STRING_CLASS & machine )  { return get_machine_dir_file_name( computer, machine, DSTAT_DB );  }
inline BOOLEAN get_exe_directory( STRING_CLASS & computer ) { return get_exe_dir_file_name(computer, 0); }
inline BOOLEAN get_external_parameter_settings_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, EXTERNAL_PARAMETER_SETTINGS_CSV ); }
inline BOOLEAN get_external_program_list_csvname() { return get_exe_dir_file_name( EXTERNAL_PROGRAM_LIST_CSV ); }
inline BOOLEAN get_external_shot_name_file_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, EXTERNAL_SHOT_NAME_FILE_CSV ); }
inline BOOLEAN get_ft2_boards_dbname( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, FTII_BOARDS_DB );  }
inline BOOLEAN get_ft2_ctrl_prog_steps_file_name( STRING_CLASS & computer, STRING_CLASS & machine )  { return get_machine_dir_file_name( computer, machine, FTII_CTRL_PROG_STEPS_FILE );  }
inline BOOLEAN get_ft2_editor_settings_file_name( STRING_CLASS & computer, STRING_CLASS & machine )  { return get_machine_dir_file_name( computer, machine, FTII_EDITOR_SETTINGS_FILE );  }
inline BOOLEAN get_ft2_global_settings_file_name( STRING_CLASS & computer, STRING_CLASS & machine )  { return get_machine_dir_file_name( computer, machine, FTII_GLOBAL_SETTINGS_FILE );  }
inline BOOLEAN get_ft2_machine_settings_file_name( STRING_CLASS & computer, STRING_CLASS & machine )  { return get_machine_dir_file_name( computer, machine, FTII_MACHINE_SETTINGS_FILE );  }
inline BOOLEAN get_ft2_part_settings_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )  { return get_part_dir_file_name( computer, machine, part, FTII_PART_SETTINGS_FILE );  }
inline BOOLEAN get_ftcalc_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, FTCALC_CSV ); }
inline BOOLEAN get_ftanalog_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, FTANALOG_DB ); }
inline BOOLEAN get_ftanalog_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, FTANALOG_CSV ); }
inline BOOLEAN get_ftchan_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, FTCHAN_DB ); }
inline BOOLEAN get_good_shot_setup_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, GOOD_SHOT_SETUP_FILE ); }
inline BOOLEAN get_gpvar_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, GPVAR_DB ); }
inline BOOLEAN get_graphlst_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_results_dir_file_name( computer, machine, part, GRAPHLST_DB ); }
inline BOOLEAN get_impact_param_file_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, IMPACT_PARAM_FILE_CSV ); }
inline BOOLEAN get_wire_connector_list_csv( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_dir_file_name( computer, machine, WIRE_CONNECTOR_LIST_CSV );  }
inline BOOLEAN get_limit_switch_desc_file_name(  STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )  { return get_part_dir_file_name( computer, machine, part, LIMIT_SWITCH_DESC_FILE  );  }
inline BOOLEAN get_limit_switch_pos_file_name(   STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )  { return get_part_dir_file_name( computer, machine, part, LIMIT_SWITCH_POS_FILE   );  }
inline BOOLEAN get_limit_switch_wires_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )  { return get_part_dir_file_name( computer, machine, part, LIMIT_SWITCH_WIRES_FILE );  }
inline BOOLEAN get_local_ini_file_name( TCHAR * file_name ) { return get_exe_dir_file_name( file_name );  }
inline BOOLEAN get_machine_ctrl_prog_steps_file_name( STRING_CLASS & computer, STRING_CLASS & machine )  { return get_machine_dir_file_name( computer, machine, MACHINE_CTRL_PROG_STEPS_FILE );  }
inline BOOLEAN get_machine_directory( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_dir_file_name(computer, machine, 0); }
inline BOOLEAN get_machine_results_directory( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_results_dir_file_name(computer, machine, 0); }
inline BOOLEAN get_machset_csvname( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_dir_file_name( computer, machine,  MACHSET_CSV ); }
inline BOOLEAN get_marklist_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, MARKLIST_CSV ); }
inline BOOLEAN get_marklist_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, MARKLIST_DB ); }
inline BOOLEAN get_multipart_runlist_settings_csvname( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_dir_file_name( computer, machine, MULTIPART_RUNLIST_SETUP_CSV ); }
inline BOOLEAN get_multipart_runlist_wires_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS part ) { return get_part_dir_file_name( computer, machine, part, MULTIPART_RUNLIST_WIRES_CSV ); }
inline BOOLEAN get_parmlist_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, PARMLIST_CSV ); }
inline BOOLEAN get_parmlist_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, PARMLIST_DB ); }
inline BOOLEAN get_operator_dbname() { return get_data_dir_file_name( OPERATOR_DB ); }
inline BOOLEAN get_operator_dbname( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, OPERATOR_DB ); }
inline BOOLEAN get_part_directory( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, 0 ); }
inline BOOLEAN get_part_results_directory( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_results_dir_file_name( computer, machine, part, 0 ); }
inline BOOLEAN get_part_display_ini_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, PART_DISPLAY_INI_FILE ); }
inline BOOLEAN get_partset_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, PARTSET_CSV ); }
inline BOOLEAN get_new_password_dbname( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, NEW_PASSWORD_DB ); }
inline BOOLEAN get_new_password_dbname() { return get_data_dir_file_name( NEW_PASSWORD_DB ); }
inline BOOLEAN get_parameter_sort_order_file_name( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa ) { return get_part_dir_file_name( co, ma, pa, PARAMETER_SORT_ORDER_FILE ); }
inline BOOLEAN get_password_dbname() { return get_data_dir_file_name( PASSWORD_DB ); }
inline BOOLEAN get_plookup_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, PLOOKUP_CSV ); }
inline BOOLEAN get_psensor_dbname() { return get_data_dir_file_name( PSENSOR_DB ); }
inline BOOLEAN get_psensor_dbname( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, PSENSOR_DB ); }
inline BOOLEAN get_rodpitch_dbname() { return get_data_dir_file_name( RODPITCH_DB ); }
inline BOOLEAN get_rodpitch_dbname( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, RODPITCH_DB ); }
inline BOOLEAN get_runlist_csvname( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_dir_file_name( computer, machine, RUNLIST_CSV ); }
inline BOOLEAN get_semaphor_dbname( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_dir_file_name( computer, machine, SEMAPHOR_DB ); }
inline BOOLEAN get_setup_sheet_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, SETUP_SHEET_FILE ); }
inline BOOLEAN get_shift_dbname() { return get_data_dir_file_name( SHIFT_DB ); }
inline BOOLEAN get_shift_dbname( STRING_CLASS & computer) { return get_data_dir_file_name( computer, SHIFT_DB ); }
inline BOOLEAN get_shot_events_setup_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, SHOT_EVENTS_CSV); }
inline BOOLEAN get_shot_name_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, SHOT_NAME_CSV ); }
inline BOOLEAN get_shot_name_reset_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, SHOT_NAME_RESET_CSV ); }
inline BOOLEAN get_shotparm_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_results_dir_file_name( computer, machine, part, SHOTPARM_CSV ); }
inline BOOLEAN get_shotparm_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_results_dir_file_name( computer, machine, part, SHOTPARM_DB ); }
BOOLEAN get_static_text_parameters_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
inline BOOLEAN get_stddown_dbname() { return get_data_dir_file_name( STDDOWN_DB ); }
inline BOOLEAN get_status_screen_config_file_name() { return get_exe_dir_file_name( STATUS_SCREEN_CONFIG_FILE_NAME ); }
inline BOOLEAN get_stlimits_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, STLIMITS_DB ); }
inline BOOLEAN get_stlimits_datname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, STLIMITS_DATA ); }
inline BOOLEAN get_stparam_file_name( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, STPARAM_FILE ); }
inline BOOLEAN get_stparam_file_name( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_dir_file_name( computer, machine, STPARAM_FILE ); }
inline BOOLEAN get_stpres_datname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, STPRES_DATA ); }
inline BOOLEAN get_stsetup_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, STSETUP_CSV ); }
inline BOOLEAN get_ststeps_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, STSTEPS_DB ); }
inline BOOLEAN get_subparts_dbname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, SUBPARTS_DB ); }
inline BOOLEAN get_tc_parameter_list_file_name() { return get_exe_dir_file_name( TC_PARAMETERS_FILENAME ); }
inline BOOLEAN get_tc_parameter_list_file_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, TC_PARAMETERS_FILENAME ); }
inline BOOLEAN get_temp_control_program_file_name( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, TEMP_CONTROL_PROGRAM_FILE ); }
inline BOOLEAN get_valve_test_master_file_name( STRING_CLASS & computer ) { return get_data_dir_file_name( computer, VTMASTER_FILE ); }
inline BOOLEAN get_valve_test_master_file_name( STRING_CLASS & computer, STRING_CLASS & machine ) { return get_machine_dir_file_name( computer, machine, VTMASTER_FILE ); }
inline BOOLEAN get_vartype_dbname() { return get_data_dir_file_name( VARTYPE_DB ); }
inline BOOLEAN get_upload_copy_file_name( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa ) { return get_part_dir_file_name( co, ma, pa, UPLOAD_COPY_FILENAME ); }
inline BOOLEAN get_warmup_csvname( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_part_dir_file_name( computer, machine, part, WARMUP_CSV ); }
inline BOOLEAN get_master_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_profile_name( computer, machine, part, CURRENT_MASTER_TRACE_NAME, MASTER_TRACE_SUFFIX ); }
inline BOOLEAN get_master_ii_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part ) { return get_profile_name( computer, machine, part, CURRENT_MASTER_TRACE_NAME, MASTER_TRACE_II_SUFFIX ); }
inline BOOLEAN get_profile_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, TCHAR * shot_name ) { return get_profile_name( computer, machine, part, shot_name,   PROFILE_NAME_SUFFIX ); }
inline BOOLEAN get_profile_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, STRING_CLASS & shot_name ) { return get_profile_name( computer, machine, part, shot_name.text(), PROFILE_NAME_SUFFIX ); }
inline BOOLEAN get_profile_ii_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, TCHAR * shot_name ) { return get_profile_name( computer, machine, part, shot_name,   PROFILE_II_SUFFIX ); }
inline BOOLEAN get_profile_ii_name( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, STRING_CLASS & shot_name ) { return get_profile_name( computer, machine, part, shot_name.text(), PROFILE_II_SUFFIX ); }

};

#endif