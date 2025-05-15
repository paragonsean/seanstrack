#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\iniclass.h"
#include "..\include\fifo.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\rectclas.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\structs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

WINDOW_CLASS  DestWin;

const int     MAX_ASCII_LEN = 5000;
const int     PACKET_LEN    = 16;
const unsigned short FIRST_PACKET_NUMBER = 1;

const unsigned char START_OF_BINARY_POS_SAMPLES       = 0;
const unsigned char START_OF_BINARY_TIME_SAMPLES      = 1;
const unsigned char START_OF_BINARY_PARAMETERS        = 2;
const unsigned char START_OF_BINARY_OSCILLOSCOPE_DATA = 3;
const unsigned char START_OF_BINARY_OP_STATUS_DATA    = 4;
const unsigned char START_OF_BINARY_TEXT_DATA         = 5;

const unsigned short DONT_ACK_BIT = 1;

struct BINARY_HEADER {
    char           bchar;
    unsigned char  data_type;
    unsigned short flags;
    unsigned short data_set_num;
    unsigned short packet_num;
    unsigned short nof_packets;
    unsigned short nof_bytes;
    };

const DWORD BINARY_HEADER_LEN = sizeof( BINARY_HEADER );

struct BOARD_ENTRY {
    int          current_type;
    int          current_len;  /* Binary data only */
    int          chars_in_buffer;
    int          current_packet_number;
    char       * bp;
    char       * binarybuf;
    sockaddr_in  a;
    bool         waiting_for_ack;
    int          nak_count;
    BOARD_ENTRY::BOARD_ENTRY();
    BOARD_ENTRY::~BOARD_ENTRY();
    };

BOARD_ENTRY MyBoard;

extern bool          ShuttingDown;

extern HWND          MainEbox;
extern LISTBOX_CLASS MainListBox;
extern char          START_OF_UPLOAD[];
extern char          END_OF_UPLOAD[];
extern STRING_CLASS  CommPort;

static HANDLE       CommPortHandle = INVALID_HANDLE_VALUE;
static HANDLE       NewDataEvent   = 0;
static HANDLE       SendEvent      = 0;
static bool         Uploading      = false;

static FIFO_CLASS   ReceivedAcks;
static FIFO_CLASS   AckList;
static FIFO_CLASS   SendData;
static FIFO_CLASS   NewData;

const char AckChar      = '\006';
const char BChar        = 'B';
const char CrChar       = '\r';
const char LfChar       = '\n';
const char NakChar      = '\025';
const char NullChar     = '\0';
const char PlusChar     = '+';
const char QuestionChar = '?';

static int  PlussesNeeded     = 0;

const TCHAR UnderscoreString[] = TEXT("_");

const int AckLen          = 2;
const int MAX_NAK_COUNT   = 3;
const int MAX_MESSAGE_LEN = 1350;
const unsigned NO_MESSAGE_TYPE     = 0;
const unsigned ASCII_MESSAGE_TYPE  = 1;
const unsigned BINARY_MESSAGE_TYPE = 2;

static char  ACK_MESSAGE[3] = { AckChar, CrChar, '\0' };

class COMM_MESSAGE {
    char      *  s;
    int         len;
    BOOLEAN     need_to_wait_for_ack;

    COMM_MESSAGE() { s = 0; len = 0; }
    ~COMM_MESSAGE();

    void  empty();
    void  operator=( const char * sorc );
    void  operator=( const COMM_MESSAGE * sorc );
    };

static char   AsciiBuf[MAX_ASCII_LEN+1];

/*
------------------------------------------------------------------
These two variables are used by read_thread. ReadBuf is written to
until the entre message is received and then it is moved to a
COMM_MESSAGE and sent to the new_data_thread. Rbp points to the
next write location in the ReadBuf.
------------------------------------------------------------------ */
static char * ReadBuf = 0;
static char * Rbp     = 0;

