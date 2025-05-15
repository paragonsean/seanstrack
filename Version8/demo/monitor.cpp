#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\events.h"
#include "..\include\ftanalog.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\ftcalc.h"
#include "..\include\plookup.h"
#include "..\include\setpoint.h"
#include "..\include\shotname.h"
#include "..\include\stddown.h"
#include "..\include\structs.h"
#include "..\include\stringcl.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

#include "resource.h"
#include "extern.h"

static const DWORD MS_TO_WAIT = 1000;
static const short WRITE_PARAM_TIMEOUT = 150;   /* About 1/4 second */

MACHINE_CLASS      SorcMachine;
PART_CLASS         SorcPart;
PARAMETER_CLASS    SorcParam;
ANALOG_SENSOR      AnalogSensor;
FTANALOG_CLASS     SorcFtAnalog;
STRING_CLASS       LastSorcDir;

/*
--------------------------------
Indexes for hardware status bits
-------------------------------- */
static const int DATA_COL_MODE_STATE_INDEX = 0;
static const int AUTO_SHOT_STATE_INDEX     = 1;
static const int NEW_DATA_STATE_INDEX      = 2;

bool        HaveCycleStart = false;
bool        Started = false;

long        AutoShotDisplaySeconds = 2;
long        AutoShotDisplayTime    = 0;

SYSTEMTIME  NowSystemTime;
long        Now                    = 0;
long        LastShotTime           = 0;
STRING_CLASS ShotName;

static STDDOWN_CLASS StdDown;

static PART_NAME_ENTRY DestPart;

static const TCHAR     ProfilePattern[] = TEXT( "*.PR2" );
TCHAR MonallIniFile[]               = TEXT( "monall.ini" );
TCHAR ConfigSection[]               = TEXT( "Config" );

static TEXT_LIST_CLASS FileName;

double * calculate_ftii_parameters( FTII_PROFILE & f, MACHINE_CLASS & machine, PART_CLASS & part, PARAMETER_CLASS & param, ANALOG_SENSOR & asensor, FTANALOG_CLASS & ftanalog, FTCALC_CLASS & ftclass );

/***********************************************************************
*                           CURRENT_OPERATOR                           *
***********************************************************************/
TCHAR * current_operator()
{
static TCHAR opnumber[OPERATOR_NUMBER_LEN+1];

NAME_CLASS     s;
SETPOINT_CLASS sp;

lstrcpy( opnumber, NO_OPERATOR_NUMBER );

s.get_machset_csvname( DestPart.computer, DestPart.machine );
if ( sp.get(s) )
    {
    if ( !sp[MACHSET_OPERATOR_NUMBER_INDEX].value.isempty() )
        copystring( opnumber, sp[MACHSET_OPERATOR_NUMBER_INDEX].value.text(), OPERATOR_NUMBER_LEN );
    }

return opnumber;
}

/***********************************************************************
*                          SET_DOWNTIME_STATE                          *
***********************************************************************/
void set_downtime_state( TCHAR * newcat, TCHAR * newsubcat, SYSTEMTIME st )

{
TCHAR     * cp;
TCHAR     * worker;
NAME_CLASS  s;
DB_TABLE    t;
DSTAT_CLASS d;
int         old_state;

if ( !Started )
    return;

d.get( DestPart.computer, DestPart.machine );
old_state = d.down_state();

if ( d.same_code(newcat, newsubcat) )
    return;

d.set_cat( newcat );
d.set_subcat( newsubcat );
d.set_time( st );
d.put( DestPart.computer, DestPart.machine );

worker = current_operator();

/*
---------------------------------
Only write if downtime is enabled
--------------------------------- */
if ( DownTimeoutSeconds > 0 )
    {
    if ( ((old_state == HUMAN_DOWN_STATE) && (d.down_state() == MACH_UP_STATE)) ||
         ((old_state == MACH_UP_STATE) && (d.down_state() != MACH_UP_STATE))       )
        {
        s.get_downtime_dbname( DestPart.computer, DestPart.machine );
        if ( t.open(s, DOWNTIME_RECLEN, PWL) )
            {
            make_downtime_record( t, NowSystemTime, newcat, newsubcat, worker, DestPart.part );
            t.rec_append();
            t.close();
            }
        }
    }

cp = make_downtime_event_string( DestPart.machine, DestPart.part, st, newcat, newsubcat, worker );
if ( cp )
    MainWindow.post( WM_POKEMON, (WPARAM) DOWN_DATA_INDEX, (LPARAM) cp );

CurrentDownState = d.down_state();
}

