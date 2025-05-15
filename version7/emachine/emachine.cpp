#include <windows.h>
#include <math.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\ftii.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\limit_switch_class.h"   // Included for choose_wire
#include "..\include\machine.h"
#include "..\include\part.h"                 // Included for choose_wire
#include "..\include\param.h"                // Included for choose_wire
#include "..\include\machname.h"
#include "..\include\setpoint.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\verrors.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"

#include "resource.h"

#define _MAIN_
#include "..\include\chars.h"
#include "..\include\events.h"

struct MACHINE_ENTRY
    {
    MACHINE_CLASS machine;
    BOOLEAN       has_changed;
    BOOLEAN       had_binary_valve;
    int           old_quadrature_divisor;
    };

int             NofMachines    = 0;
int             CurrentMachine = 0;
MACHINE_ENTRY * Machines       = 0;

HINSTANCE MainInstance;
HWND   MainWindow;
HWND   MachSetupWindow;
HWND   NewMachineWindow    = 0;
HWND   EditEquationsWindow = 0;

static BOOLEAN HaveSureTrakControl = FALSE;
static BOOLEAN HavePressureControl = FALSE;
static BOOLEAN HaveDigitalServoAmp = FALSE;  // Set by show_machine

static TCHAR ChooseWireResourceFile[] = TEXT( "choose_wire" );
static TCHAR CommonResourceFile[]   = TEXT( "common" );
static TCHAR DisplayIniFile[]       = TEXT( "display.ini" );
static TCHAR EmachineResourceFile[] = TEXT( "emachine" );
static TCHAR VisiTrakIniFile[]      = TEXT( "visitrak.ini" );
TCHAR ConfigSection[]               = TEXT( "Config" );
static TCHAR ChanPreConfigKey[]     = TEXT( "ChanPreConfig"  );
static TCHAR ChanPostConfigKey[]    = TEXT( "ChanPostConfig" );
static TCHAR CurrentLanguageKey[]   = TEXT( "CurrentLanguage" );
static TCHAR HavePressureControlKey[] = TEXT( "HavePressureControl" );
static TCHAR HaveSureTrakKey[]      = TEXT( "HaveSureTrak" );

short NofDCurves;
short * DCurves;

static  WNDPROC OldEditProc;

TCHAR MyClassName[]   = TEXT("EMachine");
STRING_CLASS MyWindowTitle;
TCHAR MyIconName[] = TEXT("EMACHINE_ICON" );

STRINGTABLE_CLASS StringTable;

static STRING_CLASS MyEmptyString;

/*
---------------------------------------------
Global Variables for choose_servo_dialog_proc
--------------------------------------------- */
static HANDLE Mvo_06596_Bmp     = 0;
static HANDLE Digital_Servo_Bmp = 0;

BOOL CALLBACK edit_dcurve_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

