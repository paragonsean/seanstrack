#include <windows.h>
#include <commctrl.h>
#include <math.h>

#include "..\include\ugctrl.h"

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\array.h"
#include "..\include\asensor.h"
#include "..\include\chaxis.h"
#include "..\include\color.h"
#include "..\include\dbclass.h"
#include "..\include\external_parameter.h"
#include "..\include\fontclas.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\structs.h"
#include "..\include\computer.h"
#include "..\include\limit_switch_class.h"
#include "..\include\machine.h"
#include "..\include\multipart_runlist.h"
#include "..\include\param.h"
#include "..\include\param_index_class.h"
#include "..\include\part.h"
#include "..\include\marks.h"
#include "..\include\fileclas.h"
#include "..\include\ftanalog.h"
#include "..\include\ftcalc.h"
#include "..\include\stparam.h"
#include "..\include\stpres.h"
#include "..\include\stringtable.h"
#include "..\include\stsetup.h"
#include "..\include\plookup.h"
#include "..\include\plotclas.h"
#include "..\include\rectclas.h"
#include "..\include\subs.h"
#include "..\include\shotname.h"
#include "..\include\shot_name_reset.h"
#include "..\include\textlen.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\v5help.h"
#include "..\include\warmupcl.h"
#include "..\include\wclass.h"
#include "..\include\workclas.h"

#include "..\include\ftii.h"
#include "..\include\ftii_file.h"

#define _MAIN_
#include "PartData.h"
#include "resource.h"
#include "resrc1.h"

#include "..\include\chars.h"
#include "..\include\events.h"

#define OPEN_CHOICE          102

#define NO_TAB              -1
#define BASIC_SETUP_TAB      0
#define ADVANCED_SETUP_TAB   1
#define PARAMETER_LIMITS_TAB 2
#define USER_DEFINED_TAB     3
#define PROFILE_MARKS_TAB    4
#define FASTRAK_CHANNELS_TAB 5
#define SURETRAK_PROFILE_TAB 6   // If there is no suretrak then tab 6 = setup sheet
#define PRESSURE_CONTROL_TAB 7   // If there is suretrak control but no pressure control then tab 7 is the setup sheet
#define SETUP_SHEET_TAB      8
#define TAB_COUNT            9

#define MAX_TAB_LEN   25
#define HAS_PART_PREFIX_LEN 3

struct HELP_ENTRY
    {
    UINT id;
    DWORD context;
    };

struct FULL_HELP_ENTRY
    {
    UINT  id;
    UINT  type;
    DWORD context;
    };

struct MY_WINDOW_INFO
        {
        DLGPROC procptr;
        HWND    tab_rb;
        HWND    dialogwindow;
        short   password_level;
        };

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

extern GENERIC_LIST_CLASS MachineList;

HACCEL  AccelHandle;
TCHAR   AccelName[] = TEXT("EDITPART_HOT_KEYS");

HINSTANCE MainInstance;
HWND      MainWindow                  = 0;

HWND      MainDialogWindow            = 0;
HWND      ParamDialogWindow           = 0;
HWND      AddPartDialogWindow         = 0;
HWND      CopyToDialogWindow          = 0;
HWND      CopyMultipleDialogWindow    = 0;
HWND      AddMarksDialogWindow        = 0;
HWND      SuretrakProfileWindow       = 0;
HWND      ExtendedChannelsDialog      = 0;
HWND      ShotEventsDialog            = 0;

FONT_LIST_CLASS FontList;
LISTBOX_CLASS   ParamLimitLb;

/*
----------------------------------------------------
This is used to tell the show_percent_error routine
not to show anything because I am toggling percent.
---------------------------------------------------- */
BOOLEAN TogglingProportionalValve = FALSE;

/*
--------
SureTrak
-------- */
static BOOLEAN                 SureTrakHasChanged = FALSE;
static BOOLEAN                 HaveSureTrakPlot = FALSE;
static WINDOW_CLASS            SureTrakPlotWindow;
static PLOT_CLASS              SureTrakPlot;
static SURETRAK_SETUP_CLASS    SureTrakSetup;
       SURETRAK_PARAM_CLASS    SureTrakParam;
static PRESSURE_CONTROL_CLASS  PressureControl;
static WARMUP_CLASS            Warmup;
static BOOLEAN                 LoadingSureTrakSteps;
static BOOLEAN                 LoadingPressureControl;
static BOOLEAN                 LimitSwitchDescriptionsHaveChanged = FALSE;

static const unsigned HMI_NO_MESSAGE           = 0;
static const unsigned HMI_CREATE_PART_MESSAGE  = 1;
static const unsigned HMI_SHOW_PART_MESSAGE    = 2;
static const unsigned HMI_MONITOR_PART_MESSAGE = 4;

static unsigned HMIMessageType = HMI_NO_MESSAGE;

/*
--------------------
New Parameter Dialog
-------------------- */
HWND NewParameterDialogWindow = 0;

/*
--------------
Ftii Constants
-------------- */
static const int FTII_VACUUM_WAIT_INPUT = 58;
static const int FTII_LOW_IMPACT_INPUT  = 60;

/*
----
Bits
---- */
static const unsigned Bit31 = 0x80000000;
static const unsigned Bit30 = 0x40000000;

static const COLORREF BlackColor = RGB( 0,   0, 0 );
static const COLORREF RedColor   = RGB( 255, 0, 0 );

BOOLEAN IsOfficeWorkstation = FALSE;  // Set by read_machines_and_parts()

/*
--------------------------------
These are initialized in startup
-------------------------------- */
static       COLORREF EboxBackgroundColor = 0;
static       HBRUSH   EboxBackgroundBrush = 0;

static const int32    ST_POS_AXIS_ID  = 1;
static const int32    ST_VEL_AXIS_ID  = 2;
static const int32    ST_TIME_AXIS_ID = 1;
static const int32    ST_PRES_AXIS_ID = 2;

static HFONT MyFont      = INVALID_FONT_HANDLE;
static HFONT StaticsFont = INVALID_FONT_HANDLE;

/*
-------------
Hmi Interface
------------- */
static TCHAR HmiCreateString[]          = TEXT( "CREATE" );
static TCHAR HmiMonitorString[]         = TEXT( "MONITOR" );
static TCHAR HmiShowString[]            = TEXT( "SHOW" );

const int HmiCreateLen                  = sizeof(HmiCreateString)/sizeof(TCHAR);
STRING_CLASS HmiOldPart;
STRING_CLASS HmiNewPart;

TCHAR     ChooseWireResourceFile[]      = TEXT( "choose_wire" );
TCHAR     CommonResourceFile[]          = TEXT( "common" );
TCHAR     EditPartResourceFile[]        = TEXT( "editpart" );
TCHAR     Ft2ClassName[]                = TEXT( "FASTRAK2" );
TCHAR     MyClassName[]                 = TEXT( "EditPart" );
TCHAR     SureTrakPlotWindowClassName[] = TEXT( "SureTrakPlotWindow" );

TCHAR     DisplayIniFile[]              = TEXT( "display.ini" );
TCHAR     EditPartCommandLineFile[]     = TEXT( "editpart_command_line.txt" );
TCHAR     EditPartIniFile[]             = TEXT( "editpart.ini" );
TCHAR     EmachineIniFile[]             = TEXT( "emachine.ini" );
TCHAR     SureTrakIniFile[]             = TEXT( "suretrak.ini" );
TCHAR     VisiTrakIniFile[]             = TEXT( "visitrak.ini" );

TCHAR     ConfigSection[]               = TEXT( "Config" );
TCHAR     MultipartSetupSection[]       = TEXT( "Multipart" );

TCHAR     ChangeExternalShotNameLevelKey[]  = TEXT( "ChangeExternalShotNameLevel" );
TCHAR     ChangeShotNameLevelKey[]          = TEXT( "ChangeShotNameLevel" );
TCHAR     ChangeShotResetLevelKey[]         = TEXT( "ChangeShotResetLevel" );
TCHAR     CurrentLanguageKey[]              = TEXT( "CurrentLanguage" );
TCHAR     CurrentTabKey[]                   = TEXT( "CurrentTab" );
TCHAR     HavePressureControlKey[]          = TEXT( "HavePressureControl" );
TCHAR     HaveSureTrakKey[]                 = TEXT( "HaveSureTrak" );
TCHAR     IsoPageOneKey[]                   = TEXT( "IsoPageOne" );
TCHAR     IsoPageTwoKey[]                   = TEXT( "IsoPageTwo" );
TCHAR     LastMachineKey[]                  = TEXT( "LastMachine" );
TCHAR     LastPartKey[]                     = TEXT( "LastPart" );
TCHAR     LoadCurrentPartWhenRunKey[]       = TEXT( "LoadCurrentPartWhenRun" );
TCHAR     MonitorLogFileKey[]               = TEXT( "MonitorLogFile" );
TCHAR     MonitorLogFormatKey[]             = TEXT( "MonitorLogFormat" );
TCHAR     ParameterTabEditLevelKey[]        = TEXT( "ParameterTabEditLevel" );
TCHAR     PfsvDefSmoothNKey[]               = TEXT( "PfsvDefSmoothN");
TCHAR     PfsvDefSmoothMultiplierKey[]      = TEXT( "PfsvDefSmoothMultiplier");
TCHAR     PrintParametersKey[]              = TEXT( "PrintParameters" );
TCHAR     SaveAllPartsLevelKey[]            = TEXT( "SaveAllPartsLevel" );
TCHAR     SaveChangesLevelKey[]             = TEXT( "SaveChangesLevel" );
TCHAR     ShowSetupSheetKey[]               = TEXT( "ShowSetupSheet" );
TCHAR     SetupSheetLevelKey[]              = TEXT( "SetupSheetLevel" );
TCHAR     EditMaOnOtherCoKey[]              = TEXT( "EditMaOnOtherCoLevel" );
TCHAR     EditOtherComputerKey[]            = TEXT( "EditOtherComputerLevel" );
TCHAR     EditOtherCurrentPartKey[]         = TEXT( "EditOtherCurrentPartLevel" );
TCHAR     EditOtherSuretrakKey[]            = TEXT( "EditOtherSuretrakLevel" );
TCHAR     EditOtherCurrentSuretrakKey[]     = TEXT( "EditOtherCurrentSuretrakLevel" );
TCHAR     EditPressureSetupLevelKey[]       = TEXT( "EditPressureSetupLevel" );
TCHAR     EditPressureProfileLevelKey[]     = TEXT( "EditPressureProfileLevel" );
TCHAR     HideSureTrakLimitSwitchWiresKey[] = TEXT("HideSureTrakLimitSwitchWires");
TCHAR     VelocityChangesLevelKey[]         = TEXT( "VelocityChangesLevel" );
TCHAR     WorkstationChangeDisableKey[]     = TEXT( "WorkstationEnable" );  /* Deliberately obscure */

MACHINE_CLASS      CurrentMachine;
PART_CLASS         CurrentPart;
PARAMETER_CLASS    CurrentParam;
PARAM_INDEX_CLASS  CurrentSort;
MARKS_CLASS        CurrentMarks;
FTANALOG_CLASS     CurrentFtAnalog;
FTCALC_CLASS       CurrentFtCalc;
LIMIT_SWITCH_CLASS CurrentLimitSwitch;
STRINGTABLE_CLASS  StringTable;

short           ChangeExternalShotNameLevel    = SYSTEM_PASSWORD_LEVEL;
short           ChangeShotNameLevel            = SYSTEM_PASSWORD_LEVEL;
short           ChangeShotResetLevel           = SYSTEM_PASSWORD_LEVEL;
short           CurrentSelectedMark            = 0;
short           CurrentPasswordLevel           = 5;
short           DefaultPasswordLevel           = 5;
short           EditMaOnOtherCoLevel           = 5;
short           EditOtherComputerLevel         = EDIT_OTHER_COMPUTER_LEVEL;
short           EditOtherCurrentPartLevel      = EDIT_OTHER_CURRENT_PART_LEVEL;
short           EditOtherSuretrakLevel         = EDIT_OTHER_COMPUTER_LEVEL;
short           EditOtherCurrentSuretrakLevel  = EDIT_OTHER_CURRENT_SURETRAK_LEVEL;
short           EditPressureSetupLevel         = ENGINEER_PASSWORD_LEVEL;
short           EditPressureProfileLevel       = ENGINEER_PASSWORD_LEVEL;
short           SaveChangesLevel               = ENGINEER_PASSWORD_LEVEL;
short           OriginalAnalogSensor[MAX_FT2_CHANNELS];  /* I fill this when I load the part, check when I save */
BOOLEAN         HaveCurrentPart                = FALSE;
BOOLEAN         HavePressureControl            = FALSE;
BOOLEAN         HaveSureTrakControl            = FALSE;
BOOLEAN         HideSureTrakLimitSwitchWires   = FALSE;
BOOLEAN         ShowEofFS                      = FALSE;
BOOLEAN         ShowSetupSheet                 = TRUE;
BOOLEAN         WorkstationChangeDisable       = FALSE;

static TCHAR * DialogName[TAB_COUNT] = {
              TEXT("BASIC_SETUP"),
              TEXT("ADVANCED_SETUP"),
              TEXT("PARAMETER_LIMITS"),
              TEXT("USER_DEFINED_POSITIONS"),
              TEXT("PROFILE_MARKS"),
              TEXT("FASTRAK_CHANNELS"),
              TEXT("SURETRAK_PROFILE"),
              TEXT("PRESSURE_CONTROL_PROFILE"),
              TEXT("SETUP_SHEET_DIALOG")
              };

static MY_WINDOW_INFO WindowInfo[TAB_COUNT];
static int            CurrentDialogNumber = NO_TAB;
static int            AlarmIndex[NOF_ALARM_LIMIT_TYPES] = { WARNING_MIN, WARNING_MAX, ALARM_MIN, ALARM_MAX };

/*
----------------------------------------------------------
When the main screen initializes I use this to set the tab
---------------------------------------------------------- */
static int StartupTab = BASIC_SETUP_TAB;

/*
-------------------------------------------------------------------
If this is true I need to make sure channels 5 and 7 are -10 to +10
This is only used when upgrading from monitor only to st2.
------------------------------------------------------------------- */
static BOOLEAN NeedToCheckChannels5and7 = FALSE;

/*
--------------------------------------
Basic Setup Distance Buttons and Units
-------------------------------------- */
static short   NofDistanceButtons    = 3;
static int     DistanceButton[]      = { INCHES_BUTTON, MM_BUTTON, CM_BUTTON };
static short   DistanceUnits[]       = { INCH_UNITS,    MM_UNITS,  CM_UNITS  };
static BOOLEAN DistanceButtonState[] = {FALSE, FALSE, FALSE };

/*
--------------------------------------
Basic Setup Velocity Buttons and Units
-------------------------------------- */
static int     NofVelocityButtons    = 4;
static int     VelocityButton[]      = { IPS_BUTTON, FPM_BUTTON, CMPS_BUTTON, MPS_BUTTON };
static short   VelocityUnits[]       = { IPS_UNITS,  FPM_UNITS,  CMPS_UNITS,  MPS_UNITS  };
static BOOLEAN VelocityButtonState[] = { FALSE, FALSE, FALSE, FALSE };

/*
---------------------------------------------------------
Ramp Goose Button / MS Units buttons for Pressure Control
--------------------------------------------------------- */
static int GooseBox[3] = {PC_GOOSE_1_XBOX, PC_GOOSE_2_XBOX, PC_GOOSE_3_XBOX };
static int MsBox[3]    = {PC_1_MS_UNITS_TB, PC_2_MS_UNITS_TB, PC_3_MS_UNITS_TB };

/*
--------------------------------------------------------
A list of modified edit controls on the suretrak profile
-------------------------------------------------------- */
static WINDOW_ARRAY_CLASS StModList;
static STRING_CLASS       SaveChangesTitle;
static short              VelocityChangesLevel      = NO_PASSWORD_LEVEL;
static BOOLEAN            NeedToCallVelocityChanges = FALSE;

/*
----------------------------------------------------------------------------
Normally hidden text box to tell operator there are unsaved ST param changes
---------------------------------------------------------------------------- */
static WINDOW_CLASS HaveStModTbox;

/*
---------------------------------
Static_text_parameter_dialog_proc
--------------------------------- */
static int StaticTextParameterNumber;

/*
-------------------------------------------------------------------------
The number of editable fastrak analog channels is 4 for the fastrak board
and 8 for the ftii. I set this when I load the CurrentMachine.
------------------------------------------------------------------------- */
int Nof_Editable_Channels = MAX_EDITABLE_FT_CHANNELS;

/*
---------------------------------
Restricted Entry Screen Variables
--------------------------------- */
TCHAR IsEnabledKey[]          = TEXT( "IsEnabled" );
TCHAR ScreenNameKey[]         = TEXT( "ScreenName" );
TCHAR EditableIdsKey[]        = TEXT( "EditableIds" );
TCHAR PositionOnLastChanged[] = TEXT( "PositionOnLastChanged" );
TCHAR ExitWhenDoneKey[]       = TEXT( "ExitWhenDone" );
TCHAR PasswordLevelKey[]      = TEXT( "PasswordLevel" );
TCHAR LastIdKey[]             = TEXT( "LastId" );
TCHAR CurrentPartOnlyKey[]    = TEXT( "CurrentPartOnly" );

class RESTRICTED_PART_ENTRY {
    public:
    BOOLEAN         is_enabled;
    STRING_CLASS    screen_name;
    INT_ARRAY_CLASS editable_ids;
    BOOLEAN         position_on_last_changed;
    BOOLEAN         exit_when_done;
    BOOLEAN         current_part_only;
    short           password_level;
    int             last_id;
    RESTRICTED_PART_ENTRY();
   ~RESTRICTED_PART_ENTRY();
    void            cleanup();
    BOOLEAN         get();
    };

RESTRICTED_PART_ENTRY RestrictedPartEntry;

/*
-----------------
MUTIPART_LB_CLASS
----------------- */
class MULTIPART_LB_ENTRY {
    public:
    STRING_CLASS selection_number;
    STRING_CLASS part_name;
    STRING_CLASS wire_list;
    MULTIPART_LB_ENTRY();
    ~MULTIPART_LB_ENTRY();
    bool lb_line( STRING_CLASS & dest );
    void operator=( const MULTIPART_LB_ENTRY & sorc );
    };

class MULTIPART_LB_CLASS {
    public:
    MULTIPART_LB_ENTRY * array;
    int                  count;
    MULTIPART_LB_CLASS();
    ~MULTIPART_LB_CLASS();
    void cleanup();
    bool init( int nof_wires );
    void operator=( const MULTIPART_LB_CLASS & sorc );
    MULTIPART_LB_ENTRY & operator[]( int i );
    bool read();
    bool write();
    };

/*
------------------------------------------------------------------
Global multipart runlist for use by MULTIPART_RUNLIST_SETUP_DIALOG
------------------------------------------------------------------ */
MULTIPART_LB_CLASS MultipartLbClass;

/*
---------------
Printer Globals
--------------- */
HFONT PrinterFont = 0;

struct ADD_MARKS_ENTRY
    {
    TCHAR       parameter_type;
    short       parameter_number;
    };

static  ADD_MARKS_ENTRY AddMarksList[MAX_MARKS];
static  short   NofAddMarks;
static  WNDPROC OldEditProc;

static TCHAR LowerXString[]  = TEXT("x");
static TCHAR PercentString[] = TEXT("%");
static TCHAR OneString[]     = TEXT("1");
static TCHAR ZeroString[]    = TEXT("0");

BOOL CALLBACK calculator_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK ParamDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void  load_new_parameter( int new_parameter_index );
int32 draw_vertical_string( HDC dc, TCHAR * sorc, int32 x, int32 y );
void  register_choose_wire_control();
static void refresh_velocity_control_plot( void );
static void refresh_pressure_control_plot( void );
static void load_extended_channels();
static void monitor_this_part();
static void save_changes();
static void exit_restricted_part_entry_screen();

/***********************************************************************
*                         RESTRICTED_PART_ENTRY                        *
*                               cleanup()                              *
***********************************************************************/
void RESTRICTED_PART_ENTRY::cleanup()
{
screen_name.empty();
editable_ids.cleanup();
is_enabled               = FALSE;
position_on_last_changed = FALSE;
exit_when_done           = FALSE;
current_part_only        = FALSE;
password_level           = NO_PASSWORD_LEVEL;
last_id                  = 0;
}

/***********************************************************************
*                         RESTRICTED_PART_ENTRY                        *
***********************************************************************/
RESTRICTED_PART_ENTRY::RESTRICTED_PART_ENTRY()
{
is_enabled               = FALSE;
position_on_last_changed = FALSE;
exit_when_done           = FALSE;
current_part_only        = FALSE;
password_level           = NO_PASSWORD_LEVEL;
last_id                  = 0;
}

/***********************************************************************
*                        ~RESTRICTED_PART_ENTRY                        *
***********************************************************************/
RESTRICTED_PART_ENTRY::~RESTRICTED_PART_ENTRY()
{
}

/***********************************************************************
*                         RESTRICTED_PART_ENTRY                        *
                                   get
***********************************************************************/
BOOLEAN RESTRICTED_PART_ENTRY::get()
{
int        i;
INI_CLASS  ini;
NAME_CLASS s;

cleanup();

s.get_local_ini_file_name( RESTRICTED_PART_ENTRY_SCREEN_INI_FILE );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.exists() )
    {
    if ( ini.find(IsEnabledKey) )
        is_enabled = ini.get_boolean();

    if ( ini.find(ScreenNameKey) )
        screen_name = ini.get_string();

    if ( ini.find(EditableIdsKey) )
        {
        s = ini.get_string();
        i = 0;
        while ( s.next_field() )
            {
            editable_ids[i] = s.int_value();
            i++;
            }
        }

    if ( ini.find(PositionOnLastChanged) )
        position_on_last_changed = ini.get_boolean();

    if ( ini.find(ExitWhenDoneKey) )
        exit_when_done = ini.get_boolean();

    if ( ini.find(CurrentPartOnlyKey) )
        current_part_only = ini.get_boolean();

    if ( ini.find(PasswordLevelKey) )
        password_level = (short) ini.get_int();

    if ( ini.find(LastIdKey) )
        last_id = ini.get_int();
    }

if ( screen_name.isempty() || editable_ids.count()<1 )
    is_enabled = FALSE;

return is_enabled;
}

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
RESOURCE_TEXT_ENTRY rsn[] = {
    { THIS_IS_CURRENT_PART_STRING,    TEXT("CURRENT_PART") },
    { MONITOR_THIS_PART_STRING,       TEXT("MONITOR_THIS_PART") },
    { NOT_CURRENT_PART_STRING,        TEXT("NOT_CURRENT_PART") },
    { BISCUIT_TIME_DELAY_STRING,      TEXT("BISCUIT_TIME_DELAY") },
    { TIME_TO_MEASURE_BISCUIT_STRING, TEXT("TIME_TO_MEASURE_BISCUIT" ) },
    { BREAK_1_STRING,                 TEXT("BREAK_1" ) },
    { BREAK_2_STRING,                 TEXT("BREAK_2" ) },
    { BREAK_3_STRING,                 TEXT("BREAK_3" ) },
    { VEL_LOOP_GAIN_STRING,           TEXT("VEL_LOOP_GAIN" ) },
    { MISSING_FILE_DESC_STRING,       TEXT("MISSING_FILE_DESC" ) },
    { MISSING_FILE_ACTION_STRING,     TEXT("MISSING_FILE_ACTION" ) },
    { CHANNEL_1_LABEL_STRING,         TEXT("CH_1_LABEL" ) },
    { CHANNEL_2_LABEL_STRING,         TEXT("CH_2_LABEL" ) },
    { CHANNEL_3_LABEL_STRING,         TEXT("CH_3_LABEL" ) },
    { CHANNEL_4_LABEL_STRING,         TEXT("CH_4_LABEL" ) },
    { CHANNEL_5_LABEL_STRING,         TEXT("CH_5_LABEL" ) },
    { CHANNEL_6_LABEL_STRING,         TEXT("CH_6_LABEL" ) },
    { CHANNEL_7_LABEL_STRING,         TEXT("CH_7_LABEL" ) },
    { CHANNEL_8_LABEL_STRING,         TEXT("CH_8_LABEL" ) },
    { INTERNAL_PARAMETER_STRING,      TEXT("INTERNAL_PARAM" ) },
    { FT_ANALOG_STRING,               TEXT("FASTRAK_ANALOG_INPUT" ) },
    { FT_ONOFF_STRING,                TEXT("FT_ONOFF" ) },
    { FT_TARGET_STRING,               TEXT("FT_TARGET" ) },
    { FT_DIGITAL_STRING,              TEXT("FT_DIGITAL" ) },
    { FT_CALC_STRING,                 TEXT("FT_CALC" ) },
    { FT_EXTENDED_ANALOG_STRING,      TEXT("FT_EXTENDED_ANALOG" ) },
    { FT_PEAK_FS_VELOCITY_STRING,     TEXT("FT_PEAK_FS_VEL" ) },
    { FT_EXTERNAL_PARAMETER_STRING,   TEXT("FT_EXTERNAL_PARAM" ) },
    { FT_STATIC_TEXT_PARAMETER_STRING,TEXT("FT_STATIC_TEXT_PARAM" ) },
    { NOT_MONITORED_STRING,           TEXT("MACHINE_NOT_MONITORED") }
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
*                      stringtable_message_box                         *
***********************************************************************/
static int stringtable_message_box( TCHAR * msg, TCHAR * title )
{
STRING_CLASS m;
STRING_CLASS t;

m = StringTable.find( msg );
t = StringTable.find( title );

return MessageBox( NULL, m.text(), t.text(), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                      stringtable_okcancel_box                        *
***********************************************************************/
static int stringtable_okcancel_box( TCHAR * msg, TCHAR * title )
{
STRING_CLASS m;
STRING_CLASS t;

m = StringTable.find( msg );
t = StringTable.find( title );

return MessageBox( NULL, m.text(), t.text(), MB_OKCANCEL | MB_SYSTEMMODAL );
}

/***********************************************************************
*                           SET_STATICS_FONT                           *
***********************************************************************/
void set_statics_font( WINDOW_CLASS & w )
{
static LPARAM lParam = MAKELPARAM( TRUE, 0 );
if ( StaticsFont != INVALID_FONT_HANDLE )
    w.post( WM_SETFONT, (WPARAM) StaticsFont, lParam );
}

/***********************************************************************
*                           SET_STATICS_FONT                           *
***********************************************************************/
void set_statics_font( HWND hWnd, UINT id )
{
WINDOW_CLASS w;
w = GetDlgItem( hWnd, id );
set_statics_font( w );
}

/**********************************************************************
*                            UPDATE_STATICS                           *
**********************************************************************/
void update_statics( HWND hWnd, RESOURCE_TEXT_ENTRY * rlist, int n )
{
int  i;
STRING_CLASS s;
WINDOW_CLASS w;

for ( i=0; i<n; i++ )
    {
    w = GetDlgItem( hWnd, rlist[i].id );
    set_statics_font( w );
    s = StringTable.find( rlist[i].s  );
    if ( !s.isempty() )
        w.set_title( s.text() );
    }
}

/***********************************************************************
*                           ADD_TO_STMODLIST                           *
***********************************************************************/
static void add_to_stmodlist( HWND w )
{
/*
------------------------------------------------------------
Show the unsaved changes message if this is the first change
------------------------------------------------------------ */
if ( !StModList.count() )
    HaveStModTbox.show();

if ( !StModList.contains(w) )
    {
    StModList.add( w );
    InvalidateRect( w, 0, TRUE );
    }
}

/***********************************************************************
*                          IS_REMOTE_MONITOR                           *
***********************************************************************/
static BOOLEAN is_remote_monitor()
{
if ( IsOfficeWorkstation )
    {
    if ( FindWindow(Ft2ClassName,0) )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                     SEND_NEW_MONITOR_SETUP_EVENT                     *
***********************************************************************/
static void send_new_monitor_setup_event()
{
STRING_CLASS s;

/*
--------------------------------
See if this is the current part.
-------------------------------- */
s = CurrentMachine.current_part;

if ( s == CurrentPart.name )
    {
    Warmup.create_runlist( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    if ( !Warmup.is_enabled )
        {
        /*
        --------------------------------------------------------------------------------------------
        Make sure an alternate part has not been left in the current part field of the machset table
        -------------------------------------------------------------------------------------------- */
        current_part_name( s, CurrentPart.computer, CurrentPart.machine );
        if ( s != CurrentPart.name )
            set_current_part_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        }

    if ( CurrentMachine.monitor_flags & MA_MONITORING_ON || is_remote_monitor() )
        poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentMachine.name.text() );
    }
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( UINT helptype, DWORD context )
{
NAME_CLASS s;

s.get_exe_dir_file_name( TEXT("v5help.hlp") );
if ( s.file_exists() )
    WinHelp( MainWindow, s.text(), helptype, context );
}

/***********************************************************************
*                          GET_DEFAULT_PRINTER_DC                      *
***********************************************************************/
HDC get_default_printer_dc()
{
HDC        dc;
HANDLE     ph;
TCHAR      s[MAX_PATH+1];
TCHAR    * cp;

DWORD     bytes_needed;
HGLOBAL   dh;
DEVMODE * dm;

dc = 0;
dm = 0;

GetProfileString( TEXT("windows"), TEXT("device"), TEXT("..."), s, MAX_PATH );

cp = findchar( CommaChar, s );
if ( cp )
    {
    *cp = NullChar;

    if ( OpenPrinter(s, &ph, 0) )
        {
        bytes_needed = DocumentProperties( MainWindow, ph, s, 0, 0, 0 );
        dh = GlobalAlloc( GHND, bytes_needed );
        if ( dh )
            {
            dm = (DEVMODE *) GlobalLock( dh );
            DocumentProperties( MainWindow, ph, s, dm, 0, DM_OUT_BUFFER );
            if ( dm->dmFields & DM_ORIENTATION )
                {
                dm->dmOrientation = DMORIENT_PORTRAIT;
                DocumentProperties( MainWindow, ph, s, dm, dm, DM_IN_BUFFER | DM_OUT_BUFFER );
                }
            }
        dc = CreateDC( 0, s, 0, dm );
        if ( dh )
            GlobalUnlock( dh );
        ClosePrinter( ph );
        }
    }

return dc;
}

/***********************************************************************
*                         DELETE_PRINTER_FONT                          *
***********************************************************************/
static void delete_printer_font( HDC dc )
{
if ( PrinterFont )
    {
    SelectObject( dc, GetStockObject(SYSTEM_FIXED_FONT) ) ;
    DeleteObject( PrinterFont );
    }
}

/***********************************************************************
*                           SELECT_PRINTER_FONT                        *
***********************************************************************/
static void select_printer_font( HDC dc )
{
if ( PrinterFont )
    SelectObject( dc, PrinterFont ) ;
}

/***********************************************************************
*                           CREATE_PRINTER_FONT                        *
***********************************************************************/
static void create_printer_font( HDC dc, RECT & r )
{
long       height;
long       width;
static TCHAR FontName[] = TEXT( "MS Sans Serif" );

if ( dc )
    {
    delete_printer_font( dc );

    width  = 0;
    height = r.bottom - r.top;
    height /= 66;

    PrinterFont = create_font( dc, width, height, FontName );
    select_printer_font( dc );
    }
}

/***********************************************************************
*                               TEXT_OUT                               *
***********************************************************************/
void text_out( HDC dc, int32 x, int32 y, TCHAR * sorc )
{
TextOut( dc, (int) x, (int) y, sorc, lstrlen(sorc) );
}

/***********************************************************************
*                    PRINT_GLOBAL_SURETRAK_PARAMS                      *
***********************************************************************/
void print_global_suretrak_params( HDC dc, int y, int dx, int dy, RECT & border )
{
RECT r;

r.left = (border.left + border.right + dx) / 2;
r.right = border.right;

SetTextAlign( dc, TA_TOP | TA_LEFT );
text_out( dc, r.left, y, StringTable.find(TEXT("ST_GLOBAL_PARAMS")) );

y += dy;
r.top = y + dy/2;
r.bottom = r.top + dy*8;
Rectangle( dc, r.left, r.top-2, r.right, r.bottom+2 );
}

/***********************************************************************
*                       PRINT_FASTRAK_CHANNELS                         *
***********************************************************************/
void print_fastrak_channels( HDC dc, int y, int dx, int dy, RECT & border )
{
const int CHANNEL_NUMBER_INDEX = 0;
const int SENSOR_DESC_INDEX    = 1;
const int PRES_TYPE_INDEX      = 2;
const int NOF_POSITIONS        = 3;
const int MAXLINE = 256;

int  i;
int  width;
int  x[NOF_POSITIONS];
RECT r;
HWND w;
TCHAR  buf[MAXLINE+1];
LISTBOX_CLASS  lb;
TEXT_LEN_CLASS t;

r.left = border.left;

/*
---------------
Print the title
--------------- */
SetTextAlign( dc, TA_TOP | TA_LEFT );
text_out( dc, r.left, y, StringTable.find(TEXT("CHANNELS_TAB_STRING")) );

w = WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow;

x[CHANNEL_NUMBER_INDEX] = r.left + dx;

get_text( buf, w, FASTRAK_CHANNEL_STATIC, MAXLINE );
width = pixel_width( dc, buf );
x[SENSOR_DESC_INDEX] = x[CHANNEL_NUMBER_INDEX] + width + 2*dx;

x[CHANNEL_NUMBER_INDEX] += width/2;

get_text( buf, w, ANALOG_SENSOR_STATIC, MAXLINE );
t.init( dc );
t.check( buf );

for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    lb.init( w, FT_CHANNEL_1_LISTBOX+i );
    t.check( lb.selected_text() );
    }

x[PRES_TYPE_INDEX] = x[SENSOR_DESC_INDEX] + t.width() + 2*dx;

t.init( dc );
get_text( buf, w, HEAD_PRES_STATIC, MAXLINE );
t.check( buf );

get_text( buf, w, ROD_PRES_STATIC, MAXLINE );
t.check( buf );

r.right = x[PRES_TYPE_INDEX] + t.width() + 2*dx;

y += dy;
r.top = y + dy/2;
r.bottom = r.top + dy*6;
Rectangle( dc, r.left, r.top-2, r.right, r.bottom+2 );

y += dy;

SetTextAlign( dc, TA_TOP | TA_CENTER );
get_text( buf, w, FASTRAK_CHANNEL_STATIC, MAXLINE );
text_out( dc, x[CHANNEL_NUMBER_INDEX], y, buf );

SetTextAlign( dc, TA_TOP | TA_LEFT );
get_text( buf, w, ANALOG_SENSOR_STATIC, MAXLINE );
text_out( dc, x[SENSOR_DESC_INDEX], y, buf );


for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    y += dy;

    /*
    --------------
    Channel number
    -------------- */
    SetTextAlign( dc, TA_TOP | TA_CENTER );
    text_out( dc, x[CHANNEL_NUMBER_INDEX], y, int32toasc(i+1) );

    /*
    -----------
    Sensor type
    ----------- */
    SetTextAlign( dc, TA_TOP | TA_LEFT );
    lb.init( w, FT_CHANNEL_1_LISTBOX+i );
    text_out( dc, x[SENSOR_DESC_INDEX], y, lb.selected_text() );

    /*
    -------------
    Pressure type
    ------------- */
    *buf = NullChar;
    if ( is_checked(w, HEAD_PRES_CHAN_1_BUTTON+i) )
        get_text( buf, w, HEAD_PRES_STATIC, MAXLINE );
    else if ( is_checked(w, ROD_PRES_CHAN_1_BUTTON+i) )
        get_text( buf, w, ROD_PRES_STATIC, MAXLINE );

    if ( *buf != NullChar )
        text_out( dc, x[PRES_TYPE_INDEX], y, buf );
    }
}

/***********************************************************************
*                        PRINT_SURETRAK_PARAMS                         *
***********************************************************************/
static int print_suretrak_params( HDC dc, int y, int dx, int dy, RECT & border )
{
const int MAXLINE = 256;
int    i;
int    id;
int    x1;
int    x;
int    velocity_x;
int    gain_x;
RECT   r;
TCHAR  buf[MAXLINE+1];

r.left  = border.left;
r.right = (border.left + border.right - dx) / 2;

SetTextAlign( dc, TA_TOP | TA_LEFT );
text_out( dc, r.left, y, StringTable.find(TEXT("ST_PARAMS")) );

y += dy;
r.top = y + dy/2;
r.bottom = r.top + dy*7;
Rectangle( dc, r.left, r.top-2, r.right, r.bottom+2 );

y += dy;
x = r.left + dx;

lstrcpy( buf, StringTable.find(TEXT("VEL_LOOP_GAIN")) );
x1 = x + pixel_width( dc, buf ) + 2 * dx;
text_out( dc, x,  y, buf );
text_out( dc, x1, y, SureTrakSetup.velocity_loop_gain().text() );

velocity_x = x + pixel_width(dc, StringTable.find(TEXT("BREAK_1"))) + 2*dx;

get_text( buf, WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow, VELOCITY_STATIC, MAXLINE );
gain_x = velocity_x + pixel_width(dc, buf) + 2*dx;

y += dy;
y += dy/2;

text_out( dc, velocity_x, y, buf );
text_out( dc, gain_x,     y, StringTable.find(TEXT("GAIN_STRING")) );

id = BREAK_1_STRING;
for ( i=0; i<NOF_VEL_GAIN_SETTINGS; i++ )
    {
    y += dy;
    text_out( dc, x,          y, resource_string(id) );
    text_out( dc, velocity_x, y, SureTrakSetup.gain_break_velocity(i).text() );
    text_out( dc, gain_x,     y, SureTrakSetup.gain_break_gain(i).text() );
    id++;
    }

return (int) r.bottom;
}

/***********************************************************************
*                            PRINT_PARAMETERS                          *
***********************************************************************/
void print_parameters( HDC dc, int y, int dx, int dy, RECT & border )
{
const int MAXLINE       = 256;
const int NOF_POSITIONS =  11;  /* Desc, units, target,low wire high wire low wire high wire */
const int PARAM_DESC_INDEX      = 0;
const int UNITS_INDEX           = 1;
const int TARGET_INDEX          = 2;
const int WARN_LOW_INDEX        = 3;
const int WARN_LOW_WIRE_INDEX   = 4;
const int WARN_HIGH_INDEX       = 5;
const int WARN_HIGH_WIRE_INDEX  = 6;
const int ALARM_LOW_INDEX       = 7;
const int ALARM_LOW_WIRE_INDEX  = 8;
const int ALARM_HIGH_INDEX      = 9;
const int ALARM_HIGH_WIRE_INDEX = 10;

struct LABEL_ENTRY {
    int  id;
    UINT flags;
    };

const LABEL_ENTRY Label[NOF_POSITIONS]     = {
        { PARAM_DESC_STATIC,        TA_TOP | TA_LEFT   },
        { UNITS_STATIC,             TA_TOP | TA_LEFT   },
        { TARGET_STATIC,            TA_TOP | TA_LEFT   },
        { LOW_WARNING_STATIC,       TA_TOP | TA_LEFT   },
        { LOW_WARNING_WIRE_STATIC,  TA_TOP | TA_CENTER },
        { HIGH_WARNING_STATIC,      TA_TOP | TA_LEFT   },
        { HIGH_WARNING_WIRE_STATIC, TA_TOP | TA_CENTER },
        { LOW_ALARM_STATIC,         TA_TOP | TA_LEFT   },
        { LOW_ALARM_WIRE_STATIC,    TA_TOP | TA_CENTER },
        { HIGH_ALARM_STATIC,        TA_TOP | TA_LEFT   },
        { HIGH_ALARM_WIRE_STATIC,   TA_TOP | TA_CENTER }
        };

TCHAR buf[MAXLINE+1];
int   i;
int   n;
int   n_to_print;
int   x[NOF_POSITIONS];
int   x1;
HWND  w;
RECT  r;
TEXT_LEN_CLASS t;

w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;

x[PARAM_DESC_INDEX] = border.left + dx;

/*
-------------------------------------------
Get the width of the  widest parameter name
------------------------------------------- */
t.init( dc );

get_text( buf, w, PARAM_DESC_STATIC, MAXLINE );
t.check( buf );

n_to_print = 0;
n = CurrentParam.count();
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].has_any_limits() )
        {
        t.check( CurrentParam.parameter[i].name );
        n_to_print++;
        }
    }

x[UNITS_INDEX] = x[PARAM_DESC_INDEX] + t.width() + 2*dx;

/*
----------------------------------------
Get the width of the widest units string
---------------------------------------- */
t.init( dc );
get_text( buf, w, UNITS_STATIC, MAXLINE );
t.check( buf );
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].has_any_limits() )
        t.check( units_name(CurrentParam.parameter[i].units) );
    }

x[TARGET_INDEX]          = x[UNITS_INDEX] + t.width() + 2*dx;
x[WARN_LOW_INDEX]        = x[TARGET_INDEX]         + 10*dx;
x[WARN_LOW_WIRE_INDEX]   = x[WARN_LOW_INDEX]       + 10*dx;
x[WARN_HIGH_INDEX]       = x[WARN_LOW_WIRE_INDEX]  + 5*dx;
x[WARN_HIGH_WIRE_INDEX]  = x[WARN_HIGH_INDEX]      + 10*dx;
x[ALARM_LOW_INDEX]       = x[WARN_HIGH_WIRE_INDEX] + 5*dx;
x[ALARM_LOW_WIRE_INDEX]  = x[ALARM_LOW_INDEX]      + 10*dx;
x[ALARM_HIGH_INDEX]      = x[ALARM_LOW_WIRE_INDEX] + 5*dx;
x[ALARM_HIGH_WIRE_INDEX] = x[ALARM_HIGH_INDEX]     + 10*dx;

r.left  = border.left;
r.right = x[ALARM_HIGH_WIRE_INDEX] + 4*dx;

SetTextAlign( dc, TA_TOP | TA_LEFT );
text_out( dc, r.left, y, StringTable.find(TEXT("PARAMETERS_TAB")) );

y += dy;
r.top = y + dy/2;
r.bottom = r.top + dy*(n_to_print+3);
Rectangle( dc, r.left, r.top-2, r.right, r.bottom+2 );

y += dy;
SetTextAlign( dc, TA_TOP | TA_CENTER );

x1 = x[WARN_LOW_INDEX] + x[ALARM_LOW_INDEX] - dx;
x1 /= 2;

get_text( buf, w, WARN_LIMITS_STATIC, MAXLINE );
text_out( dc, x1, y, buf );

x1 = x[ALARM_LOW_INDEX] + x[ALARM_HIGH_WIRE_INDEX] + 2*dx;
x1 /= 2;

get_text( buf, w, ALARM_LIMITS_STATIC, MAXLINE );
text_out( dc, x1, y, buf );

y += dy;
SetTextAlign( dc, TA_TOP | TA_LEFT );

get_text( buf, w, PARAM_DESC_STATIC, MAXLINE );
text_out( dc, x[PARAM_DESC_INDEX], y, buf );

for ( i=0; i<NOF_POSITIONS; i++ )
    {
    SetTextAlign( dc, Label[i].flags );
    get_text( buf, w, Label[i].id, MAXLINE );
    text_out( dc, x[i], y, buf );
    };

SetTextAlign( dc, TA_TOP | TA_LEFT );
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].has_any_limits() )
        {
        y += dy;
        text_out( dc, x[PARAM_DESC_INDEX],      y, CurrentParam.parameter[i].name );
        text_out( dc, x[UNITS_INDEX],           y, units_name(CurrentParam.parameter[i].units) );
        text_out( dc, x[TARGET_INDEX],          y, CurrentParam.parameter[i].target.text() );
        text_out( dc, x[WARN_LOW_INDEX],        y, ascii_float(CurrentParam.parameter[i].limits[WARNING_MIN].value)      );
        text_out( dc, x[WARN_LOW_WIRE_INDEX],   y, int32toasc(CurrentParam.parameter[i].limits[WARNING_MIN].wire_number) );
        text_out( dc, x[WARN_HIGH_INDEX],       y, ascii_float(CurrentParam.parameter[i].limits[WARNING_MAX].value)      );
        text_out( dc, x[WARN_HIGH_WIRE_INDEX],  y, int32toasc(CurrentParam.parameter[i].limits[WARNING_MAX].wire_number) );
        text_out( dc, x[ALARM_LOW_INDEX],       y, ascii_float(CurrentParam.parameter[i].limits[ALARM_MIN].value)      );
        text_out( dc, x[ALARM_LOW_WIRE_INDEX],  y, int32toasc(CurrentParam.parameter[i].limits[ALARM_MIN].wire_number) );
        text_out( dc, x[ALARM_HIGH_INDEX],      y, ascii_float(CurrentParam.parameter[i].limits[ALARM_MAX].value)      );
        text_out( dc, x[ALARM_HIGH_WIRE_INDEX], y, int32toasc(CurrentParam.parameter[i].limits[ALARM_MAX].wire_number) );
        }
    }
}

/***********************************************************************
*                          PRINT_SETUP_SHEET                           *
***********************************************************************/
static int print_setup_sheet( HDC dc, int x, int y, int dx, int dy )
{
NAME_CLASS   s;
FILE_CLASS   f;
TCHAR      * cp;
RECT         r;
TEXT_LEN_CLASS t;

s.get_setup_sheet_file_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
if ( s.file_exists() )
    {
    /*
    -----------------------------------------
    Y is the bottom of the previous rectangle
    ----------------------------------------- */
    y += dy;
    text_out( dc, x, y, StringTable.find(TEXT("SETUP_SHEET_TAB")) );
    y += dy + dy/4;

    r.left   = x;
    r.top    = y;
    r.bottom = y;

    t.init( dc );

    if ( f.open_for_read(s.text()) )
        {
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;
            r.bottom += dy;
            t.check( cp );
            }

        r.right = r.left + t.width() + 2*dx;
        r.bottom += 4;
        Rectangle( dc, r.left, r.top, r.right, r.bottom );

        x += dx;
        y += 2;

        f.rewind();
        while ( true )
            {
            cp = f.readline();
            if ( !cp )
                break;
            text_out( dc, x, y, cp );
            y += dy;
            }
        f.close();
        }
    }

return (int) r.bottom;
}

/***********************************************************************
*                         PRINT_SURETRAK_STEPS                         *
***********************************************************************/
void print_suretrak_steps( HDC dc, int y, int dx, int dy, RECT & border )
{
const int MAXLINE = 256;

int  id;
HWND dw;
HWND w;

int  column;
int  dialog_box_char_height;
int  dialog_box_char_width;
int  label_width;
int  label_height;
int  ls_desc_x;
int  ls_value_x;
int  ls_y;
int  step;
int  width;
int  x;

RECT r;
TCHAR  buf[MAXLINE+1];

const int VAC_WAIT_COLUMN      =  3;
const int LI_COLUMN            =  4;
const int LI_FROM_INPUT_COLUMN =  5;

struct SURETRAK_COLUMN_ENTRY
    {
    int     width;
    int     position;
    UINT    align_flags;
    };

SURETRAK_COLUMN_ENTRY sc[] = {
    { 0, 0, TA_TOP | TA_RIGHT  }, /* Accel */
    { 0, 0, TA_TOP | TA_RIGHT  }, /* Target Velocity */
    { 0, 0, TA_TOP | TA_RIGHT  }, /* Ending Position */
    { 0, 0, TA_TOP | TA_CENTER }, /* Vacuum Wait */
    { 0, 0, TA_TOP | TA_RIGHT  }, /* Low Impact Percentage */
    { 0, 0, TA_TOP | TA_CENTER }  /* LI from Input */
    };

const int nof_columns = sizeof(sc)/sizeof(SURETRAK_COLUMN_ENTRY);

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

r.left   = 0;
r.right  = 4;
r.top    = 0;
r.bottom = 8;

MapDialogRect( w, &r );

dialog_box_char_width  = r.right;
dialog_box_char_height = r.bottom;

x      = 4 * dx;
y     += 3 * dy;
id     = ACCEL_STATIC;
for ( column=0; column<nof_columns; column++ )
    {
    GetWindowRect( GetDlgItem(w, id), &r );
    label_width  = r.right - r.left;
    label_height = r.bottom - r.top;

    r.left    = x;
    r.bottom  = y;

    /*
    ----------------------------------------------------------------
    The printer label width is the screen label width times
    the ratio of the printer font to the screen font (plus a fudge).
    ---------------------------------------------------------------- */
    sc[column].width  = label_width * (dx+1);
    sc[column].width /= dialog_box_char_width;
    r.right = r.left + sc[column].width;

    r.top  = label_height * (dy+1);
    r.top /= dialog_box_char_height;
    r.top  = r.bottom - r.top;

    get_text( buf, w, id, MAXLINE );
    DrawText( dc, buf, -1, &r, DT_CENTER | DT_WORDBREAK );

    if ( column == VAC_WAIT_COLUMN || column == LI_FROM_INPUT_COLUMN )
        {
        sc[column].position = r.left + sc[column].width/2;
        }
    else if ( column == LI_COLUMN )
        {
        sc[column].position = r.left + sc[column].width;
        }
    else
        {
        /*
        -------------------------------------------------------------
        Position the numbers 3/4 of the way to the right (right just)
        ------------------------------------------------------------- */
        sc[column].position = r.left + sc[column].width*3/4;
        }

    x = r.right + dx;

    id++;
    }

/*
-------------------------------------
Show the limit switch positions title
------------------------------------- */
SetTextAlign( dc, TA_TOP | TA_LEFT );
get_text( buf, w, LIMIT_SWITCH_POS_STATIC, MAXLINE );

ls_desc_x = x + 3*dx;
text_out( dc, ls_desc_x, y-2*dy, buf );

/*
---------------------------------
Get the width of the widest label
--------------------------------- */
label_width = 0;
for ( column=0; column<MAX_FTII_LIMIT_SWITCHES; column++ )
    {
    width = pixel_width( dc, CurrentLimitSwitch.desc(column).text() );
    if ( width > label_width )
        label_width = width;
    }

/*
---------------------------------------------------------------------
Calculate the position of the limit switch position (right justified)
--------------------------------------------------------------------- */
ls_value_x = ls_desc_x + label_width + 6*dx;

id = ACCEL_1_EDITBOX;
for ( step=0; step<MAX_ST_STEPS; step++ )
    {
    for ( column=0; column<nof_columns; column++ )
        {
        if ( column == VAC_WAIT_COLUMN || column == LI_FROM_INPUT_COLUMN )
            {
            if ( is_checked(w, id) )
                {
                SetTextAlign( dc, sc[column].align_flags );
                text_out( dc, sc[column].position, y, LowerXString );
                }
            }
        else
            {
            dw = GetDlgItem(w, id );
            get_text( buf, dw, MAXLINE );
            if ( *buf != NullChar )
                {
                SetTextAlign( dc, sc[column].align_flags );
                text_out( dc, sc[column].position, y, buf );
                }
            }
        id++;
        }

    if ( step < MAX_FTII_LIMIT_SWITCHES )
        {
        ls_y = y - dy/2;
        SetTextAlign( dc, TA_TOP | TA_LEFT );
        text_out( dc, ls_desc_x, ls_y, CurrentLimitSwitch.desc(step).text() );

        SetTextAlign( dc, TA_TOP | TA_RIGHT );
        get_text( buf, w, LS1_POS_EB+step, MAXLINE );
        text_out( dc, ls_value_x, ls_y, buf );
        }

    y += dy;
    }
}

/***********************************************************************
*                            PRINT_HEADER                              *
***********************************************************************/
static void print_header( HDC dc, int y, int dy, RECT & r )
{
TIME_CLASS t;

SetTextAlign( dc, TA_TOP | TA_RIGHT );
text_out( dc, r.right, y, CurrentMachine.name.text() );

y += dy;
text_out( dc, r.right, y, CurrentPart.name.text() );

/*
-----------
Todays Date
----------- */
y += dy;
t.get_local_time();
text_out( dc, r.right, y, t.mmddyyyy() );
}

/***********************************************************************
*                          PRINT_PART_SETUP                            *
***********************************************************************/
static void print_part_setup()
{
const int MAXLINE = 256;
static DOCINFO di = { sizeof(DOCINFO), TEXT("TrueTrak Profile"), 0 };

struct EBOX_TBOX_ENTRY {
    int editbox;
    int textbox;
    };

EBOX_TBOX_ENTRY id[] = {
    { PLUNGER_DIAMETER_EDITBOX,       PLUNGER_DIAMETER_STATIC   },
    { CSFS_RISE_VELOCITY_EDITBOX,     VEL_FOR_RISE_STATIC       },
    { SLEEVE_FILL_DISTANCE_EDITBOX,   P1_TO_P3_STATIC           },
    { TIME_FOR_INTENS_PRES_EDITBOX,   TIME_FOR_INTENS_STATIC    },
    { RUNNER_FILL_DISTANCE_EDITBOX,   P2_TO_P3_STATIC           },
    { PRES_FOR_RESPONSE_TIME_EDITBOX, PRES_FOR_RESPONSE_STATIC  },
    { CSFS_MIN_POSITION_EDITBOX,      MIN_CSFS_POS_STATIC       },
    { BISCUIT_TIME_DELAY_EDITBOX,     BISCUIT_TIME_DELAY_STATIC },
    { CSFS_MIN_VELOCITY_EDITBOX,      MIN_CSFS_VEL_STATIC       }
    };
const int nof_ids = sizeof(id)/sizeof(int);

HDC     dc;
HWND    w;
RECT    r;
RECT    tr;
RECT    dr;
POINT   p;
int     i;
int     line;
int     n;
int     x;
int     x1;
int     y;
int     dy;
int     dx;
int     label_width;
int     pos_label_width;
BOOLEAN have_alarms;
TCHAR   buf[MAXLINE+1];

dc = get_default_printer_dc();
if ( dc )
    {

    if ( StartDoc(dc, &di) > 0 )
        {
        if ( StartPage(dc) > 0 )
            {
            p.x = GetDeviceCaps( dc, PHYSICALOFFSETX );
            if ( p.x == 0 )
                {
                /*
                ------------------------------------
                If no margin force a 1/2 inch margin
                ------------------------------------ */
                p.x = .50 * GetDeviceCaps( dc, LOGPIXELSX );
                }
            p.y = GetDeviceCaps( dc, PHYSICALOFFSETY );
            if ( p.y == 0 )
                {
                p.y = .50 * GetDeviceCaps( dc, LOGPIXELSY );
                }
            r.left   = p.x;

            r.top    = p.y;
            r.right  = GetDeviceCaps( dc, HORZRES ) - p.x;
            r.bottom = GetDeviceCaps( dc, VERTRES ) - p.y;
            create_printer_font( dc, r );
            dy = character_height( dc );
            dx = average_character_width( dc );

            y = dy;
            print_header( dc, y, dy, r );

            /*
            --------------------------------------------
            Draw the rectangle for the basic part setups
            -------------------------------------------- */
            dr = r;
            dr.top = y + dy/2;
            dr.bottom = dr.top + dy*7;
            dr.right = dr.left + (dr.right - dr.left)/2;
            Rectangle( dc, dr.left, dr.top, dr.right, dr.bottom );

            y += dy;
            x  = r.left + dx;
            w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

            /*
            --------------
            Distance Units
            -------------- */
            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, DISTANCE_UNITS_STATIC, MAXLINE );
            lstrcat( buf, SpaceString );
            lstrcat( buf, units_name(CurrentPart.distance_units) );
            text_out( dc, x, y, buf );

            /*
            --------------
            Velocity Units
            -------------- */
            y += dy;
            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, VELOCITY_UNITS_STATIC, MAXLINE );
            lstrcat( buf, SpaceString );
            lstrcat( buf, units_name(CurrentPart.velocity_units) );
            text_out( dc, x, y, buf );

            /*
            -------------------
            Total Stroke Length
            ------------------- */
            y += dy;
            y += dy/2;
            x += 8 * dx;
            SetTextAlign( dc, TA_TOP | TA_RIGHT );
            get_text( buf, w, TOTAL_STROKE_LEN_EDITBOX, MAXLINE );
            text_out( dc, x, y, buf );

            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, TOTAL_STROKE_STATIC, MAXLINE );
            text_out( dc, x+dx, y, buf );

            /*
            -----------------
            Min Stroke Length
            ----------------- */
            y += dy;
            SetTextAlign( dc, TA_TOP | TA_RIGHT );
            get_text( buf, w, MIN_STROKE_LEN_EDITBOX, MAXLINE );
            text_out( dc, x, y, buf );

            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, MIN_STROKE_STATIC, MAXLINE );
            text_out( dc, x+dx, y, buf );

            /*
            ------------
            EOS Velocity
            ------------ */
            y += dy;
            SetTextAlign( dc, TA_TOP | TA_RIGHT );
            get_text( buf, w, EOS_VEL_EDITBOX, MAXLINE );
            text_out( dc, x, y, buf );

            SetTextAlign( dc, TA_TOP | TA_LEFT );
            get_text( buf, w, EOS_STATIC, MAXLINE );
            text_out( dc, x+dx, y, buf );

            /*
            --------------
            Advanced Setup
            -------------- */
            y = dr.bottom + dy;
            SetTextAlign( dc, TA_TOP | TA_LEFT );
            text_out( dc, r.left, y, StringTable.find(TEXT("ADVANCED_TAB")) );

            y += dy;

            /*
            -----------------------------------------------
            Draw the rectangle for the advanced part setups
            ----------------------------------------------- */
            dr.top = y + dy/2;
            dr.bottom = dr.top + dy*6;
            dr.right = r.right - dx;
            Rectangle( dc, dr.left, dr.top, dr.right, dr.bottom );

            w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

            i = 0;
            for ( line=0; line<5; line++ )
                {
                y += dy;
                x  = r.left + 8*dx;
                SetTextAlign( dc, TA_TOP | TA_RIGHT );
                get_text( buf, w, id[i].editbox, MAXLINE );
                text_out( dc, x, y, buf );

                SetTextAlign( dc, TA_TOP | TA_LEFT );
                get_text( buf, w, id[i].textbox, MAXLINE );
                text_out( dc, x+dx, y, buf );
                i++;

                if ( i < nof_ids )
                    {
                    x = dr.left + (dr.right - dr.left)/2;
                    x += 8*dx;
                    SetTextAlign( dc, TA_TOP | TA_RIGHT );
                    get_text( buf, w, id[i].editbox, MAXLINE );
                    text_out( dc, x, y, buf );

                    SetTextAlign( dc, TA_TOP | TA_LEFT );
                    get_text( buf, w, id[i].textbox, MAXLINE );
                    text_out( dc, x+dx, y, buf );
                    i++;
                    }
                }

            /*
            ---------
            Positions
            --------- */
            y += 2*dy + dy/3;
            SetTextAlign( dc, TA_TOP | TA_LEFT );
            text_out( dc, r.left, y, StringTable.find(TEXT("POSITIONS_TAB")) );

            y += dy;

            /*
            -------------------------------------------------
            Draw the rectangle for the user defined positions
            ------------------------------------------------- */
            dr.top = y + dy/4;
            dr.bottom = dr.top + dy*7;
            Rectangle( dc, dr.left, dr.top-2, dr.right, dr.bottom+2 );

            w = WindowInfo[USER_DEFINED_TAB].dialogwindow;

            label_width = (dr.right - dr.left) / 7;
            tr.top      = dr.top + 2*dy;
            tr.bottom   = dr.bottom - dy;
            tr.left     = dr.left + dx;
            tr.right    = dr.left + label_width;

            get_text( buf, w, LS_POS_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            x = tr.right + 9*dx;

            tr.left  += label_width + 12*dx;
            tr.right += label_width + 12*dx;

            get_text( buf, w, VEL_POS_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            tr.top   -= dy/2;
            tr.left  += label_width + 12*dx;
            tr.right += label_width + 12*dx;
            tr.bottom = dr.bottom - dy/2;

            get_text( buf, w, AVG_VEL_RANGE_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            x = dr.left + label_width + 3*dx;
            y  = dr.top + dy;
            get_text( buf, w, POSITION_STATIC, MAXLINE );
            text_out( dc, x, y, buf );

            x += label_width + 12 * dx;
            text_out( dc, x, y, buf );

            pos_label_width = pixel_width(dc, buf);

            /*
            --------------------
            Start position label
            -------------------- */
            tr.top    = dr.top;
            tr.bottom = tr.top + 2*dy;
            tr.left   = x + label_width + 12 * dx;
            tr.right  = tr.left + pos_label_width;
            get_text( buf, w, START_POS_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            /*
            -----------------
            End postion label
            ----------------- */
            if ( 7*dx > pos_label_width )
                pos_label_width = 7*dx;

            pos_label_width += dx;

            tr.left  += pos_label_width;
            tr.right += pos_label_width;
            get_text( buf, w, END_POS_STATIC, MAXLINE );
            DrawText( dc, buf, -1, &tr, DT_LEFT | DT_WORDBREAK );

            y  = dr.top + 2*dy;
            line = 1;
            for ( i=0; i<LIMIT_SWITCH_COUNT; i++ )
                {
                /*
                ------------
                Line numbers
                ------------ */
                x  = dr.left + label_width + 2*dx;
                SetTextAlign( dc, TA_TOP | TA_LEFT );
                lstrcpy( buf, int32toasc(line) );
                text_out( dc, x, y, buf );

                x1 = x + label_width + 12 * dx;
                text_out( dc, x1, y, buf );

                x1 += label_width + 12 * dx;
                text_out( dc, x1, y, buf );

                /*
                ---------------------
                Limit switch position
                --------------------- */
                x1 = x + 7*dx;
                SetTextAlign( dc, TA_TOP | TA_RIGHT );
                get_text( buf, w, LS1_POS_EB+i, MAXLINE );
                text_out( dc, x1, y, buf );

                /*
                -----------------
                Velocity position
                ----------------- */
                x1 += label_width + 12 * dx;
                get_text( buf, w, VELOCITY_POSITION_1_EDITBOX+i, MAXLINE );
                text_out( dc, x1, y, buf );

                /*
                -----------------------------
                Velocity range start position
                ----------------------------- */
                x1 += label_width + 12 * dx;
                get_text( buf, w, AVG_VELOCITY_START_1_EDITBOX+i, MAXLINE );
                text_out( dc, x1, y, buf );

                /*
                ---------------------------
                Velocity range end position
                --------------------------- */
                x1 +=  pos_label_width;
                get_text( buf, w, AVG_VELOCITY_END_1_EDITBOX+i, MAXLINE );
                text_out( dc, x1, y, buf );

                line++;
                y += dy;
                }

            y += 2*dy;
            print_fastrak_channels( dc, y, dx, dy, r );

            if ( HaveSureTrakControl )
                {
                /*
                ----------------
                Suretrak profile
                ---------------- */
                y += 8*dy + dy/2;
                SetTextAlign( dc, TA_TOP | TA_LEFT );
                text_out( dc, r.left, y, StringTable.find(TEXT("SURETRAK_TAB_STRING")) );

                y += dy;

                dr.top = y + dy/2;
                dr.bottom = dr.top + dy*8;
                Rectangle( dc, dr.left, dr.top-2, dr.right, dr.bottom+2 );

                print_suretrak_steps( dc, y, dx, dy, r );

                y += 10 * dy;

                y = print_suretrak_params( dc, y, dx, dy, r );
                /* print_global_suretrak_params( dc, y, dx, dy, r ); */
                }

            if ( ShowSetupSheet )
                y = print_setup_sheet( dc, r.left, y, dx, dy );

            lstrcpy( buf, get_ini_string(EditPartIniFile, ConfigSection, IsoPageOneKey) );
            if ( !unknown(buf) && lstrlen(buf) > 0 )
                {
                y = r.bottom - dy - 1;
                x = r.left + dx;
                text_out( dc, x, y, buf );
                }
            EndPage(dc);
            }

        if ( *get_ini_string(EditPartIniFile, ConfigSection, PrintParametersKey) == YChar )
            {
            /*
            ---------------------------------
            See if there is anything to print
            --------------------------------- */
            have_alarms = FALSE;
            n = CurrentParam.count();
            for ( i=0; i<n; i++ )
                {
                if ( CurrentParam.parameter[i].has_any_limits() )
                    have_alarms = TRUE;
                }

            if ( have_alarms )
                {
                StartPage( dc );
                select_printer_font( dc );

                y = dy;
                print_header( dc, y, dy, r );

                print_parameters( dc, y, dx, dy, r );
                lstrcpy( buf, get_ini_string(EditPartIniFile, ConfigSection, IsoPageTwoKey) );
                if ( !unknown(buf) && lstrlen(buf) > 0 )
                    {
                    y = r.bottom - dy - 1;
                    x = r.left + dx;
                    text_out( dc, x, y, buf );
                    }
                EndPage(dc);
                }
            }

        EndDoc( dc );
        delete_printer_font( dc );
        }
    DeleteDC( dc );
    dc = NULL;
    }
}

/***********************************************************************
*                          GET_OVERVIEW_HELP                           *
***********************************************************************/
static void get_overview_help()
{
DWORD context;

context = BASIC_PART_HELP;

if ( CurrentDialogNumber >= 0 && CurrentDialogNumber < TAB_COUNT )
    {
    switch ( CurrentDialogNumber )
        {
        case BASIC_SETUP_TAB:
            context = BASIC_PART_HELP;
            break;

        case ADVANCED_SETUP_TAB:
            context = ADV_PART_OVERVIEW_HELP;
            break;

        case PARAMETER_LIMITS_TAB:
            context = PARAMETER_LIMITS_OVERVIEW_HELP;
            break;

        case USER_DEFINED_TAB:
            context = USER_DEF_POS_OVERVIEW_HELP;
            break;

        case PROFILE_MARKS_TAB:
            context = MARKS_SETUP_OVERVIEW_HELP;
            break;

        case FASTRAK_CHANNELS_TAB:
            context = CHANNELS_SETUP_OVERVIEW_HELP;
            break;

        case SURETRAK_PROFILE_TAB:
            if ( HaveSureTrakControl )
                context = PART_SETUP_SURETRAK_HELP;
            else
                context = SETUP_SHEET_HELP;
            break;

        case SETUP_SHEET_TAB:
            context = SETUP_SHEET_HELP;
            break;

        case PRESSURE_CONTROL_TAB:
            context = PRESSURE_CONTROL_HELP;
            break;
        }
    }

gethelp( HELP_CONTEXT, context );
}

/***********************************************************************
*                      POSITION_DROPDOWN_WINDOW                        *
***********************************************************************/
static void position_dropdown_window( HWND w )
{
RECT pr;
RECT mr;
int  x;

if ( !w )
    return;

if ( !IsWindowVisible(w) )
    return;

GetWindowRect( MainWindow, &mr );
GetWindowRect( w,          &pr );

x = mr.right - pr.right;

pr.left  += x;
pr.right += x;

x = mr.bottom - pr.top;

pr.top    += x;
pr.bottom += x;

MoveWindow( w, pr.left, pr.top, pr.right-pr.left, pr.bottom-pr.top, TRUE );
}

/***********************************************************************
*                     POSITION_CURRENT_DIALOG                          *
***********************************************************************/
static void position_current_dialog()
{
WINDOW_CLASS parent;
WINDOW_CLASS child;
RECT cr;
RECT r;
int  h;
int  w;
int  x;
int  y;

if ( CurrentDialogNumber != NO_TAB )
    {
    GetWindowRect( WindowInfo[CurrentDialogNumber].dialogwindow, &r );
    w = r.right - r.left;
    h = r.bottom - r.top;

    /*
    ----------------------------------------------------------------------------------
    These are not client dialogs so I have to add the upper left corner to the x and y
    ---------------------------------------------------------------------------------- */
    GetWindowRect( MainDialogWindow, &r );
    y = r.top;
    x = r.left;

    /*
    --------------------------------------------------------
    Get the rectangle that the current dialog is a client of
    -------------------------------------------------------- */
    parent = MainDialogWindow;
    parent.get_client_rect( r );

    child = parent.control( SHOT_CONTROL_SETUP_RB );
    child.get_move_rect( cr );
    r.top = cr.bottom;

    child = parent.control( PART_LISTBOX_ID );
    child.get_move_rect( cr );
    r.left = cr.right;

    child = parent.control( SAVE_CHANGES_BUTTON );
    child.get_move_rect( cr );
    r.bottom = cr.top;

    y += r.top + ((r.bottom - r.top) - h)/2;
    x += r.left + ( (r.right - r.left) - w )/2;
    MoveWindow( WindowInfo[CurrentDialogNumber].dialogwindow, x, y, w, h, TRUE );
    }

position_dropdown_window( ParamDialogWindow );
position_dropdown_window( SureTrakPlotWindow.handle() );
}

/***********************************************************************
*                          SHOW_CURRENT_DIALOG                         *
***********************************************************************/
static void show_current_dialog( int new_dialog_number )
{
if ( new_dialog_number == CurrentDialogNumber )
    return;

if ( CurrentDialogNumber != NO_TAB )
    {
    ShowWindow( WindowInfo[CurrentDialogNumber].dialogwindow, SW_HIDE );
    }

if ( new_dialog_number >= 0 && new_dialog_number < TAB_COUNT )
    {
    CurrentDialogNumber = new_dialog_number;
    if ( CurrentDialogNumber == SURETRAK_PROFILE_TAB )
        refresh_velocity_control_plot();
    //else if ( CurrentDialogNumber == PRESSURE_CONTROL_TAB )
        //refresh_pressure_control_plot();
    ShowWindow( WindowInfo[CurrentDialogNumber].dialogwindow, SW_SHOW );
    position_current_dialog();
    }
}

/***********************************************************************
*                           SHOW_TAB_CONTROL                           *
* This used to be a true tab control but I replaced it with radio      *
* buttons BASIC_SETUP_RB to SETUP_SCREEN_SETUP_RB.                     *
***********************************************************************/
static void show_tab_control( int new_tab_number )
{
if ( new_tab_number == SURETRAK_PROFILE_TAB && !CurrentMachine.suretrak_controlled )
    new_tab_number = BASIC_SETUP_TAB;
else if ( new_tab_number == PRESSURE_CONTROL_TAB && !HavePressureControl )
    new_tab_number = SETUP_SHEET_TAB;

if ( new_tab_number == SETUP_SHEET_TAB && !ShowSetupSheet )
    new_tab_number = BASIC_SETUP_TAB;

if ( new_tab_number != CurrentDialogNumber )
    {
    if ( new_tab_number == PARAMETER_LIMITS_TAB )
        {
        if ( CurrentPasswordLevel >= WindowInfo[PARAMETER_LIMITS_TAB].password_level )
            ShowWindow( ParamDialogWindow, SW_SHOW );
        }
    else if ( CurrentDialogNumber == PARAMETER_LIMITS_TAB )
        {
        if ( IsWindowVisible(ParamDialogWindow) )
            ShowWindow( ParamDialogWindow, SW_HIDE );
        }

    if ( new_tab_number == SURETRAK_PROFILE_TAB )
        SureTrakPlotWindow.show();
    else if ( SureTrakPlotWindow.is_visible() )
        SureTrakPlotWindow.hide();
    }

show_current_dialog( new_tab_number );
}

/***********************************************************************
*                               EBOX_FLOAT                             *
***********************************************************************/
float ebox_float( HWND w, int editbox_id )
{
UINT  n;
TCHAR s[31];

n = GetDlgItemText( w, editbox_id, s, 30 );
if ( n > 0 )
    return extfloat(s, short(n));

return 0.0;
}

/***********************************************************************
*                              MARK_NAME                               *
***********************************************************************/
TCHAR * mark_name( TCHAR type, short number )
{
static TCHAR s[PARAMETER_NAME_LEN+1];
STRING_CLASS sc;
int          i;

lstrcpy( s, UNKNOWN );
i = number;
i--;
if ( type == PARAMETER_MARK )
    {
    if ( i >= 0 && i < CurrentParam.count() )
        lstrcpy( s, CurrentParam.parameter[i].name );
    }
else
    {
    if ( type == START_POS_MARK )
        sc = TEXT("START_POS_MARK_");
    else if ( type == END_POS_MARK )
        sc = TEXT("END_POS_MARK_");
    else
        sc = TEXT("VEL_POS_MARK_");
    sc += (int) number;
    copystring( s, StringTable.find(sc), PARAMETER_NAME_LEN );
    }

return s;
}

/***********************************************************************
*                              MARK_NAME                               *
***********************************************************************/
TCHAR * mark_name( MARKLIST_ENTRY & m )
{
return mark_name( m.parameter_type, m.parameter_number );
}

/***********************************************************************
*                          SHOW_CURRENT_MARK                           *
***********************************************************************/
static void show_current_mark()
{
HWND w;
MARKLIST_ENTRY * m;
COLORS_CLASS c;
int x;
STRING_CLASS s;

w = WindowInfo[PROFILE_MARKS_TAB].dialogwindow;
if ( !w )
    return;

if ( CurrentSelectedMark >= 0 && CurrentSelectedMark < CurrentMarks.n )
    {
    m = &CurrentMarks.array[CurrentSelectedMark];

    /*
    -----
    Color
    ----- */
    s = c.name( m->color );
    s.uppercase();
    set_current_lb_item( w, MARKS_COLOR_LISTBOX_ID, s.text() );

    /*
    ---------
    Line Type
    --------- */
    if ( m->symbol & VLINE_MARK )
        x = MARKS_VLINE_BUTTON;
    else if ( m->symbol & BLINE_MARK )
        x = MARKS_BASE_LINE_BUTTON;
    else
        x = MARKS_NO_LINE_BUTTON;

    CheckRadioButton( w, MARKS_NO_LINE_BUTTON, MARKS_BASE_LINE_BUTTON, x );

    /*
    -----
    Label
    ----- */
    SetDlgItemText( w, MARKS_LABEL_ID, m->label );

    /*
    ------
    Offset
    ------ */
    m->offset.set_text( w, MARKS_OFFSET_EBOX );
    }
}

/***********************************************************************
*                              SHOW_MARKS                              *
***********************************************************************/
static void show_marks()
{
short i;
HWND w;

w = WindowInfo[PROFILE_MARKS_TAB].dialogwindow;
if ( !w )
    return;

SendDlgItemMessage( w, MARKS_LISTBOX_ID, LB_RESETCONTENT, 0, 0L );

if ( !CurrentMarks.n )
    return;

for ( i=0; i<CurrentMarks.n; i++ )
    {
    SendDlgItemMessage( w, MARKS_LISTBOX_ID, LB_ADDSTRING, 0, (LPARAM) ((LPSTR) mark_name(CurrentMarks.array[i])) );
    }

if ( CurrentSelectedMark >= CurrentMarks.n )
    CurrentSelectedMark = CurrentMarks.n - 1;

SendDlgItemMessage( w, MARKS_LISTBOX_ID, LB_SETCURSEL, CurrentSelectedMark, 0L );
show_current_mark();
}

/***********************************************************************
*                       FILL_ANALOG_LIST_BOXES                         *
***********************************************************************/
static void fill_analog_list_boxes( HWND w, int box, int nof_boxes )
{
int           bi;
int           i;
ANALOG_SENSOR a;

for ( i=0; i<a.count(); i++ )
    {
    for ( bi=0; bi<nof_boxes; bi++ )
        SendDlgItemMessage( w, box+bi, CB_ADDSTRING, 0, (LPARAM) a[i].desc.text() );
    }
}

/***********************************************************************
*                       FILL_ANALOG_LIST_BOXES                         *
***********************************************************************/
static void fill_analog_list_boxes()
{
fill_analog_list_boxes( WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow, FT_CHANNEL_1_LISTBOX, MAX_EDITABLE_FTII_CHANNELS );
}

/***********************************************************************
*                              CAT_W_TAB                               *
***********************************************************************/
static void cat_w_tab( TCHAR * dest, const TCHAR * sorc )
{
lstrcat( dest, sorc );
lstrcat( dest, TEXT("\t") );
}

/***********************************************************************
*                               PARMLINE                               *
***********************************************************************/
static TCHAR * parmline( int i )
{
const short LINE_LEN = PARAMETER_NAME_LEN + 1 + UNITS_LEN + 1 + MAX_FLOAT_LEN + 1 + 4*(PARMLIST_FLOAT_LEN+1+WIRE_LEN+1);
static TCHAR s[LINE_LEN+1];
TCHAR wirename[WIRE_LEN+1];
int j;
int k;

*s = NullChar;
cat_w_tab( s, CurrentParam.parameter[i].name );
cat_w_tab( s, units_name(CurrentParam.parameter[i].units) );
cat_w_tab( s, CurrentParam.parameter[i].target.text() );
for ( k=0; k<NOF_ALARM_LIMIT_TYPES; k++ )
    {
    j = AlarmIndex[k];
    cat_w_tab( s, ascii_float(CurrentParam.parameter[i].limits[j].value) );
    int32toasc( wirename, CurrentParam.parameter[i].limits[j].wire_number, DECIMAL_RADIX );
    cat_w_tab( s, wirename );
    }

*(s+lstrlen(s)-1) = NullChar;
return s;
}

/***********************************************************************
*                      CURRENT_PARAM_LISTBOX_INDEX                     *
***********************************************************************/
int current_param_listbox_index()
{
HWND  w;
LRESULT x;

w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;

x = SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_GETCURSEL, 0, 0L);
if ( x == LB_ERR )
    x = NO_PARAMETER_INDEX;

return (int) x;
}

/***********************************************************************
*                   REFRESH_CURRENT_PARAMETER_LIMITS                   *
***********************************************************************/
void refresh_current_parameter_limits()
{
int x;
int parameter;

x = current_param_listbox_index();
if ( x == NO_PARAMETER_INDEX )
    return;

parameter = CurrentSort.parameter_number( x );
ParamLimitLb.replace( parmline(parameter) );
ParamLimitLb.setcursel( x );
}

/***********************************************************************
*                      REFRESH_PARAMETER_LIMITS                        *
***********************************************************************/
static void refresh_parameter_limits()
{
HWND  w;
int i;
int n;
int parameter;
LRESULT x;
LRESULT ti;
STRING_CLASS s;

w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;

x  = SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_GETCURSEL,   0, 0L );
ti = SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_GETTOPINDEX, 0, 0L );
SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_RESETCONTENT, 0, 0L );

n = CurrentParam.count();

if ( x != LB_ERR && x >= LRESULT(n) )
    x = LRESULT(n-1);

/*
--------------------------------------------------
This loads the parameters in the proper sort order
-------------------------------------------------- */
for ( i=0; i<n; i++ )
    {
    parameter = CurrentSort.parameter_number( i );
    SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_ADDSTRING, 0, (LPARAM)(LPSTR) parmline(parameter) );
    }

if ( ti != LB_ERR )
    SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_SETTOPINDEX, ti, 0L );

if ( x != LB_ERR )
    {
    SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_SETCURSEL, x, 0L );
    parameter = CurrentSort.parameter_number( x );
    load_new_parameter( parameter );
    SetFocus( GetDlgItem(w, PARAMETER_LIMIT_LISTBOX) );
    }

/*
-------------
Alarm Timeout
------------- */
if ( CurrentPart.alarm_timeout_seconds < 1 )
    s.null();
else
    s = CurrentPart.alarm_timeout_seconds;

s.set_text( w, ALARM_TIMEOUT_EB );
}

/***********************************************************************
*                          REFRESH_BASIC_SETUP                         *
***********************************************************************/
static void refresh_basic_setup()
{
HWND     w;
int      i;
WPARAM   x;

w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

if ( !w )
    return;

x = INCHES_BUTTON;
switch ( CurrentPart.distance_units )
    {
    case INCH_UNITS:
        x = INCHES_BUTTON;
        break;

    case CM_UNITS:
        x = CM_BUTTON;
        break;

    case MM_UNITS:
        x = MM_BUTTON;
        break;
    }

/*
---------------------------------------
Set the state before setting the button
--------------------------------------- */
for ( i=0; i<NofDistanceButtons; i++ )
    {
    if ( DistanceButton[i] == (int) x )
        DistanceButtonState[i] = TRUE;
    else
        DistanceButtonState[i] = FALSE;
    }

CheckRadioButton( w, INCHES_BUTTON, CM_BUTTON, x );

x = IPS_BUTTON;
switch ( CurrentPart.velocity_units )
    {
    case IPS_UNITS:
        x = IPS_BUTTON;
        break;

    case FPM_UNITS:
        x = FPM_BUTTON;
        break;

    case CMPS_UNITS:
        x = CMPS_BUTTON;
        break;

    case MPS_UNITS:
        x = MPS_BUTTON;
        break;
    }

for ( i=0; i<NofVelocityButtons; i++ )
    {
    if ( VelocityButton[i] == (int) x )
        VelocityButtonState[i] = TRUE;
    else
        VelocityButtonState[i] = FALSE;
    }

CheckRadioButton( w, IPS_BUTTON, MPS_BUTTON, x );

CurrentPart.sp[PARTSET_TOTAL_STROKE_LEN_INDEX].value.set_text( w, TOTAL_STROKE_LEN_EDITBOX );
CurrentPart.sp[PARTSET_MIN_STROKE_LEN_INDEX].value.set_text( w, MIN_STROKE_LEN_EDITBOX );
CurrentPart.sp[PARTSET_EOS_VEL_INDEX].value.set_text( w, EOS_VEL_EDITBOX );
set_checkbox( w, ZERO_SPEED_OUTPUT_ENABLE_XB, CurrentPart.sp[PARTSET_ZERO_SPEED_OUTPUT_ENABLE_INDEX].value.boolean_value() );
CurrentPart.sp[PARTSET_ZERO_SPEED_OUTPUT_WIRE_INDEX].value.set_text( w, ZERO_SPEED_OUTPUT_WIRE_PB );
}

/***********************************************************************
*                          ADD_X_AXIS_TO_PLOT                          *
***********************************************************************/
static void add_x_axis_to_plot()
{
float x;

x = CurrentPart.total_stroke_length;
if ( x > 10.0 )
    {
    x /= 10.0;
    x = ceil(x);
    x *= 10.0;
    }
else
    x = ceil(x);

SureTrakPlot.add_axis( ST_POS_AXIS_ID, X_AXIS_INDEX, 0.0, x, BlackColor );
SureTrakPlot.set_decimal_places( ST_POS_AXIS_ID, 2 );

SureTrakPlot.set_tic_count( X_AXIS_INDEX, 10 );
}

/***********************************************************************
*                        ALL_STEPS_ARE_PERCENT                         *
***********************************************************************/
static BOOLEAN all_steps_are_percent()
{
SureTrakSetup.rewind();

while ( SureTrakSetup.next() )
    {
    if ( !SureTrakSetup.vel_is_percent() )
        return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                          ADD_Y_AXIS_TO_PLOT                          *
***********************************************************************/
static void add_y_axis_to_plot()
{
float ymax;
BOOLEAN all_percent;

all_percent = FALSE;
ymax        = 0.0;

if ( SureTrakSetup.nof_steps() > 0 )
    {
    if ( all_steps_are_percent() )
        {
        ymax = 100.0;
        all_percent = TRUE;
        }
    else
        {
        SureTrakSetup.rewind();
        while ( SureTrakSetup.next() )
            {
            if ( !SureTrakSetup.vel_is_percent() )
                maxfloat( ymax, SureTrakSetup.vel_value() );
            }
        }
    }

if ( !all_percent )
    maxfloat( ymax, SureTrakParam.max_velocity() );

SureTrakPlot.add_axis( ST_VEL_AXIS_ID, Y_AXIS_INDEX, 0.0, ymax, BlackColor );
SureTrakPlot.set_decimal_places( ST_VEL_AXIS_ID, 1 );
SureTrakPlot.set_tic_count( Y_AXIS_INDEX,  5 );
}

/***********************************************************************
*                          ADD_LABELS_TO_PLOT                          *
***********************************************************************/
static void add_labels_to_plot()
{
STRING_CLASS s;
TCHAR      * cp;

SureTrakPlot.empty_labels();

s = StringTable.find( TEXT("POSITION_STRING") );
s += SpaceString;
s += units_name( CurrentPart.distance_units );

SureTrakPlot.add_label( X_AXIS_INDEX, s.text(), BlackColor );

SureTrakPlot.add_label( Y_AXIS_INDEX, StringTable.find(TEXT("VELOCITY_STRING")), BlackColor );


if ( all_steps_are_percent() )
    cp = StringTable.find( TEXT("PERCENT_STRING") );
else
    cp = units_name(CurrentPart.velocity_units);

SureTrakPlot.add_label( Y_AXIS_INDEX, cp, BlackColor );
}

/***********************************************************************
*                          SET_FLOAT_POINT                             *
***********************************************************************/
inline void set_float_point( FLOAT_POINT & p, float x, float y ) { p.x = x; p.y = y; }

/***********************************************************************
*                          ADD_PROFILE_CURVE                           *
***********************************************************************/
static void add_profile_curve()
{
AXIS_ENTRY * ae;
float dx;
float x;
float v;
const int32 MAX_POINTS_PER_STEP = 3;
//int32  i;
int32  n;
BOOLEAN y_is_percent;
BOOLEAN no_end_value;
static FLOAT_POINT p[MAX_ST_STEPS*MAX_POINTS_PER_STEP];

n = 0;
x = 0.0;
v = 0.0;
no_end_value = FALSE;

set_float_point( p[n], x, v );
n++;

if ( SureTrakSetup.nof_steps() > 0 )
    {
    y_is_percent = all_steps_are_percent();

    SureTrakSetup.rewind();
    while ( SureTrakSetup.next() )
        {
        if ( no_end_value )
            {
            dx = SureTrakSetup.end_pos_value();
            if ( !SureTrakSetup.accel_is_percent() )
                dx -= SureTrakSetup.accel_value();
            dx -= x;
            dx /= 2;
            x += dx;
            p[n-1].x = x;
            no_end_value = FALSE;
            }

        if ( !SureTrakSetup.accel_is_percent() )
            x += SureTrakSetup.accel_value();

        if ( SureTrakSetup.vel_is_percent() && !y_is_percent )
            {
            ae = SureTrakPlot.findaxis( ST_VEL_AXIS_ID );
            if ( ae )
                v = ae->max_value * SureTrakSetup.vel_value() / 100.0;
            }
        else
            v =  SureTrakSetup.vel_value();

        set_float_point( p[n], x, v );
        n++;

//      i = NO_INDEX;
//      if ( SureTrakSetup.have_vacuum_wait() )
//          i = SureTrakSetup.vacuum_limit_switch_index();
//      if ( i != NO_INDEX )
//          {
//          x = (float) CurrentLimitSwitch.real_pos( i );
//          }
//      else
//          {
            x = SureTrakSetup.end_pos_value();
            if ( is_float_zero(x) )
                {
                no_end_value = TRUE;
                x = p[n-1].x;
                }
//          }

        set_float_point( p[n], x, v );
        n++;

        if ( findchar(PercentChar, SureTrakSetup.low_impact_string()) )
            {
            ae = SureTrakPlot.findaxis( ST_VEL_AXIS_ID );
            if ( ae )
                v = ae->max_value * SureTrakSetup.low_impact_value() / 100.0;
            }

        set_float_point( p[n], x, v );
        n++;
        }
    }

ae = SureTrakPlot.findaxis( ST_POS_AXIS_ID );
if ( ae )
    {
    if ( x < ae->max_value )
        {
        x = ae->max_value;
        set_float_point( p[n], x, v );
        n++;
        }
    }

if ( n > 1 )
    SureTrakPlot.add_curve( ST_POS_AXIS_ID, ST_VEL_AXIS_ID, n, p );
}

/***********************************************************************
*                    REFRESH_VELOCITY_CONTROL_PLOT                     *
***********************************************************************/
static void refresh_velocity_control_plot()
{
if ( CurrentDialogNumber != SURETRAK_PROFILE_TAB )
    return;

SureTrakPlot.empty_curves();
SureTrakPlot.empty_axes();

add_labels_to_plot();

add_x_axis_to_plot();
add_y_axis_to_plot();
add_profile_curve();

SureTrakPlot.size( SureTrakPlotWindow.handle() );
HaveSureTrakPlot = TRUE;
SureTrakPlotWindow.refresh();
}

/***********************************************************************
*                            REFRESH_ST_PARAMS                         *
***********************************************************************/
void refresh_st_params( HWND w )
{
SureTrakSetup.velocity_loop_gain().set_text( w, VEL_LOOP_GAIN_EBOX );
SureTrakSetup.ft_stop_pos().set_text( w, FT_STOP_POS_EBOX );
SureTrakSetup.gain_break_velocity(0).set_text( w, BREAK_VEL_1_EBOX );
SureTrakSetup.gain_break_gain(0).set_text( w, BREAK_GAIN_1_EBOX );
SureTrakSetup.gain_break_velocity(1).set_text( w, BREAK_VEL_2_EBOX );
SureTrakSetup.gain_break_gain(1).set_text( w, BREAK_GAIN_2_EBOX );
SureTrakSetup.gain_break_velocity(2).set_text( w, BREAK_VEL_3_EBOX );
SureTrakSetup.gain_break_gain(2).set_text( w, BREAK_GAIN_3_EBOX );
}

/***********************************************************************
*                          SET_RUN_IF_ON_TEXT                          *
***********************************************************************/
static void set_run_if_on_text( HWND w )
{
w = GetDlgItem( w, WARMUP_RUN_IF_ON_XBOX );

if ( is_checked(w) )
    set_text(w, StringTable.find(TEXT("ON_STRING")) );
else
    set_text(w, StringTable.find(TEXT("OFF_STRING")) );
}

/***********************************************************************
*                         GET_CURRENT_PARTLIST                         *
***********************************************************************/
static BOOLEAN get_current_partlist( TEXT_LIST_CLASS & dest )
{
MACHINE_ENTRY * m;

m = find_machine_entry( CurrentMachine.name );
if ( m )
    {
    dest = m->partlist;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         REFRESH_WARMUP_SHOT                          *
***********************************************************************/
static void refresh_warmup_shot()
{
HWND            w;
LISTBOX_CLASS   lb;
TEXT_LIST_CLASS t;
BOOLEAN         warmup_part_exists;

if ( CurrentMachine.suretrak_controlled )
    w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;
else
    w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

Warmup.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

lb.init( w, WARMUP_PART_CBOX );
lb.empty();

warmup_part_exists = FALSE;
if ( get_current_partlist(t) )
    {
    t.rewind();
    while ( t.next() )
        {
        if ( CurrentPart.name != t.text() )
            {
            lb.add( t.text() );
            if ( Warmup.partname == t.text() )
                warmup_part_exists = TRUE;
            }
        }

    if ( warmup_part_exists )
        lb.setcursel( Warmup.partname.text() );
    else
        Warmup.is_enabled = FALSE;
    }

set_checkbox( w, WARMUP_ENABLE_XBOX,    Warmup.is_enabled );
set_checkbox( w, WARMUP_RUN_IF_ON_XBOX, Warmup.run_if_on  );
set_run_if_on_text( w );
set_text( w, WARMUP_WIRE_EBOX, Warmup.wire );
}

/***********************************************************************
*                           SAVE_WARMUP_SHOT                           *
***********************************************************************/
static void save_warmup_shot()
{
HWND w;
COMPUTER_CLASS c;
AUTO_BACKUP_CLASS ab;
LISTBOX_CLASS   lb;

if ( CurrentMachine.suretrak_controlled )
    w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;
else
    w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

Warmup.is_enabled = is_checked( w, WARMUP_ENABLE_XBOX );
Warmup.run_if_on  = is_checked( w, WARMUP_RUN_IF_ON_XBOX );
get_text( Warmup.wire, w, WARMUP_WIRE_EBOX, WARMUP_WIRE_LEN );

lb.init( w, WARMUP_PART_CBOX );
Warmup.partname = lb.selected_text();

Warmup.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

if ( CurrentPart.computer == c.whoami() )
    {
    ab.get_backup_dir();
    ab.backup_part( CurrentPart.machine, CurrentPart.name );
    }
}

/***********************************************************************
*                    ENABLE_LAST_STEP_LOW_IMPACT_EB                    *
***********************************************************************/
static void enable_last_step_low_impact_eb()
{
STRING_CLASS s;
int     box;
int     i;
bool    i_own_the_flag;
int     last_step_index;
int     n;
int     old_low_impact_index;
WINDOW_CLASS wc;
WINDOW_CLASS parent;

parent = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

n = SureTrakSetup.nof_steps();
if ( n == 0 )
    return;

last_step_index = n-1;

i_own_the_flag = false;
if ( !LoadingSureTrakSteps )
    {
    LoadingSureTrakSteps = TRUE;
    i_own_the_flag = true;
    }

/*
-----------------------------------------------------
Get the low impact string. There is only one, if any.
Remember the old index so I can set the low impact
editbox as changed if I move the value to a new line.
----------------------------------------------------- */
old_low_impact_index = 0;
SureTrakSetup.rewind();
while ( SureTrakSetup.next() )
    {
    s = SureTrakSetup.low_impact_string();
    if ( !s.isempty() )
        break;
    old_low_impact_index++;
    }

box = LI_1_EDITBOX;
for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    wc = parent.control( box );

    if ( i == last_step_index )
        {
        wc.show();
        wc.set_title( s.text() );
        if ( !s.isempty() && i != old_low_impact_index )
            {
            SureTrakSetup.gotostep( old_low_impact_index + 1 );
            SureTrakSetup.set_low_impact( EmptyString );
            SureTrakSetup.gotostep( i + 1 );
            SureTrakSetup.set_low_impact( s.text() );
            add_to_stmodlist( wc.handle() );
            }
        }
    else
        {
        wc.hide();
        wc.set_title( EmptyString );
        }
    box += CONTROLS_PER_STEP;
    }

if ( i_own_the_flag )
    LoadingSureTrakSteps = FALSE;
}

/***********************************************************************
*                       REFRESH_SURETRAK_SETUP                         *
***********************************************************************/
static void refresh_suretrak_setup()
{
HWND         w;
WINDOW_CLASS wc;
int          box;
int          last_box;
UINT         checkbox_state;
int32        step_number;
int32        i;
int          n;
STRING_CLASS s;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

if ( !w )
    return;

s = StringTable.find( TEXT("LOW_IMPACT") );
s.set_text( w, LOW_IMPACT_STATIC );

show_window( w, FS_POSITION_MSG_TBOX, FALSE );

LoadingSureTrakSteps = TRUE;

step_number = 0;
box         = ACCEL_1_EDITBOX;
SureTrakSetup.rewind();
while ( SureTrakSetup.next() )
    {
    s = SureTrakSetup.accel_string();
    s.set_text( w, box  );

    box++;
    s = SureTrakSetup.vel_string();
    s.set_text( w, box );

    box++;
    s = SureTrakSetup.end_pos_string();
    s.set_text( w, box );

    box++;
    if ( SureTrakSetup.have_vacuum_wait() )
        {
        checkbox_state = BST_CHECKED;
        ShowWindow( GetDlgItem(w, FS_POSITION_MSG_TBOX), SW_SHOWNORMAL );
        }
    else
        {
        checkbox_state = BST_UNCHECKED;
        }

    CheckDlgButton( w, box, checkbox_state );
    box++;

    wc = GetDlgItem( w, box );
    if ( wc.is_enabled() )
        {
        s = SureTrakSetup.low_impact_string();
        s.set_text( wc.handle() );
        }
    //set_text( w, box, SureTrakSetup.low_impact_string() );
    box++;

    if ( SureTrakSetup.have_low_impact_from_input() )
        checkbox_state = BST_CHECKED;
    else
        checkbox_state = BST_UNCHECKED;

    CheckDlgButton( w, box, checkbox_state );
    box++;
    step_number++;
    }

/*
-------------------------
Clear any remaining steps
------------------------- */
while ( step_number < MAX_ST_STEPS )
    {
    set_text( w, box, EmptyString );
    box++;
    set_text( w, box, EmptyString );
    box++;
    set_text( w, box, EmptyString );
    EnableWindow( GetDlgItem(w, box), TRUE );
    box++;
    CheckDlgButton( w, box, BST_UNCHECKED );
    box++;
    set_text( w, box, EmptyString );
    box++;
    CheckDlgButton( w, box, BST_UNCHECKED );
    box++;
    step_number++;
    }

/*
--------------
Limit Switches
-------------- */
n = MAX_FTII_LIMIT_SWITCHES;

for ( i=0; i<n; i++ )
    {
    CurrentLimitSwitch.desc(i).set_text( w, LS1_DESC_EB+i );
    CurrentLimitSwitch.pos(i).set_text(  w, LS1_POS_EB+i );
    }

/*
---------------------------------------------------------------
The limit switch wires can be hidden on the co-options screen.
--------------------------------------------------------------- */
box = LS1_WIRE_PB;
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    wc = GetDlgItem( w, box );
    if ( HideSureTrakLimitSwitchWires )
        {
        wc.hide();
        }
    else
        {
        wc.set_title( CurrentLimitSwitch.wire(i).text() );
        wc.show();
        box++;
        }
    }

for ( i=VAC_SWITCH_5_RADIO; i<=VAC_SWITCH_6_RADIO; i++ )
    {
    wc = GetDlgItem( w, box );
    wc.enable();
    }

last_box = VAC_SWITCH_1_RADIO + n - 1;
box = VAC_SWITCH_1_RADIO + SureTrakSetup.vacuum_limit_switch_index();
if ( box >= VAC_SWITCH_1_RADIO && box <= last_box )
    {
    CheckRadioButton( w, VAC_SWITCH_1_RADIO, last_box, box );
    }
else
    {
    for ( box=VAC_SWITCH_1_RADIO; box<=last_box; box++ )
        set_checkbox( w, box, FALSE );
    }

refresh_st_params( w );

if ( StModList.count() )
    {
    StModList.remove_all();
    HaveStModTbox.hide();
    }

if ( !CurrentMachine.multipart_runlist_settings.enable )
    refresh_warmup_shot();

LoadingSureTrakSteps = FALSE;

refresh_velocity_control_plot();
}

/***********************************************************************
*                      ADD_PRESSURE_CONTROL_CURVE                      *
***********************************************************************/
static void add_pressure_control_curve()
{
AXIS_ENTRY * ae;
float x;
float y;
const int32 MAX_POINTS_PER_STEP = 3;
int32  i;
int32  n;
static FLOAT_POINT p[MAX_POINTS_PER_STEP+MAX_PRESSURE_CONTROL_STEPS*MAX_POINTS_PER_STEP];

n = 0;
x = 0.0;
y = 0.0;

set_float_point( p[n], x, y );
n++;

for ( i=0; i<PressureControl.nof_steps; i++ )
    {
    x += PressureControl.step[i].ramp.float_value();
    y =  PressureControl.step[i].pressure.float_value();
    set_float_point( p[n], x, y );
    n++;

    x = PressureControl.step[i].holdtime.float_value();
    set_float_point( p[n], x, y );
    n++;
    }

ae = SureTrakPlot.findaxis( ST_POS_AXIS_ID );
if ( ae )
    {
    if ( x < ae->max_value )
        {
        x = ae->max_value;
        set_float_point( p[n], x, y );
        n++;
        }
    }

if ( n > 1 )
    SureTrakPlot.add_curve( ST_POS_AXIS_ID, ST_VEL_AXIS_ID, n, p );
}

/***********************************************************************
*                     ADD_LABELS_TO_PRESSURE_PLOT                      *
***********************************************************************/
static void add_labels_to_pressure_plot()
{
ANALOG_SENSOR a;
STRING_CLASS  s;

SureTrakPlot.empty_labels();

s = StringTable.find( TEXT("TIME_STRING") );
s += SpaceString;
s += units_name( MS_UNITS );

SureTrakPlot.add_label( X_AXIS_INDEX, s.text(), BlackColor );

SureTrakPlot.add_label( Y_AXIS_INDEX, StringTable.find(TEXT("PRESSURE_STRING")), BlackColor );

SureTrakPlot.add_label( Y_AXIS_INDEX, units_name(a[PressureControl.sensor_number].units_index()), BlackColor );
}

/***********************************************************************
*                       ADD_Y_AXIS_TO_PRESSURE_PLOT                    *
***********************************************************************/
static void add_y_axis_to_pressure_plot()
{
float ymax;
float ymin;
ANALOG_SENSOR a;

ymin = (float) a[PressureControl.sensor_number].low_value.real_value();
ymax = (float) a[PressureControl.sensor_number].high_value.real_value();

SureTrakPlot.add_axis( ST_VEL_AXIS_ID, Y_AXIS_INDEX, ymin, ymax, BlackColor );
SureTrakPlot.set_decimal_places( ST_VEL_AXIS_ID, 0 );
SureTrakPlot.set_tic_count( Y_AXIS_INDEX,  5 );
}

/***********************************************************************
*                      ADD_X_AXIS_TO_PRESSURE_PLOT                     *
***********************************************************************/
static void add_x_axis_to_pressure_plot()
{
int32 i;

i =  CurrentPart.ms_per_time_sample;
i *= (int32) CurrentPart.nof_time_samples;

SureTrakPlot.add_axis( ST_TIME_AXIS_ID, X_AXIS_INDEX, 0.0, (float) i, BlackColor );
SureTrakPlot.set_decimal_places( ST_TIME_AXIS_ID, 0 );

SureTrakPlot.set_tic_count( X_AXIS_INDEX, 10 );
}

/***********************************************************************
*                    REFRESH_PRESSURE_CONTROL_PLOT                     *
***********************************************************************/
static void refresh_pressure_control_plot()
{
//if ( CurrentDialogNumber != PRESSURE_CONTROL_TAB )
    return;

SureTrakPlot.empty_curves();
SureTrakPlot.empty_axes();

add_labels_to_pressure_plot();

add_x_axis_to_pressure_plot();
add_y_axis_to_pressure_plot();
add_pressure_control_curve();

SureTrakPlot.size( SureTrakPlotWindow.handle() );
HaveSureTrakPlot = TRUE;
SureTrakPlotWindow.refresh();
}

/***********************************************************************
*                              SET_PERCENT                             *
*    Add or remove a percent sign, if necessary, from a string.        *
*    Do nothing if the string is empty. Return TRUE if the string      *
*    has changed.                                                      *
***********************************************************************/
static BOOLEAN set_percent( STRING_CLASS & s, BOOLEAN need_percent )
{
TCHAR * cp;

if ( s.isempty() )
    return FALSE;

cp = s.find( PercentChar );
if ( need_percent )
    {
    if ( !cp )
        {
        s += PercentChar;
        return TRUE;
        }
    }
else
    {
    if ( cp )
        {
        *cp = NullChar;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                           SET_EBOX_PERCENT                           *
*    Add or remove a percent sign, if necessary, from an editbox       *
***********************************************************************/
static void set_ebox_percent( HWND parent, int ebox_id, BOOLEAN need_percent )
{
TCHAR      * cp;
HWND         ebox;
STRING_CLASS s;

ebox = GetDlgItem( parent, ebox_id );
s.get_text( ebox );
if ( s.isempty() )
    return;

cp = s.find( PercentChar );
if ( need_percent )
    {
    if ( !cp )
        {
        s += PercentChar;
        s.set_text( ebox );
        }
    }
else
    {
    if ( cp )
        {
        *cp = NullChar;
        s.set_text( ebox );
        }
    }
}

/***********************************************************************
*                            EBOX_HAS_PERCENT                          *
***********************************************************************/
static BOOLEAN ebox_has_percent( HWND parent, int ebox_id )
{
STRING_CLASS s;

s.get_text( parent, ebox_id );
if ( s.contains(PercentChar) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                        SHOW_NEW_GAIN_CONTROLS                        *
***********************************************************************/
static void show_new_gain_controls( HWND hWnd, BOOLEAN is_enabled )
{
struct CONTROL_ENTRY {
    int id;
    short * password_level;
    };

static CONTROL_ENTRY new_gain_control[] = {
    { PC_LOOP_GAIN_EBOX,       &EditPressureSetupLevel },
    { PC_INTEGRAL_GAIN_EBOX,   &EditPressureSetupLevel },
    { PC_DERIVATIVE_GAIN_EBOX, &EditPressureSetupLevel }
    };

static const int nof_new_gain_controls = sizeof(new_gain_control)/sizeof(CONTROL_ENTRY);

BOOLEAN      b;
int          i;
WINDOW_CLASS w;

if ( is_enabled )
    {
    if ( is_checked(hWnd, ALL_OPEN_LOOP_XBOX) || is_checked(hWnd, PROPORTIONAL_VALVE_XBOX) )
        is_enabled = FALSE;
    }

for( i=0; i<nof_new_gain_controls; i++ )
    {
    w = GetDlgItem( hWnd, new_gain_control[i].id );
    b = is_enabled;
    if ( *new_gain_control[i].password_level > CurrentPasswordLevel )
        b = FALSE;
    w.enable( b );
    }
}

/***********************************************************************
*                        SHOW_NEW_GAIN_CONTROLS                        *
***********************************************************************/
static void show_new_gain_controls( HWND hWnd )
{
show_new_gain_controls( hWnd, is_checked(hWnd, PRES_CONTROL_ENABLE_XBOX) );
}

/***********************************************************************
*                           LABEL_GOOSE_BUTTON                         *
***********************************************************************/
static void label_goose_button( HWND w, BOOLEAN is_percent )
{
ANALOG_SENSOR a;
int           u;

if ( is_checked( w ) )
    {
    if ( is_percent )
        {
        set_text( w, PercentString );
        return;
        }
    u = a[PressureControl.sensor_number].units;
    }
else
    {
    u = MS_UNITS;
    }

set_text( w, units_name(u) );
}

/***********************************************************************
*                           LABEL_GOOSE_BUTTON                         *
*                                                                      *
* I made this version for when you press the button. I need to figure  *
* if the goose command is a pressure or a percent based on the dialog  *
* box contents. The id of the ramp pressure is one less than the       *
* goose button. The id of the setpoint is one more.                    *
*                                                                      *
***********************************************************************/
static void label_goose_button( HWND goose_button, HWND parent, int goose_button_id )
{
ANALOG_SENSOR a;
BOOLEAN       is_percent;
STRING_CLASS  s;
int           u;

is_percent = FALSE;
if ( is_checked(goose_button) )
    {
    if ( is_checked(parent, PROPORTIONAL_VALVE_XBOX) )
        {
        is_percent = TRUE;
        }
    else
        {
        s.get_text( parent, goose_button_id+1 );
        if ( s.contains(PercentChar) )
            {
            is_percent = TRUE;
            }
        else
            {
            s.get_text( parent, goose_button_id-1 );
            if ( s.contains(PercentChar) )
                is_percent = TRUE;
            }
        }

    if ( is_percent )
        {
        set_text( goose_button, PercentString );
        return;
        }

    u = a[PressureControl.sensor_number].units;
    }
else
    {
    u = MS_UNITS;
    }

set_text( goose_button, units_name(u) );
}

/***********************************************************************
*                          CHECK_GOOSE_BUTTON                          *
* If the proportional valve button is checked I need to check the      *
* goose button to see if the label should change.                      *
***********************************************************************/
static void check_goose_button( HWND parent, int goose_button_id )
{
HWND goose_button;

goose_button = GetDlgItem( parent, goose_button_id );
if ( is_checked(goose_button) )
    label_goose_button( goose_button, parent, goose_button_id );
}

/***********************************************************************
*                        DO_ALL_OPEN_LOOP_XBOX                         *
***********************************************************************/
static void do_all_open_loop_xbox( HWND parent )
{
static int xbox[3] = {PC_OPEN_LOOP_1_XBOX, PC_OPEN_LOOP_2_XBOX, PC_OPEN_LOOP_3_XBOX };
INT          id;
BOOLEAN      b;
WINDOW_CLASS pw;

b = is_checked( parent, ALL_OPEN_LOOP_XBOX );
for ( id=0; id<3; id++ )
    set_checkbox( parent, xbox[id], b );

b ^= TRUE;
pw = GetDlgItem( parent, PROPORTIONAL_VALVE_XBOX );
pw.enable( b );

show_new_gain_controls( parent );
}

/***********************************************************************
*                   SHOW_PROPORTIONAL_VALVE_CONTROLS                   *
* If the proportional valve checkbox is checked I need to change       *
* pressure labels to percent and hide the analog sensor listbox        *
***********************************************************************/
static void show_proportional_valve_controls( HWND parent, BOOLEAN is_proportional_valve )
{
bool is_visible;
STRING_CLASS s;
WINDOW_CLASS w;

if ( is_proportional_valve )
    {
    is_visible = false;
    s = StringTable.find( TEXT("PERCENT_STRING") );
    }
else
    {
    is_visible = true;
    s = StringTable.find( TEXT("PRESSURE_STRING") );
    }

s.set_text( parent, PRES_STATIC );

/*
--------------------------------------------------------------------------
Hide the pressure sensor listbox and title if this is a proportional valve
-------------------------------------------------------------------------- */
w = GetDlgItem( parent, PC_PRES_SENSOR_CBOX );
w.show( is_visible );
w = GetDlgItem( parent, PC_PRES_SENSOR_STATIC );
w.show( is_visible );

if ( is_proportional_valve )
    s = StringTable.find( TEXT("RETRACT_PERCENT") );
else
    s = StringTable.find( TEXT("RETRACT_VOLTS") );
s.set_text( parent, RETRACT_VOLTS_STATIC );

if ( is_proportional_valve )
    s = StringTable.find( TEXT("VALVE_PARK_PERCENT") );
else
    s = StringTable.find( TEXT("VALVE_PARK_VOLTS") );
s.set_text( parent, VALVE_PARK_VOLTS_STATIC );

show_new_gain_controls( parent );
}

/***********************************************************************
*                       CONVERT_PERCENT_TO_PRESSURE                    *
* Convert a string from percent to pressure using the pressure sensor  *
***********************************************************************/
static void convert_percent_to_pressure( STRING_CLASS & s )
{
const double max_percent = 100.0;
const double smallest_increment = 0.001;

ANALOG_SENSOR a;
double percent;
double range;
double x;
TCHAR * cp;

range = a[PressureControl.sensor_number].high_value.real_value() - a[PressureControl.sensor_number].low_value.real_value();
percent = s.real_value();
percent *= 2.0;
percent /= max_percent;
x = a[PressureControl.sensor_number].low_value.real_value() + percent * range;
round( x, smallest_increment );
s = x;
/*
---------------------------------------------------------------------
Remove the .0 from the end of the string if there is no decimal value
--------------------------------------------------------------------- */
if ( s.len() > 2 )
    {
    cp = s.last_char();
    if ( *cp == ZeroChar )
        {
        cp--;
        if ( *cp == PeriodChar )
            *cp = NullChar;
        }
    }
}

/***********************************************************************
*                       CONVERT_PRESSURE_TO_PERCENT                    *
* Convert a string from pressure to percent using the pressure sensor  *
***********************************************************************/
static void convert_pressure_to_percent( STRING_CLASS & s )
{
const double max_percent        = 100.0;
const double smallest_increment = 0.001;

ANALOG_SENSOR a;
double percent;
double range;
double pressure;

range = a[PressureControl.sensor_number].high_value.real_value() - a[PressureControl.sensor_number].low_value.real_value();
if ( is_zero(range) )
    range = 1.0;
pressure = s.real_value();
percent  = pressure - a[PressureControl.sensor_number].low_value.real_value();
percent *= max_percent;
percent /= range;
percent /= 2.0;
round( percent, smallest_increment );
s = percent;
s += PercentString;
s.replace( TEXT(".0%"), PercentString );
}

/***********************************************************************
*                       CONVERT_PERCENT_TO_VOLTS                       *
* Convert a string from percent to volts using the                     *
* current voltage output range                                         *
* e.g. if volt range is +-10v then convert -100% to -10 volts          *
***********************************************************************/
static void convert_percent_to_volts( STRING_CLASS & s )
{
const double max_percent = 100.0;
const double max_volts   =  10.0;
const double smallest_increment = 0.001;
double v;

v = s.real_value();
v *= max_volts;
v /= max_percent;
round( v, smallest_increment );
s = v;
}

/***********************************************************************
*                       CONVERT_VOLTS_TO_PERCENT                       *
* Convert a string from volts to percent using the                     *
* current voltage output range                                         *
* e.g. if volt range is +-10v then convert -10 volts to -100%          *
***********************************************************************/
static void convert_volts_to_percent( STRING_CLASS & s )
{
const double max_percent = 100.0;
const double max_volts   =  10.0;
const double smallest_increment = 0.01;
double v;

v = s.real_value();
v *= max_percent;
v /= max_volts;
round( v, smallest_increment );
s = v;
s += PercentString;
}

/***********************************************************************
*                      TOGGLE_PROPORTIONAL_VALVE                       *
***********************************************************************/
static void toggle_proportional_valve( HWND parent )
{
static int pressurebox[MAX_PRESSURE_CONTROL_STEPS] = {PRES_1_EBOX, PRES_2_EBOX, PRES_3_EBOX };
static int old_volt_range_radio = PC_5_VOLT_RANGE_RADIO;

BOOLEAN      is_proportional_valve;
int          id;
STRING_CLASS s;
HWND         w;

TogglingProportionalValve = TRUE;
is_proportional_valve = is_checked( parent, PROPORTIONAL_VALVE_XBOX );

PressureControl.park_volts.get_text( parent, PARK_PRES_EBOX );
PressureControl.retract_volts.get_text( parent, RETRACT_PRES_EBOX );
w = GetDlgItem( parent, ALL_OPEN_LOOP_XBOX );
if ( is_proportional_valve )
    {
    if ( is_checked(w) )
        {
        set_checkbox( w, FALSE );
        do_all_open_loop_xbox( parent );
        }
    EnableWindow( w, FALSE );

    for ( id=PC_2_VOLT_RANGE_RADIO; id<=PC_10_VOLT_RANGE_RADIO; id++ )
        {
        if ( is_checked(parent, id) )
            {
            old_volt_range_radio = id;
            break;
            }
        }
    /*
    -------------------------------------------------
    Hide the goose buttons and show the MS text boxes
    ------------------------------------------------- */
    for ( id=0; id<MAX_PRESSURE_CONTROL_STEPS; id++ )
        {
        show_window( parent, GooseBox[id], FALSE );
        show_window( parent, MsBox[id],    TRUE );
        }

    CheckRadioButton( parent, PC_2_VOLT_RANGE_RADIO, PC_10_VOLT_RANGE_RADIO, PC_10_VOLT_RANGE_RADIO );
    convert_volts_to_percent( PressureControl.park_volts );
    convert_volts_to_percent( PressureControl.retract_volts );
    }
else
    {
    /*
    -------------------------------------------------
    Hide the ms buttons and show the goose text boxes
    ------------------------------------------------- */
    for ( id=0; id<MAX_PRESSURE_CONTROL_STEPS; id++ )
        {
        show_window( parent, GooseBox[id], TRUE );
        show_window( parent, MsBox[id],    FALSE );
        }
    CheckRadioButton( parent, PC_2_VOLT_RANGE_RADIO, PC_10_VOLT_RANGE_RADIO, old_volt_range_radio );
    EnableWindow( w, TRUE );
    convert_percent_to_volts( PressureControl.park_volts );
    convert_percent_to_volts( PressureControl.retract_volts );
    }

LoadingPressureControl = TRUE;
PressureControl.park_volts.set_text( parent, PARK_PRES_EBOX );
PressureControl.retract_volts.set_text( parent, RETRACT_PRES_EBOX );
for ( id=0; id<MAX_PRESSURE_CONTROL_STEPS; id++ )
    {
    s.get_text( parent, pressurebox[id] );
    if ( s.isempty() )
        continue;

    if ( is_proportional_valve )
        convert_pressure_to_percent( s );
    else
        convert_percent_to_pressure( s );

    s.set_text( parent, pressurebox[id] );
    }
LoadingPressureControl = FALSE;

for ( id=0; id<MAX_PRESSURE_CONTROL_STEPS; id++ )
    {
    /*
    -------------------------------------------------
    Add or remove the percent sign from the ramp ebox
    The two lines below are not needed any more because only MS units are used for ramping proportional valves.
    if ( is_checked(parent, GooseBox[id]) )
        set_ebox_percent( parent, GooseBox[id]-1, is_proportional_valve );
    ------------------------------------------------- */
    set_ebox_percent( parent, GooseBox[id]+1, is_proportional_valve );
    check_goose_button( parent, GooseBox[id] );
    }

show_proportional_valve_controls( parent, is_proportional_valve );
TogglingProportionalValve = FALSE;
}

/**********************************************************************
*                      show_retctl_controls                           *
**********************************************************************/
static void show_retctl_controls( HWND hWnd )
{
static int control[] = { RETCTL_ARM_PRES_EBOX, RETCTL_PRES_EBOX };
const int n = sizeof(control) / sizeof( int );

int          i;
BOOLEAN      is_enabled;
WINDOW_CLASS w;

is_enabled = TRUE;
if ( EditPressureProfileLevel > CurrentPasswordLevel )
    is_enabled = FALSE;

w = GetDlgItem( hWnd, RETCTL_ENABLE_XBOX );
w.enable( is_enabled );

if ( is_enabled )
    is_enabled = is_checked( w.handle() );

for ( i=0; i<n; i++ )
    {
    w = GetDlgItem( hWnd, control[i] );
    w.enable( is_enabled );
    }
}

/**********************************************************************
*          ENABLE_ALL_OPEN_LOOP_AND_PROPORTIONAL_VALVE_BUTTONS        *
*  I assume the pressure control is enabled and the pw level is ok.   *
*  If either the ALL_OPEN_LOOP_XBOX or PROPORTIONAL_VALVE_XBOX are    *
*  checked I disable the other. If both are checked I disable neither *
*  so the operator can fix the problem.                               *
**********************************************************************/
static void enable_all_open_loop_and_portional_valve_buttons( HWND parent )
{
BOOLEAN a_is_enabled;
BOOLEAN p_is_enabled;
BOOLEAN a_is_checked;
BOOLEAN p_is_checked;
WINDOW_CLASS a;
WINDOW_CLASS p;

a_is_enabled = TRUE;
p_is_enabled = TRUE;

a = GetDlgItem( parent, ALL_OPEN_LOOP_XBOX );
p = GetDlgItem( parent, PROPORTIONAL_VALVE_XBOX );
a_is_checked = is_checked( a.handle() );
p_is_checked = is_checked( p.handle() );
if ( a_is_checked && !p_is_checked )
    p_is_enabled = FALSE;
else if ( !a_is_checked && p_is_checked )
    a_is_enabled = FALSE;

a.enable( a_is_enabled );
p.enable( p_is_enabled );
}

/***********************************************************************
*                          SHOW_PRES_CONTROLS                          *
***********************************************************************/
static void show_pres_controls( HWND hWnd )
{
struct CONTROL_ENTRY {
    int id;
    short * password_level;
    };

static CONTROL_ENTRY control[] = {
    { RAMP_1_EBOX,             &EditPressureProfileLevel },
    { PC_GOOSE_1_XBOX,         &EditPressureProfileLevel },
    { PRES_1_EBOX,             &EditPressureProfileLevel },
    { HOLD_TIME_1_EBOX,        &EditPressureProfileLevel },
    { RAMP_2_EBOX,             &EditPressureProfileLevel },
    { PC_GOOSE_2_XBOX,         &EditPressureProfileLevel },
    { PRES_2_EBOX,             &EditPressureProfileLevel },
    { HOLD_TIME_2_EBOX,        &EditPressureProfileLevel },
    { RAMP_3_EBOX,             &EditPressureProfileLevel },
    { PC_GOOSE_3_XBOX,         &EditPressureProfileLevel },
    { PRES_3_EBOX,             &EditPressureProfileLevel },
    { HOLD_TIME_3_EBOX,        &EditPressureProfileLevel },
    { PC_PRES_SENSOR_CBOX,     &EditPressureSetupLevel   },
    { INTENS_ARM_PRES_EBOX,    &EditPressureSetupLevel   },
    { INITIAL_PRES_EBOX,       &EditPressureSetupLevel   },
    { RETRACT_PRES_EBOX,       &EditPressureSetupLevel   },
    { PARK_PRES_EBOX,          &EditPressureSetupLevel   },
    { PC_2_VOLT_RANGE_RADIO,   &EditPressureSetupLevel   },
    { PC_5_VOLT_RANGE_RADIO,   &EditPressureSetupLevel   },
    { PC_10_VOLT_RANGE_RADIO,  &EditPressureSetupLevel   },
    { POSITIVE_ONLY_XBOX,      &EditPressureSetupLevel   },
    { PROPORTIONAL_VALVE_XBOX, &EditPressureSetupLevel   },
    { PC_ARM_LS_CBOX,          &EditPressureSetupLevel   },
    { ALL_OPEN_LOOP_XBOX,      &EditPressureSetupLevel   },
    { DIGITAL_POT_GAIN_EBOX,   &EditPressureSetupLevel   }
    };

static const int nof_controls = sizeof(control)/sizeof(CONTROL_ENTRY);

static CONTROL_ENTRY retract_control[] = {
    { RETCTL_ENABLE_XBOX,      &EditPressureSetupLevel   },
    { RETCTL_ARM_PRES_EBOX,    &EditPressureSetupLevel   },
    { RETRACT_PRES_EBOX,       &EditPressureSetupLevel   }
    };
static const int nof_retract_controls = sizeof(retract_control)/sizeof(CONTROL_ENTRY);

BOOLEAN      b;
BOOLEAN      is_enabled;
int          i;
int          id;
WINDOW_CLASS w;

if ( EditPressureSetupLevel > CurrentPasswordLevel )
    {
    b = FALSE;
    is_enabled = FALSE;
    }
else
    {
    b = TRUE;
    is_enabled = is_checked(hWnd, PRES_CONTROL_ENABLE_XBOX );
    }

w = GetDlgItem( hWnd, PRES_CONTROL_ENABLE_XBOX );
w.enable( b );

for( i=0; i<nof_controls; i++ )
    {
    id = control[i].id;
    w = GetDlgItem( hWnd, id );
    b = is_enabled;
    if ( *control[i].password_level > CurrentPasswordLevel )
        b = FALSE;
    if ( b && (id==ALL_OPEN_LOOP_XBOX || id==PROPORTIONAL_VALVE_XBOX) )
        continue;
    w.enable( b );
    }

if ( is_enabled )
    enable_all_open_loop_and_portional_valve_buttons( hWnd );

show_retctl_controls( hWnd );
show_new_gain_controls( hWnd, is_enabled );
}

/***********************************************************************
*                    LOAD_PC_ARM_LIMIT_SWITCH_CBOX                     *
***********************************************************************/
static void load_pc_arm_limit_switch_cbox( HWND parent )
{
int           i;
LISTBOX_CLASS lb;
STRING_CLASS  s;

lb.init( parent, PC_ARM_LS_CBOX );
lb.redraw_off();
lb.empty();
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    s = CurrentLimitSwitch.desc(i);
    if ( s.isempty() )
        {
        s = TEXT( "LS" );
        s += i+1;
        }
    lb.add( s.text() );
    }
lb.redraw_on();

i = PressureControl.arm_intensifier_ls_number;
if ( i > MAX_FTII_LIMIT_SWITCHES )
    i = 0;

if ( i > 0 )
    i--;

lb.setcursel( i );
}

/***********************************************************************
*                    REFRESH_PRESSURE_CONTROL_SETUP                    *
***********************************************************************/
static void refresh_pressure_control_setup()
{
BOOLEAN       b;
ANALOG_SENSOR a;
int32         i;
int32         id;
HWND          w;
HWND          button;
LISTBOX_CLASS lb;
STRING_CLASS  s;
PRES_CONTROL_STEP_ENTRY * se;

w = WindowInfo[PRESSURE_CONTROL_TAB].dialogwindow;

if ( !w )
    return;

LoadingPressureControl = TRUE;

lb.init( w, PC_PRES_SENSOR_CBOX );
lb.setcursel( a[PressureControl.sensor_number].desc.text() );
i = PressureControl.voltage_output_range;
if ( i < 0 || i > 2 )
    i = 1;
CheckRadioButton( w, PC_2_VOLT_RANGE_RADIO, PC_10_VOLT_RANGE_RADIO, PC_2_VOLT_RANGE_RADIO+i );

set_checkbox( w, PRES_CONTROL_ENABLE_XBOX, PressureControl.is_enabled );
set_checkbox( w, POSITIVE_ONLY_XBOX,       PressureControl.use_only_positive_voltages );
set_checkbox( w, RETCTL_ENABLE_XBOX,       PressureControl.retctl_is_enabled );
set_checkbox( w, PROPORTIONAL_VALVE_XBOX,  PressureControl.proportional_valve );

if ( PressureControl.proportional_valve )
    {
    /*
    -------------------------------------------------
    Hide the goose buttons and show the MS text boxes
    ------------------------------------------------- */
    for ( id=0; id<MAX_PRESSURE_CONTROL_STEPS; id++ )
        {
        show_window( w, GooseBox[id], FALSE );
        show_window( w, MsBox[id],    TRUE );
        }
    }
else
    {
    /*
    -------------------------------------------------
    Hide the ms text boxes and show the goose buttons
    ------------------------------------------------- */
    for ( id=0; id<MAX_PRESSURE_CONTROL_STEPS; id++ )
        {
        show_window( w, GooseBox[id], TRUE );
        show_window( w, MsBox[id],    FALSE );
        }
    }

id = RAMP_1_EBOX;
for ( i=0; i<PressureControl.nof_steps; i++ )
    {
    se = &PressureControl.step[i];
    s = se->ramp;
    if ( PressureControl.proportional_valve && se->is_goose_percent )
        s.replace( PercentChar, NullChar );
    s.set_text( w, id );
    id++;
    button = GetDlgItem( w, id );
    set_checkbox( button, se->is_goose_step );
    label_goose_button( button, se->is_goose_percent );
    id++;
    se->pressure.set_text( w, id );
    id++;
    set_checkbox( w, id, se->is_open_loop );
    id++;
    se->holdtime.set_text( w, id );
    id++;
    }

while ( i < MAX_PRESSURE_CONTROL_STEPS )
    {
    set_text( w, id, EmptyString );
    id++;
    button = GetDlgItem( w, id );
    set_checkbox( button, FALSE );
    label_goose_button( button, FALSE );
    id++;
    set_text( w, id, EmptyString );
    id++;
    set_checkbox( w, id, FALSE );
    id++;
    set_text( w, id, EmptyString );
    id++;
    i++;
    }

b = FALSE;
for ( i=0; i<PressureControl.nof_steps; i++ )
    {
    if ( PressureControl.step[i].is_open_loop )
        b = TRUE;
    }

button = GetDlgItem( w, ALL_OPEN_LOOP_XBOX );
set_checkbox( button, b );

PressureControl.retctl_arm_pres.set_text( w, RETCTL_ARM_PRES_EBOX );
PressureControl.retctl_pres.set_text( w, RETCTL_PRES_EBOX );
PressureControl.pressure_loop_gain.set_text( w, PC_LOOP_GAIN_EBOX );
PressureControl.integral_gain.set_text( w, PC_INTEGRAL_GAIN_EBOX );
PressureControl.derivative_gain.set_text( w, PC_DERIVATIVE_GAIN_EBOX );
PressureControl.retract_volts.set_text( w, RETRACT_PRES_EBOX );
PressureControl.park_volts.set_text( w, PARK_PRES_EBOX );
PressureControl.digital_pot_gain.set_text( w, DIGITAL_POT_GAIN_EBOX );

show_proportional_valve_controls( w, PressureControl.proportional_valve );
show_pres_controls( w );

b = is_checked( w, ALL_OPEN_LOOP_XBOX );
if ( b && !PressureControl.proportional_valve )
    {
    button = GetDlgItem(w, PROPORTIONAL_VALVE_XBOX );
    EnableWindow( button, FALSE );
    }
else if ( PressureControl.proportional_valve && !b )
    {
    button = GetDlgItem( w, ALL_OPEN_LOOP_XBOX );
    EnableWindow( button, FALSE );
    }

if ( PressureControl.retctl_is_enabled )
    show_retctl_controls( w );

load_pc_arm_limit_switch_cbox( w );

LoadingPressureControl = FALSE;

//refresh_pressure_control_plot();
}

/***********************************************************************
*                      ENABLE_SN_TEMPLATE_EDITBOX                      *
***********************************************************************/
static void enable_sn_template_editbox()
{
static int id[] = { RESET_SHOT_COUNT_ENABLE_XB, RESET_TIME_EB, PURGE_ON_RESET_XB };
static int nof_ids = sizeof(id)/sizeof(int);
BOOLEAN      b;
int          i;
WINDOW_CLASS parent;
WINDOW_CLASS wc;

/*
--------------------------
Shot Name Template Editbox
-------------------------- */
parent = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;
wc     = parent.control( SN_TEMPLATE_EDITBOX );

b = TRUE;
if ( ChangeShotNameLevel > CurrentPasswordLevel )
    b = FALSE;

wc.enable( b );

/*
----------------------------------
Enable External Shot Name Checkbox
---------------------------------- */
wc = parent.control( SNF_ENABLE_XB );

b = TRUE;
if ( ChangeExternalShotNameLevel > CurrentPasswordLevel )
    b = FALSE;

wc.enable( b );

/*
------------------------
Shot Name Reset Controls
------------------------ */
b = TRUE;
if ( ChangeShotResetLevel > CurrentPasswordLevel )
    b = FALSE;

for ( i=0; i<nof_ids; i++ )
    {
    wc = parent.control( id[i] );
    wc.enable( b );
    }
}

/***********************************************************************
*                     SHOW_ADVANCED_MULTIPART_SETUP                    *
***********************************************************************/
static void show_advanced_multipart_setup()
{
int  i;
HWND w;

w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

if ( !w )
    return;

if ( CurrentPart.multipart_runlist_wires.wires == ZeroString )
    set_text( w, MULTIPART_RUNLIST_WIRES_TB, EmptyString );
else
    CurrentPart.multipart_runlist_wires.wires.set_text( w, MULTIPART_RUNLIST_WIRES_TB );
i = CurrentMachine.multipart_runlist_settings.part_selection_number( CurrentPart.multipart_runlist_wires.wires );
set_int_in_ebox( w, MULTIPART_RUNLIST_PSN_TB, i );
}

/***********************************************************************
*                       REFRESH_ADVANCED_SETUP                         *
***********************************************************************/
static void refresh_advanced_setup()
{
EXTERNAL_PARAMETER_SETTINGS_CLASS eps;
EXTERNAL_SHOT_NAME_CLASS          esn;
int                               i;
UINT                              id;
NAME_CLASS                        s;
STRING_CLASS                      shot_template;
int                               show_cmd;
SHOT_NAME_CLASS                   sn;
SETPOINT_CLASS                    sp;
SHOT_NAME_RESET_CLASS             sr;
HWND                              w;
WINDOW_CLASS                      wc;
double                            x;

static UINT multipart_runlist_ids[] =
    {
    MULTIPART_RUNLIST_PSN_TB,
    MULTIPART_RUNLIST_WIRES_TB,
    MULTIPART_RUNLIST_STATIC,
    MULTIPART_RUNLIST_PSN_STATIC,
    MULTIPART_RUNLIST_WIRES_STATIC,
    MULTIPART_RUNLIST_SETUP_PB
    };
static int nof_multipart_runlist_ids = sizeof(multipart_runlist_ids)/sizeof(UINT);

static UINT warmup_ids[] =
    {
    WARMUP_WIRE_EBOX,
    WARMUP_PART_CBOX,
    WARMUP_ENABLE_XBOX,
    WARMUP_RUN_IF_ON_XBOX,
    WARMUP_GROUP_STATIC,
    WARMUP_IF_STATIC,
    WARMUP_IS_STATIC
    };
static int nof_warmup_ids = sizeof(warmup_ids)/sizeof(UINT);

w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

if ( !w )
    return;

CurrentPart.sp[PARTSET_PLUNGER_DIA_INDEX].value.set_text(          w, PLUNGER_DIAMETER_EDITBOX       );
CurrentPart.sp[PARTSET_SLEEVE_FILL_DIST_INDEX].value.set_text(     w, SLEEVE_FILL_DISTANCE_EDITBOX   );
CurrentPart.sp[PARTSET_RUNNER_FILL_DIST_INDEX].value.set_text(     w, RUNNER_FILL_DISTANCE_EDITBOX   );
CurrentPart.sp[PARTSET_MIN_CSFS_POS_INDEX].value.set_text(         w, CSFS_MIN_POSITION_EDITBOX      );
CurrentPart.sp[PARTSET_MIN_CSFS_VEL_INDEX].value.set_text(         w, CSFS_MIN_VELOCITY_EDITBOX      );
CurrentPart.sp[PARTSET_RISE_CSFS_VEL_INDEX].value.set_text(        w, CSFS_RISE_VELOCITY_EDITBOX     );
CurrentPart.sp[PARTSET_TIME_FOR_INTENS_PRES_INDEX].value.set_text( w, TIME_FOR_INTENS_PRES_EDITBOX   );
CurrentPart.sp[PARTSET_PRES_FOR_RESP_TIME_INDEX].value.set_text(   w, PRES_FOR_RESPONSE_TIME_EDITBOX );

x = (double) CurrentPart.biscuit_time_delay;
if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT && CurrentPart.biscuit_time_delay > 10 )
    x /= 1000.0;
SetDlgItemText( w, BISCUIT_TIME_DELAY_EDITBOX, ascii_double(x) );

if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT )
    id = TIME_TO_MEASURE_BISCUIT_STRING;
else
    id = BISCUIT_TIME_DELAY_STRING;
set_text( w, BISCUIT_TIME_DELAY_STATIC, resource_string(id) );

/*
-----------------------------------
Fill the shot name template editbox
----------------------------------- */
sn.init( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
sn.get();
sn.get_shot_name_template( shot_template );
shot_template.set_text( w, SN_TEMPLATE_EDITBOX );

/*
----------------------------------
Fill the reset shot count setboxes
---------------------------------- */
sr.init( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
sr.get();
set_checkbox( w, RESET_SHOT_COUNT_ENABLE_XB, sr.is_enabled() );
sr.reset_time().set_text( w, RESET_TIME_EB );
set_checkbox( w, PURGE_ON_RESET_XB, sr.purge_on_reset() );

/*
--------------------------------
External Shot Name File Settings
-------------------------------- */
esn.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
set_checkbox( w, SNF_ENABLE_XB, esn.is_enabled() );
esn[EXTERNAL_SHOT_NAME_FILE_NAME_INDEX].value.set_text( w, SNF_NAME_EB );
esn[EXTERNAL_SHOT_NAME_FILE_COMMAND_INDEX].value.set_text( w, SNF_COMMAND_EB );

enable_sn_template_editbox();

/*
---------------------------
External Parameter Settings
--------------------------- */
eps.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
eps.file_name.set_text( w, EXTERNAL_PARAMETER_FILE_NAME_EB );
set_checkbox( w, EXTERNAL_PARAMETER_FILE_ENABLE_XB, eps.enable );
set_checkbox( w, EXTERNAL_PARAMETER_FILE_IS_EOS_XB, eps.available_at_eos );

s.get_impact_param_file_csvname( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
if ( s.file_exists() )
    {
    sp.get( s );
    set_checkbox( w, IMPACT_PARAM_FILE_ENABLE_XB, sp[IMPACT_PARAM_FILE_ENABLED_INDEX].boolean_value() );
    sp[IMPACT_PARAM_FILE_NAME_INDEX].value.set_text( w, IMPACT_PARAM_FILE_NAME_EB );
    }
else
    {
    set_checkbox( w, IMPACT_PARAM_FILE_ENABLE_XB, FALSE );
    set_text( w, IMPACT_PARAM_FILE_NAME_EB, EmptyString );
    }

/*
-------------------------------------------
Show or hide the Multipart Runlist Controls
------------------------------------------- */
show_cmd = SW_HIDE;
if ( CurrentMachine.multipart_runlist_settings.enable )
    show_cmd = SW_SHOW;

for ( i=0; i<nof_multipart_runlist_ids; i++ )
    {
    wc = GetDlgItem( w, multipart_runlist_ids[i] );
    wc.show( show_cmd );
    }

if ( show_cmd == SW_SHOW )
    show_advanced_multipart_setup();

/*
-------------------------------------
Show or hide the Warmup Shot Controls
------------------------------------- */
if ( (show_cmd == SW_SHOW) || CurrentMachine.suretrak_controlled )
    show_cmd = SW_HIDE;
else
    show_cmd = SW_SHOW;

for ( i=0; i<nof_warmup_ids; i++ )
    {
    wc = GetDlgItem( w, warmup_ids[i] );
    wc.show( show_cmd );
    }

/*
if ( CurrentPart.setpoint[0].is_numeric() )
    s = CurrentPart.setpoint[0].text();
else
    s = EmptyString;
SetDlgItemText( w, END_OF_FS_VELOCITY_EDITBOX, s );
*/
}

/***********************************************************************
*                           ADD_ONE_PARAMETER                          *
***********************************************************************/
BOOLEAN add_one_parameter( short new_parameter_type )
{
ANALOG_SENSOR a;
int  i;
INI_CLASS  ini;
NAME_CLASS s;
HWND w;

i = CurrentParam.count();
if ( i >= MAX_PARMS )
    return FALSE;

CurrentParam.parameter[i].input.type = new_parameter_type;
if ( new_parameter_type == FT_DIGITAL_INPUT )
    {
    CurrentParam.parameter[i].vartype = DIGITAL_INPUT_VAR;
    CurrentParam.parameter[i].units   = DIGITAL_INPUT_UNITS;
    }
else if ( new_parameter_type == FT_EXTENDED_ANALOG_INPUT )
    {
    CurrentParam.parameter[i].input.number = 0;
    a.find( CurrentPart.analog_sensor[0] );
    CurrentParam.parameter[i].units        = a.units_index();
    CurrentParam.parameter[i].vartype      = a.vartype();
    CurrentFtAnalog.array[i].channel       = 1;
    CurrentFtAnalog.array[i].result_type   = SINGLE_POINT_RESULT_TYPE;
    CurrentFtAnalog.array[i].ind_value     = ZeroString;  /* 0 => No Wire */
    CurrentFtAnalog.array[i].ind_var_type  = TRIGGER_WHEN_GREATER;
    CurrentFtAnalog.array[i].end_value     = ZeroString;
    }
else if ( new_parameter_type == FT_PEAK_FS_VELOCITY )
    {
    CurrentParam.parameter[i].input.number = 0;
    CurrentParam.parameter[i].units        = CurrentPart.velocity_units;
    CurrentParam.parameter[i].vartype      = VELOCITY_VAR;
    CurrentFtAnalog.array[i].channel       = FT_ANALOG_VELOCITY_CHANNEL;
    CurrentFtAnalog.array[i].result_type   = MAX_RESULT_TYPE;
    CurrentFtAnalog.array[i].ind_value     = TEXT( "P13" );
    CurrentFtAnalog.array[i].ind_var_type  = POSITION_VAR;
    CurrentFtAnalog.array[i].end_value     = TEXT( "P32" );
    /*
    ----------------------------------------------------------------------------
    The FTANALOG_CLASS initializes the smooth. Replace this if I have a default.
    ---------------------------------------------------------------------------- */
    s.get_local_ini_file_name( EditPartIniFile );
    if ( s.file_exists() )
        {
        ini.set_file( s.text() );
        ini.set_section( ConfigSection );
        if ( ini.find(PfsvDefSmoothNKey) )
            CurrentFtAnalog.array[i].smooth_n = ini.get_string();
        if ( ini.find(PfsvDefSmoothMultiplierKey) )
            CurrentFtAnalog.array[i].smooth_multiplier = ini.get_string();
        }
    }
else if ( new_parameter_type == FT_EXTERNAL_PARAMETER )
    {
    CurrentParam.parameter[i].input.number = 0;
    CurrentParam.parameter[i].units        = NO_UNITS;
    CurrentParam.parameter[i].vartype      = TEXT_VAR;
    }
else if ( new_parameter_type == FT_STATIC_TEXT_PARAMETER )
    {
    CurrentParam.parameter[i].input.number = 0;
    CurrentParam.parameter[i].units        = NO_UNITS;
    CurrentParam.parameter[i].vartype      = TEXT_VAR;
    CurrentParam.parameter[i].static_text.init( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    CurrentParam.parameter[i].static_text.parameter_number = i+1;
    }

if ( new_parameter_type == FT_PEAK_FS_VELOCITY )
    lstrcpyn( CurrentParam.parameter[i].name, StringTable.find(TEXT("PEAK_FS_VELOCITY")), PARAMETER_NAME_LEN );
else
    lstrcpyn( CurrentParam.parameter[i].name, StringTable.find(TEXT("NEW_PARM_NAME")), PARAMETER_NAME_LEN );

w = WindowInfo[PARAMETER_LIMITS_TAB].dialogwindow;
refresh_parameter_limits();

load_new_parameter( i );
SendDlgItemMessage( w, PARAMETER_LIMIT_LISTBOX, LB_SETCURSEL, CurrentSort.index(i), 0L );
return TRUE;
}

/***********************************************************************
*                         REMOVE_CURRENT_PARAMETER                     *
***********************************************************************/
static void remove_current_parameter()
{
int x;
int true_index;

x = current_param_listbox_index();
if ( x == NO_PARAMETER_INDEX )
    return;

true_index = CurrentSort.parameter_number( x );

if ( CurrentParam.parameter[true_index].input.type == INTERNAL_PARAMETER )
    {
    stringtable_message_box( TEXT("NO_DEL_INTERNAL_PARAM"), TEXT("CANNOT_COMPLY_STRING") );
    return;
    }

CurrentSort.remove( true_index );
CurrentParam.remove( true_index );
CurrentFtAnalog.remove( true_index );
CurrentFtCalc.remove( true_index );

/*
----------------------------
Reload the parameter listbox
---------------------------- */
refresh_parameter_limits();

/*
-----------------------------------------------------------------
True_index now points to the parameter after the one just removed
I need to make that the currently selected index.
----------------------------------------------------------------- */
if ( true_index >= CurrentParam.count() )
    true_index--;

if ( true_index >= 0 )
    {
    load_new_parameter( true_index );
    x = CurrentSort.index( true_index );
    ParamLimitLb.setcursel( x );
    }
}

/***********************************************************************
*                    REFRESH_USER_DEFINED_POSITIONS                    *
***********************************************************************/
static void refresh_user_defined_positions()
{
BOOLEAN      b;
BOOLEAN      bw;
int          i;
int          n;
HWND         w;
WINDOW_CLASS wc;

w = WindowInfo[USER_DEFINED_TAB].dialogwindow;

for ( i=0; i<USER_VEL_COUNT; i++ )
    CurrentPart.sp[PARTSET_USER_VEL_1_INDEX+i].value.set_text( w, VELOCITY_POSITION_1_EDITBOX+i );

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    CurrentPart.sp[PARTSET_AVG_START_1_INDEX+i].value.set_text( w, AVG_VELOCITY_START_1_EDITBOX+i );
    CurrentPart.sp[PARTSET_AVG_END_1_INDEX+i].value.set_text(   w, AVG_VELOCITY_END_1_EDITBOX+i   );
    }

/*
---------------------------------------------------------------------------------------
                                     Limit Switches
If this is a suretrak control, all the limit switch settings are on the control screen.
Note: LIMIT_SWITCH_COUNT = 4,  MAX_ST_LIMIT_SWITCHES = 4,  MAX_FTII_LIMIT_SWITCHES = 6
--------------------------------------------------------------------------------------- */
    n = MAX_FTII_LIMIT_SWITCHES;

b  = TRUE;
bw = TRUE;
if ( CurrentMachine.suretrak_controlled )
    {
    b  = FALSE;
    bw = FALSE;
    }

for ( i=0; i<n; i++ )
    {
    wc = GetDlgItem( w, LS1_DESC_EB+i );
    wc.enable( b );
    wc = GetDlgItem( w, LS1_POS_EB+i );
    wc.enable( b );
    wc = GetDlgItem( w, LS1_WIRE_PB+i );
    wc.enable( bw );
    }

for ( i=0; i<n; i++ )
    {
    CurrentLimitSwitch.desc(i).set_text( w, LS1_DESC_EB+i );
    CurrentLimitSwitch.pos(i).set_text( w, LS1_POS_EB+i );
    CurrentLimitSwitch.wire(i).set_text( w, LS1_WIRE_PB+i );
    }
}

/***********************************************************************
*                          CHECK_LOW_IMPACT_VALUES                     *
***********************************************************************/
BOOLEAN check_low_impact_values()
{
TCHAR      * cp;
BOOLEAN      status;
float        x;
float        xmin;
STRING_CLASS s;

status = TRUE;
SureTrakSetup.rewind();
while ( SureTrakSetup.next() )
    {
    cp = SureTrakSetup.low_impact_string();
    if ( !is_empty(cp) )
        {
        x = extfloat( cp );
        xmin = SureTrakParam.min_low_impact_percent();
        if ( x < xmin )
            {
            if ( not_float_zero(xmin - x) )
                {
                status = FALSE;
                s = ascii_float(xmin);
                s += PercentChar;
                SureTrakSetup.set_low_impact( s.text() );
                SureTrakHasChanged = TRUE;
                }
            }
        }
    }

return status;
}

/***********************************************************************
*                            SHOW_LOW_IMPACT_ERROR                     *
***********************************************************************/
static void show_low_impact_error()
{
TCHAR      * cp;
STRING_CLASS s;

s = StringTable.find( TEXT("LI_GT") );
s += ascii_float( SureTrakParam.min_low_impact_percent() );
s += PercentChar;

cp = StringTable.find(TEXT("INVALID_ENTRY") );
MessageBox( MainWindow, s.text(), cp, MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                      ENABLE_LOW_IMPACT_CONTROLS                      *
***********************************************************************/
static void enable_low_impact_controls()
{
static INT LowImpactControlId[] = {
    LI_1_EDITBOX, LI_2_EDITBOX, LI_3_EDITBOX, LI_4_EDITBOX,
    LI_5_EDITBOX, LI_6_EDITBOX, LI_7_EDITBOX, LI_8_EDITBOX
    };
const int NOF_LOW_IMPACT_CONTROLS = sizeof(LowImpactControlId)/sizeof(INT);

BOOLEAN  b;
int  i;
HWND w;

b = TRUE;
if ( CurrentMachine.has_binary_valve() )
    b = false;
for ( i=0; i<NOF_LOW_IMPACT_CONTROLS; i++ )
    {
    w = GetDlgItem( SuretrakProfileWindow, LowImpactControlId[i] );
    EnableWindow( w, b );
    }
}

/***********************************************************************
*                         POSITION_TAB_BUTTONS                         *
***********************************************************************/
static void position_tab_buttons()
{
const long DEFAULT_PADDING = 2;

int  i;
int  max_top_buttons;
int  n;
int  nof_visible_buttons;
bool is_visible[TAB_COUNT];
int  width[TAB_COUNT];
WINDOW_CLASS wc;
HWND w;
HDC  dc;
STRING_CLASS s;
RECTANGLE_CLASS total_r;
RECTANGLE_CLASS r;
long            delta;
long            height;
long            sum;
long            total_width;
long            padding;


/*
--------------------------------------------
Get a device context from one of the buttons
and use it to get the width needed for each.
-------------------------------------------- */
wc = WindowInfo[BASIC_SETUP_TAB].tab_rb;
dc = wc.get_dc();

nof_visible_buttons = 0;
for ( i=0; i<TAB_COUNT; i++ )
    {
    w = WindowInfo[i].tab_rb;
    if ( IsWindowVisible(w) )
        {
        s.get_text( w );
        width[i] = (int) pixel_width( dc, s.text(), s.len() );
        is_visible[i] = true;
        nof_visible_buttons++;
        }
    else
        {
        width[i] = 0;
        is_visible[i] = false;
        }
    }
wc.release_dc( dc );

/*
--------------------------------------------------------------------
Get the rect of the basic button to give me the left, top and bottom
-------------------------------------------------------------------- */
wc.get_move_rect( total_r );

/*
-----------------------------------------------------------------
Get the client rect of the main dialog to give me the right edge.
----------------------------------------------------------------- */
wc = MainDialogWindow;
wc.get_client_rect( r );
total_r.right = r.right;
total_width = total_r.width();

max_top_buttons = nof_visible_buttons / 2;
if ( max_top_buttons*2 < nof_visible_buttons )
    max_top_buttons++;

/*
------------------
Top Row of Buttons
------------------ */
sum = 0;
n   = 0;

/*
---------------------------------------------------
Calculate how many buttons will fit on the top line
--------------------------------------------------- */
for ( i=0; i<max_top_buttons; i++ )
    {
    wc = WindowInfo[i].tab_rb;
    delta = width[i] + 2*DEFAULT_PADDING;
    if ( (sum+delta) > total_width )
        break;
    sum += delta;
    n++;
    }

if ( n < 1 )
    return;

/*
--------------------------------------------------------
Calculate the amount of padding I can add to each button
-------------------------------------------------------- */
padding = 2 * DEFAULT_PADDING + ( total_width - sum ) / n;

r = total_r;
for ( i=0; i<n; i++ )
    {
    wc = WindowInfo[i].tab_rb;
    delta = width[i] + padding;
    r.right = r.left + delta;
    wc.move( r );
    r.offset( delta, 0 );
    }

/*
-----------------------------------------------------
Move the rectangle down for the second row of buttons
----------------------------------------------------- */
height = total_r.height();
total_r.offset( 0, height );

sum = 0;
for ( i=n; i<TAB_COUNT; i++ )
    {
    if ( is_visible[i] )
        sum += width[i] + 2*DEFAULT_PADDING;
    }

padding = 2 * DEFAULT_PADDING + ( total_width - sum ) / (nof_visible_buttons - n);

r = total_r;
for ( i=n; i<TAB_COUNT; i++ )
    {
    if ( is_visible[i] )
        {
        wc = WindowInfo[i].tab_rb;
        delta = width[i] + padding;
        r.right = r.left + delta;
        wc.move( r );
        r.offset( delta, 0 );
        }
    }
}

/***********************************************************************
*                            ENABLE_BUTTONS                            *
***********************************************************************/
static void enable_buttons()
{
static INT SureTrakControlId[] = {
    ACCEL_1_EDITBOX, VEL_1_EDITBOX, EP_1_EDITBOX, VW_1_CHECKBOX, LIFI_1_CHECKBOX,
    ACCEL_2_EDITBOX, VEL_2_EDITBOX, EP_2_EDITBOX, VW_2_CHECKBOX, LIFI_2_CHECKBOX,
    ACCEL_3_EDITBOX, VEL_3_EDITBOX, EP_3_EDITBOX, VW_3_CHECKBOX, LIFI_3_CHECKBOX,
    ACCEL_4_EDITBOX, VEL_4_EDITBOX, EP_4_EDITBOX, VW_4_CHECKBOX, LIFI_4_CHECKBOX,
    ACCEL_5_EDITBOX, VEL_5_EDITBOX, EP_5_EDITBOX, VW_5_CHECKBOX, LIFI_5_CHECKBOX,
    ACCEL_6_EDITBOX, VEL_6_EDITBOX, EP_6_EDITBOX, VW_6_CHECKBOX, LIFI_6_CHECKBOX,
    ACCEL_7_EDITBOX, VEL_7_EDITBOX, EP_7_EDITBOX, VW_7_CHECKBOX, LIFI_7_CHECKBOX,
    ACCEL_8_EDITBOX, VEL_8_EDITBOX, EP_8_EDITBOX, VW_8_CHECKBOX, LIFI_8_CHECKBOX,
    LS1_DESC_EB, LS2_DESC_EB, LS3_DESC_EB, LS4_DESC_EB, LS5_DESC_EB,LS6_DESC_EB,
    LS1_POS_EB, LS2_POS_EB, LS3_POS_EB, LS4_POS_EB, LS5_POS_EB,LS6_POS_EB,
    LS1_WIRE_PB, LS2_WIRE_PB, LS3_WIRE_PB, LS4_WIRE_PB, LS5_WIRE_PB,LS6_WIRE_PB
    };
const int NOF_SURETRAK_CONTROLS = sizeof(SureTrakControlId)/sizeof(INT);

static INT Ft2SureTrakControlId[] = {
    FT_STOP_POS_EBOX, FT_STOP_POS_STATIC
    };
const int NOF_FT2_SURETRAK_CONTROLS = sizeof(Ft2SureTrakControlId)/sizeof(INT);

COMPUTER_CLASS c;
HWND     w;
BOOLEAN  b;
BOOLEAN  is_current_part;
BOOLEAN  is_this_computer;
BOOLEAN  is_enabled;
BOOLEAN  is_visible;
BOOLEAN  was_enabled;
int      i;
UINT     id;
TCHAR  * cp;
short    required_password_level;
STRING_CLASS s;

/*
-------------------------------------------------------------------
If this machine is being monitored and this is NOT the current part
then show the monitor this part button.
------------------------------------------------------------------- */
id              = 0;
is_enabled      = FALSE;

is_this_computer = FALSE;
if ( c.whoami() == CurrentMachine.computer )
    is_this_computer = TRUE;

is_current_part  = FALSE;
if ( CurrentMachine.current_part == CurrentPart.name )
    is_current_part  = TRUE;

if ( is_this_computer || CurrentPasswordLevel >= EditOtherCurrentPartLevel )
    {
    if ( CurrentMachine.monitor_flags & MA_MONITORING_ON || is_remote_monitor() )
        {
        /*
        --------------------------------------------------------------------
        Enable the "Start Monitoring" button if this is not the current part
        -------------------------------------------------------------------- */
        if ( !is_current_part )
            {
            is_enabled = TRUE;
            id = MONITOR_THIS_PART_STRING;
            }
        }
    else
        {
        id = NOT_MONITORED_STRING;
        }
    }

w = GetDlgItem( MainDialogWindow, MONITOR_THIS_PART_BUTTON );

if ( !id )
    {
    if ( is_current_part )
        id = THIS_IS_CURRENT_PART_STRING;
    else
        id = NOT_CURRENT_PART_STRING;
    }

set_text( w, resource_string(id) );
EnableWindow( w, is_enabled );

is_enabled = TRUE;
if ( !is_this_computer )
    {
    if ( CurrentPasswordLevel < EditOtherCurrentPartLevel )
        {
        if ( CurrentMachine.monitor_flags & MA_MONITORING_ON && is_current_part )
            is_enabled = FALSE;

        if ( CurrentPasswordLevel < EditOtherComputerLevel )
            is_enabled = FALSE;
        }
    }

if ( CurrentPasswordLevel < SaveChangesLevel )
    is_enabled = FALSE;

/*
-------------------
Save Changes Button
------------------- */
w = GetDlgItem( MainDialogWindow, SAVE_CHANGES_BUTTON );
if ( VelocityChangesLevel != NO_PASSWORD_LEVEL )
    {
    /*
    --------------------------------------------------------
    Make sure the title of the save chages button is correct
    -------------------------------------------------------- */
    if ( is_enabled )
        {
        SaveChangesTitle.set_text( w );
        NeedToCallVelocityChanges = FALSE;
        }
    else if ( CurrentPasswordLevel >= VelocityChangesLevel )
        {
        set_text( w, StringTable.find(TEXT("VELOCITY_CHANGES_STRING")) );
        NeedToCallVelocityChanges = TRUE;
        is_enabled = TRUE;
        }
    }

/*
---------------------------------------------------------------------------------------
If the WorkstationChangeDisable is true nothing else matters, disable the change button
--------------------------------------------------------------------------------------- */
if ( !is_this_computer && WorkstationChangeDisable )
    is_enabled = FALSE;
EnableWindow( w, is_enabled );

enable_sn_template_editbox();

/*
-----------------------------------------------
Show or hide suretrak and pressure control tabs
----------------------------------------------- */
is_visible = CurrentMachine.suretrak_controlled;

/*
------------------------------------------------------------
Disable the Suretrak Control button if this is not a control
------------------------------------------------------------ */
w = WindowInfo[SURETRAK_PROFILE_TAB].tab_rb;
show_window( w, is_visible );

/*
------------------------------------------------------------
Disable the Pressure Control button if this is not a control
------------------------------------------------------------ */
w = WindowInfo[PRESSURE_CONTROL_TAB].tab_rb;
if ( is_visible && HavePressureControl )
    show_window( w, TRUE );
else
    show_window( w, FALSE );

if ( HaveSureTrakControl )
    {
    was_enabled = is_enabled;
    if ( !is_this_computer )
        {
        if ( CurrentPasswordLevel < EditOtherSuretrakLevel )
            is_enabled = FALSE;

        if ( is_current_part && CurrentPasswordLevel < EditOtherCurrentSuretrakLevel )
            is_enabled = FALSE;
        }

    for ( i=0; i<NOF_SURETRAK_CONTROLS; i++ )
        {
        w = GetDlgItem( SuretrakProfileWindow, SureTrakControlId[i] );
        EnableWindow( w, is_enabled );
        }

    for ( i=0; i<NOF_FT2_SURETRAK_CONTROLS; i++ )
        {
        w = GetDlgItem( SuretrakProfileWindow, Ft2SureTrakControlId[i] );
        show_window( w, is_enabled );
        EnableWindow( w, is_enabled );
        }

    is_visible = CurrentMachine.suretrak_controlled;

    show_window( SuretrakProfileWindow, LIMIT_SWITCH_POS_STATIC, is_visible );
    show_window( SuretrakProfileWindow, ST_LS_LINE_STATIC, is_visible );
    show_window( SuretrakProfileWindow, ST_LS_DESC_STATIC, is_visible );
    show_window( SuretrakProfileWindow, ST_LS_POS_STATIC, is_visible );
    show_window( SuretrakProfileWindow, ST_LS_WIRE_STATIC, is_visible );
    show_window( SuretrakProfileWindow, ST_VAC_SWITCH_STATIC, is_visible );
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        {
        show_window( SuretrakProfileWindow, LS1_POS_EB+i,  is_visible );
        show_window( SuretrakProfileWindow, LS1_DESC_EB+i, is_visible );
        b = is_visible;
        if ( HideSureTrakLimitSwitchWires )
            b = FALSE;
        show_window( SuretrakProfileWindow, LS1_WIRE_PB+i, b );
        show_window( SuretrakProfileWindow, VAC_SWITCH_1_RADIO+i, is_visible );
        }

    is_visible = CurrentMachine.suretrak_controlled;
    if ( HideSureTrakLimitSwitchWires )
        is_visible = FALSE;
    show_window(SuretrakProfileWindow, ST_LS_WIRE_STATIC, is_visible );

    /*
    ----------------------------------------------------------------------
    If this machine has a cyclone board I need to hide break gains 2 and 3
    (5/8/13) I'm not using integral and derivitive any more. I'm hiding
    both the vel and gain settings since they have no effect on a
    ciclone board.
    ---------------------------------------------------------------------- */
    if ( CurrentMachine.is_cyclone )
        {
        show_window(SuretrakProfileWindow, BREAK_VEL_2_EBOX, FALSE );
        show_window(SuretrakProfileWindow, BREAK_VEL_3_EBOX, FALSE );
        show_window(SuretrakProfileWindow, BREAK_GAIN_2_EBOX, FALSE );
        show_window(SuretrakProfileWindow, BREAK_GAIN_3_EBOX, FALSE );
        show_window(SuretrakProfileWindow, BREAK_2_STATIC, FALSE );
        show_window(SuretrakProfileWindow, BREAK_3_STATIC, FALSE );
        }
    else
        {
        show_window(SuretrakProfileWindow, BREAK_VEL_2_EBOX, TRUE );
        show_window(SuretrakProfileWindow, BREAK_VEL_3_EBOX, TRUE );
        s = TEXT("2" );
        s.set_text( SuretrakProfileWindow, BREAK_2_STATIC );
        s = TEXT("3" );
        s.set_text( SuretrakProfileWindow, BREAK_3_STATIC );
        }

    is_enabled = was_enabled;
    }

w = WindowInfo[SETUP_SHEET_TAB].tab_rb;
show_window( w, ShowSetupSheet );

/*
---------------------------------------------------------
Position and size the tab buttons based upon their titles
--------------------------------------------------------- */
position_tab_buttons();

if ( is_current_part )
    is_enabled = FALSE;

w = GetDlgItem( MainDialogWindow, DELETE_PART_BUTTON  );
EnableWindow( w, is_enabled );

is_enabled = TRUE;
if ( !is_this_computer && (CurrentPasswordLevel < EditOtherComputerLevel) )
    is_enabled = FALSE;

if ( CurrentPasswordLevel < ENGINEER_PASSWORD_LEVEL )
    is_enabled = FALSE;

w = GetDlgItem( MainDialogWindow, CREATE_PART_BUTTON  );
EnableWindow( w, is_enabled );

is_enabled = FALSE;
required_password_level = ENGINEER_PASSWORD_LEVEL;
cp = get_ini_string(EditPartIniFile, ConfigSection, SaveAllPartsLevelKey );
if ( !unknown(cp) )
    required_password_level = (short) asctoint32( cp );
if ( CurrentPasswordLevel >= required_password_level )
    is_enabled = TRUE;

w = GetDlgItem( MainDialogWindow, SAVE_ALL_PARTS_PB );
EnableWindow( w, is_enabled );

is_enabled = FALSE;

cp = get_ini_string(EditPartIniFile, ConfigSection, SetupSheetLevelKey );
if ( !unknown(cp) )
    {
    required_password_level = (short) asctoint32( cp );
    if ( CurrentPasswordLevel < required_password_level )
        is_enabled = TRUE;
    w = GetDlgItem( WindowInfo[SETUP_SHEET_TAB].dialogwindow, SETUP_EBOX );
    SendMessage( w, EM_SETREADONLY, (WPARAM) (BOOL) is_enabled, 0 );
    }

/*
-----------------------------------------------------------------------------------
If StartupTab is not NO_TAB then this is the first time I have done this and I need
to "Press" the tab button for the tab that was displayed the last time the program
was run. Actually I only do BASIC, SURETRAK, and PRESSURE_CONTROL.
----------------------------------------------------------------------------------- */
if ( BASIC_SETUP_TAB <= StartupTab && StartupTab <= SETUP_SHEET_TAB )
    {
    if ( StartupTab == SURETRAK_PROFILE_TAB && !CurrentMachine.suretrak_controlled )
        StartupTab = BASIC_SETUP_TAB;
    else if ( StartupTab == PRESSURE_CONTROL_TAB && !(CurrentMachine.monitor_flags & MA_HAS_PRESSURE_CONTROL) )
        StartupTab = BASIC_SETUP_TAB;
    PostMessage( WindowInfo[StartupTab].tab_rb, BM_CLICK, 0, 0 );
    StartupTab = NO_TAB;
    }
}

/***********************************************************************
*                     SAVE_EDIT_CONTROL_TO_FILE                        *
***********************************************************************/
static void save_edit_control_to_file( NAME_CLASS & file_name, HWND w, UINT id )
{
HANDLE  fh;
TCHAR * s;
DWORD   slen;
DWORD   bytes_to_write;
DWORD   bytes_written;

const DWORD access_mode  = GENERIC_WRITE;
const DWORD share_mode   = 0;
const DWORD create_flags = CREATE_ALWAYS;

w = GetDlgItem( w, id);
slen = GetWindowTextLength( w );
s    = maketext( slen+1 );
if ( s )
    {
    if ( get_text(s, w, slen) )
        {
        fh = CreateFile( file_name.text(), access_mode, share_mode, NULL, create_flags, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
        if ( fh != INVALID_HANDLE_VALUE )
            {
            *(s+slen) = NullChar;
            slen = lstrlen( s );
            bytes_to_write = slen * sizeof(TCHAR);
            WriteFile( fh, s, bytes_to_write, &bytes_written, 0 );
            CloseHandle( fh );
            }
        }
    delete[] s;
    }
}

/***********************************************************************
*                     LOAD_EDIT_CONTROL_FROM_FILE                      *
***********************************************************************/
static void load_edit_control_from_file( HWND w, UINT id, NAME_CLASS & file_name )
{
HANDLE  fh;
TCHAR * s;
DWORD   slen;
DWORD   bytes_to_read;
DWORD   bytes_read;
DWORD   max_slen;
BOOL    status;

const DWORD access_mode  = GENERIC_READ;
const DWORD share_mode   = FILE_SHARE_READ | FILE_SHARE_WRITE;
const DWORD create_flags = OPEN_EXISTING;

w = GetDlgItem( w, id );
max_slen = SendMessage(  w, EM_GETLIMITTEXT, 0, 0 );

fh = CreateFile( file_name.text(), access_mode, share_mode, NULL, create_flags, FILE_FLAG_SEQUENTIAL_SCAN, 0 );

if ( fh != INVALID_HANDLE_VALUE )
    {
    bytes_to_read = GetFileSize( fh, 0 );
    slen = bytes_to_read/sizeof(TCHAR);
    if ( slen > max_slen )
        slen = max_slen;

    s = maketext( slen+1 );
    if ( s )
        {
        status = ReadFile( fh, s, bytes_to_read, &bytes_read, 0 );
        if ( status && bytes_read > 0 )
            {
            *(s+slen) = NullChar;
            set_text( w, s );
            SendMessage( w, EM_SETMODIFY, (WPARAM) (UINT) FALSE, 0 );
            }
        delete[] s;
        }
    CloseHandle( fh );
    }
}

/***********************************************************************
*                           SHOW_SETUP_SHEET                           *
***********************************************************************/
static void show_setup_sheet()
{
HWND       w;
NAME_CLASS s;
NAME_CLASS d;
BOOLEAN    have_file;

if ( !HaveCurrentPart )
    return;

w = WindowInfo[SETUP_SHEET_TAB].dialogwindow;

s.get_setup_sheet_file_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

have_file = s.file_exists();
if ( !have_file )
    {
    /*
    ---------------------
    Copy the default file
    --------------------- */
    d.get_exe_dir_file_name( SETUP_SHEET_FILE );
    if ( d.file_exists() )
        have_file = d.copyto( s );
    }

if ( have_file )
    load_edit_control_from_file( w, SETUP_EBOX, s );
else
    set_text( w, SETUP_EBOX, EmptyString );
}

 /***********************************************************************
*                   SET_ZERO_BREAK_SETUP_STRINGS                       *
***********************************************************************/
static void set_zero_break_setup_strings( HWND hWnd )
{
WINDOW_CLASS w;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { ONE_OR_MORE_STATIC, TEXT("ONE_OR_MORE") },
    { ZERO_NOT_VALID_STATIC, TEXT("ZERO_NOT_VALID") },
    { IDOK, TEXT("CLOSE_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rsn, nof_rsn );

w = hWnd;
w.set_title( StringTable.find(TEXT("CHANGE_INFO")) );
w.refresh();
}

/***********************************************************************
*                  ZERO_BREAK_GAIN_CHANGED_DIALOG_PROC                 *
***********************************************************************/
BOOL CALLBACK zero_break_gain_changed_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_zero_break_setup_strings( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                   SET_NO_LAST_SETUP_STRINGS                          *
***********************************************************************/
static void set_no_last_setup_strings( HWND hWnd )
{
WINDOW_CLASS w;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { NFPV_1_TBOX, TEXT("MACHINE_HAS_BINARY_VALVE") },
    { NFPV_2_TBOX, TEXT("MUST_SPECIFY_LAST_STEP_END_POS") },
    { IDOK, TEXT("CLOSE_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rsn, nof_rsn );

w = hWnd;
w.set_title( StringTable.find(TEXT("NO_LAST_STEP_END_POS_FOUND")) );
w.refresh();
}

/***********************************************************************
*                     NO_LAST_STEP_END_POS_DIALOG_PROC                 *
***********************************************************************/
BOOL CALLBACK no_last_step_end_pos_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_no_last_setup_strings( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                   SET_NLS_LOW_SETUP_STRINGS                       *
***********************************************************************/
static void set_nls_low_setup_strings( HWND hWnd )
{
WINDOW_CLASS w;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { NFPV_1_TBOX, TEXT("MACHINE_HAS_BINARY_VALVE") },
    { NFPV_2_TBOX, TEXT("MUST_SPECIFY_LAST_STEP_LOW_IMPACT") },
    { IDOK, TEXT("CLOSE_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rsn, nof_rsn );

w = hWnd;
w.set_title( StringTable.find(TEXT("NO_LAST_STEP_LOW_IMPACT_FOUND")) );
w.refresh();
}

/***********************************************************************
*                   NO_LAST_STEP_LOW_IMPACT_DIALOG_PROC                *
***********************************************************************/
BOOL CALLBACK no_last_step_low_impact_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_nls_low_setup_strings( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                   SET_MISSING_FILE_SETUP_STRINGS                     *
***********************************************************************/
static void set_missing_file_setup_strings( HWND hWnd )
{
WINDOW_CLASS w;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { MISSING_FILE_DESC_TBOX,   TEXT("MISSING_FILE_DESC") },
    { MISSING_FILE_ACTION_TBOX, TEXT("MISSING_FILE_ACTION") },
    { IDOK,                     TEXT("CLOSE_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rsn, nof_rsn );

w = hWnd;
w.set_title( StringTable.find(TEXT("MISSING_FILE")) );
w.refresh();
}

/***********************************************************************
*                        MISSING_FILE_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK missing_file_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_missing_file_setup_strings( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                       SHOW_MISSING_FILE_DIALOG                       *
***********************************************************************/
static void show_missing_file_dialog()
{
DialogBox(
    MainInstance,
    TEXT("MISSING_FILE_DIALOG"),
    MainDialogWindow,
    (DLGPROC) missing_file_dialog_proc );
}

/***********************************************************************
*               UPDATE_SURETRAK_PARAMS_WITH_FT2_SETTINGS               *
***********************************************************************/
static void update_suretrak_params_with_ft2_settings()
{
INI_CLASS  ini;
NAME_CLASS s;

s.get_ft2_editor_settings_file_name( CurrentPart.computer, CurrentPart.machine );
if ( !s.file_exists() )
    return;

ini.set_file( s.text() );
ini.set_section( ConfigSection );

SureTrakParam.set_min_low_impact_percent( (float) ini.get_double( MinLowImpactPercentKey) );
SureTrakParam.set_max_velocity( (float) ini.get_double( MaxVelocityKey) );
}

/***********************************************************************
*                         SHOW_WARMUP_CONTROLS                         *
***********************************************************************/
static void show_warmup_controls()
{
/*
---------------------------------------------------------------------------------
The following is a short list that is hidden when the multipart runlist is active
--------------------------------------------------------------------------------- */
static const int warmup_control[] = {
    WARMUP_WIRE_EBOX,
    WARMUP_RUN_IF_ON_XBOX,
    WARMUP_IF_STATIC,
    WARMUP_IS_STATIC,
    WARMUP_PART_CBOX,
    WARMUP_GROUP_STATIC,
    WARMUP_ENABLE_XBOX
    };
static const int nof_warmup_controls = sizeof(warmup_control)/sizeof(int);

int  i;
WINDOW_CLASS parent;
WINDOW_CLASS w;

if ( CurrentMachine.multipart_runlist_settings.enable || CurrentMachine.suretrak_controlled )
    {
    /*
    -----------------------------------------------------
    Hide the warmup controls on the advanced setup screen
    ----------------------------------------------------- */
    parent = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

    for ( i=0; i<nof_warmup_controls; i++ )
        {
        w = parent.control( warmup_control[i] );
        w.hide();
        }

    if ( !CurrentMachine.multipart_runlist_settings.enable )
        {
        parent = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

        for ( i=0; i<nof_warmup_controls; i++ )
            {
            w = parent.control( warmup_control[i] );
            w.show();
            }
        }
    }
else
    {
    /*
    -----------------------------
    This is a monitor only system
    ----------------------------- */
    parent = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

    for ( i=0; i<nof_warmup_controls; i++ )
        {
        w = parent.control( warmup_control[i] );
        w.show();
        }

    parent = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

    for ( i=0; i<nof_warmup_controls; i++ )
        {
        w = parent.control( warmup_control[i] );
        w.hide();
        }
    }
}

/***********************************************************************
*                            GET_CURRENT_PART                          *
***********************************************************************/
static void get_current_part()
{
ANALOG_SENSOR  a;
int            i;
short          mysensor;
WPARAM         x;
STRING_CLASS   mypart;
TCHAR        * cp;
HWND           w;
LISTBOX_CLASS  lb;
NAME_CLASS     s;
CHAXIS_LABEL_CLASS label;

LimitSwitchDescriptionsHaveChanged = FALSE;
lb.init( MainDialogWindow, PART_LISTBOX_ID );
mypart = lb.selected_text();
if ( mypart.isempty() )
    return;

s.get_ftchan_dbname( CurrentMachine.computer, CurrentMachine.name, mypart );
if ( !s.file_exists() )
    show_missing_file_dialog();

if ( !CurrentPart.find(CurrentMachine.computer, CurrentMachine.name, mypart) )
    return;

/*
---------------------------------------------------------------------------------------
Make a copy of the sensors so I can check for changes when I save.
(This is only necessary because the extended sensors screen saves directly to the part)
--------------------------------------------------------------------------------------- */
for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    {
    mysensor = CurrentPart.analog_sensor[i];
    OriginalAnalogSensor[i] = mysensor;
    if ( !a.exists(mysensor) )
        {
        s = StringTable.find( TEXT("NO_ASENSOR_START") );
        s += i;
        s += StringTable.find( TEXT("NO_ASENSOR_END") );
        MessageBox( 0, StringTable.find(TEXT("NO_ASENSOR_LINE2")), s.text(), MB_OK | MB_SYSTEMMODAL );
        mysensor = DEFAULT_FT2_ANALOG_SENSOR;
        CurrentPart.analog_sensor[i] = mysensor;
        }
    }

HaveCurrentPart = TRUE;

refresh_basic_setup();

CurrentFtAnalog.load( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
CurrentFtCalc.load( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
CurrentLimitSwitch.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
----------------
Parameter Limits
---------------- */
CurrentSort.load( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

if ( CurrentParam.find(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
    refresh_parameter_limits();

if ( CurrentMarks.load(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
    show_marks();

SureTrakParam.find( CurrentPart.computer, CurrentPart.machine );

/*
----------------------------------------------------------------------
This is a ft2 machine, get the settings that I need from the ft2 files
---------------------------------------------------------------------- */
update_suretrak_params_with_ft2_settings();

SureTrakSetup.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

enable_last_step_low_impact_eb();

PressureControl.find( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
SureTrakHasChanged = FALSE;

if ( HaveStModTbox.is_visible() )
    HaveStModTbox.hide();

if ( HaveSureTrakControl )
    {
    if ( !CurrentMachine.has_binary_valve() )
        {
        if ( !check_low_impact_values() )
            stringtable_message_box( TEXT("SOME_LI_CHANGED"), TEXT("INVALID_LI_ENTRY") );
        }
    }

refresh_user_defined_positions();
show_warmup_controls();
refresh_advanced_setup();
refresh_suretrak_setup();
refresh_pressure_control_setup();

/*
---------------------------
Get the channel axis labels
--------------------------- */
label.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
---------------------------------------
Set the analog sensors for each channel
--------------------------------------- */
w = WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow;
for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    a.find( CurrentPart.analog_sensor[i] );
    cp = a.desc();
    if ( cp )
        {
        lb.init( w, FT_CHANNEL_1_LISTBOX+i );
        lb.setcursel( cp );

        /*
        ---------------------------
        Set the channel axis labels
        --------------------------- */
        if ( lstrlen(label[i]) > 0 )
            set_text( w, CHAN_1_AXIS_LABEL_EBOX+i, label[i] );
        else
            set_text( w, CHAN_1_AXIS_LABEL_EBOX+i, resource_string(CHANNEL_1_LABEL_STRING+i) );
        }
    }

/*
------------------------------------------------------------------------------
If the extended channels dialog is visible I need to load the current settings
------------------------------------------------------------------------------ */
if ( ExtendedChannelsDialog )
    load_extended_channels();

/*
--------------------------------------
Set the head and rod pressure channels
-------------------------------------- */
if ( CurrentPart.head_pressure_channel == NO_FT_CHANNEL )
    x = HEAD_PRES_NO_CHAN_BUTTON;
else
    x = HEAD_PRES_CHAN_1_BUTTON + CurrentPart.head_pressure_channel - 1;
CheckRadioButton( w, HEAD_PRES_CHAN_1_BUTTON, HEAD_PRES_NO_CHAN_BUTTON, x );
if ( CurrentPart.rod_pressure_channel == NO_FT_CHANNEL )
    x = ROD_PRES_NO_CHAN_BUTTON;
else
    x = ROD_PRES_CHAN_1_BUTTON + CurrentPart.rod_pressure_channel - 1;
CheckRadioButton( w, ROD_PRES_CHAN_1_BUTTON, ROD_PRES_NO_CHAN_BUTTON, x );

/*
-------------------
Get the Setup Sheet
------------------- */
show_setup_sheet();
enable_buttons();
}

/***********************************************************************
*                          FILL_PART_LISTBOX                           *
***********************************************************************/
short fill_part_listbox( HWND w, int listbox_id, MACHINE_ENTRY * m )
{
short           n;
LISTBOX_CLASS   lb;

lb.init( w, listbox_id );

n = 0;
lb.empty();

m->partlist.rewind();
while ( m->partlist.next() )
    {
    lb.add( m->partlist.text() );
    n++;
    }

return n;
}

/***********************************************************************
*                        HIDE_FASTRAK_CHANNELS                         *
***********************************************************************/
static void hide_fastrak_channels()
{
static UINT eid[] = {
    FT_CHANNEL_5_LISTBOX, FT_CHANNEL_6_LISTBOX, FT_CHANNEL_7_LISTBOX, FT_CHANNEL_8_LISTBOX,
    CHAN_5_TBOX, CHAN_6_TBOX, CHAN_7_TBOX, CHAN_8_TBOX
    };
const int nof_eids = sizeof(eid)/sizeof(UINT);

static UINT id[] = {
    HEAD_PRES_CHAN_5_BUTTON, HEAD_PRES_CHAN_6_BUTTON, HEAD_PRES_CHAN_7_BUTTON, HEAD_PRES_CHAN_8_BUTTON,
    ROD_PRES_CHAN_5_BUTTON, ROD_PRES_CHAN_6_BUTTON, ROD_PRES_CHAN_7_BUTTON, ROD_PRES_CHAN_8_BUTTON,
    CHAN_5_AXIS_LABEL_EBOX, CHAN_6_AXIS_LABEL_EBOX, CHAN_7_AXIS_LABEL_EBOX, CHAN_8_AXIS_LABEL_EBOX
    };
const int nof_ids = sizeof(id)/sizeof(UINT);

WINDOW_CLASS w;
WINDOW_CLASS c;
int i;

w = WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow;
for ( i=0; i<nof_eids; i++ )
    {
    c = w.control( eid[i] );
    c.enable( CurrentMachine.is_ftii );
    }

for ( i=0; i<nof_ids; i++ )
    {
    c = w.control( id[i] );
    if ( CurrentMachine.is_ftii )
        c.show();
    else
        c.hide();
    }
}

/***********************************************************************
*                               FILL_PARTS                             *
***********************************************************************/
static void fill_parts()
{
static bool firstime = true;
MACHINE_ENTRY * m;
STRING_CLASS    mymachine;
int             n;
LISTBOX_CLASS   cb;

HaveCurrentPart = FALSE;

cb.init( MainDialogWindow, MACHINE_LISTBOX_ID );
mymachine = cb.selected_text();
if ( !mymachine.isempty() )
    {
    m = find_machine_entry( mymachine );
    if ( m )
        {
        CurrentMachine.find( m->computer, m->name );

        /*
        --------------------------------------------------
        See if the number of editable channels has changed
        -------------------------------------------------- */
        n = MAX_EDITABLE_FTII_CHANNELS;

        if ( firstime || n != Nof_Editable_Channels )
            {
            firstime = false;
            Nof_Editable_Channels = n;
            hide_fastrak_channels();
            }
        fill_part_listbox( MainDialogWindow, PART_LISTBOX_ID, m );
        }
    }
}

/***********************************************************************
*                               FILL_PARTS                             *
*          Call with i = NO_INDEX for the current monitor part.        *
***********************************************************************/
static void fill_parts( INT i )
{
LISTBOX_CLASS   lb;

fill_parts();

lb.init( MainDialogWindow, PART_LISTBOX_ID );
if ( i == NO_INDEX )
    {
    /*
    -------------------------------------------------
    Load the current part if I can, else default to 0
    ------------------------------------------------- */
    if ( !lb.setcursel(CurrentMachine.current_part.text()) )
        i = 0;
    }

if ( i != NO_INDEX )
    lb.setcursel( i );

get_current_part();
}

/***********************************************************************
*                              FILL_PARTS                              *
***********************************************************************/
static void fill_parts( STRING_CLASS & part )
{
LISTBOX_CLASS   lb;

fill_parts();

lb.init( MainDialogWindow, PART_LISTBOX_ID );
lb.setcursel( part.text() );
get_current_part();
}

/***********************************************************************
*                         FILL_MACHINE_CB                              *
*                 Return the number of machines found                  *
***********************************************************************/
static short fill_machine_cb( HWND w, int listbox_id )
{
COMPUTER_CLASS  c;
MACHINE_ENTRY * m;
LISTBOX_CLASS   cb;

cb.init( w, listbox_id );
cb.empty();

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;

    if ( (w == CopyToDialogWindow) && (m->computer != c.whoami() && CurrentPasswordLevel < EditMaOnOtherCoLevel) )
            continue;

    cb.add( m->name.text() );
    }

return (short) cb.count();
}

/***********************************************************************
*                         FILL_MACHINE_CB                              *
*                 Return the number of machines found                  *
***********************************************************************/
static short fill_machine_cb( HWND w, int listbox_id, TCHAR * machine_name )
{
MACHINE_ENTRY * m;
LISTBOX_CLASS   lb;
INT             i;
BOOLEAN         have_machine;

have_machine = FALSE;

lb.init( w, listbox_id );
lb.empty();

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;
    lb.add( m->name.text() );
    if ( m->name == machine_name )
        have_machine = TRUE;
    }

i = lb.count();

if ( have_machine )
    lb.setcursel( machine_name );
else if ( i > 0 )
    lb.setcursel( 0 );

return (short) i;
}

/***********************************************************************
*                            DO_HMI_COMMAND                            *
***********************************************************************/
static void do_hmi_command()
{
unsigned      flags;
LISTBOX_CLASS lb;

if ( HMIMessageType == HMI_NO_MESSAGE )
        return;

if ( CurrentPart.name != HmiOldPart )
    {
    lb.init( MainDialogWindow, PART_LISTBOX_ID );
    lb.setcursel( HmiOldPart.text() );
    get_current_part();
    }

if ( HMIMessageType & HMI_CREATE_PART_MESSAGE )
    {
    if ( !HmiNewPart.isempty() )
        {
        if ( !part_exists(CurrentPart.computer, CurrentPart.machine, HmiNewPart) )
            {
            flags = COPY_PART_ALL & ~COPY_PART_REFERENCE_TRACE;
            copypart( CurrentPart.computer, CurrentPart.machine, HmiNewPart, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, flags );
            reload_partlist( CurrentPart.machine );
            }
        fill_parts( HmiNewPart );
        }
    }

if ( HMIMessageType & HMI_MONITOR_PART_MESSAGE )
    {
    monitor_this_part();
    }

ShowWindow( MainWindow, SW_SHOWNORMAL );
SetForegroundWindow( MainWindow );
}

/***********************************************************************
*                          CLEAR_HMI_MESSAGE                           *
***********************************************************************/
static void clear_hmi_message()
{
HMIMessageType = HMI_NO_MESSAGE;
HmiOldPart.empty();
HmiNewPart.empty();
}

/***********************************************************************
*                             FILL_MACHINES                            *
***********************************************************************/
static void fill_machines()
{
STRING_CLASS  startup_machine;
STRING_CLASS  startup_part;
TCHAR       * cp;
LISTBOX_CLASS lb;
BOOLEAN       have_startup_part;

have_startup_part = FALSE;
if ( HMIMessageType == HMI_NO_MESSAGE )
    have_startup_part = get_startup_part( startup_machine, startup_part );

if ( !have_startup_part )
    {
    cp = get_ini_string(EditPartIniFile, ConfigSection, LastMachineKey );
    if ( !unknown(cp) )
        {
        startup_machine = cp;
        cp = get_ini_string(EditPartIniFile, ConfigSection, LastPartKey );
        if ( !unknown(cp) )
            {
            startup_part = cp ;
            have_startup_part = TRUE;
            }
        }
    }

if ( fill_machine_cb(MainDialogWindow, MACHINE_LISTBOX_ID, startup_machine.text()) )
    {
    if ( HMIMessageType & HMI_SHOW_PART_MESSAGE )
        {
        fill_parts( HmiOldPart );
        clear_hmi_message();
        }
    else if ( have_startup_part )
        {
        fill_parts( startup_part );
        }
    else
        {
        fill_parts( NO_INDEX );
        }
    }

if ( HMIMessageType != HMI_NO_MESSAGE )
    do_hmi_command();

}

/***********************************************************************
*                          SHOW_CONTEXT_HELP                           *
***********************************************************************/
static BOOL show_context_help( HWND w, LPARAM lParam )
{
const int32 NOF_CONTROLS = 3;
const int32 NOF_STATIC_CONTROLS = 7;

static UINT myid[] = {
    TOTAL_STROKE_LEN_EDITBOX,
    MIN_STROKE_LEN_EDITBOX,
    EOS_VEL_EDITBOX };

static UINT static_id[] = {
    DIST_UNITS_STATIC,
    VEL_UNITS_STATIC,
    TOTAL_STROKE_STATIC,
    MIN_STROKE_STATIC,
    EOS_STATIC,
    ZERO_SPEED_OUTPUT_GB,
    ZERO_SPEED_OUTPUT_WIRE_STATIC };

static DWORD mycontext[] = {
    TOTAL_STROKE_HELP,
    MIN_STROKE_HELP,
    EOS_HELP };

static DWORD static_context[] = {
    DIST_UNITS_HELP,
    VEL_UNITS_HELP,
    TOTAL_STROKE_HELP,
    MIN_STROKE_HELP,
    EOS_HELP,
    ZERO_SPEED_OUTPUT_HELP,
    ZERO_SPEED_OUTPUT_WIRE_HELP };

int32 i;
POINT p;
HWND  bw;
HWND  sw;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

bw = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    sw = GetDlgItem(bw, static_id[i] );
    if ( GetWindowRect(sw, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            gethelp( HELP_CONTEXTPOPUP, static_context[i] );
            return TRUE;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(bw, myid[i]) == w )
        {
        gethelp( HELP_CONTEXTPOPUP, mycontext[i] );
        return TRUE;
        }
    }

get_overview_help();
return TRUE;
}

static short DestUnitsId = NO_UNITS;
static short SorcUnitsId = NO_UNITS;

/***********************************************************************
*                             FLOAT_ADJUST                             *
***********************************************************************/
static float float_adjust( float x )
{
return (float) adjust_for_units( DestUnitsId, x, SorcUnitsId );
}

/***********************************************************************
*                             FLOAT_ADJUST                             *
***********************************************************************/
static double float_adjust( double x )
{
return adjust_for_units( DestUnitsId, x, SorcUnitsId );
}

/***********************************************************************
*                        CHANGE_DISTANCE_UNITS                         *
***********************************************************************/
static void change_distance_units()
{
int    old_button_index;
int    new_button_index;
int    i;
int    j;
int    n;
STRING_CLASS s;
HWND   w;
double x;

w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

if ( !w )
    return;

new_button_index = NO_INDEX;

for ( i=0; i<NofDistanceButtons; i++ )
    {
    if ( is_checked(w, DistanceButton[i]) )
        {
        new_button_index = i;
        break;
        }
    }

if ( new_button_index == NO_INDEX )
    return;

old_button_index  = new_button_index;

/*
-----------------------
Get the previous button
----------------------- */
for ( i=0; i<NofDistanceButtons; i++ )
    {
    if ( DistanceButtonState[i] )
        {
        old_button_index = i;
        break;
        }
    }

if ( old_button_index == new_button_index )
    return;

DestUnitsId = DistanceUnits[new_button_index];
SorcUnitsId = DistanceUnits[old_button_index];

CurrentPart.set_distance_units( DestUnitsId );
CurrentPart.set_total_stroke_length( float_adjust(CurrentPart.total_stroke_length) );
CurrentPart.set_min_stroke_length( float_adjust(CurrentPart.min_stroke_length) );

refresh_basic_setup();

CurrentPart.set_sleeve_fill_distance( float_adjust(CurrentPart.sleeve_fill_distance) );
CurrentPart.set_runner_fill_distance( float_adjust(CurrentPart.runner_fill_distance) );
CurrentPart.set_csfs_min_position(    float_adjust(CurrentPart.csfs_min_position)    );
CurrentPart.set_plunger_diameter(     float_adjust(CurrentPart.plunger_diameter)     );

refresh_advanced_setup();

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    s = CurrentLimitSwitch.pos(i);
    if ( !s.isempty() )
        {
        x = s.real_value();
        if ( !is_zero(x) )
            {
            x = float_adjust(x);
            CurrentLimitSwitch.pos(i) = x;
            }
        }
    }

for ( i=0; i<USER_VEL_COUNT; i++ )
    CurrentPart.set_user_velocity_position( i, float_adjust(CurrentPart.user_velocity_position[i]) );

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    CurrentPart.set_user_avg_velocity_end(   i, float_adjust(CurrentPart.user_avg_velocity_end[i])   );
    CurrentPart.set_user_avg_velocity_start( i, float_adjust(CurrentPart.user_avg_velocity_start[i]) );
    }

refresh_user_defined_positions();

n = CurrentParam.count();
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].vartype & (DISTANCE_VAR | POSITION_VAR) )
        {
        SorcUnitsId = CurrentParam.parameter[i].units;
        for ( j=0; j<NOF_ALARM_LIMIT_TYPES; j++ )
            CurrentParam.parameter[i].limits[j].value = float_adjust( CurrentParam.parameter[i].limits[j].value );
        CurrentParam.parameter[i].units = DestUnitsId;
        }
    }

refresh_parameter_limits();
}

/***********************************************************************
*                        CHANGE_VELOCITY_UNITS                         *
***********************************************************************/
static void change_velocity_units()
{
int   old_button_index;
int   new_button_index;
int   i;
int   j;
int   n;
HWND  w;

w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

if ( !w )
    return;

new_button_index = NO_INDEX;

for ( i=0; i<NofVelocityButtons; i++ )
    {
    if ( is_checked(w, VelocityButton[i]) )
        {
        new_button_index = i;
        break;
        }
    }

if ( new_button_index == NO_INDEX )
    return;

old_button_index  = new_button_index;

/*
-----------------------
Get the previous button
----------------------- */
for ( i=0; i<NofVelocityButtons; i++ )
    {
    if ( VelocityButtonState[i] )
        {
        old_button_index = i;
        break;
        }
    }

if ( old_button_index == new_button_index )
    return;

DestUnitsId = VelocityUnits[new_button_index];
SorcUnitsId = VelocityUnits[old_button_index];

CurrentPart.velocity_units = DestUnitsId;

CurrentPart.set_eos_velocity( float_adjust(CurrentPart.eos_velocity) );

refresh_basic_setup();

CurrentPart.set_csfs_min_velocity(  float_adjust(CurrentPart.csfs_min_velocity) );
CurrentPart.set_csfs_rise_velocity( float_adjust(CurrentPart.csfs_rise_velocity) );

refresh_advanced_setup();

n = CurrentParam.count();
for ( i=0; i<n; i++ )
    {
    if ( CurrentParam.parameter[i].vartype & VELOCITY_VAR )
        {
        SorcUnitsId = CurrentParam.parameter[i].units;
        for ( j=0; j<NOF_ALARM_LIMIT_TYPES; j++ )
            CurrentParam.parameter[i].limits[j].value = float_adjust( CurrentParam.parameter[i].limits[j].value );
        CurrentParam.parameter[i].units = DestUnitsId;
        }
    }

refresh_parameter_limits();
}

/***********************************************************************
*                              IS_A_STEP                               *
*                                                                      *
* This checks to see if there is a number in one of the three          *
* editboxes begining with the id sent. I assume box is the id of the   *
* accel editbox.                                                       *
*                                                                      *
***********************************************************************/
BOOLEAN is_a_step( HWND w, int box  )
{
int32   i;
TCHAR   buf[STSTEPS_FLOAT_LEN+1];
TCHAR * cp;

for ( i=0; i<3; i++ )
    {
    if ( get_text(buf, w, box, STSTEPS_FLOAT_LEN) )
        {
        cp = buf;
        while ( *cp != NullChar )
            {
            if ( IsCharAlphaNumeric(*cp) && !IsCharAlpha(*cp) )
                return TRUE;
            cp++;
            }
        }
    box++;
    }

return FALSE;
}

/***********************************************************************
*                     GET_NEW_VACUUM_SWITCH_NUMBER                     *
***********************************************************************/
static void get_new_vacuum_switch_number()
{
int i;
int box;
HWND w;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

i = NO_WIRE;
for ( box=VAC_SWITCH_1_RADIO; box<=VAC_SWITCH_6_RADIO; box++ )
    {
    if ( is_checked(w, box) )
        {
        i = 1 + box - VAC_SWITCH_1_RADIO;
        break;
        }
    }

if ( i != SureTrakSetup.vacuum_limit_switch_number() )
    {
    SureTrakSetup.set_vacuum_limit_switch_number(i);
    SureTrakHasChanged = TRUE;
    }
}

/***********************************************************************
*               CHECK_FOR_NEW_INTERMEDIATE_STEP_END_POS                *
* If this machine has a binary valve the value of the pre fast and fast*
* limit switches is based on the intermediate step end position. The   *
* intermediate step is the last one that the Sure-Trak2 controls as    *
* the HMI does the fast shot.                                          *
***********************************************************************/
static void check_for_new_intermediate_step_end_pos( HWND parent, int id )
{
int  box;
int  i;
bool i_own_the_flag;
int  last_endpoint_id;
HWND w;
NAME_CLASS      s;
SETPOINT_CLASS  sp;
double end_position;
double x;

/*
---------------------------------------------------
Get the id of the endpoint editbox of the last step
--------------------------------------------------- */
box = ACCEL_1_EDITBOX;
last_endpoint_id = 0;
for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    if ( !is_a_step(parent, box) )
        break;
    last_endpoint_id = box + 2;
    box += CONTROLS_PER_STEP;
    }

/*
--------------------------------------------
Don't do anything if the first step is blank
-------------------------------------------- */
if ( last_endpoint_id == 0 )
    return;

i_own_the_flag = false;
if ( !LoadingSureTrakSteps )
    {
    LoadingSureTrakSteps = TRUE;
    i_own_the_flag = true;
    }

if ( id == last_endpoint_id )
    {
    /*
    ------------------------------------------------------------------------------------
    Read the binary valve defaults to get the limit switch numbers for pre fast and fast
    ------------------------------------------------------------------------------------ */
    s.get_default_hmi_settings_csvname();
    sp.get( s );

    s.get_text( parent, id );
    end_position = s.real_value();

    /*
    -------------------------------------------
    The Fast LS is the same as the end position
    ------------------------------------------- */
    i = sp[DEF_HMI_FAST_LS_INDEX].int_value();
    if ( i > 0 )
        {
        i--;
        w = GetDlgItem( parent, LS1_POS_EB + i );
        s.set_text( w );
        add_to_stmodlist( w );
        }

    /*
    -----------------------------------
    Pre Fast is end position - 3 inches
    ----------------------------------- */
    i = sp[DEF_HMI_PRE_FAST_LS_INDEX].int_value();
    if ( i > 0 )
        {
        i--;
        x = 3.0;
        if ( CurrentPart.distance_units != INCH_UNITS )
            x = adjust_for_units( CurrentPart.distance_units, x, INCH_UNITS );
        x *= double(-1.0);
        x += end_position;
        s = x;
        w = GetDlgItem( parent, LS1_POS_EB + i );
        s.set_text( w );
        add_to_stmodlist( w );
        }
    sp.cleanup();
    }

if ( i_own_the_flag )
    LoadingSureTrakSteps = FALSE;
}

/***********************************************************************
*                      GET_NEW_SURETRAK_STEPS                          *
***********************************************************************/
static void get_new_suretrak_steps()
{
int          box;
TCHAR        buf[MAX_FLOAT_LEN+1];
int          i;
int          n;
STRING_CLASS oldli;
HWND         w;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

/*
----------------------------------------------
Get the current count to see if it has changed
---------------------------------------------- */
n = SureTrakSetup.nof_steps();
if ( n > 0 )
    {
    SureTrakSetup.gotostep( n );
    oldli = SureTrakSetup.low_impact_string();
    }

box = ACCEL_1_EDITBOX;
SureTrakSetup.clear_steps();
for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    if ( !is_a_step(w, box) )
        break;

    SureTrakSetup.append();

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_accel( buf );
    box++;

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_vel( buf );
    box++;

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_end_pos( buf );
    box++;

    SureTrakSetup.set_have_vacuum_wait( is_checked(w, box) );
    box++;

    get_text(buf, w, box, STSTEPS_FLOAT_LEN );
    strip( buf );
    SureTrakSetup.set_low_impact( buf );
    box++;

    SureTrakSetup.set_have_low_impact_from_input( is_checked(w, box) );
    box++;

    if ( SureTrakSetup.accel_is_percent() && SureTrakSetup.vel_is_percent() )
        stringtable_message_box( TEXT("NO_BOTH_PERCENT"), TEXT("INVALID_ENTRY") );
    }


i = SureTrakSetup.nof_steps();
if ( n != i && i>0 )
    {
    if ( n > i  )
        {
        /*
        -------------------------------------------------------------------------------
        I have just reduced the number of steps. If there was a low impact value on the
        old last step I need to copy it to the new last step. gotostep( [1,6] );
        ------------------------------------------------------------------------------- */
        if ( !oldli.isempty() )
            {
            SureTrakSetup.gotostep(i);
            SureTrakSetup.set_low_impact( oldli.text() );
            /*
            --------------------------------------------------------------
            Put the new low impact editbox on the list of changed controls
            First convert i to an index.
            -------------------------------------------------------------- */
            i--;
            box = LI_1_EDITBOX + i*CONTROLS_PER_STEP;
            add_to_stmodlist( GetDlgItem(w, box) );
            i++;
            }

        if ( CurrentMachine.has_binary_valve() )
            {
            i--;
            box = EP_1_EDITBOX + i*CONTROLS_PER_STEP;
            check_for_new_intermediate_step_end_pos( w, box );
            }
        }

    enable_last_step_low_impact_eb();
    }

get_new_vacuum_switch_number();

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    CurrentLimitSwitch.desc(i).get_text( w, LS1_DESC_EB+i );
    CurrentLimitSwitch.pos(i).get_text( w, LS1_POS_EB+i );
    if ( !HideSureTrakLimitSwitchWires )
        CurrentLimitSwitch.wire(i).get_text( w, LS1_WIRE_PB+i );
    }

SureTrakHasChanged = TRUE;
}

/***********************************************************************
*                          IS_VACUUM_CHECKBOX                          *
***********************************************************************/
static BOOLEAN is_vacuum_checkbox( int id )
{
int i;
int bid;

bid = VW_1_CHECKBOX;
for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    if ( bid == id )
        return TRUE;

    bid += CONTROLS_PER_STEP;
    }

return FALSE;
}

/***********************************************************************
*                      CHECK_VACUUM_WAIT_BUTTONS                       *
***********************************************************************/
static void check_vacuum_wait_buttons( HWND w, int id )
{
int i;
int bid;
HWND wep;

if ( !is_vacuum_checkbox(id) )
    return;

/*
------------------------------------------------
Redisplay the end position editbox for this step
to set or remove the highlight.
------------------------------------------------ */
InvalidateRect( GetDlgItem(w, id-1), 0, TRUE );

if ( !is_checked(w, id) )
    {
    /*
    ------------------------------------
    Erase the fast shot position message
    ------------------------------------ */
    ShowWindow( GetDlgItem(w, FS_POSITION_MSG_TBOX), SW_HIDE );

    return;
    }

bid = VW_1_CHECKBOX;
for ( i=0; i<MAX_ST_STEPS; i++ )
    {
    if ( bid != id )
        {
        if ( is_checked(w, bid) )
            {
            set_checkbox( w, bid, FALSE );
            wep = GetDlgItem( w, bid-1 );
            EnableWindow( wep, TRUE );
            /*
            --------------------------------------------------
            Redisplay the end position to remove the highlight
            -------------------------------------------------- */
            InvalidateRect( wep, 0, TRUE );
            }

        }

    bid += CONTROLS_PER_STEP;
    }

//ShowWindow( GetDlgItem(w, id-1), SW_HIDE );
ShowWindow( GetDlgItem(w, FS_POSITION_MSG_TBOX), SW_SHOWNORMAL );

/*------------------------------------------------------------------------
i = SureTrakSetup.vacuum_limit_switch_index();
if ( i != NO_INDEX )
    {
    --------------------------------
    See if the limit switch is empty
    --------------------------------
    if ( CurrentLimitSwitch.pos(i).isempty() )
        {
        SetFocus( GetDlgItem(w, LS1_POS_EB+i) );
        resource_message_box( w, MainInstance, NO_START_OF_FS_LS_STRING, SHOULD_BE_ENTERED_STRING, MB_OK | MB_SYSTEMMODAL );
        }
    }
--------------------------------------------------------------------------*/

}

/***********************************************************************
*                     CHECK_FOR_LAST_STEP_END_POS                      *
* Return TRUE if there is an end position for the last step. If not -  *
* nag, nag, nag.                                                       *
***********************************************************************/
static BOOLEAN check_for_last_step_end_pos()
{
int i;
STRING_CLASS s;
HWND w;

w = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;
i = SureTrakSetup.nof_steps();
if ( i > 0 )
    {
    SureTrakSetup.gotostep( i );
    /*
    -------------------------
    Check for an end position
    ------------------------- */
    s = SureTrakSetup.end_pos_string();
    if ( s.isempty() || s.int_value() < 1 )
        {
        DialogBox(
           MainInstance,
           TEXT("NO_LAST_STEP_END_POS_DIALOG"),
           w,
           (DLGPROC) no_last_step_end_pos_dialog_proc );
        i--;
        i *= CONTROLS_PER_STEP;
        i += EP_1_EDITBOX;
        SetFocus( GetDlgItem(w, i) );
        return FALSE;
        }

    /*
    ------------------------------
    Check for a low impact setting
    ------------------------------ */
    s = SureTrakSetup.low_impact_string();
    if ( s.isempty() )
        {
        DialogBox(
           MainInstance,
           TEXT("NO_LAST_STEP_LOW_IMPACT_DIALOG"),
           w,
           (DLGPROC) no_last_step_low_impact_dialog_proc );
        i--;
        i *= CONTROLS_PER_STEP;
        i += LI_1_EDITBOX;
        SetFocus( GetDlgItem(w, i) );
        return FALSE;
        }
    }

return TRUE;
}

/***********************************************************************
*                            UPDATE_ST_PARAMS                          *
***********************************************************************/
static void update_st_params( HWND w )
{
SureTrakSetup.gain_break_velocity(0).get_text( w, BREAK_VEL_1_EBOX );
SureTrakSetup.gain_break_velocity(1).get_text( w, BREAK_VEL_2_EBOX );
SureTrakSetup.gain_break_velocity(2).get_text( w, BREAK_VEL_3_EBOX );
SureTrakSetup.gain_break_gain(0).get_text( w, BREAK_GAIN_1_EBOX );
SureTrakSetup.gain_break_gain(1).get_text( w, BREAK_GAIN_2_EBOX );
SureTrakSetup.gain_break_gain(2).get_text( w, BREAK_GAIN_3_EBOX );
SureTrakSetup.velocity_loop_gain().get_text( w, VEL_LOOP_GAIN_EBOX );
SureTrakSetup.ft_stop_pos().get_text( w, FT_STOP_POS_EBOX );

SureTrakHasChanged = TRUE;
}

/***********************************************************************
*                       ST_PARAMS_CONTEXT_HELP                         *
***********************************************************************/
static void st_params_context_help( HWND parent, LPARAM lParam )
{
static HELP_ENTRY static_help[] = {
      { VEL_LOOP_GAIN_STATIC,             SURETRAK_VEL_LOOP_GAIN_HELP  },
      { BREAK_VEL_STATIC,                 SURETRAK_BREAK_VEL_HELP      },
      { BREAK_1_STATIC,                   SURETRAK_BREAK_VEL_HELP      },
      { BREAK_GAIN_STATIC,                SURETRAK_BREAK_GAIN_HELP     }
      };

const int32 NOF_STATIC_CONTROLS = sizeof(static_help)/sizeof(HELP_ENTRY);

int32 i;
POINT p;
HWND  w;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    w = GetDlgItem( parent, static_help[i].id );
    if ( GetWindowRect(w, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            gethelp( HELP_CONTEXTPOPUP, static_help[i].context );
            return;
            }
        }
    }

gethelp( HELP_CONTEXT, SURETRAK_PART_PARAMETER_HELP );
}

/***********************************************************************
*                    SURETRAK_PROFILE_CONTEXT_HELP                     *
***********************************************************************/
static void suretrak_profile_context_help( HWND cw, LPARAM lParam )
{
static HELP_ENTRY static_help[] = {
      { ACCEL_STATIC,                     SURETRAK_ACCEL_POPUP_HELP    },
      { VELOCITY_STATIC,                  SURETRAK_VELOCITY_POPUP_HELP },
      { ENDING_POS_STATIC,                SURETRAK_END_POS_POPUP_HELP  },
      { VACUUM_WAIT_STATIC,               SURETRAK_VAC_WAIT_POPUP_HELP },
      { LOW_IMPACT_STATIC,                SURETRAK_LI_POPUP_HELP       },
      { LOW_IMPACT_FROM_INPUT_STATIC,     SURETRAK_LI_INPUT_HELP       },
      { LIMIT_SWITCH_POS_STATIC,          SURETRAK_LS_HELP             },
      { VEL_LOOP_GAIN_STATIC,             SURETRAK_VEL_LOOP_GAIN_HELP  },
      { BREAK_VEL_STATIC,                 SURETRAK_BREAK_VEL_HELP      },
      { BREAK_GAIN_STATIC,                SURETRAK_BREAK_GAIN_HELP     },
      { WARMUP_GROUP_STATIC,              SURETRAK_WARMUP_SHOT_HELP    },
      { FT_STOP_POS_STATIC,               FOLLOW_THROUGH_STOP_POSITION_HELP },
      { BREAK_2_STATIC,                   SURETRAK_BREAK_GAIN_HELP },
      { BREAK_3_STATIC,                   SURETRAK_BREAK_GAIN_HELP }
      };

const int32 NOF_STATIC_CONTROLS = sizeof(static_help)/sizeof(HELP_ENTRY);

DWORD mycontext;
int32 i;
POINT p;
HWND  dw;
HWND  w;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

dw = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    w = GetDlgItem( dw, static_help[i].id );
    if ( GetWindowRect(w, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            mycontext = static_help[i].context;
            gethelp( HELP_CONTEXTPOPUP, static_help[i].context );
            return;
            }
        }
    }

w = GetDlgItem( dw, LS4_WIRE_PB );
GetWindowRect( w, &r );

/*
----------------------------------------------------------------------
The low word of lParam is the x position of the mouse. If it is to the
right of the limit switch stuff then get the break gain help screen.
---------------------------------------------------------------------- */
if ( LOWORD(lParam) > r.right )
    {
    gethelp( HELP_CONTEXT, SURETRAK_PART_PARAMETER_HELP );
    }
else
    {
    get_overview_help();
    }
}

/**********************************************************************
*                          SHOW_PERCENT_ERROR                         *
**********************************************************************/
void show_percent_error()
{
STRING_CLASS title;
STRING_CLASS message;

if ( TogglingProportionalValve )
    return;
title = StringTable.find( TEXT("INPUT_ERROR_STRING") );
message = StringTable.find( TEXT("PERCENT_NOT_ALLOWED") );
MessageBox( 0, message.text(), title.text(), MB_OK );
}

/***********************************************************************
*                             GET_PERCENTS                             *
***********************************************************************/
static void get_percents( PRES_CONTROL_STEP_ENTRY * se )
{
if ( PressureControl.proportional_valve )
    {
    se->is_percent       = TRUE;
    if ( se->is_goose_step )
        se->is_goose_percent = TRUE;
    else
        se->is_goose_percent = FALSE;
    }
else
    {
    se->is_percent       = FALSE;
    se->is_goose_percent = FALSE;
    }
}

/***********************************************************************
*                    GET_NEW_PRESSURE_CONTROL_STEPS                    *
***********************************************************************/
static void get_new_pressure_control_steps()
{
BOOLEAN b;
int32 i;
HWND  w;
HWND  goose_button;
LISTBOX_CLASS lb;
PRES_CONTROL_STEP_ENTRY * se;

w = WindowInfo[PRESSURE_CONTROL_TAB].dialogwindow;

if ( !w )
    return;

b = is_checked(w, ALL_OPEN_LOOP_XBOX);

lb.init( w, PC_PRES_SENSOR_CBOX );

PressureControl.sensor_number = lb.selected_index();

for ( i=0; i<3; i++ )
    {
    if ( is_checked(w,  PC_2_VOLT_RANGE_RADIO+i) )
        {
        PressureControl.voltage_output_range = i;
        break;
        }
    }

PressureControl.retctl_arm_pres.get_text( w, RETCTL_ARM_PRES_EBOX );
PressureControl.integral_gain.get_text( w, PC_INTEGRAL_GAIN_EBOX );
PressureControl.derivative_gain.get_text( w, PC_DERIVATIVE_GAIN_EBOX );
PressureControl.pressure_loop_gain.get_text( w, PC_LOOP_GAIN_EBOX );
PressureControl.retctl_pres.get_text(w, RETCTL_PRES_EBOX );
PressureControl.retract_volts.get_text(w, RETRACT_PRES_EBOX );
PressureControl.park_volts.get_text(w, PARK_PRES_EBOX );
PressureControl.digital_pot_gain.get_text(w, DIGITAL_POT_GAIN_EBOX );

PressureControl.is_enabled = is_checked( w, PRES_CONTROL_ENABLE_XBOX );
PressureControl.use_only_positive_voltages = is_checked( w, POSITIVE_ONLY_XBOX );
PressureControl.retctl_is_enabled = is_checked( w, RETCTL_ENABLE_XBOX );
PressureControl.proportional_valve = is_checked( w, PROPORTIONAL_VALVE_XBOX );

lb.init( w, PC_ARM_LS_CBOX );
PressureControl.arm_intensifier_ls_number = lb.selected_index() + 1;

se = &PressureControl.step[0];
se->ramp.get_text( w, RAMP_1_EBOX );
goose_button = GetDlgItem( w, PC_GOOSE_1_XBOX );
if ( PressureControl.proportional_valve )
    se->is_goose_step = FALSE;
else
    se->is_goose_step = is_checked( goose_button );
se->pressure.get_text( w, PRES_1_EBOX );
if ( se->pressure.contains(PercentChar) && !PressureControl.proportional_valve )
    {
    set_percent( se->pressure, FALSE );
    se->pressure.set_text( w, PRES_1_EBOX );
    show_percent_error();
    }
get_percents( se );
se->is_open_loop = b; // is_checked( w, PC_OPEN_LOOP_1_XBOX );
se->holdtime.get_text( w, HOLD_TIME_1_EBOX  );

se = &PressureControl.step[1];
se->ramp.get_text( w, RAMP_2_EBOX );
goose_button = GetDlgItem( w, PC_GOOSE_2_XBOX );
if ( PressureControl.proportional_valve )
    se->is_goose_step = FALSE;
else
    se->is_goose_step = is_checked( goose_button );
se->pressure.get_text( w, PRES_2_EBOX );
if ( se->pressure.contains(PercentChar) && !PressureControl.proportional_valve )
    {
    set_percent( se->pressure, FALSE );
    se->pressure.set_text( w, PRES_2_EBOX );
    show_percent_error();
    }
get_percents( se );
se->is_open_loop = b; // is_checked( w, PC_OPEN_LOOP_2_XBOX );
se->holdtime.get_text( w, HOLD_TIME_2_EBOX  );

se = &PressureControl.step[2];
se->ramp.get_text(     w, RAMP_3_EBOX );
goose_button = GetDlgItem( w, PC_GOOSE_3_XBOX );
if ( PressureControl.proportional_valve )
    se->is_goose_step = FALSE;
else
    se->is_goose_step = is_checked( goose_button );
se->pressure.get_text( w, PRES_3_EBOX );
if ( se->pressure.contains(PercentChar) && !PressureControl.proportional_valve )
    {
    set_percent( se->pressure, FALSE );
    se->pressure.set_text( w, PRES_3_EBOX );
    show_percent_error();
    }
get_percents( se );
se->is_open_loop = b; //is_checked( w, PC_OPEN_LOOP_3_XBOX );
se->holdtime.get_text( w, HOLD_TIME_3_EBOX  );

PressureControl.nof_steps = 0;
for ( i=0; i<MAX_PRESSURE_CONTROL_STEPS; i++ )
    {
    if ( !PressureControl.step[i].pressure.isempty() )
        PressureControl.nof_steps = i+1;
    }
}

/***********************************************************************
*                            SETUP_SHEET_PROC                          *
***********************************************************************/
BOOL CALLBACK setup_sheet_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        break;
    }

return FALSE;
}

/***********************************************************************
*                  PRESSURE_CONTROL_CONTEXT_HELP                       *
***********************************************************************/
static BOOL pressure_control_context_help( HWND w, LPARAM lParam )
{

static UINT static_id[] = {
    RAMP_STATIC,
    PRES_STATIC,
    OPEN_LOOP_STATIC,
    HOLD_TIME_STATIC,
    PRES_LOOP_GAIN_STATIC,
    RETRACT_VOLTS_STATIC,
    VALVE_PARK_VOLTS_STATIC,
    RETCTL_ARM_PRES_TBOX,
    RETCTL_PRES_TBOX,
    PC_PRES_SENSOR_STATIC,
    OUTPUT_VOLTAGE_RANGE_STATIC,
    RETCTL_GBOX
    };
const int32 NOF_STATIC_CONTROLS = sizeof(static_id)/sizeof(UINT);

static DWORD static_context[] = {
    PC_RAMP_HELP,
    PC_PRES_HELP,
    PC_OPEN_LOOP_HELP,
    PC_HOLD_TIME_HELP,
    PC_LOOP_GAIN_HELP,
    RETRACT_VOLTS_HELP,
    VALVE_PARK_VOLTS_HELP,
    RETCTL_ARM_PRES_HELP,
    RETCTL_PRES_HELP,
    PC_PRES_SENSOR_HELP,
    OUTPUT_VOLTAGE_RANGE_HELP,
    RETCTL_HELP
    };

static UINT myid[] = {
    RAMP_1_EBOX,
    PC_GOOSE_1_XBOX,
    PRES_1_EBOX,
    PC_OPEN_LOOP_1_XBOX,
    HOLD_TIME_1_EBOX,
    RAMP_2_EBOX,
    PC_GOOSE_2_XBOX,
    PRES_2_EBOX,
    PC_OPEN_LOOP_2_XBOX,
    HOLD_TIME_2_EBOX,
    RAMP_3_EBOX,
    PC_GOOSE_3_XBOX,
    PRES_3_EBOX,
    PC_OPEN_LOOP_3_XBOX,
    HOLD_TIME_3_EBOX,
    PRES_CONTROL_ENABLE_XBOX,
    POSITIVE_ONLY_XBOX,
    RETCTL_ENABLE_XBOX,
    PROPORTIONAL_VALVE_XBOX,
    ALL_OPEN_LOOP_XBOX,
    PC_PRES_SENSOR_CBOX,
    PC_2_VOLT_RANGE_RADIO,
    PC_5_VOLT_RANGE_RADIO,
    PC_10_VOLT_RANGE_RADIO,
    PC_LOOP_GAIN_EBOX,
    RETRACT_PRES_EBOX,
    PARK_PRES_EBOX,
    RETCTL_PRES_EBOX,
    RETCTL_ARM_PRES_EBOX,
    DIGITAL_POT_GAIN_EBOX
    };
const int32 NOF_CONTROLS = sizeof(myid)/sizeof(UINT);

static DWORD mycontext[] = {
    PC_RAMP_HELP,
    PC_GOOSE_BUTTON_HELP,
    PC_PRES_HELP,
    PC_OPEN_LOOP_HELP,
    PC_HOLD_TIME_HELP,
    PC_RAMP_HELP,
    PC_GOOSE_BUTTON_HELP,
    PC_PRES_HELP,
    PC_OPEN_LOOP_HELP,
    PC_HOLD_TIME_HELP,
    PC_RAMP_HELP,
    PC_GOOSE_BUTTON_HELP,
    PC_PRES_HELP,
    PC_OPEN_LOOP_HELP,
    PC_HOLD_TIME_HELP,
    PRES_CONTROL_ENABLE_HELP,
    POSITIVE_ONLY_HELP,
    RETCTL_ENABLE_HELP,
    PROPORTIONAL_VALVE_HELP,
    PC_OPEN_LOOP_HELP,
    PC_PRES_SENSOR_HELP,
    OUTPUT_VOLTAGE_RANGE_HELP,
    OUTPUT_VOLTAGE_RANGE_HELP,
    OUTPUT_VOLTAGE_RANGE_HELP,
    PC_LOOP_GAIN_HELP,
    RETRACT_VOLTS_HELP,
    VALVE_PARK_VOLTS_HELP,
    RETCTL_PRES_HELP,
    RETCTL_ARM_PRES_HELP,
    DIGITAL_POT_GAIN_HELP
    };

int32 i;
POINT p;
HWND  bw;
HWND  sw;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

bw = WindowInfo[PRESSURE_CONTROL_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    sw = GetDlgItem(bw, static_id[i] );
    if ( GetWindowRect(sw, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            gethelp( HELP_CONTEXTPOPUP, static_context[i] );
            return TRUE;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(bw, myid[i]) == w )
        {
        gethelp( HELP_CONTEXTPOPUP, mycontext[i] );
        return TRUE;
        }
    }

get_overview_help();
return TRUE;
}

/***********************************************************************
*                        DO_GOOSE_BUTTON_PRESS                         *
***********************************************************************/
static void do_goose_button_press( HWND goose_button, HWND parent, int goose_button_id )
{
HWND ebox;

BOOLEAN goose_button_pressed;
BOOLEAN ramp_is_percent;
STRING_CLASS s;
TCHAR * cp;

ebox = GetDlgItem( parent, goose_button_id-1 );

/*
------------------------------------------------------------------------------------------
At the present time the only time you can use percents is if you have a proportional valve
------------------------------------------------------------------------------------------ */
if ( is_checked(parent, PROPORTIONAL_VALVE_XBOX) )
    {
    s.get_text( ebox );
    ramp_is_percent = s.contains( PercentChar );
    goose_button_pressed = is_checked( goose_button );
    if ( ramp_is_percent && !goose_button_pressed )
        {
        cp = s.find( PercentChar );
        if ( cp )
            *cp = NullChar;
        s.set_text( ebox );
        }
    else if ( goose_button_pressed && !ramp_is_percent )
        {
        if ( !s.isempty() )
            {
            s += PercentChar;
            s.set_text( ebox );
            }
        set_text( goose_button, PercentString );
        return;
        }
    }

label_goose_button( goose_button, FALSE );
}

/***********************************************************************
*                         DO_RAMP_VALUE_CHANGE                         *
*    This checks for a ramp change from % to no % or vise versa.       *
***********************************************************************/
static void do_ramp_value_change( HWND parent, int id )
{
HWND goose_button;
HWND ebox;

BOOLEAN goose_button_pressed;
BOOLEAN ramp_is_percent;
STRING_CLASS s;

ebox = GetDlgItem( parent, id );
goose_button = GetDlgItem( parent, id+1 );

s.get_text( ebox );
ramp_is_percent = s.contains( PercentChar );
goose_button_pressed = is_checked( goose_button );

/*
------------------------------------------------------------------------------------------
At the present time the only time you can use percents is if you have a proportional valve
------------------------------------------------------------------------------------------ */
if ( !is_checked(parent, PROPORTIONAL_VALVE_XBOX) )
    {
    if ( ramp_is_percent )
        {
        set_percent( s, FALSE );
        s.set_text( ebox );
        show_percent_error();
        }
    return;
    }

/*
--------------------------------------------
See if a % has just been added to the string
-------------------------------------------- */
if ( ramp_is_percent && !goose_button_pressed )
    {
    set_checkbox( goose_button, TRUE );
    set_text( goose_button, PercentString );
    }
else if ( goose_button_pressed && !ramp_is_percent )
    {
    set_checkbox( goose_button, FALSE );
    label_goose_button( goose_button, FALSE );
    }
}

/**********************************************************************
*           UPDATE_PRESSURE_CONTROL_PROFILE_STRING_SETTINGS           *
**********************************************************************/
static void update_pressure_control_profile_string_settings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { PC_PROFILE_SETUP_STATIC,     TEXT( "PC_PROFILE_SETUP" ) },
    { RAMP_STATIC,                 TEXT( "RAMP_STRING" ) },
    { HOLD_TIME_STATIC,            TEXT( "HOLD_TIME" ) },
    { PRES_CONTROL_ENABLE_XBOX,    TEXT( "ENABLE_PRES_CONTROL" ) },
    { PC_2_VOLT_RANGE_RADIO,       TEXT( "TWO_VOLT_RANGE" ) },
    { PC_5_VOLT_RANGE_RADIO,       TEXT( "FIVE_VOLT_RANGE" ) },
    { PC_10_VOLT_RANGE_RADIO,      TEXT( "TEN_VOLT_RANGE" ) },
    { POSITIVE_ONLY_XBOX,          TEXT( "POSITIVE_ONLY" ) },
    { RETCTL_GBOX,                 TEXT( "RETRACT_CONTROL" ) },
    { RETCTL_PRES_TBOX,            TEXT( "RETRACT_PERCENT" ) },
    { RETCTL_ARM_PRES_TBOX,        TEXT( "ARM_PERCENT" ) },
    { RETCTL_ENABLE_XBOX,          TEXT( "RETRACT_ENABLE" ) },
    { PROPORTIONAL_VALVE_XBOX,     TEXT( "TOTAL_OPEN_LOOP" ) },
    { PC_PRES_SENSOR_STATIC,       TEXT( "PRES_SENSOR" ) },
    { PRES_LOOP_GAIN_STATIC,       TEXT( "PRES_LOOP_GAIN" ) },
    { PC_INTEGRAL_GAIN_STATIC,     TEXT( "INTEGRAL_GAIN" ) },
    { PC_DERIVATIVE_GAIN_STATIC,   TEXT( "DERIVATIVE_GAIN" ) },
    { OUTPUT_VOLTAGE_RANGE_STATIC, TEXT( "OUTPUT_VOLT_RANGE" ) },
    { ALL_OPEN_LOOP_XBOX,          TEXT( "HARDWARE_CLOSED_LOOP" ) },
    { PC_ARM_LS_STATIC,            TEXT( "PC_ARM_LS" ) },
    { PERCENT_ONLY_STATIC,         TEXT( "PERCENT_ONLY" ) },
    { DIGITAL_POT_GAIN_STATIC,     TEXT( "DIGITAL_POT_GAIN" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, PRES_STATIC );
set_statics_font( hWnd, RETRACT_VOLTS_STATIC );
set_statics_font( hWnd, VALVE_PARK_VOLTS_STATIC );
set_statics_font( hWnd, PC_GOOSE_1_XBOX );
set_statics_font( hWnd, PC_GOOSE_2_XBOX );
set_statics_font( hWnd, PC_GOOSE_3_XBOX );
set_statics_font( hWnd, PC_PRES_SENSOR_CBOX );
set_statics_font( hWnd, PC_ARM_LS_CBOX );
set_statics_font( hWnd, PC_1_MS_UNITS_TB );
set_statics_font( hWnd, PC_2_MS_UNITS_TB );
set_statics_font( hWnd, PC_3_MS_UNITS_TB );
}

/***********************************************************************
*                       PRESSURE_CONTROL_PROFILE_PROC                  *
***********************************************************************/
BOOL CALLBACK pressure_control_profile_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   cmd;
int   i;
int   id;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_pressure_control_profile_string_settings( hWnd );
        LoadingPressureControl = TRUE;
        fill_analog_list_boxes( hWnd, PC_PRES_SENSOR_CBOX, 1 );
        LoadingPressureControl = FALSE;
        break;

    case WM_CONTEXTMENU:
        return pressure_control_context_help( (HWND) wParam, lParam );

    case WM_NEW_DATA:
        /*
        ------------------------------------------------------------------------------------------------
        This is used by the ls description screen to tell me the limit switch descriptions have changed.
        ------------------------------------------------------------------------------------------------ */
        load_pc_arm_limit_switch_cbox( hWnd );
        break;

    case WM_COMMAND:
        if ( id == PC_GOOSE_1_XBOX || id == PC_GOOSE_2_XBOX || id == PC_GOOSE_3_XBOX )
            {
            if ( cmd == BN_CLICKED )
                do_goose_button_press( (HWND) lParam, hWnd, id );
            }

        if ( id == PRES_CONTROL_ENABLE_XBOX )
            {
            show_pres_controls( hWnd );
            }

        if ( id == RETCTL_ENABLE_XBOX )
            {
            show_retctl_controls( hWnd );
            }

        if ( id == PROPORTIONAL_VALVE_XBOX )
            toggle_proportional_valve(hWnd);

        if ( id == ALL_OPEN_LOOP_XBOX )
            do_all_open_loop_xbox( hWnd );

        if ( !LoadingPressureControl )
            {
            if ( id == DIGITAL_POT_GAIN_EBOX && cmd == EN_CHANGE )
                {
                i = get_int_from_ebox( hWnd, DIGITAL_POT_GAIN_EBOX );
                if ( i<MIN_DIGITAL_POT_GAIN )
                    set_int_in_ebox( hWnd, DIGITAL_POT_GAIN_EBOX, MIN_DIGITAL_POT_GAIN );
                else if ( i > MAX_DIGITAL_POT_GAIN )
                    set_int_in_ebox( hWnd, DIGITAL_POT_GAIN_EBOX, MAX_DIGITAL_POT_GAIN );
                }

            if ( id == PC_PRES_SENSOR_CBOX && cmd == CBN_SELCHANGE )
                {
                get_new_pressure_control_steps();
                //refresh_pressure_control_plot();
                }
            else if ( cmd == EN_CHANGE || cmd == BN_CLICKED )
                {
                if ( id >= RAMP_1_EBOX && id <= POSITIVE_ONLY_XBOX )
                    {
                    if ( id == RAMP_1_EBOX || id == RAMP_2_EBOX || id == RAMP_3_EBOX )
                        do_ramp_value_change( hWnd, id );
                    get_new_pressure_control_steps();
                    //SureTrakPlot.empty_curves();
                    //add_pressure_control_curve();
                    //SureTrakPlotWindow.refresh();
                    return TRUE;
                    }
                }
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                       CHECK_FOR_NEW_LAST_STEP                        *
* I now only enable the low impact editbox for the last step. This     *
* routine is called whenever a step value changes to see if a new step *
* is being created. If so the current low impact setting is moved to   *
* the low impact eb of the new last step.                              *
***********************************************************************/
static void check_for_new_last_step()
{
}

/**********************************************************************
*                UPDATE_SURETRAK_PROFILE_STRING_SETTINGS              *
**********************************************************************/
static void update_suretrak_profile_string_settings()
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { ACCEL_STATIC,                  TEXT( "ACCEL" ) },
    { VELOCITY_STATIC,               TEXT( "VELOCITY_STRING" ) },
    { ENDING_POS_STATIC,             TEXT( "ENDING_POS" ) },
    { VACUUM_WAIT_STATIC,            TEXT( "VACUUM_WAIT" ) },
    { LOW_IMPACT_STATIC,             TEXT( "LOW_IMPACT" ) },
    { LOW_IMPACT_FROM_INPUT_STATIC,  TEXT( "LOW_IMPACT_FROM_INPUT" ) },
    { LIMIT_SWITCH_POS_STATIC,       TEXT( "LIMIT_SWITCHES" ) },
    { ST_LS_DESC_STATIC,             TEXT( "DESCRIPTION" ) },
    { ST_LS_POS_STATIC,              TEXT( "POS_STRING" ) },
    { ST_LS_WIRE_STATIC,             TEXT( "WIRE_STRING" ) },
    { ST_VAC_SWITCH_STATIC,          TEXT( "VAC_SWITCH" ) },
    { FT_STOP_POS_STATIC,            TEXT( "FT_STOP_POS" ) },
    { VEL_LOOP_GAIN_STATIC,          TEXT( "VELOCITY_LOOP_GAIN" ) },
    { BREAK_SETTINGS_STATIC,         TEXT( "BREAK_SETTINGS" ) },
    { HAVE_ST_MOD_TBOX,              TEXT( "CHANGES_NOT_SAVED" ) },
    { FS_POSITION_MSG_TBOX,          TEXT( "FS_START_POS" ) },
    { FT_STOP_POS_STATIC,            TEXT( "FT_STOP_POS" ) },
    { BREAK_VEL_STATIC,              TEXT( "VELOCITY_STRING" ) },
    { BREAK_GAIN_STATIC,             TEXT( "GAIN_STRING" ) },
    { WARMUP_GROUP_STATIC,           TEXT( "WARMUP_SHOT" ) },
    { WARMUP_IF_STATIC,              TEXT( "WARMUP_IF_STRING" ) },
    { WARMUP_ENABLE_XBOX,            TEXT( "ENABLE_STRING" ) },
    { WARMUP_IS_STATIC,              TEXT( "IS_STRING" ) },
    { WARMUP_RUN_IF_ON_XBOX,         TEXT( "OFF_STRING" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( SuretrakProfileWindow, rlist, nr );

set_statics_font( SuretrakProfileWindow, WARMUP_PART_CBOX );
set_statics_font( SuretrakProfileWindow, LS1_DESC_EB );
set_statics_font( SuretrakProfileWindow, LS2_DESC_EB );
set_statics_font( SuretrakProfileWindow, LS3_DESC_EB );
set_statics_font( SuretrakProfileWindow, LS4_DESC_EB );
set_statics_font( SuretrakProfileWindow, LS5_DESC_EB );
set_statics_font( SuretrakProfileWindow, LS6_DESC_EB );
}

/***********************************************************************
*                          SURETRAK_PROFILE_PROC                       *
***********************************************************************/
BOOL CALLBACK suretrak_profile_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static const COLORREF mycolor  = RGB( 255, 255,   0 );

static const int warmup_control[] = {
    WARMUP_WIRE_EBOX,
    WARMUP_RUN_IF_ON_XBOX,
    WARMUP_IF_STATIC,
    WARMUP_IS_STATIC,
    WARMUP_GROUP_STATIC
    };
static const int nof_warmup_controls = sizeof(warmup_control)/sizeof(int);

int  bid;
int  i;
int  id;
int  cmd;
HWND w;
HDC  dc;
bool need_color;
bool is_modified;
WINDOW_CLASS    wc;
RECTANGLE_CLASS dest;

static HBRUSH mybrush = 0;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SuretrakProfileWindow = hWnd;
        update_suretrak_profile_string_settings();
        HaveStModTbox = GetDlgItem( hWnd, HAVE_ST_MOD_TBOX );
        HaveStModTbox.hide();
        LoadingSureTrakSteps = TRUE;
        for ( i= ACCEL_1_EDITBOX; i<=LIFI_6_CHECKBOX; i++ )
            set_text( hWnd, i, EmptyString );
        for ( i= LS1_DESC_EB; i<=LS6_WIRE_PB; i++ )
            set_text( hWnd, i, EmptyString );
        LoadingSureTrakSteps = FALSE;
        ShowWindow( GetDlgItem(hWnd, FS_POSITION_MSG_TBOX), SW_HIDE );

        show_warmup_controls();
        if ( CurrentMachine.multipart_runlist_settings.enable  )
            {
            for ( i=0; i<nof_warmup_controls; i++ )
                {
                wc = GetDlgItem( hWnd, warmup_control[i] );
                wc.hide();
                }
            }
        break;

    case WM_CONTEXTMENU:
        suretrak_profile_context_help( (HWND) wParam, lParam );
        return TRUE;

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
        w  = (HWND) lParam;
        dc = (HDC)  wParam;

        if ( HaveStModTbox == w )
            {
            SetTextColor( dc, RedColor );
            SetBkColor( dc, EboxBackgroundColor );
            return (int) EboxBackgroundBrush;
            }

        is_modified = false;
        need_color  = false;
        if ( w == GetDlgItem(hWnd, FS_POSITION_MSG_TBOX) )
            {
            need_color = true;
            }
        else
            {
            bid = VW_1_CHECKBOX;
            for ( i=0; i<MAX_ST_STEPS; i++ )
                {
                if ( is_checked(hWnd, bid) )
                    {
                    /*
                    --------------------------------------------------------------
                    The id of the end pos eb for this step is one less than the vw
                    -------------------------------------------------------------- */
                    bid--;
                    if ( w == GetDlgItem(hWnd,bid) )
                        need_color = true;
                    break;
                    }
                bid += CONTROLS_PER_STEP;
                }
            }

        if ( msg == WM_CTLCOLOREDIT && StModList.contains(w) )
            {
            is_modified = true;
            SetTextColor( dc, RedColor );
            }

        if ( need_color )
            {
            if ( !mybrush )
                mybrush = CreateSolidBrush( mycolor );

            if ( mybrush )
                {
                SetBkColor( dc, mycolor );
                return (int) mybrush;
                }
            }
        else if ( is_modified )
            {
            SetBkColor( dc, EboxBackgroundColor );
            return (int) EboxBackgroundBrush;
            }
        break;

    case WM_DESTROY:
        if ( mybrush )
            {
            DeleteObject( mybrush );
            mybrush = 0;
            }
        break;

    case WM_COMMAND:
        if ( cmd == BN_CLICKED )
            {
            switch ( id )
                {
                case LS1_WIRE_PB:
                case LS2_WIRE_PB:
                case LS3_WIRE_PB:
                case LS4_WIRE_PB:
                case LS5_WIRE_PB:
                case LS6_WIRE_PB:
                    if ( HideSureTrakLimitSwitchWires )
                        return TRUE;
                    if ( choose_wire( GetDlgItem(hWnd, id), hWnd, (int) LIMIT_SWITCH_OUTPUT_TYPE, (int) id-LS1_WIRE_PB, &CurrentMachine, &CurrentPart, &CurrentParam, &CurrentLimitSwitch) )
                        {
                        SureTrakHasChanged = TRUE;
                        if ( !HaveStModTbox.is_visible() )
                            HaveStModTbox.show();
                        }
                    return TRUE;
                }

            if ( id == WARMUP_RUN_IF_ON_XBOX )
                {
                set_run_if_on_text( hWnd );
                }
            else
                {
                check_vacuum_wait_buttons( hWnd, id );
                }
            }

        if ( cmd == CBN_SELCHANGE || cmd == EN_CHANGE || cmd == BN_CLICKED )
            {
            if ( !LoadingSureTrakSteps )
                {
                w = (HWND) lParam;
                add_to_stmodlist( w );

                if ( (id >= BREAK_GAIN_1_EBOX && id <= VEL_LOOP_GAIN_EBOX) || id == FT_STOP_POS_EBOX )
                    {
                    update_st_params( hWnd );
                    }
                else if ( id >= ACCEL_1_EDITBOX && id <= LS6_POS_EB )
                    {
                    if ( id <= LIFI_6_CHECKBOX )
                        {
                        check_for_new_last_step();
                        }
                    if ( CurrentMachine.has_binary_valve() )
                        check_for_new_intermediate_step_end_pos( hWnd, id );
                    get_new_suretrak_steps();
                    SureTrakPlot.empty_curves();
                    SureTrakPlot.empty_axes();
                    add_x_axis_to_plot();
                    add_y_axis_to_plot();
                    add_profile_curve();
                    SureTrakPlotWindow.refresh();
                    return TRUE;
                    }
                else if ( id>=VAC_SWITCH_1_RADIO && id <=VAC_SWITCH_6_RADIO )
                    {
                    get_new_vacuum_switch_number();
                    }
                }
            }
        break;
    }

return FALSE;
}

/**********************************************************************
*                    UPDATE_BASIC_SETUP_STRINGS                       *
**********************************************************************/
static void update_basic_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { INCHES_BUTTON,  TEXT("INCHES") },
    { MM_BUTTON, TEXT("MILLIMETERS") },
    { CM_BUTTON, TEXT("CENTIMETERS") },
    { IPS_BUTTON,  TEXT("IPS") },
    { FPM_BUTTON, TEXT("FPM") },
    { CMPS_BUTTON, TEXT("CMPS") },
    { MPS_BUTTON, TEXT("MPS") },
    { DISTANCE_UNITS_STATIC, TEXT("DISTANCE_UNITS") },
    { VELOCITY_UNITS_STATIC, TEXT("VELOCITY_UNITS") },
    { TOTAL_STROKE_STATIC, TEXT("TOTAL_STROKE_LENGTH") },
    { MIN_STROKE_STATIC, TEXT("MIN_STROKE_LEN") },
    { EOS_STATIC, TEXT("EOS_VELOCITY") },
    { ZERO_SPEED_OUTPUT_GB, TEXT("ZERO_SPEED_OUTPUT") },
    { ZERO_SPEED_OUTPUT_WIRE_STATIC, TEXT("OUTPUT_WIRE") },
    { ZERO_SPEED_OUTPUT_ENABLE_XB, TEXT("ENABLE_STRING") }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );
}

/***********************************************************************
*                               GET_INT                                *
***********************************************************************/
static int get_int( HWND w )
{
STRING_CLASS s;
s.get_text( w );
return s.int_value();
}

/***********************************************************************
*                          BASIC_SETUP_PROC                            *
***********************************************************************/
BOOL CALLBACK basic_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;
HWND w;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_CONTEXTMENU:
        return show_context_help( (HWND) wParam, lParam );

    case WM_DBINIT:
        update_basic_setup_strings( hWnd );
        SetWindowContextHelpId( GetDlgItem(hWnd, INCHES_BUTTON), 100 );
        SetWindowContextHelpId( GetDlgItem(hWnd, MM_BUTTON),     100 );
        SetWindowContextHelpId( GetDlgItem(hWnd, CM_BUTTON),     100 );
        return TRUE;

    case WM_COMMAND:
        if ( cmd == BN_CLICKED )
            {
            /*
            -----------------------------
            Get the handle of the control
            ----------------------------- */
            w = (HWND) lParam;

            switch ( id )
                {
                case INCHES_BUTTON:
                case MM_BUTTON:
                case CM_BUTTON:
                    change_distance_units();
                    return TRUE;

                case IPS_BUTTON:
                case FPM_BUTTON:
                case CMPS_BUTTON:
                case MPS_BUTTON:
                    change_velocity_units();
                    return TRUE;

                case ZERO_SPEED_OUTPUT_ENABLE_XB:
                    CurrentPart.set_zero_speed_output_enable( is_checked(w) );
                    return TRUE;

                case ZERO_SPEED_OUTPUT_WIRE_PB:
                    choose_wire( w, hWnd, ZERO_SPEED_OUTPUT_TYPE, NO_INDEX, &CurrentMachine, &CurrentPart, &CurrentParam, &CurrentLimitSwitch );
                    CurrentPart.set_zero_speed_output_wire( short(get_int(w)) );
                    return TRUE;
                }
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      RETURN_FROM_PARAMETER_EDIT                      *
***********************************************************************/
void return_from_parameter_edit()
{
if ( CurrentDialogNumber != NO_TAB )
    SetFocus( WindowInfo[CurrentDialogNumber].dialogwindow );
}

/***********************************************************************
*                            N_SELECTED_LB                             *
***********************************************************************/
static short n_selected_lb( HWND w, int box )
{
LRESULT x;

x = SendDlgItemMessage( w, box, LB_GETSELCOUNT, 0, 0L );
if ( x == LB_ERR )
    x = 0;

return short( x );
}

/***********************************************************************
*                      GET_SELECTED_COPYTO_MACHINE                     *
***********************************************************************/
static MACHINE_ENTRY * get_selected_copyto_machine()
{
STRING_CLASS machine;
LISTBOX_CLASS cb;

cb.init( CopyToDialogWindow, COPYTO_MACHINE_CB_ID );
machine = cb.selected_text();

if ( machine.isempty() )
    return 0;

return find_machine_entry( machine );
}

/***********************************************************************
*                        COPYTO_SELECTED_PARTS                         *
***********************************************************************/
static void copyto_selected_parts( unsigned copy_flags )
{
INT * ip;
int i;
int n;
LISTBOX_CLASS   lb;
STRING_CLASS    dest;
MACHINE_ENTRY * m;

m = get_selected_copyto_machine();
if ( !m )
    return;

lb.init( CopyToDialogWindow, COPYTO_PART_LISTBOX_ID );
n = lb.get_select_list( &ip );
if ( n <= 0 )
    return;

for ( i=0; i<n; i++ )
    {
    dest = lb.item_text( ip[i] );
    if ( !dest.isempty() )
        copypart( m->computer, m->name, dest, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, copy_flags );
    }

if ( ip )
    delete[] ip;
}

/***********************************************************************
*                            COPYTO_MACHINE                            *
***********************************************************************/
void copyto_machine( MACHINE_ENTRY * m, unsigned copy_flags, short copy_type )
{
BOOLEAN      need_to_copy;
AUTO_BACKUP_CLASS ab;
COMPUTER_CLASS    c;
STRING_CLASS dest;

ab.get_backup_dir();
m->partlist.rewind();
while ( m->partlist.next() )
    {
    dest = m->partlist.text();

    need_to_copy = TRUE;
    if ( copy_type == COPYTO_SAME_PARTS_RADIO )
        if ( CurrentPart.name != dest )
            need_to_copy = FALSE;

    if ( need_to_copy )
        {
        copypart( m->computer, m->name, dest, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, copy_flags );
        if ( m->computer == c.whoami() )
            ab.backup_part( m->name, dest );

        /*
        -----------------------------------------------------------------------
        If I have a warmup part, make sure it exists on the destination machine
        ----------------------------------------------------------------------- */
        if ( copy_flags & COPY_PART_WARMUP_PART )
            {
            if ( !Warmup.partname.isempty() )
                {
                if ( !part_exists(m->computer, m->name, Warmup.partname) )
                    {
                    copy_flags = COPY_PART_ALL & ~COPY_PART_WARMUP_PART;
                    copypart( m->computer, m->name, Warmup.partname, CurrentPart.computer, CurrentPart.machine, Warmup.partname, copy_flags );
                    if ( m->computer == c.whoami() )
                        ab.backup_part( m->name, Warmup.partname );
                    }
                }
            }
        }
    }
}

/***********************************************************************
*                          COPYTO_COPY_FLAGS                           *
***********************************************************************/
static unsigned copyto_copy_flags()
{
unsigned copy_flags;

copy_flags = 0;

if ( is_checked(CopyToDialogWindow, COPYTO_BASIC_CHECKBOX)  )
    copy_flags |= COPY_PART_BASIC;

if ( is_checked(CopyToDialogWindow, COPYTO_ADVANCED_CHECKBOX) )
    copy_flags |= COPY_PART_ADVANCED;

if ( is_checked(CopyToDialogWindow, COPYTO_SHOT_NAME_XBOX) )
    copy_flags |= COPY_PART_SHOT_NAME;

if ( is_checked(CopyToDialogWindow, COPYTO_SHOT_NAME_RESET_XBOX) )
    copy_flags |= COPY_PART_SHOT_NAME_RESET;

if ( is_checked(CopyToDialogWindow, COPYTO_PARAMETER_CHECKBOX) )
    copy_flags |= COPY_PART_PARAMETERS;

if ( is_checked(CopyToDialogWindow, COPYTO_MARKS_CHECKBOX) )
    copy_flags |= COPY_PART_MARKS;

if ( is_checked(CopyToDialogWindow, COPYTO_USER_POSITIONS_CHECKBOX) )
    copy_flags |= COPY_PART_USER_POSITIONS;

if ( is_checked(CopyToDialogWindow, COPYTO_CHANNELS_CHECKBOX) )
    copy_flags |= COPY_PART_CHANNELS;

if ( is_checked(CopyToDialogWindow, COPYTO_REFERENCE_CHECKBOX) )
    copy_flags |= COPY_PART_REFERENCE_TRACE;

if ( is_checked(CopyToDialogWindow, COPYTO_SURETRAK_CHECKBOX) )
    copy_flags |= COPY_PART_SURETRAK_SETUP;

if ( is_checked(CopyToDialogWindow, COPYTO_PARAM_NAME_CHECKBOX) )
    copy_flags |= COPY_PART_PARAM_NAMES;

if ( is_checked(CopyToDialogWindow, COPYTO_WARMUP_PART_CHECKBOX) )
    copy_flags |= COPY_PART_WARMUP_PART;

if ( is_checked(CopyToDialogWindow, COPYTO_PART_DISPLAY_CHECKBOX) )
    copy_flags |= COPY_PART_DISPLAY_SETUP;

if ( is_checked(CopyToDialogWindow, COPYTO_CHANNEL_AXIS_LABELS_XBOX) )
    copy_flags |= COPY_PART_CHAXIS_LABEL;

if ( is_checked(CopyToDialogWindow, COPYTO_PARAM_SORT_XBOX) )
    copy_flags |= COPY_PART_PARAM_SORT;

if ( is_checked(CopyToDialogWindow, COPYTO_PRES_CONTROL_XBOX) )
    copy_flags |= COPY_PART_PRES_CONTROL;

if ( is_checked(CopyToDialogWindow, COPYTO_LS_DESC_XBOX) )
    copy_flags |= COPY_PART_LS_DESC;

if ( is_checked(CopyToDialogWindow, COPYTO_LS_POS_XBOX) )
    copy_flags |= COPY_PART_LS_POS;

if ( is_checked(CopyToDialogWindow, COPYTO_LS_WIRES_XBOX) )
    copy_flags |= COPY_PART_LS_WIRES;

return copy_flags;
}

/***********************************************************************
*                       DO_COPYTO_SELECT_ALL_PB                        *
***********************************************************************/
static void do_copyto_select_all_pb()
{
int button[]  = { COPYTO_BASIC_CHECKBOX, COPYTO_ADVANCED_CHECKBOX, COPYTO_SHOT_NAME_XBOX, COPYTO_SHOT_NAME_RESET_XBOX,
                  COPYTO_PARAMETER_CHECKBOX, COPYTO_MARKS_CHECKBOX, COPYTO_USER_POSITIONS_CHECKBOX, COPYTO_CHANNELS_CHECKBOX,
                  COPYTO_REFERENCE_CHECKBOX, COPYTO_SURETRAK_CHECKBOX, COPYTO_PARAM_NAME_CHECKBOX, COPYTO_WARMUP_PART_CHECKBOX,
                  COPYTO_PART_DISPLAY_CHECKBOX, COPYTO_CHANNEL_AXIS_LABELS_XBOX, COPYTO_PARAM_SORT_XBOX, COPYTO_PRES_CONTROL_XBOX,
                  COPYTO_LS_DESC_XBOX, COPYTO_LS_POS_XBOX, COPYTO_LS_WIRES_XBOX };
const int nof_buttons = sizeof( button ) / sizeof( int );
HWND handle[nof_buttons];
int i;
int nselected;
BOOLEAN checked;

nselected = 0;
for ( i=0; i<nof_buttons; i++ )
    {
    handle[i] = GetDlgItem( CopyToDialogWindow, button[i] );
    if ( is_checked(handle[i]) )
        nselected++;
    }

checked = FALSE;
if ( nselected < nof_buttons )
    checked = TRUE;

for ( i=0; i<nof_buttons; i++ )
    set_checkbox( handle[i], checked );
}

/***********************************************************************
*                            COPYTO_COPY_TYPE                          *
***********************************************************************/
static short copyto_copy_type()
{
short copy_type;

copy_type = 0;

if ( is_checked(CopyToDialogWindow, COPYTO_SEL_PARTS_RADIO) )
    copy_type = COPYTO_SEL_PARTS_RADIO;

else if ( is_checked(CopyToDialogWindow, COPYTO_ALL_PARTS_RADIO) )
    copy_type = COPYTO_ALL_PARTS_RADIO;

else if ( is_checked(CopyToDialogWindow, COPYTO_ALL_PARTS_MACH_RADIO) )
    copy_type = COPYTO_ALL_PARTS_MACH_RADIO;

else if ( is_checked(CopyToDialogWindow, COPYTO_SAME_PARTS_RADIO) )
    copy_type = COPYTO_SAME_PARTS_RADIO;

return copy_type;
}

/***********************************************************************
*                             COPYTO_COPY                              *
***********************************************************************/
static void copyto_copy()
{
MACHINE_ENTRY * m;
unsigned        copy_flags;
short           copy_type;

if ( !CopyToDialogWindow )
    return;

copy_flags = copyto_copy_flags();
copy_type  = copyto_copy_type();

if ( copy_type == COPYTO_SEL_PARTS_RADIO )
    {
    copyto_selected_parts( copy_flags );
    }
else if ( copy_type == COPYTO_ALL_PARTS_RADIO )
    {
    m = get_selected_copyto_machine();
    if ( m )
        copyto_machine( m, copy_flags, copy_type );
    }
else
    {
    MachineList.rewind();
    while ( TRUE )
        {
        m = (MACHINE_ENTRY *) MachineList.next();
        if ( !m )
            break;
        copyto_machine( m, copy_flags, copy_type );
        }
    }
}

/***********************************************************************
*                          FILL_COPYTO_PARTS                           *
***********************************************************************/
static void fill_copyto_parts()
{
MACHINE_ENTRY * m;
STRING_CLASS    mymachine;
LISTBOX_CLASS   cb;

if ( !CopyToDialogWindow )
    return;
cb.init( CopyToDialogWindow, COPYTO_MACHINE_CB_ID );
mymachine = cb.selected_text();
if ( !mymachine.isempty() )
    {
    m = find_machine_entry( mymachine );
    if ( m )
        fill_part_listbox( CopyToDialogWindow, COPYTO_PART_LISTBOX_ID, m );
    }
}

/***********************************************************************
*                        POSITION_COPY_DIALOG                          *
***********************************************************************/
static void position_copy_dialog( HWND hWnd )
{
RECT r;
int  h;
int  w;
int  x;
int  y;

GetWindowRect( hWnd, &r );

w = r.right - r.left;
h = r.bottom - r.top;

GetWindowRect( GetDlgItem(MainDialogWindow,PART_LISTBOX_ID), &r );

x = r.right;
y = r.top;

MoveWindow( hWnd, x, y, w, h, TRUE );
}

/**********************************************************************
*                    UPDATE_COPY_TO_STRING_SETTINGS                  *
**********************************************************************/
static void update_copy_to_string_settings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { COPYTO_MA_STATIC,               TEXT( "MACHINE_STRING" ) },
    { COPYTO_PA_STATIC,               TEXT( "PART_STRING" ) },
    { COPYTO_BASIC_CHECKBOX,          TEXT( "BASIC_SETUP" ) },
    { COPYTO_ADVANCED_CHECKBOX,       TEXT( "ADVANCED_SETUP" ) },
    { COPYTO_PARAMETER_CHECKBOX,      TEXT( "PARAMETER_LIMIT" ) },
    { COPYTO_USER_POSITIONS_CHECKBOX, TEXT( "USER_DEFINED_POS" ) },
    { COPYTO_MARKS_CHECKBOX,          TEXT( "PROFILE_MARKS" ) },
    { COPYTO_CHANNELS_CHECKBOX,       TEXT( "FASTRAK_CHANNELS" ) },
    { COPYTO_REFERENCE_CHECKBOX,      TEXT( "REFERENCE_TRACE" ) },
    { COPYTO_SURETRAK_CHECKBOX,       TEXT( "SURETRAK_STEPS" ) },
    { COPYTO_PARAM_NAME_CHECKBOX,     TEXT( "PARAMETER_NAMES" ) },
    { COPYTO_WARMUP_PART_CHECKBOX,    TEXT( "WARMUP_PART" ) },
    { COPYTO_PART_DISPLAY_CHECKBOX,   TEXT( "PART_DISPLAY_SETUP" ) },
    { COPYTO_CHANNEL_AXIS_LABELS_XBOX,TEXT( "CHANNEL_AXIS_LABELS" ) },
    { COPYTO_PARAM_SORT_XBOX,         TEXT( "PARAMETER_SORT_ORDER" ) },
    { COPYTO_PRES_CONTROL_XBOX,       TEXT( "PRES_CONTROL_TAB" ) },
    { COPYTO_SHOT_NAME_XBOX,          TEXT( "SHOT_NAME" ) },
    { COPYTO_SHOT_NAME_RESET_XBOX,    TEXT( "SHOT_NAME_RESET" ) },
    { COPYTO_LS_DESC_XBOX,            TEXT( "LS_DESC" ) },
    { COPYTO_LS_POS_XBOX,             TEXT( "LS_POS" ) },
    { COPYTO_LS_WIRES_XBOX,           TEXT( "LS_WIRE" ) },
    { COPYTO_SEL_PARTS_RADIO,         TEXT( "SELECTED_PARTS" ) },
    { COPYTO_ALL_PARTS_RADIO,         TEXT( "ALL_PARTS" ) },
    { COPYTO_ALL_PARTS_MACH_RADIO,    TEXT( "ALL_PARTS_ALL_MA" ) },
    { COPYTO_SAME_PARTS_RADIO,        TEXT( "SAME_PART_ALL_MA" ) },
    { COPYTO_SELECT_ALL_PB,           TEXT( "ALL_STRING" ) },
    { COPYTO_SECTIONS_STATIC,         TEXT( "SECTIONS_TO_COPY" ) },
    { COPYTO_PARTS_STATIC,            TEXT( "PARTS_TO_COPY_TO" ) },
    { IDOK,                           TEXT( "OK_STRING" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, COPYTO_PART_LISTBOX_ID );
set_statics_font( hWnd, COPYTO_MACHINE_CB_ID );
}

/***********************************************************************
*                         COPY_TO_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK copy_to_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        CopyToDialogWindow = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DESTROY:
        CopyToDialogWindow = 0;
        break;

    case WM_HELP:
        gethelp( HELP_CONTEXT, COPY_TO_EXISTING_HELP );
        return TRUE;

    case WM_DBINIT:
        position_copy_dialog( hWnd );
        update_copy_to_string_settings( hWnd );
        fill_machine_cb( hWnd, COPYTO_MACHINE_CB_ID );
        SendDlgItemMessage( hWnd, COPYTO_MACHINE_CB_ID, CB_SETCURSEL, 0, 0L );
        fill_copyto_parts();
        CheckRadioButton( hWnd, COPYTO_SEL_PARTS_RADIO, COPYTO_SAME_PARTS_RADIO, COPYTO_SEL_PARTS_RADIO );
        if ( CurrentMachine.multipart_runlist_settings.enable )
            set_text( hWnd, COPYTO_WARMUP_PART_CHECKBOX, StringTable.find(TEXT("MULTIPART_RUNLIST")) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case COPYTO_MACHINE_CB_ID:
                if ( cmd == CBN_SELCHANGE )
                    {
                    fill_copyto_parts();
                    return TRUE;
                    }
                break;

            case COPYTO_SELECT_ALL_PB:
                do_copyto_select_all_pb();
                break;

            case IDOK:
                hourglass_cursor();
                copyto_copy();
                restore_cursor();
                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                DestroyWindow( hWnd );
                break;
            }
    break;
    }


return FALSE;
}

/***********************************************************************
*                           FILL_MULTIPLE_LB                           *
***********************************************************************/
static void fill_multiple_lb()
{
COMPUTER_CLASS  c;
LISTBOX_CLASS   lb;
MACHINE_ENTRY * m;
STRING_CLASS    s;

if ( !CopyMultipleDialogWindow )
    return;

lb.init( CopyMultipleDialogWindow, CMULTIPLE_MACHINE_LISTBOX_ID );
lb.empty();
lb.set_tabs( CM_X_MARK_STATIC, CM_MACHINE_NAME_STATIC );

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;

    if ( CurrentMachine.name == m->name  )
        continue;

    if ( (m->computer != c.whoami()) && (CurrentPasswordLevel < EditMaOnOtherCoLevel) )
            continue;

    if ( find_part_entry(m, CurrentPart.name) )
        s = TEXT( "\tY\t" );
    else
        s = TEXT( "\t \t" );

    s += m->name;
    lb.add( s.text() );
    }
}

/***********************************************************************
*                            COPY_MULTIPLE                             *
***********************************************************************/
static void copy_multiple()
{
AUTO_BACKUP_CLASS ab;
COMPUTER_CLASS    c;
LISTBOX_CLASS   lb;
MACHINE_ENTRY * m;
STRING_CLASS    dest;
TCHAR * cp;
int     i;
int     n;
INT   * ip;
unsigned copy_flags;

lb.init( CopyMultipleDialogWindow, CMULTIPLE_MACHINE_LISTBOX_ID );

n = lb.get_select_list( &ip );
if ( n <= 0 )
    return;

ab.get_backup_dir();
for ( i=0; i<n; i++ )
    {
    dest = lb.item_text( ip[i] );
    if ( dest.len() > HAS_PART_PREFIX_LEN )
        {
        cp = dest.text() + HAS_PART_PREFIX_LEN;
        copystring( dest.text(), cp );
        m = find_machine_entry( dest );
        if ( m )
            {
            copypart( m->computer, m->name, CurrentPart.name, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, COPY_PART_ALL );

            if ( !Warmup.partname.isempty() )
                {
                if ( !part_exists( m->computer, m->name, Warmup.partname) )
                    {
                    copy_flags = COPY_PART_ALL & ~COPY_PART_WARMUP_PART;
                    copypart( m->computer, m->name, Warmup.partname, CurrentPart.computer, CurrentPart.machine, Warmup.partname, copy_flags );
                    if ( m->computer == c.whoami() )
                        ab.backup_part( m->name, Warmup.partname );
                    }
                }
            reload_partlist( m->name );
            }
        }
    }

if ( ip )
    delete[] ip;
}

/**********************************************************************
*                  UPDATE_COPY_MULTIPLE_STRING_SETTINGS               *
**********************************************************************/
static void update_copy_multiple_string_settings()
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { CM_X_MARK_STATIC,       TEXT( "HAS_PART" ) },
    { CM_MACHINE_NAME_STATIC, TEXT( "MACHINE_NAME" ) },
    { IDOK,                   TEXT( "BEGIN_COPYING" ) },
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

WINDOW_CLASS w;

update_statics( CopyMultipleDialogWindow, rlist, nr );

w = CopyMultipleDialogWindow;
w.set_title( StringTable.find(TEXT("CMULTIPLE_CAPTION")) );
w.refresh();
set_statics_font( CopyMultipleDialogWindow, CMULTIPLE_MACHINE_LISTBOX_ID );
}

/***********************************************************************
*                     COPY_MULTIPLE_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK copy_multiple_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        CopyMultipleDialogWindow = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DESTROY:
        CopyMultipleDialogWindow = 0;
        break;

    case WM_DBINIT:
        position_copy_dialog( hWnd );
        update_copy_multiple_string_settings();
        fill_multiple_lb();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                copy_multiple();
                DestroyWindow( hWnd );
                return TRUE;

            case IDCANCEL:
                DestroyWindow( hWnd );
                break;
            }
    break;
    }


return FALSE;
}


/***********************************************************************
*                 POSITION_STATIC_TEXT_PARAMETER_DIALOG                *
***********************************************************************/
static void position_static_text_parameter_dialog( HWND hWnd )
{
RECT r;
int  h;
int  w;
int  x;
int  y;

GetWindowRect( hWnd, &r );

w = r.right - r.left;
h = r.bottom - r.top;

GetWindowRect( GetDlgItem(MainDialogWindow,PART_LISTBOX_ID), &r );

x = r.right;
y = r.top;

MoveWindow( hWnd, x, y, w, h, TRUE );
}

/**********************************************************************
*           UPDATE_STATIC_TEXT_PARAMETER_STRING_SETTINGS              *
**********************************************************************/
static void update_static_text_parameter_string_settings( HWND hWnd )
{
WINDOW_CLASS w;
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { STATIC_TEXT_PARAMETER_OK_PB,   TEXT( "OK_STRING" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

w = hWnd;
w.set_title( StringTable.find(TEXT("POPUP_STATIC_TEXT_CAPTION")) );
w.refresh();

set_statics_font( hWnd, STATIC_TEXT_PARAMETER_VALUE_EB );
set_statics_font( hWnd, STATIC_TEXT_PARAMETER_NAME_TB );
}

/***********************************************************************
*                    STATIC_TEXT_PARAMETER_DIALOG_PROC                 *
* Note: You MUST initialize StaticTextParameterNumber before opening   *
* the static_text_parameter_window;                                    *
***********************************************************************/
BOOL CALLBACK static_text_parameter_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  i;
int  cmd;
int  id;
HWND w;
bool loading_data;
loading_data = false;
cmd = HIWORD( wParam );
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_static_text_parameter_string_settings( hWnd );
        loading_data = true;
        i = StaticTextParameterNumber - 1;
        if ( i>=0 && i<MAX_PARMS )
            {
            set_text( hWnd, STATIC_TEXT_PARAMETER_NAME_TB, CurrentParam.parameter[i].name );
            w = GetDlgItem( hWnd, STATIC_TEXT_PARAMETER_VALUE_EB );
            CurrentParam.parameter[i].static_text.value.set_text( w );
            SetFocus( w );
            eb_select_all( w );
            }
        loading_data = false;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        position_static_text_parameter_dialog( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case STATIC_TEXT_PARAMETER_VALUE_EB:
                if ( cmd == EN_CHANGE && !loading_data )
                    {
                    w = GetDlgItem( hWnd, STATIC_TEXT_PARAMETER_VALUE_EB );
                    i = StaticTextParameterNumber - 1;
                    if ( i>=0 && i<MAX_PARMS )
                        CurrentParam.parameter[i].static_text.value.get_text( w );
                    }
                return TRUE;

            case STATIC_TEXT_PARAMETER_OK_PB:
                if ( cmd == BN_CLICKED )
                    {
                    i = StaticTextParameterNumber - 1;
                    if ( i>=0 && i<MAX_PARMS )
                        {
                        CurrentParam.parameter[i].static_text.init( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
                        CurrentParam.parameter[i].static_text.put( i+1 );
                        }
                    EndDialog( hWnd, 0 );
                    }
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                     POSITION_ADD_PART_DIALOG                         *
***********************************************************************/
static void position_add_part_dialog( HWND hWnd )
{
RECT r;
int  h;
int  w;
int  x;
int  y;

GetWindowRect( hWnd, &r );

w = r.right - r.left;
h = r.bottom - r.top;

GetWindowRect( GetDlgItem(MainDialogWindow,PART_LISTBOX_ID), &r );

x = r.right;
y = r.top;

MoveWindow( hWnd, x, y, w, h, TRUE );
}

/***********************************************************************
*                              SUB_PROC                                *
***********************************************************************/
LRESULT CALLBACK sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static TCHAR good_chars[] = TEXT( "-_+ " );
TCHAR c;

if ( msg == WM_CHAR )
    {
    c = (TCHAR) wParam;
    if ( c != VK_BACK )
        {
        if ( !IsCharAlphaNumeric(c) && !findchar(c, good_chars) )
            {
            MessageBeep( 0xFFFFFFFF );
            return 0;
            }
        }
    }

return CallWindowProc( OldEditProc, w, msg, wParam, lParam );
}

/**********************************************************************
*                       FILL_ADD_PART_STATICS                         *
**********************************************************************/
static void fill_add_part_statics( HWND hWnd )
{
static TCHAR ADDPART_CAPTION[] = TEXT( "CREATE_PART" );
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { IDOK,                       TEXT( "OK_STRING" ) },
    { ADD_PART_PART_TB,           TEXT("NEW_PART_NAME") },
    { ADD_PART_COPY_REF_TRACE_XB, TEXT("COPY_REF_TRACE") }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);
WINDOW_CLASS w;

update_statics( hWnd, rlist, nr );

w = hWnd;
w.set_title( StringTable.find(ADDPART_CAPTION) );
w.refresh();
set_statics_font( hWnd, ADD_PART_PART_EDITBOX  );
}

/***********************************************************************
*                ASSIGN_THE_NEXT_PART_SELECTION_NUMBER                 *
***********************************************************************/
static void assign_the_next_part_selection_number( STRING_CLASS & new_part_name )
{
int                           i;
int                           nof_psn;
int                         * psn;
MULTIPART_RUNLIST_WIRES_CLASS rw;
STRING_CLASS                  s;
TEXT_LIST_CLASS               t;

nof_psn = CurrentMachine.multipart_runlist_settings.max_part_selection_number();
if ( nof_psn <= 0 )
    return;

nof_psn++;
psn = new int[nof_psn];
if ( !psn )
    return;

for ( i=0; i<nof_psn; i++ )
    psn[i] = 0;

if ( get_current_partlist(t) )
    {
    t.rewind();
    while ( t.next() )
        {
        s = t.text();
        i = -1;
        if ( rw.get(CurrentMachine.computer, CurrentMachine.name, s) )
            i = rw.part_selection_number;
        if ( 0 <= i && i < nof_psn )
            psn[i] = 1;
        }

    for ( i=0; i<nof_psn; i++ )
        {
        if ( psn[i] == 0 )
            {
            rw.part_selection_number = i;
            CurrentMachine.multipart_runlist_settings.get_wirelist( rw.wires, i );
            rw.bits = bits_from_wirelist( rw.wires );
            rw.put( CurrentMachine.computer, CurrentMachine.name, new_part_name );
            break;
            }
        }
    }

delete [] psn;
}

/***********************************************************************
*                         ADD_PART_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK add_part_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int      id;
unsigned flags;
HWND     w;
STRING_CLASS name;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        AddPartDialogWindow = hWnd;
        w = GetDlgItem( AddPartDialogWindow, ADD_PART_PART_EDITBOX );
        OldEditProc = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) sub_proc );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        position_add_part_dialog( hWnd );
        fill_add_part_statics( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                name.get_text( hWnd, ADD_PART_PART_EDITBOX );
                if ( !name.isempty() )
                    {
                    flags = COPY_PART_ALL;
                    if ( !is_checked(hWnd, ADD_PART_COPY_REF_TRACE_XB) )
                        flags &= ~COPY_PART_REFERENCE_TRACE;
                    copypart( CurrentPart.computer, CurrentPart.machine, name, CurrentPart.computer, CurrentPart.machine, CurrentPart.name, flags );
                    if ( CurrentMachine.multipart_runlist_settings.enable )
                        assign_the_next_part_selection_number( name );
                    reload_partlist( CurrentPart.machine );
                    fill_parts( name );
                    }

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_DESTROY:
        AddPartDialogWindow = 0;
        break;
    }


return FALSE;
}

/***********************************************************************
*                      SET_PARAMETER_LISTBOX_TABS                      *
***********************************************************************/
static void set_parameter_listbox_tabs( HWND w )
{
static int ids[] = {
UNITS_STATIC, TARGET_STATIC, LOW_WARNING_STATIC, LOW_WARNING_WIRE_STATIC,
HIGH_WARNING_STATIC, HIGH_WARNING_WIRE_STATIC, LOW_ALARM_STATIC, LOW_ALARM_WIRE_STATIC,
HIGH_ALARM_STATIC, HIGH_ALARM_WIRE_STATIC
};

int NOF_TABS = sizeof(ids)/sizeof(int);

set_listbox_tabs_from_title_positions( w, PARAMETER_LIMIT_LISTBOX, ids, NOF_TABS );
}

/**********************************************************************
*                  UPDATE_NEW_PARAMETER_STRING_SETTINGS               *
**********************************************************************/
static void update_new_parameter_string_settings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { CREATE_FT_ANALOG_BUTTON,                TEXT( "FASTRAK_ANALOG_INPUT" ) },
    { CREATE_FT_ONOFF_BUTTON,                 TEXT( "FASTRAK_ONOFF_INPUT" ) },
    { CREATE_FT_TARGET_BUTTON,                TEXT( "TARGET_INPUT" ) },
    { CREATE_FT_DIGITAL_BUTTON,               TEXT( "DIGITAL_INPUT" ) },
    { CREATE_FT_CALC_BUTTON,                  TEXT( "CALCULATED_PARAMETER" ) },
    { CREATE_FT_EXTENDED_ANALOG_BUTTON,       TEXT( "EXTENDED_INPUT" ) },
    { CREATE_FT_PEAK_FAST_SHOT_BUTTON,        TEXT( "PEAK_FS_VELOCITY" ) },
    { CREATE_FT_EXTERNAL_PARAMETER_BUTTON,    TEXT( "EXTERNAL_PARAMETER" ) },
    { CREATE_FT_STATIC_TEXT_PARAMETER_BUTTON, TEXT( "STATIC_TEXT_PARAMETER" ) },
    { CHOOSE_PARAM_TYPE_STATIC,               TEXT( "CHOOSE_PARAM_TYPE" ) },
    { IDCANCEL,                               TEXT( "CANCEL" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );
}

/***********************************************************************
*                       NEW_PARAMETER_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK new_parameter_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
HWND w;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        NewParameterDialogWindow = hWnd;
        update_new_parameter_string_settings( hWnd );
        SetFocus( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CREATE_FT_ANALOG_BUTTON:
            case CREATE_FT_ONOFF_BUTTON:
            case CREATE_FT_TARGET_BUTTON:
            case CREATE_FT_DIGITAL_BUTTON:
            case CREATE_FT_EXTENDED_ANALOG_BUTTON:
            case CREATE_FT_CALC_BUTTON:
            case CREATE_FT_PEAK_FAST_SHOT_BUTTON:
            case CREATE_FT_EXTERNAL_PARAMETER_BUTTON:
            case CREATE_FT_STATIC_TEXT_PARAMETER_BUTTON:
                add_one_parameter( FT_ANALOG_INPUT + id - CREATE_FT_ANALOG_BUTTON );
                w = GetDlgItem( ParamDialogWindow, PARAMETER_DESC_EDITBOX );
                SetFocus( w );
                eb_select_all( w );
                NewParameterDialogWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;

            case IDCANCEL:
                NewParameterDialogWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                          MULTIPART_LB_CLASS                          *
***********************************************************************/
MULTIPART_LB_CLASS::MULTIPART_LB_CLASS()
{
count = 0;
array = 0;
}

/***********************************************************************
*                          ~MULTIPART_LB_CLASS                         *
***********************************************************************/
MULTIPART_LB_CLASS::~MULTIPART_LB_CLASS()
{
cleanup();
}

/***********************************************************************
*                          MULTIPART_LB_CLASS                          *
*                               cleanup                                *
***********************************************************************/
void MULTIPART_LB_CLASS::cleanup()
{
if ( array )
    {
    count = 0;
    delete[] array;
    array = 0;
    }
}

/***********************************************************************
*                          MULTIPART_LB_CLASS                          *
*                                 init                                 *
* This allocates an array of MULTIPART_LB_ENTRY's that is large enough *
* to hold all of the part selection numbers.                           *
***********************************************************************/
bool MULTIPART_LB_CLASS::init( int nof_wires )
{
int                              i;
MULTIPART_RUNLIST_SETTINGS_CLASS mrs;
int                              n;

cleanup();

if ( nof_wires < 1 )
    return false;

/*
-----------------------------------------
Calculate the number of wire combinations
----------------------------------------- */
mrs.get( CurrentPart.computer, CurrentPart.machine );
n = mrs.max_part_selection_number();
n++;

/*
-------------------------------------------------------------------------
Create the array and fill in the part selection number and the wire list.
Leave the part_name blank so I will know that it is unused.
------------------------------------------------------------------------- */
array = new MULTIPART_LB_ENTRY[n];
if ( array )
    {
    count = n;
    for ( i=0; i<count; i++ )
        {
        array[i].selection_number = i;
        mrs.get_wirelist( array[i].wire_list, i );
        }
    return true;
    }

return false;
}

/***********************************************************************
*                          MULTIPART_LB_CLASS                          *
*                                  =                                   *
* This will probably never be used but it is defined to prevent        *
* disaster.                                                            *
***********************************************************************/
void MULTIPART_LB_CLASS::operator=( const MULTIPART_LB_CLASS & sorc )
{
int i;
int n;

cleanup();
n = sorc.count;
if ( n > 0 )
    {
    if ( init(n) )
        {
        for ( i=0; i<n; i++ )
            {
            array[i] = sorc.array[i];
            }
        }
    }
}

/***********************************************************************
*                          MULTIPART_LB_CLASS                          *
*                                 []                                   *
***********************************************************************/
MULTIPART_LB_ENTRY & MULTIPART_LB_CLASS::operator[]( int index )
{
static MULTIPART_LB_ENTRY empty_entry;

if ( index >= 0 && index < count )
    return array[index];

return empty_entry;
}

/***********************************************************************
*                          MULTIPART_LB_CLASS                          *
*                                 read                                 *
* This reads the part selection number
***********************************************************************/
bool MULTIPART_LB_CLASS::read()
{
int                           i;
bool                          is_saved;
int                           max_part_count;
MULTISTRING_CLASS             ms;
int                           n;
int                           nof_parts;
MULTIPART_RUNLIST_WIRES_CLASS rw;
STRING_CLASS                  s;
TEXT_LIST_CLASS               t;

/*
-----------------------------
Get the number of input wires
----------------------------- */
n = CurrentMachine.multipart_runlist_settings.n;
if ( n < 1 )
    return false;

/*
------------------------
This should never happen
------------------------ */
if ( !get_current_partlist(t) )
    return false;

/*
-----------------------------------------------
See if there are enough wires for all the parts
----------------------------------------------- */
nof_parts = t.count();

if ( nof_parts < 1 )
    return false;

max_part_count = CurrentMachine.multipart_runlist_settings.max_part_selection_number();
max_part_count++;
if ( nof_parts > max_part_count )
    {
    stringtable_message_box( TEXT("TOO_FEW_INPUT_WIRES"), TEXT("UNABLE_TO_CONTINUE") );
    return false;
    }

/*
-------------------------------------------
Initialize the array of MULTIPART_LB_ENTRYs
------------------------------------------- */
if ( !init(n) )
    return false;

/*
------------------------------------------------------------------
Fill in the parts that already have a valid part selection number.
ms is a list of parts that do not have valid selection numbers.
------------------------------------------------------------------ */
t.rewind();
while ( t.next() )
    {
    s = t.text();
    is_saved = false;
    i = -1;
    if ( rw.get(CurrentPart.computer, CurrentPart.machine, s) )
        i = rw.part_selection_number;
    if ( i>=0 && i<count )
        {
        if ( array[i].part_name.isempty() )
            array[i].part_name = s;
        else
            ms.append( s );
        }
    else
        {
        ms.append( s );
        }
    }

/*
-------------------------------------------------------------
Put the unassigned part names into unused MULTIPART_LB_ENTRYs
------------------------------------------------------------- */
if ( ms.count() > 0 )
    {
    ms.rewind();
    while ( ms.next() )
        {
        for ( i=0; i<count; i++ )
            {
            if ( array[i].part_name.isempty() )
                {
                array[i].part_name = ms;
                /*
                ------------------
                Save the new setup
                ------------------ */
                rw.part_selection_number = i;
                rw.put( CurrentPart.computer, CurrentPart.machine, ms );

                break;
                }
            }
        }
    }

return true;
}

/***********************************************************************
*                          MULTIPART_LB_CLASS                          *
*                                write                                 *
***********************************************************************/
bool MULTIPART_LB_CLASS::write()
{
int                           i;
MULTIPART_RUNLIST_WIRES_CLASS rw;

if ( count < 1 )
    return false;

for ( i=0; i<count; i++ )
    {
    rw.part_selection_number = i;
    rw.wires                 = array[i].wire_list;
    rw.bits                  = bits_from_wirelist( rw.wires );
    rw.put( CurrentPart.computer, CurrentPart.machine, array[i].part_name );
    }

return true;
}

/***********************************************************************
*                          MULTIPART_LB_ENTRY                          *
***********************************************************************/
MULTIPART_LB_ENTRY::MULTIPART_LB_ENTRY()
{
}

/***********************************************************************
*                        ~MULTIPART_LB_ENTRY                           *
***********************************************************************/
MULTIPART_LB_ENTRY::~MULTIPART_LB_ENTRY()
{
}

/***********************************************************************
*                         MULTIPART_LB_ENTRY                           *
                                 =
***********************************************************************/
void MULTIPART_LB_ENTRY::operator=( const MULTIPART_LB_ENTRY & sorc )
{
selection_number = sorc.selection_number;
part_name        = sorc.part_name;
wire_list        = sorc.wire_list;
}

/***********************************************************************
*                               LB_LINE                                *
*  part selection number + tab + part name + tab + wire list           *
***********************************************************************/
bool MULTIPART_LB_ENTRY::lb_line( STRING_CLASS & dest )
{
dest = selection_number;
dest += TabChar;
dest += part_name;
dest += TabChar;
dest += wire_list;
return true;
}

/***********************************************************************
*                           FILL_MPR_LISTBOX                           *
***********************************************************************/
static bool fill_mpr_listbox( LISTBOX_CLASS & lb )
{
INT           current_index;
int           i;
int           n;
STRING_CLASS  s;

if ( !MultipartLbClass.read() )
    return false;

current_index = NO_INDEX;

n = MultipartLbClass.count;
for ( i=0; i<n; i++ )
    {
    MultipartLbClass[i].lb_line( s );
    lb.add( s.text() );
    if ( MultipartLbClass[i].part_name == CurrentPart.name )
        current_index = i;
    }

if ( current_index != NO_INDEX )
    lb.setcursel( current_index );

return true;
}

/**********************************************************************
*                   FILL_MULTIPART_RUNLIST_STATICS                    *
**********************************************************************/
static void fill_multipart_runlist_setup_statics( HWND hWnd )
{
static TCHAR CAPTION[] = TEXT( "MULTIPART_RUNLIST_SETUP" );
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { IDOK,                         TEXT( "SAVE_CHANGES")         },
    { IDCANCEL,                     TEXT( "CANCEL" )              },
    { PART_SELECTION_NUMBER_STATIC, TEXT("PART_SELECTION_NUMBER") },
    { PART_NAME_STATIC,             TEXT("PART_STRING")           },
    { WIRE_LIST_STATIC,             TEXT("WIRE_LIST")             }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);
WINDOW_CLASS w;

update_statics( hWnd, rlist, nr );

w = hWnd;
w.set_title( StringTable.find(CAPTION) );
w.refresh();
set_statics_font( hWnd, MULTIPART_RUNLIST_LB  );
}

/***********************************************************************
*                  MULTIPART_RUNLIST_SETUP_DIALOG_PROC                 *
***********************************************************************/
BOOL CALLBACK multipart_runlist_setup_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static INT current_index;
static LISTBOX_CLASS lb;

int          cmd;
STRING_CLASS current_name;
int          id;
INT          new_index;
STRING_CLASS new_name;
STRING_CLASS s;

cmd = HIWORD( wParam );
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        fill_multipart_runlist_setup_statics( hWnd );
        lb.init( hWnd, MULTIPART_RUNLIST_LB );
        lb.set_tabs( PART_SELECTION_NUMBER_STATIC, WIRE_LIST_STATIC );
        fill_mpr_listbox( lb );
        current_index = lb.selected_index();
        SetFocus( lb.handle() );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                MultipartLbClass.write();
                MultipartLbClass.cleanup();
                CurrentPart.multipart_runlist_wires.get( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
                show_advanced_multipart_setup();
                if ( CurrentMachine.monitor_flags & MA_MONITORING_ON || is_remote_monitor() )
                    poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentMachine.name.text() );
                EndDialog( hWnd, 0 );

            case IDCANCEL:
                MultipartLbClass.cleanup();
                EndDialog( hWnd, 0 );
                return TRUE;

            case MULTIPART_RUNLIST_LB:
                if ( cmd == LBN_SELCHANGE )
                    {
                    new_index = lb.selected_index();
                    if ( GetKeyState(VK_SHIFT) < 0 )
                        {
                        new_name = MultipartLbClass[new_index].part_name;
                        current_name = MultipartLbClass[current_index].part_name;
                        MultipartLbClass[new_index].part_name = current_name;
                        MultipartLbClass[current_index].part_name = new_name;
                        MultipartLbClass[current_index].lb_line( s );
                        lb.setcursel( current_index );
                        lb.replace( s.text() );

                        MultipartLbClass[new_index].lb_line( s );
                        lb.setcursel( new_index );
                        lb.replace( s.text() );
                        }
                    current_index = new_index;
                    }
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         GET_SHOT_EVENT_WIRE                          *
***********************************************************************/
static void get_shot_event_wire( unsigned & dest, int button_id, int alarm_type )
{
STRING_CLASS s;

if ( choose_wire( GetDlgItem(ShotEventsDialog, button_id), ShotEventsDialog, alarm_type, (int) NO_INDEX, &CurrentMachine, &CurrentPart, &CurrentParam, &CurrentLimitSwitch) )
    {
    s.get_text( ShotEventsDialog, button_id );

    dest = s.uint_value();
    if ( dest > HIGH_FT2_OUTPUT_WIRE )
        dest = NO_WIRE;
    }
}

/**********************************************************************
*                      SHOT_EVENTS_SETUP_STRINGS                      *
**********************************************************************/
static void shot_events_setup_strings()
{
WINDOW_CLASS w;

static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { THIRTY_SECOND_TIMEOUT_STATIC, TEXT( "THIRTY_SECOND_TIMEOUT" ) },
    { ST2_PROGRAM_ABORT_STATIC,     TEXT( "ST2_PROGRAM_ABORT" ) },
    { SHOT_COMPLETE_STATIC,         TEXT( "SHOT_COMPLETE" ) },
    { SHOT_SAVE_COMPLETE_STATIC,    TEXT( "SHOT_SAVE_COMPLETE" ) },
    { WARNING_SHOT_STATIC,          TEXT( "WARNING_SHOT" ) },
    { ALARM_SHOT_STATIC,            TEXT( "ALARM_SHOT" ) },
    { GOOD_SHOT_STATIC,             TEXT( "GOOD_SHOT" ) },
    { GOOD_SHOT_NO_WARNINGS_XBOX,   TEXT( "GOOD_SHOT_NO_WARNINGS" ) },
    { SHOT_COMPLETE_TIMEOUT_STATIC, TEXT( "ALARM_TIMEOUT" ) },
    { IDOK,                         TEXT( "RETURN_TO_PARAMETERS" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( ShotEventsDialog, rlist, nr );

w = ShotEventsDialog;
w.set_title( StringTable.find(TEXT("SHOT_EVENT_OUTPUTS")) );
w.refresh();
}

/***********************************************************************
*                       INIT_SHOT_EVENTS_DIALOG                        *
***********************************************************************/
static void init_shot_events_dialog()
{
STRING_CLASS s;

/*
----------------------
Shot events wire setup
---------------------- */
s = CurrentPart.thirty_second_timeout_wire;
s.set_text( ShotEventsDialog, THIRTY_SECOND_TIMEOUT_WIRE_PB );

s = CurrentPart.st2_program_abort_wire;
s.set_text( ShotEventsDialog, ST2_PROGRAM_ABORT_WIRE_PB );

s = CurrentPart.shot_complete_wire;
s.set_text( ShotEventsDialog, SHOT_COMPLETE_WIRE_PB );

s = CurrentPart.shot_complete_timeout_seconds;
s.set_text( ShotEventsDialog, SHOT_COMPLETE_TIMEOUT_EB );

s = CurrentPart.shot_save_complete_wire;
s.set_text( ShotEventsDialog, SHOT_SAVE_COMPLETE_WIRE_PB );

s = CurrentPart.warning_shot_wire;
s.set_text( ShotEventsDialog, WARNING_SHOT_WIRE_PB );

s = CurrentPart.alarm_shot_wire;
s.set_text( ShotEventsDialog, ALARM_SHOT_WIRE_PB );

s = CurrentPart.good_shot_wire;
s.set_text( ShotEventsDialog, GOOD_SHOT_WIRE_PB );

set_checkbox( ShotEventsDialog, GOOD_SHOT_NO_WARNINGS_XBOX, CurrentPart.good_shot_requires_no_warnings );
}

/***********************************************************************
*                       SHOT_EVENTS_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK shot_events_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ShotEventsDialog = hWnd;
        shot_events_setup_strings();
        init_shot_events_dialog();
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                ShotEventsDialog = 0;
                EndDialog( hWnd, 0 );
                return TRUE;

            case THIRTY_SECOND_TIMEOUT_WIRE_PB:
                get_shot_event_wire( CurrentPart.thirty_second_timeout_wire, id, (int) THIRTY_SECOND_TIMEOUT_TYPE );
                break;
            case ST2_PROGRAM_ABORT_WIRE_PB:
                get_shot_event_wire( CurrentPart.st2_program_abort_wire, id, (int) ST2_PROGRAM_ABORT_TYPE );
                break;
            case SHOT_COMPLETE_WIRE_PB:
                get_shot_event_wire( CurrentPart.shot_complete_wire, id, (int) SHOT_COMPLETE_TYPE );
                break;
            case SHOT_COMPLETE_TIMEOUT_EB:
                CurrentPart.shot_complete_timeout_seconds = get_int_from_ebox( hWnd, SHOT_COMPLETE_TIMEOUT_EB );
                break;
            case SHOT_SAVE_COMPLETE_WIRE_PB:
                get_shot_event_wire( CurrentPart.shot_save_complete_wire, id, (int) SHOT_SAVE_COMPLETE_TYPE );
                break;
            case WARNING_SHOT_WIRE_PB:
                get_shot_event_wire( CurrentPart.warning_shot_wire, id, (int) WARNING_SHOT_TYPE );
                break;
            case ALARM_SHOT_WIRE_PB:
                get_shot_event_wire( CurrentPart.alarm_shot_wire, id, (int) ALARM_SHOT_TYPE );
                break;
            case GOOD_SHOT_WIRE_PB:
                get_shot_event_wire( CurrentPart.good_shot_wire, id, (int) GOOD_SHOT_TYPE );
                break;
            case GOOD_SHOT_NO_WARNINGS_XBOX:
                CurrentPart.good_shot_requires_no_warnings = is_checked( hWnd, GOOD_SHOT_NO_WARNINGS_XBOX );
                break;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            MOVE_LB_LINE                              *
* This changes the sort order of the current line, moving it up or     *
* down in the listbox.                                                 *
***********************************************************************/
static void move_lb_line( int key )
{
int x;

x = ParamLimitLb.current_index();
if ( x == NO_INDEX )
    return;

if ( key == VK_DOWN )
    {
    if ( x < (ParamLimitLb.count()-1) )
        {
        CurrentSort.move( x+1, x );
        refresh_current_parameter_limits();
        ParamLimitLb.setcursel(x+1);
        refresh_current_parameter_limits();
        }
    }
else if ( key == VK_UP )
    {
    if ( x > 0 )
        {
        CurrentSort.move( x-1, x );
        refresh_current_parameter_limits();
        ParamLimitLb.setcursel(x-1);
        refresh_current_parameter_limits();
        }
    }
}

/***********************************************************************
*                          GET_ALARM_TIMEOUT                           *
***********************************************************************/
static void get_alarm_timeout( HWND eb )
{
STRING_CLASS s;
s.get_text( eb );
CurrentPart.set_alarm_timeout( (int32) s.int_value() );
}

/**********************************************************************
*                      UPDATE_PARAM_SETUP_STRINGS                     *
**********************************************************************/
static void update_parameter_limits_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { ADD_PARAMETER_BUTTON,     TEXT( "ADD_PARAMETER" )    },
    { REMOVE_PARAMETER_BUTTON,  TEXT( "REMOVE_PARAMETER" ) },
    { WARN_LIMITS_STATIC,       TEXT( "WARNING_LIMITS" )   },
    { ALARM_LIMITS_STATIC,      TEXT( "ALARM_LIMITS" )     },
    { UNITS_STATIC,             TEXT( "UNITS_STRING" )     },
    { PARAM_DESC_STATIC,        TEXT( "PARAM_DESC" )       },
    { LOW_WARNING_STATIC,       TEXT( "LOW_STRING" )       },
    { LOW_WARNING_WIRE_STATIC,  TEXT( "WIRE_STRING" )      },
    { HIGH_WARNING_STATIC,      TEXT( "HIGH_STRING" )      },
    { HIGH_WARNING_WIRE_STATIC, TEXT( "WIRE_STRING" )      },
    { LOW_ALARM_STATIC,         TEXT( "LOW_STRING" )       },
    { LOW_ALARM_WIRE_STATIC,    TEXT( "WIRE_STRING" )      },
    { HIGH_ALARM_STATIC,        TEXT( "HIGH_STRING" )      },
    { HIGH_ALARM_WIRE_STATIC,   TEXT( "WIRE_STRING" )      },
    { SHOT_EVENTS_PB,           TEXT( "SHOT_EVENTS" )      },
    { ALARM_TIMEOUT_STATIC,     TEXT( "ALARM_TIMEOUT" )    },
    { TARGET_STATIC,            TEXT( "TARGET_STRING" )    }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, PARAMETER_LIMIT_LISTBOX );
}

/***********************************************************************
*                         PARAMETER_LIMITS_PROC                        *
***********************************************************************/
BOOL CALLBACK parameter_limits_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     cmd;
int     id;
LRESULT x;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ParamLimitLb.init( hWnd, PARAMETER_LIMIT_LISTBOX );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        update_parameter_limits_setup_strings( hWnd );
        set_parameter_listbox_tabs( hWnd );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, PARAMETER_LIMITS_OVERVIEW_HELP );
        return TRUE;

    case WM_VKEYTOITEM:     /* Message from PARAMETER_LIMIT_LISTBOX */
        if ( id == VK_DOWN || id == VK_UP )
            {
            if ( GetKeyState(VK_SHIFT) < 0 )
                {
                move_lb_line( id );
                return -2;
                }
            }
        return -1;

    case WM_COMMAND:
        switch ( id )
            {
            case PARAMETER_LIMIT_LISTBOX:
                if ( cmd == LBN_DBLCLK )
                    {
                    if ( !IsWindowVisible(ParamDialogWindow) && (CurrentPasswordLevel >= WindowInfo[PARAMETER_LIMITS_TAB].password_level) )
                        {
                        ShowWindow( ParamDialogWindow, SW_SHOW );
                        }
                    }

                if ( cmd == LBN_DBLCLK || cmd == LBN_SELCHANGE )
                    {
                    x = ParamLimitLb.current_index();
                    x = CurrentSort.parameter_number( x );
                    load_new_parameter( (int) x );
                    /*
                    -------------------------------------
                    Set the focus back to me just in case
                    ------------------------------------- */
                    SetFocus( ParamLimitLb.handle() );
                    }

                return TRUE;

            case ALARM_TIMEOUT_EB:
                if ( cmd == EN_CHANGE )
                    get_alarm_timeout( (HWND) lParam );
                return TRUE;

            case SHOT_EVENTS_PB:
                DialogBox(
                    MainInstance,
                    TEXT("SHOT_EVENTS_DIALOG"),
                    MainWindow,
                    (DLGPROC) shot_events_dialog_proc
                    );

                return TRUE;

            case ADD_PARAMETER_BUTTON:
                if ( CurrentPasswordLevel < WindowInfo[PARAMETER_LIMITS_TAB].password_level )
                    {
                    stringtable_message_box( TEXT("LOW_PASSWORD"), TEXT("CANNOT_COMPLY_STRING") );
                    return TRUE;
                    }
                else if ( !NewParameterDialogWindow )
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("NEW_PARAMETER_DIALOG"),
                        MainWindow,
                        (DLGPROC) new_parameter_dialog_proc
                        );
                    }
                else
                    {
                    SetFocus( NewParameterDialogWindow );
                    }
                return TRUE;

            case REMOVE_PARAMETER_BUTTON:
                if ( CurrentPasswordLevel < WindowInfo[PARAMETER_LIMITS_TAB].password_level )
                    {
                    stringtable_message_box( TEXT("LOW_PASSWORD"), TEXT("CANNOT_COMPLY_STRING") );
                    }
                else
                    {
                    remove_current_parameter();
                    }
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          EDIT_ANALOG_SENSORS                         *
***********************************************************************/
static void edit_analog_sensors()
{
static TCHAR taskname[] = TEXT( "esensor.exe" );
NAME_CLASS s;

s.get_exe_dir_file_name( taskname );
execute( s.text() );
}

/***********************************************************************
*                        LOAD_EXTENDED_CHANNELS                        *
* The extended channels are 9-16, which translate to indexes of        *
* Nof_Editable_Channels to MAX_FT2_CHANNELS-1 (8-15).                  *
* This uses the same listbox ids as the FASTRAK_CHANNELS screen so     *
* I have to use a separate index (c) for the channels.                 *
***********************************************************************/
static void load_extended_channels()
{
ANALOG_SENSOR a;
int i;
int c;
TCHAR * cp;
LISTBOX_CLASS lb;

if ( !ExtendedChannelsDialog )
    return;

for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    c = Nof_Editable_Channels + i;
    a.find( CurrentPart.analog_sensor[c] );
    cp = a.desc();
    if ( cp )
        {
        lb.init( ExtendedChannelsDialog, FT_CHANNEL_1_LISTBOX+i );
        lb.setcursel( cp );
        }
    }
}

/***********************************************************************
*                        SAVE_EXTENDED_CHANNELS                        *
***********************************************************************/
static void save_extended_channels()
{
ANALOG_SENSOR a;
int i;
int c;
int x;
LISTBOX_CLASS lb;

if ( !ExtendedChannelsDialog )
    return;

for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    c = Nof_Editable_Channels + i;

    lb.init( ExtendedChannelsDialog, FT_CHANNEL_1_LISTBOX+i );
    x = a.index( lb.selected_text() );
    if ( x == NO_INDEX )
        x = DEFAULT_ANALOG_SENSOR;

    CurrentPart.analog_sensor[c] = x;
    }
}

/**********************************************************************
*                UPDATE_EXTENDED_CHANNELS_STRING_SETTINGS             *
**********************************************************************/
static void update_extended_channels_string_settings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { FASTRAK_CHANNEL_STATIC, TEXT( "EXTENDED_CHANNEL" ) },
    { ANALOG_SENSOR_STATIC,   TEXT( "ANALOG_SENSOR_STRING" ) },
    { IDOK,                   TEXT( "OK_STRING" ) },
    { IDCANCEL,               TEXT( "CANCEL" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);
WINDOW_CLASS w;

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, FT_CHANNEL_1_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_2_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_3_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_4_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_5_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_6_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_7_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_8_LISTBOX );

w = hWnd;
w.set_title( StringTable.find(TEXT("EXTENDED_CHANNELS_CAPTION")) );
w.refresh();

}

/***********************************************************************
*                       EXTENDED_CHANNELS_PROC                         *
***********************************************************************/
BOOL CALLBACK extended_channels_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ExtendedChannelsDialog = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

   case WM_DBINIT:
        update_extended_channels_string_settings( hWnd );
        fill_analog_list_boxes( hWnd, FT_CHANNEL_1_LISTBOX, MAX_EDITABLE_FTII_CHANNELS );
        load_extended_channels();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save_extended_channels();
            case IDCANCEL:
                ExtendedChannelsDialog = 0;
                EndDialog( hWnd, 0 );
                break;
            }

        return TRUE;

    }

return FALSE;
}

/**********************************************************************
*               UPDATE_FASTRAK_CHANNELS_STRING_SETTINGS               *
**********************************************************************/
static void update_fastrak_channels_string_settings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { EDIT_ANALOG_SENSORS_BUTTON, TEXT( "ANALOG_SENSOR_SETUPS" ) },
    { HEAD_PRES_STATIC,           TEXT( "HEAD_PRESSURE_LABEL_STRING" ) },
    { ROD_PRES_STATIC,            TEXT( "ROD_STRING" ) },
    { FASTRAK_CHANNEL_STATIC,     TEXT( "CHANNEL_STRING" ) },
    { ANALOG_SENSOR_STATIC,       TEXT( "ANALOG_SENSOR_STRING" ) },
    { EXTENDED_CHANNELS_PB,       TEXT( "EXTENDED_CHANNELS" ) },
    { CHANNEL_NAMES_STATIC,       TEXT( "CHANNEL_NAMES" ) },
    { CHANNEL_NAMES_HELP_STATIC,  TEXT( "CHANNEL_NAMES_HELP" ) },
    { NONE_STATIC,                TEXT( "NONE_STRING" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, FT_CHANNEL_1_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_2_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_3_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_4_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_5_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_6_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_7_LISTBOX );
set_statics_font( hWnd, FT_CHANNEL_8_LISTBOX );
set_statics_font( hWnd, CHAN_1_AXIS_LABEL_EBOX );
set_statics_font( hWnd, CHAN_2_AXIS_LABEL_EBOX );
set_statics_font( hWnd, CHAN_3_AXIS_LABEL_EBOX );
set_statics_font( hWnd, CHAN_4_AXIS_LABEL_EBOX );
set_statics_font( hWnd, CHAN_5_AXIS_LABEL_EBOX );
set_statics_font( hWnd, CHAN_6_AXIS_LABEL_EBOX );
set_statics_font( hWnd, CHAN_7_AXIS_LABEL_EBOX );
set_statics_font( hWnd, CHAN_8_AXIS_LABEL_EBOX );
}

/***********************************************************************
*                         FASTRAK_CHANNELS_PROC                        *
***********************************************************************/
BOOL CALLBACK fastrak_channels_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

   case WM_DBINIT:
        update_fastrak_channels_string_settings( hWnd );
        fill_analog_list_boxes();
        return TRUE;


    case WM_COMMAND:
        switch ( id )
            {
            case EDIT_ANALOG_SENSORS_BUTTON:
                edit_analog_sensors();
                return TRUE;

            case EXTENDED_CHANNELS_PB:
               DialogBox(
                   MainInstance,
                   TEXT("EXTENDED_CHANNELS"),
                   hWnd,
                  (DLGPROC) extended_channels_proc );
                return TRUE;
            }
        break;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, CHANNELS_SETUP_OVERVIEW_HELP );
        return TRUE;
    }

return FALSE;
}

/**********************************************************************
*                  UPDATE_USER_DEFINED_SETUP_STRINGS                  *
**********************************************************************/
static void update_user_defined_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { VEL_POS_STATIC,       TEXT( "MEASURE_VELOCITY" )    },
    { AVG_VEL_RANGE_STATIC, TEXT( "MEASURE_AVERAGE_VELOCITY" )    },
    { LS_POS_STATIC,        TEXT( "SET_LIMIT_SWITCHES" )    },
    { POSITION_STATIC,      TEXT( "POSITION_STRING" )    },
    { POSITION2_STATIC,     TEXT( "POSITION_STRING" )    },
    { START_POS_STATIC,     TEXT( "STARTING_POSITION" )    },
    { END_POS_STATIC,       TEXT( "ENDING_POSITION" )    },
    { LS_WIRE_STATIC,       TEXT( "WIRE_STRING" )    },
    { LIMIT_SWITCH_STATIC,  TEXT( "LIMIT_SWITCH" )    }

    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, LS1_DESC_EB );
set_statics_font( hWnd, LS2_DESC_EB );
set_statics_font( hWnd, LS3_DESC_EB );
set_statics_font( hWnd, LS4_DESC_EB );
set_statics_font( hWnd, LS5_DESC_EB );
set_statics_font( hWnd, LS6_DESC_EB );
}

/***********************************************************************
*                            USER_DEFINED_PROC                         *
************************************************************************/
BOOL CALLBACK user_defined_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int i;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_user_defined_setup_strings( hWnd );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, USER_DEF_POS_OVERVIEW_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( RestrictedPartEntry.is_enabled && (RestrictedPartEntry.password_level==CurrentPasswordLevel) )
                    save_changes();

                if ( RestrictedPartEntry.exit_when_done )
                    {
                    exit_restricted_part_entry_screen();
                    SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                    }
                return TRUE;

            case LS1_WIRE_PB:
            case LS2_WIRE_PB:
            case LS3_WIRE_PB:
            case LS4_WIRE_PB:
            case LS5_WIRE_PB:
            case LS6_WIRE_PB:
                i = id - LS1_WIRE_PB;
                choose_wire( GetDlgItem(hWnd, id), hWnd, LIMIT_SWITCH_OUTPUT_TYPE, i, &CurrentMachine, &CurrentPart, &CurrentParam, &CurrentLimitSwitch );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         FILL_COLOR_LIST_BOX                          *
***********************************************************************/
static void fill_color_list_box()
{
COLORS_CLASS c;
short i;
short n;

n = c.count();
for ( i=0; i<n; i++ )
    {
    SendDlgItemMessage( WindowInfo[PROFILE_MARKS_TAB].dialogwindow, MARKS_COLOR_LISTBOX_ID, LB_ADDSTRING, 0, (LPARAM) ((LPSTR) c.name(i) ) );
    }
}

/***********************************************************************
*                            SET_MARK_LINE                             *
***********************************************************************/
static void set_mark_line( int id )
{
BITSETYPE b;

b = (VLINE_MARK | BLINE_MARK | NOLINE_MARK );
CurrentMarks.array[CurrentSelectedMark].symbol &= ~b;

b = 0;
switch ( id )
    {
    case MARKS_NO_LINE_BUTTON:
        b = NOLINE_MARK;
        break;

    case MARKS_VLINE_BUTTON:
        b = VLINE_MARK;
        break;

    case MARKS_BASE_LINE_BUTTON:
        b = BLINE_MARK;
        break;
    }

CurrentMarks.array[CurrentSelectedMark].symbol |= b;
}

/***********************************************************************
*                          ADD_MARK_TO_LISTBOX                         *
***********************************************************************/
static void add_mark_to_listbox( TCHAR type, short number, TCHAR * s )
{
SendDlgItemMessage( AddMarksDialogWindow, ADD_MARKS_LISTBOX_ID, LB_ADDSTRING, 0, (LPARAM)(LPSTR) s );
AddMarksList[NofAddMarks].parameter_type = type;
AddMarksList[NofAddMarks].parameter_number = number;
NofAddMarks++;
}

/***********************************************************************
*                        fill_add_marks_listbox                        *
***********************************************************************/
static BOOLEAN fill_add_marks_listbox()
{
short i;
PARAMETER_ENTRY * p;
p = CurrentParam.parameter;

SendDlgItemMessage( AddMarksDialogWindow, ADD_MARKS_LISTBOX_ID, LB_RESETCONTENT, 0, 0L );
NofAddMarks = 0;

for ( i=0; i<CurrentParam.count(); i++ )
    {
    if ( p->vartype & (POSITION_VAR | TIME_VAR) )
        {
        if ( CurrentMarks.index(PARAMETER_MARK, i+1) == NO_INDEX )
            add_mark_to_listbox( PARAMETER_MARK, i+1, p->name );
        }
    p++;
    }

for ( i=1; i<=USER_AVG_VEL_COUNT; i++ )
    {
    if ( CurrentMarks.index(START_POS_MARK, i) == NO_INDEX )
        add_mark_to_listbox( START_POS_MARK, i, mark_name(START_POS_MARK, i) );

    if ( CurrentMarks.index(END_POS_MARK, i) == NO_INDEX )
        add_mark_to_listbox( END_POS_MARK, i, mark_name(END_POS_MARK, i) );
    }

for ( i=1; i<=USER_VEL_COUNT; i++ )
    if ( CurrentMarks.index(VEL_POS_MARK, i) == NO_INDEX )
        add_mark_to_listbox( VEL_POS_MARK, i, mark_name(VEL_POS_MARK, i) );

return TRUE;
}

/***********************************************************************
*                            ADD_NEW_MARKS                             *
***********************************************************************/
static void add_new_marks()
{
short i;

if ( !AddMarksDialogWindow )
    return;

for ( i=0; i<NofAddMarks; i++ )
    {
    if ( SendDlgItemMessage( AddMarksDialogWindow, ADD_MARKS_LISTBOX_ID, LB_GETSEL, i, 0L) )
        CurrentMarks.add( AddMarksList[i].parameter_type, AddMarksList[i].parameter_number );
    }
}

/**********************************************************************
*                   UPDATE_ADD_MARKS_STRING_SETTINGS                  *
**********************************************************************/
static void update_add_marks_string_settings()
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { IDOK,                TEXT( "OK_STRING" ) },
    { MARKS_TO_ADD_STATIC, TEXT( "MARKS_TO_ADD" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( AddMarksDialogWindow, rlist, nr );

set_statics_font( AddMarksDialogWindow, ADD_MARKS_LISTBOX_ID );
}

/***********************************************************************
*                        ADD_MARKS_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK add_marks_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        AddMarksDialogWindow = hWnd;
        update_add_marks_string_settings();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_add_marks_listbox();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                add_new_marks();
                show_marks();

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                break;
            }

        return TRUE;

    case WM_DESTROY:
        AddMarksDialogWindow = 0;
        break;
    }

return FALSE;
}

/***********************************************************************
*                           MARKS_CONTEXT_HELP                         *
***********************************************************************/
static void marks_context_help( HWND cw, LPARAM lParam )
{
const int32 NOF_CONTROLS = 1;
const int32 NOF_STATIC_CONTROLS = 5;

static HELP_ENTRY control_help[] = {
    { MARKS_ADD_BUTTON, ADD_MARK_HELP }
    };

static HELP_ENTRY static_help[] = {
    { MARK_SOURCE_DESCRIPTION_STATIC, MARK_SOURCE_DESCRIPTION_HELP },
    { MARK_COLOR_CHOICE_STATIC,       MARK_COLOR_CHOICE_HELP       },
    { MARK_LINE_TYPE_STATIC,          MARK_LINE_TYPE_HELP          },
    { MARK_LABEL_STATIC,              MARK_LABEL_HELP              },
    { MARK_OFFSET_STATIC,             MARK_OFFSET_HELP             }
    };

int32 i;
POINT p;
HWND  dw;
HWND  w;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

dw = WindowInfo[PROFILE_MARKS_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    w = GetDlgItem( dw, static_help[i].id );
    if ( GetWindowRect(w, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            gethelp( HELP_CONTEXTPOPUP, static_help[i].context );
            return;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(dw, control_help[i].id) == cw )
        {
        gethelp( HELP_CONTEXTPOPUP, control_help[i].context );
        return;
        }
    }

get_overview_help();
}



/**********************************************************************
*                 UPDATE_PROFILE_MARKS_STRING_SETTINGS               *
**********************************************************************/
static void update_profile_marks_string_settings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { MARKS_NO_LINE_BUTTON,           TEXT( "NONE_STRING" ) },
    { MARKS_VLINE_BUTTON,             TEXT( "VERTICAL" ) },
    { MARKS_BASE_LINE_BUTTON,         TEXT( "BASE" ) },
    { MARKS_ADD_BUTTON,               TEXT( "ADD_PARAMETER" ) },
    { MARKS_REMOVE_BUTTON,            TEXT( "REMOVE_PARAMETER" ) },
    { MARK_SOURCE_DESCRIPTION_STATIC, TEXT( "MARK_SOURCE_DESC" ) },
    { MARK_COLOR_CHOICE_STATIC,       TEXT( "COLOR" ) },
    { MARK_LINE_TYPE_STATIC,          TEXT( "LINE_TYPE" ) },
    { MARK_LABEL_STATIC,              TEXT( "LABEL" ) },
    { MARK_OFFSET_STATIC,             TEXT( "OFFSET" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, MARKS_LISTBOX_ID );
set_statics_font( hWnd, MARKS_COLOR_LISTBOX_ID );
set_statics_font( hWnd, MARKS_LABEL_ID );
}

/***********************************************************************
*                           PROFILE_MARKS_PROC                         *
***********************************************************************/
BOOL CALLBACK profile_marks_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;
LRESULT x;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_CONTEXTMENU:
        marks_context_help( (HWND) wParam, lParam );
        return TRUE;

    case WM_HELP:
        gethelp( HELP_CONTEXT, MARKS_SETUP_OVERVIEW_HELP );
        return TRUE;

    case WM_DBINIT:
        update_profile_marks_string_settings( hWnd );
        fill_color_list_box();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case MARKS_LISTBOX_ID:
                if ( cmd == LBN_SELCHANGE )
                    {
                    x = SendDlgItemMessage( hWnd, MARKS_LISTBOX_ID, LB_GETCURSEL, 0, 0L);
                    if ( x != LB_ERR )
                        {
                        CurrentSelectedMark = short( x );
                        show_current_mark();
                        }
                    }
                return TRUE;

            case MARKS_COLOR_LISTBOX_ID:
                if ( cmd == LBN_SELCHANGE )
                    {
                    x = SendDlgItemMessage( hWnd, id, LB_GETCURSEL, 0, 0L);
                    if ( x != LB_ERR )
                        CurrentMarks.array[CurrentSelectedMark].color = short( x );
                    }
                return TRUE;

            case MARKS_NO_LINE_BUTTON:
            case MARKS_VLINE_BUTTON:
            case MARKS_BASE_LINE_BUTTON:
                set_mark_line( id );
                return TRUE;

            case MARKS_LABEL_ID:
                GetDlgItemText( hWnd, id, CurrentMarks.array[CurrentSelectedMark].label, MARK_LABEL_LEN+1 );
                return TRUE;

            case MARKS_OFFSET_EBOX:
                CurrentMarks.array[CurrentSelectedMark].offset.get_text( hWnd, id );
                return TRUE;

            case MARKS_ADD_BUTTON:
                if ( !AddMarksDialogWindow )
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("ADD_MARKS"),
                        MainDialogWindow,
                       (DLGPROC) add_marks_dialog_proc );
                    }
                return TRUE;

            case MARKS_REMOVE_BUTTON:
                x = SendDlgItemMessage( hWnd, MARKS_LISTBOX_ID, LB_GETCURSEL, 0, 0L);
                if ( x != LB_ERR )
                    {
                    CurrentMarks.remove( short(x) );
                    show_marks();
                    }
                return TRUE;
            }

        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    SHOW_ADVANCED_SETUP_CONTEXT_HELP                  *
***********************************************************************/
static void show_advanced_setup_context_help( LPARAM lParam )
{
const int32 NOF_STATIC_CONTROLS = 9;

static HELP_ENTRY staticbox[] = {
    { PLUNGER_DIAMETER_STATIC,   PLUNGER_DIAMETER_HELP  },
    { P1_TO_P3_STATIC,           P1_TO_P3_HELP          },
    { P2_TO_P3_STATIC,           P2_TO_P3_HELP          },
    { MIN_CSFS_POS_STATIC,       MIN_CSFS_POS_HELP      },
    { MIN_CSFS_VEL_STATIC,       MIN_CSFS_VEL_HELP      },
    { VEL_FOR_RISE_STATIC,       VEL_FOR_RISE_HELP      },
    { TIME_FOR_INTENS_STATIC,    TIME_FOR_INTENS_HELP   },
    { PRES_FOR_RESPONSE_STATIC,  PRES_FOR_RESPONSE_HELP },
    { BISCUIT_TIME_DELAY_STATIC, ADV_PART_OVERVIEW_HELP },
    { SN_TEMPLATE_STATIC,        SN_TEMPLATE_HELP       },
    { SN_EXAMPLE_STATIC,         SN_EXAMPLE_HELP        }
    };

int32 i;
POINT p;
HWND  bw;
HWND  sw;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

bw = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    sw = GetDlgItem( bw, staticbox[i].id );
    if ( GetWindowRect(sw, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            if ( staticbox[i].context == ADV_PART_OVERVIEW_HELP )
                break;

            gethelp( HELP_CONTEXTPOPUP, staticbox[i].context );
            return;
            }
        }
    }

gethelp( HELP_CONTEXT, ADV_PART_OVERVIEW_HELP );
}

/**********************************************************************
*                   UPDATE_ADVANCED_SETUP_STRINGS                     *
**********************************************************************/
static void update_advanced_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { PLUNGER_DIAMETER_STATIC,           TEXT("PLUNGER_DIAMETER")        },
    { P1_TO_P3_STATIC,                   TEXT("P1_TO_P3")                },
    { P2_TO_P3_STATIC,                   TEXT("P2_TO_P3")                },
    { MIN_CSFS_POS_STATIC,               TEXT("MIN_CSFS_POS")            },
    { MIN_CSFS_VEL_STATIC,               TEXT("MIN_CSFS_VEL")            },
    { VEL_FOR_RISE_STATIC,               TEXT("VELOCITY_FOR_RISE")       },
    { TIME_FOR_INTENS_STATIC,            TEXT("TIME_FOR_INTENS")         },
    { PRES_FOR_RESPONSE_STATIC,          TEXT("PRES_FOR_RESPONSE")       },
    { BISCUIT_TIME_DELAY_STATIC,         TEXT("BISCUIT_TIME_DELAY")      },
    { END_OF_FS_VELOCITY_STATIC,         TEXT("EOF_FS_VEL")              },
    { SHOT_NAME_STATIC,                  TEXT("SHOT_NAME")               },
    { SN_TEMPLATE_STATIC,                TEXT("SHOT_NAME_TEMPLATE")      },
    { SN_EXAMPLE_STATIC,                 TEXT("SHOT_NAME_EXAMPLE")       },
    { SN_GB_STATIC,                      TEXT("SHOT_NAME")               },
    { SNF_NAME_STATIC,                   TEXT("FILE_NAME")               },
    { SNF_COMMAND_STATIC,                TEXT("COMMAND_NAME")            },
    { SHOT_COUNT_RESET_STATIC,           TEXT("SHOT_COUNT_RESET")        },
    { SN_RESET_GB_STATIC,                TEXT("SHOT_COUNT_RESET")        },
    { SNF_ENABLE_XB,                     TEXT("READ_FROM_EXTERNAL_FILE") },
    { EXTERNAL_PARAMETER_FILE_TB,        TEXT("EXTERNAL_PARAMETER_FILE") },
    { EXTERNAL_PARAMETER_FILE_NAME_EB,   TEXT("EXTERNAL_PARAMETER_FILE") },
    { EXTERNAL_PARAMETER_FILE_ENABLE_XB, TEXT("ENABLE_STRING")           },
    { EXTERNAL_PARAMETER_FILE_IS_EOS_XB, TEXT("AVAILABLE_AT_EOS")        },
    { IMPACT_PARAM_FILE_TB,              TEXT("IMPACT_PARAM_FILE")       },
    { IMPACT_PARAM_FILE_ENABLE_XB,       TEXT("ENABLE_STRING")           },
    { MULTIPART_RUNLIST_STATIC,          TEXT("MULTIPART_RUNLIST")       },
    { MULTIPART_RUNLIST_WIRES_STATIC,    TEXT("WIRES_USED")              },
    { MULTIPART_RUNLIST_PSN_STATIC,      TEXT("PART_SELECTION_NUMBER")   },
    { MULTIPART_RUNLIST_SETUP_PB,        TEXT("SETUP_SHEET_TAB")         },
    { WARMUP_GROUP_STATIC,               TEXT("WARMUP_SHOT")             },
    { WARMUP_ENABLE_XBOX,                TEXT("ENABLE_STRING")           },
    { WARMUP_IF_STATIC,                  TEXT("WARMUP_IF_STRING")        },
    { WARMUP_IS_STATIC,                  TEXT("IS_STRING")               },
    { WARMUP_RUN_IF_ON_XBOX,             TEXT("OFF_STRING")              },
    { RESET_SHOT_COUNT_ENABLE_XB,        TEXT("SHOT_COUNT_RESET_ENABLE") },
    { PURGE_ON_RESET_XB,                 TEXT("SHOT_COUNT_PURGE")        },
    { RESET_TIME_STATIC,                 TEXT("SHOT_COUNT_RESET_TIME")   }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );
}

/***********************************************************************
*                          UPDATE_SN_EXAMPLE                           *
***********************************************************************/
static void update_sn_example( HWND w )
{
PLOOKUP_CLASS   p;
PART_NAME_ENTRY pn;
STRING_CLASS    s;
int             shot_number;

shot_number = p.get_last_shot_number( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
shot_number++;

if ( shot_number < 1 )
    shot_number = 1;

s.get_text( w, SN_TEMPLATE_EDITBOX );

pn.computer = CurrentPart.computer;
pn.machine  = CurrentPart.machine;
pn.part     = CurrentPart.name;

if ( shot_name(s, pn, shot_number) )
    {
    s.set_text( w, SN_EXAMPLE_TB );
    if ( s.contains(SpaceChar) || s.contains(TabChar) )
        {
        s = StringTable.find( TEXT("NO_SPACES_IN_SHOT_NAME") );
        MessageBox( w, s.text(), StringTable.find(TEXT("SHOT_NAME_ERROR")), MB_OK | MB_SYSTEMMODAL );
        }
    }
}

/***********************************************************************
*                            ADVANCED_SETUP_PROC                       *
***********************************************************************/
BOOL CALLBACK advanced_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_advanced_setup_strings( hWnd );
        if ( !ShowEofFS )
            {
            ShowWindow( GetDlgItem(hWnd, END_OF_FS_VELOCITY_EDITBOX),  SW_HIDE );
            ShowWindow( GetDlgItem(hWnd, END_OF_FS_VELOCITY_STATIC),  SW_HIDE );
            }
        set_statics_font( hWnd, SNF_COMMAND_EB );
        set_statics_font( hWnd, SNF_NAME_EB );
        set_statics_font( hWnd, SN_TEMPLATE_EDITBOX );
        set_statics_font( hWnd, SN_EXAMPLE_TB );
        set_statics_font( hWnd, IMPACT_PARAM_FILE_NAME_EB );
        return TRUE;

    case WM_HELP:
        gethelp( HELP_CONTEXT, ADV_PART_OVERVIEW_HELP );
        return TRUE;

    case WM_CONTEXTMENU:
        show_advanced_setup_context_help( lParam );
        return TRUE;

    case WM_COMMAND:
        if ( cmd == EN_CHANGE )
            {
            if ( id == SN_TEMPLATE_EDITBOX )
                {
                update_sn_example( hWnd );
                return TRUE;
                }
            }
        else if ( cmd == BN_CLICKED )
            {
            if ( id == WARMUP_RUN_IF_ON_XBOX )
                {
                set_run_if_on_text( hWnd );
                }
            else if ( id == MULTIPART_RUNLIST_SETUP_PB )
                {
                DialogBox(
                    MainInstance,
                    TEXT("MULTIPART_RUNLIST_SETUP_DIALOG"),
                    MainWindow,
                    (DLGPROC) multipart_runlist_setup_dialog_proc
                    );
                return TRUE;
                }
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                            CREATE_DIALOGS                            *
***********************************************************************/
static void create_dialogs()
{
int i;
int id;

WindowInfo[BASIC_SETUP_TAB].procptr      = basic_setup_proc;
WindowInfo[PARAMETER_LIMITS_TAB].procptr = parameter_limits_proc;
WindowInfo[USER_DEFINED_TAB].procptr     = user_defined_proc;
WindowInfo[PROFILE_MARKS_TAB].procptr    = profile_marks_proc;
WindowInfo[ADVANCED_SETUP_TAB].procptr   = advanced_setup_proc;
WindowInfo[FASTRAK_CHANNELS_TAB].procptr = fastrak_channels_proc;
WindowInfo[SURETRAK_PROFILE_TAB].procptr = suretrak_profile_proc;
WindowInfo[PRESSURE_CONTROL_TAB].procptr = pressure_control_profile_proc;
WindowInfo[SETUP_SHEET_TAB].procptr      = setup_sheet_proc;

id = BASIC_SETUP_RB;
for ( i=0; i<TAB_COUNT; i++ )
    {
    WindowInfo[i].password_level = LOWEST_PASSWORD_LEVEL;
    WindowInfo[i].tab_rb         = GetDlgItem( MainDialogWindow, id );
    WindowInfo[i].dialogwindow   = CreateDialog(
        MainInstance,
        DialogName[i],
        MainDialogWindow,
        WindowInfo[i].procptr );
    id++;
    }
}

/***********************************************************************
*                         REPLACE_DISTANCE_VALUE                       *
***********************************************************************/
static STRING_CLASS & replace_distance_value( TCHAR * sorc, float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
i = round( x, 1.0 );

return replace_value( sorc, i );
}

/***********************************************************************
*                       REPLACE_X1_DISTANCE_VALUE                      *
***********************************************************************/
static STRING_CLASS & replace_x1_distance_value( TCHAR * sorc, float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
x /= 4.0;
i = round( x, 1.0 );

return replace_value( sorc, i );
}

/***********************************************************************
*                         REPLACE_VELOCITY_VALUE                       *
***********************************************************************/
static STRING_CLASS & replace_velocity_value( TCHAR * sorc, float new_value, BOOLEAN need_bit_30 )
{
static TCHAR setbit30[] = TEXT( "|H40000000" );
const int SLEN = sizeof(setbit30)/sizeof(TCHAR);

TCHAR s[MAX_INTEGER_LEN + SLEN + 1];
TCHAR * cp;
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_velocity( x );
x /= 4.0;
i = round( x, 1.0 );

cp = copystring( s, int32toasc((int32) i) );

if ( need_bit_30 )
    copystring( cp, setbit30 );

return replace_value( sorc, s );
}

/***********************************************************************
*                         REPLACE_VELOCITY_VALUE                       *
***********************************************************************/
static STRING_CLASS & replace_velocity_value( TCHAR * sorc, float new_value )
{
return replace_velocity_value( sorc, new_value, (BOOLEAN) FALSE );
}

/***********************************************************************
*                         X4_FROM_DISTANCE_VALUE                       *
***********************************************************************/
static unsigned x4_from_distance_value( double new_value )
{
int    i;

new_value = CurrentPart.x4_from_dist( new_value );
i = (int) round( new_value, 1.0 );
return (unsigned) i;
}

/***********************************************************************
*                         X4_FROM_DISTANCE_VALUE                       *
***********************************************************************/
static unsigned x4_from_distance_value( float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
i = (int) round( x, 1.0 );
return (unsigned) i;
}

/***********************************************************************
*                         X1_FROM_DISTANCE_VALUE                       *
***********************************************************************/
static unsigned x1_from_distance_value( float new_value )
{
double x;
int    i;

x = (double) new_value;
x = CurrentPart.x4_from_dist( x );
x /= 4.0;
i = (int) round( x, 1.0 );
return (unsigned) i;
}

/***********************************************************************
*                              X1_VELOCITY                             *
***********************************************************************/
static unsigned x1_velocity( double x )
{
int    i;

x = CurrentPart.x4_from_velocity( x );
x /= 4.0;
i = round( x, 1.0 );

return (unsigned) i;
}

/***********************************************************************
*                              X1_VELOCITY                             *
***********************************************************************/
static unsigned x1_velocity( float new_value )
{
return x1_velocity( (double) new_value );
}

/***********************************************************************
*                              FTII_PERCENT                            *
***********************************************************************/
static unsigned ftii_percent( double d )
{
unsigned u;

d *= 100.0;
u = (unsigned) round( d, 1.0 );
u |= Bit31;

return u;
}

/***********************************************************************
*                              FTII_PERCENT                            *
***********************************************************************/
static unsigned ftii_percent( float new_value )
{
return ftii_percent( (double) new_value );
}

/***********************************************************************
*                    UPDATE_SURETRAK_STEP_VALUES                       *
***********************************************************************/
static void update_suretrak_step_value( FTII_VAR_ARRAY & va, int vi )
{
int      step_number;
int      vn;
bool     have_low_impact;
double   d;
unsigned u;

SureTrakSetup.rewind();

vn              = ST_ACCEL_1_VN;
have_low_impact = false;

step_number = 0;
while ( true )
    {
    if ( step_number >= MAX_FTII_CONTROL_STEPS )
        break;

    if ( SureTrakSetup.next() )
        {
        if ( !have_low_impact )
            {
            if ( not_float_zero(SureTrakSetup.low_impact_value()) )
                have_low_impact = true;
            }

        /*
        -----
        Accel
        ----- */
        d = (double) SureTrakSetup.accel_value();
        if ( SureTrakSetup.accel_is_percent() )
            u = ftii_percent( d );
        else
            u = x1_from_distance_value( d );

        va[vi++].set( vn, u );
        vn++;

        /*
        --------
        Velocity
        -------- */
        d = (double) SureTrakSetup.vel_value();
        if ( SureTrakSetup.vel_is_percent() )
            u = ftii_percent( d );
        else
            u = x1_velocity( d );

        if ( SureTrakSetup.have_vacuum_wait() )
            u |= Bit30;

        va[vi++].set( vn, u );
        vn++;

        /*
        ------------
        End Position
        ------------ */
        u = x4_from_distance_value( SureTrakSetup.end_pos_value() );

        va[vi++].set( vn, u );
        vn++;

        /*
        -------------
        Input to test
        ------------- */
        u = 0;
        if ( SureTrakSetup.have_vacuum_wait() )
            u = (unsigned) FTII_VACUUM_WAIT_INPUT;
        else if ( SureTrakSetup.have_low_impact_from_input() )
            u = (unsigned) FTII_LOW_IMPACT_INPUT;

        va[vi++].set( vn, u );
        vn++;

        /*
        ----------------------------------------------------------------
        If there a low impact value I have to make another step for this
        ---------------------------------------------------------------- */
        if ( have_low_impact )
            {
            va[vi++].set( vn++, (unsigned) 0 );
            d = (double) SureTrakSetup.low_impact_value();
            u = ftii_percent( d );
            va[vi++].set( vn++, (unsigned) u );
            va[vi++].set( vn++, (unsigned) 0 );
            va[vi++].set( vn++, (unsigned) 0 );
            step_number++;
            }
        }
    else
        {
        va[vi++].set( vn++, (unsigned) 0 );
        va[vi++].set( vn++, (unsigned) 0x7FFF );
        va[vi++].set( vn++, (unsigned) 0 );
        va[vi++].set( vn++, (unsigned) 0 );
        }
    step_number++;
    }
}

/***********************************************************************
*                     PRESSURE_TO_VOLTS_SETPOINT                       *
*             Convert a pressure setpoint to volts * 1000              *
* The setpoint can be a 16 bit positive or negative number which       *
* is then zero extended to 32 bits.                                    *
***********************************************************************/
unsigned pressure_to_volts_setpoint( STRING_CLASS & sorc )
{
ANALOG_SENSOR a;
double max_pressure;
double min_pressure;
double max_volts;
double min_volts;
double v;
unsigned short us;

a.find( PressureControl.sensor_number );
max_pressure = a.high_value();
min_pressure = - max_pressure;
if ( max_pressure <= min_pressure )
    max_pressure = min_pressure + 1.0;
max_volts = a.max_volts();
/*
---------------------------------------------------------------------------------------
The voltage output range does not apply here, I need the actual voltage range,
which I dont have yet. For now (i.e., for Diecast Press), assume the voltage range is
5 volts, as in 0-5 volts = 0-10000 psi).

if ( PressureControl.voltage_output_range == 0 )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == 2 )
    max_volts = 10.0;
-------------------------------------------------------------------------------------- */
min_volts = -max_volts;

v = sorc.real_value();
if ( v < min_pressure )
    v = min_pressure;
else if ( v > max_pressure )
    v = max_pressure;
v -= min_pressure;
v *= (max_volts - min_volts);
v *= 1000.0;
v /= (max_pressure - min_pressure);
v += min_volts*1000.0;

us = (unsigned short) v;

return (unsigned) us;
}
/***********************************************************************
*                     PRESSURE_TO_VOLTS_16BIT                          *
*             Convert a pressure setpoint to volts * 1000              *
* The setpoint can be a 16 bit positive or negative number which       *
* is then zero extended to 32 bits.                                    *
***********************************************************************/
unsigned pressure_to_volts_16bit( STRING_CLASS & sorc, double max_volts )
{
ANALOG_SENSOR a;
double max_pressure;
double min_pressure;
double min_volts;
double v;
union { unsigned u; unsigned short us; };

u = 0;

if ( sorc.contains(PercentChar) )
    {
    max_pressure = 100.0;
    min_pressure = -100.0;
    }
else
    {
    max_pressure = a[PressureControl.sensor_number].high_value.real_value();
    min_pressure = - max_pressure;
    if ( max_pressure <= min_pressure )
        max_pressure = min_pressure + 1.0;
    }

min_volts = -max_volts;

v = sorc.real_value();
if ( v < min_pressure )
    v = min_pressure;
else if ( v > max_pressure )
    v = max_pressure;
v -= min_pressure;
v *= (max_volts - min_volts);
v *= 1000.0;
v /= (max_pressure - min_pressure);
v += min_volts*1000.0;

us = (unsigned short) v;

return u;
}

/***********************************************************************
*                     PRESSURE_TO_VOLTS_16BIT                          *
*             Convert a pressure setpoint to volts * 1000              *
* The setpoint can be a 16 bit positive or negative number which       *
* is then zero extended to 32 bits.                                    *
***********************************************************************/
unsigned pressure_to_volts_16bit( STRING_CLASS & sorc )
{
double max_volts;

/*
-------------------------------------------------------------------------------------
The voltage output range of the sensor does not apply here, I need the actual voltage
range, which is set by the radio buttons under the sensor drop down list box.
------------------------------------------------------------------------------------- */
max_volts = 5.0;

if ( PressureControl.voltage_output_range == PC_2_VOLT_RANGE )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == PC_10_VOLT_RANGE )
    max_volts = 10.0;

return pressure_to_volts_16bit( sorc, max_volts );
}

/***********************************************************************
*                      PERCENT_TO_UNSIGNED_VOLTS                       *
* This converts a string percent to an unsigned volts*1000.            *
* All 32 bits are used, so for +/- 10 volts -.01% = -.001 volts        *
* = -1 = 0xFFFFFFFF.                                                   *
***********************************************************************/
static unsigned percent_to_unsigned_volts( STRING_CLASS & sorc, double max_volts )
{
double d;

d = sorc.real_value();
d *= 100.0;
if ( (10.0 - max_volts) > 1.0 )
    {
    d *= max_volts;
    d /= 10.0;
    }

return (unsigned) round( d, 1.0 );
}

/***********************************************************************
*                      PERCENT_TO_UNSIGNED_VOLTS                       *
*            This uses the selected voltage output range.              *
***********************************************************************/
static unsigned percent_to_unsigned_volts( STRING_CLASS & sorc )
{
double max_volts;

max_volts = 5.0;

if ( PressureControl.voltage_output_range == PC_2_VOLT_RANGE )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == PC_10_VOLT_RANGE )
    max_volts = 10.0;

return percent_to_unsigned_volts( sorc, max_volts );
}

/***********************************************************************
*                           PERCENT_TO_VOLTS                           *
* Converts percent to volts using the current voltage output range     *
***********************************************************************/
double percent_to_volts( STRING_CLASS & sorc )
{
const double max_percent   =  100.0;
const double percent_range =  200.0;
double max_volts;
double volt_range;
double v;

/*
-------------------------------------------------------------------------------------
The voltage output range of the sensor does not apply here, I need the actual voltage
range, which is set by the radio buttons under the sensor drop down list box.
------------------------------------------------------------------------------------- */
max_volts = 5.0;

if ( PressureControl.voltage_output_range == PC_2_VOLT_RANGE )
    max_volts = 2.5;
else if ( PressureControl.voltage_output_range == PC_10_VOLT_RANGE )
    max_volts = 10.0;

volt_range = 2 * max_volts;

v = sorc.real_value();
v += max_percent;
v *= volt_range;
v /= percent_range;
v -= max_volts;

return v;
}

/***********************************************************************
*                     UPDATE_FTII_CONTROL_PROGRAM                      *
* The limit_switch_mask is an array of 6 masks for the limit switches  *
* if this is NOT a suretrak2.                                          *
***********************************************************************/
static void update_ftii_control_program()
{
static int TSL_INDEX                 = 0;
static int MSL_INDEX                 = 1;
static int ESV_INDEX                 = 2;
static int LS1_INDEX                 = 3;
static int LS2_INDEX                 = 4;
static int LS3_INDEX                 = 5;
static int LS4_INDEX                 = 6;
static int LS5_INDEX                 = 7;
static int LS6_INDEX                 = 8;
static int MY_VEL_LOOP_GAIN_INDEX    = 9;
static int MY_FT_STOP_POS_INDEX      = 10;
static int BREAK_VEL_1_INDEX         = 11;
static int BREAK_VEL_2_INDEX         = 12;
static int BREAK_VEL_3_INDEX         = 13;
static int BREAK_GAIN_1_INDEX        = 14;
static int BREAK_GAIN_2_INDEX        = 15;
static int BREAK_GAIN_3_INDEX        = 16;
static int TIME_INTERVAL_MS_INDEX    = 17;
static int NOF_TIME_SAMPLES_INDEX    = 18;
static int BISCUIT_DELAY_MS_INDEX    = 19;
static int ZERO_SPEED_OUTPUT_INDEX   = 20;
static int PC_CONFIG_WORD_INDEX      = 21;
static int PC_PRES_LOOP_GAIN_INDEX   = 22;
static int PC_RETRACT_VOLTS_INDEX    = 23;
static int PC_PARK_VOLTS_INDEX       = 24;
static int PC_RAMP_1_INDEX           = 25;
static int PC_PRES_1_INDEX           = 26;
static int PC_HOLDTIME_1_INDEX       = 27;
static int PC_RAMP_2_INDEX           = 28;
static int PC_PRES_2_INDEX           = 29;
static int PC_HOLDTIME_2_INDEX       = 30;
static int PC_RAMP_3_INDEX           = 31;
static int PC_PRES_3_INDEX           = 32;
static int PC_HOLDTIME_3_INDEX       = 33;
static int PC_INTEGRAL_GAIN_INDEX    = 34;
static int PC_DERIVATIVE_GAIN_INDEX  = 35;
static int PC_RETCTL_DAC_INDEX       = 36; /* Retctl must be last */
static int PC_RETCTL_ARM_PRES_INDEX  = 37;
static int PC_RETCTL_PRES_INDEX      = 38;

static int vars[] = { TSL_VN, MSL_VN, ESV_VN,
 LS1_VN, LS2_VN, LS3_VN, LS4_VN, LS5_VN, LS6_VN,
 VEL_LOOP_GAIN_VN,
 FT_STOP_POS_VN,
 BREAK_VEL_1_VN,  BREAK_VEL_2_VN,  BREAK_VEL_3_VN,
 BREAK_GAIN_1_VN, BREAK_GAIN_2_VN, BREAK_GAIN_3_VN,
 TIME_INTERVAL_MS_VN, NOF_TIME_SAMPLES_VN,
 BISCUIT_DELAY_MS_VN,
 ZERO_SPEED_OUTPUT_MASK_VN,
 PC_CONFIG_WORD_VN,
 PC_PRES_LOOP_GAIN_VN,
 PC_RETRACT_VOLTS_VN,
 PC_PARK_VOLTS_VN,
 PC_RAMP_1_VN,
 PC_PRES_1_VN,
 PC_HOLDTIME_1_VN,
 PC_RAMP_2_VN,
 PC_PRES_2_VN,
 PC_HOLDTIME_2_VN,
 PC_RAMP_3_VN,
 PC_PRES_3_VN,
 PC_HOLDTIME_3_VN,
 PC_INTEGRAL_GAIN_VN,
 PC_DERIVATIVE_GAIN_VN,
 PC_RETCTL_DAC_VN,
 PC_RETCTL_ARM_PRES_VN,
 PC_RETCTL_PRES_VN
 };

const int nof_vars = sizeof(vars)/sizeof(int);

static int normal_gain_vars[]  = { BREAK_GAIN_1_VN, BREAK_GAIN_2_VN,  BREAK_GAIN_3_VN    };
static int cyclone_gain_vars[] = { BREAK_GAIN_1_VN, INTEGRAL_GAIN_VN, DERIVATIVE_GAIN_VN };
static int maskvn[] = { LS1_MASK_VN, LS2_MASK_VN, LS3_MASK_VN, LS4_MASK_VN, LS5_MASK_VN, LS6_MASK_VN };
const  int nof_masks = sizeof(maskvn)/sizeof(int);
const unsigned USE_ONLY_POSITIVE_VOLTAGES_MASK = 0x80;

BOOLEAN        is_cyclone;
int            vi;
int            vn;
int            i;
int            step_index;
int            n;
unsigned       u;
unsigned       v;
double         x;
NAME_CLASS     s;
FTII_VAR_ARRAY va;
BOOLEAN        setpoint_not_zero;

is_cyclone = CurrentMachine.is_cyclone;
n = nof_vars;
i = nof_vars;
if ( CurrentMachine.suretrak_controlled )
    i += ST_LAST_STEP_VN + 1;
i += nof_masks;
va.upsize( i );

vi = 0;

va[vi++].set( TSL_VN, x4_from_distance_value(CurrentPart.total_stroke_length) );
va[vi++].set( MSL_VN, x4_from_distance_value(CurrentPart.min_stroke_length) );
va[vi++].set( ESV_VN, x1_velocity(CurrentPart.eos_velocity) );

/*
--------------
Limit Switches
-------------- */
for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    {
    /*
    --------
    Position
    -------- */
    x = CurrentLimitSwitch.real_pos(i);
    u  = 0x7FFF;
    setpoint_not_zero = not_zero( x );
    if ( setpoint_not_zero )
        u = x4_from_distance_value( x );
    va[vi++].set( vars[LS1_INDEX+i], u );

    /*
    ----
    Mask
    ---- */
    u = 0;
    if ( setpoint_not_zero )
        u = CurrentLimitSwitch.mask( i );
    va[vi++].set( maskvn[i], u );
    }

/*
------------------
Velocity Loop Gain
------------------ */
x = SureTrakSetup.velocity_loop_gain_value();
x = round( x, 1.0 );
u = (unsigned) x;
va[vi++].set( VEL_LOOP_GAIN_VN, u );

/*
----------------------------
Follow Through Stop Position
---------------------------- */
x = SureTrakSetup.ft_stop_pos_value();
u  = 0x7FFF;
if ( not_zero(x) )
    u = x4_from_distance_value( x );
va[vi++].set( FT_STOP_POS_VN, u );

va[vi++].set( BREAK_VEL_1_VN, x1_velocity(SureTrakSetup.gain_break_velocity_value(0)) );
if ( is_cyclone )
    {
    /*
    ----------------------------------------
    Set the break gain velocities to 300 IPS
    ---------------------------------------- */
    x = 300.0;
    x = adjust_for_units( CurrentPart.velocity_units, x, IPS_UNITS );
    u = (unsigned) x1_velocity( x );
    va[vi++].set( BREAK_VEL_2_VN, u );
    va[vi++].set( BREAK_VEL_3_VN, u );
    }
else
    {
    va[vi++].set( BREAK_VEL_2_VN, x1_velocity(SureTrakSetup.gain_break_velocity_value(1)) );
    va[vi++].set( BREAK_VEL_3_VN, x1_velocity(SureTrakSetup.gain_break_velocity_value(2)) );
    }

for (i=0; i<3; i++ )
    {
    vn = normal_gain_vars[i];

    if ( i > 0 && is_cyclone )
        {
        /*
        ---------------------------------------------------------
        I don't set the integral and derivative any more (5/8/13)
        Break gains 2&3 (indexes 1 & 2) don't do anything but
        I set them to 300,1. 1 is specified by "1000".
        --------------------------------------------------------- */
        u = 1000;
        }
    else
        {
        x = SureTrakSetup.gain_break_gain_value(i);
        x *= 1000.0;
        u = (unsigned) round( x, 1.0 );
        }
    va[vi++].set( vn, u );
    }

va[vi++].set( TIME_INTERVAL_MS_VN, (unsigned) CurrentPart.ms_per_time_sample );
va[vi++].set( NOF_TIME_SAMPLES_VN, (unsigned) CurrentPart.nof_time_samples   );

/*
-----------------------------------------------------------------------------------
See if this machine uses biscuit time delay or absolute time to measure the biscuit
if absolute then zero the time delay as I am going to calculate the biscuit size.
----------------------------------------------------------------------------------- */
if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT )
    u = 0;
else
    u = (unsigned) CurrentPart.biscuit_time_delay*1000;
va[vi++].set( BISCUIT_DELAY_MS_VN, u );

u = (unsigned) CurrentPart.zero_speed_output_wire;
va[vi++].set( ZERO_SPEED_OUTPUT_MASK_VN, mask_from_wire(u) );

/*
----------------
Pressure Control
---------------- */
if ( HavePressureControl && PressureControl.is_enabled )
    {
    u = 1;
    v = PressureControl.voltage_output_range;
    v <<= 4;
    u |= v;
    if ( PressureControl.use_only_positive_voltages )
        u |= USE_ONLY_POSITIVE_VOLTAGES_MASK;
    /*
    --------------------------------------------------------
    The ls that arms the pressure control goes in bits 1,2,3
    Range = [LS1 = 0 to LS6 = 5]
    -------------------------------------------------------- */
    v = PressureControl.arm_intensifier_ls_number;
    v--;
    if ( v >= MAX_FTII_LIMIT_SWITCHES )
        v = 0;
    else
        v <<= 1;
    u |= v;

    va[vi++].set( PC_CONFIG_WORD_VN,     u );
    va[vi++].set( PC_PRES_LOOP_GAIN_VN,  PressureControl.pressure_loop_gain.uint_value() );
    va[vi++].set( PC_INTEGRAL_GAIN_VN,   PressureControl.integral_gain.uint_value() );
    va[vi++].set( PC_DERIVATIVE_GAIN_VN, PressureControl.derivative_gain.uint_value() );

    if ( PressureControl.proportional_valve )
        {
        x = percent_to_volts( PressureControl.retract_volts );
        }
    else
        {
        x = PressureControl.retract_volts.real_value();
        if ( x < -10.0 || x > 10.0 )
            x /= 1000.0;
        }
    va[vi++].set( PC_RETRACT_VOLTS_VN, x );

    if ( PressureControl.proportional_valve )
        {
        x = percent_to_volts( PressureControl.park_volts );
        }
    else
        {
        x = PressureControl.park_volts.real_value();
        if ( x < -10.0 || x > 10.0 )
            x /= 1000.0;
        }
    va[vi++].set( PC_PARK_VOLTS_VN, x );

    step_index = PC_RAMP_1_INDEX;
    for ( i=0; i<PressureControl.nof_steps; i++ )
        {
        if ( PressureControl.step[i].is_goose_step )
            {
            s = PressureControl.step[i].ramp;
            if ( PressureControl.step[i].is_goose_percent )
                set_percent( s, TRUE );
            u = pressure_to_volts_16bit( s );
            u |= Bit31;
            if ( PressureControl.step[i].is_goose_percent )
                u |= Bit30;
            }
        else
            {
            u = PressureControl.step[i].ramp.uint_value();
            /*
            -------------------------------------------------------------
            If this is open loop the ramp value must be greater than 0 ms
            ------------------------------------------------------------- */
            if ( PressureControl.proportional_valve )
                {
                if ( u < 1 )
                    u = 1;
                }
            }
        va[vi++].set( vars[step_index++], u );

        s = PressureControl.step[i].pressure;
        if ( PressureControl.step[i].is_percent )
            set_percent( s, TRUE );
        u = pressure_to_volts_16bit( s );
        if ( PressureControl.step[i].is_open_loop )
            u |= Bit31;
        if ( PressureControl.step[i].is_percent )
            u |= Bit30;
        va[vi++].set( vars[step_index++], u );

        u = PressureControl.step[i].holdtime.uint_value();
        va[vi++].set( vars[step_index++], u );
        }

    while( i<MAX_PRESSURE_CONTROL_STEPS )
        {
        u = 0;
        va[vi++].set( vars[step_index++], u );

        u = 0x7FFF;
        va[vi++].set( vars[step_index++], u );

        u = 0;
        va[vi++].set( vars[step_index++], u );
        i++;
        }
    }
else
    {
    u = 0;
    va[vi++].set( PC_CONFIG_WORD_VN, u );
    }

/*
---------------
Retract Control
--------------- */

if ( PressureControl.retctl_is_enabled )
    {
    va[vi++].set( PC_RETCTL_DAC_VN, (unsigned) 4 );  /* DAC channel to be used for retract control */

    /*
    ------------------------------------------------------------------------------
    The Retract control arm pressure is stored as a voltage * 1000 but the type is
    just a number so I have to convert to an unsigned value myself.
    ------------------------------------------------------------------------------ */
    va[vi++].set( PC_RETCTL_ARM_PRES_VN, percent_to_unsigned_volts(PressureControl.retctl_arm_pres, 10.0) );

    /*
    -------------------------------------------------------------------------------------------------
    At the present time the only pressure control is for a proportional valve, which will always be %
    ------------------------------------------------------------------------------------------------- */
    s = PressureControl.retctl_pres;
    set_percent( s, TRUE );
    u = pressure_to_volts_16bit( s, 10.0 );
    u |= Bit31;
    va[vi++].set( PC_RETCTL_PRES_VN, u );
    }
else
    {
    va[vi++].set( PC_RETCTL_ARM_PRES_VN, (unsigned) 0 );

    /*
    ---------------------------------------------------------
    Setting the pressure to zero disables the retract control
    --------------------------------------------------------- */
    va[vi++].set( PC_RETCTL_PRES_VN, (unsigned) 0 );
    }

/*
----------------------------------------------------------------------------
I only allocated memory for the suretrak steps if this machine is controlled
---------------------------------------------------------------------------- */
if ( CurrentMachine.suretrak_controlled )
    update_suretrak_step_value( va, vi );
s.get_ft2_part_settings_file_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
va.put( s );
}

/***********************************************************************
*                       CHECK_BREAK_GAIN_VALUES                        *
***********************************************************************/
static bool check_break_gain_values()
{
static STRING_CLASS s = TEXT("1.0");
bool         have_no_zero_gains;
int          i;
int          n;
double       x;

if ( CurrentMachine.is_cyclone )
    n = 1;
else
    n = NOF_BREAK_GAIN_SETTINGS;
have_no_zero_gains = true;

for ( i=0; i<n; i++ )
    {
    x = SureTrakSetup.gain_break_gain_value(i);

    if ( is_zero(x) )
        {
        x = 1.0;
        SureTrakSetup.set_gain_break_gain(i, s );
        s.set_text( WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow, BREAK_GAIN_1_EBOX+i );
        have_no_zero_gains = false;
        }
    }

return have_no_zero_gains;
}

/***********************************************************************
*                      CHECK_MSL_OF_SURETRAK_PART                      *
***********************************************************************/
static void check_msl_of_suretrak_part()
{
int i;
int fast_shot_step;
int n;
float estimated_max_velocity;
float fast_shot_velocity;
float v;
double x;
HWND   w;
STRING_CLASS s1;
STRING_CLASS s2;

if ( !CurrentMachine.suretrak_controlled  )
    return;

n = SureTrakSetup.nof_steps();
estimated_max_velocity = SureTrakParam.max_velocity();

fast_shot_step = 1;
fast_shot_velocity = -100.0;

SureTrakSetup.rewind();
for ( i=0; i<n; i++ )
    {
    SureTrakSetup.next();
    if ( SureTrakSetup.vel_is_percent() )
        v = estimated_max_velocity * SureTrakSetup.vel_value() / 100.0;
    else
        v =  SureTrakSetup.vel_value();

    if ( v > fast_shot_velocity )
        {
        fast_shot_velocity = v;
        fast_shot_step     = i;
        }
    }

if ( fast_shot_step > 0 && fast_shot_velocity > 0.0 )
    {
    SureTrakSetup.rewind();
    v = 0;
    i = 0;
    while ( SureTrakSetup.next() )
        {
        if ( i == (fast_shot_step-1) )
            {
            v = SureTrakSetup.end_pos_value();
            if ( CurrentPart.min_stroke_length < v )
                {
                SureTrakSetup.next();
                if ( is_empty(SureTrakSetup.end_pos_string()) )
                    v += CurrentPart.total_stroke_length;
                else
                    v += SureTrakSetup.end_pos_value();
                x = v;
                x /= 2.0;
                x = round( x, 0.01 );
                s2 = StringTable.find( TEXT("MSL_TOO_SMALL") );
                s1 = StringTable.find( TEXT("RECOMMENDED_MSL") );
                s1 += ascii_double( x );
                if ( MessageBox(MainWindow, s1.text(), s2.text(), MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK )
                    {
                    CurrentPart.set_min_stroke_length( x );
                    w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;
                    set_text( w, MIN_STROKE_LEN_EDITBOX, ascii_double(x) );
                    }
                }
            break;
            }
        i++;
        }
    }
}

/***********************************************************************
*                        SAVE_VELOCITY_CHANGES                         *
***********************************************************************/
void save_velocity_changes()
{
int id;
HWND parent;
HWND w;
STRING_CLASS s;
BOOLEAN have_change;

have_change = FALSE;

parent = WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow;

/*
--------------------------------------------------------------------------------
Reload the setups. The fact that I exist means the operator is allowed to change
nothing but the velocities. Make sure if any other changes were made that
I ignore them.
-------------------------------------------------------------------------------- */
if ( SureTrakSetup.find(CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
    {
    id = VEL_1_EDITBOX;

    SureTrakSetup.rewind();
    while ( SureTrakSetup.next() )
        {
        w = GetDlgItem( parent, id );
        if ( StModList.contains(w) )
            {
            s.get_text( w );
            SureTrakSetup.set_vel( s.text() );
            have_change = TRUE;
            }
        id += CONTROLS_PER_STEP;
        }

    if ( have_change )
        {
        if ( SureTrakSetup.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name) )
            {
            MessageBox( 0, StringTable.find(TEXT("SAVING_VELOCITY_CHANGES")), CurrentPart.name.text(), MB_OK );
            refresh_suretrak_setup();
            update_ftii_control_program();
            send_new_monitor_setup_event();
            }
        }
    }
}

/***********************************************************************
*                      CLEAR_SHOTS_AND_PARAMETERS                      *
***********************************************************************/
static BOOLEAN clear_shots_and_parameters( HWND w )
{
static TCHAR all_profiles[] = TEXT( "*.pr?" );

DB_TABLE     db;
NAME_CLASS   s;
STRING_CLASS t;

s = StringTable.find( TEXT("DELETE_ALL_SHOTS") );
t = StringTable.find( TEXT("SHOT_NAME_HAS_CHANGED") );
if ( IDCANCEL == MessageBox(w, s.text(), t.text(), MB_OKCANCEL | MB_ICONSTOP | MB_SYSTEMMODAL) )
    return FALSE;

s.get_part_results_dir_file_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name, 0 );
empty_directory( s.text(), all_profiles );

s.get_graphlst_dbname( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
s.delete_file();
db.create( s );

s.get_shotparm_csvname( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
s.delete_file();
db.create( s );

return TRUE;
}

/***********************************************************************
*                       SAVE_SHOT_NAME_TEMPLATE                        *
***********************************************************************/
static void save_shot_name_template( HWND w )
{
int             length;
int             old_length;
STRING_CLASS    s;
SHOT_NAME_CLASS sn;

sn.init( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
sn.get();

old_length = sn.shot_name_length();

/*
--------------------------------------------------------
The shot name length is the length of the example string
-------------------------------------------------------- */
s.get_text( w, SN_EXAMPLE_TB );
length = s.len();
s = length;

if ( length != old_length )
    {
    if ( !clear_shots_and_parameters(w) )
        return;
    }
sn.set_shot_name_length( s );

s.get_text( w, SN_TEMPLATE_EDITBOX );
sn.set_shot_name_template( s );
sn.put();
}

/***********************************************************************
*                               SAVE_CHANGES                           *
***********************************************************************/
static void save_changes()
{
ANALOG_SENSOR a;
int       i;
int       n;
int       new_units;
int       old_units;
int       pi;
unsigned  new_type;
unsigned  old_type;
int       b;
bool      need_parameter_refresh;
bool      need_suretrak_refresh;
int       x;
float     y;
HWND      w;
HWND      wc;
int       sensor_index;
EXTERNAL_PARAMETER_SETTINGS_CLASS eps;
EXTERNAL_SHOT_NAME_CLASS esn;
NAME_CLASS      s;
SETPOINT_CLASS  sp;
CHAXIS_LABEL_CLASS label;
LISTBOX_CLASS lb;
SHOT_NAME_RESET_CLASS sr;

temp_message( StringTable.find(TEXT("SAVING")) );

w = WindowInfo[BASIC_SETUP_TAB].dialogwindow;

/*
--------------
Distance Units
-------------- */
for ( i=0; i<NofDistanceButtons; i++ )
    {
    if ( is_checked(w, DistanceButton[i]) )
        {
        CurrentPart.set_distance_units( (int) DistanceUnits[i] );
        break;
        }
    }

/*
--------------
Velocity Units
-------------- */
for ( i=0; i<NofVelocityButtons; i++ )
    {
    if ( is_checked(w, VelocityButton[i]) )
        {
        CurrentPart.set_velocity_units( VelocityUnits[i] );
        break;
        }
    }

CurrentPart.read_x4_divisor();

need_suretrak_refresh = false;
y = ebox_float( w, TOTAL_STROKE_LEN_EDITBOX );
if ( not_float_zero(CurrentPart.total_stroke_length - y) )
    need_suretrak_refresh = true;

CurrentPart.sp[PARTSET_TOTAL_STROKE_LEN_INDEX].value.get_text( w, TOTAL_STROKE_LEN_EDITBOX );
CurrentPart.sp[PARTSET_MIN_STROKE_LEN_INDEX].value.get_text( w, MIN_STROKE_LEN_EDITBOX );
CurrentPart.sp[PARTSET_EOS_VEL_INDEX].value.get_text( w, EOS_VEL_EDITBOX );
CurrentPart.sp[PARTSET_ZERO_SPEED_OUTPUT_WIRE_INDEX].value.get_text( w, ZERO_SPEED_OUTPUT_WIRE_PB );
CurrentPart.set_zero_speed_output_enable( is_checked(w, ZERO_SPEED_OUTPUT_ENABLE_XB) );

/*
----------------------
User Defined Positions
---------------------- */
w = WindowInfo[USER_DEFINED_TAB].dialogwindow;

for ( i=0; i<USER_VEL_COUNT; i++ )
    CurrentPart.sp[PARTSET_USER_VEL_1_INDEX+i].value.get_text( w, VELOCITY_POSITION_1_EDITBOX+i );

for ( i=0; i<USER_AVG_VEL_COUNT; i++ )
    {
    CurrentPart.sp[PARTSET_AVG_START_1_INDEX+i].value.get_text( w, AVG_VELOCITY_START_1_EDITBOX+i );
    CurrentPart.sp[PARTSET_AVG_END_1_INDEX+i].value.get_text( w, AVG_VELOCITY_END_1_EDITBOX+i );
    }

/*
--------------
Limit Switches
-------------- */
if ( CurrentMachine.suretrak_controlled )
    wc = SuretrakProfileWindow;
else
    wc = w;

n = MAX_FTII_LIMIT_SWITCHES;

for ( i=0; i<n; i++ )
    {
    CurrentLimitSwitch.desc(i).get_text( wc, LS1_DESC_EB+i );
    CurrentLimitSwitch.pos(i).get_text(  wc, LS1_POS_EB+i );
    if ( !(CurrentMachine.suretrak_controlled && HideSureTrakLimitSwitchWires) )
        CurrentLimitSwitch.wire(i).get_text( wc, LS1_WIRE_PB+i );
    }

CurrentLimitSwitch.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
-------------------
Advanced Part Setup
------------------- */
w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;

CurrentPart.sp[PARTSET_PLUNGER_DIA_INDEX].value.get_text( w, PLUNGER_DIAMETER_EDITBOX );
CurrentPart.sp[PARTSET_SLEEVE_FILL_DIST_INDEX].value.get_text( w, SLEEVE_FILL_DISTANCE_EDITBOX );
CurrentPart.sp[PARTSET_RUNNER_FILL_DIST_INDEX].value.get_text( w, RUNNER_FILL_DISTANCE_EDITBOX );
CurrentPart.sp[PARTSET_MIN_CSFS_POS_INDEX].value.get_text( w, CSFS_MIN_POSITION_EDITBOX );
CurrentPart.sp[PARTSET_MIN_CSFS_VEL_INDEX].value.get_text( w, CSFS_MIN_VELOCITY_EDITBOX );
CurrentPart.sp[PARTSET_RISE_CSFS_VEL_INDEX].value.get_text( w, CSFS_RISE_VELOCITY_EDITBOX );
CurrentPart.sp[PARTSET_TIME_FOR_INTENS_PRES_INDEX].value.get_text( w, TIME_FOR_INTENS_PRES_EDITBOX );
CurrentPart.sp[PARTSET_PRES_FOR_RESP_TIME_INDEX].value.get_text( w, PRES_FOR_RESPONSE_TIME_EDITBOX );

y = ebox_float( w, BISCUIT_TIME_DELAY_EDITBOX );
if ( CurrentMachine.monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT )
    y *= 1000.0;
CurrentPart.set_biscuit_time_delay( (int32) y );

save_shot_name_template( w );

sr.init( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
sr.get();
sr.set_enable( tobool(is_checked(w, RESET_SHOT_COUNT_ENABLE_XB)) );
s.get_text( w, RESET_TIME_EB );
sr.set_time( s );
sr.set_purge_on_reset( is_checked(w, PURGE_ON_RESET_XB) );
sr.put();

/*
---------------------------
External parameter settings
--------------------------- */
eps.file_name.get_text( w, EXTERNAL_PARAMETER_FILE_NAME_EB );
eps.enable           = tobool( is_checked(w, EXTERNAL_PARAMETER_FILE_ENABLE_XB) );
eps.available_at_eos = tobool( is_checked(w, EXTERNAL_PARAMETER_FILE_IS_EOS_XB) );
eps.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
--------------------------------
External Shot Name File Settings
-------------------------------- */
esn.init( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
esn.get();
esn.set_enable( is_checked(w, SNF_ENABLE_XB) );
esn[EXTERNAL_SHOT_NAME_FILE_NAME_INDEX].value.get_text( w, SNF_NAME_EB );
esn[EXTERNAL_SHOT_NAME_FILE_COMMAND_INDEX].value.get_text( w, SNF_COMMAND_EB );
esn.put();

/*
-----------------
Impact Param File
----------------- */
if ( sp.count() > 0 && sp.count() != IMPACT_PARAM_FILE_SETPOINT_COUNT )
    sp.empty();
sp.setsize( IMPACT_PARAM_FILE_SETPOINT_COUNT );

sp[IMPACT_PARAM_FILE_ENABLED_INDEX].desc = TEXT( "Enable" );
sp[IMPACT_PARAM_FILE_NAME_INDEX].desc    = TEXT( "File Name" );
sp[IMPACT_PARAM_FILE_ENABLED_INDEX].value = is_checked( w, IMPACT_PARAM_FILE_ENABLE_XB );
sp[IMPACT_PARAM_FILE_NAME_INDEX].value.get_text( w, IMPACT_PARAM_FILE_NAME_EB );

s.get_impact_param_file_csvname( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
sp.put( s );

/*
---------------------
FasTrak Channel Setup
--------------------- */
w = WindowInfo[FASTRAK_CHANNELS_TAB].dialogwindow;

/*
----------------------------
Save the channel axis labels
---------------------------- */
label.init( Nof_Editable_Channels );
for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    if ( NeedToCheckChannels5and7 && (i==4) )
        label.set( i, StringTable.find(TEXT("LVDT")) );
    else if ( NeedToCheckChannels5and7 && (i==6) )
        label.set( i, StringTable.find(TEXT("CMD_STRING")) );
    else if ( s.get_text(w, CHAN_1_AXIS_LABEL_EBOX+i) )
        label.set( i, s.text() );
    else
        label.set( i, EmptyString );
    }

label.put( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
label.cleanup();

for ( i=0; i<Nof_Editable_Channels; i++ )
    {
    OriginalAnalogSensor[i] = CurrentPart.analog_sensor[i];
    if ( NeedToCheckChannels5and7 && (i==4 || i==6) )
        {
        CurrentPart.analog_sensor[i] = PLUS_MINUS_10_VOLT_SENSOR;
        }
    else
        {
        sensor_index = SendDlgItemMessage( w, FT_CHANNEL_1_LISTBOX+i, CB_GETCURSEL, 0, 0L );
        if ( sensor_index != CB_ERR )
            {
            lb.init( w, FT_CHANNEL_1_LISTBOX+i );
            x = a.index( lb.selected_text() );
            if ( x == NO_INDEX )
                x = DEFAULT_ANALOG_SENSOR;
            CurrentPart.analog_sensor[i] = x;
            }
        }
    }

CurrentPart.set_head_pressure_channel( (short) NO_FT_CHANNEL );
CurrentPart.set_rod_pressure_channel( (short) NO_FT_CHANNEL );
for ( b=HEAD_PRES_CHAN_1_BUTTON; b<HEAD_PRES_NO_CHAN_BUTTON; b++ )
    {
    if ( is_checked(w, b) )
        {
        CurrentPart.set_head_pressure_channel( (short) (1 + b - HEAD_PRES_CHAN_1_BUTTON) );
        break;
        }
    }

for ( b=ROD_PRES_CHAN_1_BUTTON; b<ROD_PRES_NO_CHAN_BUTTON; b++ )
    {
    if ( is_checked(w, b) )
        {
        CurrentPart.set_rod_pressure_channel( (short) (1 + b - ROD_PRES_CHAN_1_BUTTON) );
        break;
        }
    }

need_parameter_refresh = false;

/*
--------------
Pressure units
-------------- */
i = 0;

if ( CurrentMachine.impact_pres_type[POST_IMPACT_INDEX] & ROD_PRESSURE_CURVE )
    {
    if ( CurrentPart.rod_pressure_channel != NO_FT_CHANNEL )
        i = CurrentPart.rod_pressure_channel - 1;
    }
else
    {
    if ( CurrentPart.head_pressure_channel != NO_FT_CHANNEL )
        i = CurrentPart.head_pressure_channel - 1;
    }

new_units = a[CurrentPart.analog_sensor[i]].units ;
if ( new_units != a[OriginalAnalogSensor[i]].units )
    need_parameter_refresh = true;

if ( new_units != NO_UNITS )
    {
    /*
    ------------------------------------------------------------------------------------------------------------------
    The internal parameter pressure type units must be changed if the units of the head and rod pressures have changed
    ------------------------------------------------------------------------------------------------------------------ */
    CurrentPart.set_pressure_units( new_units );
    n = CurrentParam.count();
    for ( i=0; i<n; i++ )
        {
        if ( CurrentParam.parameter[i].vartype & PRESSURE_VAR && CurrentParam.parameter[i].input.type == INTERNAL_PARAMETER )
            CurrentParam.parameter[i].units = CurrentPart.pressure_units;
        }
    }

n = CurrentParam.count();

for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    {
    if ( OriginalAnalogSensor[i] != CurrentPart.analog_sensor[i] )
        {
        old_units = a[OriginalAnalogSensor[i]].units;
        new_units = a[CurrentPart.analog_sensor[i]].units;
        old_type  = a[OriginalAnalogSensor[i]].vartype;
        new_type  = a[CurrentPart.analog_sensor[i]].vartype;
        if ( old_units != new_units || old_type != new_type )
            {
            for ( pi=0; pi<n; pi++ )
                {
                if ( CurrentParam.parameter[pi].input.type == FT_ANALOG_INPUT || CurrentParam.parameter[pi].input.type == FT_TARGET_INPUT || CurrentParam.parameter[pi].input.type == FT_EXTENDED_ANALOG_INPUT )
                    {
                    if ( CurrentFtAnalog.array[pi].channel == (i+1) )
                        {
                        need_parameter_refresh = true;
                        CurrentParam.parameter[pi].units   = new_units;
                        CurrentParam.parameter[pi].vartype = new_type;
                        }
                    }
                }
            }
        }
    }

/*
------------------------------------------------------------------------------------------------
The FT_TARGET_INPUT now uses the end_value for the start position. If that value is "0" in the
file then I assume it is an old value and really represents MSL, as that is what we used for the
start position then. If any of the values are "0" here I need to change them to "0.0" so I will
know that the start position really is 0 and not MSL.
------------------------------------------------------------------------------------------------ */
for ( pi=0; pi<n; pi++ )
    {
    if ( CurrentParam.parameter[pi].input.type == FT_TARGET_INPUT )
        {
        if ( CurrentFtAnalog.array[pi].end_value == ZeroString )
            CurrentFtAnalog.array[pi].end_value = ZeroPointZero;
        }
    }

/*
---------------------------------------------------
Check to see if the min stroke length is too small.
--------------------------------------------------- */
if ( HaveSureTrakControl )
    check_msl_of_suretrak_part();

CurrentPart.refresh_time_sample();   // The monitor setup may have changed the time sample
CurrentPart.update();
CurrentPart.save();
CurrentParam.save();
CurrentMarks.save();
CurrentFtAnalog.save(CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
CurrentFtCalc.save(CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
--------------------------------------------------------------------------
Update the original analog sensor list so I will know if it changes again.
-------------------------------------------------------------------------- */
for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    OriginalAnalogSensor[i] = CurrentPart.analog_sensor[i];

/*
--------------------
Save the setup sheet
-------------------- */
w = WindowInfo[SETUP_SHEET_TAB].dialogwindow;
if ( SendDlgItemMessage( w, SETUP_EBOX, EM_GETMODIFY, 0, 0) )
    {
    s.get_part_dir_file_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name, SETUP_SHEET_FILE );
    save_edit_control_to_file( s, w, SETUP_EBOX );
    }

/*
-----------------------------
Save the parameter sort order
----------------------------- */
CurrentSort.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
----------------------------------------------------------------------------------------
I want to check the break gains even if no changes have been made to the suretrak setups
---------------------------------------------------------------------------------------- */
if ( CurrentMachine.suretrak_controlled )
    {
    if ( !check_break_gain_values() )
        {
        SureTrakHasChanged = TRUE;
        /*
        ---------------------------------------------------------
        I don't need to nag if this is an upgrade from ft2 to st2
        --------------------------------------------------------- */
        if ( !NeedToCheckChannels5and7 )
            {
            DialogBox(
                MainInstance,
                TEXT("ZERO_BREAK_GAIN_CHANGED_DIALOG"),
                MainDialogWindow,
                (DLGPROC) zero_break_gain_changed_dialog_proc );
            }
        }
    }

/*
-----------------
Sure-Trak Changes
----------------- */
if ( SureTrakHasChanged )
    {
    if ( !CurrentMachine.suretrak_controlled )
        {
        stringtable_message_box( TEXT("CHANGE_MA_SETUP"), TEXT("NOT_ST_MA") );
        CurrentMachine.suretrak_controlled = TRUE;
        }

    if ( !CurrentMachine.has_binary_valve() )
        {
        if ( !check_low_impact_values() )
            {
            show_low_impact_error();
            need_suretrak_refresh = true;
            }
        }
    SureTrakHasChanged = FALSE;
    }

/*
------------------------------------------------------------------------------------------
I used to save the suretrak only if someting was changed but now I do it every time (6.51)
------------------------------------------------------------------------------------------ */
if ( HaveSureTrakControl )
    SureTrakSetup.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
-----------------------------------------------------------------------------------------
Save the warmup setup. The multipart run list is saved when you run the setup program and
does not need to be saved here.
----------------------------------------------------------------------------------------- */
save_warmup_shot();

/*
--------------------------------------------
Clean up the mod lists for the suretrak page
-------------------------------------------- */
if ( StModList.count() > 0 )
    {
    if ( CurrentDialogNumber == SURETRAK_PROFILE_TAB )
        InvalidateRect( WindowInfo[CurrentDialogNumber].dialogwindow, 0, TRUE );
    StModList.remove_all();
    }

if ( HaveStModTbox.is_visible() )
    HaveStModTbox.hide();

if ( HavePressureControl )
    {
    get_new_pressure_control_steps();
    PressureControl.save( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    }

if ( need_suretrak_refresh )
    refresh_suretrak_setup();

if ( need_parameter_refresh )
    refresh_parameter_limits();

update_ftii_control_program();

send_new_monitor_setup_event();
}

/***********************************************************************
*                       SHOW_SELECT_PART_MESSAGE                       *
***********************************************************************/
static void show_select_part_message()
{
STRING_CLASS s;
STRING_CLASS t;
t = StringTable.find( TEXT("CANNOT_COMPLY_STRING") );
s = StringTable.find( TEXT("MUST_SELECT") );
MessageBox( NULL, s.text(), t.text(), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                        SHOW_LAST_PART_MESSAGE                        *
***********************************************************************/
static void show_last_part_message()
{
STRING_CLASS s;
STRING_CLASS t;
t = StringTable.find( TEXT("CANNOT_COMPLY_STRING") );
s = StringTable.find( TEXT("LAST_PART") );
MessageBox( NULL, s.text(), t.text(), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                              KILL_PART                               *
***********************************************************************/
static void kill_part( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
NAME_CLASS s;

s.get_part_dir_file_name( computer, machine, part, 0 );
s.delete_directory();
}

/***********************************************************************
*                         DELETE_CURRENT_PART                          *
***********************************************************************/
static void delete_current_part()
{
MACHINE_ENTRY * m;
INT             answer_button;

if ( !HaveCurrentPart )
    {
    show_select_part_message();
    return;
    }

if ( CurrentMachine.current_part == CurrentPart.name )
    {
    stringtable_message_box( TEXT("IS_CURRENT_PA"), TEXT("CANNOT_COMPLY_STRING") );
    return;
    }

m = find_machine_entry( CurrentMachine.name );
if ( !m )
    return;

if ( m->partlist.count() < 2 )
    {
    show_last_part_message();
    return;
    }

/*
--------------------------------------------
See if this part is a warmup part for anyone
-------------------------------------------- */
TEXT_LIST_CLASS t;
WARMUP_CLASS    wc;
STRING_CLASS    s;

if ( get_current_partlist(t) )
    {
    t.rewind();
    while ( t.next() )
        {
        if ( CurrentPart.name != t.text() )
            {
            s = t.text();
            if ( wc.get(CurrentPart.computer, CurrentPart.machine, s) )
                {
                if ( wc.partname == CurrentPart.name )
                    {
                    s = StringTable.find(TEXT("WARMUP_PART_FOR") );
                    s += t.text();
                    MessageBox( MainWindow, s.text(), StringTable.find(TEXT("CANNOT_COMPLY_STRING")), MB_OK | MB_SYSTEMMODAL );
                    return;
                    }
                }
            }
        }
    }

answer_button = stringtable_okcancel_box( TEXT("WISH_TO_CONTINUE"), TEXT("ABOUT_TO_DEL") );
if ( answer_button != IDOK )
    return;

kill_part( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

reload_partlist( CurrentMachine.name );
fill_parts(0);
}

/***********************************************************************
*                      NEW_MACHINE_SETUP_CALLBACK                      *
***********************************************************************/
void new_machine_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
SendMessage( MainWindow, WM_NEWSETUP, 0, 0L );
}

/***********************************************************************
*                          MONITOR_THIS_PART                           *
***********************************************************************/
static void monitor_this_part()
{
HWND       w;
WORKER_LIST_CLASS worker;
STRING_CLASS file;
STRING_CLASS format;
STRING_CLASS oldpart;
STRING_CLASS s;
FILE_CLASS   f;
int          i;
BOOLEAN have_monitor_log = FALSE;

file = get_ini_string( VisiTrakIniFile, ConfigSection, MonitorLogFileKey );
if ( file != unknown_string() )
    {
    format = get_ini_string( VisiTrakIniFile, ConfigSection, MonitorLogFormatKey );
    if ( file != unknown_string() )
        {
        worker.load();
        have_monitor_log = TRUE;
        }
    }

CurrentMachine.current_part = CurrentPart.name;
set_current_part_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
Warmup.create_runlist( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );

/*
------------------------------------------------------------------------
Send a monitor setup event so the board monitor and suretrak will update
------------------------------------------------------------------------ */
poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentMachine.name.text() );

/*
---------------------------------
Hide the monitor this part button
--------------------------------- */
w = GetDlgItem( MainDialogWindow, MONITOR_THIS_PART_BUTTON );
set_text( w, StringTable.find(TEXT("IS_CURRENT_PA")) );
EnableWindow( w, FALSE );

if ( have_monitor_log )
    {
    if ( log_file_string(s, format.text(), worker.full_name(CurrentMachine.human.text()), CurrentMachine.name, oldpart, CurrentPart.name) )
        {
        if ( f.open_for_append(file.text()) )
            {
            f.writeline( s.text() );
            f.close();
            }
        }
    }

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( CurrentParam.parameter[i].input.type == FT_STATIC_TEXT_PARAMETER )
        {
        if ( CurrentParam.parameter[i].static_text.popup_on_monitor )
            {
            StaticTextParameterNumber = i+1;
            DialogBox(
                MainInstance,
                TEXT("POPUP_STATIC_TEXT_PARAMETER"),
                MainWindow,
                (DLGPROC) static_text_parameter_dialog_proc );
            }
        }
    }
}

/***********************************************************************
*                            DO_NEWSETUP                               *
***********************************************************************/
static void do_newsetup()
{
BOOLEAN         had_current_part;
LISTBOX_CLASS   lb;
MACHINE_ENTRY * e;

had_current_part = HaveCurrentPart;
read_machines_and_parts();
HaveCurrentPart = FALSE;

if ( had_current_part )
    {
    e = find_machine_entry( CurrentMachine.name );
    if ( e )
        {
        lb.init( MainDialogWindow, MACHINE_LISTBOX_ID );
        fill_machine_cb( MainDialogWindow, MACHINE_LISTBOX_ID );
        if ( lb.setcursel(CurrentMachine.name.text()) )
            {
            fill_part_listbox( MainDialogWindow, PART_LISTBOX_ID, e );
            lb.init( MainDialogWindow, PART_LISTBOX_ID );
            if ( lb.setcursel(CurrentPart.name.text()) )
                HaveCurrentPart = TRUE;
            }
        }
    }

if ( !HaveCurrentPart )
    {
    lb.init( MainDialogWindow, MACHINE_LISTBOX_ID );
    lb.setcursel( 0 );
    fill_parts(0);
    }
}

/***********************************************************************
*                           MAIN_CONTEXT_HELP                          *
***********************************************************************/
static void main_context_help( HWND cw )
{
const int32 NOF_CONTROLS = 6;

static FULL_HELP_ENTRY control_help[] = {
    { CREATE_PART_BUTTON,            HELP_CONTEXT,      CREATE_NEW_PART_HELP     },
    { DELETE_PART_BUTTON,            HELP_CONTEXT,      DELETE_THIS_PART_HELP    },
    { COPY_TO_EXISTING_PARTS_BUTTON, HELP_CONTEXT,      COPY_TO_EXISTING_HELP    },
    { COPY_MULTIPLE_BUTTON,          HELP_CONTEXT,      COPY_TO_OTHER_HELP       },
    { MAIN_PRINT_BUTTON,             HELP_CONTEXTPOPUP, PART_SETUP_PRINTING_HELP },
    { RELOAD_BUTTON,                 HELP_CONTEXTPOPUP, RELOAD_SAVED_SETUP_HELP  }
    };

int32 i;

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(MainDialogWindow, control_help[i].id) == cw )
        {
        gethelp( control_help[i].type, control_help[i].context );
        return;
        }
    }

get_overview_help();
}

/***********************************************************************
*                    POSITION_SAVE_ALL_PARTS_DIALOG                    *
*                                                                      *
* The default position for the save_all_parts_dialog is the upper left *
* corner of the client rect of the main dialog. I want to offset this  *
* so I am centered in the main dialog.                                 *
*                                                                      *
***********************************************************************/
static void position_save_all_parts_dialog( HWND w )
{
RECTANGLE_CLASS rm;
RECTANGLE_CLASS r;
WINDOW_CLASS    wc;
long dx;
long dy;

wc = MainDialogWindow;
wc.get_client_rect( rm );

wc = w;
wc.get_move_rect( r );

dx = (rm.width() - r.width() )/2;
dy = (rm.height() - r.height() )/2;

wc.offset( dx, dy );
}

/**********************************************************************
*                      FILL_SAVE_ALL_PARTS_STATICS                    *
**********************************************************************/
static void fill_save_all_parts_statics( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { IDOK,                 TEXT("OK_STRING") },
    { IDCANCEL,             TEXT("CANCEL_STRING") },
    { PRESS_BEGIN_STATIC,   TEXT("SAVE_ALL_PRESS_BEGIN") },
    { ONLY_PARTS_STATIC,    TEXT("SAVE_ALL_ONLY_PARTS") },
    { LOAD_AND_SAVE_STATIC, TEXT("LOAD_AND_SAVE") }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);
WINDOW_CLASS w;

update_statics( hWnd, rlist, nr );

w = hWnd;
w.set_title( StringTable.find(TEXT("SAVE_ALL_PARTS")) );
w.refresh();

set_statics_font( hWnd, DENSITY_CBOX );
}

/***********************************************************************
*                  SET_SAVE_ALL_SETUP_STRINGS                    *
***********************************************************************/
static void set_save_all_setup_strings( HWND hWnd )
{
WINDOW_CLASS w;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { PRESS_BEGIN_STATIC, TEXT("SAVE_ALL_PRESS_BEGIN") },
    { ONLY_PARTS_STATIC, TEXT("SAVE_ALL_ONLY_PARTS") },
    { IDOK, TEXT("BEGIN") },
    { IDCANCEL, TEXT("CANCEL") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rsn, nof_rsn );

w = hWnd;
w.set_title( StringTable.find(TEXT("SAVE_ALL_PARTS")) );
w.refresh();
}

/***********************************************************************
*                    SAVE_ALL_PARTS_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK save_all_parts_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static LISTBOX_CLASS mlb;
static int           nof_machines;
static LISTBOX_CLASS plb;
static int           nof_parts;
static int mi;
static int pi;
static STRING_CLASS    s;
static PART_NAME_ENTRY ori;

int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        position_save_all_parts_dialog( hWnd );
        set_save_all_setup_strings( hWnd );
        ori.set( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        mlb.init( MainDialogWindow, MACHINE_LISTBOX_ID );
        nof_machines = mlb.count();
        plb.init( MainDialogWindow, PART_LISTBOX_ID );
        nof_parts = 0;
        mi  = -1;
        pi  = -1;
        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_SAVE_NEXT_PART:
        if ( pi < 0 )
            {
            mi++;
            if ( mi >= nof_machines )
                {
                mlb.setcursel( ori.machine.text() );
                fill_parts( ori.part );
                NeedToCheckChannels5and7 = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
                }
            s = mlb.item_text( mi );
            if ( machine_is_local(s) )
                {
                mlb.setcursel( mi );
                pi = 0;
                fill_parts(pi);
                nof_parts = plb.count();
                }
            }
        if ( pi >= 0 )
            {
            if ( pi >= nof_parts )
                {
                pi = -1;
                }
            else
                {
                s = plb.item_text(pi);
                plb.setcursel( pi );
                get_current_part();
                save_changes();
                pi++;
                }
            }
        PostMessage( hWnd, WM_SAVE_NEXT_PART, 0, 0 );
        return TRUE;


    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                PostMessage( hWnd, WM_SAVE_NEXT_PART, 0, 0 );
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/**********************************************************************
*                            FILL_STATICS                             *
**********************************************************************/
static void fill_statics( HWND hWnd )
{
static TCHAR EDITPART_CAPTION[] = TEXT( "EDITPART_CAPTION" );
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { MAIN_MACHINE_STATIC,           TEXT("MACHINE_STRING") },
    { MAIN_PART_STATIC,              TEXT("PART_STRING") },
    { CREATE_PART_BUTTON,            TEXT("CREATE_PART") },
    { DELETE_PART_BUTTON,            TEXT("DELETE_PART") },
    { COPY_TO_EXISTING_PARTS_BUTTON, TEXT("COPY_TO_EXISTING_PARTS") },
    { COPY_MULTIPLE_BUTTON,          TEXT("COPY_TO_OTHER_MACHINES") },
    { SAVE_ALL_PARTS_PB,             TEXT("SAVE_ALL_PARTS") },
    { SAVE_CHANGES_BUTTON,           TEXT("SAVE_CHANGES") },
    { CALCULATOR_BUTTON,             TEXT("CALCULATOR") },
    { MAIN_PRINT_BUTTON,             TEXT("PRINT") },
    { CHANGE_FOCUS_STATIC,           TEXT("CHANGE_FOCUS") },
    { RELOAD_BUTTON,                 TEXT("RELOAD") },
    { BASIC_SETUP_RB,                TEXT("BASIC_TAB") },
    { ADVANCED_SETUP_RB,             TEXT("ADVANCED_TAB") },
    { PARAMETERS_SETUP_RB,           TEXT("PARAMETERS_TAB") },
    { POSITIONS_SETUP_RB,            TEXT("POSITIONS_TAB") },
    { MARKS_SETUP_RB,                TEXT("MARKS_TAB") },
    { CHANNELS_SETUP_RB,             TEXT("CHANNELS_TAB") },
    { SHOT_CONTROL_SETUP_RB,         TEXT("SURETRAK_TAB") },
    { PRESSURE_CONTROL_SETUP_RB,     TEXT("PRES_CONTROL_TAB") },
    { SETUP_SCREEN_SETUP_RB,         TEXT("SETUP_SHEET_TAB") }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);
WINDOW_CLASS w;

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, MACHINE_LISTBOX_ID );
set_statics_font( hWnd, PART_LISTBOX_ID );

w = MainWindow;
w.set_title( StringTable.find(EDITPART_CAPTION) );
w.refresh();

w = hWnd;
w.refresh();
}

/***********************************************************************
*                        POSITION_MAIN_WINDOW                          *
***********************************************************************/
static void position_main_window()
{
RECTANGLE_CLASS dr;
RECTANGLE_CLASS mr;
RECTANGLE_CLASS r;
int  x;
int  y;
HWND desktopwindow;

desktopwindow = GetDesktopWindow();
if ( !desktopwindow )
    return;

GetClientRect( desktopwindow, &dr );
GetWindowRect( MainWindow, &mr );
x = (dr.width() - mr.width())/2;

/*
------------------------------------------------------------------------------------
The total height is the height of the main window plus that of the parameters window
or suretrak profile window.
------------------------------------------------------------------------------------ */
y = mr.height();

if ( HaveSureTrakControl )
    GetWindowRect( SuretrakProfileWindow, &r );
else
    GetWindowRect( ParamDialogWindow, &r );
y += r.height();

y = (dr.height() - y)/2;
if ( y < 0 )
    y = 0;

MoveWindow( MainWindow, x, y, mr.width(), mr.height(), TRUE );
}

/***********************************************************************
*                            MainDialogProc                            *
***********************************************************************/
BOOL CALLBACK MainDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int            cmd;
int            i;
int            id;
HWND           w;
HWND           fw;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        fill_statics( hWnd );
        SaveChangesTitle.get_text( hWnd, SAVE_CHANGES_BUTTON );
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        InvalidateRect( hWnd, 0, TRUE );
        return TRUE;

    case WM_DESTROY:
        fill_statics( 0 );
        break;

    case WM_NEW_DATA:
        /*
        --------------------------------------------------------------------
        This is used by the calculator to signal that p1 and p2 have changed
        -------------------------------------------------------------------- */
        refresh_advanced_setup();
        if ( CurrentDialogNumber != ADVANCED_SETUP_TAB )
            {
            show_tab_control( ADVANCED_SETUP_TAB );
            w = WindowInfo[ADVANCED_SETUP_TAB].dialogwindow;
            if ( w )
                {
                w = GetDlgItem( w, SLEEVE_FILL_DISTANCE_EDITBOX );
                SetFocus( w );
                }
            }
        return TRUE;

    case WM_COMMAND:
        if ( cmd == CBN_SELCHANGE && id == MACHINE_LISTBOX_ID )
            {
            fw = GetFocus();
            fill_parts(0);
            SetFocus( fw );
            return TRUE;
            }

        if ( cmd == LBN_SELCHANGE && id == PART_LISTBOX_ID )
            {
            fw = GetFocus();
            get_current_part();
            SetFocus( fw );
            return TRUE;
            }

        switch ( id )
            {
            case SAVE_CHANGES_BUTTON:
                if ( HaveCurrentPart )
                    {
                    if ( CurrentMachine.has_binary_valve() )
                        {
                        if ( !check_for_last_step_end_pos() )
                            {
                            DialogBox(
                               MainInstance,
                               TEXT("NO_LAST_STEP_END_POS_DIALOG"),
                               WindowInfo[SURETRAK_PROFILE_TAB].dialogwindow,
                               (DLGPROC) no_last_step_end_pos_dialog_proc );
                            return TRUE;
                            }
                        }

                    if ( NeedToCallVelocityChanges )
                        save_velocity_changes();
                    else
                        save_changes();
                    }
                else
                    {
                    stringtable_message_box( TEXT("NO_PART_SELECTED"), TEXT("CANNOT_COMPLY_STRING") );
                    }

                if ( RestrictedPartEntry.is_enabled && (RestrictedPartEntry.password_level==CurrentPasswordLevel) && RestrictedPartEntry.exit_when_done )
                    SendMessage( MainWindow, WM_CLOSE, 0, 0L );

                return TRUE;

            case MONITOR_THIS_PART_BUTTON:
                monitor_this_part();
                return TRUE;

            case SAVE_ALL_PARTS_PB:
                CreateDialog(
                    MainInstance,
                    TEXT("SAVE_ALL_PARTS_DIALOG"),
                    MainWindow,
                    (DLGPROC) save_all_parts_dialog_proc );
                return TRUE;

            case RELOAD_BUTTON:
                get_current_part();
                return TRUE;

            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;

            case COPY_MULTIPLE_BUTTON:
                if ( !HaveCurrentPart )
                    show_select_part_message();
                else
                    {
                    CreateDialog(
                        MainInstance,
                        TEXT("COPY_MULTIPLE"),
                        MainWindow,
                        (DLGPROC) copy_multiple_dialog_proc );
                    }
                return TRUE;

            case COPY_TO_EXISTING_PARTS_BUTTON:
                if ( !HaveCurrentPart )
                    show_select_part_message();
                else if ( !CopyToDialogWindow )
                    {
                    CreateDialog(
                        MainInstance,
                        TEXT("COPYTODIALOG"),
                        MainWindow,
                        (DLGPROC) copy_to_dialog_proc );
                    }
                return TRUE;

            case MAIN_PRINT_BUTTON:
                print_part_setup();
                return TRUE;

            case CALCULATOR_BUTTON:
                if ( !HaveCurrentPart )
                    {
                    show_select_part_message();
                    }
                else
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("CALCULATOR_DIALOG"),
                        MainWindow,
                       (DLGPROC) calculator_dialog_proc );
                    }
                return TRUE;

            case DELETE_PART_BUTTON:
                delete_current_part();
                return TRUE;

            case CREATE_PART_BUTTON:
                if ( !HaveCurrentPart )
                    show_select_part_message();
                else
                    {
                    DialogBox(
                        MainInstance,
                        TEXT("ADD_PART"),
                        MainWindow,
                        (DLGPROC) add_part_dialog_proc );
                    for ( i=0; i<MAX_PARMS; i++ )
                        {
                        if ( CurrentParam.parameter[i].input.type == FT_STATIC_TEXT_PARAMETER )
                            {
                            if ( CurrentParam.parameter[i].static_text.popup_on_create )
                                {
                                StaticTextParameterNumber = i+1;
                                DialogBox(
                                    MainInstance,
                                    TEXT("POPUP_STATIC_TEXT_PARAMETER"),
                                    MainWindow,
                                    (DLGPROC) static_text_parameter_dialog_proc );
                                }
                            }
                        }
                    }
                return TRUE;

            case BASIC_SETUP_RB:
            case ADVANCED_SETUP_RB:
            case PARAMETERS_SETUP_RB:
            case POSITIONS_SETUP_RB:
            case MARKS_SETUP_RB:
            case CHANNELS_SETUP_RB:
            case SHOT_CONTROL_SETUP_RB:
            case PRESSURE_CONTROL_SETUP_RB:
            case SETUP_SCREEN_SETUP_RB:
                show_tab_control( id - BASIC_SETUP_RB );
                return TRUE;
            }
        break;

    case WM_CONTEXTMENU:
        main_context_help( (HWND) wParam );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              PAINT_ME                                *
***********************************************************************/
static void paint_me()
{
PAINTSTRUCT  ps;
HWND         w;

w = SureTrakPlotWindow.handle();

if ( !GetUpdateRect(w, NULL, FALSE) )
    return;

BeginPaint( w, &ps );

if ( MyFont )
    SelectObject( ps.hdc, MyFont );

if ( HaveSureTrakPlot )
    SureTrakPlot.paint( ps.hdc );

EndPaint( w, &ps );
}

/***********************************************************************
*                         SURETRAK_PLOT_PROC                           *
***********************************************************************/
LRESULT CALLBACK suretrak_plot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
switch (msg)
    {
    case WM_CREATE:
        SureTrakPlotWindow = hWnd;
        break;

    case WM_PAINT:
        paint_me();
        return 0;

    case WM_DBINIT:
        BringWindowToTop( hWnd );
        BringWindowToTop( hWnd );
        return 0;

    case WM_ACTIVATEAPP:
        if ( BOOL(wParam) )
            {
            if ( CurrentDialogNumber == SURETRAK_PROFILE_TAB || CurrentDialogNumber == PRESSURE_CONTROL_TAB )
                SureTrakPlotWindow.post( WM_DBINIT );
            }
        return 0;

    case WM_SETFOCUS:
        SetFocus( WindowInfo[CurrentDialogNumber].dialogwindow );
        return TRUE;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                     ENUMERATE_CHILD_WINDOWS_PROC                     *
***********************************************************************/
BOOL CALLBACK enumerate_child_windows_proc( HWND hwnd, LPARAM lParam )
{
const int BUFSIZE = 100;
static TCHAR * classname[] = { TEXT("Button"), TEXT("Edit") };
static int nof_classes = 2;

STRING_CLASS s;
TCHAR * buf;
int  i;
int  id;
BOOL is_enabled;
int  n;

s.upsize( BUFSIZE );
buf = s.text();
n = GetClassName( hwnd, buf, BUFSIZE );
if ( n > 0 )
    {
    buf[n] = NullChar;
    for ( i=0; i<nof_classes; i++ )
        {
        if ( s == classname[i] )
            {
            id = (int) GetWindowLong( hwnd, GWL_ID );
            is_enabled = FALSE;
            if ( RestrictedPartEntry.editable_ids.contains(id) )
                is_enabled = TRUE;
            EnableWindow( hwnd, is_enabled );
            }
        }
    }
return TRUE;
}

/***********************************************************************
*                  SHOW_RESTRICTED_PART_ENTRY_SCREEN                   *
* This routine displayes the restricted part entry screen and disables *
* the tab control so no others can be chosen. It then enumerates the   *
* controls on the screen and enables just those in my list. When this  *
* is done the only controls are the "enabled_ids" and the ok button.   *
***********************************************************************/
static void show_restricted_part_entry_screen()
{
static int ids_to_disable[] = { MACHINE_LISTBOX_ID, PART_LISTBOX_ID, COPY_MULTIPLE_BUTTON, COPY_TO_EXISTING_PARTS_BUTTON,
                                DELETE_PART_BUTTON, CREATE_PART_BUTTON, MONITOR_THIS_PART_BUTTON, CALCULATOR_BUTTON, SAVE_ALL_PARTS_PB,
                                BASIC_SETUP_RB, ADVANCED_SETUP_RB, PARAMETERS_SETUP_RB, POSITIONS_SETUP_RB, MARKS_SETUP_RB,
                                CHANNELS_SETUP_RB, SHOT_CONTROL_SETUP_RB, PRESSURE_CONTROL_SETUP_RB, SETUP_SCREEN_SETUP_RB
                                };
static int nof_ids = sizeof(ids_to_disable)/sizeof(int);

BOOLEAN ok_to_continue;;
int  i;
HWND w;

ok_to_continue = FALSE;
for ( i=0; i<TAB_COUNT; i++ )
    {
    if ( RestrictedPartEntry.screen_name == DialogName[i] )
        {
        show_current_dialog( i );
        ok_to_continue = TRUE;
        break;
        }
    }

if ( !ok_to_continue )
    return;

for ( i=0; i<nof_ids; i++ )
    EnableWindow( GetDlgItem(MainDialogWindow, ids_to_disable[i]), FALSE );

i = RestrictedPartEntry.editable_ids[0];
if ( RestrictedPartEntry.position_on_last_changed && RestrictedPartEntry.last_id != 0 )
    i = RestrictedPartEntry.last_id;
w = GetDlgItem( WindowInfo[CurrentDialogNumber].dialogwindow, i );

EnumChildWindows( WindowInfo[CurrentDialogNumber].dialogwindow, enumerate_child_windows_proc, 0 );
SetFocus( w );
eb_select_all( w );
}

/***********************************************************************
*                  EXIT_RESTRICTED_PART_ENTRY_SCREEN                   *
***********************************************************************/
static void exit_restricted_part_entry_screen()
{
int        id;
INI_CLASS  ini;
NAME_CLASS s;
HWND       w;

w  = GetFocus();
id = (int) GetWindowLong( w, GWL_ID );
if ( id != 0 )
    {
    s.get_local_ini_file_name( RESTRICTED_PART_ENTRY_SCREEN_INI_FILE );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    if ( ini.exists() )
        ini.put_int( LastIdKey, id );
    }
}

/***********************************************************************
*                          PASSWORD_LEVEL_CALLBACK                     *
***********************************************************************/
void password_level_callback( TCHAR * topic, short item, HDDEDATA edata )
{
TCHAR buf[MAX_INTEGER_LEN+3];
DWORD bytes_copied;
DWORD bufsize = sizeof( buf );

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
    }

enable_buttons();
if ( HavePressureControl )
    show_pres_controls( WindowInfo[PRESSURE_CONTROL_TAB].dialogwindow );

if ( RestrictedPartEntry.is_enabled && (RestrictedPartEntry.password_level == CurrentPasswordLevel) )
    {
    show_restricted_part_entry_screen();
    }
}

/***********************************************************************
*                              PLOT_MIN_CALLBACK                       *
***********************************************************************/
void plot_min_callback( TCHAR * topic, short item, HDDEDATA edata )
{
if ( !IsIconic(MainWindow) )
    CloseWindow( MainWindow );
}

/***********************************************************************
*                             TOGGLE_FOCUS                             *
***********************************************************************/
void toggle_focus()
{
HWND w;

w = GetFocus();
w = GetParent( w );

if ( w == MainDialogWindow )
    {

    if ( CurrentDialogNumber != NO_TAB )
        SetFocus( WindowInfo[CurrentDialogNumber].dialogwindow );
    }
else
    {
    SetFocus( MainDialogWindow );
    }
}

/***********************************************************************
*                          CHECK_FOR_AUTOSAVE                          *
***********************************************************************/
static void check_for_autosave()
{
NAME_CLASS s;

s.get_auto_save_all_parts_file_name();
if ( s.file_exists() )
    {
    NeedToCheckChannels5and7 = TRUE;
    CreateDialog(
        MainInstance,
        TEXT("AUTO_SAVE_ALL_PARTS_DIALOG"),
        MainWindow,
        (DLGPROC) save_all_parts_dialog_proc );

    s.delete_file();
    }
}

/***********************************************************************
*                        CHECK_FOR_HMI_MESSAGE                         *
* 1. CREATE,old part name,new part name                                *
* 2. MONITOR,new part name                                             *
* 3. SHOW,new part name                                                *
************************************************************************/
static void check_for_hmi_message( STRING_CLASS & cmd_line )
{
BOOLEAN      need_to_load_current_part;
NAME_CLASS   fn;
INI_CLASS    ini;
STRING_CLASS s;

HMIMessageType = HMI_NO_MESSAGE;
HmiOldPart.null();
HmiNewPart.null();

s = cmd_line;
if ( !s.find(CommaChar) )
    return;

s.next_field( CommaChar );

if ( s == HmiShowString )
    HMIMessageType = HMI_SHOW_PART_MESSAGE;

if ( s.contains(HmiCreateString) )
    HMIMessageType |= HMI_CREATE_PART_MESSAGE;

if ( s.contains(HmiMonitorString) )
    HMIMessageType |= HMI_MONITOR_PART_MESSAGE;

need_to_load_current_part = FALSE;
if ( HMIMessageType == HMI_NO_MESSAGE )
    {
    fn.get_local_ini_file_name( EditPartIniFile );
    ini.set_file( fn.text() );
    ini.set_section( ConfigSection );

    if ( ini.find(LoadCurrentPartWhenRunKey) )
        need_to_load_current_part = ini.get_boolean();
    }

if ( need_to_load_current_part )
    HMIMessageType = HMI_SHOW_PART_MESSAGE;

if ( HMIMessageType != HMI_NO_MESSAGE )
    {
    s.next_field( CommaChar );
    HmiOldPart = s;

    /*
    ------------------------------------------------------------------
    I always create the hmi part from the currently monitored part now
    ------------------------------------------------------------------ */
    if ( HMIMessageType & HMI_CREATE_PART_MESSAGE )
        {
        HmiOldPart = CurrentMachine.current_part;
        HmiNewPart = s;
        }
    }
}

/***********************************************************************
*                           GET_NEW_HMI_COMMAND                        *
***********************************************************************/
static void get_new_hmi_command()
{
FILE_CLASS f;
NAME_CLASS s;

s.get_exe_dir_file_name( EditPartCommandLineFile );
if ( f.open_for_read(s) )
    {
    s = f.readline();
    f.close();
    check_for_hmi_message( s );
    }

s.get_exe_dir_file_name( EditPartCommandLineFile );
s.delete_file();
do_hmi_command();
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_NEWSETUP:
        do_newsetup();
        return 0;

    case WM_CONTEXTMENU:
    case WM_HELP:
        get_overview_help();
        return 0;

    case WM_EV_SHUTDOWN:
        SendMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        register_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX, new_machine_setup_callback );
        register_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX,   password_level_callback    );
        register_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX,   plot_min_callback          );
        get_current_password_level();
        fill_machines();
        SetFocus( GetDlgItem(MainDialogWindow, MACHINE_LISTBOX_ID) );
        check_for_autosave();
        return 0;

    case WM_NEW_HMI_COMMAND:
        get_new_hmi_command();
        return 0;

    case WM_COMMAND:
        switch ( id )
            {
            case OPEN_CHOICE:
                if ( CurrentDialogNumber == PARAMETER_LIMITS_TAB )
                    ShowWindow( ParamDialogWindow, SW_SHOW );
                return 0;

            case F10_KEY:
                toggle_focus();
                return 0;
            }
        break;

    case WM_SETFOCUS:
        if ( MainDialogWindow )
            SetFocus( GetDlgItem(MainDialogWindow, MACHINE_LISTBOX_ID) );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    case WM_SIZE:
    case WM_MOVE:
        position_current_dialog();
        break;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                            CREATE_MYFONT                             *
***********************************************************************/
static void create_myfont()
{
LOGFONT f;

f.lfHeight          = 14;
f.lfWidth           = 0;
f.lfEscapement      = 0;
f.lfOrientation     = 0;
f.lfWeight          = FW_DONTCARE;
f.lfItalic          = FALSE;
f.lfUnderline       = FALSE;
f.lfStrikeOut       = FALSE;
f.lfOutPrecision    = 0;
f.lfClipPrecision   = 0;
f.lfQuality         = DEFAULT_QUALITY;
f.lfPitchAndFamily  = 0;

if ( PRIMARYLANGID(GetUserDefaultLangID())==LANG_JAPANESE )
    {
    f.lfCharSet = SHIFTJIS_CHARSET;
    lstrcpy(f.lfFaceName,TEXT("MS ����"));
    }
else
    {
    f.lfCharSet = ANSI_CHARSET;
    lstrcpy( f.lfFaceName, TEXT("Arial") );
    }

MyFont = CreateFontIndirect( &f );
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
    s = ini.get_string();
    StringTable.set_language( s );
    }

/*
-----------------------------------------------------
Read my file first so I can override the common names
----------------------------------------------------- */
s.get_stringtable_name( EditPartResourceFile );
StringTable.read( s );

s.get_stringtable_name( ChooseWireResourceFile );
StringTable.append( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );

StaticsFont = FontList.get_handle( StringTable.fontclass() );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( int cmd_show )
{
COMPUTER_CLASS c;
INI_CLASS      ini;
NAME_CLASS     s;
TCHAR * cp;
WNDCLASS wc;
RECT r;
int  width;
int  height;
int  i;
int  oldtab;

read_machines_and_parts();
if ( machine_count(c.whoami()) < 1 )
    IsOfficeWorkstation = TRUE;
EboxBackgroundColor = (COLORREF) GetSysColor( COLOR_WINDOW) ;
EboxBackgroundBrush = CreateSolidBrush( EboxBackgroundColor );

load_resources();

HaveSureTrakControl = boolean_from_ini( VisiTrakIniFile, ConfigSection, HaveSureTrakKey );
HavePressureControl = boolean_from_ini( VisiTrakIniFile, ConfigSection, HavePressureControlKey );
WorkstationChangeDisable = boolean_from_ini( VisiTrakIniFile, ConfigSection, WorkstationChangeDisableKey );

/*
-----------------------------------------------------------
Default ShowSetupSheet to TRUE so I don't break old systems
----------------------------------------------------------- */
ShowSetupSheet      = TRUE;
s.get_local_ini_file_name( EditPartIniFile );
ini.set_file( s.text() );
if ( ini.exists() )
    {
    ini.set_section( ConfigSection );
    if ( ini.find(ShowSetupSheetKey) )
        ShowSetupSheet = ini.get_boolean();
    }

cp = get_ini_string(EditPartIniFile, ConfigSection, VelocityChangesLevelKey );
if ( !unknown(cp) )
    VelocityChangesLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, SaveChangesLevelKey );
if ( !unknown(cp) )
    SaveChangesLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditOtherComputerKey );
if ( !unknown(cp) )
    EditOtherComputerLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, ChangeExternalShotNameLevelKey );
if ( !unknown(cp) )
    ChangeExternalShotNameLevel = (short) asctoint32( cp );
else
    ChangeExternalShotNameLevel = highest_password_level();

cp = get_ini_string(EditPartIniFile, ConfigSection, ChangeExternalShotNameLevelKey );
if ( !unknown(cp) )
    ChangeExternalShotNameLevel = (short) asctoint32( cp );
else
    ChangeExternalShotNameLevel = highest_password_level();

cp = get_ini_string(EditPartIniFile, ConfigSection, ChangeShotNameLevelKey );
if ( !unknown(cp) )
    ChangeShotNameLevel = (short) asctoint32( cp );
else
    ChangeShotNameLevel = highest_password_level();

cp = get_ini_string(EditPartIniFile, ConfigSection, ChangeShotResetLevelKey );
if ( !unknown(cp) )
    ChangeShotResetLevel = (short) asctoint32( cp );
else
    ChangeShotResetLevel = highest_password_level();

cp = get_ini_string(EditPartIniFile, ConfigSection, EditOtherCurrentPartKey );
if ( !unknown(cp) )
    EditOtherCurrentPartLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditOtherSuretrakKey );
if ( !unknown(cp) )
    EditOtherSuretrakLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditOtherCurrentSuretrakKey );
if ( !unknown(cp) )
    EditOtherCurrentSuretrakLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditPressureSetupLevelKey );
if ( !unknown(cp) )
    EditPressureSetupLevel = (short) asctoint32( cp );

cp = get_ini_string(EditPartIniFile, ConfigSection, EditPressureProfileLevelKey );
if ( !unknown(cp) )
    EditPressureProfileLevel = (short) asctoint32( cp );

HideSureTrakLimitSwitchWires = boolean_from_ini( EditPartIniFile, ConfigSection, HideSureTrakLimitSwitchWiresKey );

s.get_local_ini_file_name( EmachineIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(EditMaOnOtherCoKey) )
    EditMaOnOtherCoLevel = (short) ini.get_int();

RestrictedPartEntry.get();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MAKEINTRESOURCE(EDITPART_ICON) );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

wc.lpszClassName = SureTrakPlotWindowClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) suretrak_plot_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

register_choose_wire_control();

cp = StringTable.find( TEXT("EDITPART_CAPTION") );

MainWindow = CreateWindow(
    MyClassName,
    cp,
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    0, 0,                             // X,y
    640, 290,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("MAIN_DIALOG"),
    MainWindow,
    (DLGPROC) MainDialogProc );

ANALOG_SENSOR::get();
create_myfont();
units_startup();
create_dialogs();

ParamDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("PARAMETER_DIALOG"),
    MainWindow,
    (DLGPROC) ParamDialogProc );

/*
--------------------------------------------------------------------------
Make the suretrak plot window the same height as the parameter edit window
-------------------------------------------------------------------------- */
GetWindowRect( ParamDialogWindow, &r );
height = r.bottom - r.top;

/*
-------------------------------------
And the same width as the main window
------------------------------------- */
GetWindowRect( MainWindow, &r );
width = r.right - r.left;

cp = resource_string( SURETRAK_PLOT_WINDOW_TITLE_STRING );

CreateWindowEx(
    WS_EX_TOOLWINDOW,
    SureTrakPlotWindowClassName,
    cp,
    WS_POPUP | WS_DLGFRAME,
    0, 0,                             // X,y
    width, height,
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );

ShowWindow( ParamDialogWindow,  SW_HIDE );

StartupTab = BASIC_SETUP_TAB;
if ( HaveSureTrakControl )
    {
    oldtab = asctoint32( get_ini_string(EditPartIniFile, ConfigSection, CurrentTabKey) );
    if ( oldtab == SURETRAK_PROFILE_TAB || oldtab == PRESSURE_CONTROL_TAB )
        StartupTab = oldtab;
    }

i = asctoint32( get_ini_string(EditPartIniFile, ConfigSection, ParameterTabEditLevelKey) );
if ( i >= 0 )
    WindowInfo[PARAMETER_LIMITS_TAB].password_level = i;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
if ( MyFont )
    {
    DeleteObject( MyFont );
    MyFont = 0;
    }

if ( StaticsFont != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( StaticsFont );
    StaticsFont = INVALID_FONT_HANDLE;
    }

if ( EboxBackgroundBrush )
    {
    DeleteObject( EboxBackgroundBrush );
    EboxBackgroundBrush = 0;
    }

cleanup_machines_and_parts();
unregister_for_event( DDE_ACTION_TOPIC, PLOT_MIN_INDEX   );
unregister_for_event( DDE_CONFIG_TOPIC, PW_LEVEL_INDEX   );
unregister_for_event( DDE_CONFIG_TOPIC, MACH_SETUP_INDEX );

put_ini_string( EditPartIniFile, ConfigSection, CurrentTabKey, int32toasc(CurrentDialogNumber) );
put_ini_string( EditPartIniFile, ConfigSection, LastMachineKey, CurrentPart.machine.text() );
put_ini_string( EditPartIniFile, ConfigSection, LastPartKey,    CurrentPart.name.text() );
}

/***********************************************************************
*                     CHECK_FOR_PREVIOUS_INSTANCE                      *
* If there is a previous instance, let that instance handle the        *
* HMI command (if there is one).                                       *
***********************************************************************/
BOOLEAN check_for_previous_instance( STRING_CLASS & cmd_line )
{
HWND w;
FILE_CLASS f;
NAME_CLASS s;

w = FindWindow( MyClassName, 0 );
if ( w )
    {
    /*
    ------------------------------------
    Save the command line to a temp file
    ------------------------------------ */
    if ( HMIMessageType != HMI_NO_MESSAGE )
        {
        s.get_exe_dir_file_name( EditPartCommandLineFile );
        if ( f.open_for_write(s) )
            {
            f.writeline( cmd_line );
            f.close();
            }
        /*
        ----------------------------------------------------------------------
        Send a message to the existing instance telling him of the new command
        ---------------------------------------------------------------------- */
        PostMessage( w, WM_NEW_HMI_COMMAND, 0, 0 );
        }
    else
        {
        /*
        ----------------------------------------
        Only do this if it is not an HMI command
        ---------------------------------------- */
        ShowWindow( w, SW_SHOWNORMAL );
        SetForegroundWindow( w );
        }
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
COMPUTER_CLASS c;
MSG  msg;
BOOL status;
STRING_CLASS s;

MainInstance = this_instance;

c.startup();
s.get_ascii( cmd_line );
check_for_hmi_message( s );
if ( check_for_previous_instance(s) )
    return 0;

InitCommonControls();

LoadingSureTrakSteps = TRUE;
init( cmd_show );
LoadingSureTrakSteps = FALSE;
position_main_window();

AccelHandle = LoadAccelerators( this_instance, AccelName );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status )   //  && MainDialogWindow )
        status = TranslateAccelerator( MainWindow, AccelHandle, &msg );

    if ( !status && MainDialogWindow )
        status = IsDialogMessage( MainDialogWindow, &msg );

    if ( !status && AddPartDialogWindow )
        status = IsDialogMessage( AddPartDialogWindow, &msg );

    if ( !status && CopyToDialogWindow )
        status = IsDialogMessage( CopyToDialogWindow, &msg );

    if ( !status && CopyMultipleDialogWindow )
        status = IsDialogMessage( CopyMultipleDialogWindow, &msg );

    if ( !status && AddMarksDialogWindow )
        status = IsDialogMessage( AddMarksDialogWindow, &msg );

    if ( !status && ParamDialogWindow )
        status = IsDialogMessage( ParamDialogWindow, &msg );

    if ( !status )
        {
        if ( CurrentDialogNumber != NO_TAB )
            {
            status = IsDialogMessage( WindowInfo[CurrentDialogNumber].dialogwindow, &msg );
            }
        }

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        }
    }

shutdown();
client_dde_shutdown();
ANALOG_SENSOR::shutdown();
units_shutdown();
return(msg.wParam);
}
