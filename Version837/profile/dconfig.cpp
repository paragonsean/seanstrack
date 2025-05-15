#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chaxis.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\machine.h"
#include "..\include\part.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "extern.h"
#include "resource.h"
#include "fixaxis.h"

#define PRESSURE_STRING 20
#define CHANNEL_STRING  28

TCHAR * resource_string( UINT resource_id );

static HWND MyWindowHandle;
static BITSETYPE PreChannelTypes;
static BITSETYPE PostChannelTypes;
static BITSETYPE PrePressureTypes;
static BITSETYPE PostPressureTypes;

static TCHAR PRE_PRES_CONFIG[]  = TEXT( "PresPreConfig"  );
static TCHAR POST_PRES_CONFIG[] = TEXT( "PresPostConfig" );
static TCHAR PRE_CHAN_CONFIG[]  = TEXT( "ChanPreConfig"  );
static TCHAR POST_CHAN_CONFIG[] = TEXT( "ChanPostConfig" );
static TCHAR IsPresDisplayKey[] = TEXT( "IsPressureDisplay" );
static TCHAR NChar              = TEXT( 'N' );
static TCHAR NString[]          = TEXT( "N" );
static TCHAR NullChar           = TEXT( '\0' );
static TCHAR YChar              = TEXT( 'Y' );
static TCHAR YString[]          = TEXT( "Y" );
static TCHAR EmptyString[]      = TEXT( "" );

static BITSETYPE CurveTypesToCheck[] = {
    CHAN_1_ANALOG_CURVE,
    CHAN_2_ANALOG_CURVE,
    CHAN_3_ANALOG_CURVE,
    CHAN_4_ANALOG_CURVE,
    CHAN_5_ANALOG_CURVE,
    CHAN_6_ANALOG_CURVE,
    CHAN_7_ANALOG_CURVE,
    CHAN_8_ANALOG_CURVE,
    DIFF_PRESSURE_CURVE
    };

const int NofCurveTypesToCheck = sizeof(CurveTypesToCheck)/sizeof(BITSETYPE);

/*
-------------------------------------------------------------------
The bits are not in the same order as humans want to see the curves
I need to check them in the bit order as follows.
------------------------------------------------------------------- */
static int CurveTypesCheckOrder[NofCurveTypesToCheck] = { 0, 1, 2, 3, 8, 4, 5, 6, 7 };

class FIXED_SCREEN_CLASS : public FIXED_AXIS_CLASS
    {
    public:
    void put_to_screen();
    void get_from_screen();
    void set_one_fixed_axis( int32 dest_axis, int32 sorc_index );
    void set_fixed_axes();
    };

static FIXED_SCREEN_CLASS FixedPressures;
static FIXED_SCREEN_CLASS FixedChannels;

static INI_CLASS Ini;

void set_smooth( int32 n, float factor );

/***********************************************************************
*                          FIXED_SCREEN_CLASS                          *
*                            PUT_TO_SCREEN                             *
***********************************************************************/
void FIXED_SCREEN_CLASS::put_to_screen()
{
int32 i;
int32 channel;
UINT end_box;
UINT tbox;
UINT xbox;
UINT min_ebox;
UINT max_ebox;
STRING_CLASS s;

if ( n < 1 )
    return;

/*
---------------------------------
Fill in the names of the channels
--------------------------------- */
channel = 1;
for ( tbox=CHANNEL_1_TBOX; tbox<VELOCITY_TBOX; tbox++ )
    {
    if ( channel == CurrentPart.head_pressure_channel )
        s = resource_string( HEAD_PRES_STRING );
    else if ( channel == CurrentPart.rod_pressure_channel )
        s = resource_string( ROD_PRES_STRING );
    else
        s = ChannelAxisLabel[channel-1];
    s.set_text( MyWindowHandle, tbox );
    channel++;
    }

s = resource_string( DIF_PRES_STRING );
s.set_text( MyWindowHandle, DIF_CHAN_TBOX );

xbox     = FIX_FTCHAN_1_XBOX;
min_ebox = MIN_FTCHAN_1_EBOX;
max_ebox = MAX_FTCHAN_1_EBOX;
end_box  = FIX_VELOCITY_XBOX;

for ( i=0; i<n; i++ )
    {
    set_checkbox( MyWindowHandle, xbox, f[i].enabled );
    set_text( MyWindowHandle, min_ebox, ascii_float( f[i].min) );
    set_text( MyWindowHandle, max_ebox, ascii_float( f[i].max) );

    xbox++;
    min_ebox++;
    max_ebox++;
    }
}