/***********************************************************************
*                            RESOURCE_STRING                           *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    catpath( fname, TEXT("v5help.hlp") );
    WinHelp( MainWindow, fname, helptype, context );
    }
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp()
{
DWORD context;
HWND  w;

w = GetActiveWindow();
if ( w == NewMachineWindow )
    context = NEW_MACHINE_HELP;
else if ( w == EditEquationsWindow )
    context = EDIT_DIFF_EQUATIONS_HELP;
else
    context = MACHINE_EDIT_OVERVIEW_HELP;

gethelp( HELP_CONTEXT, context );
}

/***********************************************************************
*                         CHECK_CURRENT_MACHINE                        *
***********************************************************************/
inline BOOLEAN check_current_machine()
{
if ( CurrentMachine >= 0 && CurrentMachine < NofMachines )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            GET_NEW_TYPE                              *
***********************************************************************/
static void get_new_type()
{
MACHINE_CLASS * m;

if ( check_current_machine() )
    {
    m = &Machines[CurrentMachine].machine;
    m->type &= ~(COLD_CHAMBER_TYPE | HOT_CHAMBER_TYPE );

    if ( IsDlgButtonChecked(MachSetupWindow, HOT_CHAMBER_RADIO_BUTTON) == BST_CHECKED )
        m->type |= HOT_CHAMBER_TYPE;
    else
        m->type |= COLD_CHAMBER_TYPE;

    Machines[CurrentMachine].has_changed = TRUE;
    }
}

/***********************************************************************
*                      GET_NEW_QUADRATURE_DIVISOR                      *
***********************************************************************/
static void get_new_quadrature_divisor()
{
MACHINE_CLASS * m;
int new_divisor;

if ( CurrentMachine < NofMachines )
    {
    m = &Machines[CurrentMachine].machine;
    new_divisor = get_int_from_ebox( MachSetupWindow, QUADRATURE_DIVISOR_EB );
    if ( new_divisor >= 1 && new_divisor<=255 )
        {
        m->quadrature_divisor = new_divisor;
        Machines[CurrentMachine].has_changed = TRUE;
        }
    else
        {
        MessageBox( MachSetupWindow, TEXT("Divisor must be 1 to 255"), TEXT("Quadrature Divisor Out Of Range"), MB_OK );
        }
    }
}

/***********************************************************************
*                            GET_NEW_PITCH                             *
***********************************************************************/
static void get_new_pitch()
{
MACHINE_CLASS * m;
LISTBOX_CLASS lb;

if ( CurrentMachine < NofMachines )
    {
    m = &Machines[CurrentMachine].machine;

    /*
    ----------------------
    Get selected rod pitch
    ---------------------- */
    lb.init( MachSetupWindow, ROD_PITCH_COMBO_BOX );
    m->rodpitch = lb.selected_text();
    Machines[CurrentMachine].has_changed = TRUE;
    }
}

/***********************************************************************
*                       NEED_FT2_RESET_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK need_ft2_reset_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int   id;

id  = LOWORD( wParam );
switch (msg)
    {
    case WM_INITDIALOG:
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                break;
            }
        return TRUE;
    }

return FALSE;
}


/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static BOOLEAN save_changes()
{
static TCHAR digital_curves[]  = TEXT( "1400" );
static TCHAR external_curves[] = TEXT( "3000" );
static TCHAR enable_valve_setting_after_jog[] = TEXT(".096 UD1=V505 #D0");

short             i;
double            x;
TCHAR           * cp;
AUTO_BACKUP_CLASS ab;
COMPUTER_CLASS    c;
FILE_CLASS        f;
MACHINE_CLASS   * m;
BITSETYPE         pressure_control_mask;
BOOLEAN           have_digital_servo_amp_now;
BOOLEAN           has_binary_valve;
BOOLEAN           need_part_update;
STRING_CLASS      title;
STRING_CLASS      prompt;
BOOLEAN           need_to_redisplay_machine;
NAME_CLASS        s;
FTII_VAR_ARRAY    va;

need_to_redisplay_machine = FALSE;
pressure_control_mask = MA_HAS_PRESSURE_CONTROL;

m = &Machines[CurrentMachine].machine;
if ( m->monitor_flags & pressure_control_mask )
    {
    if ( !HavePressureControl )
        {
        m->monitor_flags &= ~pressure_control_mask;
        Machines[CurrentMachine].has_changed = TRUE;
        }
    }
else if ( HavePressureControl )
    {
    m->monitor_flags |= pressure_control_mask;
    Machines[CurrentMachine].has_changed = TRUE;
    }

if ( m->monitor_flags & MA_HAS_DIGITAL_SERVO )
    have_digital_servo_amp_now = TRUE;
else
    have_digital_servo_amp_now = FALSE;

if (have_digital_servo_amp_now != HaveDigitalServoAmp )
    {
    HaveDigitalServoAmp = have_digital_servo_amp_now;
    if ( HaveDigitalServoAmp )
        cp = digital_curves;
    else
        cp = external_curves;
    put_ini_string( DisplayIniFile, ConfigSection, ChanPreConfigKey, cp );
    put_ini_string( DisplayIniFile, ConfigSection, ChanPostConfigKey, cp );
    }

ab.get_backup_dir();

for ( i=0; i<NofMachines; i++ )
    {
    if ( Machines[i].has_changed )
        {
        m = &Machines[i].machine;

        has_binary_valve = m->has_binary_valve();
        if ( Machines[i].had_binary_valve != has_binary_valve )
            {
            if ( has_binary_valve )
                {
                need_part_update = FALSE;
                title = StringTable.find( TEXT("DEFAULT_HMI_SETTINGS_UPDATE") );
                prompt = StringTable.find( TEXT("UPDATE_HMI_PART_SETTINGS") );
                if ( MessageBox(MachSetupWindow, prompt.text(), title.text(),  MB_YESNO | MB_SYSTEMMODAL) == IDYES )
                    need_part_update = TRUE;
                machine_to_hmi( Machines[i].machine, need_part_update );
                if ( need_part_update )
                    {
                    title = StringTable.find( TEXT("CHANGES_COMPLETE") );
                    prompt = StringTable.find( TEXT("SAVE_ALL_PARTS_REQUEST") );
                    MessageBox( MachSetupWindow, prompt.text(), title.text(),  MB_OK | MB_SYSTEMMODAL );
                    }
                }
            else
                {
                /*
                -------------------------------------------------------
                At the present this only affects binary valve settings.
                ------------------------------------------------------- */
                remove_hmi_from_machine( m->computer, m->name );
                }

            need_to_redisplay_machine = TRUE;
            }

        s.get_ft2_machine_settings_file_name( m->computer, m->name );
        va.get( s );
        if ( has_binary_valve )
            {
            /*
            ---------------------------
            Save the binary valve setup
            --------------------------- */
            va.update( VALVE_TYPES_1_VN, unsigned(PRINCE_BINARY_VALVE_TYPE) );

            x = m->pulse_time.real_value();
            x *= 1000000.0;
            x = floor( x );
            va.update( PULSE_TIME_VN, unsigned(x) );

            x = m->devent_time.real_value();
            x *= 1000000.0;
            x = floor( x );
            va.update( DEVENT_TIME_VN, unsigned(x) );

            va.update( PULSE_WIRE_VN, mask_from_wire(m->pulse_wire.uint_value()) );
            }
        va.update( VALVE_SETTING_AFTER_JOG_SHOT_VN, m->valve_setting_after_jog_shot.real_value() );
        va.put( s );

        m->save();
        Machines[i].had_binary_valve     = has_binary_valve;
        Machines[i].has_changed = FALSE;
        poke_data( DDE_CONFIG_TOPIC,  ItemList[MACH_SETUP_INDEX].name, m->name.text() );
        if ( Machines[i].machine.monitor_flags & MA_MONITORING_ON )
            poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, m->name.text() );
        if ( c.find(m->computer) )
            {
            if ( c.is_this_computer() )
                ab.backup_machine( m->name );
            }
        if ( m->quadrature_divisor != Machines[i].old_quadrature_divisor )
            {
            Machines[i].old_quadrature_divisor = m->quadrature_divisor;
            DialogBox(
                MainInstance,
                TEXT("NEED_FT2_RESET_DIALOG"),
                MainWindow,
                (DLGPROC) need_ft2_reset_dialog_proc );
            }
        }

    s.get_machine_ctrl_prog_steps_file_name( m->computer, m->name );
    if ( m->suretrak_controlled && !is_zero(m->valve_setting_after_jog_shot.real_value()) )
        {
        f.open_for_write(s);
        f.writeline( enable_valve_setting_after_jog );
        f.close();
        }
    else
        {
        s.delete_file();
        }
    }

return need_to_redisplay_machine;
}

