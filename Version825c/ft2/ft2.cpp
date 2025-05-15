#include <windows.h>
#include <ddeml.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\autoback.h"
#include "..\include\array.h"
#include "..\include\dbclass.h"
#include "..\include\evclass.h"
#include "..\include\external_parameter.h"
#include "..\include\fileclas.h"
#include "..\include\fifo.h"
#include "..\include\fontclas.h"
#include "..\include\computer.h"
#include "..\include\iniclass.h"
#include "..\include\limit_switch_class.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\msgclas.h"
#include "..\include\optocontrol.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\rectclas.h"
#include "..\include\setpoint.h"
#include "..\include\stparam.h"
#include "..\include\stringtable.h"
#include "..\include\structs.h"
#include "..\include\stpres.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\menuctrl.h"
#include "..\include\statics.h"

#include "..\include\ftii.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_file.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

#define _MAIN_
#include "..\include\chars.h"
#include "extern.h"
#include "resource.h"
#include "shotsave.h"
#include "..\include\events.h"

/*
--------------------------------------------------------------------------
The following indexes can be used with current_lvdt_feedback_index(), etc.
-------------------------------------------------------------------------- */
static const int DAC_1 = 0;
static const int DAC_2 = 1;

TCHAR MyClassName[]       = TEXT( "FASTRAK2" );
TCHAR TerminalClassName[] = TEXT( "FT2TERMINAL" );
TCHAR StatusClassName[]   = TEXT( "FT2STATUS" );

HACCEL  AccelHandle;
TCHAR   AccelName[] = TEXT("HOT_KEYS");

LISTBOX_CLASS ShutdownStatusListbox;
WINDOW_CLASS  ShutdownStatusDialog;

LISTBOX_CLASS OutputListbox;
WINDOW_CLASS OutputListDialog;
WINDOW_CLASS NullStatusMessage;
WINDOW_CLASS ManualInfoTbox;
static const int NOF_OPTOS = 4;
static WINDOW_CLASS InputOpto[NOF_OPTOS];
static WINDOW_CLASS OutputOpto[NOF_OPTOS];

static const int NOF_TEST_ALARM_OPTOS = 8;
static WINDOW_CLASS TestAlarmOpto[NOF_TEST_ALARM_OPTOS];

MENU_CONTROL_ENTRY MainMenu;
MENU_CONTROL_ENTRY TerminalMenu;

FONT_LIST_CLASS    FontList;
STRINGTABLE_CLASS  StringTable;

bool         ManualNullInProgress = false;

HWND       * Dnu = 0;
static HWND  ToolTipWindow  = 0;

/*
------------------------------------------------------------------------------------------
If I am started minimized all the get_rect calls fail so I put the main window rects here.
------------------------------------------------------------------------------------------ */
RECTANGLE_CLASS MainWindowClientRect;
RECTANGLE_CLASS MainWindowMoveRect;

static TCHAR DotMsg[]           = TEXT( ".MSG" );
static TCHAR DotTxt[]           = TEXT( ".TXT" );
static TCHAR OffString[]        = TEXT( " OFF" );
static TCHAR OnString[]         = TEXT( " ON" );
static TCHAR PoundString[]      = TEXT( "#" );
static TCHAR ResetString[]      = TEXT( "RESET" );
static TCHAR TString[]          = TEXT( "T" );
static TCHAR TwoSpacesString[]  = TEXT( "  " );
static TCHAR SpacePoundString[] = TEXT( " #" );
static TCHAR ZeroString[]       = TEXT( "0" );

static TCHAR ChannelModeSetString[] = TEXT( "V429=" );
static TCHAR ConfigWordMsg[]  = TEXT( "V313_" );
static TCHAR ConfigWord2Msg[] = TEXT( "V314_" );
static int   ConfigWordMsgLen = 5;

static TCHAR OpenValve_1_Loop[]     = TEXT( "OC10=1" );
static TCHAR CloseValve_1_Loop[]    = TEXT( "OC10=0" );
static TCHAR OpenValve_2_Loop[]     = TEXT( "OC11=1" );
static TCHAR CloseValve_2_Loop[]    = TEXT( "OC11=0" );
static TCHAR ClearControlFileDate[] = TEXT( "V447=0" );
static TCHAR HaltString[]           = TEXT( ".H" );

static TCHAR CycleString[]  = TEXT( "Cycle" );
static INT   CycleStringLen = 5;
static TCHAR FatalString[]  = TEXT( "Fatal" );
static INT   FatalStringLen = 5;
static TCHAR MainMenuWindowName[]     = TEXT( "FT2MainMenu" );
static TCHAR NoFileString[]           = TEXT( "No Upload File!" );
static INT   NoFileStringLen          = 15;
static TCHAR TerminalMenuWindowName[] = TEXT( "FT2TerminalMenu" );
static TCHAR WarningString[]  = TEXT( "Warning" );
static INT   WarningStringLen = 7;
static TCHAR ZeroPositionCmd[]        = TEXT( ".UA1=0" );

static STRING_CLASS WireString;
static STRING_CLASS StartScreen;

static TCHAR ArialFontName[] = TEXT( "Arial" );
static HFONT MyFont = INVALID_FONT_HANDLE;

static AUTO_BACKUP_CLASS AutoBack;

FTII_VAR_ARRAY FtiiAutoNull;
FTII_VAR_ARRAY CurrentMachineVars;
HPEN           MasterPen;

static FIFO_CLASS ErrorLog;

static bool    HaveValveMasterData = false;
static int     ValveMasterData[2000];
static int     ValveMasterMax = 1;
static int     ValveMasterMin = 0;

static int     ValveTestData[2000];
static int     ValveTestMax = 1;
static int     ValveTestMin = 0;

/*
--------------------------------------------
Part variables for the current values screen
-------------------------------------------- */
static bool HasPressureControl = false;

/*
--------------------------------------
Globals for the CompareVariablesDialog
-------------------------------------- */
static TCHAR CVListFileName[] = TEXT( "compare_variables_list.csv" );
static const int NOF_FT2_VARIABLES = 512;
static const int LIST_1_INDEX = 0;
static const int LIST_2_INDEX = 1;
static int           CurrentListIndex = LIST_1_INDEX;
static STRING_CLASS  BoardVariable[NOF_FT2_VARIABLES][2];
static STRING_CLASS  BoardVariableDescription[NOF_FT2_VARIABLES];
static int           CurrentBoardVariable = 0;
static BOOLEAN       SendingBoardVariableRequests = FALSE;
static BOOLEAN       ShowDifOnly = FALSE;
static LISTBOX_CLASS CVLb;
static WINDOW_CLASS  CVGettingTb;
static WINDOW_CLASS  CVCurrentVariableTb;
static WINDOW_CLASS  CVofTb;

/*
----------------------------------
Hires Sensor Test Dialog Variables
---------------------------------- */
static HWND         Start_Shot_Tbox_Handle  = 0;
static HWND         Follow_Thru_Tbox_Handle = 0;
static HWND         Retract_Tbox_Handle     = 0;
static TCHAR        HiresSensorConfigFile[] = TEXT( "hires_sensor_test_config.csv" );
static int32        HIRES_SENSOR_START_SHOT_WIRE_INDEX  = 0;
static int32        HIRES_SENSOR_FOLLOW_THRU_WIRE_INDEX = 1;
static int32        HIRES_SENSOR_RETRACT_WIRE_INDEX     = 2;
static int32        HIRES_SENSOR_CYCLE_TIME_INDEX       = 3;
static int32        HIRES_SENSOR_FOLLOW_THRU_TIME_INDEX = 4;
static int32        HIRES_SENSOR_RETRACT_TIME_INDEX     = 5;
static int32        HIRES_SENSOR_RETRACT_END_TIME_INDEX = 6;
static int32        HIRES_SENSOR_SETPOINT_COUNT         = 7;

static const int    NO_SHOT_PERIOD     = 0;
static const int    START_SHOT_PERIOD  = 1;
static const int    FOLLOW_THRU_PERIOD = 2;
static const int    RETRACT_PERIOD     = 3;
static const int    END_OF_SHOT_PERIOD = 4;
static const int    NOF_PERIODS        = 5;

static int          CurrentPasswordLevel     = 0;
static int          ServiceToolsGrayLevel    = 0;
static int          CurrentHiresSensorPeriod = -1;
static int          HiresSensorPeriodMs[NOF_PERIODS] = { 0, 0, 0, 0, 0 };  /* The zeroth is not used */
static int          HiresSensorStartShotWire  = NO_WIRE;
static int          HiresSensorFollowThruWire = NO_WIRE;
static int          HiresSensorRetractWire    = NO_WIRE;

const UINT          HIRES_SENSOR_TEST_TIMER_ID  = 4;
const UINT          REMOTE_MONITOR_TIMER_ID     = 3;
const UINT          CLOSING_TIMER_ID            = 2;
const UINT          RESUME_TIMER_ID             = 1;

static bool         WaitingToResume    = false;
static bool         ClosingAfterRemote = false;
static bool         ShutdownAnyway     = false;

static WINDOW_CLASS ResumeDialog;
static WINDOW_CLASS Ft2TestDialog;
static HWND         SwitchChannelsXbox = 0;
static HWND         AbortedMachineNameTbox = 0;

/*
------------------------------------------------------------------------------------
When I send a MONITOR_SETUP_INDEX dde message I need to ignore it. This is done only
if I am a remote monitor.
------------------------------------------------------------------------------------ */
static int MyMonitorSetupCount = 0;

/*
------------------------------------------------------------------------
This is to tell set_current_window to send a wm_close to the main window
------------------------------------------------------------------------ */
static bool NeedToCloseMainWindow = false;

/*
-----------------------------------------------------------------------------
This is to tell the current values dialog that the analogs all need refreshed
----------------------------------------------------------------------------- */
static bool NeedToRefreshAnalogs = true;

/*
------
Colors
------ */
static HBRUSH GreenBackgroundBrush  = 0;
static HBRUSH OrangeBackgroundBrush = 0;
static HBRUSH RedBackgroundBrush    = 0;

/*
-------
Buttons
------- */
const int NofButtons = 9;
RESOURCE_TEXT_ENTRY Button[NofButtons] = {
    { F2_RADIO,   TEXT("F2_RADIO") },
    { F3_RADIO,   TEXT("F3_RADIO") },
    { F4_RADIO,   TEXT("F4_RADIO") },
    { F5_RADIO,   TEXT("F5_RADIO") },
    { F6_RADIO,   TEXT("F6_RADIO") },
    { F7_RADIO,   TEXT("F7_RADIO") },
    { F8_RADIO,   TEXT("F8_RADIO") },
    { F9_RADIO,   TEXT("F9_RADIO") },
    { F10_RADIO,  TEXT("F10_RADIO") }
    };

/*
------------------------------------------
List of machines for UPLOAD_ABORTED_DIALOG
------------------------------------------ */
TEXT_LIST_CLASS UploadAbortedList;

/*
----------------------
Current Values Globals
---------------------- */
static BOOLEAN  NeedRefresh    = TRUE;
static int      Max_Position   = 0;
static int      Max_Velocity   = 0;
static unsigned Control_Status = 0;
static unsigned Monitor_Status = 0;
static unsigned LastInputBits  = 0;
static unsigned LastOutputBits = 0;
static unsigned AnalogValue[ANALOG_VALUE_COUNT] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

/*
------------------------------------------
Used by global_parameter_dialog_proc, etc.
------------------------------------------ */
static int  NULL_DAC_INDEX         = 0;
static int  LVDT_DAC_INDEX         = 1;
static int  JOG_SHOT_VALVE_INDEX   = 2;
static int  FT_VALVE_INDEX         = 3;
static int  RETRACT_VALVE_INDEX    = 4;
static int  ZERO_SPEED_CHECK_INDEX = 5;
static int  VC_DITHER_FREQ_INDEX   = 6;
static int  VC_DITHER_AMPL_INDEX   = 7;
static int  PC_DITHER_FREQ_INDEX   = 8;
static int  PC_DITHER_AMPL_INDEX   = 9;
static int  NULL_DAC_2_INDEX       = 10;
static int  LVDT_DAC_2_INDEX       = 11;

FTII_VAR_ARRAY GlobalParameters;

/*
---------------------------
Used by control_mods_dialog
--------------------------- */
const static int CTRL_PROG_NUMBER_LEN = 4;   /* Used to compare line numbers like .022 */
static bool         UsingAltStep;
static TCHAR NormalStep[] = TEXT( ".025 D0" );

static STRING_CLASS AltStepNum;
static STRING_CLASS AltStepEnd;
static STRING_CLASS AltStepVel;
static STRING_CLASS AltStepAcc;
static STRING_CLASS AltStepPos;
static STRING_CLASS AltStepVelPrefix;
static STRING_CLASS AltStepAccPrefix;
static STRING_CLASS AltStepPosPrefix;
static STRING_CLASS ParkStepNum;
static STRING_CLASS ParkStep2Num;
static STRING_CLASS ParkStepVolts;
static STRING_CLASS ParkStepVoltsPrefix;
static STRING_CLASS StartStepNum;
static STRING_CLASS StartStepVolts;
static STRING_CLASS StartStepVoltsPrefix;
static bool         UpdatingControlModsDialog = false;

/*
------------------------------------------
Bit controls for the current values dialog
------------------------------------------ */
struct BIT_CONTROL {
    unsigned     bit;
    bool         is_visible;
    WINDOW_CLASS w;
    STRING_CLASS name;
    BIT_CONTROL() { bit=0; is_visible=false; }
    ~BIT_CONTROL() {}
    };

/*
------------
Als Log Bits
------------ */
static unsigned SLOW_SHOT_OUTPUT_MASK   = 0x10000;
static unsigned ACCUMULATOR_OUTPUT_MASK = 0x20000;

static unsigned MANUAL_MODE_MASK        = 0x200000;

/*
-----------------
Input Bit Indexes
----------------- */
const int START_SHOT_BIT_INDEX      =  0;
const int FOLLOW_THROUGH_BIT_INDEX  =  1;
const int RETRACT_BIT_INDEX         =  2;
const int JOG_SHOT_BIT_INDEX        =  3;
const int MANUAL_MODE_BIT_INDEX     =  4;
const int AT_HOME_BIT_INDEX         =  5;
const int OK_FOR_VAC_FAST_BIT_INDEX =  6;
const int VACUUM_ON_BIT_INDEX       =  7;
const int LOW_IMPACT_BIT_INDEX      =  8;
const int FILL_TEST_BIT_INDEX       =  9;
const int NofInputBitControls       = 10;

BIT_CONTROL InputBitControl[NofInputBitControls];

const UINT InputBitControlId[NofInputBitControls] = {
    START_SHOT_TBOX, FOLLOW_THROUGH_TBOX,
    RETRACT_TBOX, JOG_SHOT_TBOX, MANUAL_MODE_TBOX,
    AT_HOME_TBOX, OK_FOR_VAC_FAST_TBOX, VACUUM_ON_TBOX, LOW_IMPACT_TBOX,
    FILL_TEST_TBOX
    };

static BOOLEAN OptoMode = FALSE;

/*
---------------
Realtime Labels
--------------- */
static STRING_CLASS CmdString;
static STRING_CLASS LvdtString;
static STRING_CLASS MeasString;

/*
------------------------------------
ErrorLogFile temp and zip file names
------------------------------------ */
static NAME_CLASS ErrorLogTempFile;
static NAME_CLASS ErrorLogZipFile;

/*
----------------------------------------------------------------------
Indexes and Variable Numbers for the ftii machine level variables file
These are set in the set_dac and valve_test screens and need to be
reloaded whenever the current machine changes.
---------------------------------------------------------------------- */
const int LVDT_1_BOARD_LEVEL_OFFSET_INDEX = 0;
const int LVDT_2_BOARD_LEVEL_OFFSET_INDEX = 1;
const int DIGITAL_POTS_INDEX = 2;

static int MachineVars[] = {
    LVDT_1_BOARD_LEVEL_OFFSET_VN,
    LVDT_2_BOARD_LEVEL_OFFSET_VN,
    DIGITAL_POTS_VN
    };
const int NofMachineVars = sizeof(MachineVars)/sizeof(int);
static BOOLEAN UpdatingSetDacScreen    = FALSE;
static BOOLEAN UpdatingValveTestScreen = FALSE;
static BOOLEAN UsingValveTestOutput    = FALSE;
static BOOLEAN UsingNullValveOutput    = FALSE;

/*
-------------------
Digital pot indexes
------------------- */
const int AMP_2_CURRENT_GAIN = 0;
const int AMP_2_LVDT_GAIN    = 1;
const int AMP_1_CURRENT_GAIN = 2;
const int AMP_1_LVDT_GAIN    = 3;

/*
------------------
Output Bit Indexes
------------------ */
const int SLOW_SHOT_BIT_INDEX       = 0;
const int ACCUMULATOR_BIT_INDEX     = 1;
const int LS1_BIT_INDEX             = 2;
const int LS2_BIT_INDEX             = 3;
const int LS3_BIT_INDEX             = 4;
const int LS4_BIT_INDEX             = 5;
const int LS5_BIT_INDEX             = 6;
const int LS6_BIT_INDEX             = 7;
const int NofOutputBitControls      = 8;

BIT_CONTROL OutputBitControl[NofOutputBitControls];

const UINT OutputBitControlId[NofOutputBitControls] = {
    SLOW_SHOT_TBOX, ACCUMULATOR_TBOX, LS1_TBOX,
    LS2_TBOX, LS3_TBOX, LS4_TBOX, LS5_TBOX, LS6_TBOX
    };

struct STATUS_CONTROL {
    unsigned     bit;
    bool         is_visible;
    WINDOW_CLASS w;
    STATUS_CONTROL() { bit=0; is_visible=false; }
    };

/*
-----------------------------
Error Log Analog Sample Count
----------------------------- */
static int Lindex = -1;

/*
-----------------------------------------------------------------
These two values are set every time I get a new op status message
----------------------------------------------------------------- */
static int CurrentChannelMode  = 1;
static int CurrentFlashVersion = 6;

/*
---------------------------------------------------------
These are used to resize the main window for the terminal
--------------------------------------------------------- */
static int             MainEboxHeight   = 20;
static int             TopOfListBox     = 0;
static int             MainClientWidth  = 0;
WNDPROC  OriEditControlProc;

const int NofMonitorStatusControls = 4;
static STATUS_CONTROL MonitorStatusControl[NofMonitorStatusControls];

void add_board_variable_to_list( TCHAR * sorc );
BOOL CALLBACK AutoShotDisplayProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
bool begin_monitoring();
void check_to_see_if_any_threads_are_running();
int  current_board_index();
int  current_board_number();
int  current_channel_mode();
int  current_sub_version();
TCHAR * current_upload_file_name();
int  current_version();
TCHAR * current_version_string();
void display_board_info();
void exit_main_proc();
void fill_global_parameters();
BOOLEAN  is_suretrak_controlled( int i );
int  load_ftii_boards();
void send_command( STRING_CLASS & machine_name, TCHAR * command, bool need_to_set_event );
void send_command_to_current_machine( TCHAR * command, bool need_to_set_event );
void start_board_monitor_thread();
bool start_chars_are_equal( const char * sorc, const char * s );
void start_shotsave_thread();
void stop_monitoring();
void toggle_io_display();
void update_downtime( DOWN_DATA_CLASS dd );
unsigned wirebit( int wire_number );


/***********************************************************************
*                             SEND_COMMAND                             *
* This is called for reset.                                            *
***********************************************************************/
static void send_command( TCHAR * machine_name, TCHAR * command, bool need_to_set_event )
{
STRING_CLASS s;
s.acquire( machine_name );
send_command( s, command, need_to_set_event );
s.release();
}

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
static RESOURCE_TEXT_ENTRY rsn[] = {
    { UNABLE_TO_CONTINUE_STRING,        TEXT("UNABLE_TO_CONTINUE_STRING") },
    { MONITOR_WINDOW_TITLE_STRING,      TEXT("MONITOR_WINDOW_TITLE_STRING") },
    { LOW_DISK_SPACE_STRING,            TEXT("LOW_DISK_SPACE_STRING") },
    { NO_SHOTS_SAVED_STRING,            TEXT("NO_SHOTS_SAVED_STRING") },
    { BOARD_MONITOR_EVENT_FAIL_STRING,  TEXT("BOARD_MONITOR_EVENT_FAIL_STRING") },
    { BOARD_MONITOR_THREAD_STRING,      TEXT("BOARD_MONITOR_THREAD_STRING") },
    { BOARD_MONITOR_WAIT_FAIL_STRING,   TEXT("BOARD_MONITOR_WAIT_FAIL_STRING") },
    { CREATE_FAILED_STRING,             TEXT("CREATE_FAILED_STRING") },
    { NOT_CONNECTED_STRING,             TEXT("NOT_CONNECTED_STRING") },
    { CONNECTED_STRING,                 TEXT("CONNECTED_STRING") },
    { CONTROL_ACTIVE_STRING,            TEXT("CONTROL_ACTIVE_STRING") },
    { CONTROL_FATAL_STRING,             TEXT("CONTROL_FATAL_STRING") },
    { CONTROL_WARNING_STRING,           TEXT("CONTROL_WARNING_STRING") },
    { SAVING_STRING,                    TEXT("SAVING") },
    { NO_PROGRAM_STRING,                TEXT("NO_PROGRAM_STRING") },
    { UNKNOWN_STRING,                   TEXT("UNKNOWN_STRING") },
    { STACK_OVERFLOW_STRING,            TEXT("STACK_OVERFLOW_STRING") },
    { BLOCK_TIMEOUT_STRING,             TEXT("BLOCK_TIMEOUT_STRING") },
    { INVALID_BLOCK_STRING,             TEXT("INVALID_BLOCK_STRING") },
    { BLOCK_EXECUTION_ERROR_STRING,     TEXT("BLOCK_EXECUTION_ERROR_STRING") },
    { PROGRAM_STOPPED_BY_USER_STRING,   TEXT("PROGRAM_STOPPED_BY_USER_STRING") },
    { TEST_FATAL_ERROR_STRING,          TEXT("TEST_FATAL_ERROR_STRING") },
    { VAC_ABORT_POS_TOO_LONG_STRING,    TEXT("VAC_ABORT_POS_TOO_LONG_STRING") },
    { BLOCK_TIMEOUT_WARNING_STRING,     TEXT("BLOCK_TIMEOUT_WARNING_STRING") },
    { FILL_TEST_ABORT_STRING,           TEXT("FILL_TEST_ABORT_STRING") },
    { MASTER_ENABLE_DROPPED_OUT_STRING, TEXT("MASTER_ENABLE_DROPPED_OUT_STRING") },
    { VAC_ABORT_ZSPEED_STRING,          TEXT("VAC_ABORT_ZSPEED_STRING") },
    { TEST_WARNING_STRING,              TEXT("TEST_WARNING_STRING") },
    { SHOTSAVE_WAIT_FAIL_STRING,        TEXT("SHOTSAVE_WAIT_FAIL_STRING") },
    { SHOTSAVE_EVENT_FAIL_STRING,       TEXT("SHOTSAVE_EVENT_FAIL_STRING") },
    { SHOTSAVE_THREAD_STRING,           TEXT("SHOTSAVE_THREAD_STRING") },
    { UNABLE_TO_COMPLY_STRING,          TEXT("UNABLE_TO_COMPLY_STRING") },
    { N0_NEW_AUTO_NULL_STRING,          TEXT("N0_NEW_AUTO_NULL_STRING") },
    { MANUAL_NULLING_STRING,            TEXT("MANUAL_NULLING_STRING") },
    { AUTO_NULL_BEGIN_STRING,           TEXT("AUTO_NULL_BEGIN_STRING") },
    { AUTO_NULL_FAIL_STRING,            TEXT("AUTO_NULL_FAIL_STRING") },
    { AUTO_NULL_SUCCESS_STRING,         TEXT("AUTO_NULL_SUCCESS_STRING") },
    { NULL_ALREADY_GOOD_STRING,         TEXT("NULL_ALREADY_GOOD_STRING") },
    { UNUSABLE_NULL_LEVEL_STRING,       TEXT("UNUSABLE_NULL_LEVEL_STRING") },
    { NOT_MONITORED_STRING,             TEXT("NOT_MONITORED_STRING") },
    { CONNECTING_STRING,                TEXT("CONNECTING_STRING") },
    { NO_MASTER_ENABLE_STRING,          TEXT("NO_MASTER_ENABLE_STRING") },
    { NO_CONNECT_STRING,                TEXT("NO_CONNECT_STRING") },
    { DISCONNECTING_STRING,             TEXT("DISCONNECTING_STRING") },
    { NO_WARMUP_PART_STRING,            TEXT("NO_WARMUP_PART_STRING") },
    { CANT_OPEN_FILE_STRING,            TEXT("CANT_OPEN_FILE_STRING") },
    { NEW_SETUP_STRING,                 TEXT("NEW_SETUP_STRING") },
    { NAK_RECEIVED_STRING,              TEXT("NAK_RECEIVED_STRING") },
    { UPLOAD_FAIL_STRING,               TEXT("UPLOAD_FAIL_STRING") },
    { REPEATED_STRING,                  TEXT("REPEATED_STRING") },
    { MISSING_STRING,                   TEXT("MISSING_STRING") },
    { PACKET_STRING,                    TEXT("PACKET_STRING") },
    { SAVING_SHOT_STRING,               TEXT("SAVING_SHOT_STRING") },
    { READY_STRING,                     TEXT("READY_STRING") },
    { PACKET_ERROR_STRING,              TEXT("PACKET_ERROR_STRING") },
    { BEGIN_UPLOAD_STRING,              TEXT("BEGIN_UPLOAD_STRING") },
    { UPLOAD_COMPLETE_STRING,           TEXT("UPLOAD_COMPLETE_STRING") },
    { CYCLE_START_STRING,               TEXT("CYCLE_START") },
    { TIMEOUT_STRING,                   TEXT("TIMEOUT_STRING") },
    { ZERO_STROKE_LENGTH_STRING,        TEXT("ZERO_STROKE_LENGTH_STRING") },
    { ON_STRING,                        TEXT("ON_STRING") },
    { OFF_STRING,                       TEXT("OFF_STRING") },
    { NO_UPLOAD_FILE_STRING,            TEXT("NO_UPLOAD_FILE_STRING") },
    { CONTROL_INPUT_18_STRING,          TEXT("CONTROL_INPUT_18_STRING") },
    { CONTROL_INPUT_19_STRING,          TEXT("CONTROL_INPUT_19_STRING") },
    { CONTROL_INPUT_20_STRING,          TEXT("CONTROL_INPUT_20_STRING") },
    { CONTROL_INPUT_21_STRING,          TEXT("CONTROL_INPUT_21_STRING") },
    { CONTROL_INPUT_22_STRING,          TEXT("CONTROL_INPUT_22_STRING") },
    { CONTROL_INPUT_23_STRING,          TEXT("CONTROL_INPUT_23_STRING") },
    { CONTROL_INPUT_24_STRING,          TEXT("CONTROL_INPUT_24_STRING") },
    { CONTROL_INPUT_25_STRING,          TEXT("CONTROL_INPUT_25_STRING") },
    { CONTROL_INPUT_26_STRING,          TEXT("CONTROL_INPUT_26_STRING") },
    { CONTROL_INPUT_27_STRING,          TEXT("CONTROL_INPUT_27_STRING") },
    { CONTROL_INPUT_28_STRING,          TEXT("CONTROL_INPUT_28_STRING") },
    { CONTROL_INPUT_29_STRING,          TEXT("CONTROL_INPUT_29_STRING") },
    { RESTARTED_STRING,                 TEXT("RESTARTED_STRING") },
    { SHUTTING_DOWN_MONITOR_STRING,     TEXT("SHUTTING_DOWN_MONITOR_STRING") },
    { SHUTTING_DOWN_SHOTSAVE_STRING,    TEXT("SHUTTING_DOWN_SHOTSAVE_STRING") },
    { SHUTTING_DOWN_REMOTE_STRING,      TEXT("SHUTTING_DOWN_REMOTE_STRING") },
    { SHOTSAVE_NOT_RUNNING_STRING,      TEXT("SHOTSAVE_NOT_RUNNING_STRING") },
    { TURN_ON_ALL_OUTPUTS_STRING,       TEXT("TURN_ON_ALL_OUTPUTS_STRING") },
    { TURN_OFF_ALL_OUTPUTS_STRING,      TEXT("TURN_OFF_ALL_OUTPUTS_STRING") },
    { NO_RESPONSE_STRING,               TEXT("NO_RESPONSE_STRING") },
    { BOARD_STRING,                     TEXT("BOARD_STRING") },
    { ADDRESS_STRING,                   TEXT("ADDRESS_STRING") },
    { COM_STATUS_STRING,                TEXT("COM_STATUS_STRING") },
    { MACHINE_STRING,                   TEXT("MACHINE_STRING") },
    { PART_STRING,                      TEXT("PART_STRING") },
    { MONITOR_STATUS_STRING,            TEXT("MONITOR_STATUS_STRING") },
    { LOW_PW_LEVEL_STRING,              TEXT("LOW_PW_LEVEL_STRING") },
    { INVALID_SHOT_NAME_STRING,         TEXT("INVALID_SHOT_NAME_STRING") }
    };

int nof_rsn = sizeof( rsn)/sizeof(RESOURCE_TEXT_ENTRY);
int i;
TCHAR * rs;
rs = 0;

for ( i=0; i<nof_rsn; i++ )
    {
    if ( resource_id == rsn[i].id )
        {
        rs = StringTable.find( rsn[i].s );
        if ( *rs != NullChar )
            return rs;
        break;
        }
    }

return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                          ASCII_0_TO_5_VOLTS                          *
*               Convert unsigned 0-4095 to 0 to 5 volts                *
***********************************************************************/
static TCHAR * ascii_0_to_5_volts( unsigned x )
{
double d;

d = (double) x;
d *= 5.0;
d /= 4095.0;
return fixed_string( d, 3, 2 );
}

/***********************************************************************
*                              DAC_VOLTS                               *
***********************************************************************/
static double dac_volts( unsigned x )
{
double d;

d = double( x );
d *= 20.0;
d /= 4095.0;
d -= 10.0;
return round( d, 0.01 );
}

/***********************************************************************
*                          ASCII_DAC_VOLTS                             *
***********************************************************************/
static TCHAR * ascii_dac_volts( unsigned x )
{
double d;

d = double( x );
d *= 20.0;
d /= 4095.0;
d -= 10.0;

return fixed_string( d, 3, 2 );
}

/***********************************************************************
*                         ASCII_DAC_VOLTS_3                            *
*                         3 decimal places                             *
***********************************************************************/
static TCHAR * ascii_dac_volts_3( unsigned x )
{
double d;

d = double( x );
d /= 4095.0;
d *= 20.0;
d -= 10.0;
d = round( d, 0.001 );

return ascii_double( d );
}

/**********************************************************************
*                        SET_GRAY_MENU_ITEMS                          *
**********************************************************************/
static void set_gray_menu_items()
{
static UINT  item[] = {
    FTII_SAVE_LISTBOX_CHOICE,
    EXIT_MENU_CHOICE,
    FTII_TERMINAL_CHOICE,
    FTII_CLEAR_TERMINAL_CHOICE,
    FTII_OUTPUT_LIST_CHOICE,
    FTII_LOG_MESSAGES_CHOICE,
    FTII_CHECK_CONNECTION_CHOICE,
    FTII_RESET_MACHINE_CHOICE,
    FTII_SAVE_SHOT_NAME_FILES_CHOICE
    };

const int32 NOF_ITEMS = sizeof(item)/sizeof(UINT);

int        i;
int        pw_level_needed;
UINT       menu_flags;
HMENU      menu_handle;
INI_CLASS  ini;
NAME_CLASS s;

s.get_exe_dir_file_name( SureTrakIniFile );
ini.set_file( s.text() );
pw_level_needed = ini.get_int( ConfigSection, ExitEnableLevelKey );

if ( pw_level_needed <= CurrentPasswordLevel )
    menu_flags = MF_ENABLED;
else
    menu_flags = MF_GRAYED;

menu_handle = GetMenu( MainWindow.handle() );

if ( menu_handle )
    {
    for ( i=0; i<NOF_ITEMS; i++ )
        EnableMenuItem( menu_handle, item[i], menu_flags );
    }
}

/***********************************************************************
*                               SHOWBITS                               *
*                                                                      *
*          Control ..2.  4..1  4...  .32.  Monitor ..6....1            *
***********************************************************************/
static TCHAR * showbits( TCHAR * dest, unsigned sorc )
{
const unsigned mask = 0x80000000;
unsigned u;
int      led;
int      module;

u = sorc;
dest = copystring( dest, TEXT("Control") );
for ( module=0; module<4; module++ )
    {
    *dest++ = SpaceChar;
    *dest++ = SpaceChar;
    for ( led=3; led>=0; led-- )
        {
        if ( u & mask )
            *dest++ = OneChar+led;
        else
            *dest++ = PeriodChar;
        u <<= 1;
        }
    }

dest = copystring( dest, TEXT("  Monitor ") );

u = sorc;
u <<= 24;
for ( module=7; module>=0; module-- )
    {
    if ( u & mask )
        *dest++ = OneChar+module;
    else
        *dest++ = PeriodChar;
    u <<= 1;
    }

*dest = NullChar;
return dest;
}

/***********************************************************************
*                     CURRENT_LVDT_FEEDBACK_INDEX                      *
***********************************************************************/
int current_lvdt_feedback_index( int dac )
{
int i;

/*
---------------------------------------------------
Default is ( CurrentChannelMode == CHANNEL_MODE_5 )
--------------------------------------------------- */
i = 6;

if ( CurrentChannelMode == CHANNEL_MODE_7 )
    {
    if ( dac == 0 )
        i = 4;
    else
        i = 5;
    }

return i;
}

/***********************************************************************
*                 CURRENT_RAW_LVDT_FEEDBACK_INDEX                      *
***********************************************************************/
unsigned current_raw_lvdt_feedback_index( int dac )
{
int i;

/*
----------------------------------------------------
Assume that ( CurrentChannelMode == CHANNEL_MODE_7 )
---------------------------------------------------- */
if ( dac == 0 )
    i = 9;
else
    i = 11;

return i;
}

/***********************************************************************
*                     CURRENT_MEASURED_COMMAND_INDEX                   *
***********************************************************************/
unsigned current_measured_command_index( int dac )
{
int i;

/*
---------------------------------------------------
Default is ( CurrentChannelMode == CHANNEL_MODE_5 )
--------------------------------------------------- */
i = 7;

if ( CurrentChannelMode == CHANNEL_MODE_7 )
    {
    if ( dac == 0 )
        i = 6;
    else
        i = 7;
    }

return i;
}

/***********************************************************************
*                               GET_HELP                               *
***********************************************************************/
void get_help()
{
static char s[] = "name=ft2base.swf";
const int SLEN = sizeof(s);
static STARTUPINFO info = {
    sizeof(STARTUPINFO),
    0, 0, 0,            // lpReserved, lpDesktop, lpTitle
    0,                  // dwX
    0,                  // dwY
    0, 0, 0, 0, 0,      // dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute
    STARTF_USEPOSITION, // dwFlags
    0, 0, 0, 0, 0, 0    // wShowWindow, cbReserved2, lpReserved2, hStdInput, hStdOutput, hStdError
    };

NAME_CLASS path;
FILE_CLASS f;
PROCESS_INFORMATION pi;

path.get_exe_dir_file_name( TEXT("ft2help.txt") );
if ( f.open_for_write(path.text()) )
    {
    f.writebinary( s, SLEN );
    f.close();

    path.get_exe_dir_file_name( TEXT("ft2help.exe") );

    CreateProcess( 0,          // lpApplicationName
        path.text(),           // lpCommandLine
        0, 0, 0,               // lpProcessAttributes, lpThreadAttributes, bInheritHandles,
        NORMAL_PRIORITY_CLASS, // dwCreationFlags
        0, 0,                  // lpEnvironment, lpCurrentDirectory,
        &info,                 // lpStartupInfo = input
        &pi                    // lpProcessInformation = output
        );
    }
}

