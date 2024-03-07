#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#define _MAIN_
#include "..\include\chars.h"
#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS ConvertDialog;
WINDOW_CLASS MainWindow;

static TCHAR MyClassName[]         = TEXT( "VtwConvert" );
static TCHAR MyIniFileName[]       = TEXT( "convert.ini" );
static TCHAR ConfigSection[]       = TEXT( "Config" );
static TCHAR ConvertResourceFile[] = TEXT( "convert" );
static TCHAR CommonResourceFile[]  = TEXT( "common" );
static TCHAR CurrentLanguageKey[]  = TEXT( "CurrentLanguage" );
static TCHAR DisplayIniFile[]      = TEXT( "display.ini" );
static TCHAR DistBetweenMarksKey[] = TEXT( "DistBetweenMarks" );
static TCHAR UnitsKey[]            = TEXT( "Units" );
static TCHAR InDistKey[]           = TEXT( "InDist" );
static TCHAR MmDistKey[]           = TEXT( "MmDist" );
static TCHAR X4DistKey[]           = TEXT( "X4Dist" );
static TCHAR IpsVelKey[]           = TEXT( "IpsVel" );
static TCHAR MpsVelKey[]           = TEXT( "MpsVel" );
static TCHAR X1VelKey[]            = TEXT( "X1Vel" );
static TCHAR XPosKey[]             = TEXT( "XPos" );
static TCHAR YPosKey[]             = TEXT( "YPos" );

static bool SettingIN  = false;
static bool SettingMM  = false;
static bool SettingX4  = false;
static bool SettingIPS = false;
static bool SettingMPS = false;
static bool SettingX1  = false;
static const double MM_PER_INCH = 25.4;

FONT_LIST_CLASS   FontList;
STRING_CLASS      MyTitle;
STRINGTABLE_CLASS StringTable;

/***********************************************************************
*                        DISTANCE_BETWEEN_MARKS                        *
***********************************************************************/
static double distance_between_marks( short & units )
{
STRING_CLASS s;

s.get_text( ConvertDialog.control(UC_DIST_BETWEEN_MARKS_EB) );
if ( is_checked(ConvertDialog.control(UC_DIST_IN_RB)) )
    units = INCH_UNITS;
else
    units = MM_UNITS;

return s.real_value();
}

/***********************************************************************
*                             DO_MM_CHANGE                             *
***********************************************************************/
static void do_mm_change()
{
STRING_CLASS s;
double x4;
double in;
double mm;
double dist_between_marks;
short  units_id;

if ( SettingMM )
    {
    SettingMM = false;
    return;
    }

s.get_text( ConvertDialog.control(UC_MM_EB) );
mm = s.real_value();
in = mm / MM_PER_INCH;
s = in;

SettingIN = true;
s.set_text( ConvertDialog.control(UC_IN_EB) );

s = ZeroChar;
dist_between_marks = distance_between_marks( units_id );
if ( not_zero(dist_between_marks) )
    {
    if ( units_id == INCH_UNITS )
        x4 = in;
    else
        x4 = mm;

    x4 *= 4;
    x4 /= dist_between_marks;
    s = x4;
    }

SettingX4 = true;
s.set_text( ConvertDialog.control(UC_X4_EB) );
}

/***********************************************************************
*                             DO_IN_CHANGE                             *
***********************************************************************/
static void do_in_change()
{
STRING_CLASS s;
double x4;
double mm;
double in;
double dist_between_marks;
short  units_id;

if ( SettingIN )
    {
    SettingIN = false;
    return;
    }

s.get_text( ConvertDialog.control(UC_IN_EB) );
in = s.real_value();
mm = in * 25.4;
s = mm;

SettingMM = true;
s.set_text( ConvertDialog.control(UC_MM_EB) );

dist_between_marks = distance_between_marks( units_id );
s = ZeroChar;
if ( not_zero(dist_between_marks) )
    {
    if ( units_id == INCH_UNITS )
        x4 = in;
    else
        x4 = mm;

    x4 *= 4.0;
    x4/=dist_between_marks;
    s = x4;
    }

SettingX4 = true;
s.set_text( ConvertDialog.control(UC_X4_EB) );
}

/***********************************************************************
*                             DO_X4_CHANGE                             *
***********************************************************************/
static void do_x4_change()
{
STRING_CLASS s;
double x4;
double mm;
double in;
double dist_between_marks;
short  units_id;

if ( SettingX4 )
    {
    SettingX4 = false;
    return;
    }

s.get_text( ConvertDialog.control(UC_X4_EB) );
x4 = s.real_value();

dist_between_marks = distance_between_marks( units_id );
mm = 0.0;
in = 0.0;
if ( not_zero(dist_between_marks) )
    {
    if ( units_id == INCH_UNITS )
        {
        in = x4 * dist_between_marks / 4.0;
        mm = in * MM_PER_INCH;
        }
    else
        {
        mm = x4 * dist_between_marks / 4.0;
        in = mm / MM_PER_INCH;
        }
    }

SettingMM = true;
s = mm;
s.set_text( ConvertDialog.control(UC_MM_EB) );

SettingIN = true;
s = in;
s.set_text( ConvertDialog.control(UC_IN_EB) );
}

