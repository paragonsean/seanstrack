#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\asensor.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\genlist.h"
#include "..\include\computer.h"
#include "..\include\limit_switch_class.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\subs.h"
#include "..\include\ftanalog.h"
#include "..\include\ftcalc.h"
#include "..\include\listbox.h"
#include "..\include\static_text_parameter.h"
#include "..\include\stringtable.h"
#include "..\include\units.h"
#include "..\include\wclass.h"

#include "extern.h"
#include "resource.h"

#define CANCEL_BUTTON     2
#define OK_BUTTON         1

#define NO_TAB                       -1
#define INTERNAL_PARAM_TAB            0
#define FT_ANALOG_TAB                 1
#define FT_ONOFF_TAB                  2
#define FT_TARGET_TAB                 3
#define FT_DIGITAL_TAB                4
#define FT_CALC_TAB                   5
#define FT_EXTENDED_ANALOG_TAB        6
#define FT_PEAK_FS_VELOCITY_TAB       7
#define FT_EXTERNAL_PARAMETER_TAB     8
#define FT_STATIC_TEXT_PARAMETER_TAB  9
#define TAB_COUNT                    10

#define INTERNAL_PARAM_RADIO_BUTTON   111
#define FT_ANALOG_RADIO_BUTTON        112
#define FT_ONOFF_RADIO_BUTTON         113

#define PARAMETER_LISTBOX_ID 1108

#define MAX_TAB_LEN   25

/*
-------------------
Globals for FT_CALC
------------------- */
LISTBOX_CLASS UnitsCbox;
LISTBOX_CLASS VartypeCbox;

/*
---------------------------------
Globals for FT_EXTERNAL_PARAMETER
--------------------------------- */
LISTBOX_CLASS FtEpUnitsCbox;
LISTBOX_CLASS FtEpVartypeCbox;

/*
---------------------------------
Global for STATIC_TEXT_PARAMETERS
--------------------------------- */
static bool LoadingStaticText = false;

/*
--------------
Variable Types
-------------- */
struct VARTYPE_ENTRY
    {
    BITSETYPE    type;
    STRING_CLASS name;
    };

class VARTYPE_CLASS
    {
    public:
    VARTYPE_ENTRY * vartypes;
    int             n;
    int             x;
    bool            rewound;
    VARTYPE_CLASS();
    ~VARTYPE_CLASS();
    void      cleanup();
    int       count() { return n; }
    int       get();
    bool      next();
    void      rewind() { x = 0; rewound = true; }
    BITSETYPE type();
    BITSETYPE type( STRING_CLASS & name_to_find );
    STRING_CLASS & name();
    STRING_CLASS & name( BITSETYPE type_to_find );
    };

VARTYPE_CLASS Vartypes;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * s;
    };

struct MY_WINDOW_INFO
        {
        DLGPROC procptr;
        HWND    dialogwindow;
        };

static TCHAR * ParamDialogName[TAB_COUNT] = {
              TEXT("INTERNAL_PARAMETERS"),
              TEXT("FT_ANALOG"),
              TEXT("FT_ONOFF"),
              TEXT("FT_TARGET"),
              TEXT("FT_DIGITAL"),
              TEXT("FT_CALC"),
              TEXT("FT_EXTENDED_ANALOG"),
              TEXT("FT_PEAK_FS_VELOCITY"),
              TEXT("FT_EXTERNAL_PARAMETER"),
              TEXT("FT_STATIC_TEXT_PARAMETER")
              };

static MY_WINDOW_INFO WindowInfo[TAB_COUNT];
static int   CurrentDialogNumber = NO_TAB;
static int   CurrentParameterIndex = 0;
static int   DIFFERENTIAL_PRESSURE_BUTTON_INDEX = DIF_CHANNEL_BUTTON - CHANNEL_1_BUTTON;
static int   TIME_POSITION_BUTTON_INDEX         = TIME_OR_POS_BUTTON - CHANNEL_1_BUTTON;
static int   VELOCITY_BUTTON_INDEX              = VELOCITY_BUTTON - CHANNEL_1_BUTTON;

BOOLEAN LoadingParams = FALSE;
BOOLEAN LoadingFtAnalogs = FALSE;
BOOLEAN LoadingFtExtendedAnalogs = FALSE;
BOOLEAN StartValueIsNegative     = FALSE;

TCHAR * ascii_float( float sorc );
TCHAR * resource_string( UINT resource_id );
float ebox_float( HWND w, int editbox_id );
void return_from_parameter_edit();
void refresh_current_parameter_limits();
void get_new_ft_onoff_settings();
void update_statics( HWND hWnd, RESOURCE_TEXT_ENTRY * rlist, int n );
void set_statics_font( HWND hWnd, UINT id );
void set_statics_font( WINDOW_CLASS & w );

/***********************************************************************
*                              VARTYPE_CLASS                           *
***********************************************************************/
VARTYPE_CLASS::VARTYPE_CLASS()
{
vartypes = 0;
n = 0;
rewound=true;
x = 0;
}

/***********************************************************************
*                             ~VARTYPE_CLASS                           *
***********************************************************************/
VARTYPE_CLASS::~VARTYPE_CLASS()
{
cleanup();
}

/***********************************************************************
*                                  get                                 *
*               Return the number of variable types read.              *
***********************************************************************/
int VARTYPE_CLASS::get()
{
NAME_CLASS s;
DB_TABLE   t;
int        nof_recs;

cleanup();
s.get_data_dir_file_name( VARTYPE_DB );
if ( t.open(s, VARTYPE_RECLEN, PFL) )
    {
    nof_recs = (int) t.nof_recs();
    if ( nof_recs > 0 )
        {
        n = 0;
        vartypes = new VARTYPE_ENTRY[nof_recs];
        if ( vartypes )
            {
            while ( t.get_next_record(NO_LOCK) && n < nof_recs )
                {
                vartypes[n].type = t.get_bitsetype( VARTYPE_VALUE_OFFSET );
                t.get_alpha( vartypes[n].name, VARTYPE_NAME_OFFSET, VARTYPE_NAME_LEN );
                n++;
                }
            }
        }
    t.close();
    }

return n;
}

/***********************************************************************
*                               cleanup                                *
***********************************************************************/
void VARTYPE_CLASS::cleanup()
{
int i;
int ncopy;
VARTYPE_ENTRY * v;

if ( !vartypes )
    return;

ncopy = n;
n = 0;

v = vartypes;
vartypes = 0;

for ( i=0; i<ncopy; i++ )
    v[i].name.empty();

delete[] v;
}

/***********************************************************************
*                                  type                                *
***********************************************************************/
BITSETYPE VARTYPE_CLASS::type()
{
if ( n > 0 )
    return vartypes[x].type;

return 0;
}

/***********************************************************************
*                                  type                                *
***********************************************************************/
BITSETYPE VARTYPE_CLASS::type( STRING_CLASS & name_to_find )
{
if ( n < 1 )
    return 0;
rewind();
while ( next() )
    {
    if ( vartypes[x].name == name_to_find )
        return vartypes[x].type;
    }

return 0;
}

/***********************************************************************
*                                  next                                *
***********************************************************************/
bool VARTYPE_CLASS::next()
{
if ( rewound )
    {
    rewound = false;
    return true;
    }
if ( x < (n-1) )
    {
    x++;
    return true;
    }

return false;
}

/***********************************************************************
*                                 NAME                                 *
***********************************************************************/
STRING_CLASS & VARTYPE_CLASS::name()
{
static STRING_CLASS noname;

if ( n > 0 )
    return vartypes[x].name;

return noname;
}

/***********************************************************************
*                                 NAME                                 *
***********************************************************************/
STRING_CLASS & VARTYPE_CLASS::name( BITSETYPE type_to_find )
{
static STRING_CLASS noname;
rewind();
while ( next() )
    {
    if ( vartypes[x].type == type_to_find )
        return vartypes[x].name;
    }

return noname;
}

/***********************************************************************
*                     POSITION_CURRENT_DIALOG                          *
***********************************************************************/
static void position_current_dialog()
{
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

    GetWindowRect( ParamDialogWindow, &r );

    y = r.bottom - h - GetSystemMetrics( SM_CYEDGE );
    x = r.right - w;

    GetWindowRect( GetDlgItem(ParamDialogWindow,LOW_WARN_WIRE_PB), &r );
    x += r.right;
    x /= 2;

    GetWindowRect( GetDlgItem(ParamDialogWindow,CLEAR_BUTTON), &r );
    if ( y > r.top )
        {
        y += r.top;
        y /= 2;
        }

    MoveWindow( WindowInfo[CurrentDialogNumber].dialogwindow, x, y, w, h, TRUE );
    }
}
/***********************************************************************
*                    SHOW_TIME_POSITION_DESCIPTION                     *
***********************************************************************/
void show_time_position_desciption( HWND w )
{
STRING_CLASS s;

if ( is_checked(w, TIME_SAMPLE_BUTTON) )
    s = StringTable.find( TEXT("POSITION_STRING") );
else
    s = StringTable.find( TEXT("TIME_STRING") );

s.set_text( w, ANALOG_DESC_TEXTBOX );
}

/***********************************************************************
*                        SHOW_ANALOG_DESCRIPTION                       *
***********************************************************************/
static void show_analog_description( HWND w, int channel )
{
static TCHAR unknown_text[] = TEXT( "???" );
ANALOG_SENSOR a;
STRING_CLASS  s;

s = unknown_text;
if ( channel >= 0 && channel <MAX_FT2_CHANNELS )
    {
    if ( a.find(CurrentPart.analog_sensor[channel]) )
        s = a.desc();
    }
else if ( channel == FT_ANALOG_VELOCITY_CHANNEL )
    {
    s = StringTable.find( TEXT("VELOCITY_STRING") );
    }

s.set_text( w, ANALOG_DESC_TEXTBOX );
}