/***********************************************************************
*                              DRAW_LINE                               *
***********************************************************************/
static void draw_line( HDC dc, int x1, int y1, int x2, int y2 )
{
MoveToEx( dc, x1, y1, 0 );
LineTo( dc, x2, y2 );
}

/***********************************************************************
*                              WRITE_LOG                               *
***********************************************************************/
static void write_log( TCHAR * sorc )
{
static FILE_CLASS   f;
static TIME_CLASS   t;
static STRING_CLASS s;
static TCHAR * cp;

t.get_local_time();
s = t.text();
s += SpaceString;
s += sorc;
f.open_for_append( ErrorLogFile );
while ( ErrorLog.count() )
    {
    cp = (TCHAR *)ErrorLog.pop();
    f.writeline( cp );
    delete[] cp;
    }
f.writeline( s.text() );
f.close();
}

/***********************************************************************
*                         PUSH_LOG_MESSAGE                             *
***********************************************************************/
void push_log_message( TCHAR * sorc )
{
static TIME_CLASS   t;
static STRING_CLASS s;
static TCHAR * cp;

t.get_local_time();
s = t.text();
s += SpaceString;
s += sorc;
ErrorLog.push( s.release() );
}

/***********************************************************************
*                      CLEAR_CURRENT_VALUE_COPIES                      *
***********************************************************************/
static void clear_current_value_copies()
{
int i;

Max_Position   = 0;
Max_Velocity   = 0;
Control_Status = 0;
Monitor_Status = 0;
LastInputBits  = 0;
LastOutputBits = 0;
for ( i=0; i<ANALOG_VALUE_COUNT; i++ )
    AnalogValue[i] = 0;
}

/***********************************************************************
*                           FATAL_ERROR_STRING                         *
***********************************************************************/
TCHAR * fatal_error_string( int fatal_error_to_find )
{
static UINT id[] = { STACK_OVERFLOW_STRING, BLOCK_TIMEOUT_STRING, INVALID_BLOCK_STRING,
                     BLOCK_EXECUTION_ERROR_STRING, PROGRAM_STOPPED_BY_USER_STRING,
                     TEST_FATAL_ERROR_STRING };

static int fatal_error_number[] = { STACK_OVERFLOW, BLOCK_TIMEOUT, INVALID_BLOCK,
                                    BLOCK_EXECUTION_ERROR, PROGRAM_STOPPED_BY_USER,
                                    TEST_FATAL_ERROR };

const int nof_errors = sizeof(fatal_error_number)/sizeof(int);

int i;
int myid;

myid = 0;
for ( i=0; i<nof_errors; i++ )
    {
    if ( fatal_error_to_find == fatal_error_number[i] )
        {
        myid = id[i];
        break;
        }
    }
if ( myid > 0 )
    return resource_string( MainInstance, myid );

return EmptyString;
}

/***********************************************************************
*                         WARNING_ERROR_STRING                         *
***********************************************************************/
TCHAR * warning_error_string( int warning_to_find )
{
static UINT id[] = {
    VAC_ABORT_POS_TOO_LONG_STRING, BLOCK_TIMEOUT_WARNING_STRING, FILL_TEST_ABORT_STRING,
    MASTER_ENABLE_DROPPED_OUT_STRING, VAC_ABORT_ZSPEED_STRING, TEST_WARNING_STRING
    };

static int warning_number[] = {
    VAC_ABORT_POS_TOO_LONG, BLOCK_TIMEOUT_WARNING, FILL_TEST_ABORT,
    MASTER_ENABLE_DROPPED_OUT, VAC_ABORT_ZSPEED, TEST_WARNING
    };

const int nof_warnings = sizeof(warning_number)/sizeof(int);

int i;
int myid;

myid = 0;
for ( i=0; i<nof_warnings; i++ )
    {
    if ( warning_to_find == warning_number[i] )
        {
        myid = id[i];
        break;
        }
    }
if ( myid > 0 )
    return resource_string( MainInstance, myid );

return EmptyString;
}

/***********************************************************************
*                        RESOURCE_MESSAGE_BOX                          *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
STRING_CLASS m;
STRING_CLASS t;
m = resource_string( msg_id );
t = resource_string( title_id );
MessageBox( NULL, m.text(), t.text(), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                         OKCANCEL_MESSAGE_BOX                         *
***********************************************************************/
bool okcancel_message_box( UINT msg_id, UINT title_id )
{
if ( resource_message_box(MainInstance, msg_id, title_id, MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK )
    return true;

return false;
}

/***********************************************************************
*                               GET_BYTE                               *
***********************************************************************/
static unsigned get_byte( unsigned sorc, int byte_number )
{
static unsigned mask[4] = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
unsigned n;

if ( byte_number < 0 || byte_number > 3 )
    return 0;

sorc &= mask[byte_number];
n = byte_number * 8;
if ( n )
    sorc >>= n;

return sorc;
}

/***********************************************************************
*                               PUT_BYTE                               *
* Shift the low byte of sorc into byte_number [0,3] of dest.           *
***********************************************************************/
static void put_byte( unsigned & dest, unsigned sorc, int byte_number )
{
static unsigned mask[4] = { 0xFFFFFF00, 0xFFFF00FF, 0xFF00FFFF, 0x00FFFFFF };
unsigned n;

if ( byte_number < 0 || byte_number > 3 )
    return;

sorc &= 0xFF;
n = byte_number * 8;
if ( n )
    sorc <<= n;

dest &= mask[byte_number];
dest |= sorc;
}

/***********************************************************************
*                     FT2_CONTROL_PROGRAM_NAME                         *
* This returns the name of the file only. You need to append this      *
* to the exes directory to make the entire path.                       *
* The flash version is 4 or above..
***********************************************************************/
TCHAR * ft2_control_program_name( int flash_version, MACHINE_CLASS & m )
{
static TCHAR control4c[]  = TEXT("control4c.txt" );
static TCHAR control4m[]  = TEXT("control4m.txt" );
static TCHAR control5c[]  = TEXT("control5c.txt" );
static TCHAR control5m[]  = TEXT("control5m.txt" );
static TCHAR control5e[]  = TEXT("control5e.txt" );
static TCHAR control5p[]  = TEXT("control5p.txt" );
static TCHAR controlall[] = TEXT("control_all.txt" );

TCHAR * cp;

if ( flash_version < 5 )
    {
    if ( m.suretrak_controlled )
        cp = control4c;
    else
        cp = control4m;
    }
else if ( flash_version > 5 )
    {
    cp = controlall;
    }
else
    {
    if ( m.suretrak_controlled )
        {
        if ( m.monitor_flags & MA_HAS_DIGITAL_SERVO )
            {
            if ( m.monitor_flags & MA_HAS_PRESSURE_CONTROL )
                cp = control5p;
            else
                cp = control5c;
            }
        else
            {
            cp = control5e;
            }
        }
    else
        {
        cp = control5m;
        }
    }
return cp;
}

/***********************************************************************
*                          SAVE_MACHINE_VARS                           *
* I am only responsible for the first three variables. If there are    *
* more than that restore them as well.                                 *
* I used to keep the in the first three lines of the file but I now    *
* just make sure they are in the file with the update() function.      *
***********************************************************************/
static BOOLEAN save_machine_vars()
{
int        i;
NAME_CLASS s;
FTII_VAR_ARRAY va;

s.get_ft2_machine_settings_file_name( ComputerName, CurrentMachine.name );
va.get(s);

for ( i=0; i<NofMachineVars; i++ )
    va.update( CurrentMachineVars[i] );

if ( va.put(s) )
    {
    if ( !IsRemoteMonitor )
        AutoBack.backup_machine( CurrentMachine.name );
    return true;
    }

return false;
}

/***********************************************************************
*                        LOAD_MACHINE_VARS                             *
***********************************************************************/
static BOOLEAN load_machine_vars()
{
NAME_CLASS s;
int        i;
int        version;

CurrentMachineVars.empty();
CurrentMachineVars.upsize( NofMachineVars );

for ( i=0; i<NofMachineVars; i++ )
    CurrentMachineVars[i].set( MachineVars[i], (unsigned) 0 );

version = current_version();
s.get_ft2_machine_settings_file_name( ComputerName, CurrentMachine.name );
if ( !s.file_exists() )
    {
    s.get_exe_dir_file_name( ComputerName, ft2_control_program_name(version, CurrentMachine) );
    if ( !s.file_exists() )
        return FALSE;
    }

CurrentMachineVars.get_from( s );

if ( CurrentMachine.monitor_flags & MA_HAS_DIGITAL_SERVO )
    HasDigitalServoAmp = TRUE;
else
    HasDigitalServoAmp = FALSE;

return TRUE;
}

/***********************************************************************
*         LOAD_LIMIT_SWITCH_WIRES_FOR_REALTIME_OUTPUT_DISPLAY          *
***********************************************************************/
static void load_limit_switch_wires_for_realtime_output_display()
{
int i;
int j;
TCHAR * cp;
LIMIT_SWITCH_WIRE_CLASS lsw;

if( lsw.get(CurrentMachine.computer, CurrentMachine.name, CurrentMachine.current_part) )
    {
    /*
    -----------------------------------------------------------------------------
    If the output bits are limit switches then set the bits to those of this part
    ----------------------------------------------------------------------------- */
    for ( i=0; i<NofOutputBitControls; i++ )
        {
        cp = OutputBitControl[i].name.text();
        if ( *cp == LChar )
            {
            cp++;
            if ( *cp == SChar )
                {
                cp++;
                if ( is_numeric(*cp) )
                    {
                    j = int( *cp - ZeroChar );
                    j--;
                    if ( j>=0 && j<MAX_FTII_LIMIT_SWITCHES )
                        OutputBitControl[i].bit = lsw.mask(j);
                    }
                }
            }
        }
    }
}

/***********************************************************************
*                          LOAD_CURRENT_PART                           *
***********************************************************************/
static void load_current_part()
{
PRESSURE_CONTROL_CLASS  p;

CurrentPart.find( CurrentMachine.computer, CurrentMachine.name, CurrentMachine.current_part );
HasPressureControl = false;
if ( p.find(CurrentMachine.computer, CurrentMachine.name, CurrentMachine.current_part) )
    if ( p.is_enabled )
        HasPressureControl = true;

load_limit_switch_wires_for_realtime_output_display();
}

/***********************************************************************
*                         DIGITAL_POT_SETTING                          *
*               ---------- Amp 2 ------  --------- Amp 1 ------        *
* V431          Current Gain  LVDT Gain  Current Gain  LVDT Gain       *
* byte_offset         0           1            2           3           *
***********************************************************************/
static unsigned digital_pot_setting( int byte_offset )
{
unsigned u;
unsigned mask;
unsigned n;

mask = 0x7F; /* Only 7 bits are significant */
n    = byte_offset * 8;

if ( n > 0 )
    mask <<= n;

u = CurrentMachineVars[DIGITAL_POTS_INDEX].uval();
u &= mask;
if ( n > 0 )
    u >>= n;

return u;
}

/***********************************************************************
*                     SCROLL_LISTBOX_TO_LAST_ENTRY                     *
***********************************************************************/
static void scroll_listbox_to_last_entry()
{
int ti;

ti = MainListBox.count() - MainListBox.visible_row_count();

if ( ti > MainListBox.top_index() )
    MainListBox.set_top_index( ti );
}

/***********************************************************************
*                             SAVE_LISTBOX                             *
***********************************************************************/
static void save_listbox()
{
FILE_CLASS f;
int i;
int n;

n = MainListBox.count();
if ( n > 0 )
    {
    f.open_for_write( TEXT("C:\\terminal.txt") );

    for ( i=0; i<n; i++ )
        f.writeline( MainListBox.item_text(i) );

    f.close();
    }
}

/***********************************************************************
*                         NEW_DOWNTIME_CALLBACK                        *
***********************************************************************/
void new_downtime_callback( TCHAR * topic, short item, HDDEDATA edata )
{
DOWN_DATA_CLASS dd;

if ( dd.set(edata) )
    update_downtime( dd );
}

/***********************************************************************
*                       POSITION_FT2_TEST_DIALOG                       *
***********************************************************************/
static void position_ft2_test_dialog()
{
RECTANGLE_CLASS rp;
RECTANGLE_CLASS r;
long x;

MainWindow.getrect( rp );
Ft2TestDialog.getrect( r );

x = r.width();
r.left = (rp.left + rp.right - x)/2;
r.right = r.left + x;

x = r.height();
r.top = (rp.top + rp.bottom - x)/2;
r.bottom = r.top + x;

Ft2TestDialog.move( r );
}

/***********************************************************************
*                            EDIT_CONTROL_PROC                         *
***********************************************************************/
LRESULT CALLBACK edit_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static STRING_CLASS lastcommand;
static STRING_CLASS s;

if ( msg == WM_CHAR && LOWORD(wParam) == VK_RETURN )
    {
    s.get_text( MainEbox );
    if ( s.isempty() )
        s = EmptyString;

    s.uppercase();
    /*
    ------------------------------------------------
    Check to see if I'm starting the ft2 test dialog
    ------------------------------------------------ */
    if ( s == TString )
        {
        s = EmptyString;
        s.set_text( MainEbox );
        Ft2TestDialog.show();
        position_ft2_test_dialog();
        SetFocus( Ft2TestDialog.control(TEST_7_RADIO) );
        return 0;
        }
    lastcommand = s;
    send_command_to_current_machine( s.text(), true );

    s = EmptyString;
    s.set_text( MainEbox );
    SetFocus( MainEbox );
    return 0;
    }
else if ( (msg == WM_KEYUP || msg == WM_KEYDOWN)  && LOWORD(wParam) == VK_UP )
    {
    if ( msg == WM_KEYUP )
        {
        if ( !lastcommand.isempty() )
            {
            lastcommand.set_text( MainEbox );
            SetFocus( MainEbox );
            }
        }
    }
else
    {
    return CallWindowProc( OriEditControlProc, hWnd, msg, wParam, lParam );
    }

return 0;
}

/***********************************************************************
*                     TOGGLE_SAVE_SHOT_NAME_FILES                      *
***********************************************************************/
void toggle_save_shot_name_files()
{
if ( SaveShotNameFiles )
    SaveShotNameFiles = FALSE;
else
    SaveShotNameFiles = TRUE;

MainMenu.set_checkmark( (UINT) FTII_SAVE_SHOT_NAME_FILES_CHOICE, SaveShotNameFiles );
}

/***********************************************************************
*                     TOGGLE_LOGGING_SENT_COMMANDS                     *
***********************************************************************/
static void toggle_logging_sent_commands()
{
INI_CLASS ini;
NAME_CLASS s;

if ( SentCommandsLogIsEnabled )
    SentCommandsLogIsEnabled = FALSE;
else
    SentCommandsLogIsEnabled = TRUE;

MainMenu.set_checkmark( (UINT) FTII_LOG_MESSAGES_CHOICE, SentCommandsLogIsEnabled );

s.get_exe_dir_file_name( MonallIniFile );
ini.set_file( s.text() );
ini.put_boolean( ConfigSection, SentCommandsLogIsEnabledKey, SentCommandsLogIsEnabled );
}

/***********************************************************************
*                      SEND_NULL_OFFSET_TO_BOARD                       *
***********************************************************************/
bool send_null_offset_to_board()
{
NAME_CLASS   s;
FTII_VAR_ARRAY va;

va.upsize(1);
va[0].set( NULL_DAC_OFFSET_VN, (unsigned) 0 );

s.get_ft2_global_settings_file_name( ComputerName, CurrentMachine.name );
if ( va.get_from(s) )
    {
    if ( va[0].makeline(s) )
        {
        send_command_to_current_machine( s.text(), true );
        return true;
        }
    }
return false;
}

/***********************************************************************
*                      SEND_LVDT_OFFSET_TO_BOARD                       *
***********************************************************************/
bool send_lvdt_offset_to_board()
{
NAME_CLASS   s;
FTII_VAR_ARRAY va;

va.upsize(1);
va[0].set( LVDT_DAC_OFFSET_VN, (unsigned) 0 );

s.get_ft2_global_settings_file_name( ComputerName, CurrentMachine.name );
if ( va.get_from(s) )
    {
    if ( va[0].makeline(s) )
        {
        send_command_to_current_machine( s.text(), true );
        return true;
        }
    }
return false;
}

/***********************************************************************
*                            SET_CONTROL_Y                             *
***********************************************************************/
void set_control_y( int y, UINT id )
{
WINDOW_CLASS    w;
RECTANGLE_CLASS r;

w = CurrentValuesDialog.control( id );
w.get_move_rect( r );
r.bottom = y + r.height();
r.top = y;
w.move( r );
}

/***********************************************************************
*                        SET_CHANNEL_5_8_LABELS                        *
***********************************************************************/
static void set_channel_5_8_labels( int channel_mode )
{
WINDOW_CLASS w;
RECTANGLE_CLASS r;
int  channel;
int  i;
int  y;
int  delta_y;
int  therm_y_offset;

UINT id;
BOOLEAN checked;
bool    is_visible;

w = CurrentValuesDialog.control( CH_17_20_XBOX );
checked = is_checked( w.handle() );
if ( checked )
    is_visible = false;
else
    is_visible = true;

w = CurrentValuesDialog.control( FT2_MODE_UP_PB );
w.show( is_visible );
w = CurrentValuesDialog.control( FT2_MODE_DOWN_PB );
w.show( is_visible );

/*
-----------------------------------------------------
Get the offset from one analog value tbox to the next
----------------------------------------------------- */
w = CurrentValuesDialog.control( VOLTS_2_TBOX );
w.get_move_rect( r );
delta_y = r.top;
y       = r.top;

w = CurrentValuesDialog.control(FT_A2_THERM);
w.get_move_rect( r );
therm_y_offset = r.top - y;

w = CurrentValuesDialog.control(VOLTS_1_TBOX);
w.get_move_rect( r );
delta_y -= r.top;

/*
-----------------------------------------------------------------------------------
Channel is the channel number to appear on the labels 5-8 if the mode is 1, 2, or 3
----------------------------------------------------------------------------------- */
channel = 3;
if ( channel_mode != CHANNEL_MODE_7 )
    {
    for ( i=CH_3_TBOX; i<=CH_5_TBOX; i++ )
        {
        w = CurrentValuesDialog.control( i );
        if ( i == CH_5_TBOX )
            {
            if ( channel_mode == CHANNEL_MODE_2 )
                channel = 9;
            else if ( channel_mode == CHANNEL_MODE_3 )
                channel = 13;
            }
        w.set_title( int32toasc(channel) );
        channel++;
        }
    }

if ( checked )
    {
    if ( HasDigitalServoAmp )
        {
        w = CurrentValuesDialog.control( FT_A3_THERM );
        w.post( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 20000) );

        w = CurrentValuesDialog.control( CH_3_TBOX );
        w.set_title( CmdString.text()  );

        w = CurrentValuesDialog.control( CH_4_TBOX );
        w.set_title( MeasString.text() );

        w = CurrentValuesDialog.control( CH_5_TBOX );
        w.set_title( LvdtString.text() );
        }

    w = CurrentValuesDialog.control( CH_6_TBOX );
    w.set_title( CmdString.text()  );

    w = CurrentValuesDialog.control( CH_7_TBOX );
    w.set_title( MeasString.text() );

    w = CurrentValuesDialog.control( CH_8_TBOX );
    w.set_title( LvdtString.text() );

    w = CurrentValuesDialog.control( FT_A6_THERM );
    w.post( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 20000) );
    }
else
    {
    w = CurrentValuesDialog.control( FT_A3_THERM );
    w.post( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

    /*
    -------------------------------------------------------------------------------------------
    The channel number is set in the previous section, as it is different for modes 1, 2, and 3
    ------------------------------------------------------------------------------------------- */
    for ( i=CH_6_TBOX; i<=CH_8_TBOX; i++ )
        {
        w = CurrentValuesDialog.control( i );
        w.set_title( int32toasc(channel) );
        channel++;
        }

    w = CurrentValuesDialog.control( FT_A6_THERM );
    w.post( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );
    }

for ( i=0; i<6; i++ )
    {
    y += delta_y;
    if ( checked )
        {
        if ( (i==0 && channel_mode==CHANNEL_MODE_7) || (i==3 && channel_mode==CHANNEL_MODE_5) )
            {
            set_control_y( y, MIN_VOLTS_TBOX );
            set_control_y( y, MAX_VOLTS_TBOX );
            y += delta_y;
            }
         }
    set_control_y( y, CH_3_TBOX+i    );
    set_control_y( y, VOLTS_3_TBOX+i );
    set_control_y( y+ therm_y_offset, FT_A3_THERM+i );
    }

for ( id=MIN_VOLTS_TBOX; id<=MAX_VOLTS_TBOX; id++ )
    {
    w = CurrentValuesDialog.control(id);
    if ( channel_mode==CHANNEL_MODE_7 || channel_mode == CHANNEL_MODE_5 )
        w.show();
    else
        w.hide();
    }

CurrentValuesDialog.refresh();

NeedToRefreshAnalogs = true;
}

/***********************************************************************
*                        SET_CHANNEL_5_8_LABELS                        *
***********************************************************************/
static void set_channel_5_8_labels()
{
set_channel_5_8_labels( current_channel_mode() );
}

/***********************************************************************
*                         HIDE_TERMINAL_WINDOW                         *
* If the terminal window is docked, hide it, otherwise leave it alone. *
***********************************************************************/
static void hide_terminal_window()
{
RECTANGLE_CLASS pr;
RECTANGLE_CLASS r;
MainWindow.get_move_rect( pr );
TerminalWindow.get_move_rect( r );
if ( r.top >= pr.top )
    {
    TerminalIsVisible = FALSE;
    TerminalWindow.hide();
    }
}

/***********************************************************************
*                          SET_CURRENT_WINDOW                          *
***********************************************************************/
static void set_current_window( WINDOW_CLASS & newwindow )
{
static bool new_is_diagnostic_window;
static bool current_is_diagnostic_window;
static bool firstime = true;
int     channel_mode;
BOOLEAN checked;
TCHAR * cp;
HWND w;

if ( !newwindow.handle() )
    return;

if ( newwindow == CurrentWindow )
    return;

new_is_diagnostic_window     = (newwindow == SetDacDialog) || (newwindow == SetNullDialog) || (newwindow == CurrentValuesDialog) || (newwindow == TestAlarmOutputsDialog);
current_is_diagnostic_window = (CurrentWindow == SetDacDialog) || (CurrentWindow == SetNullDialog) || (CurrentWindow == CurrentValuesDialog) || (CurrentWindow == TestAlarmOutputsDialog);

if ( !WaitingToResume )
    {
    if ( CurrentWindow == StatusWindow || (firstime && newwindow == CurrentValuesDialog) )
        {
        firstime = false;

        fill_global_parameters();

        w = CurrentValuesDialog.control( VELOCITY_UNITS_TBOX );
        set_text( w, units_name(CurrentPart.velocity_units) );

        w = CurrentValuesDialog.control( POSITION_UNITS_TBOX );
        set_text( w, units_name(CurrentPart.distance_units) );
        }

    /*
    ------------------------------------------------------------------------------------------------------
    If I am leaving the set null, set dac, or valve test screens, turn off the valve test in operation bit
    ------------------------------------------------------------------------------------------------------ */
    checked = FALSE;
    if ( CurrentWindow == SetNullDialog && UsingNullValveOutput )
        {
        /*
        ------------------------------------------------
        I'm leaving the Null Valve. Turn off the output;
        ------------------------------------------------ */
        checked = TRUE;

        if ( newwindow == SetDacDialog || newwindow == ValveTestDialog )
            {
            /*
            -----------------------------------------------------------------------
            If I'm going to another screen that needs the output, don't turn it off
            ----------------------------------------------------------------------- */
            if ( UsingValveTestOutput )
                checked = FALSE;
            }
        }

    if ( CurrentWindow == SetDacDialog || CurrentWindow == ValveTestDialog )
        {
        if ( newwindow != SetDacDialog && newwindow != ValveTestDialog )
            {
            if ( UsingValveTestOutput )
                checked = TRUE;

            if ( newwindow == SetNullDialog && UsingNullValveOutput )
                checked = FALSE;
            }
        }

    if ( checked )
        send_command_to_current_machine( TEXT("OO1=~00100000"), true );

    /*
    -----------------------------------------------------------------------------------
    If I am leaving the set null dialog I need to restore the null offset to the board.
    ----------------------------------------------------------------------------------- */
    if ( CurrentWindow == SetNullDialog )
        send_null_offset_to_board();
    else if ( CurrentWindow == SetDacDialog )
        send_lvdt_offset_to_board();

    if ( CurrentWindow != MainWindow )
        {
        if ( CurrentWindow == TerminalWindow )
            hide_terminal_window();
        else
            CurrentWindow.hide();
        }
    /*
    ----------------------------------------
    Turn off the operational status messages
    ---------------------------------------- */
    if ( current_is_diagnostic_window && !new_is_diagnostic_window )
        {
        send_command_to_current_machine( DONT_SEND_OP_DATA_PACKETS,   true );
        }

    if ( CurrentWindow == SetDacDialog )
        {
        if ( is_checked( SetDacDialog.control(AMP_1_RADIO) ) )
           cp = CloseValve_1_Loop;
        else
           cp = CloseValve_2_Loop;

        send_command_to_current_machine( cp,  true );
        }

    if ( CurrentWindow == SetNullDialog || CurrentWindow == SetDacDialog || CurrentWindow == TestAlarmOutputsDialog )
        {
        if ( newwindow != SetNullDialog && newwindow != SetDacDialog && newwindow != TestAlarmOutputsDialog )
            send_command_to_current_machine( StartControlPgmCmd,  true );   /* Start the control program */

        if ( CurrentWindow == SetNullDialog )
            NullStatusMessage.set_title( EmptyString );
        }
    else if ( CurrentWindow == ValveTestDialog )
        {
        send_command_to_current_machine( TEXT("OC1=0"),   true );    /* Stop Oscilloscope mode */
        Sleep(1);
        send_command_to_current_machine( TEXT(".J0.1"),   true );   /* Jump to control        */
        if ( newwindow == SetNullDialog || newwindow == SetDacDialog || newwindow == TestAlarmOutputsDialog )
            {
            send_command_to_current_machine( HaltString, true );
            ResumeDialog = newwindow;
            WaitingToResume = true;
            temp_message( TEXT("Waiting for Control Halt..."), 3000 );
            SetTimer( MainWindow.handle(), RESUME_TIMER_ID, 3000, 0 );
            return;
            }
        }

    /*
    ------------------------------------------------------------------------------------
    If I am switching to the diagnostic screens, turn on the operational status messages
    ------------------------------------------------------------------------------------ */
    if ( new_is_diagnostic_window && !current_is_diagnostic_window )
        {
        load_machine_vars();
        send_command_to_current_machine( SEND_OP_DATA_PACKETS, true );
        }

    if ( (newwindow == StatusWindow) && NeedToCloseMainWindow )
        {
        ResumeDialog = newwindow;
        WaitingToResume = true;
        temp_message( TEXT("Waiting for Shutdown..."), 3000 );
        SetTimer( MainWindow.handle(), RESUME_TIMER_ID, 3000, 0 );
        return;
        }
    }

WaitingToResume = false;

if ( newwindow == SetNullDialog || newwindow == SetDacDialog || newwindow == TestAlarmOutputsDialog )
    {
    if ( CurrentWindow != SetNullDialog && CurrentWindow != SetDacDialog && CurrentWindow != ValveTestDialog && CurrentWindow != TestAlarmOutputsDialog )
        {
        send_command_to_current_machine( HaltString, true );
        Sleep( 100 );
        }

    if ( newwindow == SetNullDialog )
        send_command_to_current_machine( TEXT("OC5=1"), true );
    else if ( newwindow == SetDacDialog )
        send_command_to_current_machine( TEXT("V351=0"), true );

    }
else if ( newwindow == ValveTestDialog )
    {
    ValveTestDialog.post( WM_DBINIT );
    send_command_to_current_machine( TEXT("V390=750"),  true ); /* .5 Volts trigger        */
    Sleep(1);
    send_command_to_current_machine( TEXT("V391=17"),   true ); /* Analog channel 17       */
    Sleep(1);
    send_command_to_current_machine( TEXT("V392=1000"), true ); /* 1000 us per sample      */
    Sleep(1);
    send_command_to_current_machine( TEXT(".J0.105"),   true ); /* Jump to valve test      */
    Sleep(1);
    send_command_to_current_machine( TEXT("OC1=0"),    true );  /* Stop Oscilloscope mode */
    Sleep(1);
    send_command_to_current_machine( TEXT("OC1=1"),    true );  /* Start Oscilloscope mode */
    }

/*
------------------------------------------------------------------------------------------------------
If I am entering the set null, set dac, or valve test screens, turn on the valve test in operation bit
------------------------------------------------------------------------------------------------------ */
checked = FALSE;
if ( newwindow == SetNullDialog && UsingNullValveOutput )
    checked = TRUE;

if ( newwindow == SetDacDialog || newwindow == ValveTestDialog )
    {
    if ( UsingValveTestOutput )
        checked = TRUE;
    }
if ( checked )
    send_command_to_current_machine( TEXT("OO1=00100000"), true );

if ( newwindow == CurrentValuesDialog )
    {
    NeedToRefreshAnalogs = true;

    checked = is_checked(SwitchChannelsXbox);
    channel_mode = current_channel_mode();
    if ( (channel_mode==CHANNEL_MODE_5 || channel_mode==CHANNEL_MODE_7) && !checked )
        set_checkbox( SwitchChannelsXbox, TRUE );
    else if ( channel_mode==CHANNEL_MODE_1 && checked )
        set_checkbox( SwitchChannelsXbox, FALSE );
    CurrentValuesDialog.post( WM_DBINIT );
    }

/*
-----------------------------------------------------
If I was on the valve test dialog I missed doing this
----------------------------------------------------- */
if ( CurrentWindow == ValveTestDialog && new_is_diagnostic_window )
    {
    load_machine_vars();
    send_command_to_current_machine( SEND_OP_DATA_PACKETS, true );
    }

CurrentWindow = newwindow;
CurrentWindow.show();
CurrentWindow.set_focus();
if ( CurrentWindow == TerminalWindow )
    TerminalIsVisible = TRUE;

if ( CurrentWindow == SetDacDialog && HasDigitalServoAmp )
    {
    CurrentWindow.post( WM_DBINIT );
    if ( is_checked( SetDacDialog.control(AMP_1_RADIO) ) )
       cp = OpenValve_1_Loop;
    else
       cp = OpenValve_2_Loop;
    send_command_to_current_machine( cp,  true );
    }

if ( (CurrentWindow == StatusWindow) && NeedToCloseMainWindow )
    {
    MainWindow.close();
    }
}

/***********************************************************************
*                            ULTOHEXSTRING                             *
*H 0000 000F and returns a pointer to the null at the end of the string*
***********************************************************************/
static TCHAR * ultohexstring( TCHAR * dest, unsigned u )
{
*dest++ = HChar;
*dest++ = SpaceChar;

ultohex( dest, u );
if ( lstrlen(dest) < 8 )
    rjust( dest, 8, ZeroChar );

dest += 4;
insert_char( dest, SpaceChar );

dest += 5;
return dest;
}

/***********************************************************************
*                            DO_CONFIG_WORD                            *
***********************************************************************/
static void do_config_word( TCHAR * sorc )
{
TCHAR s[80];
TCHAR * cp;
unsigned u;

cp = sorc + ConfigWordMsgLen;
u  = asctoul( cp );

cp = copystring( s, sorc );
cp = copystring( cp, TEXT(" = ") );
ultohexstring( cp, u );

MainListBox.set_current_index( MainListBox.add(s) );
scroll_listbox_to_last_entry();
}

/***********************************************************************
*                             PERCENT_TO_HEX                           *
***********************************************************************/
static TCHAR * percent_to_hex( double percent )
{
static TCHAR buf[] = TEXT( "H80000000" );
TCHAR * cp;
int32 x;

percent *= 100.0;
percent = round( percent, 1.0 );
x = (int32) percent;

cp = buf + 5;
insalph( cp, x, 4, ZeroChar, HEX_RADIX );

return buf;
}

/***********************************************************************
*                            SHOULD_BE_HEX                             *
*        Assumes dest points to a string like "V500=H00000000"         *
*        V319_256 = H 0000 0010 = H00000000 + 256                      *
***********************************************************************/
static BOOLEAN should_be_hex( TCHAR * dest, TCHAR * sorc )
{
static TCHAR equalspad[] = TEXT( " = " );
static TCHAR pluspad[]   = TEXT( " + " );

const unsigned HIGH_WORD_MASK = 0xFFFF0000;
const unsigned LOW_WORD_MASK  = 0x0000FFFF;

unsigned u;
unsigned masked_value;

if ( *sorc != VChar )
    return FALSE;

dest = copystring( dest, sorc );
dest = copystring( dest, equalspad );

sorc = findchar( UnderScoreChar, sorc );
if ( !sorc )
    return FALSE;

sorc++;
if ( *sorc == HChar )
    return FALSE;

u = asctoul( sorc );

ultohexstring( dest, u );
dest += lstrlen(dest);

masked_value = u & HIGH_WORD_MASK;
if ( masked_value )
    {
    dest = copystring( dest, equalspad );
    dest = ultohexstring( dest, masked_value );
    dest = copystring( dest, pluspad );

    masked_value = u & LOW_WORD_MASK;
    ultoascii( dest, masked_value, DECIMAL_RADIX );
    }

return TRUE;
}