/*
-------------------------------------------
These two variables are used by send_thread
------------------------------------------- */
COMM_MESSAGE * CurrentMessage = 0;
char         * CurrentMp;

BOOLEAN        TxCommBufIsEmpty  = TRUE;
BOOLEAN        RxCommBufHasChars = FALSE;

/***********************************************************************
*                      COMM_MESSAGE::COMM_MESSAGE                      *
***********************************************************************/
void COMM_MESSAGE::COMM_MESSAGE()
{
s   = 0;
len = 0;
need_to_wait_for_ack = FALSE;
}

/***********************************************************************
*                         COMM_MESSAGE::EMPTY                          *
***********************************************************************/
void COMM_MESSAGE::empty()
{
char * copy;

if ( s )
    {
    copy = s;
    s = 0;
    delete[] copy;
    }
len = 0;
need_to_wait_for_ack = FALSE;
}

/***********************************************************************
*                     COMM_MESSAGE::~COMM_MESSAGE                      *
***********************************************************************/
void COMM_MESSAGE::~COMM_MESSAGE()
{
empty();
}

/***********************************************************************
*                       COMM_MESSAGE::OPERATOR=                        *
***********************************************************************/
void COMM_MESSAGE::operator=( const char * sorc )
{
empty();
if ( sorc )
    {
    len = lstrlen( sorc );
    if ( len > 0 )
        {
        s = maketext( len );
        if ( s )
            lstrcpy( s, sorc );
        else
            len = 0;
        }
    }
}

/***********************************************************************
*                       COMM_MESSAGE::OPERATOR=                        *
***********************************************************************/
void COMM_MESSAGE::operator=( const COMM_MESSAGE * sorc )
{
empty();
if ( sorc )
    {
    len = sorc->len;
    if ( len > 0 )
        {
        s = maketext( len );
        if ( s )
            lstrcpy( s, sorc->s );
        else
            len = 0;
        }
    need_to_wait_for_ack = sorc->need_to_wait_for_ack;
    }
}

/***********************************************************************
*                           COPY_COMM_MESSAGE                          *
***********************************************************************/
COMM_MESSAGE * copy_comm_message( const COMM_MESSAGE * sorc )
{
COMM_MESSAGE * dest;
dest = new COMM_MESSAGE;
if (dest )
    {
    *dest = sorc;
    return dest;
    }

return 0;
}

/***********************************************************************
*                              BOARD_ENTRY                             *
***********************************************************************/
BOARD_ENTRY::BOARD_ENTRY()
{
current_type       = ASCII_TYPE;
current_len        = 0;
chars_in_buffer    = 0;
bp                 = 0;
binarybuf          = 0;

nak_count          = 0;
waiting_for_ack    = false;
}

/***********************************************************************
*                              BOARD_ENTRY                             *
***********************************************************************/
BOARD_ENTRY::~BOARD_ENTRY()
{
if ( binarybuf )
    {
    delete[] binarybuf;
    binarybuf = 0;
    }
}

/***********************************************************************
*                        START_CHARS_ARE_EQUAL                         *
***********************************************************************/
static bool start_chars_are_equal( const char * sorc, const char * s )
{
while ( true )
    {
    if ( *sorc == NullChar )
        return true;

    if ( *sorc != *s )
        return false;

    sorc++;
    s++;
    }
}

/***********************************************************************
*                             SEND_COMM_MESSAGE                        *
***********************************************************************/
bool send_comm_message( char * sorc, bool need_to_set_event )
{
COMM_MESSAGE * mp;

if ( ShuttingDown )
    return false;

mp = new COMM_MESSAGE;

if ( mp )
    {
    *mp = sorc;

    SendData.push( mp );
    if ( need_to_set_event )
        SetEvent( SendEvent );

    return true;
    }

return false;
}