/***********************************************************************
*                             NEXTSHOT                                 *
***********************************************************************/
bool nextshot()
{
TIME_CLASS    t;
int           n;
PLOOKUP_CLASS p;
SHOT_NAME_CLASS sn;
STRING_CLASS    s;

sn.init( DestPart.computer, DestPart.machine, DestPart.part );
sn.get();
sn.get_shot_name_template( s );

t.set( NowSystemTime );

p.init( DestPart.computer, DestPart.machine, DestPart.part );
p.get();

n = p.get_last_shot_number();
n++;

if ( shot_name(s, DestPart.computer, DestPart.machine, DestPart.part, n) )
    {
    p.set_last_shot_number( n );
    p.set_last_shot_name( s );
    p.set_time( t );
    if ( p.put() )
        {
        ShotName = s;
        return true;
        }
    }

return false;
}

/***********************************************************************
*                   SEND_MACHINE_STATE_EVENT_STRING                    *
*                                                                      *
*                         "M01\t00F1\t00FF"                            *
***********************************************************************/
static void send_machine_state_event_string( int index )
{
static const TCHAR * buffers[] = {
    { TEXT( "\t0000\tA000") },
    { TEXT( "\tA000\tA400") },
    { TEXT( "\tA400\t0000") }
    };

TCHAR * buf;
TCHAR * cp;
int DdeLineLen;

DdeLineLen = DestPart.machine.len() + 1 + BOARD_DATA_HEX_LEN + 1 + BOARD_DATA_HEX_LEN + 1;

if ( index < 0 || index > 2 )
    return;

buf = maketext( DdeLineLen );
if ( !buf )
    return;

cp = buf;
cp = copystring( cp, DestPart.machine.text() );
copystring( cp, buffers[index] );

MainWindow.post( WM_POKEMON, (WPARAM) NEW_MACH_STATE_INDEX, (LPARAM) buf );
}

/***********************************************************************
*                            CHECK_SORC_DIR                            *
* If this is a new source directory I need to extract the machine      *
* and part names from the directory name                               *
***********************************************************************/
static BOOLEAN check_sorc_dir()
{
TCHAR         * bp;
TCHAR         * cp;

COMPUTER_CLASS  c;
NAME_CLASS      s;
PART_NAME_ENTRY pn;
BOOLEAN         status;

status = FALSE;
if ( LastSorcDir != SorcDir && !SorcDir.isempty() )
    {
    LastSorcDir = SorcDir;
    s = SorcDir;
    s.remove_ending_backslash();
    cp = s.find( RESULTS_DIRECTORY_NAME );
    if ( !cp )
        return FALSE;
    cp--;
    *cp = NullChar;

    bp = s.text();
    cp = find_last_char( BackSlashChar, bp );
    if ( cp )
        {
        cp++;
        pn.part = cp;
        cp--;
        *cp = NullChar;

        cp = find_last_char( BackSlashChar, bp );
        if ( cp )
            {
            cp++;
            pn.machine = cp;
            cp--;
            *cp = NullChar;

            cp = find_last_char( BackSlashChar, bp );
            if ( cp )
                {
                *cp = NullChar;
                if ( c.find(bp) )
                    {
                    pn.computer = c.name();
                    SorcMachine.find( pn.computer, pn.machine );
                    SorcPart.find( pn.computer, pn.machine, pn.part );
                    SorcParam.find( pn.computer, pn.machine, pn.part );
                    SorcFtAnalog.load( pn.computer, pn.machine, pn.part );
                    status = TRUE;
                    }
                }
            }
        }
    }

return status;
}