/***********************************************************************
*                          FIXED_SCREEN_CLASS                          *
*                           GET_FROM_SCREEN                            *
***********************************************************************/
void FIXED_SCREEN_CLASS::get_from_screen()
{
TCHAR   buf[MAX_FLOAT_LEN+1];

UINT    xbox;
UINT    min_ebox;
UINT    max_ebox;

int32   i;
const int32 nof_boxes = FIX_VELOCITY_XBOX - FIX_FTCHAN_1_XBOX + 1;

empty();

if ( !init(nof_boxes) )
    return;

xbox     = FIX_FTCHAN_1_XBOX;
min_ebox = MIN_FTCHAN_1_EBOX;
max_ebox = MAX_FTCHAN_1_EBOX;

for ( i=0; i<nof_boxes; i++ )
    {
    f[i].enabled = is_checked( MyWindowHandle, xbox );

    get_text( buf, MyWindowHandle, min_ebox, MAX_FLOAT_LEN );
    f[i].min = extfloat( buf );

    get_text( buf, MyWindowHandle, max_ebox, MAX_FLOAT_LEN );
    f[i].max = extfloat( buf );

    xbox++;
    min_ebox++;
    max_ebox++;
    }
}

/***********************************************************************
*                          FIXED_SCREEN_CLASS                          *
*                          SET_ONE_FIXED_AXIS                          *
***********************************************************************/
void FIXED_SCREEN_CLASS::set_one_fixed_axis( int32 dest_axis, int32 sorc_index )
{

if ( sorc_index<0 || sorc_index >= n )
    {
    Axis[dest_axis].is_fixed = FALSE;
    Axis[dest_axis].min = 0.0;
    Axis[dest_axis].max = 0.0;
    return;
    }

Axis[dest_axis].is_fixed = f[sorc_index].enabled;
if ( Axis[dest_axis].is_fixed )
    {
    Axis[dest_axis].min = f[sorc_index].min;
    Axis[dest_axis].max = f[sorc_index].max;
    }
}

/***********************************************************************
*                          FIXED_SCREEN_CLASS                          *
*                            SET_FIXED_AXES                            *
***********************************************************************/
void FIXED_SCREEN_CLASS::set_fixed_axes()
{
int32 axis_index;
int32 fixed_index;
int32 n;

UINT  pre_button;
UINT  post_button;

for ( axis_index=0; axis_index<NOF_AXES; axis_index++ )
    Axis[axis_index].is_fixed = FALSE;

/*
---------------------------------------------------------
Only call this after setting the global IsPressureDisplay
--------------------------------------------------------- */
pre_button  = PRE_CHANNEL_1_XBOX;
post_button = POST_CHANNEL_1_XBOX;
axis_index  = FIRST_ANALOG_AXIS;
fixed_index = 0;

n = NOF_FTII_ANALOGS;

while ( axis_index< NOF_AXES && fixed_index < n )
    {
    /*
    ------------------------------------------------------------------
    The axes are not 1 to 1 with the curves.
    If a curve is not displayed, that axis is used for the next curve.
    ------------------------------------------------------------------ */
    if ( is_checked(MyWindowHandle, pre_button) || is_checked(MyWindowHandle, post_button) )
        {
        set_one_fixed_axis( axis_index, fixed_index );
        axis_index++;
        }
    fixed_index++;
    pre_button++;
    post_button++;
    }

set_one_fixed_axis( VELOCITY_AXIS, FIXAXIS_VELOCITY_INDEX  );
}

