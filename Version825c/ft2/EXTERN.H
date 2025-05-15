#define BOARD_NUMBER_COLUMN   0
#define TCP_ADDRESS_COLUMN    1
#define CONNECTED_COLUMN      2
#define MACHINE_NAME_COLUMN   3
#define PART_NAME_COLUMN      4
#define MONITOR_STATUS_COLUMN 5
#define NofCols               6

#define ANALOG_VALUE_COUNT   24
#define NOF_OP_STATUS_ANALOGS 16

#define NO_EXTERNAL_INTERFACE    0
#define Rev_B_EXTERNAL_INTERFACE 1
#define Rev_E_EXTERNAL_INTERFACE 2

#define NO_SERVO_AMP       0
#define EXTERNAL_SERVO_AMP 1
#define DIGITAL_SERVO_AMP  2

struct CONTROL_INFO {
    int  id;
    HWND w;
    };

/*
-------------------------------------------------------------------------------
The following is used by shotsave to send the external alarms to board monitor.
index is the parameter index [0,MAX_PARMS). So, if you have one machine
running on one computer the board_index is zero. If parameter 36 is an external
parameter then the index of that parameter is 35. Thus
ExternalParameterAlarms[board_index].index[0] = 35.
ExternalParameterAlarms[board_index].value[0] is the value of that parameter
This is only necessary if the external parameters are not available when I save
the shot file.
------------------------------------------------------------------------------- */
class EXTERNAL_PARAMETER_ALARM_CLASS {
    public:
    int        * index;
    STRING_CLASS machine_name;
    int          n;
    float      * value;

    EXTERNAL_PARAMETER_ALARM_CLASS();
    ~EXTERNAL_PARAMETER_ALARM_CLASS();
    void cleanup();
    bool init( int new_n );
    };

/*
---------------------------------------------------------------------------
The following is used by board_monitor to tell shotsave when a machine with
external_parameters has gotten a CYCLE_START signal. The code for this
is in board_monitor. Board_monitor uses put() to put the next machine name
into the array. Shotsave uses get() to retrieve the next message.
--------------------------------------------------------------------------- */
class RACETRACK_CLASS
    {
    private:
    int            get_index;;
    int            n;
    int            put_index;
    STRING_CLASS * sa;

    public:
    RACETRACK_CLASS();
    ~RACETRACK_CLASS();
    void operator=( const RACETRACK_CLASS & sorc );
    bool setsize( int new_n );
    void cleanup();
    int count();
    bool get( STRING_CLASS & dest );
    bool put( STRING_CLASS & sorc );
    };

#define NOT_SHUTTING_DOWN        0
#define SHUTTING_DOWN_MONITORING 1
#define SHUTTING_DOWN_SHOTSAVE   2
#define SHUTTING_DOWN_REMOTE     3
#define SHUTDOWN_COMPLETE        4

#ifdef _MAIN_

int     NofRows = 1;   /* Number of rows in Grid */

HINSTANCE MainInstance;

WINDOW_CLASS CurrentWindow;
WINDOW_CLASS MainWindow;
WINDOW_CLASS AutoShotDisplayWindow;
WINDOW_CLASS CompareVariablesDialog;
WINDOW_CLASS CurrentDacDialog;
WINDOW_CLASS CurrentValuesDialog;
WINDOW_CLASS GlobalParameterDialog;
WINDOW_CLASS ControlModsDialog;
WINDOW_CLASS SetNullDialog;
WINDOW_CLASS SetDacDialog;
WINDOW_CLASS StatusWindow;
WINDOW_CLASS TerminalWindow;
WINDOW_CLASS TestAlarmOutputsDialog;
WINDOW_CLASS UploadFlashDialog;
WINDOW_CLASS ValveTestDialog;
WINDOW_CLASS HiresSensorTestDialog;
WINDOW_CLASS UploadAbortedDialog;

/*
-------------
Main Controls
------------- */
int  Char_Width  = 14;
int  Line_Height = 14;

CONTROL_INFO * MainPushButton    = 0;
CONTROL_INFO * AddressTbox       = 0;
CONTROL_INFO * ConnectedTbox     = 0;
CONTROL_INFO * MachineTbox       = 0;
CONTROL_INFO * PartTbox          = 0;
CONTROL_INFO * MonitorStatusTbox = 0;

HWND          MainEbox = 0;
LISTBOX_CLASS MainListBox;

HWND CurrentValuesCommStatusTb    = 0;
HWND CurrentValuesMonitorStatusTb = 0;

