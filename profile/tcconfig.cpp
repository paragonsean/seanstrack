#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\array.h"
#include "..\include\bitclass.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\param.h"
#include "..\include\listbox.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"
#include "..\include\v5help.h"

#include "resource.h"
#include "extern.h"
#include "fixparam.h"
#include "tcconfig.h"

const int MAX_LB_LEN = PARAMETER_NAME_LEN + 1 + MAX_DOUBLE_LEN + 1 + MAX_DOUBLE_LEN + 1 + MAX_YES_LEN;

const TCHAR NoString[]  = TEXT( "No" );
const TCHAR YesString[] = TEXT( "Yes" );

static TCHAR FixedParamIniFile[] = TEXT( "fixedparam.ini" );
static TCHAR TcConfigIniFile[]   = TEXT( "tcconfig.ini" );

static TCHAR AverageOverTypeKey[]   = TEXT("AverageOverType");
static TCHAR ChartTypeKey[]         = TEXT("ChartType");
static TCHAR FirstShotKey[]         = TEXT("FirstShot");
static TCHAR LastShotKey[]          = TEXT("LastShot");
static TCHAR LclSigmaValueKey[]     = TEXT("LclSigmaValue");
static TCHAR NOfPointsKey[]         = TEXT("NOfPoints");
static TCHAR NToAverageKey[]        = TEXT("NToAverage");
static TCHAR NeedAlarmLimitsKey[]   = TEXT("NeedAlarmLimits");
static TCHAR NeedLclLimitKey[]      = TEXT("NeedLclLimit");
static TCHAR NeedUclLimitKey[]      = TEXT("NeedUclLimit");
static TCHAR NeedWarningLimitsKey[] = TEXT("NeedWarningLimits");
static TCHAR NeedTargetKey[]        = TEXT("NeedTarget");
static TCHAR TimePeriodTypeKey[]    = TEXT("TimePeriodType");
static TCHAR UclSigmaValueKey[]     = TEXT("UclSigmaValue");
static TCHAR VisibleListKey[]       = TEXT("VisibleList");
static TCHAR XAxisTypeKey[]         = TEXT("XAxisType");

static TCHAR IsFixedKey[] = TEXT( "IsFixed" );
static TCHAR MaxValueKey[] = TEXT( "MaxValue" );
static TCHAR MinValueKey[] = TEXT( "MinValue" );

static bool UpdatingEditControls = false;

static BIT_CLASS linechanged;

class TREND_SCREEN_CONFIG_CLASS : public TREND_CONFIG_CLASS
{
public:

void put_to_screen( HWND w );
void get_from_screen( HWND w );
};

static TREND_SCREEN_CONFIG_CLASS Tc;

void erase_trend_cursor_line();
TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                           BOOLEAN_FROM_INI                           *
***********************************************************************/
static BOOLEAN boolean_from_ini( TCHAR * key )
{
return boolean_from_ini( TcConfigIniFile, ConfigSection, key );
}

/***********************************************************************
*                            BOOLEAN_TO_INI                            *
***********************************************************************/
static void boolean_to_ini( TCHAR * key, BOOLEAN b )
{
boolean_to_ini( TcConfigIniFile, ConfigSection, key, b );
}

/***********************************************************************
*                            INT32_FROM_INI                            *
***********************************************************************/
static int32 int32_from_ini( TCHAR * key )
{
return int32_from_ini( TcConfigIniFile, ConfigSection, key );
}

/***********************************************************************
*                              INT32_TO_INI                            *
***********************************************************************/
static void int32_to_ini( TCHAR * key, int32 i )
{
int32_to_ini( TcConfigIniFile, ConfigSection, key, i );
}

/***********************************************************************
*                            FLOAT_FROM_INI                            *
***********************************************************************/
static float float_from_ini( TCHAR * key )
{
return float_from_ini( TcConfigIniFile, ConfigSection, key );
}

/***********************************************************************
*                              FLOAT_TO_INI                            *
***********************************************************************/
static void float_to_ini( TCHAR * key, float x )
{
float_to_ini( TcConfigIniFile, ConfigSection, key, x );
}