/***********************************************************************
*                     SET_DIFF_BUTTON_ENABLE_STATE                     *
***********************************************************************/
void set_diff_button_enable_state( short dcurve_number )
{
BOOL    is_enabled;
TCHAR   buf[MAX_STRING_LEN+1];

if ( dcurve_number == NO_DIFF_CURVE_NUMBER )
    is_enabled = FALSE;
else
    is_enabled = TRUE;

EnableWindow( GetDlgItem(MachSetupWindow,DIFF_PRESS_RADIO_BUTTON), is_enabled );
if ( !is_enabled && IsDlgButtonChecked(MachSetupWindow, DIFF_PRESS_RADIO_BUTTON) == BST_CHECKED )
    {
    LoadString( MainInstance, NEED_NEW_PARM_TYPE_STRING, buf, sizeof(buf) );
    MessageBox( NULL, buf, TEXT(""),  MB_OK | MB_ICONWARNING );
    }
}

/***********************************************************************
*                        SHOW_ADV_SETUP                                *
***********************************************************************/
void show_adv_setup()
{
short i;
short on_button;
MACHINE_CLASS * m;

if ( !check_current_machine() )
    return;

m = &Machines[CurrentMachine].machine;

if ( m->impact_pres_type[POST_IMPACT_INDEX] & ROD_PRESSURE_CURVE )
    on_button = ROD_PRESS_RADIO_BUTTON;

else if ( m->impact_pres_type[POST_IMPACT_INDEX] & HEAD_PRESSURE_CURVE )
    on_button = HEAD_PRESS_RADIO_BUTTON;

else
    on_button = DIFF_PRESS_RADIO_BUTTON;

CheckRadioButton( MachSetupWindow, ROD_PRESS_RADIO_BUTTON, DIFF_PRESS_RADIO_BUTTON, on_button );

SetDlgItemInt( MachSetupWindow, POS_IMPACT_POINTS_BOX,  m->impact_points[PRE_IMPACT_INDEX],  TRUE );
SetDlgItemInt( MachSetupWindow, TIME_IMPACT_POINTS_BOX, m->impact_points[POST_IMPACT_INDEX], TRUE );
SetDlgItemInt( MachSetupWindow, CYCLE_TIMEOUT_BOX,      m->cycle_timeout_seconds,            TRUE );
SetDlgItemInt( MachSetupWindow, AUTOSHOT_TIMEOUT_BOX,   m->autoshot_timeout_seconds,         TRUE );

for ( i=0; i<NofDCurves; i++)
   {
   /*
   ---------------------------------
   Find CB index for this curve type
   --------------------------------- */
   if ( DCurves[i] == m->diff_curve_number )
      {
      /*
      -----------------------------
      Select diff curve description
      ----------------------------- */
      SendDlgItemMessage( MachSetupWindow, DIFF_CURVE_TYPE_COMBO_BOX, CB_SETCURSEL, i, 0L);
      set_diff_button_enable_state( DCurves[i] );
      break;
      }
   }
}

/***********************************************************************
*                         SHOW_SERVO_CONTROLS                          *
***********************************************************************/
static void show_servo_controls()
{
BOOL    is_enabled;
int     on_button;
MACHINE_CLASS * m;

m = &Machines[CurrentMachine].machine;

if ( m->suretrak_controlled )
    is_enabled = TRUE;
else
    is_enabled = FALSE;

EnableWindow( GetDlgItem(MachSetupWindow, MVO_06596_RADIO), is_enabled );
EnableWindow( GetDlgItem(MachSetupWindow, DIGITAL_SERVO_RADIO), is_enabled );
EnableWindow( GetDlgItem(MachSetupWindow, SERVO_AMP_BOARD_TYPE_STATIC), is_enabled );
EnableWindow( GetDlgItem(MachSetupWindow, CHOOSE_SERVO_PB), is_enabled );
EnableWindow( GetDlgItem(MachSetupWindow, VALVE_SETTING_AFTER_JOG_SHOT_STATIC), is_enabled );
EnableWindow( GetDlgItem(MachSetupWindow, VALVE_SETTING_AFTER_JOG_SHOT_EB), is_enabled );

if ( is_enabled && (m->monitor_flags & MA_HAS_DIGITAL_SERVO) )
    on_button = DIGITAL_SERVO_RADIO;
else
    on_button = MVO_06596_RADIO;

CheckRadioButton( MachSetupWindow, MVO_06596_RADIO, DIGITAL_SERVO_RADIO, on_button );
}

/***********************************************************************
*                      SHOW_BINARY_VALVE_CONTROLS                      *
***********************************************************************/
static void show_binary_valve_controls( MACHINE_CLASS & m )
{
m.devent_time.set_text( MachSetupWindow, BINARY_VALVE_DEVENT_EB );
m.pulse_time.set_text(  MachSetupWindow, BINARY_VALVE_PULSE_EB  );
m.pulse_wire.set_text(  MachSetupWindow, BINARY_VALVE_OUTPUT_PB );
}

/***********************************************************************
*                     ENABLE_BINARY_VALVE_CONTROLS                     *
***********************************************************************/
static void enable_binary_valve_controls()
{
static int binary_valve_id[] = { BINARY_VALVE_OUTPUT_PB, BINARY_VALVE_OUTPUT_STATIC, BINARY_VALVE_DEVENT_EB,
                                 BINARY_VALVE_DEVENT_STATIC, BINARY_VALVE_PULSE_EB, BINARY_VALVE_PULSE_STATIC };

const int NOF_BINARY_VALVE_WINDOWS = sizeof(binary_valve_id)/sizeof(int);

int             i;
BOOL            is_enabled;
HWND            has_hmi_xb;
HWND            has_binary_valve_xb;
HWND            binary_valve_window[NOF_BINARY_VALVE_WINDOWS];
NAME_CLASS      s;
SETPOINT_CLASS  sp;

has_hmi_xb = GetDlgItem( MachSetupWindow, HAS_HMI_XBOX );
has_binary_valve_xb = GetDlgItem( MachSetupWindow, HAS_BINARY_VALVE_XBOX );

for ( i=0; i<NOF_BINARY_VALVE_WINDOWS; i++ )
    binary_valve_window[i] = GetDlgItem( MachSetupWindow, binary_valve_id[i] );

if ( !is_checked(has_hmi_xb) )
    {
    set_checkbox( has_binary_valve_xb, FALSE );
    EnableWindow(has_binary_valve_xb, FALSE );
    for ( i=0; i<NOF_BINARY_VALVE_WINDOWS; i++ )
        EnableWindow(binary_valve_window[i], FALSE );
    return;
    }

if ( !IsWindowEnabled(has_binary_valve_xb) )
    EnableWindow( has_binary_valve_xb, TRUE );

is_enabled = FALSE;
if ( is_checked(has_binary_valve_xb) )
    is_enabled = TRUE;

for ( i=0; i<NOF_BINARY_VALVE_WINDOWS; i++ )
    EnableWindow(binary_valve_window[i], is_enabled );
}

