#ifdef _DEBUG_DONT_USE_THIS_NOW
//#define _CRTDBG_MAP_ALLOC
#endif
#include <windows.h>
#include <process.h>
#include <ddeml.h>

#ifdef _DEBUG_DONT_USE_THIS_NOW
#include <crtdbg.h>
#endif

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\alarmsum.h"
#include "..\include\array.h"
#include "..\include\asensor.h"
#include "..\include\chars.h"
#include "..\include\dstat.h"
#include "..\include\evclass.h"
#include "..\include\external_parameter.h"
#include "..\include\fileclas.h"
#include "..\include\ftanalog.h"
#include "..\include\computer.h"
#include "..\include\fifo.h"
#include "..\include\fontclas.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\multipart_runlist.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\ftcalc.h"
#include "..\include\rectclas.h"
#include "..\include\runlist.h"
#include "..\include\shotname.h"
#include "..\include\static_text_parameter.h"
#include "..\include\strarray.h"
#include "..\include\stringtable.h"
#include "..\include\stddown.h"
#include "..\include\stpres.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\vdbclass.h"
#include "..\include\wclass.h"
#include "..\include\ftii.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\limit_switch_class.h"

#include "extern.h"
#include "resource.h"
#include "upload_copy_class.h"
#include "shotsave.h"
#include "..\include\events.h"
#include "ftii_board_entry.h"

extern STRINGTABLE_CLASS  StringTable;

const unsigned short FIRST_PACKET_NUMBER = 1;
const static int CTRL_PROG_NUMBER_LEN = 4;   /* Used to compare line numbers like .022 */
const static int VARIABLE_NUMBER_LEN  = 4;   /* Used to compare strings that begin V447, etc */

const BOARD_DATA VIS_AUTO_SHOT = 0x0400;
const BOARD_DATA VIS_TIME_OUT  = 0x0080;

static unsigned ALS_BITS = 0x600E0000;
static unsigned FOLLOW_THROUGH_MASK = 0x40000;
static unsigned START_SHOT_MASK     = 0x20000;
static unsigned RETRACT_MASK        = 0x80000;
static unsigned ACCUMULATOR_MASK    = 0x40000000;
static unsigned SLOW_SHOT_MASK      = 0x20000000;

static HANDLE       SendEvent = 0;
static HANDLE       NewDataEvent = 0;

static bool         Wsa_Is_Started = false;
static SOCKET       MySocket = INVALID_SOCKET;
static sockaddr_in  MyAddress;

static bool         Read_Thread_Is_Running     = false;
static bool         ShuttingDown         = false;

static FIFO_CLASS   ReceivedAcks;
static FIFO_CLASS   AckList;
static FIFO_CLASS   NewData;

static FIFO_CLASS   SocketMessageStore;     /* Unused SOCKET_MESSAGEs */
static FIFO_CLASS   AckMessageStore;        /* Unused ACK_MESSAGEs    */

const  char AckChar  = '\006';
const  char CrCharAscii   = '\r';
const  char NakChar  = '\025';
const  int  AckLen   = 2;
const  char PlusCharAscii = '+';

const int MAX_NAK_COUNT   = 3;
const int MAX_MESSAGE_LEN = 1350;

// FasTrak3 TCP/IP refactor
#define FT3_ASYNC_TYPE_BIT         (1 << 7)
#define FT3_ASYNC_TYPE_HEADER_LEN  12
#define FT3_RESPONSE_TYPE_LEN      256
#define FT3_RECV_BUFFER_LEN        1350

static TCHAR FT3_ECHO_REQUEST[] = TEXT("*\r");
static const char FT3_ECHO_RESPONSE_INIT = '*';
static const bool FT3_ECHO_RESPONSE_VERBOSE = false;

/*
-------------
Upload States
------------- */
const int NO_UPLOAD_STATE      = 0;
const int WAITING_FOR_INFO     = 1;  /* Waiting for version, v447, v448 */
const int UPLOAD_ALL           = 2;  /* Upload control programm and all variables */
const int UPLOAD_ALL_VARIABLES = 3;  /* Upload all variables */
const int UPLOAD_NEW_VARIABLES = 4;  /* Upload only variables that have changed */
const int SEND_THREAD_START_FAILURE = 5; /* Unable to start send thread */
const int SEND_THREAD_WAIT_FAILURE  = 6; /* Unable to wait in send thread */
static const unsigned NO_FILE_DATE = 0x80000000;

struct ACK_MESSAGE {
    sockaddr_in a;
    char        s[AckLen];
    ACK_MESSAGE() { s[0]=AckChar; s[1]=CrCharAscii; }
    };

struct SOCKET_MESSAGE {
    sockaddr_in a;
    char        s[MAX_MESSAGE_LEN+1];
    int         len;
    };

FTII_BOARD_ENTRY * Boards    = 0;
int                NofBoards = 0;
static STDDOWN_CLASS        StdDown;

const TCHAR CrString[]     = TEXT( "\r" );
const TCHAR EqualsString[] = TEXT( "=" );
const TCHAR VString[]      = TEXT( "V" );

const  TCHAR ConfigSetString[]         = TEXT( "V313=H" );
static TCHAR ControlFileDateString[]   = TEXT( "V447" );
static TCHAR ControlStatusString[]     = TEXT( "V311" );
static TCHAR FlashVariablesString[]    = TEXT( "VF\r" );
static TCHAR GoString[]                = TEXT( ".G\r" );
static TCHAR IsControlSetString[]      = TEXT( "V500=H" );
static TCHAR DigitalPotGainSetString[] = TEXT( "V431=H" );
static TCHAR GetAllAnalogInputs[]      = TEXT( "OAA\r"  );
static TCHAR GetControlFileDateString[]= TEXT( "V447\r" );
static TCHAR GetControlStatusString[]  = TEXT( "V311\r" );
static TCHAR GetMonitorStatusString[]  = TEXT( "V312\r" );
static TCHAR GetMSLString[]            = TEXT( "V319\r" );
static TCHAR GetTimerFrequencyString[] = TEXT( "V301\r" );
static TCHAR GetInputBitsString[]      = TEXT( "OI1\r"  );
static TCHAR GetUploadCopyDateString[] = TEXT( "V448\r" );
static TCHAR GetVersionString[]        = TEXT( "OV\r" );
static TCHAR MonitorStatusString[]     = TEXT( "V312" );
static TCHAR ConfigWord2SetString[]    = TEXT( "V314=H" );
static TCHAR TimeoutString[]           = TEXT( "Timeout" );
static TCHAR UploadCopyDateString[]    = TEXT( "V448" );

static int PrinceCharOffset   = 13;          /* V314=H00000011 #Config word 2 */
static int RetctlCharOffset   = 9;

static TCHAR NString[]       = TEXT( "N" );
static TCHAR YString[]       = TEXT( "Y" );
static TCHAR ChannelModeSetString[] = TEXT( "V429=" );
static TCHAR OpDataPacketString[]   = TEXT( "OC3="  );
static int   ChannelModeSetStringLen= 5;
static TCHAR Ft2UdpPort[]            = TEXT( "20000" );

static TCHAR VarLineStart[]      = TEXT( "V" );
static TCHAR OldStVarLineStart[] = TEXT( ".UV" );

static const TCHAR INPUT_BITS_RESPONSE[]    = TEXT( "O_ISW1=" );
static const int   INPUT_BITS_RESPONSE_SLEN = 7;

static char  ShutdownMessage[]      = "shutdown";
static int   ShutdownMessageLen     = 8;

extern HWND * Dnu;

double * calculate_ftii_parameters( FTII_PROFILE & f, MACHINE_CLASS & machine, PART_CLASS & part, PARAMETER_CLASS & param, FTANALOG_CLASS & ftanalog, FTCALC_CLASS & ftclac, LIMIT_SWITCH_CLASS &flimits );
TCHAR * ft2_control_program_name( int flash_version, MACHINE_CLASS & m );
TCHAR * resource_string( UINT resource_id );
void resource_message_box( UINT msg_id, UINT title_id );
void push_log_message( TCHAR * sorc );
BOOLEAN write_binary_file_to_serial_port( NAME_CLASS & fn ); /* In shotsave.cpp */

/***********************************************************************
*                       ASCII_STRINGS_ARE_EQUAL                        *
***********************************************************************/
static BOOLEAN ascii_strings_are_equal( const char * s1, const char * s2, int slen )
{
while ( slen )
    {
    if ( *s1 != *s2 )
        return FALSE;
    if ( *s1 == '\0' )
        break;
    s1++;
    s2++;
    }

return TRUE;
}

/***********************************************************************
*                           ASCII_COPYSTRING                           *
***********************************************************************/
static void ascii_copystring( char * dest, const char * sorc, int maxlen )
{
while ( maxlen )
    {
    if ( *sorc == '\0' )
        break;
    *dest++ = *sorc++;
    maxlen--;
    }
*dest = '\0';
}

/***********************************************************************
*                           ASCII_COPYSTRING                           *
***********************************************************************/
static void ascii_copystring( char * dest, const char * sorc )
{

while ( true )
    {
    *dest = *sorc;
    if ( *dest == '\0' )
        break;
    dest++;
    sorc++;
    }
}

/***********************************************************************
*                          ENCODED_FILE_DATE                           *
***********************************************************************/
static unsigned encoded_file_date( NAME_CLASS & s )
{
static const unsigned mask = 0x7FFFFFFF;
FILE_CLASS f;
FILETIME  ft;
unsigned  u;

u = NO_FILE_DATE;
if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
        {
        if ( f.get_creation_time(&ft) )
            {
            u = (unsigned) ft.dwLowDateTime;
            u &= mask;
            }
        f.close();
        }
    }

return u;
}

/***********************************************************************
*                          GET_SOCKET_MESSAGE                          *
***********************************************************************/
static SOCKET_MESSAGE * get_socket_message()
{
SOCKET_MESSAGE * p;

p = 0;
if ( SocketMessageStore.count() )
    p = (SOCKET_MESSAGE *) SocketMessageStore.pop();

if ( !p )
    p = new SOCKET_MESSAGE;

return p;
}

/***********************************************************************
*                           GET_ACK_MESSAGE                            *
***********************************************************************/
static ACK_MESSAGE * get_ack_message()
{
ACK_MESSAGE * p;

p = 0;
if ( AckMessageStore.count() )
    p = (ACK_MESSAGE *) AckMessageStore.pop();

if ( !p )
    p = new ACK_MESSAGE;

return p;
}

/***********************************************************************
*                         CLEAN_MESSAGE_STORES                         *
***********************************************************************/
static void clean_message_stores()
{
SOCKET_MESSAGE * sp;
ACK_MESSAGE    * ap;

while ( SocketMessageStore.count() )
    {
    sp = (SOCKET_MESSAGE *) SocketMessageStore.pop();
    delete sp;
    }

while ( AckMessageStore.count() )
    {
    ap = (ACK_MESSAGE *) AckMessageStore.pop();
    delete ap;
    }
}

#ifdef UNICODE
/***********************************************************************
*                        START_CHARS_ARE_EQUAL                         *
***********************************************************************/
bool start_chars_are_equal( const TCHAR * sorc, const char * s )
{
union {
char  c;
TCHAR t;
};

t = NullChar;

while ( true )
    {
    if ( *sorc == NullChar )
        return true;

    c = *s;

    if ( *sorc != t )
        return false;

    sorc++;
    s++;
    }
}
#endif

/***********************************************************************
*                             BOARD_INDEX                              *
***********************************************************************/
static int board_index( sockaddr_in & sorc )
{
int i;

for ( i=0; i<NofBoards; i++ )
    {
    if ( Boards[i].a.sin_addr.S_un.S_addr == sorc.sin_addr.S_un.S_addr )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                             BOARD_INDEX                              *
***********************************************************************/
static int board_index( const FTII_BOARD_ENTRY * board_to_find )
{
int i;

i = board_to_find - Boards;

if ( i>=0 && i<NofBoards )
    return i;

return NO_INDEX;
}

/***********************************************************************
*                             BOARD_INDEX                              *
***********************************************************************/
static int board_index( const STRING_CLASS & machine_name )
{
int i;

for ( i=0; i<NofBoards; i++ )
    {
    if ( Boards[i].m.name == machine_name )
        return i;
    }

return NO_INDEX;
}

/***********************************************************************
*                            FIND_BOARD                                *
***********************************************************************/
FTII_BOARD_ENTRY * find_board( sockaddr_in & sorc )
{
int i = board_index( sorc );

if ( i != NO_INDEX )
    return &Boards[i];

return 0;
}

/***********************************************************************
*                         CURRENT_BOARD_NUMBER                         *
***********************************************************************/
int current_board_number()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].number;

return NO_BOARD_NUMBER;
}

/***********************************************************************
*                         CURRENT_BOARD_INDEX                          *
***********************************************************************/
int current_board_index()
{
return board_index( CurrentMachine.name );
}

/***********************************************************************
*                        IS_SURETRAK_CONTROLLED                        *
*  This allows the main ft2 thread to check the type of machine.       *
***********************************************************************/
BOOLEAN is_suretrak_controlled( int i )
{
if ( i>=0 && i<NofBoards )
    return Boards[i].m.suretrak_controlled;

return FALSE;
}

/***********************************************************************
*                         CURRENT_CHANNEL_MODE                         *
***********************************************************************/
int current_channel_mode()
{
int bi;
bi = board_index( CurrentMachine.name );
if ( bi != NO_INDEX )
    return Boards[bi].channel_mode;

return CHANNEL_MODE_1;
}

/***********************************************************************
*                     CURRENT_VERSION_STRING                           *
***********************************************************************/
TCHAR * current_version_string()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].version_string.text();

return EmptyString;
}

/***********************************************************************
*                       CURRENT_UPLOAD_FILE_NAME                       *
***********************************************************************/
TCHAR * current_upload_file_name()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].upload_file_name.text();

return EmptyString;
}

/***********************************************************************
*                         CURRENT_VERSION                              *
***********************************************************************/
int current_version()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].version;

return 0;
}

/***********************************************************************
*                       CURRENT_SUB_VERSION                            *
***********************************************************************/
int current_sub_version()
{
if ( CurrentBoardIndex != NO_INDEX )
    return Boards[CurrentBoardIndex].sub_version;

return 0;
}

/***********************************************************************
*                             ADD_BOARD_DATA                           *
***********************************************************************/
static TCHAR * add_board_data( TCHAR * dest, BOARD_DATA x )
{
insalph( dest, (int32) x, BOARD_DATA_HEX_LEN, ZeroChar, HEX_RADIX );
dest += BOARD_DATA_HEX_LEN;

*dest = TabChar;
dest++;
return dest;
}

/***********************************************************************
*                               WIREBIT                                *
***********************************************************************/
unsigned wirebit( int wire_number )
{
unsigned x;

x = 0;
if ( wire_number > NO_WIRE )
    {
    x = 1;
    wire_number--;
    if ( wire_number > 0 )
        x <<= wire_number;
    }

return x;
}

/***********************************************************************
*                   SEND_MACHINE_STATE_EVENT_STRING                    *
*                                                                      *
*                         "M01\t00F1\t00FF"                            *
***********************************************************************/
static void send_machine_state_event_string( STRING_CLASS & machine, BOARD_DATA old_status, BOARD_DATA new_status )
{
TCHAR * buf;
TCHAR * cp;
int     slen;

slen = machine.len() + 1 + BOARD_DATA_HEX_LEN + 1 + BOARD_DATA_HEX_LEN + 1;
buf = maketext( slen );
if ( !buf )
    return;

cp = buf;
cp = copy_w_char( cp, machine.text(), TabChar );
cp = add_board_data( cp, old_status );
cp = add_board_data( cp, new_status );

/*
-----------------------------------
Replace the final comma with a null
----------------------------------- */
cp--;
*cp = NullChar;

MainWindow.post( WM_POKEMON, (WPARAM) NEW_MACH_STATE_INDEX, (LPARAM) buf );
}

/***********************************************************************
*                        UPDATE_MONITOR_STATUS                         *
***********************************************************************/
static void update_monitor_status( FTII_BOARD_ENTRY * board, TCHAR * s )
{
static BOOLEAN CycleStartIsVisible = FALSE;

HWND     w;
int      i;
BOARD_DATA old_status;

i = board_index( board );

if ( i != NO_INDEX )
    {
    if ( i < NofRows )
        set_text( MonitorStatusTbox[i].w, s );
        if ( i == CurrentBoardIndex )
            set_text( CurrentValuesMonitorStatusTb, s );

    old_status = board->statusbits;
    if ( CycleStartString == s )
        {
        w = AutoShotDisplayWindow.control( AUTO_SHOT_MACHINE_NUMBER_TBOX );
        board->m.name.set_text( w );
        AutoShotDisplayWindow.show();
        CycleStartIsVisible = TRUE;
        board->statusbits |= VIS_AUTO_SHOT;
        board->statusbits &= ~VIS_TIME_OUT;
        }
    else
        {
        if ( CycleStartIsVisible )
            {
            CycleStartIsVisible = FALSE;
            AutoShotDisplayWindow.hide();
            }

        if ( findstring(TimeoutString, s) )
            {
            board->statusbits &= ~VIS_AUTO_SHOT;
            board->statusbits |=  VIS_TIME_OUT;
            }
        else
            {
            board->statusbits &= ~(VIS_AUTO_SHOT | VIS_TIME_OUT);
            }
        }

    if ( old_status != board->statusbits )
        send_machine_state_event_string( board->m.name, old_status, board->statusbits );
    }
}

/***********************************************************************
*                            LOG_NETWORK_ERROR                         *
***********************************************************************/
void log_network_error( int error_number )
{

struct SOCKET_ERROR_ENTRY {
    TCHAR * name;
    int     number;
    };

static SOCKET_ERROR_ENTRY se[] = {
      { TEXT("WSABASEERR"),         10000 },
      { TEXT("WSAEINTR"),           10004 },
      { TEXT("WSAEBADF"),           10009 },
      { TEXT("WSAEACCES"),          10013 },
      { TEXT("WSAEFAULT"),          10014 },
      { TEXT("WSAEINVAL"),          10022 },
      { TEXT("WSAEMFILE"),          10024 },
      { TEXT("WSAEWOULDBLOCK"),     10035 },
      { TEXT("WSAEINPROGRESS"),     10036 },
      { TEXT("WSAEALREADY"),        10037 },
      { TEXT("WSAENOTSOCK"),        10038 },
      { TEXT("WSAEDESTADDRREQ"),    10039 },
      { TEXT("WSAEMSGSIZE"),        10040 },
      { TEXT("WSAEPROTOTYPE"),      10041 },
      { TEXT("WSAENOPROTOOPT"),     10042 },
      { TEXT("WSAEPROTONOSUPPORT"), 10043 },
      { TEXT("WSAESOCKTNOSUPPORT"), 10044 },
      { TEXT("WSAEOPNOTSUPP"),      10045 },
      { TEXT("WSAEPFNOSUPPORT"),    10046 },
      { TEXT("WSAEAFNOSUPPORT"),    10047 },
      { TEXT("WSAEADDRINUSE"),      10048 },
      { TEXT("WSAEADDRNOTAVAIL"),   10049 },
      { TEXT("WSAENETDOWN"),        10050 },
      { TEXT("WSAENETUNREACH"),     10051 },
      { TEXT("WSAENETRESET"),       10052 },
      { TEXT("WSAECONNABORTED"),    10053 },
      { TEXT("WSAECONNRESET"),      10054 },
      { TEXT("WSAENOBUFS"),         10055 },
      { TEXT("WSAEISCONN"),         10056 },
      { TEXT("WSAENOTCONN"),        10057 },
      { TEXT("WSAESHUTDOWN"),       10058 },
      { TEXT("WSAETOOMANYREFS"),    10059 },
      { TEXT("WSAETIMEDOUT"),       10060 },
      { TEXT("WSAECONNREFUSED"),    10061 },
      { TEXT("WSAELOOP"),           10062 },
      { TEXT("WSAENAMETOOLONG"),    10063 },
      { TEXT("WSAEHOSTDOWN"),       10064 },
      { TEXT("WSAEHOSTUNREACH"),    10065 },
      { TEXT("WSAENOTEMPTY"),       10066 },
      { TEXT("WSAEPROCLIM"),        10067 },
      { TEXT("WSAEUSERS"),          10068 },
      { TEXT("WSAEDQUOT"),          10069 },
      { TEXT("WSAESTALE"),          10070 },
      { TEXT("WSAEREMOTE"),         10071 },
      { TEXT("WSAEDISCON"),         10101 }
      };
int nof_errors = sizeof(se)/sizeof(SOCKET_ERROR_ENTRY);

TCHAR        space_equals_space[] = TEXT( " = " );
int          i;
STRING_CLASS s;
FILE_CLASS   f;
TIME_CLASS   t;

if ( !NetworkLogFileEnabled )
    return;

t.get_local_time();
s = t.text();
s += TEXT( " Socket Error " );

for ( i=0; i<nof_errors; i++ )
    {
    if ( se[i].number == error_number )
        {
        s += se[i].name;
        s += space_equals_space;
        break;
        }
    }

s += error_number;
f.open_for_append( NetworkLogFile );
f.writeline( s );
f.close();
}