/***********************************************************************
*                             post_data                                *
*                                                                      *
* I create a buffer and a struct to hold the info and send them        *
* all to the recv_window, who is responsible for deleting the          *
* buffers when finished with them.                                     *
*                                                                      *
***********************************************************************/
void post_data()
{
NEW_FTII_DATA * nd;
char          * cp;
int             len;

nd = new NEW_FTII_DATA;
if ( nd )
    {
    nd->sorc = 1;
    nd->type = MyBoard.current_type;
    if ( MyBoard.current_type == ASCII_TYPE )
        {
        len = lstrlen( AsciiBuf );
        if ( len > 0 )
            {
            cp = new char[len+1];
            if ( cp )
                {
                memcpy( cp, AsciiBuf, len );
                *(cp+len) = '\0';
                nd->buf = cp;
                nd->len = len;
                }
            }
        else
            {
            delete nd;
            nd = 0;
            }
        }
    else
        {
        /*
        ------------------------------------------------------------------------------
        This is binary data. Send it to the main window and reset myself to ascii mode
        ------------------------------------------------------------------------------ */
        if ( MyBoard.binarybuf && MyBoard.current_len > 0 )
            {
            nd->buf     = MyBoard.binarybuf;
            nd->len     = MyBoard.current_len;
            MyBoard.binarybuf = 0;
            }
        else
            {
            delete nd;
            nd = 0;
            }

        MyBoard.current_type = ASCII_TYPE;
        MyBoard.bp           = AsciiBuf;
        }

    if ( nd )
        DestWin.post( WM_NEW_DATA, 0, (LPARAM) nd );
    }
}

/***********************************************************************
*                           DO_ASCII_MESSAGE                           *
***********************************************************************/
void do_ascii_message( COMM_MESSAGE * mp )
{
int  n;
char * sorc;
BINARY_HEADER * bh;
bool   need_to_post;

need_to_post = true;
MyBoard.current_type = ASCII_TYPE;

sorc = mp->s;
if ( *sorc == BChar )
    {
    bh = (BINARY_HEADER *) sorc;
    sorc += sizeof( BINARY_HEADER );
    n  = (int) bh->nof_bytes;
    if ( n < 1 )
        return;
    }
else
    {
    n = mp->len;
    }

if ( Uploading && *sorc==PlusChar )
    {
    PlussesNeeded--;
    if ( PlussesNeeded == 0 )
        SetEvent( SendEvent );
    need_to_post = false;
    }

memcpy( AsciiBuf, sorc, n );
*(AsciiBuf+n) = NullChar;

/*
-----------------------------------------
Post data doesn't affect the ascii buffer
----------------------------------------- */
if ( need_to_post )
    post_data();
}

/***********************************************************************
*                         POST_BEGIN_MESSAGE                           *
***********************************************************************/
void post_begin_message( BINARY_HEADER * bh )
{
TCHAR s[100];
NEW_FTII_DATA * nd;
TCHAR * cp;

cp = 0;
switch( bh->data_type )
    {
    case START_OF_BINARY_POS_SAMPLES:
        cp = copystring( s, TEXT( "P_BEGIN_") );
        break;

    case START_OF_BINARY_TIME_SAMPLES:
        cp = copystring( s, TEXT( "T_BEGIN_") );
        break;

    case START_OF_BINARY_PARAMETERS:
        cp = copystring( s, TEXT( "C_BEGIN_") );
        break;
    }

if ( cp )
    {
    nd = new NEW_FTII_DATA;
    if ( nd )
        {
        nd->sorc = 1;
        nd->type = ASCII_TYPE;
        cp = copystring( cp, int32toasc((int32) bh->data_set_num) );
        cp = copystring( cp, UnderscoreString );
        cp = copystring( cp, int32toasc((int32) MyBoard.current_len) );

        nd->buf = maketext( s );
        nd->len = lstrlen( s );
        DestWin.post( WM_NEW_DATA, 0, (LPARAM) nd );
        }
    }
}