/***********************************************************************
*                        SHOW_ANALOG_DESCRIPTION                       *
***********************************************************************/
static void show_analog_description()
{
int  i;
int  channel;
int  button;
int  nof_buttons;
HWND w;

w = WindowInfo[CurrentDialogNumber].dialogwindow;

if ( CurrentDialogNumber == FT_EXTENDED_ANALOG_TAB )
    {
    button = CHANNEL_1_PB;
    nof_buttons = MAX_FT2_CHANNELS;
    }
else
    {
    button = CHANNEL_1_BUTTON;
    if ( CurrentDialogNumber == FT_TARGET_TAB )
        nof_buttons = CHANNEL_8_BUTTON - CHANNEL_1_BUTTON + 1;
    else
        nof_buttons = VELOCITY_BUTTON_INDEX + 1;
    }
channel = -1;

for ( i=0; i<nof_buttons; i++ )
    {
    if ( is_checked(w, button) )
        {
        if ( i < DIFFERENTIAL_PRESSURE_BUTTON_INDEX || CurrentDialogNumber == FT_EXTENDED_ANALOG_TAB )
            {
            channel = i;
            }
        else if ( i == DIFFERENTIAL_PRESSURE_BUTTON_INDEX )
            {
            channel = CurrentPart.head_pressure_channel - 1;
            }
        else if ( i == TIME_POSITION_BUTTON_INDEX )
            {
            show_time_position_desciption( w );
            return;
            }
        else if ( i == VELOCITY_BUTTON_INDEX )
            {
            channel = FT_ANALOG_VELOCITY_CHANNEL;
            }

        if ( (channel < 0 || channel >=MAX_FT2_CHANNELS) && channel != FT_ANALOG_VELOCITY_CHANNEL )
            channel = 0;

        break;
        }
    button++;
    }
show_analog_description( w, channel );
}

/***********************************************************************
*                          SHOW_ANALOG_UNITS                           *
***********************************************************************/
static void show_analog_units()
{
int           i;
int           j;
HWND          w;

w = WindowInfo[CurrentDialogNumber].dialogwindow;

if ( CurrentDialogNumber == FT_TARGET_TAB )
    {
    SetDlgItemText( w, ANALOG_UNITS_TEXTBOX,     EmptyString );
    SetDlgItemText( w, END_ANALOG_UNITS_TEXTBOX, units_name(CurrentPart.distance_units) );
    }
else
    {
    i = CurrentPart.distance_units;
    j = i;
    if ( IsDlgButtonChecked(w, TIME_SAMPLE_BUTTON) == BST_CHECKED )
        {
        j = MS_UNITS;
        if ( !StartValueIsNegative )
            i = MS_UNITS;
        }
    SetDlgItemText( w, ANALOG_UNITS_TEXTBOX,     units_name(i) );
    SetDlgItemText( w, END_ANALOG_UNITS_TEXTBOX, units_name(j) );
    }
}