/***********************************************************************
*                             DO_X1_CHANGE                             *
***********************************************************************/
static void do_x1_change()
{
STRING_CLASS s;
double x1;
double mps;
double ips;
double dist_between_marks;
short  units_id;

if ( SettingX1 )
    {
    SettingX1 = false;
    return;
    }

s.get_text( ConvertDialog.control(UC_X1_EB) );
x1 = s.real_value();

dist_between_marks = distance_between_marks( units_id );
mps = 0.0;
ips = 0.0;
if ( not_zero(dist_between_marks) )
    {
    if ( units_id == INCH_UNITS )
        {
        ips = x1 * dist_between_marks;
        mps = ips * MM_PER_INCH / 1000.0;
        }
    else
        {
        mps = x1 * dist_between_marks;
        ips = mps / MM_PER_INCH;
        mps /= 1000.0;
        }
    }

SettingMPS = true;
s = mps;
s.set_text( ConvertDialog.control(UC_MPS_EB) );

SettingIPS = true;
s = ips;
s.set_text( ConvertDialog.control(UC_IPS_EB) );
}

/***********************************************************************
*                             DO_IPS_CHANGE                             *
***********************************************************************/
static void do_ips_change()
{
STRING_CLASS s;
double x1;
double mps;
double ips;
double dist_between_marks;
short  units_id;

if ( SettingIPS )
    {
    SettingIPS = false;
    return;
    }

s.get_text( ConvertDialog.control(UC_IPS_EB) );
ips = s.real_value();

mps = ips * MM_PER_INCH / 1000.0;
s = mps;
SettingMPS = true;
s.set_text( ConvertDialog.control(UC_MPS_EB) );

s = ZeroChar;
dist_between_marks = distance_between_marks( units_id );
if ( not_zero(dist_between_marks) )
    {
    if ( units_id == INCH_UNITS )
        x1 = ips;
    else
        x1 = mps;

    x1/=dist_between_marks;
    s = x1;
    }

SettingX1 = true;
s.set_text( ConvertDialog.control(UC_X1_EB) );
}

/***********************************************************************
*                             DO_MPS_CHANGE                            *
***********************************************************************/
static void do_mps_change()
{
STRING_CLASS s;
double x1;
double mps;
double ips;
double dist_between_marks;
short  units_id;

if ( SettingMPS )
    {
    SettingMPS = false;
    return;
    }

s.get_text( ConvertDialog.control(UC_MPS_EB) );
mps = s.real_value();

ips = mps * 1000.0 / MM_PER_INCH;
s = ips;
SettingIPS = true;
s.set_text( ConvertDialog.control(UC_IPS_EB) );

s = ZeroChar;
dist_between_marks = distance_between_marks( units_id );
if ( not_zero(dist_between_marks) )
    {
    if ( units_id == INCH_UNITS )
        x1 = ips;
    else
        x1 = mps * 1000.0;

    x1/=dist_between_marks;
    s = x1;
    }

SettingX1 = true;
s.set_text( ConvertDialog.control(UC_X1_EB) );
}

/***********************************************************************
*                              INITIALIZE                              *
***********************************************************************/
static void initialize()
{
UINT       id;
INI_CLASS  ini;
NAME_CLASS s;
int x;
int y;
int units_id;

s = exe_directory();
s.cat_path( MyIniFileName );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(XPosKey) )
    {
    x = ini.get_int();
    if ( ini.find(YPosKey) )
        {
        y = ini.get_int();
        MainWindow.move( x, y );
        }
    }

units_id = INCH_UNITS;
id = UC_DIST_IN_RB;
if ( ini.find(UnitsKey) )
    {
    units_id = ini.get_int();
    if ( units_id == (int)MM_UNITS )
        id = UC_DIST_MM_RB;
    set_checkbox( ConvertDialog.control(id), TRUE );
    }

if ( ini.find(DistBetweenMarksKey) )
    {
    s = ini.get_string();
    s.set_text( ConvertDialog.control(UC_DIST_BETWEEN_MARKS_EB) );
    }

if ( units_id == INCH_UNITS )
    {
    if ( ini.find(InDistKey) )
        {
        s = ini.get_string();
        s.set_text( ConvertDialog.control(UC_IN_EB) );
        }
    if ( ini.find(IpsVelKey) )
        {
        s = ini.get_string();
        s.set_text( ConvertDialog.control(UC_IPS_EB) );
        }
    }
else
    {
    if ( ini.find(MmDistKey) )
        {
        s = ini.get_string();
        s.set_text( ConvertDialog.control(UC_MM_EB) );
        }
    if ( ini.find(MpsVelKey) )
        {
        s = ini.get_string();
        s.set_text( ConvertDialog.control(UC_MPS_EB) );
        }
    }
}