/***********************************************************************
*                          GET_ASCII_RT_DATA                           *
* I am responsible for deleting the buffer. lParam was allocated as    *
* TCHAR.                                                               *
***********************************************************************/
static void get_ascii_rt_data( LPARAM lParam )
{
static const TCHAR analog_response[] = TEXT( "O_CH" );
static const int  analog_response_slen = 4;

static const TCHAR null_response[] = TEXT( "R_AXIS" );
static const int  null_response_slen = 6;

static const TCHAR osw1_response[] = TEXT( "O_OSW1=" );
static const int  osw1_response_slen = 7;

static TCHAR mybuf[150];

static STRING_CLASS sc;

int    channel_index;
TCHAR * buf;
TCHAR * cp;
TCHAR * s;
unsigned x;

buf = (TCHAR *) lParam;
s   = buf;

if ( !buf )
    return;

if ( SendingBoardVariableRequests )
     add_board_variable_to_list( s );

if ( TerminalIsVisible )
    {
    if ( strings_are_equal(s, ConfigWordMsg, ConfigWordMsgLen) || strings_are_equal(s, ConfigWord2Msg, ConfigWordMsgLen) )
        {
        do_config_word( s );
        }
    else if ( !strings_are_equal( s, analog_response, analog_response_slen) )
        {
        if ( strings_are_equal( s, osw1_response, osw1_response_slen) )
            {
            copystring( mybuf, s );
            cp = mybuf + osw1_response_slen;
            x = (unsigned) hextoul( cp );
            rjust( cp, 8, ZeroChar );
            insert_char( cp, SpaceChar );
            insert_char( cp, HChar );
            cp += 6;
            insert_char( cp, SpaceChar );
            cp += 5;
            *cp++ = SpaceChar;
            showbits( cp, x );
            cp = mybuf;
            }
        else
            {
            cp = s;
            if ( should_be_hex(mybuf, s) )
                cp = mybuf;
            }
        MainListBox.set_current_index( MainListBox.add(cp) );
        scroll_listbox_to_last_entry();
        }
    }

if ( strings_are_equal(s, analog_response, analog_response_slen) )
    {
    sc = s;
    s += analog_response_slen;
    cp = findchar( EqualsChar, s );
    if ( cp )
        {
        *cp = NullChar;
        channel_index = asctoint32( s );
        channel_index--;
        cp++;

        /*
        --------------------------------------------------------------------------
        If the channel index is > 0 then the first hex digit is the channel index,
        unless the channel is 17-20, in which case the channel index is
        the channel index - 4. I skip this digit in the following two lines.
        -------------------------------------------------------------------------- */
        if ( channel_index > 0 )
            cp++;
        x = (unsigned) hextoul( cp );
        if ( x != CurrentAnalogValue[channel_index] )
            {
            CurrentAnalogValue[channel_index] = x;
            if ( CurrentWindow != StatusWindow )
                CurrentWindow.post( WM_NEW_DATA );
            }

        if ( TerminalIsVisible )
            {
            sc += TEXT( " = " );
            if ( channel_index < 16 )
                {
                sc += ascii_0_to_5_volts( x );
                }
            else
                {
                sc += ascii_dac_volts( x );
                }
            sc += TEXT( " Volts" );
            MainListBox.set_current_index( MainListBox.add(sc.text()) );
            scroll_listbox_to_last_entry();
            }
        }
    }
else if ( strings_are_equal(s, null_response, null_response_slen) )
    {
    if ( CurrentWindow == SetNullDialog )
        {
        CurrentWindow.post( WM_NEW_RT_DATA, 0, (LPARAM) buf );
        buf = 0;
        }
    }
else if ( ErrorLogIsEnabled )
    {
    /*
    ------------------------------------------------
    Start logging the feedback values for 10 seconds
    ------------------------------------------------ */
    if ( findstring( CycleString, buf) != 0 )
        Lindex = 0;
    write_log( buf );
    }

if ( buf )
    delete[] buf;
}

/***********************************************************************
*                             COPY_ANALOG                              *
***********************************************************************/
static void copy_analog( int channel_index, unsigned short raw_value )
{
unsigned x;

x = (unsigned) raw_value;
x &= 0xFFF;
CurrentAnalogValue[channel_index] = x;
}

/***********************************************************************
*                        SHOW_CURRENT_OP_STATUS                        *
***********************************************************************/
static void show_current_op_status()
{
TCHAR buf[150];
TCHAR * cp;
int i;
unsigned u;

if ( !TerminalIsVisible )
    return;

cp = buf;

for ( i=0; i<2; i++ )
    {
    cp = copystring( cp,  int32toasc((int32)CurrentOpStatus.dac[i]) );
    *cp = SpaceChar;
    cp++;
    }

for ( i=0; i<16; i++ )
    {
    u = (unsigned) CurrentOpStatus.analog[i];
    u &= 0xFFF;
    cp = copystring( cp, ultoascii((unsigned long)u) );
    *cp = SpaceChar;
    cp++;
    }
*cp = NullChar;

MainListBox.set_current_index( MainListBox.add(buf) );
scroll_listbox_to_last_entry();
}

/***********************************************************************
*                            DO_NEW_RT_DATA                            *
***********************************************************************/
static void do_new_rt_data( WPARAM wParam, LPARAM lParam )
{
int i;
FTII_OP_STATUS_DATA * opstat;

switch ( wParam )
    {
    case ASCII_TYPE:
        get_ascii_rt_data( lParam );
        lParam = 0;
        break;

    case OP_STATUS_TYPE:
        CurrentChannelMode  = current_channel_mode();
        CurrentFlashVersion = current_version();
        opstat = (FTII_OP_STATUS_DATA *) lParam;
        if ( CurrentOpStatusIsEmpty || CurrentOpStatus != *opstat )
            {
            CurrentOpStatus        = *opstat;
            CurrentOpStatusIsEmpty = false;

            //show_current_op_status();

            /*
            --------------------------------------------------------
            There are only 16 analog values (4 on the end are blank)
            -------------------------------------------------------- */
            for ( i=0; i<NOF_OP_STATUS_ANALOGS; i++ )
                copy_analog( i, CurrentOpStatus.analog[i] );

            if ( CurrentWindow != StatusWindow )
                CurrentWindow.post( WM_NEW_DATA );
            }
        break;

    case OSCILLOSCOPE_TYPE:
        if ( CurrentWindow == ValveTestDialog )
            {
            ValveTestDialog.post( WM_NEW_DATA, 0, lParam );
            lParam = 0;
            }
        break;
    }

if ( lParam )
    delete[] (char *) lParam;
}

/***********************************************************************
*                            SIZE_TERMINAL                             *
***********************************************************************/
static void size_terminal()
{
RECTANGLE_CLASS r;
WINDOW_CLASS    w;

TerminalWindow.get_client_rect( r );
TerminalMenu.resize();

r.top = TerminalMenu.mr.bottom;
r.bottom -= MainEboxHeight;

w = MainListBox.handle();
w.move( r );

TerminalWindow.get_client_rect( r );
r.top = r.bottom - MainEboxHeight;
w = MainEbox;
w.move( r );
}

/***********************************************************************
*                       CHECK_TERMINAL_POSITION                        *
***********************************************************************/
static void check_terminal_position()
{
RECTANGLE_CLASS pr;
RECTANGLE_CLASS r;
WINDOW_CLASS    w;

MainWindow.get_move_rect( pr );
TerminalWindow.get_move_rect( r );
if ( r.top >= pr.top )
    {
    w = MainWindow.control( F2_RADIO );
    w.get_move_rect( r );
    pr.left = r.right + 1;
    TerminalWindow.move( pr );
    if ( CurrentWindow != TerminalWindow )
        {
        TerminalIsVisible = FALSE;
        TerminalWindow.hide();
        }
    }
}

/***********************************************************************
*                             DOCK_TERMINAL                            *
* This is the opposite of the above. This is in response to the        *
* Doc Terminal menu choice. If I am not docked, dock me.               *
***********************************************************************/
static void dock_terminal()
{
RECTANGLE_CLASS pr;
RECTANGLE_CLASS r;
WINDOW_CLASS    w;

MainWindow.get_move_rect( pr );
TerminalWindow.get_move_rect( r );
if ( r.top < pr.top )
    {
    w = MainWindow.control( F2_RADIO );
    w.get_move_rect( r );
    pr.left = r.right + 1;
    TerminalWindow.move( pr );
    if ( CurrentWindow != TerminalWindow )
        {
        MainWindow.set_focus();
        TerminalIsVisible = FALSE;
        TerminalWindow.hide();
        }
    }
}

/***********************************************************************
*                          LOAD_TERMINAL_MENU                          *
***********************************************************************/
static void load_terminal_menu()
{
INI_CLASS ini;
NAME_CLASS s;

s.get_exe_dir_file_name( TerminalMenuFile );
TerminalMenu.read( s, CurrentPasswordLevel );
TerminalMenu.resize();
}

/***********************************************************************
*                           TERMINAL_PROC                              *
***********************************************************************/
long CALLBACK terminal_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
static BOOLEAN NcButtonDown = FALSE;
static BOOLEAN Moving = FALSE;
static int  i;
static int  id;
static int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        TerminalWindow = hWnd;
        TerminalMenu.create_windows( TerminalMenuWindowName, hWnd );
        load_terminal_menu();
        return 0;

    case WM_NCLBUTTONDOWN:
        NcButtonDown = TRUE;
        break;

    case WM_MOVE:
        if ( NcButtonDown )
            Moving = TRUE;
        break;

    case WM_EXITSIZEMOVE:
        if ( Moving )
            {
            NcButtonDown = FALSE;
            Moving = FALSE;
            check_terminal_position();
            }
        break;

    case WM_SIZE:
        size_terminal();
        break;

    case WM_VTW_MENU_CHOICE:
        switch (id)
            {
            case FTII_SAVE_LISTBOX_CHOICE:
                save_listbox();
                return 0;

            case FTII_CLEAR_TERMINAL_CHOICE:
                MainListBox.empty();
                return 0;

            case EXIT_MENU_CHOICE:
                dock_terminal();
                return 0;
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                           CREATE_CONTROLS                            *
***********************************************************************/
static void create_controls( int nof_rows )
{
RECTANGLE_CLASS r;
int          col;
int          height;
int          i;
int          id;
int          left_edge;
int          row;
int          width;
int          x;
int          y;
int          max_y;
HDC          dc;
HFONT        old_font;
STRING_CLASS s;
WINDOW_CLASS w;

static int ColWidth[NofCols] = { 6, 16, 17, 16, 25, 30 };

dc = MainWindow.get_dc();

old_font = INVALID_FONT_HANDLE;
if ( MyFont != INVALID_FONT_HANDLE )
    old_font = (HFONT) SelectObject( dc, MyFont );

Char_Width  = average_character_width( dc );
Line_Height = character_height( dc );

/*
-----------------------
Create the main buttons
----------------------- */
width = 0;
for ( i=0; i<NofButtons; i++ )
    {
    s = StringTable.find( Button[i].s );
    if ( !s.isempty() )
        maxint( width, pixel_width(dc, s.text()) );
    }

/*
-------------------------------------------------------------
Petzold says a button should be two chars wider than the text
------------------------------------------------------------- */
width += 2*Char_Width;

/*
---------------------------------------------
Save this as the left edge of the main window
--------------------------------------------- */
left_edge = width;

/*
----------------------------------------
The height should be 7/4 the char height
---------------------------------------- */
height = Line_Height * 10 / 9;

/*
---------------------------------------------------------
Create the main buttons along the left side of the window
--------------------------------------------------------- */
x = 0;
y = 0;
for ( i=0; i<NofButtons; i++ )
    {
    y = i * Line_Height;
    y *= 10;
    y /= 9;
    s = StringTable.find( Button[i].s );
    w = CreateWindow(
        TEXT("button"),
        s.text(),
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
        x, y, width, height,
        MainWindow.handle(),
        (HMENU) Button[i].id,
        MainInstance,
        NULL
        );

    if ( MyFont != INVALID_FONT_HANDLE )
        w.send( WM_SETFONT, (WPARAM) MyFont, MAKELPARAM(false, 0) );
    }

if ( Dnu )
    delete[] Dnu;

Dnu = 0;
if ( nof_rows > 0 )
    Dnu = new HWND[ nof_rows ];

if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );
MainWindow.release_dc( dc );

Line_Height += 2;

/*
-------------------------------------------
Create the status window
left_edge is the right edge of the buttons.
------------------------------------------- */
r = MainWindowClientRect;
width = r.right - left_edge;
s = StringTable.find( TEXT("STATUS_TITLE") );
StatusWindow = CreateWindow(
    StatusClassName,
    s.text(),
    WS_CHILD,
    left_edge, 0, width, r.height(),
    MainWindow.handle(),
    (HMENU) STATUS_WINDOW_ID,
    MainInstance,
    NULL
    );

/*
--------------------------
Create the terminal window
-------------------------- */
r = MainWindowMoveRect;
s = StringTable.find( TEXT("TERMINAL_TITLE") );
CreateWindow(
    TerminalClassName,
    s.text(),
    WS_OVERLAPPED | WS_THICKFRAME,
    left_edge, r.top, width, r.height(),
    MainWindow.handle(),
    NULL,
    MainInstance,
    NULL
    );

/*
-------------------------------------------------------------
Use r to hold the outer rectangle of the main window controls
------------------------------------------------------------- */
StatusWindow.get_client_rect( r );
r.top =  MainMenu.mr.bottom;
width = 0;
for ( i=0; i<NofCols; i++ )
    width += Char_Width * ColWidth[i];
width = (r.width() - width )/2;
/*
----------------------------------
Width is now the horizontal margin
---------------------------------- */
r.left  += width;
r.right -= width;

if ( nof_rows > 1 )
    NofRows = nof_rows;
else
    NofRows = 1;

MainPushButton    = new CONTROL_INFO[NofRows];
AddressTbox       = new CONTROL_INFO[NofRows];
ConnectedTbox     = new CONTROL_INFO[NofRows];
MachineTbox       = new CONTROL_INFO[NofRows];
PartTbox          = new CONTROL_INFO[NofRows];
MonitorStatusTbox = new CONTROL_INFO[NofRows];

id     = BOARD_1_PB;
y      = Line_Height + r.top;
width  = Char_Width * 5;
height = Line_Height - 2;
max_y  = 0;

for ( row=0; row<NofRows; row++ )
    {
    x = r.left;
    width  = Char_Width * ColWidth[BOARD_NUMBER_COLUMN];
    MainPushButton[row].id = id;
    MainPushButton[row].w = CreateWindow(
        TEXT("button"),
        int32toasc(row),
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        x, y, width, height,
        StatusWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    if ( MyFont != INVALID_FONT_HANDLE )
        PostMessage( MainPushButton[row].w, WM_SETFONT, (WPARAM) MyFont, FALSE );

    x += width;
    width  = Char_Width * ColWidth[TCP_ADDRESS_COLUMN];
    id++;
    AddressTbox[row].id = id;
    AddressTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("192.168.254.100"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        StatusWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    if ( MyFont )
        PostMessage( AddressTbox[row].w, WM_SETFONT, (WPARAM) MyFont, FALSE );

    x += width;
    width  = Char_Width * ColWidth[CONNECTED_COLUMN];
    id++;
    ConnectedTbox[row].id = id;
    ConnectedTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("Startup"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        StatusWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    if ( MyFont )
        PostMessage( ConnectedTbox[row].w, WM_SETFONT, (WPARAM) MyFont, FALSE );

    x += width;
    width  = Char_Width * ColWidth[MACHINE_NAME_COLUMN];
    id++;
    MachineTbox[row].id = id;
    MachineTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("MA"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        StatusWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    if ( MyFont )
        PostMessage( MachineTbox[row].w, WM_SETFONT, (WPARAM) MyFont, FALSE );

    x += width;
    width  = Char_Width * ColWidth[PART_NAME_COLUMN];
    id++;
    PartTbox[row].id = id;
    PartTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("PA"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        StatusWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    if ( MyFont )
        PostMessage( PartTbox[row].w, WM_SETFONT, (WPARAM) MyFont, FALSE );

    x += width;
    width  = Char_Width * ColWidth[MONITOR_STATUS_COLUMN];
    id++;
    MonitorStatusTbox[row].id = id;
    MonitorStatusTbox[row].w = CreateWindow(
        TEXT("static"),
        TEXT("Startup Status"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y, width, height,
        StatusWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    if ( MyFont )
        PostMessage( MonitorStatusTbox[row].w, WM_SETFONT, (WPARAM) MyFont, FALSE );

    x += width + 10;
    width  = Char_Width;
    Dnu[row] = CreateWindow(
        TEXT("static"),
        EmptyString,
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, y+1, width, height,
        StatusWindow.handle(),
        (HMENU) 2000,
        MainInstance,
        NULL
        );

    if ( MyFont )
        PostMessage( Dnu[row], WM_SETFONT, (WPARAM) MyFont, FALSE );

    x += width;
    MainClientWidth = x;
    MainClientWidth += Char_Width;

    id++;
    y += Line_Height;
    max_y = y;
    }

/*
------
Titles
------ */
x = r.left;
for ( col=0; col<NofCols; col++ )
    {
    width  = Char_Width * ColWidth[col];
    s = resource_string( BOARD_STRING + col );
    w = CreateWindow(
        TEXT("static"),
        s.text(),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        x, r.top, width, height,
        StatusWindow.handle(),
        (HMENU) id,
        MainInstance,
        NULL
        );

    if ( MyFont )
        PostMessage( w.handle(), WM_SETFONT, (WPARAM) MyFont, FALSE );

    x += width;
    id++;
    }

display_board_info();
if ( !IsRemoteMonitor )
    CurrentMachine.name.get_text( MachineTbox[0].w );

CurrentMachine.find( ComputerName, CurrentMachine.name );
if ( IsRemoteMonitor )
    CurrentMachine.monitor_flags |= MA_MONITORING_ON;
load_current_part();
CurrentBoardIndex = current_board_index();
load_machine_vars();
set_checkbox( MainPushButton[CurrentBoardIndex].w, TRUE );

TopOfListBox = y;
TerminalWindow.get_client_rect( r );
r.top = TerminalMenu.mr.bottom;
r.bottom -= MainEboxHeight;
id = MAIN_LISTBOX_ID;
MainListBox.init( CreateWindow(TEXT ("listbox"),
                  NULL,
                  WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOSEL,
                  0, 0, r.width(), r.height(),                       // x, y, w, h
                  TerminalWindow.handle(),
                  (HMENU) id,
                  MainInstance,
                  NULL)
                );


y = r.bottom;
id = MAIN_EBOX;
MainEbox = CreateWindow( TEXT("EDIT"),
              NULL,
              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
              0, y, r.width(), MainEboxHeight,
              TerminalWindow.handle(),
              (HMENU) id,
              MainInstance,
              NULL );

OriEditControlProc = (WNDPROC) SetWindowLong( MainEbox, GWL_WNDPROC, (LONG) edit_control_proc );
}

/***********************************************************************
*                   RETRY_UPLOAD_TO_ABORTED_MACHINE                    *
***********************************************************************/
static void retry_upload_to_aborted_machine()
{
STRING_CLASS s;

s.get_text( UploadAbortedDialog.control(ABORTED_MACHINE_NAME_TBOX) );
send_command( s.text(), RESET_MACHINE_CMD, true );
}

/***********************************************************************
*                   GET_NEXT_UPLOAD_ABORTED_MACHINE                    *
***********************************************************************/
static bool get_next_upload_aborted_machine()
{
if ( UploadAbortedList.count() )
    {
    UploadAbortedList.rewind();
    UploadAbortedList.next();
    set_text( AbortedMachineNameTbox, UploadAbortedList.text() );
    UploadAbortedList.remove();
    return true;
    }
return false;
}

/***********************************************************************
*                  SET_UPLOAD_ABORTED_SETUP_STRINGS                    *
***********************************************************************/
static void set_upload_aborted_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,                       TEXT("OK_STRING") },
    { IDCANCEL,                   TEXT("CANCEL") },
    { PREV_UPLOAD_ABORTED_STATIC, TEXT("PREV_UPLOAD_ABORT") },
    { OK_TO_RETRY_STATIC,         TEXT("OK_TO_RETRY") },
    { UA_MACHINE_STATIC,          TEXT("MACHINE_INTRO_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    ABORTED_MACHINE_NAME_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, TEXT("CONNECTION_RESTORED") );
}

/***********************************************************************
*                        UPLOAD_ABORTED_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK upload_aborted_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int id;
HWND       w;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        UploadAbortedDialog = hWnd;
        set_upload_aborted_setup_strings( hWnd );
        AbortedMachineNameTbox = UploadAbortedDialog.control( ABORTED_MACHINE_NAME_TBOX );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                retry_upload_to_aborted_machine();
                if ( !get_next_upload_aborted_machine() )
                    UploadAbortedDialog.hide();
                return TRUE;

            case IDCANCEL:
                if ( !get_next_upload_aborted_machine() )
                    UploadAbortedDialog.hide();
                return TRUE;
            }
        break;

    case WM_CTLCOLORSTATIC:
        w = (HWND) lParam;
        if ( w == AbortedMachineNameTbox )
            {
            SetBkColor( (HDC) wParam, OrangeColor );
            return (int) OrangeBackgroundBrush;
            }
        break;

    case WM_CLOSE:
        UploadAbortedDialog.hide();
        return FALSE;

    case WM_DESTROY:
        UploadAbortedDialog = (HWND) 0;
        break;
    }

return FALSE;
}

/***********************************************************************
*                      SHOW_UPLOAD_ABORTED_DIALOG                      *
***********************************************************************/
static void show_upload_aborted_dialog( int i )
{
STRING_CLASS s;
s.get_text( MachineTbox[i].w );
if ( !s.isempty() )
    {
    if ( !is_suretrak_controlled(i) )
        {
        send_command( s.text(), RESET_MACHINE_CMD, true );
        }
    else if ( !UploadAbortedDialog.is_visible() )
        {
        s.set_text( UploadAbortedDialog.control(ABORTED_MACHINE_NAME_TBOX) );
        UploadAbortedDialog.show();
        }
    else
        {
        UploadAbortedList.append( s.text() );
        }
    }
}

/***********************************************************************
*                       DO_NEW_FT2_CONNECT_STATE                       *
***********************************************************************/
static void do_new_ft2_connect_state( WPARAM wParam, LPARAM lParam )
{
int      i;
unsigned new_state;
TCHAR *  s;

i = (int) wParam;
new_state = 0;

if ( i != NO_INDEX && i < NofRows )
    {
    new_state = (unsigned) lParam;

    if ( new_state == NO_FT2_CONNECT_STATE || new_state & NOT_MONITORED_STATE )
        {
        s = NoConnectState.text();
        }
    else if ( new_state & NOT_CONNECTED_STATE )
        {
        s = NotConnected.text();
        }
    else if ( new_state & CONNECTING_STATE )
        {
        s = Connecting.text();
        }
    else if ( new_state & CONNECTED_STATE )
        {
        s = Connected.text();
        if ( new_state & UPLOAD_WAS_ABORTED_STATE )
            show_upload_aborted_dialog(i);
        }
    else
        {
        s = unknown_string();
        }

    if ( i < NofRows )
        {
        set_text( ConnectedTbox[i].w, s );
        if ( i == CurrentBoardIndex )
            set_text( CurrentValuesCommStatusTb, s );
        if ( new_state & NOT_MONITORED_STATE )
            {
            set_text( MonitorStatusTbox[i].w, NotMonitored.text() );
            if ( i == CurrentBoardIndex )
                set_text( CurrentValuesMonitorStatusTb, NotMonitored.text() );
            }
        else if ( new_state & ZERO_STROKE_LENGTH_STATE )
            {
            set_text( MonitorStatusTbox[i].w, ZeroStrokeLength.text() );
            if ( i == CurrentBoardIndex )
                set_text( CurrentValuesMonitorStatusTb, ZeroStrokeLength.text() );
            }
        }
    }
}

/***********************************************************************
*                           CREATE_TOOLTIPS                            *
* The first time you call this it creates the tooltip window.          *
* Thereafter it just updates the text.                                 *
***********************************************************************/
static void create_tooltips()
{
static TCHAR default_text[] = TEXT( "None" );

WINDOW_CLASS    w;
RECTANGLE_CLASS r;
LIMIT_SWITCH_WIRE_CLASS lsw;
PARAMETER_CLASS p;

int i;
int led;
int module;
int wire;
UINT id;

long halfway_across;
long halfway_up;
BOOLEAN firstime;
STRING_CLASS s;

TOOLINFO ti;

lsw.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
p.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

firstime = FALSE;
if ( !ToolTipWindow )
    {
    firstime = TRUE;
    ToolTipWindow = CreateWindowEx(
        0,
        TOOLTIPS_CLASS,     /* Class name */
        0,                  /* Title */
        TTS_ALWAYSTIP,      /* Style */
        CW_USEDEFAULT,      /* x */
        CW_USEDEFAULT,      /* x */
        CW_USEDEFAULT,      /* x */
        CW_USEDEFAULT,      /* x */
        CurrentValuesDialog.handle(),
        0,
        MainInstance,
        0
        );
    SendMessage( ToolTipWindow, TTM_SETDELAYTIME, TTDT_AUTOPOP, 30000 );
    }

ti.cbSize = sizeof(TOOLINFO);
ti.uFlags = 0;
ti.hwnd   = CurrentValuesDialog.handle();
ti.hinst  = MainInstance;
ti.uId    = OPTO_TOOLTIP_1;

if ( firstime )
    {
    /*
    -------------------------------------------------------------
    Input messages are constant. Assume this is a control system.
    ------------------------------------------------------------- */
    wire = 16;
    id = CONTROL_INPUT_18_STRING;
    for ( module=0; module<4; module++ )
        {
        w = CurrentValuesDialog.control( INPUT_1_OPTO+module );
        w.get_move_rect( r );
        halfway_across = (r.left + r.right) / 2;
        halfway_up = (r.top + r.bottom) / 2;
        for ( led=0; led<4; led++ )
            {
            wire++;
            ti.lpszText = default_text;
            if ( wire > 17 && wire < 30 )
                {
                ti.lpszText = resource_string( id );
                id++;
                }
            ti.rect.left   = r.left;
            ti.rect.right  = r.right;
            ti.rect.top    = r.top;
            ti.rect.bottom = r.bottom;
            if ( led == 0 )
                {
                ti.rect.left = halfway_across;
                ti.rect.top  = halfway_up;
                }
            else if ( led == 1 )
                {
                ti.rect.left   = halfway_across;
                ti.rect.bottom = halfway_up;
                }
            else if ( led == 2 )
                {
                ti.rect.right = halfway_across;
                ti.rect.top   = halfway_up;
                }
            else if ( led == 3 )
                {
                ti.rect.right  = halfway_across;
                ti.rect.bottom = halfway_up;
                }
            SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );
            }
        }
    }

/*
-------
Outputs
------- */
wire = 16;
ti.uId    = OPTO_TOOLTIP_17;
for ( module=0; module<4; module++ )
    {
    w = CurrentValuesDialog.control( OUTPUT_1_OPTO+module );
    w.get_move_rect( r );
    halfway_across = (r.left + r.right) / 2;
    halfway_up = (r.top + r.bottom) / 2;
    for ( led=0; led<4; led++ )
        {
        wire++;
        ti.lpszText = default_text;
        if ( wire == 17 )
            s = TEXT( "Slow Shot Output  " );
        else if ( wire == 18 )
            s = TEXT( "Accumulator  " );
        else if ( wire == 21 && UsingValveTestOutput )
            s = TEXT( "Valve Test in Operation  " );
        else if ( wire == 28 )
            s = TEXT( "Warning (Active Low)  " );
        else
            s = EmptyString;
        if ( wire == DownTimeAlarmWire )
            s += TEXT( "Downtime Alarm Wire  " );
        if ( wire == MonitorWire )
            s += TEXT( "Monitor Wire  " );

        for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
            {
            if ( lsw.wire_number(i) == wire )
                {
                s += TEXT( "Limit Switch #" );
                s += i+1;
                s += TEXT( "  " );
                }
            }

        for ( i=0; i<MAX_PARMS; i++ )
            {
            if ( wire == (int) p.parameter[i].limits[ALARM_MIN].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=Low Alarm  " );
                }
            if ( wire == (int) p.parameter[i].limits[ALARM_MAX].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=High Alarm  " );
                }
            if ( wire == (int) p.parameter[i].limits[WARNING_MIN].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=Low Warning  " );
                }
            if ( wire == (int) p.parameter[i].limits[WARNING_MAX].wire_number )
                {
                s += p.parameter[i].name;
                s += TEXT( "=High Warning  " );
                }
            }

        if ( s.len() > 0 )
            ti.lpszText = s.text();

        ti.rect.left   = r.left;
        ti.rect.right  = r.right;
        ti.rect.top    = r.top;
        ti.rect.bottom = r.bottom;
        if ( led == 0 )
            {
            ti.rect.left = halfway_across;
            ti.rect.top  = halfway_up;
            }
        else if ( led == 1 )
            {
            ti.rect.left   = halfway_across;
            ti.rect.bottom = halfway_up;
            }
        else if ( led == 2 )
            {
            ti.rect.right = halfway_across;
            ti.rect.top   = halfway_up;
            }
        else if ( led == 3 )
            {
            ti.rect.right  = halfway_across;
            ti.rect.bottom = halfway_up;
            }

        if ( firstime )
            SendMessage( ToolTipWindow, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti );
        else
            SendMessage( ToolTipWindow, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti );
        ti.uId++;
        }
    }
}

/***********************************************************************
*                        REFRESH_CURRENT_VALUES                        *
***********************************************************************/
static void refresh_current_values()
{
NAME_CLASS   filename;
NAME_CLASS   s;
WINDOW_CLASS w;

s = current_version_string();
s.set_text( CurrentValuesDialog.control(FLASH_VERSION_TBOX) );

s = current_upload_file_name();
s.set_text( CurrentValuesDialog.control(UPLOAD_FILE_NAME_TBOX) );

filename = s;
filename.replace( DotTxt, DotMsg );
s.get_exe_dir_file_name( filename.text() );
if ( !s.file_exists() )
    s.get_exe_dir_file_name( CONTROL_MESSAGE_FILE );
if ( s.file_exists() )
    StepMessage.load( s.text() );

w = CurrentValuesDialog.control( PRESSURE_CONTROL_STATIC );
w.show( HasPressureControl );

w = CurrentValuesDialog.control( BINARY_VALVE_STATIC );
if ( CurrentMachine.has_binary_valve() )
    w.show();
else
    w.hide();

w = CurrentValuesDialog.control( TOGGLE_IO_DISPLAY_PB );
w.show();
create_tooltips();
}

/***********************************************************************
*                       REFRESH_CURRENT_MACHINE                        *
***********************************************************************/
static void refresh_current_machine( STRING_CLASS & new_machine_name )
{
CurrentMachine.find( ComputerName, new_machine_name );
if ( IsRemoteMonitor )
    CurrentMachine.monitor_flags |= MA_MONITORING_ON;
load_current_part();
load_machine_vars();
}

/***********************************************************************
*                      NEW_MONITOR_SETUP_CALLBACK                      *
***********************************************************************/
void new_monitor_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
BYTE        * bp;
MACHINE_CLASS m;
int           n;
DWORD         nof_bytes;
STRING_CLASS  s;
HWND          w;

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

nof_bytes = DdeGetData( edata, NULL, 0, 0 );
if ( nof_bytes == 0 )
    return;

n = nof_bytes;

#ifdef UNICODE
n++;
n /= sizeof(TCHAR);
#endif

if ( !s.init(n) )
    return;

bp = (BYTE *) s.text();
nof_bytes = DdeGetData( edata, bp, nof_bytes, 0 );
if ( nof_bytes == 0 || MyMonitorSetupCount>0 )
    {
    if ( MyMonitorSetupCount > 0 )
        MyMonitorSetupCount--;
    return;
    }

*(s.text()+n) = NullChar;
fix_dde_name( s.text() );

if ( !m.find(ComputerName, s) )
    return;

if ( m.ft_board_number == current_board_number() )
    {
    if ( (CurrentMachine.name == m.name) || (m.monitor_flags & MA_MONITORING_ON) )
        {
        if ( IsRemoteMonitor )
            m.monitor_flags |= MA_MONITORING_ON;

        refresh_current_machine( s );

        /*
        ---------------------------------------------------------------------
        If this machine is not being monitored then close the realtime window
        --------------------------------------------------------------------- */
        if ( (CurrentWindow != StatusWindow) && !(m.monitor_flags & MA_MONITORING_ON) )
            set_current_window( StatusWindow );
        }
    }

if ( CurrentWindow == CurrentValuesDialog && (CurrentMachine.name==s) )
    {
    w = CurrentValuesDialog.control( VELOCITY_UNITS_TBOX );
    set_text( w, units_name(CurrentPart.velocity_units) );

    w = CurrentValuesDialog.control( POSITION_UNITS_TBOX );
    set_text( w, units_name(CurrentPart.distance_units) );
    }

MainWindow.post( WM_NEWSETUP, 0, (LPARAM) s.release() );
}

/***********************************************************************
*                      POSITION_BUTTON_CHILD                           *
***********************************************************************/
static void position_button_child( WINDOW_CLASS & w )
{
WINDOW_CLASS pbw;
long x;
long y;
long width;

RECTANGLE_CLASS parent_rect;
RECTANGLE_CLASS pbr;
RECTANGLE_CLASS r;

parent_rect = MainWindowClientRect;
w.get_move_rect( r );

pbw = MainWindow.control( F2_RADIO );
pbw.get_move_rect( pbr );

/*
---------------------------------------------------------
Center the dialog in the area to the right of the buttons
--------------------------------------------------------- */
x     = pbr.width() + 2*pbr.left;
width = parent_rect.width() - x;
width -= r.width();
width /= 2;

if ( width > 0 )
    x += width;

y = parent_rect.height() - r.height();
if ( y > 0 )
    y /= 2;
else
    y = 0;
r.moveto( x, y );
w.move( r );
}

/***********************************************************************
*                   SEND_COMMAND_TO_CURRENT_MACHINE                    *
***********************************************************************/
static void send_command_to_current_machine( TCHAR * command, bool need_to_set_event )
{
static TCHAR NullMessage[] = TEXT("NULL");
static STRING_CLASS s;
TCHAR * cp;

if ( command )
    cp = command;
else
    cp = NullMessage;

if ( TerminalIsVisible )
    {
    MainListBox.set_current_index( MainListBox.add(cp) );
    scroll_listbox_to_last_entry();
    }

if ( command )
    {
    s = command;
    s += CrChar;
    send_command( CurrentMachine.name, s.text(), need_to_set_event );
    }
else
    {
    send_command( CurrentMachine.name, 0, need_to_set_event );
    }
}

/***********************************************************************
*                    SET_DEACTIVATE_SETUP_STRINGS                      *
***********************************************************************/
static void set_deactivate_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,                    TEXT("OK_STRING") },
    { IDCANCEL,                TEXT("CANCEL") },
    { WISH_TO_CONTINUE_STATIC, TEXT("WISH_TO_CONTINUE") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, TEXT("DEACTIVATE_CONTROL") );
}

/***********************************************************************
*                    DEACTIVATE_CONTROL_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK deactivate_control_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_deactivate_setup_strings( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                EndDialog( hWnd, TRUE );
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, FALSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         FILL_MACHINE_LISTBOX                         *
***********************************************************************/
static BOOLEAN fill_machine_listbox( HWND hWnd )
{
MACHINE_NAME_LIST_CLASS m;
COMPUTER_CLASS          c;
LISTBOX_CLASS           lb;
STRING_CLASS            s;

c.rewind();
while ( c.next() )
    {
    if ( !c.is_this_computer() )
        {
        if ( c.is_present() )
            {
            m.add_computer( c.name() );
            }
        }
    }

if ( m.count() > 0 )
    {
    lb.init( hWnd, CHOOSE_MACHINE_LB );
    m.rewind();
    while ( m.next() )
        {
        s = m.computer_name();
        s += TwoSpacesString;
        s += m.name();
        lb.add( s.text() );
        }
    }

return TRUE;
}

/***********************************************************************
*                           REMOTE_SHUTDOWN                            *
***********************************************************************/
static void remote_shutdown()
{
const unsigned mask = MA_MONITORING_ON;
NAME_CLASS      s;
SETPOINT_CLASS  sp;
unsigned        my_monitor_flags;

ShutdownStatusListbox.add( TEXT("Turning monitoring back on...") );
s.get_machset_csvname( ComputerName, CurrentMachine.name );
if ( sp.get(s) )
    {
    my_monitor_flags = sp[MACHSET_MONITOR_FLAGS_INDEX].value.uint_value();
    my_monitor_flags |= mask;
    sp[MACHSET_MONITOR_FLAGS_INDEX].value = my_monitor_flags;
    sp.put( s );
    }

ShutdownStatusListbox.add( TEXT("Poking the monitor topic...") );
MyMonitorSetupCount++;
poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentMachine.name.text() );
}

/***********************************************************************
*                        LOAD_ONE_MACHINE_ONLY                         *
***********************************************************************/
void load_one_machine_only( HWND hWnd )
{
const unsigned mask = ~MA_MONITORING_ON;
STRING_CLASS m;
STRING_CLASS c;
NAME_CLASS   s;
SETPOINT_CLASS sp;
unsigned    my_monitor_flags;
LISTBOX_CLASS lb;
TCHAR * cp;

lb.init( hWnd, CHOOSE_MACHINE_LB );
c = lb.selected_text();
if ( !c.isempty() )
    {
    cp = findchar( SpaceChar, c.text() );
    if ( cp )
        {
        *cp = NullChar;
        cp += 2;
        m = cp;
        ComputerName = c;
        refresh_current_machine( m );
        /*
        ------------------------------------------------------------------------
        m = TEXT( "Computer = " );
        m += ComputerName;
        m += TEXT( "  Machine = " );
        m += CurrentMachine.name;
        m += TEXT( "  Part = " );
        m += CurrentPart.name;
        MessageBox( 0, "Current Machine set to the following", m.text(), MB_OK );
        ------------------------------------------------------------------------- */
        IsRemoteMonitor = TRUE;
        if ( CurrentMachine.monitor_flags & MA_MONITORING_ON )
            {
            my_monitor_flags = (unsigned) CurrentMachine.monitor_flags;
            my_monitor_flags &= mask;
            s.get_machset_csvname( ComputerName, CurrentMachine.name );
            if ( sp.get(s) )
                {
                sp[MACHSET_MONITOR_FLAGS_INDEX].value = my_monitor_flags;
                sp.put( s );
                }
            }
        }
    }
}

/***********************************************************************
*                  SET_CHOOSE_MONITOR_SETUP_STRINGS                    *
***********************************************************************/
static void set_choose_monitor_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { CHOOSE_MONITOR_MA_STATIC, TEXT("CHOOSE_MONITOR_MA") },
    { IDOK, TEXT("BEGIN_MONITORING") },
    { IDCANCEL, TEXT("CANCEL") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    CHOOSE_MACHINE_LB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, TEXT("CHOOSE_MON_MA_TITLE") );
}

/***********************************************************************
*                CHOOSE_MONITOR_MACHINE_DIALOG_PROC                    *
***********************************************************************/
BOOL CALLBACK choose_monitor_machine_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_choose_monitor_setup_strings( hWnd );
        fill_machine_listbox( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                load_one_machine_only( hWnd );
                EndDialog( hWnd, TRUE );
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, FALSE );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                SEND_OUTPUT_BITS_TO_CURRENT_MACHINE                   *
***********************************************************************/
void send_output_bits_to_current_machine( unsigned bits, BOOLEAN set_bits )
{
static TCHAR OnString[]  = TEXT("OO1=" );
static TCHAR OffString[] = TEXT("OO1=~" );
TCHAR s[30];
TCHAR * cp;

if ( set_bits )
    cp = copystring( s, OnString );
else
    cp = copystring( s, OffString );

ultohex(cp, (unsigned long) bits );

send_command_to_current_machine( s, true );
}

/***********************************************************************
*                        STOP_HIRES_SENSOR_TEST                        *
***********************************************************************/
static void stop_hires_sensor_test()
{
unsigned bit;

CurrentHiresSensorPeriod = -1;

bit = wirebit( HiresSensorStartShotWire );
bit |= wirebit( HiresSensorFollowThruWire );
bit |= wirebit( HiresSensorRetractWire );
if ( bit > 0 )
    send_output_bits_to_current_machine( bit, FALSE );

InvalidateRect( Start_Shot_Tbox_Handle, 0, TRUE );
InvalidateRect( Follow_Thru_Tbox_Handle, 0, TRUE );
InvalidateRect( Retract_Tbox_Handle, 0, TRUE );
}

/***********************************************************************
*                      DO_HIRES_SENSOR_TEST_TIMER                      *
***********************************************************************/
static void do_hires_sensor_test_timer( HWND w )
{
int      wire_number;
unsigned bit;
int      ms;
HWND     tw;

if ( CurrentHiresSensorPeriod < NO_SHOT_PERIOD )
    return;

CurrentHiresSensorPeriod++;
if ( CurrentHiresSensorPeriod > END_OF_SHOT_PERIOD )
    CurrentHiresSensorPeriod = START_SHOT_PERIOD;

/*
-------------------------
Turn off the current wire
------------------------- */
tw          = 0;
wire_number = NO_WIRE;
switch ( CurrentHiresSensorPeriod )
    {
    case FOLLOW_THRU_PERIOD:
        wire_number = HiresSensorStartShotWire;
        tw = Start_Shot_Tbox_Handle;
        break;

    case RETRACT_PERIOD:
        wire_number = HiresSensorFollowThruWire;
        tw = Follow_Thru_Tbox_Handle;
        break;

    case END_OF_SHOT_PERIOD:
        wire_number = HiresSensorRetractWire;
        tw = Retract_Tbox_Handle;
        break;
    }

if ( wire_number != NO_WIRE )
    {
    bit = wirebit( wire_number );
    send_output_bits_to_current_machine( bit, FALSE );
    }

if ( tw )
    InvalidateRect( tw, 0, TRUE );

/*
--------------------
Turn on the new wire
-------------------- */
tw          = 0;
wire_number = NO_WIRE;
switch ( CurrentHiresSensorPeriod )
    {
    case START_SHOT_PERIOD:
        wire_number = HiresSensorStartShotWire;
        tw = Start_Shot_Tbox_Handle;
        break;
    case FOLLOW_THRU_PERIOD:
        wire_number = HiresSensorFollowThruWire;
        tw = Follow_Thru_Tbox_Handle;
        break;
    case RETRACT_PERIOD:
        wire_number = HiresSensorRetractWire;
        tw = Retract_Tbox_Handle;
        break;
    }

if ( wire_number != NO_WIRE )
    {
    bit = wirebit( wire_number );
    send_output_bits_to_current_machine( bit, TRUE );
    }

if ( tw )
    InvalidateRect( tw, 0, TRUE );

ms = HiresSensorPeriodMs[CurrentHiresSensorPeriod];
if ( ms > 0 )
    SetTimer( MainWindow.handle(), HIRES_SENSOR_TEST_TIMER_ID, (UINT) ms, 0 );
else
    CurrentHiresSensorPeriod = -1;
}

/***********************************************************************
*                    SET_GLOBALS_FROM_EBOXES                           *
***********************************************************************/
static void set_globals_from_eboxes( HWND w )
{
int          end_ms;
int          start_ms;
STRING_CLASS s;

s.get_text( w, FOLLOW_THRU_TIME_EBOX );
start_ms = s.int_value();
HiresSensorPeriodMs[START_SHOT_PERIOD]  = start_ms * 1000;

s.get_text( w, RETRACT_TIME_EBOX );
end_ms = s.int_value();

HiresSensorPeriodMs[FOLLOW_THRU_PERIOD] = (end_ms - start_ms) * 1000;
start_ms = end_ms;
s.get_text( w, RETRACT_END_TIME_EBOX );
end_ms = s.int_value();
HiresSensorPeriodMs[RETRACT_PERIOD]     = (end_ms - start_ms) * 1000;

start_ms = end_ms;
s.get_text( w, CYCLE_TIME_EBOX );
end_ms = s.int_value();
HiresSensorPeriodMs[END_OF_SHOT_PERIOD] = (end_ms - start_ms) * 1000;

s.get_text( w, START_SHOT_WIRE_EBOX );
HiresSensorStartShotWire  = s.int_value();

s.get_text( w, FOLLOW_THRU_WIRE_EBOX );
HiresSensorFollowThruWire = s.int_value();

s.get_text( w, RETRACT_WIRE_EBOX );
HiresSensorRetractWire = s.int_value();
}

/***********************************************************************
*                    GET_HIRES_SENSOR_TEST_CONFIG                      *
***********************************************************************/
void get_hires_sensor_test_config( HWND hWnd )
{
int            i;
int            id;
NAME_CLASS     s;
SETPOINT_CLASS sp;

s.get_exe_dir_file_name( HiresSensorConfigFile );
if ( !sp.get(s) )
    {
    sp.setsize( HIRES_SENSOR_SETPOINT_COUNT );
    for ( i=0; i<HIRES_SENSOR_SETPOINT_COUNT; i++ )
        sp[i].value = 0;
    }

id = START_SHOT_WIRE_EBOX;
for ( i=0; i<HIRES_SENSOR_SETPOINT_COUNT; i++ )
    sp[i].value.set_text( hWnd, id++ );

set_globals_from_eboxes( hWnd );
}

/***********************************************************************
*                    PUT_HIRES_SENSOR_TEST_CONFIG                      *
***********************************************************************/
void put_hires_sensor_test_config( HWND hWnd )
{
static TCHAR * Description[] = {
    { TEXT( "Start Shot Wire" ) },
    { TEXT( "Follow Thru Wire" ) },
    { TEXT( "Retract Wire" ) },
    { TEXT( "Cycle Time" ) },
    { TEXT( "Follow Thru Time" ) },
    { TEXT( "Retract Time" ) },
    { TEXT( "Retract End Time" ) }
    };
int i;
int id;
NAME_CLASS     s;
SETPOINT_CLASS sp;

sp.setsize( HIRES_SENSOR_SETPOINT_COUNT );
id = START_SHOT_WIRE_EBOX;
for ( i=0; i<HIRES_SENSOR_SETPOINT_COUNT; i++ )
    {
    sp[i].value.get_text( hWnd, id++ );
    sp[i].desc = Description[i];
    }

s.get_exe_dir_file_name( HiresSensorConfigFile );
sp.put( s );
}

/***********************************************************************
*                    HIRES_SENSOR_TEST_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK hires_sensor_test_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
HWND w;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        HiresSensorTestDialog = hWnd;
        get_hires_sensor_test_config( hWnd );
        Start_Shot_Tbox_Handle  = HiresSensorTestDialog.control( START_SHOT_WIRE_TBOX );
        Follow_Thru_Tbox_Handle = HiresSensorTestDialog.control( FOLLOW_THRU_WIRE_TBOX );
        Retract_Tbox_Handle     = HiresSensorTestDialog.control( RETRACT_WIRE_TBOX );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                put_hires_sensor_test_config( hWnd );
                set_globals_from_eboxes( hWnd );
                if ( CurrentHiresSensorPeriod < NO_SHOT_PERIOD )
                    {
                    CurrentHiresSensorPeriod = NO_SHOT_PERIOD;
                    do_hires_sensor_test_timer( hWnd );
                    }
                return TRUE;

            case IDCANCEL:
                stop_hires_sensor_test();
                return TRUE;
            }
        break;

    case WM_CLOSE:
        HiresSensorTestDialog.hide();
        return FALSE;

    case WM_DESTROY:
        HiresSensorTestDialog = (HWND) 0;
        break;

    case WM_CTLCOLORSTATIC:
        w = (HWND) lParam;
        if ( (w == Start_Shot_Tbox_Handle && CurrentHiresSensorPeriod == START_SHOT_PERIOD) || (w == Follow_Thru_Tbox_Handle && CurrentHiresSensorPeriod == FOLLOW_THRU_PERIOD) ||(w == Retract_Tbox_Handle && CurrentHiresSensorPeriod == RETRACT_PERIOD) )
            {
            SetBkColor( (HDC) wParam, GreenColor );
            return (int) GreenBackgroundBrush;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         SAVE_NULL_VALUE                              *
***********************************************************************/
static void save_null_value()
{
NAME_CLASS filename;

temp_message( SavingString.text() );

filename.get_ft2_global_settings_file_name( ComputerName, CurrentMachine.name );
FtiiAutoNull.put_into( filename );

set_text( GlobalParameterDialog.control(NULL_DAC_EDITBOX), int32toasc(FtiiAutoNull[0].ival()) );
}

/***********************************************************************
*                        SEND_AUTO_NULL_COMMAND                        *
***********************************************************************/
static void send_auto_null_command()
{
send_command_to_current_machine( TEXT("OC6=1"),  true );
NullStatusMessage.set_title( resource_string(AUTO_NULL_BEGIN_STRING) );
ManualInfoTbox.hide();
}

/***********************************************************************
*                      SEND_MANUAL_NULL_COMMAND                        *
***********************************************************************/
static void send_manual_null_command()
{
ManualNullInProgress = true;
send_command_to_current_machine( TEXT("V350=0"), false );
send_command_to_current_machine( TEXT("VF"), true );
Sleep( 100 );
send_command_to_current_machine( TEXT("OC5=1"),  true );
NullStatusMessage.set_title( resource_string(MANUAL_NULLING_STRING) );
ManualInfoTbox.show();

FtiiAutoNull[0].set( NULL_DAC_OFFSET_VN, (unsigned) 0 );
save_null_value();
}

/***********************************************************************
*                        DO_NEW_SET_NULL_VALUES                        *
***********************************************************************/
static void do_new_set_null_values()
{
static int          i;
static HWND         w;
static unsigned     x;
static STRING_CLASS ns;
static bool         firstime = true;

i = current_lvdt_feedback_index( DAC_1 );
if ( firstime || AnalogValue[i] != CurrentAnalogValue[i] )
    {
    AnalogValue[i] = CurrentAnalogValue[i];

    x = AnalogValue[i];
    ns = ascii_dac_volts_3( x );
    w  = SetNullDialog.control( NULL_VOLTS_TBOX );
    ns.set_text( w );
    }

firstime = false;
}

/***********************************************************************
*                      DO_NEW_SET_NULL_ASCII_DATA                      *
***********************************************************************/
void do_new_set_null_ascii_data( LPARAM lParam )
{
static const int UNUSABLE_NULL_LEVEL = 0;
static const int AUTO_NULL_FAIL      = 1;
static const int AUTO_NULL_SUCCESS   = 2;
static const int NULL_ALREADY_GOOD   = 3;

struct MY_ENTRY {
    TCHAR * s;
    UINT    id;
    };

static MY_ENTRY response[] = {
    { TEXT("BEGINNING_NULL"),    UNUSABLE_NULL_LEVEL_STRING },    /* BEGINNING_NULL_EXCEEDS_ALLOWABLE_RANGE  */
    { TEXT("AUTO_NULL_FAILED"),  AUTO_NULL_FAIL_STRING      },    /* AUTO_NULL_FAILED_UNABLE_TO_CORRECT      */
    { TEXT("AUTO_NULL_SUCCESS"), AUTO_NULL_SUCCESS_STRING   },    /* AUTO_NULL_SUCCESS                       */
    { TEXT("NULL_ALREADY_GOOD"), NULL_ALREADY_GOOD_STRING   }     /* NULL_ALREADY_GOOD_NO_NEED_TO_CORRECT    */
    };

int nof_responses = sizeof(response)/sizeof(MY_ENTRY);

TCHAR * cp;
TCHAR * sorc;
int     i;

sorc = (TCHAR *) lParam;
if ( sorc )
    {
    for( i=0; i<nof_responses; i++ )
        {
        cp = findstring( response[i].s, sorc );
        if ( cp )
            {
            switch ( i )
                {
                case UNUSABLE_NULL_LEVEL:
                    break;

                case AUTO_NULL_FAIL:
                    break;

                case AUTO_NULL_SUCCESS:
                    cp = findchar( PoundChar, cp );
                    if ( cp )
                        {
                        cp++;
                        FtiiAutoNull[0] = cp;
                        save_null_value();
                        }
                    break;

                case NULL_ALREADY_GOOD:
                    break;
                }

            NullStatusMessage.set_title( resource_string(response[i].id) );
            break;
            }
        }

    delete[] sorc;
    }
}
/***********************************************************************
*                   POSITION_SHUTDOWN_STATUS_DIALOG                    *
***********************************************************************/
static void position_shutdown_status_dialog( HWND hWnd )
{
WINDOW_CLASS w;
RECTANGLE_CLASS rp;
RECTANGLE_CLASS r;

w = hWnd;
w.get_move_rect( r );
MainWindow.get_client_rect( rp );

if ( r.height() < rp.height() )
    {
    r.bottom = r.top + rp.height();
    w.move( r );
    }
}

/***********************************************************************
*                        SHUTDOWN_STATUS_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK shutdown_status_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
WINDOW_CLASS w;
if ( msg == WM_INITDIALOG )
    {
    w = GetDlgItem( hWnd, SHUTDOWN_STATUS_LB );
    set_statics_font( w );
    position_shutdown_status_dialog( hWnd );
    }
return FALSE;
}

/***********************************************************************
*                  SET_NULL_SETUP_STRINGS                    *
***********************************************************************/
static void set_null_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rsn[] = {
    { SET_NULL_VOLTS_STATIC,    TEXT("SET_NULL_VOLTAGE") },
    { NULL_VOLTAGE_STATIC,      TEXT("NULL_VOLTAGE") },
    { AUTO_NULL_PB,             TEXT("AUTO_NULL") },
    { MANUAL_NULL_PB,           TEXT("MANUAL_NULL") },
    { MANUAL_INSTRUCTIONS_TBOX, TEXT("MANUAL_INSTRUCTIONS") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    SET_NULL_MSG_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                          SET_NULL_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK set_null_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;

id = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SetNullDialog = hWnd;
        set_null_setup_strings( hWnd );
        NullStatusMessage = SetNullDialog.control( SET_NULL_MSG_TBOX );
        ManualInfoTbox    = SetNullDialog.control( MANUAL_INSTRUCTIONS_TBOX );
        NullStatusMessage.set_title( EmptyString );

        position_button_child( SetNullDialog );
        SetNullDialog.hide();
        FtiiAutoNull[0].set( NULL_DAC_OFFSET_VN, (unsigned) 0 );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case AUTO_NULL_PB:
                send_auto_null_command();
                return TRUE;

            case MANUAL_NULL_PB:
                send_manual_null_command();
                return TRUE;
            }
        break;

    case WM_NEW_DATA:
        do_new_set_null_values();
        return TRUE;

    case WM_NEW_RT_DATA:
        do_new_set_null_ascii_data( lParam );
        return TRUE;
    }

return FALSE;

}