/***********************************************************************
*                     TRUE_INTERNAL_PARAMETER_NAME                     *
***********************************************************************/
static void true_internal_parameter_name( short parameter_number )
{
HWND w;
TCHAR name[PARAMETER_NAME_LEN+1];
NAME_CLASS s;
DB_TABLE   t;

w = WindowInfo[INTERNAL_PARAM_TAB].dialogwindow;

s.get_data_dir_file_name( PARMNAME_DB );
if (t.open(s, PARMNAME_RECLEN, PFL) )
    {
    t.put_short( PARMNAME_NUMBER_OFFSET, parameter_number, PARAMETER_NUMBER_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        {
        if (t.get_alpha(name, PARMNAME_NAME_OFFSET, PARAMETER_NAME_LEN) )
            set_text( w, TRUE_PARAMETER_NAME_TEXTBOX, name );
        }
    t.close();
    }
}

/***********************************************************************
*                       SHOW_INTERNAL_PARAMETER                        *
***********************************************************************/
static void show_internal_parameter()
{
if ( CurrentParam.parameter[CurrentParameterIndex].input.type == INTERNAL_PARAMETER )
    true_internal_parameter_name( CurrentParam.parameter[CurrentParameterIndex].input.number );
}

/***********************************************************************
*                   SHOW_FT_EXTENDED_ANALOG_PARAMETER                  *
***********************************************************************/
static void show_ft_extended_analog_parameter()
{
int  x;
HWND w;

LoadingFtExtendedAnalogs = TRUE;

w = WindowInfo[CurrentDialogNumber].dialogwindow;
x = CurrentFtAnalog.array[CurrentParameterIndex].channel-1;
x += CHANNEL_1_PB;

if ( x >= CHANNEL_1_PB && x <= CHANNEL_16_PB )
    CheckRadioButton( w, CHANNEL_1_PB, CHANNEL_16_PB, x );

CurrentFtAnalog.array[CurrentParameterIndex].ind_value.set_text( w, FT_EXTENDED_INPUT_WIRE_EBOX );

x = FT_EXTENDED_TRIGGER_HIGH_RADIO;
if ( CurrentFtAnalog.array[CurrentParameterIndex].ind_var_type == TRIGGER_WHEN_SMALLER )
    x = FT_EXTENDED_TRIGGER_LOW_RADIO;
CheckRadioButton( w, FT_EXTENDED_TRIGGER_HIGH_RADIO, FT_EXTENDED_TRIGGER_LOW_RADIO, x );

LoadingFtExtendedAnalogs = FALSE;
}

/***********************************************************************
*                       SHOW_FTANALOG_PARAMETER                        *
***********************************************************************/
static void show_ftanalog_parameter()
{
HWND bw;
HWND w;
short x;
short i;
short input_type;
BITSETYPE type;
int   last_button;
BOOL  have_differential_equation;

LoadingFtAnalogs = TRUE;

have_differential_equation = TRUE;
if ( CurrentMachine.diff_curve_number == NO_DIFF_CURVE_NUMBER )
    have_differential_equation = FALSE;

/*
------------------------------------------------------------------
The ftanalog entries are stored at the same index as the parameter
------------------------------------------------------------------ */
i = CurrentParameterIndex;
input_type = CurrentParam.parameter[i].input.type;

if ( input_type == FT_ANALOG_INPUT || input_type == FT_TARGET_INPUT )
    {
    w = WindowInfo[CurrentDialogNumber].dialogwindow;

    if ( input_type == FT_ANALOG_INPUT )
        {
        bw = GetDlgItem( w, DIF_CHANNEL_BUTTON );
        EnableWindow( bw, have_differential_equation );
        bw = GetDlgItem( w, VELOCITY_BUTTON );
        EnableWindow( bw, TRUE );
        }

    /*
    ----------------------
    FasTrak Channel Number
    ---------------------- */
    if ( input_type == FT_ANALOG_INPUT && CurrentFtAnalog.array[i].channel == FT_ANALOG_VELOCITY_CHANNEL )
        x = VELOCITY_BUTTON_INDEX;
    else
        x = CurrentFtAnalog.array[i].channel - 1;
    if ( x >= 0 )
        {
        x += CHANNEL_1_BUTTON;
        last_button = CHANNEL_8_BUTTON;
        if ( input_type == FT_ANALOG_INPUT )
            last_button += VELOCITY_BUTTON - CHANNEL_8_BUTTON;
        CheckRadioButton( w, CHANNEL_1_BUTTON, last_button, x );
        }
    else if ( x == -1 )
        {
        /*
        -------------------------------------------------------------------
        The only case where the channel is zero is if this is a time or pos
        ------------------------------------------------------------------- */
        x = TIME_OR_POS_BUTTON;
        last_button = VELOCITY_BUTTON;
        CheckRadioButton( w, CHANNEL_1_BUTTON, last_button, x );
        }

    if ( input_type == FT_ANALOG_INPUT )
        {
        /*
        -------------------------
        Position or Time interval
        ------------------------- */
        if ( CurrentFtAnalog.array[i].ind_var_type & POSITION_VAR )
            x = POSITION_SAMPLE_BUTTON;
        else
            x = TIME_SAMPLE_BUTTON;
        }
    else
        {
        /*
        -----------------------------------------------
        The same buttons are used with different labels
        ----------------------------------------------- */
        if ( CurrentFtAnalog.array[i].ind_var_type & TRIGGER_WHEN_GREATER )
            x = POSITION_SAMPLE_BUTTON;
        else
            x = TIME_SAMPLE_BUTTON;
        }

    CheckRadioButton( w, POSITION_SAMPLE_BUTTON, TIME_SAMPLE_BUTTON, x );

    CurrentFtAnalog.array[i].end_value.set_text( w, END_POINT_EBOX );
    if ( input_type == FT_ANALOG_INPUT )
        {
        type = CurrentFtAnalog.array[i].result_type;
        x = SINGLE_POINT_RADIO;
        if ( type & MAX_RESULT_TYPE )
            x = MAX_RADIO;
        else if ( type & MIN_RESULT_TYPE )
            x = MIN_RADIO;
        else if ( type & AVG_RESULT_TYPE )
            x = AVG_RADIO;
        else if ( type & SUB_RESULT_TYPE )
            x = SUB_RADIO;
        bw = GetDlgItem( w, x );
        /*
        -------------------------------------------
        Simulate a button click on the radio button
        So I will get a BN_CLICKED notification.
        ------------------------------------------- */
        if ( !is_checked(bw) )
            SendMessage( bw, BM_CLICK, 0, 0 );
        }

    CurrentFtAnalog.array[i].ind_value.set_text( w, SAMPLE_POINT_EDITBOX );
    if ( CurrentFtAnalog.array[i].ind_value.contains(MinusChar) )
        StartValueIsNegative = TRUE;
    else
        StartValueIsNegative = FALSE;
    }

LoadingFtAnalogs = FALSE;
}

/***********************************************************************
*                       SHOW_FTONOFF_PARAMETER                         *
***********************************************************************/
static void show_ftonoff_parameter()
{
HWND  w;
int i;
int wire_number;
int pressed_button;

i = CurrentParameterIndex;
if ( CurrentParam.parameter[i].input.type != FT_ON_OFF_INPUT )
    return;

w = WindowInfo[CurrentDialogNumber].dialogwindow;
wire_number = CurrentParam.parameter[i].input.number;

pressed_button = ONOFF_ON_BUTTON;
if ( CurrentFtAnalog.array[i].ind_var_type & TRIGGER_WHEN_SMALLER )
    pressed_button = ONOFF_OFF_BUTTON;

if ( wire_number < 1 || wire_number > HIGH_FT_INPUT_WIRE )
    {
    get_new_ft_onoff_settings();
    return;
    }

SetDlgItemInt( w, ONOFF_INPUT_WIRE_EB, (UINT) wire_number, FALSE );
CheckRadioButton( w, ONOFF_ON_BUTTON, ONOFF_OFF_BUTTON, pressed_button );
}

/***********************************************************************
*                     SHOW_FT_DIGITAL_PARAMETER                        *
***********************************************************************/
static void show_ft_digital_parameter()
{
HWND  w;

if ( CurrentParam.parameter[CurrentParameterIndex].input.type != FT_DIGITAL_INPUT )
    return;

w = WindowInfo[CurrentDialogNumber].dialogwindow;

set_text( w, FT_DIGITAL_INPUT_WIRE_EBOX, int32toasc(CurrentParam.parameter[CurrentParameterIndex].input.number) );
}

/***********************************************************************
*                              FILL_UNITS                              *
***********************************************************************/
static bool fill_units( LISTBOX_CLASS & destcbox, BITSETYPE current_vartype )
{
int i;
int my_index;
int n;
short current_units_id;
UNITS_CLASS u;

n = u.count();
if ( n < 1 )
    return false;

current_units_id = CurrentParam.parameter[CurrentParameterIndex].units;

i = 0;
my_index = 0;
destcbox.empty();
while ( u.next() )
    {
    if ( u.type() & current_vartype )
        destcbox.add( u.name() );
    if ( u.id() == current_units_id )
        my_index = i;
    i++;
    }

if ( i > 0 )
    destcbox.set_current_index( my_index );

return true;
}

/***********************************************************************
*                        SHOW_FT_CALC_PARAMETER                        *
***********************************************************************/
static void show_ft_calc_parameter()
{
HWND  w;
BITSETYPE current_vartype;

if ( CurrentParam.parameter[CurrentParameterIndex].input.type != FT_CALC_INPUT )
    return;

w = WindowInfo[CurrentDialogNumber].dialogwindow;

current_vartype = CurrentParam.parameter[CurrentParameterIndex].vartype;

CurrentFtCalc.cmd[CurrentParameterIndex].set_text( w, FT_CALC_COMMAND_EBOX );
VartypeCbox.setcursel( Vartypes.name(current_vartype).text() );
fill_units( UnitsCbox, current_vartype );
UnitsCbox.setcursel( units_name(CurrentParam.parameter[CurrentParameterIndex].units) );
}

/***********************************************************************
*                   SHOW_FT_PEAK_FS_VELOCITY_PARAMETER                 *
***********************************************************************/
static void show_ft_peak_fs_velocity_parameter()
{
HWND w;
int i;

i = CurrentParameterIndex;

if ( CurrentParam.parameter[i].input.type != FT_PEAK_FS_VELOCITY )
    return;

LoadingFtAnalogs = TRUE;

w = WindowInfo[CurrentDialogNumber].dialogwindow;

CurrentFtAnalog.array[i].ind_value.set_text( w, PFSV_START_EBOX );
CurrentFtAnalog.array[i].end_value.set_text( w, PFSV_END_EBOX );
CurrentFtAnalog.array[i].smooth_n.set_text( w, PFSV_N_EBOX );
CurrentFtAnalog.array[i].smooth_multiplier.set_text( w, PFSV_MULTIPLIER_EBOX );

i = CurrentPart.distance_units;
SetDlgItemText( w, PFSV_POS_UNITS_TBOX, units_name(i) );

LoadingFtAnalogs = FALSE;
}

/***********************************************************************
*                     FILL_EXTERNAL_PARAMETER_UNITS                    *
***********************************************************************/
static bool fill_external_parameter_units( BITSETYPE current_vartype )
{
int i;
int my_index;
int n;
short current_units_id;
UNITS_CLASS u;

n = u.count();
if ( n < 1 )
    return false;

current_units_id = CurrentParam.parameter[CurrentParameterIndex].units;

i = 0;
my_index = 0;
FtEpUnitsCbox.empty();
while ( u.next() )
    {
    if ( u.type() & current_vartype )
        FtEpUnitsCbox.add( u.name() );
    if ( u.id() == current_units_id )
        my_index = i;
    i++;
    }

FtEpUnitsCbox.set_current_index( my_index );
return true;
}

/***********************************************************************
*                      SHOW_FT_EXTERNAL_PARAMETER                      *
***********************************************************************/
static void show_ft_external_parameter()
{
HWND  w;
BITSETYPE current_vartype;

w = WindowInfo[CurrentDialogNumber].dialogwindow;

current_vartype = CurrentParam.parameter[CurrentParameterIndex].vartype;
FtEpVartypeCbox.setcursel( Vartypes.name(current_vartype).text() );
fill_units(FtEpUnitsCbox, current_vartype );
FtEpUnitsCbox.setcursel( units_name(CurrentParam.parameter[CurrentParameterIndex].units) );
}

/***********************************************************************
*                      SHOW_FT_STATIC_TEXT_PARAMETER                      *
***********************************************************************/
static void show_ft_static_text_parameter()
{
HWND  w;

w = WindowInfo[CurrentDialogNumber].dialogwindow;
LoadingStaticText = true;
CurrentParam.parameter[CurrentParameterIndex].static_text.value.set_text( w, FT_STATIC_TEXT_VALUE_EB );
set_checkbox( w, FT_STATIC_TEXT_CREATE_XB, CurrentParam.parameter[CurrentParameterIndex].static_text.popup_on_create );
set_checkbox( w, FT_STATIC_TEXT_MONITOR_XB, CurrentParam.parameter[CurrentParameterIndex].static_text.popup_on_monitor );
LoadingStaticText = false;
}

/***********************************************************************
*                           UPDATE_CURRENT_DIALOG                      *
***********************************************************************/
static void update_current_dialog()
{
switch ( CurrentDialogNumber )
    {
    case INTERNAL_PARAM_TAB:
        show_internal_parameter();
        break;

    case FT_ANALOG_TAB:
    case FT_TARGET_TAB:
        show_ftanalog_parameter();
        show_analog_description();
        show_analog_units();
        break;

    case FT_ONOFF_TAB:
        show_ftonoff_parameter();
        break;

    case FT_DIGITAL_TAB:
        show_ft_digital_parameter();
        break;

    case FT_CALC_TAB:
        show_ft_calc_parameter();
        break;

    case FT_EXTENDED_ANALOG_TAB:
        show_ft_extended_analog_parameter();
        show_analog_description();
        show_analog_units();
        break;

    case FT_PEAK_FS_VELOCITY_TAB:
        show_ft_peak_fs_velocity_parameter();
        break;

    case FT_EXTERNAL_PARAMETER_TAB:
        show_ft_external_parameter();
        break;

    case FT_STATIC_TEXT_PARAMETER_TAB:
        show_ft_static_text_parameter();
        break;
    }
}

/***********************************************************************
*                          HIDE_CURRENT_DIALOG                         *
***********************************************************************/
static void hide_current_dialog()
{
if ( CurrentDialogNumber != NO_TAB )
    ShowWindow( WindowInfo[CurrentDialogNumber].dialogwindow, SW_HIDE );
}

/***********************************************************************
*                          SHOW_CURRENT_DIALOG                         *
***********************************************************************/
static void show_current_dialog()
{
WINDOWPLACEMENT wp;

if ( CurrentDialogNumber < 0 || CurrentDialogNumber >= TAB_COUNT )
    {
    CurrentDialogNumber = NO_TAB;
    }
else
    {
    wp.length = sizeof(WINDOWPLACEMENT);
    if ( GetWindowPlacement(WindowInfo[CurrentDialogNumber].dialogwindow, &wp)  )
        {
        if ( wp.showCmd != SW_SHOW )
            {
            ShowWindow( WindowInfo[CurrentDialogNumber].dialogwindow, SW_SHOW );
            position_current_dialog();
            update_current_dialog();
            }
        }
    }
}

/***********************************************************************
*                            SHOW_NEW_DIALOG                           *
***********************************************************************/
static void show_new_dialog( int new_dialog_number )
{
if ( CurrentDialogNumber != new_dialog_number )
    hide_current_dialog();

if ( new_dialog_number >= 0 && new_dialog_number < TAB_COUNT )
    {
    CurrentDialogNumber = new_dialog_number;
    show_current_dialog();
    }
}

/***********************************************************************
*                       INTERNAL_PARAMETER_UNITS                       *
***********************************************************************/
static short internal_parameter_units( short input_number, BITSETYPE vartype )
{
if ( vartype == DISTANCE_VAR )
    {
    return CurrentPart.distance_units;
    }
else if ( vartype == POSITION_VAR )
    {
    return CurrentPart.distance_units;
    }
else if ( vartype == PRESSURE_VAR )
    {
    return CurrentPart.pressure_units;
    }
else if ( vartype == TIME_VAR )
    {
    if ( input_number == (CYCLE_TIME_PARM+1) )
        return SECOND_UNITS;
    else
        return MS_UNITS;
    }
else if ( vartype == TIME_INTERVAL_VAR )
    {
    return MS_UNITS;
    }
else if ( vartype == VELOCITY_VAR )
    {
    return CurrentPart.velocity_units;
    }
else if ( vartype == WORK_VAR )
    {
    return KJ_UNITS;
    }

return NO_UNITS;
}

/***********************************************************************
*                      INTERNAL_PARAMETER_VARTYPE                      *
***********************************************************************/
static BITSETYPE internal_parameter_vartype( short input_number )
{
BITSETYPE vartype;
DB_TABLE  t;
NAME_CLASS s;

vartype = NO_VARIABLE_TYPE;

s.get_data_dir_file_name( PARMNAME_DB );
if ( t.open(s, PARMNAME_RECLEN, PFL) )
    {
    t.put_short( PARMNAME_NUMBER_OFFSET, input_number, PARAMETER_NUMBER_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        vartype = (BITSETYPE) t.get_long( PARMNAME_TYPE_OFFSET );
    t.close();
    }

return vartype;
}

/***********************************************************************
*                   CHOSEN_INTERNAL_PARAMETER_NUMBER                   *
*                                                                      *
*   Return the parameter number from the list of internal parameters.  *
***********************************************************************/
static short chosen_internal_parameter_number()
{
HWND w;
LRESULT x;

w = WindowInfo[INTERNAL_PARAM_TAB].dialogwindow;
x = SendDlgItemMessage( w, PARAMETER_LISTBOX_ID, LB_GETCURSEL, 0, 0L);
if ( x != LB_ERR )
    return (short) x+1;

return NO_PARAMETER_NUMBER;
}

/***********************************************************************
*                      GET_NEW_PARAMETER_SETTINGS                      *
***********************************************************************/
static void get_new_parameter_settings()
{
short i;
STRING_CLASS s;

i = CurrentParameterIndex;

if ( i < 0 || i >= MAX_PARMS )
    return;

GetDlgItemText( ParamDialogWindow, PARAMETER_DESC_EDITBOX, CurrentParam.parameter[i].name, PARAMETER_NAME_LEN+1 );

CurrentParam.parameter[i].limits[WARNING_MIN].svalue.get_text( ParamDialogWindow, LOW_WARN_VALUE_EDITBOX );
CurrentParam.parameter[i].limits[WARNING_MIN].value = CurrentParam.parameter[i].limits[WARNING_MIN].svalue.float_value();

CurrentParam.parameter[i].limits[WARNING_MAX].svalue.get_text( ParamDialogWindow, HIGH_WARN_VALUE_EDITBOX  );
CurrentParam.parameter[i].limits[WARNING_MAX].value = CurrentParam.parameter[i].limits[WARNING_MAX].svalue.float_value();

CurrentParam.parameter[i].limits[ALARM_MIN].svalue.get_text( ParamDialogWindow, LOW_ALARM_VALUE_EDITBOX  );
CurrentParam.parameter[i].limits[ALARM_MIN].value = CurrentParam.parameter[i].limits[ALARM_MIN].svalue.float_value();

CurrentParam.parameter[i].limits[ALARM_MAX].svalue.get_text( ParamDialogWindow, HIGH_ALARM_VALUE_EDITBOX );
CurrentParam.parameter[i].limits[ALARM_MAX].value = CurrentParam.parameter[i].limits[ALARM_MAX].svalue.float_value();

CurrentParam.parameter[i].target.get_text( ParamDialogWindow, TARGET_VALUE_EDITBOX  );

s.get_text( ParamDialogWindow, LOW_WARN_WIRE_PB );
CurrentParam.parameter[i].limits[WARNING_MIN].wire_number = s.int_value();

s.get_text( ParamDialogWindow, HIGH_WARN_WIRE_PB );
CurrentParam.parameter[i].limits[WARNING_MAX].wire_number = s.int_value();

s.get_text( ParamDialogWindow, LOW_ALARM_WIRE_PB );
CurrentParam.parameter[i].limits[ALARM_MIN].wire_number   = s.int_value();

s.get_text( ParamDialogWindow, HIGH_ALARM_WIRE_PB );
CurrentParam.parameter[i].limits[ALARM_MAX].wire_number   = s.int_value();

refresh_current_parameter_limits();
}

/***********************************************************************
*                 GET_NEW_INTERNAL_PARAMETER_SETTINGS                  *
***********************************************************************/
static void get_new_internal_parameter_settings()
{
short i;
short ipn;
short type;

i = CurrentParameterIndex;

if ( i < 0 || i >= MAX_PARMS )
    return;

ipn = chosen_internal_parameter_number();
if ( ipn != NO_PARAMETER_NUMBER )
    {
    CurrentParam.parameter[i].input.type   = INTERNAL_PARAMETER;
    CurrentParam.parameter[i].input.number = ipn;

    type = internal_parameter_vartype(ipn);
    CurrentParam.parameter[i].vartype      = type;
    CurrentParam.parameter[i].units        = internal_parameter_units(ipn, type );
    }
}

/**********************************************************************
*                 UPDATE_INTERNAL_PARAM_STRING_SETTINGS               *
**********************************************************************/
static void update_internal_param_string_settings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { INTERNAL_NAME_GB,    TEXT( "INTERNAL_NAME" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, TRUE_PARAMETER_NAME_TEXTBOX );
}

/***********************************************************************
*                          INTERNAL_PARAM_PROC                         *
***********************************************************************/
BOOL CALLBACK internal_param_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;

cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        update_internal_param_string_settings( hWnd );
        return TRUE;

    case WM_COMMAND:
        if ( cmd == LBN_SELCHANGE )
            {
            get_new_internal_parameter_settings();
            refresh_current_parameter_limits();
            return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                   GET_NEW_FT_EXTENDED_CHANNEL                        *
*   This returns the INDEX of the channel and not the channel number.  *
***********************************************************************/
static int get_new_ft_extended_channel( HWND w )
{
ANALOG_SENSOR a;
int           i;
int           p;

p = CurrentParameterIndex;
if ( p < 0 || p >= MAX_PARMS )
    return 0;

for ( i=0; i<MAX_FT2_CHANNELS; i++ )
    {
    if ( is_checked(w, CHANNEL_1_PB+i) )
        {
        CurrentFtAnalog.array[p].channel = i+1;
        a.find( CurrentPart.analog_sensor[i] );
        CurrentParam.parameter[p].units   = a.units_index();
        CurrentParam.parameter[p].vartype = a.vartype();
        break;
        }
    }

return i;
}

/***********************************************************************
*                        GET_NEW_FT_EXTENDED_WIRE                      *
***********************************************************************/
static void get_new_ft_extended_wire( HWND w )
{
int p;

p = CurrentParameterIndex;
if ( p < 0 || p >= MAX_PARMS )
    return;

CurrentFtAnalog.array[p].result_type = SINGLE_POINT_RESULT_TYPE;
CurrentFtAnalog.array[p].ind_value.get_text( w, FT_EXTENDED_INPUT_WIRE_EBOX );
}

/***********************************************************************
*                      GET_NEW_FT_EXTENDED_TRIGGER                     *
***********************************************************************/
static void get_new_ft_extended_trigger( HWND w )
{
int p;

p = CurrentParameterIndex;
if ( p < 0 || p >= MAX_PARMS )
    return;

CurrentFtAnalog.array[p].result_type = SINGLE_POINT_RESULT_TYPE;

CurrentFtAnalog.array[p].ind_var_type = TRIGGER_WHEN_GREATER;
if ( is_checked( w, FT_EXTENDED_TRIGGER_LOW_RADIO) )
    CurrentFtAnalog.array[p].ind_var_type = TRIGGER_WHEN_SMALLER;
}

/***********************************************************************
*                GET_NEW_FT_EXTENDED_ANALOG_SETTINGS                   *
***********************************************************************/
static void get_new_ft_extended_analog_settings( HWND w )
{
int p;

if ( LoadingFtExtendedAnalogs )
    return;

p = CurrentParameterIndex;
if ( p < 0 || p >= MAX_PARMS )
    return;

CurrentParam.parameter[p].input.type = FT_EXTENDED_ANALOG_INPUT;
CurrentParam.parameter[p].input.number = 0;

get_new_ft_extended_channel( w );
get_new_ft_extended_wire( w );
get_new_ft_extended_trigger( w );
CurrentFtAnalog.array[p].end_value = ZeroChar;
}

/***********************************************************************
*                      GET_NEW_FT_ANALOG_SETTINGS                      *
***********************************************************************/
static void get_new_ft_analog_settings()
{
ANALOG_SENSOR a;
int     i;
int     nof_buttons;
int     p;
int     channel;
HWND    w;
BOOLEAN was_negative;

if ( LoadingFtAnalogs )
    return;

p = CurrentParameterIndex;
if ( p < 0 || p >= MAX_PARMS )
    return;

if ( CurrentDialogNumber == FT_ANALOG_TAB )
    {
    CurrentParam.parameter[p].input.type = FT_ANALOG_INPUT;
    nof_buttons = VELOCITY_BUTTON_INDEX + 1;
    }
else if ( CurrentDialogNumber == FT_EXTENDED_ANALOG_TAB )
    {
    CurrentParam.parameter[p].input.type = FT_EXTENDED_ANALOG_INPUT;
    nof_buttons = MAX_FT2_CHANNELS;
    }
else
    {
    CurrentParam.parameter[p].input.type = FT_TARGET_INPUT;
    nof_buttons = MAX_FT_CHANNELS;
    }

/*
------------------------------------
Input number is NOT used for analogs
------------------------------------ */
CurrentParam.parameter[p].input.number = 0;

w = WindowInfo[CurrentDialogNumber].dialogwindow;

/*
----------------------
FasTrak Channel Number
---------------------- */
channel = NO_FT_CHANNEL;
for ( i=0; i<nof_buttons; i++ )
    {
    if ( is_checked(w, CHANNEL_1_BUTTON+i) )
        {
        CurrentFtAnalog.array[p].channel = i+1;
        if ( CurrentDialogNumber == FT_ANALOG_TAB )
            {
            if ( i == TIME_POSITION_BUTTON_INDEX )
                {
                CurrentFtAnalog.array[p].channel = NO_FT_CHANNEL;
                }
            else if ( i == VELOCITY_BUTTON_INDEX )
                {
                CurrentFtAnalog.array[p].channel = FT_ANALOG_VELOCITY_CHANNEL;
                CurrentParam.parameter[p].units  = CurrentPart.velocity_units;
                }
            }

        /*
        ---------------------------------------------------------------------------------
        The last button is the differental channel button. Use the head pressure channel.
        --------------------------------------------------------------------------------- */
        if ( i < MAX_FT_CHANNELS || CurrentDialogNumber == FT_EXTENDED_ANALOG_TAB )
            channel = CurrentFtAnalog.array[p].channel;
        else if ( i == DIFFERENTIAL_PRESSURE_BUTTON_INDEX )
            channel = CurrentPart.head_pressure_channel;
        else if ( i == TIME_POSITION_BUTTON_INDEX )
            channel = NO_FT_CHANNEL;
        else if ( i == VELOCITY_BUTTON_INDEX )
            channel = FT_ANALOG_VELOCITY_CHANNEL;
        if ( channel != NO_FT_CHANNEL && channel != FT_ANALOG_VELOCITY_CHANNEL )
            {
            a.find( CurrentPart.analog_sensor[channel-1] );
            CurrentParam.parameter[p].units   = a.units_index();
            CurrentParam.parameter[p].vartype = a.vartype();
            }

        if ( CurrentDialogNumber == FT_TARGET_TAB )
            {
            CurrentParam.parameter[p].units   = CurrentPart.distance_units;
            CurrentParam.parameter[p].vartype = POSITION_VAR;
            }

        /*
        -----------------------------------------------------------
        Position (pre-impact) or Time (post-impact) Sample Interval
        ----------------------------------------------------------- */
        if ( CurrentDialogNumber == FT_ANALOG_TAB )
            {
            if ( is_checked(w, POSITION_SAMPLE_BUTTON) )
                {
                CurrentFtAnalog.array[p].ind_var_type = POSITION_VAR;
                if ( channel == NO_FT_CHANNEL )
                    {
                    /*
                    ---------------------------------------------------
                    This is time in ms at a specified pos before impact
                    --------------------------------------------------- */
                    CurrentParam.parameter[p].units   = MS_UNITS;
                    CurrentParam.parameter[p].vartype = TIME_VAR;
                    }
                }
            else
                {
                CurrentFtAnalog.array[p].ind_var_type = TIME_VAR;
                if ( channel == NO_FT_CHANNEL )
                    {
                    /*
                    ----------------------------------------------
                    This is a pos at a specified time after impact
                    ---------------------------------------------- */
                    CurrentParam.parameter[p].units   = CurrentPart.distance_units;
                    CurrentParam.parameter[p].vartype = POSITION_VAR;
                    }
                }
            }
        else
            {
            if ( is_checked(w, POSITION_SAMPLE_BUTTON) )
                CurrentFtAnalog.array[p].ind_var_type = TRIGGER_WHEN_GREATER;
            else
                CurrentFtAnalog.array[p].ind_var_type = TRIGGER_WHEN_SMALLER;
            }

        CurrentFtAnalog.array[p].result_type = SINGLE_POINT_RESULT_TYPE;
        if ( CurrentDialogNumber == FT_ANALOG_TAB )
            {
            if ( is_checked(w, MAX_RADIO) )
                CurrentFtAnalog.array[p].result_type = MAX_RESULT_TYPE;
            else if ( is_checked(w, MIN_RADIO) )
                CurrentFtAnalog.array[p].result_type = MIN_RESULT_TYPE;
            else if ( is_checked(w, AVG_RADIO) )
                CurrentFtAnalog.array[p].result_type = AVG_RESULT_TYPE;
            else if ( is_checked(w, SUB_RADIO) )
                CurrentFtAnalog.array[p].result_type = SUB_RESULT_TYPE;
            }

        CurrentFtAnalog.array[p].ind_value.get_text( w, SAMPLE_POINT_EDITBOX );
        was_negative = StartValueIsNegative;
        if ( CurrentFtAnalog.array[p].ind_value.contains(MinusChar) )
            StartValueIsNegative = TRUE;
        else
            StartValueIsNegative = FALSE;

        CurrentFtAnalog.array[p].end_value.get_text( w, END_POINT_EBOX );
        if ( was_negative != StartValueIsNegative )
            show_analog_units();
        break;
        }
    }
}

/**********************************************************************
*                    UPDATE_FT_ANALOG_SETUP_STRINGS                   *
**********************************************************************/
static void update_ft_analog_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { DIF_STATIC,               TEXT( "DIF_STRING" ) },
    { TIME_POS_STATIC,          TEXT( "TIME_POS_STRING" ) },
    { FT_ANALOG_VEL_STATIC,     TEXT( "VELOCITY_STRING" ) },
    { FT_ANALOG_CHANNEL_STATIC, TEXT( "FT2_ANALOG_CHANNEL" ) },
    { POSITION_SAMPLE_BUTTON,   TEXT( "POS_DURING_SHOT" ) },
    { TIME_SAMPLE_BUTTON,       TEXT( "TIME_DURING_INTENS" ) },
    { SINGLE_POINT_RADIO,       TEXT( "SINGLE_POINT" ) },
    { MAX_RADIO,                TEXT( "MAX_STRING" ) },
    { MIN_RADIO,                TEXT( "MIN_STRING" ) },
    { AVG_RADIO,                TEXT( "AVG_STRING" ) },
    { SUB_RADIO,                TEXT( "SUBTRACT_STRING" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, ANALOG_DESC_TEXTBOX );
set_statics_font( hWnd, ANALOG_UNITS_TEXTBOX );
set_statics_font( hWnd, END_ANALOG_UNITS_TEXTBOX );
set_statics_font( hWnd, END_POINT_TBOX );
set_statics_font( hWnd, START_POINT_TBOX );
}

/***********************************************************************
*                              FT_ANALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK ft_analog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static WINDOW_CLASS end_point_ebox;
static WINDOW_CLASS end_point_tbox;
static WINDOW_CLASS start_point_tbox;

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CheckRadioButton( hWnd, CHANNEL_1_BUTTON, VELOCITY_BUTTON, CHANNEL_1_BUTTON );
        CheckRadioButton( hWnd, POSITION_SAMPLE_BUTTON, TIME_SAMPLE_BUTTON, POSITION_SAMPLE_BUTTON );
        end_point_ebox   = GetDlgItem( hWnd, END_POINT_EBOX );
        end_point_tbox   = GetDlgItem( hWnd, END_POINT_TBOX );
        start_point_tbox = GetDlgItem( hWnd, START_POINT_TBOX );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        update_ft_analog_setup_strings( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CHANNEL_1_BUTTON:
            case CHANNEL_2_BUTTON:
            case CHANNEL_3_BUTTON:
            case CHANNEL_4_BUTTON:
            case CHANNEL_5_BUTTON:
            case CHANNEL_6_BUTTON:
            case CHANNEL_7_BUTTON:
            case CHANNEL_8_BUTTON:
            case DIF_CHANNEL_BUTTON:
            case TIME_OR_POS_BUTTON:
            case VELOCITY_BUTTON:
                show_analog_description();
                get_new_ft_analog_settings();
                /*
                ---------------------------------------------------------
                This changes the units and should be shown in the listbox
                --------------------------------------------------------- */
                refresh_current_parameter_limits();
                return TRUE;

            case SINGLE_POINT_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    if ( end_point_ebox.is_visible() )
                        {
                        start_point_tbox.set_title( StringTable.find(TEXT("SAMPLE_AT")) );
                        end_point_ebox.hide();
                        end_point_tbox.hide();
                        }
                    get_new_ft_analog_settings();
                    }
                return TRUE;

            case MAX_RADIO:
            case MIN_RADIO:
            case AVG_RADIO:
            case SUB_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    if ( !end_point_ebox.is_visible() )
                        {
                        start_point_tbox.set_title( StringTable.find(TEXT("START_STRING")) );
                        end_point_tbox.set_title( StringTable.find(TEXT("END_STRING")) );
                        end_point_ebox.show();
                        end_point_tbox.show();
                        }
                    get_new_ft_analog_settings();
                    }
                return TRUE;

            case POSITION_SAMPLE_BUTTON:
            case TIME_SAMPLE_BUTTON:
                show_analog_units();
                get_new_ft_analog_settings();
                if ( is_checked(hWnd, TIME_OR_POS_BUTTON) )
                    {
                    show_analog_description();
                    refresh_current_parameter_limits();
                    }
                return TRUE;

            case SAMPLE_POINT_EDITBOX:
            case END_POINT_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    get_new_ft_analog_settings();
                    return TRUE;
                    }
                break;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                      GET_NEW_FT_TARGET_SETTINGS                      *
