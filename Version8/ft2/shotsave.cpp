#include <windows.h>
#include <process.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\array.h"
#include "..\include\asensor.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\events.h"
#include "..\include\external_parameter.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\plookup.h"
#include "..\include\setpoint.h"
#include "..\include\shotname.h"
#include "..\include\shot_name_reset.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\vdbclass.h"
#include "..\include\wclass.h"
#include "..\include\workclas.h"

#include "resource.h"
#include "extern.h"
#include "shotsave.h"

static TCHAR * SerialFile    = 0;
static TCHAR DelimitChar   = TEXT( '\t' );

static BOOLEAN SaveAllAlarmShots   = FALSE;
static BOOLEAN ShowMachine         = TRUE;
static BOOLEAN ShowPart            = TRUE;
static BOOLEAN ShowDate            = TRUE;
static BOOLEAN ShowTime            = TRUE;
static BOOLEAN ShowShot            = TRUE;
static BOOLEAN ShowParam[MAX_PARMS];

static BOOLEAN EosAsciiToSerial        = FALSE;
static TCHAR * EosComPort              = 0;
static TCHAR * EosComSetup             = 0;
static TCHAR * EosHaveAlarmShot        = 0;
static TCHAR * EosHaveWarningShot      = 0;
static TCHAR * EosHaveGoodShot         = 0;
static TCHAR * EosHaveAlarmParameter   = 0;
static TCHAR * EosHaveWarningParameter = 0;
static TCHAR * EosHaveGoodParameter    = 0;
static TCHAR * EosMachineName          = 0;
static TCHAR * EosOutputFile           = 0;
static TCHAR * EosOutputFileSetup      = 0;
static TCHAR   EosComDelimitChar       = TEXT( ',' );
static TCHAR   EosComEol[3]            = TEXT( "\r\n" );
static int     EosFixedWidth           = 0;
static BOOLEAN EosNeedsStxEtx          = FALSE;
static BOOLEAN EosKillOutputFile       = TRUE;
static BOOLEAN EosOnlyIfNoAlarms       = FALSE;
static TCHAR   EosPyramidLines         = ZeroChar;
static BOOLEAN HaveDelimitChar         = TRUE;

static char EtxBuf[]   = "\003";
static char StxBuf[]   = "\002";

static NAME_CLASS EosEndFile;
static NAME_CLASS EosStartFile;

static TCHAR EosAsciiToSerialKey[]       = TEXT( "EosAsciiToSerial" );
static TCHAR EosComSetupKey[]            = TEXT( "EosComSetup" );
static TCHAR EosEndFileKey[]             = TEXT( "EosEndFile" );
static TCHAR EosHaveAlarmParameterKey[]  = TEXT( "EosHaveAlarmParameter" );
static TCHAR EosHaveGoodParameterKey[]   = TEXT( "EosHaveGoodParameter" );
static TCHAR EosHaveWarningParameterKey[]= TEXT( "EosHaveWarningParameter" );
static TCHAR EosHaveAlarmShotKey[]       = TEXT( "EosHaveAlarmShot" );
static TCHAR EosHaveGoodShotKey[]        = TEXT( "EosHaveGoodShot" );
static TCHAR EosHaveWarningShotKey[]     = TEXT( "EosHaveWarningShot" );
static TCHAR EosKillOutputFileKey[]      = TEXT( "EosKillOutputFile" );
static TCHAR EosMachineNameKey[]         = TEXT( "EosMachineName" );
static TCHAR EosOutputFileKey[]          = TEXT( "EosOutputFile" );
static TCHAR EosOutputFileSetupKey[]     = TEXT( "EosOutputFileSetup" );
static TCHAR EosStartFileKey[]           = TEXT( "EosStartFile" );
static TCHAR EosParamFileDirKey[]        = TEXT( "EosParamFileDir" );
static TCHAR EosParamFileNameKey[]       = TEXT( "EosParamFileName" );
static TCHAR EosParamFileSetupKey[]      = TEXT( "EosParamFileSetup" );
static TCHAR EosParamFileInfoSetupKey[]  = TEXT( "EosParamFileInfoSetup" );

static short  WRITE_PARAM_TIMEOUT = 150;   /* About 1/4 second */

static const float MaxFloatShort = float( 0xFFFF );

TCHAR * resource_string( UINT resource_id );
void    post_new_data_message( STRING_CLASS & sorc );

WORKER_LIST_CLASS Worker;

/***********************************************************************
*                          CLASS DEFINITION                            *
*                         WRITE_CSV_SHOT_FILE                          *
***********************************************************************/
class WRITE_CSV_SHOT_FILE
{
public:
BOOLEAN      is_enabled;
STRING_CLASS dest_file_dir;

WRITE_CSV_SHOT_FILE();
~WRITE_CSV_SHOT_FILE();
BOOLEAN get( PROFILE_NAME_ENTRY & pn );
};

/***********************************************************************
*                          WRITE_CSV_SHOT_FILE                         *
***********************************************************************/
WRITE_CSV_SHOT_FILE::WRITE_CSV_SHOT_FILE()
{
is_enabled = FALSE;
}

/***********************************************************************
*                         ~WRITE_CSV_SHOT_FILE                         *
***********************************************************************/
WRITE_CSV_SHOT_FILE::~WRITE_CSV_SHOT_FILE()
{
}