/***********************************************************************
*                           LOAD_ONE_CONFIG                            *
*          The file and section should already be set in ini.          *
***********************************************************************/
static void load_one_config( BITSETYPE & b, TCHAR * key )
{
TCHAR * cp;

b = 0;
if ( Ini.find(key) )
    {
    cp = Ini.get_string();
    b = (BITSETYPE) extlong( cp, 4, HEX_RADIX );
    }
}

/***********************************************************************
*                           SAVE_ONE_CONFIG                            *
***********************************************************************/
static void save_one_config( TCHAR * key, BITSETYPE b )
{
TCHAR s[5];

insalph( s,  int32(b), 4, TEXT('0'), HEX_RADIX );
s[4] = TEXT( '\0' );

Ini.put_string( key, s );
}

/***********************************************************************
*                     GET_CURVE_TYPES_FROM_SCREEN                      *
***********************************************************************/
void get_curve_types_from_screen( BITSETYPE & dest, int button )
{
int i;

dest = 0;
for ( i=0; i<NofCurveTypesToCheck; i++ )
    {
    if ( is_checked(MyWindowHandle, button) )
        {
        dest |= CurveTypesToCheck[i];

        if ( (i+1) == (int)CurrentPart.head_pressure_channel )
            dest |= HEAD_PRESSURE_CURVE;

        if ( (i+1) == (int)CurrentPart.rod_pressure_channel )
            dest |= ROD_PRESSURE_CURVE;
        }
    button++;
    }
}

/***********************************************************************
*                    SAVE_SMOOTH_CONFIG_FROM_SCREEN                    *
***********************************************************************/
static void save_smooth_config_from_screen()
{
const int nof_channels = 1 + SMOOTH_P8_XBOX - SMOOTH_P1_XBOX;
STRING_CLASS s;
int32 n;
int   id;
int   i;

s.upsize( nof_channels );

/*
------------------------------------------
Smooth factors are in the display.ini file
------------------------------------------ */
s.get_text( MyWindowHandle, SMOOTH_N_EBOX );
n = s.int_value();
Ini.put_string( SmoothNKey, s.text() );

s.get_text( MyWindowHandle, SMOOTH_MULTIPLIER_EBOX );
Ini.put_string( SmoothFactorKey, s.text() );

set_smooth( n, s.float_value() );
if ( n > 0 )
    Smoothing = TRUE;
else
    Smoothing = FALSE;

s = EmptyString;

id = SMOOTH_P1_XBOX;
for ( i=0; i<nof_channels; i++ )
    {
    SmoothingChannel[i]= is_checked( MyWindowHandle, id );
    if ( SmoothingChannel[i] )
        s += YString;
    else
        s += NString;
    id++;
    }

SmoothingVelocity= is_checked( MyWindowHandle, SMOOTH_VELOCITY_XBOX );
if ( SmoothingVelocity )
    s += YString;
else
    s += NString;

Ini.put_string( SmoothCurvesKey, s.text() );
}