/***********************************************************************
*               POSITON_CHOOSE_VALVE_TYPE_DIALOG_PROC                  *
***********************************************************************/
static void position_choose_valve_type_dialog( HWND sorc )
{
WINDOW_CLASS parent;
WINDOW_CLASS w;
RECTANGLE_CLASS rp;
RECTANGLE_CLASS r;

w = sorc;
parent = GetParent( sorc );
parent.getrect( rp );
w.getrect( r );
rp.left = rp.right - r.width();
rp.top  = rp.bottom - r.height();
w.move( rp );
}

/***********************************************************************
*                       SHOW_REVISION_D_CONTROLS                       *
***********************************************************************/
static void show_revision_d_controls()
{
static UINT id[] = {
    LVDT_BOARD_LEVEL_OFFSET_EBOX,
    LVDT_DIGITAL_POT_EBOX,
    LVDT_DIGITAL_POT_SPIN,
    AMP_1_RADIO,
    AMP_2_RADIO,
    OFFSET_VOLTAGE_STATIC,
    OFFSET_VOLT_RANGE_STATIC,
    SPAN_STATIC,
    SPAN_RANGE_STATIC
    };
const int nof_ids = sizeof(id)/sizeof(int);

WINDOW_CLASS w;
int          i;
bool         is_visible;

if ( HasDigitalServoAmp )
    is_visible = true;
else
    is_visible = false;

for ( i=0; i<nof_ids; i++ )
    {
    w = SetDacDialog.control( id[i] );
    w.show( is_visible );
    }

w = SetDacDialog.control( AMP_2_RADIO );
w.enable( HasPressureControl );
}

/***********************************************************************
*                        REFRESH_SET_DAC_DIALOG                        *
***********************************************************************/
static void refresh_set_dac_dialog()
{
WINDOW_CLASS w;
int          i;
unsigned     u;
BOOLEAN      is_amp_1;
double       d;
STRING_CLASS s;

show_revision_d_controls();

if ( HasDigitalServoAmp )
    {
    is_amp_1 = is_checked( SetDacDialog.handle(), AMP_1_RADIO );

    if ( is_amp_1 )
        u = digital_pot_setting( AMP_1_LVDT_GAIN );
    else
        u = digital_pot_setting( AMP_2_LVDT_GAIN );

    w = SetDacDialog.control( LVDT_DIGITAL_POT_SPIN );
    w.send( UDM_SETPOS,   0, MAKELPARAM((WORD) u, (WORD) 0) );

    if ( is_amp_1 )
        i = LVDT_1_BOARD_LEVEL_OFFSET_INDEX;
    else
        i = LVDT_2_BOARD_LEVEL_OFFSET_INDEX;

    d = CurrentMachineVars[i].realval();
    w = SetDacDialog.control( LVDT_BOARD_LEVEL_OFFSET_EBOX );
    w.set_title( ascii_double(d) );
    }
}

/***********************************************************************
*                         INIT_SET_DAC_DIALOG                          *
***********************************************************************/
static void init_set_dac_dialog()
{
static UDACCEL accel = { 1, 1 };
WINDOW_CLASS  w;

w = SetDacDialog.control( DAC_COMMAND_THERM );
w.send( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 20000) );

w = SetDacDialog.control( COMMAND_VOLTS_THERM );
w.send( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

w = SetDacDialog.control( LVDT_FEEDBACK_THERM );
w.send( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

w = SetDacDialog.control( LVDT_RAW_FEEDBACK_THERM );
w.send( PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

CheckRadioButton( SetDacDialog.handle(), AMP_1_RADIO, AMP_2_RADIO, AMP_1_RADIO );

w = SetDacDialog.control( LVDT_DIGITAL_POT_SPIN );
w.send( UDM_SETRANGE, 0, MAKELPARAM((WORD) 127, (WORD) 0) );
w.send( UDM_SETACCEL, 0, (LPARAM) &accel );
w.send( UDM_SETPOS,   0, MAKELPARAM((WORD) 0, (WORD) 0) );

show_revision_d_controls();
}

/***********************************************************************
*                        DO_NEW_SET_DAC_VALUES                         *
***********************************************************************/
static void do_new_set_dac_values()
{
static STRING_CLASS ns;
static STRING_CLASS os;
static WINDOW_CLASS wc;
static HWND         w;
static int          i;
static double       d;
static unsigned     x;
static bool         should_be_visible;
static bool         firstime = true;

BOOLEAN is_amp_1;
int     mydac;

is_amp_1 = is_checked( SetDacDialog.control(AMP_1_RADIO) );
mydac = 0;
if ( !is_amp_1 )
    mydac = 2;

w  = SetDacDialog.control( DAC_COMMAND_TBOX );
i  = CurrentOpStatus.dac[mydac];
d = (double) i;
d /= 1000.0;
ns = fixed_string( d, 3, 2 );
os.get_text( w );
if ( firstime || os != ns )
    {
    ns.set_text( w );
    w = SetDacDialog.control( DAC_COMMAND_THERM );
    i += 10000;
    PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
    }

i = current_measured_command_index( mydac );
if ( firstime || AnalogValue[i] != CurrentAnalogValue[i] )
    {
    AnalogValue[i] = CurrentAnalogValue[i];
    x = AnalogValue[i];
    ns = ascii_dac_volts( x );
    w  = SetDacDialog.control( COMMAND_VOLTS_TBOX );
    ns.set_text( w );

    w = SetDacDialog.control( COMMAND_VOLTS_THERM );
    PostMessage( w, PBM_SETPOS, (WPARAM) x, 0L );
    }

i = current_lvdt_feedback_index( mydac );
if ( firstime || AnalogValue[i] != CurrentAnalogValue[i] )
    {
    AnalogValue[i] = CurrentAnalogValue[i];

    x = AnalogValue[i];
    ns = ascii_dac_volts( x );
    w  = SetDacDialog.control( LVDT_FEEDBACK_TBOX );
    ns.set_text( w );

    w = SetDacDialog.control( LVDT_FEEDBACK_THERM );
    PostMessage( w, PBM_SETPOS, (WPARAM) x, 0L );
    }

if ( HasDigitalServoAmp )
    {
    i = current_raw_lvdt_feedback_index( mydac );
    if ( firstime || AnalogValue[i] != CurrentAnalogValue[i] )
        {
        AnalogValue[i] = CurrentAnalogValue[i];

        x = AnalogValue[i];
        ns = ascii_dac_volts( x );
        w  = SetDacDialog.control( LVDT_RAW_FEEDBACK_TBOX );
        ns.set_text( w );

        w = SetDacDialog.control( LVDT_RAW_FEEDBACK_THERM );
        PostMessage( w, PBM_SETPOS, (WPARAM) x, 0L );
        }
    }

firstime = false;
}

/***********************************************************************
*                     SEND_LVDT_DIGITAL_POT_COMMAND                    *
***********************************************************************/
static void send_lvdt_digital_pot_command( BOOLEAN have_enter_key )
{
static TCHAR amp_1_cmd[] = TEXT("ODP1=");
static TCHAR amp_2_cmd[] = TEXT("ODP3=");
STRING_CLASS s;
STRING_CLASS cmd;
int          i;
unsigned     u;

s.get_text( SetDacDialog.control(LVDT_DIGITAL_POT_EBOX) );
if ( is_checked(SetDacDialog.control(AMP_1_RADIO)) )
    {
    cmd = amp_1_cmd;
    i = AMP_1_LVDT_GAIN;
    }
else
    {
    cmd = amp_2_cmd;
    i = AMP_2_LVDT_GAIN;
    }
cmd += s;

if ( have_enter_key )
    temp_message( cmd.text(), 1000 );
send_command_to_current_machine( cmd.text(), true );

u = CurrentMachineVars[DIGITAL_POTS_INDEX].uval();
put_byte( u, s.uint_value(), i );
CurrentMachineVars[DIGITAL_POTS_INDEX] = u;
save_machine_vars();
}

/***********************************************************************
*                         SEND_SET_DAC_COMMAND                         *
***********************************************************************/
void send_set_dac_command( int id )
{
static TCHAR amp_1_cmd[] = TEXT( "V361=" );
static TCHAR amp_2_cmd[] = TEXT( "V362=" );

STRING_CLASS s;
double   d;
double   copy;
int      i;
int      vi;
BOOLEAN  is_amp_1;
HWND     w;
HWND     control_with_focus;

is_amp_1 = is_checked( SetDacDialog.control(AMP_1_RADIO) );

control_with_focus = GetFocus();
if ( id == SEND_TO_BOARD_PB )
    {
    if ( control_with_focus == SetDacDialog.control(LVDT_DIGITAL_POT_EBOX) )
        {
        send_lvdt_digital_pot_command( TRUE );
        return;
        }
    else
        {
        w = SetDacDialog.control( LVDT_BOARD_LEVEL_OFFSET_EBOX );
        if ( w == control_with_focus )
            {
            s.get_text( w );
            d = s.real_value();
            copy = d;
            d *= 1000.0;
            d = round( d, 1.0 );
            i = int( d );
            if ( is_amp_1 )
                {
                s = amp_1_cmd;
                vi = LVDT_1_BOARD_LEVEL_OFFSET_INDEX;
                }
            else
                {
                s = amp_2_cmd;
                vi = LVDT_2_BOARD_LEVEL_OFFSET_INDEX;
                }
            s += i;
            temp_message( s.text(), 1000 );
            send_command_to_current_machine( s.text(), true );

            CurrentMachineVars[vi] = copy;
            save_machine_vars();
            return;
            }
        }
    }

s.get_text( SetDacDialog.control(DAC_VOLTS_EBOX) );
if ( s.isempty() )
    return;

d = s.real_value();
d *= 1000.0;
d = round( d, 1.0 );
i = int( d );

if ( is_amp_1 )
    s = TEXT( "OD1=V" );
else
    s = TEXT( "OD3=V" );
s += i;

temp_message( s.text(), 1000 );
send_command_to_current_machine( s.text(), true );
}

/***********************************************************************
*                          SEND_TEST_MESSAGE                           *
***********************************************************************/
static void send_test_message( int id )
{
static int last_id = 0;

/*
---------------------------------------------------------
When you press a down arrow I get two identical messages.
Ignore the second one
--------------------------------------------------------- */
if ( id == last_id )
    {
    last_id = 0;
    return;
    }

last_id = id;

switch( id )
    {
    case TEST_7_RADIO:
        send_command_to_current_machine( TEXT("OD1=V0"),  false );
        send_command_to_current_machine( TEXT("OD3=V0"),  false );
        send_command_to_current_machine( TEXT("OD2=V0"),  false );
        send_command_to_current_machine( TEXT("OD4=V0"),  false );
        send_command_to_current_machine( TEXT("ODP1=0"),  false );
        send_command_to_current_machine( TEXT("ODP3=0"),  false );
        send_command_to_current_machine( TEXT("ODP2=0"),  false );
        send_command_to_current_machine( TEXT("ODP4=0"),  false );
        send_command_to_current_machine( TEXT("OC12=0"),  true );
        break;

    case TEST_8A_RADIO:
        send_command_to_current_machine( TEXT("OD1=V-5000"),  false );
        send_command_to_current_machine( TEXT("OD3=V-5000"),  true );
        break;

    case TEST_8B_RADIO:
        send_command_to_current_machine( TEXT("OD1=V5000"),  false );
        send_command_to_current_machine( TEXT("OD3=V5000"),  true );
        break;

    case TEST_9A_RADIO:
        send_command_to_current_machine( TEXT("OC10=1"),  false );
        send_command_to_current_machine( TEXT("OC11=1"),  true );
        break;

    case TEST_9B_RADIO:
        send_command_to_current_machine( TEXT("OC10=0"),  false );
        send_command_to_current_machine( TEXT("OC11=0"),  true );
        break;

    case TEST_10A_RADIO:
        send_command_to_current_machine( TEXT("OC12=1"),  true );
        break;

    case TEST_10B_RADIO:
        send_command_to_current_machine( TEXT("OC12=0"),  true );
        break;

    case TEST_11A_RADIO:
        send_command_to_current_machine( TEXT("ODP2=127"),  false );
        send_command_to_current_machine( TEXT("ODP4=127"),  true );
        break;

    case TEST_11B_RADIO:
        send_command_to_current_machine( TEXT("ODP2=0"),  false );
        send_command_to_current_machine( TEXT("ODP4=0"),  true );
        break;

    case TEST_11C_RADIO:
        send_command_to_current_machine( TEXT("OD1=V0"),  false );
        send_command_to_current_machine( TEXT("OD3=V0"),  true );
        break;

    case TEST_18A_RADIO:
        send_command_to_current_machine( TEXT("ODP1=14"),  false );
        send_command_to_current_machine( TEXT("ODP3=14"),  true );
        break;

    case TEST_18B_RADIO:
        send_command_to_current_machine( TEXT("ODP1=41"),  false );
        send_command_to_current_machine( TEXT("ODP3=41"),  true );
        break;

    case TEST_18C_RADIO:
        send_command_to_current_machine( TEXT("ODP1=96"),  false );
        send_command_to_current_machine( TEXT("ODP3=96"),  true );
        break;

    case TEST_19_RADIO:
        send_command_to_current_machine( TEXT("OD2=V5000"),  false );
        send_command_to_current_machine( TEXT("OD4=V5000"),  true );
        break;
    }
}

/***********************************************************************
*                        FT2_TEST_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK ft2_test_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                Ft2TestDialog.hide();
                return TRUE;

            case TEST_7_RADIO:
            case TEST_8A_RADIO:
            case TEST_8B_RADIO:
            case TEST_9A_RADIO:
            case TEST_9B_RADIO:
            case TEST_10A_RADIO:
            case TEST_10B_RADIO:
            case TEST_11A_RADIO:
            case TEST_11B_RADIO:
            case TEST_11C_RADIO:
            case TEST_18A_RADIO:
            case TEST_18B_RADIO:
            case TEST_18C_RADIO:
            case TEST_19_RADIO:
               if ( cmd == BN_CLICKED )
                    send_test_message( id );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        Ft2TestDialog.hide();
        return FALSE;
    }

return FALSE;
}

/***********************************************************************
*                      SET_DAC_SETUP_STRINGS                           *
***********************************************************************/
static void set_dac_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { OFFSET_VOLTAGE_STATIC,    TEXT("OFFSET_VOLTAGE") },
    { SEND_TO_BOARD_PB,         TEXT("SEND_TO_BOARD") },
    { SET_DAC_STATIC,           TEXT("SET_DAC") },
    { CMD_STATIC,               TEXT("COMMAND") },
    { VOLTS_STATIC,             TEXT("VOLTS") },
    { LVDT_FEEDBACK_STATIC,     TEXT("LVDT_FEEDBACK") },
    { PLUS_5_VOLTS_PB,          TEXT("PLUS_5_VOLTS") },
    { MINUS_5_VOLTS_PB,         TEXT("MINUS_5_VOLTS") },
    { MEAS_CMD_STATIC,          TEXT("MEAS_CMD") },
    { OFFSET_VOLT_RANGE_STATIC, TEXT("OFFSET_V_RANGE") },
    { AMP_1_RADIO,              TEXT("AMP_1") },
    { AMP_2_RADIO,              TEXT("AMP_2") },
    { SPAN_STATIC,              TEXT("SPAN") },
    { SPAN_RANGE_STATIC,        TEXT("SPAN_RANGE") },
    { RAW_LVDT_STATIC,          TEXT("RAW_LVDT") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    MINUS_10_STATIC,
    PLUS_10_STATIC
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, EmptyString );
}

/***********************************************************************
*                           SET_DAC_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK set_dac_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  id;
static int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SetDacDialog = hWnd;
        set_dac_setup_strings( hWnd );
        position_button_child( SetDacDialog );
        init_set_dac_dialog();
        SetDacDialog.hide();
        SetDacDialog.send( WM_DBINIT );
        break;

    case WM_DBINIT:
        if ( UpdatingSetDacScreen )
            {
            UpdatingSetDacScreen = FALSE;
            }
        else
            {
            UpdatingSetDacScreen = TRUE;
            refresh_set_dac_dialog();
            SetDacDialog.send( WM_DBINIT );
            }
        return TRUE;

    case WM_NEW_DATA:
        do_new_set_dac_values();
        return TRUE;

    case WM_VSCROLL:
        if ( HWND(lParam) == SetDacDialog.control(LVDT_DIGITAL_POT_SPIN) && !UpdatingSetDacScreen )
            {
            if ( id == SB_ENDSCROLL )
                {
                /*
                --------------------------------------------------------------------------------------------
                The SB_ENDSCROLL message is only sent when the mouse button is released. Sending the command
                at this time prevents me from banging away at the st2 board.
                -------------------------------------------------------------------------------------------- */
                send_lvdt_digital_pot_command( FALSE );
                return TRUE;
                }
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            /*
            --------------------------------------------------
            case CHANGE_VALVE_TYPE_PB:
                DialogBox(
                    MainInstance,
                    TEXT( "CHOOSE_VALVE_TYPE_DIALOG" ),
                    hWnd,
                    (DLGPROC) choose_valve_type_dialog_proc );
                return TRUE;
            -------------------------------------------------- */

            case AMP_1_RADIO:
            case AMP_2_RADIO:
                if ( !UpdatingSetDacScreen && HasDigitalServoAmp )
                    {
                    if ( id == AMP_1_RADIO )
                        {
                        send_command_to_current_machine( CloseValve_2_Loop,  true );
                        send_command_to_current_machine( OpenValve_1_Loop,  true );
                        }
                    else
                        {
                        send_command_to_current_machine( CloseValve_1_Loop,  true );
                        send_command_to_current_machine( OpenValve_2_Loop,  true );
                        }
                    }
                refresh_set_dac_dialog();
                return TRUE;

            case SEND_TO_BOARD_PB:
                send_set_dac_command( id );
                return TRUE;

            case PLUS_5_VOLTS_PB:
                set_text( hWnd, DAC_VOLTS_EBOX, TEXT("5.0") );
                send_set_dac_command( id );
                return TRUE;

            case MINUS_5_VOLTS_PB:
                set_text( hWnd, DAC_VOLTS_EBOX, TEXT("-5.0") );
                send_set_dac_command( id );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        START_IF_OK_TO_DEACTIVATE                     *