/***********************************************************************
*                              MAKE_A_SHOT                             *
***********************************************************************/
static void make_a_shot()
{
const BOOL OVERWRITE_EXISTING = FALSE;

BOOL           status;
NAME_CLASS     sorc;
NAME_CLASS     dest;
int            i;
float          parms[MAX_PARMS];
double       * dp;
BITSETYPE      save_flags;
float          value;
DB_TABLE       t;
FIELDOFFSET    fo;
FTII_PROFILE   f;
BOOLEAN        is_ftii;
short          record_length;
short          dif;
short          field_length;
short          shot_name_len;
VDB_CLASS      vt;
TCHAR        * cp;
STRING_CLASS   s;
FTCALC_CLASS   ftcalc;

if ( !FileName.next () )
    {
    FileName.rewind();
    if ( !FileName.next() )
        return;
    }

sorc = SorcDir;
sorc += FileName.text();

if ( get_shotsave_semaphore(DestPart.computer, DestPart.machine) )
    {
    if ( nextshot() )
        {
        is_ftii = FALSE;
        if ( findstring(PROFILE_II_SUFFIX, FileName.text()) )
            is_ftii = TRUE;

        /*
        ----------------------------------------------------------
        Copy the file to the destination directory with a new name
        ---------------------------------------------------------- */
        dest.get_profile_ii_name( DestPart.computer, DestPart.machine, DestPart.part, ShotName );
        CopyFile( sorc.text(), dest.text(), OVERWRITE_EXISTING );
        status = TRUE;

        ftcalc.load( DestPart.computer, DestPart.machine, DestPart.part );

        check_sorc_dir();
        if ( f.get(sorc) )
            {
            dp = calculate_ftii_parameters( f, SorcMachine, SorcPart, SorcParam, AnalogSensor, SorcFtAnalog, ftcalc );
            if ( dp )
                {
                for ( i=0; i<MAX_PARMS; i++ )
                    parms[i] = dp[i];
                delete[] dp;
                dp = 0;
                }
            }

        save_flags = SAVE_PARM_DATA | SAVE_PROFILE_DATA;

        /*
        ------------------------------------
        Append a record to the graphlst file
        ------------------------------------ */
        dest.get_graphlst_dbname( DestPart.computer, DestPart.machine, DestPart.part );
        t.ensure_existance( dest );

        shot_name_len = (short) ShotName.len();
        field_length = shot_name_len;
        dif = field_length + 2 - SHOT_LEN;
        record_length = GRAPHLST_RECLEN + dif;


        t.open( dest, record_length, PWL );

        fo = (FIELDOFFSET) GRAPHLST_SHOT_NUMBER_OFFSET;
        t.put_alpha( fo, ShotName, shot_name_len );
        fo = (FIELDOFFSET) GRAPHLST_DATE_OFFSET + dif;
        t.put_date( fo, NowSystemTime );
        fo = (FIELDOFFSET) GRAPHLST_TIME_OFFSET + dif;
        t.put_time( fo, NowSystemTime );
        fo = (FIELDOFFSET) GRAPHLST_SAVE_FLAGS_OFFSET + dif;
        t.put_short( fo, (short) save_flags, GRAPHLST_SAVE_FLAGS_LEN );
        t.rec_append();
        t.close();

        /*
        ------------------------------------
        Append a record to the shotparm file
        ------------------------------------ */
        if ( status )
            {
            dest.get_shotparm_csvname( DestPart.computer, DestPart.machine, DestPart.part );
            if ( !dest.file_exists() )
                vt.create( dest );

            vt.set_nof_cols( SHOTPARM_NOF_COLS );
            if ( vt.open_for_rw(dest) )
                {
                vt.ra.rewind();
                vt.ra.next();
                vt.ra = ShotName;
                vt.put_date( SHOTPARM_DATE_INDEX, NowSystemTime );
                vt.put_time( SHOTPARM_TIME_INDEX, NowSystemTime );
                vt.put_bitsetype( SHOTPARM_SAVE_FLAGS_INDEX, save_flags );
                for ( i=0; i<MAX_PARMS; i++ )
                    {
                    vt.ra.next();
                    value = parms[i];
                    vt.ra = value;
                    }
                vt.rec_append();
                vt.close();
                }
            }
        }
    free_shotsave_semaphore();
    }

LastShotTime = Now;
s = DestPart.machine;
s += CommaChar;
s += DestPart.part;

cp = maketext( ShotName.text() );
if ( cp )
    MainWindow.post( WM_NEWSHOT, (WPARAM) cp, (LPARAM) s.release() );
send_machine_state_event_string( DATA_COL_MODE_STATE_INDEX );
set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT, NowSystemTime );
}

/***********************************************************************
*                             START_A_SHOT                             *
***********************************************************************/
static void start_a_shot()
{
if ( !Started )
    return;

send_machine_state_event_string( AUTO_SHOT_STATE_INDEX );

AutoShotDisplayWindow.show();
AutoShotDisplayTime = Now;
HaveCycleStart      = true;
}

/***********************************************************************
*                           START_MONITORING                           *
***********************************************************************/
static void start_monitoring()
{
hourglass_cursor();

DestPart.machine.set_text( AutoShotDisplayWindow.control(AUTO_SHOT_MACH_NAME_TBOX) );

FileName.empty();
FileName.get_file_list( SorcDir.text(), ProfilePattern );
if ( FileName.count() < 1 )
    {
    Started = false;
    BrouseWindow.post( WM_NO_DATA );
    return;
    }

FileName.sort();
FileName.rewind();

restore_cursor();

HaveNewSorc = FALSE;        // Tell the brouse dialog I am using the current sorc dir
if ( SorcDirEbox )
    InvalidateRect( SorcDirEbox, 0, TRUE );

Started = true;
if ( !ManualMode )
    start_a_shot();
}