***********************************************************************/
static void get_new_ft_target_settings()
{
ANALOG_SENSOR a;
int     channel;
int     i;
int     nof_buttons;
int     p;
HWND    w;

if ( LoadingFtAnalogs )
    return;

p = CurrentParameterIndex;
if ( p < 0 || p >= MAX_PARMS )
    return;

CurrentParam.parameter[p].input.type = FT_TARGET_INPUT;
nof_buttons = MAX_FT_CHANNELS;

/*
------------------------------------
Input number is NOT used for analogs
------------------------------------ */
CurrentParam.parameter[p].input.number = 0;

w = WindowInfo[CurrentDialogNumber].dialogwindow;

/*
----------------------
FasTrak Channel Number
---------------------- */
channel = NO_FT_CHANNEL;
for ( i=0; i<nof_buttons; i++ )
    {
    if ( is_checked(w, CHANNEL_1_BUTTON+i) )
        {
        CurrentFtAnalog.array[p].channel = i+1;
        a.find( CurrentPart.analog_sensor[channel-1] );
        CurrentParam.parameter[p].units   = a.units_index();
        CurrentParam.parameter[p].vartype = a.vartype();
        break;
        }
    }
CurrentParam.parameter[p].units   = CurrentPart.distance_units;
CurrentParam.parameter[p].vartype = POSITION_VAR;

if ( is_checked(w, POSITION_SAMPLE_BUTTON) )
    CurrentFtAnalog.array[p].ind_var_type = TRIGGER_WHEN_GREATER;
else
    CurrentFtAnalog.array[p].ind_var_type = TRIGGER_WHEN_SMALLER;

CurrentFtAnalog.array[p].result_type = SINGLE_POINT_RESULT_TYPE;

CurrentFtAnalog.array[p].ind_value.get_text( w, SAMPLE_POINT_EDITBOX );
CurrentFtAnalog.array[p].end_value.get_text( w, END_POINT_EBOX );
}