***********************************************************************/
void start_if_ok_to_deactivate( WINDOW_CLASS & w )
{
BOOL ok_to_deactivate;

ok_to_deactivate = DialogBox( MainInstance,
                              TEXT("DEACTIVATE_CONTROL_DIALOG"),
                              MainWindow.handle(),
                              (DLGPROC) deactivate_control_dialog_proc );

if ( ok_to_deactivate )
    set_current_window( w );
}

/***********************************************************************
*                           MV_FROM_PERCENT                            *
*    Converts to signed mv from percent of range [-10v, 10v]           *
***********************************************************************/
static int mv_from_percent( double sorc )
{
sorc *= 100.0;
return (int) round( sorc, 1.0 );
}

/***********************************************************************
*                           MV_FROM_PERCENT                            *
*    Converts to signed mv from percent of range [-10v, 10v]           *
***********************************************************************/
static int mv_from_percent( STRING_CLASS & sorc )
{
double   x;
int      i;

if ( sorc.contains(PeriodChar) )
    {
    x = sorc.real_value();
    x *= 100.0;
    i = (int) round( x, 1.0 );
    }
else
    {
    i = sorc.int_value();
    i *= 100;
    }

return i;
}

/***********************************************************************
*                 REPLACE_VOLTS_FROM_PERCENT_VALUE                     *
*           Volts are saved as volts*1000 which == %*100               *
***********************************************************************/
STRING_CLASS & replace_volts_from_percent_value( TCHAR * sorc, float new_value )
{
double   x;
int      i;

x = (double) new_value;
x *= 100.0;
i = (int) round( x, 1.0 );

return replace_value( sorc, i );
}

/***********************************************************************
*                       SAVE_GLOBAL_PARAMETERS                         *
***********************************************************************/
static void save_global_parameters()
{
AUTO_BACKUP_CLASS ab;
HWND         w;
TCHAR      * cp;
NAME_CLASS   s;
INI_CLASS    ini;

w = GlobalParameterDialog.handle();

s.get_ft2_editor_settings_file_name( ComputerName, CurrentMachine.name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

s.get_text( w, MIN_LI_EDITBOX );
ini.put_string( MinLowImpactPercentKey, s.text() );

s.get_text( w, MAX_VELOCITY_EDITBOX );
ini.put_string( MaxVelocityKey, s.text() );

s.get_text( w, NULL_DAC_EDITBOX );
GlobalParameters[NULL_DAC_INDEX] = s.int_value();

s.get_text( w, LVDT_DAC_EDITBOX );
GlobalParameters[LVDT_DAC_INDEX] = s.int_value();

s.get_text( w, JOG_SHOT_VALVE_EDITBOX );
GlobalParameters[JOG_SHOT_VALVE_INDEX] = mv_from_percent( s );

s.get_text( w, FT_VALVE_EDITBOX );
GlobalParameters[FT_VALVE_INDEX] = mv_from_percent( s );

s.get_text( w, RETRACT_VALVE_EDITBOX );
GlobalParameters[RETRACT_VALVE_INDEX] = mv_from_percent( s );

s.get_text( w, ZERO_SPEED_CHECK_EDITBOX );
GlobalParameters[ZERO_SPEED_CHECK_INDEX] = CurrentPart.x4_from_dist( s.real_value() );

s.get_text( w, PC_DITHER_FREQ_EB  );
GlobalParameters[PC_DITHER_FREQ_INDEX] = s.int_value();

s.get_text( w, PC_DITHER_AMPL_EB  );
GlobalParameters[PC_DITHER_AMPL_INDEX] = s.int_value();

s.get_text( w, VC_DITHER_FREQ_EB  );
GlobalParameters[VC_DITHER_FREQ_INDEX] = s.int_value();

s.get_text( w, VC_DITHER_AMPL_EB  );
GlobalParameters[VC_DITHER_AMPL_INDEX] = s.int_value();

s.get_text( w, NULL_DAC_2_EB );
GlobalParameters[NULL_DAC_2_INDEX] = s.int_value();

s.get_text( w, LVDT_DAC_2_EB );
GlobalParameters[LVDT_DAC_2_INDEX] = s.int_value();

s.get_ft2_global_settings_file_name( ComputerName, CurrentMachine.name );
GlobalParameters.put( s );

if ( !IsRemoteMonitor )
    {
    ab.get_backup_dir();
    ab.backup_machine( CurrentPart.machine );
    }
/*
--------------------------------------
Make text that main window will delete
-------------------------------------- */
cp = maketext( CurrentMachine.name.text() );
MainWindow.send( WM_NEWSETUP, 0, (LPARAM) cp );
}

/***********************************************************************
*                           PERCENT_FROM_MV                            *
*         Converts signed mv to percent of range [-10, 10];            *
***********************************************************************/
static TCHAR * percent_from_mv( int sorc )
{
double d;
d = double( sorc );
d /= 100.0;
return ascii_double( d );
}

/**********************************************************************
*                       CHECK_FOR_SAVEPART_UPDATES                    *
* Zitai can chage the FT_VALVE_SETTING AND JOG_SHOT_VALVE_SETTINGS    *
* If they do this I need to copy the changes to my globals and to     *
* the corresponding editboxes.                                        *
**********************************************************************/
static void check_for_savepart_updates()
{
NAME_CLASS s;
FTII_VAR_ARRAY v;
HWND           w;

s.get_exe_dir_file_name( SavepartExeName );
if ( !s.file_exists() )
    return;

w = GlobalParameterDialog.handle();

s.get_ft2_global_settings_file_name( ComputerName, CurrentMachine.name );
v.get( s );
if ( v.find(FT_SETTING_VN) )
    {
    if ( GlobalParameters[FT_VALVE_INDEX].ival() != v.ival() )
        {
        GlobalParameters[FT_VALVE_INDEX] = v.ival();
        set_text( w, FT_VALVE_EDITBOX, percent_from_mv(GlobalParameters[FT_VALVE_INDEX].ival()) );
        }
    }
if ( v.find(JOG_SHOT_SETTING_VN) )
    {
    if ( GlobalParameters[JOG_SHOT_VALVE_INDEX].ival() != v.ival() )
        {
        GlobalParameters[JOG_SHOT_VALVE_INDEX] = v.ival();
        set_text( w, JOG_SHOT_VALVE_EDITBOX, percent_from_mv(GlobalParameters[JOG_SHOT_VALVE_INDEX].ival()) );
        }
    }
}

/***********************************************************************
*                       FILL_GLOBAL_PARAMETERS                         *
***********************************************************************/
static void fill_global_parameters()
{
static int vars[] = { NULL_DAC_OFFSET_VN, LVDT_DAC_OFFSET_VN, JOG_SHOT_SETTING_VN,
                      FT_SETTING_VN,      RETRACT_SETTING_VN, ZERO_SPEED_CHECK_VN,
                      VC_DITHER_FREQ_VN, VC_DITHER_AMPL_VN,
                      PC_DITHER_FREQ_VN, PC_DITHER_AMPL_VN,
                      NULL_DAC_2_OFFSET_VN, LVDT_DAC_2_OFFSET_VN
                      };
const int nof_vars = sizeof(vars)/sizeof(int);

SURETRAK_PARAM_CLASS SureTrakParameters;
INI_CLASS  ini;
HWND       w;
NAME_CLASS s;
int        i;

w = GlobalParameterDialog.handle();

if ( !w )
    return;

s.get_ft2_editor_settings_file_name( ComputerName, CurrentMachine.name );
if ( s.file_exists() )
    {
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );

    s = ini.get_string( MinLowImpactPercentKey );
    if ( s == UNKNOWN )
        s = EmptyString;
    s.set_text( w, MIN_LI_EDITBOX );

    set_text( w, MAX_VELOCITY_EDITBOX,    ini.get_string( MaxVelocityKey)         );
    }
else
    {
    SureTrakParameters.find( ComputerName, CurrentMachine.name );
    set_text( w, MIN_LI_EDITBOX,          ascii_float(SureTrakParameters.min_low_impact_percent()) );
    set_text( w, MAX_VELOCITY_EDITBOX, ascii_float(SureTrakParameters.max_velocity()) );
    }

GlobalParameters.upsize( nof_vars );
for ( i=0; i<nof_vars; i++ )
    GlobalParameters[i].set( vars[i], (unsigned) 0 );

s.get_ft2_global_settings_file_name( ComputerName, CurrentMachine.name );
if ( s.file_exists() )
    {
    GlobalParameters.get_from( s );
    }
else
    {
    SureTrakParameters.find( ComputerName, CurrentMachine.name );
    GlobalParameters[NULL_DAC_INDEX]         = SureTrakParameters.null_dac_offset();
    GlobalParameters[LVDT_DAC_INDEX]         = SureTrakParameters.lvdt_dac_offset();
    GlobalParameters[JOG_SHOT_VALVE_INDEX]   = mv_from_percent( (double) SureTrakParameters.jog_valve_percent() );
    GlobalParameters[FT_VALVE_INDEX]         = mv_from_percent( (double) SureTrakParameters.ft_valve_percent() );
    GlobalParameters[RETRACT_VALVE_INDEX]    = mv_from_percent( (double) SureTrakParameters.retract_valve_percent() );
    GlobalParameters[ZERO_SPEED_CHECK_INDEX] = CurrentPart.x4_from_dist( (double) SureTrakParameters.zero_speed_check_pos() );
    }

set_text( w, NULL_DAC_EDITBOX,         int32toasc((int32) GlobalParameters[NULL_DAC_INDEX].ival()) );
set_text( w, LVDT_DAC_EDITBOX,         int32toasc((int32) GlobalParameters[LVDT_DAC_INDEX].ival()) );
set_text( w, JOG_SHOT_VALVE_EDITBOX,   percent_from_mv(GlobalParameters[JOG_SHOT_VALVE_INDEX].ival()) );
set_text( w, FT_VALVE_EDITBOX,         percent_from_mv(GlobalParameters[FT_VALVE_INDEX].ival())       );
set_text( w, RETRACT_VALVE_EDITBOX,    percent_from_mv(GlobalParameters[RETRACT_VALVE_INDEX].ival())  );
set_text( w, ZERO_SPEED_CHECK_EDITBOX, ascii_double( CurrentPart.dist_from_x4(GlobalParameters[ZERO_SPEED_CHECK_INDEX].realval())) );
set_text( w, PC_DITHER_FREQ_EB,        int32toasc((int32) GlobalParameters[PC_DITHER_FREQ_INDEX].ival()) );
set_text( w, PC_DITHER_AMPL_EB,        int32toasc((int32) GlobalParameters[PC_DITHER_AMPL_INDEX].ival()) );
set_text( w, VC_DITHER_FREQ_EB,        int32toasc((int32) GlobalParameters[VC_DITHER_FREQ_INDEX].ival()) );
set_text( w, VC_DITHER_AMPL_EB,        int32toasc((int32) GlobalParameters[VC_DITHER_AMPL_INDEX].ival()) );
set_text( w, NULL_DAC_2_EB,            int32toasc((int32) GlobalParameters[NULL_DAC_2_INDEX].ival()) );
set_text( w, LVDT_DAC_2_EB,            int32toasc((int32) GlobalParameters[LVDT_DAC_2_INDEX].ival()) );
set_text( w, GLOBAL_DIST_UNITS_XBOX,   units_name(CurrentPart.distance_units) );
}

/***********************************************************************
*                  SET_GLOBAL_PARAMETER_SETUP_STRINGS                    *
***********************************************************************/
static void set_global_parameter_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rsn[] = {
    { NULL_DAC_LABEL,         TEXT("NULL_DAC_COMP") },
    { LVDT_DAC_LABEL,         TEXT("LVDT_DAC_COMP") },
    { JOG_SHOT_VALVE_LABEL,   TEXT("JOG_VALVE_SET") },
    { RETRACT_VALVE_LABEL,    TEXT("RETRACT_VALVE_SET") },
    { VC_DITHER_FREQ_LABEL,   TEXT("DITHER_FREQ") },
    { VC_DITHER_AMPL_LABEL,   TEXT("DITHER_AMPL") },
    { FT_VALVE_LABEL,         TEXT("FT_VALVE_SET") },
    { ZERO_SPEED_CHECK_LABEL, TEXT("ZERO_SPEED_POS") },
    { MIN_LI_LABEL,           TEXT("MIN_LI") },
    { MAX_VELOCITY_LABEL,     TEXT("MAX_VEL") },
    { PC_AXIS2_LABEL,         TEXT("PC_AXIS2") },
    { PC_DITHER_FREQ_LABEL,   TEXT("DITHER_FREQ") },
    { PC_DITHER_AMPL_LABEL,   TEXT("DITHER_AMPL") },
    { NULL_DAC_2_LABEL,       TEXT("NULL_DAC_COMP") },
    { LVDT_DAC_2_LABEL,       TEXT("LVDT_DAC_COMP") },
    { SAVE_CHANGES_BUTTON,    TEXT("SAVE_CHANGES") },
    { CANCEL_CHANGES_BUTTON,  TEXT("REVERT_TO_SAVED") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, NULL );
}

/***********************************************************************
*                    GLOBAL_PARAMETER_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK global_parameter_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        GlobalParameterDialog = w;
        set_global_parameter_setup_strings( w );
        position_button_child( GlobalParameterDialog );
        GlobalParameterDialog.hide();
        return TRUE;

    case WM_NEW_DATA:
        check_for_savepart_updates();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_CHANGES_BUTTON:
                temp_message( SavingString.text() );
                save_global_parameters();
                return TRUE;

            case CANCEL_CHANGES_BUTTON:
                fill_global_parameters();
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                   UPDATE_CURRENT_VARIABLES_LISTBOX                   *
***********************************************************************/
static void update_current_variables_listbox()
{
STRING_CLASS s;
int i;
BOOLEAN isempty = TRUE;

CVLb.redraw_off();
CVLb.empty();
for ( i=0; i<NOF_FT2_VARIABLES; i++ )
    {
    if ( ShowDifOnly )
        {
        if ( BoardVariable[i][LIST_1_INDEX].isempty() && BoardVariable[i][LIST_2_INDEX].isempty() )
            continue;
        if ( BoardVariable[i][LIST_1_INDEX] == BoardVariable[i][LIST_2_INDEX] )
            continue;
        }
    s = i;
    s += TabChar;
    s += BoardVariable[i][LIST_1_INDEX];
    s += TabChar;
    s += BoardVariable[i][LIST_2_INDEX];
    s += TabChar;
    s += BoardVariableDescription[i];
    CVLb.add( s.text() );
    isempty = FALSE;
    }
CVLb.redraw_on();

if ( isempty )
    CVLb.add( TEXT("No Differences Found") );
}

/***********************************************************************
*                 START_ACQUIRING_ALL_BOARD_VARIABLES                  *
***********************************************************************/
static BOOLEAN start_acquiring_all_board_variables( int list_index )
{
int i;

if ( SendingBoardVariableRequests )
    return FALSE;

if ( list_index < 0 || list_index > 2 )
    return FALSE;

for ( i=0; i<NOF_FT2_VARIABLES; i++ )
    BoardVariable[i][list_index].null();

CurrentListIndex = list_index;
CurrentBoardVariable = 0;
SendingBoardVariableRequests = TRUE;
send_command_to_current_machine( TEXT("V0"), true );
CVGettingTb.show();
CVCurrentVariableTb.show();
CVCurrentVariableTb.set_title( TEXT("0") );
CVofTb.show();

return TRUE;
}

/***********************************************************************
*                      ADD_BOARD_VARIABLE_TO_LIST                      *
***********************************************************************/
static void add_board_variable_to_list( TCHAR * sorc )
{
TCHAR * cp;
STRING_CLASS s;

cp = sorc;
if ( *cp != VChar )
    return;
cp++;
s = cp;
if ( s.next_field(UnderScoreChar) )
    {
    if ( s.int_value() != CurrentBoardVariable )
        return;
    if ( s.next_field() )
        {
        BoardVariable[CurrentBoardVariable][CurrentListIndex] = s;
        CurrentBoardVariable++;
        s = CurrentBoardVariable;
        s.set_text( CVCurrentVariableTb.handle() );
        if ( CurrentBoardVariable == NOF_FT2_VARIABLES )
            {
            SendingBoardVariableRequests = FALSE;
            CurrentBoardVariable = 0;
            update_current_variables_listbox();
            CVGettingTb.hide();
            CVCurrentVariableTb.hide();
            CVofTb.hide();
            }
        else
            {
            s = VChar;
            s += CurrentBoardVariable;
            send_command_to_current_machine( s.text(), true );
            }
        }
    }
}

/***********************************************************************
*                   GET_BOARD_VARIABLE_DESCRIPTIONS                    *
***********************************************************************/
static void get_board_variable_descriptions()
{
TCHAR    * cp;
TCHAR    * ep;
FILE_CLASS f;
int        i;
NAME_CLASS s;

s.get_exe_dir_file_name( VARIABLE_LIST_CSV_FILE );
if ( f.open_for_read(s) )
    {
    while ( true )
        {
        cp = f.readline();
        if ( !cp )
            break;
        if ( *cp == VChar )
            {
            cp++;
            ep = findchar( CommaChar, cp );
            if ( ep )
                {
                *ep = NullChar;
                s = cp;
                i = s.int_value();
                if ( i < NOF_FT2_VARIABLES )
                    {
                    cp = ep;
                    cp++;
                    BoardVariableDescription[i] = cp;
                    }
                }
            }
        }
    f.close();
    }
}

/***********************************************************************
*                     RESTORE_SAVED_BOARD_VARIABLES                    *
***********************************************************************/
static BOOLEAN restore_saved_board_variables()
{
TCHAR    * cp;
FILE_CLASS f;
int        i;
NAME_CLASS s;

s.get_exe_dir_file_name( CVListFileName );
if ( !s.file_exists() )
    return FALSE;
if ( f.open_for_read(s) )
    {
    for ( i=0; i<NOF_FT2_VARIABLES; i++ )
        {
        cp = f.readline();
        if ( !cp )
            {
            while ( i<NOF_FT2_VARIABLES )
                {
                BoardVariable[i][LIST_1_INDEX] = ZeroChar;
                BoardVariable[i][LIST_2_INDEX] = ZeroChar;
                i++;
                }
            break;
            }
        s = cp;
        if ( s.next_field(CommaChar) )
            {
            if ( s.next_field() )
                {
                BoardVariable[i][LIST_1_INDEX] = s;
                if ( s.next_field() )
                    BoardVariable[i][LIST_2_INDEX] = s;
                }
            }
        }
    f.close();
    }
return TRUE;
}

/***********************************************************************
*                         SAVE_BOARD_VARIABLES                         *
***********************************************************************/
static void save_board_variables()
{
FILE_CLASS f;
int        i;
NAME_CLASS s;

s.get_exe_dir_file_name( CVListFileName );
if ( f.open_for_write(s) )
    {
    for ( i=0; i<NOF_FT2_VARIABLES; i++ )
        {
        s = VChar;
        s += i;
        s += CommaChar;
        s += BoardVariable[i][LIST_1_INDEX];
        s += CommaChar;
        s += BoardVariable[i][LIST_2_INDEX];
        f.writeline(s);
        }
    f.close();
    }
}