/***********************************************************************
*                       GET_DOWN_TIMEOUT_SECONDS                       *
* This is called just before copying the CurrentPart to the DestPart   *
* so I want to use the CurrentPart values.                             *
***********************************************************************/
void get_down_timeout_seconds()
{
NAME_CLASS     s;
SETPOINT_CLASS sp;

DownTimeoutSeconds = 0;

s.get_machset_csvname( CurrentPart.computer, CurrentPart.machine );
if ( sp.get(s) )
    {
    if ( !sp[MACHSET_CYCLE_TIMEOUT_INDEX].value.isempty() )
        DownTimeoutSeconds = (long) sp[MACHSET_CYCLE_TIMEOUT_INDEX].int_value();
    }
}

/***********************************************************************
*                         BOARD_MONITOR_THREAD                         *
***********************************************************************/
DWORD board_monitor_thread( int * notused )
{
DWORD       status;
SYSTEMTIME  st;
TIME_CLASS  tc;
DSTAT_CLASS d;

BoardMonitorIsRunning = TRUE;

StdDown.startup();

while ( TRUE )
    {
    GetLocalTime(&NowSystemTime);
    Now = sec_since1990( NowSystemTime );

    if ( ShuttingDown )
        {
        BoardMonitorIsRunning = FALSE;
        if ( CurrentDownState == PROG_UP_STATE || CurrentDownState == MACH_UP_STATE || CurrentDownState == NO_DOWN_STATE )
            set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT, NowSystemTime );

        MainWindow.close();
        break;
        }

    if ( HaveNewPart )
        {
        /*
        -----------------------------
        Down this machine if it is up
        ----------------------------- */
        if ( CurrentDownState == PROG_UP_STATE || CurrentDownState == MACH_UP_STATE || CurrentDownState == NO_DOWN_STATE )
            set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT, NowSystemTime );

        /*
        ----------------------------------------
        Get the timeout seconds for the new part
        ---------------------------------------- */
        get_down_timeout_seconds();

        DestPart = CurrentPart;

        d.get( DestPart.computer, DestPart.machine );
        CurrentDownState = d.down_state();

        if ( CurrentDownState == PROG_DOWN_STATE )
            set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_START_SUBCAT, NowSystemTime );

        HaveNewPart = FALSE;
        LastShotTime = Now;
        }

    if ( BoardMonitorCommand )
        {
        switch ( BoardMonitorCommand )
            {
            case START_MONITORING:
                start_monitoring();
                break;

            case STOP_MONITORING:
                break;

            case MANUAL_SHOT_REQ:
                start_a_shot();
                break;
            }
        BoardMonitorCommand = 0;
        }

   if ( HaveCycleStart )
        {
        if ( (Now - AutoShotDisplayTime) > AutoShotDisplaySeconds )
            {
            send_machine_state_event_string( NEW_DATA_STATE_INDEX );
            HaveCycleStart = false;
            AutoShotDisplayWindow.hide();
            make_a_shot();
            }
        }
    else if ( Started && !ManualMode )
        {
        if ( (Now - LastShotTime) > SecondsBetweenShots )
            start_a_shot();
        }

    else if ( Started && DownTimeoutSeconds > 0 )
        {
        /*
        ------------------------------------------------------------
        If there was no status change for this board, check for down
        ------------------------------------------------------------ */
        if ( CurrentDownState == PROG_UP_STATE || CurrentDownState == MACH_UP_STATE || CurrentDownState == NO_DOWN_STATE )
            {
            if ( (Now - LastShotTime) > DownTimeoutSeconds )
                {
                if ( SubtractTimeoutFromAutoDown )
                    {
                    tc.set( (unsigned long) Now );
                    tc -= (unsigned long) DownTimeoutSeconds;
                    st = tc.system_time();
                    }
                else
                    {
                    st = NowSystemTime;
                    }

                if ( StdDown.find( Now ) )
                    set_downtime_state( StdDown.cat(), StdDown.subcat(), st );
                else
                    set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_UNSP_DOWN_SUBCAT, st );
                }
            }
        }
    status = WaitForSingleObject( BoardMonitorEvent, MS_TO_WAIT );
    if ( status == WAIT_FAILED )
        resource_message_box( MainInstance,  UNABLE_TO_CONTINUE_STRING, THREAD_WAIT_FAIL_STRING,  MB_OK | MB_SYSTEMMODAL );
    }

return 0;
}

/***********************************************************************
*                      START_AQUIRING_DATA_THREAD                      *
***********************************************************************/
void start_aquiring_data_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) board_monitor_thread, /* thread function       */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, MONITOR_THREAD_STRING, MB_OK | MB_SYSTEMMODAL );

}