/***********************************************************************
*                         START_OF_BINARY_DATA                         *
***********************************************************************/
void start_of_binary_data( BINARY_HEADER * bh )
{
if ( MyBoard.binarybuf )
    {
    delete[] MyBoard.binarybuf;
    MyBoard.binarybuf = 0;
    }

MyBoard.current_len     = 0;
MyBoard.chars_in_buffer = 0;
switch( bh->data_type )
    {
    case START_OF_BINARY_POS_SAMPLES:
    case START_OF_BINARY_TIME_SAMPLES:
        MyBoard.chars_in_buffer = bh->nof_bytes * bh->nof_packets;
        MyBoard.current_type = SAMPLE_TYPE;
        break;

    case START_OF_BINARY_PARAMETERS:
        MyBoard.chars_in_buffer = bh->nof_bytes * bh->nof_packets;
        MyBoard.current_type = PARAMETERS_TYPE;
        break;

    case START_OF_BINARY_OP_STATUS_DATA:
        MyBoard.current_type = OP_STATUS_TYPE;
        MyBoard.chars_in_buffer = sizeof(FTII_OP_STATUS_DATA);
        break;

    case START_OF_BINARY_OSCILLOSCOPE_DATA:
        MyBoard.current_type = OSCILLOSCOPE_TYPE;
        MyBoard.chars_in_buffer = sizeof(FTII_OSCILLOSCOPE_DATA);
        break;
    }

if ( MyBoard.chars_in_buffer > 0 )
    {
    MyBoard.binarybuf             = new char[MyBoard.chars_in_buffer];
    MyBoard.bp                    = MyBoard.binarybuf;
    MyBoard.current_packet_number = 0;
    }
}

/***********************************************************************
*                            NEW_DATA_THREAD                           *
***********************************************************************/
DWORD new_data_thread( int * notused )
{
const DWORD wait_ms = INFINITE;

STRING_CLASS    s;
COMM_MESSAGE  * mp;
BINARY_HEADER * bh;
int             n;
char          * cp;

while ( true )
    {
    if ( WaitForSingleObject(NewDataEvent, wait_ms) == WAIT_FAILED )
        break;

    if ( ShuttingDown )
        break;

    while ( NewData.count() )
        {
        mp = (COMM_MESSAGE *) NewData.pop();
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
                do_ascii_message( mp );
                }
            else
                {
                if ( bh->packet_num == FIRST_PACKET_NUMBER )
                    start_of_binary_data( bh );

                if ( int(bh->packet_num) == MyBoard.current_packet_number  )
                    {
                    s = TEXT("Packet " );
                    s += MyBoard.current_packet_number;
                    s += TEXT(" repeated" );
                    MainListBox.add( s.text() );
                    }
                if ( int(bh->packet_num) == (MyBoard.current_packet_number+1)  )
                    {
                    n = (int) bh->nof_bytes;
                    if ( n > 0 && MyBoard.chars_in_buffer > 0 )
                        {
                        if ( n > MyBoard.chars_in_buffer )
                            n = MyBoard.chars_in_buffer;
                        cp += sizeof( BINARY_HEADER );
                        memcpy( MyBoard.bp, cp, n );
                        MyBoard.chars_in_buffer -= n;
                        MyBoard.current_len     += n;
                        if ( MyBoard.chars_in_buffer > 0 )
                            MyBoard.bp += n;
                        }
                    }
                else if ( int(bh->packet_num) > (MyBoard.current_packet_number+1)  )
                    {
                    s = TEXT("**** Packet " );
                    s += MyBoard.current_packet_number+1;
                    s += TEXT(" missing ***" );
                    MainListBox.add( s.text() );
                    }

                MyBoard.current_packet_number = (int) bh->packet_num;

                if ( bh->packet_num==bh->nof_packets || MyBoard.chars_in_buffer==0 )
                    {
                    post_begin_message( bh );
                    post_data();
                    }
                }
            }
        else
            {
            /*
            ----------
            Ascii data
            ---------- */
            do_ascii_message( mp );
            }

        delete mp;
        }
    }

return 0;
}