BOOLEAN TerminalIsVisible = FALSE;
bool    BoardMonitorIsRunning = false;
bool    ShotSaveIsRunning     = false;

BOOLEAN ClearAlarmsEachShot         = FALSE;
BOOLEAN ClearMonitorWireOnDown      = FALSE;
BOOLEAN ClearDownWireFromPopup      = FALSE;
BOOLEAN SaveAllAlarmShots           = FALSE;
BOOLEAN SubtractTimeoutFromAutoDown = FALSE;
BOOLEAN HasDigitalServoAmp          = FALSE;  /* Refers to CurrentBoard */
BOOLEAN IsHiresSensor               = FALSE;
BOOLEAN IsRemoteMonitor             = FALSE;
BOOLEAN TurnOffAlarmsOnDown         = FALSE;

int      DownTimeAlarmWire  = NO_WIRE;
int      MonitorWire        = NO_WIRE;
int      CurrentBoardIndex  = NO_INDEX;

unsigned NoUploadWire       = NO_WIRE;
unsigned NoUploadWireMask   = 0;
unsigned RetractWire        = NO_WIRE;
unsigned RetractWireMask    = 0x080000;

/*
------------------------------------------------------------------------------
This will be initialized by board_monitor and then updated by shotsave.
Shotsave will put a machine name in the ExternalAlarmsRaceTrack when new
alarms are available. Board_monitor should not write to this class except when
creating it.
------------------------------------------------------------------------------ */
EXTERNAL_PARAMETER_ALARM_CLASS * ExternalParameterAlarms    = 0;
int                              NofExternalParameterAlarms = 0;

/*
-------------------------------------------------
The following is used by update_monitor_status
in board_monitor and by need_monitor_brush in ft2
------------------------------------------------- */
STRING_CLASS CycleStartString;

/*
-------------------------------------------------------------------------------
Racetrack used to send cycle start notifications from board_monitor to shotsave
------------------------------------------------------------------------------- */
RACETRACK_CLASS CycleStartRaceTrack;

/*
------------------------------------------------------------------
Racetrack used to send notifictions from shotsave to board_monitor
------------------------------------------------------------------ */
RACETRACK_CLASS ExternalAlarmsRaceTrack;

/*
------------------------------------------------
Machine and part of the currently selected board
------------------------------------------------ */
MACHINE_CLASS CurrentMachine;
PART_CLASS    CurrentPart;

STRING_CLASS ComputerName;
STRING_CLASS NoConnectState;
STRING_CLASS NoResponse;
STRING_CLASS NotMonitored;
STRING_CLASS NotConnected;
STRING_CLASS Connected;
STRING_CLASS Connecting;
STRING_CLASS SavingString;
STRING_CLASS ZeroStrokeLength;
STRING_CLASS UploadFailed;

/*
----
Logs
---- */
BOOLEAN AlsLogIsEnabled          = FALSE;
BOOLEAN ErrorLogIsEnabled        = FALSE;
BOOLEAN NetworkLogIsEnabled      = FALSE;
BOOLEAN SentCommandsLogIsEnabled = FALSE;

NAME_CLASS AlsLogFile;
NAME_CLASS ErrorLogFile;
NAME_CLASS NetworkLogFile;
NAME_CLASS SentCommandsLogFile;

TCHAR AlsLogIsEnabledKey[]          = TEXT( "NewLog" );
TCHAR NetworkLogIsEnabledKey[]      = TEXT( "NetWorkLogFileEnabled" );
TCHAR SentCommandsLogIsEnabledKey[] = TEXT( "LogSentCommands" );

/*
------------
Ft2 Timeouts
------------ */
unsigned UploadRetractTimeout    = 0;
unsigned UploadStartShotTimeout  = 0;

/*
----------------
Connected states
---------------- */
unsigned NO_FT2_CONNECT_STATE = 0;
unsigned NOT_CONNECTED_STATE  = 1;
unsigned NOT_MONITORED_STATE  = 2;
unsigned CONNECTING_STATE     = 4;
unsigned CONNECTED_STATE      = 8;
unsigned ZERO_STROKE_LENGTH_STATE = 16;
unsigned UPLOAD_WAS_ABORTED_STATE = 32;

ANALOG_SENSOR AnalogSensor;
int           HistoryShotCount = 50;

int   ShutdownState = NOT_SHUTTING_DOWN;

TCHAR ConfigSection[]             = TEXT( "Config" );
TCHAR CurrentValuesSection[]      = TEXT( "CurrentValues" );