/***********************************************************************
*                                FINALIZE                              *
***********************************************************************/
static void finalize()
{
INI_CLASS  ini;
NAME_CLASS s;
int x;
int y;
int units_id;
RECT r;

s = exe_directory();
s.cat_path( MyIniFileName );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( MainWindow.getrect(r) )
    {
    x = r.left;
    maxint( x, 0 );
    ini.put_int( XPosKey, x );
    y = r.top;
    maxint( y, 0 );
    ini.put_int( YPosKey, y );
    }

units_id = INCH_UNITS;
if ( is_checked( ConvertDialog.control(UC_DIST_MM_RB) ) )
    units_id = MM_UNITS;
ini.put_int( UnitsKey, units_id );

s.get_text( ConvertDialog.control(UC_DIST_BETWEEN_MARKS_EB) );
ini.put_string( DistBetweenMarksKey, s.text() );

s.get_text( ConvertDialog.control(UC_IN_EB) );
ini.put_string( InDistKey, s.text() );

s.get_text( ConvertDialog.control(UC_IPS_EB) );
ini.put_string(IpsVelKey, s.text() );

s.get_text( ConvertDialog.control(UC_MM_EB) );
ini.put_string(MmDistKey, s.text() );

s.get_text( ConvertDialog.control(UC_MPS_EB) );
ini.put_string(MpsVelKey, s.text() );
}

/***********************************************************************
*                  SET_CONVERT_SETUP_STRINGS                    *
***********************************************************************/
static void set_convert_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { DIST_STATIC,       TEXT("DISTANCE_STRING") },
    { MM_STATIC,         TEXT("MM_UNITS") },
    { IN_STATIC,         TEXT("IN_UNITS") },
    { X4_STATIC,         TEXT("X4_COUNTS") },
    { VEL_STATIC,        TEXT("VELOCITY_STRING") },
    { MPS_STATIC,        TEXT("MPS") },
    { IPS_STATIC,        TEXT("IPS") },
    { X1_PER_SEC_STATIC, TEXT("X1_COUNTS") },
    { DIST_BET_STATIC,   TEXT("DIST_BETWEEN") },
    { UC_DIST_IN_RB,     TEXT("IN_UNITS") },
    { UC_DIST_MM_RB,     TEXT("MM_UNITS") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

set_setup_strings( hWnd, rsn, nof_rsn, NULL, 0, NULL );
}

/***********************************************************************
*                         CONVERT_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK convert_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        ConvertDialog = hWnd;
        MainWindow.shrinkwrap( hWnd );
        set_convert_setup_strings( hWnd );
        ConvertDialog.post( WM_DBINIT );
        return TRUE;

    case WM_DBINIT:
        initialize();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case UC_MM_EB:
                if ( cmd == EN_CHANGE )
                    do_mm_change();
                return TRUE;
            case UC_IN_EB:
                if ( cmd == EN_CHANGE )
                    do_in_change();
                return TRUE;
            case UC_X4_EB:
                if ( cmd == EN_CHANGE )
                    do_x4_change();
                return TRUE;
            case UC_IPS_EB:
                if ( cmd == EN_CHANGE )
                    do_ips_change();
                return TRUE;
            case UC_MPS_EB:
                if ( cmd == EN_CHANGE )
                    do_mps_change();
                return TRUE;
            case UC_X1_EB:
                if ( cmd == EN_CHANGE )
                    do_x1_change();
                return TRUE;
            case UC_DIST_BETWEEN_MARKS_EB:
                if ( cmd == EN_CHANGE )
                    {
                    do_x4_change();
                    do_x1_change();
                    }
                return TRUE;
            case UC_DIST_IN_RB:
            case UC_DIST_MM_RB:
                if ( cmd == BN_CLICKED )
                    {
                    do_x4_change();
                    do_x1_change();
                    }
                return TRUE;

            case IDCANCEL:
                finalize();
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
//static RECTANGLE_CLASS r;

switch (msg)
    {
    case WM_CREATE:
        MainWindow = hWnd;
        break;

    case WM_CLOSE:
        finalize();
        break;

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
s.get_stringtable_name( ConvertResourceFile );
StringTable.read( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );

MyTitle = StringTable.find( TEXT("MY_TITLE") );
statics_startup();
}

/***********************************************************************
*                         MY_PREVIOUS_INSTANCE                         *
***********************************************************************/
BOOLEAN my_previous_instance()
{
if ( is_previous_instance(MyClassName, MyTitle.text()) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static bool startup( int cmd_show )
{
COMPUTER_CLASS c;
WNDCLASS wc;

c.startup();
load_resources();
if ( my_previous_instance() )
    return false;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("CONVERT_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyTitle.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT("CONVERT"),
    MainWindow.handle(),
    (DLGPROC) convert_dialog_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );

return true;
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown()
{
statics_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

if ( !startup(cmd_show) )
    return 0;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( ConvertDialog.handle() )
        status = IsDialogMessage( ConvertDialog.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