/***********************************************************************
*                             LOAD_SMOOTH                              *
***********************************************************************/
static void load_smooth( BOOLEAN need_to_update_screen )
{
const int nof_channels = 1 + SMOOTH_P8_XBOX - SMOOTH_P1_XBOX;

HWND    w;
TCHAR * cp;
float factor;
int32 n;
int   i;
int   id;
STRING_CLASS smooth_n;
STRING_CLASS smoothfactor;

if ( need_to_update_screen )
    {
    smooth_n.upsize( nof_channels );
    smoothfactor.upsize( 20 );
    smooth_n = EmptyString;
    smoothfactor = EmptyString;
    }
n = 0;
factor = 1.00;
if ( Ini.find(SmoothNKey) )
    {
    smooth_n = Ini.get_string();
    n = smooth_n.int_value();
    }

if ( Ini.find(SmoothFactorKey) )
    {
    smoothfactor = Ini.get_string();
    factor = smoothfactor.float_value();
    }

if ( n > 0 )
    {
    set_smooth( n, factor );
    Smoothing = TRUE;
    }
else
    {
    Smoothing = FALSE;
    }

if ( need_to_update_screen )
    {
    smoothfactor.set_text( MyWindowHandle, SMOOTH_MULTIPLIER_EBOX );
    smooth_n.set_text( MyWindowHandle, SMOOTH_N_EBOX );
    }

for ( i=0; i<nof_channels; i++ )
    SmoothingChannel[i] = FALSE;
SmoothingVelocity = FALSE;

if ( Ini.find(SmoothCurvesKey) )
    {
    cp = Ini.get_string();
    for ( i=0; i<NOF_DYNAMIC_CHANNELS; i++ )
        {
        if ( cp[i] == YChar )
            SmoothingChannel[i] = TRUE;
        else if ( cp[i] == NullChar )
            break;
        }

    if ( cp[i] == YChar )
        SmoothingVelocity = TRUE;
    }

if ( need_to_update_screen )
    {
    id = SMOOTH_P1_XBOX;
    for ( i=0; i<NOF_DYNAMIC_CHANNELS; i++ )
        {
        w = GetDlgItem( MyWindowHandle, id );
        set_checkbox( w, SmoothingChannel[i] );
        id++;
        }

    w = GetDlgItem( MyWindowHandle, SMOOTH_VELOCITY_XBOX );
    set_checkbox( w, SmoothingVelocity );
    }
}

/***********************************************************************
*                           SAVE_THIS_CONFIG                           *
***********************************************************************/
static void save_this_config( int id )
{
COMPUTER_CLASS c;
NAME_CLASS s;
INI_CLASS  ini;
int        i;
BOOLEAN    is_newdir;

is_newdir = FALSE;

/*
------------------------------------------
Id is the id of the button that was pushed
------------------------------------------ */
if ( id == SAVE_DEFAULT_DISPLAY_CONFIG_PB )
    {
    s.get_exe_dir_file_name( 0 );
    }
else
    {
    s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, 0 );
    if ( !s.directory_exists() )
        {
        if ( !s.create_directory() )
            {
            MessageBox( 0, s.text(), resource_string(CANT_CREATE_DEST_DIR_STRING), MB_OK | MB_SYSTEMMODAL | MB_ICONERROR );
            return;
            }
        is_newdir = TRUE;
        }
    }

s.cat_path( DisplayIniFile );
Ini.set_file( s.text() );
Ini.set_section( ConfigSection );

if ( is_checked(MyWindowHandle, PRESSURES_RADIO_BUTTON) )
    {
    get_curve_types_from_screen( PreImpactPlotCurveTypes,  PRE_CHANNEL_1_XBOX );
    get_curve_types_from_screen( PostImpactPlotCurveTypes, POST_CHANNEL_1_XBOX );
    save_one_config( PRE_PRES_CONFIG,  PreImpactPlotCurveTypes );
    save_one_config( POST_PRES_CONFIG, PostImpactPlotCurveTypes );
    if ( is_newdir )
        {
        save_one_config( PRE_CHAN_CONFIG, PreChannelTypes );
        save_one_config( POST_CHAN_CONFIG, PostChannelTypes );
        }
    FixedPressures.get_from_screen();
    IsPressureDisplay = TRUE;
    FixedPressures.set_fixed_axes();
    }