/***********************************************************************
*                            DO_ACK_MESSAGE                            *
*           I have recieved an ack or nak from the board.              *
***********************************************************************/
static void do_ack_message( COMM_MESSAGE * mp )
{
COMM_MESSAGE * dead_mp;

if ( !MyBoard.waiting_for_ack )
    return;

if ( mp->s[0] == NakChar )
    {
    MainListBox.add( "*** Nak Received ***" );
    MyBoard.nak_count++;
    if ( MyBoard.nak_count < MAX_NAK_COUNT )
        {
        MyBoard.waiting_for_ack = false;
        return;
        }
    }

dead_mp = (COMM_MESSAGE *) SendData.pop();
delete dead_mp;

MyBoard.waiting_for_ack = false;
MyBoard.nak_count       = 0;
}

/***********************************************************************
*                          CHECK_FOR_TIMEOUTS                          *
*    This just nak's any board that is waiting for an ack or nak.      *
***********************************************************************/
static void check_for_timeouts()
{
COMM_MESSAGE * dead_mp;

if ( MyBoard.waiting_for_ack )
    {
    MyBoard.nak_count++;
    if ( MyBoard.nak_count < MAX_NAK_COUNT )
        {
        MainListBox.add( "*** TimeOut Resend ***" );
        MyBoard.waiting_for_ack = false;
        }
    else
        {
        dead_mp = (COMM_MESSAGE *) SendData.pop();
        delete dead_mp;

        MyBoard.waiting_for_ack = false;
        MyBoard.nak_count       = 0;
        }
    }
}

/***********************************************************************
*                           SEND_ONE_PACKET                            *
* This is the only routine that writes to the comm port. If I write    *
* any bytes I return TRUE.                                             *
* I am the only one who can set TxCommBufIsEmpty to FALSE.             *
***********************************************************************/
BOOLEAN send_one_packet()
{
DWORD bytes_to_write;
DWORD bytes_written;

if ( !TxCommBufIsEmpty )
    return FALSE;

if ( !CurrentMp )
    CurrentMp = CurrentMessage.s;

if ( *CurrentMp == NullChar )
    {
    CurrentMp = 0;
    delete CurrentMessage;
    CurrentMessage = 0;
    return FALSE;
    }

bytes_to_write = (DWORD)  lstrlen( CurrentMp );
if ( bytes_to_write > PACKET_LEN )
    bytes_to_write = PACKET_LEN;

TxCommBufIsEmpty = FALSE;
WriteFile( CommPortHandle, CurrentMp, (DWORD) slen, &bytes_written, 0 );

CurrentMp += bytes_to_write;
if ( *CurrentMp == NullChar )
    {
    CurrentMp = 0;
    if ( CurrentMessage->need_to_wait_for_ack )
        {
        MyBoard.waiting_for_ack = true;
        wait_ms = ACK_WAIT_MS;
        }
    delete CurrentMessage;
    CurrentMessage = 0;
    }

return TRUE;
}

