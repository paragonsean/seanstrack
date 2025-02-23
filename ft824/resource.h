#define GET_SHOT_NAME_FILE_CONFIG_CHOICE   102
#define MINIMIZE_MENU_CHOICE               103
#define EXIT_MENU_CHOICE                   108
#define FTII_SETUP_MENU_CHOICE             110
#define FTII_TERMINAL_CHOICE               111
#define FTII_CLEAR_TERMINAL_CHOICE         112
#define FTII_CHECK_CONNECTION_CHOICE       113
#define FTII_RECONNECT_CHOICE              114
#define FTII_RESET_MACHINE_CHOICE          115
#define UPLOAD_FLASH_MENU_CHOICE           116
#define FTII_SAVE_LISTBOX_CHOICE           117
#define FTII_OUTPUT_LIST_CHOICE            118
#define FTII_HIRES_SENSOR_TEST_CHOICE      119
#define FTII_LOG_MESSAGES_CHOICE           120
#define FTII_SAVE_SHOT_NAME_FILES_CHOICE   121

#define BOARD_1_PB                  501          /* Main Controls start with this number */
#define MAIN_EBOX                  1002
#define MAIN_LISTBOX_ID            1003

/*
-------------------------
DEACTIVATE_CONTROL_DIALOG
------------------------- */
#define WISH_TO_CONTINUE_STATIC      100

/*
----------------
Auto shot window
---------------- */
#define AUTO_SHOT_MACHINE_NUMBER_TBOX   100
#define AUTO_SHOT_LABEL_TBOX            101

/*
-------
Strings
------- */
#define UNABLE_TO_CONTINUE_STRING        1
#define MONITOR_WINDOW_TITLE_STRING      2
#define LOW_DISK_SPACE_STRING            3
#define NO_SHOTS_SAVED_STRING            4
#define BOARD_MONITOR_EVENT_FAIL_STRING  5
#define BOARD_MONITOR_THREAD_STRING      6
#define BOARD_MONITOR_WAIT_FAIL_STRING   7
#define CREATE_FAILED_STRING             8
#define NOT_CONNECTED_STRING             9
#define CONNECTED_STRING                10
#define CONTROL_ACTIVE_STRING           11
#define CONTROL_FATAL_STRING            12
#define CONTROL_WARNING_STRING          13
#define SAVING_STRING                   14
#define NO_PROGRAM_STRING               15
#define UNKNOWN_STRING                  16

/*
-------------------
Fatal Error Strings
------------------- */
#define STACK_OVERFLOW_STRING           17
#define BLOCK_TIMEOUT_STRING            18
#define INVALID_BLOCK_STRING            19
#define BLOCK_EXECUTION_ERROR_STRING    20
#define PROGRAM_STOPPED_BY_USER_STRING  21
#define TEST_FATAL_ERROR_STRING         22
/*
---------------
Warning Strings
--------------- */
#define VAC_ABORT_POS_TOO_LONG_STRING     23
#define BLOCK_TIMEOUT_WARNING_STRING      24
#define FILL_TEST_ABORT_STRING            25
#define MASTER_ENABLE_DROPPED_OUT_STRING  26
#define VAC_ABORT_ZSPEED_STRING           27
#define TEST_WARNING_STRING               28

#define SHOTSAVE_WAIT_FAIL_STRING       29
#define SHOTSAVE_EVENT_FAIL_STRING      30
#define SHOTSAVE_THREAD_STRING          31
#define UNABLE_TO_COMPLY_STRING         32
#define N0_NEW_AUTO_NULL_STRING         33

#define MANUAL_NULLING_STRING           34
#define AUTO_NULL_BEGIN_STRING          35
#define AUTO_NULL_FAIL_STRING           36
#define AUTO_NULL_SUCCESS_STRING        37
#define NULL_ALREADY_GOOD_STRING        38
#define UNUSABLE_NULL_LEVEL_STRING      39