/***********************************************************************
*                            LOG_NETWORK_ERROR                         *
***********************************************************************/
static void log_network_error_string( TCHAR * message )
{
STRING_CLASS s;
FILE_CLASS   f;
TIME_CLASS   t;

if ( !NetworkLogFileEnabled )
    return;

t.get_local_time();
s = t.text();
s += message;
f.open_for_append( NetworkLogFile );
f.writeline( s );
f.close();
}

/***********************************************************************
*                            LOG_NETWORK_ERROR                         *
***********************************************************************/
static void log_network_error_string( STRING_CLASS & message )
{
log_network_error_string( message.text() );
}

/***********************************************************************
*                    BOARD_EXTENDED_ANALOG_ARRAY                       *
***********************************************************************/
BOARD_EXTENDED_ANALOG_ARRAY::~BOARD_EXTENDED_ANALOG_ARRAY()
{
if ( array )
    {
    n = 0;
    delete[] array;
    array = 0;
    }
}

/***********************************************************************
*                    BOARD_EXTENDED_ANALOG_ARRAY                       *
*                              clear                                   *
***********************************************************************/
void BOARD_EXTENDED_ANALOG_ARRAY::clear()
{
if ( array )
    {
    n = 0;
    delete[] array;
    array = 0;
    }
}

/***********************************************************************
*                    BOARD_EXTENDED_ANALOG_ARRAY                       *
*                              init                                    *
***********************************************************************/
BOOLEAN BOARD_EXTENDED_ANALOG_ARRAY::init( int new_n )
{
int i;

if ( new_n != n )
    {
    clear();
    if ( new_n > 0 )
        {
        array = new BOARD_EXTENDED_ANALOG_ENTRY[new_n];
        if ( array )
            n = new_n;
        else
            return FALSE;
        }
    }
else
    {
    for ( i=0; i<n; i++ )
        {
        array[i].mask = 0;
        array[i].ae.parameter_number = NO_PARAMETER_NUMBER;
        }
    }

return TRUE;
}