/**********************************************************************
*                    UPDATE_FT_TARGET_SETUP_STRINGS                   *
**********************************************************************/
static void update_ft_target_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { FT_ANALOG_CHANNEL_STATIC, TEXT( "FT2_ANALOG_CHANNEL" ) },
    { POSITION_SAMPLE_BUTTON,   TEXT( "TRIGGER_WHEN_GT" ) },
    { TIME_SAMPLE_BUTTON,       TEXT( "TRIGGER_WHEN_LT" ) },
    { START_POINT_TBOX,         TEXT( "TARGET_VALUE" ) },
    { END_POINT_TBOX,           TEXT( "STARTING_POSITION" ) },
    { TRIGGER_TYPE_STATIC,      TEXT( "TRIGGER_TYPE" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, ANALOG_DESC_TEXTBOX );
set_statics_font( hWnd, ANALOG_UNITS_TEXTBOX );
}
/***********************************************************************
*                              FT_TARGET_PROC                          *
***********************************************************************/
BOOL CALLBACK ft_target_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_ft_target_setup_strings( hWnd );
        CheckRadioButton( hWnd, CHANNEL_1_BUTTON, CHANNEL_8_BUTTON, CHANNEL_1_BUTTON );
        CheckRadioButton( hWnd, POSITION_SAMPLE_BUTTON, TIME_SAMPLE_BUTTON, POSITION_SAMPLE_BUTTON );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CHANNEL_1_BUTTON:
            case CHANNEL_2_BUTTON:
            case CHANNEL_3_BUTTON:
            case CHANNEL_4_BUTTON:
            case CHANNEL_5_BUTTON:
            case CHANNEL_6_BUTTON:
            case CHANNEL_7_BUTTON:
            case CHANNEL_8_BUTTON:
                show_analog_description();
                get_new_ft_target_settings();
                /*
                ---------------------------------------------------------
                This changes the units and should be shown in the listbox
                --------------------------------------------------------- */
                refresh_current_parameter_limits();
                return TRUE;

            case POSITION_SAMPLE_BUTTON:
            case TIME_SAMPLE_BUTTON:
                show_analog_units();
                get_new_ft_target_settings();
                return TRUE;

            case SAMPLE_POINT_EDITBOX:
            case END_POINT_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    get_new_ft_target_settings();
                    return TRUE;
                    }
                break;
            }
        break;

    }