/***********************************************************************
*                              SEND_THREAD                             *
***********************************************************************/
DWORD send_thread( int * notused )
{
const  DWORD ACK_WAIT_MS = 1500;
static DWORD wait_ms = INFINITE;
static int   bytes_uploaded = 0;

ACK_MESSAGE  * ap;
COMM_MESSAGE * mp;
DWORD          status;

ap = 0;
mp = 0;
while ( true )
    {
    status = WaitForSingleObject( SendEvent, wait_ms );
    if ( status == WAIT_FAILED )
        break;

    if ( ShuttingDown )
        break;

    /*
    ----------------------------------------------------------------
    I can't do anything until the serial port output buffer is empty
    ---------------------------------------------------------------- */
    if ( !TxCommBufIsEmpty )
        continue;

    /*
    ---------------------------------------
    See if I am currently sending a message
    --------------------------------------- */
    if ( CurrentMessage )
        {
        if ( send_one_packet() )
            continue;
        }

    /*
    -------------------
    Send any acks I owe
    ------------------- */
    if ( AckList.count() )
        {
        CurrentMessage = (COMM_MESSAGE *) AckList.pop();
        if ( send_one_packet() )
            continue;
        }

    /*
    ---------------------------------------
    Handle any acks or naks I have received
    --------------------------------------- */
    while ( ReceivedAcks.count() )
        {
        mp = (COMM_MESSAGE *) ReceivedAcks.pop();
        do_ack_message( mp );
        delete mp;
        mp = 0;
        }

    /*
    ----------------------------------------------------------
    If I timed out and there is anyone still waiting then they
    have timed out.
    ---------------------------------------------------------- */
    if ( status == WAIT_TIMEOUT )
        check_for_timeouts();

    wait_ms = INFINITE;
    if ( MyBoard.waiting_for_ack )
        {
        wait_ms = ACK_WAIT_MS;
        }
    else if ( PlussesNeeded < 1 )
        {
        if ( SendData.count() )
            {
            mp = (COMM_MESSAGE *) SendData.peek();

            if ( strings_are_equal(START_OF_UPLOAD, mp->s) )
                {
                Uploading = true;
                bytes_uploaded = 0;
                mp = (COMM_MESSAGE *) SendData.pop();
                delete mp;
                mp = 0;
                MainListBox.add( TEXT("Begin Upload...") );
                }
            else if ( strings_are_equal(END_OF_UPLOAD, mp->s) )
                {
                Uploading = false;
                bytes_uploaded = 0;
                mp = (COMM_MESSAGE *) SendData.pop();
                delete mp;
                mp = 0;
                MainListBox.add( TEXT("Upload Complete") );
                DestWin.post( WM_DB_SAVE_DONE, 0, 0 );
                }

            /*
            --------------------------------------------------------------------------------
            If I get there and mp is null then I must have just received a file start or end
            -------------------------------------------------------------------------------- */
            if ( !mp && SendData.count() )
                mp = (COMM_MESSAGE *) SendData.peek();

            if ( mp )
                {
                if ( Uploading )
                    {
                    PlussesNeeded = 1;
                    bytes_uploaded += mp->len;
                    set_text( MainEbox, int32toasc((int32)bytes_uploaded) );
                    }
                CurrentMessage = copy_comm_message( mp );
                if ( !Broadcasting )
                    CurrentMessage->need_to_wait_for_ack = TRUE;
                send_one_packet();
                if ( Broadcasting )
                    {
                    mp = (COMM_MESSAGE *) SendData.pop();
                    delete mp;
                    }
                mp = 0;
                }
            }
        }
    }

return 0;
}

/***********************************************************************
*                               SEND_ACK                               *
***********************************************************************/
static void send_ack( sockaddr_in & dest )
{
ACK_MESSAGE * ap;

ap = new ACK_MESSAGE;
ap->a = dest;
AckList.push( ap );
SetEvent( SendEvent );
}