/***********************************************************************
*                 SET_COMPARE_VARIABLES_SETUP_STRINGS                  *
***********************************************************************/
static void set_compare_variables_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { CV_NUMBER_TB,            TEXT("NUM_STRING") },
    { CV_LIST_1_TB,            TEXT("LIST1") },
    { CV_LIST_2_TB,            TEXT("LIST2") },
    { CV_DESCRIPTION_TB,       TEXT("DESCRIPTION") },
    { CV_GETTING_TB,           TEXT("GETTING_VAR") },
    { CV_GET_LIST_1_PB,        TEXT("GET_LIST1") },
    { CV_GET_LIST_2_PB,        TEXT("GET_LIST2") },
    { CV_SHOW_DIF_XB,          TEXT("SHOW_DIF") },
    { CV_SAVE_VARIABLES_PB,    TEXT("SAVE_VARS") },
    { CV_RESTORE_VARIABLES_PB, TEXT("RESTORE_VARS") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    CV_LB,
    CV_CURRENT_VARIABLE_TB,
    CV_OF_TB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                    COMPARE_VARIABLES_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK compare_variables_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int title_id[] = { CV_LIST_1_TB, CV_LIST_2_TB, CV_DESCRIPTION_TB };
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CompareVariablesDialog = w;
        set_compare_variables_setup_strings( w );
        CVLb.init( CompareVariablesDialog.control(CV_LB) );
        CVGettingTb         = CompareVariablesDialog.control( CV_GETTING_TB );
        CVCurrentVariableTb = CompareVariablesDialog.control( CV_CURRENT_VARIABLE_TB );
        CVofTb              = CompareVariablesDialog.control( CV_OF_TB );
        CVGettingTb.hide();
        CVCurrentVariableTb.hide();
        CVofTb.hide();
        position_button_child( CompareVariablesDialog );
        set_listbox_tabs_from_title_positions( w, CV_LB, title_id, 3 );
        CompareVariablesDialog.hide();
        get_board_variable_descriptions();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CV_GET_LIST_1_PB:
                start_acquiring_all_board_variables( LIST_1_INDEX );
                return TRUE;

            case CV_GET_LIST_2_PB:
                start_acquiring_all_board_variables( LIST_2_INDEX );
                return TRUE;

            case CV_SHOW_DIF_XB:
                ShowDifOnly = is_checked( w, CV_SHOW_DIF_XB  );
                update_current_variables_listbox();
                return TRUE;

            case CV_SAVE_VARIABLES_PB:
                temp_message( SavingString.text() );
                save_board_variables();
                return TRUE;

            case CV_RESTORE_VARIABLES_PB:
                if ( restore_saved_board_variables() )
                    update_current_variables_listbox();
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                       INIT_CONTROL_MODS_DIALOG                       *
***********************************************************************/
static void init_control_mods_dialog()
{
UsingAltStep = false;
AltStepNum   = TEXT( ".025 " );
AltStepEnd   = TEXT( "_E1.34" );
AltStepVelPrefix = TEXT( "VA1" );
AltStepAccPrefix = TEXT( "AC" );
AltStepPosPrefix = TEXT( "P" );
AltStepVel       = TEXT( "80" );
AltStepAcc       = TEXT( "0" );
AltStepPos       = TEXT( "60" );

StartStepNum         = TEXT( ".022 " );
StartStepVolts       = TEXT( "-1000" );
StartStepVoltsPrefix = TEXT( "UD1=" );

ParkStepNum          = TEXT( ".075 " );
ParkStep2Num         = TEXT( ".129 " );
ParkStepVolts        = TEXT( "-2000" );
ParkStepVoltsPrefix  = TEXT( "UD1=" );
}

/***********************************************************************
*                          FT2_VOLTS_TOASCII                           *
*           Convert -1000 to -1.000, 10000 to 10.000, etc.             *
***********************************************************************/
TCHAR * ft2_volts_toascii( TCHAR * ft2_volts )
{
static TCHAR buf[8];    /* -10.000 */
TCHAR * cp;
TCHAR * dest;
TCHAR * sorc;
int     count;

sorc = ft2_volts;
dest = buf;
if ( *sorc == MinusChar )
    *dest++ = *sorc++;

count = 0;
cp = sorc;
while ( *cp >= ZeroChar && *cp <= NineChar )
    {
    count++;
    cp++;
    }
if ( count > 5 )
    count = 5;

while ( count )
    {
    if ( count == 3 )
        *dest++= PeriodChar;
    *dest++ = *sorc++;
    count--;
    }

*dest = NullChar;
return buf;
}

/***********************************************************************
*                      ASCII_VOLTS_TO_FT2_VOLTS                        *
*           Convert -1.0 to -1000, 10.000 to 10000, etc.               *
* The user will enter the volts in decimal like -9.54. I need to       *
* convert this into -9540 for the board.                               *
***********************************************************************/
TCHAR * ascii_volts_to_ft2_volts( TCHAR * sorc )
{
const int buflen = 6;
static TCHAR buf[buflen+1];    /* -10000 */
TCHAR * dest;
int     chars_needed;
int     n;
bool    past_decimal_point;

past_decimal_point = false;
chars_needed = 3; /* Number of chars after the decimal point */
n = 0;

dest = buf;
while ( (n < buflen) && (chars_needed > 0) && (*sorc != NullChar) )
    {
    if ( *sorc == PeriodChar )
        {
        sorc++;
        past_decimal_point = true;
        }
    else
        {
        *dest++ = *sorc++;
        if ( past_decimal_point )
            chars_needed--;
        n++;
        }
    }

if ( n == 0 )
    {
    copystring( buf, ZeroString );
    }
else
    {
    while ( chars_needed > 0 )
        {
        *dest++ = ZeroChar;
        chars_needed--;
        }
    }

*dest = NullChar;

n = asctoint32( buf );
copystring( buf, int32toasc((int32) n), buflen );

return buf;
}

/***********************************************************************
*                        GET_ALT_STEP_FROM_LINE                        *
* #.025 VA180AC0P60_E1.34 #<--- Alternative short step and then acc.   *
***********************************************************************/
static void get_alt_step_from_line( TCHAR * sorc )
{
TCHAR buf[31];
TCHAR * cp;
TCHAR * ep;

copystring( buf, sorc, 30 );

cp = findstring( AltStepVelPrefix.text(), buf );
if ( !cp )
    return;

cp += AltStepVelPrefix.len();
ep = findstring( AltStepAccPrefix.text(), cp );
if ( !ep )
    return;
*ep = NullChar;
AltStepVel = cp;

cp = ep + AltStepAccPrefix.len();
ep = findstring( AltStepPosPrefix.text(), cp );
if ( !ep )
    return;
*ep = NullChar;
AltStepAcc = cp;

cp = ep + AltStepPosPrefix.len();
ep = findstring( AltStepEnd.text(), cp );
if ( !ep )
    return;
*ep = NullChar;
AltStepPos = cp;
}

/***********************************************************************
*                      GET_STRING_VALUE_FROM_LINE                      *
*                          .0XX UD1=-2000                              *
***********************************************************************/
static TCHAR * get_string_value_from_line( TCHAR * sorc )
{
static TCHAR zerostring[] = TEXT( "0" );
TCHAR * cp;

cp = findchar( EqualsChar, sorc );
if ( cp )
    {
    cp++;
    return cp;
    }

return zerostring;
}

/***********************************************************************
*                     LOAD_CONTROL_MODS_FROM_FILE                      *
* Get them from the c:\v5\data\machine\ftii_ctrl_prog_steps.txt        *
* if any are missing get them from control_all.txt                     *
***********************************************************************/
static bool load_control_mods_from_file()
{
NAME_CLASS   s;
FILE_CLASS   f;
TCHAR      * cp;
bool         no_comment;
bool         have_alt_step;
bool         have_park_step;
bool         have_start_step;
int          version;

have_alt_step   = false;
have_park_step  = false;
have_start_step = false;

s.get_ft2_ctrl_prog_steps_file_name( ComputerName, CurrentMachine.name );
if ( s.file_exists() )
    {

    if ( f.open_for_read(s) )
        {
        /*
        -----------------------
        Get the start valve step
        ----------------------- */
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;

            if ( !have_park_step )
                {
                if ( strings_are_equal(cp, ParkStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                    {
                    ParkStepVolts = get_string_value_from_line( cp );
                    have_park_step = true;
                    }
                }

            if ( !have_start_step )
                {
                if ( strings_are_equal(cp, StartStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                    {
                    StartStepVolts = get_string_value_from_line( cp );
                    have_start_step = true;
                    }
                }

            if ( have_park_step && have_start_step )
                break;
            }

        /*
        ----------------------
        Get the alternate step
        ---------------------- */
        UsingAltStep = false;
        f.rewind();
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;

            no_comment = true;
            if ( *cp == PoundChar )
                {
                no_comment = false;
                cp++;
                }

            if ( strings_are_equal(cp, AltStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                {
                UsingAltStep = no_comment;
                get_alt_step_from_line( cp );
                have_alt_step = true;
                break;
                }
            }

        f.close();
        }
    }

if ( have_park_step && have_alt_step && have_start_step )
    return true;

/*
----------------------------------------------------
Get any missing values from the control_all.txt file
---------------------------------------------------- */
version = current_version();
s.get_exe_dir_file_name( ComputerName, ft2_control_program_name(version, CurrentMachine) );
if ( !s.file_exists() )
    return false;

if ( f.open_for_read(s.text()) )
    {
    UsingAltStep = false;
    while ( true )
        {
        cp = f.readline();
        if ( !cp )
            break;

        /*
        ------------------------
        Get the start valve step
        ------------------------ */
        if ( !have_start_step )
            {
            if ( strings_are_equal(cp, StartStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                {
                StartStepVolts = get_string_value_from_line( cp );
                have_start_step = true;
                }
            }

        /*
        ----------------------
        Get the alternate step
        ---------------------- */
        if ( !have_alt_step )
            {
            no_comment = true;
            if ( *cp == PoundChar )
                {
                no_comment = false;
                cp++;
                }

            if ( strings_are_equal(cp, AltStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                {
                UsingAltStep = no_comment;
                get_alt_step_from_line( cp );
                have_alt_step = true;
                }
            }

        /*
        -----------------------
        Get the park valve step
        ----------------------- */
        if ( !have_park_step )
            {
            if ( strings_are_equal(cp, ParkStepNum.text(), CTRL_PROG_NUMBER_LEN) )
                {
                ParkStepVolts = get_string_value_from_line( cp );
                have_park_step = true;
                }
            }

        if ( have_park_step && have_alt_step && have_start_step )
            break;
        }
    f.close();
    }

return true;
}

/***********************************************************************
*                            ALT_STEP_TEXT                             *
***********************************************************************/
TCHAR * alt_step_text()
{
static TCHAR buf[100];
TCHAR * cp;

cp = copystring( buf, AltStepNum.text() );
cp = copystring( cp,  AltStepVelPrefix.text() );
cp = copystring( cp,  AltStepVel.text() );
cp = copystring( cp,  AltStepAccPrefix.text() );
cp = copystring( cp,  AltStepAcc.text() );
cp = copystring( cp,  AltStepPosPrefix.text() );
cp = copystring( cp,  AltStepPos.text() );
cp = copystring( cp,  AltStepEnd.text() );

return buf;
}

/***********************************************************************
*                        SHOW_ALT_STEP_CONTROLS                        *
***********************************************************************/
static void show_alt_step_controls()
{
static UINT ids[] = { SLOW_SHOT_VELOCITY_EBOX,   ACCELERATION_EBOX,   ACCUMULATOR_ON_POSITION_EBOX,
               SLOW_SHOT_VELOCITY_STATIC, ACCELERATION_STATIC, ACCUMULATOR_ON_POSITION_STATIC };
const int nof_ids = sizeof(ids)/sizeof(UINT);

int i;
int command;
WINDOW_CLASS w;

command = SW_SHOW;
if ( is_checked(ControlModsDialog.control(NORMAL_STEP_RADIO)) )
    command = SW_HIDE;
for ( i=0; i<nof_ids; i++ )
    {
    w = ControlModsDialog.control(ids[i]);
    w.show( command );
    }

}

/***********************************************************************
*                            SHOW_ALT_STEP                             *
*  Construct the Alt Step line and display it in the ALT_STEP_TBOX.    *
***********************************************************************/
static void show_alt_step()
{
set_text( ControlModsDialog.control(ALT_STEP_TBOX), alt_step_text() );
}

/***********************************************************************
*                           SHOW_START_STEP                            *
***********************************************************************/
static void show_start_step()
{
STRING_CLASS s;

s.upsize(20);

s = StartStepNum;
s += StartStepVoltsPrefix;
s += StartStepVolts;
s.set_text( ControlModsDialog.control(VOLTAGE_STEP_TBOX) );
}

/***********************************************************************
*                            SHOW_PARK_STEP                            *
***********************************************************************/
static void show_park_step()
{
STRING_CLASS s;

s.upsize(20);

s = ParkStepNum;
s += ParkStepVoltsPrefix;
s += ParkStepVolts;
s.set_text( ControlModsDialog.control(PARK_VOLTAGE_STEP_TBOX) );
}

/***********************************************************************
*                       SAVE_CONTROL_MOD_CHANGES                       *
***********************************************************************/
static void save_control_mod_changes()
{
NAME_CLASS s;
FILE_CLASS f;

s.get_ft2_ctrl_prog_steps_file_name( ComputerName, CurrentMachine.name );

if ( f.open_for_write(s.text()) )
    {
    temp_message( SavingString.text() );

    s.get_text( ControlModsDialog.control(VOLTAGE_STEP_TBOX) );
    f.writeline( s.text() );

    if ( UsingAltStep )
        {
        s.get_text( ControlModsDialog.control(ALT_STEP_TBOX) );
        }
    else
        {
        s = PoundString;
        s += alt_step_text();
        }
    f.writeline( s.text() );

    s.get_text( ControlModsDialog.control(PARK_VOLTAGE_STEP_TBOX) );
    f.writeline( s.text() );

    /*
    ---------------------
    Step .129 is the same
    --------------------- */
    if ( s.replace(ParkStepNum.text(), ParkStep2Num.text()) )
        f.writeline( s.text() );

    f.close();
    }
else
    {
    MessageBox( 0, s.text(), resource_string(CANT_OPEN_FILE_STRING), MB_OK | MB_SYSTEMMODAL );
    }

if ( !IsRemoteMonitor )
    AutoBack.backup_machine( CurrentMachine.name );

send_command_to_current_machine( ClearControlFileDate, true );

/*
--------------------------------------
Make text that main window will delete
-------------------------------------- */
MainWindow.send( WM_NEWSETUP, 0, (LPARAM) maketext(CurrentMachine.name.text()) );
}

/***********************************************************************
*                      GET_ALT_STEP_VELOCITY_FOR_EBOX                  *
***********************************************************************/
static void get_alt_step_velocity_for_ebox( STRING_CLASS & dest )
{
TCHAR * buf;
TCHAR * cp;
int        i;
double     d;
BOOLEAN    is_percent;

is_percent = FALSE;
if ( !AltStepVel.contains(HChar) )
    {
    d = AltStepVel.real_value();
    d *= 4;
    d = CurrentPart.velocity_from_x4( d );
    }
else
    {
    buf = 0;
    d   = 0.0;
    if ( !AltStepVel.isempty() )
        buf = maketext( AltStepVel.text() );
    if ( buf )
        {
        is_percent = TRUE;
        strip( buf );
        i = 0;
        if ( is_numeric(*buf) )
            i = asctoint32( buf );
        cp = findchar( HChar, buf );
        if ( cp )
            {
            cp += 2; /* Skip the H8 */
            i += (int) hextoul( cp );
            }
        d = (double) i;
        d /= 100.0;

        delete[] buf;
        }
    }
dest.upsize( MAX_DOUBLE_LEN );
rounded_double_to_tchar( dest.text(), d );
if ( is_percent )
    dest += PercentChar;
}

/***********************************************************************
*                         COPY_MODS_TO_SCREEN                          *
***********************************************************************/
static void copy_mods_to_screen()
{
STRING_CLASS s;
HWND   w;
WINDOW_CLASS wc;
int    x;
double d;

s.upsize(30);

w = ControlModsDialog.handle();
UpdatingControlModsDialog = true;

/*
----------
Start Step
---------- */
set_text( w, VOLTAGE_EBOX, ft2_volts_toascii(StartStepVolts.text()) );
show_start_step();

/*
----------
Park Valve
---------- */
set_text( w, PARK_VOLTAGE_EBOX, ft2_volts_toascii(ParkStepVolts.text()) );
show_park_step();

/*
--------
Alt Step
-------- */
if ( UsingAltStep )
    show_alt_step();
else
    set_text( w, ALT_STEP_TBOX, NormalStep );


x = NORMAL_STEP_RADIO;
if ( UsingAltStep )
    x = ALT_STEP_RADIO;
CheckRadioButton( w, NORMAL_STEP_RADIO, ALT_STEP_RADIO, x );

show_alt_step_controls();

/*
--------
Velocity
-------- */
get_alt_step_velocity_for_ebox( s );
s.set_text( w, SLOW_SHOT_VELOCITY_EBOX );

/*
------------
Acceleration
------------ */
d = AltStepAcc.real_value();
d *= 4;
d = CurrentPart.dist_from_x4( d );

s.upsize( MAX_DOUBLE_LEN );
rounded_double_to_tchar( s.text(), d );
s.set_text( w, ACCELERATION_EBOX );

/*
-----------------------
Accumulator On Position
----------------------- */
d = AltStepPos.real_value();
d = CurrentPart.dist_from_x4( d );

s.upsize( MAX_DOUBLE_LEN );
rounded_double_to_tchar( s.text(), d );
s.set_text( w, ACCUMULATOR_ON_POSITION_EBOX );

UpdatingControlModsDialog = false;
}

/***********************************************************************
*                      LOAD_CURRENT_CONTROL_MODS                       *
***********************************************************************/
static void load_current_control_mods()
{
load_control_mods_from_file();
copy_mods_to_screen();
}

/***********************************************************************
*                         CHECK_FOR_NEW_BOARD                          *
***********************************************************************/
static void check_for_new_board()
{
static STRING_CLASS my_machine;

if ( my_machine != CurrentMachine.name )
    {
    my_machine = CurrentMachine.name;
    load_current_control_mods();
    }
}

/***********************************************************************
*                       DO_ALT_STEP_TYPE_SELECT                        *
***********************************************************************/
static void do_alt_step_type_select()
{
if ( is_checked(ControlModsDialog.control(NORMAL_STEP_RADIO)) )
    {
    UsingAltStep = false;
    set_text( ControlModsDialog.control(ALT_STEP_TBOX), NormalStep );
    }
else
    {
    UsingAltStep = true;
    show_alt_step();
    }
}

/***********************************************************************
*                           UPDATE_PARK_STEP                           *
***********************************************************************/
static void update_park_step()
{
STRING_CLASS s;

s.get_text( ControlModsDialog.control(PARK_VOLTAGE_EBOX) );
ParkStepVolts = ascii_volts_to_ft2_volts( s.text() );
show_park_step();
}

/***********************************************************************
*                          UPDATE_START_STEP                           *
***********************************************************************/
static void update_start_step()
{
STRING_CLASS s;

s.get_text( ControlModsDialog.control(VOLTAGE_EBOX) );
StartStepVolts = ascii_volts_to_ft2_volts( s.text() );
show_start_step();
}

/***********************************************************************
*                      EXTRACT_ALT_STEP_VELOCITY                       *
***********************************************************************/
static void extract_alt_step_velocity( STRING_CLASS & s )
{
double d;

d = s.real_value();

if ( s.contains(PercentChar) )
    {
    s = percent_to_hex( d );
    }
else
    {
    d = CurrentPart.x4_from_velocity( d );
    d /= 4;
    d += .5;
    s = int32toasc( (int32) d );
    }
}

/***********************************************************************
*                           UPDATE_ALT_STEP                            *
* The user has changed one of the three alt step values. Update the    *
* globals and display the new step if the alt step is selected.        *
***********************************************************************/
static void update_alt_step( int id )
{
double       d;
int          i;
STRING_CLASS s;

s.upsize( MAX_NUMBER_LEN );

s.get_text( ControlModsDialog.control(id) );
d = s.real_value();

switch ( id )
    {
    case SLOW_SHOT_VELOCITY_EBOX:
        extract_alt_step_velocity( s );
        AltStepVel = s;
        break;

    case ACCELERATION_EBOX:
    case ACCUMULATOR_ON_POSITION_EBOX:
        d = CurrentPart.x4_from_dist( d );
        if ( id == ACCELERATION_EBOX )
            d /= 4;
        d += .5;
        i = int( d );

        if ( id == ACCELERATION_EBOX )
            AltStepAcc = i;
        else
            AltStepPos = i;
        break;
    }

if ( UsingAltStep )
    show_alt_step();
}

/***********************************************************************
*                 SET_CONTROL_MODS_SETUP_STRINGS                     *
***********************************************************************/
static void set_control_mods_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rsn[] = {
    { VALVE_START_STATIC,             TEXT("VALVE_START_VOLTS") },
    { GEN_STEP_1_STATIC,              TEXT("GEN_STEP") },
    { VOLTAGE_1_STATIC,               TEXT("VOLTS_PROMPT") },
    { PARK_VOLTAGE_STATIC,            TEXT("PARK_VOLTAGE") },
    { GEN_STEP_2_STATIC,              TEXT("GEN_STEP") },
    { VOLTAGE_2_STATIC,               TEXT("VOLTS_PROMPT") },
    { ALTERNATE_STEP_STATIC,          TEXT("ALT_STEP_DESC") },
    { GEN_STEP_2_STATIC,              TEXT("GEN_STEP") },
    { GEN_STEP_3_STATIC,              TEXT("GEN_STEP") },
    { NORMAL_STEP_RADIO,              TEXT("NORMAL_STEP") },
    { ALT_STEP_RADIO,                 TEXT("ALT_STEP") },
    { SLOW_SHOT_VELOCITY_STATIC,      TEXT("SSV") },
    { ACCELERATION_STATIC,            TEXT("ACCEL") },
    { ACCUMULATOR_ON_POSITION_STATIC, TEXT("ACC_ON_POS") },
    { IDOK,                           TEXT("SAVE_CHANGES") },
    { IDCANCEL,                       TEXT("REVERT_TO_SAVED") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, EmptyString );
}

/***********************************************************************
*                       CONTROL_MODS_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK control_mods_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ControlModsDialog = w;
        set_control_mods_setup_strings( w );
        position_button_child( ControlModsDialog );
        init_control_mods_dialog();
        ControlModsDialog.hide();
        return TRUE;

    case WM_SHOWWINDOW:
        if ( id != 0 )
            check_for_new_board();
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case NORMAL_STEP_RADIO:
            case ALT_STEP_RADIO:
                if ( cmd == BN_CLICKED && !UpdatingControlModsDialog )
                    {
                    do_alt_step_type_select();
                    show_alt_step_controls();
                    return TRUE;
                    }
                break;

            case SLOW_SHOT_VELOCITY_EBOX:
            case ACCELERATION_EBOX:
            case ACCUMULATOR_ON_POSITION_EBOX:
                if ( cmd == EN_CHANGE && !UpdatingControlModsDialog )
                    {
                    update_alt_step( id );
                    return TRUE;
                    }
                break;

            case PARK_VOLTAGE_EBOX:
                if ( cmd == EN_CHANGE && !UpdatingControlModsDialog )
                    {
                    update_park_step();
                    return TRUE;
                    }
                break;

            case VOLTAGE_EBOX:
                if ( cmd == EN_CHANGE && !UpdatingControlModsDialog )
                    {
                    update_start_step();
                    return TRUE;
                    }
                break;

            case IDOK:
                save_control_mod_changes();
                return TRUE;

            case IDCANCEL:
                load_current_control_mods();
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                  SEND_CURRENT_DIGITAL_POT_COMMAND                    *
***********************************************************************/
static void send_current_digital_pot_command()
{
static TCHAR amp_1_cmd[] = TEXT("ODP2=");
STRING_CLASS s;
STRING_CLASS cmd;
unsigned u;

s.get_text( ValveTestDialog.control(CURRENT_DIGITAL_POT_EBOX) );
cmd = amp_1_cmd;
cmd += s;

send_command_to_current_machine( cmd.text(), true );

u = CurrentMachineVars[DIGITAL_POTS_INDEX].uval();
put_byte( u, s.uint_value(), AMP_1_CURRENT_GAIN );
CurrentMachineVars[DIGITAL_POTS_INDEX] = u;
save_machine_vars();
}

/***********************************************************************
*                          LOAD_MASTER_TRACE                           *
***********************************************************************/
static void load_master_trace()
{
FILE_CLASS f;
NAME_CLASS s;

HaveValveMasterData = false;
ValveMasterMax = ValveTestMax;
ValveMasterMin = ValveTestMin;

s.get_valve_test_master_file_name( ComputerName, CurrentMachine.name );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s.text()) )
        {
        f.readbinary( &ValveMasterMax,  sizeof(int) );
        f.readbinary( &ValveMasterMin,  sizeof(int) );
        f.readbinary( ValveMasterData, sizeof(ValveMasterData) );
        f.close();
        HaveValveMasterData = true;
        }
    }
}

/***********************************************************************
*                          SAVE_MASTER_TRACE                           *
***********************************************************************/
static void save_master_trace()
{
FILE_CLASS f;
NAME_CLASS s;

int i;
for ( i=0; i<2000; i++ )
    {
    ValveMasterData[i] = ValveTestData[i];
    }
ValveMasterMax = ValveTestMax;
ValveMasterMin = ValveTestMin;
HaveValveMasterData = true;

s.get_valve_test_master_file_name( ComputerName, CurrentMachine.name );
if ( f.open_for_write(s.text()) )
    {
    f.writebinary( &ValveMasterMax,  sizeof(int) );
    f.writebinary( &ValveMasterMin,  sizeof(int) );
    f.writebinary( ValveMasterData, sizeof(ValveMasterData) );
    f.close();
    }

if ( !IsRemoteMonitor )
    AutoBack.backup_machine( CurrentMachine.name );
}

/***********************************************************************
*                        PLOT_OSCILLOSCOPE_DATA                        *
***********************************************************************/
static void plot_oscilloscope_data( int * data )
{
const int DEFAULT_MAX = 5000;
const int DEFAULT_MIN =    0;

RECTANGLE_CLASS r;
HDC             dc;
HPEN            oldpen;
WINDOW_CLASS    w;

int             dy;
int             i;
int             height;
int             width;
int             x;
int             y;
int             ymax;
int             ymin;
int             yrange;

/*
--------------------------------------
Figure the range to show on the screen
-------------------------------------- */
ymax = DEFAULT_MAX;
if ( ValveTestMax > ymax || ValveMasterMax > ymax )
    {
    ymax = ValveTestMax;
    if ( ValveMasterMax > ymax )
        ymax = ValveMasterMax;
    ymax /= 1000;
    if ( ymax < 10 )
        ymax++;
    ymax *= 1000;
    }

ymin = DEFAULT_MIN;

if ( ValveTestMin < ymin || ValveMasterMin < ymin )
    {
    ymin = ValveTestMin;
    if ( ValveMasterMin < ymax )
        ymin = ValveMasterMin;

    ymin /= 1000;
    if ( ymin < 0 && ymin > -10 )
        ymin--;
    ymin *= 1000;
    }

w = ValveTestDialog.control(VALVE_TEST_MAX_Y_XBOX);
w.set_title( int32toasc((int32) ymax/1000) );

w = ValveTestDialog.control(VALVE_TEST_MIN_Y_XBOX);
w.set_title( int32toasc((int32) ymin/1000) );

yrange = ymax - ymin;
if ( !yrange )
    yrange = 1;

dc = ValveTestDialog.get_dc();

oldpen = 0;
if ( data == ValveMasterData )
    oldpen = (HPEN) SelectObject( dc, MasterPen );

w =  ValveTestDialog.control( Y_AXIS_LINE );

w.get_move_rect( r );
x = r.right + 2;

ValveTestDialog.get_client_rect( r );
r.left = x;

r.right--;
r.bottom--;

width  = r.width();
height = r.height();

x = r.left;
dy = data[0] - ymin;
dy *= height;
dy /= yrange;
y = r.bottom - dy;

MoveToEx( dc, r.left, y, 0 );

for ( i=1; i<2000; i++ )
    {
    x = r.left + i*width/2000;
    dy = data[i] - ymin;
    dy *= height;
    dy /= yrange;
    y = r.bottom - dy;
    LineTo( dc, x, y );
    }

if ( oldpen )
    SelectObject( dc, oldpen );

ValveTestDialog.release_dc( dc );
}

/***********************************************************************
*                       DO_NEW_OSCILLOSCOPE_DATA                       *
***********************************************************************/
static void do_new_oscilloscope_data( LPARAM lParam )
{
int i;
int x;
FTII_OSCILLOSCOPE_DATA * op;

if ( !lParam )
    return;

op = (FTII_OSCILLOSCOPE_DATA *) lParam;

x = op->ch_17_20[0][0];
ValveTestMax = x;
ValveTestMin = x;
if ( op )
    {
    for ( i=0; i<2000; i++ )
        {
        x = op->ch_17_20[i][0];
        ValveTestData[i] = x;
        if ( x > ValveTestMax )
            ValveTestMax = x;
        if ( x < ValveTestMin )
            ValveTestMin = x;
        }

    if ( !HaveValveMasterData )
        {
        ValveMasterMax = ValveTestMax;
        ValveMasterMin = ValveTestMin;
        }

    /*
    -------------------------------------------------------
    All the data was originally allocated as character data
    ------------------------------------------------------- */
    delete[] (char *) lParam;

    ValveTestDialog.refresh();
    }
}

/***********************************************************************
*              SHOW_VALVE_TEST_REVISION_D_CONTROLS                     *
***********************************************************************/
static void show_valve_test_revision_d_controls()
{
static UINT id[] = {
    CURRENT_DIGITAL_POT_EBOX,
    CURRENT_DIGITAL_POT_SPIN,
    DIGITAL_POT_STATIC
    };
const int nof_ids = sizeof(id)/sizeof(int);

WINDOW_CLASS w;
int          i;
bool         is_visible;

if ( HasDigitalServoAmp )
    is_visible = true;
else
    is_visible = false;

for ( i=0; i<nof_ids; i++ )
    {
    w = ValveTestDialog.control( id[i] );
    w.show( is_visible );
    }
}

/***********************************************************************
*                      REFRESH_VALVE_TEST_SCREEN                       *
***********************************************************************/
static void refresh_valve_test_screen()
{
WINDOW_CLASS w;
unsigned u;

show_valve_test_revision_d_controls();

if ( HasDigitalServoAmp )
    {
    u = digital_pot_setting( AMP_1_CURRENT_GAIN );

    w = ValveTestDialog.control( CURRENT_DIGITAL_POT_SPIN );
    w.send( UDM_SETPOS,   0, MAKELPARAM((WORD) u, (WORD) 0) );
    }
}

/***********************************************************************
*                         INIT_VALVE_TEST_DATA                         *
***********************************************************************/
static void init_valve_test_data()
{
static UDACCEL accel = { 1, 1 };
WINDOW_CLASS w;
unsigned u;
int i;

for ( i=0; i<2000; i++ )
    {
    ValveTestData[i]   = 0;
    ValveMasterData[i] = 0;
    }

MasterPen = CreatePen( PS_SOLID, 1, RedColor );

u = digital_pot_setting( AMP_1_CURRENT_GAIN );


w = ValveTestDialog.control( CURRENT_DIGITAL_POT_SPIN );
w.send( UDM_SETRANGE, 0, MAKELPARAM((WORD) 127, (WORD) 0) );
w.send( UDM_SETACCEL, 0, (LPARAM) &accel );
w.send( UDM_SETPOS,   0, MAKELPARAM((WORD) u, (WORD) 0) );

show_valve_test_revision_d_controls();
}

/***********************************************************************
*                    SET_VALVE_TEST_SETUP_STRINGS                     *
***********************************************************************/
static void set_valve_test_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rsn[] = {
    { SAVE_AS_MASTER_TRACE_PB, TEXT("SAV_AS_MASTER") },
    { VERT_VOLTS_STATIC,       TEXT("VERT_VOLTS") },
    { VALVE_TEST_STATIC,       TEXT("VALVE_TEST") },
    { DIGITAL_POT_STATIC,      TEXT("DIG_POT_GAIN") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, NULL );
}

/***********************************************************************
*                        VALVE_TEST_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK valve_test_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static UINT timer_id = 0;

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ValveTestDialog = w;
        set_valve_test_setup_strings( w );
        position_button_child( ValveTestDialog );
        ValveTestDialog.hide();
        init_valve_test_data();
        ValveTestDialog.send( WM_DBINIT ); /* This will turn off the UpdatingValveTestScreen */
        return TRUE;

    case WM_DBINIT:
        if ( UpdatingValveTestScreen )
            {
            UpdatingValveTestScreen = FALSE;
            }
        else
            {
            UpdatingValveTestScreen = TRUE;
            load_master_trace();
            refresh_valve_test_screen();
            ValveTestDialog.send( WM_DBINIT );
            }
        return TRUE;

    case WM_PAINT:
        ValveTestDialog.post( WM_NEW_DATA, 0, 0 );
        break;

    case WM_TIMER:
        if ( timer_id )
            {
            KillTimer( w, timer_id );
            timer_id = 0;
            ValveTestDialog.refresh();
            }
        return TRUE;

    case WM_NEW_DATA:
        if ( lParam )
            {
            if ( timer_id )
                {
                KillTimer( w, timer_id );
                timer_id = 0;
                }
            do_new_oscilloscope_data( lParam );
            timer_id = SetTimer (w, 0, 5000, NULL) ;
            }
        else
            {
            if ( HaveValveMasterData )
                plot_oscilloscope_data( ValveMasterData );
            if ( timer_id )
                plot_oscilloscope_data( ValveTestData );
            }

        return TRUE;

    case WM_VSCROLL:
        if ( HWND(lParam) == ValveTestDialog.control(CURRENT_DIGITAL_POT_SPIN) && !UpdatingSetDacScreen )
            {
            if ( id == SB_ENDSCROLL )
                {
                /*
                --------------------------------------------------------------------------------------------
                The SB_ENDSCROLL message is only sent when the mouse button is released. Sending the command
                at this time prevents me from banging away at the st2 board.
                -------------------------------------------------------------------------------------------- */
                send_current_digital_pot_command();
                return TRUE;
                }
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_AS_MASTER_TRACE_PB:
                /*
                --------------------------------------------------------------------------------------------
                This is the default push button. If the operator changes the digital pot and presses enter I
                need to check here.
                -------------------------------------------------------------------------------------------- */
                if ( GetFocus() == ValveTestDialog.control(CURRENT_DIGITAL_POT_EBOX) )
                    send_current_digital_pot_command();
                else
                    save_master_trace();
                return TRUE;
            }
        break;

    case WM_DESTROY:
        if ( timer_id )
            {
            KillTimer( w, timer_id );
            timer_id = 0;
            }

        if ( MasterPen )
            {
            DeleteObject( MasterPen );
            MasterPen = 0;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      INIT_CURRENT_VALUES_DIALOG                      *
***********************************************************************/
static void init_current_values_dialog()
{
WINDOW_CLASS w;
int          i;
INI_CLASS    ini;
NAME_CLASS   s;
TCHAR      * cp;
unsigned     x;
BOOLEAN      is_checked;

SwitchChannelsXbox = CurrentValuesDialog.control( CH_17_20_XBOX );
i  = current_channel_mode();
is_checked = FALSE;
if ( i == CHANNEL_MODE_5 || i == CHANNEL_MODE_7 )
    is_checked = TRUE;
set_checkbox( SwitchChannelsXbox, is_checked );

position_button_child( CurrentValuesDialog );

w = CurrentValuesDialog.control( CYCLE_START_TBOX );
w.hide();

w = CurrentValuesDialog.control( ERROR_MESSAGE_TBOX );
set_text( w.handle(), EmptyString );

w = CurrentValuesDialog.control( WAITING_FOR_TBOX );
set_text( w.handle(), EmptyString );

w = CurrentValuesDialog.control( CONTROL_STATE_TBOX );
set_text( w.handle(), EmptyString );

/*
-------------
Analog Values
------------- */
for ( i=0; i<8; i++ )
    {
    w = CurrentValuesDialog.control( FT_A1_THERM+i );
    PostMessage( w.handle(), PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) 4095) );

    w = CurrentValuesDialog.control( VOLTS_1_TBOX+i );
    set_text( w.handle(), EmptyString );
    }

MonitorStatusControl[0].bit = CYCLE_START_BIT;
MonitorStatusControl[0].w   = CurrentValuesDialog.control( CYCLE_START_TBOX );
MonitorStatusControl[0].w.hide();

MonitorStatusControl[1].bit = SHOT_IN_PROGRESS_BIT;
MonitorStatusControl[1].w   = CurrentValuesDialog.control( SHOT_IN_PROGRESS_TBOX );
MonitorStatusControl[1].w.hide();

MonitorStatusControl[2].bit = ZSPEED_BIT;
MonitorStatusControl[2].w   = CurrentValuesDialog.control( ZERO_SPEED_TBOX );
MonitorStatusControl[2].w.hide();

MonitorStatusControl[3].bit = SHOT_COMPLETE_BIT;
MonitorStatusControl[3].w   = CurrentValuesDialog.control( SHOT_COMPLETE_TBOX );
MonitorStatusControl[3].w.hide();

w = CurrentValuesDialog.control( FOLLOWING_OVERFLOW_TBOX );
w.hide();

s.get_exe_dir_file_name( ComputerName, FtiiIniFile );

ini.set_file( s.text() );

for ( i=0; i<NofInputBitControls; i++ )
    {
    InputBitControl[i].w = CurrentValuesDialog.control( InputBitControlId[i] );
    InputBitControl[i].w.hide();
    s = TEXT( "InputBitCtrl" );
    s += i;
    if ( ini.find(CurrentValuesSection, s.text()) )
        {
        s = ini.get_string();
        cp = findchar( CommaChar, s.text() );
        if ( cp )
            {
            *cp = NullChar;
            x = s.uint_value();
            InputBitControl[i].bit = 1;
            if ( x > 0 )
                InputBitControl[i].bit <<= x;
            cp++;
            InputBitControl[i].name = cp;
            }
        }
    }

for ( i=0; i<NofOutputBitControls; i++ )
    {
    OutputBitControl[i].w = CurrentValuesDialog.control( OutputBitControlId[i] );
    OutputBitControl[i].w.hide();
    s = TEXT( "OutputBitCtrl" );
    s += i;
    if ( ini.find(CurrentValuesSection, s.text()) )
        {
        s = ini.get_string();
        cp = findchar( CommaChar, s.text() );
        if ( cp )
            {
            *cp = NullChar;
            x = s.uint_value();
            OutputBitControl[i].bit = 1;
            if ( x > 0 )
                OutputBitControl[i].bit <<= x;
            cp++;
            OutputBitControl[i].name = cp;
            }
        }
    }

load_limit_switch_wires_for_realtime_output_display();
}

/***********************************************************************
*                              LOG_ALS_FILE                            *
***********************************************************************/
void log_als_file( STRING_CLASS & s )
{
FILE_CLASS f;
TIME_CLASS t;

if ( !AlsLogIsEnabled )
    return;

t.get_local_time();
s += SpaceChar;
s += t.hhmmss();
if ( f.open_for_append(AlsLogFile) )
    {
    f.writeline( s.text() );
    f.close();
    }
}

/***********************************************************************
*                             LOG_INPUT_BITS                           *
***********************************************************************/
static void log_input_bits()
{
static TCHAR * instring[32] = {
    { TEXT("Mon 0") },
    { TEXT("Mon 1") },
    { TEXT("Mon 2") },
    { TEXT("Mon 3") },
    { TEXT("Mon 4") },
    { TEXT("Mon 5") },
    { TEXT("Mon 6") },
    { TEXT("Mon 7") },
    { TEXT("Mon 8") },
    { TEXT("Mon 9") },
    { TEXT("Mon 10") },
    { TEXT("Mon 11") },
    { TEXT("Mon 12") },
    { TEXT("Mon 13") },
    { TEXT("Mon 14") },
    { TEXT("Mon 15") },
    { TEXT("Pump On") },
    { TEXT("Start Shot") },
    { TEXT("Follow Through") },
    { TEXT("Retract") },
    { TEXT("Jog Shot") },
    { TEXT("Manual Mode") },
    { TEXT("At Home") },
    { TEXT("Bit 23") },
    { TEXT("Bit 24") },
    { TEXT("OK for FS") },
    { TEXT("Vac ON") },
    { TEXT("Low Impact") },
    { TEXT("Fill Test") },
    { TEXT("Intens") },
    { TEXT("Bit 30 (0-31)") },
    { TEXT("E-Stop") } };

static STRING_CLASS s;
unsigned mask;
unsigned changedbits;
int      i;

changedbits = LastInputBits ^ CurrentOpStatus.isw1;
if ( changedbits == 0 )
    return;

mask = 1;
for ( i=0; i<32; i++ )
    {
    if ( mask & changedbits )
        {
        s = instring[i];
        if ( mask & CurrentOpStatus.isw1 )
            s += OnString;
        else
            s += OffString;
        if ( ErrorLogIsEnabled )
            write_log( s.text() );
        }
    mask <<= 1;
    }
}

/***********************************************************************
*                            LOG_OUTPUT_BITS                           *
***********************************************************************/
static void log_output_bits()
{
static TCHAR * outstring[32] = {
    { TEXT("MonOut 0") },
    { TEXT("MonOut 1") },
    { TEXT("MonOut 2") },
    { TEXT("MonOut 3") },
    { TEXT("MonOut 4") },
    { TEXT("MonOut 5") },
    { TEXT("MonOut 6") },
    { TEXT("MonOut 7") },
    { TEXT("MonOut 8") },
    { TEXT("MonOut 9") },
    { TEXT("MonOut 10") },
    { TEXT("MonOut 11") },
    { TEXT("MonOut 12") },
    { TEXT("MonOut 13") },
    { TEXT("MonOut 14") },
    { TEXT("MonOut 15") },
    { TEXT("Slow Shot      OUTPUT") },
    { TEXT("Accumulator    OUTPUT") },
    { TEXT("OutBit 18") },
    { TEXT("OutBit 19") },
    { TEXT("OutBit 20") },
    { TEXT("OutBit 21") },
    { TEXT("OutBit 22") },
    { TEXT("OutBit 23") },
    { TEXT("OutBit 24") },
    { TEXT("LS 5") },
    { TEXT("LS 6") },
    { TEXT("Fault") },
    { TEXT("LS 1") },
    { TEXT("LS 2") },
    { TEXT("LS 3") },
    { TEXT("LS 4") }
    };

static STRING_CLASS s;
unsigned mask;
unsigned changedbits;
int      i;

changedbits = LastOutputBits ^ CurrentOpStatus.osw1;
if ( changedbits == 0 )
    return;

mask = 1;
for ( i=0; i<32; i++ )
    {
    if ( mask & changedbits )
        {
        s = outstring[i];
        if ( mask & CurrentOpStatus.osw1 )
            s += OnString;
        else
            s += OffString;
        if ( ErrorLogIsEnabled )
            write_log( s.text() );
        if ( i == 16 || i == 17)
            log_als_file( s );
        }
    mask <<= 1;
    }
}

/***********************************************************************
*                             UPDATE_OPTOS                             *
* This updates the opto displays. I assume OptoMode is TRUE.           *
* Only the control optos are displayed.                                *
***********************************************************************/
void update_optos( unsigned outputbits, unsigned inputbits )
{
const unsigned mask = 0xF;
int i;

/*
-----------------------------------
The control bits are the high words
----------------------------------- */
outputbits >>= 16;
inputbits >>= 16;

for ( i=0; i<NOF_OPTOS; i++ )
    {
    InputOpto[i].send( WM_NEW_DATA, (WPARAM) (inputbits & mask), 0 );
    inputbits >>= 4;
    OutputOpto[i].send( WM_NEW_DATA, (WPARAM) (outputbits & mask), 0 );
    outputbits >>= 4;
    }
}

/***********************************************************************
*                        DO_NEW_CURRENT_VALUES                         *
***********************************************************************/
static void do_new_current_values()
{
static STRING_CLASS ns;
static STRING_CLASS os;
static WINDOW_CLASS wc;
static HWND         w;
static int          i;
static int          chan;
static unsigned     x;
static double       d;
static bool         should_be_visible;
static BOOLEAN      show_channels_17_20;
static int          channel_mode;
static int          cmdchan;
static int          lvdtchan;
static STRING_CLASS s;
static STRING_CLASS logs;

bool is_dac_analog;

if ( Lindex >= 0 )
    logs = TEXT("Pos = " );

channel_mode  = current_channel_mode();

w = CurrentValuesDialog.control( FT_CURRENT_POSITION_TBOX );
d = CurrentPart.dist_from_x4( (double) CurrentOpStatus.pos );
if ( IsHiresSensor )
    {
    d /= 16.0;
    ns = fixed_string( d, 5, 3 );
    }
else
    {
    ns = position_string( d );
    }

if ( Lindex >= 0 )
    {
    logs += ns;
    logs += TEXT(", Vel = " );
    }
os.get_text( w );
if ( os != ns || NeedRefresh )
    {
    ns.set_text( w );
    w = CurrentValuesDialog.control( FT_POS_THERM );
    if ( CurrentOpStatus.pos > Max_Position )
        {
        Max_Position = CurrentOpStatus.pos;
        if ( IsHiresSensor )
            ns = fixed_string( d, 4, 2 );
        ns.set_text( CurrentValuesDialog.control(MAX_POSITION_TBOX) );
        PostMessage( w, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) Max_Position) );
        }
    i = 0;
    maxint( i, CurrentOpStatus.pos );
    PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
    }

w  = CurrentValuesDialog.control( FT_CURRENT_VELOCITY_TBOX );
d  = CurrentPart.velocity_from_x4((double)(CurrentOpStatus.vel * 4));
ns = fixed_string( d, 4, 4 );
if ( Lindex >= 0 )
    logs += ns;

os.get_text( w );
if ( os != ns || NeedRefresh )
    {
    ns.set_text( w );
    w = CurrentValuesDialog.control( FT_VEL_THERM );
    if ( CurrentOpStatus.vel > Max_Velocity )
        {
        Max_Velocity = CurrentOpStatus.vel;
        PostMessage( w, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) Max_Velocity) );
        }
    i = 0;
    maxint( i, CurrentOpStatus.vel );
    PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
    }

w = CurrentValuesDialog.control( CURRENT_STEP_NUMBER_TBOX );
ns = int32toasc( CurrentOpStatus.blk_no );
os.get_text( w );
if ( os != ns || NeedRefresh )
    {
    ns.set_text( w );
    w = CurrentValuesDialog.control( WAITING_FOR_TBOX );
    os = StepMessage.message( (int32) ns.int_value() );
    os.set_text( w );
    if ( ErrorLogIsEnabled )
        {
        s = TEXT("Current Step = ");
        s += ns;
        s += SpaceString;
        s += os;
        write_log( s.text() );
        }
    }

if ( CurrentOpStatus.status_word1 != Control_Status || NeedRefresh )
    {
    Control_Status = CurrentOpStatus.status_word1;
    w = CurrentValuesDialog.control( CONTROL_STATE_TBOX );
    if ( Control_Status & PROGRAM_RUNNING_BIT )
        {
        ns = resource_string( CONTROL_ACTIVE_STRING );
        }
    else if ( !(Control_Status & PROGRAM_LOADED_BIT) )
        {
        ns = resource_string( NO_PROGRAM_STRING );
        }
    else if ( Control_Status & FATAL_ERROR_BIT )
        {
        ns = resource_string( CONTROL_FATAL_STRING );
        }
    else if ( Control_Status & WARNING_BIT )
        {
        ns = resource_string( CONTROL_WARNING_STRING );
        }
    else if ( !(CurrentOpStatus.isw4 & MASTER_ENABLE_BIT) )
        {
        ns = resource_string( NO_MASTER_ENABLE_STRING );
        }
    else
        {
        ns = resource_string( UNKNOWN_STRING );
        }
    ns.set_text( w );
    if ( ErrorLogIsEnabled )
        {
        s = TEXT("Status Word 1 = ");
        s += ns;
        write_log( s.text() );
        }

    /*
    ----------------------------------------------------------------------
    This section used to be outside the if brackets. I moved it in 9/14/05
    ---------------------------------------------------------------------- */
    w = CurrentValuesDialog.control( ERROR_MESSAGE_TBOX );
    if ( Control_Status & FATAL_ERROR_BIT )
        {
        ns = fatal_error_string( CurrentOpStatus.fatal_error );
        ns.set_text( w );
        if ( ErrorLogIsEnabled )
            write_log( ns.text() );
        }
    else if ( Control_Status & WARNING_BIT )
        {
        ns = warning_error_string( CurrentOpStatus.warning );
        ns.set_text( w );
        if ( ErrorLogIsEnabled )
            write_log( ns.text() );
        }
    else
        {
        set_text( w, EmptyString );
        }
    }

if ( OptoMode )
    update_optos( CurrentOpStatus.osw1, CurrentOpStatus.isw1 );

if ( CurrentOpStatus.isw1 != LastInputBits || NeedRefresh )
    {
    for ( i=0; i<NofInputBitControls; i++ )
        {
        x = InputBitControl[i].bit & CurrentOpStatus.isw1;
        if ( x )
            should_be_visible = true;
        else
            should_be_visible = false;
        if ( should_be_visible != InputBitControl[i].is_visible )
            {
            InputBitControl[i].is_visible = should_be_visible;
            if ( !OptoMode )
                InputBitControl[i].w.show( should_be_visible );
            }
        }

    if ( ErrorLogIsEnabled )
        {
        log_input_bits();
        }
    else
        {
        Lindex = -1;
        }
    LastInputBits = CurrentOpStatus.isw1;
    }

if ( CurrentOpStatus.osw1 != LastOutputBits || NeedRefresh )
    {
    for ( i=0; i<NofOutputBitControls; i++ )
        {
        x = OutputBitControl[i].bit & CurrentOpStatus.osw1;
        if ( x )
            should_be_visible = true;
        else
            should_be_visible = false;
        if ( should_be_visible != OutputBitControl[i].is_visible )
            {
            OutputBitControl[i].is_visible = should_be_visible;
            if ( !OptoMode )
                OutputBitControl[i].w.show( should_be_visible );
            }
        }

    log_output_bits();
    LastOutputBits = CurrentOpStatus.osw1;
    }

if ( Monitor_Status != CurrentOpStatus.monitor_status || NeedRefresh )
    {
    for ( i=0; i<NofMonitorStatusControls; i++ )
        {
        x = MonitorStatusControl[i].bit & CurrentOpStatus.monitor_status;
        if ( x )
            should_be_visible = true;
        else
            should_be_visible = false;
        if ( should_be_visible != MonitorStatusControl[i].is_visible )
            {
            MonitorStatusControl[i].is_visible = should_be_visible;
            MonitorStatusControl[i].w.show( should_be_visible );
            }
        }

    Monitor_Status = CurrentOpStatus.monitor_status;
    }

show_channels_17_20 = is_checked( SwitchChannelsXbox );

if ( show_channels_17_20 )
    {
    if ( channel_mode == CHANNEL_MODE_7 )
        {
        w = CurrentValuesDialog.control(VOLTS_3_TBOX);
        i = CurrentOpStatus.dac[0];
        d = (double) i;
        d /= 1000.0;
        ns = fixed_string( d, 3, 2 );
        os.get_text( w );
        if ( os != ns )
            {
            ns.set_text( w );
            w = CurrentValuesDialog.control( FT_A3_THERM );
            i += 10000;
            PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
            }
        }

    w = CurrentValuesDialog.control(VOLTS_6_TBOX);
    /*
    -------------------------------------------------------
    Pressure Control is shown on the second set of readings
    ------------------------------------------------------- */
    if ( channel_mode == CHANNEL_MODE_7 )
        i = CurrentOpStatus.dac[2];
    else
        i = CurrentOpStatus.dac[0];

    d = (double) i;
    d /= 1000.0;
    ns = fixed_string( d, 3, 2 );
    os.get_text( w );
    if ( os != ns )
        {
        ns.set_text( w );
        w = CurrentValuesDialog.control( FT_A6_THERM );
        i += 10000;
        PostMessage( w, PBM_SETPOS, (WPARAM) i, 0L );
        }
    }

chan = 0;
for ( i=0; i<8; i++ )
    {
    if ( show_channels_17_20 )
        {
        if ( channel_mode == CHANNEL_MODE_7 )
            {
            /*
            -------------------------------------------------------
            In channel mode 7 boxes 3 and 6 are the command outputs
            and have already been set.
            ------------------------------------------------------- */
            if ( i == 2 || i == 5 )
                {
                chan++;
                continue;
                }
            switch ( i )
                {
                case 3:
                    chan = 6;   /* W1 = Meas. Command 1 */
                    break;
                case 4:
                    chan = 4;   /* C1 = LVDT 1 */
                    break;
                case 6:
                    chan = 7;   /* W2 = Meas Command 2 */
                    break;
                case 7:
                    chan = 5;   /* C3 = LVDT 2 */
                    break;
                }
            }
        else
            {
            /*
            -----------------------------------------------------------
            The 6th box is the command voltage and has already been set
            ----------------------------------------------------------- */
            if ( i == 5 )
                {
                chan++;
                continue;
                }
            }
        }

    if ( AnalogValue[chan] != CurrentAnalogValue[chan] || NeedToRefreshAnalogs || NeedRefresh )
        {
        AnalogValue[chan] = CurrentAnalogValue[chan];
        w  = CurrentValuesDialog.control( VOLTS_1_TBOX+i);

        x = AnalogValue[chan];
        is_dac_analog = false;
        switch ( channel_mode )
            {
            case 4:
                if ( chan > 3 )
                    is_dac_analog = true;
                break;
            case 5:
                if ( chan > 5 )
                    is_dac_analog = true;
                break;
            case 7:
                if ( chan > 1 )
                    is_dac_analog = true;
                break;
            }

        if ( is_dac_analog )
            ns = ascii_dac_volts( x );
        else
            ns = ascii_0_to_5_volts( x );

        ns.set_text( w );
        w = CurrentValuesDialog.control( FT_A1_THERM+i );
        PostMessage( w, PBM_SETPOS, (WPARAM) x, 0L );
        }

    chan++;
    }

if ( Lindex >= 0 )
    {
    if ( show_channels_17_20 )
        {
        cmdchan  = current_measured_command_index( DAC_1 );
        lvdtchan = current_lvdt_feedback_index( DAC_1 );

        logs += TEXT( ", Meas CMD = " );
        x = AnalogValue[cmdchan];
        logs += ascii_dac_volts( x );

        logs += TEXT( ", LVDT Feedback = " );
        x = AnalogValue[lvdtchan];
        logs += ascii_dac_volts( x );
        }

    /*
    -------------------------
    Channel 1 = head pressure
    ------------------------- */
    logs += TEXT( ", Ch 1= " );
    i = CurrentPart.analog_sensor[0];
    x = AnalogValue[0];
    logs += ascii_double( AnalogSensor[i].converted_value(x) );

    /*
    -------------------------
    Channel 2 = head pressure
    ------------------------- */
    logs += TEXT( ", Ch 2= " );
    i = CurrentPart.analog_sensor[1];
    x = AnalogValue[1];
    logs += ascii_double( AnalogSensor[i].converted_value(x) );

    write_log( logs.text() );
    Lindex++;
    if ( Lindex > 9 )
        Lindex = -1;
    }

NeedToRefreshAnalogs = false;
NeedRefresh          = FALSE;
}

/***********************************************************************
*                              NEED_BRUSH                              *
***********************************************************************/
bool need_brush( HWND w )
{
int i;

for ( i=0; i<NofInputBitControls; i++ )
    {
    if ( InputBitControl[i].w == w )
        return true;
    }

for ( i=0; i<NofOutputBitControls; i++ )
    {
    if ( OutputBitControl[i].w == w )
        return true;
    }

for ( i=0; i<NofMonitorStatusControls; i++ )
    {
    if ( MonitorStatusControl[i].w == w )
        return true;
    }

return false;
}

/***********************************************************************
*                          SAVE_CHANNEL_MODE                           *
***********************************************************************/
static void save_channel_mode( int new_mode )
{
NAME_CLASS  s;
INI_CLASS   ini;

s.get_ft2_editor_settings_file_name( CurrentMachine.computer, CurrentMachine.name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
ini.put_int( CurrentChannelModeKey, new_mode );
}

/***********************************************************************
*                       SEND_CHANNEL_MODE_COMMAND                      *
***********************************************************************/
static int send_channel_mode_command()
{
STRING_CLASS s;
int mode;

if ( is_checked(SwitchChannelsXbox) )
    {
    if ( HasDigitalServoAmp )
        mode = CHANNEL_MODE_7;
    else
        mode = CHANNEL_MODE_5;
    }
else
    {
    mode = CHANNEL_MODE_1;
    }

s = ChannelModeSetString;
s += mode;;

send_command_to_current_machine( s.text(), true );
save_channel_mode( mode );

return mode;
}

/***********************************************************************
*                       SEND_CHANNEL_MODE_COMMAND                      *
***********************************************************************/
static void send_channel_mode_command( int new_mode )
{
STRING_CLASS s;

s = ChannelModeSetString;
s += new_mode;

send_command_to_current_machine( s.text(), true );
save_channel_mode( new_mode );
}

/***********************************************************************
*                          TOGGLE_IO_DISPLAY                           *
*   Toggle between the text display and the opto module display        *
***********************************************************************/
static void toggle_io_display()
{
int i;
STRING_CLASS s;

/*
-------------------------------
Hide the visible controls first
------------------------------- */
if ( OptoMode )
    {
    OptoMode = FALSE;
    for ( i=0; i<NOF_OPTOS; i++ )
        {
        InputOpto[i].hide();
        OutputOpto[i].hide();
        }
    }
else
    {
    OptoMode = TRUE;
    for ( i=0; i<NofInputBitControls; i++ )
        {
        if ( InputBitControl[i].is_visible )
            InputBitControl[i].w.hide();
        }
    for ( i=0; i<NofOutputBitControls; i++ )
        {
        if ( OutputBitControl[i].is_visible )
            OutputBitControl[i].w.hide();
        }
    }

if ( OptoMode )
    {
    for ( i=0; i<NOF_OPTOS; i++ )
        {
        InputOpto[i].show();
        OutputOpto[i].show();
        }
    update_optos( LastOutputBits, LastInputBits );
    }
else
    {
    for ( i=0; i<NofInputBitControls; i++ )
        {
        if ( InputBitControl[i].is_visible )
            InputBitControl[i].w.show();
        }
    for ( i=0; i<NofOutputBitControls; i++ )
        {
        if ( OutputBitControl[i].is_visible )
            OutputBitControl[i].w.show();
        }
    }

if ( OptoMode )
    s = StringTable.find( TEXT("VERT_TEXT") );
else
    s = StringTable.find( TEXT("VERT_OPTO") );

set_text( CurrentValuesDialog.control(TOGGLE_IO_DISPLAY_PB), s.text() );

if ( OptoMode )
    SendMessage( ToolTipWindow, TTM_ACTIVATE, (WPARAM) TRUE, 0 );
else
    SendMessage( ToolTipWindow, TTM_ACTIVATE, (WPARAM) FALSE, 0 );
}

/***********************************************************************
*                            KILL_LOG_FILE                             *
***********************************************************************/
void kill_log_file()
{
STRING_CLASS s;
STRING_CLASS e;

if ( is_checked(CurrentValuesDialog.control(LOG_FILE_ZIP_XBOX)) )
    {
    if ( ErrorLogZipFile.file_exists() )
        ErrorLogZipFile.delete_file();

    if ( ErrorLogFile.file_exists() )
        {
        if ( ErrorLogTempFile.file_exists() )
            ErrorLogTempFile.delete_file();
        if ( ErrorLogFile.moveto(ErrorLogTempFile) )
            {
            s = TEXT( "pkzip25 -add -lev=9 " );
            s += ErrorLogZipFile;
            s += SpaceString;
            s += ErrorLogTempFile;
            e = exe_directory();
            execute_and_wait( s.text(), e.text() );
            }
        ErrorLogTempFile.delete_file();
        }
    }
else
    {
    ErrorLogFile.delete_file();
    }
}

/***********************************************************************
*                            DO_OPTO_CLICK                             *
* wParam is the wire [1,2,3,4] of this opto. lParam is the window      *
* handle.                                                              *
***********************************************************************/
static void do_opto_click( WPARAM wParam, LPARAM lParam )
{
const int WIRES_PER_OPTO = 4;
int i;
int wire_to_toggle;
unsigned bit;
BOOLEAN  turn_on;

wire_to_toggle = 0;

for ( i=0; i<NOF_TEST_ALARM_OPTOS; i++ )
    {
    if ( TestAlarmOpto[i].handle() == (HWND) lParam )
        {
        wire_to_toggle += (int) wParam;
        bit = wirebit( wire_to_toggle );
        turn_on = TRUE;
        if ( bit & CurrentOpStatus.osw1 )
            turn_on = FALSE;
        send_output_bits_to_current_machine( bit, turn_on );
        break;
        }
    wire_to_toggle += WIRES_PER_OPTO;
    }
}

/***********************************************************************
*                      SEND_ZERO_MESSAGE_TO_BOARD                      *
***********************************************************************/
static void send_zero_message_to_board()
{
if ( CurrentOpStatus.status_word1 & PROGRAM_RUNNING_BIT )
    {
    if ( !(MANUAL_MODE_MASK & CurrentOpStatus.isw1) )
        return;
    }

send_command_to_current_machine( ZeroPositionCmd, true );
}

/***********************************************************************
*                      DO_NEW_ALARM_OUTPUT_VALUES                      *
***********************************************************************/
static void do_new_alarm_output_values()
{
const unsigned mask = 0xF;
static unsigned Last_Osw1;

unsigned outputbits;
int i;

if ( CurrentOpStatus.osw1 != Last_Osw1 || NeedRefresh )
    {
    Last_Osw1 = CurrentOpStatus.osw1;
    outputbits = Last_Osw1;;

    for ( i=0; i<NOF_TEST_ALARM_OPTOS; i++ )
        {
        TestAlarmOpto[i].send( WM_NEW_DATA, (WPARAM) (outputbits & mask), 0 );
        outputbits >>= 4;
        }
    }
}

/***********************************************************************
*                        INIT_TEST_ALARM_OPTOS                         *
***********************************************************************/
static void init_test_alarm_optos()
{
int id;
int i;
id = MONITOR_OUTPUT_1_OPTO;
for ( i=0; i<NOF_TEST_ALARM_OPTOS; i++ )
    {
    TestAlarmOpto[i] = TestAlarmOutputsDialog.control( id );
    id++;
    }
}

/***********************************************************************
*                  POSITION_TEST_ALARM_OUTPUTS_DIALOG                  *
***********************************************************************/
static void position_test_alarm_outputs_dialog()
{
RECTANGLE_CLASS pr;
RECTANGLE_CLASS r;
WINDOW_CLASS    w;
int  x;
int  y;

/*
-----------------------------------------------------------------------
Get the client window and set the left edge to the right of the buttons
----------------------------------------------------------------------- */
w = MainWindow.control( Button[0].id );
pr = MainWindowClientRect;
w.get_move_rect( r );
pr.left = r.left;

/*
-----------------------------------
Center the dialog in this rectangle
----------------------------------- */
TestAlarmOutputsDialog.get_move_rect( r );
x = pr.left + ( pr.width() - r.width() )/2;
y = ( pr.height() - r.height() )/2;
TestAlarmOutputsDialog.move( x, y );
}

/***********************************************************************
*                  SET_TEST_ALARM_SETUP_STRINGS                    *
***********************************************************************/
static void set_test_alarm_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rsn[] = {
    { ALARM_TEST_STATIC,       TEXT("ALARM_TEST") },
    { TOGGLE_ALL_OUTPUTS_XBOX, TEXT("TURN_ON_ALL_OUTPUTS_STRING") },
    { MON_OUTPUTS_STATIC,      TEXT("MON_OUTPUTS") },
    { CTL_OUTPUTS_STATIC,      TEXT("CTL_OUTPUTS") },
    { MON_OPTO_1_STATIC,       TEXT("0_TO_3") },
    { MON_OPTO_2_STATIC,       TEXT("4_TO_7") },
    { MON_OPTO_3_STATIC,       TEXT("8_TO_11") },
    { MON_OPTO_4_STATIC,       TEXT("12_TO_15") },
    { CTL_OPTO_1_STATIC,       TEXT("0_TO_3") },
    { CTL_OPTO_2_STATIC,       TEXT("4_TO_7") },
    { CTL_OPTO_3_STATIC,       TEXT("8_TO_11") },
    { CTL_OPTO_4_STATIC,       TEXT("12_TO_15") },
    { CLICK_LED_STATIC,        TEXT("CLICK_TO_TOGGLE") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);
const UINT box = 0;
const int nof_boxes = 0;

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                     TEST_ALARM_OUTPUTS_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK test_alarm_outputs_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
const unsigned ALL_BITS = 0xFFFFFFFF;
int     id;
BOOLEAN turn_on;
HWND    w;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        TestAlarmOutputsDialog = hWnd;
        set_test_alarm_setup_strings( hWnd );
        TestAlarmOutputsDialog.hide();
        //position_test_alarm_outputs_dialog();
        position_button_child( TestAlarmOutputsDialog );

        init_test_alarm_optos();
        break;

    case WM_NEW_DATA:
        do_new_alarm_output_values();
        return TRUE;

    case WM_OPTO_CLICK:
        do_opto_click( wParam, lParam );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case TOGGLE_ALL_OUTPUTS_XBOX:
                w = GetDlgItem( hWnd, TOGGLE_ALL_OUTPUTS_XBOX );
                turn_on = is_checked( w );
                if ( turn_on )
                    id = TURN_OFF_ALL_OUTPUTS_STRING;
                else
                    id = TURN_ON_ALL_OUTPUTS_STRING;
                set_text( w, resource_string(id) );
                send_output_bits_to_current_machine( ALL_BITS, turn_on );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                           SIZE_OPTO_BUTTON                           *
***********************************************************************/
static void size_opto_button()
{
TCHAR           cs[] = TEXT("A");
TCHAR         * cp;
HDC             dc;
RECT            r;
STRING_CLASS    s;
WINDOW_CLASS    w;
int             width;

s = StringTable.find( TEXT("VERT_OPTO") );
s += StringTable.find( TEXT("VERT_TEXT") );
if ( s.isempty() )
    return;

/*
-------------------------------------
Get the width of the widest character
------------------------------------- */
w = CurrentValuesDialog.control( TOGGLE_IO_DISPLAY_PB );
dc = w.get_dc();
cp = s.text();
width = 0;
while ( *cp != NullChar )
    {
    if ( *cp != SpaceChar )
        {
        cs[0] = *cp;
        maxint( width, pixel_width(dc, cs) );
        }
    cp++;
    }

/*
----------------------------------
Make the button 1.5*max_char_width
---------------------------------- */
width *= 3;
width /= 2;

w.get_move_rect( r );
if ( width > (r.right-r.left) )
    {
    r.right = r.left + width;
    w.move( r );
    }
w.release_dc( dc );
}

/***********************************************************************
*                               DO_UPDOWN                              *
***********************************************************************/
static void do_updown( int id )
{
int mymode;
int i;
WINDOW_CLASS w;

mymode = current_channel_mode();

if ( id == FT2_MODE_UP_PB )
    {
    if ( mymode < CHANNEL_MODE_3 )
        mymode++;
    else
        mymode = 0;
    }
else
    {
    if ( mymode > CHANNEL_MODE_1 )
        mymode--;
    else
        mymode = 0;
    }

if ( mymode > 0 )
    {
    send_channel_mode_command( mymode );
    send_command_to_current_machine( SEND_OP_DATA_PACKETS, true );

    i = 5;
    if ( mymode == CHANNEL_MODE_2 )
        i = 9;
    else if ( mymode == CHANNEL_MODE_3 )
        i = 13;

    for ( id=CH_5_TBOX; id<=CH_8_TBOX; id++ )
        {
        w = CurrentValuesDialog.control( id );
        w.set_title( int32toasc(i) );
        i++;
        }
    }
}

/***********************************************************************
*                  SET_CURRENT_VALUES_SETUP_STRINGS                    *
***********************************************************************/
static void set_current_values_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rsn[] = {
    { CV_VEL_STATIC,             TEXT("VELOCITY_STRING") },
    { CV_POS_STATIC,             TEXT("POSITION_STRING") },
    { CV_CHAN_STATIC,            TEXT("CHAN_STRING") },
    { CV_VOLTS_STATIC,           TEXT("VOLTS") },
    { MAX_POS_STATIC,            TEXT("MAX_POS") },
    { MAX_POSITION_RESET_BUTTON, TEXT("ZERO") },
    { SAVE_TOTAL_STROKE_BUTTON,  TEXT("SAVE_TSL") },
    { START_SHOT_TBOX,           TEXT("START_SHOT") },
    { FOLLOW_THROUGH_TBOX,       TEXT("FOLLOW_THRU") },
    { RETRACT_TBOX,              TEXT("RETRACT") },
    { JOG_SHOT_TBOX,             TEXT("JOG_SHOT") },
    { MANUAL_MODE_TBOX,          TEXT("MANUAL_MODE") },
    { AT_HOME_TBOX,              TEXT("AT_HOME") },
    { OK_FOR_VAC_FAST_TBOX,      TEXT("OK_FOR_VAC") },
    { VACUUM_ON_TBOX,            TEXT("VAC_ON") },
    { LOW_IMPACT_TBOX,           TEXT("LOW_IMPACT") },
    { FILL_TEST_TBOX,            TEXT("FILL_TEST") },
    { SLOW_SHOT_TBOX,            TEXT("SLOW_SHOT") },
    { ACCUMULATOR_TBOX,          TEXT("ACCUMULATOR") },
    { LS1_TBOX,                  TEXT("LS1") },
    { LS2_TBOX,                  TEXT("LS2") },
    { LS3_TBOX,                  TEXT("LS3") },
    { LS4_TBOX,                  TEXT("LS4") },
    { LS5_TBOX,                  TEXT("LS5") },
    { LS6_TBOX,                  TEXT("LS6") },
    { CV_CUR_STEP_STATIC,        TEXT("CUR_STEP") },
    { CYCLE_START_TBOX,          TEXT("CYCLE_START") },
    { SHOT_IN_PROGRESS_TBOX,     TEXT("SHOT_IN_PROGRESS") },
    { SHOT_COMPLETE_TBOX,        TEXT("SHOT_COMPLETE") },
    { ZERO_SPEED_TBOX,           TEXT("ZERO_SPEED") },
    { FOLLOWING_OVERFLOW_TBOX,   TEXT("FOLLOWING_OVERFLOW") },
    { CV_INPUTS_STATIC,          TEXT("INPUTS") },
    { CV_OUTPUTS_STATIC,         TEXT("OUTPUTS") },
    { CV_STATUS_STATIC,          TEXT("STATUS_TITLE") },
    { CV_CTL_STATE_STATIC,       TEXT("CTL_STATE") },
    { WAITING_FOR_TBOX,          TEXT("WAITING_FOR") },
    { PRESSURE_CONTROL_STATIC,   TEXT("PRES_CTL") },
    { BINARY_VALVE_STATIC,       TEXT("BIN_VALVE") },
    { LOG_FILE_VIEW_PB,          TEXT("VIEW") },
    { LOG_FILE_KILL_PB,          TEXT("DEL") },
    { LOG_FILE_ZIP_XBOX,         TEXT("ZIP") },
    { ERROR_LOG_TBOX,            TEXT("EVENT_LOG") },
    { UPLOAD_FILE_STATIC,        TEXT("UPLOAD_FILE") },
    { COMM_STATUS_STATIC,        TEXT("COM_STATUS_STRING") },
    { LOG_FILE_SETUP_PB,         TEXT("OFF_STRING") },
    { TOGGLE_IO_DISPLAY_PB,      TEXT("VERT_OPTO") },
    { MONITOR_STATUS_STATIC,     TEXT("MONITOR_STATUS_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    VELOCITY_UNITS_TBOX,
    POSITION_UNITS_TBOX,
    FT_CURRENT_VELOCITY_TBOX,
    FT_CURRENT_POSITION_TBOX,
    MAX_POSITION_TBOX,
    CURRENT_STEP_NUMBER_TBOX,
    CONTROL_STATE_TBOX,
    ERROR_MESSAGE_TBOX,
    FLASH_VERSION_TBOX,
    UPLOAD_FILE_NAME_TBOX,
    COMM_STATUS_TB,
    MONITOR_STATUS_TB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                            VIEW_ERRORLOG                             *
* WinExec needs an ascii argument. unicode_to_char works here because  *
* the ascii string is half the size of the unicode and I know the      *
* string is not empty.                                                 *
***********************************************************************/
static void view_errorlog()
{
char       * cs;
STRING_CLASS s;

s = TEXT( "c:\\windows\\system32\\notepad.exe " );
s += ErrorLogFile;

#ifdef UNICODE
cs = (char *) s.text();
unicode_to_char( cs, s.text() );
#else
cs = s.text();
#endif

WinExec( cs, SW_SHOWNORMAL );
}

/***********************************************************************
*                         CURRENT_VALUES_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK current_values_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOOLEAN firstime = TRUE;
static HWND w;
static int  id;
static HWND error_log_tbox_handle = 0;

MSG     mymsg;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CurrentValuesDialog = hWnd;
        size_opto_button();
        set_current_values_setup_strings( hWnd );
        CurrentValuesCommStatusTb = CurrentValuesDialog.control(COMM_STATUS_TB);
        CurrentValuesMonitorStatusTb = CurrentValuesDialog.control(MONITOR_STATUS_TB);
        error_log_tbox_handle = GetDlgItem( hWnd, ERROR_LOG_TBOX );
        init_current_values_dialog();
        /*
        -------------------------------------------------------------------
        Initialize the window classes for the input and output opto modules
        ------------------------------------------------------------------- */
        for ( id=0; id<NOF_OPTOS; id++ )
            {
            InputOpto[id]  = CurrentValuesDialog.control(  INPUT_1_OPTO+id );
            OutputOpto[id] = CurrentValuesDialog.control( OUTPUT_1_OPTO+id );
            }
        CurrentValuesDialog.hide();
        break;

    case WM_NEW_DATA:
        do_new_current_values();
        return TRUE;

    case WM_DBINIT:
        if ( firstime )
            {
            set_channel_5_8_labels();
            firstime = FALSE;
            }
        refresh_current_values();
        NeedRefresh = TRUE;
        return TRUE;

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        if ( OptoMode )
            {
            mymsg.lParam  = lParam;
            mymsg.wParam  = wParam;
            mymsg.message = msg;
            mymsg.hwnd    = hWnd;
            SendMessage( ToolTipWindow, TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &mymsg );
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case FT2_MODE_UP_PB:
            case FT2_MODE_DOWN_PB:
                do_updown( id );
                return TRUE;

            case CH_17_20_XBOX:
                set_channel_5_8_labels( send_channel_mode_command() );
                /*
                ----------------------------------------------------------------
                I must now restart the op status packets so the board reads v429
                ---------------------------------------------------------------- */
                send_command_to_current_machine( SEND_OP_DATA_PACKETS, true );
                return TRUE;

            case TOGGLE_IO_DISPLAY_PB:
                toggle_io_display();
                return TRUE;

            case MAX_POSITION_RESET_BUTTON:
                w = CurrentValuesDialog.control( FT_POS_THERM );
                PostMessage( w, PBM_SETPOS, (WPARAM) 0, 0L );
                Max_Position = 0;
                set_text( CurrentValuesDialog.control(FT_CURRENT_POSITION_TBOX), TEXT("0") );
                set_text( CurrentValuesDialog.control(MAX_POSITION_TBOX), TEXT("0") );
                PostMessage( w, PBM_SETRANGE, 0, MAKELPARAM(0, (WORD) Max_Position) );
                send_zero_message_to_board();
                return TRUE;

            case LOG_FILE_VIEW_PB:
                view_errorlog();
                return TRUE;

            case LOG_FILE_KILL_PB:
                if ( MessageBox(hWnd, TEXT("Delete Error Log File?"), TEXT("Confirm"), MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK )
                    kill_log_file();
                return TRUE;

            case LOG_FILE_SETUP_PB:
                /*
                ---------------------------------------------------------------------
                ErrorLogIsEnabled is FALSE at program start. It is only changed here.
                --------------------------------------------------------------------- */
                if ( ErrorLogIsEnabled )
                    {
                    ErrorLogIsEnabled = FALSE;
                    set_text( hWnd, LOG_FILE_SETUP_PB, resource_string(OFF_STRING) );
                    }
                else
                    {
                    ErrorLogIsEnabled = TRUE;
                    set_text( hWnd, LOG_FILE_SETUP_PB, resource_string(ON_STRING) );
                    }
            InvalidateRect( error_log_tbox_handle, 0, TRUE );
            }
        break;

    case WM_CTLCOLORSTATIC:
        w = (HWND) lParam;
        if ( w == error_log_tbox_handle )
            {
            if ( ErrorLogIsEnabled )
                {
                SetBkColor( (HDC) wParam, OrangeColor );
                return (int) OrangeBackgroundBrush;
                }
            }
        else if ( need_brush(w) )
            {
            SetBkColor( (HDC) wParam, GreenColor );
            return (int) GreenBackgroundBrush;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      CREATE_DIAGNOSTIC_DIALOGS                       *
***********************************************************************/
static void create_diagnostic_dialogs()
{
CreateDialog(
    MainInstance,
    TEXT("CURRENT_VALUES_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) current_values_dialog_proc );

UpdatingSetDacScreen = TRUE;
CreateDialog(
    MainInstance,
    TEXT("SET_DAC_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) set_dac_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("SET_NULL_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) set_null_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("GLOBAL_PARAMETER_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) global_parameter_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("COMPARE_VARIABLES_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) compare_variables_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("CONTROL_MODS_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) control_mods_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("TEST_ALARM_OUTPUTS_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) test_alarm_outputs_dialog_proc );

UpdatingValveTestScreen = TRUE;
CreateDialog(
    MainInstance,
    TEXT("VALVE_TEST_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) valve_test_dialog_proc );
}

/***********************************************************************
*                          START_NULL_DIALOG                           *
***********************************************************************/
static void start_null_dialog()
{

if ( CurrentWindow == SetDacDialog || CurrentWindow == ValveTestDialog || CurrentWindow == TestAlarmOutputsDialog )
    set_current_window( SetNullDialog );
else
    start_if_ok_to_deactivate( SetNullDialog );
}

/***********************************************************************
*                       START_SET_DAC_DIALOG                           *
***********************************************************************/
static void start_set_dac_dialog()
{

if ( CurrentWindow == SetNullDialog || CurrentWindow == ValveTestDialog || CurrentWindow == TestAlarmOutputsDialog )
    set_current_window( SetDacDialog );
else
    start_if_ok_to_deactivate( SetDacDialog );
}

/***********************************************************************
*                       START_VALVE_TEST_DIALOG                        *
***********************************************************************/
static void start_valve_test_dialog()
{

if ( CurrentWindow == SetNullDialog || CurrentWindow == SetDacDialog || CurrentWindow == TestAlarmOutputsDialog )
    set_current_window( ValveTestDialog );
else
    start_if_ok_to_deactivate( ValveTestDialog );
}

/***********************************************************************
*                       START_ALARM_TEST_DIALOG                        *
***********************************************************************/
static void start_alarm_test_dialog()
{
if ( CurrentWindow == SetNullDialog || CurrentWindow == SetDacDialog || CurrentWindow == ValveTestDialog )
    set_current_window( TestAlarmOutputsDialog );
else
    start_if_ok_to_deactivate( TestAlarmOutputsDialog );
}

/***********************************************************************
*                              ID_ENABLED                              *
***********************************************************************/
static BOOLEAN id_enabled( int id )
{
struct BUTTON_ENABLE_ENTRY {
    int id;
    TCHAR * key;
    };
static BUTTON_ENABLE_ENTRY ButtonLevel[] = {
    { F2_RADIO, Ft2F2LevelKey   },
    { F3_RADIO, Ft2F3LevelKey   },
    { F4_RADIO, Ft2F4LevelKey   },
    { F5_RADIO, Ft2F5LevelKey   },
    { F6_RADIO, Ft2F6LevelKey   },
    { F7_RADIO, Ft2F7LevelKey   },
    { F8_RADIO, Ft2F8LevelKey   },
    { F9_RADIO, Ft2F9LevelKey   },
    { F10_RADIO,Ft2F10LevelKey  }
    };

const int NofButtonLevels = 9;
int i;
int level_needed;
NAME_CLASS s;
INI_CLASS  ini;

for ( i=0; i<NofButtonLevels; i++ )
    {
    if ( ButtonLevel[i].id == id )
        {
        s.get_exe_dir_file_name( MonallIniFile );
        ini.set_file( s.text() );
        ini.set_section( ConfigSection );
        if ( ini.find(ButtonLevel[i].key) )
            {
            level_needed = ini.get_int();
            if ( level_needed <= CurrentPasswordLevel )
                return TRUE;
            return FALSE;
            }
        }
    }

return TRUE;
}

/**********************************************************************
*                               SHOW_BUTTONS                          *
**********************************************************************/
static void show_buttons()
{
static UINT id[] = { F2_RADIO, F3_RADIO, F4_RADIO, F5_RADIO, F6_RADIO, F7_RADIO, F8_RADIO, F9_RADIO, F10_RADIO };
const int nof_ids = sizeof(id)/sizeof(UINT);
int          i;
WINDOW_CLASS w;

for ( i=0; i<nof_ids; i++ )
    {
    w = MainWindow.control( id[i] );
    w.enable( id_enabled(id[i]) );
    }
}

/***********************************************************************
*                         ADD_WIRE_DESCRIPTION                         *
***********************************************************************/
static void add_wire_description( STRING_CLASS & dest, TCHAR * sorc, int wire, unsigned wire_to_check )
{
if ( wire == (int) wire_to_check )
    {
    dest += StringTable.find( sorc );
    dest += SpaceChar;
    dest += WireString;
    dest += TwoSpacesString;
    }
}

/***********************************************************************
*                         OUTPUT_LIST_UPDATE                           *
***********************************************************************/
static void output_list_update()
{
LIMIT_SWITCH_WIRE_CLASS lsw;
PARAMETER_CLASS         p;
PART_CLASS              part;
STRING_CLASS            s;
int base_length;
int i;
int led;
int module;
int wire;

if ( !OutputListbox.handle() )
    return;

if ( WireString.isempty() )
    WireString = StringTable.find( TEXT("WIRE_STRING") );

OutputListbox.empty();
lsw.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
p.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
part.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
wire = 0;
for ( module=0; module<8; module++ )
    {
    for ( led=0; led<4; led++ )
        {
        wire++;
        s = WireString;
        s += SpaceChar;
        s += wire;
        s += SpaceChar;

        if ( module < 4 )
            {
            s += StringTable.find( TEXT("MONITOR_MODULE") );
            s += SpaceChar;
            s += module*4+led+1;
            }
        else
            {
            s += StringTable.find( TEXT("CONTROL_MODULE") );
            s += SpaceChar;
            s += module - 3;
            s += SpaceChar;
            s += StringTable.find( TEXT("LED") );
            s += SpaceChar;
            s += led + 1;
            }
        s += SpaceString;
        base_length = s.len();
        if ( wire == DownTimeAlarmWire )
            {
            s += StringTable.find( TEXT("DOWNTIME_ALARM_WIRE") );
            s += TwoSpacesString;
            }
        if ( wire == MonitorWire )
            {
            s += StringTable.find( TEXT("MONITOR_WIRE") );
            s += TwoSpacesString;
            }
        for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
            {
            if ( lsw.wire_number(i) == wire )
                {
                s += StringTable.find( TEXT("LIMIT_SWITCH") );
                s += SpacePoundString;
                s += i+1;
                s += TwoSpacesString;
                }
            }

        for ( i=0; i<MAX_PARMS; i++ )
            {
            if ( wire == (int) p.parameter[i].limits[ALARM_MIN].wire_number )
                {
                s += p.parameter[i].name;
                s += StringTable.find( TEXT("LOW_ALARM") );
                s += TwoSpacesString;
                }
            if ( wire == (int) p.parameter[i].limits[ALARM_MAX].wire_number )
                {
                s += p.parameter[i].name;
                s += StringTable.find( TEXT("HIGH_ALARM") );
                s += TwoSpacesString;
                }
            if ( wire == (int) p.parameter[i].limits[WARNING_MIN].wire_number )
                {
                s += p.parameter[i].name;
                s += StringTable.find( TEXT("LOW_WARNING") );
                s += TwoSpacesString;
                }
            if ( wire == (int) p.parameter[i].limits[WARNING_MAX].wire_number )
                {
                s += p.parameter[i].name;
                s += StringTable.find( TEXT("HIGH_WARNING") );
                s += TwoSpacesString;
                }

            }

        add_wire_description( s, TEXT("THIRTY_SECOND_TIMEOUT"), wire, part.thirty_second_timeout_wire );
        add_wire_description( s, TEXT("ST2_PGM_ABORT"),         wire, part.st2_program_abort_wire );
        add_wire_description( s, TEXT("SHOT_COMPLETE"),         wire, part.shot_complete_wire );
        add_wire_description( s, TEXT("SHOT_SAVE_COMPLETE"),    wire, part.shot_save_complete_wire );
        add_wire_description( s, TEXT("WARNING_SHOT"),          wire, part.warning_shot_wire );
        add_wire_description( s, TEXT("ALARM_SHOT"),            wire, part.alarm_shot_wire );
        add_wire_description( s, TEXT("GOOD_SHOT"),             wire, part.good_shot_wire );

        if ( s.len() > base_length )
            OutputListbox.add( s.text() );
        }
    }
}

/***********************************************************************
*                       SIZE_OUTPUT_LIST_DIALOG                        *
***********************************************************************/
static void size_output_list_dialog()
{
RECTANGLE_CLASS r;
WINDOW_CLASS w;

OutputListDialog.get_client_rect( r );
w = OutputListbox.handle();

w.move( r );
}

/***********************************************************************
*                  SET_OUTPUT_LIST_SETUP_STRINGS                    *
***********************************************************************/
static void set_output_list_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;

s = StringTable.find( TEXT("OUTPUT_LIST_TITLE") );
w = hWnd;
w.set_title( StringTable.find(s.text()) );
w.refresh();
}

/***********************************************************************
*                       OUTPUT_LIST_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK output_list_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        OutputListDialog = hWnd;

        set_output_list_setup_strings( hWnd );
        OutputListbox.init( hWnd, OUTPUT_LIST_LB );
        break;

    case WM_SIZE:
        size_output_list_dialog();
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                OutputListDialog.hide();
                return TRUE;
            }
        break;

    case WM_CLOSE:
        OutputListDialog.hide();
        return FALSE;
    }

return FALSE;
}

/***********************************************************************
*                       END_UPLOAD_FLASH_DIALOG                        *
***********************************************************************/
void end_upload_flash_dialog()
{
HWND w;

w = UploadFlashDialog.handle();
UploadFlashDialog = 0;

if ( w )
    {
    EndDialog( w, 0 );
    }
}

/***********************************************************************
*                      UPLOAD_FLASH_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK upload_flash_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        UploadFlashDialog = hWnd;
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case EXIT_RADIO:
            case IDCANCEL:
                end_upload_flash_dialog();
                return TRUE;
            }
        break;

    case WM_CLOSE:
        end_upload_flash_dialog();
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    SEND_DATA_SERVER_SHOT_NOTIFICATION                *
*                                                                      *
*                           M01,VISITRAK,NS,1395                       *
***********************************************************************/
static void send_data_server_shot_notification( TCHAR * mach_part, TCHAR * shot )
{
static TCHAR ns[] = TEXT( ",NS," );
static STRING_CLASS s;

s = mach_part;
s += ns;
s += shot;

poke_data( DDE_MONITOR_TOPIC, ItemList[DS_NOTIFY_INDEX].name, s.text() );
}

/***********************************************************************
*                         POKE_NEWSHOT_MESSAGE                         *
***********************************************************************/
void poke_newshot_message( WPARAM wParam, LPARAM lParam )
{
TCHAR * buf;
TCHAR * shot;
TCHAR * cp;
STRING_CLASS machine;
STRING_CLASS part;
STRING_CLASS s;
NAME_CLASS   fn;
DWORD        file_attributes;

/*
--------------------------------------------------------
The machine name and part name are pointed to by lParam.
                      "M01,VISITRAK"
-------------------------------------------------------- */
buf = (TCHAR *) lParam;
s = buf;
s.next_field();
machine = s;
s.next_field();
part = s;

/*
---------------------------------------
The shot number is pointed to by wParam
--------------------------------------- */
shot = (TCHAR *) wParam;

/*
---------------------------------------------------------------------------------
I now use the FILE_ATTRIBUTE_ARCHIVE to indicate the shot is ready to be archived
--------------------------------------------------------------------------------- */
fn.get_profile_ii_name( ComputerName, machine, part, shot );
file_attributes = GetFileAttributes( fn.text() );

if ( file_attributes & FILE_ATTRIBUTE_ARCHIVE )
    send_data_server_shot_notification( buf, shot );

/*
-------------------------------------
Need machine name only for topic name
------------------------------------- */
cp = findchar( CommaChar, buf );
if ( cp )
    *cp = NullChar;

poke_data( buf, ItemList[SHOT_NUMBER_INDEX].name, shot );

/*
-------------------------------------------------
I have to clean up the memory used by the strings
------------------------------------------------- */
delete[] buf;
delete[] shot;
}

/***********************************************************************
*                    SEND_DATA_SERVER_DOWN_NOTIFICATION                *
*                                                                      *
*                            M01,VISITRAK,DT                           *
***********************************************************************/
static void send_data_server_down_notification( TCHAR * sorc )
{
static TCHAR s[] = TEXT( ",DT" );
TCHAR * cp;

cp = findchar( TabChar, sorc );
if ( cp )
    {
    *cp = CommaChar;
    cp++;
    cp = findchar( TabChar, cp );
    }

if ( cp )
    {
    lstrcpy( cp, s );
    poke_data( DDE_MONITOR_TOPIC, ItemList[DS_NOTIFY_INDEX].name, sorc );
    }
}

/***********************************************************************
*                            LOAD_MAIN_MENU                            *
***********************************************************************/
static void load_main_menu()
{
NAME_CLASS s;

s.get_exe_dir_file_name( MyMenuFile );
MainMenu.read( s, CurrentPasswordLevel );
MainMenu.resize();
}

/***********************************************************************
*                          PASSWORD_LEVEL_CALLBACK                     *
***********************************************************************/
void password_level_callback( TCHAR * topic, short item, HDDEDATA edata )
{
TCHAR buf[MAX_INTEGER_LEN+3];
DWORD bytes_copied;
DWORD bufsize = sizeof( buf );
INI_CLASS    ini;
NAME_CLASS   s;

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

bytes_copied = DdeGetData( edata, (LPBYTE) buf, bufsize, 0 );
if ( bytes_copied > 0 )
    {
    *(buf+MAX_INTEGER_LEN) = NullChar;
    fix_dde_name( buf );
    CurrentPasswordLevel = asctoint32( buf );

    s.get_exe_dir_file_name( SureTrakIniFile );
    ini.set_file( s.text() );
    ServiceToolsGrayLevel = ini.get_int( ConfigSection, ServiceToolsGrayLevelKey );
    }

load_main_menu();
load_terminal_menu();
set_gray_menu_items();
show_buttons();
}

/***********************************************************************
*                              PLOT_MIN_CALLBACK                       *
***********************************************************************/
void plot_min_callback( TCHAR * topic, short item, HDDEDATA edata )
{
if ( !MainWindow.is_minimized() )
    MainWindow.minimize();
}

HDC MyDc;

/***********************************************************************
*                               TEXT_OUT                               *
***********************************************************************/
static void text_out( int x, int y, TCHAR * sorc )
{
TextOut( MyDc, (int) x, (int) y, sorc, lstrlen(sorc) );
}

/**********************************************************************
*                                CANEXIT                              *
**********************************************************************/
bool canexit()
{
NAME_CLASS s;
INI_CLASS  ini;
int        pw_level_needed;

/*
------------------------------
Always shut down from eventman
------------------------------ */
if ( ShutdownAnyway )
    return true;

s.get_exe_dir_file_name( SureTrakIniFile );
ini.set_file( s.text() );
pw_level_needed = ini.get_int( ConfigSection, ExitEnableLevelKey );

return ( pw_level_needed <= CurrentPasswordLevel );
}

/***********************************************************************
*                            NOF_FT2_BOARDS                            *
***********************************************************************/
static int nof_ft2_boards()
{
NAME_CLASS s;
DB_TABLE   t;
int        n;

n = 0;
s.get_ft2_boards_dbname( ComputerName );
if ( s.file_exists() )
    {
    t.open( s, FTII_BOARDS_RECLEN, PFL );
    n = t.nof_recs();
    t.close();
    }

return n;
}

/***********************************************************************
*                         POSITION_MAIN_WINDOW                         *
***********************************************************************/
static void position_main_window()
{
long y;
RECTANGLE_CLASS r;
HDC             dc;
HFONT           old_font;
int             n;

SystemParametersInfo( SPI_GETWORKAREA, 0, &r, 0 );

/*
---------------------------------------------------------------------------
The size of the window is determined by the size of the 9 buttons. Each of
these will be 10/9 * font height.
--------------------------------------------------------------------------- */
dc = MainWindow.get_dc();

old_font = INVALID_FONT_HANDLE;
if ( MyFont != INVALID_FONT_HANDLE )
    old_font = (HFONT) SelectObject( dc, MyFont );
y = character_height( dc );
if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );
MainWindow.release_dc( dc );

/*
---------------------------------------------------------------------
I only have room for 7 boards normally. If there are 8 then make room
--------------------------------------------------------------------- */
n = nof_ft2_boards();
n -= 7;
if ( n < 0 )
    n = 0;
n += 9;

y *= n;    /* y*10/9 * 9 buttons */
y *= 10;
y /= 9;
r.top = r.bottom - y;
MainWindow.move( r );

/*
--------------------------------------------------------------------------------
Because I can be started minimized I cannot depend upon get_rect calls so I must
keep my own copies of the main window rects.
-------------------------------------------------------------------------------- */
MainWindowMoveRect = r;

r.bottom -= r.top;
r.top = 0;
MainWindowClientRect = r;
}

/***********************************************************************
*                         NEED_CONNECTED_BRUSH                         *
***********************************************************************/
static HBRUSH need_connected_brush( HWND w )
{
STRING_CLASS s;

s.get_text( w );
if ( s == NotConnected )
    return RedBackgroundBrush;

return 0;
}

/***********************************************************************
*                          NEED_MONITOR_BRUSH                          *
***********************************************************************/
static HBRUSH need_monitor_brush( HWND w )
{
TCHAR buf[101];

if ( get_text(buf, w, 100) )
    {
    if ( CycleStartString == buf )
        return GreenBackgroundBrush;

    if ( compare( buf, FatalString, FatalStringLen) == 0 )
        return RedBackgroundBrush;

    if ( compare( buf, WarningString, WarningStringLen) == 0 )
        return OrangeBackgroundBrush;

    if ( compare( buf, NoFileString, NoFileStringLen) == 0 )
        return RedBackgroundBrush;

    if ( UploadFailed == buf )
        return RedBackgroundBrush;

    if ( NoResponse == buf )
        return RedBackgroundBrush;
    }

return 0;
}

/***********************************************************************
*                             STATUS_PROC                              *
***********************************************************************/
long CALLBACK status_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
BOOLEAN  checked;
int      i;
int      id;
int      cmd;
int      mymode;
HBRUSH   my_brush;
COLORREF my_color;
HWND     w;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        StatusWindow = hWnd;
        MainMenu.create_windows( MainMenuWindowName, hWnd );
        load_main_menu();
        return 0;

    case WM_CTLCOLORSTATIC:
        w = (HWND) lParam;
        for ( i=0; i<NofRows; i++ )
            {
            if ( w == MonitorStatusTbox[i].w )
                {
                my_brush = need_monitor_brush( w );
                if ( my_brush )
                    {
                    if ( my_brush == GreenBackgroundBrush )
                        my_color = GreenColor;
                    else if ( my_brush == OrangeBackgroundBrush )
                        my_color = OrangeColor;
                    else
                        my_color = RedColor;
                    SetBkColor( (HDC) wParam, my_color );
                    return (int) my_brush;
                    }
                break;
                }
            if ( w == ConnectedTbox[i].w )
                {
                my_brush = need_connected_brush( w );
                if ( my_brush )
                    {
                    my_color = RedColor;
                    SetBkColor( (HDC) wParam, my_color );
                    return (int) my_brush;
                    }
                }
            }
        break;

    case WM_COMMAND:
        if ( cmd == BN_CLICKED )
            {
            for ( i=0; i<NofRows; i++ )
                {
                if ( id == MainPushButton[i].id )
                    {
                    CurrentMachine.name.get_text( MachineTbox[i].w );
                    CurrentMachine.find( ComputerName, CurrentMachine.name );
                    if ( IsRemoteMonitor )
                        CurrentMachine.monitor_flags |= MA_MONITORING_ON;
                    load_current_part();
                    CurrentBoardIndex = current_board_index();
                    load_machine_vars();

                    mymode = current_channel_mode();

                    checked = FALSE;
                    if ( mymode == CHANNEL_MODE_5 || mymode == CHANNEL_MODE_7 )
                        checked = TRUE;
                    set_checkbox( SwitchChannelsXbox, checked );
                    set_channel_5_8_labels( mymode );

                    clear_current_value_copies();
                    /*
                    ---------------------------------------------------
                    Show or hide the controls for the digital servo amp
                    --------------------------------------------------- */
                    show_revision_d_controls();
                    refresh_valve_test_screen();

                    return 0;
                    }
                }
            }
        break;

    case WM_VTW_MENU_CHOICE:
        switch (id)
            {
            case MINIMIZE_MENU_CHOICE:
                MainWindow.minimize();
                return 0;

            case FTII_SAVE_LISTBOX_CHOICE:
                save_listbox();
                return 0;

            case FTII_HIRES_SENSOR_TEST_CHOICE:
                HiresSensorTestDialog.show();
                return 0;

            case FTII_CHECK_CONNECTION_CHOICE:
                send_command_to_current_machine( TEXT("V319"), true );
                return 0;

            case FTII_RESET_MACHINE_CHOICE:
                send_command_to_current_machine( CLEAR_CONTROL_FILE_DATE_CMD, true );
                send_command_to_current_machine( CLEAR_NO_UPLOAD_FLAG_CMD, true );
                send_command_to_current_machine( RESET_MACHINE_CMD, true );
                if ( ErrorLogIsEnabled )
                    write_log( ResetString );
                return 0;

            case FTII_CLEAR_TERMINAL_CHOICE:
                MainListBox.empty();
                return 0;

            case FTII_SAVE_SHOT_NAME_FILES_CHOICE:
                toggle_save_shot_name_files();
                return 0;

            case FTII_LOG_MESSAGES_CHOICE:
                toggle_logging_sent_commands();
                return 0;

            case EXIT_MENU_CHOICE:
                exit_main_proc();
                return 0;

            case FTII_OUTPUT_LIST_CHOICE:
                OutputListDialog.show();
                output_list_update();
                if ( TerminalIsVisible )
                    SetFocus( MainEbox );
                return 0;

            case UPLOAD_FLASH_MENU_CHOICE:
                DialogBox(
                    MainInstance,
                    TEXT( "UPLOAD_FLASH_DIALOG" ),
                    MainWindow.handle(),
                    (DLGPROC) upload_flash_dialog_proc
                     );
                return 0;

            case IDHELP:
                get_help();
                return 0;
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                            EXIT_MAIN_PROC                            *
***********************************************************************/
static void exit_main_proc()
{
if ( canexit() )
    {
    if ( ShutdownState == NOT_SHUTTING_DOWN )
        MainWindow.post( WM_CLOSE );
    else if ( ShutdownState == SHUTTING_DOWN_MONITORING )
        BoardMonitorIsRunning = false;
    else if ( ShutdownState == SHUTTING_DOWN_SHOTSAVE )
        ShotSaveIsRunning = false;
    }
else
    {
    resource_message_box( LOW_PW_LEVEL_STRING, UNABLE_TO_COMPLY_STRING );
    }
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int  i;
static int  id;
static int  cmd;
static bool is_diagnostic_window;
TCHAR * s;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        MainWindow    = hWnd;
        CurrentWindow = hWnd;
        position_main_window();
        MainWindow.post( WM_DBINIT );
        return 0;

    case WM_DBINIT:
        hourglass_cursor();
        set_menu_view_state( SentCommandsLogIsEnabled, MainWindow.handle(), (UINT) FTII_LOG_MESSAGES_CHOICE );
        client_dde_startup( hWnd );
        register_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX, new_monitor_setup_callback );
        register_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX,     new_downtime_callback );
        register_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX,   plot_min_callback          );
        register_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX,   password_level_callback    );
        create_controls( load_ftii_boards() );
        create_diagnostic_dialogs();
        if ( StartScreen == START_FT2_ON_REALTIME )
            set_current_window( CurrentValuesDialog );
        else
            set_current_window( StatusWindow );
        get_current_password_level();
        if ( IsRemoteMonitor )
            {
            MyMonitorSetupCount++;
            poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentMachine.name.text() );
            temp_message( TEXT("Shutting Down Current Monitor..."), 0 );
            SetTimer( MainWindow.handle(), REMOTE_MONITOR_TIMER_ID, 3000, 0 );
            }
        else
            {
            begin_monitoring();
            restore_cursor();
            }
        return 0;

    case WM_SYSCOMMAND:
        if ( wParam == SC_CLOSE )
            {
            if ( !canexit() )
                {
                resource_message_box( LOW_PW_LEVEL_STRING, UNABLE_TO_COMPLY_STRING );
                return 0;
                }
            }
        break;

    case WM_TIMER:
        if ( RESUME_TIMER_ID == (UINT) wParam )
            {
            KillTimer( hWnd, RESUME_TIMER_ID );
            if ( WaitingToResume )
                set_current_window( ResumeDialog );
            }
        else if ( CLOSING_TIMER_ID == (UINT) wParam )
            {
            if ( ShutdownState == SHUTTING_DOWN_MONITORING )
                {
                if ( !BoardMonitorIsRunning )
                    {
                    ShutdownState = SHUTTING_DOWN_SHOTSAVE;
                    if ( ShotSaveIsRunning )
                        {
                        ShutdownStatusListbox.add( resource_string(SHUTTING_DOWN_SHOTSAVE_STRING) );
                        SetEvent( ShotSaveEvent );
                        return 0;
                        }
                    else
                        {
                        ShutdownStatusListbox.add( resource_string(SHOTSAVE_NOT_RUNNING_STRING) );
                        }
                    }

                }

            if ( ShutdownState == SHUTTING_DOWN_SHOTSAVE )
                {
                if ( !ShotSaveIsRunning )
                    {
                    if ( IsRemoteMonitor )
                        {
                        ShutdownState = SHUTTING_DOWN_REMOTE;
                        remote_shutdown();
                        ShutdownStatusListbox.add( resource_string(SHUTTING_DOWN_REMOTE_STRING) );
                        return 0;
                        }
                    else
                        {
                        ShutdownState = SHUTDOWN_COMPLETE;
                        }
                    }
                }


            if ( ShutdownState == SHUTTING_DOWN_REMOTE )
                ShutdownState = SHUTDOWN_COMPLETE;

            if ( ShutdownState == SHUTDOWN_COMPLETE )
                {
                KillTimer( hWnd, CLOSING_TIMER_ID );
                MainWindow.post( WM_CLOSE );
                }
            }
        else if ( REMOTE_MONITOR_TIMER_ID == (UINT) wParam )
            {
            KillTimer( hWnd, REMOTE_MONITOR_TIMER_ID );
            kill_temp_message();
            begin_monitoring();
            restore_cursor();
            }
        else if ( HIRES_SENSOR_TEST_TIMER_ID == (UINT) wParam )
            {
            KillTimer( hWnd, HIRES_SENSOR_TEST_TIMER_ID );
            do_hires_sensor_test_timer( hWnd );
            }
        return 0;

    case WM_NO_DATA:
        /*
        ----------------------------------------------------------------------------------------
        This is used by nextshot (in shotsave.cpp) to tell the operator the shot name is invalid
        ---------------------------------------------------------------------------------------- */
        center_temp_message();
        temp_message( resource_string(INVALID_SHOT_NAME_STRING), 3000 );
        return 0;

    case WM_EV_SHUTDOWN:
        ShutdownAnyway = true;
        PostMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_NEW_RT_DATA:
        do_new_rt_data( wParam, lParam );
        return 0;

    case WM_NEW_FT2_STATE:
        do_new_ft2_connect_state( wParam, lParam );
        return 0;

    case WM_NEWSHOT:
        if ( lParam && wParam )
            poke_newshot_message( wParam, lParam );
        return 0;

    case WM_POKEMON:
        /*
        -----------------------------------------------------------
        wParam is the index to the item name, lParam is a pointer
        to a string allocated for this purpose (which I must free).
        ----------------------------------------------------------- */
        s = 0;
        if ( lParam )
            {
            s = (TCHAR *) lParam;
            poke_data( DDE_MONITOR_TOPIC, ItemList[wParam].name, s );

            if ( wParam == (WPARAM) DOWN_DATA_INDEX )
                send_data_server_down_notification( s );

            /*
            -------------------------------------------------------------
            I have to clean up the memory used by the machine name string
            ------------------------------------------------------------- */
            delete[] s;
            }
        return 0;

    case WM_COMMAND:
        if ( cmd == BN_CLICKED )
            {
            /*
            ---------------------------------------------------
            Do nothing if the password level is not high enough
            --------------------------------------------------- */
            if ( !id_enabled(id) )
                return TRUE;

            if ( !WaitingToResume )
                {
                switch ( id )
                    {
                    case F2_RADIO:
                        start_set_dac_dialog();
                        return TRUE;

                    case F3_RADIO:
                        start_null_dialog();
                        return TRUE;

                    case F4_RADIO:
                        start_valve_test_dialog();
                        return TRUE;

                    case F5_RADIO:
                        start_alarm_test_dialog();
                        return TRUE;

                    case F6_RADIO:
                        set_current_window( StatusWindow );
                        return TRUE;

                    case F7_RADIO:
                        set_current_window( TerminalWindow );
                        return TRUE;

                    case F8_RADIO:
                        set_current_window( CurrentValuesDialog );
                        return TRUE;

                    case F9_RADIO:
                        set_current_window( GlobalParameterDialog );
                        return TRUE;

                    case F10_RADIO:
                        set_current_window( ControlModsDialog );
                        return TRUE;
                    }
                }
            }

        switch (id)
            {
            case EXIT_PB:
                exit_main_proc();
                return 0;

            case ALT_F7_KEY:
                if ( !WaitingToResume )
                    set_current_window( CompareVariablesDialog );
                return TRUE;

            case F2_KEY:
            case F3_KEY:
            case F4_KEY:
            case F5_KEY:
            case F6_KEY:
            case F7_KEY:
            case F8_KEY:
            case F9_KEY:
            case F10_KEY:
                if ( !WaitingToResume )
                    {
                    if ( cmd == BN_CLICKED || cmd == 1 )     // 1 set by TranslateAccelerator
                        {
                        if ( CurrentPasswordLevel >= ServiceToolsGrayLevel )
                            {
                            id += F2_RADIO;
                            id -= F2_KEY;
                            if ( id_enabled(id) )
                                PostMessage( MainWindow.control(id), BM_CLICK, 0, 0L );
                            }
                        }
                    }
                break;
            }
        break;

    case WM_HELP:
        get_help();
        return 0;

    case WM_NEWSETUP:
        send_command( (TCHAR *) lParam, RESET_MACHINE_CMD, true );
        delete[] (TCHAR *) lParam;
        create_tooltips();
        return 0;

    case WM_CLOSE:
        if ( CurrentWindow != StatusWindow )
            {
            NeedToCloseMainWindow = true;
            set_current_window( StatusWindow );
            return 0;
            }

        if ( ShutdownState == NOT_SHUTTING_DOWN )
            {
            ShutdownState = SHUTTING_DOWN_MONITORING;
            stop_monitoring();
            ShutdownStatusDialog  = CreateDialog(
               MainInstance,
               TEXT("SHUTDOWN_STATUS_DIALOG"),
               MainWindow.handle(),
               (DLGPROC) shutdown_status_dialog_proc );
            ShutdownStatusDialog.show();
            ShutdownStatusListbox.init( ShutdownStatusDialog.control(SHUTDOWN_STATUS_LB) );
            ShutdownStatusListbox.add( resource_string(SHUTTING_DOWN_MONITOR_STRING) );
            SetTimer( MainWindow.handle(), CLOSING_TIMER_ID, 100, 0 );
            }

        if ( ShutdownState == SHUTDOWN_COMPLETE )
            {
            ShutdownStatusListbox.add( TEXT("Exiting from main dialog proc...") );
            break;
            }
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                          GET_HISTORY_LENGTH                          *
***********************************************************************/
static void get_history_length()
{
INI_CLASS ini;
STRING_CLASS fname;

fname = exe_directory();
fname.cat_path( VisiTrakIniFile );

ini.set_file( fname.text() );

if ( ini.find(ConfigSection, LastNKey) )
    HistoryShotCount = ini.get_int();
}

/***********************************************************************
*                        CHECK_DISK_FREE_SPACE                         *
*       The computer name is the same as the old v5_directory().       *
***********************************************************************/
static void check_disk_free_space()
{
if ( *ComputerName.text() != BackSlashChar )
    {
    if ( free_megabytes(ComputerName.text()) < MIN_FREE_DISK_MB )
        resource_message_box( LOW_DISK_SPACE_STRING, NO_SHOTS_SAVED_STRING );
    }
}

/***********************************************************************
*                        CHECK_FOR_WORKSTATION                         *
* If this computer has no machines of it's own then this is a laptop   *
* office workstation that has been taken out and hooked to a dcm.      *
*                                                                      *
* I need to ask which machine I am going to monitor and then take      *
* over monitoring from the normal DCT.                                 *
*                                                                      *
***********************************************************************/
static void check_for_workstation()
{
MACHINE_NAME_LIST_CLASS m;

m.add_computer( ComputerName );

if ( m.count() < 1 )
    {
    DialogBox(
        MainInstance,
        TEXT("CHOOSE_MONITOR_MACHINE_DIALOG"),
        MainWindow.handle(),
        (DLGPROC) choose_monitor_machine_dialog_proc
        );
    }
}
/**********************************************************************
*                           LOAD_RESOURCES                            *
**********************************************************************/
static void load_resources()
{
INI_CLASS ini;
NAME_CLASS s;

s.get_local_ini_file_name( DisplayIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(CurrentLanguageKey) )
    {
    MainMenu.language = ini.get_string();
    TerminalMenu.language = MainMenu.language;
    StringTable.set_language( MainMenu.language );
    }

s.get_exe_dir_file_name( MyMenuFile );
MainMenu.read( s, 0 );

s.get_stringtable_name( MyResourceFile );
StringTable.read( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );

s.get_stringtable_name( EditPartResourceFile );
StringTable.append( s );

MyFont = FontList.get_handle( StringTable.fontclass() );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOLEAN init( int cmd_show )
{
COMPUTER_CLASS c;
FILE_CLASS     f;
INI_CLASS      ini;
NAME_CLASS     s;
TIME_CLASS     t;
int            w;
WNDCLASS       wc;

if ( is_previous_instance(MyClassName, 0) )
    return FALSE;

MENU_CONTROL_ENTRY::register_window();

c.startup();
ComputerName = c.whoami();
check_for_workstation();
get_history_length();
skip_startup();
shifts_startup();
dcurve_startup();
units_startup();
ANALOG_SENSOR::get();
check_disk_free_space();
AutoBack.get_backup_dir();
s.get_exe_dir_file_name( CONTROL_MESSAGE_FILE );
StepMessage.load( s.text() );

load_resources();
statics_startup();

/*
-----------------------------------------------
Read the labels for the realtime screen analogs
----------------------------------------------- */
CmdString  = StringTable.find( TEXT("CMD_STRING") );
LvdtString = StringTable.find( TEXT("LVDT") );
MeasString = StringTable.find( TEXT("MEAS_STRING") );

/*
-----------------------------------------
Initialize the downtime and alarm options
----------------------------------------- */
s.get_exe_dir_file_name( MonallIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

ClearAlarmsEachShot         = ini.get_boolean( ClearAlarmsEachShotKey );
ClearDownWireFromPopup      = ini.get_boolean( ClearDownWireFromPopupKey );
ClearMonitorWireOnDown      = ini.get_boolean( ClearMonitorWireOnDownKey );
IsHiresSensor               = ini.get_boolean( IsHiresSensorKey );
SaveAllAlarmShots           = ini.get_boolean( SaveAllAlarmShotsKey      );
SubtractTimeoutFromAutoDown = ini.get_boolean( SubtractTimeoutKey        );
DownTimeAlarmWire           = ini.get_int(     DownTimeAlarmWireKey );
MonitorWire                 = ini.get_int(     MonitorWireKey       );
TurnOffAlarmsOnDown         = ini.get_boolean( TurnOffAlarmsOnDownKey );

UploadRetractTimeout = 0;

/*
----
Logs
---- */
AlsLogIsEnabled          = ini.get_boolean( AlsLogIsEnabledKey );
NetworkLogIsEnabled      = ini.get_boolean( NetworkLogIsEnabledKey );
SentCommandsLogIsEnabled = ini.get_boolean( SentCommandsLogIsEnabledKey );

s.get_log_dir_file_name( 0 );
if ( !s.directory_exists() )
    s.create_directory();

AlsLogFile.get_log_dir_file_name( TEXT("io_log_file.txt") );
ErrorLogFile.get_log_dir_file_name( TEXT("ft2.log") );

ErrorLogTempFile.get_log_dir_file_name( TEXT("ft2copy.log") );
ErrorLogZipFile.get_log_dir_file_name(  TEXT("ft2.zip") );
static NAME_CLASS ErrorLogZipFile;

NetworkLogFile.get_log_dir_file_name( TEXT("network_log_file.txt") );
SentCommandsLogFile.get_log_dir_file_name( TEXT("sent_commands_log_log_file.txt") );

if ( ini.find(UploadRetractTimeoutKey) )
    UploadRetractTimeout = (unsigned) ini.get_ul();

UploadStartShotTimeout = 0;
if ( ini.find(UploadStartShotTimeoutKey) )
    UploadStartShotTimeout = (unsigned) ini.get_ul();

UsingValveTestOutput = boolean_from_ini( SureTrakIniFile, ConfigSection, UseValveTestOutputKey );
UsingNullValveOutput = boolean_from_ini( SureTrakIniFile, ConfigSection, UseNullValveOutputKey );

if ( ini.find(Ft2StartScreenKey) )
    StartScreen = ini.get_string();

/*
-------
Uploads
------- */
NoUploadWire  = ini.get_ul( NoUploadWireKey );
NoUploadWireMask = 0;
if ( NoUploadWire )
    NoUploadWireMask = mask_from_wire( NoUploadWire );

RetractWire = ini.get_ul( RetractWireKey );
if ( RetractWire == NO_WIRE )
    RetractWire = 20;
RetractWireMask = mask_from_wire( RetractWire );

if ( AlsLogIsEnabled )
    {
    t.get_local_time();
    s = TEXT("PROGRAM START ");
    s += t.hhmmss();
    if ( f.open_for_append(AlsLogFile) )
        {
        f.writeline( s.text() );
        f.close();
        }
    }

/*
-------------------------------
Load the monitor status strings
------------------------------- */
NoConnectState   = resource_string( NO_CONNECT_STRING );
NotMonitored     = resource_string( NOT_MONITORED_STRING );
Connected        = resource_string( CONNECTED_STRING );
Connecting       = resource_string( CONNECTING_STRING    );
NotConnected     = resource_string( NOT_CONNECTED_STRING );
ZeroStrokeLength = resource_string( ZERO_STROKE_LENGTH_STRING );
UploadFailed     = resource_string( UPLOAD_FAIL_STRING );
NoResponse       = resource_string( NO_RESPONSE_STRING );
CycleStartString = StringTable.find( TEXT("CYCLE_START") );
SavingString     = StringTable.find( TEXT("SAVING_STRING") );

/*
---------------------------------------
Create an event for the shotsave thread
--------------------------------------- */
ShotSaveEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !ShotSaveEvent )
    resource_message_box( UNABLE_TO_CONTINUE_STRING, SHOTSAVE_EVENT_FAIL_STRING );

register_vtww_opto_control();

/*
------------------------------
Register the main window class
------------------------------ */
wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MyClassName );
wc.lpszMenuName  = NULL;
wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1); //GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

/*
--------------------------------
Register the status window class
-------------------------------- */
wc.lpszClassName = StatusClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) status_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = NULL;
wc.lpszMenuName  = NULL;
wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1); //GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

/*
----------------------------------
Register the terminal window class
---------------------------------- */
wc.lpszClassName = TerminalClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) terminal_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = NULL;
wc.lpszMenuName  = NULL;
wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

s = resource_string( MainInstance, MONITOR_WINDOW_TITLE_STRING );

w = 765;

CreateWindow(
    MyClassName,
    s.text(),
    WS_POPUP,
    0, 100,     // X,y
    w, 90,      // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );

start_shotsave_thread();

GreenBackgroundBrush  = CreateSolidBrush ( GreenColor );
OrangeBackgroundBrush = CreateSolidBrush ( OrangeColor );
RedBackgroundBrush    = CreateSolidBrush ( RedColor );

AutoShotDisplayWindow  = CreateDialog(
   MainInstance,
   TEXT("AUTO_SHOT"),
   MainWindow.handle(),
   (DLGPROC) AutoShotDisplayProc );

AutoShotDisplayWindow.hide();

Ft2TestDialog  = CreateDialog(
   MainInstance,
   TEXT("FT2_TEST_DIALOG"),
   MainWindow.handle(),
   (DLGPROC) ft2_test_dialog_proc );

Ft2TestDialog.hide();

CreateDialog(
    MainInstance,
    TEXT("OUTPUT_LIST_DIALOG"),
    NULL,
    (DLGPROC) output_list_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("HIRES_SENSOR_TEST_DIALOG"),
    NULL,
    (DLGPROC) hires_sensor_test_dialog_proc );

CreateDialog(
    MainInstance,
    TEXT("UPLOAD_ABORTED_DIALOG"),
    NULL,
    (DLGPROC) upload_aborted_dialog_proc );

return TRUE;
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( void )
{
unregister_for_event( DDE_CONFIG_TOPIC,  PW_LEVEL_INDEX      );
unregister_for_event( DDE_ACTION_TOPIC,  PLOT_MIN_INDEX      );
unregister_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX     );
unregister_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX );
client_dde_shutdown();

statics_shutdown();

if ( ShotSaveEvent )
    {
    CloseHandle( ShotSaveEvent );
    ShotSaveEvent = 0;
    }

if ( GreenBackgroundBrush )
    {
    DeleteObject( GreenBackgroundBrush );
    GreenBackgroundBrush = 0;
    }

if ( OrangeBackgroundBrush )
    {
    DeleteObject( OrangeBackgroundBrush );
    OrangeBackgroundBrush = 0;
    }

if ( RedBackgroundBrush )
    {
    DeleteObject( RedBackgroundBrush );
    RedBackgroundBrush = 0;
    }

if ( Dnu )
    {
    delete[] Dnu;
    Dnu = 0;
    }

if ( MainPushButton )
    {
    delete[] MainPushButton;
    MainPushButton = 0;
    }

if ( AddressTbox )
    {
    delete[] AddressTbox;
    AddressTbox = 0;
    }

if ( ConnectedTbox )
    {
    delete[] ConnectedTbox;
    ConnectedTbox = 0;
    }

if ( MachineTbox )
    {
    delete[] MachineTbox;
    MachineTbox = 0;
    }

if ( PartTbox )
    {
    delete[] PartTbox;
    PartTbox = 0;
    }

if ( MonitorStatusTbox )
    {
    delete[] MonitorStatusTbox;
    MonitorStatusTbox = 0;
    }

if ( MyFont != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( MyFont );
    MyFont = INVALID_FONT_HANDLE;
    }

dcurve_shutdown();
skip_shutdown();
shifts_shutdown();
ANALOG_SENSOR::shutdown();
units_shutdown();
shutdown_vtww_opto_control();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG     msg;
BOOL    status;

InitCommonControls();

MainInstance = this_instance;

if ( !init(cmd_show) )
    return 0;

AccelHandle = LoadAccelerators( MainInstance, AccelName );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status && MainWindow.handle() )
        status = TranslateAccelerator( MainWindow.handle(), AccelHandle, &msg );

    if ( !status && AutoShotDisplayWindow.handle() )
        status = IsDialogMessage( AutoShotDisplayWindow.handle(), &msg );

    if ( !status && GlobalParameterDialog.handle() )
        status = IsDialogMessage( GlobalParameterDialog.handle(), &msg );

    if ( !status && ControlModsDialog.handle() )
        status = IsDialogMessage( ControlModsDialog.handle(), &msg );

    if ( !status && SetDacDialog.handle() )
        status = IsDialogMessage( SetDacDialog.handle(), &msg );

    if ( !status && CurrentValuesDialog.handle() )
        status = IsDialogMessage( CurrentValuesDialog.handle(), &msg );

    if ( !status && SetNullDialog.handle() )
        status = IsDialogMessage( SetNullDialog.handle(), &msg );

    if ( !status && ValveTestDialog.handle() )
        status = IsDialogMessage( ValveTestDialog.handle(), &msg );

    if ( !status && Ft2TestDialog.handle() )
        status = IsDialogMessage( Ft2TestDialog.handle(), &msg );

    if ( !status && OutputListDialog.handle() )
        status = IsDialogMessage( OutputListDialog.handle(), &msg );

    if ( !status && TestAlarmOutputsDialog.handle() )
        status = IsDialogMessage( TestAlarmOutputsDialog.handle(), &msg );

    if ( !status && ShutdownStatusDialog.handle() )
        status = IsDialogMessage( ShutdownStatusDialog.handle(), &msg );

    if ( !status && HiresSensorTestDialog.handle() )
        status = IsDialogMessage( HiresSensorTestDialog.handle(), &msg );

    if ( !status && UploadAbortedDialog.handle() )
        status = IsDialogMessage( UploadAbortedDialog.handle(), &msg );

    if ( !status && CompareVariablesDialog.handle() )
        status = IsDialogMessage( CompareVariablesDialog.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