else
    {
    get_curve_types_from_screen( PreImpactPlotCurveTypes,  PRE_CHANNEL_1_XBOX );
    get_curve_types_from_screen( PostImpactPlotCurveTypes, POST_CHANNEL_1_XBOX );
    save_one_config( PRE_CHAN_CONFIG,  PreImpactPlotCurveTypes );
    save_one_config( POST_CHAN_CONFIG, PostImpactPlotCurveTypes );
    if ( is_newdir )
        {
        save_one_config( PRE_PRES_CONFIG,  PrePressureTypes );
        save_one_config( POST_PRES_CONFIG, PostPressureTypes );
        }
    FixedChannels.get_from_screen();
    IsPressureDisplay = FALSE;
    FixedChannels.set_fixed_axes();
    }

Ini.put_boolean( IsPresDisplayKey, IsPressureDisplay );

if ( id == SAVE_DEFAULT_DISPLAY_CONFIG_PB )
    {
    s.get_data_dir_file_name( FIXPRES_DB );
    FixedPressures.write( s.text() );
    s.get_data_dir_file_name( FIXCHAN_DB );
    FixedChannels.write(  s.text() );
    }
else
    {
    s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, FIXPRES_DB );
    FixedPressures.write( s.text() );
    s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, FIXCHAN_DB );
    FixedChannels.write( s.text() );
    }

save_smooth_config_from_screen();
s.get_text( MyWindowHandle, CURVE_PEN_WIDTH_EBOX );
i = s.int_value();
if ( i > 0 )
    {
    CurvePenWidth = i;
    Ini.put_string( CurvePenWidthKey, s.text() );
    }
}

/***********************************************************************
*                        LOAD_THE_OTHER_CONFIG                         *
***********************************************************************/
void load_the_other_config()
{
if ( is_checked(MyWindowHandle, PRESSURES_RADIO_BUTTON) )
    {
    load_one_config(  PreChannelTypes,  PRE_CHAN_CONFIG  );
    load_one_config(  PostChannelTypes, POST_CHAN_CONFIG );
    }
else
    {
    load_one_config( PrePressureTypes,  PRE_PRES_CONFIG  );
    load_one_config( PostPressureTypes, POST_PRES_CONFIG );
    }
}

/***********************************************************************
*                       READ_THE_FIXED_PRESSURES                       *
***********************************************************************/
static void read_the_fixed_pressures()
{
COMPUTER_CLASS c;
NAME_CLASS     s;

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, FIXPRES_DB );
if ( !s.file_exists() )
    s.get_data_dir_file_name( FIXPRES_DB );
FixedPressures.read( s.text() );
}

/***********************************************************************
*                       READ_THE_FIXED_CHANNELS                       *
***********************************************************************/
static void read_the_fixed_channels()
{
COMPUTER_CLASS c;
NAME_CLASS     s;

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, FIXCHAN_DB );
if ( !s.file_exists() )
    s.get_data_dir_file_name( FIXCHAN_DB );
FixedChannels.read( s.text() );
}

/***********************************************************************
*                       INIT_GLOBAL_CURVE_TYPES                        *
*                                                                      *
*    This is called by profile at startup and any time the config      *
*    changes, either by toggling the display type or by loading a      *
*    different machine.                                                *
***********************************************************************/
void init_global_curve_types()
{
int       axis_index;
int       fixed_index;
int       i;

BITSETYPE type_mask;
COMPUTER_CLASS c;
NAME_CLASS     s;
BOOLEAN        using_machine_config;

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, DisplayIniFile );
using_machine_config = s.file_exists();
if ( !using_machine_config )
    s.get_exe_dir_file_name( DisplayIniFile );

Ini.set_file( s.text() );
Ini.set_section( ConfigSection );

/*
----------------------------
Get the current display type
---------------------------- */
IsPressureDisplay = Ini.get_boolean( IsPresDisplayKey );

axis_index  = FIRST_ANALOG_AXIS;
i = 0;

