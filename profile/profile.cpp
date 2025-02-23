#include <windows.h>
#include <VersionHelpers.h>
#include <commctrl.h>
#include <ddeml.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\autoback.h"
#include "..\include\catclass.h"
#include "..\include\chaxis.h"
#include "..\include\color.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\evclass.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\ftanalog.h"
#include "..\include\ft.h"
#include "..\include\computer.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\ftcalc.h"
#include "..\include\plookup.h"
#include "..\include\alarmsum.h"
#include "..\include\marks.h"
#include "..\include\rectclas.h"
#include "..\include\setpoint.h"
#include "..\include\textlen.h"
#include "..\include\textlist.h"
#include "..\include\structs.h"
#include "..\include\strarray.h"
#include "..\include\stringtable.h"
#include "..\include\param_index_class.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"
#include "..\include\ftii.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\v5help.h"
#include "..\include\vdbclass.h"
#include "..\include\wclass.h"
#include "..\include\workclas.h"
#include "..\include\menuctrl.h"
#include "..\include\popctrl.h"
#include "..\include\statics.h"

#define _MAIN_
#include "..\include\chars.h"
#include "..\include\events.h"

#include "gridclas.h"
#include "rtmlist.h"
#include "resource.h"
#include "extern.h"

const int NO_DISPLAY        = 0;
const int POS_TIME_DISPLAY  = 1;
const int POS_ONLY_DISPLAY  = 2;
const int TIME_ONLY_DISPLAY = 3;    /* Plot.cpp knows this too */

#define REAL_TIME_MAIN_MENU    118
#define REAL_TIME_MENU         103
#define OVERLAY_MENU           101
#define CORNER_PARAMETERS_MENU 104
#define ABOUT_BOX_MENU         105
#define WHATS_NEW_MENU         106

#define DISPLAY_CONFIGURATION_MENU 605
#define COLOR_DEFINITION_MENU      606
#define CHOOSE_RT_MACHINES_MENU    608

#define CHOOSE_PART_MENU          113
#define CHOOSE_SHOT_MENU          109
#define EXIT_MENU                 108
#define MINIMIZE_MENU             114
#define SAVE_MENU                 115
#define SAVE_AS_MASTER_MENU       116
#define PRINT_MENU                117
#define MULTIPLE_SHOT_PRINT_MENU  119
#define CLEAR_BACKUP_LIST_MENU    120
#define ADD_TO_BACKUP_LIST_MENU   121
#define MACHINE_LIST_MENU         122
#define CHOOSE_LANGUAGE_MENU      123

#define POS_TIME_MENU             501
#define POS_ONLY_MENU             502
#define TIME_ONLY_MENU            503
#define LAST_50_SCREEN_MENU       504
#define STD_SCREEN_MENU           505
#define VIEW_REFERENCE_TRACE_MENU 506
#define VIEW_PARAMETERS_MENU      507
#define BOX_SCREEN_MENU           508
#define TREND_SCREEN_MENU         509
#define CHOOSE_TREND_PARAMS_MENU  510
#define TREND_CONFIG_MENU         511
#define TREND_REALTIME_MENU       512
#define PROFILE_SCREEN_MENU       513
#define DOWNBAR_SCREEN_MENU       514
#define DOWNBAR_CONFIG_MENU       515
#define TREND_ALLSHOTS_MENU       516
#define STAT_SUMMARY_MENU         517
#define NO_STAT_SUMMARY_MENU      518
#define STATUS_SCREEN_MENU        519
#define EDIT_STATUS_SCREEN_MENU   520
#define SAVE_STATUS_SCREEN_MENU   521
#define XDCR_MENU                 522
#define FONT_STATUS_SCREEN_MENU   523
#define SET_TSL_MENU              524
#define BOX_SCREEN_SET_FONT_MENU  525
#define DAY_PARAM_SCREEN_MENU     526

#define PART_EDITOR_MENU          701
#define MACHINE_EDITOR_MENU       702
#define BOARD_MONITOR_MENU        703
#define MONITOR_CONFIG_MENU       704
#define SENSORS_MENU              705
#define BACKUP_AND_RESTORE_MENU   706
#define SETUPS_MENU               707
#define DOWNTIME_MENU             708
#define EDITDOWN_MENU             709
#define SURETRAK_CONTROL_MENU     710
#define VISIEDIT_MENU             711
#define EDIT_SHIFTS_MENU          712
#define STANDARD_DOWNTIMES_MENU   713
#define CHOOSE_DOWNTIME_MACHINE_MENU 714
#define SHIFT_REPORT_MENU         715           /* Consecutive run_tool exes */
#define CONVERT_UNITS_MENU        716

#define LOW_RUN_EXTERNAL_PROGRAM_ID  750
#define HIGH_RUN_EXTERNAL_PROGRAM_ID 780

#define OPERATOR_SIGNON_MENU      800
#define EXPORT_SHOTS_MENU         801
#define ALARM_SUMMARY_MENU        803
#define PASSWORDS_MENU            804
#define RESTORE_PASSWORD_MENU     805
#define ALARM_LOG_SETUP_MENU      806
#define EXPORT_ALARMS_MENU        807
#define CSV_SHOT_FILE_MENU        808
#define DOWNTIME_POPUP_MENU       809
#define RESTORE_FOCUS_TO_HMI_MENU 810
#define HELP_MENU                 900
#define FT2_HELP_MENU             901

#define DAY_PARAM_EB_ID          1001
#define DAY_PARAM_LB_ID          1002

const COLORREF BlackColor = RGB(   0,   0,   0 );
const COLORREF WhiteColor = RGB( 255, 255, 255 );

/*
-----------------
WM_NEW_DATA types
----------------- */
WPARAM LOAD_MAIN_MENU_REQ        = 1;
WPARAM SCREEN_REFRESH_REQ        = 2;
WPARAM PASSWORD_DIALOG_REQ       = 3;
WPARAM EDIT_PASSWORDS_DIALOG_REQ = 4;
WPARAM CO_OPTIONS_DIALOG_REQ     = 5;

/*
-----------------
Zoom box controls
----------------- */
#define POS_TEXT_BOX      110
#define TIME_TEXT_BOX     120
#define VELOCITY_TEXT_BOX 130
#define ROD_TEXT_BOX      140
#define HEAD_TEXT_BOX     150
#define DIFF_TEXT_BOX     160
#define CHAN_4_TEXT_BOX   170
#define START_BOX_OFFSET    0
#define END_BOX_OFFSET      1
#define DELTA_BOX_OFFSET    2

/*
-----------------------
Machine list sort types
----------------------- */
const int ML_SORT_BY_COMPUTER = 0;
const int ML_SORT_BY_MACHINE  = 1;

#ifdef UNICODE
static UINT ClipFormat = CF_UNICODETEXT;
#else
static UINT ClipFormat = CF_TEXT;
#endif

const UINT PASSWORD_LEVEL_TIMEOUT_TIMER_NUMBER = 2;

/*
-------------------------------
Display corner parameter titles
------------------------------- */
STRING_CLASS ParameterListTitle[NOF_PARAMETER_LIST_TITLES];

/*
------------------------------------
Display corner parameter limit types
------------------------------------ */
int LowLimitType  = ALARM_MIN;
int HighLimitType = ALARM_MAX;

/*
---------------------------------
Display corner parameter controls
--------------------------------- */
#define FIRST_CORNER_PARAM_NAME         110
#define FIRST_CORNER_PARAM_VALUE        130
#define FIRST_CORNER_PARAM_UNITS        150
#define DISPLAY_PARAMETER_LISTBOX       101

#define LEFT_AXIS_TYPE   1
#define RIGHT_AXIS_TYPE  2

#define POS_BASED_X_TICS  7
#define TIME_BASED_X_TICS 4
#define ANALOG_Y_TICS     4
#define COUNT_Y_TICS      4

HACCEL  AccelHandle;
TCHAR   AccelName[] = TEXT("PROFILE_HOT_KEYS");

HWND    MainPlotWindow              = 0;
HWND    MenuHelpWindow              = 0;
HWND    ChospartWindow              = 0;
HWND    ChosshotWindow              = 0;
HWND    ParameterWindow             = 0; /* Corner Parameters */
HWND    ZoomBoxWindow               = 0;
HWND    SmallZoomBoxWindow          = 0;
HWND    CornerParamDialogWindow     = 0; /* Corner Parameter Setup */
HWND    ParamHistoryWindow          = 0;
HWND    ViewParametersDialogWindow  = 0;
HWND    ColorDefinitionDialogWindow = 0;
HWND    AlarmSummaryDialogWindow    = 0;
HWND    LoadFromNetworkDialogWindow = 0;

MENU_CONTROL_ENTRY MainMenu;
POPUP_CONTROL_ENTRY PopupMenu;

bool    IsHPM = false;

BOOLEAN ReadyForZoom = FALSE;

GRID_CLASS Grid;
BOOLEAN    GridIsVisible = FALSE;

/*
-----------------------------
Number of columns in the grid
----------------------------- */
int NofColumns = NOF_STATSUM_STRINGS;

int     CurrentDisplay = POS_TIME_DISPLAY;
int     LastPosDisplay = POS_TIME_DISPLAY; /* For Display Toggle */

HDC     MyDc           = NULL;
int     NofDcOwners    = 0;
RECT    MainWindowRect;
RECT    PlotWindowRect;
RECT    PrinterMargin;
RECT    RealTimeButtonRect = { 0, 0, 0, 0};

TCHAR   MyClassName[]               = TEXT( "Profile" );
TCHAR   MyWindowTitle[]             = TEXT( "Plot" );

TCHAR   CommonResourceFile[]        = TEXT( "common" );
TCHAR   MainMenuWindowName[]        = TEXT( "MainMenuWindow" );
TCHAR   MainPlotWindowClass[]       = TEXT( "MainPlotClass" );
TCHAR   ParameterWindowClassName[]  = TEXT( "ParameterWindowClass" );
TCHAR   PopupMenuWindowName[]       = TEXT( "MainPopupWindow" );
TCHAR   ProfileMenuFile[]           = TEXT( "profile.mnu" );
TCHAR   ProfileResourceFile[]       = TEXT( "profile" );
TCHAR   XdcrWindowClass[]           = TEXT( "XdcrPlotClass" );

TCHAR   DefaultArialFontName[]      = TEXT( "Arial" );
TCHAR   DefaultArialBoldFontName[]  = TEXT( "Arial Bold" );
TCHAR   DefaultTimesRomanFontName[] = TEXT( "Times New Roman" );

STRING_CLASS ArialFontName;
STRING_CLASS ArialBoldFontName;
STRING_CLASS TimesRomanFontName;

STRING_CLASS NewSetupMachine;

TCHAR   MainHelpFileName[]  = TEXT( "v5help.hlp" );
TCHAR   CpkHelpFileName[]   = TEXT( "cpk.hlp" );
TCHAR   ToolsHelpFileName[] = TEXT( "toolmenu.hlp" );

int     NofCornerParameters;
int     CornerParameters[MAX_PARMS];

short   MyScreenType      = STANDARD_SCREEN_TYPE;

static short StandardProfileScreenType = ZABISH_SCREEN_TYPE;

/*
------------------------------------
Cursor line and zoom state variables
------------------------------------ */
BOOLEAN HaveLeftCursorLine  = FALSE;
BOOLEAN HaveCursorLine      = FALSE;
BOOLEAN Zooming             = FALSE;
BOOLEAN Printing            = FALSE;

static BOOLEAN WasRealtimeBeforeZoom      = FALSE;
static BOOLEAN WasSingleMachineBeforeZoom = FALSE;
static int     DisplayBeforeZoom          = POS_TIME_DISPLAY;
static BOOLEAN NeedToUpdateZoomLabels     = FALSE;

static BOOLEAN HaveMonitorProgram = TRUE;
static BOOLEAN HaveSureTrakControl = FALSE;
static BOOLEAN UseDowntimeEntry    = FALSE;

BOOLEAN QuickPrint = FALSE;

static int CursorStartY;
static int CursorEndY;
static int LeftCursorX;
static int CursorX;

static int LeftZoomIndex = 0;
static int LeftZoomAxis  = NO_AXIS;
static int ZoomIndex     = 0;
static int ZoomAxis      = NO_AXIS;
const  int NofZoomAnalogChannels = 4;
static BITSETYPE ZoomAnalogType[NofZoomAnalogChannels] = {0, 0, 0, 0};

const int AnalogTbox[NofZoomAnalogChannels] =
    {
    ROD_TEXT_BOX,
    HEAD_TEXT_BOX,
    DIFF_TEXT_BOX,
    CHAN_4_TEXT_BOX
    };

/*
-----------------------
Alarm summary variables
----------------------- */
static SYSTEMTIME FirstAlarmDate;
static int32      FirstAlarmShift;
static SYSTEMTIME CurrentAlarmDate;
static int32      CurrentAlarmShift;
static SYSTEMTIME LastAlarmDate;
static int32      LastAlarmShift;

/*
-----------------------
Status screen variables
----------------------- */
BOOLEAN Editing_Status_Screen = FALSE;

/*
--------------------------
Day param screen variables
-------------------------- */
static POINT         DayParamDatePositon;
static INT         * DayParamTitlePosition = 0;
static int           NofDayParamTitles     = 0;
static HFONT         DayParamFontHandle    = INVALID_FONT_HANDLE;
static FONT_CLASS    DayParamFont;
static int           DayParamFontHeight = 0;
static int           DayParamFontWidth  = 0;
static LISTBOX_CLASS DayParamLb;
static HWND          DayParamEb;
static WNDPROC       OldDayParamEditProc = 0;
static STRING_CLASS  DayParamTitle;
static STRING_CLASS  DayParamCurrentEntry;

/*
------------------------
Downtime entry variables
------------------------ */
static HWND DowntimeEntryDialog;
static HWND DowntimeEntryEbox;
static HWND DowntimeEntryOkButton;
static LISTBOX_CLASS DowntimeEntryLb;
static bool DowntimeEntryIsChanging = false;
static WNDPROC OldEditProc;
static STRING_CLASS DowntimeEntryMachine;
static SYSTEMTIME   DowntimeEntrySt;

#define PARAMETER_INFO_BOX 1
#define ZOOM_INFO_BOX      2
#define XDCR_INFO_BOX      3

static int InfoBoxType;
static BOOL NeedCornerParamUpdate;
static BOOL PTestIsRunning = FALSE;
static HWND PTestWindow    = 0;

static BOOLEAN ShowWorker = FALSE;
static STRING_CLASS CurrentWorker;

static HFONT PrinterFont = 0;

static TCHAR PercentString[]    = TEXT( "%" );
static TCHAR TabString[]        = TEXT( "\t" );
static TCHAR TwoSpacesString[]  = TEXT( "  " );
static TCHAR HmiSection[]       = TEXT( "HMI" );
static TCHAR ClassKey[]         = TEXT( "ClassName" );

BOOLEAN       begin_box_display();
BOOLEAN       begin_downbar_display();
BOOLEAN       begin_status_display();
BOOLEAN       begin_trend_display();
BOOLEAN       choose_box_font();
void          choose_status_screen_font();
void          downbar_left_button_up( POINT & p );
void          do_downbar_button( int id );
void          do_new_dtr_config();
void          do_new_tc_config( WPARAM force_change );
void          end_box_display();
void          end_downbar_display();
void          end_status_display();
void          end_trend_display();
void          box_display_shutdown();
BOOLEAN       box_display_startup();
void          mouse_to_trend( int32 x, int32 y );
void          paint_box_display( PAINTSTRUCT & ps );
void          paint_downbar_display();
void          paint_status_display( PAINTSTRUCT & ps );
void          paint_trend_display( const RECT & ur );
void          resource_message_box( UINT msg_id, UINT title_id );
void          box_screen_down_data_change( STRING_CLASS & machine_name, TCHAR * category, TCHAR * subcategory );
void          box_screen_left_button_up( POINT & p );
void          box_screen_new_machine_status( STRING_CLASS & machine_name, BOARD_DATA new_status );
void          box_screen_new_monitor_setup( STRING_CLASS & machine_name );
void          box_screen_new_shot( STRING_CLASS & machine_name );
void          status_screen_down_data_change( STRING_CLASS & machine_name, TCHAR * category, TCHAR * subcategory );
void          status_screen_left_button_up( POINT & p );
void          status_screen_new_machine_status( STRING_CLASS & machine_name, BOARD_DATA new_status );
void          status_screen_new_shot( STRING_CLASS & machine_name );
void          status_screen_timer_update();
void          save_status_config();
void          toggle_pressure_display_type();
void          trend_allshots();
void          trend_escape();
void          trend_screen_new_shot();
void          trend_left_button_up( POINT & p );
void          trend_screen_reset();
void          trend_realtime();

void    load_profile();
void    load_profile( PROFILE_NAME_ENTRY * p );
BOOLEAN load_reference_trace();
void init_global_curve_types();

void read_corner_parameters( int * dest, int & n );
void save_corner_parameters( int * sorc, int n );

BOOL CALLBACK chospart_proc(                     HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK chosshot_proc(                     HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK co_options_dialog_proc(            HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK color_definition_dialog_proc(      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK corner_param_dialog_proc(          HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK display_configuration_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK downtime_report_setup_proc(        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK edit_operators_dialog_proc(        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK edit_passwords_proc(               HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK edit_shifts_dialog_proc(           HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK enter_password_proc(               HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK export_alarms_dialog_proc(         HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK export_shots_proc(                 HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK print_multiple_shots_proc(         HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK realtime_machine_list_proc(        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK trend_config_dialog_proc(          HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK view_parameters_dialog_proc(       HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK parameter_window_proc(             HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT CALLBACK xdcr_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

int CALLBACK ParamHistoryProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void position_parameter_window();
void position_param_history_window();
void position_view_parameters_window();
void toggle_profile_save_flags();
void save_as_master_trace();
int     get_alarm_state();
BOOLEAN is_alarm( short parameter_index, float value );
BOOLEAN is_warning( short parameter_index, float value );
void fill_view_parameters_listbox();
void load_parameter_list_titles();

int  default_password_level();
void do_new_control_limits();
void print_alarm_summary();
void restore_colors();
void save_colors();
void set_default_password_level( bool need_log );
void set_smooth( int32 n, float factor );
void update_screen_type( short new_screen_type );

BOOLEAN append_overlay_shot();
BOOLEAN write_olmsted_file( STRING_CLASS & destfile, STRING_CLASS & sorcfile );
BOOLEAN write_counts_file(  STRING_CLASS & destfile, STRING_CLASS & sorcfile );

/***********************************************************************
*                              REGION_ENTRY                            *
***********************************************************************/
REGION_ENTRY::REGION_ENTRY()
{
rect.left   = 0;
rect.top    = 0;
rect.right  = 0;
rect.bottom = 0;
is_visible  = FALSE;
}

/***********************************************************************
*                              ~REGION_ENTRY                            *
***********************************************************************/
REGION_ENTRY::~REGION_ENTRY()
{
}

/***********************************************************************
*                              REGION_ENTRY                            *
*                                    =                                 *
***********************************************************************/
void REGION_ENTRY::operator=( const REGION_ENTRY & sorc )
{
rect.left   = sorc.rect.left;
rect.top    = sorc.rect.top;
rect.right  = sorc.rect.right;
rect.bottom = sorc.rect.bottom;
is_visible  = sorc.is_visible;
}

/***********************************************************************
*                              OVERLAYING                              *
***********************************************************************/
bool overlaying()
{
if ( !OverlayMenuState )
    return false;

if ( IsHPM )
    return  (CurrentDisplay == TIME_ONLY_DISPLAY);

//if ( CurrentDisplay == POS_ONLY_DISPLAY )  Lawyer code
  //  return false;

return true;
}

/***********************************************************************
*                          VIEWING_REFERENCE                           *
***********************************************************************/
bool viewing_reference()
{
if ( !ViewReferenceMenuState )
    return false;

if ( IsHPM )
    return  (CurrentDisplay == TIME_ONLY_DISPLAY);

//if ( CurrentDisplay == POS_ONLY_DISPLAY ) Lawyer code
  //  return false;

return true;
}

/***********************************************************************
*                              DRAW_LINE                               *
***********************************************************************/
static void draw_line( int x1, int y1, int x2, int y2 )
{
MoveToEx( MyDc, x1, y1, 0 );
LineTo( MyDc, x2, y2 );
}

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

RESOURCE_TEXT_ENTRY rsn[] = {
    { 1, TEXT("LISTBOX_FULL_MESSAGE") },
    { 2, TEXT("CANT_DO_THAT_STRING") },
    { 3, TEXT("HEAD_PRESSURE_LABEL_STRING") },
    { 4, TEXT("ROD_PRESSURE_LABEL_STRING") },
    { 5, TEXT("DIFF_PRESSURE_LABEL_STRING") },
    { 6, TEXT("DISTANCE_TYPE_STRING") },
    { 7, TEXT("POSITION_TYPE_STRING") },
    { 8, TEXT("PRESSURE_TYPE_STRING") },
    { 10, TEXT("TIME_TYPE_STRING") },
    { 12, TEXT("VELOCITY_TYPE_STRING") },
    { 36, TEXT("CHANNEL_TYPE_STRING") },
    { 16, TEXT("CHANNEL_1_LABEL_STRING") },
    { 17, TEXT("CHANNEL_2_LABEL_STRING") },
    { 18, TEXT("CHANNEL_3_LABEL_STRING") },
    { 19, TEXT("CHANNEL_4_LABEL_STRING") },
    { 103, TEXT("MAX_OVERLAY_REACHED_STRING") },
    { 104, TEXT("DEL_SOME_FIRST_STRING") },
    { 506, TEXT("EMPTY_LIST") },
    { 507, TEXT("NO_MACHINES_HERE") },
    { 508, TEXT("OPERATOR_NOT_FOUND") },
    { 509, TEXT("SHOT_NUMBER_STRING") },
    { 510, TEXT("DATE_NAME_STRING") },
    { 511, TEXT("TIME_NAME_STRING") },
    { 512, TEXT("NO_MEMORY_STRING") },
    { 513, TEXT("PRINTER_LIST_STRING") },
    { 514, TEXT("UNITS_TITLE_STRING") },
    { 515, TEXT("REALTIME_ON_MENU_STRING") },
    { 516, TEXT("NO_ALARM_STRING") },
    { 517, TEXT("WARNING_STRING") },
    { 518, TEXT("ALARM_STRING") },
    { 519, TEXT("NO_DOWN_STRING") },
    { 520, TEXT("HUMAN_DOWN_STRING") },
    { 521, TEXT("AUTO_DOWN_STRING") },
    { 522, TEXT("PROG_DOWN_STRING") },
    { 523, TEXT("PROG_UP_STRING") },
    { 524, TEXT("MACH_UP_STRING") },
    { 525, TEXT("NO_MONITOR_STRING") },
    { 526, TEXT("BOX_SCREEN_MENU_STRING") },
    { 527, TEXT("MACHINE_TT_STRING") },
    { 528, TEXT("PART_TT_STRING") },
    { 529, TEXT("SHOT_TT_STRING") },
    { 530, TEXT("DATE_TT_STRING") },
    { 531, TEXT("NO_DOWN_TT_STRING") },
    { 532, TEXT("HUMAN_DOWN_TT_STRING") },
    { 533, TEXT("AUTO_DOWN_TT_STRING") },
    { 534, TEXT("PROG_DOWN_TT_STRING") },
    { 535, TEXT("PROG_UP_TT_STRING") },
    { 536, TEXT("MACH_UP_TT_STRING") },
    { 537, TEXT("NO_MONITOR_TT_STRING") },
    { 538, TEXT("TOTAL_ALARM_SHOTS_STRING") },
    { 539, TEXT("TOTAL_WARNING_SHOTS_STRING") },
    { 540, TEXT("LOW_ALARM_STRING") },
    { 541, TEXT("HIGH_ALARM_STRING") },
    { 542, TEXT("LOW_WARNING_STRING") },
    { 543, TEXT("HIGH_WARNING_STRING") },
    { 544, TEXT("NEXT_SHIFT_STRING") },
    { 545, TEXT("PREV_SHIFT_STRING") },
    { 546, TEXT("NEXT_DAY_STRING") },
    { 547, TEXT("PREV_DAY_STRING") },
    { 548, TEXT("NEXT_STRING") },
    { 549, TEXT("PREV_STRING") },
    { 550, TEXT("NO_ALARM_DATA_STRING") },
    { 551, TEXT("NOTHING_TO_DISPLAY_STRING") },
    { 552, TEXT("SHIFT_STRING") },
    { 553, TEXT("PARAMETER_STRING") },
    { 554, TEXT("LOW_LIMIT_STRING") },
    { 555, TEXT("HIGH_LIMIT_STRING") },
    { 556, TEXT("PARAMETER_VALUE_STRING") },
    { 557, TEXT("ALARM_TT_STRING") },
    { 558, TEXT("REALTIME_OFF_MENU_STRING") },
    { 559, TEXT("REALTIME_SINGLE_MENU_STRING") },
    { 560, TEXT("TIME_AXIS_LABEL_STRING") },
    { 561, TEXT("TREND_ALLSHOTS_MENU_STRING") },
    { 562, TEXT("TREND_CONFIG_MENU_STRING") },
    { 563, TEXT("TREND_SCREEN_MENU_STRING") },
    { 564, TEXT("NO_GRAPHS_FOUND_STRING") },
    { 565, TEXT("TREND_REALTIME_MENU_STRING") },
    { 566, TEXT("N_EQUALS_STRING") },
    { 567, TEXT("END_PARENS_STRING") },
    { 568, TEXT("TOO_MANY_GRAPHS_STRING") },
    { 569, TEXT("TIMEOUT_STRING") },
    { 570, TEXT("DOWNBAR_TITLE_STRING") },
    { 571, TEXT("TOTAL_HOURS_STRING") },
    { 572, TEXT("TOTAL_HOURS_UP_STRING") },
    { 573, TEXT("TOTAL_HOURS_DOWN_STRING") },
    { 574, TEXT("MACH_UTILIZATION_STRING") },
    { 575, TEXT("MAX_DOWNTIME_HOURS_STRING") },
    { 576, TEXT("MAX_DOWNTIME_PERCENT_STRING") },
    { 577, TEXT("DOWNTIME_FROM_STRING") },
    { 578, TEXT("DOWNTIME_TO_STRING") },
    { 579, TEXT("HOURS_STRING") },
    { 580, TEXT("CATEGORY_TITLE_STRING") },
    { 581, TEXT("DOWNBAR_CONFIG_MENU_STRING") },
    { 582, TEXT("NO_DOWNTIME_DATA_STRING") },
    { 583, TEXT("CHOOSE_PART_MENU_HELP") },
    { 584, TEXT("CHOOSE_SHOT_MENU_HELP") },
    { 585, TEXT("EXIT_MENU_HELP") },
    { 586, TEXT("MINIMIZE_MENU_HELP") },
    { 587, TEXT("SAVE_MENU_HELP") },
    { 588, TEXT("SAVE_AS_MASTER_MENU_HELP") },
    { 589, TEXT("PRINT_MENU_HELP") },
    { 590, TEXT("POS_TIME_MENU_HELP") },
    { 591, TEXT("POS_ONLY_MENU_HELP") },
    { 592, TEXT("TIME_ONLY_MENU_HELP") },
    { 593, TEXT("LAST_50_SCREEN_MENU_HELP") },
    { 594, TEXT("STD_SCREEN_MENU_HELP") },
    { 595, TEXT("VIEW_REFERENCE_TRACE_MENU_HELP") },
    { 596, TEXT("VIEW_PARAMETERS_MENU_HELP") },
    { 597, TEXT("BOX_SCREEN_MENU_HELP") },
    { 598, TEXT("TREND_SCREEN_MENU_HELP") },
    { 602, TEXT("PROFILE_SCREEN_MENU_HELP") },
    { 603, TEXT("DOWNBAR_SCREEN_MENU_HELP") },
    { 605, TEXT("PART_EDITOR_MENU_HELP") },
    { 606, TEXT("MACHINE_EDITOR_MENU_HELP") },
    { 621, TEXT("BOARD_MONITOR_MENU_HELP") },
    { 608, TEXT("MONITOR_CONFIG_MENU_HELP") },
    { 610, TEXT("BACKUP_AND_RESTORE_MENU_HELP") },
    { 611, TEXT("SETUPS_MENU_HELP") },
    { 612, TEXT("DOWNTIME_MENU_HELP") },
    { 613, TEXT("EDITDOWN_MENU_HELP") },
    { 614, TEXT("SURETRAK_CONTROL_MENU_HELP") },
    { 615, TEXT("OPERATOR_SIGNON_MENU_HELP") },
    { 616, TEXT("EXPORT_SHOTS_MENU_HELP") },
    { 618, TEXT("ALARM_SUMMARY_MENU_HELP") },
    { 619, TEXT("PASSWORDS_MENU_HELP") },
    { 620, TEXT("RESTORE_PASSWORD_MENU_HELP") },
    { 622, TEXT("HELP_MENU_HELP") },
    { 623, TEXT("UNZOOM_MENU_HELP") },
    { 624, TEXT("ZOOM_MENU_HELP") },
    { 625, TEXT("DISPLAY_CONFIGURATION_MENU_HELP") },
    { 626, TEXT("COLOR_DEFINITION_MENU_HELP") },
    { 627, TEXT("CHOOSE_RT_MACHINES_MENU_HELP") },
    { 628, TEXT("CORNER_PARAMETERS_MENU_HELP") },
    { 629, TEXT("OVERLAY_MENU_HELP") },
    { 630, TEXT("MACHINE_INTRO_STRING") },
    { 631, TEXT("PART_INTRO_STRING") },
    { 632, TEXT("TOTAL_SHOTS_INTRO_STRING") },
    { 633, TEXT("TOTAL_ALARM_SHOTS_INTRO_STRING") },
    { 634, TEXT("TOTAL_WARNING_SHOTS_INTRO_STRING") },
    { 635, TEXT("QUANTITY_BELOW_STRING") },
    { 636, TEXT("QUANTITY_ABOVE_STRING") },
    { 637, TEXT("CANNOT_COMPLY_STRING") },
    { 638, TEXT("PASSWORD_NOT_FOUND_STRING") },
    { 639, TEXT("MS_TIME_LABEL_STRING") },
    { 640, TEXT("SEC_TIME_LABEL_STRING") },
    { 641, TEXT("SHOT_LABEL_STRING") },
    { 642, TEXT("MULTIPLE_SHOT_PRINT_MENU_HELP") },
    { 643, TEXT("MULTIPLE_PRINT_STRING") },
    { 644, TEXT("PRINTING_COMPLETE_STRING") },
    { 645, TEXT("STAT_N_EQUALS_STRING") },
    { 646, TEXT("ALARM_LIMITS_STRING") },
    { 647, TEXT("ALARM_SHOT_LABEL_STRING") },
    { 648, TEXT("ALARM_DATE_LABEL_STRING") },
    { 649, TEXT("ALARM_TIME_LABEL_STRING") },
    { 650, TEXT("TREND_REALTIME_MENU_HELP") },
    { 651, TEXT("TREND_ALLSHOTS_MENU_HELP") },
    { 652, TEXT("TREND_CONFIG_MENU_HELP") },
    { 653, TEXT("STAT_SUMMARY_MENU_STRING") },
    { 654, TEXT("STAT_SUMMARY_MENU_HELP") },
    { 655, TEXT("NO_STAT_SUMMARY_MENU_STRING") },
    { 656, TEXT("NO_STAT_SUMMARY_MENU_HELP") },
    { 657, TEXT("ZOOM_PRINT_POSITION_STRING") },
    { 658, TEXT("ZOOM_PRINT_TIME_STRING") },
    { 659, TEXT("ZOOM_PRINT_VELOCITY_STRING") },
    { 660, TEXT("ZOOM_PRINT_ROD_PRES_STRING") },
    { 661, TEXT("ZOOM_PRINT_HEAD_PRES_STRING") },
    { 662, TEXT("ZOOM_PRINT_START_STRING") },
    { 663, TEXT("ZOOM_PRINT_END_STRING") },
    { 664, TEXT("ZOOM_PRINT_DELTA_STRING") },
    { 665, TEXT("FIELD_TOO_LONG_STRING") },
    { 666, TEXT("INPUT_ERROR_STRING") },
    { 667, TEXT("NO_DEL_ALL_ROWS_STRING") },
    { 668, TEXT("ATTEMPT_DEL_ALL_ROWS_STRING") },
    { 669, TEXT("NOTHING_SELECTED_STRING") },
    { 670, TEXT("SELECT_ROW_STRING") },
    { 671, TEXT("N_PER_GROUP_STRING") },
    { 672, TEXT("KILLING_ALARMSUM_DATA_STRING") }
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
*                               TEXT_OUT                               *
***********************************************************************/
void text_out( int32 x, int32 y, TCHAR * sorc )
{

TextOut( MyDc, (int) x, (int) y, sorc, lstrlen(sorc) );
}

/***********************************************************************
*                               TEXT_OUT                               *
***********************************************************************/
void text_out( int32 x, int32 y, UINT sorc_id )
{
TCHAR * s;

s = resource_string( sorc_id );
if ( s )
    TextOut( MyDc, (int) x, (int) y, s, lstrlen(s) );
}

/***********************************************************************
*                          PLOTCURVE_CONTAINS                          *
***********************************************************************/
static BOOLEAN plotcurve_contains( int index_to_find )
{
int c;

for ( c=0; c<NofPlotCurves; c++ )
    {
    if ( PlotCurve[c].curve_index == index_to_find )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         PLOT_CURVE_INDEX                             *
***********************************************************************/
static int plot_curve_index( BITSETYPE curve_type )
{
int i;

for ( i=0; i<MAX_CURVES; i++ )
    {
    if ( (Ph.curves[i].type & curve_type) == curve_type )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                         SET_ZOOM_BOX_LABELS                          *
***********************************************************************/
void set_zoom_box_labels( HWND w )
{
int channel;
int c;
int i;
int zi;
BITSETYPE curvetype;
STRING_CLASS s;

NeedToUpdateZoomLabels = FALSE;

zi = 0;
for ( c=0; c<NofPlotCurves; c++ )
    {
    s.null();
    if ( zi >= NofZoomAnalogChannels )
        break;
    i = PlotCurve[c].curve_index;
    channel = Ph.curves[i].ft_channel_number();
    if ( channel > NO_FT_CHANNEL || i == DIFFERENTIAL_CURVE_INDEX )
        {
        curvetype = Ph.curves[i].ft_channel_type();
        if ( channel == CurrentPart.head_pressure_channel )
            curvetype |= HEAD_PRESSURE_CURVE;
        else if ( channel == CurrentPart.rod_pressure_channel )
            curvetype |= ROD_PRESSURE_CURVE;
        else if ( i == DIFFERENTIAL_CURVE_INDEX )
            curvetype |= DIFF_PRESSURE_CURVE;
        ZoomAnalogType[zi] = curvetype;

        if ( curvetype & DIFF_PRESSURE_CURVE )
            {
            s = StringTable.find( TEXT("DIF_PRES_ZOOM_STRING") );
            }
        else if ( IsPressureDisplay )
            {
            if ( curvetype & HEAD_PRESSURE_CURVE )
                s = StringTable.find( TEXT("HEAD_PRES_ZOOM_STRING") );
            else if ( curvetype & ROD_PRESSURE_CURVE )
                s = StringTable.find( TEXT("ROD_PRES_ZOOM_STRING") );
            else if ( curvetype & DIFF_PRESSURE_CURVE )
                s = StringTable.find( TEXT("DIFF_PRES_ZOOM_STRING") );
            }
        if ( s.isempty() )
            {
            if ( channel > NO_FT_CHANNEL )
                channel--;
            s = ChannelAxisLabel[channel];
            }
        s.set_text( w, CHAN_1_ZOOM_TITLE_TBOX+zi );
        if ( i == DIFFERENTIAL_CURVE_INDEX )
            ZoomAnalogType[zi] = DIFF_PRESSURE_CURVE;
        else
            ZoomAnalogType[zi] = Ph.curves[i].ft_channel_type();
        zi++;
        }
    }

while ( zi < NofZoomAnalogChannels )
    {
    ZoomAnalogType[zi] = 0;
    set_text( w, CHAN_1_ZOOM_TITLE_TBOX+zi, EmptyString );
    for ( i=START_BOX_OFFSET; i<DELTA_BOX_OFFSET; i++ )
        set_text( w, AnalogTbox[zi]+i, EmptyString );
    zi++;
    }
}

/***********************************************************************
*                        MY_DESKTOP_CLIENT_RECT                        *
***********************************************************************/
RECT & my_desktop_client_rect()
{
static RECT r;
HWND w;

r.top = r.bottom = r.right = r.left = 0;

if ( IsFullScreen )
    {
    w = GetDesktopWindow();
    if ( w )
        {
        GetClientRect( w, &r );
        }
    }
else
    {
    r = desktop_client_rect();
    }

return r;
}

/***********************************************************************
*                       SHOW_FIXED_AXIS_MESSAGE                        *
*                                                                      *
*    This is called by the paint_me routine so I can use MyDc.         *
*                                                                      *
***********************************************************************/
void show_fixed_axis_message()
{
RECT  r;
HFONT old_font;
STRING_CLASS s;
SIZE         sz;

old_font = (HFONT) SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) );

s = StringTable.find( TEXT("FIXED_AXIS_STRING") );

GetTextExtentPoint32( MyDc, s.text(), s.len(), &sz );

GetClientRect( MainPlotWindow, &r );

r.top+= 2;
r.bottom = r.top + (sz.cy );
r.left = r.right - (sz.cx);

SetTextAlign( MyDc, TA_TOP | TA_LEFT );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR] );

SetBkMode( MyDc, TRANSPARENT );

TextOut( MyDc, r.left, r.top+2, s.text(), s.len() );

SelectObject( MyDc, old_font );
}

/***********************************************************************
*                             GET_FT2_HELP                             *
***********************************************************************/
void get_ft2_help()
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

if ( path.get_exe_dir_file_name(TEXT("ft2help.txt")) )
    {
    if ( f.open_for_write(path.text()) )
        {
        f.writebinary( s, SLEN );
        f.close();
        }
    if ( path.get_exe_dir_file_name(TEXT("ft2help.exe")) )
        {
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
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( TCHAR * fname, UINT helptype, DWORD context )
{
NAME_CLASS s;

if ( s.get_exe_dir_file_name(fname) )
    WinHelp( MainWindow, s.text(), helptype, context );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( UINT helptype, DWORD context )
{
gethelp( MainHelpFileName, helptype, context );
}

/***********************************************************************
*                   END_DOWNTIME_ENTRY_DIALOG_PROC                     *
***********************************************************************/
static void end_downtime_entry_dialog()
{
HWND w;

if ( DowntimeEntryDialog )
    {
    w = DowntimeEntryDialog;
    DowntimeEntryDialog = 0;
    DestroyWindow( w );
    }
}

/***********************************************************************
*                       SET_CAT_AND_SUBCAT_TABS                        *
***********************************************************************/
void set_cat_and_subcat_tabs()
{
int tabs[] = { 30, 100 };
const int n = sizeof(tabs)/sizeof(int);

SendDlgItemMessage( DowntimeEntryDialog, CAT_AND_SUBCAT_LB, LB_SETTABSTOPS, (WPARAM) n, (LPARAM) tabs );
}

/***********************************************************************
*                      FILL_CAT_AND_SUBCAT_LB                          *
***********************************************************************/
static void fill_cat_and_subcat_lb()
{
STRING_CLASS  s;
int i;

if ( DowntimeEntryIsChanging )
    return;

DowntimeEntryIsChanging = true;
for ( i=0; i<SubCats.count(); i++ )
    {
    s = SubCats[i].catnumber;
    s.cat_w_char( SubCats[i].subnumber, TabChar );
    s.cat_w_char( Cats.name(SubCats[i].catnumber), TabChar );
    s += SubCats[i].name;
    DowntimeEntryLb.add( s.text() );
    }
DowntimeEntryIsChanging = false;
}

/***********************************************************************
*                         SET_XBOX_FROM_LB                             *
***********************************************************************/
static void set_xbox_from_lb()
{
static STRING_CLASS s;

if ( DowntimeEntryIsChanging )
    return;

DowntimeEntryIsChanging = true;

s = DowntimeEntryLb.selected_text();
if ( s.len() > 4 )
    *(s.text()+4) = NullChar;
set_text( DowntimeEntryEbox, s.text() );
eb_select_all( DowntimeEntryEbox );
EnableWindow( DowntimeEntryOkButton, TRUE );

DowntimeEntryIsChanging = false;
}

/***********************************************************************
*                          SET_LB_FROM_XBOX                            *
***********************************************************************/
static void set_lb_from_xbox()
{
const int FULL_ENTRY_LENGTH = DOWNCAT_NUMBER_LEN * 2;
static STRING_CLASS s;
static int  i;
static BOOL is_ok;

if ( DowntimeEntryIsChanging )
    return;

DowntimeEntryIsChanging = true;
is_ok = FALSE;

if ( s.get_text(DowntimeEntryEbox) )
    {
    if ( DowntimeEntryLb.findprefix(s.text()) )
        {
        i = DowntimeEntryLb.current_index();
        DowntimeEntryLb.setcursel( i );
        if ( s.len() == FULL_ENTRY_LENGTH )
            is_ok = TRUE;
        }
    else
        {
        DowntimeEntryLb.select_all( FALSE );
        }
    }

EnableWindow( DowntimeEntryOkButton, is_ok );
DowntimeEntryIsChanging = false;
}

/***********************************************************************
*                            SAVE_CLASS_NAME                           *
***********************************************************************/
void save_class_name( HWND w )
{
TCHAR buf[256];
int   n;

n = GetClassName( w, buf, 256 );
if ( n > 0 )
    {
    buf[n] = NullChar;
    put_ini_string( VisiTrakIniFile, HmiSection, ClassKey, buf );
    }
}

/***********************************************************************
*                            EnumWindowsProc                           *
***********************************************************************/
BOOL CALLBACK my_enum_windows_proc( HWND w, LPARAM lParam )
{
STRING_CLASS s;

s.get_text( w );
if ( !s.isempty() )
    {
    if ( s.contains( TEXT("Frech") ) )
        {
        save_class_name( w );
        SetForegroundWindow( w );
        return FALSE;
        }
    }

return TRUE;
}

/***********************************************************************
*                          RESTORE_FOCUS_TO_HMI                        *
***********************************************************************/
void restore_focus_to_hmi()
{
static TCHAR defclass[] = TEXT( "VisiTrakEditor" );
TCHAR * cp;
HWND w;

cp = get_ini_string( VisiTrakIniFile, HmiSection, ClassKey );
if ( unknown(cp) )
    {
    cp = defclass;
    put_ini_string( VisiTrakIniFile, HmiSection, ClassKey, cp );
    }
w = FindWindow( cp, NULL );
if ( w != NULL )
    {
    SetForegroundWindow( w );
    }
else
    {
    EnumWindows( my_enum_windows_proc, 0 );
    }
}

/***********************************************************************
*                        SET_DOWNTIME_FROM_CHOICE                      *
***********************************************************************/
static bool set_downtime_from_choice()
{
const int FULL_ENTRY_LENGTH = DOWNCAT_NUMBER_LEN * 2;

TCHAR           worker[OPERATOR_NUMBER_LEN+1];
STRING_CLASS    s;
NAME_CLASS      filename;
HCURSOR         old_cursor;
DB_TABLE        t;
TCHAR         * cp;
COMPUTER_CLASS  c;
DSTAT_CLASS     d;
PART_NAME_ENTRY pn;
int             last_record;
int             current_record;
TIME_CLASS      new_time;
TIME_CLASS      old_time;
BOOLEAN         is_new_record;

last_record    = -1;
current_record = -1;

pn.computer = c.whoami();
pn.machine  = DowntimeEntryMachine;
pn.part.null();

*worker = NullChar;

if ( !s.get_text(DowntimeEntryEbox) )
    return false;

if ( s.len() != FULL_ENTRY_LENGTH )
    return false;

if ( !DowntimeEntryLb.findprefix(s.text()) )
    return false;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

if ( !waitfor_shotsave_semaphor(pn.computer, pn.machine)  )
    {
    resource_message_box( MainInstance, CANNOT_COMPLY_STRING, SEMAPHOR_FAIL_STRING );
    SetCursor( old_cursor );
    return false;
    }

is_new_record = TRUE;
new_time.set( DowntimeEntrySt );
filename.get_downtime_dbname( pn.computer, pn.machine );
if ( t.open(filename, DOWNTIME_RECLEN, FL) )
    {
    last_record = (int) t.nof_recs();
    last_record--;
    if ( last_record >= 0 )
        {
        t.goto_record_number( last_record );
        if ( t.get_rec() )
            {
            if ( get_time_class_from_db(old_time, t, DOWNTIME_DATE_OFFSET) )
                {

                if ( old_time >= new_time )
                    {
                    /*
                    -------------------------------------------------------------------------
                    Either the is the record I am modifying or a new downtime record has been
                    inserted since we started. In either case, replace it with the new one.
                    ------------------------------------------------------------------------- */
                    is_new_record = FALSE;
                    }
                }
            }
        }

    t.put_date( DOWNTIME_DATE_OFFSET, DowntimeEntrySt );
    t.put_time( DOWNTIME_TIME_OFFSET, DowntimeEntrySt );
    if ( is_new_record )
        {
        pn.part = CurrentMachine.current_part;
        copystring( worker,  CurrentMachine.human.text() );
        }
    else
        {
        t.get_alpha( pn.part, DOWNTIME_PART_NAME_OFFSET, OLD_PART_NAME_LEN );
        t.get_alpha( worker,  DOWNTIME_OPERATOR_OFFSET,  OPERATOR_NUMBER_LEN );
        }
    cp = s.text();
    d.set_cat( cp );
    t.put_alpha( DOWNTIME_CAT_OFFSET, cp, DOWNCAT_NUMBER_LEN );
    cp += DOWNCAT_NUMBER_LEN;
    d.set_subcat( cp );
    t.put_alpha( DOWNTIME_SUBCAT_OFFSET, cp, DOWNCAT_NUMBER_LEN );

    if ( is_new_record )
        t.rec_append();
    else
        t.rec_update();
    current_record = (int) t.current_record_number();
    if ( is_new_record )
        last_record = current_record;
    t.close();
    }

free_shotsave_semaphore();

/*
-------------------------------------------------------------------------------
Only update the dstat if the downtime has not changed since the dialog appeared
------------------------------------------------------------------------------- */
if ( current_record >= 0 && current_record == last_record )
    {
    if ( !pn.part.isempty() )
        {
        d.set_time( DowntimeEntrySt );
        d.put( pn.computer, pn.machine );
        cp = make_downtime_event_string( pn.machine, pn.part, DowntimeEntrySt, d.category(), d.subcategory(), worker );
        if ( cp )
            {
            poke_data( DDE_MONITOR_TOPIC, ItemList[DOWN_DATA_INDEX].name, cp );
            delete[] cp;
            }
        }
    }

SetCursor( old_cursor );
return true;
}

/***********************************************************************
*                     DOWNTIME_ENTRY_SUB_PROC                          *
* Subclass the edit control so if the operator presses an up           *
* or down arrow, the current selection in the listbox changes.         *
***********************************************************************/
LRESULT CALLBACK downtime_entry_sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  i;

if ( msg == WM_KEYDOWN )
    {
    if ( wParam == VK_UP || wParam == VK_DOWN )
        {
        i = DowntimeEntryLb.current_index();
        if ( wParam == VK_UP && i > 0 )
            i--;
        else if ( wParam == VK_DOWN && i < (DowntimeEntryLb.count() - 1) )
            i++;
        else
            return 0;

        DowntimeEntryLb.setcursel( i );
        set_xbox_from_lb();
        return 0;
        }

    }

return CallWindowProc( OldEditProc, w, msg, wParam, lParam );
}

/***********************************************************************
*                         SHOW_DOWNTIME_ENTRY_TIME                     *
***********************************************************************/
void show_downtime_entry_time()
{
COMPUTER_CLASS c;
DB_TABLE       t;
NAME_CLASS     s;
int32          n;
TCHAR          subcat[DOWNCAT_NUMBER_LEN+1];

DowntimeEntryMachine.set_text( DowntimeEntryDialog, EVENT_MACHINE_TBOX );

init_systemtime_struct( DowntimeEntrySt );

s.get_downtime_dbname( c.whoami(), DowntimeEntryMachine );

GetLocalTime( &DowntimeEntrySt );

if ( t.open(s, DOWNTIME_RECLEN, FL) )
    {
    n = t.nof_recs();
    if ( n > 0 )
        {
        n--;
        t.goto_record_number(n);
        t.get_current_record( NO_LOCK );
        t.get_alpha( subcat, DOWNTIME_SUBCAT_OFFSET, DOWNCAT_NUMBER_LEN );
        if ( strings_are_equal( subcat, DOWNCAT_UNSP_DOWN_SUBCAT) )
            {
            t.get_date( DowntimeEntrySt, DOWNTIME_DATE_OFFSET );
            t.get_time( DowntimeEntrySt, DOWNTIME_TIME_OFFSET );
            }
        }
    t.close();
    }

s = alphadate(DowntimeEntrySt);
s += SpaceChar;
s += alphatime( DowntimeEntrySt );

s.set_text( DowntimeEntryDialog, EVENT_TIME_TBOX );
}

/***********************************************************************
*                    POSITION_DOWNTIME_ENTRY_DIALOG                    *
***********************************************************************/
void position_downtime_entry_dialog()
{
WINDOW_CLASS w;
RECTANGLE_CLASS desktop;
RECTANGLE_CLASS r;
LONG x;
LONG y;

GetClientRect( GetDesktopWindow(), &desktop );

w = DowntimeEntryDialog;
w.getrect( r );

x = desktop.right - desktop.left;
x /= 2;
x -= r.width()/2;
if ( x < 0 )
    x = 0;

y = desktop.bottom - desktop.top;
y /= 2;
y -= r.height()/2;
if ( y < 0 )
    y = 0;

w.move( x, y );
}

/***********************************************************************
*                    SET_DOWNTIME_ENTRY_SETUP_STRINGS                  *
***********************************************************************/
static void set_downtime_entry_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

static RESOURCE_TEXT_ENTRY rsn[] = {
    { EVENT_MACHINE_STATIC, TEXT("MACHINE_STRING") },
    { DOWNTIME_STATIC     , TEXT("DOWNTIME_PROMPT") },
    { IDOK,                 TEXT("OK_STRING") },
    { IDCANCEL,             TEXT("CANCEL") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    EVENT_MACHINE_TBOX,
    EVENT_TIME_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("DOWNTIME_ENTRY_CAPTION")) );
w.refresh();
}
/***********************************************************************
*                       DOWNTIME_ENTRY_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK downtime_entry_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        DowntimeEntryDialog = hWnd;
        set_downtime_entry_setup_strings( hWnd );
        position_downtime_entry_dialog();
        DowntimeEntryEbox     = GetDlgItem( hWnd, CAT_AND_SUBCAT_EBOX );
        DowntimeEntryOkButton = GetDlgItem( hWnd, IDOK );
        EnableWindow( DowntimeEntryOkButton, FALSE );
        DowntimeEntryLb.init( GetDlgItem(hWnd, CAT_AND_SUBCAT_LB) );
        show_downtime_entry_time();
        OldEditProc = (WNDPROC) SetWindowLong( DowntimeEntryEbox, GWL_WNDPROC, (LONG) downtime_entry_sub_proc );
        set_cat_and_subcat_tabs();
        fill_cat_and_subcat_lb();
        SetFocus( DowntimeEntryEbox );
        return TRUE;

    case WM_CLOSE:
        end_downtime_entry_dialog();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, DOWNTIME_ENTRY_SCREEN_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CAT_AND_SUBCAT_LB:
                if ( cmd == LBN_SELCHANGE )
                    {
                    set_xbox_from_lb();
                    }
                return TRUE;

            case CAT_AND_SUBCAT_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    set_lb_from_xbox();
                    }
                return TRUE;

            case IDOK:
                if ( !set_downtime_from_choice() )
                    {
                    resource_message_box( MainInstance, NO_DOWNTIME_SELECTED_STRING, CANNOT_COMPLY_STRING );
                    return TRUE;
                    }

            case IDCANCEL:
                end_downtime_entry_dialog();
                return TRUE;

            }
        break;
    }
return FALSE;
}

/***********************************************************************
*                       LOAD_REALTIME_MACHINE_LIST                     *
***********************************************************************/
void load_realtime_machine_list()
{
COMPUTER_CLASS          c;
NAME_CLASS              s;
SETPOINT_CLASS          sp;
MACHINE_NAME_LIST_CLASS m;
BITSETYPE               monitor_flags;

if ( c.count() > 1 )
    RealtimeMachineList.load();
else
    {
    RealtimeMachineList.empty();
    m.add_computer( c.whoami() );
    m.rewind();
    while ( m.next() )
        {
        s.get_machset_csvname( m.computer_name(), m.name() );
        if ( s.file_exists() )
            {
            if ( sp.get_one_value(s, s, MACHSET_MONITOR_FLAGS_INDEX) )
                {
                monitor_flags = (BITSETYPE) s.uint_value();
                if ( monitor_flags & MA_MONITORING_ON )
                    RealtimeMachineList.append( m.name().text() );
                }
            }
        }
    }
}

/***********************************************************************
*                            LOAD_MACHLIST                             *
***********************************************************************/
static void load_machlist()
{
COMPUTER_CLASS  c;
FILE_CLASS      f;
INI_CLASS       ini;
bool            is_remote;
STRING_CLASS    lastco;
MACHINE_CLASS   ma;
NAME_CLASS      s;
TEXT_LIST_CLASS t;
bool            this_is_a_monitoring_computer;

MachList.empty();

/*
---------------------------------------
Load the machines at this computer.
to see if this is a monitoring computer
--------------------------------------- */
this_is_a_monitoring_computer = false;
MachList.add_computer( c.whoami() );
if ( MachList.count() > 0 )
    {
    MachList.rewind();
    while ( MachList.next() )
        {
        if ( ma.find(c.whoami(), MachList.name()) )
            {
            if ( ma.monitor_flags & MA_MONITORING_ON )
                {
                this_is_a_monitoring_computer = true;
                break;
                }
            }
        }
    }

/*
------------------------------------------------------------
See if I should override not accessing the network computers
------------------------------------------------------------ */
if ( this_is_a_monitoring_computer )
    {
    s.get_local_ini_file_name( VisiTrakIniFile );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    if ( ini.find(LoadNetworkedMachinesKey) )
        {
        if ( ini.get_boolean() )
            this_is_a_monitoring_computer = false;
        }
    }

c.rewind();
/*
------------------------------------------------------------------------------------------------
If this is a monitoring computer then all the other computers in my list are office workstations
and I don't need to see if they are accessable
------------------------------------------------------------------------------------------------ */
if ( !this_is_a_monitoring_computer )
    {
    while ( c.next() )
        {
        if ( (c.name() != c.whoami()) )
            {
            if ( LoadFromNetworkDialogWindow )
                c.name().set_text( LoadFromNetworkDialogWindow, NETWORK_COMPUTER_TBOX );

            if ( c.is_present() )
                MachList.add_computer( c.name() );
            else
                t.append( c.name().text() );
            }
        }
    }

/*
-------------------------------------------------------
Save the list of machines on each computer that I found
------------------------------------------------------- */
if ( MachList.count() > 0 )
    {
    is_remote = false;
    MachList.rewind();
    while ( MachList.next() )
        {
        if ( lastco != MachList.computer_name() )
            {
            if ( is_remote )
                f.close();
            lastco = MachList.computer_name();
            is_remote = false;
            if ( lastco != c.whoami() )
                {
                is_remote = true;
                if ( s.get_machine_list_file_name(lastco) )
                    {
                    if ( !f.open_for_write(s) )
                        is_remote = false;
                    }
                }
            }
        if ( is_remote )
            f.writeline( MachList.name().text() );
        }

    if ( is_remote )
        f.close();
    }

/*
-----------------------------------------------------------------------------------------------------------
If any computers are off line, see if I can load from the machine_list_file that was saved the last time it
was on line.
----------------------------------------------------------------------------------------------------------- */
if ( t.count() > 0 )
    {
    t.rewind();
    while ( t.next() )
        {
        lastco = t.text();
        if ( s.get_machine_list_file_name(lastco) )
            {
            if ( f.open_for_read(s.text()) )
                {
                while ( true )
                    {
                    s = f.readline();
                    if ( s.isempty() )
                        break;
                    MachList.add( lastco, s, NO_SORT );
                    }
                f.close();
                }
            }
        }
    }
}

/***********************************************************************
*                            LOAD_PART_INFO                            *
***********************************************************************/
static BOOLEAN load_part_info()
{
COMPUTER_CLASS c;
INI_CLASS  ini;
NAME_CLASS s;
WORKER_LIST_CLASS worker;

if ( !c.find(ComputerName) )
    return FALSE;

if ( !c.is_present() )
    return FALSE;

if ( MachineName != CurrentMachine.name )
    {
    CurrentMachine.find( ComputerName, MachineName );
    init_global_curve_types();
    worker.load();
    CurrentWorker = worker.full_name( CurrentMachine.human.text() );
    }

if ( CurrentPart.find(ComputerName, MachineName, PartName) )
    {
    CurrentSort.load( ComputerName, MachineName, PartName );
    CurrentParam.find( ComputerName, MachineName, PartName );
    CurrentMarks.load( ComputerName, MachineName, PartName );
    CurrentFtAnalog.load( ComputerName, MachineName, PartName );
    CurrentFtCalc.load( ComputerName, MachineName, PartName );

    s.get_part_display_ini_name( ComputerName, MachineName, PartName );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );

    PostImpactDisplayTime = 0.0;
    if ( ini.find(DisplaySecondsKey) )
        {
        s = ini.get_string();
        PostImpactDisplayTime = s.float_value();
        }

    if ( PartBasedDisplay )
        read_corner_parameters( CornerParameters, NofCornerParameters );

    ChannelAxisLabel.get( ComputerName, MachineName, PartName );

    return TRUE;
    }
else
    {
    PartName.null();
    }
return FALSE;
}

/***********************************************************************
*                            LOAD_PART_INFO                            *
***********************************************************************/
static BOOLEAN load_part_info( PROFILE_NAME_ENTRY * p )
{
ComputerName = p->computer;
MachineName  = p->machine;
PartName     = p->part;
ShotName     = p->shot;

return load_part_info();
}

/***********************************************************************
*                        SHOW_LAST_ERROR                               *
***********************************************************************/
void show_last_error()
{
DWORD status;
TCHAR s[34];

status = GetLastError();
s[0] = TEXT('$');
int32toasc( s+1, status, HEX_RADIX );
MessageBox( MainPlotWindow, s, TEXT("Last Error"), MB_OK );
}

/***********************************************************************
*                       SELECT_PRINTER_FONT                            *
***********************************************************************/
static void select_printer_font()
{
TEXTMETRIC tm;

if ( !MyDc )
    return;

if ( !PrinterFont )
    return;

SelectObject( MyDc, PrinterFont ) ;

GetTextMetrics( MyDc, &tm );
MyCharHeight = tm.tmHeight - tm.tmInternalLeading;
MyCharWidth  = tm.tmAveCharWidth;
}

/***********************************************************************
*                         DELETE_PRINTER_FONT                          *
***********************************************************************/
static void delete_printer_font()
{
if ( PrinterFont )
    {
    DeleteObject( PrinterFont );
    PrinterFont = 0;
    }
}

/***********************************************************************
*                           CREATE_PRINTER_FONT                        *
***********************************************************************/
static void create_printer_font( RECT & r )
{
long       divisor;
long       height;
long       width;
TEXTMETRIC tm;
HFONT      f;
TCHAR    * name;

if ( !MyDc )
    return;

if ( MyScreenType == TREND_SCREEN_TYPE && GridIsVisible )
    {
    width  = r.right - r.left;
    height = r.bottom - r.top;

    if ( width > height )
        {
        height /= CurrentParam.count() + 9;
        width  = 0;
        }
    else
        {
        height = 0;
        width /= 130;
        }

    name = TimesRomanFontName.text();
    }
else
    {
    width   = 0;
    divisor = 130;

    /*
    -----------------------------------------
    Get the aspect ratio of the original font
    ----------------------------------------- */
    GetTextMetrics( MyDc, &tm );
    height = tm.tmHeight - tm.tmInternalLeading;

    /*
    ---------------------------------------------------
    Adjust by the ratio of the new width to the old one
    --------------------------------------------------- */
    height *= ( r.right - r.left );
    height /= divisor;
    height /= tm.tmAveCharWidth;

    name = ArialFontName.text();
    }

f = create_font( MyDc, width, height, name );
if ( f )
    {
    /*
    ---------------------------------------------------------------
    The following gives me a default and assures that MyFont is not
    currently selected (it is bad form to delete the current font).
    --------------------------------------------------------------- */
    if ( PrinterFont )
        {
        SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) ) ;
        DeleteObject( PrinterFont );
        }

    PrinterFont = f;
    }
}

/***********************************************************************
*                          GET_DEFAULT_PRINTER_DC                      *
***********************************************************************/
HDC get_default_printer_dc()
{
PRINTER_INFO_2 * p;
DWORD     n;
DWORD     bytes_needed;
DWORD     slen;
HDC       mydc;


DEVMODE * dm;
TCHAR   * cp;
TCHAR     s[MAX_PATH+1];
HANDLE    ph;

mydc = 0;

GetProfileString( TEXT("windows"), TEXT("device"), TEXT("..."), s, MAX_PATH );
cp = findchar( CommaChar, s );
if ( cp )
    *cp = NullChar;

if ( OpenPrinter(s, &ph, 0) )
    {
    bytes_needed = 0;

    if ( !GetPrinter(ph, 2, 0, 0, &bytes_needed) )
        {
        if ( bytes_needed > 0 )
            {
            slen = sizeof( PRINTER_INFO_2 );
            n = bytes_needed / slen + 1;
            slen *= n;

            p = new PRINTER_INFO_2[n];
            if ( p )
                {
                if ( GetPrinter(ph, 2, (BYTE *) p, slen, &bytes_needed) )
                    {
                    dm = p->pDevMode;
                    if ( dm )
                        {
                        if ( dm->dmOrientation == DMORIENT_PORTRAIT )
                            {
                            PrinterMargin.right  =   0;    /* mils */
                            PrinterMargin.bottom = 500;
                            }
                        else
                            {
                            PrinterMargin.right  = 500;  /* mils */
                            PrinterMargin.bottom =   0;
                            }
                        mydc = CreateDC( 0, s, 0, 0 );
                        }

                    }
                delete[] p;
                }
            else
                {
                resource_message_box( MainInstance, PRINTER_LIST_STRING, NO_MEMORY_STRING );
                }
            }
        }

    ClosePrinter( ph );
    }

return mydc;
}

/***********************************************************************
*                            GET_PRINTER_DC                            *
***********************************************************************/
HDC get_printer_dc()
{
HDC        mydc;
static PRINTDLG   pd;
DEVMODE  * dm;

pd.lStructSize = sizeof(PRINTDLG);
pd.hwndOwner   = MainPlotWindow;
pd.hDevMode    = 0;
pd.hDevNames   = 0;
pd.hDC         = 0;
pd.Flags       = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_HIDEPRINTTOFILE | PD_NOSELECTION | PD_NOPAGENUMS;
pd.nFromPage   = 1;
pd.nToPage     = 1;
pd.nMinPage    = 1;
pd.nMaxPage    = 1;
pd.nCopies     = 1;
pd.hInstance   = MainInstance;
pd.lCustData            = 0;
pd.lpfnPrintHook        = 0;
pd.lpfnSetupHook        = 0;
pd.lpPrintTemplateName  = 0;
pd.lpSetupTemplateName  = 0;
pd.hPrintTemplate       = 0;
pd.hSetupTemplate       = 0;

mydc = 0;
if ( PrintDlg(&pd) )
    {
    mydc = pd.hDC;
    if ( pd.hDevMode )
        {
        dm = (DEVMODE *) GlobalLock( pd.hDevMode );
        if ( dm->dmOrientation == DMORIENT_PORTRAIT )
            {
            PrinterMargin.right  =   0;    /* mils */
            PrinterMargin.bottom = 500;
            }
        else
            {
            PrinterMargin.right  = 500;  /* mils */
            PrinterMargin.bottom =   0;
            }

        GlobalUnlock( pd.hDevMode );
        GlobalFree( pd.hDevMode );
        }

    if ( pd.hDevNames )
        GlobalFree( pd.hDevNames );
    }

return mydc;
}

/***********************************************************************
*                           CLEAR_ONE_AXIS                             *
***********************************************************************/
static void clear_one_axis( short i )
{
if ( !Axis[i].is_fixed )
    {
    Axis[i].max = 0.0;
    Axis[i].min = 0.0;
    Axis[i].is_on_hold = FALSE;
    Axis[i].is_clear   = TRUE;
    }
}

/***********************************************************************
*                             CLEAR_Y_AXES                             *
***********************************************************************/
static void clear_y_axes()
{
short i;

for ( i=0; i<NOF_AXES; i++ )
    {
    if ( i != TIME_AXIS && i != X_POS_AXIS )
        clear_one_axis( i );
    }

}

/***********************************************************************
*                              CLEAR_AXES                              *
***********************************************************************/
static void clear_axes()
{
short i;

for ( i=0; i<NOF_AXES; i++ )
    clear_one_axis( i );
}

/***********************************************************************
*                          RESET_PLOT_CURVES                           *
***********************************************************************/
static void reset_plot_curves()
{
int i;

NofPlotCurves = 0;
for ( i=0; i<MAX_CONTINUOUS_CHANNELS; i++ )
    {
    PlotCurve[i].curve_index = NO_INDEX;
    *PlotCurve[i].label      = NullChar;
    }
}

/***********************************************************************
*                           SET_PLOT_CURVES                            *
***********************************************************************/
void set_plot_curves()
{
int  channel;
int  i;
int  nof_channels;
BITSETYPE my_curve_types;
BITSETYPE curve_type;

if ( !HaveHeader )
    return;

reset_plot_curves();

my_curve_types = PreImpactPlotCurveTypes | PostImpactPlotCurveTypes;

if ( my_curve_types & HEAD_PRESSURE_CURVE )
    {
    i  = plot_curve_index( HEAD_PRESSURE_CURVE );
    if ( i != NO_INDEX )
        {
        PlotCurve[NofPlotCurves].curve_index = i;
        lstrcpy( PlotCurve[NofPlotCurves].label, StringTable.find(TEXT("HEAD_PRESSURE_LABEL_STRING")) );
        NofPlotCurves++;
        }
    }

if ( my_curve_types & ROD_PRESSURE_CURVE )
    {
    i  = plot_curve_index( ROD_PRESSURE_CURVE );
    if ( i != NO_INDEX )
        {
        PlotCurve[NofPlotCurves].curve_index = i;
        lstrcpy( PlotCurve[NofPlotCurves].label, resource_string(ROD_PRESSURE_LABEL_STRING) );
        NofPlotCurves++;
        }
    }

if ( my_curve_types & DIFF_PRESSURE_CURVE )
    {
    i  = plot_curve_index( DIFF_PRESSURE_CURVE );
    if ( i != NO_INDEX )
        {
        PlotCurve[NofPlotCurves].curve_index = i;
        lstrcpy( PlotCurve[NofPlotCurves].label, resource_string(DIFF_PRESSURE_LABEL_STRING) );
        NofPlotCurves++;
        }
    }

curve_type = CHAN_1_ANALOG_CURVE;
nof_channels = NOF_FTII_ANALOGS;
for ( channel=0; channel<nof_channels; channel++ )
    {
    if ( NofPlotCurves >= MAX_CONTINUOUS_CHANNELS )
        break;
    if ( my_curve_types & curve_type )
        {
        i  = plot_curve_index( curve_type );
        if ( i != NO_INDEX )
            {
            if ( !plotcurve_contains(i) )
                {
                PlotCurve[NofPlotCurves].curve_index = i;
                i = channel;
                if ( i < 4 )
                    i += CHANNEL_1_LABEL_STRING;
                else
                    i += CHANNEL_5_LABEL_STRING - 4;
                lstrcpy( PlotCurve[NofPlotCurves].label, resource_string(i) );
                NofPlotCurves++;
                }
            }
        }
    /*
    ---------------------------------------------------------
    Each consecutive curve type is the next bit
    except the DIFF_PRESSURE_CURVE which is inbetween 4 and 5
    --------------------------------------------------------- */
    curve_type <<= 1;
    if ( curve_type == DIFF_PRESSURE_CURVE )
        curve_type <<= 1;
    }

NeedToUpdateZoomLabels = TRUE;
}

/***********************************************************************
*                                ZOOMED                                *
***********************************************************************/
static BOOLEAN zoomed()
{

if ( LeftZoomIndex != 0 || ZoomIndex != 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                           HIDE_ALL_WINDOWS                           *
***********************************************************************/
void hide_all_windows()
{
ShowWindow( MainPlotWindow,     SW_HIDE );
ShowWindow( SmallZoomBoxWindow, SW_HIDE );

ShowWindow( ParameterWindow,     SW_HIDE );
ShowWindow( ParamHistoryWindow, SW_HIDE );
ShowWindow( ZoomBoxWindow,      SW_HIDE );
XdcrWindow.hide();
}

/***********************************************************************
*                           SHOW_ALL_WINDOWS                           *
***********************************************************************/
void show_all_windows()
{
int state;

if ( IsIconic(MainWindow) )
    ShowWindow( MainWindow, SW_RESTORE );

if ( !IsWindowVisible(MainPlotWindow) )
    ShowWindow( MainPlotWindow, SW_SHOW );

if ( MyScreenType == ZABISH_SCREEN_TYPE )
    {
    ShowWindow( SmallZoomBoxWindow, SW_HIDE );
    if ( !zoomed() )
        ShowWindow( ZoomBoxWindow,  SW_HIDE );

    if ( InfoBoxType == XDCR_INFO_BOX )
        {
        XdcrWindow.show();
        state = SW_HIDE;
        }
    else
        {
        XdcrWindow.hide();
        state = SW_SHOW;
        }

    ShowWindow( ParameterWindow, state );
    ShowWindow( ParamHistoryWindow, state );
    if ( zoomed() )
        ShowWindow( ZoomBoxWindow,  SW_SHOW );
    }
else if ( MyScreenType == STANDARD_SCREEN_TYPE )
    {
    ShowWindow( ParamHistoryWindow, SW_HIDE );
    ShowWindow( ZoomBoxWindow,      SW_HIDE );
    if ( !zoomed() )
        ShowWindow( SmallZoomBoxWindow, SW_HIDE );

    if ( InfoBoxType == XDCR_INFO_BOX )
        {
        XdcrWindow.show();
        state = SW_HIDE;
        }
    else
        {
        XdcrWindow.hide();
        state = SW_SHOW;
        }

    ShowWindow( ParameterWindow,    SW_SHOW );
    if ( zoomed() )
        ShowWindow( SmallZoomBoxWindow, SW_SHOW );
    }
else
    {
    ShowWindow( MainPlotWindow,     SW_SHOW );
    ShowWindow( SmallZoomBoxWindow, SW_HIDE );

    ShowWindow( ParameterWindow,    SW_HIDE );
    ShowWindow( ParamHistoryWindow, SW_HIDE );
    ShowWindow( ZoomBoxWindow,      SW_HIDE );
    XdcrWindow.hide();
    }
}

/***********************************************************************
*                        TOGGLE_MENU_VIEW_STATE                        *
***********************************************************************/
void toggle_menu_view_state( BOOLEAN & state, UINT menu_choice_id )
{
if ( state )
    state = FALSE;
else
    state = TRUE;

MainMenu.set_checkmark( menu_choice_id, state );
}

/***********************************************************************
*                          SELECT_MYFONT                               *
***********************************************************************/
static void select_myfont()
{
TEXTMETRIC tm;

if ( !MyDc )
    return;

if ( MyFont )
    SelectObject( MyDc, MyFont );
else
    SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) ) ;

GetTextMetrics( MyDc, &tm );
MyCharHeight = tm.tmHeight - tm.tmInternalLeading;
MyCharWidth  = tm.tmAveCharWidth;
}

/***********************************************************************
*                               GET_MYDC                               *
***********************************************************************/
static void get_mydc()
{

if ( !MyDc )
    {
    MyDc = GetDC( MainPlotWindow );
    select_myfont();
    }

NofDcOwners++;
}

/***********************************************************************
*                             RELEASE_MYDC                             *
***********************************************************************/
static void release_mydc()
{

if ( !MyDc )
    return;

if ( NofDcOwners )
    NofDcOwners--;

if ( NofDcOwners == 0 )
    {
    ReleaseDC( MainPlotWindow, MyDc );
    MyDc = NULL;
    }

}

/***********************************************************************
*                          DRAW_VERTICAL_TICS                          *
***********************************************************************/
void draw_vertical_tics( RECT & r, int nof_tics, HPEN gridpen,  HPEN borderpen )
{

int i;
int x;
int range;
HPEN oldpen;

oldpen = (HPEN) SelectObject( MyDc, borderpen );

range = r.right - r.left;
for ( i=0; i<=nof_tics; i++ )
    {
    if ( i == 1 )
        SelectObject( MyDc, gridpen );
    else if ( i == nof_tics )
        SelectObject( MyDc, borderpen );

    x  = range * i;
    x /= nof_tics;
    x += r.left;

    MoveToEx( MyDc, x, r.top, 0 );
    LineTo( MyDc, x, r.bottom );
    }

SelectObject( MyDc, oldpen );
}

/***********************************************************************
*                         DRAW_HORIZONTAL_TICS                         *
***********************************************************************/
static void draw_horizontal_tics( RECT & r, int nof_tics, HPEN gridpen,  HPEN borderpen )
{

int i;
int y;
HPEN oldpen;

oldpen = (HPEN) SelectObject( MyDc, borderpen );

for ( i=0; i<=nof_tics; i++ )
    {
    if ( i == 1 )
        SelectObject( MyDc, gridpen );
    if ( i == nof_tics )
        SelectObject( MyDc, borderpen );
    y = r.top + (r.bottom - r.top - 1) * i / nof_tics;
    MoveToEx( MyDc, r.left, y, 0 );
    LineTo( MyDc, r.right, y );
    }

SelectObject( MyDc, oldpen );
}

/***********************************************************************
*                           DRAW_CURSOR_LINE                           *
***********************************************************************/
static void draw_cursor_line( int x )
{
int     orimode;

get_mydc();

if ( MyDc )
    {
    orimode = SetROP2( MyDc, R2_NOT );

    MoveToEx( MyDc, x, CursorStartY, 0 );
    LineTo(   MyDc, x, CursorEndY      );

    SetROP2( MyDc, orimode );
    }

release_mydc();
}

/***********************************************************************
*                          ERASE_CURSOR_LINE                           *
***********************************************************************/
static void erase_cursor_line()
{

if ( HaveCursorLine )
    {
    draw_cursor_line( CursorX );
    HaveCursorLine = FALSE;
    }
}

/***********************************************************************
*                       ERASE_LEFT_CURSOR_LINE                         *
***********************************************************************/
static void erase_left_cursor_line()
{

if ( HaveLeftCursorLine )
    {
    draw_cursor_line( LeftCursorX );
    HaveLeftCursorLine = FALSE;
    }
}

/***********************************************************************
*                     TOGGLE_BIG_PARAM_DISPLAY_TYPE                    *
***********************************************************************/
void toggle_big_param_display_type()
{
static BOOLEAN was_realtime       = TRUE;
static BOOLEAN was_single_machine = FALSE;
static short   old_screen_type    = PROFILE_SCREEN_TYPE;

if ( MyScreenType != BIG_PARAM_SCREEN_TYPE )
    {
    old_screen_type    = MyScreenType;
    was_realtime       = RealTime;
    was_single_machine = SingleMachineOnly;
    RealTime          = TRUE;
    SingleMachineOnly = TRUE;
    update_screen_type( BIG_PARAM_SCREEN_TYPE );
    }
else
    {
    RealTime          = was_realtime;
    SingleMachineOnly = was_single_machine;
    update_screen_type( old_screen_type );
    }

}

/***********************************************************************
*                      POSITION_XDCR_WINDOW                            *
***********************************************************************/
void position_xdcr_window()
{
RECT d;
RECT r;

d = my_desktop_client_rect();
GetWindowRect( MainPlotWindow,     &r );

d.top = r.bottom;
XdcrWindow.move( d );
}

/***********************************************************************
*                          SET_INFO_BOX_TYPE                           *
***********************************************************************/
void set_info_box_type( int new_type )
{

WINDOW_CLASS zoombox;
WINDOW_CLASS parambox;

if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == TREND_SCREEN_TYPE || MyScreenType == STATUS_SCREEN_TYPE )
    return;

parambox = ParameterWindow;
if ( MyScreenType == ZABISH_SCREEN_TYPE )
    zoombox  = ZoomBoxWindow;
else
    zoombox = SmallZoomBoxWindow;

if ( new_type != InfoBoxType )
    {
    switch ( InfoBoxType )
        {
        case ZOOM_INFO_BOX:
            zoombox.hide();
            break;

        case PARAMETER_INFO_BOX:
            if ( new_type == XDCR_INFO_BOX )
                ShowWindow( ParamHistoryWindow, SW_HIDE );
            break;

        case XDCR_INFO_BOX:
            XdcrWindow.hide();
            parambox.show();
            break;
        }

    switch ( new_type )
        {
        case ZOOM_INFO_BOX:
            zoombox.show();
            set_zoom_box_labels( zoombox.handle() );
            break;

        case PARAMETER_INFO_BOX:
            if ( MyScreenType == ZABISH_SCREEN_TYPE )
                ShowWindow( ParamHistoryWindow, SW_SHOW );
            break;

        case XDCR_INFO_BOX:
            parambox.hide();
            position_xdcr_window();
            XdcrWindow.show();
            break;
        }
    }

InfoBoxType = new_type;
}

/***********************************************************************
*                             DRAW_GRID                                *
***********************************************************************/
void draw_grid( int rindex )
{

int  nof_tics;
HPEN borderpen;
HPEN gridpen;

if ( !Region[rindex].is_visible )
    return;

gridpen   = CreatePen( PS_SOLID, 1, CurrentColor[GRID_COLOR]   );
borderpen = CreatePen( PS_SOLID, 1, CurrentColor[BORDER_COLOR] );

if ( gridpen && borderpen )
    {
    /*
    --------
    Vertical
    -------- */
    if ( CurrentDisplay != POS_TIME_DISPLAY )
        nof_tics = POS_BASED_X_TICS + TIME_BASED_X_TICS;

    else if ( rindex == POS_BASED_ANALOG || rindex == POS_BASED_COUNT )
        nof_tics = POS_BASED_X_TICS;

    else
        nof_tics = TIME_BASED_X_TICS;

    draw_vertical_tics( Region[rindex].rect, nof_tics, gridpen, borderpen );

    /*
    ----------
    Horizontal
    ---------- */
    if ( rindex == POS_BASED_ANALOG || rindex == TIME_BASED_ANALOG )
        nof_tics = ANALOG_Y_TICS;
    else
        nof_tics = COUNT_Y_TICS;

    draw_horizontal_tics( Region[rindex].rect, nof_tics, gridpen, borderpen );
    }

if ( gridpen )
    DeleteObject( gridpen );

if ( borderpen )
    DeleteObject( borderpen );
}

/***********************************************************************
*                             X_AXIS_LABEL                             *
***********************************************************************/
static void x_axis_label( RECT & r, TCHAR * sorc )
{

int  x;
int  y;

SetTextAlign( MyDc, TA_TOP | TA_CENTER );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );
SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );

x = (r.left + r.right)/2;
y = r.bottom + 2 * MyCharHeight;

TextOut( MyDc, x, y, sorc, lstrlen(sorc) );
}

/***********************************************************************
*                             ROD_HEAD_LABEL                           *
***********************************************************************/
static void rod_head_label()
{

int  c;
int  i;
int  slen;
int  true_channel;
int  x;
int  y;
STRING_CLASS s;

SetTextAlign( MyDc, TA_BOTTOM | TA_LEFT );
SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );

x = 1;

y = Region[POS_BASED_COUNT].rect.top;
y -= MyCharHeight/2;

for ( c=0; c<2; c++ )
    if ( NofPlotCurves > c )
        {
        slen = lstrlen( PlotCurve[c].label );
        if ( slen )
            {
            SetTextColor( MyDc, CurrentColor[FIRST_ANALOG_AXIS+c] );
            TextOut( MyDc, x, y, PlotCurve[c].label, slen );
            y -= MyCharHeight;
            }
        }

SetTextAlign( MyDc, TA_BOTTOM | TA_RIGHT );
y = Region[TIME_BASED_COUNT].rect.top;
y -= MyCharHeight/2;
x = Region[RIGHT_AXIS].rect.right - 1;

for ( c=2; c<4; c++ )
    if ( NofPlotCurves > c )
        {
        s.null();
        i = PlotCurve[c].curve_index;
        if ( i == NO_INDEX )
            continue;
        if ( i == DIFFERENTIAL_CURVE_INDEX )
            {
            s = StringTable.find( TEXT("DIF_AXIS_LABEL_STRING") );
            }
        else
            {
            true_channel = Ph.curves[i].ft_channel_number();
            if ( true_channel != NO_FT_CHANNEL )
                true_channel--;
            if ( i == DIFFERENTIAL_CURVE_INDEX )
                s = StringTable.find( TEXT("DIF_AXIS_LABEL_STRING") );
            else if ( i >= CHAN_1_ANALOG_CURVE_INDEX )
                s = ChannelAxisLabel[true_channel];
            }
        if ( s.isempty() )
            s = PlotCurve[c].label;

        slen = s.len();
        if ( slen )
            {
            SetTextColor( MyDc, CurrentColor[FIRST_ANALOG_AXIS+c] );
            TextOut( MyDc, x, y, s.text(), slen );
            y -= MyCharHeight;
            }
        }

SetTextAlign( MyDc, TA_BOTTOM | TA_LEFT );
}

/***********************************************************************
*                             Y_AXIS_LABEL                             *
***********************************************************************/
static void y_axis_label( RECT & r, TCHAR * sorc, TCHAR * sorc2, BITSETYPE axis_type, COLORREF color1, COLORREF color2 )
{

int      i;
int      delta_x;
int      delta_y;
int      slen;
int      x;
int      y;
int      height;
COLORREF oricolor;
SIZE     sorc_size;

if ( axis_type == LEFT_AXIS_TYPE )
    x = PlotWindowRect.left + 4 + MyCharWidth / 2;
else
    x = PlotWindowRect.right - 4 - MyCharWidth / 2;

slen   = lstrlen(sorc);

height = slen * MyCharHeight;
y = r.top;
delta_y = r.bottom - r.top;
if ( height < delta_y )
    y += (delta_y - height)/2;

oricolor = GetTextColor( MyDc );
SetTextColor( MyDc, color1 );
SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );
SetTextAlign( MyDc, TA_BOTTOM | TA_CENTER );

delta_x = 0;
for ( i=0; i<slen; i++ )
    {
    GetTextExtentPoint32( MyDc, sorc, 1, &sorc_size );
    if ( delta_x < sorc_size.cx )
        delta_x = sorc_size.cx;
    TextOut( MyDc, x, y, sorc, 1 );
    y += MyCharHeight;
    sorc++;
    }

SetTextColor( MyDc, color2 );
slen   = lstrlen(sorc2);
height = slen * MyCharHeight;
y = r.top;
delta_y = r.bottom - r.top;
if ( height < delta_y )
    y += (delta_y - height)/2;

if ( axis_type == LEFT_AXIS_TYPE )
    x += delta_x;
else
    x -= delta_x;

for ( i=0; i<slen; i++ )
    {
    TextOut( MyDc, x, y, sorc2, 1 );
    y += MyCharHeight;
    sorc2++;
    }

SetTextColor( MyDc, oricolor );
}

/***********************************************************************
*                             Y_AXIS_LABEL                             *
***********************************************************************/
static void y_axis_label( RECT & r, TCHAR * sorc, TCHAR * sorc2, BITSETYPE axis_type )
{
COLORREF c;
c = CurrentColor[TEXT_COLOR];

y_axis_label( r, sorc, sorc2, axis_type, c, c );
}

/***********************************************************************
*                             FIX_DECIMALS                             *
***********************************************************************/
void fix_decimals( TCHAR * buf, short decimal_places )
{

short slen;
TCHAR * cp;

cp = findchar( TEXT('.'), buf, 6 );
if ( cp )
    {
    slen = (short) lstrlen( cp );
    if ( decimal_places <= 0 )
        {
        *cp = NullChar;
        return;
        }
    slen--;
    cp++;
    if ( slen > decimal_places )
        {
        cp += decimal_places;
        *cp = NullChar;
        }
    }
}

/***********************************************************************
*                              DRAW_AXIS                               *
***********************************************************************/
void draw_axis( int axdex, short decimal_places, RECT & r,
                UINT alignment, BITSETYPE axis_type, short nof_tics,
                short labels_to_skip )
{

int      height;
int      i;
int      x;
int      y;
float    range;
float    value;
TCHAR    buf[10];
COLORREF oricolor;
UINT     oralign;
INT      orbkmode;

oralign  = GetTextAlign( MyDc );
oricolor = GetTextColor( MyDc );
orbkmode = GetBkMode( MyDc );

if ( axis_type == LEFT_AXIS_TYPE )
    {
    alignment |= TA_RIGHT;
    x = r.left - MyCharWidth;
    }
else
    {
    alignment |= TA_LEFT;
    x = r.right + MyCharWidth;
    }

SetTextAlign( MyDc, alignment );
SetTextColor( MyDc, CurrentColor[axdex] );
SetBkMode(    MyDc, TRANSPARENT );

if ( decimal_places < 0 )
    {
    decimal_places = 0;
    value = Axis[axdex].max;
    if ( value < 10000.0 )
        decimal_places = 1;
    if ( value < 1000.0 )
        decimal_places = 2;
    if ( value < 100.0 )
        decimal_places = 3;
    if ( value < 10.0 )
        decimal_places = 4;
    }

height = r.bottom - r.top;
range  = Axis[axdex].max - Axis[axdex].min;
for ( i=0; i<=nof_tics; i++ )
    {
    y = height * i;
    y /= nof_tics;
    y = r.bottom - y;
    value = Axis[axdex].min + range * float(i)/float(nof_tics);
    insalph( buf, value, 6, TEXT(' ') );
    *(buf+6) = NullChar;
    fix_decimals( buf, decimal_places );
    if ( labels_to_skip )
        labels_to_skip--;
    else
        TextOut( MyDc, x, y, buf, lstrlen(buf) );
    }

SetTextAlign( MyDc, oralign  );
SetTextColor( MyDc, oricolor );
SetBkMode(    MyDc, orbkmode );
}

/***********************************************************************
*                             DRAW_X_AXIS                              *
***********************************************************************/
static void draw_x_axis( MINMAX &limits, int decimal_places,
RECT & r, short nof_tics, short labels_to_skip, COLORREF textcolor )
{

int  i;
int  x;
int  y;
int  width;
int  dy;
double value;
double range;
TCHAR buf[MAX_DOUBLE_LEN+1];
UINT oralign;
UINT alignment;
UINT alignmask;

oralign = GetTextAlign( MyDc );

SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );
SetTextColor( MyDc, textcolor                      );

alignment = TA_CENTER | TA_TOP;
alignmask = TA_BASELINE | TA_TOP;

dy =  (short) MyCharHeight;

y = Region[POS_BASED_COUNT].rect.bottom;

width  = r.right - r.left;
range  = double(limits.max) - double(limits.min);
for ( i=0; i<=nof_tics; i++ )
    {
    if ( labels_to_skip )
        labels_to_skip--;
    else
        {
        SetTextAlign( MyDc, alignment );
        x = width * i;
        x /= nof_tics;
        x += r.left;
        value = double(limits.min) + range * double(i)/double(nof_tics);
        if ( double_to_tchar( buf, value, decimal_places ) )
            TextOut( MyDc, x, y+dy, buf, lstrlen(buf) );
        else
            lstrcpy( buf, TEXT("***") );
        alignment ^= alignmask;
        }
    }

SetTextAlign( MyDc, oralign );
}

/***********************************************************************
*                              SHOW_CURVE                              *
***********************************************************************/
void show_curve( short rindex, short yaxis, short xaxis, short ycurve, short xcurve, COLORREF color )
{
int32   i;
int     last_point;
int     pen_width;
int     n;

float * xp;
float * yp;

float   dif;
float * myx;
float * dest;

HPEN     oldpen;
HPEN     mypen;
RECT     r;

if ( !HaveHeader )
    return;

if ( !Region[rindex].is_visible )
    return;

r = Region[rindex].rect;

/*
-----------------------------------------------------------
Initialize the variables used to adjust the time based data
----------------------------------------------------------- */
dif = 0.0;

/*
-----------------------------------------------------------------------
If this is the time axis and I am overlaying and display is pos/time
then I need to adjust the data to begin at the p3 point of the original
plot. I am going to use the diff variable for this.
----------------------------------------------------------------------- */
if ( (overlaying() || viewing_reference()) && xaxis == TIME_AXIS && CurrentDisplay == POS_TIME_DISPLAY )
    {
    i = plot_curve_index( TIME_CURVE );
    dif = Axis[xaxis].min - Points[i][Ph.last_pos_based_point];
    }

pen_width = 1;
if ( CurvePenWidth > 0 )
    pen_width = CurvePenWidth;

if ( Printing && pen_width == 1 )
    pen_width = 2;

mypen = CreatePen( PS_SOLID, pen_width, color );
if ( mypen )
    {
    oldpen = (HPEN) SelectObject( MyDc, mypen );
    }

xp = Points[xcurve];
yp = Points[ycurve];

/*
-------------------------------------
Default to plotting all of the points
------------------------------------- */
i = 0;
last_point = Ph.n - 1;

/*
--------------------------------------------------------------------
If this is an analog curve, check to see if I need to plot all of it
-------------------------------------------------------------------- */
if ( !(Ph.curves[ycurve].type & (TIME_CURVE | POSITION_CURVE | VELOCITY_CURVE)) )
    {
    /*
    ------------------------------------------
    See if I should skip the pre impact points
    ------------------------------------------ */
    if ( !(Ph.curves[ycurve].type & PreImpactPlotCurveTypes) )
        {
        i = Ph.last_pos_based_point + 1;
        xp += i;
        yp += i;
        }
    /*
    -------------------------------------------
    See if I should skip the post impact points
    ------------------------------------------- */
    if ( !(Ph.curves[ycurve].type & PostImpactPlotCurveTypes) )
        last_point = Ph.last_pos_based_point;
    }

if ( rindex == POS_BASED_ANALOG || rindex == POS_BASED_COUNT )
    {
    if ( last_point > Ph.last_pos_based_point )
        last_point = Ph.last_pos_based_point;
    }

if ( last_point <= i )
    return;

n  = 1 + last_point - i;

/*
---------------------------------------------
I'm done using i so I can use it as a counter
---------------------------------------------  */
myx = 0;
if ( not_float_zero(dif) )
    {
    myx = new float[n];
    if ( myx )
        {
        dest = myx;
        i = n;
        while ( i )
            {
            *dest = *xp + dif;
            dest++;
            xp++;
            i--;
            }
        xp = myx;
        }
    }

plot_data( MyDc, r, Axis[xaxis].min, Axis[xaxis].max, Axis[yaxis].min, Axis[yaxis].max, xp, yp, n );

if ( myx )
    delete [] myx;

if ( mypen )
    {
    SelectObject( MyDc, oldpen );
    DeleteObject( mypen );
    }

}

/***********************************************************************
*                      CALCULATE_YAXIS_LIMITS                          *
*   This can now be called more than once for a given curve.           *
*   If the limits have been set before, they will be adjusted only.    *
***********************************************************************/
static void calculate_yaxis_limits( int curv, int yaxis )
{
int     n          = 0;
float * fp         = 0;
float * xp         = 0;
float   xmax       = 0.0;
float   xmin       = 0.0;
bool    need_check = false;

if ( Axis[yaxis].is_fixed )
    return;

n  = Ph.n;
fp = Points[curv];

if ( CurrentDisplay == TIME_ONLY_DISPLAY && Axis[TIME_AXIS].is_on_hold )
    {
    xp   = Points[TimeCurve];
    xmin = Axis[TIME_AXIS].min;
    xmax = Axis[TIME_AXIS].max;
    }
else if ( CurrentDisplay == POS_ONLY_DISPLAY && Axis[X_POS_AXIS].is_on_hold )
    {
    n    = Ph.last_pos_based_point+1;
    xp   = Points[PosCurve];
    xmin = Axis[X_POS_AXIS].min;
    xmax = Axis[X_POS_AXIS].max;
    }
else if ( yaxis == VELOCITY_AXIS && CurrentDisplay != TIME_ONLY_DISPLAY )
    {
    n    = Ph.last_pos_based_point+1;
    if ( Axis[X_POS_AXIS].is_on_hold )
        {
        xp   = Points[PosCurve];
        xmin = Axis[X_POS_AXIS].min;
        xmax = Axis[X_POS_AXIS].max;
        }
    else
        {
        xp = 0;
        }
    }
else if ( yaxis == Y_POS_AXIS )
    {
    xp   = Points[TimeCurve];
    xmin = Axis[TIME_AXIS].min;
    xmax = Axis[TIME_AXIS].max;
    }
else
    {
    n = 0;
    }

if ( n )
    {
    if ( Axis[yaxis].is_clear )
        {
        Axis[yaxis].min = Ph.curves[curv].max;
        Axis[yaxis].max = Ph.curves[curv].min;
        }

    while( n )
        {
        need_check = true;
        if ( xp )
            {
            need_check = xmin <= *xp && *xp <= xmax;
            xp++;
            }

        if ( need_check )
            {
            if ( *fp > Axis[yaxis].max )
               Axis[yaxis].max = *fp;

            if ( *fp < Axis[yaxis].min )
               Axis[yaxis].min = *fp;
            }
        fp++;
        n--;
        }
    }
else
    {
    if ( Ph.curves[curv].max > Axis[yaxis].max || Axis[yaxis].is_clear )
        Axis[yaxis].max = Ph.curves[curv].max;

    if ( Ph.curves[curv].min < Axis[yaxis].min || Axis[yaxis].is_clear )
        Axis[yaxis].min = Ph.curves[curv].min;
    }

/*
-----------------------------------------------
Make all unzoomed analog channels start at zero
if ( IsPressureDisplay && !zoomed() && FIRST_ANALOG_AXIS <= yaxis && yaxis <= LAST_ANALOG_AXIS )
----------------------------------------------- */
if ( !zoomed() && FIRST_ANALOG_AXIS <= yaxis && yaxis <= LAST_ANALOG_AXIS )
    {
    if ( Axis[yaxis].min > 0.0 )
        Axis[yaxis].min = 0.0;
    }

Axis[yaxis].is_clear = FALSE;
}

/***********************************************************************
*                        ADJUST_ALL_Y_AXIS_LIMITS                      *
***********************************************************************/
void adjust_all_y_axis_limits()
{
int  channel;
int  i;
int  x;

if ( !HaveHeader )
    return;

for ( channel=0; channel<NofPlotCurves; channel++ )
    {
    i = PlotCurve[channel].curve_index;
    if ( i == NO_INDEX )
        continue;

    x = FIRST_ANALOG_AXIS + channel;
    if ( !Axis[x].is_on_hold )
        calculate_yaxis_limits( i, x );
    }

i = plot_curve_index( VELOCITY_CURVE );
if ( i != NO_INDEX )
    {
    x = VELOCITY_AXIS;
    if ( !Axis[x].is_on_hold )
        calculate_yaxis_limits( i, x );
    }

i = plot_curve_index( POSITION_CURVE );
if ( i != NO_INDEX )
    {
    x = Y_POS_AXIS;
    if ( !Axis[x].is_on_hold )
        if ( CurrentDisplay == TIME_ONLY_DISPLAY || CurrentDisplay == POS_TIME_DISPLAY || Axis[TIME_AXIS].is_on_hold )
            calculate_yaxis_limits( i, x );
    }

}

/***********************************************************************
*                            SHOW_LEFT_AXES                            *
***********************************************************************/
void show_left_axes()
{

int  channel;
int  next_channel;
int  true_channel;
int  i;
int  x;
RECT r;
BOOLEAN firstime;
STRING_CLASS s1;
STRING_CLASS s2;
COLORREF     c1;
COLORREF     c2;

if ( !HaveHeader )
    return;

firstime = TRUE;

/*
-------------------------------------
Show the axes for the analog channels
channel is the index of PlotCurve.
i is the index of Ph.curves.
x is the index of the screen axis
------------------------------------- */
for ( channel=0; channel<2; channel++ )
    {
    if ( channel >= NofPlotCurves )
        break;

    i = PlotCurve[channel].curve_index;
    if ( i == NO_INDEX )
        continue;

    x = FIRST_ANALOG_AXIS + channel;
    r = Region[POS_BASED_ANALOG].rect;
    if ( firstime )
        {
        draw_axis( x,-1, r, TA_TOP, LEFT_AXIS_TYPE, 4, 1 );

        if ( IsPressureDisplay )
            {
            s1 = resource_string( PRESSURE_TYPE_STRING );
            s2 = units_name( Ph.curves[i].units );
            c1 = CurrentColor[TEXT_COLOR];
            c2 = c1;
            }
        else
            {
            true_channel = Ph.curves[i].ft_channel_number();
            if ( true_channel != NO_FT_CHANNEL )
                true_channel--;
            if ( i == DIFFERENTIAL_CURVE_INDEX )
                s1 = StringTable.find( TEXT("DIF_AXIS_LABEL_STRING") );
            else if ( i >= CHAN_1_ANALOG_CURVE_INDEX )
                s1 = ChannelAxisLabel[true_channel];
            if ( s1.isempty() )
                s1 = PlotCurve[channel].label;
            s1 += SpaceString;
            s1 += units_name( Ph.curves[i].units );

            c1 = CurrentColor[x];
            c2 = c1;
            next_channel = channel + 1;
            if ( next_channel < NofPlotCurves )
                {
                i = PlotCurve[next_channel].curve_index;
                true_channel = Ph.curves[i].ft_channel_number();
                if ( true_channel != NO_FT_CHANNEL )
                    true_channel--;
                if ( i == DIFFERENTIAL_CURVE_INDEX )
                    s2 = StringTable.find( TEXT("DIF_AXIS_LABEL_STRING") );
                else if ( i >= CHAN_1_ANALOG_CURVE_INDEX )
                    s2 = ChannelAxisLabel[true_channel];
                if ( s2.isempty() )
                    s2 = PlotCurve[next_channel].label;
                if ( i != NO_INDEX )
                    {
                    s2 += SpaceString;
                    s2 += units_name( Ph.curves[i].units );
                    }
                c2 = CurrentColor[x+1];
                }
            }
        y_axis_label( r, s1.text(), s2.text(), LEFT_AXIS_TYPE, c1, c2 );
        firstime = FALSE;
        }
    else
        {
        draw_axis( x,-1, r, TA_BASELINE, LEFT_AXIS_TYPE, 4, 1 );
        }
    }

/*
------------------------------------
Show the axis for the velocity curve
------------------------------------ */
i = plot_curve_index( VELOCITY_CURVE );
if ( i != NO_INDEX )
    {
    x = VELOCITY_AXIS;
    r = Region[POS_BASED_COUNT].rect;
    draw_axis( x,  2, r, TA_BASELINE, LEFT_AXIS_TYPE, 4, 0 );
    y_axis_label( r, resource_string(VELOCITY_TYPE_STRING), units_name(Ph.curves[i].units), LEFT_AXIS_TYPE );
    }
}

/***********************************************************************
*                            SHOW_RIGHT_AXES                           *
***********************************************************************/
void show_right_axes()
{
int  channel;
int  next_channel;
int  true_channel;
int  x;
BOOLEAN firstime;
STRING_CLASS s1;
STRING_CLASS s2;
COLORREF     c1;
COLORREF     c2;

RECT    r;
int     i;

if ( !HaveHeader )
    return;

firstime = TRUE;
for ( channel=2; channel<NofPlotCurves; channel++ )
    {
    i = PlotCurve[channel].curve_index;
    if ( i == NO_INDEX )
        continue;

    x = FIRST_ANALOG_AXIS + channel;
    r = Region[TIME_BASED_ANALOG].rect;
    if ( firstime )
        {
        draw_axis( x, -1, r, TA_TOP, RIGHT_AXIS_TYPE, 4, 1 );
        firstime = FALSE;

        if ( IsPressureDisplay )
            {
            s1 = resource_string( PRESSURE_TYPE_STRING );
            s2 = units_name( Ph.curves[i].units );
            c1 = CurrentColor[TEXT_COLOR];
            c2 = c1;
            }
        else
            {
            true_channel = Ph.curves[i].ft_channel_number();
            if ( true_channel != NO_FT_CHANNEL )
                true_channel--;
            if ( i == DIFFERENTIAL_CURVE_INDEX )
                s1 = StringTable.find( TEXT("DIF_AXIS_LABEL_STRING") );
            else if ( i >= CHAN_1_ANALOG_CURVE_INDEX )
                s1 = ChannelAxisLabel[true_channel];
            if ( s1.isempty() )
                s1 = PlotCurve[channel].label;
            s1 += SpaceString;
            s1 += units_name( Ph.curves[i].units );

            c1 = CurrentColor[x];
            c2 = c1;

            next_channel = channel + 1;
            if ( (next_channel) < NofPlotCurves )
                {
                i = PlotCurve[next_channel].curve_index;
                true_channel = Ph.curves[i].ft_channel_number();
                if ( true_channel != NO_FT_CHANNEL )
                    true_channel--;

                if ( i == DIFFERENTIAL_CURVE_INDEX )
                    s2 = StringTable.find( TEXT("DIF_AXIS_LABEL_STRING") );
                else if ( i >= CHAN_1_ANALOG_CURVE_INDEX )
                    s2 = ChannelAxisLabel[true_channel];
                if ( s2.isempty() )
                    s2 = PlotCurve[next_channel].label;

                if ( i != NO_INDEX )
                    {
                    s2 += SpaceString;
                    s2 += units_name( Ph.curves[i].units );
                    }
                c2 = CurrentColor[x+1];
                }
            }
        y_axis_label( r, s2.text(), s1.text(), RIGHT_AXIS_TYPE, c2, c1 );
        }
    else
        {
        draw_axis( x, -1, r, TA_BASELINE, RIGHT_AXIS_TYPE, 4, 1 );
        }
    }

/*
-----------------------
Show the position curve
----------------------- */
i = plot_curve_index( POSITION_CURVE );
if ( i != NO_INDEX )
    {
    x = Y_POS_AXIS;

    if ( !Axis[x].is_on_hold )
        {
        r = Region[TIME_BASED_COUNT].rect;
        if ( CurrentDisplay != POS_ONLY_DISPLAY )
            draw_axis( x,     2, r, TA_BASELINE, RIGHT_AXIS_TYPE, 4, 0 );

        y_axis_label( r, resource_string(POSITION_TYPE_STRING), units_name(Ph.curves[i].units), RIGHT_AXIS_TYPE );
        }

    }
}

/***********************************************************************
*                            SHOW_X_AXES                               *
***********************************************************************/
void show_x_axes()
{
RECT    r;
int     i;
UINT    id;
int     decimal_places;
short   nof_tics;
MINMAX  range;
float   x;
TCHAR   pos_x_label[13 + UNITS_LEN];
STRING_CLASS s;

if ( !HaveHeader )
    return;

for ( i=0; i<MAX_CURVES; i++ )
    {
    if ( Ph.curves[i].type & POSITION_CURVE )
        {
        if ( CurrentDisplay == TIME_ONLY_DISPLAY )
            continue;

        if ( !Axis[X_POS_AXIS].is_on_hold )
            {
            Axis[X_POS_AXIS].min = Ph.curves[i].min;
            Axis[X_POS_AXIS].max = Points[i][Ph.last_pos_based_point];
            }

        lstrcpy( pos_x_label, resource_string(POSITION_TYPE_STRING) );
        lstrcat( pos_x_label, TEXT(" (") );
        lstrcat( pos_x_label, units_name(Ph.curves[i].units) );
        lstrcat( pos_x_label, TEXT(")") );

        range.max = Axis[X_POS_AXIS].max;
        range.min = Axis[X_POS_AXIS].min;

        if ( range.max >= 1000.0 )
            decimal_places = 1;
        else if ( range.max < 10.0 )
            decimal_places = 3;
        else
            decimal_places = 2;

        r = Region[POS_BASED_COUNT].rect;
        nof_tics = POS_BASED_X_TICS;
        if ( CurrentDisplay != POS_TIME_DISPLAY )
            nof_tics += TIME_BASED_X_TICS;
        draw_x_axis( range, decimal_places, r, nof_tics, 0, CurrentColor[X_POS_AXIS] );
        x_axis_label( r, pos_x_label );
        }

    if ( Ph.curves[i].type == TIME_CURVE )
        {
        if ( CurrentDisplay == POS_ONLY_DISPLAY )
            continue;

        if ( !Axis[TIME_AXIS].is_on_hold )
            {
            Axis[TIME_AXIS].max = Ph.curves[i].max;
            if ( CurrentDisplay != TIME_ONLY_DISPLAY )
                {
                /*
                -------------------------------------------
                Set the time axis min to the time at impact
                ------------------------------------------- */
                Axis[TIME_AXIS].min = Points[i][Ph.last_pos_based_point];

                /*
                ------------------------------------------------
                Use the post impact display time if there is one
                ------------------------------------------------ */
                if ( PostImpactDisplayTime > 0.0 )
                    {
                    x = Axis[TIME_AXIS].min + PostImpactDisplayTime;
                    if ( x < Axis[TIME_AXIS].max )
                        Axis[TIME_AXIS].max = x;
                    }
                }
            else
                {
                Axis[TIME_AXIS].min = 0.0;
                }
            }

        range.max = Axis[TIME_AXIS].max;
        range.min = Axis[TIME_AXIS].min;

        nof_tics = TIME_BASED_X_TICS;
        id = SEC_TIME_LABEL_STRING;

        if ( CurrentDisplay == POS_TIME_DISPLAY )
            {
            /*
            --------------------------------
            Display the axis in milliseconds
            -------------------------------- */
            range.max -= range.min;
            range.min = 0.0;
            range.max *= 1000.0;
            decimal_places = 0;
            id = MS_TIME_LABEL_STRING;
            }
        else
            {
            decimal_places = 3;
            nof_tics += POS_BASED_X_TICS;
            }

        r = Region[TIME_BASED_COUNT].rect;
        draw_x_axis( range, decimal_places, r, nof_tics, 0, CurrentColor[TIME_AXIS] );
        s = resource_string( id );
        x_axis_label( r, s.text() );
        }
    }

}

/***********************************************************************
*                           CREATE_BIGFONT                             *
***********************************************************************/
static void create_bigfont( HDC dc )
{
const TCHAR big_number[] = TEXT( "0000.00" );
const TCHAR gap_text[]   = TEXT( "WW" );

long  height;
long  width;
long  max_height;

int32 i;
int32 j;
int32 n;
int32 y;
int32 gap_len;
int32 name_len;
int32 number_len;
int32 navg;
int32 slen;
float value;
COLORREF my_text_color;
STRING_CLASS s;

HFONT      BigFont;
SIZE       textsize;
TEXTMETRIC tm;

slen = lstrlen( big_number );
GetTextExtentPoint32( dc, big_number, slen, &textsize );
number_len = textsize.cx;

slen = lstrlen( gap_text );
GetTextExtentPoint32( dc, gap_text, slen, &textsize );
gap_len = textsize.cx;

name_len = 0;
n = 0;
/*
------------------------------------------------------------
Get the length of the longest parameter name plus the number
------------------------------------------------------------ */
for ( j=0; j<NofCornerParameters; j++ )
    {
    i = CornerParameters[j];
    if ( i > 0 && i < MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            {
            slen = lstrlen( CurrentParam.parameter[i].name );
            GetTextExtentPoint32( dc, CurrentParam.parameter[i].name, slen, &textsize );
            if ( textsize.cx > name_len )
                name_len = textsize.cx;
            n++;
            }
        }
    }

/*
----------------------------------------------------------
Calculate the number of average characters this represents
---------------------------------------------------------- */
GetTextMetrics( dc, &tm );
navg = (name_len + gap_len + number_len) / tm.tmAveCharWidth;

/*
--------
Round up
-------- */
navg++;

if ( n <= 0 )
    n = 1;

/*
----------------------------------------------------------
Now, calculate the size of the largest average char to fit
---------------------------------------------------------- */
width = (PlotWindowRect.right - PlotWindowRect.left ) / navg;
height          = (PlotWindowRect.bottom - PlotWindowRect.top) / n;

max_height = 3*width + width/2;
if ( height > max_height )
    height = max_height;

BigFont = create_font( dc, width, height, ArialBoldFontName.text() );

if ( !BigFont )
    return;

SelectObject( dc, BigFont );
GetTextMetrics( dc, &tm );

SetBkColor(   dc, CurrentColor[BACKGROUND_COLOR] );
y = PlotWindowRect.top + tm.tmHeight;

for ( j=0; j<NofCornerParameters; j++ )
    {
    i = CornerParameters[j];
    if ( i > 0 && i < MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            {
            SetTextColor( dc, CurrentColor[TEXT_COLOR]       );
            SetTextAlign( dc, TA_BOTTOM | TA_LEFT );
            slen = lstrlen( CurrentParam.parameter[i].name );
            TextOut( dc, PlotWindowRect.left, y, CurrentParam.parameter[i].name, slen );

            SetTextAlign( dc, TA_BOTTOM | TA_RIGHT );
            my_text_color = CurrentColor[TEXT_COLOR];
            if ( CurrentParam.parameter[i].vartype == TEXT_VAR )
                {
                s = Parms[i];
                }
            else
                {
                value = Parms[i].float_value();
                s.upsize( MAX_DOUBLE_LEN * 2 );
                rounded_double_to_tchar( s.text(), (double) value );
                if ( is_alarm(i, value) )
                    my_text_color = AlarmColor;
                else if ( is_warning(i, value) )
                    my_text_color = WarningColor;
                }

            SetTextColor( dc, my_text_color );
            TextOut( dc, PlotWindowRect.right, y, s.text(), s.len() );

            y += tm.tmHeight;
            }
        }
    }

SelectObject( dc, GetStockObject(SYSTEM_FIXED_FONT) ) ;
DeleteObject( BigFont );
}

/***********************************************************************
*                             LOAD_MYFONT                              *
*                                                                      *
*                    The rectangle is the plot window.                 *
*                                                                      *
***********************************************************************/
static void load_myfont()
{

long       lines_per_page;
long       height;
HFONT      f;
TEXTMETRIC tm;

height = PlotWindowRect.bottom - PlotWindowRect.top;
if ( height < 1000 )
    lines_per_page = 25L;
else
    lines_per_page = 35L;

height /= lines_per_page;

f = create_font( MyDc, 0, height, ArialFontName.text() );

if ( f )
    {
    /*
    ---------------------------------------------------------------
    The following gives me a default and assures that MyFont is not
    currently selected (it is bad form to delete the current font).
    --------------------------------------------------------------- */
    SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) ) ;

    if ( MyFont )
        DeleteObject( MyFont );

    MyFont = f;
    SelectObject( MyDc, MyFont );
    GetTextMetrics( MyDc, &tm );
    MyCharHeight = tm.tmHeight - tm.tmInternalLeading;
    MyCharWidth  = tm.tmAveCharWidth;
    }
}

/***********************************************************************
*                               X_VALUE                                *
***********************************************************************/
BOOLEAN x_value( float & dest, int & axis_index, int screen_x )
{

int    ax;
RECT * r;
float  xrange;

ax = NO_AXIS;
r    = 0;

if ( Region[POS_BASED_COUNT].is_visible )
    {
    if ( screen_x >= Region[POS_BASED_COUNT].rect.left && screen_x <= Region[POS_BASED_COUNT].rect.right )
        {
        ax = X_POS_AXIS;
        r  = &Region[POS_BASED_COUNT].rect;
        }
    }

if ( Region[TIME_BASED_COUNT].is_visible )
    {
    if ( screen_x >= Region[TIME_BASED_COUNT].rect.left && screen_x <= Region[TIME_BASED_COUNT].rect.right )
        {
        ax = TIME_AXIS;
        r  = &Region[TIME_BASED_COUNT].rect;
        }
    }

if ( ax == NO_AXIS )
    return FALSE;

axis_index = ax;
dest   = float( screen_x );
xrange = float( r->right - r->left );
dest  -= r->left;
dest  *= Axis[ax].max - Axis[ax].min;

if ( not_float_zero(xrange) )
    dest /= xrange;

dest += Axis[ax].min;

return TRUE;
}

/***********************************************************************
*                            SHOW_SHOT_INFO                            *
***********************************************************************/
void show_shot_info()
{
TCHAR * cp;
POINT   p;
SIZE    sz;
int32   width;
STRING_CLASS s;
STRING_CLASS myshotname;

myshotname = ShotName;
if ( !myshotname.isempty() )
    {
    cp = myshotname.text();
    while ( *cp == SpaceChar || *cp == ZeroChar )
        remove_char( cp );
    if ( *cp == NullChar )
        myshotname = ZeroChar;
    }

if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
    {
    s = MachineName;
    s += TEXT(" : ");
    s += PartName;
    s += TEXT(" : ");
    s += resource_string( SHOT_LABEL_STRING );
    s += TEXT(" ");
    s += myshotname;
    s += TEXT(" : ");
    s += DateString;

    p.y = PlotWindowRect.top + 2;
    if ( !Printing && GetTextExtentPoint32(MyDc, s.text(), s.len(), &sz) )
        {
        SetTextAlign( MyDc, TA_TOP | TA_LEFT );
        width = (PlotWindowRect.right - PlotWindowRect.left);
        width -= sz.cx;
        width /= 2;
        p.x = PlotWindowRect.left + width;
        }
    else
        {
        SetTextAlign( MyDc, TA_TOP | TA_CENTER );
        p.x = (PlotWindowRect.left + PlotWindowRect.right)/2;
        }


    SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );
    SetTextColor( MyDc, CurrentColor[TEXT_COLOR] );

    TextOut( MyDc, p.x, p.y, s.text(), s.len() );
    }
}

/***********************************************************************
*                           SHOW_OFFLINE_INFO                          *
***********************************************************************/
void show_offline_info()
{
POINT p;
SIZE  sz;
int32 width;
STRING_CLASS s;

if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
    {
    s = MachineName;
    s += TEXT(" : ");
    s += resource_string( OFF_LINE_STRING );

    p.y = PlotWindowRect.top + 2;
    if ( !Printing && GetTextExtentPoint32(MyDc, s.text(), s.len(), &sz) )
        {
        SetTextAlign( MyDc, TA_TOP | TA_LEFT );
        width = (PlotWindowRect.right - PlotWindowRect.left);
        width -= sz.cx;
        width /= 2;
        p.x = PlotWindowRect.left + width;
        }
    else
        {
        SetTextAlign( MyDc, TA_TOP | TA_CENTER );
        p.x = (PlotWindowRect.left + PlotWindowRect.right)/2;
        }


    SetBkColor(   MyDc, CurrentColor[BACKGROUND_COLOR] );
    SetTextColor( MyDc, CurrentColor[TEXT_COLOR] );

    TextOut( MyDc, p.x, p.y, s.text(), s.len() );
    }
}

/***********************************************************************
*                              FIND_INDEX                              *
* This routine finds the index that corresponds to a x VALUE in an     *
* axis region (axis_index). Axis_index is either X_POS_AXIS or         *
* TIME_AXIS. The "i" value is the previous index. This is used to      *
* speed up the search, assuming I'm not far from where I was before.   *
*                                                                      *
* If I have changed regions from time to pos then I should set the     *
* previous index to the last position based point.                     *
*                                                                      *
***********************************************************************/
int find_index( float xpos, int axis_index, int i )
{

int     increment;
int     min_index;
float * p;
float   min_delta;
float   delta;

if ( axis_index == X_POS_AXIS )
    {
    p = Points[POSITION_CURVE_INDEX];
    if ( i > Ph.last_pos_based_point )
        i = Ph.last_pos_based_point;
    }
else
    {
    p = Points[TIME_CURVE_INDEX];
    }

min_index = i;
min_delta = absolute_value( xpos - p[i] );
increment = 1;
if ( p[i] > xpos )
    increment = -1;

while ( i >= 0 && i < NofPoints )
    {
    delta = absolute_value( xpos - p[i] );
    if ( delta <= min_delta )
        {
        min_delta = delta;
        min_index = i;
        }
    else
        break;
    i += increment;
    }

return min_index;
}

/***********************************************************************
*                              HIGH_INDEX                              *
*                                                                      *
*  Return the upper index of the first index pair that contain the     *
*  target point. high_index in [ 1, NofPoints ).                       *
*                                                                      *
*  h = high_index( 1.0, X_POS_AXIS );                                  *
*  if ( h != NO_INDEX )                                                *
*      1.0 is contained in the interval from point h-1 to h (points    *
*      start at 0).                                                    *
*                                                                      *
***********************************************************************/
int high_index( float target, int axis_index )
{
int     i;
float * p;
float   x1;
float   x2;

if ( axis_index == X_POS_AXIS )
    p = Points[POSITION_CURVE_INDEX];
else
    p = Points[TIME_CURVE_INDEX];

i = 0;
while ( i < NofPoints )
    {
    x1 = p[i];
    i++;
    x2 = p[i];

    if ( x2 < x1 )
        swap( x1, x2 );

    if ( target >= x1 && target <= x2 )
        break;
    }

if ( i >= NofPoints )
    return NO_INDEX;

return i;
}

/***********************************************************************
*                             TIME_OF_POS                              *
***********************************************************************/
static BOOLEAN time_of_pos( float & target )
{
float xmax;
float xmin;
float xrange;
float percentage;
float * p;
int   i;

i = high_index( target, X_POS_AXIS );
if ( i == NO_INDEX )
    return FALSE;

/*
------------------------------------------------------------
Positions are always pre-impact. If I am past impact, backup
------------------------------------------------------------ */
//minint( i, Ph.last_pos_based_point );

p = Points[POSITION_CURVE_INDEX];

xmax   = p[i];
xmin   = p[i-1];
xrange = xmax - xmin;

p = Points[TIME_CURVE_INDEX];

if ( is_float_zero(xrange) )
    {
    target = p[i];
    return TRUE;
    }

percentage = (target - xmin) /xrange;

xmin   = p[i-1];
xrange = p[i] - xmin;

target = xmin + percentage * xrange;

return TRUE;
}

/***********************************************************************
*                             POS_OF_TIME                              *
***********************************************************************/
static BOOLEAN pos_of_time( float & target )
{
float xmax;
float xmin;
float xrange;
float percentage;
float * p;
int   i;

/*
--------------------------------------------------------------------
Times are always post-impact so start at the first time-based point.
-------------------------------------------------------------------- */
p = Points[TIME_CURVE_INDEX];
i = Ph.last_pos_based_point + 1;
while ( i < NofPoints )
    {
    if ( target <= p[i] )
        break;
    i++;
    }

minint( i, NofPoints - 1 );

xmax   = p[i];
xmin   = p[i-1];
xrange = xmax - xmin;

p = Points[POSITION_CURVE_INDEX];

if ( is_float_zero(xrange) )
    {
    target = p[i];
    return TRUE;
    }

percentage = (target - xmin) /xrange;

xmin   = p[i-1];
xrange = p[i] - xmin;

target = xmin + percentage * xrange;

return TRUE;
}

/***********************************************************************
*                           SHOW_ZOOM_VALUE                            *
***********************************************************************/
void show_zoom_value( int text_box_id, BITSETYPE curve_type, int i1, int i2, short decimal_places )
{
TCHAR s[MAX_FLOAT_LEN+1];
float x1;
float x2;
HWND w;
int  curve_index;

if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == TREND_SCREEN_TYPE || MyScreenType == BIG_PARAM_SCREEN_TYPE )
    return;

if ( MyScreenType == STATUS_SCREEN_TYPE )
    return;

curve_index = plot_curve_index( curve_type );
if ( curve_index == NO_INDEX )
    return;

if ( MyScreenType == ZABISH_SCREEN_TYPE )
    w = ZoomBoxWindow;
else
    w = SmallZoomBoxWindow;

x1 = Points[curve_index][i1];
x2 = Points[curve_index][i2];

ftoasc( s, x1 );
fix_decimals( s, decimal_places );
SetDlgItemText( w, text_box_id+START_BOX_OFFSET, s  );

ftoasc( s, x2 );
fix_decimals( s, decimal_places );
SetDlgItemText( w, text_box_id+END_BOX_OFFSET, s  );

ftoasc( s, x2-x1 );
fix_decimals( s, decimal_places );
SetDlgItemText( w, text_box_id+DELTA_BOX_OFFSET, s  );
}

/***********************************************************************
*                            SHOW_ZOOM_INFO                            *
***********************************************************************/
void show_zoom_info()
{
int   ax;
float lx;
int   i;
int   i1;
int   i2;
short decimal_places;
HWND  w;

if ( NeedToUpdateZoomLabels )
    {
    if ( MyScreenType == ZABISH_SCREEN_TYPE )
        w = ZoomBoxWindow;
    else
        w = SmallZoomBoxWindow;
    set_zoom_box_labels( w );
    }

if ( x_value(lx, ax, LeftCursorX) )
    {
    LeftZoomIndex = find_index( lx, ax, LeftZoomIndex );
    if ( x_value(lx, ax, CursorX) )
        {
        ZoomIndex = find_index( lx, ax, ZoomIndex );
        }
    }

if ( LeftCursorX <= CursorX )
    {
    i1 = LeftZoomIndex;
    i2 = ZoomIndex;
    }
else
    {
    i1 = ZoomIndex;
    i2 = LeftZoomIndex;
    }

decimal_places = 4;
//if ( CurrentPart.distance_units == INCH_UNITS )
  //decimal_places = 2;

show_zoom_value( POS_TEXT_BOX,      POSITION_CURVE,      i1, i2, decimal_places );
show_zoom_value( TIME_TEXT_BOX,     TIME_CURVE,          i1, i2, 3 );
show_zoom_value( VELOCITY_TEXT_BOX, VELOCITY_CURVE,      i1, i2, 3 );

/*
----------------------------------------------------------------------------
if ( IsPressureDisplay )
    {
    show_zoom_value( ROD_TEXT_BOX,      ROD_PRESSURE_CURVE,  i1, i2, 2 );
    show_zoom_value( HEAD_TEXT_BOX,     HEAD_PRESSURE_CURVE, i1, i2, 2 );
    if ( CurrentMachine.diff_curve_number != NO_DIFF_CURVE_NUMBER )
        show_zoom_value( DIFF_TEXT_BOX,     DIFF_PRESSURE_CURVE, i1, i2, 2 );
    }
else
    {
    }
---------------------------------------------------------------------------- */
for ( i=0; i<NofZoomAnalogChannels; i++ )
    {
    if ( ZoomAnalogType[i] )
        show_zoom_value( AnalogTbox[i], ZoomAnalogType[i], i1, i2, 2 );
    }
}

/***********************************************************************
*                        SHOW_CORNER_PARAMETERS                        *
***********************************************************************/
static void show_corner_parameters()
{
if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == TREND_SCREEN_TYPE || MyScreenType == BIG_PARAM_SCREEN_TYPE )
    return;

if ( MyScreenType == STATUS_SCREEN_TYPE )
    return;

InvalidateRect( ParameterWindow, 0, TRUE );
}

/***********************************************************************
*                              DRAW_MARK                               *
***********************************************************************/
void draw_mark( FLOAT target, int ax, MARKLIST_ENTRY  * me )
{
COLORS_CLASS c;

int    y;
int    penwidth;
int    rx;
int    ix;
int    slen;
UINT   align_flags;
float  xrange;
float  x;
float  xmax;
float  xmin;
double d;
HPEN   mypen;
HPEN   oldpen;

const COLORREF white_color = RGB( 255, 255, 255 );
const COLORREF black_color = RGB(   0,   0,   0 );
COLORREF pencolor;


if ( ax == X_POS_AXIS )
    rx = POS_BASED_COUNT;
else
    rx = TIME_BASED_COUNT;

xmax = Axis[ax].max;
xmin = Axis[ax].min;

xrange = xmax - xmin;

if ( is_float_zero(xrange) )
    return;

if ( target < xmin )
    {
    if ( zoomed() )
        return;
    ix = Region[rx].rect.left;
    }
else if ( xmax < target )
    {
    if ( zoomed() )
        return;
    ix = Region[rx].rect.right;
    }
else
    {
    /*
    --------------------------------------
    target - xmin
    ------------- * (right - left) + left;
     xmax - xmin
    -------------------------------------- */

    x = Region[rx].rect.right - Region[rx].rect.left;
    x *= target - xmin;
    x /= xrange;
    x += Region[rx].rect.left;
    ix = (int) x;
    }

penwidth = 1;
if ( me->symbol & BLINE_MARK )
    {
    penwidth = MyCharWidth / 2;
    maxint( penwidth, 1 );
    }

if ( MyDc )
    {
    pencolor = c[me->color];
    if ( Printing && pencolor == white_color )
        pencolor = black_color;
    mypen = CreatePen( PS_SOLID, penwidth, pencolor );
    if ( mypen )
        {
        oldpen = (HPEN) SelectObject( MyDc, mypen );
        slen   = lstrlen( me->label );
        if ( me->symbol & VLINE_MARK )
            {
            MoveToEx( MyDc, ix, Region[POS_BASED_ANALOG].rect.top,  0 );
            LineTo(   MyDc, ix, Region[POS_BASED_COUNT].rect.bottom   );
            }
        else if ( (me->symbol & BLINE_MARK) && !slen )
            {
            y = Region[POS_BASED_COUNT].rect.top - Region[POS_BASED_COUNT].rect.bottom;
            y /= 30;
            y += Region[POS_BASED_COUNT].rect.bottom;

            MoveToEx( MyDc, ix, y,  0 );
            LineTo(   MyDc, ix, Region[POS_BASED_COUNT].rect.bottom   );
            }

        if ( me->symbol & (VLINE_MARK | NOLINE_MARK | BLINE_MARK) )
            {
            slen = lstrlen( me->label );
            if ( slen )
                {
                if ( me->symbol & VLINE_MARK )
                    align_flags = TA_BOTTOM | TA_LEFT;
                else
                    align_flags = TA_BOTTOM | TA_CENTER;
                SetTextAlign( MyDc, align_flags );
                SetBkMode(    MyDc, TRANSPARENT );
                SetTextColor( MyDc, pencolor );
                if ( me->symbol & BLINE_MARK )
                    {
                    y = Region[POS_BASED_COUNT].rect.bottom;
                    d = (double) (Region[POS_BASED_COUNT].rect.bottom - Region[POS_BASED_COUNT].rect.top);
                    }
                else
                    {
                    y = Region[POS_BASED_ANALOG].rect.bottom;
                    d = (double) (Region[POS_BASED_ANALOG].rect.bottom - Region[POS_BASED_ANALOG].rect.top);;
                    }

                d *= me->offset.real_value();
                d /= 4.0;
                d = round( d, 1.0 );
                y -= (int) d;
                TextOut( MyDc, ix, y, me->label, slen );
                if ( me->symbol & NOLINE_MARK )
                    {
                    y += MyCharHeight/4;
                    TextOut( MyDc, ix, y, TEXT("."), 1 );
                    }
                }
            }
        SelectObject( MyDc, oldpen );
        DeleteObject( mypen );
        }
    }

}

/***********************************************************************
*                            SHOW_POS_MARK                             *
***********************************************************************/
static void show_pos_mark( float target, MARKLIST_ENTRY * me )
{

int   ax;

ax = X_POS_AXIS;

if ( CurrentDisplay == TIME_ONLY_DISPLAY )
    {
    if ( !time_of_pos(target) )
        return;
    ax = TIME_AXIS;
    }

draw_mark( target, ax, me );
}

/***********************************************************************
*                            SHOW_TIME_MARK                            *
***********************************************************************/
static void show_time_mark( float target, MARKLIST_ENTRY * me, PARAMETER_ENTRY * pe )
{

int   ax;

/*
-------------------------------------------
Time-based parameters are in ms past impact
Convert this to seconds from start.
------------------------------------------- */
target = adjust_for_units( SECOND_UNITS, target, pe->units );
target += Points[TIME_CURVE_INDEX][Ph.last_pos_based_point];

ax = TIME_AXIS;

if ( CurrentDisplay == POS_ONLY_DISPLAY )
    {
    if ( !pos_of_time(target) )
        return;
    ax = X_POS_AXIS;
    }

draw_mark( target, ax, me );
}

/***********************************************************************
*                              SHOW_MARKS                              *
***********************************************************************/
static void show_marks()
{
short i;
short pi;
float value;
MARKLIST_ENTRY  * me;
PARAMETER_ENTRY * pe;

for ( i=0; i<CurrentMarks.n; i++ )
    {
    me =  CurrentMarks.array+i;
    switch ( me->parameter_type )
        {
        case PARAMETER_MARK:
            pi = me->parameter_number - 1;
            if ( pi < 0 || pi >= MAX_PARMS )
                continue;
            value = Parms[pi].float_value();
            pe = CurrentParam.parameter+pi;
            if ( pe->vartype & POSITION_VAR )
                {
                show_pos_mark( value, me );
                }
            else if ( pe->vartype & TIME_VAR )
                {
                show_time_mark( value, me, pe );
                }
            break;

        case START_POS_MARK:
             show_pos_mark( (float) CurrentPart.user_avg_velocity_start[me->parameter_number-1], me );
            break;

        case END_POS_MARK:
             show_pos_mark( (float) CurrentPart.user_avg_velocity_end[me->parameter_number-1], me );
            break;

        case VEL_POS_MARK:
             show_pos_mark( (float) CurrentPart.user_velocity_position[me->parameter_number-1], me );
            break;
        }
    }
}

/***********************************************************************
*                SHOW_ALARM_STATE_IN_LOWER_LEFT_CORNER                 *
* Put a "warning" or "alarm" if any of the parameters are out of spec. *
***********************************************************************/
static void show_alarm_state_in_lower_left_corner()
{
const UINT align_flags = TA_BASELINE | TA_LEFT;

COLORREF     old_background_color;
INT          old_background_mode;
HFONT        old_font;
STRING_CLASS s;
int          state;
int          x;

state = get_alarm_state();
if ( state == NO_ALARM_STATE && !ShowWorker )
    return;

old_font = (HFONT) SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) );

SetTextAlign( MyDc, align_flags );
old_background_mode = SetBkMode( MyDc, OPAQUE );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );

if ( state != NO_ALARM_STATE )
    {
    if ( state == ALARM_STATE )
        {
        old_background_color = SetBkColor( MyDc, AlarmColor );
        s = StringTable.find( TEXT("ALARM_STRING") );
        }
    else
        {
        old_background_color = SetBkColor( MyDc, WarningColor );
        s = StringTable.find( TEXT("WARNING_STRING") );
        }

    TextOut( MyDc, PlotWindowRect.left, PlotWindowRect.bottom-3, s.text(), s.len() );
    }

if ( ShowWorker )
    {
    SetBkMode( MyDc, TRANSPARENT );
    x = PlotWindowRect.left;
    if ( !s.isempty() )
        {
        x += (int) pixel_width( MyDc, s.text() );
        x += 3 * average_character_width( MyDc );
        }
    TextOut( MyDc, x, PlotWindowRect.bottom-3, CurrentWorker.text(), CurrentWorker.len() );
    }

SetBkColor( MyDc, old_background_color );
SetBkMode( MyDc, old_background_mode );
SelectObject( MyDc, old_font );
}

/***********************************************************************
*                           PAINT_BACKGROUND                           *
***********************************************************************/
static void paint_background()
{
const int count = 4;
const int rindex[] = { POS_BASED_ANALOG, POS_BASED_COUNT, TIME_BASED_ANALOG, TIME_BASED_COUNT };

int i;
int ri;

for ( ri=0; ri<count; ri++ )
    {
    i = rindex[ri];
    draw_grid( i );
    }

show_x_axes();
adjust_all_y_axis_limits();

if ( viewing_reference() )
    {
    if (load_reference_trace() )
        {
        adjust_all_y_axis_limits();
        load_profile();
        }
    }

show_left_axes();
show_right_axes();

if ( IsPressureDisplay )
    rod_head_label();

show_alarm_state_in_lower_left_corner();
}

/***********************************************************************
*                           PAINT_THE_CURVES                           *
***********************************************************************/
static void paint_the_curves()
{

int pi;
int i;
short yaxis;

yaxis = FIRST_ANALOG_AXIS;

for ( pi=0; pi<NofPlotCurves; pi++ )
    {
    i = PlotCurve[pi].curve_index;

    if ( i == NO_INDEX )
        continue;

    show_curve( POS_BASED_ANALOG,  yaxis, X_POS_AXIS, i, PosCurve,  CurrentColor[pi+FIRST_ANALOG_AXIS] );
    show_curve( TIME_BASED_ANALOG, yaxis,  TIME_AXIS, i, TimeCurve, CurrentColor[pi+FIRST_ANALOG_AXIS] );

    yaxis++;
    }

if ( Region[POS_BASED_COUNT].is_visible )
    show_curve( POS_BASED_COUNT, VELOCITY_AXIS, X_POS_AXIS, VELOCITY_CURVE_INDEX, PosCurve, CurrentColor[VELOCITY_AXIS] );

if ( Region[TIME_BASED_COUNT].is_visible )
    {
    show_curve( TIME_BASED_COUNT, Y_POS_AXIS, TIME_AXIS, POSITION_CURVE_INDEX, TimeCurve, CurrentColor[Y_POS_AXIS] );
    if ( CurrentDisplay == TIME_ONLY_DISPLAY )
        show_curve( TIME_BASED_COUNT, VELOCITY_AXIS, TIME_AXIS, VELOCITY_CURVE_INDEX, TimeCurve, CurrentColor[VELOCITY_AXIS] );
    }
}

/***********************************************************************
*                             SHOW_OVERLAYS                            *
***********************************************************************/
static void show_overlays()
{
int i;

if ( overlaying() && NofOverlayShots > 1 )
    {
    for ( i=1; i<NofOverlayShots; i++ )
        {
        load_profile( OverlayShot+i );
        paint_the_curves();
        }
    i = NofOverlayShots - 1;
    load_part_info( OverlayShot+i );
    }
}

/***********************************************************************
*                       SET_REALTIME_MENU_STRING                       *
***********************************************************************/
void set_realtime_menu_string()
{
UINT         id;
STRING_CLASS s;
SIZE         textsize;
HDC          dc;
HBRUSH       oldbrush;
HBRUSH       mybrush;
INT          orbkmode;
COLORREF     mycolor;

if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == BIG_PARAM_SCREEN_TYPE || MyScreenType == DOWNBAR_SCREEN_TYPE )
    return;

if ( MyScreenType == STATUS_SCREEN_TYPE )
    return;

if ( RealTime )
    {
    if ( SingleMachineOnly )
        id = REALTIME_SINGLE_MENU_STRING;
    else
        id = REALTIME_ON_MENU_STRING;
    }
else
    {
    id = REALTIME_OFF_MENU_STRING;
    }

s = resource_string( id );

dc = GetDC( MainPlotWindow );

if ( RealTimeButtonRect.right > 0 )
    {
    mybrush  = CreateSolidBrush( CurrentColor[BACKGROUND_COLOR] );
    oldbrush = (HBRUSH) SelectObject( dc, mybrush );
    FillRect( dc, &RealTimeButtonRect, mybrush );
    SelectObject( dc, oldbrush );
    DeleteObject( mybrush );
    }

if ( (MainWindowRect.right) > 640 || !MyFont )
    SelectObject( dc, GetStockObject(DEFAULT_GUI_FONT) ) ;
else
    SelectObject( dc, MyFont );

GetTextExtentPoint32( dc, s.text(), s.len(), &textsize );

RealTimeButtonRect.top    = 1;
RealTimeButtonRect.left   = 0;
RealTimeButtonRect.right  = textsize.cx + 3;
RealTimeButtonRect.bottom = RealTimeButtonRect.top + textsize.cy - 3;
if ( (MainWindowRect.right) > 640 )
    RealTimeButtonRect.bottom++;

if ( id == REALTIME_OFF_MENU_STRING )
    mycolor = AlarmColor;
else
    mycolor = GetSysColor( COLOR_MENU );

mybrush  = CreateSolidBrush( mycolor );
oldbrush = (HBRUSH) SelectObject( dc, mybrush );
FillRect( dc, &RealTimeButtonRect, mybrush );
SelectObject( dc, oldbrush );
DeleteObject( mybrush );

orbkmode = GetBkMode( dc );
SetBkMode( dc, TRANSPARENT );

SetTextAlign( dc, TA_TOP | TA_LEFT );
SetTextColor( dc, RGB(0, 0, 0) );
TextOut( dc, RealTimeButtonRect.left+1, RealTimeButtonRect.top - 1, s.text(), s.len() );

SetBkMode( dc, orbkmode );
ReleaseDC( MainPlotWindow, dc );
}

/**********************************************************************
*                        UPDATE_DAY_PARAM_DISPLAY                     *
*        Figure the date from the editbox and load the listbox.       *
**********************************************************************/
static void update_day_param_display()
{
static TCHAR separator[] = TEXT( " : " );

TIME_CLASS   closetime;
long         delta;
int          i;
int          j;
int          last_corner_parameter_index;
int          new_day;
bool         openok;
TIME_CLASS   opentime;
int          today;
VDB_CLASS    vdb;
SYSTEMTIME   st;
SYSTEMTIME   nt;
STRING_CLASS s;
NAME_CLASS   name;
TIME_CLASS   t;

t.get_local_time();
t.get( st );
today = st.wDay;

new_day = today;
s.get_text( DayParamEb );
if ( !s.isempty() )
    new_day = s.int_value();

if ( new_day <= today )
    {
    delta = today - new_day;
    delta *= SECONDS_PER_DAY;
    t -= delta;
    }
else
    {
    delta = today * SECONDS_PER_DAY;
    t -= delta;
    t.get( st );
    today = st.wDay;
    if ( new_day < today )
        {
        delta = today - new_day;
        delta *= SECONDS_PER_DAY;
        t -= delta;
        }
    }
DayParamTitle = MachineName;
DayParamTitle += separator;
DayParamTitle += PartName;
DayParamTitle += separator;
DayParamTitle += t.mmddyyyy();
t.get(st);
DayParamLb.redraw_off();
DayParamLb.empty();

name.get_shotparm_csvname( ComputerName, MachineName, PartName );

openok = false;
opentime.get_local_time();
closetime = opentime;

if ( vdb.open_for_read(name) )
    {
    openok = true;
    opentime.get_local_time();
    if ( vdb.get_first_equal_or_greater_date(SHOTPARM_DATE_INDEX, st) )
        {
        last_corner_parameter_index = NofCornerParameters - 1;
        while ( true )
            {
            /*
            ------------------------------------
            I'm done if this is not the same day
            ------------------------------------ */
            vdb.get_date( nt, SHOTPARM_DATE_INDEX );
            if ( nt.wDay != st.wDay )
                break;
            if ( nt.wMonth != st.wMonth )
                break;
            if ( nt.wYear != st.wYear )
                break;

            s = vdb.ra[SHOTPARM_TIME_INDEX];
            s += TabChar;

            s.cat_w_char( vdb.ra[SHOTPARM_SHOT_NAME_INDEX], TabChar );

            for ( j=0; j<NofCornerParameters; j++ )
                {
                i = CornerParameters[j];
                if ( i > 0 && i <= MAX_PARMS )
                    {
                    i--;
                    i += SHOTPARM_PARAM_1_INDEX;
                    if ( j < last_corner_parameter_index )
                        s.cat_w_char( vdb.ra[i], TabChar );
                    else
                        s += vdb.ra[i];
                    }
                }
            DayParamLb.add( s.text() );
            if ( !vdb.get_next_record() )
                break;
            }
        }
    vdb.close();
    closetime.get_local_time();
    }

write_shotparm_log( MachineName, PartName, TEXT("update_day_param_display"), openok, opentime, closetime, ShotparmLogFile );
DayParamLb.redraw_on();

InvalidateRect( MainPlotWindow, 0, TRUE );
SetFocus( DayParamEb );
PostMessage( DayParamEb, EM_SETSEL, 0, -1L ); /* Select All */
}

/***********************************************************************
*                       DAY_PARAM_EB_SUB_PROC                          *
* Subclass the edit control so I can process the enter key.            *
***********************************************************************/
LRESULT CALLBACK day_param_eb_sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int n;
int r;
int t;

if ( msg == WM_KEYDOWN )
    {
    if ( wParam == VK_RETURN )
        {
        update_day_param_display();
        return 0;
        }
    if ( wParam == VK_NEXT )
        {
        n = DayParamLb.count();
        r = DayParamLb.visible_row_count();
        t = DayParamLb.top_index();
        t += r;
        if ( t < n )
            DayParamLb.set_top_index(t);
        return 0;
        }
    if ( wParam == VK_PRIOR )
        {
        r = DayParamLb.visible_row_count();
        t = DayParamLb.top_index();
        if ( t < r )
            t = 0;
        else
            t -= r;
        DayParamLb.set_top_index(t);
        return 0;
        }
    if ( wParam == VK_DOWN )
        {
        n = DayParamLb.count();
        r = DayParamLb.visible_row_count();
        t = DayParamLb.top_index();
        if ( (t+r) < n )
            DayParamLb.set_top_index(t+1);
        return 0;
        }
    if ( wParam == VK_UP )
        {
        t = DayParamLb.top_index();
        if ( t > 0 )
            DayParamLb.set_top_index(t-1);
        return 0;
        }
    }

return CallWindowProc( OldDayParamEditProc, w, msg, wParam, lParam );
}

/***********************************************************************
*                        BEGIN_DAY_PARAM_DISPLAY                       *
***********************************************************************/
static void begin_day_param_display()
{
union { LONG signed_dword; WORD unsigned_word; };
FONT_CLASS      f;
HFONT           old_font;
POINT           p;
RECTANGLE_CLASS r;
STRING_CLASS    s;
TIME_CLASS      t;
TEXTMETRIC      tm;
INT             base_units;
INT           * copy;
HDC dc;
int i;
int j;
int h;
int pos;
int w;

signed_dword = GetDialogBaseUnits();
base_units   = (INT) unsigned_word;
GetClientRect( MainPlotWindow, &r);

/*
-------------------------------------------------------------------
Each column is w pixels wide. Count the most characters I must fit.
------------------------------------------------------------------- */
p.x = r.width()/(NofCornerParameters + 2);
w   = 0;
s   = StringTable.find( TEXT("TIME_NAME_STRING") );
w   = s.len();
s   = StringTable.find( TEXT("SHOT_NUMBER_STRING") );
maxint( w, s.len() );

for ( j=0; j<NofCornerParameters; j++ )
    {
    i = CornerParameters[j];
    if ( i > 0 && i <= MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            {
            s = CurrentParam.parameter[i].name;
            maxint( w, s.len() );
            }
        }
    }

/*
-------------------------------------------------------
Divide the column width by the max number of characters
------------------------------------------------------- */
p.x /= w;
p.y = 0;
dc = GetDC( MainPlotWindow );
DPtoLP( dc, &p, 1 );
if ( p.x < 1 )
    p.x = 1;

if ( DayParamFontHandle != INVALID_FONT_HANDLE )
    FontList.free_handle( DayParamFontHandle );

DayParamFont          = StringTable.fontclass();
DayParamFont.lfHeight = 0;
DayParamFont.lfWidth  = p.x;
DayParamFontHandle    = FontList.get_handle( DayParamFont );

old_font = INVALID_FONT_HANDLE;
if ( DayParamFontHandle != INVALID_FONT_HANDLE )
    old_font = (HFONT) SelectObject( dc, DayParamFontHandle );

GetTextMetrics( dc, &tm );
DayParamFontHeight = tm.tmHeight - tm.tmInternalLeading;
DayParamFontWidth  = tm.tmAveCharWidth;
if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );
ReleaseDC( MainPlotWindow, dc );

h = 2 * DayParamFontHeight + DayParamFontHeight/2;
r.top += 2 * DayParamFontHeight + DayParamFontHeight/2;
r.bottom -= DayParamFontHeight + DayParamFontHeight/2;
DayParamLb.init( CreateWindow( TEXT("LISTBOX"), 0, WS_VSCROLL | WS_CHILD| WS_BORDER | WS_VISIBLE | LBS_USETABSTOPS | LBS_NOSEL, 0,  r.top, r.width(), r.height(), MainPlotWindow, (HMENU) DAY_PARAM_LB_ID, MainInstance, NULL) );
DayParamLb.add( TEXT("15:34:45  2.344  123.223  45.67") );

/*
------------------------------------------
Calculate the tab positions for the titles
Time, shot number, p1, p2,... pn
------------------------------------------ */
if ( DayParamTitlePosition )
    delete[] DayParamTitlePosition;

NofDayParamTitles     = NofCornerParameters + 1;
DayParamTitlePosition = new INT[NofDayParamTitles];
copy                  = new INT[NofDayParamTitles];
if ( DayParamTitlePosition && copy )
    {
    w = r.width() / (NofDayParamTitles+1);
    pos = 0;
    for ( i=0; i<NofDayParamTitles; i++ )
        {
        pos += w;
        DayParamTitlePosition[i] = pos;
        copy[i] = (pos * 4) / base_units;
        }
    SendMessage( DayParamLb.handle(), LB_SETTABSTOPS, (WPARAM) NofDayParamTitles, (LPARAM) copy );
    delete[] copy;
    copy = 0;
    }
else
    {
    NofDayParamTitles = 0;
    }

r.top = r.bottom + DayParamFontHeight/4;
r.bottom = r.top + DayParamFontHeight;

w = DayParamFontWidth * 4;
r.left = r.width()/2 - w/2;
r.right = r.left + w;
DayParamEb = CreateWindow( TEXT("EDIT"), 0, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT, r.left,  r.top, r.width(), r.height(), MainPlotWindow, (HMENU) DAY_PARAM_EB_ID, MainInstance, NULL );
OldDayParamEditProc = (WNDPROC) SetWindowLong( DayParamEb, GWL_WNDPROC, (LONG) day_param_eb_sub_proc );
if ( DayParamCurrentEntry.isempty() )
    {
    t.get_local_time();
    DayParamCurrentEntry = t.dd();
    }

DayParamCurrentEntry.set_text( DayParamEb );
update_day_param_display();
}

/***********************************************************************
*                         END_DAY_PARAM_DISPLAY                        *
***********************************************************************/
static void end_day_param_display()
{
HWND w;
w = DayParamLb.handle();
DayParamLb.init( 0 );
DestroyWindow( w );

DestroyWindow( DayParamEb );
DayParamEb = 0;
OldDayParamEditProc = 0;

if ( DayParamFontHandle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( DayParamFontHandle );
    DayParamFontHandle = INVALID_FONT_HANDLE;
    DayParamFont.lfFaceName[0] = NullChar;
    }

if ( DayParamTitlePosition )
    delete[] DayParamTitlePosition;
DayParamTitlePosition = 0;
NofDayParamTitles = 0;
}

/***********************************************************************
*                        PAINT_DAY_PARAM_DISPLAY                        *
***********************************************************************/
static void paint_day_param_display( PAINTSTRUCT & ps )
{
HFONT            old_font;
RECTANGLE_CLASS  r;
STRING_CLASS     s;
HDC dc;
int i;
int j;
int y;

GetClientRect( MainPlotWindow, &r);
dc = ps.hdc;
r.fill( dc, WhiteColor );
SetBkMode(    dc, TRANSPARENT );
SetTextAlign( dc, TA_TOP | TA_CENTER );
SetTextColor( dc, BlackColor );

/*
-------------
Load the font
------------- */
old_font = INVALID_FONT_HANDLE;
if ( DayParamFontHandle != INVALID_FONT_HANDLE )
    old_font = (HFONT) SelectObject( dc, DayParamFontHandle );

y = DayParamFontHeight/4;
if ( !DayParamTitle.isempty() )
    TextOut( dc, r.width()/2, y, DayParamTitle.text(), DayParamTitle.len() );

SetTextAlign( dc, TA_TOP | TA_LEFT );
y += DayParamFontHeight;
s = StringTable.find( TEXT("TIME_NAME_STRING") );
TextOut( dc, 2, y, s.text(), s.len() );

s = StringTable.find( TEXT("SHOT_NUMBER_STRING") );
TextOut( dc, DayParamTitlePosition[0], y, s.text(), s.len() );

for ( j=0; j<NofCornerParameters; j++ )
    {
    i = CornerParameters[j];
    if ( i > 0 && i <= MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            {
            s = CurrentParam.parameter[i].name;
            TextOut( dc, DayParamTitlePosition[j+1], y, s.text(), s.len() );
            }
        }
    }

if ( old_font != INVALID_FONT_HANDLE )
    SelectObject( dc, old_font );
}

/***********************************************************************
*                        PAINT_BIG_PARAM_DISPLAY                       *
***********************************************************************/
static void paint_big_param_display( PAINTSTRUCT & ps )
{
create_bigfont( ps.hdc );
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static void paint_me()
{
COMPUTER_CLASS c;
BOOLEAN     need_to_set_curves;
PAINTSTRUCT ps;
int         i;

if ( MyScreenType == PROFILE_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE || MyScreenType == ZABISH_SCREEN_TYPE )
    {
    if ( !c.online_status(ComputerName) )
        {
        BeginPaint( MainPlotWindow, &ps );
        MyDc = ps.hdc;
        show_offline_info();
        MyDc = NULL;
        EndPaint( MainPlotWindow, &ps );
        return;
        }
    }
/*
---------------------------------------
Do nothing if there is no update region
--------------------------------------- */
if ( Printing || !GetUpdateRect(MainPlotWindow, NULL, FALSE) )
    {
    BeginPaint( MainPlotWindow, &ps );
    EndPaint( MainPlotWindow, &ps );
    return;
    }

if ( overlaying() && NofOverlayShots > 1 )
    {
    HaveHeader = FALSE;
    load_part_info( OverlayShot );
    }

if ( !HaveHeader )
    {
    need_to_set_curves = TRUE;
    load_profile();
    if ( need_to_set_curves )
        {
        clear_y_axes();
        set_plot_curves();
        }
    }

BeginPaint( MainPlotWindow, &ps );

if ( MyScreenType == BOX_SCREEN_TYPE )
    {
    paint_box_display( ps );
    }
else if ( MyScreenType == STATUS_SCREEN_TYPE )
    {
    paint_status_display( ps );
    }
else if ( MyScreenType == DAY_PARAM_SCREEN_TYPE )
    {
    paint_day_param_display( ps );
    }
else if ( HaveHeader && MyScreenType == BIG_PARAM_SCREEN_TYPE )
    {
    paint_big_param_display( ps );
    }
else if ( HaveHeader || MyScreenType == TREND_SCREEN_TYPE || MyScreenType == DOWNBAR_SCREEN_TYPE )
    {
    /*
    ----------------------------------------------------
    Set the global device context so everyone can use it
    ---------------------------------------------------- */
    MyDc = ps.hdc;

    /*
    -------------
    Load the font
    ------------- */
    select_myfont();

    if ( MyScreenType == TREND_SCREEN_TYPE )
        {
        paint_trend_display( ps.rcPaint );
        }

    else if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
        {
        paint_downbar_display();
        }
    else
        {
        /*
        -------------------------------------------------------------
        The screen has been cleared so I need to paint the background
        ------------------------------------------------------------- */
        paint_background();
        paint_the_curves();
        show_overlays();
        show_marks();
        show_shot_info();

        for ( i=0; i<NOF_AXES; i++ )
            {
            if ( Axis[i].is_fixed )
                {
                show_fixed_axis_message();
                break;
                }
            }

        if ( viewing_reference() )
            {
            if ( load_reference_trace() )
                {
                if ( viewing_reference() )   //Lawyer Code, check viewing_ref again for and old ref trace
                    {
                    CurrentColor = ReferenceColor;
                    paint_the_curves();
                    CurrentColor = NormalColor;
                    }
                load_profile();
                }
            }
        }

    MyDc = NULL;
    }

EndPaint( MainPlotWindow, &ps );
set_realtime_menu_string();
}

/***********************************************************************
*                                                                      *
*                   SIZE_RECTANGLES_TO_CURRENT_DISPLAY                 *
*                                                                      *
*  For the pos/time display, the right edge of the pos-based and       *
*  left edge of the time-based plots are 7/10ths of the way            *
*  across the ploting region.                                          *
*                                                                      *
*  Otherwise I set all the windows to the total width.                 *
*                                                                      *
***********************************************************************/
static void size_rectangles_to_current_display()
{
int  x;
RECT r;

r.top    = Region[POS_BASED_ANALOG].rect.top;
r.left   = Region[POS_BASED_ANALOG].rect.left;
r.right  = Region[TIME_BASED_COUNT].rect.right;
r.bottom = Region[TIME_BASED_COUNT].rect.bottom;

if ( CurrentDisplay == POS_TIME_DISPLAY )
    {
    x = ( r.right - r.left ) * 7;
    x /= 10;
    x += r.left;
    }
else
    x = r.right;

Region[POS_BASED_ANALOG].rect.right = x;
Region[POS_BASED_COUNT].rect.right  = x;

if ( CurrentDisplay == POS_TIME_DISPLAY )
    x++;
else
    x = r.left;

Region[TIME_BASED_ANALOG].rect.left = x;
Region[TIME_BASED_COUNT].rect.left  = x;
}

/***********************************************************************
*                                                                      *
*                         SIZE_PROFILE_WINDOW                          *
*                                                                      *
*  This should only be called in response to an actual size change     *
*  because I create a new font each time. Since everything is global,  *
*  it is not necessary to call for any other reason.                   *
*                                                                      *
*  Creating the new font also sets the size of the MyCharHeight and    *
*  MyCharWidth variables.                                              *
*                                                                      *
*  You should get the device context (MyDc) before calling this.       *
*                                                                      *
***********************************************************************/
static void size_profile_window( RECT mr )
{
int  x;
int  y;
SIZE s;;

PlotWindowRect = mr;

if ( !Printing )
    load_myfont();

mr.top += MyCharHeight;

Region[BOTTOM_AXIS].rect.left   = mr.left;
Region[BOTTOM_AXIS].rect.right  = mr.right;
y                               = mr.bottom - 1;
Region[BOTTOM_AXIS].rect.bottom = y;
y                              -= 3 * MyCharHeight;
Region[BOTTOM_AXIS].rect.top    = y;

y--;
Region[POS_BASED_COUNT].rect.bottom  = y;
Region[TIME_BASED_COUNT].rect.bottom = y;

y                                    = mr.top + MyCharHeight;
Region[POS_BASED_ANALOG].rect.top    = y;
Region[TIME_BASED_ANALOG].rect.top   = y;

y = y + (Region[POS_BASED_COUNT].rect.bottom - y) / 2;

Region[POS_BASED_ANALOG].rect.bottom  = y;
Region[TIME_BASED_ANALOG].rect.bottom = y;

y++;
Region[POS_BASED_COUNT].rect.top      = y;
Region[TIME_BASED_COUNT].rect.top     = y;

/*
-------------------------------------------------------------
See how much room I need for two vertical labels and a number
------------------------------------------------------------- */
GetTextExtentPoint32( MyDc, TEXT("W P 000000."), 11, &s );
x = s.cx;

Region[POS_BASED_ANALOG].rect.left   = mr.left  + x;
Region[POS_BASED_COUNT].rect.left    = mr.left  + x;
Region[TIME_BASED_ANALOG].rect.right = mr.right - x;
Region[TIME_BASED_COUNT].rect.right  = mr.right - x;

size_rectangles_to_current_display();

Region[LEFT_AXIS].rect.left   = mr.left;
Region[LEFT_AXIS].rect.right  = Region[POS_BASED_ANALOG].rect.left - 1;
Region[LEFT_AXIS].rect.top    = mr.top;
Region[LEFT_AXIS].rect.bottom = mr.bottom - 1;

Region[RIGHT_AXIS].rect.left   = Region[TIME_BASED_ANALOG].rect.right + 1;
Region[RIGHT_AXIS].rect.right  = mr.right;
Region[RIGHT_AXIS].rect.top    = mr.top;
Region[RIGHT_AXIS].rect.bottom = mr.bottom - 1;
}

/***********************************************************************
*                       SIZE_PLOT_WINDOW_TO_SCREEN                     *
***********************************************************************/
static void size_plot_window_to_screen()
{
RECT  mr;

GetClientRect( MainPlotWindow, &mr);
MyDc = GetDC( MainPlotWindow );
size_profile_window( mr );
ReleaseDC(MainPlotWindow, MyDc );
MyDc = NULL;
}

/***********************************************************************
*                        PRINTER_ZOOM_BOX_HEIGHT                       *
*   This assumes you have already created the printer font.            *
***********************************************************************/
static int printer_zoom_box_height()
{
const int  lineheight = 1;
TEXTMETRIC tm;
int        ch;
int        vgap;
int        total;

GetTextMetrics( MyDc, &tm );

ch     = tm.tmHeight - tm.tmInternalLeading;
vgap   = tm.tmExternalLeading;

total = 4*vgap + 7*(vgap + ch) + 5*lineheight;

return total;
}

/***********************************************************************
*                     PRINTER_PARAMETER_BOX_HEIGHT                     *
*   This assumes you have already created the printer font.            *
***********************************************************************/
static int printer_parameter_box_height()
{
const int  lineheight = 1;
TEXTMETRIC tm;
int        ch;
int        n;
int        vgap;
int        total;

GetTextMetrics( MyDc, &tm );

ch     = tm.tmHeight - tm.tmInternalLeading;
vgap   = tm.tmExternalLeading;

if ( ViewParametersDisplay )
    {
    n = CurrentParam.count() / 2;
    if ( n*2 < CurrentParam.count() )
        n++;
    }
else
    {
    n = NofCornerParameters/2;
    if ( 2*n < NofCornerParameters )
        n++;
    }
/*
---------------------
Add one for the title
--------------------- */
n++;

total = 4*vgap + n*(vgap + ch) + 5*lineheight;

return total;
}

/***********************************************************************
*                            DRAW_RECTANGLE                            *
***********************************************************************/
static void draw_rectangle( RECT & r )
{
MoveToEx( MyDc, r.left, r.top, 0 );
LineTo( MyDc, r.right, r.top    );
LineTo( MyDc, r.right, r.bottom );
LineTo( MyDc, r.left,  r.bottom );
LineTo( MyDc, r.left,  r.top    );
}

/***********************************************************************
*                            PRINT_ZOOM_BOX                            *
***********************************************************************/
static void print_zoom_box()
{
TEXTMETRIC tm;
int        col;
int        row;
int        dx;
int        dy;
int        x;
int        y;
int        y0;
TCHAR      s[MAX_FLOAT_LEN+1];
UINT       id;
RECT       r;
HWND       w;

if ( MyScreenType == ZABISH_SCREEN_TYPE )
    w = ZoomBoxWindow;
else
    w = SmallZoomBoxWindow;

GetTextMetrics( MyDc, &tm );

dy = tm.tmHeight - tm.tmInternalLeading + tm.tmExternalLeading;
dy *=3;
dy /=2;

y  = PlotWindowRect.bottom + 2*dy;
r  = PlotWindowRect;
r.top = y - dy/2;
r.bottom = y + 4*dy;

dx = PlotWindowRect.right - PlotWindowRect.left;
dx /= 6;

SetBkMode(    MyDc, TRANSPARENT );
SetTextAlign( MyDc, TA_TOP | TA_CENTER );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );

x = dx + dx/2;

text_out( x, y, ZOOM_PRINT_POSITION_STRING );
x += dx;
text_out( x, y, ZOOM_PRINT_TIME_STRING );
x += dx;

text_out( x, y, ZOOM_PRINT_VELOCITY_STRING );
x += dx;

text_out( x, y, ZOOM_PRINT_ROD_PRES_STRING );
x += dx;

text_out( x, y, ZOOM_PRINT_HEAD_PRES_STRING );

y += dy - 4;

draw_line( r.left, y, r.right, y );

y += 8;
x = r.left + tm.tmAveCharWidth;

SetTextAlign( MyDc, TA_TOP | TA_LEFT );
text_out( x, y,      ZOOM_PRINT_START_STRING );
text_out( x, y+dy,   ZOOM_PRINT_END_STRING   );
text_out( x, y+2*dy, ZOOM_PRINT_DELTA_STRING );

x = dx;
x *= 7;
x /= 4;
SetTextAlign( MyDc, TA_TOP | TA_RIGHT );

y0 = y;
id = FIRST_ZOOM_VALUE_TBOX;
for ( col = 0; col<5; col++ )
    {
    y = y0;
    for ( row=0; row<3; row++ )
        {
        get_text( s, w, id, MAX_FLOAT_LEN );
        text_out( x, y, s );
        y += dy;
        id++;
        }
    id += 7;
    x += dx;
    }

draw_rectangle( r );

x = dx;
for ( col = 0; col<5; col++ )
    {
    draw_line( x, r.top, x, r.bottom );
    x += dx;
    }

}

/***********************************************************************
*                       SHOW_PRINTER_PARAMETERS                        *
***********************************************************************/
static void show_printer_parameters()
{
const int  lineheight = 1;
const int  linewidth  = 1;

TEXTMETRIC tm;
int        ascent;
int        ch;
int        hgap;
int        i;
int        j;
int        n;
int        nof_lines;
int        vgap;
int        dx;
int        x;
int        y;
RECT       r;
INT        orbkmode;
TCHAR    * cp;
HPEN       mypen;
HPEN       oldpen;
COLORREF   my_text_color;

GetTextMetrics( MyDc, &tm );

ch     = tm.tmHeight - tm.tmInternalLeading;
vgap   = tm.tmExternalLeading;
ascent = tm.tmAscent - tm.tmInternalLeading;
hgap   = tm.tmAveCharWidth / 2;

orbkmode = GetBkMode( MyDc );
SetBkMode(    MyDc, TRANSPARENT );
SetTextAlign( MyDc, TA_BASELINE | TA_LEFT );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );

x  = PlotWindowRect.left + hgap + linewidth;
y  = PlotWindowRect.bottom + vgap;
dx = (PlotWindowRect.right - PlotWindowRect.left)/2  - 5*linewidth;
dx /= 12;

if ( ViewParametersDisplay )
    n = CurrentParam.count();
else
    n = NofCornerParameters;

/*
-----------------------------
Calculate the number of lines
----------------------------- */
nof_lines = n/2;
if ( 2*nof_lines < n )
    nof_lines++;

for ( j=0; j<n; j++ )
    {
    if ( j == 0 || j == nof_lines )
        {
        if ( j == 0 )
            x  = PlotWindowRect.left + hgap + linewidth;
        else
            x = (PlotWindowRect.right + PlotWindowRect.left)/2 + 2*linewidth + hgap;

        y  = PlotWindowRect.bottom + vgap + lineheight + 3*vgap + ascent;

        TextOut( MyDc, x,      y, ParameterListTitle[0].text(), ParameterListTitle[0].len() );
        TextOut( MyDc, x+5*dx, y, ParameterListTitle[1].text(), ParameterListTitle[1].len() );
        TextOut( MyDc, x+7*dx, y, ParameterListTitle[2].text(), ParameterListTitle[2].len() );
        TextOut( MyDc, x+9*dx, y, ParameterListTitle[3].text(), ParameterListTitle[3].len() );

        y += ch + lineheight + 2*vgap;
        }

    if ( ViewParametersDisplay )
        i = j+1;
    else
        i = CornerParameters[j];

    if ( i > 0 && i <= MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            {
            cp = CurrentParam.parameter[i].name;
            TextOut( MyDc, x, y, cp, lstrlen(cp) );

            my_text_color = CurrentColor[TEXT_COLOR];

            if ( CurrentParam.parameter[i].vartype != TEXT_VAR )
                {
                cp = ascii_float( CurrentParam.parameter[i].limits[LowLimitType].value );
                TextOut( MyDc, x+5*dx, y, cp, lstrlen(cp) );

                cp = ascii_float( CurrentParam.parameter[i].limits[HighLimitType].value );
                TextOut( MyDc, x+7*dx, y, cp, lstrlen(cp) );

                if ( is_alarm(i, Parms[i].float_value()) )
                    my_text_color = AlarmColor;
                else if ( is_warning(i, Parms[i].float_value()) )
                    my_text_color = WarningColor;
                }

            SetTextColor( MyDc, my_text_color );
            TextOut( MyDc, x+9*dx, y, Parms[i].text(), lstrlen(cp) );

            SetTextColor( MyDc, CurrentColor[TEXT_COLOR]       );

            if ( CurrentParam.parameter[i].vartype != TEXT_VAR )
                {
                cp = units_name( CurrentParam.parameter[i].units );
                TextOut( MyDc, x+11*dx, y, cp, lstrlen(cp) );
                }
            }

        y += ch + vgap;
        }
    }

SetBkMode( MyDc, orbkmode );

mypen  = CreatePen( PS_SOLID, 1, CurrentColor[BORDER_COLOR]   );
if ( mypen )
    {
    /*
    -----------------------------
    Draw the box around the title
    ----------------------------- */
    oldpen = (HPEN) SelectObject( MyDc, mypen );

    r.left   = PlotWindowRect.left;
    r.top    = PlotWindowRect.bottom + 2*vgap;
    r.right  = r.left + (PlotWindowRect.right - PlotWindowRect.left)/2  - linewidth;
    r.bottom = r.top + vgap + ch + vgap + lineheight;

    draw_rectangle( r );

    /*
    ----------------------------------
    Draw the box around the parameters
    ---------------------------------- */
    r.top     = r.bottom;
    r.bottom += vgap + nof_lines*(ch + vgap) + lineheight;

    draw_rectangle( r );

    /*
    -----------------------------
    Draw the box around the title
    ----------------------------- */
    r.left   = r.right + 1 + linewidth;
    r.top    = PlotWindowRect.bottom + 2*vgap;
    r.right  = PlotWindowRect.right;
    r.bottom = r.top + vgap + ch + vgap + lineheight;

    draw_rectangle( r );

    /*
    ----------------------------------
    Draw the box around the parameters
    ---------------------------------- */
    r.top     = r.bottom;
    r.bottom += vgap + nof_lines*(ch + vgap) + lineheight;
    draw_rectangle( r );

    SelectObject( MyDc, oldpen );
    DeleteObject( mypen );
    }
}

/***********************************************************************
*                               PRINT_ME                               *
***********************************************************************/
void print_me()
{
static DOCINFO di = { sizeof(DOCINFO), TEXT("TrueTrak Profile"), 0 };
BOOLEAN        need_to_set_curves;
int            x;
RECT           mr;
POINT          p;
HDC            dc;

if ( (PrintingMultiplePlots && MultipleShotDialogWindow) || QuickPrint )
    dc = get_default_printer_dc();
else
    dc = get_printer_dc();

QuickPrint = FALSE;

if ( dc != 0 )
    {
    Printing = TRUE;
    MyDc     = dc;

    if ( overlaying() && NofOverlayShots > 1 )
        {
        HaveHeader = FALSE;
        load_part_info( OverlayShot );
        }

    if ( !HaveHeader || viewing_reference() )
        {
        need_to_set_curves = FALSE;
        if ( !HaveHeader )
            need_to_set_curves = TRUE;
        load_profile();
        if ( need_to_set_curves )
            {
            clear_y_axes();
            set_plot_curves();
            }
        }

    CurrentColor = PrNormalColor;

    if ( StartDoc(MyDc, &di) > 0 )
        {
        if ( StartPage(MyDc) > 0 )
            {
            p.x = GetDeviceCaps( MyDc, PHYSICALOFFSETX );
            p.y = GetDeviceCaps( MyDc, PHYSICALOFFSETY );

            mr.left   = p.x;
            mr.top    = p.y;
            mr.right  = GetDeviceCaps( MyDc, HORZRES ) - p.x;
            mr.bottom = GetDeviceCaps( MyDc, VERTRES ) - p.y;

            x = GetDeviceCaps( MyDc, LOGPIXELSX );

            if ( PrinterMargin.left > 0 )
                mr.left  += (x * PrinterMargin.left) / 1000;

            if ( PrinterMargin.right > 0 )
                mr.right -= (x * PrinterMargin.right) / 1000;

            x = GetDeviceCaps( MyDc, LOGPIXELSY );

            if ( PrinterMargin.top > 0 )
                mr.top += (x * PrinterMargin.top) / 1000;

            if ( PrinterMargin.bottom > 0 )
                mr.bottom -= (x * PrinterMargin.bottom) / 1000;

            create_printer_font( mr );
            select_printer_font();

            if ( zoomed() )
                mr.bottom -= printer_zoom_box_height();
            else
                mr.bottom -= printer_parameter_box_height();

            size_profile_window( mr );

            if ( AlarmSummaryDialogWindow )
                {
                print_alarm_summary();
                }
            else if ( MyScreenType == TREND_SCREEN_TYPE )
                {
                paint_trend_display( mr );
                }
            else if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
                {
                paint_downbar_display();
                }
            else if ( HaveHeader )
                {
                paint_background();
                paint_the_curves();
                show_overlays();
                show_marks();
                show_shot_info();
                if ( viewing_reference() )
                    {
                    if ( load_reference_trace() )
                        {
                        if ( viewing_reference() )  // Lawyer Code incase of old ref trace
                            {
                            CurrentColor = PrReferenceColor;
                            paint_the_curves();
                            CurrentColor = PrNormalColor;
                            }
                        load_profile();
                        }
                    }

                if ( zoomed() )
                    print_zoom_box();
                else
                    show_printer_parameters();
                }

            select_myfont();
            delete_printer_font();

            if ( EndPage(MyDc) > 0 )
                EndDoc( MyDc );
            }
        }
    Printing = FALSE;
    DeleteDC( MyDc );
    MyDc = NULL;
    CurrentColor = NormalColor;
    }

/*
------------------------------------
Resize the window to the screen size
------------------------------------ */
size_plot_window_to_screen();
}

/***********************************************************************
*                           TURN_OFF_ZOOMING                           *
***********************************************************************/
static void turn_off_zooming()
{

if ( Zooming )
    {
    Zooming = FALSE;
    erase_cursor_line();
    erase_left_cursor_line();
    }
}

/***********************************************************************
*                         DO_OVERLAY_TOGGLE                            *
***********************************************************************/
static void do_overlay_toggle( int id )
{
NofOverlayShots = 0;
if ( !OverlayMenuState )
    append_overlay_shot();

toggle_menu_view_state( OverlayMenuState, id );
if ( ChosshotWindow )
    SendMessage( ChosshotWindow, WM_OVERLAY_CHANGE, 0, 0L );
}

/***********************************************************************
*                         SET_ZOOM_GRAYNESS                            *
*  Originally this was just for zoom menu items but now is for         *
*  any that should just work with the profile plot screen.             *
***********************************************************************/
static void set_zoom_grayness()
{
UINT  item[] = {
    ZOOM_MENU,
    UNZOOM_MENU,
    VIEW_REFERENCE_TRACE_MENU,
    OVERLAY_MENU,
    SAVE_AS_MASTER_MENU,
    CHOOSE_SHOT_MENU
    };

const int32 NOF_ITEMS = sizeof(item)/sizeof(UINT);

int i;
BOOLEAN is_gray;

if ( MyScreenType == STANDARD_SCREEN_TYPE || MyScreenType == ZABISH_SCREEN_TYPE )
    is_gray = FALSE;
else
    is_gray = TRUE;

for ( i=0; i<NOF_ITEMS; i++ )
    MainMenu.set_gray( item[i], is_gray );
}

/***********************************************************************
*                            LOAD_MAIN_MENU                            *
***********************************************************************/
static void load_main_menu()
{
NAME_CLASS s;
WINDOW_CLASS w;

s.get_exe_dir_file_name( ProfileMenuFile );
MainMenu.read( s, (int)CurrentPasswordLevel );

MainMenu.resize();
w = MainWindow;
w.shrinkwrap( MainMenu.mw.handle() );

ArialFontName      = MainMenu.lf.lfFaceName;
ArialBoldFontName  = MainMenu.lf.lfFaceName;
TimesRomanFontName = MainMenu.lf.lfFaceName;
load_myfont();

set_zoom_grayness();

if ( !HaveMonitorProgram )
    MainMenu.set_gray( BOARD_MONITOR_MENU, TRUE );

if ( !HaveSureTrakControl )
    MainMenu.set_gray( SURETRAK_CONTROL_MENU, TRUE );

if ( overlaying() )
    MainMenu.set_checkmark( OVERLAY_MENU, TRUE );

if ( viewing_reference() )
    MainMenu.set_checkmark( VIEW_REFERENCE_TRACE_MENU, TRUE );

PopupMenu.set_font( MainMenu.lf );
}

/***********************************************************************
*                         SET_CURRENT_DISPLAY                          *
***********************************************************************/
static void set_current_display( int new_display_state )
{
clear_y_axes();

CurrentDisplay = new_display_state;

if ( CurrentDisplay == POS_TIME_DISPLAY )
    {
    Region[POS_BASED_ANALOG].is_visible  = TRUE;
    Region[POS_BASED_COUNT].is_visible   = TRUE;
    Region[TIME_BASED_ANALOG].is_visible = TRUE;
    Region[TIME_BASED_COUNT].is_visible  = TRUE;
    }

else if ( CurrentDisplay == POS_ONLY_DISPLAY )
    {
    Region[POS_BASED_ANALOG].is_visible  = TRUE;
    Region[POS_BASED_COUNT].is_visible   = TRUE;
    Region[TIME_BASED_ANALOG].is_visible = FALSE;
    Region[TIME_BASED_COUNT].is_visible  = FALSE;
    }

else if ( CurrentDisplay == TIME_ONLY_DISPLAY )
    {
    Region[POS_BASED_ANALOG].is_visible  = FALSE;
    Region[POS_BASED_COUNT].is_visible   = FALSE;
    Region[TIME_BASED_ANALOG].is_visible = TRUE;
    Region[TIME_BASED_COUNT].is_visible  = TRUE;
    }

size_rectangles_to_current_display();

load_main_menu();
}

/***********************************************************************
*                     TOGGLE_POS_TIME_DISPLAY_TYPE                     *
***********************************************************************/
void toggle_pos_time_display_type()
{
int new_display_type;

if (  MyScreenType != STANDARD_SCREEN_TYPE && MyScreenType != ZABISH_SCREEN_TYPE )
    return;

if ( IsHPM )
    return;

new_display_type = NO_DISPLAY;

if ( CurrentDisplay == POS_TIME_DISPLAY )
    new_display_type = POS_ONLY_DISPLAY;
else if ( CurrentDisplay == POS_ONLY_DISPLAY )
    new_display_type = POS_TIME_DISPLAY;

if ( new_display_type != NO_DISPLAY )
    {
    set_current_display( new_display_type );
    InvalidateRect( MainPlotWindow, 0, TRUE );
    }
}

/***********************************************************************
*                        CREATE_CHOSPART_WINDOW                        *
***********************************************************************/
static void create_chospart_window()
{
if ( ChospartWindow )
    {
    if ( !ChosshotWindow || !IsWindowVisible(ChosshotWindow) )
        ShowWindow( ChospartWindow, SW_SHOW );
    SetFocus( ChospartWindow );
    }
else
    {
    ChospartWindow  = CreateDialog( MainInstance, TEXT("CHOOSE_PART"), MainWindow, (DLGPROC) chospart_proc );
    }

PostMessage( ChospartWindow, WM_DBINIT, 0, 0L );
}

/***********************************************************************
*                        CREATE_CHOSSHOT_WINDOW                        *
***********************************************************************/
static void create_chosshot_window()
{
static TCHAR CsDialog[] = TEXT( "CHOOSE_SHOTS" );

if ( ChosshotWindow )
    {
    if ( IsWindowVisible(ChosshotWindow) )
        SendMessage( ChosshotWindow, WM_NEWPART, 0, 0L );
    else
        ShowWindow ( ChosshotWindow, SW_SHOW );
    }
else
    {
    ChosshotWindow = CreateDialog( MainInstance, CsDialog, MainWindow, (DLGPROC) chosshot_proc );
    }
}

/***********************************************************************
*                             SET_ZOOM_AXIS                            *
***********************************************************************/
static void set_zoom_axis( int ax, int left, int right, int region_index )
{
RECT r;
float arange;
float xrange;
float x;

r  = Region[region_index].rect;

if ( left < r.left )
    left = r.left;

if ( right > r.right )
    right = r.right;

/*
-----------------------
Get the existing ranges
----------------------- */
xrange = float( r.right - r.left );
arange = Axis[ax].max - Axis[ax].min;

right -= r.left;
x = Axis[ax].min + arange * float(right) / xrange;
if ( x < Axis[ax].max )
    Axis[ax].max = x;

left -= r.left;
x = Axis[ax].min + arange * float(left) / xrange;
if ( x > Axis[ax].min )
    Axis[ax].min = x;

Axis[ax].is_on_hold = TRUE;
}

/***********************************************************************
*                                 ZOOM                                 *
***********************************************************************/
static void zoom( POINT p )
{
int new_display;

int left;
int right;
int i;

if ( p.x == LeftCursorX )
    {
    turn_off_zooming();
    return;
    }

new_display = CurrentDisplay;

if ( p.x > LeftCursorX )
    {
    left  = LeftCursorX;
    right = p.x;
    }
else
    {
    left  = p.x;
    right = LeftCursorX;
    }

if ( CurrentDisplay == POS_TIME_DISPLAY )
    {
    i = Region[POS_BASED_ANALOG].rect.right;
    if ( right <= i )
        new_display = POS_ONLY_DISPLAY;
    else if ( left >= i )
        new_display = TIME_ONLY_DISPLAY;
    }

get_mydc();

if ( MyFont )
    SelectObject( MyDc, MyFont );
else
    SelectObject( MyDc, GetStockObject(SYSTEM_FIXED_FONT) ) ;

if ( new_display == POS_ONLY_DISPLAY || new_display == POS_TIME_DISPLAY )
    set_zoom_axis( X_POS_AXIS, left, right, POS_BASED_ANALOG );

if ( new_display == TIME_ONLY_DISPLAY || new_display == POS_TIME_DISPLAY )
    set_zoom_axis( TIME_AXIS,  left, right, TIME_BASED_ANALOG );

set_current_display( new_display );
clear_y_axes();

release_mydc();

HaveLeftCursorLine = FALSE;
HaveCursorLine     = FALSE;
Zooming            = FALSE;

InvalidateRect( MainPlotWindow, 0, TRUE );
}

/***********************************************************************
*                                UNZOOM                                *
***********************************************************************/
static void unzoom( BOOLEAN need_update )
{
clear_axes();

LeftCursorX   = 0;
LeftZoomIndex = 0;
ZoomIndex     = 0;
HaveLeftCursorLine = FALSE;
HaveCursorLine     = FALSE;
Zooming            = FALSE;
set_info_box_type( PARAMETER_INFO_BOX );

if ( WasRealtimeBeforeZoom && !RealTime )
    {
    RealTime = WasRealtimeBeforeZoom;
    SingleMachineOnly = WasSingleMachineBeforeZoom;
    set_realtime_menu_string();
    }

if ( CurrentDisplay != DisplayBeforeZoom )
    set_current_display( DisplayBeforeZoom );

if ( need_update )
    InvalidateRect( MainPlotWindow, 0, TRUE );
}

/***********************************************************************
*                                UNZOOM                                *
***********************************************************************/
static void unzoom()
{
unzoom( TRUE );
}

/***********************************************************************
*                          NEW_SHOT_CALLBACK                           *
***********************************************************************/
void new_shot_callback( TCHAR * topic, short item, HDDEDATA edata )
{
DWORD          bytes_to_copy;
DWORD          bytes_copied;
COMPUTER_CLASS c;
TCHAR        * cp;
STRING_CLASS   s;

if ( !RealTime )
    return;

if ( Zooming )
    return;

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

s = topic;
s.null_first_control_char();

if ( !MachList.find(s) )
    return;

c.set_online_status( MachList.computer_name(), TRUE );

/*
-----------------------------------------------------------------------
If there are more than one computer, see if this machine is on the list
----------------------------------------------------------------------- */
if ( c.count() > 1 )
    if ( !RealtimeMachineList.find(MachList.name().text()) )
        return;

if ( SingleMachineOnly && (MachineName != MachList.name()) )
    return;

ComputerName = MachList.computer_name();
MachineName  = MachList.name();
if ( MachineName != CurrentMachine.name )
    {
    CurrentMachine.find( ComputerName, MachineName );
    init_global_curve_types();
    }
PartName = CurrentMachine.current_part;

bytes_to_copy = DdeGetData( edata, 0, 0, 0 );
if ( bytes_to_copy > 0 )
    {
    s.upsize( bytes_to_copy/sizeof(TCHAR) + 1 );
    bytes_copied = DdeGetData( edata, (LPBYTE) s.text(), bytes_to_copy, 0 );
    if ( bytes_copied > 0 )
        {
        cp = s.text();
        cp += bytes_copied/sizeof(TCHAR);
        *cp = NullChar;
        s.strip_crlf();
        ShotName = s;
        SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
        }
    }
}

/***********************************************************************
*                  START_PASSWORD_LEVEL_TIMEOUT_TIMER                  *
*   This, of course, does nothing if there is no timeout setting.      *
***********************************************************************/
static bool start_password_level_timeout_timer()
{
INI_CLASS ini;
NAME_CLASS s;
double     x;

if ( PasswordLevelTimeoutId != 0 )
    return false;

s.get_local_ini_file_name( VisiTrakIniFile );
if ( s.file_exists() )
    {
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    if ( ini.find(ConfigSection, PasswordLevelTimeoutKey) )
        {
        s = ini.get_string();
        x = s.real_value();
        if ( x > 0.0 )
            {
            x *= 60000.0;
            PasswordLevelTimeoutMs = (UINT) x;
            PasswordLevelTimeoutId = SetTimer( MainWindow, PASSWORD_LEVEL_TIMEOUT_TIMER_NUMBER, PasswordLevelTimeoutMs, 0 );
            }
        }
    }

if ( PasswordLevelTimeoutId != 0 )
    return true;

return false;
}

/***********************************************************************
*                          PASSWORD_LEVEL_CALLBACK                     *
***********************************************************************/
void password_level_callback( TCHAR * topic, short item, HDDEDATA edata )
{
static bool firstime = true;

int32 old_level;
int32 default_level;
TCHAR buf[MAX_INTEGER_LEN+3];
DWORD bytes_copied;
DWORD bufsize = sizeof( buf );

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

old_level = CurrentPasswordLevel;
bytes_copied = DdeGetData( edata, (LPBYTE) buf, bufsize, 0 );
if ( bytes_copied > 0 )
    {
    *(buf+MAX_INTEGER_LEN) = NullChar;
    fix_dde_name( buf );
    CurrentPasswordLevel = asctoint32( buf );
    load_main_menu();
    }

default_level = default_password_level();
if ( firstime )
    {
    firstime = false;
    if ( CurrentPasswordLevel == LOWEST_PASSWORD_LEVEL )
        set_default_password_level( false );
    if ( CurrentPasswordLevel > default_level )
        {
        start_password_level_timeout_timer();
        }
    }
else
    {
    if ( (PasswordLevelTimeoutId==0) && (old_level <= default_level) && (CurrentPasswordLevel > default_level) )
        {
        start_password_level_timeout_timer();
        }
    else if ( CurrentPasswordLevel <= default_level )
        {
        PasswordLevelTimeoutMs = 0;
        }
    }
}

/***********************************************************************
*                          NEW_PART_SETUP_CALLBACK                     *
***********************************************************************/
void new_part_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
COMPUTER_CLASS c;
STRING_CLASS   s;
STRING_CLASS   cs;

if ( Zooming )
    return;

if ( !edata )
    return;

if ( edata == (HDDEDATA) TRUE )
    return;

s = topic;
s.strip_crlf();

/*
----------------------------------------------------------------------------------
My assumption here is that if someone changed the part the computer must be online
---------------------------------------------------------------------------------- */
if ( MachList.get_computer_name(cs, s) )
    c.set_online_status( cs, TRUE );

if ( s != MachineName )
    return;

if ( get_dde_data(s, edata) )
    {
    if ( s == PartName )
        return;
    if ( Zooming )
        turn_off_zooming();
    load_part_info();
    if ( MyScreenType == TREND_SCREEN_TYPE )
        trend_screen_reset();
    else
        SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
    }
}

/***********************************************************************
*                      POSITION_MAIN_PLOT_WINDOW                       *
***********************************************************************/
void position_main_plot_window()
{
RECT pbr;
RECT r;
HWND w;

show_all_windows();

w = 0;
if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
    w = ParameterWindow;

r = my_desktop_client_rect();
if ( w )
    {
    GetWindowRect( w, &pbr );
    r.bottom = pbr.top;
    r.bottom++;
    }

GetWindowRect( MainWindow, &pbr );
r.top = pbr.bottom;

MoveWindow( MainPlotWindow, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                       CHOOSE_CORNER_PARAMETERS                       *
***********************************************************************/
void choose_corner_parameters()
{
DialogIsActive = TRUE;
DialogBox( MainInstance, TEXT("CHOOSE_PARAMETER_LIST"), MainWindow, (DLGPROC) corner_param_dialog_proc );
}

/***********************************************************************
*                      POSITION_ZOOM_BOX_WINDOW                        *
***********************************************************************/
static void position_zoom_box_window()
{
HWND w;
RECT r;
RECT cr;

if ( MyScreenType == ZABISH_SCREEN_TYPE )
    {
    w = ZoomBoxWindow;
    }
else if ( MyScreenType == STANDARD_SCREEN_TYPE )
    {
    w = SmallZoomBoxWindow;
    }
else
    {
    w = 0;
    }

if ( !w )
    return;

GetWindowRect( w, &r );
cr = my_desktop_client_rect();
OffsetRect( &r, cr.right - r.right, cr.bottom - r.bottom );
MoveWindow( w, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
if ( w == SmallZoomBoxWindow || w == ZoomBoxWindow )
    set_zoom_box_labels( w );
}

/***********************************************************************
*                    SET_ZOOM_SETUP_STRINGS                     *
***********************************************************************/
static void set_zoom_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { ZOOM_POS_STATIC,        TEXT("POSITION_STRING") },
    { ZOOM_TIME_STATIC,       TEXT("TIME_STRING") },
    { ZOOM_VEL_STATIC,        TEXT("VELOCITY_STRING") },
    { ZOOM_START_STATIC,      TEXT("START_STRING") },
    { ZOOM_END_STATIC,        TEXT("END_STRING") },
    { ZOOM_DELTA_STATIC,      TEXT("DELTA_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    CHAN_1_ZOOM_TITLE_TBOX,
    CHAN_2_ZOOM_TITLE_TBOX,
    CHAN_3_ZOOM_TITLE_TBOX,
    CHAN_4_ZOOM_TITLE_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }
for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }
}

/***********************************************************************
*                              ZOOMBOXPROC                             *
***********************************************************************/
int CALLBACK ZoomBoxProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
switch ( msg )
    {
    case WM_INITDIALOG:
        set_zoom_setup_strings( hWnd );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         SMALLZOOMBOXPROC                             *
***********************************************************************/
int CALLBACK SmallZoomBoxProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
switch ( msg )
    {
    case WM_INITDIALOG:
        set_zoom_setup_strings( hWnd );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      FILL_CORNER_PARAM_LISTBOX                       *
***********************************************************************/
static void fill_corner_param_listbox()
{
short i;
short j;
BOOLEAN is_corner;

/*
-----------------------------------
Fill the corner parameters in order
----------------------------------- */
for ( j=0; j<NofCornerParameters; j++ )
    {
    i = CornerParameters[j];
    if ( i > 0 && i < MAX_PARMS )
        {
        i--;
        if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
            SendDlgItemMessage( CornerParamDialogWindow, CORNER_PARAMETER_LISTBOX, LB_ADDSTRING, 0,(LPARAM)(LPSTR) CurrentParam.parameter[i].name );
        }
    }

/*
---------------------------------
Put the rest in the available box
--------------------------------- */
for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE )
        {
        is_corner = FALSE;
        for ( j=0; j<NofCornerParameters; j++ )
            {
            if ( (i+1) == CornerParameters[j] )
                {
                is_corner = TRUE;
                break;
                }
            }
        if ( !is_corner )
            SendDlgItemMessage( CornerParamDialogWindow, AVAILABLE_PARAMETER_LISTBOX, LB_ADDSTRING, 0,(LPARAM)(LPSTR) CurrentParam.parameter[i].name );
        }
    }
}

/***********************************************************************
*                           HANDLE_SELECTION                           *
***********************************************************************/
static void handle_selection( int box )
{
int i;
int otherbox;
LISTBOX_CLASS lb;
STRING_CLASS  s;

otherbox = AVAILABLE_PARAMETER_LISTBOX;
if ( box == otherbox )
    otherbox = CORNER_PARAMETER_LISTBOX;

lb.init( CornerParamDialogWindow, box );
i = lb.selected_index();
if ( i != NO_INDEX )
    {
    s = lb.item_text(i);
    lb.remove( i );
    if ( !s.isempty() )
        {
        lb.init( CornerParamDialogWindow, otherbox );
        lb.add( s.text() );
        }
    }
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes()
{
TCHAR s[PARAMETER_NAME_LEN+1];
LRESULT status;
short parameter_number;
short i;
short j;
short old_count;

old_count = NofCornerParameters;
NofCornerParameters = 0;
for ( j=0; j<MAX_PARMS; j++ )
    {
    parameter_number = NO_PARAMETER_NUMBER;
    status = SendDlgItemMessage( CornerParamDialogWindow, CORNER_PARAMETER_LISTBOX, LB_GETTEXT, j, (LPARAM)(LPSTR) s );
    if ( status != LB_ERR )
        {
        for ( i=0; i<MAX_PARMS; i++ )
            {
            if ( strings_are_equal(s, CurrentParam.parameter[i].name, PARAMETER_NAME_LEN) )
                {
                parameter_number = i+1;
                break;
                }
            }
        }
    CornerParameters[j] = parameter_number;
    if (  parameter_number != NO_PARAMETER_NUMBER )
        NofCornerParameters = j+1;
    }

save_corner_parameters( CornerParameters, NofCornerParameters );
if ( NofCornerParameters != old_count )
    {
    position_parameter_window();
    position_main_plot_window();
    if ( MyScreenType == ZABISH_SCREEN_TYPE )
        position_param_history_window();
    }
}
/***********************************************************************
*                            X4_FROM_DISTANCE                          *
***********************************************************************/
static unsigned x4_from_distance( STRING_CLASS & distance )
{
double d;
int    i;

d = CurrentPart.x4_from_dist( distance.real_value() );
i = (int) round( d, 1.0 );
return (unsigned) i;
}

/***********************************************************************
*                        SAVE_TSL_TBOX_TO_PART                         *
***********************************************************************/
static void save_tsl_tbox_to_part( HWND w )
{
AUTO_BACKUP_CLASS ab;
COMPUTER_CLASS    c;
TCHAR           * cp;
STRING_CLASS      ending;
NAME_CLASS        s;
SETPOINT_CLASS    sp;
unsigned          u;
FTII_VAR_ARRAY    va;

temp_message( resource_string(SAVING_STRING) );
ending.init( UNITS_LEN + 1 );
ending = SpaceString;
ending += units_name( CurrentPart.distance_units );
s.get_partset_csvname( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
if ( sp.get(s) )
    {
    sp[PARTSET_TOTAL_STROKE_LEN_INDEX].value.get_text( w, TSL_TBOX );
    cp = sp[PARTSET_TOTAL_STROKE_LEN_INDEX].value.find( ending );
    if ( cp )
        *cp = NullChar;
    sp.put( s );

    u = x4_from_distance( sp[PARTSET_TOTAL_STROKE_LEN_INDEX].value );
    s.get_ft2_part_settings_file_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    va.get( s );
    va.update( TSL_VN, u );
    va.put( s );

    if ( CurrentMachine.monitor_flags & MA_MONITORING_ON )
        poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, CurrentPart.machine.text() );

    if ( c.find(CurrentPart.computer) )
        {
        if ( c.is_this_computer() )
            {
            ab.get_backup_dir();
            ab.backup_part( CurrentPart.machine, CurrentPart.name );
            }
        }
    }
}

/***********************************************************************
*                       SET_TSL_TBOX_FROM_BISCUIT                      *
***********************************************************************/
static void set_tsl_tbox_from_biscuit( HWND w )
{
STRING_CLASS s;
double       x;

s.init( 30 );

s.get_text( w, STROKE_LENGTH_TBOX );
x = s.real_value();
s.get_text( w, BISCUIT_SIZE_EBOX );
x += s.real_value();
s = x;
s += SpaceChar;
s += units_name( CurrentPart.distance_units );
s.set_text( w, TSL_TBOX );
}

/***********************************************************************
*                             INIT_TSL_BOXES                           *
***********************************************************************/
static void init_tsl_boxes( HWND w )
{
STRING_CLASS s;
double       x;
float      * fp;
float        fmax;
int          n;
int          i;

fp = Points[PosCurve];
n  = (int) Ph.n;
fmax = 0.0;
for ( i=0; i<n; i++ )
    {
    maxfloat( fmax, *fp );
    fp++;
    }

s.init( 30 );
x = (double) fmax;
s = x;
s += SpaceChar;
s += units_name( CurrentPart.distance_units );
s.set_text( w, STROKE_LENGTH_TBOX );

s = CurrentPart.total_stroke_length;
s += SpaceChar;
s += units_name( CurrentPart.distance_units );
s.set_text( w, TSL_TBOX );

w = GetDlgItem( w, BISCUIT_SIZE_EBOX );
s = CurrentPart.total_stroke_length - x;

s.set_text( w );
SetFocus( w );
eb_select_all( w );
}

/***********************************************************************
*                         SET_TSL_SETUP_STRINGS                        *
***********************************************************************/
static void set_tsl_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,                 TEXT("SAVE_TSL") },
    { IDCANCEL,             TEXT("CANCEL") },
    { EVENT_MACHINE_STATIC, TEXT("MACHINE_STRING") },
    { DOWNTIME_STATIC,      TEXT("DOWNTIME_PROMPT") },
    { CURRENT_TSL_STATIC,   TEXT("CURRENT_TSL") },
    { TSL_IS_STATIC,        TEXT("TSL_IS") },
    { PRESS_SAVE_STATIC,    TEXT("SAVE_TSL") },
    { ENTER_BISCUIT_STATIC, TEXT("ENTER_BISCUIT") },
    { MUST_RELOAD_STATIC,   TEXT("MUST_RELOAD") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    EVENT_TIME_TBOX,
    EVENT_MACHINE_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("SET_TSL_CAPTION")) );
w.refresh();
}

/***********************************************************************
*                           SET_TSL_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK set_tsl_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_tsl_setup_strings( hWnd );
        init_tsl_boxes( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {

            case BISCUIT_SIZE_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    set_tsl_tbox_from_biscuit( hWnd );
                    }
                return TRUE;

            case IDOK:
                save_tsl_tbox_to_part( hWnd );

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                     POSITION_CORNER_PARAM_DIALOG                     *
***********************************************************************/
static void position_corner_param_dialog()
{
WINDOW_CLASS    w;
RECTANGLE_CLASS dr;
RECTANGLE_CLASS r;
int             i;
POINT           p;

if ( !CornerParamDialogWindow )
    return;

GetCursorPos( &p );

w = CornerParamDialogWindow;
w.getrect( r );

p.x -= r.width()/2;
p.y -= r.height()/2;

dr = my_desktop_client_rect();
if ( p.x < 0 )
    p.x = 0;

if ( p.y < 0 )
    p.y = 0;

i = dr.right - r.width();
if ( p.x > i )
    p.x = i;

i = dr.bottom - r.height();
if ( p.y > i )
    p.y = i;

w.move( p.x, p.y );
}

/***********************************************************************
*                    SET_CORNER_PARAM_SETUP_STRINGS                    *
***********************************************************************/
static void set_corner_param_setup_strings()
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

static RESOURCE_TEXT_ENTRY rsn[] = {
    { PARAMS_TO_DISPLAY_STATIC, TEXT("PARAMS_TO_DISPLAY") },
    { AVAILABLE_PARAMS_STATIC,  TEXT("AVAILABLE_PARAMS") },
    { IDOK,                     TEXT("OK_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    CORNER_PARAMETER_LISTBOX,
    AVAILABLE_PARAMETER_LISTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( CornerParamDialogWindow, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( CornerParamDialogWindow, box[i] );
    set_statics_font( w );
    }

w = CornerParamDialogWindow;
w.set_title( StringTable.find(TEXT("CHOOSE_PARAM_LIST_CAPTION")) );
w.refresh();
}

/***********************************************************************
*                       CORNER_PARAM_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK corner_param_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CornerParamDialogWindow = hWnd;
        position_corner_param_dialog();
        set_corner_param_setup_strings();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_corner_param_listbox();
        SetFocus( CornerParamDialogWindow );
        return TRUE;

    case WM_COMMAND:
        if ( cmd == LBN_SELCHANGE )
            {
            handle_selection( id );
            return TRUE;
            }

        switch ( id )
            {
            case IDOK:
                save_changes();
                show_corner_parameters();
                if ( MyScreenType == BIG_PARAM_SCREEN_TYPE )
                    InvalidateRect( MainPlotWindow, 0, TRUE );
                else
                    InvalidateRect( ParamHistoryWindow, 0, TRUE );

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, CORNER_PARAMS_MENU_HELP );
        return TRUE;

    case WM_DESTROY:
        CornerParamDialogWindow = 0;
        break;
    }

return FALSE;
}

/***********************************************************************
*                       GET_NEW_DOWNTIME_MACHINE                       *
***********************************************************************/
static void get_new_downtime_machine( LISTBOX_CLASS & lb )
{
STRING_CLASS s;
s = lb.selected_text();
if ( !s.isempty() )
    {
    if ( replace_tab_with_null(s.text()) )
        {
        if ( MachList.find(s) )
            {
            DowntimeComputerName = MachList.computer_name();
            DowntimeMachineName  = MachList.name();
            SendMessage( MainWindow, WM_NEWPART, 0, 0L );
            }
        }
    }
}

/***********************************************************************
*                 FILL_CHOOSE_DOWNTIME_MACHINE_LISTBOX                 *
***********************************************************************/
static BOOLEAN fill_choose_downtime_machine_listbox( LISTBOX_CLASS & lb )
{
int          count;
NAME_CLASS   s;
DB_TABLE     t;
int          last_record;
TIME_CLASS   tc;

count = 0;

MachList.rewind();
while ( MachList.next() )
    {
    s.get_downtime_dbname( MachList.computer_name(), MachList.name() );
    if ( s.file_exists() )
        {
        if ( t.open(s, DOWNTIME_RECLEN, FL) )
            {
            last_record = (int) t.nof_recs();
            last_record--;

            if ( last_record > 0 )
                {
                if ( t.goto_record_number(last_record) )
                    {
                    if ( t.get_rec() )
                        {
                        if ( get_time_class_from_db(tc, t, DOWNTIME_DATE_OFFSET) )
                            {
                            s = MachList.name();
                            s += TabString;
                            s += tc.text();
                            lb.add( s.text() );
                            count++;
                            }
                        }
                    }
                }
            t.close();
            }
        }
    }

if ( count )
    return TRUE;

return FALSE;
}

/***********************************************************************
*              SET_CHOOSE_DOWNTIME_MACHINE_SETUP_STRINGS               *
***********************************************************************/
static void set_choose_downtime_machine_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

static RESOURCE_TEXT_ENTRY rsn[] = {
    { CDM_MACHINE_STATIC, TEXT("MACHINE_STRING") }
    };


const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    DOWNTIME_MACHINE_LISTBOX,
    DOWNTIME_MACHINE_DATE_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("CHOOSE_DOWNTIME_MACHINE")) );
w.refresh();
}

/***********************************************************************
*                CHOOSE_DOWNTIME_MACHINE_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK choose_downtime_machine_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int title_ids[] = {
    DOWNTIME_MACHINE_DATE_TBOX
    };
const int nof_title_ids = sizeof(title_ids)/sizeof(int);

static LISTBOX_CLASS lb;

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_listbox_tabs_from_title_positions( hWnd, DOWNTIME_MACHINE_LISTBOX, title_ids, nof_title_ids );
        lb.init( hWnd, DOWNTIME_MACHINE_LISTBOX );
        set_choose_downtime_machine_setup_strings( hWnd );
        break;

    case WM_DBINIT:
        if ( fill_choose_downtime_machine_listbox(lb) )
            ShowWindow( hWnd, SW_SHOW );
        else
            resource_message_box( MainInstance, NO_DOWNTIME_DATA_STRING, NOTHING_TO_DISPLAY_STRING );
        break;

    case WM_DBCLOSE:
        lb.empty();
        ShowWindow( hWnd, SW_HIDE );
        break;

    case WM_CLOSE:
        SendMessage( hWnd, WM_DBCLOSE, 0, 0L );
        return TRUE;

    case WM_COMMAND:
        if ( cmd == LBN_SELCHANGE )
            {
            get_new_downtime_machine(lb);
            return TRUE;
            }

        switch ( id )
            {
            case IDOK:
            case IDCANCEL:
                SendMessage( hWnd, WM_DBCLOSE, 0, 0L );
                return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                       SAVE_SIGNED_ON_OPERATOR                        *
***********************************************************************/
static void save_signed_on_operator( HWND w )
{
BOOLEAN        have_operator;
COMPUTER_CLASS c;
LISTBOX_CLASS  lb;
MACHINE_NAME_LIST_CLASS m;
NAME_CLASS     s;
SETPOINT_CLASS sp;
WORKER_LIST_CLASS       worker;
DB_TABLE       t;
int            i;
int            n;
TCHAR          operator_number[OPERATOR_NUMBER_LEN + 1];

have_operator = FALSE;
lb.init( w, OPERATOR_LISTBOX );
i = lb.selected_index();
if ( i != NO_INDEX )
    {
    /*
    ----------------------------------------------
    Find this operator in the operator table. This
    is possible because the listbox is not sorted
    ---------------------------------------------- */
    s.get_operator_dbname();
    if ( t.open(s, OPERATOR_RECLEN, PFL) )
        {
        if ( t.goto_record_number(i) )
            {
            if ( t.get_rec() )
                have_operator = t.get_alpha( operator_number, OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
            }
        t.close();
        }
    }

if ( !have_operator )
    {
    s = lb.selected_text();
    MessageBox( NULL, s.text(), resource_string(MainInstance, OPERATOR_NOT_FOUND), MB_OK );
    return;
    }

/*
----------------------------------------------
Put this operator number into all the machines
---------------------------------------------- */
lb.init( w, OPERATOR_MACHINE_LISTBOX );
n = lb.count();

if ( n <= 0 )
    return;

m.add_computer( c.whoami() );
m.rewind();
while ( m.next() )
    {
    s.get_machset_csvname( m.computer_name(), m.name() );
    if ( s.file_exists() )
        {
        if ( sp.get(s) )
            {
            sp[MACHSET_OPERATOR_NUMBER_INDEX].value = operator_number;
            sp.put( s );
            }
        }
    CurrentMachine.human = operator_number;
    worker.load();
    CurrentWorker = worker.full_name( operator_number );
    }
}

/***********************************************************************
*                    FILL_OPERATOR_SIGNON_LISTBOXES                    *
***********************************************************************/
static void fill_operator_signon_listboxes( HWND w )
{
COMPUTER_CLASS c;
LISTBOX_CLASS  lb;
DB_TABLE       t;
NAME_CLASS     s;
STRING_CLASS   lastname;

lb.init( w, OPERATOR_MACHINE_LISTBOX );
lb.empty();

MachList.rewind();
while ( MachList.next() )
    {
    if ( MachList.computer_name() == c.whoami() )
        lb.add( MachList.name().text() );
    }

if ( lb.count() < 1 )
    resource_message_box( MainInstance, NO_MACHINES_HERE, EMPTY_LIST );
else
    lb.select_all();

lb.init( w, OPERATOR_LISTBOX );
lb.empty();

s.get_operator_dbname();
if ( t.open(s, OPERATOR_RECLEN, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( s, OPERATOR_FIRST_NAME_OFFSET, OPERATOR_FIRST_NAME_LEN );
        s += SpaceChar;
        t.get_alpha( lastname, OPERATOR_LAST_NAME_OFFSET,  OPERATOR_LAST_NAME_LEN );
        s += lastname;
        lb.add( s.text() );
        }
    t.close();
    }
}

/***********************************************************************
*                          ENABLE_EDIT_BUTTON                          *
***********************************************************************/
static void enable_edit_button( HWND w )
{
TCHAR * cp;
int32   level_needed;

cp = get_ini_string( VisiTrakIniFile, ButtonLevelsSection, EditOperatorsButtonKey );
if ( !unknown(cp) )
    {
    level_needed = asctoint32( cp );
    if ( CurrentPasswordLevel >= level_needed )
        EnableWindow( GetDlgItem(w,EDIT_OPERATORS_BUTTON), TRUE );
    }
}

/***********************************************************************
*                    SET_OP_SIGNON_SETUP_STRINGS                     *
***********************************************************************/
static void set_op_signon_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,                         TEXT("OK_STRING") },
    { ALL_OPERATOR_MACHINES_BUTTON, TEXT("ALL_STRING") },
    { EDIT_OPERATORS_BUTTON,        TEXT("EDIT_STRING") },
    { OP_OPERATOR_STATIC,           TEXT("OPERATOR") },
    { OP_MACHINE_STATIC,            TEXT("MACHINE_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    OPERATOR_MACHINE_LISTBOX,
    OPERATOR_LISTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("OPERATOR_SIGNON")) );
w.refresh();
}

/***********************************************************************
*                         OPERATOR_SIGNON_PROC                         *
*                        OPERATOR_SIGNON_DIALOG                        *
***********************************************************************/
BOOL CALLBACK operator_signon_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
LISTBOX_CLASS lb;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_op_signon_setup_strings( hWnd );
        enable_edit_button( hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_operator_signon_listboxes(hWnd);
        SetFocus( GetDlgItem(hWnd,OPERATOR_LISTBOX) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case ALL_OPERATOR_MACHINES_BUTTON:
                lb.init( hWnd, OPERATOR_MACHINE_LISTBOX );
                lb.select_all();
                return TRUE;

            case EDIT_OPERATORS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("EDIT_OPERATORS_DIALOG"),
                    hWnd, /*        MainPlotWindow, */
                    (DLGPROC) edit_operators_dialog_proc );
                fill_operator_signon_listboxes(hWnd);
                return TRUE;

            case IDOK:
                save_signed_on_operator( hWnd );

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            DO_POPUP_MENU                             *
***********************************************************************/
static void do_popup_menu( LPARAM lParam )
{
POINT p;

if ( DialogIsActive )
    return;

turn_off_zooming();

p.x = LOWORD( lParam );
p.y = HIWORD( lParam );

ClientToScreen( MainPlotWindow, (LPPOINT) &p );

PopupMenu.empty();

if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
    {
    PopupMenu.add( StringTable.find(TEXT("CHOOSE_DOWNTIME_MACHINE_MENU_STRING")), CHOOSE_DOWNTIME_MACHINE_MENU );
    PopupMenu.add( StringTable.find(TEXT("DOWNBAR_CONFIG_MENU_STRING")), DOWNBAR_CONFIG_MENU );
    }

if ( MyScreenType == TREND_SCREEN_TYPE )
    {
    if ( !RealTime && !GridIsVisible )
        PopupMenu.add( StringTable.find(TEXT("TREND_REALTIME_MENU_STRING")), TREND_REALTIME_MENU );

    PopupMenu.add( StringTable.find(TEXT("TREND_ALLSHOTS_MENU_STRING")), TREND_ALLSHOTS_MENU );
    PopupMenu.add( StringTable.find(TEXT("TREND_CONFIG_MENU_STRING")), TREND_CONFIG_MENU );
    if ( GridIsVisible )
        PopupMenu.add( StringTable.find(TEXT("NO_STAT_SUMMARY_MENU_STRING")), NO_STAT_SUMMARY_MENU );
    else
        PopupMenu.add( StringTable.find(TEXT("STAT_SUMMARY_MENU_STRING")), STAT_SUMMARY_MENU );
    }

if ( MyScreenType == STANDARD_SCREEN_TYPE || MyScreenType == ZABISH_SCREEN_TYPE )
    {
    if ( zoomed() )
        PopupMenu.add( StringTable.find(TEXT("UNZOOM_MENU")), UNZOOM_MENU );
    else
        PopupMenu.add( StringTable.find(TEXT("ZOOM_MENU")), ZOOM_MENU );

    PopupMenu.add( StringTable.find(TEXT("TREND_SCREEN_MENU_STRING")), TREND_SCREEN_MENU );
    PopupMenu.add( StringTable.find(TEXT("BOX_SCREEN_MENU_STRING")), BOX_SCREEN_MENU );
    PopupMenu.add( StringTable.find(TEXT("STATUS_SCREEN_MENU_STRING")), STATUS_SCREEN_MENU );
    PopupMenu.add( StringTable.find(TEXT("DAY_PARAM_SCREEN_MENU_STRING")), DAY_PARAM_SCREEN_MENU );

    if ( InfoBoxType != XDCR_INFO_BOX && !zoomed() )
        PopupMenu.add( StringTable.find(TEXT("XDCR_MENU_STRING")), XDCR_MENU );
    else if ( InfoBoxType == XDCR_INFO_BOX )
        PopupMenu.add( StringTable.find(TEXT("STD_SCREEN_MENU")), PROFILE_SCREEN_MENU );

    if ( CurrentDisplay != POS_TIME_DISPLAY && !zoomed() )
        PopupMenu.add( StringTable.find(TEXT("POS_TIME_MENU")), POS_TIME_MENU );

    if ( CurrentDisplay != POS_ONLY_DISPLAY && !IsHPM ) // && !zoomed() )
        PopupMenu.add( StringTable.find(TEXT("POS_ONLY_MENU")), POS_ONLY_MENU );

    if ( CurrentDisplay != TIME_ONLY_DISPLAY )
        PopupMenu.add( StringTable.find(TEXT("TIME_ONLY_MENU")), TIME_ONLY_MENU );
    }
else
    {
    PopupMenu.add( StringTable.find(TEXT("STD_SCREEN_MENU")), PROFILE_SCREEN_MENU );
    }

if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
    {
    PopupMenu.add( StringTable.find(TEXT("STATUS_SCREEN_MENU_STRING")), STATUS_SCREEN_MENU );
    }

if ( MyScreenType == BOX_SCREEN_TYPE )
    {
    PopupMenu.add( StringTable.find(TEXT("FONT_STATUS_SCREEN_MENU_STRING")), BOX_SCREEN_SET_FONT_MENU );
    }

if ( MyScreenType == STATUS_SCREEN_TYPE && CurrentPasswordLevel >= EditStatusScreenLevel )
    {
    if ( Editing_Status_Screen )
        {
        PopupMenu.add( StringTable.find(TEXT("FONT_STATUS_SCREEN_MENU_STRING")), FONT_STATUS_SCREEN_MENU );
        PopupMenu.add( StringTable.find(TEXT("SAVE_STATUS_SCREEN_MENU_STRING")), SAVE_STATUS_SCREEN_MENU );
        }
    else
        {
        PopupMenu.add( StringTable.find(TEXT("EDIT_STATUS_SCREEN_MENU_STRING")), EDIT_STATUS_SCREEN_MENU );
        }
    }

PopupMenu.show_menu( (int) p.x, (int) p.y );
}

/***********************************************************************
*                            DO_ZOOM_BUTTON                            *
***********************************************************************/
static void do_zoom_button( POINT p )
{
if ( p.x < Region[POS_BASED_ANALOG].rect.left || p.x > Region[TIME_BASED_ANALOG].rect.right )
    return;

if ( !Zooming )
    {
    if ( !zoomed() )
        {
        WasRealtimeBeforeZoom      = RealTime;
        WasSingleMachineBeforeZoom = SingleMachineOnly;
        DisplayBeforeZoom          = CurrentDisplay;

        if ( RealTime )
            {
            RealTime = FALSE;
            SingleMachineOnly = FALSE;
            set_realtime_menu_string();
            }
        }

    if ( HaveHeader && ReadyForZoom )
        {
        Zooming = TRUE;
        CursorStartY = Region[POS_BASED_ANALOG].rect.top;
        CursorEndY   = Region[POS_BASED_COUNT].rect.bottom;

        CursorX = p.x;
        draw_cursor_line( CursorX );
        HaveCursorLine = TRUE;

        set_info_box_type( ZOOM_INFO_BOX );
        show_zoom_info();
        }
    }
else if ( !HaveLeftCursorLine )
    {
    LeftCursorX = p.x;
    draw_cursor_line( LeftCursorX );
    HaveLeftCursorLine = TRUE;
    }
else
    {
    Zooming = FALSE;
    zoom( p );
    }
SetFocus( MainWindow );
}

/***********************************************************************
*                       DO_KEYBOARD_ZOOM_BUTTON                        *
***********************************************************************/
static void do_keyboard_zoom_button()
{
POINT p;

if ( !Zooming )
    p.x = Region[POS_BASED_ANALOG].rect.left + 2;
else
    p.x = CursorX;

p.y = Region[POS_BASED_ANALOG].rect.bottom + Region[POS_BASED_ANALOG].rect.top;
p.y /= 2;
do_zoom_button( p );
}

/***********************************************************************
*                     TOGGLE_CURRENT_DISPLAY_TYPE                      *
***********************************************************************/
static void toggle_current_display_type()
{
int new_display_type;

if ( MyScreenType == TREND_SCREEN_TYPE )
    {
    if ( GridIsVisible )
        GridIsVisible = FALSE;
    else
        GridIsVisible = TRUE;
    }
else
    {
    if ( CurrentDisplay == POS_ONLY_DISPLAY || CurrentDisplay == POS_TIME_DISPLAY )
        {
        LastPosDisplay   = CurrentDisplay;
        new_display_type = TIME_ONLY_DISPLAY;
        }
    else
        {
        new_display_type = LastPosDisplay;
        }

    clear_y_axes();
    set_current_display( new_display_type );
    }

InvalidateRect( MainPlotWindow, 0, TRUE );
}

/***********************************************************************
*                         PAINT_PLOT_BACKGROUND                        *
***********************************************************************/
void paint_plot_background( HWND w, HDC dc )
{
HBRUSH oldbrush;
HBRUSH mybrush;
RECT r;

GetClientRect(w, &r);

mybrush  = (HBRUSH) CreateSolidBrush( CurrentColor[BACKGROUND_COLOR] );
oldbrush = (HBRUSH) SelectObject( dc, mybrush );

FillRect( dc, &r, mybrush );

SelectObject( dc, oldbrush );
DeleteObject( mybrush );
}

/***********************************************************************
*                          TOGGLE_REALTIME                             *
***********************************************************************/
void toggle_realtime()
{
if ( RealTime )
    {
    if ( !SingleMachineOnly || (MyScreenType == TREND_SCREEN_TYPE) )
        RealTime = FALSE;

    SingleMachineOnly = FALSE;
    }
else if ( MyScreenType == TREND_SCREEN_TYPE )
    {
    trend_realtime();
    return;
    }
else
    {
    RealTime = TRUE;
    }

set_realtime_menu_string();
}

/***********************************************************************
*                          SHOW_LAST_SCREEN_TYPE                       *
*                                                                      *
* This lets the program restore whatever was showing the last time     *
* the program was run.                                                 *
*                                                                      *
***********************************************************************/
static void show_last_screen_type()
{
TCHAR * cp;
short   i;

cp = get_ini_string( DisplayIniFile, ConfigSection, TEXT("LastScreenType") );
if ( !unknown(cp) )
    {
    i = (short) asctoint32( cp );
    if ( MyScreenType != i )
        update_screen_type( i );
    }
}

/***********************************************************************
*                       DO_VIEW_PARAMETERS_MENU                        *
***********************************************************************/
void do_view_parameters_menu()
{
turn_off_zooming();
toggle_menu_view_state( ViewParametersDisplay, VIEW_PARAMETERS_MENU );

if ( ViewParametersDisplay )
    {
    ShowWindow( ViewParametersDialogWindow, SW_SHOW );
    fill_view_parameters_listbox();
    }
else
    ShowWindow( ViewParametersDialogWindow, SW_HIDE );
}

/***********************************************************************
*                                SIZE_GRID                             *
***********************************************************************/
static void size_grid()
{
RECT r;

GetClientRect( MainPlotWindow, &r );
r.top += r.bottom;
r.top /= 2;

SetWindowPos( Grid.m_ctrlWnd, 0, r.left, r.top, r.right-r.left, r.bottom-r.top, SWP_NOZORDER );
}

/***********************************************************************
*                              CREATE_GRID                             *
***********************************************************************/
static void create_grid()
{
CUGCell cell;
RECT    r;
int32   col;
int32   i;
UINT    id;

GetClientRect( MainPlotWindow, &r );
r.top += r.bottom;
r.top /= 2;

Grid.CreateGrid( WS_CHILD|WS_VISIBLE, r, MainPlotWindow, 104 );
Grid.GetCell( 0,0,&cell );
Grid.SetNumberCols( NofColumns, FALSE );

i = Grid.GetRowHeight( 0 );
i *= 2;
Grid.SetTH_NumberRows( 2 );
Grid.SetTH_Height( i );

cell.SetText( resource_string(ALARM_LIMITS_STRING) );
Grid.SetCell( LOW_ALARM_COLUMN_NUMBER, -2, &cell );

Grid.JoinCells( LOW_ALARM_COLUMN_NUMBER, -2, HIGH_ALARM_COLUMN_NUMBER, -2 );

Grid.SetSH_Width( 150 );

id = FIRST_STATSUM_STRING;
for ( col=0; col<NOF_STATSUM_STRINGS; col++ )
    {
    cell.SetText( resource_string(id) );
    Grid.SetCell( col, -1, &cell );
    id++;
    }

GridIsVisible = FALSE;
ShowWindow( Grid.m_ctrlWnd, SW_HIDE );
}

/***********************************************************************
*                            GOTO_NEXT_SHOT                            *
*                Previous shot if direction is < 0.                    *
***********************************************************************/
static void goto_next_shot( int direction )
{
int32      new_record_number;
int32      record_number;
int32      last_record;
short      slen;
NAME_CLASS s;
DB_TABLE   t;
BOOLEAN    have_shot;

have_shot = FALSE;

s.get_graphlst_dbname( ComputerName, MachineName, PartName );
if ( t.open(s, 0, PFL) )
    {
    slen = t.field_length( GRAPHLST_SHOT_NAME_INDEX );
    if ( t.is_alpha(GRAPHLST_SHOT_NAME_INDEX) )
        t.put_alpha( GRAPHLST_SHOT_NUMBER_OFFSET, ShotName, slen );
    else
        t.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, ShotName.int_value(), slen );
    if ( t.get_next_key_match( 1, NO_LOCK ) )
        {
        last_record = (int32) t.nof_recs();
        last_record--;
        record_number = t.current_record_number();
        new_record_number = record_number + direction;
        if ( new_record_number < 0 )
            new_record_number = 0;
        else if ( new_record_number > last_record )
            new_record_number = last_record;

        if ( new_record_number != record_number )
            {
            if ( t.goto_record_number( new_record_number) )
                {
                if ( t.get_current_record(NO_LOCK) )
                    {
                    t.get_alpha( ShotName, GRAPHLST_SHOT_NAME_INDEX );
                    ShotName.strip();
                    have_shot = TRUE;
                    }
                }
            }
        }
    t.close();
    }

if ( have_shot )
    SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
}

/***********************************************************************
*                            DO_ZOOM_ARROW_KEYS                        *
***********************************************************************/
BOOLEAN do_zoom_arrow_keys( int key )
{
static int direction;
POINT  p;

if ( !Zooming && !zoomed() )
    {
    /*
    ------------------------------------------------
    See if the operator wants to go to the next shot
    ------------------------------------------------ */
    if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
        {
        direction = 0;
        if ( key == VK_RIGHT )
            direction = 1;
        else if ( key == VK_LEFT )
            direction = -1;
        else if ( key == VK_UP )
            direction = -10;
        else if ( key == VK_DOWN )
            direction = 10;
        if ( direction )
            {
            if ( InfoBoxType == XDCR_INFO_BOX )
                XdcrWindow.post( WM_NEW_DATA, 0, (LPARAM) direction );
            else
                goto_next_shot( direction );
            }
        }
    return FALSE;
    }

if ( key == VK_SPACE || key == VK_RETURN )
    {
    do_keyboard_zoom_button();
    return TRUE;
    }

if ( !Zooming )
    {
    if ( key == VK_LEFT || key == VK_UP )
        p.x = Region[TIME_BASED_ANALOG].rect.right - 2;
    else if ( key == VK_RIGHT || key == VK_DOWN )
        p.x = Region[POS_BASED_ANALOG].rect.left + 2;
    else
        return FALSE;

    p.y = Region[POS_BASED_ANALOG].rect.bottom + Region[POS_BASED_ANALOG].rect.top;
    p.y /= 2;
    do_zoom_button( p );
    return TRUE;
    }

p.x = CursorX;

if ( GetKeyState(VK_SHIFT) < 0 )
    {
    if ( key == VK_LEFT )
        key = VK_UP;
    else if ( key == VK_RIGHT )
        key = VK_DOWN;
    }

switch ( key )
    {
    case VK_DOWN:
         p.x += 10;
        break;

    case VK_RIGHT:
        p.x++;
        break;

    case VK_UP:
        p.x -= 10;
        break;

    case VK_LEFT:
        p.x--;
        break;

    default:
        return FALSE;
    }

if ( p.x < Region[POS_BASED_ANALOG].rect.left || p.x > Region[TIME_BASED_ANALOG].rect.right )
    return 0;

erase_cursor_line();
CursorX = p.x;
if ( !HaveLeftCursorLine )
    LeftCursorX = p.x;
draw_cursor_line( CursorX );
HaveCursorLine = TRUE;
show_zoom_info();
return TRUE;
}

/***********************************************************************
*                          ADD_TO_BACKUP_LIST                          *
***********************************************************************/
void add_to_backup_list()
{
NAME_CLASS   s;
STRING_CLASS line;
FILE_CLASS   f;
DWORD        my_position;
BOOLEAN      need_to_append;
int          i;

center_temp_message();
temp_message( resource_string(ADDING_TO_BACKUP_STRING) );

line.init( ComputerName.len() + MachineName.len() + PartName.len() + ShotName.len() + 3 );
line = ComputerName;
line += CommaChar;
line += MachineName;
line += CommaChar;
line += PartName;
line += CommaChar;
line += ShotName;

need_to_append = FALSE;
s.get_backup_list_csvname();
if ( !s.file_exists() )
    {
    if ( f.open_for_write(s.text()) )
        {
        f.writeline( line.text() );
        f.close();
        }
    return;
    }

if ( f.open_for_update(s.text()) )
    {
    while ( true )
        {
        my_position = f.getpos();
        s = f.readline();
        if ( s.isempty() )
            {
            need_to_append = TRUE;
            break;
            }
        if ( s.countchars(CommaChar) != 3 )
            break;

        s.next_field();
        i = ComputerName.compare(s);
        if ( i > 0 )
            continue;
        if ( i == 0 )
            {
            s.next_field();
            i = MachineName.compare(s);
            if ( i > 0 )
                continue;
            if ( i == 0 )
                {
                s.next_field();
                i = PartName.compare(s);
                if ( i > 0 )
                    continue;
                if ( i == 0 )
                    {
                    s.next_field();
                    i = ShotName.compare( s );
                    if ( i >= 0 )
                        continue;
                    }
                }
            }
        /*
        ---------------------------------------------------------------------------
        If I get here the new record needs to be inserted before the current record
        --------------------------------------------------------------------------- */
        f.setpos( my_position );
        f.insertline( line.text() );
        break;
        }
    if ( need_to_append )
        f.writeline( line.text() );
    f.close();
    }
}

/***********************************************************************
*                             DO_XDCR_CLICK                            *
*   If I click on the screen send the x4 position to XdcrWindow        *
***********************************************************************/
static void do_xdcr_click( POINT p )
{
float x;
double d;
int   axis_index;
WPARAM x4_value;

if ( x_value( x, axis_index, p.x ) )
    {
    if ( axis_index == X_POS_AXIS )
        {
        d = (double) x;
        d = CurrentPart.x4_from_dist( d );
        d = round( d, 1.0 );
        x4_value = (WPARAM) d;
        XdcrWindow.post( WM_NEW_DATA, d, 0 );
        }
    }
}

/***********************************************************************
*                            MAIN_PLOT_PROC                            *
***********************************************************************/
LRESULT CALLBACK main_plot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOOLEAN first_paint = TRUE;
POINT   p;
int     id;
LPNMHDR nm;
MSG     mymsg;

id  = LOWORD( wParam );

if ( MyScreenType == BOX_SCREEN_TYPE && ToolTipWindow )
    {
    switch ( msg )
        {
        case WM_LBUTTONDOWN:
        case WM_MOUSEMOVE:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            mymsg.lParam  = lParam;
            mymsg.wParam  = wParam;
            mymsg.message = msg;
            mymsg.hwnd    = hWnd;
            SendMessage( ToolTipWindow, TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &mymsg );
            break;
        }
    }

switch ( msg )
    {
    case WM_TIMER:
        if ( MyScreenType == STATUS_SCREEN_TYPE )
            {
            KillTimer( hWnd, StatusScreenTimerId );
            StatusScreenTimerId = 0;
            status_screen_timer_update();
            }
        return 0;

    case WM_NOTIFY:
        nm = (LPNMHDR) lParam;
        if ( nm->code == HDN_ENDTRACK && MyScreenType == STATUS_SCREEN_TYPE )
            {
            /*
            ----------------------------------------------------------------------
            The status screen needs to be redisplayed to show the new column width
            ---------------------------------------------------------------------- */
            NeedNewStatusScreenColumnWidths = TRUE;
            InvalidateRect( MainPlotWindow, 0, TRUE );
            }
        break;

    case WM_COMMAND:

        if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
            {
            if ( id == ESCAPE_KEY || (id >= FirstDownCatButton && id <= LastDownCatButton) )
                {
                do_downbar_button( id );
                return 0;
                }
            }

        switch ( id )
            {
            case SPACEBAR_KEY:
                if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
                    do_keyboard_zoom_button();
                return 0;

            case ESCAPE_KEY:
                if ( Zooming )
                    {
                    turn_off_zooming();
                    set_info_box_type( PARAMETER_INFO_BOX );
                    }
                else if ( MyScreenType == TREND_SCREEN_TYPE )
                    trend_escape();
                else if ( MyScreenType == DAY_PARAM_SCREEN_TYPE )
                    update_screen_type( StandardProfileScreenType );
                return 0;

            case F5_KEY:
                toggle_current_display_type();
                return 0;

            case F6_KEY:
                toggle_big_param_display_type();
                return 0;

            case F7_KEY:
                toggle_pos_time_display_type();
                return 0;

            case F8_KEY:
                if ( MyScreenType == TREND_SCREEN_TYPE )
                    trend_allshots();
                return 0;

            case F9_KEY:
                add_to_backup_list();
                return 0;

            case F10_KEY:
                restore_focus_to_hmi();
                return 0;

            case REALTIME_TOGGLE_KEY:
                if ( (MyScreenType != BOX_SCREEN_TYPE) && (MyScreenType != STATUS_SCREEN_TYPE) )
                    toggle_realtime();
                return 0;

            case VIEW_PARAMETERS_KEY:
                do_view_parameters_menu();
                return 0;

            }
        break;

    case WM_SIZE:
        size_plot_window_to_screen();
        size_grid();
        break;

    case WM_CREATE:
        MainPlotWindow = hWnd;
        create_grid();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:

        /*
        ------------------------------------------------
        Set the plot window to the bottom of the z order
        ------------------------------------------------ */
        SetWindowPos( hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        box_display_startup();

        ToolTipWindow = CreateWindowEx(
            0,
            TOOLTIPS_CLASS,     /* Class name */
            0,                  /* Title */
            TTS_ALWAYSTIP,      /* Style */
            CW_USEDEFAULT,      /* x */
            CW_USEDEFAULT,      /* x */
            CW_USEDEFAULT,      /* x */
            CW_USEDEFAULT,      /* x */
            MainPlotWindow,
            0,
            MainInstance,
            0
            );

        set_realtime_menu_string();
        return 0;

    case WM_SETFOCUS:
        if ( ChosshotWindow )
            SetFocus( ChosshotWindow );
        else if ( MyScreenType == DAY_PARAM_SCREEN_TYPE )
            SetFocus( DayParamEb );
        else if ( !GridIsVisible && !Editing_Status_Screen && !DayParamEb )
            SetFocus( MainWindow );
        ReadyForZoom = TRUE;
        return 0;

    case WM_ERASEBKGND:
        paint_plot_background( hWnd, (HDC) wParam );
        return 1;  /* 1 tells windows I have erased the window */

    case WM_PAINT:
        paint_me();
        if ( NeedCornerParamUpdate )
            {
            NeedCornerParamUpdate = FALSE;
            show_corner_parameters();
            if ( ViewParametersDisplay )
                fill_view_parameters_listbox();
            }
        if ( first_paint )
            {
            first_paint = FALSE;
            show_last_screen_type();
            }

        if ( PrintingMultiplePlots && MultipleShotDialogWindow )
            {
            print_me();
            PostMessage( MultipleShotDialogWindow, WM_NEWSHOT, 0, 0L );
            }
        return 0;

    case WM_RBUTTONUP:
        do_popup_menu(lParam);
        return 0;

    case WM_MOUSEMOVE:
        if ( MyScreenType == TREND_SCREEN_TYPE )
            {
            mouse_to_trend( (int32) LOWORD(lParam), (int32) HIWORD(lParam) );
            }
        else if ( HaveHeader && ReadyForZoom )
            {
            if ( Zooming )
                {
                p.x = (LONG) LOWORD(lParam);
                if ( Region[POS_BASED_ANALOG].rect.left <= p.x && p.x <= Region[TIME_BASED_ANALOG].rect.right )
                    {
                    erase_cursor_line();
                    CursorX = p.x;
                    if ( !HaveLeftCursorLine )
                        LeftCursorX = p.x;
                    draw_cursor_line( CursorX );
                    HaveCursorLine = TRUE;
                    show_zoom_info();
                    }
                }
            else
                {
                if ( HaveCursorLine )
                    erase_cursor_line();

                if ( HaveLeftCursorLine )
                    erase_left_cursor_line();
                }
            }
        return 0;

    case WM_LBUTTONUP:
        p.x = (LONG) ( LOWORD(lParam) );
        p.y = (LONG) ( HIWORD(lParam) );

        if ( MyScreenType == TREND_SCREEN_TYPE )
            {
            if ( PtInRect(&RealTimeButtonRect, p) )
                toggle_realtime();
            else
                trend_left_button_up( p );
            }
        else if ( MyScreenType == BOX_SCREEN_TYPE )
            {
            box_screen_left_button_up( p );
            }
        else if ( MyScreenType == STATUS_SCREEN_TYPE )
            {
            status_screen_left_button_up( p );
            }
        else if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
            {
            downbar_left_button_up( p );
            }
        else if ( !Zooming && PtInRect(&RealTimeButtonRect, p) )
            {
            toggle_realtime();
            }
        else if ( Zooming || zoomed() )
            {
            do_zoom_button( p );
            return 0;
            }
        else if ( InfoBoxType == XDCR_INFO_BOX )
            {
            do_xdcr_click( p );
            return 0;
            }

        break;

    case WM_KEYDOWN:
        if ( do_zoom_arrow_keys((int) wParam) )
            return 0;
        break;

    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                     REGISTER_FOR_CURRENT_MACHINE                       *
***********************************************************************/
static void register_for_current_machine()
{
register_for_event( MachList.name().text(), SHOT_NUMBER_INDEX, new_shot_callback );
register_for_event( MachList.name().text(), PART_SETUP_INDEX,  new_part_setup_callback );
}

/***********************************************************************
*                   UNREGISTER_FOR_CURRENT_MACHINE                     *
***********************************************************************/
static void unregister_for_current_machine()
{
unregister_for_event( MachList.name().text(), SHOT_NUMBER_INDEX );
unregister_for_event( MachList.name().text(), PART_SETUP_INDEX  );
}

/***********************************************************************
*                   UNREGISTER_FOR_NEW_SHOT_EVENTS                     *
***********************************************************************/
static void unregister_for_new_shot_events()
{
MachList.rewind();
while ( MachList.next() )
    unregister_for_current_machine();
}

/***********************************************************************
*                             DO_NEWSETUP                              *
***********************************************************************/
static void do_newsetup()
{
COMPUTER_CLASS c;
STRING_CLASS   s;

if ( MachList.find(NewSetupMachine) )
    {
    /*
    ------------------------------------
    See if this machine has been deleted
    ------------------------------------ */
    if ( !machine_exists(MachList.computer_name(), MachList.name()) )
        {
        unregister_for_current_machine();
        MachList.remove();
        }
    else if ( MachList.name() == CurrentMachine.name )
        {
        CurrentMachine.find( ComputerName, MachineName );
        if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
            SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
        }
    }
else
    {
    /*
    ---------------------
    This is a new machine
    --------------------- */
    if ( machine_computer_name(s, NewSetupMachine) )
        {
        MachList.add( s, NewSetupMachine, SORT_BY_MACHINE );
        if ( MachList.find(NewSetupMachine) )
            register_for_current_machine();

        /*
        ---------------------------------------------------------------------
        Add this to the list of machines to monitor if it is on this computer
        --------------------------------------------------------------------- */
        if ( s == c.whoami() )
            {
            if ( !RealtimeMachineList.find(NewSetupMachine.text()) )
                {
                if ( RealtimeMachineList.append(NewSetupMachine.text()) )
                    {
                    RealtimeMachineList.save();
                    begin_box_display();
                    }
                }
            }
        }
    }
}

/***********************************************************************
*                           DOWN_DATA_CALLBACK                         *
***********************************************************************/
void down_data_callback( TCHAR * topic, short item, HDDEDATA edata )
{
DOWN_DATA_CLASS dd;
COMPUTER_CLASS  c;
STRING_CLASS    s;

if ( dd.set(edata) )
    {
    /*
    ----------------------------------------------
    If the computer was offline set it back online
    ---------------------------------------------- */
    if ( MachList.get_computer_name(s, dd.machine_name()) )
        c.set_online_status( s, TRUE );
    if ( MyScreenType == BOX_SCREEN_TYPE )
        box_screen_down_data_change( dd.machine_name(), dd.category(), dd.subcategory() );

    else if ( MyScreenType == STATUS_SCREEN_TYPE )
        status_screen_down_data_change( dd.machine_name(), dd.category(), dd.subcategory() );

    if ( !UseDowntimeEntry )
        return;

    /*
    ----------------------------------------------------------------------------------
    Do nothing if the downtime entry is already displayed (I will miss this downtime).
    ---------------------------------------------------------------------------------- */
    if ( DowntimeEntryDialog )
        return;

    if ( MachList.find(dd.machine_name()) )
        {
        if ( c.whoami() == MachList.computer_name() )
            {
            if ( strings_are_equal(DOWNCAT_SYSTEM_CAT, dd.category()) && strings_are_equal(DOWNCAT_UNSP_DOWN_SUBCAT, dd.subcategory()) )
                {
                DowntimeEntryMachine = dd.machine_name();
                ShowWindow( CreateDialog( MainInstance, TEXT("DOWNTIME_ENTRY_DIALOG"), MainWindow, (DLGPROC) downtime_entry_dialog_proc ), SW_SHOW );
                }
            }
        }
    }
}

/***********************************************************************
*                             SHOW_TIMEOUT_MESSAGE                     *
***********************************************************************/
static void show_timeout_message( TCHAR * machine_name )
{
const COLORREF white = RGB( 255, 255, 255 );

HDC   dc;
RECT  r;
HFONT old_font;
STRING_CLASS s;
SIZE         sz;

dc  = GetDC( MainPlotWindow );
old_font = (HFONT) SelectObject( dc, GetStockObject(SYSTEM_FIXED_FONT) );

s = resource_string( TIMEOUT_STRING );
s += SpaceString;
s += machine_name;

GetTextExtentPoint32( dc, s.text(), s.len(), &sz );

GetClientRect( MainPlotWindow, &r );

r.top+= 2;
r.bottom = r.top + (sz.cy + 6 );
r.left = r.right - (sz.cx + 6);

fill_rectangle( dc, r, AlarmColor );
draw_rectangle( dc, r, white );

SetTextAlign( dc, TA_TOP | TA_LEFT );
SetTextColor( dc, CurrentColor[TEXT_COLOR] );

SetBkMode( dc, TRANSPARENT );

TextOut( dc, r.left+2, r.top+2, s.text(), s.len() );

SelectObject( dc, old_font );

ReleaseDC( MainPlotWindow, dc );
}

/***********************************************************************
*                      NEW_MACHINE_STATE_CALLBACK                      *
***********************************************************************/
void new_machine_state_callback( TCHAR * topic, short item, HDDEDATA edata )
{
NEW_MACHINE_STATE_CLASS ms;

if ( !ms.set(edata) )
    return;

if ( MyScreenType == BOX_SCREEN_TYPE )
    box_screen_new_machine_status( ms.machine_name(), ms.new_status() );
else if ( MyScreenType == STATUS_SCREEN_TYPE )
    status_screen_new_machine_status( ms.machine_name(), ms.new_status() );
else
    {
    /*if ( strings_are_equal(ms.machine_name(), MachineName) ) */
    if ( ms.new_status() & VIS_TIME_OUT )
        show_timeout_message( ms.machine_name().text() );
    }
}

/***********************************************************************
*                      NEW_MACHINE_SETUP_CALLBACK                      *
***********************************************************************/
void new_machine_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
COMPUTER_CLASS c;
STRING_CLASS   s;

if ( get_dde_data(NewSetupMachine, edata) )
    {
    if ( MachList.get_computer_name(s,NewSetupMachine) )
        c.set_online_status( s, TRUE );
    SendMessage( MainWindow, WM_NEWSETUP, 0, 0L );
    }
}

/***********************************************************************
*                        MONITOR_SETUP_CALLBACK                        *
***********************************************************************/
void monitor_setup_callback( TCHAR * topic, short item, HDDEDATA edata )
{
STRING_CLASS   s;
COMPUTER_CLASS c;

if ( get_dde_data(s, edata) )
    {
    if ( c.count() > 1 )
        {
        box_screen_new_monitor_setup( s );
        }
    else
        {
        if ( MyScreenType == BOX_SCREEN_TYPE )
            end_box_display();
        load_realtime_machine_list();
        if ( MyScreenType == BOX_SCREEN_TYPE )
            {
            begin_box_display();
            InvalidateRect( MainPlotWindow, 0, TRUE );
            }
        }

    /*
    --------------------------------------------
    Reload the current machine if it is this one
    -------------------------------------------- */
    if ( s == MachineName )
        CurrentMachine.find( ComputerName, MachineName );
    }
}

/***********************************************************************
*                          UPDATE_SCREEN_TYPE                          *
***********************************************************************/
void update_screen_type( short new_screen_type )
{
static BOOLEAN was_realtime       = FALSE;
static BOOLEAN was_single_machine = FALSE;
short old_screen_type;


if ( new_screen_type == PROFILE_SCREEN_TYPE || new_screen_type == STANDARD_SCREEN_TYPE || new_screen_type == ZABISH_SCREEN_TYPE )
    {
    /*
    -------------------------------------
    See if I am turning off the xdcr test
    ------------------------------------- */
    if ( InfoBoxType == XDCR_INFO_BOX )
        set_info_box_type( PARAMETER_INFO_BOX );
    }

if ( new_screen_type == MyScreenType )
    {
    return;
    }

if ( new_screen_type == STANDARD_SCREEN_TYPE || new_screen_type == ZABISH_SCREEN_TYPE )
    StandardProfileScreenType = new_screen_type;

if ( zoomed() || Zooming )
    {
    turn_off_zooming();
    unzoom( FALSE );
    }
else
    {
    clear_axes();
    }

switch ( MyScreenType )
    {
    case STATUS_SCREEN_TYPE:
        end_status_display();
        break;

    case BOX_SCREEN_TYPE:
        end_box_display();
        break;

    case TREND_SCREEN_TYPE:
        end_trend_display();
        RealTime          = was_realtime;
        SingleMachineOnly = was_single_machine;
        break;

    case DOWNBAR_SCREEN_TYPE:
        end_downbar_display();
        RealTime          = was_realtime;
        SingleMachineOnly = was_single_machine;
        break;

    case DAY_PARAM_SCREEN_TYPE:
        end_day_param_display();
        RealTime          = was_realtime;
        SingleMachineOnly = was_single_machine;
        break;

    default:
        break;
    }

old_screen_type = MyScreenType;

/*
-------------------------------------------------------------------------
If you ask for a profile screen you get whichever standard was last used.
------------------------------------------------------------------------- */
if ( new_screen_type == PROFILE_SCREEN_TYPE )
    MyScreenType = StandardProfileScreenType;
else
    MyScreenType = new_screen_type;

position_parameter_window();
position_zoom_box_window();
position_main_plot_window();
if ( MyScreenType == ZABISH_SCREEN_TYPE )
    position_param_history_window();

show_corner_parameters();

SetFocus( MainWindow );

switch ( MyScreenType )
    {
    case STATUS_SCREEN_TYPE:
        begin_status_display();
        InvalidateRect( MainPlotWindow, 0, TRUE );
        break;

    case BOX_SCREEN_TYPE:
        begin_box_display();
        InvalidateRect( MainPlotWindow, 0, TRUE );
        break;

    case TREND_SCREEN_TYPE:
        was_realtime       = RealTime;
        was_single_machine = SingleMachineOnly;
        begin_trend_display();
        break;

    case DAY_PARAM_SCREEN_TYPE:
        was_realtime       = RealTime;
        was_single_machine = SingleMachineOnly;
        begin_day_param_display();
        break;

    case DOWNBAR_SCREEN_TYPE:
        begin_downbar_display();

        /* if ( !begin_downbar_display() )
            {
            resource_message_box( MainInstance, NO_DOWNTIME_DATA_STRING, NOTHING_TO_DISPLAY_STRING );
            update_screen_type( PROFILE_SCREEN_TYPE );
            return;
            }
        */
        was_realtime       = RealTime;
        was_single_machine = SingleMachineOnly;
        RealTime           = FALSE;
        SingleMachineOnly  = FALSE;

        if ( old_screen_type == TREND_SCREEN_TYPE || old_screen_type == BOX_SCREEN_TYPE || old_screen_type == STATUS_SCREEN_TYPE )
            InvalidateRect( MainPlotWindow, 0, TRUE );
        break;

    case BIG_PARAM_SCREEN_TYPE:
        if ( old_screen_type == TREND_SCREEN_TYPE || old_screen_type == BOX_SCREEN_TYPE || old_screen_type == STATUS_SCREEN_TYPE )
            InvalidateRect( MainPlotWindow, 0, TRUE );
        break;

    default:
        load_main_menu();
        break;
    }

set_zoom_grayness();
}

/***********************************************************************
*                         IS_CURRENT_SHOT                              *
* I used to check for the computer but machines are unique (v6.38).    *
***********************************************************************/
BOOLEAN is_current_shot( PROFILE_NAME_ENTRY * p )
{
if ( p->machine == MachineName )
    if ( p->part == PartName )
        if ( p->shot == ShotName )
            return TRUE;

return FALSE;
}

/***********************************************************************
*                          REMOVE_OVERLAY_SHOT                         *
***********************************************************************/
static void remove_overlay_shot( int dead_shot )
{
int i;

/*
------------------------------------
Do nothing if there is only one shot
------------------------------------ */
if ( NofOverlayShots < 2 )
    return;

if ( dead_shot < 0 || dead_shot >= NofOverlayShots )
    return;

NofOverlayShots--;
for ( i=dead_shot; i<NofOverlayShots; i++ )
    OverlayShot[i] = OverlayShot[i+1];

}

/***********************************************************************
*                         APPEND_OVERLAY_SHOT                          *
*  If the new shot is already on the list, Remove it unless it is      *
*  the only one. Return TRUE only if I add to the list;                *
***********************************************************************/
static BOOLEAN append_overlay_shot()
{
int i;

if ( NofOverlayShots > 1 )
    {
    for ( i=0; i<NofOverlayShots; i++ )
        {
        if ( is_current_shot(OverlayShot+i) )
            {
            remove_overlay_shot(i);
            return FALSE;
            }
        }
    }

/*
------------------------------------------
Remove the oldest shot if the list is full
------------------------------------------ */
if ( NofOverlayShots >= MAX_OVERLAY_SHOTS )
    remove_overlay_shot(0);

OverlayShot[NofOverlayShots].computer = ComputerName;
OverlayShot[NofOverlayShots].machine  = MachineName;
OverlayShot[NofOverlayShots].part     = PartName;
OverlayShot[NofOverlayShots].shot     = ShotName;

NofOverlayShots++;
return TRUE;
}

/***********************************************************************
*                              DO_NEWSHOT                              *
***********************************************************************/
static void do_newshot()
{
COMPUTER_CLASS  c;
BOOLEAN         need_reset;
int             old_count;

ReadyForZoom = FALSE;
HaveHeader   = FALSE;
need_reset   = TRUE;

c.set_online_status( ComputerName, TRUE );
old_count = NofCornerParameters;
load_part_info();
HaveNewXdcrPart = true;

if ( MyScreenType == BOX_SCREEN_TYPE )
    {
    load_profile();
    box_screen_new_shot( MachineName );
    return;
    }
else if ( MyScreenType == STATUS_SCREEN_TYPE )
    {
    status_screen_new_shot( MachineName );
    return;
    }
else if ( MyScreenType == TREND_SCREEN_TYPE )
    {
    trend_screen_new_shot();
    return;
    }
else if ( MyScreenType == BIG_PARAM_SCREEN_TYPE )
    {
    InvalidateRect( MainPlotWindow, 0, TRUE );
    return;
    }

if ( overlaying() )
    {
    if ( append_overlay_shot() )
        {
        get_mydc();
        load_profile();
        paint_the_curves();
        show_shot_info();
        show_corner_parameters();
        release_mydc();
        need_reset = FALSE;
        }
    else
        {
        load_part_info( OverlayShot );
        }
    }

if ( need_reset )
    {
    if ( PartBasedDisplay && old_count != NofCornerParameters )
        {
        position_parameter_window();
        position_main_plot_window();
        if ( MyScreenType == ZABISH_SCREEN_TYPE )
            position_param_history_window();
        }
    reset_plot_curves();
    InvalidateRect( MainPlotWindow, 0, TRUE );
    NeedCornerParamUpdate = TRUE;
    }

if ( zoomed() )
    show_zoom_info();
else if ( IsWindowVisible(ParamHistoryWindow) )
    InvalidateRect( ParamHistoryWindow, 0, TRUE );
else if ( InfoBoxType == XDCR_INFO_BOX )
    XdcrWindow.refresh();

ReadyForZoom = TRUE;
}

/***********************************************************************
*                         SAVE_LAST_SHOT_NAME                          *
***********************************************************************/
void save_last_shot_name()
{
put_ini_string( DisplayIniFile, ConfigSection, LastMachineKey, MachineName.text() );
put_ini_string( DisplayIniFile, ConfigSection, LastPartKey,    PartName.text() );
put_ini_string( DisplayIniFile, ConfigSection, LastShotKey,    ShotName.text() );
}

/***********************************************************************
*                         REDISPLAY_LAST_SHOT                          *
***********************************************************************/
BOOLEAN redisplay_last_shot()
{
NAME_CLASS s;
INI_CLASS    ini;

s.get_local_ini_file_name( DisplayIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( !ini.find(LastMachineKey) )
    return FALSE;
s = ini.get_string();

if ( !MachList.find(s) )
    return FALSE;

ComputerName = MachList.computer_name();
MachineName  = s;

if ( !ini.find(LastPartKey) )
    return FALSE;

PartName = ini.get_string();
s.get_partset_csvname( ComputerName, MachineName, PartName );
if ( !s.file_exists() )
    {
    PartName.null();
    return FALSE;
    }

if ( !ini.find(LastShotKey) )
    return FALSE;

ShotName = ini.get_string();
return TRUE;
}

/***********************************************************************
*                             FILL_VERSION                             *
***********************************************************************/
void fill_version( HWND w )
{
TCHAR * cp;

cp = get_ini_string( VisiTrakIniFile, ConfigSection, TEXT("Version") );
if ( cp )
    set_text( w, ABOUT_VERSION_TEXTBOX, cp );

cp = get_ini_string( VisiTrakIniFile, ConfigSection, SerialNumberKey );
if ( cp )
    if ( !unknown(cp) )
        set_text( w, ABOUT_SERIAL_EBOX, cp );
}

/***********************************************************************
*                          SAVE_SERIAL_NUMBER                          *
***********************************************************************/
static bool save_serial_number( HWND w )
{
STRING_CLASS s;
TCHAR * cp;

w = GetDlgItem( w, ABOUT_SERIAL_EBOX );

s.get_text( w );
s.uppercase();
cp = s.text();
if ( *cp == NChar )
    {
    cp++;
    put_ini_string( VisiTrakIniFile, ConfigSection, SerialNumberKey, cp );
    set_text( w, cp );
    return true;
    }
return false;
}

/***********************************************************************
*                        SET_ABOUT_SETUP_STRINGS                       *
***********************************************************************/
static void set_about_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

static RESOURCE_TEXT_ENTRY rsn[] = {
    { TT2020_STATIC     , TEXT("TT2020") },
    { PROCESS_MON_STATIC, TEXT("PROCESS_MON") },
    { VERSION_STATIC    , TEXT("VERSION") },
    { SN_STATIC         , TEXT("SN_PROMPT") },
    { IDOK,               TEXT("OK_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    ABOUT_VERSION_TEXTBOX,
    ABOUT_SERIAL_EBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.refresh();
}

/***********************************************************************
*                           ABOUT_DIALOG_PROC                          *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK about_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        fill_version( hWnd );
        set_about_setup_strings( hWnd );
        return TRUE;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( save_serial_number(hWnd) )
                    return TRUE;
            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/**********************************************************************
*                            fill_languages                           *
**********************************************************************/
void fill_languages( HWND hWnd )
{
static wchar_t UnicodeIntroducer = L'\xFEFF';

TCHAR       * cp;
FILE_CLASS    f;
int           i;
LISTBOX_CLASS lb;
int           nof_languages;
NAME_CLASS    s;

lb.init( hWnd, LANGUAGE_LB );

s.get_exe_dir_file_name( ProfileMenuFile );

if ( f.open_for_read(s) )
    {
    /*
    ----------------------------
    Read the number of languages
    ---------------------------- */
    cp = f.readline();

    #ifdef UNICODE
    if ( *cp == UnicodeIntroducer )
        cp++;
    #endif
    excel_to_csv( cp );
    nof_languages = asctoint32( cp );

    /*
    ---------------------------------
    For each language, read the font.
    --------------------------------- */
    for ( i=0; i<nof_languages; i++ )
        {
        cp = f.readline();
        excel_to_csv( cp );
        if ( replace_char_with_null(cp, CommaChar) )
            lb.add( cp );
        }
    f.close();
    }
}

/**********************************************************************
*                          do_language_selection                      *
**********************************************************************/
void do_language_selection( HWND hWnd )
{
INI_CLASS     ini;
NAME_CLASS    s;
LISTBOX_CLASS lb;

s.get_local_ini_file_name( DisplayIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

lb.init( hWnd, LANGUAGE_LB );
s = lb.selected_text();
if ( !s.isempty() )
    {
    ini.put_string( CurrentLanguageKey, s.text() );
    CurrentLanguage = s;
    MainMenu.language = s;
    load_main_menu();
    position_main_plot_window();
    StringTable.set_language( CurrentLanguage );
    s.get_stringtable_name( ProfileResourceFile );
    StringTable.read( s );
    s.get_stringtable_name( CommonResourceFile );
    StringTable.append( s );
    load_parameter_list_titles();
    PopupMenu.set_font( MainMenu.lf );
    }
}

/***********************************************************************
*                    SET_CHOOSE_LANGUAGE_SETUP_STRINGS                 *
***********************************************************************/
static void set_choose_language_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static UINT box[] = {
    LANGUAGE_LB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("CHOOSE_LANG_TITLE")) );
w.refresh();
}

/***********************************************************************
*                       CHOOSE_LANGUAGE_DIALOG_PROC                    *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK choose_language_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;
int     cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        fill_languages( hWnd );
        set_choose_language_setup_strings( hWnd );
        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case LANGUAGE_LB:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_language_selection(hWnd);
                    DialogIsActive = FALSE;
                    EndDialog( hWnd, 0 );
                    }
                break;

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                    POSITION_MACHINE_LIST_LISTBOX                     *
***********************************************************************/
static void position_machine_list_listbox( HWND hWnd )
{
RECTANGLE_CLASS r;
RECTANGLE_CLASS rc;
WINDOW_CLASS w;

w = hWnd;
w.get_client_rect( r );

w = w.control( MACHINE_LIST_LB );
w.get_move_rect( rc );
rc.left = r.left + 4;
rc.right = r.right - 4;
rc.bottom = r.bottom - 4;
w.move(rc);
}

/***********************************************************************
*                       MAKE_A_LIST_OF_MACHINES                        *
***********************************************************************/
static void make_a_list_of_machines( HWND hWnd )
{
STRING_CLASS   address;
DB_TABLE       t;
LISTBOX_CLASS  lb;
NAME_CLASS     s;
SETPOINT_CLASS sp;
int            board;
BOOLEAN        monitoring_is_on;
BOOLEAN        exists;

lb.init( hWnd, MACHINE_LIST_LB );
lb.set_tabs( COMPUTER_NAME_STATIC, ADDRESS_STATIC );

s.upsize( MAX_PATH );
MachList.rewind();
while ( MachList.next() )
    {
    board = 0;
    monitoring_is_on = FALSE;
    address.null();
    s.get_machset_csvname( MachList.computer_name(), MachList.name() );

    exists = s.file_exists();
    if ( exists )
        {
        if ( sp.get(s) )
            {
            board = sp[MACHSET_FT_BOARD_NO_INDEX].int_value();
            if ( MA_MONITORING_ON & sp[MACHSET_MONITOR_FLAGS_INDEX].uint_value() )
                monitoring_is_on = TRUE;
            }

        s.get_ft2_boards_dbname( MachList.computer_name() );
        if ( s.file_exists() )
            {
            t.open( s, FTII_BOARDS_RECLEN, PFL );
            while ( t.get_next_record(FALSE) )
                {
                if ( board == (int) t.get_short(FTII_BOARDS_NUMBER_OFFSET) )
                    {
                    t.get_alpha( address, FTII_BOARDS_ADDRESS_OFFSET, TCP_ADDRESS_LEN );
                    break;
                    }
                }
            }
        }

    s = MachList.name();
    s += TabChar;
    s += MachList.computer_name();
    s += TabChar;
    if ( exists )
        {
        if ( monitoring_is_on )
            s += YChar;
        else
            s += NChar;
        s += TabChar;
        s += board;
        s += TabChar;
        s += address;
        }
    else
        {
        s += resource_string( OFF_LINE_STRING );
        }
    lb.add( s.text() );
    }
}

/***********************************************************************
*                          SORT_MACHINE_LIST                           *
***********************************************************************/
static void sort_machine_list( HWND hWnd, int sort_type )
{
static TCHAR    tabstring[] =  TEXT( "\t" );
int             i;
int             n;
LISTBOX_CLASS   lb;
RECTANGLE_CLASS rc;
RECTANGLE_CLASS rm;
WINDOW_CLASS    w;
WINDOW_CLASS    wc;
WINDOW_CLASS    wm;
TEXT_LIST_CLASS t;
STRING_CLASS    s;
STRING_ARRAY_CLASS sa;

w = hWnd;
wc = w.control( COMPUTER_NAME_STATIC );
wm = w.control( MACHINE_NAME_STATIC );

wc.get_move_rect( rc );
wm.get_move_rect( rm );

if ( (sort_type == ML_SORT_BY_COMPUTER) && (rc.left < rm.left) )
    return;

if ( (sort_type == ML_SORT_BY_MACHINE) && (rm.left < rc.left) )
    return;

/*
---------------------------------------------------------
Swap the column titles for the machine and computer names
--------------------------------------------------------- */
wc.move( rm.left, rm.top );
wm.move( rc.left, rc.top );

lb.init( hWnd, MACHINE_LIST_LB );
n = lb.count();
for ( i=0; i<n; i++ )
    {
    sa = lb.item_text( i );
    sa.split( tabstring );
    sa.swap( 0, 1 );
    sa.join( tabstring );
    t.append( sa.text() );
    }
lb.empty();
t.rewind();
while ( t.next() )
    lb.add( t.text() );
}

/***********************************************************************
*                      COPY_MACHINE_LIST_TO_CLIPBOARD                  *
***********************************************************************/
static void copy_machine_list_to_clipboard( HWND hWnd )
{
LISTBOX_CLASS lb;
int          i;
int          n;
DWORD        slen;
DWORD        nof_bytes;
HGLOBAL      mymem;
HGLOBAL      newmem;
TCHAR      * buf;
TCHAR      * cp;
STRING_CLASS s;

lb.init( hWnd, MACHINE_LIST_LB );

n = lb.count();
if ( n > 0 )
    {
    slen = 4 + 2; /* 4 tabs + crlf */
    s.get_text( hWnd, MACHINE_NAME_STATIC );
    slen += s.len() + 2;
    s.get_text( hWnd, COMPUTER_NAME_STATIC );
    slen += s.len() + 2;
    s.get_text( hWnd, BOARD_NUMBER_STATIC );
    slen += s.len() + 2;
    s.get_text( hWnd, IS_MONITORED_STATIC );
    slen += s.len() + 2;
    s.get_text( hWnd, ADDRESS_STATIC );
    slen += s.len() + 2;

    for ( i=0; i<n; i++ )
        {
        s = lb.item_text( i );
        slen += s.len() + 2;
        }
    slen++;
    slen *= sizeof( TCHAR );

    mymem = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, slen );
    if ( !mymem )
        {
        resource_message_box( NO_MEMORY_STRING, CANNOT_COMPLY_STRING );
        return;
        }

    buf = (TCHAR *) GlobalLock(mymem);
    if ( !buf )
        {
        resource_message_box( NO_MEMORY_STRING, CANNOT_COMPLY_STRING );
        GlobalFree( mymem );
        return;
        }

    cp = buf;

    s.get_text( hWnd, MACHINE_NAME_STATIC );
    cp = copy_w_char( cp, s.text(), TabChar );
    s.get_text( hWnd, COMPUTER_NAME_STATIC );
    cp = copy_w_char( cp, s.text(), TabChar );
    s.get_text( hWnd, IS_MONITORED_STATIC );
    cp = copy_w_char( cp, s.text(), TabChar );
    s.get_text( hWnd, BOARD_NUMBER_STATIC );
    cp = copy_w_char( cp, s.text(), TabChar );
    s.get_text( hWnd, ADDRESS_STATIC );
    cp = copystring( cp, s.text() );
    cp = copystring( cp, CrLfString );
    for ( i=0; i<n; i++ )
        {
        s = lb.item_text( i );
        cp = copystring( cp, s.text() );
        cp = copystring( cp, CrLfString );
        }
    *cp = NullChar;
    cp++;

    GlobalUnlock(mymem);

    if ( DWORD(cp) > DWORD(buf) )
        {
        nof_bytes = DWORD(cp) - DWORD(buf);
        if ( nof_bytes < slen )
            {
            newmem = GlobalReAlloc( mymem, nof_bytes, 0 );
            if ( !newmem )
                {
                GlobalFree( mymem );
                return;
                }
            else
                mymem = newmem;
            }

        if ( OpenClipboard(hWnd) )
            {
            EmptyClipboard();
            if ( SetClipboardData(ClipFormat, mymem) )
                mymem = 0;
            CloseClipboard();
            }
        }

    if ( mymem )
        GlobalFree( mymem );
    }
}

/***********************************************************************
*                    SET_MACHINE_LIST_SETUP_STRINGS                    *
***********************************************************************/
static void set_machine_list_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

static RESOURCE_TEXT_ENTRY rsn[] = {
    { MACHINE_NAME_STATIC,  TEXT("MACHINE_STRING") },
    { COMPUTER_NAME_STATIC, TEXT("COMPUTER") },
    { BOARD_NUMBER_STATIC,  TEXT("BOARD") },
    { IS_MONITORED_STATIC,  TEXT("MONITORED") },
    { ADDRESS_STATIC,       TEXT("ADDRESS") },
    { SORT_BY_COMPUTER_PB,  TEXT("SORT_BY_CO") },
    { SORT_BY_MACHINE_PB,   TEXT("SORT_BY_MA") },
    { SAVE_TO_CLIPBOARD_PB, TEXT("SAVE_TO_CLIP") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    MACHINE_LIST_LB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("LIST_OF_MA")) );
w.refresh();
}

/***********************************************************************
*                        MACHINE_LIST_DIALOG_PROC                      *
* A modal dialog that displays a list of the machines I know about     *
* and the computers and directories associated with them.              *
***********************************************************************/
BOOL CALLBACK machine_list_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        make_a_list_of_machines( hWnd );
        set_machine_list_setup_strings( hWnd );
        return TRUE;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_SIZE:
        position_machine_list_listbox( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case SORT_BY_COMPUTER_PB:
                sort_machine_list( hWnd, ML_SORT_BY_COMPUTER );
                return TRUE;

            case SORT_BY_MACHINE_PB:
                sort_machine_list( hWnd, ML_SORT_BY_MACHINE );
                return TRUE;

            case SAVE_TO_CLIPBOARD_PB:
                copy_machine_list_to_clipboard( hWnd );
                return TRUE;

            case IDCANCEL:
            case IDOK:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                             CLOSE_DIALOG                             *
***********************************************************************/
void close_dialog( HWND w )
{
DialogIsActive = FALSE;
EndDialog( w, 0 );
}

/***********************************************************************
*                       ENABLE_PREV_NEXT_BUTTONS                       *
***********************************************************************/
void enable_prev_next_buttons( HWND w )
{
BOOLEAN by_shift;
BOOLEAN by_day;
BOOLEAN prev_is_enabled;
BOOLEAN next_is_enabled;
HWND    prev_window;
HWND    next_window;
HWND    focus_window;

by_shift = is_checked(w, BY_SHIFT_RADIO );
by_day   = is_checked(w, BY_DAY_RADIO );

prev_is_enabled = FALSE;
next_is_enabled = FALSE;

if ( by_day )
    {
    if ( !samedate(CurrentAlarmDate, FirstAlarmDate)  )
        prev_is_enabled = TRUE;

    if ( !samedate(CurrentAlarmDate, LastAlarmDate)  )
        next_is_enabled = TRUE;
    }
else if ( by_shift )
    {
    if ( !samedate(CurrentAlarmDate, FirstAlarmDate) || CurrentAlarmShift != FirstAlarmShift )
        prev_is_enabled = TRUE;

    if ( !samedate(CurrentAlarmDate, LastAlarmDate) || CurrentAlarmShift != LastAlarmShift )
        next_is_enabled = TRUE;
    }

prev_window  = GetDlgItem( w, PREV_BUTTON );
next_window  = GetDlgItem( w, NEXT_BUTTON );
focus_window = GetFocus();

EnableWindow( prev_window, prev_is_enabled );
EnableWindow( next_window, next_is_enabled );

if ( focus_window == prev_window && !prev_is_enabled )
    {
    if ( next_is_enabled )
        SetFocus( next_window );
    else
        SetFocus( GetDlgItem(w, ALARM_RADIO) );
    }
else if ( focus_window == next_window && !next_is_enabled )
    {
    if ( prev_is_enabled )
        SetFocus( prev_window );
    else
        SetFocus( GetDlgItem(w, ALARM_RADIO) );
    }
}

/***********************************************************************
*                           NEXT_TIME_PERIOD                           *
***********************************************************************/
static void next_time_period( HWND w, int button_id )
{
BOOLEAN    by_shift;
BOOLEAN    status;
DB_TABLE   t;
NAME_CLASS filename;
SYSTEMTIME mydate;
int32      myshift;

by_shift = is_checked(w, BY_SHIFT_RADIO );

filename.get_alarmsum_dbname( ComputerName, MachineName, PartName );
if ( !filename.file_exists() )
    return;

if ( !t.open(filename, ALARMSUM_RECLEN, PFL) )
    return;

t.put_date( ALARMSUM_DATE_OFFSET,  CurrentAlarmDate );
status = t.get_next_key_match( 1, NO_LOCK );
if ( status && by_shift )
    {
    status = FALSE;
    while ( TRUE )
        {
        t.get_date( mydate, ALARMSUM_DATE_OFFSET );
        if ( !samedate( mydate, CurrentAlarmDate) )
            break;

        myshift = t.get_long( ALARMSUM_SHIFT_OFFSET );
        if ( myshift == CurrentAlarmShift )
            {
            status = TRUE;
            break;
            }

        if ( !t.get_next_record(NO_LOCK) )
            break;
        }
    }

if ( status )
    {
    myshift = CurrentAlarmShift;
    mydate  = CurrentAlarmDate;

    while ( TRUE )
        {
        if ( button_id == NEXT_BUTTON )
            status = t.get_next_record( NO_LOCK );
        else
            status = t.get_prev_record( NO_LOCK );

        if ( !status )
            break;

        t.get_date( mydate, ALARMSUM_DATE_OFFSET );
        myshift = t.get_long( ALARMSUM_SHIFT_OFFSET );

        if ( !samedate( mydate, CurrentAlarmDate) )
            break;

        if ( by_shift && myshift != CurrentAlarmShift )
            break;
        }

    if ( status )
        {
        CurrentAlarmShift = myshift;
        CurrentAlarmDate  = mydate;
        enable_prev_next_buttons( w );
        }
    }

t.close();
}

/***********************************************************************
*                        GET_ALARM_DATE_EXTENTS                        *
***********************************************************************/
static BOOLEAN get_alarm_date_extents()
{
DB_TABLE   t;
NAME_CLASS s;
BOOLEAN    status;
int32      n;

init_systemtime_struct( FirstAlarmDate );
init_systemtime_struct( LastAlarmDate );

s.get_alarmsum_dbname( ComputerName, MachineName, PartName );

if ( !s.file_exists() )
    return FALSE;

if ( !t.open(s, ALARMSUM_RECLEN, PFL) )
    return FALSE;

n = t.nof_recs();

status = FALSE;
if ( n > 0 )
    {
    if (  t.get_next_record(NO_LOCK) )
        {
        t.get_date( FirstAlarmDate, ALARMSUM_DATE_OFFSET );
        FirstAlarmShift = t.get_long( ALARMSUM_SHIFT_OFFSET );
        n--;
        if ( t.goto_record_number(n) )
            {
            if ( t.get_rec() )
                {
                t.get_date( LastAlarmDate, ALARMSUM_DATE_OFFSET );
                LastAlarmShift = t.get_long( ALARMSUM_SHIFT_OFFSET );
                status = TRUE;
                }
            }
        }
    }

t.close();
return status;
}

/***********************************************************************
*                            ALARM_LB_STRING                           *
***********************************************************************/
static TCHAR * alarm_lb_string( HWND w, int32 i, ALARM_SUMMARY_CLASS & as )
{
const  int32 BUFLEN = PARAMETER_NAME_LEN + 1 + PARMLIST_FLOAT_LEN + 1 + ALARMSUM_COUNTS_LEN + 2 + PARMLIST_FLOAT_LEN + 1 + ALARMSUM_COUNTS_LEN +1;
static TCHAR buf[BUFLEN+1];
TCHAR      * cp;
int32        low_count;
int32        high_count;
int32        total_count;
int32        low_type;
int32        high_type;
BOOLEAN      is_percent;

total_count = as.total_shot_count();
if ( total_count <= 0 )
    total_count = 1;

if ( is_checked(w, ALARM_RADIO) )
    {
    low_type  = ALARM_MIN;
    high_type = ALARM_MAX;
    }
else
    {
    low_type  = WARNING_MIN;
    high_type = WARNING_MAX;
    }

low_count  = as.count( i+1, low_type );
high_count = as.count( i+1, high_type );

is_percent = is_checked(w, PERCENT_RADIO );
if ( is_percent )
    {
    low_count *= 100;
    low_count /= total_count;
    high_count *= 100;
    high_count /= total_count;
    }

cp = buf;

cp = copy_w_char( cp, CurrentParam.parameter[i].name, TabChar );
cp = copy_w_char( cp, ascii_float(CurrentParam.parameter[i].limits[low_type].value), TabChar );

cp = copystring( cp, int32toasc(low_count) );
if ( is_percent )
    *cp++ = PercentChar;
*cp++ = TabChar;

cp = copy_w_char( cp, ascii_float(CurrentParam.parameter[i].limits[high_type].value), TabChar );

cp = copystring( cp, int32toasc(high_count) );
if ( is_percent )
    *cp++ = PercentChar;
*cp = NullChar;

return buf;
}

/***********************************************************************
*                          GET_ALARM_PERIOD_TYPE                       *
***********************************************************************/
void get_alarm_period_type( BOOLEAN & by_shift, BOOLEAN & by_date )
{
if ( AlarmSummaryDialogWindow )
    {
    by_shift = is_checked(AlarmSummaryDialogWindow, BY_SHIFT_RADIO );

    if ( by_shift )
        by_date = TRUE;
    else
        by_date  = is_checked(AlarmSummaryDialogWindow, BY_DAY_RADIO );
    }
}

/***********************************************************************
*                         DATE_AND_SHIFT_STRING                        *
*                          "MMDDYYYY Shift 10"                         *
***********************************************************************/
TCHAR * date_and_shift_string( TIME_CLASS & t, int32 shift )
{
static TCHAR * buf = 0;
static TCHAR * cp;
int32          slen;

if ( !buf )
    {
    cp = resource_string(SHIFT_STRING);
    slen = lstrlen(cp) + ALPHADATE_LEN + 2;
    buf = maketext( slen );
    }

if ( buf )
    {
    cp = copy_w_char( buf, t.mmddyyyy(), SpaceChar );
    cp = copy_w_char( cp, resource_string(SHIFT_STRING), SpaceChar );
    lstrcpy( cp, int32toasc(shift) );
    return buf;
    }

return EmptyString;
}

/***********************************************************************
*                          PRINT_ALARM_SUMMARY                         *
***********************************************************************/
static void print_alarm_summary()
{
ALARM_SUMMARY_CLASS as;
BOOLEAN             by_shift;
BOOLEAN             by_date;
BOOLEAN             is_alarm;
BOOLEAN             is_percent;
int32               i;
int32               n;
int32               dx;
int32               dy;
int32               low_count;
int32               high_count;
int32               total_count;
int32               x;
int32               x_low_limit;
int32               x_low_value;
int32               x_high_limit;
int32               x_high_value;
int32               y;
int32               low_type;
int32               high_type;
TCHAR               buf[ALARMSUM_COUNTS_LEN+3];
TCHAR             * cp;
TEXT_LEN_CLASS      tmax;
STRING_CLASS        s;
UINT                rid;

if ( !AlarmSummaryDialogWindow )
    return;

if ( is_checked(AlarmSummaryDialogWindow, ALARM_RADIO) )
    {
    is_alarm  = TRUE;
    low_type  = ALARM_MIN;
    high_type = ALARM_MAX;
    }
else
    {
    is_alarm = FALSE;
    low_type  = WARNING_MIN;
    high_type = WARNING_MAX;
    }

dx  = MyCharWidth;
x   = dx;

dy  = character_height( MyDc );
y   = dy;

as.set_part( ComputerName, MachineName, PartName );
get_alarm_period_type( by_shift, by_date );

s = resource_string( MACHINE_INTRO_STRING );
s += MachineName;
s += resource_string( PART_INTRO_STRING );
s += PartName;
if ( by_date )
    {
    s += TwoSpacesString;
    s += alphadate( CurrentAlarmDate );
    }

if ( by_shift )
    {
    s += TwoSpacesString;
    s += resource_string( SHIFT_STRING );
    s += int32toasc( CurrentAlarmShift );
    as.get_counts( CurrentAlarmDate, CurrentAlarmShift );
    }
else if ( by_date )
    {
    as.get_counts( CurrentAlarmDate );
    }
else
    {
    as.get_counts();
    s += TwoSpacesString;
    s += resource_string( DOWNTIME_FROM_STRING );
    s += date_and_shift_string( as.start_time(), as.start_shift_number() );
    s += resource_string( DOWNTIME_TO_STRING );
    s += date_and_shift_string( as.end_time(), as.end_shift_number() );
    }

SetTextAlign( MyDc, TA_TOP | TA_CENTER );
text_out( PlotWindowRect.right/2, y, s.text() );

y += 2*dy;

SetTextAlign( MyDc, TA_TOP | TA_LEFT );

/*
------------------------------------------
Get the width of the widest parameter name
------------------------------------------ */
tmax.init( MyDc );

n = CurrentParam.count();
for ( i=0; i<n; i++ )
    tmax.check( CurrentParam.parameter[i].name );

x_low_limit  = x + tmax.width() + dx;

tmax.init( MyDc );

if ( is_alarm )
    {
    tmax.check( resource_string(LOW_ALARM_STRING) );
    tmax.check( resource_string(HIGH_ALARM_STRING) );
    }
else
    {
    tmax.check( resource_string(LOW_WARNING_STRING) );
    tmax.check( resource_string(HIGH_WARNING_STRING) );
    }
tmax.check( resource_string(QUANTITY_BELOW_STRING) );
tmax.check( resource_string(QUANTITY_ABOVE_STRING) );

dx *= 7;
maxint32( dx, tmax.width() );
dx += MyCharWidth;

x_low_value  = x_low_limit  + dx;
x_high_limit = x_low_value  + dx;
x_high_value = x_high_limit + dx;

text_out( x, y, resource_string(PARAMETER_STRING) );

if ( is_alarm )
    rid = LOW_ALARM_STRING;
else
    rid = LOW_WARNING_STRING;

text_out( x_low_limit, y, resource_string(rid) );
text_out( x_low_value, y, resource_string(QUANTITY_BELOW_STRING) );

if ( is_alarm )
    rid = HIGH_ALARM_STRING;
else
    rid = HIGH_WARNING_STRING;
text_out( x_high_limit, y, resource_string(rid) );

text_out( x_high_value, y, resource_string(QUANTITY_ABOVE_STRING) );

y += 2*dy;


total_count = as.total_shot_count();
if ( total_count <= 0 )
    total_count = 1;

is_percent = is_checked(AlarmSummaryDialogWindow, PERCENT_RADIO );

for ( i=0; i<n; i++ )
    {
    text_out( x, y, CurrentParam.parameter[i].name );

    low_count  = as.count( i+1, low_type );
    high_count = as.count( i+1, high_type );

    if ( is_percent )
        {
        low_count *= 100;
        low_count /= total_count;
        high_count *= 100;
        high_count /= total_count;
        }

    text_out( x_low_limit, y, ascii_float(CurrentParam.parameter[i].limits[low_type].value) );

    cp = buf;
    cp = copystring( cp, int32toasc(low_count) );
    if ( is_percent )
        *cp++ = PercentChar;
    *cp = NullChar;
    text_out( x_low_value, y, buf );

    text_out( x_high_limit, y, ascii_float(CurrentParam.parameter[i].limits[high_type].value) );

    cp = buf;
    cp = copystring( cp, int32toasc(high_count) );
    if ( is_percent )
        *cp++ = PercentChar;
    *cp = NullChar;
    text_out( x_high_value, y, buf );
    y += dy;
    }

y += dy;

s = resource_string( TOTAL_SHOTS_INTRO_STRING );
s += int32toasc( total_count );
text_out( x, y, s.text() );
y += dy;

if ( is_alarm )
    {
    s = resource_string( TOTAL_ALARM_SHOTS_INTRO_STRING );
    i = as.total_alarm_count();
    }
else
    {
    i = as.total_warning_count();
    s = resource_string( TOTAL_WARNING_SHOTS_INTRO_STRING );
    }

if ( is_percent )
    {
    i *= 100;
    i /= total_count;
    }

s += int32toasc( i );
if ( is_percent )
    s += PercentString;

text_out( x, y, s.text() );
}

/***********************************************************************
*                            GET_ALARM_DATA                            *
***********************************************************************/
static void get_alarm_data( HWND w )
{
ALARM_SUMMARY_CLASS as;
LISTBOX_CLASS       lb;
int32               i;
int32               n;
BOOLEAN             by_shift;
BOOLEAN             by_date;
BOOLEAN             is_percent;
TCHAR             * cp;
STRING_CLASS        s;
INT                 top_index;

hourglass_cursor();

lb.init( w, ALARM_SUMMARY_LISTBOX );

as.set_part( ComputerName, MachineName, PartName );

get_alarm_period_type( by_shift, by_date );

if ( by_date )
    cp = alphadate( CurrentAlarmDate );
else
    cp = EmptyString;

set_text( w, ALARM_DATE_TBOX, cp );

if ( by_shift )
    {
    s = resource_string( SHIFT_STRING );
    s += int32toasc( CurrentAlarmShift );
    cp = s.text();
    }
else
    {
    cp = EmptyString;
    }

set_text( w, ALARM_SHIFT_TBOX, cp );

if ( by_shift )
    as.get_counts( CurrentAlarmDate, CurrentAlarmShift );
else if ( by_date )
    as.get_counts( CurrentAlarmDate );
else
    as.get_counts();

if ( lb.count() )
    top_index = lb.top_index();
else
    top_index = NO_INDEX;

lb.redraw_off();
lb.empty();

n = CurrentParam.count();

for ( i=0; i<n; i++ )
    lb.add( alarm_lb_string(w, i, as) );

if ( top_index != NO_INDEX )
    lb.set_top_index( top_index );

lb.redraw_on();

n = as.total_shot_count();
set_text( w, ALARM_TOTAL_SHOTS_TBOX, int32toasc(n) );

if ( n <= 0 )
    n = 1;

if ( is_checked(w, ALARM_RADIO) )
    i = as.total_alarm_count();
else
    i = as.total_warning_count();

is_percent = is_checked(w, PERCENT_RADIO );
if ( is_percent )
    {
    i *= 100;
    i /= n;
    }

s = int32toasc( i );

if ( is_percent )
    s += PercentString;

set_text( w, ALARM_OUT_TBOX, s.text() );

restore_cursor();
}

/***********************************************************************
*                      SET_ALARM_SUMMARY_TAB_STOPS                     *
***********************************************************************/
static void set_alarm_summary_tab_stops( HWND w )
{
static int tabs[] = {
    ALARM_LOW_TITLE_TBOX,
    QUAL_BELOW_STATIC,
    ALARM_HIGH_TITLE_TBOX,
    QUAL_ABOVE_STATIC
    };
const int nof_tabs = sizeof(tabs)/sizeof(int);
set_listbox_tabs_from_title_positions( w, ALARM_SUMMARY_LISTBOX, tabs, nof_tabs );
}

/***********************************************************************
*                    SET_ALARM_SUMMARY_SETUP_STRINGS                     *
***********************************************************************/
static void set_alarm_summary_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

static RESOURCE_TEXT_ENTRY rsn[] = {
    { PERCENT_RADIO,         TEXT("PERCENT_STRING") },
    { COUNT_RADIO,           TEXT("COUNT") },
    { BY_SHIFT_RADIO,        TEXT("BY_SHIFT") },
    { BY_DAY_RADIO,          TEXT("BY_DAY") },
    { ALL_DATA_RADIO,        TEXT("ALL_STRING") },
    { WARNING_RADIO,         TEXT("WARNING_STRING") },
    { ALARM_RADIO,           TEXT("ALARM_STRING") },
    { PREV_BUTTON,           TEXT("PREV_SHIFT_STRING") },
    { NEXT_BUTTON,           TEXT("NEXT_SHIFT_STRING") },
    { ALARM_OUT_TITLE_TBOX,  TEXT("TOTAL_ALARM_SHOTS_STRING") },
    { ALARM_LOW_TITLE_TBOX,  TEXT("LOW_ALARM_STRING") },
    { ALARM_HIGH_TITLE_TBOX, TEXT("HIGH_ALARM_STRING") },
    { AS_PARAM_STATIC,       TEXT("PARAMETER_STRING") },
    { QUAL_BELOW_STATIC,     TEXT("QUANTITY_BELOW_STRING") },
    { QUAL_ABOVE_STATIC,     TEXT("QUANTITY_ABOVE_STRING") },
    { AS_QUAL_STATIC,        TEXT("QUANTITY") },
    { TIME_SPAN_STATIC,      TEXT("TIME_SPAN") },
    { AS_ALARM_TYPE_STATIC,  TEXT("ALARM_TYPE") },
    { AS_TOTAL_SHOTS_STATIC, TEXT("TOTAL_SHOTS_STRING") }
    };
const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    ALARM_MACHINE_TBOX,
    ALARM_PART_TBOX,
    ALARM_DATE_TBOX,
    ALARM_SHIFT_TBOX,
    ALARM_SUMMARY_LISTBOX,
    ALARM_OUT_TBOX,
    ALARM_TOTAL_SHOTS_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("ALARM_SUMMARY")) );
w.refresh();
}

/***********************************************************************
*                           ALARM_SUMMARY_PROC                         *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK alarm_summary_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
HWND w;
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        set_alarm_summary_tab_stops( hWnd );
        MachineName.set_text( hWnd, ALARM_MACHINE_TBOX );
        PartName.set_text( hWnd, ALARM_PART_TBOX );
        CheckRadioButton( hWnd, PERCENT_RADIO, COUNT_RADIO, PERCENT_RADIO );
        CheckRadioButton( hWnd, BY_SHIFT_RADIO, ALL_DATA_RADIO, BY_SHIFT_RADIO );
        CheckRadioButton( hWnd, WARNING_RADIO,  ALARM_RADIO,    ALARM_RADIO );
        if ( !get_alarm_date_extents() )
            {
            resource_message_box( MainInstance, NO_ALARM_DATA_STRING, NOTHING_TO_DISPLAY_STRING );
            close_dialog( hWnd );
            return TRUE;
            }

        CurrentAlarmDate  = LastAlarmDate;
        CurrentAlarmShift = LastAlarmShift;
        enable_prev_next_buttons( hWnd );
        AlarmSummaryDialogWindow = hWnd;
        get_alarm_data( hWnd );
        set_alarm_summary_setup_strings( hWnd );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, ALARM_SUMMARY_SCREEN_HELP );
        return TRUE;

    case WM_DBCLOSE:
    case WM_CLOSE:
        AlarmSummaryDialogWindow = 0;
        close_dialog( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
            case IDOK:
                close_dialog( hWnd );
                return TRUE;

            case BY_SHIFT_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    w = GetDlgItem( hWnd, PREV_BUTTON );
                    set_text( w, resource_string(PREV_SHIFT_STRING) );
                    EnableWindow( w, TRUE );

                    w = GetDlgItem( hWnd, NEXT_BUTTON );
                    set_text( w, resource_string(NEXT_SHIFT_STRING) );
                    enable_prev_next_buttons( hWnd );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            case BY_DAY_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    w = GetDlgItem( hWnd, PREV_BUTTON );
                    set_text( w, resource_string(PREV_DAY_STRING) );
                    EnableWindow( w, TRUE );

                    w = GetDlgItem( hWnd, NEXT_BUTTON );
                    set_text( w, resource_string(NEXT_DAY_STRING) );
                    enable_prev_next_buttons( hWnd );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            case ALL_DATA_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    w = GetDlgItem( hWnd, PREV_BUTTON );
                    set_text( w, resource_string(PREV_STRING) );

                    w = GetDlgItem( hWnd, NEXT_BUTTON );
                    set_text( w, resource_string(NEXT_STRING) );
                    enable_prev_next_buttons( hWnd );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            case ALARM_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    set_text( hWnd, ALARM_OUT_TITLE_TBOX, resource_string(TOTAL_ALARM_SHOTS_STRING) );
                    set_text( hWnd, ALARM_LOW_TITLE_TBOX, resource_string(LOW_ALARM_STRING) );
                    set_text( hWnd, ALARM_HIGH_TITLE_TBOX, resource_string(HIGH_ALARM_STRING) );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            case WARNING_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    set_text( hWnd, ALARM_OUT_TITLE_TBOX, resource_string(TOTAL_WARNING_SHOTS_STRING) );
                    set_text( hWnd, ALARM_LOW_TITLE_TBOX, resource_string(LOW_WARNING_STRING) );
                    set_text( hWnd, ALARM_HIGH_TITLE_TBOX, resource_string(HIGH_WARNING_STRING) );
                    get_alarm_data( hWnd );
                    }

            case COUNT_RADIO:
            case PERCENT_RADIO:
                if ( cmd == BN_CLICKED )
                    get_alarm_data( hWnd );

                return TRUE;

            case PREV_BUTTON:
            case NEXT_BUTTON:
                if ( cmd == BN_CLICKED )
                    {
                    next_time_period( hWnd, id );
                    get_alarm_data( hWnd );
                    }
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                    SET_ALARM_LOG_SETUP_STRINGS                     *
***********************************************************************/
static void set_alarm_log_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { ALARM_LOG_TO_FILE_XBOX,          TEXT("LOG_TO_FILE") },
    { ALARM_LOG_TO_PRINTER_XBOX,       TEXT("LOG_TO_PRINTER") },
    { CHOOSE_ALARM_LOG_PRINTER_BUTTON, TEXT("CHOOSE_PRINTER") },
    { CHOOSE_ALARM_LOG_FILE_BUTTON,    TEXT("CHOOSE_FILE") },
    { ALARM_LOG_SETUP_OK_BUTTON,       TEXT("SAVE_CHANGES") },
    { ALARM_LOG_MAX_LINES_STATIC,      TEXT("MAX_LINES") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    ALARM_LOG_FILE_EBOX,
    ALARM_LOG_MAX_LINES_EBOX,
    ALARM_LOG_PRINTER_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("ALARM_LOG_SETUP")) );
w.refresh();
}

/***********************************************************************
*                          ALARM_LOG_SETUP_PROC                        *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK alarm_log_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        set_alarm_log_setup_strings( hWnd );
        return TRUE;

    case WM_DBCLOSE:
    case WM_CLOSE:
        close_dialog( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
            case ALARM_LOG_SETUP_OK_BUTTON:
                close_dialog( hWnd );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          write_std_units_csv_file                    *
***********************************************************************/
BOOLEAN write_std_units_csv_file( STRING_CLASS & destfile, STRING_CLASS & sorcfile )
{
static BOOLEAN firstime = TRUE;
static ANALOG_SENSOR AnalogSensor;
static TCHAR title[] = TEXT( "Type,Sample,Pos,Time,Velocity,Inputs,Outputs,A1,A2,A3,A4,A5,A6,A7,A8" );
FILE_CLASS f;
STRING_CLASS s;
const int last_channel = NOF_FTII_ANALOGS - 1;
int          ai;
int          i;
int          c;
double       real_timer_frequency;
double       x;
FTII_PROFILE fp;

if ( firstime )
    {
    AnalogSensor.get();
    firstime = FALSE;
    }

if ( !fp.get(sorcfile) )
    return FALSE;

real_timer_frequency = (double) fp.timer_frequency;

if ( !f.open_for_write(destfile) )
    return FALSE;

f.writeline( title );

/*
-----------------------------------------
The next row is the units for each column
----------------------------------------- */
s = TEXT( "Pos/Time,Count," );
s += units_name( CurrentPart.distance_units );
s += CommaChar;
s += units_name( SECOND_UNITS );
s += CommaChar;
s += units_name( CurrentPart.velocity_units );
s += TEXT( ",Hex Bits,Hex Bits" );
for ( c=0; c<NOF_FTII_ANALOGS; c++ )
    {
    s += CommaChar;
    ai = (int) CurrentPart.analog_sensor[c];
    s += units_name( AnalogSensor[ai].units_index() );
    }
f.writeline( s.text() );

for ( i=0; i<fp.n; i++ )
    {
    if ( i < fp.np )
        s = TEXT( "P," );
    else
        s = TEXT( "T," );

    s.cat_w_char( int32toasc( int32(i)),           CommaChar );

    x = CurrentPart.dist_from_x4( (double) fp.position[i] );
    s += x;
    s += CommaChar;

    x = (double) fp.timer[i];
    x /= real_timer_frequency;
    s += x;
    s += CommaChar;

    x = (double) fp.velocity[i];
    x = CurrentPart.velocity_from_x4( x );
    s += x;
    s += CommaChar;

    s.cat_w_char( ultohex( fp.isw1[i]), CommaChar );
    s += ultohex( fp.osw1[i] );

    for ( c=0; c<NOF_FTII_ANALOGS; c++ )
        {
        s += CommaChar;
        ai = (int) CurrentPart.analog_sensor[c];
        s += AnalogSensor[ai].converted_value( fp.analog[c][i] );
        }
    f.writeline( s.text() );
    }

f.close();

return TRUE;
}

/***********************************************************************
*                          SET_CSV_SHOT_NAME                           *
***********************************************************************/
static void set_csv_shot_name( HWND hWnd )
{
STRING_CLASS s;
STRING_CLASS myshotname;

s.get_text( hWnd, CSV_SHOT_FILE_EBOX );
if ( s.isempty() )
    s = TEXT( "C:\\" );
else
    dir_from_filename( s.text() );
myshotname = MachineName;
myshotname += MinusChar;
myshotname += PartName;
myshotname += TEXT( "-Shot-" );
myshotname += ShotName;
myshotname.replace_all( SpaceChar, MinusChar );
s.cat_path( myshotname );
s += CSV_SUFFIX;
s.set_text( hWnd, CSV_SHOT_FILE_EBOX );
}

/***********************************************************************
*                      SET_CSV_SHOT_FILE_STRINGS                       *
***********************************************************************/
static void set_csv_shot_file_strings( HWND hWnd )
{
STRING_CLASS s;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK, TEXT("SAVE") },
    { IDCANCEL, TEXT("CANCEL") },
    { CSV_FILE_NAME_STATIC, TEXT("FILE_NAME") },
    { SAVE_COUNTS_RADIO, TEXT("COUNTS") },
    { SAVE_RAW_COUNTS_RADIO, TEXT("RAW_COUNTS") },
    { SAVE_STD_UNITS_RADIO, TEXT("STD_UNITS") },
    { USE_SHOT_NUMBER_XBOX, TEXT("USE_SHOT_NUMBER_XBOX") },
    };
const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);
int i;

for ( i=0; i<nof_rsn; i++ )
    {
    s = StringTable.find( rsn[i].s );
    s.set_text( hWnd, rsn[i].id );
    }
}

/***********************************************************************
*                       CSV_SHOT_FILE_DIALOG_PROC                      *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK csv_shot_file_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static TCHAR counts_type[]     = TEXT( "COUNTS" );
static TCHAR raw_counts_type[] = TEXT( "RAW" );
static TCHAR std_units_type[]  = TEXT( "STD" );
static BOOLEAN use_shot_number = FALSE;

TCHAR   * cp;
int  id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        {
        TCHAR * cp;
        set_csv_shot_file_strings( hWnd );
        cp = get_ini_string( DisplayIniFile, ConfigSection, CsvShotFileNameKey );
        if ( !unknown(cp) )
            set_text( hWnd, CSV_SHOT_FILE_EBOX, cp );
        cp = get_ini_string( DisplayIniFile, ConfigSection, CsvShotFileTypeKey );
        id = SAVE_COUNTS_RADIO;
        if ( strings_are_equal(cp, raw_counts_type) )
            id = SAVE_RAW_COUNTS_RADIO;
        else if ( strings_are_equal(cp, std_units_type) )
            id = SAVE_STD_UNITS_RADIO;
        CheckRadioButton( hWnd, SAVE_COUNTS_RADIO, SAVE_STD_UNITS_RADIO, id );
        use_shot_number = boolean_from_ini( DisplayIniFile, ConfigSection, CsvUseShotNumberKey );
        set_checkbox( hWnd, USE_SHOT_NUMBER_XBOX, use_shot_number );
        if ( use_shot_number )
            set_csv_shot_name( hWnd );
        return TRUE;
        }

    case WM_COMMAND:
        switch ( id )
            {
            case USE_SHOT_NUMBER_XBOX:
                use_shot_number = is_checked( hWnd, USE_SHOT_NUMBER_XBOX );
                if ( use_shot_number )
                    set_csv_shot_name( hWnd );
                return TRUE;

            case IDOK:
                {
                NAME_CLASS dest;
                NAME_CLASS sorc;
                sorc.get_profile_ii_name( ComputerName, MachineName, PartName, ShotName );
                dest.get_text( hWnd, CSV_SHOT_FILE_EBOX );
                if ( is_checked(hWnd, SAVE_RAW_COUNTS_RADIO) )
                    {
                    cp = raw_counts_type;
                    write_counts_file( dest, sorc );
                    }
                else if ( is_checked(hWnd, SAVE_STD_UNITS_RADIO) )
                    {
                    cp = std_units_type;
                    write_std_units_csv_file( dest, sorc );
                    }
                else
                    {
                    cp = counts_type;
                    write_olmsted_file( dest, sorc );
                    }

                put_ini_string( DisplayIniFile, ConfigSection, CsvShotFileNameKey, dest.text() );
                put_ini_string( DisplayIniFile, ConfigSection, CsvShotFileTypeKey, cp );
                }
            case IDCANCEL:
                boolean_to_ini( DisplayIniFile, ConfigSection, CsvUseShotNumberKey, use_shot_number );
                close_dialog( hWnd );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                              RUN_MONALL                              *
***********************************************************************/
static void run_monall()
{
static STARTUPINFO info = {
    sizeof(STARTUPINFO),
    0, 0, 0,            // lpReserved, lpDesktop, lpTitle
    0,                  // dwX
    0,                  // dwY
    0, 0, 0, 0, 0,      // dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute
    0,                  // dwFlags
    0, 0, 0, 0, 0, 0    // wShowWindow, cbReserved2, lpReserved2, hStdInput, hStdOutput, hStdError
    };

NAME_CLASS path;
PROCESS_INFORMATION pi;

if ( path.get_exe_dir_file_name(TEXT("monall.exe")) )
    {
    CreateProcess( 0,          // lpApplicationName
        path.text(),           // lpCommandLine
        0, 0, 0,               // lpProcessAttributes, lpThreadAttributes, bInheritHandles,
        HIGH_PRIORITY_CLASS,   // dwCreationFlags
        0, 0,                  // lpEnvironment, lpCurrentDirectory,
        &info,                 // lpStartupInfo = input
        &pi                    // lpProcessInformation = output
        );
    }
}

/***********************************************************************
*                               RUN_TOOL                               *
***********************************************************************/
static void run_tool( int id )
{
const   int NOF_EXES = 16;
NAME_CLASS path;

static TCHAR * taskname[] =
    {
    TEXT("editpart"),
    TEXT("emachine"),
    TEXT("monall"),
    TEXT("monedit"),
    TEXT("esensor"),
    TEXT("bupres"),
    TEXT("v5setups"),
    TEXT("downtime"),
    TEXT("editdown"),
    TEXT("suretrak"),
    TEXT("visiedit"),
    TEXT(""),                  //This was inadvertantly assigned to Edit Shifts
    TEXT("stddown"),
    TEXT(""),                  //This was inadvertantly assigned to Edit Shifts
    TEXT("shiftrpt"),
    TEXT("convert")
    };

id -= PART_EDITOR_MENU;

if ( id == 2 )
    {
    run_monall();
    }
else if ( id < NOF_EXES )
    {
    if ( path.get_exe_dir_file_name( taskname[id]) )
        {
        path += TEXT(".exe");
        if ( id == 0 || id == 8 )
            {
            path.upsize( path.len() + MachineName.len() + PartName.len() + 2 );
            path += SpaceChar;
            path.cat_w_char( MachineName, CommaChar );
            path += PartName;
            }
        }
    execute( path.text() );
    }
}

/***********************************************************************
*                         RUN_EXTERNAL_PROGRAM                         *
***********************************************************************/
static BOOLEAN run_external_program( int id )
{
FILE_CLASS f;
NAME_CLASS s;
TCHAR    * cp;

s.get_external_program_list_csvname();
if ( !s.file_exists() )
    return FALSE;

if ( f.open_for_read(s) )
    {
    while ( cp = f.readline() )
        {
        s = cp;
        if ( s.isempty() )
            break;

        if ( s.next_field() )
            {
            if ( s.int_value() == id )
                {
                if ( s.next_field() )
                    {
                    f.close();
                    execute( s.text() );
                    return TRUE;
                    }
                }
            }
        }
    f.close();
    }

return FALSE;
}

/***********************************************************************
*                           GET_CURRENT_SHOT                           *
***********************************************************************/
BOOLEAN get_current_shot()
{
PLOOKUP_CLASS p;

p.get_last_shot_name_only( ShotName, ComputerName, MachineName, PartName );
fix_shotname( ShotName );

return TRUE;
}

/***********************************************************************
*                       CREATE_MENU_HELP_BOX                           *
***********************************************************************/
void create_menu_help_box()
{
const UINT STATUS_BOX_ID = 2;
RECT r;
long x;
long y;
long h;
long w;

MenuHelpWindow = CreateStatusWindow(
    WS_CHILD | WS_VISIBLE,
    TEXT("Menu Help"),
    MainPlotWindow,
    STATUS_BOX_ID
    );

if ( MenuHelpWindow )
    {
    GetWindowRect( MenuHelpWindow, &r );
    h = r.bottom - r.top;
    w = r.right  - r.left;

    GetClientRect( MainPlotWindow, &r);
    x = r.left;
    y = r.bottom - h;
    y -= 2;
    MoveWindow( MenuHelpWindow, x, y, w, h, TRUE );
    }
}

/***********************************************************************
*                         SHOW_MENU_HELP_TEXT                          *
***********************************************************************/
void show_menu_help_text( UINT id )
{

struct MENU_HELP_TEXT_ENTRY {
    UINT id;
    UINT resource_id;
    };

static MENU_HELP_TEXT_ENTRY helplist[] =
    {
    { CHOOSE_PART_MENU,          CHOOSE_PART_MENU_HELP          },
    { CHOOSE_SHOT_MENU,          CHOOSE_SHOT_MENU_HELP          },
    { EDIT_SHIFTS_MENU,          EDIT_SHIFTS_MENU_HELP          },
    { EXIT_MENU,                 EXIT_MENU_HELP                 },
    { MINIMIZE_MENU,             MINIMIZE_MENU_HELP             },
    { SAVE_MENU,                 SAVE_MENU_HELP                 },
    { SAVE_AS_MASTER_MENU,       SAVE_AS_MASTER_MENU_HELP       },
    { PRINT_MENU,                PRINT_MENU_HELP                },
    { MULTIPLE_SHOT_PRINT_MENU,  MULTIPLE_SHOT_PRINT_MENU_HELP  },
    { POS_TIME_MENU,             POS_TIME_MENU_HELP             },
    { POS_ONLY_MENU,             POS_ONLY_MENU_HELP             },
    { TIME_ONLY_MENU,            TIME_ONLY_MENU_HELP            },
    { LAST_50_SCREEN_MENU,       LAST_50_SCREEN_MENU_HELP       },
    { STD_SCREEN_MENU,           STD_SCREEN_MENU_HELP           },
    { VIEW_REFERENCE_TRACE_MENU, VIEW_REFERENCE_TRACE_MENU_HELP },
    { VIEW_PARAMETERS_MENU,      VIEW_PARAMETERS_MENU_HELP      },
    { BOX_SCREEN_MENU,           BOX_SCREEN_MENU_HELP           },
    { TREND_SCREEN_MENU,         TREND_SCREEN_MENU_HELP         },
    { PROFILE_SCREEN_MENU,       PROFILE_SCREEN_MENU_HELP       },
    { DOWNBAR_SCREEN_MENU,       DOWNBAR_SCREEN_MENU_HELP       },
    { PART_EDITOR_MENU,          PART_EDITOR_MENU_HELP          },
    { MACHINE_EDITOR_MENU,       MACHINE_EDITOR_MENU_HELP       },
    { BOARD_MONITOR_MENU,        BOARD_MONITOR_MENU_HELP        },
    { MONITOR_CONFIG_MENU,       MONITOR_CONFIG_MENU_HELP       },
    { BACKUP_AND_RESTORE_MENU,   BACKUP_AND_RESTORE_MENU_HELP   },
    { SETUPS_MENU,               SETUPS_MENU_HELP               },
    { DOWNTIME_MENU,             DOWNTIME_MENU_HELP             },
    { EDITDOWN_MENU,             EDITDOWN_MENU_HELP             },
    { SURETRAK_CONTROL_MENU,     SURETRAK_CONTROL_MENU_HELP     },
    { OPERATOR_SIGNON_MENU,      OPERATOR_SIGNON_MENU_HELP      },
    { EXPORT_SHOTS_MENU,         EXPORT_SHOTS_MENU_HELP         },
    { ALARM_SUMMARY_MENU,        ALARM_SUMMARY_MENU_HELP        },
    { PASSWORDS_MENU,            PASSWORDS_MENU_HELP            },
    { RESTORE_PASSWORD_MENU,     RESTORE_PASSWORD_MENU_HELP     },
    { HELP_MENU,                 HELP_MENU_HELP                 },
    { UNZOOM_MENU,               UNZOOM_MENU_HELP               },
    { ZOOM_MENU,                 ZOOM_MENU_HELP                 },
    { DISPLAY_CONFIGURATION_MENU, DISPLAY_CONFIGURATION_MENU_HELP },
    { COLOR_DEFINITION_MENU,     COLOR_DEFINITION_MENU_HELP     },
    { CHOOSE_RT_MACHINES_MENU,   CHOOSE_RT_MACHINES_MENU_HELP   },
    { CORNER_PARAMETERS_MENU,    CORNER_PARAMETERS_MENU_HELP    },
    { OVERLAY_MENU,              OVERLAY_MENU_HELP              },
    { TREND_REALTIME_MENU,       TREND_REALTIME_MENU_HELP       },
    { TREND_ALLSHOTS_MENU,       TREND_ALLSHOTS_MENU_HELP       },
    { TREND_CONFIG_MENU,         TREND_CONFIG_MENU_HELP         },
    { STAT_SUMMARY_MENU,         STAT_SUMMARY_MENU_HELP         },
    { NO_STAT_SUMMARY_MENU,      NO_STAT_SUMMARY_MENU_HELP      },
    { VISIEDIT_MENU,             VISIEDIT_MENU_HELP             },
    { CLEAR_BACKUP_LIST_MENU,    CLEAR_BACKUP_LIST_MENU_HELP    },
    { ADD_TO_BACKUP_LIST_MENU,   ADD_TO_BACKUP_LIST_MENU_HELP   },
    { ALARM_LOG_SETUP_MENU,      ALARM_LOG_SETUP_MENU_HELP      },
    { STATUS_SCREEN_MENU,        STATUS_SCREEN_MENU_HELP        },
    { EDIT_STATUS_SCREEN_MENU,   EDIT_STATUS_SCREEN_MENU_HELP   },
    { SAVE_STATUS_SCREEN_MENU,   SAVE_STATUS_SCREEN_MENU_HELP   },
    { FONT_STATUS_SCREEN_MENU,   FONT_STATUS_SCREEN_MENU_HELP   },
    { EXPORT_ALARMS_MENU,        EXPORT_ALARMS_MENU_HELP        },
    { XDCR_MENU,                 XDCR_MENU_HELP                 },
    { STANDARD_DOWNTIMES_MENU,   STANDARD_DOWNTIMES_MENU_HELP   },
    { SHIFT_REPORT_MENU,         SHIFT_REPORT_MENU_HELP         },
    { WHATS_NEW_MENU,            WHATS_NEW_MENU_HELP            },
    { ABOUT_BOX_MENU,            ABOUT_BOX_MENU_HELP            }
    };

static int32 n = sizeof(helplist)/sizeof(MENU_HELP_TEXT_ENTRY);
int32 i;

if ( !MenuHelpWindow )
    return;

for ( i=0; i<n; i++ )
    {
    if ( id == helplist[i].id )
        {
        set_text( MenuHelpWindow, resource_string(helplist[i].resource_id) );
        return;
        }
    }

set_text( MenuHelpWindow, EmptyString );
}

/***********************************************************************
*                      START_ALARM_SUMMARY_DIALOG                      *
***********************************************************************/
void start_alarm_summary_dialog()
{
DialogIsActive = TRUE;
DialogBox(
    MainInstance,
    TEXT("ALARM_SUMMARY_DIALOG"),
    MainPlotWindow,
    (DLGPROC) alarm_summary_proc );
}

/***********************************************************************
*                        CHECK_ZOOM_BOX_LABELS                         *
* Check to see if either of the zoom boxes is visible. If so, update   *
* the labels and curve types.                                          *
***********************************************************************/
static void check_zoom_box_labels()
{
HWND w;
w = 0;

if ( IsWindowVisible(ZoomBoxWindow) )
    w = ZoomBoxWindow;
else if ( IsWindowVisible(SmallZoomBoxWindow) )
    w = SmallZoomBoxWindow;
if ( w )
    set_zoom_box_labels( w );
}

/***********************************************************************
*                         FORCE_DOWNTIME_POPUP                         *
***********************************************************************/
static void force_downtime_popup()
{
COMPUTER_CLASS c;

/*
----------------------------------------------------------
Do nothing if the downtime entry screen is already running
---------------------------------------------------------- */
if ( DowntimeEntryDialog )
    return;

if ( c.find(ComputerName) )
    {
    if ( c.is_this_computer() )
        {
        DowntimeEntryMachine = MachineName;
        ShowWindow( CreateDialog( MainInstance, TEXT("DOWNTIME_ENTRY_DIALOG"), MainWindow, (DLGPROC) downtime_entry_dialog_proc ), SW_SHOW );
        }
    }
}

/***********************************************************************
*                          DELETE_BACKUP_LIST                          *
***********************************************************************/
static void delete_backup_list()
{
NAME_CLASS s;

s.get_backup_list_csvname();
s.delete_file();
}

/***********************************************************************
*                           CHECK_FOR_HUMAN                            *
*                                                                      *
*  The original intent of this is to check for human input and         *
*  restore the default password level if there is no input in the      *
*  specified time. The code for this is commented out below. It        *
*  only works for XP. At the present time the default level is         *
*  restored after a fixed time. If the program is restarted, it        *
*  resets the timer.                                                   *
*                                                                      *
***********************************************************************/
static void check_for_human()
{
//LASTINPUTINFO info;
//DWORD         now;

if ( PasswordLevelTimeoutMs == 0 )
    {
    if ( PasswordLevelTimeoutId != 0 )
        {
        KillTimer( MainWindow, PasswordLevelTimeoutId );
        PasswordLevelTimeoutId = 0;
        return;
        }
    }

/*
-----------------------------------------------------
if ( IsWindowsNT )
    {
    info.cbSize = sizeof( LASTINPUTINFO );
    if ( GetLastInputInfo(&info) )
        {
        now = GetTickCount();
        if ( now > info.dwTime )
            now -= info.dwTime;
        else
            return;
        }
    }
else
    {
    now = PasswordLevelTimeoutMs + 1;
    }

if ( now > PasswordLevelTimeoutMs )
    {
----------------------------------------------------- */

KillTimer( MainWindow, PasswordLevelTimeoutId );
set_default_password_level( false );
PasswordLevelTimeoutId = 0;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
LRESULT CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
const WPARAM VK_P = 0x50;
const WPARAM VK_R = 0x52;
const WPARAM VK_U = 0x55;
const WPARAM VK_V = 0x56;
const WPARAM VK_X = 0x58;
const WPARAM VK_Z = 0x5A;
int id;
int cmd;

static BOOLEAN ShowingAllWindows = FALSE;
static BOOLEAN first_time        = TRUE;
static TEXT_LIST_CLASS t;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_TIMER:
        if ( wParam == PASSWORD_LEVEL_TIMEOUT_TIMER_NUMBER )
            {

            check_for_human();
            return 0;
            }
        break;

    case WM_KEYDOWN:
        if ( wParam == VK_R )
            {
            if ( (MyScreenType != BOX_SCREEN_TYPE) && (MyScreenType != STATUS_SCREEN_TYPE) )
                toggle_realtime();
            }
        else if ( wParam == VK_V )
            {
            do_view_parameters_menu();
            }
        else if ( wParam == VK_P )
            {
            toggle_pressure_display_type();
            SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
            }
        else if ( wParam == VK_X )
            {
            //if ( XKeyTogglesX1X4 )
            if ( GetKeyState(VK_CONTROL) < 0 )
                {
                if ( ShowX1PointsOnly )
                    ShowX1PointsOnly = FALSE;
                else
                    ShowX1PointsOnly = TRUE;
                SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
                }
            }
        else if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
            {
            if ( wParam == VK_Z && !Zooming )
                {
                do_keyboard_zoom_button();
                return 0;
                }
            else if ( wParam == VK_U && (Zooming || zoomed()) )
                {
                unzoom();
                SetFocus( MainWindow );
                return 0;
                }

            if ( do_zoom_arrow_keys((int) wParam) )
                return 0;
            }
        break;

    case WM_CREATE:
        MainMenu.create_windows( MainMenuWindowName, hWnd );
        PopupMenu.create_windows( PopupMenuWindowName, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_MENUSELECT:
        show_menu_help_text( (UINT) id );
        return 0;

    case WM_ENTERMENULOOP:
        create_menu_help_box();
        return 0;

    case WM_EXITMENULOOP:
        if ( MenuHelpWindow )
            {
            DestroyWindow( MenuHelpWindow );
            MenuHelpWindow = 0;
            }
        return 0;

    case WM_HELP:
        if ( MyScreenType == TREND_SCREEN_TYPE )
            {
            if ( GridIsVisible )
                gethelp( HELP_CONTEXT, STAT_SUMMARY_HELP );
            else
                gethelp( HELP_CONTEXT, TREND_CHART_HELP );
            }
        else if ( MyScreenType == STATUS_SCREEN_TYPE )
            {
            gethelp( HELP_CONTEXT, STATUS_SCREEN_HELP );
            }
        else
            {
            gethelp( HELP_CONTEXT, PROFILE_MAIN_SCREEN_HELP );
            }

        return 0;

//    case WM_ACTIVATEAPP:
//            if ( !DialogIsActive && BOOL(wParam) && IsWindowVisible(MainPlotWindow) )
//                SetFocus( MainPlotWindow );
//            break;

//    case WM_ACTIVATE:
//        if ( id == WA_INACTIVE && cmd != 0 )
//            {
            /*
            ----------------------------------------
            Cmd is zero if the menu is not minimized
            ---------------------------------------- */
//            if ( !ShowingAllWindows && IsWindowVisible(MainPlotWindow) )
//                {
//                hide_all_windows();
//                }
//            return 0;
//            }
//        else if ( id != WA_INACTIVE )
//            {
//            if ( !IsWindowVisible(MainPlotWindow)  )
//                {
//                ShowingAllWindows = TRUE;
//                show_all_windows();
//                ShowingAllWindows = FALSE;
//                }
//            return 0;
//            }
//        break;

    case WM_DBINIT:
        /*
        -------------------------------------------------------------
        If I ask for too many dde conects without getting my messages
        things come apart, so I spread them out as per below.
        ------------------------------------------------------------- */
        if ( first_time )
            {
            first_time = FALSE;

            if ( redisplay_last_shot() )
                {
                SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
                }
            else
                {
                create_chospart_window();
                }

            load_main_menu();
            position_main_plot_window();
            client_dde_startup( hWnd );
            register_for_event( DDE_CONFIG_TOPIC,  MACH_SETUP_INDEX,     new_machine_setup_callback );
            register_for_event( DDE_MONITOR_TOPIC, NEW_MACH_STATE_INDEX, new_machine_state_callback );
            register_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX,      down_data_callback );
            register_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX,  monitor_setup_callback );
            register_for_event( DDE_CONFIG_TOPIC,  PW_LEVEL_INDEX,       password_level_callback );

            MachList.rewind();
            while ( MachList.next() )
                t.append( MachList.name().text() );
            t.rewind();
            PostMessage( hWnd, WM_DBINIT, 0, 0L );
            }
        else
            {
            if ( t.next() )
                {
                register_for_event( t.text(), SHOT_NUMBER_INDEX, new_shot_callback );
                register_for_event( t.text(), PART_SETUP_INDEX,  new_part_setup_callback );
                PostMessage( hWnd, WM_DBINIT, 0, 0L );
                }
            else
                {
                t.empty();
                get_current_password_level();
                SetFocus( MainMenu.mw.handle() );
                }
            }
        return 0;

    case WM_EV_SHUTDOWN:
        PostMessage( MainPlotWindow, WM_CLOSE, 0, 0L );
        PostMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        save_last_shot_name();
        PostQuitMessage( 0 );
        return 0;

    case WM_NEWPART:
        HaveHeader = FALSE;
        if ( OverlayMenuState )
            NofOverlayShots = 0;
        load_part_info();
        reset_plot_curves();

        if ( MyScreenType == TREND_SCREEN_TYPE )
            {
            get_current_shot();
            trend_screen_reset();
            }
        else if ( MyScreenType == DOWNBAR_SCREEN_TYPE )
            {
            get_current_shot();
            do_new_dtr_config();
            }
        else
            {
            create_chosshot_window();
            }
        return 0;

    case WM_NEW_TC_CONFIG:
        do_new_tc_config( wParam );
        return 0;

    case WM_NEW_CL_CONFIG:
        do_new_control_limits();
        return 0;

    case WM_NEW_DTR_CONFIG:
        do_new_dtr_config();
        return 0;

    case WM_NEWSHOT:
        do_newshot();
        return 0;

    case WM_NEWSETUP:
        do_newsetup();
        return 0;

    case WM_NEW_DATA:
        if ( wParam == LOAD_MAIN_MENU_REQ )
            {
            load_main_menu();
            }
        else if ( wParam == SCREEN_REFRESH_REQ )
            {
            InvalidateRect( MainPlotWindow, 0, TRUE );
            }
        else if ( wParam == PASSWORD_DIALOG_REQ )
            {
            DialogIsActive = TRUE;
            DialogBox(
                MainInstance,
                TEXT("PASSWORD_DIALOG"),
                hWnd,
                (DLGPROC) enter_password_proc );
            }
        else if ( wParam == EDIT_PASSWORDS_DIALOG_REQ )
            {
            DialogIsActive = TRUE;
            DialogBox(
                MainInstance,
                TEXT("EDIT_PASSWORDS_DIALOG"),
                hWnd,
                (DLGPROC) edit_passwords_proc );
            }
        else if ( wParam == CO_OPTIONS_DIALOG_REQ )
            {
            DialogIsActive = TRUE;
            DialogBox(
                MainInstance,
                TEXT("CO_OPTIONS_DIALOG"),
                hWnd,
                (DLGPROC) co_options_dialog_proc );
            }
        return 0;

    case WM_KEYUP:
        if ( wParam == VK_SNAPSHOT && SnapshotIsActive )
            {
            if ( MyScreenType == ZABISH_SCREEN_TYPE || MyScreenType == STANDARD_SCREEN_TYPE )
                {
                QuickPrint = TRUE;
                print_me();
                /*
                -------------------------------------
                Empty this picture from the clipboard
                ------------------------------------- */
                if ( OpenClipboard(hWnd) )
                    {
                    EmptyClipboard();
                    CloseClipboard();
                    }
                return 0;
                }
            }
        break;

    case WM_COMMAND:
    case WM_VTW_MENU_CHOICE:
        if ( id >= LOW_RUN_EXTERNAL_PROGRAM_ID && id <= HIGH_RUN_EXTERNAL_PROGRAM_ID )
            {
            run_external_program( id );
            return 0;
            }

        switch ( id )
            {
            case HELP_MENU:
                gethelp( HELP_CONTEXT, TABLE_OF_CONTENTS_HELP );
                return 0;

            case CHOOSE_LANGUAGE_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("CHOOSE_LANGUAGE_DIALOG"),
                    MainWindow,
                    (DLGPROC) choose_language_dialog_proc );
                return 0;

            case EXIT_MENU:
                /*
                -----------------------------------------------------------------------
                Tell eventman to shut down. This will shut down everyone, including me.
                ----------------------------------------------------------------------- */
                poke_data( DDE_EXIT_TOPIC,  ItemList[NEW_MACH_STATE_INDEX].name, UNKNOWN );
                return 0;

            case MINIMIZE_MENU:
                poke_data( DDE_ACTION_TOPIC, ItemList[PLOT_MIN_INDEX].name, TEXT("0") );
                CloseWindow( MainWindow );
                return 0;

            case SAVE_MENU:
                toggle_profile_save_flags();
                InvalidateRect( ParamHistoryWindow, 0, TRUE );
                return 0;

            case SAVE_AS_MASTER_MENU:
                save_as_master_trace();
                return 0;

            case POS_TIME_MENU:
                ShowX1PointsOnly = TRUE;
                set_current_display( POS_TIME_DISPLAY );
                SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
                //InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case POS_ONLY_MENU:
                ShowX1PointsOnly = TRUE;
                set_current_display( POS_ONLY_DISPLAY );
                SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
                //InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case TIME_ONLY_MENU:
                if ( zoomed() || Zooming )
                    {
                    turn_off_zooming();
                    unzoom( FALSE );
                    }
                ShowX1PointsOnly = FALSE;
                set_current_display( TIME_ONLY_DISPLAY );
                SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
                //InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case LAST_50_SCREEN_MENU:
                update_screen_type( ZABISH_SCREEN_TYPE );
                return 0;

            case STD_SCREEN_MENU:
                update_screen_type( STANDARD_SCREEN_TYPE );
                return 0;

            case PROFILE_SCREEN_MENU:
                update_screen_type( PROFILE_SCREEN_TYPE );
                return 0;

            case BOX_SCREEN_MENU:
                if ( MyScreenType != BOX_SCREEN_TYPE )
                    {
                    update_screen_type( BOX_SCREEN_TYPE );
                    }
                return 0;

            case STATUS_SCREEN_MENU:
                update_screen_type( STATUS_SCREEN_TYPE );
                return 0;

            case DAY_PARAM_SCREEN_MENU:
                update_screen_type( DAY_PARAM_SCREEN_TYPE );
                return 0;

            case EDIT_STATUS_SCREEN_MENU:
                Editing_Status_Screen = TRUE;
                return 0;

            case SAVE_STATUS_SCREEN_MENU:
                save_status_config();
                return 0;

            case FONT_STATUS_SCREEN_MENU:
                choose_status_screen_font();
                return 0;

            case TREND_CONFIG_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("TREND_CONFIG_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) trend_config_dialog_proc );
                return 0;

            case DOWNBAR_CONFIG_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("DOWNTIME_REPORT_SETUP_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) downtime_report_setup_proc );
                return 0;

            case CHOOSE_DOWNTIME_MACHINE_MENU:
                if ( ChooseDowntimeMachineWindow )
                    SendMessage( ChooseDowntimeMachineWindow, WM_DBINIT, 0, 0L );
                return 0;

            case TREND_REALTIME_MENU:
                trend_realtime();
                return 0;

            case DOWNBAR_SCREEN_MENU:
                DowntimeComputerName = ComputerName;
                DowntimeMachineName  = MachineName;
                update_screen_type( DOWNBAR_SCREEN_TYPE );
                return 0;

            case TREND_SCREEN_MENU:
                update_screen_type( TREND_SCREEN_TYPE );
                return 0;

            case PRINT_MENU:
                turn_off_zooming();
                print_me();
                return 0;

            case MULTIPLE_SHOT_PRINT_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("PRINT_MULTIPLE_SHOTS_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) print_multiple_shots_proc );
                return 0;

            case UNZOOM_MENU:
                unzoom();
                SetFocus( MainWindow );
                return 0;

            case ZOOM_MENU:
                if ( MyScreenType == BOX_SCREEN_TYPE || MyScreenType == STATUS_SCREEN_TYPE )
                    return 0;
                if ( !Zooming )
                    do_keyboard_zoom_button();
                SetFocus( MainWindow );
                return 0;

            case XDCR_MENU:
                set_info_box_type( XDCR_INFO_BOX );
                return 0;

            case REAL_TIME_MENU:
            case REAL_TIME_MAIN_MENU:
                toggle_realtime();
                return 0;

            case OVERLAY_MENU:
                do_overlay_toggle( id );
                return 0;

            case VIEW_REFERENCE_TRACE_MENU:
                turn_off_zooming();
                toggle_menu_view_state( ViewReferenceMenuState, id );
                InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case VIEW_PARAMETERS_MENU:
                do_view_parameters_menu();
                return 0;

            case COLOR_DEFINITION_MENU:
                ShowWindow( ColorDefinitionDialogWindow, SW_SHOW );
                SetFocus( ColorDefinitionDialogWindow );
                return 0;

            case CHOOSE_PART_MENU:
                turn_off_zooming();
                if ( MyScreenType == BOX_SCREEN_TYPE )
                    update_screen_type( STANDARD_SCREEN_TYPE );
                create_chospart_window();
                return 0;

            case CHOOSE_SHOT_MENU:
                turn_off_zooming();
                create_chosshot_window();
                return 0;

            case CHOOSE_RT_MACHINES_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("REALTIME_MACHINE_LIST"),
                    MainWindow,
                   (DLGPROC) realtime_machine_list_proc );
                return 0;

            case CORNER_PARAMETERS_MENU:
                choose_corner_parameters();
                return 0;

            case DISPLAY_CONFIGURATION_MENU:
                turn_off_zooming();
                DialogIsActive = TRUE;

                DialogBox(
                    MainInstance,
                    TEXT("DISPLAY_CONFIGURATION"),
                    MainWindow,
                   (DLGPROC) display_configuration_dialog_proc );

                check_zoom_box_labels();
                init_global_curve_types();
                InvalidateRect( MainPlotWindow, 0, TRUE );
                return 0;

            case CLEAR_BACKUP_LIST_MENU:
                delete_backup_list();
                return 0;

            case ADD_TO_BACKUP_LIST_MENU:
                add_to_backup_list();
                return 0;

            case WHATS_NEW_MENU:
                gethelp( HELP_CONTEXT, WHATS_NEW_HELP );
                return 0;

            case FT2_HELP_MENU:
                get_ft2_help();
                return 0;

            case MACHINE_LIST_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("MACHINE_LIST_DIALOG"),
                    MainWindow,
                    (DLGPROC) machine_list_dialog_proc );
                return 0;

            case ABOUT_BOX_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("ABOUT_DIALOG"),
                    MainWindow,
                    (DLGPROC) about_dialog_proc );
                return 0;

            case BOX_SCREEN_SET_FONT_MENU:
                choose_box_font();
                return 0;

            case TREND_ALLSHOTS_MENU:
                trend_allshots();
                return 0;

            case STAT_SUMMARY_MENU:
            case NO_STAT_SUMMARY_MENU:
                toggle_current_display_type();
                return 0;

            case PART_EDITOR_MENU:
            case MACHINE_EDITOR_MENU:
            case BOARD_MONITOR_MENU:
            case MONITOR_CONFIG_MENU:
            case SENSORS_MENU:
            case BACKUP_AND_RESTORE_MENU:
            case SETUPS_MENU:
            case DOWNTIME_MENU:
            case EDITDOWN_MENU:
            case SURETRAK_CONTROL_MENU:
            case VISIEDIT_MENU:
            case STANDARD_DOWNTIMES_MENU:
            case SHIFT_REPORT_MENU:
            case CONVERT_UNITS_MENU:
                turn_off_zooming();
                run_tool( id );
                return 0;

            case OPERATOR_SIGNON_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("OPERATOR_SIGNON_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) operator_signon_proc );
                return 0;

            case EXPORT_SHOTS_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("EXPORT_SHOTS_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) export_shots_proc );
                return 0;

            case ALARM_SUMMARY_MENU:
                start_alarm_summary_dialog();
                return 0;

            case ALARM_LOG_SETUP_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("ALARM_LOG_SETUP_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) alarm_log_setup_proc );
                return 0;

            case DOWNTIME_POPUP_MENU:
                force_downtime_popup();
                return 0;

            case SET_TSL_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                    MainInstance,
                    TEXT("SET_TSL_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) set_tsl_dialog_proc );
                return 0;

            case PASSWORDS_MENU:
                PostMessage( hWnd, WM_NEW_DATA, PASSWORD_DIALOG_REQ, 0L );
                return 0;

            case RESTORE_FOCUS_TO_HMI_MENU:
                restore_focus_to_hmi();
                return 0;

            case RESTORE_PASSWORD_MENU:
                set_default_password_level( true );
                return 0;

            case EDIT_SHIFTS_MENU:
                DialogIsActive = TRUE;
                DialogBox(
                   MainInstance,
                   TEXT("EDIT_SHIFTS_DIALOG"),
                   NULL,
                   (DLGPROC) edit_shifts_dialog_proc );
                return 0;

            case EXPORT_ALARMS_MENU:
                if ( !DialogIsActive && !ExportAlarmsDialogWindow )
                    {
                    DialogIsActive = TRUE;
                    DialogBox(
                        MainInstance,
                        TEXT("EXPORT_ALARMS_DIALOG"),
                        MainPlotWindow,
                        (DLGPROC) export_alarms_dialog_proc );
                    }
                return 0;

            case CSV_SHOT_FILE_MENU:
                if ( !DialogIsActive )
                    {
                    DialogIsActive = TRUE;
                    DialogBox(
                        MainInstance,
                        TEXT("CSV_SHOT_FILE_DIALOG"),
                        MainPlotWindow,
                        (DLGPROC) csv_shot_file_dialog_proc );
                    }
                return 0;

            }
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                          GET_HISTORY_LENGTH                          *
***********************************************************************/
static void get_history_length()
{
TCHAR  * cp;
short slen;

cp = get_ini_string( VisiTrakIniFile, ConfigSection, TEXT("LastN") );
slen = lstrlen( cp );
if ( slen > 0 )
    HistoryShotCount = extshort( cp, slen );
}

/***********************************************************************
*                           SET_FULL_SCREEN                            *
***********************************************************************/
void set_full_screen()
{
APPBARDATA ab;
UINT       taskbar_state;

IsFullScreen = FALSE;

ab.cbSize = sizeof( APPBARDATA );
ab.hWnd   = 0;
taskbar_state = SHAppBarMessage( ABM_GETSTATE, &ab );

if ( taskbar_state == ABS_ALWAYSONTOP )
    return;

if ( IsWindows7OrGreater() )
    {
    if ( taskbar_state != ABS_AUTOHIDE )
        return;
    }

if ( *get_ini_string(DisplayIniFile, ConfigSection, TEXT("HideMenu")) == TEXT('Y') )
    IsFullScreen = TRUE;
}

/***********************************************************************
*                           MAIN_MENU_WIDTH                            *
*                                                                      *
***********************************************************************/
static int main_menu_width()
{
RECT r;

if ( SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0) )
    return r.right - r.left;

/*desktop_window = GetDesktopWindow();
if ( desktop_window )
    {
    GetClientRect( desktop_window, &r );
    return r.right - r.left;
    }
*/

return 160;
}

/***********************************************************************
*                          SAVE_SCREEN_TYPES                           *
***********************************************************************/
static void save_screen_types()
{
STRING_CLASS s;

s = (int) StandardProfileScreenType;
put_ini_string( DisplayIniFile, ConfigSection, TEXT("LastStdProfileType"), s.text() );

/*
------------------------------------------------------------------------------------
If this is the Critical Parameters screen, start up on the standard screen next time
------------------------------------------------------------------------------------ */
if ( MyScreenType != DAY_PARAM_SCREEN_TYPE )
    s = (int) MyScreenType;

put_ini_string( DisplayIniFile, ConfigSection, TEXT("LastScreenType"), s.text() );
}

/***********************************************************************
*                       CHECK_FOR_MONITOR_PROGRAM                      *
***********************************************************************/
static void check_for_monitor_program()
{
NAME_CLASS s;

if ( s.get_exe_dir_file_name( MonallProgramName) )
    HaveMonitorProgram = s.file_exists();
}

/***********************************************************************
*                    SET_LOAD_FROM_NETWORK_SETUP_STRINGS              *
***********************************************************************/
static void set_load_from_network_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { LOADING_MA_STATIC, TEXT("LOADING_MA") },
    { COMPUTER_STATIC,   TEXT("CO_PROMPT") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    NETWORK_COMPUTER_TBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }
}

/***********************************************************************
*                      LOAD_FROM_NETWORK_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK load_from_network_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch (msg)
    {
    case WM_INITDIALOG:
        set_load_from_network_setup_strings( hWnd );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        SET_CURRENT_DIRECTORY                         *
***********************************************************************/
void set_current_directory()
{
TCHAR path[MAX_PATH+1];

if ( get_exe_directory(path) )
    {
    remove_backslash_from_path( path );
    SetCurrentDirectory( path );
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
    CurrentLanguage = ini.get_string();
    MainMenu.language = CurrentLanguage;
    StringTable.set_language( CurrentLanguage );
    }

s.get_stringtable_name( ProfileResourceFile );
StringTable.read( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( int cmd_show )
{
COMPUTER_CLASS c;
INI_CLASS      ini;
NAME_CLASS     s;
WNDCLASS       wc;
int h;
int i;
int w;
int y;

BOOL b;
RECTANGLE_CLASS r;
TCHAR * cp;
static DWORD status;

RealTime               = TRUE;
HaveHeader             = FALSE;
OverlayMenuState       = FALSE;
ViewReferenceMenuState = FALSE;
NofOverlayShots        = 0;

Parms.set_array_size( MAX_PARMS );
MENU_CONTROL_ENTRY::register_window();
POPUP_CONTROL_ENTRY::register_window();

LoadFromNetworkDialogWindow = 0;
c.startup();
load_resources();
statics_startup();
if ( c.count() > 1 )
    LoadFromNetworkDialogWindow = CreateDialog( MainInstance, TEXT("LOAD_FROM_NETWORK_DIALOG"), 0, (DLGPROC) load_from_network_dialog_proc );
set_current_directory();
load_machlist();
load_realtime_machine_list();
get_history_length();
read_corner_parameters( CornerParameters, NofCornerParameters );
ComputerName = c.whoami();
units_startup();
dcurve_startup();
shifts_startup();
ANALOG_SENSOR::get();
//init_global_curve_types();
check_for_monitor_program();

cp = get_ini_string( DisplayIniFile, ConfigSection, MainFontNameKey );
if ( !unknown(cp) )
    {
    ArialFontName      = cp;
    ArialBoldFontName  = cp;
    TimesRomanFontName = cp;
    }
else
    {
    ArialFontName      = DefaultArialFontName;
    ArialBoldFontName  = DefaultArialBoldFontName;
    TimesRomanFontName = DefaultTimesRomanFontName;
    }

/*
----------------------------------------------
Load the downtime categories and subcategories
---------------------------------------------- */
Cats.read();
SubCats.read();

HaveSureTrakControl = boolean_from_ini( VisiTrakIniFile, ConfigSection, HaveSureTrakKey     );
UseDowntimeEntry    = boolean_from_ini( VisiTrakIniFile, ConfigSection, UseDowntimeEntryKey );
PartBasedDisplay    = boolean_from_ini( VisiTrakIniFile, ConfigSection, PartBasedDisplayKey );

s.get_local_ini_file_name( DisplayIniFile );
if ( s.file_exists() )
    {
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    if ( ini.find(ShowX1PointsOnlyKey) )
        ShowX1PointsOnly = ini.get_boolean();
    if ( ini.find(XKeyTogglesX1X4Key) )
        XKeyTogglesX1X4 = ini.get_boolean();
    }

/*
----------------------------
Get the standard screen type
---------------------------- */
cp = get_ini_string( DisplayIniFile, ConfigSection, TEXT("LastStdProfileType") );
if ( !unknown(cp) )
    StandardProfileScreenType = (short) asctoint32( cp );
/*
---------------------------
Get the width of the curves
--------------------------- */
cp = get_ini_string( DisplayIniFile, ConfigSection, CurvePenWidthKey );
if ( !unknown(cp) )
    CurvePenWidth = (int) asctoint32( cp );

/*
------------------------------------------------------------------------------
If the snapshot key is active then pressing printscreen prints to the printer.
The default is yes.
------------------------------------------------------------------------------ */
if ( !boolean_from_ini(DisplayIniFile, ConfigSection, SnapshotKey) )
    SnapshotIsActive = FALSE;

ShowWorker = boolean_from_ini( DisplayIniFile, ConfigSection, ShowWorkerKey );

if ( LoadFromNetworkDialogWindow )
    DestroyWindow( LoadFromNetworkDialogWindow );

LoadFromNetworkDialogWindow = 0;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("PROFILE")  );
wc.lpszMenuName  = 0;
wc.hbrBackground = 0;
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

wc.lpszClassName = MainPlotWindowClass;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_plot_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = 0;
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

if ( !RegisterClass(&wc) )
    status = GetLastError();

wc.lpszClassName = XdcrWindowClass;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) xdcr_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = 0;
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

if ( !RegisterClass(&wc) )
    status = GetLastError();

/*
--------------------------------------------
Regisger the corner parameter display window
-------------------------------------------- */
wc.lpszClassName = ParameterWindowClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) parameter_window_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

w = main_menu_width();

/*
---------------------------------------------------------------
If this is a touchscreen display I need to make the menu bigger
--------------------------------------------------------------- */
h = GetSystemMetrics( SM_CYMENU );
y = 0;

if ( boolean_from_ini( DisplayIniFile, ConfigSection, HaveTouchScreenKey) )
    {
    y = 0;
    }

MainWindow = CreateWindowEx(
    WS_EX_APPWINDOW,
    MyClassName,
    MyWindowTitle, WS_POPUP, 0, y,                       /* X,y */ w, h,                       /* W,h */ NULL, NULL,
    MainInstance, NULL );

GetWindowRect( MainWindow, &MainWindowRect );

/*
------------------------------------
Get the bounds of the desktop window
------------------------------------ */
b = SystemParametersInfo( SPI_GETWORKAREA, 0, &r, 0 );
if ( b == 0 )
    {
    r = MainWindowRect;
    }
w = r.width();
h = r.height();

set_full_screen();

/*
---------------------------------------------------
Create a popup window that covers the entire screen
CreateWindow(
--------------------------------------------------- */
MainPlotWindow = CreateWindowEx(
    WS_EX_TOOLWINDOW,
    MainPlotWindowClass,               // window class name
    0,                                 // window title
    WS_POPUP,
    0, 0,
    w, h,                              // W,H
    MainWindow,
    NULL,
    MainInstance,
    NULL
    );

/*
------------------------------------------
Create the Corner Parameter Display Window
------------------------------------------ */
ParameterWindow = CreateWindowEx(
    WS_EX_TOOLWINDOW,
    ParameterWindowClassName,           // window class name
    0,                                  // window title
    WS_POPUP,
    0, 0,
    w, 200,                              // W,H
    MainPlotWindow, //NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateWindowEx(
    WS_EX_TOOLWINDOW,
    XdcrWindowClass,                   // window class name
    0,                                 // window title
    WS_POPUP,
    0, 0,
    w, h,                              // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainPlotWindow )
    status = GetLastError();

for ( i=0; i<NOF_REGIONS; i++ )
    {
    Region[i].is_visible = TRUE;
    }

clear_axes();
restore_colors();

set_current_display( POS_TIME_DISPLAY );

PrinterMargin.left   = 0;
PrinterMargin.right  = 500;    /* mils */
PrinterMargin.top    = 0;
PrinterMargin.bottom = 500;

InfoBoxType = PARAMETER_INFO_BOX;

ChooseDowntimeMachineWindow = CreateDialog(
    MainInstance,
    TEXT("CHOOSE_DOWNTIME_MACHINE_DIALOG"),
    MainPlotWindow,
    (DLGPROC) choose_downtime_machine_dialog_proc );
ShowWindow( ChooseDowntimeMachineWindow, SW_HIDE );

ZoomBoxWindow  = CreateDialog(
    MainInstance,
    TEXT("ZOOM_BOX_DIALOG"),
    MainPlotWindow,
    (DLGPROC) ZoomBoxProc );

SmallZoomBoxWindow  = CreateDialog(
    MainInstance,
    TEXT("SMALL_ZOOM_BOX_DIALOG"),
    MainPlotWindow,
    (DLGPROC) SmallZoomBoxProc );

ParamHistoryWindow  = CreateDialog(
    MainInstance,
    TEXT( "PARAM_HISTORY_DIALOG" ),
    MainPlotWindow,
    (DLGPROC) ParamHistoryProc );

ViewParametersDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("VIEW_PARAMETERS_DIALOG"),
    MainWindow,
    (DLGPROC) view_parameters_dialog_proc );

ColorDefinitionDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("COLOR_DEFINITION_DIALOG"),
    MainWindow,
    (DLGPROC) color_definition_dialog_proc );

ShowWindow( ChospartWindow, SW_SHOW );
ShowWindow( ViewParametersDialogWindow, SW_HIDE );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );

if ( IsIconic(MainWindow) )
    cmd_show = SW_MINIMIZE;

position_param_history_window();
position_main_plot_window();
position_zoom_box_window();
position_view_parameters_window();

if ( cmd_show == SW_MINIMIZE )
    CloseWindow( MainWindow );
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown()
{
if ( MyFont )
    {
    DeleteObject( MyFont );
    MyFont = NULL;
    }

if ( PrinterFont )
    {
    DeleteObject( PrinterFont );
    PrinterFont = NULL;
    }

box_display_shutdown();

unregister_for_event( DDE_CONFIG_TOPIC,  MACH_SETUP_INDEX );
unregister_for_event( DDE_MONITOR_TOPIC, NEW_MACH_STATE_INDEX );
unregister_for_event( DDE_MONITOR_TOPIC, DOWN_DATA_INDEX );
unregister_for_event( DDE_MONITOR_TOPIC, MONITOR_SETUP_INDEX );
unregister_for_event( DDE_CONFIG_TOPIC,  PW_LEVEL_INDEX      );

unregister_for_new_shot_events();
client_dde_shutdown();

Cats.cleanup();
SubCats.cleanup();

save_screen_types();
dcurve_shutdown();
shifts_shutdown();
units_shutdown();
ANALOG_SENSOR::shutdown();
save_colors();
statics_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL  status;

if ( is_previous_instance(MyClassName, 0) )
    return 0;

/*--------------------------------------------
#ifdef _DEBUG
int     flags;

flags  = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG);
flags |= _CRTDBG_CHECK_ALWAYS_DF;
flags |= _CRTDBG_CHECK_CRT_DF;
flags |= _CRTDBG_LEAK_CHECK_DF;
flags |= _CRTDBG_DELAY_FREE_MEM_DF;
_CrtSetDbgFlag( flags );

#endif
---------------------------------------------- */

MainInstance = this_instance;

init( cmd_show );

AccelHandle = LoadAccelerators( this_instance, AccelName );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status && ChospartWindow )
        status = IsDialogMessage( ChospartWindow, &msg );

    if ( !status && ChosshotWindow )
        status = IsDialogMessage( ChosshotWindow, &msg );

    if ( !status && ViewParametersDialogWindow )
        status = IsDialogMessage( ViewParametersDialogWindow, &msg );

    if ( !status && ParamHistoryWindow )
        status = IsDialogMessage( ParamHistoryWindow, &msg );

    if ( !status && ColorDefinitionDialogWindow )
        status = IsDialogMessage( ColorDefinitionDialogWindow, &msg );

    if ( !status && DowntimeEntryDialog )
        status = IsDialogMessage( DowntimeEntryDialog, &msg );

    if ( !status && ChooseDowntimeMachineWindow )
        status = IsDialogMessage( ChooseDowntimeMachineWindow, &msg );

    if ( !status && MainPlotWindow )
        status = TranslateAccelerator( MainPlotWindow, AccelHandle, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