#define NOT_MONITORED_STRING            40
#define CONNECTING_STRING               41
#define NO_MASTER_ENABLE_STRING         42
#define NO_CONNECT_STRING               43
#define DISCONNECTING_STRING            44
#define NO_WARMUP_PART_STRING           45
#define CANT_OPEN_FILE_STRING           46
#define NEW_SETUP_STRING                47
#define NAK_RECEIVED_STRING             48
#define UPLOAD_FAIL_STRING              49
#define REPEATED_STRING                 50
#define MISSING_STRING                  51
#define PACKET_STRING                   52
#define SAVING_SHOT_STRING              53
#define READY_STRING                    54
#define PACKET_ERROR_STRING             55
#define BEGIN_UPLOAD_STRING             56
#define UPLOAD_COMPLETE_STRING          57
#define CYCLE_START_STRING              58
#define TIMEOUT_STRING                  59
#define ZERO_STROKE_LENGTH_STRING       60

#define ON_STRING                       61
#define OFF_STRING                      62
#define NO_UPLOAD_FILE_STRING           63

#define CONTROL_INPUT_18_STRING         64
#define CONTROL_INPUT_19_STRING         65
#define CONTROL_INPUT_20_STRING         66
#define CONTROL_INPUT_21_STRING         67
#define CONTROL_INPUT_22_STRING         68
#define CONTROL_INPUT_23_STRING         69
#define CONTROL_INPUT_24_STRING         70
#define CONTROL_INPUT_25_STRING         71
#define CONTROL_INPUT_26_STRING         72
#define CONTROL_INPUT_27_STRING         73
#define CONTROL_INPUT_28_STRING         74
#define CONTROL_INPUT_29_STRING         75
#define RESTARTED_STRING                76
#define SHUTTING_DOWN_MONITOR_STRING    77
#define SHUTTING_DOWN_SHOTSAVE_STRING   78
#define SHUTTING_DOWN_REMOTE_STRING     79
#define SHOTSAVE_NOT_RUNNING_STRING     80
#define TURN_ON_ALL_OUTPUTS_STRING      81
#define TURN_OFF_ALL_OUTPUTS_STRING     82
#define NO_RESPONSE_STRING              83
#define BOARD_STRING                    84
#define ADDRESS_STRING                  85
#define COM_STATUS_STRING               86
#define MACHINE_STRING                  87
#define PART_STRING                     88
#define MONITOR_STATUS_STRING           89
#define LOW_PW_LEVEL_STRING             90
#define INVALID_SHOT_NAME_STRING        91

/*
---------------------
CURRENT_VALUES_DIALOG
--------------------- */
#define MAX_POSITION_RESET_BUTTON       109
#define SAVE_TOTAL_STROKE_BUTTON        110
#define RESET_FT_POSITION_BUTTON        119
#define LOG_FILE_SETUP_PB               120
#define LOG_FILE_VIEW_PB                121
#define LOG_FILE_KILL_PB                122
#define LOG_FILE_ZIP_XBOX               123

#define FT_CURRENT_VELOCITY_TBOX        201
#define FT_CURRENT_POSITION_TBOX        202
#define VELOCITY_UNITS_TBOX             203
#define POSITION_UNITS_TBOX             204
#define MAX_POSITION_TBOX               205
#define WAITING_FOR_TBOX                206

#define VOLTS_1_TBOX                    210
#define VOLTS_2_TBOX                    211
#define VOLTS_3_TBOX                    212
#define VOLTS_4_TBOX                    213
#define VOLTS_5_TBOX                    214
#define VOLTS_6_TBOX                    215
#define VOLTS_7_TBOX                    216
#define VOLTS_8_TBOX                    217

#define CH_17_20_XBOX                   222

#define FT_A1_XBOX                      223
#define FT_A2_XBOX                      224
#define FT_A3_XBOX                      225
#define FT_A4_XBOX                      226

