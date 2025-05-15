#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\bitclass.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\computer.h"
#include "..\include\machine.h"
#include "..\include\multistring.h"
#include "..\include\param.h"
#include "..\include\rectclas.h"
#include "..\include\shotname.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"
#include "..\include\vdbclass.h"
#include "..\include\wclass.h"

#include "extern.h"

/*
-----------
My Controls
----------- */
#define MIN_SHOT_TEXTBOX  201
#define MAX_SHOT_TEXTBOX  202

/*
------------------------
Active Region Definition
------------------------ */
#define LEFT_EDGE  5
int     Top_Edge = 10;

static long  DX       = 6;
static long  DY       = 13;

#define ALARM_COLOR_INDEX    0
#define NO_ALARM_COLOR_INDEX 1
#define WARNING_COLOR_INDEX  2
#define NO_SHOT_ROW         -1

int CurrentHistorySelect = NO_INDEX;
int CurrentShotRow       = NO_SHOT_ROW;
int LowHistoryShotRow    = 0;
int HighHistoryShotRow   = 0;

SHOT_NAME_PARTS_CLASS LowHistoryShotName;
SHOT_NAME_PARTS_CLASS HighHistoryShotName;

static MULTISTRING_CLASS * ParamArray     = 0;
static STRING_CLASS      * ShotNameArray  = 0;
static BITSETYPE         * SaveFlagsArray = 0;

extern int  LineHeight;

BOOLEAN is_alarm( short parameter_index, float value );
BOOLEAN is_warning( short parameter_index, float value );
RECT & my_desktop_client_rect();

/***********************************************************************
*                        CLEANUP_DYNAMIC_ARRAYS                        *
***********************************************************************/
static void cleanup_dynamic_arrays()
{
if ( ParamArray )
    {
    delete[] ParamArray;
    ParamArray = 0;
    }
if ( ShotNameArray )
    {
    delete[] ShotNameArray;
    ShotNameArray = 0;
    }
if ( SaveFlagsArray )
    {
    delete[] SaveFlagsArray;
    SaveFlagsArray = 0;
    }
}

/***********************************************************************
*                        ALLOCATE_DYNAMIC_ARRAYS                       *
***********************************************************************/
static void allocate_dynamic_arrays()
{
int i;

if ( HistoryShotCount )
    {
    ParamArray     = new MULTISTRING_CLASS[HistoryShotCount];
    for ( i=0; i<HistoryShotCount; i++ )
        ParamArray[i].set_array_size( MAX_PARMS );
    ShotNameArray  = new STRING_CLASS[HistoryShotCount];
    SaveFlagsArray = new BITSETYPE[HistoryShotCount];
    }
}

/***********************************************************************
*                          NEW_HISTORY_CHOICE                          *
***********************************************************************/
void new_history_choice( void )
{
BOOLEAN  have_new_shot;
NAME_CLASS s;
DB_TABLE   t;
int32      n;
int32      i;
STRING_CLASS shot_name;

have_new_shot = FALSE;

if ( ComputerName.isempty() )
    return;

if ( MachineName.isempty() )
    return;

if ( PartName.isempty() )
    return;

s.get_graphlst_dbname( ComputerName, MachineName, PartName );
if ( !t.open(s, AUTO_RECLEN, PFL ) )
    return;

n = (int32) t.nof_recs();
if ( n > 0 )
    {
    i = n - HistoryShotCount;
    if ( i < 0 )
        i = 0;
    i += CurrentHistorySelect;

    if ( i >= n )
        i = n - 1;

    t.goto_record_number(i);
    t.get_rec();

    have_new_shot = t.get_alpha( s, GRAPHLST_SHOT_NAME_INDEX );
    }
t.close();

if ( have_new_shot )
    {
    CurrentShotRow = i;
    ShotName         = s;
    SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
    }
}