TCHAR CommonResourceFile[]        = TEXT( "common" );
TCHAR DisplayIniFile[]            = TEXT( "display.ini" );
TCHAR EditPartResourceFile[]      = TEXT( "editpart" );
TCHAR FtiiIniFile[]               = TEXT( "ftii.ini" );
TCHAR MonallIniFile[]             = TEXT( "monall.ini" );
TCHAR MyMenuFile[]                = TEXT( "ft2.mnu" );
TCHAR MyResourceFile[]            = TEXT( "ft2" );
TCHAR SureTrakIniFile[]           = TEXT( "suretrak.ini" );
TCHAR TerminalMenuFile[]          = TEXT( "ft2terminal.mnu" );
TCHAR VisiTrakIniFile[]           = TEXT( "visitrak.ini" );

TCHAR ClearAlarmsEachShotKey[]    = TEXT( "ClearAlarmsEachShot" );
TCHAR ClearDownWireFromPopupKey[] = TEXT( "ClearDownWireFromPopup" );
TCHAR ClearMonitorWireOnDownKey[] = TEXT( "ClearMonitorWireOnDown" );
TCHAR CurrentChannelModeKey[]     = TEXT( "CurrentChannelMode" );
TCHAR CurrentLanguageKey[]        = TEXT( "CurrentLanguage" );
TCHAR DownTimeAlarmWireKey[]      = TEXT( "DownTimeAlarmWire" );
TCHAR ExitEnableLevelKey[]        = TEXT( "ExitEnableLevel" );
TCHAR FlashVersionKey[]           = TEXT( "FlashVersion" );
TCHAR Ft2F10LevelKey[]            = TEXT( "Ft2F10Level" );
TCHAR Ft2F2LevelKey[]             = TEXT( "Ft2F2Level" );
TCHAR Ft2F3LevelKey[]             = TEXT( "Ft2F3Level" );
TCHAR Ft2F4LevelKey[]             = TEXT( "Ft2F4Level" );
TCHAR Ft2F5LevelKey[]             = TEXT( "Ft2F5Level" );
TCHAR Ft2F6LevelKey[]             = TEXT( "Ft2F6Level" );
TCHAR Ft2F7LevelKey[]             = TEXT( "Ft2F7Level" );
TCHAR Ft2F8LevelKey[]             = TEXT( "Ft2F8Level" );
TCHAR Ft2F9LevelKey[]             = TEXT( "Ft2F9Level" );
TCHAR Ft2StartScreenKey[]         = TEXT( "Ft2StartScreen" );
TCHAR HavePressureControlKey[]    = TEXT( "HavePressureControl" );
TCHAR HaveSureTrakKey[]           = TEXT( "HaveSureTrak" );
TCHAR IsHiresSensorKey[]          = TEXT( "IsHiresSensor" );
TCHAR LastNKey[]                  = TEXT( "LastN" );
TCHAR MonitorWireKey[]            = TEXT( "MonitorWire" );

TCHAR NoUploadWireKey[]           = TEXT( "NoUploadWire" );    /* Use this wire instead of cycle start message */
TCHAR RetractWireKey[]            = TEXT( "RetractWire" );       /* Use this wire instead of At home message */
TCHAR SaveAllAlarmShotsKey[]      = TEXT( "SaveAllAlarmShots" );
TCHAR SavepartExeName[]           = TEXT( "savepart.exe" );
TCHAR ServiceToolsGrayLevelKey[]  = TEXT( "ServiceToolsGrayLevel" );
TCHAR SubtractTimeoutKey[]        = TEXT( "SubtractTimeout" );
TCHAR TurnOffAlarmsOnDownKey[]    = TEXT( "TurnOffAlarmsOnDown" );
TCHAR UploadRetractTimeoutKey[]   = TEXT( "UploadRetractTimeout" );
TCHAR UploadStartShotTimeoutKey[] = TEXT( "UploadStartShotTimeout" );
TCHAR UseNullValveOutputKey[]     = TEXT( "UseNullValveOutput" );
TCHAR UseValveTestOutputKey[]     = TEXT( "UseValveTestOutput" );

TCHAR StartControlPgmCmd[]   = TEXT(".G");
TCHAR EnableShotXmitCmd[]    = TEXT( "EXMIT" );
TCHAR DisableShotXmitCmd[]   = TEXT( "DXMIT" );
int   ShotXmitCmdLen         = 5;