#define FT_POS_THERM                    228
#define FT_VEL_THERM                    229

#define FT_A1_THERM                     230
#define FT_A2_THERM                     231
#define FT_A3_THERM                     232
#define FT_A4_THERM                     233
#define FT_A5_THERM                     234
#define FT_A6_THERM                     235
#define FT_A7_THERM                     236
#define FT_A8_THERM                     237

#define CURRENT_STEP_NUMBER_TBOX         245
#define FATAL_ERROR_TBOX                 246
#define CONTROL_STATE_TBOX               247
#define ERROR_MESSAGE_TBOX               248

#define CYCLE_START_TBOX                 249
#define SHOT_IN_PROGRESS_TBOX            250
#define SHOT_COMPLETE_TBOX               251
#define ZERO_SPEED_TBOX                  252
#define FOLLOWING_OVERFLOW_TBOX          253

#define START_SHOT_TBOX                  254
#define FOLLOW_THROUGH_TBOX              255
#define RETRACT_TBOX                     256
#define JOG_SHOT_TBOX                    257
#define MANUAL_MODE_TBOX                 258

#define AT_HOME_TBOX                     259
#define OK_FOR_VAC_FAST_TBOX             260
#define VACUUM_ON_TBOX                   261
#define LOW_IMPACT_TBOX                  262
#define FILL_TEST_TBOX                   263

#define SLOW_SHOT_TBOX                   264
#define ACCUMULATOR_TBOX                 265
#define LS1_TBOX                         266
#define LS2_TBOX                         267
#define LS3_TBOX                         268
#define LS4_TBOX                         269
#define LS5_TBOX                         270
#define LS6_TBOX                         271
#define MIN_VOLTS_TBOX                   280
#define MAX_VOLTS_TBOX                   281

#define CH_1_TBOX                        286
#define CH_2_TBOX                        287
#define CH_3_TBOX                        288
#define CH_4_TBOX                        289
#define CH_5_TBOX                        290
#define CH_6_TBOX                        291
#define CH_7_TBOX                        292
#define CH_8_TBOX                        293
#define FLASH_VERSION_TBOX               294
#define PRESSURE_CONTROL_STATIC          295
#define CURRENT_MODE_TBOX                296
#define ERROR_LOG_TBOX                   297
#define UPLOAD_FILE_NAME_TBOX            298
#define TOGGLE_IO_DISPLAY_PB             299
#define FT2_MODE_UP_PB                   300
#define FT2_MODE_DOWN_PB                 301
#define BINARY_VALVE_STATIC              302
#define INPUT_1_OPTO                     400
#define INPUT_2_OPTO                     401
#define INPUT_3_OPTO                     402
#define INPUT_4_OPTO                     403
#define OUTPUT_1_OPTO                    404
#define OUTPUT_2_OPTO                    405
#define OUTPUT_3_OPTO                    406
#define OUTPUT_4_OPTO                    407
#define OPTO_TOOLTIP_1                   408
#define OPTO_TOOLTIP_17                  424
#define COMM_STATUS_STATIC               425
#define COMM_STATUS_TB                   426
#define MONITOR_STATUS_STATIC            427
#define MONITOR_STATUS_TB                428
#define CV_VEL_STATIC                    429
#define CV_POS_STATIC                    430
#define CV_CHAN_STATIC                   431
#define CV_VOLTS_STATIC                  432
#define MAX_POS_STATIC                   433
#define CV_CUR_STEP_STATIC               434
#define CV_INPUTS_STATIC                 435
#define CV_OUTPUTS_STATIC                436
#define CV_STATUS_STATIC                 437
#define CV_CTL_STATE_STATIC              438
#define UPLOAD_FILE_STATIC               439