return FALSE;
}

/**********************************************************************
*              UPDATE_FT_EXTENDED_ANALOG_SETUP_STRINGS                *
**********************************************************************/
static void update_ft_extended_analog_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { FT_EXTENDED_INPUT_WIRE_EBOX,       TEXT( "INPUT_WIRE_NUMBER" ) },
    { FT_EXTENDED_TRIGGER_HIGH_RADIO,    TEXT( "TRIGGER_HIGH" ) },
    { FT_EXTENDED_TRIGGER_LOW_RADIO,     TEXT( "TRIGGER_LOW" ) },
    { FT_EXTENDED_ANALOG_CHANNEL_STATIC, TEXT( "FT2_ANALOG_CHANNEL" ) },
    { FT_EXTENDED_INPUT_NUMBER_STATIC,   TEXT( "INPUT_NUMBER" ) },
    { FT_EXTENDED_WHEN_TO_SAMPLE_GB,     TEXT( "WHEN_TO_SAMPLE" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, ANALOG_DESC_TEXTBOX );
}

/***********************************************************************
*                         FT_EXTENDED_ANALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK ft_extended_analog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CheckRadioButton( hWnd, CHANNEL_1_PB, CHANNEL_16_PB, CHANNEL_9_PB );
        update_ft_extended_analog_setup_strings( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CHANNEL_1_PB:
            case CHANNEL_2_PB:
            case CHANNEL_3_PB:
            case CHANNEL_4_PB:
            case CHANNEL_5_PB:
            case CHANNEL_6_PB:
            case CHANNEL_7_PB:
            case CHANNEL_8_PB:
            case CHANNEL_9_PB:
            case CHANNEL_10_PB:
            case CHANNEL_11_PB:
            case CHANNEL_12_PB:
            case CHANNEL_13_PB:
            case CHANNEL_14_PB:
            case CHANNEL_15_PB:
            case CHANNEL_16_PB:
                if ( id != (CHANNEL_1_PB+CurrentFtAnalog.array[CurrentParameterIndex].channel-1) )
                    {
                    if ( !LoadingFtExtendedAnalogs )
                        {
                        cmd = get_new_ft_extended_channel( hWnd );
                        show_analog_description( hWnd, cmd );
                        /*
                        ---------------------------------------------------------
                        This changes the units and should be shown in the listbox
                        --------------------------------------------------------- */
                        refresh_current_parameter_limits();
                        }
                    }
                break;

            case FT_EXTENDED_INPUT_WIRE_EBOX:
                if ( cmd == EN_CHANGE && !LoadingFtExtendedAnalogs )
                    get_new_ft_extended_wire( hWnd );
                break;

            case FT_EXTENDED_TRIGGER_LOW_RADIO:
            case FT_EXTENDED_TRIGGER_HIGH_RADIO:
                if ( !LoadingFtExtendedAnalogs )
                    get_new_ft_extended_trigger( hWnd );
                break;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                     GET_NEW_FT_DIGITAL_SETTINGS                      *
***********************************************************************/
void get_new_ft_digital_settings( HWND w )
{
short pi;
short value;
BOOL  status;

value = GetDlgItemInt(w, FT_DIGITAL_INPUT_WIRE_EBOX, &status, FALSE );

if ( status )
    {
    pi = CurrentParameterIndex;
    if ( pi >= 0 && pi < MAX_PARMS )
        CurrentParam.parameter[pi].input.number = value;
    }
}

/**********************************************************************
*                      UPDATE_FT_DIGITAL_SETUP_STRINGS                     *
**********************************************************************/
static void update_ft_digital_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { INPUT_WIRE_STATIC, TEXT( "INPUT_WIRE_PROMPT" ) },
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );
}

/***********************************************************************
*                              FT_DIGITAL_PROC                          *
***********************************************************************/
BOOL CALLBACK ft_digital_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_ft_digital_setup_strings( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case FT_DIGITAL_INPUT_WIRE_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    get_new_ft_digital_settings( hWnd );
                    return TRUE;
                    }
                break;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                       GET_NEW_FT_CALC_SETTINGS                       *
***********************************************************************/
static void get_new_ft_calc_settings( HWND w )
{
CurrentFtCalc.cmd[CurrentParameterIndex].get_text( w, FT_CALC_COMMAND_EBOX );
}

/***********************************************************************
*                            DO_NEW_VARTYPE                            *
***********************************************************************/
static void do_new_vartype()
{
STRING_CLASS s;
BITSETYPE    current_vartype;

s = VartypeCbox.selected_text();
current_vartype = Vartypes.type( s );
if ( current_vartype != CurrentParam.parameter[CurrentParameterIndex].vartype )
    {
    CurrentParam.parameter[CurrentParameterIndex].vartype = current_vartype;
    fill_units( UnitsCbox, current_vartype );

    if ( !UnitsCbox.setcursel(units_name(CurrentParam.parameter[CurrentParameterIndex].units)) )
        {
        UnitsCbox.setcursel( 0 );
        s = UnitsCbox.selected_text();
        CurrentParam.parameter[CurrentParameterIndex].units = units_id( s.text() );
        }
    refresh_current_parameter_limits();
    }
}

/***********************************************************************
*                             DO_NEW_UNITS                             *
***********************************************************************/
static void do_new_units()
{
STRING_CLASS s;

s = UnitsCbox.selected_text();
CurrentParam.parameter[CurrentParameterIndex].units = units_id( s.text() );
    refresh_current_parameter_limits();
}

/**********************************************************************
*                      UPDATE_CALC_SETUP_STRINGS                      *
**********************************************************************/
static void update_calc_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { FT_CALC_COMMAND_STATIC, TEXT( "INPUT_COMMAND" ) },
    { FT_CALC_VARTYPE_STATIC, TEXT( "VARIABLE_TYPE" ) },
    { FT_CALC_UNITS_STATIC,   TEXT( "UNITS_STRING" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, VARTYPE_CBOX );
set_statics_font( hWnd, UNITS_CBOX   );
}