TCHAR ShotSaveCompleteMessage[]  = TEXT( "ShotSaveComplete" );
int   ShotSaveCompleteMessageLen = 16;
TCHAR START_OF_UPLOAD[]          = TEXT( "START_OF_UPLOAD" );
TCHAR END_OF_UPLOAD[]            = TEXT( "END_OF_UPLOAD"   );
TCHAR RESET_MACHINE_CMD[]        = TEXT( "RESET_MACHINE"   );
TCHAR CLEAR_NO_UPLOAD_FLAG_CMD[] = TEXT( "CLEAR_NO_UPLOAD_FLAG" );

TCHAR DONT_SEND_OP_DATA_PACKETS[]   = TEXT( "OC3=0" );
TCHAR SEND_OP_DATA_PACKETS[]        = TEXT( "OC3=2" );
TCHAR CLEAR_CONTROL_FILE_DATE_CMD[] = TEXT( "V447=0" );

COLORREF NormalCellColor     = RGB( 255, 255, 255 );
COLORREF GreenColor          = RGB(   0, 255,   0 );
COLORREF OrangeColor         = RGB( 255,  50,  35 );
COLORREF RedColor            = RGB( 255,   0,   0 );

FTII_OP_STATUS_DATA CurrentOpStatus;
bool                CurrentOpStatusIsEmpty = true;
unsigned            CurrentAnalogValue[ANALOG_VALUE_COUNT] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

BLOCK_MESSAGE_CLASS StepMessage;

TCHAR ShotparmLogFile[] = TEXT( "shotparm-monall.log" );

#else

extern EXTERNAL_PARAMETER_ALARM_CLASS * ExternalParameterAlarms;
extern int                              NofExternalParameterAlarms;

extern int NofRows;

extern HINSTANCE MainInstance;

#ifdef _WINDOW_CLASS_
extern WINDOW_CLASS CurrentWindow;
extern WINDOW_CLASS MainWindow;
extern WINDOW_CLASS AutoShotDisplayWindow;
extern WINDOW_CLASS ButtonDialog;
extern WINDOW_CLASS CompareVariablesDialog;
extern WINDOW_CLASS CurrentDacDialog;
extern WINDOW_CLASS CurrentValuesDialog;
extern WINDOW_CLASS GlobalParameterDialog;
extern WINDOW_CLASS ControlModsDialog;
extern WINDOW_CLASS SetNullDialog;
extern WINDOW_CLASS SetDacDialog;
extern WINDOW_CLASS TestAlarmOutputsDialog;
extern WINDOW_CLASS UploadFlashDialog;
extern WINDOW_CLASS ValveTestDialog;
#endif

extern int  Char_Width;
extern int  Line_Height;

extern CONTROL_INFO * MainPushButton;
extern CONTROL_INFO * AddressTbox;
extern CONTROL_INFO * ConnectedTbox;
extern CONTROL_INFO * MachineTbox;
extern CONTROL_INFO * PartTbox;
extern CONTROL_INFO * MonitorStatusTbox;

extern HWND          MainEbox;

#ifdef _LISTBOX_CLASS_
extern LISTBOX_CLASS MainListBox;
#endif

extern BOOLEAN AlsLogIsEnabled;
extern BOOLEAN ErrorLogIsEnabled;
extern BOOLEAN NetworkLogIsEnabled;
extern BOOLEAN SentCommandsLogIsEnabled;

extern NAME_CLASS AlsLogFile;
extern NAME_CLASS ErrorLogFile;
extern NAME_CLASS SentCommandsLogFile;
extern NAME_CLASS NetworkLogFile;

extern TCHAR AlsLogIsEnabledKey[];
extern TCHAR NetworkLogIsEnabledKey[];
extern TCHAR SentCommandsLogIsEnabledKey[];

extern HWND CurrentValuesCommStatusTb;
extern HWND CurrentValuesMonitorStatusTb;

extern BOOLEAN TerminalIsVisible;
extern bool    BoardMonitorIsRunning;
extern bool    ShotSaveIsRunning;

extern BOOLEAN ClearAlarmsEachShot;
extern BOOLEAN ClearMonitorWireOnDown;
extern BOOLEAN ClearDownWireFromPopup;
extern BOOLEAN SaveAllAlarmShots;
extern BOOLEAN SubtractTimeoutFromAutoDown;
extern BOOLEAN HasDigitalServoAmp;
extern BOOLEAN IsHiresSensor;
extern BOOLEAN IsRemoteMonitor;
extern BOOLEAN TurnOffAlarmsOnDown;