/*
-------------------------
TEST_ALARM_OUTPUTS_DIALOG
------------------------- */
#define TOGGLE_ALL_OUTPUTS_XBOX          100
#define MONITOR_OUTPUT_1_OPTO            101  /* These 8 need to be in order */
#define MONITOR_OUTPUT_2_OPTO            102
#define MONITOR_OUTPUT_3_OPTO            103
#define MONITOR_OUTPUT_4_OPTO            104
#define CONTROL_OUTPUT_1_OPTO            105
#define CONTROL_OUTPUT_2_OPTO            106
#define CONTROL_OUTPUT_3_OPTO            107
#define CONTROL_OUTPUT_4_OPTO            108
#define ALARM_TEST_STATIC                109
#define MON_OUTPUTS_STATIC               110
#define CTL_OUTPUTS_STATIC               111
#define CTL_OPTO_1_STATIC                112
#define CTL_OPTO_2_STATIC                113
#define CTL_OPTO_3_STATIC                114
#define CTL_OPTO_4_STATIC                115
#define MON_OPTO_1_STATIC                116
#define MON_OPTO_2_STATIC                117
#define MON_OPTO_3_STATIC                118
#define MON_OPTO_4_STATIC                119
#define CLICK_LED_STATIC                 120

/*
-----------
MAIN_WINDOW
----------- */
#define STATUS_WINDOW_ID                1000
#define TERMINAL_WINDOW_ID              1001
#define F2_RADIO                        1007
#define F3_RADIO                        1008
#define F4_RADIO                        1009
#define F5_RADIO                        1010
#define F6_RADIO                        1011
#define F7_RADIO                        1012
#define F8_RADIO                        1013
#define F9_RADIO                        1014
#define F10_RADIO                       1015

/*
------------------------
COMPARE_VARIABLES_DIALOG
------------------------ */
#define CV_LB                   100
#define CV_GET_LIST_1_PB        101
#define CV_GET_LIST_2_PB        102
#define CV_SHOW_DIF_XB          103
#define CV_SAVE_VARIABLES_PB    104
#define CV_RESTORE_VARIABLES_PB 105

#define CV_GETTING_TB          201
#define CV_CURRENT_VARIABLE_TB 202
#define CV_OF_TB               203

/*
---------------------
Keep these sequential
--------------------- */
#define CV_NUMBER_TB           204
#define CV_LIST_1_TB           205
#define CV_LIST_2_TB           206
#define CV_DESCRIPTION_TB      207

/*
--------------
SET_DAC_DIALOG
-------------- */
#define PLUS_5_VOLTS_PB                 1005
#define MINUS_5_VOLTS_PB                1006
#define SEND_TO_BOARD_PB                1007
#define DAC_VOLTS_EBOX                  1004

#define DAC_COMMAND_TBOX                1102
#define DAC_COMMAND_THERM               1202

#define COMMAND_VOLTS_TBOX              1103
#define COMMAND_VOLTS_THERM             1203

#define LVDT_FEEDBACK_TBOX              1104
#define LVDT_FEEDBACK_THERM             1204

#define LVDT_RAW_FEEDBACK_TBOX          1105
#define LVDT_RAW_FEEDBACK_THERM         1205
#define SET_DAC_STATIC                  1206
#define CMD_STATIC                      1207
#define VOLTS_STATIC                    1208
#define MINUS_10_STATIC                 1209
#define PLUS_10_STATIC                  1210
#define LVDT_FEEDBACK_STATIC            1211
#define MEAS_CMD_STATIC                 1212
#define RAW_LVDT_STATIC                 1213