/***********************************************************************
*                         PARAMETER_COLOR                              *
*                                                                      *
*         Return an index [0, 1, 2] = [Red, Green, Yellow]             *
***********************************************************************/
static int parameter_color( short parameter_index, float value )
{
if ( is_alarm( parameter_index, value) )
    return ALARM_COLOR_INDEX;

if ( is_warning( parameter_index, value) )
    return WARNING_COLOR_INDEX;

return NO_ALARM_COLOR_INDEX;
}

/***********************************************************************
*                    POSITION_PARAM_HISTORY_WINDOW                     *
***********************************************************************/
void position_param_history_window( void )
{
RECTANGLE_CLASS r;
RECTANGLE_CLASS rd;
WINDOW_CLASS    w;
int             x;

if ( !ParamHistoryWindow )
    return;

if ( !ParameterWindow )
    return;

GetWindowRect( ParameterWindow, &r );

r.left = r.right;

if ( IsFullScreen )
    rd = my_desktop_client_rect();
else
    rd = desktop_client_rect();

r.right = rd.right;

DY       = LineHeight; /* This is calculated in paramwin */
Top_Edge = DY;

if ( HistoryShotCount > 0 )
    {
    DX = r.right - r.left;
    DX -= 2*LEFT_EDGE;
    DX /= HistoryShotCount;
    }

w = GetDlgItem( ParamHistoryWindow, MAX_SHOT_TEXTBOX );
w.get_move_rect( rd );

x = LEFT_EDGE + HistoryShotCount*DX;
x -= rd.width();
w.move( x, 0 );

MoveWindow( ParamHistoryWindow, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                         CHECK_CURRENT_SELECT                         *
***********************************************************************/
static BOOLEAN check_current_select()
{
if ( CurrentShotRow == NO_SHOT_ROW )
    return FALSE;

CurrentHistorySelect = CurrentShotRow - LowHistoryShotRow;
return TRUE;
}

/***********************************************************************
*                 HIGHLIGHT_CURRENT_HISTORY_SELECTION                  *
***********************************************************************/
static void highlight_current_history_selection( HDC dc )
{
RECT r;
HPEN     oldpen;
HPEN     mypen;

r.top    = Top_Edge;
r.bottom = (NofCornerParameters + 1)*DY - 1;

if ( !check_current_select() )
    return;

r.left = LEFT_EDGE + CurrentHistorySelect*DX;
r.right = r.left + DX;
mypen = CreatePen( PS_SOLID, 2, RGB(255,255,255) );
if ( mypen )
    {
    oldpen = (HPEN) SelectObject( dc, mypen );

    MoveToEx( dc, r.left,  r.top, 0 );
    LineTo(   dc, r.left,  r.bottom );
    LineTo(   dc, r.right, r.bottom );
    LineTo(   dc, r.right, r.top );
    LineTo(   dc, r.left, r.top );
    SelectObject( dc, oldpen );
    DeleteObject( mypen );
    }
}

/***********************************************************************
*                    READ_PARAMETERS_FROM_PARMLIST                     *
* nofshots is the number of shots to be read. The names of the shots   *
* are in the ShotNameArray beginning at index 0. These are the last    *
* 50, or fewer, shots from the graphlst file.                          *
***********************************************************************/
int32 read_parameters_from_parmlist( int nof_shots )
{
const int ONE_KEY_FIELD = 1;

TIME_CLASS closetime;
BIT_CLASS  have_parms;
int        i;
int        n;
bool       openok;
TIME_CLASS opentime;
int        rec_index;
NAME_CLASS s;
int        shot_index;
VDB_CLASS  v;

if ( !ParamArray )
    return 0;

have_parms.init( nof_shots );

if ( ParamArray )
    {
    s.get_shotparm_csvname( ComputerName, MachineName, PartName );

    openok = false;
    opentime.get_local_time();
    closetime = opentime;

    if ( v.open_for_read(s) )
        {
        openok = true;
        opentime.get_local_time();
        v.rewind();
        n = v.nof_recs();
        rec_index = n - nof_shots;
        maxint( rec_index, 0 );
        v.get_rec( rec_index );

        n = 0;
        while ( true )
            {
            /*
            ---------------------------------------------------------------------
            Compare the shot name of the record to each name in the ShotNameArray
            If I find a match then I copy the parameter values from the record
            to the ParamArray multistring for this shot.
            --------------------------------------------------------------------- */
            for ( shot_index=0; shot_index<nof_shots; shot_index++ )
                {
                if ( !have_parms[shot_index] )
                    {
                    if ( ShotNameArray[shot_index] == v.ra[SHOTPARM_SHOT_NAME_INDEX] )
                        {
                        have_parms.set( shot_index );
                        ParamArray[shot_index].rewind();
                        for ( i=0; i<MAX_PARMS; i++ )
                            {
                            ParamArray[shot_index].next();
                            ParamArray[shot_index] = v.ra[SHOTPARM_PARAM_1_INDEX + i];
                            }
                        break;
                        }
                    }
                }
            n++;
            if ( n == nof_shots )
                break;

            if ( !v.get_next_record() )
                break;
            }
        v.close();
        closetime.get_local_time();
        }
    write_shotparm_log( MachineName, PartName, TEXT("read_parameters_from_parmlist"), openok, opentime, closetime, ShotparmLogFile );

    /*
    ------------------------------------------------
    Empty the parameters of any shot I have not read
    ------------------------------------------------ */
    for ( shot_index=0; shot_index<nof_shots; shot_index++ )
        {
        if ( !have_parms[shot_index] )
            {
            ParamArray[shot_index].rewind();
            for ( i=0; i<MAX_PARMS; i++ )
                {
                ParamArray[shot_index].next();
                ParamArray[shot_index].null();
                }
            }
        }
    }

return MAX_PARMS;
}

/***********************************************************************
*                        DRAW_PARAMETER_HISTORY                        *
***********************************************************************/
static void draw_parameter_history( HDC dc )
{
int             b;
HBRUSH          brush[3];
FIELDOFFSET     fo;
int32           i;
int32           ip;
HPEN            mypen;
STRING_CLASS    myshotname;
int32           n;
HBRUSH          oldbrush;
HPEN            oldpen;
RECT            r;
int             row;
NAME_CLASS      s;
int             shot_number_length;
SHOT_NAME_CLASS snc;
DB_TABLE        t;
INT             x;
INT             y;

if ( ComputerName.isempty() )
    return;

if ( MachineName.isempty() )
    return;

if ( PartName.isempty() )
    return;

mypen = CreatePen( PS_SOLID, 2, RGB(0,0,0) );
if ( mypen )
    {
    oldpen = (HPEN) SelectObject( dc, mypen );
    }

brush[ALARM_COLOR_INDEX]    = CreateSolidBrush( AlarmColor   );
brush[NO_ALARM_COLOR_INDEX] = CreateSolidBrush( NoAlarmColor );
brush[WARNING_COLOR_INDEX]  = CreateSolidBrush( WarningColor );

oldbrush = (HBRUSH) SelectObject( dc, brush[0] );

for ( i=0; i<HistoryShotCount; i++ )
    ShotNameArray[i] = NO_SHOT_NUMBER;

snc.init( ComputerName, MachineName, PartName );
snc.get();
shot_number_length = snc.shot_number_length();

myshotname = ShotName;
fix_shotname( myshotname, shot_number_length );

s.get_graphlst_dbname( ComputerName, MachineName, PartName );
if ( !t.open(s, 0, PFL ) )
    return;

CurrentShotRow = NO_SHOT_ROW;

n = (int32) t.nof_recs();
i = n - HistoryShotCount;
if ( i < 0 )
    i = 0;
t.goto_record_number(i);
t.get_rec();
row = 0;
while ( i < n )
    {
    t.get_alpha( ShotNameArray[row], GRAPHLST_SHOT_NAME_INDEX );
    fix_shotname( ShotNameArray[row], shot_number_length );
    fo = t.field_offset( GRAPHLST_SAVE_FLAGS_INDEX );
    SaveFlagsArray[row]  = (BITSETYPE) t.get_short( fo );
    if ( ShotNameArray[row] == myshotname )
        CurrentShotRow = row;
    i++;
    row++;
    if ( !t.get_next_record(NO_LOCK) )
        break;
    }
t.close();

/*
------------------------
Save the number of shots
------------------------ */
n = row;

read_parameters_from_parmlist( n );

r.left = LEFT_EDGE;
for ( i=0; i<n; i++ )
    {
    if ( r.left == LEFT_EDGE )
        {
        LowHistoryShotRow = i;
        LowHistoryShotName.set( ShotNameArray[i], shot_number_length );
        LowHistoryShotName.name.set_text( ParamHistoryWindow, MIN_SHOT_TEXTBOX );
        }
    r.top    = Top_Edge;
    r.right  = r.left + DX;

    for ( row=0; row<NofCornerParameters; row++ )
        {
        r.bottom = r.top + DY;

        b  = NO_ALARM_COLOR_INDEX;
        ip = CornerParameters[row] - 1;
        if ( ip >= 0 && ip < MAX_PARMS )
            {
            if ( !ParamArray[i][ip].isempty() )
                {
                b = parameter_color( (short)ip, ParamArray[i][ip].float_value() );
                FillRect( dc, &r, brush[b] );
                }
            }
        if ( mypen && row == 0 && (SaveFlagsArray[i] & SAVE_PROFILE_DATA) )
            {
            x = (r.left + r.right )/2 - 1;
            y = (r.top + r.bottom )/2 - 1;
            MoveToEx( dc, x, y,  0 );
            LineTo(   dc, x+1, y+1 );
            }

        r.top = r.bottom;
        }

    r.left = r.right;
    }

i = n-1;
if ( !ShotNameArray[i].isempty() )
    {
    HighHistoryShotRow = i;
    HighHistoryShotName.set( ShotNameArray[i], shot_number_length );
    HighHistoryShotName.name.set_text( ParamHistoryWindow, MAX_SHOT_TEXTBOX );
    }

SelectObject( dc, oldbrush );

for ( b=0; b<3; b++ )
    DeleteObject( brush[b] );

if ( mypen )
    {
    SelectObject( dc, oldpen );
    DeleteObject( mypen );
    }

highlight_current_history_selection( dc );
}

/***********************************************************************
*                           PAINT_MY_HISTORY                           *
***********************************************************************/
static void paint_my_history( void )
{

PAINTSTRUCT ps;

BeginPaint( ParamHistoryWindow, &ps );

draw_parameter_history( ps.hdc );

EndPaint( ParamHistoryWindow, &ps );
}

/***********************************************************************
*                         PARAMHISTORYPROC                             *
***********************************************************************/
int CALLBACK ParamHistoryProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int x;
HWND w;

switch (msg)
    {
    case WM_INITDIALOG:
        allocate_dynamic_arrays();
        return TRUE;

    case WM_LBUTTONDBLCLK:
        return TRUE;

    case WM_LBUTTONDOWN:
        x = LOWORD( lParam );
        x -= LEFT_EDGE;
        if ( x >= 0 )
            {
            x /= DX;
            if ( x < HistoryShotCount )
                {
                CurrentHistorySelect = x;
                new_history_choice();
                InvalidateRect( ParamHistoryWindow, 0, TRUE );
                }
            }
        if ( IsWindowVisible(MainWindow) )
            SetFocus( MainWindow );
        return TRUE;

    case WM_PAINT:
        paint_my_history();
        w = MainWindow;
        if ( ChosshotWindow )
            if ( IsWindowVisible(ChosshotWindow) )
                w = ChosshotWindow;
        if ( GetFocus() == hWnd )
            SetFocus( w );
        return TRUE;

    case WM_DESTROY:
        cleanup_dynamic_arrays();
        break;

    }

return FALSE;
}