extern int      DownTimeAlarmWire;
extern int      MonitorWire;
extern int      CurrentBoardIndex;

extern unsigned NoUploadWire;
extern unsigned NoUploadWireMask;
extern unsigned RetractWire;
extern unsigned RetractWireMask;

extern STRING_CLASS CycleStartString;

extern RACETRACK_CLASS CycleStartRaceTrack;
extern RACETRACK_CLASS ExternalAlarmsRaceTrack;

extern STRING_CLASS GetShotNameCommand;
extern STRING_CLASS ShotNameFile;

#ifdef _MACHINE_CLASS_
extern MACHINE_CLASS CurrentMachine;
#endif

#ifdef _PART_CLASS_
extern PART_CLASS CurrentPart;
#endif

#ifdef _STRING_CLASS_
extern STRING_CLASS ComputerName;
extern STRING_CLASS NoConnectState;
extern STRING_CLASS NoResponse;
extern STRING_CLASS NotMonitored;
extern STRING_CLASS NotConnected;
extern STRING_CLASS Connected;
extern STRING_CLASS Connecting;
extern STRING_CLASS ZeroStrokeLength;
extern STRING_CLASS UploadFailed;
extern STRING_CLASS SavingString;
#endif

extern unsigned UploadRetractTimeout;
extern unsigned UploadStartShotTimeout;

extern unsigned NO_FT2_CONNECT_STATE;
extern unsigned NOT_CONNECTED_STATE;
extern unsigned NOT_MONITORED_STATE;
extern unsigned CONNECTING_STATE;
extern unsigned CONNECTED_STATE;
extern unsigned ZERO_STROKE_LENGTH_STATE;
extern unsigned UPLOAD_WAS_ABORTED_STATE;

#ifdef _ANALOG_SENSOR_
extern ANALOG_SENSOR AnalogSensor;
#endif

extern int HistoryShotCount;
extern int ShutdownState;

extern TCHAR ConfigSection[];
extern TCHAR CurrentValuesSection[];

extern TCHAR FtiiIniFile[];
extern TCHAR MonallIniFile[];
extern TCHAR SureTrakIniFile[];
extern TCHAR VisiTrakIniFile[];

extern TCHAR ClearDownWireFromPopupKey[];
extern TCHAR ClearMonitorWireOnDownKey[];
extern TCHAR CurrentChannelModeKey[];
extern TCHAR DownTimeAlarmWireKey[];
extern TCHAR ExitEnableLevelKey[];
extern TCHAR FlashVersionKey[];
extern TCHAR HavePressureControlKey[];
extern TCHAR HaveSureTrakKey[];
extern TCHAR IsHiresSensorKey[];
extern TCHAR LastNKey[];
extern TCHAR MonitorWireKey[];
extern TCHAR NoUploadWireKey[];
extern TCHAR RetractWireKey[];
extern TCHAR SaveAllAlarmShotsKey[];
extern TCHAR SavepartExeName[];
extern TCHAR SubtractTimeoutKey[];
extern TCHAR TurnOffAlarmsOnDownKey[];
extern TCHAR UseNullValveOutputKey[];
extern TCHAR UseValveTestOutputKey[];

extern TCHAR StartControlPgmCmd[];
extern TCHAR EnableShotXmitCmd[];
extern TCHAR DisableShotXmitCmd[];
extern int   ShotXmitCmdLen;

extern TCHAR ShotSaveCompleteMessage[];
extern int   ShotSaveCompleteMessageLen;
extern TCHAR START_OF_UPLOAD[];
extern TCHAR END_OF_UPLOAD[];
extern TCHAR RESET_MACHINE_CMD[];
extern TCHAR CLEAR_NO_UPLOAD_FLAG_CMD[];
extern TCHAR DONT_SEND_OP_DATA_PACKETS[];
extern TCHAR SEND_OP_DATA_PACKETS[];

extern COLORREF NormalCellColor;
extern COLORREF GreenColor;
extern COLORREF OrangeColor;
extern COLORREF RedColor;

#ifdef _FTII_SHOT_DATA_
extern FTII_OP_STATUS_DATA CurrentOpStatus;
extern bool                CurrentOpStatusIsEmpty;
#endif

#ifdef _BLOCK_MESSAGE_CLASS_
extern BLOCK_MESSAGE_CLASS StepMessage;
#endif

extern TCHAR ShotparmLogFile[];

extern unsigned CurrentAnalogValue[20];

#endif