/***********************************************************************
*                    BOARD_EXTENDED_ANALOG_ARRAY                       *
*                               []                                     *
***********************************************************************/
BOARD_EXTENDED_ANALOG_ENTRY & BOARD_EXTENDED_ANALOG_ARRAY::operator[]( int i )
{
static BOARD_EXTENDED_ANALOG_ENTRY empty;

if ( array && i>=0 && i< n )
    return array[i];

return empty;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
***********************************************************************/
FTII_BOARD_ENTRY::FTII_BOARD_ENTRY()
{
a.sin_addr.S_un.S_addr       = 0;
a.sin_family                 = AF_INET;
a.sin_port                   = 0;
alarm_bits                   = 0;
binarybuf                    = 0;
bits_currently_on            = 0;
bp                           = 0;
bytes_uploaded               = 0;
channel_mode                 = CHANNEL_MODE_1;
chars_left_in_buffer         = 0;
config_word                  = 0x3110FFFF;
connect_state          = NO_FT2_CONNECT_STATE;
control_file_date      = 0;
current_len            = 0;
current_packet_number  = 0;
current_type                 = ASCII_TYPE;
do_not_upload          = false;
do_not_upload_timeout  = 0;
down_state             = NO_DOWN_STATE;
down_timeout_seconds   = 0;
eos_bits_on            = 0;
ft3_is_connected             = false;
ft3_read_thread_is_running   = false;
ft3_send_event               = 0;
ft3_send_thread_is_running   = false;
has_alarm              = false;
has_warning            = false;
have_response                = false;
have_runlist                 = FALSE;
index                        = 0;
input_bits                   = 0;
is_ft3                       = true;
is_sending_op_data_packets   = false;
monitoring_was_stopped       = false;
ms_at_last_contact           = 0;
mysock                       = INVALID_SOCKET;
nak_count                    = 0;
need_to_check_for_shot_complete_timeout = false;
needs_impact_position_report = false;
number                       = NO_BOARD_NUMBER;
sample_type                  = 'P';
shot_number                  = 0;
start_op_data_after_upload   = false;
statusbits                   = 0;
sub_version                  = 0;
time_of_last_shot            = 0;
timer_frequency              = 16666667;
upload_copy_date             = 0;
upload_state                 = NO_UPLOAD_STATE;
uploading                    = false;
version                      = 0;
waiting_for_ack              = false;
}

/***********************************************************************
*                          ~FTII_BOARD_ENTRY                           *
***********************************************************************/
FTII_BOARD_ENTRY::~FTII_BOARD_ENTRY()
{
if ( binarybuf )
    {
    delete[] binarybuf;
    binarybuf = 0;
    }
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                           SET_CONNECT_STATE                          *
***********************************************************************/
void FTII_BOARD_ENTRY::set_connect_state( unsigned new_state )
{
int      i;
unsigned b;
unsigned old_state;

b = 0;
if ( connect_state != new_state )
    {
    old_state = connect_state;
    b = connect_state & UPLOAD_WAS_ABORTED_STATE;
    connect_state = new_state;
    if ( (new_state & CONNECTING_STATE) || (new_state & NOT_CONNECTED_STATE) )
        connect_state |= b;
    i = board_index( this );
    /*
    ------------------------------------------------------------------------------------
    If the last upload was aborted, tell the main proc so he can ask a human to reupload
    ------------------------------------------------------------------------------------ */
    if ( new_state & CONNECTED_STATE )
        {
        new_state |= b;
        if ( !(old_state & CONNECTED_STATE) )
            {
            /*
            ---------------------------------------------------------------------------
            If the board is reconnecting after an absense I need to restart the op data
            packets. If start_op_data_after_upload then I am resetting and don't need
            to do this.
            --------------------------------------------------------------------------- */
            if ( is_sending_op_data_packets && !start_op_data_after_upload )
                send_socket_message( SEND_OP_DATA_PACKETS, true );
            }
        }
    MainWindow.post( WM_NEW_FT2_STATE, (WPARAM) i, (LPARAM) new_state );
    }
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                           SEND_CONFIG_WORD                           *
***********************************************************************/
void FTII_BOARD_ENTRY::send_config_word()
{
const int max_slen = 16;
STRING_CLASS s;
TCHAR hex[MAX_HEX_LEN+1];

s.upsize( max_slen );
s = ConfigSetString;

ultohex( hex, config_word );
s += hex;
s += CrString;
send_socket_message( s.text(), true );
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                          GET_VERSION_STRING                          *
* This function retrieves the last saved version string from the       *
* ftii_editor_settings.ini file. It does not set the version and       *
* sub_version variables as these are used to tell if the board         *
* has sent a version string.                                           *
***********************************************************************/
BOOLEAN FTII_BOARD_ENTRY::get_version_string()
{
TCHAR      c;
NAME_CLASS s;
INI_CLASS  ini;

s.get_ft2_editor_settings_file_name( m.computer, m.name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(FlashVersionKey) )
    {
    s = ini.get_string();
    c = *s.text();
    if ( is_numeric(c) )
        {
        version_string.acquire( s.release() );
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                            FT3_DISCONNECT                            *
***********************************************************************/
bool FTII_BOARD_ENTRY::ft3_disconnect()
{
int status;

ft3_is_connected = false;

if ( mysock != INVALID_SOCKET )
    {
    status = closesocket( mysock );
    mysock = INVALID_SOCKET;
    if ( status == SOCKET_ERROR )
        return false;
    }

return true;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                             FT3_CONNECT                              *
***********************************************************************/
int FTII_BOARD_ENTRY::ft3_connect()
{
sockaddr * sa;
int        status;

/*
-------------
Create Socket
------------- */
if ( mysock == INVALID_SOCKET )
    {
    ft3_is_connected = false;
    mysock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( mysock == INVALID_SOCKET )
        {
        MessageBox( 0, TEXT("Socket"), TEXT("Create Failed"), MB_OK );
        return FT3_SOCKET_CREATION_ERROR;
        }
    }

/*
---------------------------
Connect Socket to ft3 board
--------------------------- */
if ( !ft3_is_connected )
    {
    sa = (sockaddr *) &a;
    status = connect( mysock, sa, (int) sizeof(sockaddr) );
    if ( status == SOCKET_ERROR )
        return FT3_SOCKET_CONNECT_ERROR;
    ft3_is_connected = true;
    }

return 0;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                           FT3_READ_THREAD                            *
***********************************************************************/
void ft3_read_thread( void * bp )
{
SOCKET_MESSAGE   * mp;
FTII_BOARD_ENTRY * myboard;
STRING_CLASS       s;

myboard = (FTII_BOARD_ENTRY *) bp;

mp = 0;
myboard->ft3_read_thread_is_running = true;

/*
 * FasTrak3 TCP/IP refactor
 * NB: Message-framing via type prefix
 */
int rv;
int rvlen;
unsigned char rvdata[FT3_RECV_BUFFER_LEN];
bool is_async;

while ( true )
    {
    mp = get_socket_message();

	rv = recv(myboard->mysock, (char *)rvdata, 1, 0);
	if (rv > 0) {
		is_async = rvdata[0] & FT3_ASYNC_TYPE_BIT ? true : false;
		rvdata[0] &= ~FT3_ASYNC_TYPE_BIT; // Unset message-framing bit.
	} else if (rv == SOCKET_ERROR) {
		goto RECV_ERR;
	} else {
		// TODO: Handle connection-closed (rv = 0) state.
		goto RECV_ERR;
	}

	if (!is_async) {
		// Response.
		rvlen = 1; // Metadata is first character of response.
		while (rvlen < FT3_RESPONSE_TYPE_LEN) {
			rv = recv(myboard->mysock, (char *)rvdata + rvlen, FT3_RESPONSE_TYPE_LEN - rvlen, 0);
			if (rv > 0) {
				rvlen += rv;
			} else if (rv == SOCKET_ERROR) {
				goto RECV_ERR;
			} else {
				// TODO: Handle connection-closed (rv = 0) state.
				goto RECV_ERR;
			}
		}

        if (rvdata[0] == FT3_ECHO_RESPONSE_INIT) {
            // FT3 echo-request / echo-response.
            if (!(myboard->connect_state & CONNECTED_STATE) &&
                !(myboard->connect_state & NOT_MONITORED_STATE)) {
                // Periodic connection state consistency enforcement.
                myboard->set_connect_state(CONNECTED_STATE);
            }

            if (!FT3_ECHO_RESPONSE_VERBOSE) {
                // Zero length to discard FT3 echo-request / echo-response
                // if verbose Boolean is false.
                rvlen = 0;
            }
        }
	} else {
		// Asynchronous event.
		rvlen = 1; // Metadata is first character of response.
		while (rvlen < FT3_ASYNC_TYPE_HEADER_LEN) {
			// Binary header.
			rv = recv(myboard->mysock, (char *)rvdata + rvlen, FT3_ASYNC_TYPE_HEADER_LEN - rvlen, 0);
			if (rv > 0) {
				rvlen += rv;
			} else if (rv == SOCKET_ERROR) {
				goto RECV_ERR;
			} else {
				// TODO: Handle connection-closed (rv = 0) state.
				goto RECV_ERR;
			}
		}

		UINT16 datalen = (UINT16)rvdata[10] | (UINT16)rvdata[11] << 8;
		if (datalen + FT3_ASYNC_TYPE_HEADER_LEN > FT3_RECV_BUFFER_LEN) {
			// WARNING Rx buffer insufficient size for async header and data.
			datalen = FT3_RECV_BUFFER_LEN - FT3_ASYNC_TYPE_HEADER_LEN;
		}

		rvlen = 0;
		while (rvlen < datalen) {
			// Binary data.
			rv = recv(myboard->mysock, (char *)rvdata + FT3_ASYNC_TYPE_HEADER_LEN + rvlen, datalen - rvlen, 0);
			if (rv > 0) {
				rvlen += rv;
			} else if (rv == SOCKET_ERROR) {
				goto RECV_ERR;
			} else {
				// TODO: Handle connection-closed (rv = 0) state.
				goto RECV_ERR;
			}
		}
		rvlen += FT3_ASYNC_TYPE_HEADER_LEN;
	}

	memcpy(mp->s, rvdata, rvlen);

RECV_ERR:
    if ( ShuttingDown )
        {
        delete mp;
        mp = 0;
        break;
        }

    if ( rv == SOCKET_ERROR || !myboard->ft3_is_connected )
        {
        if ( rv == SOCKET_ERROR )
            log_network_error( WSAGetLastError() );
        delete mp;
        mp = 0;
        break;
        }

    mp->a.sin_family           = AF_INET;
    mp->a.sin_port             = myboard->a.sin_port;
    mp->a.sin_addr.S_un.S_addr = myboard->a.sin_addr.S_un.S_addr;

    mp->len = rvlen; // FasTrak3 TCP/IP refactor
    minint( mp->len, MAX_MESSAGE_LEN );

    if ( ascii_strings_are_equal(ShutdownMessage, mp->s, ShutdownMessageLen) )
        {
        delete mp;
        mp = 0;
        break;
        }

    myboard->have_response = true;
    myboard->ms_at_last_contact = GetTickCount();

    if ( mp )
        {
        if ( mp->len <= 0 )
            {
            SocketMessageStore.push( mp );
            mp = 0;
            }
        else
            {
            NewData.push( mp );
            SetEvent( NewDataEvent );
            mp = 0;
            }
        }
    }

myboard->ft3_read_thread_is_running = false;
}

/***********************************************************************
*                           FT3_SEND_THREAD                            *
***********************************************************************/
void ft3_send_thread( void * bp )
{
const  DWORD CONNECT_TIMEOUT_MS =  3001;
const  DWORD PING_TIMEOUT_MS    = 20000;

HANDLE             h;
unsigned long      thread_handle;
SOCKET_MESSAGE   * mp;
FTII_BOARD_ENTRY * myboard;
int                status;
void             * tp;
DWORD              wait_ms;
STRING_CLASS       s;

mp                = 0;
wait_ms           = CONNECT_TIMEOUT_MS;

myboard = ( FTII_BOARD_ENTRY * ) bp;
myboard->ft3_send_thread_is_running = true;

if ( myboard->ft3_send_event == 0 )
    myboard->ft3_send_event = CreateEvent( NULL, FALSE, FALSE, NULL );

if ( myboard->ft3_send_event == 0 )
    MessageBox( 0, TEXT("ft3_send_event"), TEXT("Create Error"), MB_OK );

if ( !myboard->ft3_is_connected )
    {
    myboard->set_connect_state( CONNECTING_STATE );
    status = myboard->ft3_connect();
    if ( status == FT3_SOCKET_CONNECT_ERROR )
        {
        s = WSAGetLastError();
        MessageBox( 0, TEXT("ft3_connect_error"), s.text(), MB_OK );
        }
    if ( myboard->ft3_is_connected )
        {
        myboard->ms_at_last_contact = GetTickCount();
        myboard->set_connect_state ( CONNECTED_STATE );
        tp                = (void *) ( Boards + myboard->index );
        thread_handle     = _beginthread( ft3_read_thread, 0, tp );
        }
    else
        {
        myboard->set_connect_state( NOT_CONNECTED_STATE );
        }
    }

while ( true )
    {
    status = WaitForSingleObject( myboard->ft3_send_event, wait_ms );
    if ( status == WAIT_FAILED )
        break;

    ResetEvent( myboard->ft3_send_event );

    if ( !myboard->ft3_is_connected && !ShuttingDown )
        {
		unsigned t = GetTickCount();
		unsigned dt = t - myboard->ms_at_last_contact;

        /*
        ----------------
        Try to reconnect
        ---------------- */
        if ( dt > CONNECT_TIMEOUT_MS )
            {
            myboard->set_connect_state ( CONNECTING_STATE );
            myboard->ft3_connect();
            if ( myboard->ft3_is_connected )
                {
                myboard->ms_at_last_contact = GetTickCount();
				tp = (void *)(Boards + myboard->index);
				thread_handle = _beginthread(ft3_read_thread, 0, tp);
                myboard->set_connect_state ( CONNECTED_STATE );
                }
            else
                {
                myboard->set_connect_state( NOT_CONNECTED_STATE );
                }
            }
        }

    if ( myboard->ft3_is_connected )
        {
        /*
        ---------------------------------------------
        I can't upload while a shot is in progress
        See if it is time to cancel the do_not_upload
        ( this sould be done by the AT_HOME signal)
        --------------------------------------------- */
        if ( myboard->do_not_upload )
            {
		    unsigned t = GetTickCount();
            if ( myboard->do_not_upload_timeout > 0 && t > myboard->do_not_upload_timeout )
                {
                myboard->do_not_upload         = false;
                myboard->do_not_upload_timeout = 0;
                set_text( Dnu[myboard->index], EmptyString );
                }
            }

		if (myboard->f.count() == 0) {
			unsigned t = GetTickCount();
			unsigned dt = t - myboard->ms_at_last_contact;

			if (dt > PING_TIMEOUT_MS) {
				/*
				 * Send periodic echo-request to FT3 board as heartbeat.
				 * Ref. send_thread() ping implementation for FT2, which
				 * queries minimum stroke length variable. FT3 solution
				 * uses a dedicated echo-request/echo-response mechanism
				 * to interact with SOM Linux layer, i.e. command is not
				 * passed to FPGA performing A/D acquisition and control.
				 */
				myboard->ms_at_last_contact = t;
				myboard->send_socket_message(FT3_ECHO_REQUEST, true);
			}
		}

        while ( myboard->f.count() )
            {
            /*
            -----------------------------------------
            Point mp at the message owned by the fifo
            ----------------------------------------- */
            mp = (SOCKET_MESSAGE *) myboard->f.peek();

            if ( start_chars_are_equal(START_OF_UPLOAD, mp->s) )
                {
                /*
                ------------------------------------------------
                Don't start an upload if the do_not_upload is on
                ------------------------------------------------ */
                if ( myboard->do_not_upload )
                    {
                    mp = 0;
                    break;
                    }
                myboard->uploading      = true;
                myboard->bytes_uploaded = 0;
                mp = (SOCKET_MESSAGE *) myboard->f.pop();
                SocketMessageStore.push( mp );
                mp = 0;
                update_monitor_status( myboard, resource_string(BEGIN_UPLOAD_STRING) );
                }

            else if ( start_chars_are_equal(END_OF_UPLOAD, mp->s) )
                {
                myboard->uploading      = false;
                myboard->bytes_uploaded = 0;
                mp = (SOCKET_MESSAGE *) myboard->f.pop();
                SocketMessageStore.push( mp );
                mp = 0;
                update_monitor_status( myboard, resource_string(UPLOAD_COMPLETE_STRING) );
                /*
                --------------------------------------------------
                I turn off the realtime updates during upload so I
                need to turn them back on now.
                -------------------------------------------------- */
                if ( myboard->start_op_data_after_upload )
                    {
                    myboard->send_socket_message( SEND_OP_DATA_PACKETS, false );
                    myboard->start_op_data_after_upload = false;
                    }
                }

            if ( mp )
                {
                status = send( myboard->mysock, mp->s, mp->len, 0 );
                if ( status == SOCKET_ERROR )
                    {
                    myboard->ft3_disconnect();
                    mp = 0;
                    break;
                    }
                else
                    {
                    mp = (SOCKET_MESSAGE *) myboard->f.pop();
                    SocketMessageStore.push( mp );
                    mp = 0;
                    }
                }
            }

        if ( ShuttingDown )
            {
            h = myboard->ft3_send_event;
            myboard->ft3_send_event = 0;
            if ( h )
                CloseHandle( h );
            myboard->ft3_disconnect();
            myboard->set_connect_state( NOT_CONNECTED_STATE );
            }
        }

    if ( ShuttingDown )
        break;
    }

myboard->ft3_send_thread_is_running = false;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                          PUT_VERSION_STRING                          *
***********************************************************************/
void FTII_BOARD_ENTRY::put_version_string()
{
NAME_CLASS s;
INI_CLASS  ini;

s.get_ft2_editor_settings_file_name( m.computer, m.name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
ini.put_string( FlashVersionKey, version_string.text() );
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                            IS_MONITORING                             *
***********************************************************************/
bool FTII_BOARD_ENTRY::is_monitoring()
{
if ( m.monitor_flags & MA_MONITORING_ON )
    return true;

return false;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                           get_config_word                            *
*   This assumes that the number is in hex and begins with 'H'.        *
***********************************************************************/
void FTII_BOARD_ENTRY::get_config_word( TCHAR * sorc )
{
TCHAR * cp;

cp = findchar( HChar, sorc );
if ( cp )
    {
    cp++;
    config_word = (unsigned) hextoul( cp );
    }
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                        add_binary_to_shotdata                        *
***********************************************************************/
void FTII_BOARD_ENTRY::add_binary_to_shotdata()
{
int nof_samples;

switch ( current_type )
    {
    case SAMPLE_TYPE:
        /*
        ---------------------------------------------------
        The length is in bytes but I need number of samples
        --------------------------------------------------- */
        nof_samples = current_len / sizeof( FTII_SAMPLE );
        if ( sample_type == 'P' )
            {
            time_of_last_shot.get_local_time();
            shotdata.set_shot_time( time_of_last_shot.file_time() );
            shotdata.set_position_based_points( (FTII_SAMPLE *) binarybuf, nof_samples );
            }
        else
            {
            shotdata.set_time_based_points( (FTII_SAMPLE *) binarybuf, nof_samples );
            }
        binarybuf = 0;
        break;

    case PARAMETERS_TYPE:
        shotdata.set_parameters( (FTII_PARAMETERS *) binarybuf );
        binarybuf = 0;
        save_shot();
        break;
    }
}

/***********************************************************************
*                 CHECK_FOR_EXTENDED_CHANNEL_VALUE                     *
*              The sorc is a line like "O_CH12=xxxx"                   *
***********************************************************************/
void FTII_BOARD_ENTRY::check_for_extended_channel_value( TCHAR * sorc )
{
int i;
TCHAR * cp;

for ( i=0; i<extended_analog.n; i++ )
    {
    if ( extended_analog[i].waiting_for_value )
        {
        cp = sorc + 4;
        if ( extended_analog[i].channel == asctoint32(cp) )
            {
            cp = findchar( EqualsChar, cp );
            if ( cp )
                {
                cp++;
                extended_analog[i].ae.value = (unsigned short) hextoul(cp);
                }
            extended_analog[i].waiting_for_value = FALSE;
            }
        }
    }
}

/***********************************************************************
*                               BIT_STATE                              *
***********************************************************************/
static TCHAR * bit_state( unsigned x )
{
static TCHAR On[] = TEXT(" ON ");
static TCHAR Off[] = TEXT(" OFF");
if ( x )
    return On;
return Off;
}

/***********************************************************************
*                                MAKE_LINE                             *
***********************************************************************/
static void make_line( STRING_CLASS & s, unsigned x, TIME_CLASS & t )
{
s += bit_state( x );
s += TEXT( " at time: " );
s += t.hhmmss();
}

/***********************************************************************
*                           WRITE_ALS_BIT_CHANGES                      *
***********************************************************************/
static void write_als_bit_changes( unsigned new_bits, unsigned old_bits )
{
FILE_CLASS f;
TIME_CLASS t;
STRING_CLASS s;

if ( !AlsLogIsActive )
    return;

t.get_local_time();

if ( !f.open_for_append(AlsLogFile) )
    return;

if ( (new_bits & RETRACT_MASK) != (old_bits & RETRACT_MASK) )
    {
    s = TEXT( "Retract        INPUT" );
    make_line( s, (new_bits & RETRACT_MASK), t );
    f.writeline( s.text() );
    }
if ( (new_bits & START_SHOT_MASK) != (old_bits & START_SHOT_MASK) )
    {
    s = TEXT( "Start Shot     INPUT" );
    make_line( s, (new_bits & START_SHOT_MASK), t );
    f.writeline( s.text() );
    }
if ( (new_bits & FOLLOW_THROUGH_MASK) != (old_bits & FOLLOW_THROUGH_MASK) )
    {
    s = TEXT( "Follow Through INPUT" );
    make_line( s, (new_bits & FOLLOW_THROUGH_MASK), t );
    f.writeline( s.text() );
    }

if ( (new_bits & ACCUMULATOR_MASK) != (old_bits & ACCUMULATOR_MASK) )
    {
    s = TEXT( "Accumulator    INPUT" );
    make_line( s, (new_bits & ACCUMULATOR_MASK), t );
    f.writeline( s.text() );
    }

if ( (new_bits & SLOW_SHOT_MASK) != (old_bits & SLOW_SHOT_MASK) )
    {
    s = TEXT( "Slow Shot      INPUT" );
    make_line( s, (new_bits & SLOW_SHOT_MASK), t );
    f.writeline( s.text() );
    }

f.close();
}

/***********************************************************************
*                            WRITE_SENT_COMMAND                        *
***********************************************************************/
static void write_sent_command( int board_number, char * sorc )
{
FILE_CLASS f;
TIME_CLASS t;
STRING_CLASS s;
STRING_CLASS as;

if ( !as.get_ascii(sorc) )
    return;

as.strip_crlf();

t.get_local_time();

if ( !f.open_for_append(SentCommandsLogFile) )
    return;

s = board_number;
s += SpaceChar;
s += t.hhmmss();
s += SpaceChar;
s += as;

f.writeline( s.text() );
f.close();
}

/***********************************************************************
*                            CHECK_INPUTS                              *
***********************************************************************/
void FTII_BOARD_ENTRY::check_inputs( TCHAR * sorc )
{
static TCHAR getanalog[] = TEXT( "OAXX\r" );

STRING_CLASS s;
STRING_CLASS not_found_string;
TCHAR  * cp;
int      i;
BOOLEAN  need_message;
unsigned additional_ms;
unsigned mask;
unsigned u;

u = hextoul( sorc );

if ( m.multipart_runlist_settings.enable )
    {
    not_found_string = StringTable.find(TEXT("NO_MULTIPART_RUNLIST_PART") );
    m.multipart_runlist_settings.part_to_run( s, part.computer, part.machine, u );
    }
else if ( have_runlist )
    {
    not_found_string = StringTable.find( TEXT("NO_WARMUP_PART_STRING") );
    s = runlist.find_part( u );
    }

    if ( !s.isempty() && (s != part.name) )
        {
        if ( part_exists( part.computer, part.machine, s) )
            {
            set_current_part_name( part.computer, part.machine, s );

            s = part.machine;
            if ( !s.isempty() )
                MainWindow.post( WM_POKEMON, (WPARAM) MONITOR_SETUP_INDEX, (LPARAM) s.release() );
            }
        else
            {
        MessageBox( 0, not_found_string.text(), s.text(), MB_OK | MB_SYSTEMMODAL );
        }
    }

/*
--------------------------------------------------------------------------------------
If the NoUploadMask is set then I am using an input instead of the cycle start message
The bit must be clear in the saved input_bits before I disable uploads.
-------------------------------------------------------------------------------------- */
if ( m.suretrak_controlled && NoUploadWireMask && !do_not_upload )
    {
    if ( (u & NoUploadWireMask) && !(u & input_bits) )
        {
        do_not_upload_timeout = 0;
        do_not_upload         = true;
        if ( UploadStartShotTimeout )
            {
            do_not_upload_timeout = GetTickCount();
            additional_ms = UploadStartShotTimeout;
            additional_ms *= 1000;
            do_not_upload_timeout += additional_ms;
            }
        set_text( Dnu[index], AsterixString );
        }
    }

if ( do_not_upload && UploadRetractTimeout )
    {
    if ( u & RetractWireMask )
        {
        /*
        ------------------------------------------
        Wait 2 more seconds for the rod to retract
        before beginning an upload.
        ------------------------------------------ */
        do_not_upload_timeout = GetTickCount();
        do_not_upload_timeout += UploadRetractTimeout*1000;
        }
    }

/*
--------------------------------------------------------------------
See if any of the extended analog inputs have changed. If so, send a
OAy command to get the analog value for that channel..
-------------------------------------------------------------------- */
for ( i=0; i<extended_analog.n; i++ )
    {
    need_message = FALSE;
    if ( !extended_analog[i].waiting_for_value )
        {
        mask = extended_analog[i].mask;
        if ( extended_analog[i].trigger_type == TRIGGER_WHEN_GREATER )
            {
            if ( (mask & u) && !(mask & input_bits ) )
                need_message = TRUE;
            }
        else
            {
            if ( !(mask & u) && (mask & input_bits ) )
                need_message = TRUE;
            }
        }

    if ( need_message )
        {
        extended_analog[i].waiting_for_value = TRUE;
        cp = getanalog + 2;
        int32toasc( cp, (int32) extended_analog[i].channel, DECIMAL_RADIX );
        catstring( cp, CrString );
        send_socket_message( getanalog, true );
        }
    }

if ( (u & ALS_BITS) != (input_bits & ALS_BITS) )
    write_als_bit_changes( u, input_bits );

input_bits = u;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                         SEND_SOCKET_MESSAGE                          *
***********************************************************************/
bool FTII_BOARD_ENTRY::send_socket_message( TCHAR * sorc, bool need_to_set_event )
{
SOCKET_MESSAGE * mp;
union
{
char c;
TCHAR t;
};
int i;

mp = get_socket_message();

if ( is_ft3 )
    mp->a.sin_family = AF_INET;
else
    mp->a.sin_family           = PF_INET;

mp->a.sin_port             = a.sin_port;
mp->a.sin_addr.S_un.S_addr = a.sin_addr.S_un.S_addr;

if ( sorc )
    {
    /*
    ------------------------------------------------------------------------------
    Keep track of whether I'm sending the op data packets or not. If I'm uploading
    skip it because I will turn the packets back on when the upload is done.
    ------------------------------------------------------------------------------ */
    if ( start_chars_are_equal(OpDataPacketString, sorc) )
        {
        if ( *(sorc+4) == ZeroChar )
            is_sending_op_data_packets = false;
        else if ( *(sorc+4) == TwoChar )
            is_sending_op_data_packets = true;
        }

    /*
    ------------------------------------------
    Copy the tchar message to the char buffer.
    ------------------------------------------ */
    mp->len = lstrlen( sorc );
    if ( mp->len > MAX_MESSAGE_LEN )
        mp->len = MAX_MESSAGE_LEN;
    for ( i=0; i<mp->len; i++ )
        {
        t = sorc[i];
        mp->s[i] = c;
        };
    mp->s[mp->len] = '\0';
    }
else
    {
    /*
    -----------
    Null Packet
    ----------- */
    *(mp->s) = '\0';
    mp->len  = 0;
    }

f.push( mp );
if ( need_to_set_event )
    {
    if ( is_ft3 )
        SetEvent( ft3_send_event );
    else
    SetEvent( SendEvent );
    }

return true;
}

/***********************************************************************
*                     SET_EXTENDED_ANALOG_BITS                         *
************************************************************************/
void FTII_BOARD_ENTRY::set_extended_analog_bits()
{
int i;
int n;
int nof_params;
int wire_number;
int input_type;

/*
-----------------------------------------------------------------
Count the number of extended analogs that use inputs for triggers
----------------------------------------------------------------- */
n = 0;
nof_params = param.count();
for ( i=0; i<nof_params; i++ )
    {
    if ( param.parameter[i].input.type == FT_EXTENDED_ANALOG_INPUT )
        {
        if ( ftanalog.array[i].ind_value.int_value() > 0 )
            n++;
        }
    }

if ( !n )
    return;

extended_analog.init( n );
n = 0;
for ( i=0; i<nof_params; i++ )
    {
    input_type = param.parameter[i].input.type;

    if ( input_type == FT_EXTENDED_ANALOG_INPUT )
        {
        wire_number = ftanalog.array[i].ind_value.int_value();
        if ( wire_number > 0 )
            {
            extended_analog[n].mask = wirebit( wire_number );
            extended_analog[n].ae.parameter_number = i+1;
            extended_analog[n].trigger_type = (unsigned) ftanalog.array[i].ind_var_type;
            extended_analog[n].channel      = (int)      ftanalog.array[i].channel;
            n++;
            }
        }
    }
}

/***********************************************************************
*          FTII_BOARD_ENTRY::GET_PARAMETER_ALARM_BITS                  *
***********************************************************************/
unsigned FTII_BOARD_ENTRY::get_parameter_alarm_bits()
{
int i;
int j;
unsigned bits;
PARAMETER_ENTRY * p;

bits = 0;
for ( i=0; i<MAX_PARMS; i++ )
    {
    p = param.parameter + i;
    if ( p->input.type != NO_PARAMETER_TYPE )
        {
        if ( p->has_warning_limits() )
            {
            for ( j=WARNING_MIN; j<=WARNING_MAX; j++ )
                {
                if ( p->limits[j].wire_number != NO_WIRE )
                    bits |= wirebit( p->limits[j].wire_number );
                }
            }
        if ( p->has_alarm_limits() )
            {
            for ( j=ALARM_MIN; j<=ALARM_MAX; j++ )
                {
                if ( p->limits[j].wire_number != NO_WIRE )
                    bits |= wirebit( p->limits[j].wire_number );
                }
            }
        }
    }

return bits;
}

/***********************************************************************
*                   FTII_BOARD_ENTRY::GET_ALARM_BITS                   *
***********************************************************************/
void FTII_BOARD_ENTRY::get_alarm_bits()
{
alarm_bits = get_parameter_alarm_bits();

if ( part.thirty_second_timeout_wire != NO_WIRE )
    alarm_bits |= wirebit( part.thirty_second_timeout_wire );

if ( part.st2_program_abort_wire != NO_WIRE )
    alarm_bits |= wirebit( part.st2_program_abort_wire );

if ( part.shot_complete_wire != NO_WIRE )
    alarm_bits |= wirebit( part.shot_complete_wire );

if ( part.shot_save_complete_wire != NO_WIRE )
    alarm_bits |= wirebit( part.shot_save_complete_wire );

if ( part.warning_shot_wire != NO_WIRE )
    alarm_bits |= wirebit( part.warning_shot_wire );

if ( part.alarm_shot_wire != NO_WIRE )
    alarm_bits |= wirebit( part.alarm_shot_wire );

if ( part.good_shot_wire != NO_WIRE )
    alarm_bits |= wirebit( part.good_shot_wire );
}

/***********************************************************************
*                        CLEAR_ALL_ALARM_BITS                          *
*  This searches for all the alarm wires I am controlling and turns    *
*  them off.                                                           *
************************************************************************/
void FTII_BOARD_ENTRY::clear_all_alarm_bits()
{
send_bits( alarm_bits, false );
bits_currently_on &= ~alarm_bits;
}

/***********************************************************************
*                              SEND_BITS                               *
* I was going to change (26-Sep-05) so that I send a separate          *
* message for each bit. I've commented out the changes as I decided    *
* that change was unneccessary but I may change my mind.               *
*                                                                      *
* Note: this does not affect the bits_currenly_on, as this is most     *
* often used to send that value.                                       *
************************************************************************/
void FTII_BOARD_ENTRY::send_bits( unsigned bits, bool set_bits )
{
static TCHAR OnString[]  = TEXT("OO1=" );
static TCHAR OffString[] = TEXT("OO1=~" );
TCHAR s[30];
TCHAR * cp;

if ( set_bits )
    cp = copystring( s, OnString );
else
    cp = copystring( s, OffString );

ultohex( cp, (unsigned long) bits );

if ( NeedErrorLog )
    push_log_message( s );

catstring( cp, CrString );
send_socket_message( s, true );
}

/***********************************************************************
*                             CLEARWIRE                                *
***********************************************************************/
void FTII_BOARD_ENTRY::clearwire( int wire_to_clear )
{
unsigned u;
u = wirebit( wire_to_clear );
send_bits( u, false  );
bits_currently_on &= ~u;
}

/***********************************************************************
*                              SETWIRE                                 *
***********************************************************************/
void FTII_BOARD_ENTRY::setwire( int wire_to_set )
{
unsigned u;
u = wirebit( wire_to_set );
send_bits( u, true  );
bits_currently_on |= u;
}

/***********************************************************************
*                      SAVE_ALARM_SUMMARY_COUNTS                       *
***********************************************************************/
static void save_alarm_summary_counts( FTII_BOARD_ENTRY * board, double * dp )
{
ALARM_SUMMARY_CLASS as;
PARAMETER_CLASS   * p;
SYSTEMTIME          shift_date;
short               shift_number;
SYSTEMTIME          st;

p = &board->param;
if ( !p )
    return;

st = board->time_of_last_shot.system_time();
get_shift_number_and_date( &shift_number, &shift_date, st );

as.set_part( board->part.computer, board->part.machine, board->part.name );
as.set_counts( dp, p );
as.save( shift_date, (int32) shift_number );
}

/***********************************************************************
*                           CHECK_ONE_ALARM                            *
***********************************************************************/
static void check_one_alarm( bool & is_alarm, unsigned & setbits, float value, float low, short low_wire, float high, float high_wire )
{
/*
----------------------------------------------------
If high and low are the same I should not check this
---------------------------------------------------- */
if ( not_float_zero(high-low) )
    {
    if ( value < low )
        {
        if ( not_float_zero(low-value) )
            {
            is_alarm = true;
            if ( low_wire != NO_WIRE )
                setbits |= wirebit(low_wire);
            }
        }
    else if ( value > high )
        {
        if ( not_float_zero(value-high) )
            {
            is_alarm = true;
            if ( high_wire != NO_WIRE )
                setbits |= wirebit(high_wire);
            }
        }
    }
}

/***********************************************************************
*                             CHECK_ALARMS                             *
* 01-29-16 I moved the call to the ALARM_SUMMARY_CLASS out of this     *
* routine because I need to be able to set the alarms twice now that I *
* have external parameters that can set alarms.                        *
***********************************************************************/
static void check_alarms( FTII_BOARD_ENTRY * board, double * dp )
{
int        i;
float      parameter_value;
unsigned   clearbits;
unsigned   setbits;
bool       this_is_a_good_shot;
bool       i_need_to_wait_for_external_parameters;
PARAMETER_CLASS   * p;
PARAMETER_ENTRY   * pe;

/*
--------------------------------------------
Do nothing if alarms are not being monitored
-------------------------------------------- */
if ( !board->is_monitoring() )
    return;

if ( !(board->m.monitor_flags & MA_ALARM_MONITORING_ON) )
    return;

p = &board->param;

if ( !p )
    return;

board->has_alarm   = false;
board->has_warning = false;

setbits = 0;

for ( i=0; i<MAX_PARMS; i++ )
    {
    pe = p->parameter + i;
    /*
    -----------------------------------------------
    External parameters are handled at a later time
    ----------------------------------------------- */
    if ( pe->input.type != NO_PARAMETER_TYPE && pe->input.type != FT_STATIC_TEXT_PARAMETER && pe->input.type != FT_EXTERNAL_PARAMETER )
        {
        parameter_value = (float) dp[i];
        check_one_alarm( board->has_alarm,   setbits, parameter_value, pe->limits[ALARM_MIN].value,   pe->limits[ALARM_MIN].wire_number,   pe->limits[ALARM_MAX].value,   pe->limits[ALARM_MAX].wire_number );
        check_one_alarm( board->has_warning, setbits, parameter_value, pe->limits[WARNING_MIN].value, pe->limits[WARNING_MIN].wire_number, pe->limits[WARNING_MAX].value, pe->limits[WARNING_MAX].wire_number );
        }
    }

/*
-------------------------------------------------------------------
Save the alarm bits that I turn on so the external parameters alarm
handler (following function) won't turn them off
------------------------------------------------------------------- */
board->eos_bits_on = setbits;

if ( board->part.warning_shot_wire != NO_WIRE )
    {
    if ( board->has_warning )
        setbits |= wirebit(board->part.warning_shot_wire);
    }

if ( board->part.alarm_shot_wire != NO_WIRE )
    {
    if ( board->has_alarm )
        setbits |= wirebit(board->part.alarm_shot_wire);
    }


i_need_to_wait_for_external_parameters = false;
if ( board->external_parameter_settings.enable && board->external_parameter_settings.available_at_eos )
    {
    for ( i=0; i<MAX_PARMS; i++ )
        {
        pe = p->parameter + i;
        /*
        -----------------------------------------------
        External parameters are handled at a later time
        ----------------------------------------------- */
        if ( pe->input.type == FT_EXTERNAL_PARAMETER )
            {
            if ( not_float_zero(pe->limits[ALARM_MAX].value - pe->limits[ALARM_MIN].value) )
                {
                i_need_to_wait_for_external_parameters = true;
                break;
                }
            if ( not_float_zero(pe->limits[WARNING_MAX].value - pe->limits[WARNING_MIN].value) )
                {
                i_need_to_wait_for_external_parameters = true;
                break;
                }
            }
        }
    }

/*
----------------------------------------------------------------------
If there are external parameters that have alarms I need to wait until
I get the external parameter file before setting the good shot wire.
---------------------------------------------------------------------- */
if ( !i_need_to_wait_for_external_parameters )
    {
    if ( board->part.good_shot_wire != NO_WIRE )
        {
        this_is_a_good_shot = true;
        if ( board->has_alarm )
            this_is_a_good_shot = false;
        if ( board->part.good_shot_requires_no_warnings && board->has_warning )
            this_is_a_good_shot = false;
        if ( this_is_a_good_shot )
            setbits |= wirebit( board->part.good_shot_wire );
        }
    save_alarm_summary_counts( board, dp );
    }

board->bits_currently_on &= ~board->alarm_bits;
board->bits_currently_on |= setbits;
if ( board->bits_currently_on )
    board->send_bits( board->bits_currently_on, true );

clearbits = board->alarm_bits ^ setbits;
if ( clearbits )
    {
    board->send_bits( clearbits, false );
    board->alarm_bits = setbits;
    }
}

/***********************************************************************
*                        CHECK_EXTERNAL_ALARMS                         *
* This is called by ft2 either when the external parameter file has    *
* been received or when the cycle start has been received (signaling   *
* that the external parameters file is awol.                           *
* Ft2 has filled in the parameter values in dp.                        *
***********************************************************************/
void check_external_alarms( STRING_CLASS & ma, double * dp )
{
FTII_BOARD_ENTRY * board;
int                i;
float              parameter_value;
unsigned           mybits;
unsigned           setbits;
bool               this_is_a_good_shot;
unsigned           u;

PARAMETER_CLASS   * p;
PARAMETER_ENTRY   * pe;

i = board_index( ma );
if ( i == NO_INDEX )
    return;

board = Boards + i;

/*
--------------------------------------------
Do nothing if alarms are not being monitored
-------------------------------------------- */
if ( !board->is_monitoring() )
    return;

if ( !(board->m.monitor_flags & MA_ALARM_MONITORING_ON) )
    return;

p = &board->param;

if ( !p )
    return;

mybits  = 0;
setbits = 0;

for ( i=0; i<MAX_PARMS; i++ )
    {
    pe = p->parameter + i;

    if ( pe->input.type == FT_EXTERNAL_PARAMETER )
        {
        mybits |= wirebit(pe->limits[ALARM_MAX].wire_number);
        mybits |= wirebit(pe->limits[WARNING_MAX].wire_number);
        parameter_value = (float) dp[i];
        check_one_alarm( board->has_alarm,   setbits, parameter_value, pe->limits[ALARM_MIN].value,   pe->limits[ALARM_MIN].wire_number,   pe->limits[ALARM_MAX].value,   pe->limits[ALARM_MAX].wire_number );
        check_one_alarm( board->has_warning, setbits, parameter_value, pe->limits[WARNING_MIN].value, pe->limits[WARNING_MIN].wire_number, pe->limits[WARNING_MAX].value, pe->limits[WARNING_MAX].wire_number );
        }
    }

if ( board->part.warning_shot_wire != NO_WIRE )
    {
    if ( board->has_warning )
        setbits |= wirebit(board->part.warning_shot_wire);
    }

if ( board->part.alarm_shot_wire != NO_WIRE )
    {
    if ( board->has_alarm )
        setbits |= wirebit(board->part.alarm_shot_wire);
    }

if ( board->part.good_shot_wire != NO_WIRE )
    {
    this_is_a_good_shot = true;
    if ( board->has_alarm )
        this_is_a_good_shot = false;
    if ( board->part.good_shot_requires_no_warnings && board->has_warning )
        this_is_a_good_shot = false;
    if ( this_is_a_good_shot )
        setbits |= wirebit(board->part.good_shot_wire);
    }

u = setbits & ~board->bits_currently_on;
if ( u )
    {
    board->send_bits( u, true );
    board->bits_currently_on |= u;
    }

/*
------------------------------------
Get my bits that I am not turning on
------------------------------------ */
u = mybits & ~setbits;

/*
--------------------------------
Remove the bits turned on at eos
-------------------------------- */
u &= ~board->eos_bits_on;

/*
-----------------------------------------------------------------------------
Turn off any bits that I did not turn on this shot and that no one else uses.
----------------------------------------------------------------------------- */
if ( u )
    {
    board->send_bits( u, false );
    board->bits_currently_on &= ~u;
    }
}

/***********************************************************************
*                           CURRENT_OPERATOR                           *
***********************************************************************/
TCHAR * current_operator( STRING_CLASS & computer, STRING_CLASS & machine )
{
static TCHAR opnumber[OPERATOR_NUMBER_LEN+1];

NAME_CLASS     s;
SETPOINT_CLASS sp;

*opnumber = NullChar;

s.get_machset_csvname( computer, machine );
if ( sp.get(s) )
    {
    if ( sp.get_one_value(s, s, MACHSET_OPERATOR_NUMBER_INDEX) )
        copystring( opnumber, s.text(), OPERATOR_NUMBER_LEN );
    }

return opnumber;
}

/***********************************************************************
*                        ISSUE_DOWNTIME_EVENT                          *
***********************************************************************/
void issue_downtime_event( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat )
{
TCHAR * cp;
TCHAR * worker;

worker = current_operator( computer, machine );

cp = make_downtime_event_string( machine, part, st, cat, subcat, worker );
if ( cp )
    MainWindow.post( WM_POKEMON, (WPARAM) DOWN_DATA_INDEX, (LPARAM) cp );
}

/***********************************************************************
*                        APPEND_DOWNTIME_RECORD                        *
***********************************************************************/
BOOLEAN append_downtime_record( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat )
{
NAME_CLASS n;
DB_TABLE   t;
TCHAR    * worker;
BOOLEAN    status;

status = FALSE;

worker = current_operator( computer, machine );

n.get_machine_results_dir_file_name( computer, machine, DOWNTIME_DB );

if ( t.open(n, DOWNTIME_RECLEN, PWL) )
    {
    make_downtime_record( t, st, cat, subcat, worker, part );
    t.rec_append();
    t.close();
    status = TRUE;
    }

issue_downtime_event( computer, machine, part, st, cat, subcat );

return status;
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                              SAVE_SHOT                               *
***********************************************************************/
void FTII_BOARD_ENTRY::save_shot()
{
double           * dp;
FTII_FILE_DATA     filedata;
FTII_PROFILE       fp;
SYSTEMTIME         st;
PART_NAME_ENTRY    pn;
DOUBLE_ARRAY_CLASS pa;
DSTAT_CLASS        d;
int                i;
int                j;
int                n;
unsigned           u;
LIMIT_SWITCH_CLASS limits;

/*
--------------------------------------------------------------------------------------
If either the position data or time data are missing I must have lost some packets and
can't save the file.
-------------------------------------------------------------------------------------- */
if ( !shotdata.position_sample || !shotdata.time_sample )
    return;

pn.set( m.computer, m.name, m.current_part );

update_monitor_status( this, resource_string(SAVING_SHOT_STRING) );
FileTimeToSystemTime( &shotdata.shot_time, &st );
filedata = shotdata;
if ( extended_analog.n > 0 )
    {
    filedata.set_nof_extended_analogs( extended_analog.n );
    if ( filedata.extended_analog )
        {
        for ( i=0; i<extended_analog.n; i++ )
            {
            filedata.extended_analog[i] = extended_analog[i].ae;
            extended_analog[i].ae.value = 0;   /* Zero the value so it won't show up next time. */
            }
        }
    }
/*
-------------------------------
Copy any static text parameters
------------------------------- */
n = 0;
for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( param.parameter[i].input.type == FT_STATIC_TEXT_PARAMETER )
        n++;
    }
if ( n > 0 )
    {
    j = filedata.nof_text_parameters;
    n += filedata.nof_text_parameters;
    filedata.resize_text_parameters( n, TRUE );
    for ( i=0; i<MAX_PARMS; i++ )
        {
        if ( j >= filedata.nof_text_parameters )
            break;
        if ( param.parameter[i].input.type == FT_STATIC_TEXT_PARAMETER )
            {
            filedata.text_parameter[j].parameter_number = i+1;
            filedata.text_parameter[j].value = param.parameter[i].static_text.value;
            j++;
            }
        }
    }
fp = filedata;
dp = calculate_ftii_parameters( fp, m, part, param, ftanalog, ftcalc, limits );
if ( dp )
    {
    check_alarms( this, dp );
    pa.init( MAX_PARMS, dp );
    dp = 0;
    /*
    -----------------------------------------------
    This will transfer the data from filedata.
    At this time there are no external parameters.
    ----------------------------------------------- */
    add_to_shotsave( has_alarm, has_warning, pn, filedata, pa );
    SetEvent( ShotSaveEvent );
    }
shotdata.clear();

if ( down_state != MACH_UP_STATE )
    {
    d.set_cat( DOWNCAT_SYSTEM_CAT );
    d.set_subcat( DOWNCAT_UP_SUBCAT );
    d.set_time( st );
    d.put( part.computer, part.machine );
    down_state = d.down_state();

    /*
    -----------------------------------------------------------
    Only write to the downtime file if the cycle timeout is set
    ----------------------------------------------------------- */
    if ( down_timeout_seconds > 0 )
        append_downtime_record( part.computer, part.machine, part.name, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT );
    else
        issue_downtime_event( part.computer, part.machine, part.name, st, DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT );

    if ( down_timeout_seconds > 0 )
        {
        /*
        -------------------------------------------------
        This was a good shot, turn on the monitoring wire
        ------------------------------------------------- */
        if ( MonitorWire != NO_WIRE && ClearMonitorWireOnDown )
            {
            u = wirebit( MonitorWire );
            if ( !(bits_currently_on & u) )
                setwire( MonitorWire );
            }

        if ( DownTimeAlarmWire != NO_WIRE )
            {
            u = wirebit( DownTimeAlarmWire );
            if ( bits_currently_on & u )
                clearwire( DownTimeAlarmWire );
            }
        }
    }

update_monitor_status( this, resource_string(READY_STRING) );
if ( part.shot_complete_wire != NO_WIRE )
    {
    setwire( part.shot_complete_wire );
    need_to_check_for_shot_complete_timeout = true;
    }
}

/***********************************************************************
*                      TRUNCATE_CONTROL_FILE_LINE                      *
*             Don't send comments or spaces before comments            *
***********************************************************************/
static void truncate_control_file_line( TCHAR * line )
{
TCHAR * pp;

pp = findchar( PoundChar, line );
if ( pp )
    {
    while ( pp > line )
        {
        pp--;
        if ( *pp != SpaceChar && *pp != TabChar )
            {
            pp++;
            break;
            }
        }
    *pp = NullChar;
    }
}

/***********************************************************************
*                           update_config_word                         *
*   This assumes that the number is in hex and begins with 'H'.        *
***********************************************************************/
static void update_config_word( STRING_CLASS & s, unsigned long new_bits )
{
TCHAR buf[20];
TCHAR  * cp;
unsigned long x;

cp = s.find( HChar );
if ( cp )
    {
    cp++;
    copystring( buf, cp, 8 );
    x = hextoul( buf );
    x |= new_bits;
    ultohex( buf,  x );
    rjust( buf, 8, ZeroChar );
    *cp = NullChar;
    s += buf;
    }
}

/***********************************************************************
*                           update_config_word                         *
*   This assumes that the number is in hex and begins with 'H'.        *
*   The mask variable will be anded with the config word to mask of    *
*   any bits that should be zero. Thus, if you want to put a 5 in the  *
*   first 4 bits of the config word newbits would equal 5 and mask     *
*   would be 0xFFFFFFF0.                                               *
***********************************************************************/
static void update_config_word( STRING_CLASS & s, unsigned long new_bits, unsigned long mask )
{
TCHAR buf[20];
TCHAR  * cp;
unsigned long x;

cp = s.find( HChar );
if ( cp )
    {
    cp++;
    copystring( buf, cp, 8 );
    x = hextoul( buf );
    x &= mask;
    x |= new_bits;
    ultohex( buf,  x );
    rjust( buf, 8, ZeroChar );
    *cp = NullChar;
    s += buf;
    }
}

/***********************************************************************
*                      UPDATE_PC_DIGITAL_POT_GAIN                      *
* The pressure control digital pot gain goes in the low byte of V431.  *
***********************************************************************/
static void update_pc_digital_pot_gain( STRING_CLASS & s, PRESSURE_CONTROL_CLASS & pc )
{
static const unsigned long mask     = 0xFFFFFF00;
static const unsigned long new_mask = 0x000000FF;
TCHAR buf[20];
TCHAR  * cp;
unsigned long x;
unsigned long new_bits;

cp = s.find( HChar );
if ( cp )
    {
    cp++;
    copystring( buf, cp, 8 );
    x = hextoul( buf );
    x &= mask;

    new_bits = (unsigned long) pc.digital_pot_gain.uint_value();
    new_bits &= new_mask;

    x |= new_bits;
    ultohex( buf,  x );
    rjust( buf, 8, ZeroChar );
    *cp = NullChar;
    s += buf;
    }
}

/***********************************************************************
*                           FTII_BOARD_ENTRY                           *
*                          UPLOAD_CONTROL_FILE                         *
***********************************************************************/
bool FTII_BOARD_ENTRY::upload_control_file()
{
const unsigned BIT_0_MASK  = 1;
const unsigned BIT_16_MASK = 0x10000;
const unsigned ZERO_QUADRATURE_DIVISOR_MASK = 0xF0FFFFF0;
const unsigned LOWER_NIBBLE_MASK            = 0x0000000F;
const unsigned UPPER_NIBBLE_MASK            = 0x000000F0;
const unsigned DIGITAL_SERVO_AMP_AXIS_MASK  = 0xFFFFFCFF;
const unsigned DIGITAL_SERVO_AMP_W_ONE_AXIS = 0x00000100;
const unsigned DIGITAL_SERVO_AMP_W_TWO_AXES = 0x00000200;
static wchar_t UnicodeIntroducer = L'\xFEFF';

TCHAR * cp;
NAME_CLASS      s;
FILE_CLASS      f;
FTII_VAR_ARRAY  globals;
FTII_VAR_ARRAY  partvars;
FTII_VAR_ARRAY  machvars;
TEXT_LIST_CLASS t;
UPLOAD_COPY_CLASS uc;
unsigned        bits;
unsigned        x;
unsigned        quadrature_divisor_bits;
int             i;
bool            first_line;
bool            is_new;
bool            is_retctl;
bool            sent_something;
TCHAR           hex[MAX_HEX_LEN+1];

quadrature_divisor_bits = 1;
if ( m.quadrature_divisor > 0 && m.quadrature_divisor < 256 )
    {
    x = (unsigned) m.quadrature_divisor;
    quadrature_divisor_bits = x;
    quadrature_divisor_bits &= UPPER_NIBBLE_MASK;
    quadrature_divisor_bits <<= 20;
    x &= LOWER_NIBBLE_MASK;
    quadrature_divisor_bits |= x;
    }

s.get_ft2_global_settings_file_name( part.computer, part.machine );
globals.get( s );

/*
-------------------------------
Read the park step and alt step
------------------------------- */
s.get_ft2_ctrl_prog_steps_file_name( part.computer, part.machine );
if ( f.open_for_read(s.text()) )
    {
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;
        if ( *cp == PeriodChar )
            t.append( cp );
        }
    f.close();
    }

/*
-------------------------------------
Read the valve setting after jog shot
------------------------------------- */
s.get_machine_ctrl_prog_steps_file_name( part.computer, part.machine );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
        {
        while ( TRUE )
            {
            cp = f.readline();
            if ( !cp )
                break;
            if ( *cp == PeriodChar )
                t.append( cp );
            }
        f.close();
        }
    }

s.get_ft2_part_settings_file_name( part.computer, part.machine, part.name );
partvars.get( s );

is_retctl = false;
if ( partvars.find(PC_RETCTL_PRES_VN) )
    {
    if ( partvars.uval() != 0 )
        is_retctl = true;
    }

s.get_ft2_machine_settings_file_name( part.computer, part.machine );
if ( file_exists(s.text()) )
    {
    machvars.get( s );
    /*
    -------------------------------------------------------
    Make sure there is no V314 in the machine variable list
    ------------------------------------------------------- */
    if ( machvars.find(CONFIG_WORD_2_VN) )
        machvars.remove();
    }
else if ( machvars.count() > 0 )
    {
    machvars.empty();
    }
x = m.autoshot_timeout_seconds;
if ( x > 0 )
    {
    x *= 1000;
    i = machvars.count();
    machvars.upsize( i+1 );
    machvars[i].set( CS_TIMEOUT_MS_VN, x );
    }

upload_state = UPLOAD_ALL_VARIABLES;
x = uc.load( part.computer, part.machine, part.name );
if ( x != NO_FILE_DATE )
    {
    if ( x == upload_copy_date )
        upload_state = UPLOAD_NEW_VARIABLES;
    }
sent_something = false;

/*
-------------------------------------------------------------
Save the file name so I can display it on the realtime screen
------------------------------------------------------------- */
upload_file_name = ft2_control_program_name( version, m );

s.get_exe_dir_file_name( part.computer, upload_file_name.text() );

x = encoded_file_date( s );
if ( control_file_date != x )
    {
    control_file_date = x;  /* Save so I can send this to the board as V447 */
    upload_state = UPLOAD_ALL;
    }

if ( f.open_for_read(s.text()) )
    {
    send_socket_message( START_OF_UPLOAD, false );
    first_line = true;
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;

        #ifdef UNICODE
        if ( first_line )
            {
            if ( *cp == UnicodeIntroducer )
                cp++;
            first_line = false;
            }
        #endif

        truncate_control_file_line( cp );

        if ( *cp != NullChar && *cp != PoundChar )
            {
            /*
            ---------------------------------------------------------
            If this is a global or part variable, use the local value
            --------------------------------------------------------- */
            if ( globals.find(cp) )
                {
                globals.makeline( s );
                }
            else if ( partvars.find(cp) )
                {
                partvars.makeline( s );
                }
            else if ( machvars.find(cp) )
                {
                machvars.makeline( s );
                }
            else
                {
                s = cp;

                /*
                -------------------------------------------------------------
                Check to see if this is a program line that I want to replace
                ------------------------------------------------------------- */
                if ( *cp == PeriodChar && *(cp+1) >= ZeroChar && *(cp+1) <= NineChar )
                    {
                    t.rewind();
                    while ( t.next() )
                        {
                        if ( strings_are_equal( cp, t.text(), CTRL_PROG_NUMBER_LEN) )
                            {
                            s = t.text();
                            break;
                            }
                        }
                    }
                }

            /*
            -----------------------------------
            If this is the config word, save it
            ----------------------------------- */
            if ( s.contains(ConfigSetString) )
                {
                get_config_word( s.text() );
                if ( m.ls_off_at_eos() )
                    {
                    x = LS_OFF_AT_EOS_BIT;
                    update_config_word( s, x );
                    }
                else
                    {
                    x = ~LS_OFF_AT_EOS_BIT;
                    update_config_word( s, 0, x );
                    }
                if ( part.zero_speed_output_enable )
                    {
                    x = ZERO_SPEED_OUTPUT_ENABLE_BIT;
                    update_config_word( s, x );
                    }
                else
                    {
                    x = ~ZERO_SPEED_OUTPUT_ENABLE_BIT;
                    update_config_word( s, 0, x );
                    }
                /*
                ---------------------------------------------------
                Set the sensor bits in the config word 1=5v, 0=2.5v
                --------------------------------------------------- */
                bits = 0;
                x = 1;
                for ( i=0; i<MAX_FT2_CHANNELS; i++ )
                    {
                    if ( part.analog_sensor[i].max_volts_level != TWO_POINT_FIVE_MAX_VOLTS )
                        bits |= x;
                    x <<= 1;
                    }

                update_config_word( s, bits, (unsigned)0xFFFF0000 );
                }
            else if ( start_chars_are_equal(ConfigWord2SetString, s.text()) )
                {
                update_config_word( s, quadrature_divisor_bits, ZERO_QUADRATURE_DIVISOR_MASK );
                /*
                ----------------------------------------------------------------
                The following sets bits 8 and 9 to indicate the number of axes.
                This is for rev E external interface and digital servo amp only.
                I don't know if there is a
                ---------------------------------------------------------------- */
                if ( m.suretrak_controlled && (m.monitor_flags & MA_HAS_DIGITAL_SERVO) )
                    {
                    x = DIGITAL_SERVO_AMP_W_ONE_AXIS;
                    if ( pc.is_enabled )
                        x = DIGITAL_SERVO_AMP_W_TWO_AXES;
                    update_config_word( s, x, DIGITAL_SERVO_AMP_AXIS_MASK );
                    }
                if ( is_retctl )
                    {
                    /*
                    ---------------------------------------------------------
                    Set bit 16 to disable using DAC 4 for the Amp lvdt offset
                    --------------------------------------------------------- */
                    update_config_word( s, BIT_16_MASK );
                    }
                }
            else if ( start_chars_are_equal(DigitalPotGainSetString, s.text()) )
                {
                if ( pc.is_enabled )
                    update_pc_digital_pot_gain( s, pc );
                }
            else if ( s.contains(IsControlSetString) )
                {
                cp = s.text();
                cp = findchar( HChar, cp );
                if ( cp )
                    {
                    cp++;
                    x = (unsigned) hextoul( cp );
                    if ( m.suretrak_controlled )
                        x |= BIT_0_MASK;
                    else
                        x &= ~BIT_0_MASK;
                    *cp = NullChar;
                    ultohex( hex, (unsigned long) x );
                    s += hex;
                    }
                }

            /*
            -------------------------------------------------------------
            Strip comments again just in case my replacement had comments
            ------------------------------------------------------------- */
            if ( !s.isempty() )
                truncate_control_file_line( s.text() );

            if ( !s.isempty() )
                {
                s.replace( OldStVarLineStart, VarLineStart );  /* Don't use the old .UV anymore */
                is_new = false;
                cp = s.text();
                if ( *cp == VChar && *(cp+1) >= ZeroChar && *(cp+1) <= NineChar )
                    {
                    is_new = uc.isnew( s );
                    if ( upload_state != UPLOAD_NEW_VARIABLES )
                        is_new = true;
                    }

                if ( upload_state == UPLOAD_ALL )
                    is_new = true;

                if ( is_new )
                    {
                    s += CrString;
                    send_socket_message( s.text(), false );
                    sent_something = true;
                    }
                }
            }
        }

    f.close();

    if ( sent_something )
        {
        /*
        --------------------------------------------------------------
        Save the variables and send the encoded file date to the Board
        -------------------------------------------------------------- */
        x = uc.save( part.computer, part.machine, part.name );
        if ( x == NO_FILE_DATE )
            x = 0;
        s = UploadCopyDateString;
        s += EqualsChar;
        s += x;
        s += CrChar;
        send_socket_message( s.text(), false );

        /*
        ---------------------------------------------------------------------
        If I had to upload everything then save the control file date as well
        --------------------------------------------------------------------- */
        if ( upload_state == UPLOAD_ALL )
            {
            s = ControlFileDateString;
            s += EqualsChar;
            s += control_file_date;
            s += CrChar;
            send_socket_message( s.text(), false );
            }

        send_socket_message( FlashVariablesString, false );
        }

    if ( m.suretrak_controlled )
        {
        s.get_exe_dir_file_name( SavepartExeName );
        if ( s.file_exists() || upload_state == UPLOAD_ALL )
        send_socket_message( GoString, false );
        }
    send_socket_message( END_OF_UPLOAD, false );

    SetEvent( SendEvent );
    upload_state = NO_UPLOAD_STATE;
    return true;
    }

return false;
}

/***********************************************************************
*                       UPDATE_ONE_BOARD_DISPLAY                       *
***********************************************************************/
static void update_one_board_display( int row, TCHAR * monitor_status_text )
{
TCHAR s[MAX_INTEGER_LEN+1];

if ( row>=0 && row<NofRows )
    {
    int32toasc( s, Boards[row].number, DECIMAL_RADIX );
    set_text( MainPushButton[row].w,    s                              );
    set_text( AddressTbox[row].w,       Boards[row].address.text()     );
    set_text( ConnectedTbox[row].w,     NoConnectState.text()          );
    set_text( MonitorStatusTbox[row].w, monitor_status_text            );
    if ( row == CurrentBoardIndex )
        {
        set_text( CurrentValuesCommStatusTb,    NoConnectState.text() );
        set_text( CurrentValuesMonitorStatusTb, monitor_status_text );
        }
    Boards[row].m.name.set_text(         MachineTbox[row].w );
    Boards[row].m.current_part.set_text( PartTbox[row].w    );
    }
}

/***********************************************************************
*                          DISPLAY_BOARD_INFO                          *
***********************************************************************/
void display_board_info()
{
int row;

for ( row=0; row<NofBoards; row++ )
    update_one_board_display( row, EmptyString );
}

/***********************************************************************
*                        POSITION_AUTO_SHOT_DISPLAY                    *
***********************************************************************/
void position_auto_shot_display()
{
RECT r;
INT  w;
INT  h;

AutoShotDisplayWindow.get_move_rect( r );

w = r.right - r.left;
h = r.bottom - r.top;

GetClientRect( GetDesktopWindow(), &r );

if ( r.top == 0 )
    r.top = -1;

r.left = r.right - w;
r.bottom = r.top + h;

SetWindowPos( AutoShotDisplayWindow.handle(), HWND_TOPMOST,
              r.left, r.top, r.right-r.left, r.bottom-r.top,
              SWP_HIDEWINDOW );
}

/***********************************************************************
*                          AUTOSHOTDISPLAYPROC                         *
***********************************************************************/
BOOL CALLBACK AutoShotDisplayProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static HBRUSH InputOnBackgroundBrush = 0;
HWND ws;
int  id;

switch (msg)
    {
    case WM_WINDOWPOSCHANGING:
        return TRUE;

    case WM_INITDIALOG:
        AutoShotDisplayWindow = hWnd;
        InputOnBackgroundBrush = CreateSolidBrush ( GreenColor );
        AutoShotDisplayWindow.post( WM_DBINIT );
        return TRUE;

    case WM_DESTROY:
        if ( InputOnBackgroundBrush )
            {
            DeleteObject( InputOnBackgroundBrush );
            InputOnBackgroundBrush = 0;
            }
        break;

    case WM_CTLCOLORSTATIC:
        ws = (HWND) lParam;
        id = GetDlgCtrlID( ws );
        if ( id == AUTO_SHOT_MACHINE_NUMBER_TBOX || id == AUTO_SHOT_LABEL_TBOX )
            {
            if ( IsWindowEnabled(ws) )
                {
                SetBkColor ((HDC) wParam, GreenColor );
                return (int) InputOnBackgroundBrush;
                }
            }
        break;

    case WM_DBINIT:
        position_auto_shot_display();
        MainWindow.set_focus();
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                     NEEDS_IMPACT_PARAMETER_FILE                      *
***********************************************************************/
static bool needs_impact_parameter_file( PART_CLASS & pa )
{
NAME_CLASS     s;
SETPOINT_CLASS sp;

s.get_impact_param_file_csvname( pa.computer, pa.machine, pa.name );
if ( s.file_exists() )
    {
    sp.get( s );
    if ( sp[IMPACT_PARAM_FILE_ENABLED_INDEX].boolean_value() )
        return true;
    }

return false;
}

/***********************************************************************
*              LOAD_THE_CURRENT_MACHINE_AT_THIS_BOARD                  *
***********************************************************************/
void load_the_current_machine_at_this_board( FTII_BOARD_ENTRY & dest )
{
STRING_CLASS   machine_name;
STRING_CLASS   new_machine_name;
STRING_CLASS   old_part_name;
MACHINE_NAME_LIST_CLASS mlist;
int            i;
DSTAT_CLASS    d;
bool           is_different_machine;
bool           monitoring_is_on;
NAME_CLASS     s;
STRING_CLASS   message;
int            shot_name_length;
BOOLEAN        shot_name_is_alpha;
SETPOINT_CLASS sp;
DB_TABLE       t;
TIME_CLASS     tc;
INI_CLASS      ini;
VDB_CLASS      vdb;

/*
---------------
Turn off alarms
--------------- */
if ( dest.alarm_bits && !IsRemoteMonitor )
    {
    if ( dest.m.monitor_flags & MA_ALARM_MONITORING_ON )
        dest.clear_all_alarm_bits();
    }

if ( IsRemoteMonitor && (dest.number == CurrentMachine.ft_board_number) )
    {
    monitoring_is_on = true;
    machine_name = CurrentMachine.name;
    }
else
    {
    monitoring_is_on = false;
    mlist.add_computer( ComputerName );
    mlist.rewind();
    while ( mlist.next() )
        {
        s.get_machset_csvname( ComputerName, mlist.name() );
        if ( sp.get(s) )
            {
            if ( dest.number == sp[MACHSET_FT_BOARD_NO_INDEX].int_value() )
                {
                if ( MA_MONITORING_ON & sp[MACHSET_MONITOR_FLAGS_INDEX].uint_value() )
                    monitoring_is_on = true;

                new_machine_name = mlist.name();

                if ( monitoring_is_on || machine_name.isempty() || new_machine_name==dest.m.name )
                    {
                    machine_name = new_machine_name;
                    if ( monitoring_is_on )
                        break;
                    }
                }
            }
        }
    if ( IsRemoteMonitor )
        monitoring_is_on = false;
    }

tc.get_local_time();
is_different_machine = false;

if ( IsRemoteMonitor )
    {
    dest.m.monitor_flags = 0;
    }
else
    {
    if ( machine_name != dest.m.name )
        is_different_machine = true;
    }

if ( dest.is_monitoring() )
    {
    d.get( dest.m.computer, dest.m.name );

    if ( !monitoring_is_on || is_different_machine )
        {
        if ( !d.is_down() )
            {
            d.set_cat( DOWNCAT_SYSTEM_CAT );
            d.set_subcat( DOWNCAT_PROG_EXIT_SUBCAT );
            d.set_time( tc.system_time() );
            d.put( dest.m.computer, dest.m.name );
            if ( dest.down_timeout_seconds > 0 )
                append_downtime_record( dest.m.computer, dest.m.name, dest.m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
            else
                issue_downtime_event(   dest.m.computer, dest.m.name, dest.m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
            }
        dest.send_socket_message( 0, true );
        if ( !monitoring_is_on && !is_different_machine )
            {
            /*
            ------------------------------
            Monitoring is being turned off
            ------------------------------ */
            dest.monitoring_was_stopped = true;
            }
        }
    }

dest.down_timeout_seconds = 0;
dest.extended_analog.clear();

if ( !machine_name.isempty() )
    {
    old_part_name = dest.part.name;
    if ( dest.m.find(ComputerName, machine_name) )
        {
        if ( dest.m.cycle_timeout_seconds > 0 )
            dest.down_timeout_seconds = (int) dest.m.cycle_timeout_seconds;

        if ( IsRemoteMonitor )
            {
            if ( machine_name == CurrentMachine.name )
                {
                dest.m.monitor_flags |= BITSETYPE( MA_MONITORING_ON );
                monitoring_is_on = true;
                }
            else
                {
                dest.m.monitor_flags &= BITSETYPE( ~MA_MONITORING_ON );
                monitoring_is_on = false;
                }
            }

        d.get( dest.m.computer, dest.m.name );
        dest.down_state = d.down_state();

        dest.part.find(     ComputerName, machine_name, dest.m.current_part );
        dest.param.find(    ComputerName, machine_name, dest.m.current_part );
        dest.pc.find(       ComputerName, machine_name, dest.m.current_part );
        dest.ftanalog.load( ComputerName, machine_name, dest.m.current_part );
        dest.ftcalc.load( dest.param,  ComputerName, machine_name, dest.m.current_part );
        dest.external_parameter_settings.get( ComputerName, machine_name, dest.m.current_part );
        dest.get_alarm_bits();
        dest.have_runlist = dest.runlist.get( ComputerName, machine_name );
        if ( dest.have_runlist )
            {
            for ( i=0; i<dest.runlist.count(); i++ )
                {
                if ( !part_exists(ComputerName, machine_name, dest.runlist[i].partname) )
                    MessageBox( 0, resource_string(NO_WARMUP_PART_STRING), dest.runlist[i].partname.text(), MB_OK | MB_SYSTEMMODAL );
                }
            }
        dest.shotdata.set_us_per_time_sample( dest.part.ms_per_time_sample * 1000 );
        dest.set_extended_analog_bits();
        dest.needs_impact_position_report = needs_impact_parameter_file( dest.part );

        if ( is_different_machine )
            dest.monitoring_was_stopped = false;

        /*
        ------------------------------------
        Don't monitor if this is the NEWPART
        ------------------------------------ */
        if ( is_float_zero(dest.part.min_stroke_length) || is_float_zero(dest.part.total_stroke_length) )
            {
            dest.m.monitor_flags &= BITSETYPE(~MA_MONITORING_ON);
            monitoring_is_on = false;
            if ( !IsRemoteMonitor || (machine_name == CurrentMachine.name) )
                dest.send_socket_message( 0, true );
            }

        if ( monitoring_is_on )
            {
            /*
            -------------------------
            Get the last channel_mode
            ------------------------- */
            s.get_ft2_editor_settings_file_name( ComputerName, machine_name );
            if ( s.file_exists() )
                {
                ini.set_file( s.text() );
                ini.set_section( ConfigSection );
                if ( ini.find(CurrentChannelModeKey) )
                    dest.channel_mode = ini.get_int();
                }

            /*
            -----------------------------------------------
            Make sure the results directory databases exist
            ----------------------------------------------- */
            if ( is_different_machine )
                {
                s.get_alarmsum_dbname( dest.part.computer, dest.part.machine, dest.part.name );
                t.ensure_existance( s );
                s.get_graphlst_dbname( dest.part.computer, dest.part.machine, dest.part.name );
                t.ensure_existance( s );
                s.get_shotparm_csvname( dest.part.computer, dest.part.machine, dest.part.name );
                vdb.ensure_existance( s );
                }

            /*
            -------------------------------------------------
            Make sure the graphlst table has alpha shot names
            ------------------------------------------------- */
            s.get_graphlst_dbname( dest.part.computer, dest.part.machine, dest.part.name );
            if ( t.open(s, AUTO_RECLEN, PFL) )
                {
                shot_name_is_alpha = t.is_alpha( GRAPHLST_SHOT_NAME_INDEX );
                shot_name_length   = (int) t.field_length( GRAPHLST_SHOT_NAME_INDEX );
                t.close();
                if ( !shot_name_is_alpha )
                    {
                    message = StringTable.find( TEXT("UPDATING_GRAPHLST_STRING") );
                    temp_message( message.text() );
                    upsize_table_w_shot_name( s, shot_name_length );
                    kill_temp_message();
                    }
                }

            if ( d.down_state()==PROG_DOWN_STATE || d.down_state()==NO_DOWN_STATE || dest.down_timeout_seconds==0 )
                {
                /*
                -------------------------------------------
                Replace the program down with program start
                ------------------------------------------- */
                d.set_cat( DOWNCAT_SYSTEM_CAT );
                d.set_subcat( DOWNCAT_PROG_START_SUBCAT );
                d.set_time( tc.system_time() );
                d.put( dest.m.computer, dest.m.name );
                dest.down_state = d.down_state();
                dest.time_of_last_shot = tc;
                if ( dest.down_timeout_seconds > 0 )
                    append_downtime_record( dest.m.computer, dest.m.name, dest.m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_START_SUBCAT );
                else
                    issue_downtime_event(   dest.m.computer, dest.m.name, dest.m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_START_SUBCAT );
                }

            if ( dest.connect_state & CONNECTED_STATE )
                {
                if ( MonitorWire != NO_WIRE )
                    {
                    if ( !ClearMonitorWireOnDown || dest.down_state == MACH_UP_STATE || dest.down_state == PROG_UP_STATE )
                        dest.setwire( MonitorWire );
                    }
                }
            }
        }
    }
}

/***********************************************************************
*                           LOAD_FTII_BOARDS                           *
* This is called by the main program to load the boards once the       *
* initialization is complete.                                          *
***********************************************************************/
int load_ftii_boards()
{
NAME_CLASS s;
DB_TABLE   t;
int        i;
int        n;
char       cbuf[TCP_ADDRESS_LEN+1];

if ( Boards )
    {
    delete[] Boards;
    Boards    = 0;
    NofBoards = 0;
    }

Have_At_Least_One_FasTrak2 = FALSE;

s.get_ft2_boards_dbname( ComputerName );
if ( s.file_exists() )
    {
    t.open( s, FTII_BOARDS_RECLEN, PFL );
    n = t.nof_recs();
    if ( n > 0 )
        {
        Boards = new FTII_BOARD_ENTRY[n];
        if ( Boards )
            {
            NofBoards = 0;
            while ( t.get_next_record(FALSE) )
                {
                if ( NofBoards < n )
                    {
                    Boards[NofBoards].number = t.get_short( FTII_BOARDS_NUMBER_OFFSET );

                    Boards[NofBoards].address.upsize( TCP_ADDRESS_LEN );
                    t.get_alpha( Boards[NofBoards].address.text(), FTII_BOARDS_ADDRESS_OFFSET, TCP_ADDRESS_LEN );
                    Boards[NofBoards].port.upsize( TCP_PORT_LEN );
                    t.get_alpha( Boards[NofBoards].port.text(), FTII_BOARDS_PORT_OFFSET,    TCP_PORT_LEN );

                    Boards[NofBoards].is_ft3 = false;

                    if ( Boards[NofBoards].port != Ft2UdpPort )
                        Boards[NofBoards].is_ft3 = true;
                    else
                        Have_At_Least_One_FasTrak2 = TRUE;

                    Boards[NofBoards].connect_state = NO_FT2_CONNECT_STATE;

                    if ( Boards[NofBoards].is_ft3 )
                        Boards[NofBoards].a.sin_family = AF_INET;
                    else
                    Boards[NofBoards].a.sin_family           = PF_INET;
                    Boards[NofBoards].a.sin_port = htons( (unsigned short) Boards[NofBoards].port.uint_value() );
                    s = Boards[NofBoards].address;
                    remove_all_occurances( s.text(), SpaceChar );
                    unicode_to_char( cbuf, Boards[NofBoards].address.text() );
                    Boards[NofBoards].a.sin_addr.S_un.S_addr = inet_addr( cbuf );

                    NofBoards++;
                    }
                }
            }
        }
    t.close();
    }

if ( NofBoards > 0 )
    {
    for ( i=0; i<NofBoards; i++ )
        {
        load_the_current_machine_at_this_board( Boards[i] );
        Boards[i].index = i;
        }
    }

return NofBoards;
}

/***********************************************************************
*                        DISCONNECT_FROM_BOARDS                        *
***********************************************************************/
static void disconnect_from_boards()
{
int          i;
DSTAT_CLASS  d;
TIME_CLASS   tc;
unsigned     mask;

tc.get_local_time();
for ( i=0; i<NofBoards; i++ )
    {
    if ( Boards[i].is_monitoring() )
        {
        if ( Boards[i].connect_state & CONNECTED_STATE )
            {
            mask = Boards[i].alarm_bits;
            if ( MonitorWire != NO_WIRE )
                mask |= wirebit( MonitorWire );

            if ( DownTimeAlarmWire != NO_WIRE )
                mask |= wirebit( DownTimeAlarmWire );

            if ( mask )
                Boards[i].send_bits( mask, false );

            Boards[i].bits_currently_on = 0;

            if ( Boards[i].is_sending_op_data_packets )
                Boards[i].send_socket_message( DONT_SEND_OP_DATA_PACKETS, true );

            Boards[i].send_socket_message( 0, false );
            }

       /*
       -------------------------------------------------------------------------------
       If I am running the warmup part, restore the main part name as the current part
       ------------------------------------------------------------------------------- */
       if ( Boards[i].have_runlist )
           {
           if ( Boards[i].runlist.count() > 1 )
               {
               if ( Boards[i].runlist[0].partname != Boards[i].part.name )
                   set_current_part_name( Boards[i].part.computer, Boards[i].part.machine, Boards[i].runlist[0].partname );
               }
           }
        d.get( Boards[i].m.computer, Boards[i].m.name );
        if ( d.down_state()==PROG_UP_STATE || d.down_state()==MACH_UP_STATE || d.down_state()==NO_DOWN_STATE || Boards[i].down_timeout_seconds==0 )
            {
            d.set_cat( DOWNCAT_SYSTEM_CAT );
            d.set_subcat( DOWNCAT_PROG_EXIT_SUBCAT );
            d.set_time( tc.system_time() );
            d.put( Boards[i].m.computer, Boards[i].m.name );
            if ( Boards[i].down_timeout_seconds > 0 )
                append_downtime_record( Boards[i].m.computer, Boards[i].m.name, Boards[i].m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
            else
                issue_downtime_event(   Boards[i].m.computer, Boards[i].m.name, Boards[i].m.current_part, tc.system_time(), DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT );
            }
        }
    }
}

/***********************************************************************
*                    CONTINUE_CONNECTING_ONE_BOARD                     *
***********************************************************************/
static void continue_connecting_one_board( int i )
{
static TCHAR POS_AT_IMPACT_DISABLE[] = TEXT( "OC13=0\r" );
static TCHAR POS_AT_IMPACT_ENABLE[]  = TEXT( "OC13=1\r" );

static TCHAR set_input_bitmask_string[] = TEXT( "V427=H" );
static STRING_CLASS s;
unsigned u;
int      ds;
int      j;
TCHAR    hex[MAX_HEX_LEN+1];

if ( Boards[i].upload_state != WAITING_FOR_INFO )
    return;

if ( Boards[i].version == 0 )
    return;

if ( Boards[i].control_file_date == 0 )
    return;

if ( Boards[i].upload_copy_date == 0 )
    return;

/*
-----------------------------------------
Tell the board which bits I am monitoring
----------------------------------------- */
u = 0;
if ( Boards[i].have_runlist )
    u = Boards[i].runlist.bitmask();
if ( Boards[i].m.suretrak_controlled )
    u |= RetractWireMask;
u |= NoUploadWireMask;
for ( j=0; j<Boards[i].extended_analog.n; j++ )
    u |= Boards[i].extended_analog[j].mask;

/*
--------------------------
Add bits for Al's Log File
-------------------------- */
if ( AlsLogIsActive )
    u |= ALS_BITS;

s = set_input_bitmask_string;

ultohex( hex, (unsigned long) u );
s += hex;
s += CrString;
Boards[i].send_socket_message( s.text(), true );
Boards[i].send_socket_message( GetInputBitsString, true );

if ( Boards[i].needs_impact_position_report )
    Boards[i].send_socket_message( POS_AT_IMPACT_ENABLE, true );
else
    Boards[i].send_socket_message( POS_AT_IMPACT_DISABLE, true );

if ( !Boards[i].monitoring_was_stopped )
    {
    if ( !Boards[i].upload_control_file() )
        update_monitor_status( &Boards[i], resource_string(NO_UPLOAD_FILE_STRING) );
    }

Boards[i].monitoring_was_stopped = false;

Boards[i].clear_all_alarm_bits();
if ( MonitorWire != NO_WIRE && Boards[i].down_timeout_seconds > 0 )
    {
    ds = Boards[i].down_state;
    if ( !ClearMonitorWireOnDown || ds == PROG_UP_STATE || ds == MACH_UP_STATE || ds == NO_DOWN_STATE )
        {
        Boards[i].setwire( MonitorWire );
        }
    }
}

/***********************************************************************
*                          CONNECT_ONE_BOARD                           *
***********************************************************************/
static void connect_one_board( int i )
{
unsigned long h;
bool          not_connected;
unsigned u;

not_connected = true;
if ( Boards[i].connect_state & CONNECTED_STATE )
    not_connected = false;

if ( Boards[i].is_monitoring() )
    {
    if ( Boards[i].is_ft3 )
        {
        if ( !Boards[i].ft3_send_thread_is_running )
            {
            h = _beginthread( ft3_send_thread, 0, (void *)(Boards+i) );
            if ( h == -1 )
                {
                Boards[i].upload_state = SEND_THREAD_START_FAILURE;
                return;
                }
            }
        }

    /*
    ----------------------------------------------------------------------------
    I use the following to decide which error message to display if I don't talk
    ---------------------------------------------------------------------------- */
    Boards[i].have_response = false;

    Boards[i].upload_state      = WAITING_FOR_INFO;
    Boards[i].version           = 0;
    Boards[i].control_file_date = 0; /* V447 */
    Boards[i].upload_copy_date  = 0; /* V448 */
    Boards[i].set_connect_state( CONNECTING_STATE );
    if ( not_connected )
        Boards[i].send_socket_message( GetTimerFrequencyString, false );
    Boards[i].send_socket_message( GetVersionString, true );
    Boards[i].send_socket_message( GetControlFileDateString, true );
    Boards[i].send_socket_message( GetUploadCopyDateString, true );
    }
else
    {
    u = NOT_MONITORED_STATE;
    if ( is_float_zero(Boards[i].part.min_stroke_length) || is_float_zero(Boards[i].part.total_stroke_length) )
        u |= ZERO_STROKE_LENGTH_STATE;
    Boards[i].set_connect_state( u );
    }
}

/***********************************************************************
*                           CONNECT_TO_BOARDS                          *
*  This is only run at startup so I check here for the remote machine  *
*  and make sure it is not uploaded the first time.                    *
***********************************************************************/
static void connect_to_boards()
{
int i;

for ( i=0; i<NofBoards; i++ )
    {
    if ( IsRemoteMonitor && (Boards[i].number == CurrentMachine.ft_board_number) )
        Boards[i].monitoring_was_stopped = true;
    connect_one_board( i );
    }
}

/***********************************************************************
*                      CHECK_FOR_CYCLE_START                           *
***********************************************************************/
static bool check_for_cycle_start( FTII_BOARD_ENTRY * board,  TCHAR * sorc )
{
static TCHAR CYCLE_START[] = TEXT("Cycle start");
static TCHAR TIMEOUT[]     = TEXT("timeout");

NAME_CLASS s;
TCHAR * cp;
unsigned u;
EXTERNAL_SHOT_NAME_CLASS esn;

if ( !board )
    return false;

cp = findstring( CYCLE_START, sorc );
if ( cp )
    {
    if ( findstring(TIMEOUT, cp) )
        {
        update_monitor_status( board, resource_string(TIMEOUT_STRING) );
        board->do_not_upload_timeout = 0;
        board->do_not_upload         = false;
        set_text( Dnu[board->index], EmptyString );
        if ( board->part.thirty_second_timeout_wire != NO_WIRE )
            board->setwire( board->part.thirty_second_timeout_wire );
        }
    else
        {
        update_monitor_status( board, resource_string(CYCLE_START_STRING) );

        /*
        ------------------------------------------------
        Send a message telling ft2 about the cycle start
        ------------------------------------------------ */
        if ( board->external_parameter_settings.enable && board->external_parameter_settings.available_at_eos )
            {
            s.null();
            s.cat_w_char( board->part.machine, CommaChar );
            s += board->part.name;
            MainWindow.post( WM_NEW_DATA,(WPARAM) NEW_CYCLE_START, (LPARAM)s.release() );
            }

        if ( board->part.shot_complete_wire != NO_WIRE )
            {
            board->clearwire( board->part.shot_complete_wire );
            board->need_to_check_for_shot_complete_timeout = false;
            }

        if ( board->part.shot_save_complete_wire != NO_WIRE )
            board->clearwire( board->part.shot_save_complete_wire );

        if ( board->part.st2_program_abort_wire != NO_WIRE )
            board->clearwire( board->part.st2_program_abort_wire );

        if ( board->part.thirty_second_timeout_wire != NO_WIRE )
            board->clearwire( board->part.thirty_second_timeout_wire );

        if ( board->part.warning_shot_wire != NO_WIRE )
            board->clearwire( board->part.warning_shot_wire );

        if ( board->part.alarm_shot_wire != NO_WIRE )
            board->clearwire( board->part.alarm_shot_wire );

        if ( board->part.good_shot_wire != NO_WIRE )
            board->clearwire( board->part.good_shot_wire );

        if ( ClearAlarmsEachShot )
            {
            u = board->get_parameter_alarm_bits();
            if ( u )
                board->send_bits( u, false );
            }

        if ( !NoUploadWireMask && board->m.suretrak_controlled )
            {
            board->do_not_upload_timeout = 0;
            board->do_not_upload         = true;
            if ( UploadStartShotTimeout )
                {
                board->do_not_upload_timeout = GetTickCount();
                u = UploadStartShotTimeout;
                u *= 1000;
                board->do_not_upload_timeout += u;
                }
            set_text( Dnu[board->index], AsterixString );
            }

        esn.get( board->part.computer, board->part.machine, board->part.name );
        if ( esn.is_enabled() && esn.have_command() )
            {
            s.get_exe_dir_file_name( 0 );
            execute_and_wait( esn.command().text(), s.text(), (DWORD) 0 );
            }
        }
    return true;
    }

return false;
}

/***********************************************************************
*                             DO_ASCII_MESSAGE                         *
***********************************************************************/
static void do_ascii_message( int bindex, TCHAR * sorc )
{
static const TCHAR ANALOG_RESPONSE[] = TEXT( "O_CH" );
static const int  ANALOG_RESPONSE_SLEN = 4;

static const TCHAR TIMER_FREQ_RESPONSE[] = TEXT( "V301_" );
static const int   TIMER_FREQ_SLEN       = 5;

static const TCHAR AT_HOME_RESPONSE[]    = TEXT( "R_At home" );
static const TCHAR POS_AT_IMPACT_RESPONSE[]    = TEXT( "R_POS1EOS" );
static const int   POS_AT_IMPACT_RESPONSE_SLEN = 9;

static const TCHAR VERSION_RESPONSE[]    = TEXT( "R:6#emc_" );
static const TCHAR VERSION_RESPONSE2[]   = TEXT( "R:6#cyc_" );  /* Cyclone II board returns this */
static const int   VERSION_RESPONSE_SLEN = 8;

static const TCHAR FATAL_OR_WARNING_RESPONSE[] = TEXT( "_Cont." );
static const int   FATAL_OR_WARNING_RESPONSE_SLEN = 6;

FTII_BOARD_ENTRY * board;
TCHAR * cp;
TCHAR * commap;
FILE_CLASS         f;
LPARAM             lParam;
bool    matched_message;
double             position;
NAME_CLASS         s;
SETPOINT_CLASS     sp;
WPARAM             wParam;

board = Boards + bindex;

matched_message = check_for_cycle_start( board,  sorc );

/*
----------------------------------------
See if this is a timer frequency message
---------------------------------------- */
if ( !matched_message )
    {
    cp = findstring( TIMER_FREQ_RESPONSE, sorc );
    if ( cp )
        {
        cp += TIMER_FREQ_SLEN;
        board->timer_frequency = asctoint32( cp );
        matched_message = true;
        }
    }

if ( !matched_message )
    {
    cp = findstring( INPUT_BITS_RESPONSE, sorc );
    if ( cp )
        {
        cp += INPUT_BITS_RESPONSE_SLEN;
        board->check_inputs( cp );
        matched_message = true;
        }
    }

if ( !matched_message )
    {
    cp = findstring( AT_HOME_RESPONSE, sorc );
    if ( cp )
        {
        matched_message = true;
        if ( board->do_not_upload )
            {
            board->do_not_upload = false;
            board->do_not_upload_timeout = 0;
            set_text( Dnu[board->index], EmptyString );
            }
        }
    }

if ( !matched_message )
    {
    cp = findstring( POS_AT_IMPACT_RESPONSE, sorc );
    if ( cp )
        {
        matched_message = true;
        cp = findchar( PoundChar, cp );
        if ( cp )
            {
            cp++;
            position = double( asctoint(cp) );
            position = board->part.dist_from_x4( position );
            /*
            ------------------------
            Gibbs wants biscuit size
            ------------------------ */
            position = board->part.total_stroke_length - position;

            s.get_impact_param_file_csvname( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
            if ( s.file_exists() )
                {
                sp.get( s );
                if ( sp[IMPACT_PARAM_FILE_ENABLED_INDEX].boolean_value() )
                    {
                    s = sp[IMPACT_PARAM_FILE_NAME_INDEX].value;
                    if (!s.isempty() )
                        {
                        if ( f.open_for_write(s) )
                            {
                            s = position;
                            f.writeline( s );
                            f.close();
                            }
                        }
                    }
                }
            }
        }
    }

if ( !matched_message )
    {
    cp = findstring( VERSION_RESPONSE, sorc );
    if ( !cp )
        cp = findstring( VERSION_RESPONSE2, sorc );
    if ( cp )
        {
        cp += VERSION_RESPONSE_SLEN;
        commap = findchar( CommaChar, cp );
        if ( commap )
            {
            *commap = NullChar;
            if ( board->version_string != cp )
                {
                board->version_string = cp;
                board->put_version_string();
                }
            *commap = CommaChar;
            }
        board->version = asctoint32( cp );
        cp += 2;
        board->sub_version = asctoint32( cp );
        matched_message = true;
        /*
        ----------------------------------------------------------------
        I now wait for the version, V447, and V448 messages to come back
        before uploading.
        ---------------------------------------------------------------- */
        if ( board->upload_state == WAITING_FOR_INFO )
            continue_connecting_one_board( bindex );
        }
    }

if ( !matched_message )
    {
    if ( strings_are_equal(sorc, ControlFileDateString, VARIABLE_NUMBER_LEN) )
        {
        cp = sorc + VARIABLE_NUMBER_LEN + 1;
        if ( *cp == MinusChar )
            {
            board->control_file_date = NO_FILE_DATE;
            }
        else
            {
            board->control_file_date = asctoul(cp);
            if ( board->control_file_date == 0 )
                board->control_file_date = NO_FILE_DATE;
            }
        continue_connecting_one_board( bindex );
        matched_message = true;
        }
    }

if ( !matched_message )
    {
    if ( strings_are_equal(sorc, UploadCopyDateString, VARIABLE_NUMBER_LEN) )
        {
        cp = sorc + VARIABLE_NUMBER_LEN + 1;
        if ( *cp == MinusChar )
            {
            board->upload_copy_date = NO_FILE_DATE;
            }
        else
            {
            board->upload_copy_date = asctoul(cp);
            if ( board->upload_copy_date == 0 )
                board->upload_copy_date = NO_FILE_DATE;
            continue_connecting_one_board( bindex );
            }
        matched_message = true;
        }
    }

if ( !matched_message )
    {
    /*
    ----------------------------------------------------------------------------------------
    I always need to check to see if an analog message is in response to an extended channel
    request.
    ---------------------------------------------------------------------------------------- */
    if ( strings_are_equal(sorc, ANALOG_RESPONSE, ANALOG_RESPONSE_SLEN) )
        {
        board->check_for_extended_channel_value( sorc );
        matched_message = true;
        }
    }

if ( !matched_message )
    {
    cp = sorc;
    cp++;
    if ( strings_are_equal(cp, FATAL_OR_WARNING_RESPONSE, FATAL_OR_WARNING_RESPONSE_SLEN) )
        {
        cp += FATAL_OR_WARNING_RESPONSE_SLEN;
        cp++;
        update_monitor_status( board, cp );
        /*
        --------------------------------------------------------------------------------------
        If the first character is an F this is a fatal error and I might as well allow updates
        -------------------------------------------------------------------------------------- */
        if ( *sorc == FChar || *sorc == FCharLow )
            {
            board->do_not_upload = false;
            board->do_not_upload_timeout = 0;
            set_text( Dnu[bindex], EmptyString );
            if ( board->part.st2_program_abort_wire != NO_WIRE )
                board->setwire( board->part.st2_program_abort_wire );
            }
        }
    }

/*
---------------------------------------------------------------------
If this is the board that has the current machine then send all ascii
data to the main window in case the terminal is on.
The main window will delete the buffer.
--------------------------------------------------------------------- */
if ( bindex == CurrentBoardIndex )
    {
    lParam = (LPARAM) sorc;
    wParam = (WPARAM) ASCII_TYPE;
    MainWindow.post( WM_NEW_RT_DATA, wParam, lParam );
    }
else
    {
    delete[] sorc;
    }
}

/***********************************************************************
*                           DO_ASCII_MESSAGE                           *
***********************************************************************/
static void do_ascii_message( int bindex, SOCKET_MESSAGE * mp )
{
static TCHAR lfchar = TEXT('\n');
char * sorc;
TCHAR * dest;
int    n;
BINARY_HEADER  * bh;

sorc = mp->s;
if ( *sorc == 'B' )
    {
    bh = (BINARY_HEADER *) mp->s;
    n = (int) bh->nof_bytes;
    sorc += sizeof( BINARY_HEADER );
    }
else
    {
    n = mp->len;
    }

if ( n < 1 )
    return;

/*
----------------------------------
See if this is just a plus message
---------------------------------- */
if ( *sorc == PlusCharAscii )
    return;

dest = maketext_from_char( sorc, n );
if ( dest )
    {
    n--;
    if ( dest[n] == lfchar )
        dest[n] = NullChar;
    do_ascii_message( bindex, dest );
    }
}

/***********************************************************************
*                             DO_NEWSETUP                              *
***********************************************************************/
static void do_newsetup( STRING_CLASS & machine_name )
{
int i;
MACHINE_CLASS m;
STRING_CLASS  s;

if ( m.find(ComputerName, machine_name) )
    {
    if ( m.is_ftii )
        {
        for ( i=0; i<NofBoards; i++ )
            {
            if ( Boards[i].number == m.ft_board_number )
                {
                /*
                ------------------------------------------------------------------------
                If this board is not being monitored and that hasn't changed, do nothing
                ------------------------------------------------------------------------ */
                if ( !Boards[i].is_monitoring() && !(m.monitor_flags & MA_MONITORING_ON) )
                    return;

                if ( i < NofRows )
                    {
                    if ( Boards[i].is_monitoring() && !(Boards[i].connect_state & ZERO_STROKE_LENGTH_STATE) )
                        {
                        s = resource_string( NEW_SETUP_STRING );
                        s.set_text( MonitorStatusTbox[i].w );
                        if ( i == CurrentBoardIndex )
                            s.set_text( CurrentValuesMonitorStatusTb );
                        }
                    }
                if ( Boards[i].is_sending_op_data_packets )
                    {
                    Boards[i].send_socket_message( DONT_SEND_OP_DATA_PACKETS, true );
                    Boards[i].start_op_data_after_upload = true;
                    }
                load_the_current_machine_at_this_board( Boards[i] );
                connect_one_board( i );
                if ( Boards[i].is_monitoring() )
                    {
                    if ( Boards[i].monitoring_was_stopped )
                        {
                        s = resource_string( RESTARTED_STRING );
                        s.set_text( MonitorStatusTbox[i].w );
                        if ( i == CurrentBoardIndex )
                            s.set_text( CurrentValuesMonitorStatusTb );
                        }
                    }
                skip_reset_machine( Boards[i].m.name );
                if ( i < NofRows )
                    {
                    Boards[i].m.name.set_text( MachineTbox[i].w );
                    Boards[i].m.current_part.set_text( PartTbox[i].w );
                    }
                break;
                }
            }
        }
    }
}

/***********************************************************************
*                            CHECK_FOR_DOWN                            *
*        I also check for the alarm timeout in this routine            *
***********************************************************************/
void check_for_down()
{
unsigned   alarm_bits_currently_on;
TIME_CLASS   basetime;
int        i;
DSTAT_CLASS  d;
int        ds;
int        my_alarm_timeout_seconds;
int        my_timeout_seconds;
TIME_CLASS   now;
int        seconds_since_last_shot;
unsigned     shot_complete_alarm_bit;
int          wire_number;

basetime.get_local_time();

for ( i=0; i<NofBoards; i++ )
    {
    my_alarm_timeout_seconds = Boards[i].part.alarm_timeout_seconds;
    my_timeout_seconds = Boards[i].down_timeout_seconds;
    if ( Boards[i].is_monitoring() &&  (my_timeout_seconds > 0 || my_alarm_timeout_seconds > 0)  )
        {
        if ( my_timeout_seconds > 0 )
            {
            ds = Boards[i].down_state;
            if ( ds == PROG_UP_STATE || ds == MACH_UP_STATE || ds == NO_DOWN_STATE )
                {
                now = basetime;
                seconds_since_last_shot = now - Boards[i].time_of_last_shot;

                if ( seconds_since_last_shot > my_timeout_seconds )
                    {
                    if ( SubtractTimeoutFromAutoDown )
                        now -= (unsigned long) my_timeout_seconds;

                    if ( StdDown.find((long) now.current_value()) )
                        {
                        d.set_cat( StdDown.cat() );
                        d.set_subcat( StdDown.subcat() );
                        }
                    else
                        {
                        d.set_cat( DOWNCAT_SYSTEM_CAT );
                        d.set_subcat( DOWNCAT_UNSP_DOWN_SUBCAT );
                        }

                    d.set_time( now.system_time() );
                    d.put( Boards[i].part.computer, Boards[i].part.machine );
                    Boards[i].down_state = d.down_state();
                    append_downtime_record( Boards[i].part.computer, Boards[i].part.machine, Boards[i].part.name, now.system_time(), d.category(), d.subcategory() );

                    if ( MonitorWire != NO_WIRE && ClearMonitorWireOnDown )
                        Boards[i].clearwire( MonitorWire );

                    if ( DownTimeAlarmWire != NO_WIRE )
                        Boards[i].setwire( DownTimeAlarmWire );

                    if ( TurnOffAlarmsOnDown )
                        Boards[i].clear_all_alarm_bits();
                    }
                }
            }

        alarm_bits_currently_on = Boards[i].alarm_bits & Boards[i].bits_currently_on;

        if ( alarm_bits_currently_on )
            {
            now = basetime;
            seconds_since_last_shot = now - Boards[i].time_of_last_shot;

            if ( my_alarm_timeout_seconds > 0 )
                {
            if ( seconds_since_last_shot > my_alarm_timeout_seconds )
                    {
                Boards[i].clear_all_alarm_bits();
                    alarm_bits_currently_on = 0;
            }
        }
    }
}

    if ( Boards[i].need_to_check_for_shot_complete_timeout )
        {
        alarm_bits_currently_on = Boards[i].alarm_bits & Boards[i].bits_currently_on;
        if ( alarm_bits_currently_on )
            {
            now = basetime;
            my_timeout_seconds      = Boards[i].part.shot_complete_timeout_seconds;
            seconds_since_last_shot = now - Boards[i].time_of_last_shot;
            if ( seconds_since_last_shot > my_timeout_seconds )
                {
                wire_number = (int) Boards[i].part.shot_complete_wire;
                shot_complete_alarm_bit = wirebit( wire_number );
                if ( shot_complete_alarm_bit & alarm_bits_currently_on )
                    Boards[i].clearwire( wire_number );
                Boards[i].need_to_check_for_shot_complete_timeout = false;
                }
            }
        else
            {
            Boards[i].need_to_check_for_shot_complete_timeout = false;
            }
        }
    }
}

/***********************************************************************
*                            DO_ACK_MESSAGE                            *
*           I have recieved an ack or nak from the board.              *
***********************************************************************/
static void do_ack_message( SOCKET_MESSAGE * mp )
{
bool message_is_complete;

SOCKET_MESSAGE * dead_mp;
int      i;
unsigned new_state;
STRING_CLASS network_error_message;

i = board_index( mp->a );
if ( i == NO_INDEX )
    return;

if ( !Boards[i].waiting_for_ack )
    return;

message_is_complete = false;

if ( mp->s[0] == NakChar )
    {
    if ( TerminalIsVisible )
        MainListBox.add( resource_string(NAK_RECEIVED_STRING) );
    Boards[i].nak_count++;
    Boards[i].waiting_for_ack  = false;
    network_error_message = TEXT("Nak Received on Board ");
    network_error_message += i+1;

    if ( Boards[i].nak_count >= MAX_NAK_COUNT )
        {
        network_error_message += TEXT( " Not Connected" );
        new_state = NOT_CONNECTED_STATE;
        if ( Boards[i].uploading )
            {
            Boards[i].uploading      = false;
            Boards[i].bytes_uploaded = 0;
            /*
            -------------------------------------------------------------------
            The upload failed. Flush the que and make a note so I can try again
            ------------------------------------------------------------------- */
            new_state |= UPLOAD_WAS_ABORTED_STATE;
            while ( Boards[i].f.count() )
                {
                dead_mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                SocketMessageStore.push( dead_mp );
                }
            }
        Boards[i].set_connect_state( new_state );
        }
    log_network_error_string( network_error_message );
    }
else
    {
    message_is_complete = true;
    }

if ( message_is_complete )
    {
    dead_mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
    SocketMessageStore.push( dead_mp );

    Boards[i].waiting_for_ack = false;
    Boards[i].nak_count       = 0;
    }
}

/***********************************************************************
*                          CHECK_FOR_TIMEOUTS                          *
*    This just nak's any board that is waiting for an ack or nak       *
*    because it is time to try again. If the number of tries has       *
*    reached the MAX_NAK_COUNT I give up.                              *
***********************************************************************/
static void check_for_timeouts()
{
int      i;
unsigned new_state;
SOCKET_MESSAGE * mp;
BINARY_HEADER  * bh;
STRING_CLASS     network_error_message;
for ( i=0; i<NofBoards; i++ )
    {
    if ( Boards[i].waiting_for_ack )
        {
        Boards[i].nak_count++;
        Boards[i].waiting_for_ack  = false;
        if ( Boards[i].nak_count >= MAX_NAK_COUNT )
            {
            network_error_message.null();

            /*
            ---------------------------------------------------------------
            If the packet sent was a null packet then I don't want to leave
            it on the stack.
            --------------------------------------------------------------- */
            mp = (SOCKET_MESSAGE *) Boards[i].f.peek();
            if ( mp )
                {
                if ( mp->len == 0 )
                    {
                    mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                    SocketMessageStore.push( mp );
                    }
                }
            new_state               = NOT_CONNECTED_STATE;
            Boards[i].nak_count     = 0;
            if ( Boards[i].uploading || (Boards[i].version == 0) )
                {
                Boards[i].uploading      = false;
                Boards[i].bytes_uploaded = 0;
                if ( Boards[i].is_monitoring() )
                    {
                    if ( Boards[i].have_response )
                        {
                        update_monitor_status( &Boards[i], UploadFailed.text() );
                        network_error_message += UploadFailed;
                        }
                    else
                        {
                        update_monitor_status( &Boards[i], NoResponse.text() );
                        network_error_message += NoResponse;
                        }
                    /*
                    -------------------------------------------------------------------
                    The upload failed. Flush the que and make a note so I can try again
                    ------------------------------------------------------------------- */
                    new_state |= UPLOAD_WAS_ABORTED_STATE;
                    while ( Boards[i].f.count() )
                        {
                        mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                        SocketMessageStore.push( mp );
                        }
                    }
                }
            else
                {
                if ( Boards[i].is_monitoring() )
                    update_monitor_status( &Boards[i], EmptyString );
                }
            Boards[i].ms_at_last_contact = GetTickCount();
            if ( Boards[i].is_monitoring() )
                {
                Boards[i].set_connect_state( new_state );
                if ( NewDataEvent )
                    {
                    mp = get_socket_message();
                    mp->a = Boards[i].a;
                    bh = (BINARY_HEADER *) mp->s;
                    bh->bchar = 'B';
                    bh->data_type = CONNECTION_LOST;
                    bh->flags        = 0;
                    bh->data_set_num = 0;
                    bh->packet_num   = 0;
                    bh->nof_packets  = 0;
                    bh->nof_bytes    = 0;
                    mp->len = sizeof( BINARY_HEADER );
                    NewData.push( mp );
                    SetEvent( NewDataEvent );
                    }
                network_error_message += TEXT(" Connection Lost" );
                }
            if ( !network_error_message.isempty() )
                log_network_error_string( network_error_message );

            }
        }
    }
}

/***********************************************************************
*                    CONVERT_IO_CHANGE_TO_ASCII                        *
***********************************************************************/
void convert_io_change_to_ascii( int bindex, SOCKET_MESSAGE * mp )
{
char * cp;
unsigned int * xp;
TCHAR * s;
TCHAR * sp;
TCHAR hex[MAX_HEX_LEN+1];

cp = mp->s;
cp += sizeof( BINARY_HEADER );
xp = (unsigned int *) cp;
s = maketext( INPUT_BITS_RESPONSE, 8 );
if ( s )
    {
    sp = s + INPUT_BITS_RESPONSE_SLEN;
    ultohex( hex, *xp );
    copystring( sp, hex );
    do_ascii_message( bindex, s );
    }
}

/***********************************************************************
*                         START_OF_BINARY_DATA                         *
***********************************************************************/
void start_of_binary_data( int i, BINARY_HEADER * bh )
{
if ( Boards[i].binarybuf )
    {
    delete[] Boards[i].binarybuf;
    Boards[i].binarybuf = 0;
    }

Boards[i].current_len     = 0;
Boards[i].chars_left_in_buffer = 0;
switch( bh->data_type )
    {
    case START_OF_BINARY_POS_SAMPLES:
    case START_OF_BINARY_TIME_SAMPLES:
        Boards[i].chars_left_in_buffer = bh->nof_bytes * bh->nof_packets;
        Boards[i].current_type = SAMPLE_TYPE;
        Boards[i].sample_type  = 'P';
        if ( bh->data_type == START_OF_BINARY_TIME_SAMPLES )
            Boards[i].sample_type = 'T';
        break;

    case START_OF_BINARY_PARAMETERS:
        Boards[i].chars_left_in_buffer = bh->nof_bytes * bh->nof_packets;
        Boards[i].current_type = PARAMETERS_TYPE;
        break;

    case START_OF_BINARY_OP_STATUS_DATA:
        Boards[i].current_type = OP_STATUS_TYPE;
        Boards[i].chars_left_in_buffer = sizeof(FTII_OP_STATUS_DATA);
        break;

    case START_OF_BINARY_OSCILLOSCOPE_DATA:
        Boards[i].current_type = OSCILLOSCOPE_TYPE;
        Boards[i].chars_left_in_buffer = sizeof(FTII_OSCILLOSCOPE_DATA);
        break;
    }

if ( Boards[i].chars_left_in_buffer > 0 )
    {
    Boards[i].binarybuf             = new char[Boards[i].chars_left_in_buffer];
    Boards[i].bp                    = Boards[i].binarybuf;
    Boards[i].current_packet_number = 0;
    }
}

/***********************************************************************
*                            NEW_DATA_THREAD                           *
***********************************************************************/
void new_data_thread( void * notused )
{
const DWORD wait_ms = 2000;
static bool finished = false;

STRING_CLASS     s;
SOCKET_MESSAGE * mp;
BINARY_HEADER  * bh;
int              n;
char           * cp;
int              i;
LPARAM           lParam;
WPARAM           wParam;
TIME_CLASS       now;

while ( true )
    {
    if ( WaitForSingleObject(NewDataEvent, wait_ms) == WAIT_FAILED )
        break;

    ResetEvent( NewDataEvent );

    while ( NewData.count() )
        {
        mp = (SOCKET_MESSAGE *) NewData.pop();

        if ( ascii_strings_are_equal(ShutdownMessage, mp->s, ShutdownMessageLen) )
            {
            SocketMessageStore.push( mp );
            mp = 0;
            finished = true;
            break;
            }

        if ( strings_are_equal(ShotSaveCompleteMessage, (TCHAR *)mp->s, ShotSaveCompleteMessageLen) )
            {
            s = (TCHAR *) mp->s;
            SocketMessageStore.push( mp );
            mp = 0;
            s.next_field();
            s.next_field();
            i = board_index( s );
            if ( i != NO_INDEX )
                {
                if ( Boards[i].part.shot_save_complete_wire != NO_WIRE )
                    Boards[i].setwire( Boards[i].part.shot_save_complete_wire );
                }
            break;
            }

        i = board_index( mp->a );
        if ( i != NO_INDEX )
            {
            cp = mp->s;
            if ( *cp == 'B' )
                {
                /*
                -----------
                Binary data
                ----------- */
                bh = (BINARY_HEADER *) cp;
                if ( bh->data_type == START_OF_BINARY_TEXT_DATA )
                    {
                    do_ascii_message( i, mp );
                    }
                else if ( bh->data_type == START_OF_IO_CHANGE_DATA )
                    {
                    convert_io_change_to_ascii( i, mp );
                    }
                else if ( bh->data_type == START_OF_SINGLE_ANALOG_DATA || bh->data_type == START_OF_BLOCK_ANALOG_DATA )
                    {
                    }
                else if ( bh->data_type == CONNECTION_LOST )
                    {
                    /*
                    -----------------------------------------------------------
                    If I loose contact with the board I need to clear my buffer
                    ----------------------------------------------------------- */
                    if ( Boards[i].binarybuf )
                        {
                        delete[] Boards[i].binarybuf;
                        Boards[i].binarybuf = 0;
                        log_network_error_string( TEXT("Board returns after connection lost") );
                        }

                    Boards[i].current_len     = 0;
                    Boards[i].chars_left_in_buffer = 0;
                    Boards[i].current_packet_number = 0;
                    }
                else
                    {
                    if ( bh->packet_num == FIRST_PACKET_NUMBER )
                        start_of_binary_data( i, bh );

                    if ( int(bh->packet_num) == Boards[i].current_packet_number  )
                        {
                        if ( TerminalIsVisible )
                            {
                            s = resource_string( PACKET_STRING );
                            s += Boards[i].current_packet_number;
                            s += resource_string( REPEATED_STRING );
                            s += Boards[i].m.name;
                            MainListBox.add( s.text() );
                            }
                        /*
                        ------------------------------------------------------------------
                        I've already done this one. Mark it with an invalid packet number.
                        ------------------------------------------------------------------ */
                        bh->packet_num = 0;
                        }
                    else if ( int(bh->packet_num) == (Boards[i].current_packet_number+1)  )
                        {
                        n = (int) bh->nof_bytes;
                        if ( n > 0 && Boards[i].chars_left_in_buffer > 0 )
                            {
                            if ( n > Boards[i].chars_left_in_buffer )
                                n = Boards[i].chars_left_in_buffer;
                            cp += sizeof( BINARY_HEADER );
                            memcpy( Boards[i].bp, cp, n );
                            Boards[i].chars_left_in_buffer -= n;
                            Boards[i].current_len     += n;
                            if ( Boards[i].chars_left_in_buffer > 0 )
                                Boards[i].bp += n;
                            }
                        Boards[i].current_packet_number = (int) bh->packet_num;
                        }
                    else if ( int(bh->packet_num) > (Boards[i].current_packet_number+1)  )
                        {
                        if ( TerminalIsVisible )
                            {
                            s = TEXT("**** ");
                            s += resource_string( PACKET_STRING );
                            s += Boards[i].current_packet_number+1;
                            s += resource_string( MISSING_STRING );
                            s += Boards[i].m.name;
                            s += TEXT(" ***" );
                            MainListBox.add( s.text() );
                            log_network_error_string( s );
                            }
                        update_monitor_status( &Boards[i], resource_string(PACKET_ERROR_STRING) );
                        /*
                        -------------------------------------------------------------
                        Make the packet number invalid so I don't do something stupid
                        ------------------------------------------------------------- */
                        bh->packet_num = 0;
                        }

                    if ( bh->packet_num==bh->nof_packets )  // || Boards[i].chars_left_in_buffer==0) )
                        {
                        switch ( Boards[i].current_type )
                            {
                            case SAMPLE_TYPE:
                            case PARAMETERS_TYPE:
                                Boards[i].add_binary_to_shotdata();
                                break;

                            case OP_STATUS_TYPE:
                            case OSCILLOSCOPE_TYPE:
                                if ( i == CurrentBoardIndex )
                                    {
                                    lParam = (LPARAM) Boards[i].binarybuf;
                                    wParam = (WPARAM) Boards[i].current_type;
                                    MainWindow.post( WM_NEW_RT_DATA, wParam, lParam );
                                    }
                                else
                                    {
                                    delete[] Boards[i].binarybuf;
                                    }
                                Boards[i].binarybuf = 0;
                                break;
                            }
                        Boards[i].current_type = ASCII_TYPE;
                        }
                    }
                }
            else
                {
                /*
                ----------
                Ascii data
                ---------- */
                do_ascii_message( i, mp );
                }
            }

        SocketMessageStore.push( mp );
        }

    if ( finished )
        break;
    now.get_local_time();
    check_for_down();
    }

while ( NewData.count() )
    {
    mp = (SOCKET_MESSAGE *) NewData.pop();
    delete mp;
    }

if ( NewDataEvent )
    {
    CloseHandle( NewDataEvent );
    NewDataEvent = 0;
    }
}

/***********************************************************************
*                           GET_CHANNEL_MODE                           *
*  i is the board index, sorc is the channel_mode set string           *
*                                                                      *
*  If I am uploading then I want to change                             *
*  the channel mode to the current channel mode. Otherwise I want to   *
*  change the current mode to the one being sent. This assumes the     *
*  only time the channel mode will be set outside of an upload is by   *
*  toggling the realtime channel 17 checkbox.                          *
***********************************************************************/
void get_channel_mode( int i, char * sorc )
{
int current_mode;
int new_mode;
TCHAR buf[31];
TCHAR * cp;

char_to_tchar( buf, sorc, 30 );
cp = buf + ChannelModeSetStringLen;

new_mode     = asctoint32( cp );
current_mode = Boards[i].channel_mode;

if ( new_mode == current_mode )
    return;

if ( current_mode < CHANNEL_MODE_1 || current_mode > CHANNEL_MODE_7 )
    {
    Boards[i].channel_mode = new_mode;
    return;
    }

if ( new_mode >= CHANNEL_MODE_1 && new_mode <= CHANNEL_MODE_7 )
    {
    if ( Boards[i].uploading )
        {
        sorc += ChannelModeSetStringLen;
        *sorc = '0';
        *sorc += current_mode;
        }
    else
        {
        Boards[i].channel_mode = new_mode;
        }
    }
}

/***********************************************************************
*                              SEND_THREAD                             *
***********************************************************************/
void send_thread( void * notused )
{
const  DWORD POST_NAK_WAIT_MS   = 3000;
const  DWORD CONNECT_TIMEOUT_MS = 3001;
const  DWORD ACK_WAIT_MS        = 1500;
const  DWORD PING_TIMEOUT_MS    = 20000;
const  DWORD SHUTTING_DOWN_MS   = 200;

DWORD wait_ms;
bool  i_am_shutting_down;
bool  an_ft3_thread_is_running;
bool  waiting_for_new_data_shutdown;

ACK_MESSAGE    * ap;
SOCKET_MESSAGE * mp;
DWORD            status;
int              i;
unsigned         delta;
unsigned         ms;
static unsigned  last_check_time;
bool             ok_to_send;   /* Used to trap a start of download when shot is in progress, st only */

wait_ms                       = CONNECT_TIMEOUT_MS;
i_am_shutting_down            = false;
an_ft3_thread_is_running      = false;
waiting_for_new_data_shutdown = false;

ap = 0;
mp = 0;
last_check_time = GetTickCount();
while ( true )
    {
    status = WaitForSingleObject( SendEvent, wait_ms );
    if ( status == WAIT_FAILED )
        break;

    ResetEvent( SendEvent );

    if ( ShuttingDown && !i_am_shutting_down )
        {
        /*
        --------------
        Begin shutdown
        -------------- */
        i_am_shutting_down = true;
        wait_ms            = SHUTTING_DOWN_MS;

        /*
        -------------------------------------
        Tell the UDP read thread to shut down
        ------------------------------------- */
        if ( Read_Thread_Is_Running )
            sendto( MySocket, ShutdownMessage, ShutdownMessageLen+1, 0, (sockaddr *) &MyAddress, sizeof(sockaddr_in) );

        /*
        ----------------------------------
        See if I have to check ft3 threads
        ---------------------------------- */
        an_ft3_thread_is_running = false;
        for ( i=0; i<NofBoards; i++ )
            {
            if ( Boards[i].ft3_send_thread_is_running )
                    {
                    an_ft3_thread_is_running = true;
                    break;
                    }
            }
        }

    if ( MySocket != INVALID_SOCKET && !i_am_shutting_down )
        {
    /*
    -------------------
    Send any acks I owe
    ------------------- */
    while ( AckList.count() )
        {
        ap = (ACK_MESSAGE *) AckList.pop();
        sendto( MySocket, ap->s, AckLen, 0, (sockaddr *) &ap->a, sizeof(sockaddr_in) );
        AckMessageStore.push( ap );
        ap = 0;
        }

    /*
    ---------------------------------------
    Handle any acks or naks I have received
    --------------------------------------- */
    while ( ReceivedAcks.count() )
        {
        mp = (SOCKET_MESSAGE *) ReceivedAcks.pop();
        do_ack_message( mp );
        SocketMessageStore.push( mp );
        mp = 0;
        }

    /*
    ----------------------------------------------------------
    If I timed out and there is anyone still waiting then they
    have timed out.
    ---------------------------------------------------------- */
    ms = GetTickCount();
    delta = 0;
    if ( ms > last_check_time )
        delta = ms - last_check_time;
    if ( (ms < last_check_time) || (delta >= wait_ms ) )
        {
        check_for_timeouts();
        last_check_time = ms;
        }

    wait_ms = CONNECT_TIMEOUT_MS;
    ms      = GetTickCount();
    for ( i=0; i<NofBoards; i++ )
        {
        /*
            ----------------------
            Ignore FasTrak3 Boards
            ---------------------- */
            if ( Boards[i].is_ft3 )
                continue;

            /*
        ----------------------------------------------
        I can't Upload while the shot is in progress
        ---------------------------------------------- */
        if ( Boards[i].do_not_upload )
            {
            if ( Boards[i].do_not_upload_timeout && (ms > Boards[i].do_not_upload_timeout) )
                {
                Boards[i].do_not_upload = false;
                Boards[i].do_not_upload_timeout = 0;
                set_text( Dnu[i], EmptyString );
                }
            }

        if ( Boards[i].do_not_upload && !ShuttingDown )
            wait_ms = ACK_WAIT_MS;

        if ( Boards[i].waiting_for_ack )
            {
            wait_ms = ACK_WAIT_MS;
            }
        else if ( Boards[i].connect_state & NOT_CONNECTED_STATE )
            {
                if ( Boards[i].is_monitoring() )
                {
                /*
                -----------------------------------------------------------
                If I haven't tried in a while, ping it to see if it is back
                ----------------------------------------------------------- */
                if ( ms < Boards[i].ms_at_last_contact )
                    {
                    Boards[i].ms_at_last_contact = ms;
                    }
                else
                    {
                    delta = ms - Boards[i].ms_at_last_contact;
                    if ( delta > PING_TIMEOUT_MS )
                        {
                        Boards[i].set_connect_state( CONNECTING_STATE );
                        Boards[i].ms_at_last_contact = ms;
                        if ( Boards[i].f.count() == 0 )
                            Boards[i].send_socket_message( GetMSLString, true );
                        }
                    }
                }
            }
        else
            {
            if ( Boards[i].f.count() )
                {
                /*
                ---------------------------------------------------------
                I use delta to wait 3 seconds if I have been nak'd.
                This prevents me from banging away when the board is busy
                --------------------------------------------------------- */
                delta = POST_NAK_WAIT_MS;
                if ( Boards[i].nak_count > 0 )
                    {
                    if ( ms > Boards[i].ms_at_last_contact )
                        delta = ms - Boards[i].ms_at_last_contact;
                    }

                /*
                ----------------------------------------------
                I can't upload while the shot is in progress
                ---------------------------------------------- */
                if ( Boards[i].do_not_upload )
                    {
                    if (  Boards[i].do_not_upload_timeout && ( ms > Boards[i].do_not_upload_timeout) )
                        {
                        Boards[i].do_not_upload = false;
                        Boards[i].do_not_upload_timeout = 0;
                        update_monitor_status( &Boards[i], TEXT(" 0 Seconds") );
                        set_text( Dnu[i], EmptyString );
                        }
                    }

                if ( delta >= POST_NAK_WAIT_MS )
                    {
                    mp = 0;
                    while ( mp == 0 )
                        {
                        if ( !Boards[i].f.count() )
                            break;

                        mp = (SOCKET_MESSAGE *) Boards[i].f.peek();
                        ok_to_send = true;

                        if ( start_chars_are_equal(START_OF_UPLOAD, mp->s) )
                            {
                            /*
                            ------------------------------------------------
                            Don't start an upload if the do_not_upload is on
                            ------------------------------------------------ */
                            if ( Boards[i].do_not_upload )
                                {
                                ok_to_send = false;
                                }
                            else
                                {
                                Boards[i].uploading      = true;
                                Boards[i].bytes_uploaded = 0;
                                mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                                SocketMessageStore.push( mp );
                                mp = 0;
                                update_monitor_status( &Boards[i], resource_string(BEGIN_UPLOAD_STRING) );
                                }
                            }
                        else if ( start_chars_are_equal(END_OF_UPLOAD, mp->s) )
                            {
                            Boards[i].uploading      = false;
                            Boards[i].bytes_uploaded = 0;
                            mp = (SOCKET_MESSAGE *) Boards[i].f.pop();
                            SocketMessageStore.push( mp );
                            mp = 0;
                            update_monitor_status( &Boards[i], resource_string(UPLOAD_COMPLETE_STRING) );
                            /*
                            --------------------------------------------------
                            I turn off the realtime updates during upload so I
                            need to turn them back on now.
                            -------------------------------------------------- */
                            if ( Boards[i].start_op_data_after_upload )
                                {
                                Boards[i].send_socket_message( SEND_OP_DATA_PACKETS, true );
                                Boards[i].start_op_data_after_upload = false;
                                }
                            }
                        }

                    if ( mp && ok_to_send )
                        {
                        if ( start_chars_are_equal(ChannelModeSetString, mp->s) )
                            get_channel_mode( i, mp->s );

                        if ( Boards[i].uploading )
                            Boards[i].bytes_uploaded += mp->len;

                        if ( LoggingSentCommands )
                            write_sent_command( Boards[i].number, mp->s );

                        sendto( MySocket, mp->s, mp->len, 0, (sockaddr *) &mp->a, sizeof(sockaddr_in) );
                        Boards[i].waiting_for_ack = true;
                        wait_ms = ACK_WAIT_MS;
                        mp = 0;
                        }
                    }
                }
            else if ( Boards[i].connect_state & CONNECTED_STATE )
                {
                    /*
                    --------------------------------------------------------------------------------
                    If I haven't talked to the board in a while, ping it to see if it is still there
                    -------------------------------------------------------------------------------- */
                    if ( ms < Boards[i].ms_at_last_contact )
                        {
                        Boards[i].ms_at_last_contact = ms;
                        }
                    else
                        {
                        delta = ms - Boards[i].ms_at_last_contact;
                        if ( delta > PING_TIMEOUT_MS )
                            {
                            Boards[i].ms_at_last_contact = ms;
                            Boards[i].send_socket_message( GetMSLString, true );
                            }
                        }
                    }
                }
            }
        }

    if ( !Read_Thread_Is_Running )
        {
        an_ft3_thread_is_running = false;
        for ( i=0; i<NofBoards; i++ )
            {
            if ( Boards[i].ft3_send_thread_is_running )
                {
                an_ft3_thread_is_running = true;
                break;
                }
            }

        if ( an_ft3_thread_is_running )
            continue;

        for ( i=0; i<NofBoards; i++ )
        {
            if ( Boards[i].is_ft3 && Boards[i].ft3_is_connected )
                Boards[i].ft3_disconnect();
            }

        if ( !waiting_for_new_data_shutdown && NewDataEvent != 0 )
            {
            waiting_for_new_data_shutdown = true;
            mp = get_socket_message();
            if ( mp )
                {
                ascii_copystring( mp->s, ShutdownMessage );
                mp->len = ShutdownMessageLen;
                NewData.push( mp );
                SetEvent( NewDataEvent );
            }
            continue;
            }

        if ( NewDataEvent )
            continue;
        else if ( waiting_for_new_data_shutdown )
            break;
        }
    }

if ( MySocket != INVALID_SOCKET )
    {
    closesocket( MySocket );
    MySocket = INVALID_SOCKET;
    }

if ( SendEvent )
    {
    CloseHandle( SendEvent );
    SendEvent = 0;
    }

if ( Boards )
    {
    delete[] Boards;
    Boards    = 0;
    NofBoards = 0;
    }

if ( Wsa_Is_Started )
    {
    WSACleanup();
    Wsa_Is_Started = false;
    }

clean_message_stores();

BoardMonitorIsRunning = false;
}

/***********************************************************************
*                               SEND_ACK                               *
***********************************************************************/
static void send_ack( sockaddr_in & dest )
{
ACK_MESSAGE * ap;

ap = get_ack_message();
ap->a = dest;
AckList.push( ap );
SetEvent( SendEvent );
}

/***********************************************************************
*                             READ_THREAD                              *
***********************************************************************/
void read_thread( void * notused )
{
STRING_CLASS     s;
SOCKET_MESSAGE * mp;
BINARY_HEADER  * bh;
int              alen;
int              errno;
int              i;
char           * cp;

Read_Thread_Is_Running = true;

while ( true )
    {
    mp = get_socket_message();
    alen = sizeof( sockaddr_in );
    mp->len = recvfrom( MySocket, mp->s, MAX_MESSAGE_LEN, 0, (sockaddr *) &mp->a, &alen );

    if ( mp->len == 0 || mp->len == SOCKET_ERROR || !SendEvent )
        {
        if ( mp->len == SOCKET_ERROR )
            {
            errno = WSAGetLastError();
            log_network_error( errno );
            }
        delete mp;
        mp = 0;
        break;
        }

    if ( ascii_strings_are_equal(ShutdownMessage, mp->s, ShutdownMessageLen) )
        {
        delete mp;
        mp = 0;
        break;
        }

    i = board_index( mp->a );
    if ( i != NO_INDEX )
        {
        Boards[i].have_response = true;
        if ( !(Boards[i].connect_state & CONNECTED_STATE) )
            {
            if ( !(Boards[i].connect_state & NOT_MONITORED_STATE) )
                Boards[i].set_connect_state( CONNECTED_STATE );
            }

        Boards[i].ms_at_last_contact = GetTickCount();
        }

    if ( mp->len == AckLen )
        {
        if ( mp->s[0]==AckChar || mp->s[0]==NakChar )
            {
            ReceivedAcks.push( mp );
            SetEvent( SendEvent );
            mp = 0;
            }
        }

    if ( mp )
        {
        if ( mp->len <= 0 )
            {
            SocketMessageStore.push( mp );
            mp = 0;
            }
        else
            {
            cp = mp->s;
            if ( *cp == 'B' )
                {
                /*
                -----------------------------------------------------------------------
                Only send an ack if this is binary data and the DONT_ACK_BIT is not set
                ----------------------------------------------------------------------- */
                bh = (BINARY_HEADER *) cp;
                if ( !(bh->flags & DONT_ACK_BIT) )
                    send_ack( mp->a );
                }
            NewData.push( mp );
            SetEvent( NewDataEvent );
            mp = 0;
            }
        }
    }

Read_Thread_Is_Running = false;
}

/***********************************************************************
*                             START_SOCKET                             *
***********************************************************************/
static void start_socket()
{
const u_short my_port    = 20001;
const WORD SocketVersion = MAKEWORD(2,0);

char      buf[257];
WSADATA   wsadata;
int       status;
HOSTENT * hostname;
in_addr * ap;
STRING_CLASS s;

status =  WSAStartup( SocketVersion, &wsadata );
if ( status != 0 )
    {
    if ( status == WSASYSNOTREADY )
        s = TEXT( "WSASYSNOTREADY" );
    else if ( status == WSAVERNOTSUPPORTED )
        s = TEXT( "WSAVERNOTSUPPORTED" );
    else if ( status == WSAEINPROGRESS )
        s = TEXT( "WSAEINPROGRESS" );
    else if ( status == WSAEPROCLIM )
        s = TEXT( "WSAEPROCLIM" );
    else if ( status == WSAEFAULT )
        s = TEXT( "WSAEFAULT" );
    else if ( status == WSANO_RECOVERY )
        s = TEXT( "WSANO_RECOVERY" );
    else
        s = status;

    MessageBox( 0, TEXT("WSAStartup"), s.text(), MB_OK );
    }
else
    {
    Wsa_Is_Started = true;
    }

if ( Wsa_Is_Started && Have_At_Least_One_FasTrak2 )
    {
    MySocket = socket( PF_INET, SOCK_DGRAM, 0 );
    if ( MySocket == INVALID_SOCKET )
        {
        MessageBox( 0, TEXT("socket"), int32toasc(WSAGetLastError()), MB_OK );
        }
    else
        {
        MyAddress.sin_family           = PF_INET;
        MyAddress.sin_port             = htons( my_port );
        MyAddress.sin_addr.S_un.S_addr = htonl( INADDR_ANY );
        status = bind( MySocket, (sockaddr *) &MyAddress, sizeof(MyAddress) );
        if ( status == SOCKET_ERROR )
            MessageBox( 0, TEXT("bind"), int32toasc(WSAGetLastError()), MB_OK );

        status = gethostname( buf, 256 );
        if ( status != SOCKET_ERROR )
            {
            hostname = gethostbyname( buf );
            ap = (in_addr *)hostname->h_addr_list[0];
            if ( ap )
                {
                MyAddress.sin_addr = *ap;
                }
            }
        }
    }
}

/***********************************************************************
*                            BEGIN_MONITORING                          *
*   The global socket is for talking to the FasTrak2 boards with UDP.  *
*   If there are no FasTrak2 boards the read_thread does not need to   *
*   be run.                                                            *
***********************************************************************/
bool begin_monitoring()
{
unsigned long h;

SendEvent    = CreateEvent( NULL, FALSE, FALSE, NULL );
NewDataEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

StdDown.startup();
start_socket();

h = _beginthread( new_data_thread, 0, NULL );
if ( h == -1 )
    return false;

h = _beginthread( send_thread, 0, NULL );
if ( h == -1 )
    return false;

if ( Have_At_Least_One_FasTrak2 )
    {
h = _beginthread( read_thread, 0, NULL );
if ( h == -1 )
    return false;
    }
BoardMonitorIsRunning = true;
connect_to_boards();

return true;
}

/***********************************************************************
*                            STOP_MONITORING                           *
***********************************************************************/
void stop_monitoring()
{
int i;

disconnect_from_boards();
ShuttingDown = true;
for ( i=0; i<NofBoards; i++ )
    {
    if ( Boards[i].ft3_send_thread_is_running && Boards[i].ft3_send_event != 0 )
        SetEvent( Boards[i].ft3_send_event );
    }
if ( SendEvent != 0 )
    SetEvent( SendEvent );
}

/***********************************************************************
*                             SEND_COMMAND                             *
***********************************************************************/
void send_command( STRING_CLASS & machine_name, TCHAR * command, bool need_to_set_event )
{
int i;
bool is_reset_machine_command;
MACHINE_CLASS      m;
FTII_BOARD_ENTRY * b;

is_reset_machine_command = false;
if ( command )
    is_reset_machine_command = start_chars_are_equal( RESET_MACHINE_CMD, command );

i = board_index( machine_name );
if ( i == NO_INDEX )
    {
    /*
    --------------------------------------------
    See if I am to start monitoring this machine
    -------------------------------------------- */
    if ( is_reset_machine_command )
        {
        if ( m.find(ComputerName, machine_name) )
            {
            if ( m.monitor_flags & MA_MONITORING_ON )
                {
                do_newsetup( machine_name );
                i = board_index( machine_name );
                if ( i != NO_INDEX )
                    update_one_board_display( i, resource_string(NEW_SETUP_STRING) );
                }
            }
        }
    return;
    }

b = Boards + i;

if ( start_chars_are_equal(CLEAR_NO_UPLOAD_FLAG_CMD, command) )
    {
    b->do_not_upload_timeout = 0;
    b->do_not_upload         = false;
    set_text( Dnu[i], EmptyString );
    return;
    }

if ( (b->connect_state & NOT_CONNECTED_STATE) )
    b->set_connect_state( CONNECTING_STATE );

if ( command )
    {
    if ( is_reset_machine_command )
        {
        do_newsetup( machine_name );
        return;
        }
    b->send_socket_message( command, need_to_set_event );

    if ( start_chars_are_equal(StartControlPgmCmd, command) && MonitorWire != NO_WIRE )
        b->setwire( MonitorWire );
    }
}

/***********************************************************************
*                           UPDATE_DOWNTIME                            *
* This is called to update the downtime state from an event from the   *
* popup downtime window.                                               *
***********************************************************************/
void update_downtime( DOWN_DATA_CLASS dd )
{
int ds;
int i;
FTII_BOARD_ENTRY * b;
DSTAT_CLASS d;
bool was_up;

i = board_index( dd.machine_name() );
if ( i == NO_INDEX )
    return;

b = Boards + i;

if ( ( b->connect_state & NOT_MONITORED_STATE) )
    return;

if ( b->down_timeout_seconds < 1 )
    return;

d.set_cat( dd.category() );
d.set_subcat( dd.subcategory() );

if ( b->down_state == d.down_state() )
    return;

if ( ClearDownWireFromPopup && DownTimeAlarmWire != NO_WIRE )
    {
    was_up = false;

    ds = b->down_state;
    if ( ds == PROG_UP_STATE || ds == MACH_UP_STATE || ds == NO_DOWN_STATE )
        was_up = true;

    ds  = d.down_state();

    /*
    ---------------------------------
    If this wire is set then clear it
    --------------------------------- */
    if ( !was_up && ds == HUMAN_DOWN_STATE )
        b->clearwire( DownTimeAlarmWire );
    }

b->down_state = d.down_state();
}

/***********************************************************************
*                        POST_NEW_DATA_MESSAGE                         *
* This is for use with TCHAR messages that will not be sent to a       *
* socket.                                                              *
***********************************************************************/
void post_new_data_message( STRING_CLASS & sorc )
{
SOCKET_MESSAGE * mp;
TCHAR          * cp;

if ( NewDataEvent )
    {
    mp = get_socket_message();
    if ( mp )
        {
        cp = (TCHAR *) mp->s;
        copystring( cp, sorc.text() );
        mp->len = sorc.len();
        NewData.push( mp );
        SetEvent( NewDataEvent );
        }
    }
}