/***********************************************************************
*                             READ_THREAD                              *
*                                                                      *
* There are two types of messages. Ascii messages do not begin with    *
* 'B' and end with a line feed. Binary messages begin with the letter  *
* 'B' and start with a binary header.                                  *
***********************************************************************/
DWORD read_thread( int * notused )
{
static unsigned message_type = NO_MESSAGE_TYPE;
static DWORD    total_bytes_read_so_far = 0;
static DWORD    total_bytes_needed      = 0;  //This is only used for binary messages after the header is received

DWORD           bytes_read;
DWORD           bytes_to_read;
COMSTAT         cs;
DWORD           error_codes;
STRING_CLASS    s;
COMM_MESSAGE  * mp;
int             alen;
BINARY_HEADER * bh;
char          * cp;
bool            is_first_packet;

while ( true )
    {
    if ( ShuttingDown )
        break;

    is_first_packet = false;
    bytes_to_read = 0;
    if ( ClearCommError( CommPortHandle, &error_codes, &cs) )
        bytes_to_read = cs.cbInQue;

    if ( bytes_to_read == 0 )
        continue;

    if ( !ReadBuf )
        {
        is_first_packet = true;
        total_bytes_read_so_far = 0;
        total_bytes_needed      = 0;
        ReadBuf = new char[MAX_MESSAGE_LEN];
        Rbp = ReadBuf;
        }

    if ( !ReadBuf )
        continue;

    if ( !ReadFile(CommPortHandle, Rbp, bytes_to_read, bytes_read, 0) )
        continue;

    if ( bytes_read != bytes_to_read )
        {
        s = TEXT( "bytes read = " );
        s += (unsigned int) bytes_read;
        s += TEXT( " bytes_to_read = " );
        s += (unsigned int) bytes_to_read;
        MessageBox( 0, s.text(), "Comm Port Read Error", MB_OK | MB_SYSTEMMODAL );
        if ( bytes_read == 0 )
            continue;
        }

    total_bytes_read_so_far += bytes_read;

    if ( is_first_packet )
        {
        /*
        -----------------------------------------
        See if this is an ascii or binary message
        ----------------------------------------- */
        if ( *ReadBuf == BChar )
            message_type = BINARY_MESSAGE_TYPE;
        else
            message_type = ASCII_MESSAGE_TYPE;
        }

    if ( (message_type == BINARY_MESSAGE_TYPE) && (total_bytes_needed == 0) )
        {
        if ( total_bytes_read_so_far > = BINARY_HEADER_LEN )
            {
            bh = (BINARY_HEADER *) ReadBuf;
            total_bytes_needed = BINARY_HEADER_LEN + bh->nof_bytes;
            }
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
        if ( mp->len > 0 )
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
        else
            {
            delete mp;
            mp = 0;
            break;
            }
        }
    }

return 0;
}

/***********************************************************************
*                           OPEN_COMM_PORT                             *
***********************************************************************/
static void open_comm_port()
{
DCB     dcb;
BOOLEAN status;

CommPortHandle = CreateFile( MyCommPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( CommPortHandle == INVALID_HANDLE_VALUE )
    {
    status = GetCommState( CommPortHandle, &dcb );
    }

NewDataEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
SendEvent    = CreateEvent( NULL, FALSE, FALSE, NULL );
CommEvent    = CreateEvent( NULL, FALSE, FALSE, NULL );
}

/***********************************************************************
*                           BEGIN_MONITORING                           *
***********************************************************************/
bool begin_monitoring( WINDOW_CLASS & notification_window )
{
HANDLE h;
DWORD  id;

DestWin = notification_window;

start_socket();

h = _beginthread( new_data_thread, 0, NULL );
if ( h == -1 )
    return false;

h = _beginthread( send_thread, 0, NULL );
if ( h == -1 )
    return false;

h = _beginthread( read_thread, 0, NULL );
if ( h == -1 )
    return false;

return true;
}

/***********************************************************************
*                        SET_SOCKET_SEND_EVENT                         *
***********************************************************************/
void set_socket_send_event()
{
SetEvent( SendEvent );
}

/***********************************************************************
*                            STOP_MONITORING                           *
***********************************************************************/
void stop_monitoring()
{
COMM_MESSAGE * mp;
ACK_MESSAGE    * ap;

if ( CommPortHandle != INVALID_HANDLE_VALUE )
    {
    CloseHandle( CommPortHandle );
    CommPortHandle = INVALID_HANDLE_VALUE;
    }

if ( SendEvent )
    {
    CloseHandle( SendEvent );
    SendEvent = 0;
    }

if ( NewDataEvent )
    {
    CloseHandle( NewDataEvent );
    NewDataEvent = 0;
    }

while ( SendData.count() )
    {
    mp = (COMM_MESSAGE *) SendData.pop();
    delete mp;
    }

while ( ReceivedAcks.count() )
    {
    mp = (COMM_MESSAGE *) ReceivedAcks.pop();
    delete mp;
    }

while ( AckList.count() )
    {
    ap = (ACK_MESSAGE *) AckList.pop();
    delete ap;
    }
}