/***********************************************************************
*                            SET_CHECKED_STATE                         *
***********************************************************************/
static void set_checked_state( HWND w, INT box, BOOLEAN button_is_checked )
{
UINT state;

if ( button_is_checked )
    state = BST_CHECKED;
else
    state = BST_UNCHECKED;

CheckDlgButton( w, box, state );
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
***********************************************************************/
FIXED_PARAM_CLASS::FIXED_PARAM_CLASS()
{
*min_value = NullChar;
*max_value = NullChar;
is_fixed = false;
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
***********************************************************************/
FIXED_PARAM_CLASS::FIXED_PARAM_CLASS( const FIXED_PARAM_CLASS & sorc )
{
*this = sorc;
}

/***********************************************************************
*                          ~FIXED_PARAM_CLASS                          *
***********************************************************************/
FIXED_PARAM_CLASS::~FIXED_PARAM_CLASS()
{
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
*                                  READ                                *
***********************************************************************/
void FIXED_PARAM_CLASS::read( TCHAR * parameter_name )
{
NAME_CLASS s;
INI_CLASS  ini;

*min_value = NullChar;
*max_value = NullChar;
is_fixed   = FALSE;

ini.set_section( parameter_name );

if ( PartBasedDisplay )
    {
    s.get_part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    ini.set_file( s.text() );
    if ( ini.exists() )
        {
        if ( ini.find(IsFixedKey) )
            {
            is_fixed = ini.get_boolean();

            if ( ini.find(MaxValueKey) )
                lstrcpyn( max_value, ini.get_string(), MAX_DOUBLE_LEN+1);

            if ( ini.find(MinValueKey) )
                lstrcpyn( min_value, ini.get_string(), MAX_DOUBLE_LEN+1);

            return;
            }
        }
    }

s.get_local_ini_file_name( FixedParamIniFile );
ini.set_file( s.text() );
is_fixed = ini.get_boolean( IsFixedKey );

if ( ini.find( MaxValueKey) )
    lstrcpyn( max_value, ini.get_string(), MAX_DOUBLE_LEN+1);

if ( ini.find( MinValueKey) )
    lstrcpyn( min_value, ini.get_string(), MAX_DOUBLE_LEN+1);
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
*                                 WRITE                                *
***********************************************************************/
void FIXED_PARAM_CLASS::write( TCHAR * parameter_name )
{
NAME_CLASS s;
INI_CLASS  ini;

ini.set_section( parameter_name );

if ( PartBasedDisplay )
    s.get_part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
else
    s.get_local_ini_file_name( FixedParamIniFile );

ini.set_file( s.text() );
ini.put_boolean( IsFixedKey,  is_fixed  );
ini.put_string(  MaxValueKey, max_value );
ini.put_string(  MinValueKey, min_value );
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
*                                   =                                  *
***********************************************************************/
FIXED_PARAM_CLASS & FIXED_PARAM_CLASS::operator=( const FIXED_PARAM_CLASS & sorc )
{
lstrcpy( min_value, sorc.min_value );
lstrcpy( max_value, sorc.max_value );
is_fixed  = sorc.is_fixed;

return *this;
}

/***********************************************************************
*                           FIXED_PARAM_CLASS                          *
*                                LBLINE                                *
***********************************************************************/
TCHAR * FIXED_PARAM_CLASS::lbline( TCHAR * parameter_name )
{
static TCHAR s[MAX_LB_LEN+1];

TCHAR * cp;
UINT  id;

cp = copy_w_char( s,  parameter_name, TabChar );
cp = copy_w_char( cp, min_value,      TabChar );
cp = copy_w_char( cp, max_value,      TabChar );
if ( is_fixed )
    id = YES_STRING;
else
    id = NO_STRING;

lstrcpy( cp, resource_string(id) );

return s;
}

/***********************************************************************
*                              GET_FROM_LINE                           *
***********************************************************************/
void FIXED_PARAM_CLASS::get_from_line( TCHAR * sorc )
{
TCHAR s[MAX_LB_LEN+1];

lstrcpyn( s, sorc, MAX_LB_LEN+1 );

sorc = s;
if ( replace_char_with_null(sorc, TabChar) )
    {
    sorc = nextstring( sorc );
    if ( replace_char_with_null(sorc, TabChar) )
        {
        lstrcpy( min_value, sorc );
        sorc = nextstring( sorc );
        if ( replace_char_with_null(sorc, TabChar) )
            {
            lstrcpy( max_value, sorc );
            sorc = nextstring( sorc );
            if ( strings_are_equal(sorc, resource_string(YES_STRING)) )
                is_fixed = true;
            else
                is_fixed = false;
            }
        }
    }

}

/***********************************************************************
*                            WRITE_FROM_LINE                           *
*   This assumes that you are passing a line from the listbox.         *
***********************************************************************/
void FIXED_PARAM_CLASS::write_from_line( TCHAR * sorc )
{
TCHAR pname[PARAMETER_NAME_LEN + 1];
TCHAR * d;
TCHAR * s;
int len;

s = sorc;
d = pname;
len = 0;
while ( len <= PARAMETER_NAME_LEN )
    {
    *d = *s;
    if ( *s == NullChar || *s == TabChar )
        break;
    d++;
    s++;
    len++;
    }

*d = NullChar;
if ( len > 0 )
    {
    get_from_line( sorc );
    write( pname );
    }
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
***********************************************************************/
TREND_CONFIG_CLASS::TREND_CONFIG_CLASS()
{
int i;

chart_type          = TREND_CHART_TYPE;
nto_average         = 1;
average_over_type   = SHOT_AVERAGE_TYPE;
time_period_type    = LAST_N_TIME_PERIOD_TYPE;
nof_points          = 50;
x_axis_type         = SHOT_AVERAGE_TYPE;
need_alarm_limits   = FALSE;
need_warning_limits = FALSE;
need_target         = FALSE;
need_ucl            = FALSE;
need_lcl            = FALSE;
ucl_sigma_value     = 1.0;
lcl_sigma_value     = 1.0;

for ( i=0; i<MAX_PARMS; i++ )
    visible[i] = TRUE;
}

/***********************************************************************
*                          ~TREND_CONFIG_CLASS                         *
***********************************************************************/
TREND_CONFIG_CLASS::~TREND_CONFIG_CLASS()
{
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                                   =                                  *
***********************************************************************/
void TREND_CONFIG_CLASS::operator=( const TREND_CONFIG_CLASS & sorc )
{
int i;

chart_type          = sorc.chart_type;
nto_average         = sorc.nto_average;
average_over_type   = sorc.average_over_type;
time_period_type    = sorc.time_period_type;
nof_points          = sorc.nof_points;
first_shot          = sorc.first_shot;
last_shot           = sorc.last_shot;
x_axis_type         = sorc.x_axis_type;
need_alarm_limits   = sorc.need_alarm_limits;
need_warning_limits = sorc.need_warning_limits;
need_target         = sorc.need_target;
need_ucl            = sorc.need_ucl;
need_lcl            = sorc.need_lcl;
ucl_sigma_value     = sorc.ucl_sigma_value;
lcl_sigma_value     = sorc.lcl_sigma_value;

for ( i=0; i<MAX_PARMS; i++ )
    visible[i] = sorc.visible[i];
}

/***********************************************************************
*                         TREND_CONFIG_CLASS                           *
*                             N_VISIBLE                                *
***********************************************************************/
int32 TREND_CONFIG_CLASS::n_visible()
{
int32 i;
int32 count;

count = 0;

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( visible[i] )
        count++;
    }
return count;
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                                  ==                                  *
***********************************************************************/
BOOLEAN TREND_CONFIG_CLASS::operator==(const TREND_CONFIG_CLASS & sorc )
{
int i;

if ( chart_type != sorc.chart_type )
    return FALSE;

if ( nto_average != sorc.nto_average )
    return FALSE;

if ( average_over_type != sorc.average_over_type )
    return FALSE;

if ( time_period_type  != sorc.time_period_type )
    return FALSE;

if ( nof_points != sorc.nof_points )
    return FALSE;

if ( first_shot != sorc.first_shot )
    return FALSE;

if ( last_shot != sorc.last_shot  )
    return FALSE;

if ( x_axis_type != sorc.x_axis_type )
    return FALSE;

if ( need_alarm_limits != sorc.need_alarm_limits )
    return FALSE;

if ( need_warning_limits != sorc.need_warning_limits )
    return FALSE;

if ( need_target != sorc.need_target )
    return FALSE;

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( visible[i] != sorc.visible[i] )
        return FALSE;
    }

return TRUE;

}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                                  !=                                  *
***********************************************************************/
BOOLEAN TREND_CONFIG_CLASS::operator!=(const TREND_CONFIG_CLASS & sorc )
{
if ( operator==(sorc) )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                                 READ                                 *
***********************************************************************/
void TREND_CONFIG_CLASS::read()
{
NAME_CLASS s;
INI_CLASS  ini;
int        i;
TCHAR    * cp;

if ( PartBasedDisplay )
    {
    s.get_part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    ini.set_file( s.text() );
    }

if ( !ini.exists() )
    {
    s.get_local_ini_file_name( TcConfigIniFile );
    ini.set_file( s.text() );
    }

if ( ini.exists() )
    {
    ini.set_section( ConfigSection );
    /*
    -------------------------------------------------
    If the file has one entry it should have them all
    ------------------------------------------------- */
    if ( ini.find(ChartTypeKey)  )
        {

        chart_type          = ini.get_int();
        average_over_type   = ini.get_int( AverageOverTypeKey  );
        time_period_type    = ini.get_int( TimePeriodTypeKey   );
        x_axis_type         = ini.get_int( XAxisTypeKey        );
        nof_points          = ini.get_int( NOfPointsKey        );
        nto_average         = ini.get_int( NToAverageKey       );
        first_shot          = ini.get_string( FirstShotKey     );
        last_shot           = ini.get_string( LastShotKey      );

        need_alarm_limits   = ini.get_boolean( NeedAlarmLimitsKey   );
        need_warning_limits = ini.get_boolean( NeedWarningLimitsKey );
        need_target         = ini.get_boolean( NeedTargetKey );
        need_ucl            = ini.get_boolean( NeedUclLimitKey      );
        need_lcl            = ini.get_boolean( NeedLclLimitKey      );

        ucl_sigma_value     = ini.get_double( UclSigmaValueKey );
        lcl_sigma_value     = ini.get_double( LclSigmaValueKey );

        cp = ini.get_string( VisibleListKey );
        i = 0;

        /*
        ------------------------------------------------------
        This is not working. Just set all the visibles to true
        ------------------------------------------------------ */
//      while ( i < MAX_PARMS )
//          {
//          if ( *cp == NullChar )
//              break;
//
//          if ( *cp == YChar )
//              visible[i] = TRUE;
//          else
//              visible[i] = FALSE;
//
//          cp++;
//          i++;
//          }

        /*
        ---------------
        Default to TRUE
        --------------- */
        while ( i < MAX_PARMS )
            {
            visible[i] = TRUE;
            i++;
            }
        return;
        }
    }
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                               WRITE                                  *
***********************************************************************/
void TREND_CONFIG_CLASS::write()
{
NAME_CLASS s;
INI_CLASS ini;
TCHAR buf[MAX_PARMS+1];
int   i;

if ( PartBasedDisplay )
    s.get_part_display_ini_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
else
    s.get_local_ini_file_name( TcConfigIniFile );

ini.set_file( s.text() );
ini.set_section( ConfigSection );

ini.put_int( ChartTypeKey,       chart_type        );
ini.put_int( AverageOverTypeKey, average_over_type );
ini.put_int( TimePeriodTypeKey,  time_period_type  );
ini.put_int( XAxisTypeKey,       x_axis_type       );
ini.put_int( NOfPointsKey,       nof_points        );
ini.put_int( NToAverageKey,      nto_average       );
ini.put_string( FirstShotKey,    first_shot.text() );
ini.put_string( LastShotKey,     last_shot.text()  );

ini.put_boolean( NeedAlarmLimitsKey,   need_alarm_limits   );
ini.put_boolean( NeedWarningLimitsKey, need_warning_limits );
ini.put_boolean( NeedTargetKey,        need_target );
ini.put_boolean( NeedUclLimitKey,      need_ucl            );
ini.put_boolean( NeedLclLimitKey,      need_lcl            );

ini.put_double( UclSigmaValueKey, (double) ucl_sigma_value     );
ini.put_double( LclSigmaValueKey, (double) lcl_sigma_value     );

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( visible[i] )
        buf[i] = YChar;
    else
        buf[i] = NChar;
    }
buf[MAX_PARMS] = NullChar;
ini.put_string( VisibleListKey, buf );
}

/***********************************************************************
*                           TREND_CONFIG_CLASS                         *
*                             PUT_TO_SCREEN                            *
***********************************************************************/
void TREND_SCREEN_CONFIG_CLASS::put_to_screen( HWND w )
{
set_text( w, N_TO_AVG_EBOX, int32toasc(nto_average) );

CheckRadioButton( w, AVG_SHOTS_RADIO,    AVG_MONTHS_RADIO,       AVG_SHOTS_RADIO     + average_over_type    );

CheckRadioButton( w, TREND_RADIO,         HISTOGRAM_RADIO,       TREND_RADIO         + chart_type       );
CheckRadioButton( w, LAST_N_RANGE_RADIO, SHOT_RANGE_RADIO,       LAST_N_RANGE_RADIO + time_period_type );
CheckRadioButton( w, SHOT_X_AXIS_RADIO,  DAY_MONTH_X_AXIS_RADIO, SHOT_X_AXIS_RADIO  + x_axis_type      );

set_text( w, LAST_N_RANGE_EBOX,   int32toasc(nof_points) );
first_shot.set_text( w, SHOT_RANGE_FROM_EBOX );
last_shot.set_text( w, SHOT_RANGE_TO_EBOX );

/*
set_text( w, UCL_SIGMA_EBOX,      ascii_float(ucl_sigma_value) );
set_text( w, LCL_SIGMA_EBOX,      ascii_float(lcl_sigma_value) );
*/

set_checked_state( w, ALARM_LIMITS_CHECKBOX,   need_alarm_limits );
set_checked_state( w, WARNING_LIMITS_CHECKBOX, need_warning_limits );
set_checked_state( w, SHOW_TARGET_CHECKBOX,    need_target );

/*
set_checked_state( w, UCL_CHECKBOX, need_ucl );
set_checked_state( w, LCL_CHECKBOX, need_lcl );
*/
}

/***********************************************************************
*                           SAVE_TC_VISIBLE                            *
***********************************************************************/
static void save_tc_visible( HWND w )
{
int   i;
INT * ip;
int   j;
int   n;
LISTBOX_CLASS lb;

lb.init( w, TC_VISIBLE_LB );
n = lb.get_select_list( &ip );
if ( n > 0 )
    {
    /*
    ----------------------
    Clear all the visibles
    ---------------------- */
    for ( i=0; i<MAX_PARMS; i++ )
        Tc.visible[i] = FALSE;

    for ( i=0; i<n; i++ )
        {
        j = int( ip[i] );
        if ( j>=0 && j<n )
            Tc.visible[j] = TRUE;
        }

    delete[] ip;
    ip = 0;
    }
}

/***********************************************************************
*                          FILL_TC_VISIBLE_LB                          *
***********************************************************************/
static void fill_tc_visible_lb( HWND w )
{
int           i;
LISTBOX_CLASS lb;
int           n;

lb.init( w, TC_VISIBLE_LB );
lb.redraw_off();

n = MAX_PARMS;
for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( CurrentParam.parameter[i].input.type == NO_PARAMETER_TYPE )
        {
        n = i;
        break;
        }
    lb.add( CurrentParam.parameter[i].name );
    }

for ( i=0; i<n; i++ )
    {
    if ( Tc.visible[i] )
        lb.set_select( i, TRUE );
    }

lb.redraw_on();
}

/***********************************************************************
*                     SET_TC_VISIBLE_SETUP_STRINGS                     *
***********************************************************************/
static void set_tc_visible_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { HIGHLIGHT_PARAMS_STATIC, TEXT("HIGHLIGHT_PARAMS") },
    { IDOK, TEXT("OK_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    TC_VISIBLE_LB
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
w.set_title( StringTable.find(TEXT("PARAMS_TO_DISPLAY")) );
w.refresh();
}

/***********************************************************************
*                         TC_VISIBLE_DIALOG_PROC                       *
*                     TC_VISIBLE_DIALOG Modal Dialog                   *
***********************************************************************/
BOOL CALLBACK tc_visible_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        set_tc_visible_setup_strings( hWnd );
        fill_tc_visible_lb( hWnd );
        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;

            case IDOK:
                save_tc_visible( hWnd );
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                         GET_RADIO_BOX_INDEX                          *
***********************************************************************/
static int32 get_radio_box_index( HWND w, int32 first_box, int32 last_box )
{
int32 i;

for ( i=first_box; i<=last_box; i++ )
    if ( is_checked(w, i) )
        return i-first_box;

return 0;
}

/***********************************************************************
*                        CURRENT_PARAMETER_NAME                        *
***********************************************************************/
static TCHAR * current_parameter_name( HWND w )
{
static TCHAR s[PARAMETER_NAME_LEN+1];

INT i;
int     n;
TCHAR * dest;
TCHAR * sorc;
LISTBOX_CLASS lb;

lb.init( w, TREND_PARAMETER_LISTBOX );
i = lb.current_index();
sorc = lb.item_text( i );
dest = s;
n    = 0;

while ( *sorc != TabChar && *sorc != NullChar && n <= PARAMETER_NAME_LEN  )
    {
    *dest = *sorc;
    dest++;
    sorc++;
    n++;
    }

*dest = NullChar;

return s;
}

/***********************************************************************
*                        REFRESH_LISTBOX_ENTRY                         *
***********************************************************************/
static void refresh_listbox_entry( HWND w )
{
LISTBOX_CLASS  lb;
FIXED_PARAM_CLASS fpc;
int i;

if ( UpdatingEditControls )
    return;

lb.init( w, TREND_PARAMETER_LISTBOX );
i = lb.current_index();
linechanged.set(i);

get_text( fpc.min_value, w, PLOT_MIN_EBOX, MAX_DOUBLE_LEN );
get_text( fpc.max_value, w, PLOT_MAX_EBOX, MAX_DOUBLE_LEN );

if ( is_checked(w, USE_LIMITS_XBOX) )
    fpc.is_fixed = true;
else
    fpc.is_fixed = false;

lb.replace( fpc.lbline( current_parameter_name(w) ) );
}

/***********************************************************************
*                         DO_NEW_LISTBOX_LINE                          *
***********************************************************************/
static void do_new_listbox_line( HWND w )
{
INT i;
TCHAR * cp;
LISTBOX_CLASS lb;
FIXED_PARAM_CLASS fpc;
BOOLEAN need_check;

UpdatingEditControls = true;

lb.init( w, TREND_PARAMETER_LISTBOX );
i = lb.current_index();
cp = lb.item_text( i );
fpc.get_from_line( cp );
set_text( w, PLOT_MIN_EBOX, fpc.min_value );
set_text( w, PLOT_MAX_EBOX, fpc.max_value );
if ( fpc.is_fixed )
    need_check = TRUE;
else
    need_check = FALSE;

set_checkbox( w, USE_LIMITS_XBOX, need_check );

UpdatingEditControls = false;
}

/***********************************************************************
*                         FILL_PARAMETER_LIST                          *
***********************************************************************/
static void fill_parameter_list( HWND w )
{
int32 i;
NAME_CLASS    s;
LISTBOX_CLASS lb;
INT32_ARRAY_CLASS ia;
FIXED_PARAM_CLASS fpc;

lb.init( w, TREND_PARAMETER_LISTBOX );
lb.redraw_off();

lb.set_tabs( PARAM_COL_TBOX, USE_COL_TBOX );

linechanged.clearall();

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( CurrentParam.parameter[i].input.type != NO_PARAMETER_TYPE && CurrentParam.parameter[i].input.type != TEXT_VAR)
        {
        fpc.read( CurrentParam.parameter[i].name  );
        lb.add( fpc.lbline(CurrentParam.parameter[i].name) );
        }
    }

if ( PartBasedDisplay )
    {
    s.get_tc_parameter_list_file_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
    if ( !s.file_exists() )
        s.null();
    }

if ( s.isempty() )
    s.get_tc_parameter_list_file_name();

if ( s.file_exists() )
    {
    ia.read( s.text() );
    while ( ia.next() )
        if ( ia.value() < lb.count() )
            lb.set_select( ia.value(), TRUE );
    }

lb.redraw_on();
do_new_listbox_line( w );
}

/***********************************************************************
*                           GET_FROM_SCREEN                            *
***********************************************************************/
void TREND_SCREEN_CONFIG_CLASS::get_from_screen( HWND w )
{
TCHAR buf[MAX_FLOAT_LEN+1];

average_over_type = get_radio_box_index( w, AVG_SHOTS_RADIO,    AVG_MONTHS_RADIO       );
chart_type        = get_radio_box_index( w, TREND_RADIO,        HISTOGRAM_RADIO        );
time_period_type  = get_radio_box_index( w, LAST_N_RANGE_RADIO, SHOT_RANGE_RADIO       );
x_axis_type       = get_radio_box_index( w, SHOT_X_AXIS_RADIO,  DAY_MONTH_X_AXIS_RADIO );

get_text( buf, w, N_TO_AVG_EBOX, MAX_FLOAT_LEN );
nto_average = asctoint32( buf );

get_text( buf, w, LAST_N_RANGE_EBOX, MAX_FLOAT_LEN );
nof_points = asctoint32( buf );

first_shot.get_text( w, SHOT_RANGE_FROM_EBOX );
last_shot.get_text( w, SHOT_RANGE_TO_EBOX );

/*
get_text( buf, w, UCL_SIGMA_EBOX, MAX_FLOAT_LEN );
ucl_sigma_value = extfloat( buf );
get_text( buf, w, LCL_SIGMA_EBOX, MAX_FLOAT_LEN );
lcl_sigma_value = extfloat( buf );
*/

need_alarm_limits   = is_checked( w, ALARM_LIMITS_CHECKBOX );
need_warning_limits = is_checked( w, WARNING_LIMITS_CHECKBOX );
need_target         = is_checked( w, SHOW_TARGET_CHECKBOX );

/*
need_ucl            = is_checked( w, UCL_CHECKBOX );
need_lcl            = is_checked( w, LCL_CHECKBOX );
*/
}

/***********************************************************************
*                            INIT_CONTROLS                             *
***********************************************************************/
static void init_controls( HWND w )
{
/*
------------------------------------------------------------------
Initialize the bits that indicate whether the operator changed any
fixed parameter limits
------------------------------------------------------------------ */
linechanged.init( MAX_PARMS );

Tc.read();
Tc.put_to_screen( w );
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes( HWND w )
{
INT   * ip;
INT   * p;
NAME_CLASS s;
int32  i;
int32  n;
LISTBOX_CLASS lb;
INT32_ARRAY_CLASS ia;
FIXED_PARAM_CLASS fpc;
WPARAM force_change;

Tc.get_from_screen( w );
Tc.write();

lb.init( w, TREND_PARAMETER_LISTBOX );
n = lb.get_select_list( &ip );
if ( n > 0 )
    {
    if ( ia.init( n ) )
        {
        p = ip;
        while ( ia.next() )
            {
            ia.set( int32(*p) );
            p++;
            }

        if ( PartBasedDisplay )
            s.get_tc_parameter_list_file_name( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
        else
            s.get_tc_parameter_list_file_name();

        ia.write( s.text() );
        }
    delete[] ip;
    }

force_change = 0;
n = lb.count();
for ( i=0; i<n; i++ )
    {
    if ( linechanged[i] )
        {
        force_change = 1;
        fpc.write_from_line( lb.item_text(i) );
        }
    }

linechanged.clearall();

SendMessage( MainWindow, WM_NEW_TC_CONFIG, force_change, 0L );
}

/***********************************************************************
*                    SET_TREND_CONFIG_SETUP_STRINGS                    *
***********************************************************************/
static void set_trend_config_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { CHART_TYPE_STATIC,               TEXT("CHART_TYPE") },
    { TIME_PERIOD_STATIC,              TEXT("TIME_PERIOD") },
    { TC_XAXIS_STATIC,                 TEXT("XAXIS") },
    { AVERAGE_EVERY_STATIC,            TEXT("AVG_EVERY") },
    { TC_POINTS_STATIC,                TEXT("POINTS") },
    { AVG_SHOTS_RADIO,                 TEXT("SHOTS_STRING") },
    { AVG_MINUTES_RADIO,               TEXT("MINUTES") },
    { AVG_HOURS_RADIO,                 TEXT("HOURS_STRING") },
    { AVG_SHIFTS_RADIO,                TEXT("SHIFTS_STRING") },
    { AVG_DAYS_RADIO,                  TEXT("DAYS") },
    { AVG_WEEKS_RADIO,                 TEXT("WEEKS") },
    { AVG_MONTHS_RADIO,                TEXT("MONTHS") },
    { LAST_N_RANGE_RADIO,              TEXT("LAST") },
    { SHOT_RANGE_RADIO,                TEXT("SHOT_RANGE") },
    { TC_TO_STATIC,                    TEXT("TO_STRING") },
    { SHOT_X_AXIS_RADIO,               TEXT("SHOT_NUMBER_STRING") },
    { HOUR_DAY_X_AXIS_RADIO,           TEXT("HOUR_AND_DAY") },
    { SHIFT_DAY_X_AXIS_RADIO,          TEXT("SHIFT_AND_DAY") },
    { DAY_MONTH_X_AXIS_RADIO,          TEXT("DAY_AND_MONTH") },
    { TREND_RADIO,                     TEXT("TREND") },
    { XBAR_RADIO,                      TEXT("XBAR") },
    { RANGE_RADIO,                     TEXT("RANGE_STRING") },
    { HISTOGRAM_RADIO,                 TEXT("HISTOGRAM") },
    { ALARM_LIMITS_CHECKBOX,           TEXT("ALARM_LIMITS_STRING") },
    { WARNING_LIMITS_CHECKBOX,         TEXT("WARNING_LIMITS_STRING") },
    { SHOW_TARGET_CHECKBOX,            TEXT("TARGET_STRING") },
    { TREND_CHART_SAVE_CHANGES_BUTTON, TEXT("SAVE_CHANGES") },
    { NO_TREND_PARAMS_BUTTON,          TEXT("NONE_STRING") },
    { PARAM_COL_TBOX,                  TEXT("PARAMS_TO_DISPLAY") },
    { MIN_COL_TBOX,                    TEXT("MINIMUM") },
    { MAX_COL_TBOX,                    TEXT("MAXIMUM") },
    { USE_COL_TBOX,                    TEXT("USE") },
    { USE_LIMITS_XBOX,                 TEXT("USE_LIMITS") },
    { TC_MAX_STATIC,                   TEXT("MAXIMUM") },
    { TC_MIN_STATIC,                   TEXT("MINIMUM") },
    { CHOOSE_VISIBLE_PB,               TEXT("CHOOSE_SUMMARY_PARAMS") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    TREND_PARAMETER_LISTBOX,
    PLOT_MAX_EBOX,
    PLOT_MIN_EBOX
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
w.set_title( StringTable.find(TEXT("TREND_CONFIG_MENU_STRING")) );
w.refresh();
}

/***********************************************************************
*                        TREND_CONFIG_DIALOG_PROC                      *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK trend_config_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;
int     cmd;
LISTBOX_CLASS lb;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        set_trend_config_setup_strings( hWnd );
        init_controls( hWnd );
        fill_parameter_list( hWnd );
        return TRUE;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case PLOT_MIN_EBOX:
            case PLOT_MAX_EBOX:
                if ( cmd != EN_CHANGE )
                    break;
                refresh_listbox_entry( hWnd );
                return TRUE;

            case USE_LIMITS_XBOX:
                if ( cmd != BN_CLICKED )
                    break;
                refresh_listbox_entry(hWnd);
                return TRUE;

            case CHOOSE_VISIBLE_PB:
                DialogBox(
                    MainInstance,
                    TEXT("TC_VISIBLE_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) tc_visible_dialog_proc );
                return TRUE;

            case NO_TREND_PARAMS_BUTTON:
                lb.init( hWnd, TREND_PARAMETER_LISTBOX );
                lb.unselect_all();
                return TRUE;

            case TREND_PARAMETER_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_new_listbox_line( hWnd );
                    return TRUE;
                    }
                break;

            case IDOK:
            case TREND_CHART_SAVE_CHANGES_BUTTON:
                save_changes( hWnd );

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                InvalidateRect( MainPlotWindow, 0, TRUE );
                return TRUE;
            }
        break;
    }

return FALSE;
}