/*
------------------
New Rev D controls
------------------ */
#define LVDT_BOARD_LEVEL_OFFSET_EBOX    1300
#define LVDT_DIGITAL_POT_EBOX           1301
#define LVDT_DIGITAL_POT_SPIN           1303
#define MAIN_VALVE_TYPE_TBOX            1305
#define PILOT_VALVE_TYPE_TBOX           1306
#define CHANGE_VALVE_TYPE_PB            1307
#define OPEN_VALVE_LOOP_XBOX            1308
#define AMP_1_RADIO                     1309
#define AMP_2_RADIO                     1310
#define DEMODULATOR_PHASE_EBOX          1311
#define DEMODULATOR_PHASE_SPIN          1312
#define OFFSET_VOLTAGE_STATIC           1313
#define OFFSET_VOLT_RANGE_STATIC        1314
#define SPAN_STATIC                     1315
#define SPAN_RANGE_STATIC               1316

/*
---------------
FT2 TEST DIALOG
--------------- */
#define TEST_7_RADIO                    100
#define TEST_8A_RADIO                   101
#define TEST_8B_RADIO                   102
#define TEST_9A_RADIO                   103
#define TEST_9B_RADIO                   104
#define TEST_10A_RADIO                  105
#define TEST_10B_RADIO                  106
#define TEST_11A_RADIO                  107
#define TEST_11B_RADIO                  108
#define TEST_11C_RADIO                  109
#define TEST_18A_RADIO                  110
#define TEST_18B_RADIO                  111
#define TEST_18C_RADIO                  112
#define TEST_19_RADIO                   113

/*
---------------
SET_NULL_DIALOG
--------------- */
#define NULL_VOLTS_TBOX                  100
#define AUTO_NULL_PB                     101
#define MANUAL_NULL_PB                   102
#define SET_NULL_REVERT_PB               103
#define SET_NULL_SAVE_PB                 104
#define SET_NULL_MSG_TBOX                105
#define MANUAL_INSTRUCTIONS_TBOX         106
#define SET_NULL_VOLTS_STATIC            107
#define NULL_VOLTAGE_STATIC              108

/*
-----------------------
GLOBAL_PARAMETER_DIALOG
----------------------- */
#define NULL_DAC_EDITBOX                 100
#define LVDT_DAC_EDITBOX                 101
#define JOG_SHOT_VALVE_EDITBOX           102
#define RETRACT_VALVE_EDITBOX            103
#define FT_VALVE_EDITBOX                 104
#define MIN_LI_EDITBOX                   105
#define ZERO_SPEED_CHECK_EDITBOX         106
#define MAX_VELOCITY_EDITBOX             107
#define NULL_DAC_LABEL                   114
#define LVDT_DAC_LABEL                   115
#define JOG_SHOT_VALVE_LABEL             116
#define RETRACT_VALVE_LABEL              117
#define FT_VALVE_LABEL                   118
#define ZERO_SPEED_CHECK_LABEL           119
#define MIN_LI_LABEL                     120
#define MAX_VELOCITY_LABEL               121
#define SAVE_CHANGES_BUTTON              134
#define CANCEL_CHANGES_BUTTON            135
#define GLOBAL_DIST_UNITS_XBOX           136
#define PC_DITHER_FREQ_EB                139
#define PC_DITHER_FREQ_LABEL             140
#define PC_DITHER_AMPL_EB                141
#define PC_DITHER_AMPL_LABEL             142
#define VC_DITHER_FREQ_EB                143
#define VC_DITHER_FREQ_LABEL             144
#define VC_DITHER_AMPL_EB                145
#define VC_DITHER_AMPL_LABEL             146
#define NULL_DAC_2_EB                    147
#define NULL_DAC_2_LABEL                 148
#define LVDT_DAC_2_EB                    149
#define LVDT_DAC_2_LABEL                 150
#define PC_AXIS2_LABEL                   151
/*
-----------------
VALVE_TEST_DIALOG
----------------- */
#define SAVE_AS_MASTER_TRACE_PB          100
#define Y_AXIS_LINE                      101
#define VALVE_TEST_MAX_Y_XBOX            102
#define VALVE_TEST_MIN_Y_XBOX            103
#define CURRENT_DIGITAL_POT_EBOX         104
#define CURRENT_DIGITAL_POT_SPIN         105
#define DIGITAL_POT_STATIC               106
#define VERT_VOLTS_STATIC                107
#define VALVE_TEST_STATIC                108