/***********************************************************************
*                            SHOW_MACHINE                              *
***********************************************************************/
static void show_machine()
{
int     on_button;
MACHINE_CLASS * m;
BOOLEAN         b;
WINDOW_CLASS    w;

if ( !check_current_machine() )
    return;

m = &Machines[CurrentMachine].machine;

set_current_cb_item( MachSetupWindow, ROD_PITCH_COMBO_BOX, m->rodpitch.text() );
set_text( MachSetupWindow, QUADRATURE_DIVISOR_EB, int32toasc(m->quadrature_divisor) );
if ( m->type & COLD_CHAMBER_TYPE )
    on_button  = COLD_CHAMBER_RADIO_BUTTON;
else
    on_button  = HOT_CHAMBER_RADIO_BUTTON;

CheckRadioButton( MachSetupWindow, HOT_CHAMBER_RADIO_BUTTON, COLD_CHAMBER_RADIO_BUTTON, on_button );

set_checkbox( MachSetupWindow, SURETRAK2_XBOX, m->suretrak_controlled );
set_checkbox( MachSetupWindow, HAS_BINARY_VALVE_XBOX,   m->has_binary_valve() );
set_checkbox( MachSetupWindow, HAS_HMI_XBOX,            m->has_hmi() );
set_checkbox( MachSetupWindow, LS_OFF_AT_EOS_XBOX, m->ls_off_at_eos() );
enable_binary_valve_controls();

w = MachSetupWindow;
w = w.control( MEAS_BISCUIT_AT_DELAY_XBOX );
if ( m->is_ftii )
    {
    if ( !w.is_enabled() )
        w.enable();
    }

b = FALSE;
if ( m->is_ftii && (m->monitor_flags & MA_USE_TIME_TO_MEASURE_BISCUIT) )
    b = TRUE;
set_checkbox( MachSetupWindow, MEAS_BISCUIT_AT_DELAY_XBOX, b );

if ( !m->is_ftii )
    {
    if ( w.is_enabled() )
        w.disable();
    }

show_servo_controls();
show_binary_valve_controls( *m );
if ( m->valve_setting_after_jog_shot.isempty() )
    set_text( MachSetupWindow, VALVE_SETTING_AFTER_JOG_SHOT_EB, EmptyString );
else
    m->valve_setting_after_jog_shot.set_text(  MachSetupWindow, VALVE_SETTING_AFTER_JOG_SHOT_EB );

/*
-----------------------------------------------------------------------------------
Save the initial setting of the servo amp board type. If this changes then I should
change the curves displayed on the plot screen.
----------------------------------------------------------------------------------- */
if ( m->monitor_flags & MA_HAS_DIGITAL_SERVO )
    HaveDigitalServoAmp = TRUE;
else
    HaveDigitalServoAmp = FALSE;

show_adv_setup();
}

/***********************************************************************
*                           CLEANUP_MACHINES                           *
***********************************************************************/
static void cleanup_machines()
{
if ( Machines )
    {
    delete[] Machines;
    Machines    = 0;
    NofMachines = 0;
    }
}

/***********************************************************************
*                            CLEANUP_DCURVES                           *
***********************************************************************/
void cleanup_dcurves()
{

if ( DCurves )
    {
    delete[] DCurves;
    DCurves    = 0;
    NofDCurves = 0;
    }
}

/***********************************************************************
*                            FILL_MACHINES                             *
***********************************************************************/
void fill_machines( STRING_CLASS & current_machine_name )
{
BOOLEAN                 have_current_machine;
int                     i;
COMPUTER_CLASS          c;
MACHINE_ENTRY         * m;
STRING_CLASS            mymachine;
LISTBOX_CLASS           lb;
MACHINE_NAME_LIST_CLASS mlist;

if ( !current_machine_name.isempty() )
    mymachine = current_machine_name;
else if ( CurrentMachine >= 0 && CurrentMachine < NofMachines )
    mymachine = Machines[CurrentMachine].machine.name;
else
    mymachine.null();

lb.init( MachSetupWindow, MACHINE_LIST_BOX );
lb.empty();
lb.redraw_off();

cleanup_machines();

NofMachines = 0;
c.load();
c.rewind();
while( c.next() )
    {
    if ( c.is_present() )
        mlist.add_computer( c.name() );
    }

if ( mlist.count() > 0 )
    {
    NofMachines = mlist.count();
    Machines = new MACHINE_ENTRY[NofMachines];
    }

i = 0;
m = Machines;

mlist.rewind();
while ( mlist.next() )
    {
    if ( i >= NofMachines )
        break;
    m->machine.computer = mlist.computer_name();
    m->machine.name     = mlist.name();
    i++;
    m++;
    }

NofMachines = i;

have_current_machine = FALSE;
m = Machines;
for ( i=0; i<NofMachines; i++ )
    {
    m->machine.find( m->machine.computer, m->machine.name );
    m->has_changed = FALSE;
    m->had_binary_valve     = m->machine.has_binary_valve();
    m->old_quadrature_divisor = m->machine.quadrature_divisor;
    lb.add( m->machine.name.text() );
    if ( m->machine.name == mymachine )
        {
        CurrentMachine = i;
        have_current_machine = TRUE;
        }
    m++;
    }

if ( !have_current_machine )
    CurrentMachine = 0;

if ( NofMachines )
    lb.setcursel( Machines[CurrentMachine].machine.name.text() );

lb.redraw_on();
show_machine();
}