/***********************************************************************
*                               FT_CALC_PROC                           *
***********************************************************************/
BOOL CALLBACK ft_calc_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_calc_setup_strings( hWnd );
        UnitsCbox.init( hWnd, UNITS_CBOX );
        VartypeCbox.init( hWnd, VARTYPE_CBOX );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case FT_CALC_COMMAND_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    get_new_ft_calc_settings( hWnd );
                    return TRUE;
                    }
                break;

            case UNITS_CBOX:
                if ( cmd == LBN_SELCHANGE )
                    do_new_units();
                break;

            case VARTYPE_CBOX:
                if ( cmd == LBN_SELCHANGE )
                    do_new_vartype();
                break;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                 DO_NEW_EXTERNAL_PARAMETER_VARTYPE                    *
***********************************************************************/
static void do_new_external_parameter_vartype()
{
STRING_CLASS s;
BITSETYPE    current_vartype;

s = FtEpVartypeCbox.selected_text();
current_vartype = Vartypes.type( s );
if ( current_vartype != CurrentParam.parameter[CurrentParameterIndex].vartype )
    {
    CurrentParam.parameter[CurrentParameterIndex].vartype = current_vartype;
    fill_units( FtEpUnitsCbox, current_vartype );
    if ( !FtEpUnitsCbox.setcursel(units_name(CurrentParam.parameter[CurrentParameterIndex].units)) )
        {
        FtEpUnitsCbox.setcursel( 0 );
        s = FtEpUnitsCbox.selected_text();
        CurrentParam.parameter[CurrentParameterIndex].units = units_id( s.text() );
        }
    refresh_current_parameter_limits();
    }
}

/***********************************************************************
*                    DO_NEW_EXTERNAL_PARAMETER_UNITS                   *
***********************************************************************/
static void do_new_external_parameter_units()
{
STRING_CLASS s;

s = FtEpUnitsCbox.selected_text();
CurrentParam.parameter[CurrentParameterIndex].units = units_id( s.text() );
refresh_current_parameter_limits();
}

/**********************************************************************
*                UPDATE_EXTERNAL_PARAMETER_SETUP_STRINGS              *
**********************************************************************/
static void update_external_parameter_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { FT_EP_VARTYPE_STATIC, TEXT( "VARIABLE_TYPE" ) },
    { FT_EP_UNITS_STATIC,   TEXT( "UNITS_STRING" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, FT_EP_VARTYPE_CBOX );
set_statics_font( hWnd, FT_EP_UNITS_CBOX   );
}

/***********************************************************************
*                          FT_EXTERNAL_PARAMETER_PROC                  *
***********************************************************************/
BOOL CALLBACK ft_external_parameter_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_external_parameter_setup_strings( hWnd );
        FtEpUnitsCbox.init( hWnd, FT_EP_UNITS_CBOX );
        FtEpVartypeCbox.init( hWnd, FT_EP_VARTYPE_CBOX );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case FT_EP_UNITS_CBOX:
                if ( cmd == LBN_SELCHANGE )
                    do_new_external_parameter_units();
                break;

            case FT_EP_VARTYPE_CBOX:
                if ( cmd == LBN_SELCHANGE )
                    do_new_external_parameter_vartype();
                break;
            }
        break;
    }

return FALSE;
}

/**********************************************************************
*                   UPDATE_STATIC_TEXT_SETUP_STRINGS                  *
**********************************************************************/
static void update_static_text_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { FT_STATIC_TEXT_VALUE_TB,   TEXT( "CURRENT_VALUE" ) },
    { FT_STATIC_TEXT_CREATE_XB,  TEXT( "CREATE_POPUP" ) },
    { FT_STATIC_TEXT_MONITOR_XB, TEXT( "MONITOR_POPUP" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );

set_statics_font( hWnd, FT_STATIC_TEXT_VALUE_EB );
}