if ( IsPressureDisplay )
    {
    load_one_config( PreImpactPlotCurveTypes,  PRE_PRES_CONFIG  );
    load_one_config( PostImpactPlotCurveTypes, POST_PRES_CONFIG );

    read_the_fixed_pressures();

    type_mask = PreImpactPlotCurveTypes | PostImpactPlotCurveTypes;
    while ( axis_index < NOF_AXES && i < NofCurveTypesToCheck )
        {
        fixed_index = CurveTypesCheckOrder[i];
        if ( type_mask & CurveTypesToCheck[fixed_index] )
            {
            FixedPressures.set_one_fixed_axis( axis_index, fixed_index );
            axis_index++;
            }
        i++;
        }
    FixedPressures.set_one_fixed_axis( VELOCITY_AXIS, FIXAXIS_VELOCITY_INDEX );
    FixedPressures.empty();
    }
else
    {
    load_one_config( PreImpactPlotCurveTypes,  PRE_CHAN_CONFIG  );
    load_one_config( PostImpactPlotCurveTypes, POST_CHAN_CONFIG );

    read_the_fixed_channels();

    type_mask = PreImpactPlotCurveTypes | PostImpactPlotCurveTypes;
    while ( axis_index < NOF_AXES && i < NofCurveTypesToCheck )
        {
        fixed_index = CurveTypesCheckOrder[i];
        if ( type_mask & CurveTypesToCheck[fixed_index] )
            {
            FixedChannels.set_one_fixed_axis( axis_index, fixed_index );
            axis_index++;
            }
        i++;
        }
    FixedChannels.set_one_fixed_axis( VELOCITY_AXIS, FIXAXIS_VELOCITY_INDEX );
    FixedChannels.empty();
    }

load_smooth( FALSE );

if ( Ini.find(CurvePenWidthKey) )
    CurvePenWidth = Ini.get_int();

Ini.cleanup();
}

/***********************************************************************
*                     TOGGLE_PRESSURE_DISPLAY_TYPE                     *
***********************************************************************/
void toggle_pressure_display_type()
{
BOOLEAN        b;
COMPUTER_CLASS c;
NAME_CLASS     s;

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, DisplayIniFile );
if ( !s.file_exists() )
    s.get_exe_dir_file_name( DisplayIniFile );

Ini.set_file( s.text() );
Ini.set_section( ConfigSection );

/*
----------------------------
Get the current display type
---------------------------- */
IsPressureDisplay = Ini.get_boolean( IsPresDisplayKey );

b = TRUE;
if ( IsPressureDisplay )
    b = FALSE;

Ini.put_boolean( IsPresDisplayKey, b );
init_global_curve_types();
}

/***********************************************************************
*                        CHECK_ONE_SET_OF_BUTTONS                      *
***********************************************************************/
static void check_one_set_of_buttons( BITSETYPE pressure_type, int button )
{
int          channel;
int          i;
UINT         id;
BOOLEAN      is_set;
STRING_CLASS s;
HWND         w;

id = CHANNEL_STRING;

for ( i=0; i<NofCurveTypesToCheck; i++ )
    {
    w = GetDlgItem( MyWindowHandle, button );
    channel = i+1;

    if ( i == (NofCurveTypesToCheck-1) )
        {
        StringTable.find( s, TEXT("DIF_PRES_STRING") );
        }
    else if ( channel == CurrentPart.head_pressure_channel )
        {
        StringTable.find( s, TEXT("HEAD_PRES_STRING") );
        if ( pressure_type & HEAD_PRESSURE_CURVE )
            pressure_type |= CurveTypesToCheck[i];
        }
    else if ( channel == CurrentPart.rod_pressure_channel )
        {
        StringTable.find( s, TEXT("ROD_PRES_STRING") );
        if ( pressure_type & ROD_PRESSURE_CURVE )
            pressure_type |= CurveTypesToCheck[i];
        }
    else
        {
        s = resource_string( id );
        }

    s.set_text( w );
    id++;
    if ( pressure_type & CurveTypesToCheck[i] )
        is_set = TRUE;
    else
        is_set = FALSE;
    set_checkbox( w, is_set );
    button++;
    }
}