/***********************************************************************
*                            FILL_ROD_PITCHES                          *
***********************************************************************/
void fill_rod_pitches()
{
NAME_CLASS s;
DB_TABLE   t;
LISTBOX_CLASS lb;

lb.init( MachSetupWindow, ROD_PITCH_COMBO_BOX );
lb.empty();
lb.redraw_off();

s.get_rodpitch_dbname();
if ( t.open(s, RODPITCH_RECLEN, PFL) )
    {
    while ( t.get_next_record(FALSE) )
        {
        t.get_alpha( s, RODPITCH_NAME_OFFSET, RODPITCH_NAME_LEN );
        lb.add( s.text() );
        }
    t.close();
    }

lb.redraw_on();
}

/***********************************************************************
*                             ADD_MACHINE                              *
***********************************************************************/
void add_machine( STRING_CLASS & machine )
{
COMPUTER_CLASS c;

if ( !check_current_machine() )
    return;

copymachine( c.whoami(), machine, Machines[CurrentMachine].machine );

fill_machines( machine );
}

/***********************************************************************
*                            DELETE_MACHINE                            *
***********************************************************************/
void delete_machine()
{
INT          answer_button;
BOOLEAN      need_update;
STRING_CLASS machine;
STRING_CLASS computer;
NAME_CLASS   s;
DB_TABLE     t;
MACHINE_CLASS * m;

need_update = FALSE;

if ( NofMachines < 2 )
    {
    resource_message_box( MainInstance, LAST_MACHINE_STRING, UNABLE_TO_COMPLY_STRING, MB_OK );
    return;
    }

/*
----------------------------------------------------------
Don't allow deletion of a machine that might be monitoring
---------------------------------------------------------- */
m = &Machines[CurrentMachine].machine;
m->refresh_monitor_flags();
if ( m->monitor_flags & MA_MONITORING_ON )
    {
    resource_message_box( MainInstance, MONITORING_OFF_FIRST_STRING, UNABLE_TO_COMPLY_STRING, MB_OK );
    return;
    }

answer_button = resource_message_box( MainInstance, CONTINUE_STRING, ABOUT_TO_DELETE_STRING, MB_OKCANCEL | MB_ICONWARNING );
if ( answer_button != IDOK )
    return;

if ( !check_current_machine() )
    return;

machine  = Machines[CurrentMachine].machine.name;
computer = Machines[CurrentMachine].machine.computer;

s.get_machine_directory( computer, machine );
s.delete_directory();
s.null();
fill_machines(s);

poke_data( DDE_CONFIG_TOPIC, ItemList[MACH_SETUP_INDEX].name, machine.text() );
}

/***********************************************************************
*                              FILL_DCURVES                            *
***********************************************************************/
void fill_dcurves()
{
int           i;
NAME_CLASS    s;
DB_TABLE      t;
LISTBOX_CLASS lb;

lb.init( MachSetupWindow, DIFF_CURVE_TYPE_COMBO_BOX );
lb.empty();
lb.redraw_off();

cleanup_dcurves();

s.get_data_dir_file_name( DCURVE_DB );
if ( t.open(s, DCURVE_RECLEN, PFL ) )
    {
    NofDCurves = t.nof_recs();
    if ( NofDCurves > 0 )
        {
        DCurves = new short[NofDCurves];
        }

    if ( NofDCurves > 0 )
        {
        i = 0;
        while ( t.get_next_record(FALSE) && i < NofDCurves )
            {
            DCurves[i] = t.get_short( DCURVE_CURVE_NUMBER_OFFSET );
            i++;
            t.get_alpha( s, DCURVE_DESC_OFFSET, DCURVE_DESC_LEN );
            lb.add( s.text() );
            }
        }
   t.close();
   }

lb.setcursel( 0 );
lb.redraw_on();
}

/***********************************************************************
*                            UPDATE_DCURVE                             *
***********************************************************************/
void update_dcurve()
{
LRESULT x;
short   i;

if ( !check_current_machine() )
    return;

x = SendDlgItemMessage( MachSetupWindow, DIFF_CURVE_TYPE_COMBO_BOX, CB_GETCURSEL, 0, 0L );
if ( x != CB_ERR )
    {
    i = short( x );
    if ( i >= 0 && i < NofDCurves )
        {
        Machines[CurrentMachine].machine.diff_curve_number = DCurves[i];
        Machines[CurrentMachine].has_changed = TRUE;
        set_diff_button_enable_state( i );
        }
    }
}

/***********************************************************************
*                         POSITION_ADV_SETUP                           *
***********************************************************************/
void position_adv_setup()
{

RECT  r;
long  x;
long  y;

GetWindowRect( MainWindow, &r );
x = r.right;
y = r.top;

if ( MachSetupWindow )
    {
    GetWindowRect( MachSetupWindow, &r );
    MoveWindow( MachSetupWindow, x, y, r.right-r.left, r.bottom-r.top, TRUE );
    }

}