/*
-------------------
CONTROL_MODS_DIALOG
------------------- */
#define NORMAL_STEP_RADIO                140
#define ALT_STEP_RADIO                   141
#define VOLTAGE_STEP_TBOX                142
#define VOLTAGE_EBOX                     143
#define ALT_STEP_TBOX                    144
#define SLOW_SHOT_VELOCITY_EBOX          145
#define ACCELERATION_EBOX                146
#define ACCUMULATOR_ON_POSITION_EBOX     147
#define PARK_VOLTAGE_STEP_TBOX           148
#define PARK_VOLTAGE_EBOX                149
#define SLOW_SHOT_VELOCITY_STATIC        160
#define ACCELERATION_STATIC              161
#define ACCUMULATOR_ON_POSITION_STATIC   162
#define VALVE_START_STATIC               163
#define GEN_STEP_1_STATIC                164
#define VOLTAGE_1_STATIC                 165
#define GEN_STEP_2_STATIC                166
#define VOLTAGE_2_STATIC                 167
#define PARK_VOLTAGE_STATIC              168
#define ALTERNATE_STEP_STATIC            169
#define GEN_STEP_3_STATIC                170

/*
-----------------------------
CHOOSE_MONITOR_MACHINE_DIALOG
----------------------------- */
#define CHOOSE_MACHINE_LB                100
#define CHOOSE_MACHINE_STATUS_LABEL      101
#define CHOOSE_MACHINE_STATUS_LB         102
#define CHOOSE_MONITOR_MA_STATIC         103

/*
------------------------
Choose Valve Type Dialog
------------------------ */
#define MAIN_VALVE_LB  100
#define PILOT_VALVE_LB 101

/*
-----------------------------
NO_MACHINES_TO_MONITOR_DIALOG
----------------------------- */
#define NO_MACHINES_STATIC    100
#define MUST_BE_SETUP_STATIC  101
#define MUST_SHARE_DIR_STATIC 102

/*
-------------------
Upload Flash Dialog
------------------- */
#define TOTAL_BYTES_TBOX     100
#define CURRENT_BYTES_TBOX   101
#define UPLOAD_STATUS_TBOX   102
#define EXIT_RADIO           103

/*
------------------
OUTPUT_LIST_DIALOG
------------------ */
#define OUTPUT_LIST_LB 100

/*
----------------------
SHUTDOWN_STATUS_DIALOG
---------------------- */
#define SHUTDOWN_STATUS_LB 100

/*
--------------------------
Highres_sensor_test_dialog
-------------------------- */
#define START_SHOT_WIRE_EBOX  100
#define FOLLOW_THRU_WIRE_EBOX 101
#define RETRACT_WIRE_EBOX     102
#define CYCLE_TIME_EBOX       103
#define FOLLOW_THRU_TIME_EBOX 104
#define RETRACT_TIME_EBOX     105
#define RETRACT_END_TIME_EBOX 106

#define START_SHOT_WIRE_TBOX  200
#define FOLLOW_THRU_WIRE_TBOX 201
#define RETRACT_WIRE_TBOX     202

/*
---------------------
Upload_aborted_dialog
--------------------- */
#define ABORTED_MACHINE_NAME_TBOX  100
#define PREV_UPLOAD_ABORTED_STATIC 101
#define OK_TO_RETRY_STATIC         102
#define UA_MACHINE_STATIC          103

/*
--------
Hot Keys
-------- */
#define F2_KEY          912
#define F3_KEY          913
#define F4_KEY          914
#define F5_KEY          915
#define F6_KEY          916
#define F7_KEY          917
#define F8_KEY          918
#define F9_KEY          919
#define F10_KEY         920
#define EXIT_PB         921
#define ALT_F7_KEY      922
