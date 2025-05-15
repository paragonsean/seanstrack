#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\autoback.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\units.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"

#define _MAIN_
#include "..\include\chars.h"
#include "..\include\events.h"

STRING_CLASS  ComputerName;
ANALOG_SENSOR CurrentAnalog;
HINSTANCE     MainInstance;
HWND          MainWindow;
HWND          SensorWindow;
STRING_CLASS  MyWindowTitle;
LISTBOX_CLASS SensorListbox;
LISTBOX_CLASS UnitsCbox;
LISTBOX_CLASS VartypeCbox;

TCHAR MyClassName[]   = TEXT( "ESensor" );

struct VARTYPE_ENTRY
    {
    unsigned value;
    TCHAR    name[VARTYPE_NAME_LEN+1];
    };

VARTYPE_ENTRY * Vartypes    = 0;
short           NofVartypes = 0;

static TCHAR CommonResourceFile[]     = TEXT( "common" );
static TCHAR ConfigSection[]          = TEXT( "Config" );
static TCHAR CurrentLanguageKey[]     = TEXT( "CurrentLanguage" );
static TCHAR DisplayIniFile[]         = TEXT( "display.ini" );
static TCHAR EditSensorResourceFile[] = TEXT( "esensor" );

FONT_LIST_CLASS   FontList;
STRINGTABLE_CLASS StringTable;

/*
---------------------
Current Sensor Number
--------------------- */
int x = 0;