/***********************************************************************
*                          SHOW_CONTEXT_HELP                           *
***********************************************************************/
static BOOL show_context_help( HWND w, LPARAM lParam )
{
const INT NOF_CONTROLS = 12;
const INT NOF_STATIC_CONTROLS = 12;

static UINT myid[] = {
    MACHINE_LIST_BOX,
    HOT_CHAMBER_RADIO_BUTTON,
    COLD_CHAMBER_RADIO_BUTTON,
    DIFF_CURVE_TYPE_COMBO_BOX,
    ROD_PITCH_COMBO_BOX,
    HAS_HMI_XBOX,
    HAS_BINARY_VALVE_XBOX,
    SURETRAK2_XBOX,
    MEAS_BISCUIT_AT_DELAY_XBOX,
    LS_OFF_AT_EOS_XBOX,
    BINARY_VALVE_OUTPUT_PB,
    BINARY_VALVE_PULSE_EB,
    VALVE_SETTING_AFTER_JOG_SHOT_EB
    };

static UINT static_id[] = {
    MACHINE_NUMBER_STATIC,
    ROD_PITCH_STATIC,
    MACHINE_TYPE_STATIC,
    DIFF_CURVE_TYPE_STATIC,
    TIMEOUTS_STATIC,
    IMPACT_POINTS_STATIC,
    CALC_PARAMS_STATIC,
    QUADRATURE_DIVISOR_STATIC,
    VALVE_SETTING_AFTER_JOG_SHOT_STATIC,
    BINARY_VALVE_OUTPUT_STATIC,
    BINARY_VALVE_DEVENT_STATIC,
    BINARY_VALVE_PULSE_STATIC
    };

static DWORD mycontext[] = {
    MACHINE_NUMBER_HELP,
    MACHINE_TYPE_HELP,
    MACHINE_TYPE_HELP,
    DIFF_CURVE_TYPE_HELP,
    ROD_PITCH_HELP,
    HAS_HMI_HELP,
    HAS_BINARY_VALVE_HELP,
    SURETRAK2_XBOX_HELP,
    MEAS_BISCUIT_AT_DELAY_XBOX_HELP,
    LS_OFF_AT_EOS_HELP,
    BINARY_VALVE_OUTPUT_HELP,
    PULSE_TIME_HELP,
    VALVE_SETTING_AFTER_JOG_SHOT_HELP
    };

static DWORD static_context[] = {
    MACHINE_NUMBER_HELP,
    ROD_PITCH_HELP,
    MACHINE_TYPE_HELP,
    DIFF_CURVE_TYPE_HELP,
    TIMEOUTS_HELP,
    IMPACT_POINTS_HELP,
    CALC_PARAMS_HELP,
    QUADRATURE_DIVISOR_HELP,
    VALVE_SETTING_AFTER_JOG_SHOT_HELP,
    BINARY_VALVE_OUTPUT_HELP,
    DEVENT_TIME_HELP,
    PULSE_TIME_HELP
    };

INT   i;
POINT p;
HWND  sw;
RECT  r;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    sw = GetDlgItem(MachSetupWindow, static_id[i] );
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
    if ( GetDlgItem(MachSetupWindow, myid[i]) == w )
        {
        gethelp( HELP_CONTEXTPOPUP, mycontext[i] );
        return TRUE;
        }
    }

gethelp( HELP_CONTEXT, MACHINE_EDIT_OVERVIEW_HELP );
return TRUE;
}

/***********************************************************************
*                         GET_SELECTED_MACHINE                         *
***********************************************************************/
static void get_selected_machine()
{
short         i;
TCHAR       * cp;
LISTBOX_CLASS lb;

lb.init( MachSetupWindow, MACHINE_LIST_BOX );
cp = lb.selected_text();

for ( i=0; i<NofMachines; i++ )
    {
    /*
    -----------------------------------------------
    The first thing on the line is the machine name
    ----------------------------------------------- */
    if ( Machines[i].machine.name == cp )
        {
        CurrentMachine = i;
        show_machine();
        }
    }
}

/***********************************************************************
*                     END_CHOOSE_SERVO_DIALOG_PROC                     *
***********************************************************************/
void end_choose_servo_dialog_proc( HWND hWnd )
{
HWND w;

if ( Mvo_06596_Bmp )
    {
    w = GetDlgItem( hWnd, MVO_06596_PB );
    SendMessage( w, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE) 0 );
    DeleteObject( Mvo_06596_Bmp );
    Mvo_06596_Bmp = 0;
    }

if ( Digital_Servo_Bmp )
    {
    w = GetDlgItem( hWnd, DIGITAL_SERVO_PB );
    SendMessage( w, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE) 0 );
    DeleteObject( Digital_Servo_Bmp );
    Digital_Servo_Bmp = 0;
    }

EndDialog( hWnd, 0 );
}

/***********************************************************************
*                       CHOOSE_SERVO_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK choose_servo_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int   id;
HWND  w;

id  = LOWORD( wParam );
switch (msg)
    {
    case WM_INITDIALOG:
        w = GetDlgItem( hWnd, MVO_06596_PB );
        Mvo_06596_Bmp   = LoadBitmap( MainInstance, TEXT("Mvo_06596_Bmp") );
        SendMessage( w, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE) Mvo_06596_Bmp );
        w = GetDlgItem( hWnd, DIGITAL_SERVO_PB );
        Digital_Servo_Bmp   = LoadBitmap( MainInstance, TEXT("Digital_Servo_Bmp") );
        SendMessage( w, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE) Digital_Servo_Bmp );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case MVO_06596_PB:
                CheckRadioButton( MachSetupWindow, MVO_06596_RADIO, DIGITAL_SERVO_RADIO, MVO_06596_RADIO );
                Machines[CurrentMachine].machine.monitor_flags &= ~MA_HAS_DIGITAL_SERVO;
                Machines[CurrentMachine].has_changed = TRUE;
                end_choose_servo_dialog_proc( hWnd );
                break;

            case DIGITAL_SERVO_PB:
                CheckRadioButton( MachSetupWindow, MVO_06596_RADIO, DIGITAL_SERVO_RADIO, DIGITAL_SERVO_RADIO );
                Machines[CurrentMachine].machine.monitor_flags |= MA_HAS_DIGITAL_SERVO;
                Machines[CurrentMachine].has_changed = TRUE;
                end_choose_servo_dialog_proc( hWnd );
                break;

            case CANCEL_BUTTON:
                end_choose_servo_dialog_proc( hWnd );
                break;
            }
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                   GET_NEW_BINARY_VALVE_PULSE_WIRE                    *
***********************************************************************/
static void get_new_binary_valve_pulse_wire()
{
Machines[CurrentMachine].machine.pulse_wire.get_text( MachSetupWindow, BINARY_VALVE_OUTPUT_PB );
Machines[CurrentMachine].has_changed = TRUE;
}

/***********************************************************************
*                             MachSetupProc                            *
***********************************************************************/
BOOL CALLBACK MachSetupProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
BOOLEAN has_binary_valve;
BOOLEAN has_hmi;
int     i;
int     id;
int     cmd;
MACHINE_CLASS * m;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );
m   = 0;