/***********************************************************************
*                      CHECK_THE_CHANNEL_BUTTONS                       *
***********************************************************************/
static void check_the_channel_buttons()
{
check_one_set_of_buttons( PreChannelTypes,  PRE_CHANNEL_1_XBOX  );
check_one_set_of_buttons( PostChannelTypes, POST_CHANNEL_1_XBOX );
}

/***********************************************************************
*                      CHECK_THE_PRESSURE_BUTTONS                      *
***********************************************************************/
static void check_the_pressure_buttons( void )
{
check_one_set_of_buttons( PrePressureTypes,  PRE_CHANNEL_1_XBOX  );
check_one_set_of_buttons( PostPressureTypes, POST_CHANNEL_1_XBOX );
}

/***********************************************************************
*                             INIT_CHOICES                             *
***********************************************************************/
static void init_choices()
{
int       my_radio_button;
COMPUTER_CLASS c;
NAME_CLASS     s;

read_the_fixed_pressures();
read_the_fixed_channels();

s.get_display_dir_file_name( c.whoami(), CurrentMachine.name, DisplayIniFile );
if ( !s.file_exists() )
    s.get_exe_dir_file_name( DisplayIniFile );

Ini.set_file( s.text() );
Ini.set_section( ConfigSection );

if ( IsPressureDisplay )
    {
    PrePressureTypes  = PreImpactPlotCurveTypes;
    PostPressureTypes = PostImpactPlotCurveTypes;
    my_radio_button   = PRESSURES_RADIO_BUTTON;
    check_the_pressure_buttons();
    }
else
    {
    PreChannelTypes  = PreImpactPlotCurveTypes;
    PostChannelTypes = PostImpactPlotCurveTypes;
    my_radio_button  = CHANNELS_RADIO_BUTTON;
    check_the_channel_buttons();
    }

CheckRadioButton( MyWindowHandle, PRESSURES_RADIO_BUTTON, CHANNELS_RADIO_BUTTON, my_radio_button );
load_the_other_config();

if ( my_radio_button == PRESSURES_RADIO_BUTTON )
    FixedPressures.put_to_screen();
else
    FixedChannels.put_to_screen();

load_smooth( TRUE );

if ( Ini.find(CurvePenWidthKey) )
    s = Ini.get_string();
else
    s = int32toasc( CurvePenWidth );

s.set_text( MyWindowHandle, CURVE_PEN_WIDTH_EBOX );

Ini.cleanup();
}