/*
-------------------
Sensor Point Offset
------------------- */
int sensor_pt_offset = 0;

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
static int resource_message_box( TCHAR * msg, TCHAR * title )
{
STRING_CLASS m;
STRING_CLASS t;

StringTable.find( m, msg );
StringTable.find( t, title );

return MessageBox( NULL, m.text(), t.text(), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes()
{
AUTO_BACKUP_CLASS ab;
NAME_CLASS n;

n.get_asensor_csvname( ComputerName );
CurrentAnalog.put( n );
ab.get_backup_dir();
ab.backup_data_dir();
}

/***********************************************************************
*                               FILL_UNITS                             *
***********************************************************************/
static void fill_units()
{
int           i;
int           my_index;
int           n;
STRING_CLASS  s;
short         current_units_id;
unsigned      current_vartype;
UNITS_CLASS   u;

UnitsCbox.redraw_off();
UnitsCbox.empty();

n = u.count();

current_units_id = CurrentAnalog[x].units;
current_vartype  = CurrentAnalog[x].vartype;

i = 0;
my_index = 0;
while ( u.next() )
    {
    if ( u.type() & current_vartype )
        {
        UnitsCbox.add( u.name() );
        if ( u.id() == current_units_id )
            my_index = i;
        i++;
        }
    }

UnitsCbox.setcursel( my_index );
UnitsCbox.redraw_on();

s = UnitsCbox.selected_text();
s.set_text( SensorWindow, UNITS_TBOX );
s.set_text( SensorWindow, TEST_UNITS_TBOX );
}

/***********************************************************************
*                            SHOW_SENSOR                               *
***********************************************************************/
static void show_sensor()
{
int i;
bool is_enabled;
bool need_enabled;
WINDOW_CLASS w;
STRING_CLASS s;
STRING_CLASS low_volts_str;
STRING_CLASS low_value_str;
STRING_CLASS high_volts_str;
STRING_CLASS high_value_str;
STRING_CLASS low_static_str = TEXT( "Point 1" );
STRING_CLASS high_static_str = TEXT( "Point 2" );
int nMax;
int nPage;
HWND sbHwnd;
SCROLLINFO sensor_pt_sb_info;

/*
--------------------------------------------------
If this is one of my sensors, don't allow changes.
-------------------------------------------------- */
need_enabled = TRUE;
if ( x == CurrentAnalog.default_sensor_index || x == CurrentAnalog.cmd_sensor_index )
    need_enabled = FALSE;

w = GetDlgItem( SensorWindow, SENSOR_DESC_EBOX );
is_enabled = false;
if ( w.is_enabled() )
    is_enabled = true;

if ( is_enabled != need_enabled )
    {
    i = SENSOR_DESC_EBOX;
    while ( i<=HIGH_VALUE_EBOX )
        {
        w = GetDlgItem( SensorWindow, i );
        w.enable( need_enabled );
        i++;
        }
    }

CurrentAnalog[x].desc.set_text(SensorWindow, SENSOR_DESC_EBOX);

/*
--------------------------------------
Convert voltages and values to strings
-------------------------------------- */
low_volts_str  = CurrentAnalog[x].s_voltages[0];
low_value_str  = CurrentAnalog[x].s_values[0];
high_volts_str = CurrentAnalog[x].s_voltages[1];
high_value_str = CurrentAnalog[x].s_values[1];

/*
-----------------------
Populate the edit boxes
----------------------- */
low_volts_str.set_text(SensorWindow, LOW_VOLTS_EBOX);
low_value_str.set_text(SensorWindow, LOW_VALUE_EBOX);
high_volts_str.set_text(SensorWindow, HIGH_VOLTS_EBOX);
high_value_str.set_text(SensorWindow, HIGH_VALUE_EBOX);

/*
-------------------------
Populate the point labels
------------------------- */
sensor_pt_offset = 0;
low_static_str.set_text(SensorWindow, LOW_STATIC);
high_static_str.set_text(SensorWindow, HIGH_STATIC);

/*
--------------------------------
Set scrollbar range and position
-------------------------------- */
nMax = CurrentAnalog[x].nof_sensor_points - 2;
nPage = 1;
sbHwnd = GetDlgItem(SensorWindow, SENSOR_PTS_SCROLLBAR);
sensor_pt_sb_info.cbSize = sizeof(SCROLLINFO);
sensor_pt_sb_info.fMask = SIF_ALL;
sensor_pt_sb_info.nMin = 0;
sensor_pt_sb_info.nMax = nMax;
sensor_pt_sb_info.nPage = nPage;
sensor_pt_sb_info.nPos = sensor_pt_offset;
SetScrollInfo(sbHwnd, SB_CTL, &sensor_pt_sb_info, true);

/*
----------------------
Set vartypes and units
---------------------- */
s = UnitsCbox.selected_text();
if ( Vartypes )
    {
    for ( i=0; i<NofVartypes; i++ )
        {
            if (Vartypes[i].value == CurrentAnalog[x].vartype)
            {
            VartypeCbox.setcursel( Vartypes[i].name );
            break;
            }
        }
    }

fill_units();

s.get_text( SensorWindow, TEST_VOLTS_EBOX );
if ( !s.isempty() )
    {
    if (s.real_value() > CurrentAnalog[x].max_volts)
        s = CurrentAnalog[x].max_volts_text();

    s.set_text( SensorWindow, TEST_VOLTS_EBOX );
    }
}

/***********************************************************************
*                            UPDATE_DESC                               *
***********************************************************************/
static void update_from_ebox( int id )
{
STRING_CLASS str;

switch ( id )
    {
    case SENSOR_DESC_EBOX:
        CurrentAnalog[x].desc.get_text(SensorWindow, id);
        break;

    case LOW_VOLTS_EBOX:
        str.get_text( SensorWindow, id );
        if ( str.isnumeric() )
        {
            if (sensor_pt_offset >= (int) CurrentAnalog[x].s_voltages.size())
            {
                CurrentAnalog[x].s_voltages.push_back(str);
            }
            else
            {
                CurrentAnalog[x].s_voltages[sensor_pt_offset] = str;
            }
            CurrentAnalog[x].update();
        }
        else if ( str.isempty() )
        {
            if ( sensor_pt_offset < (int) CurrentAnalog[x].s_voltages.size() )
            {
                CurrentAnalog[x].s_voltages.erase(CurrentAnalog[x].s_voltages.begin() + sensor_pt_offset);
                CurrentAnalog[x].update();
            }
        }
        break;

    case LOW_VALUE_EBOX:
        str.get_text(SensorWindow, id);
        if ( str.isnumeric() )
        {
            if ( sensor_pt_offset >= (int) CurrentAnalog[x].values.size() )
            {
                CurrentAnalog[x].s_values.push_back(str);
                CurrentAnalog[x].values.push_back(str.real_value());
            }
            else
            {
                CurrentAnalog[x].s_values[sensor_pt_offset] = str;
                CurrentAnalog[x].values[sensor_pt_offset]   = str.real_value();
            }
            CurrentAnalog[x].update();
        }
        else if ( str.isempty() )
        {
            if ( sensor_pt_offset < (int) CurrentAnalog[x].values.size() )
            {
                CurrentAnalog[x].s_values.erase(CurrentAnalog[x].s_values.begin() + sensor_pt_offset);
                CurrentAnalog[x].values.erase(CurrentAnalog[x].values.begin() + sensor_pt_offset);
            }
        }
        break;

    case HIGH_VOLTS_EBOX:
        str.get_text( SensorWindow, id );
        if ( str.isnumeric() )
        {
            if ( sensor_pt_offset + 1 >= (int) CurrentAnalog[x].s_voltages.size() )
            {
                CurrentAnalog[x].s_voltages.push_back(str);
            }
            else
            {
                CurrentAnalog[x].s_voltages[sensor_pt_offset + 1] = str;
            }
            CurrentAnalog[x].update();
        }
        else if ( str.isempty() )
        {
            if ( sensor_pt_offset + 1 < (int) CurrentAnalog[x].s_voltages.size() )
            {
                CurrentAnalog[x].s_voltages.erase(CurrentAnalog[x].s_voltages.begin() + sensor_pt_offset + 1);
                CurrentAnalog[x].update();
            }
        }
        break;

    case HIGH_VALUE_EBOX:
        str.get_text( SensorWindow, id );
        if ( str.isnumeric() )
        {
            if ( sensor_pt_offset + 1 >= (int) CurrentAnalog[x].values.size() )
            {
                CurrentAnalog[x].s_values.push_back(str);
                CurrentAnalog[x].values.push_back(str.real_value());
            }
            else
            {
                CurrentAnalog[x].s_values[sensor_pt_offset + 1] = str;
                CurrentAnalog[x].values[sensor_pt_offset + 1]   = str.real_value();
            }
            CurrentAnalog[x].update();
        }
        else if ( str.isempty() )
        {
            if ( sensor_pt_offset + 1 < (int) CurrentAnalog[x].values.size() )
            {
                CurrentAnalog[x].s_values.erase(CurrentAnalog[x].s_values.begin() + sensor_pt_offset + 1);
                CurrentAnalog[x].values.erase(CurrentAnalog[x].values.begin() + sensor_pt_offset + 1);
            }
        }
        break;
    }
}

/***********************************************************************
*                            UPDATE_UNITS                              *
***********************************************************************/
static void update_units()
{
STRING_CLASS  s;

s = UnitsCbox.selected_text();
CurrentAnalog[x].units = units_id(s.text());
s.set_text( SensorWindow, UNITS_TBOX );
s.set_text( SensorWindow, TEST_UNITS_TBOX );
}

/***********************************************************************
*                            UPDATE_VARTYPE                            *
***********************************************************************/
static void update_vartype()
{
int i;
STRING_CLASS s;

s = VartypeCbox.selected_text();
for ( i=0; i<NofVartypes; i++ )
    {
    if ( s == Vartypes[i].name )
        {
        CurrentAnalog[x].vartype = Vartypes[i].value;
        fill_units();
        break;
        }
    }
}

/***********************************************************************
*                         FILL_SENSOR_LISTBOX                          *
***********************************************************************/
static void fill_sensor_listbox()
{
int i;

SensorListbox.redraw_off();
SensorListbox.empty();

for ( i = 0; i<CurrentAnalog.count(); i++ )
    SensorListbox.add( CurrentAnalog[i].desc.text() );

/*
----------------------------------------------
x is a global index to the analog sensor array
---------------------------------------------- */
if ( x < 0 || x >= CurrentAnalog.count() )
    x = 0;

SensorListbox.setcursel( x );
SensorListbox.redraw_on();

show_sensor();
}

/***********************************************************************
*                            LOAD_SENSORS                              *
***********************************************************************/
static void load_sensors()
{
x = 0;
fill_sensor_listbox();
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static void paint_me()
{
PAINTSTRUCT ps;

BeginPaint( MainWindow, &ps );
EndPaint( MainWindow, &ps );
}

/***********************************************************************
*                             ADD_SENSOR                               *
***********************************************************************/
static void add_sensor()
{
ANALOG_SENSOR_ENTRY ase;
int32 i;

/*
-----------------------
Copy the current sensor
----------------------- */
ase = CurrentAnalog[x];
i = CurrentAnalog.count();
CurrentAnalog.setsize(i + 1);
CurrentAnalog[i] = ase;

x = i;

fill_sensor_listbox();
}

/***********************************************************************
*                            FILL_VARTYPES                             *
***********************************************************************/
static void fill_vartypes()
{
short i;

if ( !Vartypes )
    return;

VartypeCbox.empty();
for ( i=0; i<NofVartypes; i++ )
    VartypeCbox.add( Vartypes[i].name );

VartypeCbox.set_current_index( 0 );
VartypeCbox.redraw_on();
}

/***********************************************************************
*                            LOAD_VARTYPES                             *
***********************************************************************/
static void load_vartypes()
{
int n;
NAME_CLASS s;
DB_TABLE   t;
VARTYPE_ENTRY * v;

if ( Vartypes )
    {
    delete[] Vartypes;
    Vartypes = 0;
    }

s.get_vartype_dbname();
if ( t.open(s, VARTYPE_RECLEN, PFL) )
    {
    n = t.nof_recs();
    NofVartypes = 0;
    if ( n > 0 )
        {
        /*
        --------------------------------
        Add one more for the "NONE" type
        -------------------------------- */
        n++;
        Vartypes = new VARTYPE_ENTRY[n];
        if ( Vartypes )
            {
            v = Vartypes;
            v->value = 0;
            lstrcpy( v->name, StringTable.find(TEXT("NONE_STRING")) );
            NofVartypes++;
            v++;
            while ( NofVartypes < n && t.get_next_record(FALSE) )
                {
                v->value = (BITSETYPE) t.get_long( VARTYPE_VALUE_OFFSET );
                t.get_alpha( v->name, VARTYPE_NAME_OFFSET, VARTYPE_NAME_LEN );
                v++;
                NofVartypes++;
                }
            }
        else
            {
            NofVartypes = 0;
            }
        }
    t.close();
    fill_vartypes();
    }
else
    {
    resource_message_box( TEXT("UNABLE_TO_CONTINUE"), TEXT("NO_VARTYPES") );
    }
}

/***********************************************************************
*                            DO_TEST_CHANGE                            *
***********************************************************************/
static void do_test_change( int id )
{
static bool setting_new_value = false;
STRING_CLASS s;
double y;

if ( setting_new_value )
    {
    setting_new_value = false;
    return;
    }

setting_new_value = true;

CurrentAnalog.find(x);
s.get_text( SensorWindow, id );

if ( !s.isempty() )
    {
    y = s.real_value();
    if ( id == TEST_VOLTS_EBOX )
        {
        y *= 4095.0;
        y /= CurrentAnalog.max_volts();
        y = CurrentAnalog.converted_value(y);
        s = y;
        }
    else
        {
        y = CurrentAnalog.raw_count(y);
        y *= CurrentAnalog[x].max_volts;
        y /= 4095.0;
        s = y;
        }
    }

if ( id == TEST_VOLTS_EBOX )
    id = TEST_VALUE_EBOX;
else
    id = TEST_VOLTS_EBOX;
s.set_text( SensorWindow, id );
}

/**********************************************************************
*                  UPDATE_ANALOG_SENSOR_STRING_SETTINGS               *
**********************************************************************/
static void update_analog_sensor_string_settings()
{
static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { TRANSDUCER_RANGES_STATIC, TEXT( "XDCR_RANGES" ) },
    { DESCRIPTION_STATIC,       TEXT( "DESCRIPTION" ) },
    { TEST_CALC_STATIC,         TEXT( "TEST_CALC" ) },
    { VARTYPE_STATIC,           TEXT( "VARIABLE_TYPE" ) },
    { ANALOG_SENSOR_STATIC,     TEXT( "ANALOG_SENSOR_STRING" ) },
    { VOLTS_STATIC,             TEXT( "VOLTS" ) },
    { UNITS_STATIC,             TEXT( "UNITS_STRING" ) },
    { TEST_VOLTS_STATIC,        TEXT( "VOLTS" ) },
    { LOW_STATIC,               TEXT( "POINT" ) },
    { HIGH_STATIC,              TEXT( "POINT" ) },
    { NOTE_STATIC,              TEXT( "RESTART_NOTE" ) },
    { DONT_DO_STATIC,           TEXT( "DONT_DO_NOTE" ) },
    { CREATE_SENSOR_PB,         TEXT( "CREATE_SENSOR" ) },
    { SAVE_CHANGES_BUTTON,      TEXT( "SAVE_CHANGES" ) }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);

WINDOW_CLASS w;

update_statics( SensorWindow, rlist, nr );

w = MainWindow;
MyWindowTitle.set_text( MainWindow );
w.refresh();

set_statics_font( SensorWindow, SENSOR_DESC_LISTBOX );
set_statics_font( SensorWindow, VARTYPE_CBOX );
set_statics_font( SensorWindow, UNITS_CBOX );
set_statics_font( SensorWindow, SENSOR_DESC_EBOX );
set_statics_font( SensorWindow, TEST_UNITS_TBOX  );
set_statics_font( SensorWindow, UNITS_TBOX  );
}

/***********************************************************************
*                      SHIFT_SENSOR_POINTS_LEFT                        *
***********************************************************************/
static void shift_sensor_pts_left( HWND hWnd )
{
    STRING_CLASS low_static_str;
    STRING_CLASS high_static_str;
    STRING_CLASS low_n_str;
    STRING_CLASS high_n_str;
    STRING_CLASS low_voltage;
    STRING_CLASS low_value;
    STRING_CLASS high_voltage;
    STRING_CLASS high_value;
    int low_ni;
    int high_ni;
    int nMax;
    int nPage;
    HWND sbHwnd;
    SCROLLINFO sensor_pt_sb_info;

    sensor_pt_offset++;

    low_ni  = sensor_pt_offset + 1;
    high_ni = sensor_pt_offset + 2;

    /*
    ------------------------
    Set static string labels
    ------------------------ */
    StringTable.find( low_static_str, TEXT("POINT") );
    high_static_str = low_static_str;

    low_n_str = low_ni;
    low_static_str += SpaceString;
    low_static_str += low_n_str;
    low_static_str.set_text( hWnd, LOW_STATIC );

    high_n_str = high_ni;
    high_static_str += SpaceString;
    high_static_str += high_n_str;
    high_static_str.set_text (hWnd, HIGH_STATIC );

    /*
    --------------------------
    Set volts and value eboxes
    -------------------------- */
    if ( sensor_pt_offset < (int) CurrentAnalog[x].s_voltages.size() )
    {
        low_voltage = CurrentAnalog[x].s_voltages[sensor_pt_offset];
    }
    else
    {
        low_voltage = EmptyString;
    }

    low_voltage.set_text( hWnd, LOW_VOLTS_EBOX );

    if ( sensor_pt_offset < (int) CurrentAnalog[x].s_values.size() )
    {
        low_value = CurrentAnalog[x].s_values[sensor_pt_offset];
    }
    else
    {
        low_value = EmptyString;
    }

    low_value.set_text( hWnd, LOW_VALUE_EBOX );

    if ( sensor_pt_offset + 1 < (int) CurrentAnalog[x].s_voltages.size() )
    {
        high_voltage = CurrentAnalog[x].s_voltages[sensor_pt_offset + 1];
    }
    else
    {
        high_voltage = EmptyString;
    }

    high_voltage.set_text( hWnd, HIGH_VOLTS_EBOX );

    if ( sensor_pt_offset + 1 < (int) CurrentAnalog[x].s_values.size() )
    {
        high_value = CurrentAnalog[x].s_values[sensor_pt_offset + 1];
    }
    else
    {
        high_value = EmptyString;
    }

    high_value.set_text( hWnd, HIGH_VALUE_EBOX );

    /*
    --------------------------------
    Set scrollbar range and position
    -------------------------------- */
    if ( sensor_pt_offset > (CurrentAnalog[x].nof_sensor_points - 2) )
    {
        nMax = sensor_pt_offset;
    }
    else
    {
        nMax = CurrentAnalog[x].nof_sensor_points - 2;
    }

    nPage = 1;
    sbHwnd = GetDlgItem( SensorWindow, SENSOR_PTS_SCROLLBAR );
    sensor_pt_sb_info.cbSize = sizeof(SCROLLINFO);
    sensor_pt_sb_info.fMask = SIF_ALL;
    sensor_pt_sb_info.nMin = 0;
    sensor_pt_sb_info.nMax = nMax;
    sensor_pt_sb_info.nPage = nPage;
    sensor_pt_sb_info.nPos = sensor_pt_offset;
    SetScrollInfo(sbHwnd, SB_CTL, &sensor_pt_sb_info, true);
}

/***********************************************************************
*                      SHIFT_SENSOR_POINTS_RIGHT                       *
***********************************************************************/
static void shift_sensor_pts_right( HWND hWnd )
{
    STRING_CLASS low_static_str;
    STRING_CLASS high_static_str;
    STRING_CLASS low_n_str;
    STRING_CLASS high_n_str;
    STRING_CLASS low_voltage;
    STRING_CLASS low_value;
    STRING_CLASS high_voltage;
    STRING_CLASS high_value;
    int low_ni;
    int high_ni;
    int nMax;
    int nPage;
    HWND sbHwnd;
    SCROLLINFO sensor_pt_sb_info;

    if ( sensor_pt_offset != 0 )
    {
        sensor_pt_offset--;
    }

    low_ni  = sensor_pt_offset + 1;
    high_ni = sensor_pt_offset + 2;

    /*
    ------------------------
    Set static string labels
    ------------------------ */
    low_static_str  = StringTable.find( TEXT("POINT") );
    high_static_str = low_static_str;

    low_n_str = low_ni;
    low_static_str += SpaceString;
    low_static_str += low_n_str;
    low_static_str.set_text( hWnd, LOW_STATIC );

    high_n_str = high_ni;
    high_static_str += SpaceString;
    high_static_str += high_n_str;
    high_static_str.set_text( hWnd, HIGH_STATIC );

    /*
    --------------------------
    Set volts and value eboxes
    -------------------------- */
    if ( sensor_pt_offset < (int) CurrentAnalog[x].s_voltages.size() )
    {
        low_voltage = CurrentAnalog[x].s_voltages[sensor_pt_offset];
    }
    else
    {
        low_voltage = EmptyString;
    }

    low_voltage.set_text( hWnd, LOW_VOLTS_EBOX );

    if ( sensor_pt_offset < (int) CurrentAnalog[x].s_values.size() )
    {
        low_value = CurrentAnalog[x].s_values[sensor_pt_offset];
    }
    else
    {
        low_value = EmptyString;
    }

    low_value.set_text( hWnd, LOW_VALUE_EBOX );

    if ( sensor_pt_offset + 1 < (int) CurrentAnalog[x].s_voltages.size() )
    {
        high_voltage = CurrentAnalog[x].s_voltages[sensor_pt_offset + 1];
    }
    else
    {
        high_voltage = EmptyString;
    }

    high_voltage.set_text( hWnd, HIGH_VOLTS_EBOX );

    if ( sensor_pt_offset + 1 < (int) CurrentAnalog[x].values.size() )
    {
        high_value = CurrentAnalog[x].s_values[sensor_pt_offset + 1];
    }
    else
    {
        high_value = EmptyString;
    }

    high_value.set_text( hWnd, HIGH_VALUE_EBOX );

    /*
    --------------------------------
    Set scrollbar range and position
    -------------------------------- */
    if ( sensor_pt_offset > (CurrentAnalog[x].nof_sensor_points - 2) )
    {
        nMax = sensor_pt_offset;
    }
    else
    {
        nMax = CurrentAnalog[x].nof_sensor_points - 2;
    }

    nPage = 1;
    sbHwnd = GetDlgItem( SensorWindow, SENSOR_PTS_SCROLLBAR );
    sensor_pt_sb_info.cbSize = sizeof(SCROLLINFO);
    sensor_pt_sb_info.fMask = SIF_ALL;
    sensor_pt_sb_info.nMin = 0;
    sensor_pt_sb_info.nMax = nMax;
    sensor_pt_sb_info.nPage = nPage;
    sensor_pt_sb_info.nPos = sensor_pt_offset;
    SetScrollInfo(sbHwnd, SB_CTL, &sensor_pt_sb_info, true);
}

/***********************************************************************
*                      SHIFT_SENSOR_POINTS                             *
***********************************************************************/
static void shift_sensor_pts( HWND hWnd )
{
    STRING_CLASS low_static_str;
    STRING_CLASS high_static_str;
    STRING_CLASS low_n_str;
    STRING_CLASS high_n_str;
    STRING_CLASS low_voltage;
    STRING_CLASS low_value;
    STRING_CLASS high_voltage;
    STRING_CLASS high_value;
    int low_ni;
    int high_ni;
    int nMax;
    int nPage;
    HWND sbHwnd;
    SCROLLINFO sensor_pt_sb_info;

    if ( sensor_pt_offset < 0 )
    {
        sensor_pt_offset = 0;
    }

    if ( sensor_pt_offset > CurrentAnalog[x].nof_sensor_points - 2 )
    {
        sensor_pt_offset = CurrentAnalog[x].nof_sensor_points - 2;
    }

    low_ni  = sensor_pt_offset + 1;
    high_ni = sensor_pt_offset + 2;

    /*
    ------------------------
    Set static string labels
    ------------------------ */
    low_static_str  = StringTable.find( TEXT("POINT") );
    high_static_str = low_static_str;

    low_n_str = low_ni;
    low_static_str += low_n_str;
    low_static_str.set_text( hWnd, LOW_STATIC );

    high_n_str = high_ni;
    high_static_str += high_n_str;
    high_static_str.set_text( hWnd, HIGH_STATIC );

    /*
    --------------------------
    Set volts and value eboxes
    -------------------------- */
    if ( sensor_pt_offset < (int)CurrentAnalog[x].s_voltages.size() )
    {
        low_voltage = CurrentAnalog[x].s_voltages[sensor_pt_offset];
    }
    else
    {
        low_voltage = EmptyString;
    }

    low_voltage.set_text( hWnd, LOW_VOLTS_EBOX );

    if ( sensor_pt_offset < (int) CurrentAnalog[x].values.size() )
    {
        low_value = CurrentAnalog[x].values[sensor_pt_offset];
    }
    else
    {
        low_value = EmptyString;
    }

    low_value.set_text( hWnd, LOW_VALUE_EBOX );

    if ( sensor_pt_offset + 1 < (int)CurrentAnalog[x].s_voltages.size() )
    {
        high_voltage = CurrentAnalog[x].s_voltages[sensor_pt_offset + 1];
    }
    else
    {
        high_voltage = EmptyString;
    }

    high_voltage.set_text( hWnd, HIGH_VOLTS_EBOX );

    if ( sensor_pt_offset + 1 < (int) CurrentAnalog[x].values.size() )
    {
        high_value = CurrentAnalog[x].values[sensor_pt_offset + 1];
    }
    else
    {
        high_value = EmptyString;
    }

    high_value.set_text( hWnd, HIGH_VALUE_EBOX );

    /*
    --------------------------------
    Set scrollbar range and position
    -------------------------------- */
    if ( sensor_pt_offset > (CurrentAnalog[x].nof_sensor_points - 2) )
    {
        nMax = sensor_pt_offset;
    }
    else
    {
        nMax = CurrentAnalog[x].nof_sensor_points - 2;
    }

    nPage = 1;
    sbHwnd = GetDlgItem( SensorWindow, SENSOR_PTS_SCROLLBAR );
    sensor_pt_sb_info.cbSize = sizeof(SCROLLINFO);
    sensor_pt_sb_info.fMask = SIF_ALL;
    sensor_pt_sb_info.nMin = 0;
    sensor_pt_sb_info.nMax = nMax;
    sensor_pt_sb_info.nPage = nPage;
    sensor_pt_sb_info.nPos = sensor_pt_offset;
    SetScrollInfo( sbHwnd, SB_CTL, &sensor_pt_sb_info, true );
}

/***********************************************************************
*                        validate_sensor_points                        *
***********************************************************************/
static BOOLEAN validate_sensor_points()
{
    int          i;
    int          n_volts;
    int          n_values;

    for (i = 0; i < CurrentAnalog.NofSensors; i++)
    {
        n_volts = CurrentAnalog.SensorArray[i].s_voltages.size();
        n_values = CurrentAnalog.SensorArray[i].values.size();

        if ( n_volts > n_values || n_values > n_volts )
        {
            resource_message_box( TEXT("UNABLE_TO_CONTINUE"), TEXT("WARNING_STRING") );
            return FALSE;
        }
    }

    return TRUE;
}

/***********************************************************************
*                      EDIT_ANALOG_SENSOR_DIALOG_PROC                  *
***********************************************************************/
BOOL CALLBACK edit_analog_sensor_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;
HWND sbHwnd;
SCROLLINFO sensor_pt_sb_info;
STRING_CLASS msg_box_str;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SensorWindow = hWnd;
        update_analog_sensor_string_settings();
        SensorListbox.init( SensorWindow, SENSOR_DESC_LISTBOX );
        UnitsCbox.init( SensorWindow, UNITS_CBOX );
        VartypeCbox.init( SensorWindow, VARTYPE_CBOX );
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        load_vartypes();
        if ( !Vartypes )
            SendMessage( MainWindow, WM_CLOSE, 0, 0L );
        load_sensors();
        SetFocus( GetDlgItem(SensorWindow, SENSOR_DESC_LISTBOX) );
        return TRUE;

    case WM_HSCROLL:
        switch (id)
        {
        case SB_LINELEFT:
            shift_sensor_pts_right(hWnd);
            break;
        case SB_LINERIGHT:
            shift_sensor_pts_left(hWnd);
            break;
        case SB_PAGELEFT:
            shift_sensor_pts_right(hWnd);
            break;
        case SB_PAGERIGHT:
            shift_sensor_pts_left(hWnd);
            break;
        case SB_THUMBTRACK:
            sbHwnd = GetDlgItem(SensorWindow, SENSOR_PTS_SCROLLBAR);
            sensor_pt_sb_info.cbSize = sizeof(SCROLLINFO);
            sensor_pt_sb_info.fMask = SIF_TRACKPOS;
            GetScrollInfo( sbHwnd, SB_CTL, &sensor_pt_sb_info );
            sensor_pt_offset = sensor_pt_sb_info.nTrackPos;
            shift_sensor_pts( SensorWindow );
        }
        break;

    case WM_COMMAND:
       switch ( id )
            {
            case SENSOR_DESC_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    x = SensorListbox.current_index();
                    show_sensor();
                    }
                break;

            case UNITS_CBOX:
                if ( cmd == LBN_SELCHANGE )
                    update_units();
                break;

            case VARTYPE_CBOX:
                if ( cmd == LBN_SELCHANGE )
                    update_vartype();
                break;

            case TEST_VOLTS_EBOX:
            case TEST_VALUE_EBOX:
                if ( cmd == EN_CHANGE )
                    do_test_change( id );
                break;

            case SENSOR_DESC_EBOX:
            case LOW_VOLTS_EBOX:
            case LOW_VALUE_EBOX:
            case HIGH_VOLTS_EBOX:
            case HIGH_VALUE_EBOX:
                if ( cmd == EN_CHANGE )
                    update_from_ebox( id );
                break;

            case CREATE_SENSOR_PB:
                add_sensor();
                break;

            case SAVE_CHANGES_BUTTON:
                if ( validate_sensor_points() )
                {
                    save_changes();
                    SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                }
                break;

            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                break;
            }
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        break;

    case WM_COMMAND:
        switch (id)
            {
            case MINIMIZE_MENU:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_CHOICE:
                SendMessage( hWnd, WM_CLOSE, 0, 0L );
                break;

            case CREATE_CHOICE:
                add_sensor();
                return 0;
            }

        break;

    case WM_PAINT:
        MainWindow = hWnd;
        paint_me();
        return 0;

    case WM_EV_SHUTDOWN:
        SendMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
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
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static bool init( LPSTR cmd_line, int cmd_show )
{
WNDCLASS wc;
COMPUTER_CLASS c;
NAME_CLASS s;
TCHAR *arg;

SensorWindow = 0;
c.startup();
units_startup();
arg = char_to_tchar( cmd_line );
ComputerName = arg;
s.get_asensor_csvname( ComputerName );
CurrentAnalog.get( s );
load_resources();
statics_startup();

MyWindowTitle = StringTable.find( TEXT("ANALOG_SENSOR_SETUP") );

if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return false;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MyClassName );
wc.lpszMenuName  = TEXT("GeneralMenu");
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    0, 0,                             // X,y
    460, 320,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

SensorWindow  = CreateDialog(
     MainInstance,
     TEXT("EDIT_ANALOG_SENSOR"),
     MainWindow,
    (DLGPROC) edit_analog_sensor_dialog_proc );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
return true;
}

/***********************************************************************
*                            VOID SHUTDOWN                             *
***********************************************************************/
static void shutdown()
{
COMPUTER_CLASS c;

units_shutdown();
c.shutdown();
statics_shutdown();

if ( Vartypes )
    delete[] Vartypes;
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;

MainInstance = this_instance;

if ( !init( cmd_line, cmd_show) )
    return 0;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    if ( (SensorWindow == 0) || !IsDialogMessage(SensorWindow,&msg) )
        {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }
    }

shutdown();
return(msg.wParam);
}