/***********************************************************************
*                        FT_STATIC_TEXT_PARAMETER_PROC                 *
***********************************************************************/
BOOL CALLBACK ft_static_text_parameter_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_static_text_setup_strings( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case FT_STATIC_TEXT_VALUE_EB:
                if ( cmd == EN_CHANGE && !LoadingStaticText )
                    CurrentParam.parameter[CurrentParameterIndex].static_text.value.get_text( hWnd, FT_STATIC_TEXT_VALUE_EB );
                break;

            case FT_STATIC_TEXT_CREATE_XB:
                if ( cmd == BN_CLICKED )
                    CurrentParam.parameter[CurrentParameterIndex].static_text.popup_on_create  = is_checked( hWnd, FT_STATIC_TEXT_CREATE_XB );
                break;

            case FT_STATIC_TEXT_MONITOR_XB:
                if ( cmd == BN_CLICKED )
                    CurrentParam.parameter[CurrentParameterIndex].static_text.popup_on_monitor = is_checked( hWnd, FT_STATIC_TEXT_MONITOR_XB );
                break;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                      GET_NEW_FT_ONOFF_SETTINGS                       *
***********************************************************************/
void get_new_ft_onoff_settings()
{
BITSETYPE mask = ~(TRIGGER_WHEN_GREATER | TRIGGER_WHEN_SMALLER );
HWND w;
int  input_number;
int  pi;
BOOL status;

pi = CurrentParameterIndex;
if ( pi < 0 || pi >= MAX_PARMS )
    return;

CurrentParam.parameter[pi].input.type = FT_ON_OFF_INPUT;
CurrentParam.parameter[pi].vartype    = POSITION_VAR;
CurrentParam.parameter[pi].units      = CurrentPart.distance_units;

w = WindowInfo[CurrentDialogNumber].dialogwindow;

/*
----------------------
FasTrak Channel Number
---------------------- */
input_number = GetDlgItemInt( w, ONOFF_INPUT_WIRE_EB, &status, FALSE);

CurrentFtAnalog.array[pi].ind_var_type &= mask;
if ( is_checked(w, ONOFF_ON_BUTTON) )
    CurrentFtAnalog.array[pi].ind_var_type |= TRIGGER_WHEN_GREATER;
else
    CurrentFtAnalog.array[pi].ind_var_type |= TRIGGER_WHEN_SMALLER;

CurrentParam.parameter[pi].input.number = input_number;
}

/**********************************************************************
*                      UPDATE_ONOFF_SETUP_STRINGS                     *
**********************************************************************/
static void update_onoff_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { ONOFF_TRANSITION_TYPE_TB, TEXT( "TRANSITION_TYPE" ) },
    { ONOFF_INPUT_WIRE_TB,      TEXT( "INPUT_WIRE_NUMBER" ) },
    { ONOFF_ON_BUTTON,          TEXT( "OFF_TO_ON" ) },
    { ONOFF_OFF_BUTTON,         TEXT( "ON_TO_OFF" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );
}

/***********************************************************************
*                               FT_ONOFF_PROC                          *
***********************************************************************/
BOOL CALLBACK ft_onoff_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        update_onoff_setup_strings( hWnd );
        return TRUE;

    case WM_COMMAND:
        if ( id >= ONOFF_INPUT_WIRE_EB && id <= ONOFF_OFF_BUTTON )
            {
            if ( !LoadingParams )
                {
                get_new_ft_onoff_settings();
                refresh_current_parameter_limits();
                return TRUE;
                }
            }
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                 INITIALIZE_FT_PEAK_FS_VELOCITY_DIALOG                 *
***********************************************************************/
static void initialize_ft_peak_fs_velocity_dialog( HWND w )
{
static RESOURCE_TEXT_ENTRY rlist[] = {
    { PFSV_TITLE_TBOX,         TEXT("PEAK_FS_VELOCITY") },
    { PFSV_N_TBOX,             TEXT("SMOOTH_N") },
    { PFSV_MULTIPLIER_TBOX,    TEXT("SMOOTH_MULTIPLIER") },
    { PFSV_START_TBOX,         TEXT("START_STRING") },
    { PFSV_END_TBOX,           TEXT("END_STRING") },
    { PFSV_RANGE_TBOX,         TEXT("SAMPLE_RANGE") },
    { PFSV_SMOOTH_TBOX,        TEXT("SMOOTH_SETTINGS") }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( w, rlist, nr );
}

/***********************************************************************
*                         FT_PEAK_FS_VELOCITY_PROC                     *
***********************************************************************/
BOOL CALLBACK ft_peak_fs_velocity_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        initialize_ft_peak_fs_velocity_dialog(hWnd);
        return TRUE;

    case WM_COMMAND:
        if ( !LoadingParams &&  cmd == EN_CHANGE )
            {
            switch ( id )
                {
                case PFSV_N_EBOX:
                    CurrentFtAnalog.array[CurrentParameterIndex].smooth_n.get_text( hWnd, PFSV_N_EBOX );
                    break;
                case PFSV_MULTIPLIER_EBOX:
                    CurrentFtAnalog.array[CurrentParameterIndex].smooth_multiplier.get_text( hWnd, PFSV_MULTIPLIER_EBOX );
                    break;
                case PFSV_START_EBOX:
                    CurrentFtAnalog.array[CurrentParameterIndex].ind_value.get_text( hWnd, PFSV_START_EBOX );
                    break;
                case PFSV_END_EBOX:
                    CurrentFtAnalog.array[CurrentParameterIndex].end_value.get_text( hWnd, PFSV_END_EBOX );
                    break;
                }
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                          LOAD_NEW_PARAMETER                          *
***********************************************************************/
void load_new_parameter( int new_parameter_index )
{
short input_type;
short tab_index;
int   x;

if ( new_parameter_index < 0 || new_parameter_index >= MAX_PARMS )
    return;

input_type = CurrentParam.parameter[new_parameter_index].input.type;

switch ( input_type )
    {
    case INTERNAL_PARAMETER:
        tab_index = INTERNAL_PARAM_TAB;
        break;

    case FT_ANALOG_INPUT:
        tab_index = FT_ANALOG_TAB;
        break;

    case FT_ON_OFF_INPUT:
        tab_index = FT_ONOFF_TAB;
        break;

    case FT_TARGET_INPUT:
        tab_index = FT_TARGET_TAB;
        break;

    case FT_DIGITAL_INPUT:
        tab_index = FT_DIGITAL_TAB;
        break;

    case FT_EXTENDED_ANALOG_INPUT:
        tab_index = FT_EXTENDED_ANALOG_TAB;
        break;

    case FT_PEAK_FS_VELOCITY:
        tab_index = FT_PEAK_FS_VELOCITY_TAB;
        break;

    case FT_EXTERNAL_PARAMETER:
        tab_index = FT_EXTERNAL_PARAMETER_TAB;
        break;

    case FT_STATIC_TEXT_PARAMETER:
        tab_index = FT_STATIC_TEXT_PARAMETER_TAB;
        break;

    default:
        tab_index = FT_CALC_TAB;
        break;
    }

x = INTERNAL_PARAMETER_STRING + tab_index;
set_text( ParamDialogWindow, PARAMETER_TYPE_TEXTBOX,  resource_string(x) );
set_text( ParamDialogWindow, PARAMETER_NUMBER_TBOX,   int32toasc((int32) new_parameter_index+1) );

CurrentParameterIndex = new_parameter_index;

LoadingParams = TRUE;

if ( IsWindowVisible(ParamDialogWindow) )
    {
    if ( CurrentDialogNumber != tab_index )
        show_new_dialog( tab_index );
    else
        update_current_dialog();
    }

SetDlgItemText( ParamDialogWindow, PARAMETER_DESC_EDITBOX, CurrentParam.parameter[new_parameter_index].name );

CurrentParam.parameter[new_parameter_index].limits[WARNING_MIN].svalue.set_text( ParamDialogWindow, LOW_WARN_VALUE_EDITBOX );
CurrentParam.parameter[new_parameter_index].limits[WARNING_MAX].svalue.set_text( ParamDialogWindow, HIGH_WARN_VALUE_EDITBOX );

CurrentParam.parameter[new_parameter_index].limits[ALARM_MIN].svalue.set_text( ParamDialogWindow, LOW_ALARM_VALUE_EDITBOX );
CurrentParam.parameter[new_parameter_index].limits[ALARM_MAX].svalue.set_text( ParamDialogWindow, HIGH_ALARM_VALUE_EDITBOX );

set_text( ParamDialogWindow, LOW_WARN_WIRE_PB,   int32toasc((int32) CurrentParam.parameter[new_parameter_index].limits[WARNING_MIN].wire_number) );
set_text( ParamDialogWindow, HIGH_WARN_WIRE_PB,  int32toasc((int32) CurrentParam.parameter[new_parameter_index].limits[WARNING_MAX].wire_number) );
set_text( ParamDialogWindow, LOW_ALARM_WIRE_PB,  int32toasc((int32) CurrentParam.parameter[new_parameter_index].limits[ALARM_MIN].wire_number) );
set_text( ParamDialogWindow, HIGH_ALARM_WIRE_PB, int32toasc((int32) CurrentParam.parameter[new_parameter_index].limits[ALARM_MAX].wire_number) );

CurrentParam.parameter[new_parameter_index].target.set_text( ParamDialogWindow, TARGET_VALUE_EDITBOX  );

LoadingParams = FALSE;
}

/***********************************************************************
*                       CLEAR_PARAMETER_SETTINGS                       *
***********************************************************************/
void clear_parameter_settings()
{
short i;
for ( i=0; i<NOF_ALARM_LIMIT_TYPES; i++ )
    {
    CurrentParam.parameter[CurrentParameterIndex].limits[i].wire_number = NO_WIRE;
    CurrentParam.parameter[CurrentParameterIndex].limits[i].value       = 0.0;
    CurrentParam.parameter[CurrentParameterIndex].limits[i].svalue.empty();
    }
refresh_current_parameter_limits();
load_new_parameter( CurrentParameterIndex );
}

/***********************************************************************
*                            FILL_VARTYPES                             *
***********************************************************************/
static void fill_vartypes()
{
if ( Vartypes.count() < 1 )
    return;

VartypeCbox.empty();
FtEpVartypeCbox.empty();
while ( Vartypes.next() )
    {
    VartypeCbox.add( Vartypes.name().text() );
    FtEpVartypeCbox.add( Vartypes.name().text() );
    }

VartypeCbox.set_current_index( 0 );
FtEpVartypeCbox.set_current_index( 0 );
}

/***********************************************************************
*                        CREATE_PARAM_DIALOGS                          *
***********************************************************************/
void create_param_dialogs()
{
int i;

WindowInfo[INTERNAL_PARAM_TAB].procptr           = (DLGPROC) internal_param_proc;
WindowInfo[FT_ANALOG_TAB].procptr                = (DLGPROC) ft_analog_proc;
WindowInfo[FT_ONOFF_TAB].procptr                 = (DLGPROC) ft_onoff_proc;
WindowInfo[FT_TARGET_TAB].procptr                = (DLGPROC) ft_target_proc;
WindowInfo[FT_DIGITAL_TAB].procptr               = (DLGPROC) ft_digital_proc;
WindowInfo[FT_CALC_TAB].procptr                  = (DLGPROC) ft_calc_proc;
WindowInfo[FT_EXTENDED_ANALOG_TAB].procptr       = (DLGPROC) ft_extended_analog_proc;
WindowInfo[FT_PEAK_FS_VELOCITY_TAB].procptr      = (DLGPROC) ft_peak_fs_velocity_proc;
WindowInfo[FT_EXTERNAL_PARAMETER_TAB].procptr    = (DLGPROC) ft_external_parameter_proc;
WindowInfo[FT_STATIC_TEXT_PARAMETER_TAB].procptr = (DLGPROC) ft_static_text_parameter_proc;
for ( i=0; i<TAB_COUNT; i++ )
    {
    WindowInfo[i].dialogwindow = CreateDialog(
        MainInstance,
        ParamDialogName[i],
        ParamDialogWindow,
        WindowInfo[i].procptr );

    ShowWindow( WindowInfo[i].dialogwindow, SW_HIDE );
    }
}

/**********************************************************************
*                      UPDATE_PARAM_SETUP_STRINGS                     *
**********************************************************************/
static void update_parameter_limits_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { CLEAR_BUTTON,        TEXT( "CLEAR_STRING" ) },
    { PARAMETER_TYPE_GB,   TEXT( "PARAMETER_TYPE" ) },
    { PARAMETER_NUMBER_GB, TEXT( "NUMBER_STRING" ) },
    { TARGET_VALUE_TB,     TEXT( "TARGET_STRING" ) },
    { HIGH_WARNING_TB,     TEXT( "HIGH_WARNING" ) },
    { LOW_WARNING_TB,      TEXT( "LOW_WARNING" ) },
    { LOW_ALARM_TB,        TEXT( "LOW_ALARM" ) },
    { HIGH_ALARM_TB,       TEXT( "HIGH_ALARM" ) },
    { VALUE_TB,            TEXT( "VALUE_STRING" ) },
    { ALARM_WIRE_TB,       TEXT( "ALARM_WIRE" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rlist, nr );


set_statics_font( hWnd, PARAMETER_DESC_EDITBOX );
set_statics_font( hWnd, PARAMETER_TYPE_TEXTBOX );
}

/***********************************************************************
*                            ParamDialogProc                           *
***********************************************************************/
BOOL CALLBACK ParamDialogProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int            id;
int            cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        update_parameter_limits_setup_strings( hWnd );
        Vartypes.get();
        create_param_dialogs();
        fill_vartypes();
        set_text( hWnd, PARAMETER_NUMBER_TBOX, EmptyString );
        return TRUE;

    case WM_MOVE:
        position_current_dialog();
        return TRUE;

    case WM_SHOWWINDOW:
        if ( (BOOL) wParam )
            show_current_dialog();
        else
            hide_current_dialog();
        return TRUE;

    case WM_COMMAND:
        if ( cmd == EN_CHANGE && !LoadingParams )
            {
            get_new_parameter_settings();
            return TRUE;
            }

        switch ( id )
            {
            case CLEAR_BUTTON:
                clear_parameter_settings();
                return TRUE;

            case OK_BUTTON:
                get_new_parameter_settings();
                return TRUE;

            case CANCEL_BUTTON:
                ShowWindow( ParamDialogWindow, SW_HIDE );
                return_from_parameter_edit();
                return TRUE;

            case LOW_WARN_WIRE_PB:
            case HIGH_WARN_WIRE_PB:
            case LOW_ALARM_WIRE_PB:
            case HIGH_ALARM_WIRE_PB:
                if ( choose_wire( GetDlgItem(hWnd, id), hWnd, ALARM_OUTPUT_TYPE, (int) CurrentParameterIndex, &CurrentMachine, &CurrentPart, &CurrentParam, &CurrentLimitSwitch) )
                    get_new_parameter_settings();
                return TRUE;

            /*-----------------------------------------------------
            case INTERNAL_PARAM_RADIO_BUTTON:
            case FT_ANALOG_RADIO_BUTTON:
            case FT_ONOFF_RADIO_BUTTON:
                switch ( id )
                    {
                    case INTERNAL_PARAM_RADIO_BUTTON:
                        get_new_internal_parameter_settings();
                        break;

                    case FT_ANALOG_RADIO_BUTTON:
                        get_new_ft_analog_settings();
                        break;

                    case FT_ONOFF_RADIO_BUTTON:
                        get_new_ft_onoff_settings();
                        break;
                    }
                i = id - INTERNAL_PARAM_RADIO_BUTTON;
                show_new_dialog( i );
                refresh_current_parameter_limits();
                return TRUE;
            -------------------------------------------- */
            }
        break;

    }

return FALSE;
}