/***********************************************************************
*                   SET_DISPLAY_CONFIG_SETUP_STRINGS                   *
***********************************************************************/
static void set_display_config_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { PRESSURES_RADIO_BUTTON,         TEXT("PRESSURES") },
    { CHANNELS_RADIO_BUTTON,          TEXT("CONTROL_CHANNELS") },
    { PRES_TYPE_GBOX,                 TEXT("DISPLAY_CURVES") },
    { SAVE_DEFAULT_DISPLAY_CONFIG_PB, TEXT("SAVE_DEFAULT") },
    { IDOK,                           TEXT("OK_STRING") },
    { DIF_CHAN_TBOX,                  TEXT("DIF_PRES_STRING") },
    { VELOCITY_TBOX,                  TEXT("VELOCITY_STRING") },
    { CHAN_TO_DISP_STATIC,            TEXT("CHAN_TO_DISP") },
    { FIX_RANGE_STATIC,               TEXT("FIX_RANGE") },
    { MIN_STATIC,                     TEXT("MIN_STRING") },
    { MAX_STATIC,                     TEXT("MAX_STRING") },
    { AXIS_LIMITS_STATIC,             TEXT("AXIS_LIMITS") },
    { FIX_LIMITS_STATIC,              TEXT("FIX_LIMITS") },
    { SMOOTH_STATIC,                  TEXT("SMOOTH") },
    { SMOOTHING_STATIC,               TEXT("SMOOTHING") },
    { NOF_SMOOTH_POINTS_STATIC,       TEXT("NOF_POINTS") },
    { SMOOTH_MULTIPLIER_STATIC,       TEXT("MULTIPLIER") },
    { CHANNEL_STATIC,                 TEXT("CHANNEL_STRING") },
    { VEL_STATIC,                     TEXT("VEL_STRING") },
    { CURVE_WIDTH_STATIC,             TEXT("CURVE_WIDTH") },
    { POST_IMPACT_STATIC,             TEXT("POST_IMPACT") },
    { PRE_IMPACT_STATIC,              TEXT("PRE_IMPACT") },
    { CHAN_DESC_STATIC,               TEXT("CHAN_DESC") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    CHANNEL_1_TBOX,
    CHANNEL_2_TBOX,
    CHANNEL_3_TBOX,
    CHANNEL_4_TBOX,
    CHANNEL_5_TBOX,
    CHANNEL_6_TBOX,
    CHANNEL_7_TBOX,
    CHANNEL_8_TBOX,
    MIN_FTCHAN_1_EBOX,
    MIN_FTCHAN_2_EBOX,
    MIN_FTCHAN_3_EBOX,
    MIN_FTCHAN_4_EBOX,
    MIN_FTCHAN_5_EBOX,
    MIN_FTCHAN_6_EBOX,
    MIN_FTCHAN_7_EBOX,
    MIN_FTCHAN_8_EBOX,
    MIN_DIF_CHAN_EBOX,
    MIN_VELOCITY_EBOX,
    MAX_FTCHAN_1_EBOX,
    MAX_FTCHAN_2_EBOX,
    MAX_FTCHAN_3_EBOX,
    MAX_FTCHAN_4_EBOX,
    MAX_FTCHAN_5_EBOX,
    MAX_FTCHAN_6_EBOX,
    MAX_FTCHAN_7_EBOX,
    MAX_FTCHAN_8_EBOX,
    MAX_DIF_CHAN_EBOX,
    MAX_VELOCITY_EBOX,
    SMOOTH_N_EBOX,
    SMOOTH_MULTIPLIER_EBOX,
    CURVE_PEN_WIDTH_EBOX,
    C1_STATIC,
    C2_STATIC,
    C3_STATIC,
    C4_STATIC,
    C5_STATIC,
    C6_STATIC,
    C7_STATIC,
    C8_STATIC
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    StringTable.find( s, rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("DISPLAY_SETUP")) );
w.refresh();
}

/***********************************************************************
*                  DISPLAY_CONFIGURATION_DIALOG_PROC                   *
***********************************************************************/
BOOL CALLBACK display_configuration_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MyWindowHandle = hWnd;
        set_display_config_setup_strings( hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        init_choices();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CHANNELS_RADIO_BUTTON:
                get_curve_types_from_screen( PrePressureTypes,  PRE_CHANNEL_1_XBOX );
                get_curve_types_from_screen( PostPressureTypes, POST_CHANNEL_1_XBOX );
                check_the_channel_buttons();
                FixedPressures.get_from_screen();
                FixedChannels.put_to_screen();
                return FALSE;

            case PRESSURES_RADIO_BUTTON:
                get_curve_types_from_screen( PreChannelTypes,  PRE_CHANNEL_1_XBOX );
                get_curve_types_from_screen( PostChannelTypes, POST_CHANNEL_1_XBOX );
                check_the_pressure_buttons();
                FixedChannels.get_from_screen();
                FixedPressures.put_to_screen();
                return FALSE;

            case SAVE_DEFAULT_DISPLAY_CONFIG_PB:
                save_this_config( id );
                return TRUE;

            case IDOK:
                save_this_config( id );
                HaveHeader = FALSE;

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                FixedPressures.empty();
                FixedChannels.empty();
                return TRUE;
            }
        break;

    case WM_DESTROY:
        DialogIsActive = FALSE;
        break;
    }

return FALSE;
}