switch (msg)
    {
    case WM_CONTEXTMENU:
        return show_context_help( (HWND) wParam, lParam );

    case WM_INITDIALOG:
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_NEWSETUP:
        fill_dcurves();
        show_adv_setup();
        return TRUE;

    case WM_DBINIT:
        for ( i=0; i<4; i++ )
            SendDlgItemMessage( MachSetupWindow, POS_IMPACT_POINTS_BOX+i, EM_LIMITTEXT, 5, 0L );
        return TRUE;

    case WM_COMMAND:
        if ( check_current_machine() )
            m = &Machines[CurrentMachine].machine;

        switch ( id )
            {
            case MACHINE_LIST_BOX:
                if ( cmd == LBN_SELCHANGE )
                    get_selected_machine();
                break;

            case ROD_PITCH_COMBO_BOX:
                if ( cmd == LBN_SELCHANGE )
                    get_new_pitch();
                break;

            case HOT_CHAMBER_RADIO_BUTTON:
            case COLD_CHAMBER_RADIO_BUTTON:
                get_new_type();
                break;

            case DIFF_CURVE_TYPE_COMBO_BOX:
                if ( cmd == LBN_SELCHANGE )
                    update_dcurve();
                break;

            case LS_OFF_AT_EOS_XBOX:
                m->set_ls_off_at_eos( is_checked(hWnd, LS_OFF_AT_EOS_XBOX) );
                Machines[CurrentMachine].has_changed = TRUE;
                break;

            case HAS_BINARY_VALVE_XBOX:
                m->set_has_binary_valve( is_checked(hWnd, HAS_BINARY_VALVE_XBOX) );
                Machines[CurrentMachine].has_changed = TRUE;
                enable_binary_valve_controls();
                break;

            case HAS_HMI_XBOX:
                has_hmi = is_checked( hWnd, HAS_HMI_XBOX );
                has_binary_valve = is_checked( hWnd, HAS_BINARY_VALVE_XBOX );
                m->set_has_hmi( has_hmi );
                Machines[CurrentMachine].has_changed = TRUE;
                enable_binary_valve_controls();
                if ( !has_hmi && has_binary_valve )
                    m->set_has_binary_valve( FALSE );
                break;

            case SURETRAK2_XBOX:
                if ( m )
                    {
                    m->suretrak_controlled = is_checked( hWnd, SURETRAK2_XBOX );
                    show_servo_controls();
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case MEAS_BISCUIT_AT_DELAY_XBOX:
                if ( m )
                    {
                    if ( is_checked(hWnd, MEAS_BISCUIT_AT_DELAY_XBOX) )
                        m->monitor_flags |= MA_USE_TIME_TO_MEASURE_BISCUIT;
                    else
                        m->monitor_flags &= ~MA_USE_TIME_TO_MEASURE_BISCUIT;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case MVO_06596_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    m->monitor_flags &= ~MA_HAS_DIGITAL_SERVO;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case DIGITAL_SERVO_RADIO:
                if ( cmd == BN_CLICKED )
                    {
                    m->monitor_flags |= MA_HAS_DIGITAL_SERVO;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case CHOOSE_SERVO_PB:
                DialogBox(
                    MainInstance,
                    TEXT("CHOOSE_SERVO_DIALOG"),
                    hWnd,
                    (DLGPROC) choose_servo_dialog_proc );
                break;

            case EDIT_EQUATIONS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("EDIT_DCURVE_DIALOG"),
                    hWnd,
                    (DLGPROC) edit_dcurve_dialog_proc );
                break;

            case HEAD_PRESS_RADIO_BUTTON:
                CheckRadioButton( MachSetupWindow, ROD_PRESS_RADIO_BUTTON, DIFF_PRESS_RADIO_BUTTON, id );
                if ( m )
                    {
                    m->impact_pres_type[POST_IMPACT_INDEX] = HEAD_PRESSURE_CURVE;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case ROD_PRESS_RADIO_BUTTON:
                CheckRadioButton( MachSetupWindow, ROD_PRESS_RADIO_BUTTON, DIFF_PRESS_RADIO_BUTTON, id );
                if ( m )
                    {
                    m->impact_pres_type[POST_IMPACT_INDEX] = ROD_PRESSURE_CURVE;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case DIFF_PRESS_RADIO_BUTTON:
                CheckRadioButton( MachSetupWindow, ROD_PRESS_RADIO_BUTTON, DIFF_PRESS_RADIO_BUTTON, id );
                if ( m )
                    {
                    m->impact_pres_type[POST_IMPACT_INDEX] = DIFF_PRESSURE_CURVE;
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case POS_IMPACT_POINTS_BOX:
            case TIME_IMPACT_POINTS_BOX:
            case CYCLE_TIMEOUT_BOX:
            case AUTOSHOT_TIMEOUT_BOX:
                if ( cmd == EN_KILLFOCUS && m )
                    {
                    i = GetDlgItemInt( MachSetupWindow, id, NULL, TRUE );
                    switch ( id )
                        {
                        case POS_IMPACT_POINTS_BOX:
                            m->impact_points[PRE_IMPACT_INDEX] = i;
                            break;
                        case TIME_IMPACT_POINTS_BOX:
                            m->impact_points[POST_IMPACT_INDEX] = i;
                            break;
                        case CYCLE_TIMEOUT_BOX:
                            m->cycle_timeout_seconds = i;
                            break;
                        case AUTOSHOT_TIMEOUT_BOX:
                            m->autoshot_timeout_seconds = i;
                            break;
                        }
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case QUADRATURE_DIVISOR_EB:
            case BINARY_VALVE_DEVENT_EB:
            case BINARY_VALVE_PULSE_EB:
            case VALVE_SETTING_AFTER_JOG_SHOT_EB:
                if ( cmd == EN_KILLFOCUS && m )
                    {
                    switch ( id )
                        {
                        case QUADRATURE_DIVISOR_EB:
                            get_new_quadrature_divisor();
                            break;
                        case BINARY_VALVE_DEVENT_EB:
                            m->devent_time.get_text(MachSetupWindow, id);
                            break;
                        case BINARY_VALVE_PULSE_EB:
                            m->pulse_time.get_text(MachSetupWindow, id);
                            break;
                        case VALVE_SETTING_AFTER_JOG_SHOT_EB:
                            m->valve_setting_after_jog_shot.get_text(MachSetupWindow, id);
                            break;
                        }
                    Machines[CurrentMachine].has_changed = TRUE;
                    }
                break;

            case BINARY_VALVE_OUTPUT_PB:
                if ( !m )
                    break;
                if ( choose_wire(GetDlgItem(hWnd, id), hWnd, BINARY_VALVE_PULSE_OUTPUT_TYPE, NO_INDEX, m->computer, m->name, m->current_part ) )
                    {
                    get_new_binary_valve_pulse_wire();
                    }
                break;

            case IDOK:
                if ( save_changes() )
                    get_selected_machine();
               break;

            case CANCEL_BUTTON:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                break;
            }
        return TRUE;
    }

return FALSE;
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

/***********************************************************************
*                           MAKE_NEW_MACHINE                           *
***********************************************************************/
static void make_new_machine()
{
LISTBOX_CLASS lb;
STRING_CLASS  s;

s.get_text( NewMachineWindow, NEW_MACHINE_EBOX );

if ( s.isempty() )
    {
    ShowWindow( NewMachineWindow, SW_HIDE );
    return;
    }
/*
------------------------------------------------
Machine name must be unique across all computers
------------------------------------------------ */
lb.init( MachSetupWindow, MACHINE_LIST_BOX );

if ( lb.index(s.text()) == NO_INDEX )
    {
    add_machine( s );
    ShowWindow( NewMachineWindow, SW_HIDE );
    poke_data( DDE_CONFIG_TOPIC, ItemList[MACH_SETUP_INDEX].name, s.text() );
    }
else
    MessageBox( NewMachineWindow, TEXT("Machine Already Exists"), TEXT("Error"), MB_OK );

}

/***********************************************************************
*                            NEW_MACHINE_PROC                          *
***********************************************************************/
BOOL CALLBACK new_machine_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;
HWND  w;

id  = LOWORD( wParam );
switch (msg)
    {
    case WM_INITDIALOG:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        w = GetDlgItem( NewMachineWindow, NEW_MACHINE_EBOX );
        OldEditProc = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) sub_proc );
        SetFocus( w );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                make_new_machine();
                break;

            case CANCEL_BUTTON:
                ShowWindow( NewMachineWindow, SW_HIDE );
                break;
            }
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          CREATE_NEW_MACHINE                          *
***********************************************************************/
void create_new_machine ()
{
if ( HaveSureTrakControl && !Machines[CurrentMachine].machine.is_ftii )
    resource_message_box( MainInstance, ONLY_ONE_MACHINE_STRING, SURETRAK_CONTROLLED_STRING, MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL );

if (NewMachineWindow == NULL)
   {
   NewMachineWindow = CreateDialog(
       MainInstance,
       TEXT("NEW_MACHINE"),
       MainWindow,
       (DLGPROC) new_machine_proc );
   }

ShowWindow( NewMachineWindow, SW_SHOW );
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

    case WM_HELP:
        gethelp();
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case IDCANCEL:
            case EXIT_CHOICE:
               SendMessage( hWnd, WM_CLOSE, 0, 0L );
               return 0;

            case MINIMIZE_MENU_CHOICE:
               CloseWindow( MainWindow );
               return 0;

            case CREATE_CHOICE:
               create_new_machine();
               return 0;

            case DELETE_CHOICE:
                delete_machine();
                return 0;
             }
        break;

    case WM_DBINIT:
        fill_rod_pitches();
        fill_dcurves();
        CurrentMachine = 0;
        fill_machines( MyEmptyString );
        client_dde_startup( hWnd );
        SetFocus( GetDlgItem(MachSetupWindow, MACHINE_LIST_BOX) );
        return 0;

    case WM_SETFOCUS:
        if ( MachSetupWindow )
            SetFocus( GetDlgItem(MachSetupWindow, MACHINE_LIST_BOX) );
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

/*
-----------------------------------------------------
Read my file first so I can override the common names
----------------------------------------------------- */
s.get_stringtable_name( EmachineResourceFile );
StringTable.read( s );

s.get_stringtable_name( ChooseWireResourceFile );
StringTable.append( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( HINSTANCE this_instance, int cmd_show )
{
WNDCLASS wc;
COMPUTER_CLASS c;

MainInstance = this_instance;
MachSetupWindow = 0;

c.startup();
load_resources();

if ( *get_ini_string( VisiTrakIniFile, ConfigSection, HaveSureTrakKey ) == YChar )
    HaveSureTrakControl = TRUE;
if ( *get_ini_string( VisiTrakIniFile, ConfigSection, HavePressureControlKey ) == YChar )
    HavePressureControl = TRUE;

wc.lpszClassName = MyClassName;
wc.hInstance     = this_instance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(this_instance, MyIconName );
wc.lpszMenuName  = TEXT("MachineMenu");
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );
register_choose_wire_control();

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    0, 0,                             // X,y
    610, 325,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MachSetupWindow  = CreateDialog(
    MainInstance,
    TEXT("EDIT_MACHINES"),
    MainWindow,
    (DLGPROC) MachSetupProc );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown()
{
COMPUTER_CLASS c;

client_dde_shutdown();
c.shutdown();
cleanup_machines();
cleanup_dcurves();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
BOOL status;
MSG   msg;

MyWindowTitle = resource_string( this_instance, MAIN_WINDOW_TITLE );

if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return 0;

init( this_instance, cmd_show);

while ( GetMessage(&msg, NULL, 0, 0) )
   {
   status = FALSE;

   if (MachSetupWindow )
      status = IsDialogMessage( MachSetupWindow, &msg );

   if (!status && NewMachineWindow )
      status = IsDialogMessage( NewMachineWindow, &msg );

   if ( !status )
      {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      }
   }

shutdown();
return(msg.wParam);
}