/***********************************************************************
*                                  GET                                 *
***********************************************************************/
BOOLEAN WRITE_CSV_SHOT_FILE::get( PROFILE_NAME_ENTRY & pn )
{
FILE_CLASS f;
NAME_CLASS s;

if ( s.get_part_dir_file_name(pn.computer, pn.machine, pn.part, TEXT("save_csv_with_shot.csv")) )
    {
    f.open_for_read( s );
    s = f.readline();
    s.next_field();
    s.next_field();
    is_enabled = s.boolean_value();

    s = f.readline();
    s.next_field();
    s.next_field();
    dest_file_dir = s;

    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             FREEBUF                                  *
***********************************************************************/
static void freebuf( int i )
{
SHOT_BUFFER_ENTRY * b;

b = BufPtr[i];
if ( b )
    {
    BufPtr[i] = 0;
    delete b;
    }
}

/***********************************************************************
*                        GET_DDE_PARAMETER_LIST                        *
***********************************************************************/
static void get_dde_parameter_list()
{
int32   i;
BOOLEAN have_comma;
TCHAR * cp;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowMachine") );
if ( *cp == NChar )
    ShowMachine = FALSE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowPart") );
if ( *cp == NChar )
    ShowPart = FALSE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowDate") );
if ( *cp == NChar )
    ShowDate = FALSE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowTime") );
if ( *cp == NChar )
    ShowTime = FALSE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamShowShot") );
if ( *cp == NChar )
    ShowShot = FALSE;

/*
---------------------------------
Default to showing all parameters
--------------------------------- */
for( i=0; i<MAX_PARMS; i++ )
    ShowParam[i] = TRUE;

cp = get_long_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamParmList") );
if ( !unknown(cp) )
    {
    for( i=0; i<MAX_PARMS; i++ )
        ShowParam[i] = FALSE;

    while ( TRUE )
        {
        have_comma = replace_char_with_null( cp, CommaChar );
        i = asctoint32( cp ) - 1;
        if ( i >= 0 && i < MAX_PARMS )
            ShowParam[i] = TRUE;
        if ( !have_comma )
            break;
        cp = nextstring( cp );
        }
    }
}

/***********************************************************************
*                             GET_INI_CHAR                             *
***********************************************************************/
TCHAR * get_ini_char( TCHAR * dest, TCHAR * sorc )
{
if ( *sorc == BackSlashChar )
    {
    sorc++;
    if ( *sorc == RCharLow )
        *dest = CrChar;
    else if ( *sorc == NCharLow )
        *dest = LfChar;
    else
        *dest = TabChar;
    }
else
    {
    *dest = *sorc;
    }

sorc++;
return sorc;
}

/***********************************************************************
*                           INIT_SERIAL_PORT                           *
*   DWORD DCBlength            // sizeof( DCB )                        *
*   DWORD BaudRate;            // current baud rate                    *
*   DWORD fBinary: 1;          // binary mode, no EOF check            *
*   DWORD fParity: 1;          // enable parity checking               *
*   DWORD fOutxCtsFlow:1;      // CTS output flow control              *
*   DWORD fOutxDsrFlow:1;      // DSR output flow control              *
*   DWORD fDtrControl:2;       // DTR flow control type                *
*   DWORD fDsrSensitivity:1;   // DSR sensitivity                      *
*   DWORD fTXContinueOnXoff:1; // XOFF continues Tx                    *
*   DWORD fOutX: 1;            // XON/XOFF out flow control            *
*   DWORD fInX: 1;             // XON/XOFF in flow control             *
*   DWORD fErrorChar: 1;       // enable error replacement             *
*   DWORD fNull: 1;            // enable null stripping                *
*   DWORD fRtsControl:2;       // RTS flow control                     *
*   DWORD fAbortOnError:1;     // abort reads/writes on error          *
*   DWORD fDummy2:17;          // reserved                             *
*   WORD wReserved;            // not currently used                   *
*   WORD XonLim;               // transmit XON threshold               *
*   WORD XoffLim;              // transmit XOFF threshold              *
*   BYTE ByteSize;             // number of bits/byte, 4-8             *
*   BYTE Parity;               // 0-4=no,odd,even,mark,space           *
*   BYTE StopBits;             // 0,1,2 = 1, 1.5, 2                    *
*   char XonChar;              // Tx and Rx XON character              *
*   char XoffChar;             // Tx and Rx XOFF character             *
*   char ErrorChar;            // error replacement character          *
*   char EofChar;              // end of input character               *
*   char EvtChar;              // received event character             *
*   WORD wReserved1;           // reserved; do not use                 *
***********************************************************************/
void init_serial_port()
{
HANDLE  fh;
TCHAR * cp;
DCB     dcb;
int     i;
BOOL    status;

struct STOP_BIT_ENTRY {
    TCHAR s[4];
    unsigned int value;
    };
static STOP_BIT_ENTRY sbe[] = {
    { TEXT("1"),   ONESTOPBIT },
    { TEXT("1.5"), ONE5STOPBITS },
    { TEXT("2"),   TWOSTOPBITS  }
    };

const int nof_stops = sizeof(sbe)/sizeof(STOP_BIT_ENTRY);

struct PARITY_ENTRY {
    TCHAR character;
    unsigned int value;
    };

static PARITY_ENTRY  pe[] = {
    { TEXT('N'), 0 },
    { TEXT('n'), 0 },
    { TEXT('O'), 1 },
    { TEXT('o'), 1 },
    { TEXT('E'), 2 },
    { TEXT('e'), 2 },
    { TEXT('M'), 3 },
    { TEXT('m'), 3 },
    { TEXT('S'), 4 },
    { TEXT('s'), 4 }
    };

const int nof_parity_chars = sizeof(pe) / sizeof(PARITY_ENTRY);

if ( !EosComPort )
    return;

dcb.DCBlength = sizeof( DCB );
status = FALSE;

fh = CreateFile( EosComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh != INVALID_HANDLE_VALUE )
    {
    status = GetCommState( fh, &dcb );
    CloseHandle( fh );
    }

if ( status )
    {
    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComSpeed") );
    if ( !unknown(cp) )
        dcb.BaudRate = (DWORD) asctoul( cp );

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComParity") );
    if ( !unknown(cp) )
        {
        for ( i=0; i<nof_parity_chars; i++ )
            {
            dcb.Parity  = 0;          /* No parity */
            if ( *cp == pe[i].character )
                {
                dcb.Parity  = pe[i].value;
                break;
                }
            }

        if ( dcb.Parity == 0 )
            dcb.fParity = 0;
        else
            dcb.fParity = 1;
        }

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComBits") );
    if ( !unknown(cp) )
        dcb.ByteSize = asctoul( cp );

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComStop") );
    if ( !unknown(cp) )
        {
        for ( i=0; i<nof_stops; i++ )
            {
            if ( strings_are_equal( sbe[i].s, cp) )
                {
                dcb.StopBits = sbe[i].value;
                break;
                }
            }
        }


    /*
    ---------------------------------------------------
    Tell the other computer I am always ready for chars
    --------------------------------------------------- */
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComCts") );
    if ( !unknown(cp) )
        {
        if ( *cp == YChar )
            dcb.fOutxCtsFlow = 1;
        else
            dcb.fOutxCtsFlow = 0;
        }

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComDsr") );
    if ( !unknown(cp) )
        {
        if ( *cp == YChar )
            dcb.fOutxDsrFlow = 1;
        else
            dcb.fOutxDsrFlow = 0;
        }

    dcb.fDsrSensitivity   = 0;
    dcb.fTXContinueOnXoff = 1;
    dcb.fInX              = 0;

    cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComXonXoff") );
    if ( !unknown(cp) )
        {
        if ( *cp == YChar )
            dcb.fOutX = 1;
        else
            dcb.fOutX = 0;
        }

    fh = CreateFile( EosComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if ( fh != INVALID_HANDLE_VALUE )
        {
        SetCommState( fh, &dcb );
        CloseHandle( fh );
        }
    else
        {
        status = FALSE;
        }
    }

if ( !status )
    {
    delete[] EosComPort;
    EosComPort = 0;
    }
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup()
{
TCHAR  * dp;
TCHAR  * cp;
int i;

for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
    BufPtr[i] = 0;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("SaveAllAlarmShots") );
if ( *cp == YChar )
    SaveAllAlarmShots = TRUE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("DdeParamDelimiter") );
if ( !unknown(cp) && *cp != TabChar )
    DelimitChar = *cp;

cp = get_ini_string( MonallIniFile, ConfigSection, EosStartFileKey );
if ( !unknown(cp) )
    EosStartFile.get_local_ini_file_name( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, EosEndFileKey );
if ( !unknown(cp) )
    EosEndFile.get_local_ini_file_name( cp );

/*
----------------------------------------------
Get the name of the comm port "COM1" or "COM2"
---------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComPort") );
if ( lstrcmp(cp, UNKNOWN) != 0 && (*cp != NChar) && (*cp != NCharLow) )
    EosComPort = maketext( cp );

cp = get_long_ini_string( MonallIniFile, ConfigSection, EosComSetupKey );
if ( !unknown(cp) )
    EosComSetup = maketext( cp );

/*
----------------------------------
See if I will be writing to a file
---------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, EosOutputFileKey );
if ( !unknown(cp) )
    EosOutputFile = maketext( cp );

if ( EosOutputFile )
    {
    cp = get_long_ini_string( MonallIniFile, ConfigSection, EosOutputFileSetupKey );
    if ( unknown(cp) )
        {
        if ( EosComSetup )
            EosOutputFileSetup = maketext( EosComSetup );
        }
    else
        {
        EosOutputFileSetup = maketext( cp );
        }
    }

/*
----------------------------------------------------
See if I want to kill the file before writing to it.
Default to true = the way I did it before.
---------------------------------------------------- */
EosKillOutputFile = TRUE;
cp = get_ini_string( MonallIniFile, ConfigSection, EosKillOutputFileKey );
if ( !unknown(cp) )
    {
    if ( *cp == NChar || *cp == NCharLow )
        EosKillOutputFile = FALSE;
    }

/*
-----------------------------------------------------------------------------------
Get the name of the machine. This is only needed if I monitor more than one machine
one of which is connected to a stamp machine.
----------------------------------------------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosMachineName") );
if ( !unknown(cp) && (*cp != NullChar) )
    EosMachineName = maketext( cp );

/*
-----------------------------------------------------------
Get the names of the alarm, warning, and good shot strings.
Default to A,W,G.
----------------------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, EosHaveAlarmShotKey );
if ( unknown(cp) )
    EosHaveAlarmShot = maketext( TEXT("A") );
else
    EosHaveAlarmShot = maketext( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, EosHaveWarningShotKey );
if ( unknown(cp) )
    EosHaveWarningShot = maketext( TEXT("W") );
else
    EosHaveWarningShot = maketext( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, EosHaveGoodShotKey );
if ( unknown(cp) )
    EosHaveGoodShot = maketext( TEXT("G") );
else
    EosHaveGoodShot = maketext( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, EosHaveAlarmParameterKey );
if ( unknown(cp) )
    EosHaveAlarmParameter = maketext( TEXT("A") );
else
    EosHaveAlarmParameter = maketext( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, EosHaveWarningParameterKey );
if ( unknown(cp) )
    EosHaveWarningParameter = maketext( TEXT("W") );
else
    EosHaveWarningParameter = maketext( cp );

cp = get_ini_string( MonallIniFile, ConfigSection, EosHaveGoodParameterKey );
if ( unknown(cp) )
    EosHaveGoodParameter = maketext( TEXT("G") );
else
    EosHaveGoodParameter = maketext( cp );

/*
--------------------
See if I need an STX
-------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosNeedsStxEtx") );
if ( *cp == YChar )
    EosNeedsStxEtx = TRUE;

cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosOnlyIfNoAlarms") );
if ( *cp == YChar )
    EosOnlyIfNoAlarms = TRUE;

/*
--------------------------------------------------
See if the parameters should all be the same width
-------------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosFixedWidth") );
if ( !unknown(cp) )
    {
    i = asctoint32( cp );
    if ( i > 0 && i < 50 )
        EosFixedWidth = i;
    }
/*
----------------
Look for Pyramid
---------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosPyramidLines") );
if ( *cp == OneChar || *cp == TwoChar )
    {
    EosPyramidLines = *cp;
    }

/*
---------------------------------------------------------
Get the delimiter to put after each field except the last
--------------------------------------------------------- */
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComDelimitChar") );
if ( *cp == NullChar || unknown(cp) )
    HaveDelimitChar = FALSE;
else if ( *cp == BackSlashChar && *(cp+1) == ZeroChar )
    HaveDelimitChar = FALSE;
else
    get_ini_char( &EosComDelimitChar, cp );

/*
----------------------------------------------------------
Get the end of line string (normally \r\n but may be none)
---------------------------------------------------------- */
dp = EosComEol;
cp = get_ini_string( MonallIniFile, ConfigSection, TEXT("EosComEol") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    {
    /*
    ------------------------------
    Allow up to two eol characters
    ------------------------------ */
    if ( *cp != NullChar )
        {
        cp = get_ini_char( dp, cp );
        dp++;
        }

    if ( *cp != NullChar )
        {
        get_ini_char( dp, cp );
        dp++;
        }
    }

*dp = NullChar;

if ( EosComPort && !EosComSetup )
        {
        delete[] EosComPort;
        EosComPort = 0;
        }

if ( EosComPort )
    init_serial_port();

if ( EosOutputFile && !EosOutputFileSetup )
    {
    delete[] EosOutputFile;
    EosOutputFile = 0;
    }

/*
----------------------
Load the Operator list
---------------------- */
Worker.load();

get_dde_parameter_list();
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
int i;

for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
    freebuf( i );

killtext( EosComPort );
killtext( EosComSetup );
killtext( EosHaveAlarmShot );
killtext( EosHaveWarningShot );
killtext( EosHaveGoodShot );
killtext( EosHaveAlarmParameter );
killtext( EosHaveWarningParameter );
killtext( EosHaveGoodParameter );
}

/***********************************************************************
*                        DELETE_SHOT_NAME_FILE                         *
***********************************************************************/
static void delete_shot_name_file( NAME_CLASS & sorc, PROFILE_NAME_ENTRY & pn )
{
NAME_CLASS   dest;
NAME_CLASS   cf;
STRING_CLASS s;
FILE_CLASS   f;
int          n;

if ( SaveShotNameFiles )
    {
    dest.get_part_dir_file_name( pn.computer, pn.machine, pn.part, TEXT("shot_name") );
    if ( !dest.directory_exists() )
        dest.create_directory();
    cf = dest;
    cf.cat_path( TEXT("count.txt") );
    n = 0;

    if ( f.open_for_read(cf) )
        {
        s = f.readline();
        f.close();
        n = s.int_value();
        if ( n < 0 )
            n = 0;
        }

    n++;
    s = n;
    f.open_for_write( cf );
    f.writeline( s );
    f.close();
    dest.cat_path( TEXT("sn") );
    dest += s;
    dest += TEXT(".txt" );
    sorc.moveto( dest );
    }
else
    {
    sorc.delete_file();
    }
}

/***********************************************************************
*                     SHOT_NAME_FROM_FILE_IS_GOOD                      *
***********************************************************************/
static bool shot_name_from_file_is_good( STRING_CLASS & s, SHOT_NAME_CLASS & sn )
{
static TCHAR check_digit_tag[] = TEXT( "{check digit}" );

STRING_CLASS t;
TCHAR * cp;
int     number_length;

/*
----------------------------------------------------------------------------
Nissan is the only one. The file they give me is nothing but shot_name+crlf.
---------------------------------------------------------------------------- */
if ( s.len() != sn.shot_name_length() )
    return false;

/*
-------------------------------------------------
If there is no t I might as well not worry
------------------------------------------------- */
if ( !sn.get_shot_name_template(t) )
    return true;

/*
--------------------------------------------------
Check to see that the shot number field is numeric
-------------------------------------------------- */
number_length = sn.shot_number_length();
cp = s.last_char();
if ( t.contains(check_digit_tag) )
    cp--;
while ( true )
    {
    if ( !is_numeric(*cp) )
        return false;
    number_length--;
    if ( number_length < 1 )
        break;
    cp--;
    }

return true;
}

/***********************************************************************
*                             NEXTSHOT                                 *
***********************************************************************/
BOOLEAN nextshot( SHOT_BUFFER_ENTRY * b )
{
const int     MAX_RESET_TIMES = 10;
TCHAR       * cp;
FILE_CLASS    f;
TIME_CLASS    last_time;
TIME_CLASS    new_time;
TIME_CLASS    reset_time[MAX_RESET_TIMES];
int           nof_reset_times;
int           i;
int           n;
int           shot_number_length;
PLOOKUP_CLASS p;
NAME_CLASS    fn;
NAME_CLASS    s;
SHOT_NAME_RESET_CLASS sr;
SHOT_NAME_CLASS       sn;
EXTERNAL_SHOT_NAME_CLASS esn;

n = 0;
new_time.set( b->f.shot_time );

p.init( b->pn.computer, b->pn.machine, b->pn.part );
p.get();

sr.init( b->pn.computer, b->pn.machine, b->pn.part );
sr.get();
if ( sr.is_enabled() )
    {
    p.get_time( last_time );
    i = 0;
    s = sr.reset_time().text();
    while ( s.next_field() )
        {
        reset_time[i].get_local_time();
        reset_time[i].set_time( s.text() );
        i++;
        if ( i >= MAX_RESET_TIMES )
            break;
        }
    nof_reset_times = i;

    /*
    ------------------------------------------------
    Check to see if I have just crossed a reset time
    ------------------------------------------------ */
    for ( i=0; i<nof_reset_times; i++ )
        {
        if ( last_time < reset_time[i] && reset_time[i] <= new_time )
            {
            if( sr.purge_on_reset() )
                purge_shot_data( b->pn.computer, b->pn.machine, b->pn.part );
            n = 1;
            break;
            }
        }
    }

esn.get( b->pn.computer, b->pn.machine, b->pn.part );
if ( esn.is_enabled() )
    {
    sn.init( b->pn.computer, b->pn.machine, b->pn.part );
    sn.get();
    shot_number_length = sn.shot_number_length();

    cp = esn.file().text();
    if ( *cp == BackSlashChar && *(cp+1) == BackSlashChar || *(cp+1) == ColonChar )
        fn = esn.file().text();
    else
        fn.get_exe_dir_file_name( esn.file().text() );

    if ( fn.file_exists() )
        {
        f.open_for_read( fn );
        s = f.readline();
        f.close();
        delete_shot_name_file( fn, b->pn );
        if ( shot_name_from_file_is_good(s, sn) )
            {
            n = shot_number_from_shot_name( s, shot_number_length );
            p.set_last_shot_number( n );
            p.set_last_shot_name( s );
            p.set_time( new_time );
            if ( p.put() )
                {
                b->pn.shot = s;
                return TRUE;
                }
            }
        else
            {
            /*
            -------------------------------------------------
            The shot name is not valid. Do not save the shot.
            Use WM_NO_DATA to tell the main thread to display
            the error message. I'm using temp_message and it
            won't work from this thread.
            ------------------------------------------------- */
            MainWindow.post( WM_NO_DATA, 0, 0 );
            return FALSE;
            }
        }
    /*
    ------------------------------------------------
    That didn't work. Just make it from the template
    ------------------------------------------------ */
    n = 0;
    }

/*
----------------------------------------------------------------------------------------------------
If I haven't reset the shot data n will be zero and I need to get and increment the last shot number
---------------------------------------------------------------------------------------------------- */
if ( n == 0 )
    {
    n = p.get_last_shot_number();
    n++;
    }
if ( shot_name(s, b->pn.computer, b->pn.machine, b->pn.part, n) )
    {
    p.set_last_shot_number( n );
    p.set_last_shot_name( s );
    p.set_time( new_time );
    if ( p.put() )
        {
        b->pn.shot = s;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                        WRITE_FTII_PARAMETERS                         *
***********************************************************************/
BOOLEAN write_ftii_parameters( SHOT_BUFFER_ENTRY * b, SYSTEMTIME & st, BITSETYPE save_flags )
{
int         i;
DWORD       n;
NAME_CLASS  s;
VDB_CLASS   t;
union { unsigned u; BITSETYPE bits; };

s.get_shotparm_csvname( b->pn.computer, b->pn.machine, b->pn.part );
if ( !s.file_exists() )
    t.create( s );

t.set_nof_cols( SHOTPARM_NOF_COLS );

if ( t.open_for_shared_rw(s) )
    {
    t.ra.rewind();
    t.ra.next();
    t.ra = b->pn.shot;
    t.put_date( SHOTPARM_DATE_INDEX, st );
    t.put_time( SHOTPARM_TIME_INDEX, st );
    t.put_bitsetype( SHOTPARM_SAVE_FLAGS_INDEX, save_flags );
    for ( i=0; i<MAX_PARMS; i++ )
        {
        t.ra.next();
        t.ra = b->param[i];
        }
    t.rec_append();

    /*
    ----------------------------------------------------------------------
    Check back 51 shots and see if I should delete the record.
    Record the flags for write_profile so he doesn't have to look this up.
    ---------------------------------------------------------------------- */
    n = t.nof_recs();
    if ( n > (DWORD) HistoryShotCount )
        {
        n -= HistoryShotCount;
        /*
        --------------------------------------------------
        Subtract one more because the last record is n - 1
        -------------------------------------------------- */
        n--;
        t.goto_record_number(n);
        t.get_rec();

        u =  t.ra[SHOTPARM_SAVE_FLAGS_INDEX].uint_value();
        save_flags = bits;
        if ( !(save_flags & SAVE_PARM_DATA) )
            t.rec_delete();
        }

    n = max_parameters_saved( b->pn.machine );
    if ( n > 0 && n < (unsigned) t.nof_recs() )
        {
        t.goto_record_number( 0 );
        t.rec_delete();
        }

    t.close();

    if ( t.get_last_error() == VS_SUCCESS )
        return TRUE;
    }
else
    {
    MessageBox( 0, TEXT("write_ftii_parameters"), TEXT("Could not open shotparm"), MB_OK | MB_SYSTEMMODAL );
    }
return FALSE;
}

/***********************************************************************
*                         ADD_TO_SHOTSAVE                              *
*                                                                      *
* This is called by getdata to add the shot to the buffer. Once I      *
* set the BufPtr, I should no longer make any changes to it.           *
* This is the only routine that is known globally.                     *
***********************************************************************/
bool add_to_shotsave( bool new_has_alarm, bool new_has_warning, PART_NAME_ENTRY & partname, FTII_FILE_DATA & fsorc, DOUBLE_ARRAY_CLASS & psorc )
{
int i;

SHOT_BUFFER_ENTRY * b;

for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
    {
    if ( BufPtr[i] == 0 )
        {
        b = new SHOT_BUFFER_ENTRY;
        if ( b )
            {
            b->save_failed = false;
            b->has_alarm   = new_has_alarm;
            b->has_warning = new_has_warning;
            b->pn          = partname;
            b->f.move( fsorc );
            b->param.move( psorc );
            BufPtr[i] = b;
            return true;
            }
        }
    }

return false;
}

/***********************************************************************
*                      MAKE_PARAMETER_EVENT_STRING                     *
***********************************************************************/
static TCHAR * make_parameter_event_string( SHOT_BUFFER_ENTRY * b )
{
TCHAR      * cp;
int          i;
int          n;
STRING_CLASS s;
SYSTEMTIME   st;

FileTimeToSystemTime( &b->f.shot_time, &st );

n = 0;
if ( ShowMachine )
    n += b->pn.machine.len() + 1;
if ( ShowPart )
    n += b->pn.part.len() + 1;

if ( ShowDate )
    n += ALPHADATE_LEN + 1;

if ( ShowTime )
    n += ALPHATIME_LEN + 1;

if ( ShowShot )
    n += b->pn.shot.len() + 1;

for ( i=0; i<b->param.count(); i++ )
    {
    if ( ShowParam[i] )
        n += SHOTPARM_FLOAT_LEN + 1;
    }

if ( n < 1 )
    return 0;

if ( !s.init(n) )
    return 0;

if ( ShowMachine )
    s.cat_w_char( b->pn.machine, DelimitChar );

if ( ShowPart )
    s.cat_w_char( b->pn.part,    DelimitChar );

if ( ShowDate )
    s.cat_w_char( alphadate(st), DelimitChar );

if ( ShowTime )
    s.cat_w_char( alphatime(st), DelimitChar );

/*
-----------
Shot Number
----------- */
if ( ShowShot )
    s.cat_w_char( b->pn.shot, DelimitChar );

/*
----------
Parameters
---------- */
for ( i=0; i<b->param.count(); i++ )
    {
    if ( ShowParam[i] )
        {
        s.cat_w_char( ascii_double(b->param[i]), DelimitChar );
        }
    }

cp = s.text() + s.len();
cp--;
if ( *cp == DelimitChar )
    *cp = NullChar;

return s.release();
}

/***********************************************************************
*                      WRITE_BYTES_TO_SERIAL_PORT                      *
***********************************************************************/
static BOOLEAN write_bytes_to_serial_port( void * sorc, DWORD bytes_to_write )
{
HANDLE  fh;
DWORD   bytes_written;

bytes_written  = 0;

fh = CreateFile( EosComPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh != INVALID_HANDLE_VALUE )
    {
    WriteFile( fh, sorc, bytes_to_write, &bytes_written, 0 );
    CloseHandle( fh );
    }

return TRUE;
}

/***********************************************************************
*                    WRITE_BINARY_FILE_TO_SERIAL_PORT                  *
***********************************************************************/
BOOLEAN write_binary_file_to_serial_port( NAME_CLASS & fn )
{
FILE_CLASS   f;
char       * buf;
int          n;
DWORD        bytes_to_write;

if ( fn.isempty() )
    return FALSE;

if ( !file_exists(fn.text()) )
    return FALSE;

n = file_size( fn.text() );
if ( n < 3 )
    return FALSE;

buf = new char[n];
if ( !buf )
    return FALSE;

bytes_to_write = 0;

if ( !f.open_for_read(fn.text()) )
    {
    delete[] buf;
    return FALSE;
    }

bytes_to_write = f.readbinary( buf, (DWORD) n );
f.close();

if ( bytes_to_write )
    write_bytes_to_serial_port( buf, bytes_to_write );

delete[] buf;
return TRUE;
}

/***********************************************************************
*                    WRITE_HEX_FILE_TO_SERIAL_PORT                     *
* This is just the test program. The file should be in the same dir    *
* as I am.                                                             *
***********************************************************************/
static BOOLEAN write_hex_file_to_serial_port( NAME_CLASS & fn )
{
FILE_CLASS f;
STRING_CLASS s;
TCHAR        c1;
TCHAR        c2;
TCHAR      * cp;
TCHAR        hex[3];
char       * buf;
int          n;
DWORD        bytes_to_write;

if ( fn.isempty() )
    return FALSE;

if ( !file_exists(fn.text()) )
    return FALSE;

n = file_size( fn.text() );
if ( n < 3 )
    return FALSE;

buf = new char[n];
if ( !buf )
    return FALSE;

bytes_to_write = 0;

if ( f.open_for_read(fn) )
    {
    while ( true )
        {
        s = f.readline();
        if ( s.isempty() )
            break;
        cp = s.text();
        while ( true )
            {
            c1 = *cp;
            if ( !is_hex_char(c1) )
                break;
            c2 = *(cp+1);
            if ( !is_hex_char(c2) )
                break;
            hex[0] = c1;
            hex[1] = c2;
            hex[2] = NullChar;
            buf[bytes_to_write] = ul_to_char( hextoul(hex) );
            bytes_to_write++;
            cp += 2;
            if ( *cp != SpaceChar )
                break;
            cp++;
            }
        }

    f.close();
    }

if ( bytes_to_write )
        write_bytes_to_serial_port( buf, bytes_to_write );

delete[] buf;
return TRUE;
}

/***********************************************************************
*                          CONVERT_CONTROL_CHARS                       *
*                This is safe to use on STRING_CLASS vars.             *
***********************************************************************/
static void convert_control_chars( TCHAR * sorc )
{
TCHAR * cp;

if ( !sorc )
    return;

while ( *sorc != NullChar )
    {
    if ( *sorc == BackSlashChar )
        {
        cp = sorc;
        cp++;
        if ( *cp == RCharLow )
            {
            *sorc = CrChar;
            remove_char( cp );
            }
        else if ( *cp == NCharLow )
            {
            *sorc = LfChar;
            remove_char( cp );
            }
        else if ( *cp == TCharLow )
            {
            *sorc = TabChar;
            remove_char( cp );
            }
        else if ( *cp == ZeroChar )
            {
            *sorc = NullChar;
            remove_char( cp );
            }
        else if ( ZeroChar < *cp && *cp <= NineChar )
            {
            *sorc = ( *cp - ZeroChar);
            remove_char( cp );
            }
        }
    sorc++;
    }
}

/***********************************************************************
*                         WRITE_EOS_PARAM_FILE                         *
*      {param number},{param name},{param value},{param units}         *
***********************************************************************/
static void write_eos_param_file( SHOT_BUFFER_ENTRY * b, PARAMETER_CLASS & pc )
{
static TCHAR param_number_tag[]       = TEXT( "{param number}" );
static TCHAR param_name_tag[]         = TEXT( "{param name}" );
static TCHAR param_value_tag[]        = TEXT( "{param value}" );
static TCHAR param_units_tag[]        = TEXT( "{param units}" );
static TCHAR param_high_alarm_tag[]   = TEXT( "{param high alarm}" );
static TCHAR param_low_alarm_tag[]    = TEXT( "{param low alarm}" );
static TCHAR param_high_warning_tag[] = TEXT( "{param high warning}" );
static TCHAR param_low_warning_tag[]  = TEXT( "{param low warning}" );
static TCHAR param_goodness_tag[]     = TEXT( "{param goodness}" );
static TCHAR part_name_tag[]          = TEXT( "{part name}" );
static TCHAR shot_name_tag[]          = TEXT( "{shot name}" );
static TCHAR shot_date_tag[]          = TEXT( "{shot date}" );
static TCHAR shot_time_tag[]          = TEXT( "{shot time}" );

TCHAR         * cp;
bool            have_time;
NAME_CLASS      fn;
FILE_CLASS      f;
int             i;
int             j;
INI_CLASS       ini;
STRING_CLASS    infosetup;
STRING_CLASS    limit;
int             n;
PART_NAME_ENTRY partname;
NAME_CLASS      s;
NAME_CLASS      setup;
TIME_CLASS      t;
STRING_CLASS    tag;
float           value;

s.get_local_ini_file_name( MonallIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( !ini.find(EosParamFileDirKey) )
    return;

fn = ini.get_string();
if ( fn.isempty() )
    return;
t.set( b->f.shot_time );
partname.set( b->pn.computer, b->pn.machine, b->pn.part );
if ( fn.contains(LeftCurlyBracket) )
    fn = dynamic_file_name( t, partname, fn.text() );

create_directory( fn.text() );

/*
------------------------------------------------------------------
You can add dynamic content to the file before I add the shot name
------------------------------------------------------------------ */
s.empty();
if ( ini.find(EosParamFileNameKey) )
    {
    s = ini.get_string();
    if ( s.contains(LeftCurlyBracket) )
        s = dynamic_file_name( t, partname, s.text() );
    }
if ( !s.isempty() )
    fn.cat_path( s.text() );

/*
-------------------------
The shot name.csv is last
------------------------- */
fn.cat_path( b->pn.shot );
fn += CSV_SUFFIX;

if ( ini.find(EosParamFileInfoSetupKey) )
    infosetup = ini.get_string();

if ( !ini.find(EosParamFileSetupKey) )
    return;

setup = ini.get_string();
if ( setup.isempty() )
    return;

if ( pc.count() )
    {
    if ( f.open_for_write(fn) )
        {
        /*
        ---------------------------------------------------------------------
        See if they want to write an optional first line with part name, etc.
        --------------------------------------------------------------------- */
        if ( !infosetup.isempty() )
            {
            have_time = false;
            s = infosetup;
            while ( tag.get_substring( s, LeftCurlyBracket, RightCurlyBracket ) )
                {
                if ( tag == part_name_tag )
                    {
                    s.replace( tag.text(), b->pn.part.text() );
                    }
                else if ( tag == shot_name_tag )
                    {
                    s.replace( tag.text(), b->pn.shot.text() );
                    }
                else if ( tag == shot_date_tag )
                    {
                    t.set( b->f.shot_time );
                    have_time = true;
                    s.replace( tag.text(), t.mmddyyyy()  );
                    }
                else if ( tag == shot_time_tag )
                    {
                    if ( !have_time )
                        t.set( b->f.shot_time );
                    s.replace( tag.text(), t.hhmmss() );
                    }
                else
                    {
                    s.replace( tag.text(), UNKNOWN );
                    }
                }
            f.writeline( s );
            }

        /*
        ----------------------------
        Write one line per parameter
        ---------------------------- */
        n = pc.count();
        for ( i=0; i<n; i++ )
            {
            s = setup;
            while ( tag.get_substring( s, LeftCurlyBracket, RightCurlyBracket ) )
                {
                if ( tag == param_number_tag )
                    {
                    s.replace( tag.text(), int32toasc(i+1) );
                    }
                else if ( tag == param_name_tag )
                    {
                    s.replace( tag.text(), pc.parameter[i].name );
                    }
                else if ( tag == param_value_tag )
                    {
                    if ( pc.parameter[i].vartype == TEXT_VAR )
                        {
                        fn.null();
                        for ( j=0; j<b->f.nof_text_parameters; j++ )
                            {
                            if ( b->f.text_parameter[j].parameter_number == (i+1) )
                                {
                                fn = b->f.text_parameter[j].value;
                                break;
                                }
                            }
                        }
                    else
                        {
                        fn = b->param[i];
                        }
                    s.replace( tag.text(), fn.text() );
                    }
                else if ( tag == param_units_tag )
                    {
                    s.replace( tag.text(), units_name(pc.parameter[i].units) );
                    }
                else if ( pc.parameter[i].vartype != TEXT_VAR )
                    {
                    if ( tag == param_high_alarm_tag )
                        {
                        limit = pc.parameter[i].limits[ALARM_MAX].value;
                        s.replace( tag.text(), limit.text() );
                        }
                    else if ( tag == param_low_alarm_tag )
                        {
                        limit = pc.parameter[i].limits[ALARM_MIN].value;
                        s.replace( tag.text(), limit.text() );
                        }
                    else if ( tag == param_high_warning_tag )
                        {
                        limit = pc.parameter[i].limits[WARNING_MAX].value;
                        s.replace( tag.text(), limit.text() );
                        }
                    else if ( tag == param_low_warning_tag )
                        {
                        limit = pc.parameter[i].limits[WARNING_MIN].value;
                        s.replace( tag.text(), limit.text() );
                        }
                    else if ( tag == param_goodness_tag )
                        {
                        value = (float) b->param[i];
                        if ( pc.parameter[i].is_alarm(value) )
                            cp = EosHaveAlarmShot;
                        else if ( pc.parameter[i].is_warning(value) )
                            cp = EosHaveWarningShot;
                        else
                            cp = EosHaveGoodShot;
                        s.replace( tag.text(), cp );
                        }
                    else
                        {
                        s.replace( tag.text(), UNKNOWN );
                        }
                    }
                else
                    {
                    s.replace( tag.text(), UNKNOWN );
                    }
                }
            f.writeline( s );
            }
        f.close();
        }
    }
}

/***********************************************************************
*                         MAKE_OUTPUT_LINE                             *
*       The "setup" is the EosComSetup or EosOutputFileSetup.          *
***********************************************************************/
static TCHAR * make_output_line( SHOT_BUFFER_ENTRY * b, TCHAR * setup, PARAMETER_CLASS & pc )
{
const int32 SLEN = 258 + MAX_INTEGER_LEN + ALPHADATE_LEN + 1 + ALPHATIME_LEN + (MAX_PARMS*MAX_FLOAT_LEN) + MAX_PARMS +1;
/* I use 258 because I know the machine name plus the part name is less than 255 */
int32          j;
int32          n;
TCHAR          c;
TCHAR        * bp;
TCHAR        * buf;
TCHAR        * cp;
TCHAR        * dp; /* Used for replacing the comma with a cr or lf */
TCHAR        * tp; /* Used for truncating the shot number S6L2 or S6R4 */
BOOLEAN        firstfield;
BOOLEAN        need_substitute;
float          x;
SYSTEMTIME     st;
NAME_CLASS     s;
PART_CLASS     part;
SETPOINT_CLASS sp;

buf = maketext( SLEN );
if ( !buf )
    return 0;

FileTimeToSystemTime( &b->f.shot_time, &st );

s.get_machset_csvname( b->pn.computer, b->pn.machine );
if ( sp.get(s) )
    Worker.find( sp[MACHSET_OPERATOR_NUMBER_INDEX].value.text() );

firstfield = TRUE;
cp = setup;
bp = buf;

while ( TRUE )
    {
    if ( *cp == NullChar )
        break;

    if ( firstfield )
        {
        firstfield = FALSE;
        }
    else if ( HaveDelimitChar )
        {
        *bp = EosComDelimitChar;
        bp++;
        }

    if ( is_numeric(*cp) )
        {
        n = asctoint32( cp );
        if ( n >= 0 && n <= MAX_PARMS )
            {
            if ( pc.parameter[n].vartype == TEXT_VAR )
                {
                for ( j=0; j<b->f.nof_text_parameters; j++ )
                    {
                    if ( b->f.text_parameter[j].parameter_number == (n+1) )
                        {
                        copystring( bp, b->f.text_parameter[j].value.text() );
                        break;
                        }
                    }
                }
            else
                {
                rounded_double_to_tchar( bp, b->param[n] );
                if ( EosFixedWidth > 0 )
                    rjust( bp, (short) EosFixedWidth, SpaceChar );
                }
            }
        else
            {
            lstrcpy( bp, unknown_string() );
            }
        }
    else if ( *cp == CChar )
        {
        /*
        --------------------
        This is a cmi robot.
                  10        20
        0123456789_123456789_
        mm/dd/yy,hh,mm,ss\015
        -------------------- */
        lstrcpy( bp, alphadate(st) );
        bp[6] = bp[8];
        bp[7] = bp[9];
        bp[8] = CommaChar;
        bp += 9;
        lstrcpy( bp, alphatime(st) );
        bp[2] = CommaChar;
        bp[5] = CommaChar;
        bp[8] = CrChar;
        bp[9] = NullChar;
        }
    else if ( *cp == LChar )
        {
        if ( bp > buf && HaveDelimitChar )
            {
            bp--;
            if ( *bp != EosComDelimitChar )
                bp++;
            }
        *bp = LfChar;
        bp++;
        *bp = NullChar;
        firstfield = true;
        }
    else if ( *cp == RChar )
        {
        if ( bp > buf && HaveDelimitChar )
            {
            bp--;
            if ( *bp != EosComDelimitChar )
                bp++;
            }
        *bp = CrChar;
        bp++;
        *bp = NullChar;
        firstfield = true;
        }
    else if ( *cp == GChar )
        {
        if ( b->has_alarm )
            copystring( bp, EosHaveAlarmShot );
        else if ( b->has_warning )
            copystring( bp, EosHaveWarningShot );
        else
            copystring( bp, EosHaveGoodShot );
        }
    else if ( *cp == AChar )
        {
        need_substitute = TRUE;
        /*
        -------------------------------------------
        The parameter number in the tag is an index
        ------------------------------------------- */
        j = asctoint(cp+1);
        if ( j>=0 && j<MAX_PARMS )
            {
            if ( pc.parameter[j].vartype != TEXT_VAR )
                {
                x = (float) b->param[j];
                if ( pc.parameter[j].has_any_limits() )
                    {
                    if ( pc.parameter[j].is_alarm(x) )
                        {
                        copystring( bp, EosHaveAlarmParameter );
                        need_substitute = FALSE;
                        }
                    else if ( pc.parameter[j].is_warning(x) )
                        {
                        copystring( bp, EosHaveWarningParameter );
                        need_substitute = FALSE;
                        }
                    }
                }
            }
        if ( need_substitute )
            copystring( bp, EosHaveGoodParameter );
        }
    else if ( *cp == MChar )
        {
        lstrcpy( bp, b->pn.machine.text() );
        }
    else if ( *cp == OChar )
        {
        if ( *(cp+1) == FChar )
            copystring( bp, Worker.first_name() );
        else if ( *(cp+1) == LChar )
            copystring( bp, Worker.last_name() );
        else if ( *(cp+1) == AChar )
            copystring( bp, Worker.full_name() );
        else
            {
            copystring( bp, sp[MACHSET_OPERATOR_NUMBER_INDEX].value.text() );
            strip( bp );
            }
        }
    else if ( *cp == PChar && *(cp+1) == DChar )
        {
        n = 0;
        if ( is_numeric(*(cp+2)) )
            n = asctoint32(cp+2);
        if ( part.find(b->pn.computer, b->pn.machine, b->pn.part) )
            copystring( bp, part.sp[PARTSET_PLUNGER_DIA_INDEX].value.text() );
        else
            copystring( bp, TEXT("1.0") );
        if ( n > 0 )
            rjust( bp, n, SpaceChar );
        }
    else if ( *cp == PChar )
        {
        lstrcpy( bp, b->pn.part.text() );
        }
    else if ( *cp == SChar )
        {
        n = 0;
        if ( is_numeric(*(cp+1)) )
            n = asctoint32(cp+1);
        copystring( bp, b->pn.shot.text() );
        if ( n > 0 )
            {
            rjust( bp, n, ZeroChar );
            }
        else if ( *bp != NullChar )
            {
            n = lstrlen( bp );
            }

        /*
        ------------------
        Check for L2 or R4
        ------------------ */
        tp = cp+1;
        while ( *tp != CommaChar && *tp != NullChar )
            {
            if ( *tp == LChar )
                {
                j = asctoint32(tp+1);
                if ( j < n )
                    *(bp+j) = NullChar;
                break;
                }
            else if ( *tp == RChar )
                {
                j = asctoint32(tp+1);
                if ( j < n )
                    {
                    j = n - j;
                    copystring( bp, bp+j );
                    }
                break;
                }
            tp++;
            }

        if ( *bp != NullChar )
            bp += lstrlen(bp);;
        }
    else if ( *cp == DChar )
        {
        lstrcpy( bp, alphadate(st) );
        /*
        --------------------------
        If "D0" remove the slashes
        -------------------------- */
        cp++;
        if ( *cp == ZeroChar )
            {
            *(bp+2) = *(bp+3);
            *(bp+3) = *(bp+4);
            *(bp+4) = *(bp+8);
            *(bp+5) = *(bp+9);
            *(bp+6) = NullChar;
            }
        else
            {
            cp--;
            }
        }
    else if ( *cp == QChar )
        {
        cp++;
        dp = cp;
        while ( *dp != CommaChar && *dp != NullChar )
            dp++;
        c = *dp;
        *dp = NullChar;
        copystring( bp, cp );
        *dp = c;
        convert_control_chars( bp );
        }
    else if ( *cp == YChar )
        {
        copystring( bp, alphadate(st) );
        dp = bp + ALPHADATE_LEN -1;
        if ( *(cp+1) == TwoChar )
            dp--;
        copystring( bp, dp );
        }
    else if ( *cp == JChar )
        {
        j = dofyr( st );
        j++;
        n = 0;
        if ( is_numeric(*(cp+1)) )
            n = asctoint32(cp+1);
        copystring( bp, int32toasc(j)  );
        if ( n > 0 )
            {
            rjust( bp, n, ZeroChar );
            bp += n;
            }
        else if ( *bp != NullChar )
            {
            bp += lstrlen(bp);;
            }
        *bp = NullChar;
        }
    else if ( *cp == TChar )
        {
        lstrcpy( bp, alphatime(st) );
        /*
        -------------------------
        If "T0" remove the colons
        ------------------------- */
        cp++;
        if ( *cp == ZeroChar )
            {
            *(bp+2) = *(bp+3);
            *(bp+3) = *(bp+4);
            *(bp+4) = *(bp+6);
            *(bp+5) = *(bp+7);
            *(bp+6) = NullChar;
            }
        else if ( *cp == OneChar )
            {
            *(bp+2) = *(bp+3);
            *(bp+3) = *(bp+4);
            *(bp+4) = NullChar;
            }
        else
            {
            cp--;
            }
        }

    if ( *bp != NullChar )
        bp += lstrlen(bp);

    while ( *cp != NullChar )
        {
        if ( *cp == CommaChar )
            {
            cp++;
            break;
            }
        cp++;
        }
    }

if ( *EosComEol != NullChar )
    bp = copystring( bp, EosComEol );

*bp = NullChar;

return buf;
}

/***********************************************************************
*                         WRITE_SERIAL_PORT_INFO                       *
***********************************************************************/
static void write_serial_port_info( SHOT_BUFFER_ENTRY * b, PARAMETER_CLASS & pc )
{
TCHAR      * bp;
TCHAR      * buf;
DWORD        bytes_to_write;
char       * cbuf;
INI_CLASS    ini;
bool         is_unicode;
int          n;
NAME_CLASS   s;
SYSTEMTIME   st;
STRING_CLASS shotname;

is_unicode = false;
#ifdef UNICODE
is_unicode = true;
#endif

/*
----------------------------------------------------------------------------------------------
If I am monitoring multiple machines I can only (at the present time) send output to one stamp
machine. To do this you must define EosMachineName in monall.ini
---------------------------------------------------------------------------------------------- */
if ( EosMachineName )
    {
    if ( b->pn.machine != EosMachineName )
        return;
    }

if ( EosOnlyIfNoAlarms )
    {
    if ( b->has_alarm )
        return;
    }

shotname = b->pn.shot;
if ( shotname.len() < SHOT_LEN )
    shotname.rjust( SHOT_LEN, ZeroChar );

FileTimeToSystemTime( &b->f.shot_time, &st );

if ( EosPyramidLines == OneChar )
    {
    buf = maketext( ALPHADATE_LEN + shotname.len() + 2 );
    if ( !buf )
        return;
    bp = buf;
    lstrcpy( bp, alphadate(st) );
    *(bp+2) = *(bp+3);
    *(bp+3) = *(bp+4);
    *(bp+4) = *(bp+8);
    *(bp+5) = *(bp+9);
    *(bp+6) = SpaceChar;
    bp += 7;
    bp  = copystring( bp, shotname.text() );
    *bp = CrChar;
    bp++;
    *bp = NullChar;
    }
else if ( EosPyramidLines == TwoChar )
    {
    buf = maketext( ALPHADATE_LEN + shotname.len() + 8 );
    if ( !buf )
        return;
    bp = buf;
    bp = copystring( bp, TEXT("V01") );
    lstrcpy( bp, alphadate(st) );
    *(bp+2) = *(bp+3);
    *(bp+3) = *(bp+4);
    *(bp+4) = *(bp+8);
    *(bp+5) = *(bp+9);
    *(bp+6) = CrChar;
    bp += 7;
    bp = copystring( bp, TEXT("V02") );
    bp  = copystring( bp, shotname.text() );
    *bp = CrChar;
    bp++;
    *bp = NullChar;
    }
else
    {
    if ( EosNeedsStxEtx )
        write_bytes_to_serial_port( StxBuf, 1 );
    write_hex_file_to_serial_port( EosStartFile );
    buf = make_output_line( b, EosComSetup, pc );
    }

if ( is_unicode )
    {
    s.get_local_ini_file_name( MonallIniFile );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    if ( ini.find(EosAsciiToSerialKey) )
        EosAsciiToSerial = ini.get_boolean();
    }

if ( EosAsciiToSerial )
    {
    n = lstrlen( buf );
    cbuf = new char[n+1];
    unicode_to_char( cbuf, buf );
    bytes_to_write = (DWORD) n;
    write_bytes_to_serial_port( cbuf, bytes_to_write );
    delete[] cbuf;
    cbuf = 0;
    }
else
    {
    bytes_to_write = lstrlen(buf) * sizeof(TCHAR);
    write_bytes_to_serial_port( buf, bytes_to_write );
    }

write_hex_file_to_serial_port( EosEndFile );

if ( EosNeedsStxEtx )
    write_bytes_to_serial_port( EtxBuf, 1 );

delete[] buf;
}

/***********************************************************************
*                           WRITE_OUTPUT_FILE                          *
***********************************************************************/
static void write_output_file( SHOT_BUFFER_ENTRY * b, PARAMETER_CLASS & pc )
{
TCHAR * setup;
TCHAR * buf;
DWORD   bytes_to_write;

FILE_CLASS      f;
NAME_CLASS      fn;
PART_NAME_ENTRY p;
TIME_CLASS      t;

if ( !EosOutputFile )
    return;

if ( !EosOutputFileSetup )
    setup = EosComSetup;
else
    setup = EosOutputFileSetup;

buf = make_output_line( b, setup, pc );
if ( !buf )
    return;

bytes_to_write = lstrlen(buf) * sizeof( TCHAR );

if ( bytes_to_write )
    {
    fn = EosOutputFile;
    if ( fn.contains(LeftCurlyBracket) )
        {
        p.computer = b->pn.computer;
        p.machine  = b->pn.machine;
        p.part     = b->pn.part;
        t.set( b->f.shot_time );
        fn = dynamic_file_name( t, p, fn.text() );
        }

    fn.ensure_file_directory_existance();

    if ( EosKillOutputFile )
        fn.delete_file();

    if ( f.open_for_append(fn) )
        {
        f.writebinary( buf, bytes_to_write );
        f.close();
        }
    }

delete[] buf;
}

/***********************************************************************
*                             * BIT_STATE                              *
***********************************************************************/
static TCHAR * bit_state( unsigned x )
{
static TCHAR On[] = TEXT( " ON" );
static TCHAR Off[] = TEXT( " OFF" );
if ( x )
    return On;
return Off;
}

/***********************************************************************
*                               MAKE_LINE                              *
***********************************************************************/
static void make_line( STRING_CLASS & s, unsigned x, double timer, int index )
{
TCHAR buf[100];

s += bit_state( x );
if ( index > 0 )
    {
    s += TEXT( " at time: " );
    if ( double_to_tchar( buf, timer, 3) )
        s += buf;
    }
}

/***********************************************************************
*                            WRITE_ALS_LOG                             *
***********************************************************************/
static void write_als_log( TCHAR * shot_number, FTII_FILE_DATA & fd )
{
/*
-------
Outputs
------- */
static unsigned slow_shot_output_mask   = 0x10000;
static unsigned accumulator_output_mask = 0x20000;

/*
------
Inputs
------ */
static unsigned retract_mask        = 0x80000;
static unsigned follow_through_mask = 0x40000;
static unsigned start_shot_mask     = 0x20000;
static unsigned accumulator_mask    = 0x40000000;
static unsigned slow_shot_mask      = 0x20000000;

FILE_CLASS f;
int      i;
unsigned last_input;
unsigned last_output;
unsigned x;
double   seconds;
double   real_timer_frequency;

STRING_CLASS s;
FTII_PROFILE fp;

if ( !AlsLogIsActive )
    return;

fp = fd;
real_timer_frequency = (double) fp.timer_frequency;
if ( is_zero(real_timer_frequency) )
    real_timer_frequency = 1.0;

if ( !f.open_for_append(AlsLogFile) )
    return;

f.writeline( TEXT(" ") );

s = TEXT( "Begin Shot Data: Shot Number " );
s += shot_number;
f.writeline( s.text() );

last_input = ~fp.isw1[0];
last_output = ~fp.osw1[0];

f.writeline( TEXT("--------------- At the Start of shot-------------------") );

for ( i=0; i<fp.n; i++ )
    {
    seconds = (double) fp.timer[i];
    seconds /= real_timer_frequency;
    x = fp.isw1[i];
    if ( (x & start_shot_mask) != (last_input & start_shot_mask) )
        {
        s = TEXT( "Start Shot     INPUT" );
        make_line( s, (x & start_shot_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & retract_mask) != ( last_input & retract_mask) )
        {
        s = TEXT( "Retract        INPUT" );
        make_line( s, (x & retract_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & follow_through_mask) != ( last_input & follow_through_mask) )
        {
        s = TEXT( "Follow Through INPUT" );
        make_line( s, (x & follow_through_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & slow_shot_mask) != ( last_input & slow_shot_mask) )
        {
        s = TEXT( "Slow Shot      INPUT" );
        make_line( s, (x & slow_shot_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & accumulator_mask) != ( last_input & accumulator_mask) )
        {
        s = TEXT( "Accumulator    INPUT" );
        make_line( s, (x & accumulator_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( i == fp.np )
        {
        s = TEXT( "Impact at time: " );
        s += ascii_double(seconds);
        f.writeline( s.text() );
        }

    x = fp.osw1[i];
    if ( (x & slow_shot_output_mask) != (last_output & slow_shot_output_mask) )
        {
        s = TEXT( "Slow Shot      OUTPUT" );
        make_line( s, (x & slow_shot_output_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( (x & accumulator_output_mask) != (last_output & accumulator_output_mask) )
        {
        s = TEXT( "Accumulator    OUTPUT" );
        make_line( s, (x & accumulator_output_mask), seconds, i );
        f.writeline( s.text() );
        }
    if ( i == 0 )
        f.writeline( TEXT("-------------------------------------------------------") );

    last_input  = fp.isw1[i];
    last_output = fp.osw1[i];
    }

f.writeline( TEXT("End of Shot Data") );
f.writeline( TEXT(" ") );
f.close();
}

/***********************************************************************
*                          write_std_units_csv_file                    *
***********************************************************************/
BOOLEAN write_std_units_csv_file( STRING_CLASS & dest, SHOT_BUFFER_ENTRY * b )
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
PART_CLASS   pa;

if ( firstime )
    {
    AnalogSensor.get();
    firstime = FALSE;
    }

pa.find( b->pn.computer, b->pn.machine, b->pn.part );

fp = b->f;

real_timer_frequency = (double) fp.timer_frequency;

if ( !f.open_for_write(dest) )
    return FALSE;

f.writeline( title );

/*
-----------------------------------------
The next row is the units for each column
----------------------------------------- */
s = TEXT( "Pos/Time,Count," );
s += units_name( pa.distance_units );
s += CommaChar;
s += units_name( SECOND_UNITS );
s += CommaChar;
s += units_name( pa.velocity_units );
s += TEXT( ",Hex Bits,Hex Bits" );
for ( c=0; c<NOF_FTII_ANALOGS; c++ )
    {
    s += CommaChar;
    ai = (int) pa.analog_sensor[c];
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

    x = pa.dist_from_x4( (double) fp.position[i] );
    s += x;
    s += CommaChar;

    x = (double) fp.timer[i];
    x /= real_timer_frequency;
    s += x;
    s += CommaChar;

    x = (double) fp.velocity[i];
    x = pa.velocity_from_x4( x );
    s += x;
    s += CommaChar;

    s.cat_w_char( ultohex( fp.isw1[i]), CommaChar );
    s += ultohex( fp.osw1[i] );

    for ( c=0; c<NOF_FTII_ANALOGS; c++ )
        {
        s += CommaChar;
        ai = (int) pa.analog_sensor[c];
        s += AnalogSensor[ai].converted_value( fp.analog[c][i] );
        }
    f.writeline( s.text() );
    }

f.close();

return TRUE;
}

/***********************************************************************
*                               SAVE_SHOT                              *
***********************************************************************/
void save_shot( SHOT_BUFFER_ENTRY * b )
{
static BOOLEAN have_enough_disk_space = TRUE;
TCHAR         * cp;
int             n;
short           record_length;
short           dif;
short           field_length;
short           shot_name_len;
FIELDOFFSET     fo;
DB_TABLE        t;
PARAMETER_CLASS pc;
BITSETYPE       save_flags;
NAME_CLASS      s;
STRING_CLASS    shot_name;
SYSTEMTIME      st;
DWORD           file_attributes;
EXTERNAL_PARAMETER_SETTINGS_CLASS eps;
WRITE_CSV_SHOT_FILE csv_shot_file;

s.get_data_dir_file_name( b->pn.computer, 0 );
if ( *s.text() != BackSlashChar )
    {
    if ( free_megabytes(s.text()) < MIN_FREE_DISK_MB )
        {
        if ( have_enough_disk_space )
            {
            have_enough_disk_space = FALSE;
            resource_message_box( MainInstance, LOW_DISK_SPACE_STRING, NO_SHOTS_SAVED_STRING, MB_OK | MB_SYSTEMMODAL );
            }
        return;
        }
    else
        {
        have_enough_disk_space = TRUE;
        }
    }

if ( !nextshot(b) )
    return;

save_flags = skip_new_shot_notify( b->pn.machine, b->f.shot_time );

if ( SaveAllAlarmShots && ( b->has_alarm || b->has_warning) )
    {
    save_flags |= SAVE_PARM_DATA;
    save_flags |= SAVE_PROFILE_DATA;
    }

write_als_log( b->pn.shot.text(), b->f );

s.get_profile_ii_name(  b->pn.computer, b->pn.machine, b->pn.part, b->pn.shot );

/*
-----------------
Write the profile
----------------- */
b->f.put( s );

FileTimeToSystemTime( &b->f.shot_time, &st );

/*
-------------------------------------------------------------
Turn on the archive bit if the Data Archiver will be notified
------------------------------------------------------------- */
eps.get( b->pn.computer, b->pn.machine, b->pn.part );
if ( eps.enable && eps.available_at_eos )
    file_attributes = FILE_ATTRIBUTE_NORMAL;
else
    file_attributes = FILE_ATTRIBUTE_ARCHIVE;
SetFileAttributes( s.text(), file_attributes );

if ( file_attributes == FILE_ATTRIBUTE_ARCHIVE )
    {
    if ( csv_shot_file.get( b->pn ) )
        {
        s = csv_shot_file.dest_file_dir;
        s.cat_path( b->pn.shot );
        s += CSV_SUFFIX;

        if ( csv_shot_file.is_enabled && !csv_shot_file.dest_file_dir.isempty() )
            write_std_units_csv_file( s, b );
        }
    }

/*
---------------------
Update the graph list
--------------------- */
s.get_graphlst_dbname( b->pn.computer, b->pn.machine, b->pn.part );
t.ensure_existance(s);

shot_name_len = (short) shot_name_length( b->pn );
field_length = shot_name_len;
dif = field_length + 2 - SHOT_LEN;
record_length = GRAPHLST_RECLEN + dif;

if ( t.open(s, record_length, PWL) )
    {
    fo = (FIELDOFFSET) GRAPHLST_SHOT_NUMBER_OFFSET;
    t.put_alpha( fo, b->pn.shot, shot_name_len );
    fo = (FIELDOFFSET) GRAPHLST_DATE_OFFSET + dif;
    t.put_date( fo, st );
    fo = (FIELDOFFSET) GRAPHLST_TIME_OFFSET + dif;
    t.put_time( fo, st );
    fo = (FIELDOFFSET) GRAPHLST_SAVE_FLAGS_OFFSET + dif;
    t.put_short( fo, (short) save_flags, GRAPHLST_SAVE_FLAGS_LEN );
    t.rec_append();

    /*
    ----------------------------------------------------------------------
    Check back 51 shots and see if I should delete the record and profile.
    ---------------------------------------------------------------------- */
    n = t.nof_recs();
    if ( n > HistoryShotCount )
        {
        n -= HistoryShotCount;
        /*
        --------------------------------------------------
        Subtract one more because the last record is n - 1
        -------------------------------------------------- */
        n--;

        t.goto_record_number(n);
        t.get_rec();

        /*
        ------------------------------------
        See if I need to delete this profile
        ------------------------------------ */

        save_flags = (BITSETYPE) t.get_short( GRAPHLST_SAVE_FLAGS_OFFSET + dif );
        if ( !(save_flags & SAVE_PROFILE_DATA) )
            {
            t.get_alpha( shot_name, GRAPHLST_SHOT_NUMBER_OFFSET, shot_name_len );
            t.rec_delete();
            s.get_profile_ii_name( b->pn.computer, b->pn.machine, b->pn.part, shot_name );
            s.delete_file();
            }

        n = max_profiles_saved( b->pn.machine );
        if ( n > 0 && n < (int32) t.nof_recs() )
            {
            t.goto_record_number(0);
            t.get_rec();
            t.get_alpha( shot_name, GRAPHLST_SHOT_NUMBER_OFFSET, shot_name_len );
            t.rec_delete();
            s.get_profile_ii_name( b->pn.computer, b->pn.machine, b->pn.part, shot_name );
            s.delete_file();
            }
        }

    t.close();
    }

write_ftii_parameters( b, st, save_flags );

s.empty();
n = b->pn.machine.len() + b->pn.part.len() + 1;
if ( s.init(n) )
    {
    s = b->pn.machine;
    s += CommaChar;
    s += b->pn.part;
    cp = maketext( b->pn.shot.text() );
    if ( cp )
        MainWindow.post( WM_NEWSHOT, (WPARAM) cp, (LPARAM) s.release() );
    }

cp = make_parameter_event_string( b );
if ( cp )
    MainWindow.post( WM_POKEMON, (WPARAM) PARAM_DATA_INDEX, (LPARAM) cp );
pc.find( b->pn.computer, b->pn.machine, b->pn.part );
if ( EosComPort )
    write_serial_port_info( b, pc );

if ( EosOutputFile )
    write_output_file( b, pc );

write_eos_param_file( b, pc );

/*
---------------------------------------------
Post a message to the new data thread in case
the shot save complete output wire is set.
--------------------------------------------- */
s = ShotSaveCompleteMessage;
s += CommaChar;
s += b->pn.machine;
post_new_data_message( s );
}

/***********************************************************************
*                          SAVE_THIS_MACHINE                           *
***********************************************************************/
static void save_this_machine( STRING_CLASS & machine_to_save )
{
int  i;
int  i_to_save;
FILETIME time_to_save;
FILETIME new_time;
SHOT_BUFFER_ENTRY * b;

/*
-------------------------------------
Find the oldest shot for this machine
------------------------------------- */
i_to_save = NO_INDEX;

for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
    {
    if ( BufPtr[i] )
        {
        b = BufPtr[i];
        if ( b->pn.machine == machine_to_save )
            {
            new_time = b->f.shot_time;
            if ( i_to_save == NO_INDEX )
                {
                i_to_save = i;
                time_to_save = new_time;
                }
            else if ( seconds_difference(new_time, time_to_save) < 0 )
                {
                i_to_save = i;
                time_to_save = new_time;
                }
            }
        }

    }

if ( i_to_save != NO_INDEX )
    {
    b = BufPtr[i_to_save];
    if ( get_shotsave_semaphore(b->pn.computer, b->pn.machine) )
        {
        save_shot( b );
        free_shotsave_semaphore();
        freebuf( i_to_save );
        }
    else
        {
        /*
        --------------------------------------------------
        Mark all the shots for this machine as save failed
        -------------------------------------------------- */
        for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
            {
            if ( BufPtr[i] )
                {
                b = BufPtr[i];
                if ( b->pn.machine == machine_to_save )
                    b->save_failed = true;
                }
            }
        }
    }
}

/***********************************************************************
*                            SAVE_ALL_SHOTS                            *
***********************************************************************/
void save_all_shots()
{
static int current_index = 0;
int                 first_index;
SHOT_BUFFER_ENTRY * b;

first_index = current_index;
while ( TRUE )
    {
    if ( BufPtr[current_index] )
        {
        b = BufPtr[current_index];
        if ( !b->save_failed )
            save_this_machine( b->pn.machine );
        }
    current_index++;
    if ( current_index >= MAX_SHOTSAVE_BUFFERS )
        current_index = 0;
    if ( current_index == first_index )
        break;
    }
}

/***********************************************************************
*                       SHOTSAVE_THREAD                                *
***********************************************************************/
void shotsave_thread( void * notused )
{
DWORD status;
DWORD ms_to_wait;
int   i;

startup();

ShotSaveIsRunning = true;

while ( TRUE )
    {
    ms_to_wait = INFINITE;
    for ( i=0; i<MAX_SHOTSAVE_BUFFERS; i++ )
        {
        if ( BufPtr[i] )
            {
            ms_to_wait = SHOTSAVE_WAIT_TIMEOUT;
            BufPtr[i]->save_failed = false;
            }
        }

    status = WaitForSingleObject( ShotSaveEvent, ms_to_wait );
    ResetEvent( ShotSaveEvent );
    if ( status == WAIT_FAILED )
        resource_message_box( MainInstance, UNABLE_TO_CONTINUE_STRING, SHOTSAVE_WAIT_FAIL_STRING, MB_OK | MB_SYSTEMMODAL );

    save_all_shots();

    if ( ShutdownState == SHUTTING_DOWN_SHOTSAVE )
        {
        ShotSaveIsRunning = false;
        break;
        }

    }

shutdown();
}

/***********************************************************************
*                        START_SHOTSAVE_THREAD                         *
***********************************************************************/
void start_shotsave_thread()
{
unsigned long h;

h = _beginthread( shotsave_thread, 0, NULL );
if ( !h )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, SHOTSAVE_THREAD_STRING, MB_OK | MB_SYSTEMMODAL );

}
