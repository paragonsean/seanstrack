#include <windows.h>
#include <commctrl.h>
#include <process.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS MainDialog;

static TCHAR * StopBitChoice[] =
    {
    { TEXT("1")   },
    { TEXT("1.5") },
    { TEXT("2")   }
    };
static const int NofStopBitChoices = 3;

static TCHAR ParityChoice[] = TEXT( "NOEMS" );
static const int NofParityChoices = 5;

static TCHAR MyClassName[]           = TEXT( "SerialIn" );
static TCHAR MonallIniFile[]         = TEXT( "monall.ini" );
static TCHAR SerialInIniFile[]       = TEXT( "serialin.ini" );
static TCHAR ConfigSection[]         = TEXT( "Config"     );
static TCHAR EosComBitsKey[]         = TEXT( "EosComBits" );
static TCHAR EosComCtsKey[]          = TEXT( "EosComCts" );
static TCHAR EosComDelimitCharKey[]  = TEXT( "EosComDelimitChar" );
static TCHAR EosComDsrKey[]          = TEXT( "EosComDsr" );
static TCHAR EosComEolKey[]          = TEXT( "EosComEol"  );
static TCHAR EosComParityKey[]       = TEXT( "EosComParity" );
static TCHAR EosComPortKey[]         = TEXT( "EosComPort" );
static TCHAR EosComSetupKey[]        = TEXT( "EosComSetup" );
static TCHAR EosComSpeedKey[]        = TEXT( "EosComSpeed" );
static TCHAR EosHaveAlarmShotKey[]   = TEXT( "EosHaveAlarmShot" );
static TCHAR EosHaveGoodShotKey[]    = TEXT( "EosHaveGoodShot" );
static TCHAR EosHaveWarningShotKey[] = TEXT( "EosHaveWarningShot" );
static TCHAR EosComStopKey[]         = TEXT( "EosComStop" );
static TCHAR EosComXonXoffKey[]      = TEXT( "EosComXonXoff" );
static TCHAR EosFixedWidthKey[]      = TEXT( "EosFixedWidth" );
static TCHAR EosNeedsStxEtxKey[]     = TEXT( "EosNeedsStxEtx" );
static TCHAR NullCharString[]        = TEXT( "\\0" );
static TCHAR TimerMsKey[]            = TEXT( "TimerMs" );

static TCHAR AChar         = TEXT( 'A' );
static TCHAR BackslashChar = TEXT( '\\' );
static TCHAR CommaChar     = TEXT( ',' );
static TCHAR CrChar        = TEXT( '\r' );
static TCHAR CChar         = TEXT( 'C' );
static TCHAR DChar         = TEXT( 'D' );
static TCHAR DoubleQuoteChar = TEXT( '\"' );
static TCHAR GChar         = TEXT( 'G' );
static TCHAR GTChar        = TEXT( '>' );
static TCHAR JChar         = TEXT( 'J' );
static TCHAR LChar         = TEXT( 'L' );
static TCHAR LfChar        = TEXT( '\n' );
static TCHAR LowerNChar    = TEXT( 'n' );
static TCHAR LowerRChar    = TEXT( 'r' );
static TCHAR LowerTChar    = TEXT( 't' );
static TCHAR LTChar        = TEXT( '<' );
static TCHAR MChar         = TEXT( 'M' );
static TCHAR NineChar      = TEXT( '9' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR OneChar       = TEXT( '1' );
static TCHAR PChar         = TEXT( 'P' );
static TCHAR QChar         = TEXT( 'Q' );
static TCHAR RChar         = TEXT( 'R' );
static TCHAR SChar         = TEXT( 'S' );
static TCHAR SpaceChar     = TEXT( ' ' );
static TCHAR TabChar       = TEXT( '\t' );
static TCHAR TChar         = TEXT( 'T' );
static TCHAR TwoChar       = TEXT( '2' );
static TCHAR YChar         = TEXT( 'Y' );
static TCHAR WChar         = TEXT( 'W' );
static TCHAR ZeroChar      = TEXT( '0' );

static char EtxBuf[]   = "\003";
static char StxBuf[]   = "\002";

STRING_CLASS EosComBits;
STRING_CLASS EosComDelimitChar;
STRING_CLASS EosComEol;
STRING_CLASS EosComParity;
STRING_CLASS EosComPort;
STRING_CLASS EosComSetup;
STRING_CLASS EosComSpeed;
STRING_CLASS EosComStop;
STRING_CLASS EosFixedWidth;
STRING_CLASS EosHaveAlarmShot;
STRING_CLASS EosHaveWarningShot;
STRING_CLASS EosHaveGoodShot;

BOOLEAN EosComCts      = FALSE;
BOOLEAN EosComDsr      = FALSE;
BOOLEAN EosComXonXoff  = FALSE;
BOOLEAN EosNeedsStxEtx = FALSE;

int EosComParityIndex = 0;
int EosComBitsIndex   = 0;
int EosComStopIndex   = 0;

bool   Stopping = false;
HANDLE CommPortHandle;
LISTBOX_CLASS MessageLb;

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                            LISTEN_THREAD                             *
***********************************************************************/
void listen_thread( void * notused )
{
STRING_CLASS buf;
DWORD bytes_read;
DWORD bytes_to_read;
char    c;

buf.null();
while ( true )
    {
    bytes_to_read = 1;
    if ( !ReadFile(CommPortHandle, &c, bytes_to_read, &bytes_read, NULL) )
        {
        MessageBox( MainDialog.handle(), "ReadFile", "Error", MB_OK );
        return;
        }

    if ( bytes_read != bytes_to_read )
        {
        if ( Stopping )
            return;
        else
            continue;
        }

    if ( c == CrChar || c == LfChar )
        {
        MessageLb.add( buf.text() );
        buf.null();
        }
    else
        {
        buf += c;
        }
    }
}

/***********************************************************************
*                           BEGIN_LISTENING                            *
***********************************************************************/
static bool begin_listening()
{
COMMTIMEOUTS timeouts;
unsigned long h;

CommPortHandle = CreateFile( EosComPort.text(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( CommPortHandle == INVALID_HANDLE_VALUE )
    return false;

timeouts.ReadIntervalTimeout = 0;
timeouts.ReadTotalTimeoutMultiplier = 0;
timeouts.ReadTotalTimeoutConstant = 500;
timeouts.WriteTotalTimeoutMultiplier = 0;
timeouts.WriteTotalTimeoutConstant = 0;

SetCommTimeouts( CommPortHandle, &timeouts );
Stopping = false;

h = _beginthread( listen_thread, 0, NULL );
if ( h == -1 )
    return false;

return true;
}

/***********************************************************************
*                            STOP_LISTENING                            *
***********************************************************************/
static void stop_listening()
{
Stopping = true;
Sleep( 500 );
if ( CommPortHandle != INVALID_HANDLE_VALUE )
    {
    CloseHandle( CommPortHandle );
    CommPortHandle = INVALID_HANDLE_VALUE;
    }
}

/***********************************************************************
*                          CONVERT_CONTROL_CHARS                       *
*                This is save to use on STRING_CLASS vars.             *
***********************************************************************/
static void convert_control_chars( TCHAR * sorc )
{
TCHAR * cp;

if ( !sorc )
    return;

while ( *sorc != NullChar )
    {
    if ( *sorc == BackslashChar )
        {
        cp = sorc;
        cp++;
        if ( *cp == LowerRChar )
            {
            *sorc = CrChar;
            remove_char( cp );
            }
        else if ( *cp == LowerNChar )
            {
            *sorc = LfChar;
            remove_char( cp );
            }
        else if ( *cp == LowerTChar )
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
*                         SET_COM_PORT_INDEXES                         *
***********************************************************************/
static void set_com_port_indexes()
{
int i;
TCHAR c;

i = EosComBits.int_value();
if ( i < 7 || i > 8 )
    i = 8;
EosComBitsIndex = i - 7;

EosComStopIndex   = 0;
for ( i=0; i<NofStopBitChoices; i++ )
    {
    if ( EosComStop == StopBitChoice[i] )
        {
        EosComStopIndex = i;
        break;
        }
    }

EosComParityIndex = 0;
c = *EosComParity.text();
for ( i=0; i<NofParityChoices; i++ )
    {
    if ( c == ParityChoice[i] )
        {
        EosComParityIndex = i;
        break;
        }
    }
}

/***********************************************************************
*                      WRITE_BYTES_TO_SERIAL_PORT                      *
***********************************************************************/
static BOOLEAN write_bytes_to_serial_port( void * sorc, DWORD bytes_to_write )
{
HANDLE  fh;
DWORD   bytes_written;
STRING_CLASS msg;
STRING_CLASS s;

bytes_written  = 0;

fh = CreateFile( EosComPort.text(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh != INVALID_HANDLE_VALUE )
    {
    WriteFile( fh, sorc, bytes_to_write, &bytes_written, 0 );
    CloseHandle( fh );
    }

if ( bytes_written != bytes_to_write )
    {
    s = TEXT( "Error:bytes to write = " );
    s += unsigned( bytes_to_write );
    msg = TEXT( "bytes written = " );
    msg += unsigned( bytes_written );
    MessageBox( 0, s.text(), msg.text(), MB_OK );
    return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                            SAVE_SETTINGS                             *
***********************************************************************/
static void save_settings( STRING_CLASS & s )
{
INI_CLASS ini;

ini.set_file( s.text() );
ini.set_section( ConfigSection );

ini.put_string( EosComBitsKey,        EosComBits.text()        );
ini.put_string( EosComDelimitCharKey, EosComDelimitChar.text() );
ini.put_string( EosComEolKey,         EosComEol.text()         );
ini.put_string( EosComParityKey,      EosComParity.text()      );
ini.put_string( EosComPortKey,        EosComPort.text()        );
ini.put_string( EosComSetupKey,       EosComSetup.text()       );
ini.put_string( EosComSpeedKey,       EosComSpeed.text()       );
ini.put_string( EosComStopKey,        EosComStop.text()        );
ini.put_string( EosFixedWidthKey,     EosFixedWidth.text()     );

ini.put_boolean( EosNeedsStxEtxKey, EosNeedsStxEtx );
ini.put_boolean( EosComCtsKey,      EosComCts );
ini.put_boolean( EosComDsrKey,      EosComDsr );
ini.put_boolean( EosComXonXoffKey,  EosComXonXoff );

s = exe_directory();
s.cat_path( SerialInIniFile );
ini.set_file( s.text() );
}

/***********************************************************************
*                         SAVE_SETTINGS_TO_MONALL                      *
***********************************************************************/
static void save_settings_to_monall()
{
STRING_CLASS s;
s = TEXT( "C:\\V5\\Exes" );
s.cat_path( MonallIniFile );

if ( file_exists(s.text()) )
    save_settings( s );
}

/***********************************************************************
*                    SAVE_SETTINGS_TO_SERIALTEST                       *
***********************************************************************/
static void save_settings_to_serialtest()
{
STRING_CLASS s;

s = exe_directory();
s.cat_path( SerialInIniFile );
if ( file_exists(s.text()) )
    save_settings( s );
}

/***********************************************************************
*                       GET_SETTINGS_FROM_SCREEN                       *
***********************************************************************/
static void get_settings_from_screen()
{
int i;
int id;
STRING_CLASS s;
TCHAR buf[2];

if ( is_checked(MainDialog.control(BIT_7_RADIO)) )
    i = 7;
else
    i = 8;

EosComBits = int32toasc( i );

EosComStop = StopBitChoice[0];
id = STOP_1_RADIO;
for ( i=0; i<NofStopBitChoices; i++ )
    {
    if ( is_checked(MainDialog.control(id)) )
        {
        EosComStop = StopBitChoice[i];
        break;
        }
    id++;
    }

buf[0] = ParityChoice[0];
buf[1] = NullChar;
id = NO_PARITY_RADIO;
for ( i=0; i<NofParityChoices; i++ )
    {
    if ( is_checked(MainDialog.control(id)) )
        {
        buf[0] = ParityChoice[i];
        break;
        }
    id++;
    }
EosComParity = buf;

set_com_port_indexes();

EosComEol.get_text(         MainDialog.control(COM_EOL_EBOX)     );
EosComPort.get_text(        MainDialog.control(COM_PORT_EBOX)    );
EosFixedWidth.get_text(     MainDialog.control(FIXED_WIDTH_EBOX) );
EosComDelimitChar.get_text( MainDialog.control(DELIMITER_EBOX)   );
EosComSetup.get_text(       MainDialog.control(SETUP_EBOX)       );
EosComSetup.uppercase();
EosComSpeed.get_text(       MainDialog.control(SPEED_EBOX)       );

EosComCts      = is_checked( MainDialog.control(CTS_XBOX)      );
EosComDsr      = is_checked( MainDialog.control(DSR_XBOX)      );
EosComXonXoff  = is_checked( MainDialog.control(XON_XOFF_XBOX) );
EosNeedsStxEtx = is_checked( MainDialog.control(STX_ETX_XBOX)  );
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
static bool init_serial_port()
{
static unsigned StopBitSetting[NofStopBitChoices] = { ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS };

HANDLE  fh;
DCB     dcb;
BOOL    status;

if ( EosComPort.isempty() )
    return false;

dcb.DCBlength = sizeof( DCB );
status = FALSE;

fh = CreateFile( EosComPort.text(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh == INVALID_HANDLE_VALUE )
    {
    MessageBox( 0, EosComPort.text(), "Unable to open port", MB_OK );
    return false;
    }

status = GetCommState( fh, &dcb );
CloseHandle( fh );

if ( !status )
    return false;

dcb.BaudRate = (DWORD) EosComSpeed.uint_value();
dcb.Parity   = EosComParityIndex;
if ( dcb.Parity == 0 )
    dcb.fParity = 0;
else
    dcb.fParity = 1;
dcb.ByteSize = unsigned( 7 + EosComBitsIndex );
dcb.StopBits = StopBitSetting[EosComStopIndex];

/*
---------------------------------------------------
Tell the other computer I am always ready for chars
--------------------------------------------------- */
dcb.fRtsControl = RTS_CONTROL_ENABLE;
dcb.fDtrControl = DTR_CONTROL_ENABLE;

if ( EosComCts )
    dcb.fOutxCtsFlow = 1;
else
    dcb.fOutxCtsFlow = 0;

if ( EosComDsr )
    dcb.fOutxDsrFlow = 1;
else
    dcb.fOutxDsrFlow = 0;

dcb.fDsrSensitivity   = 0;
dcb.fTXContinueOnXoff = 1;
dcb.fInX              = 0;

if ( EosComXonXoff )
    dcb.fOutX = 1;
else
    dcb.fOutX = 0;

fh = CreateFile( EosComPort.text(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
if ( fh != INVALID_HANDLE_VALUE )
    {
    SetCommState( fh, &dcb );
    CloseHandle( fh );
    return true;
    }

MessageBox( 0, EosComPort.text(), "Unable to initialize port", MB_OK );
return false;
}

/***********************************************************************
*                           INIT_MAIN_DIALOG                           *
***********************************************************************/
static void init_main_dialog()
{
INI_CLASS ini;
STRING_CLASS s;
int          i;

s = exe_directory();
s.cat_path( MonallIniFile );

if ( !file_exists(s.text()) )
    {
    s = exe_directory();
    s.cat_path( SerialInIniFile );
    }

ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( ini.find(EosComBitsKey) )
    EosComBits = ini.get_string();

if ( ini.find(EosComEolKey) )
    EosComEol = ini.get_string();

if ( ini.find(EosComParityKey) )
    EosComParity = ini.get_string();

if ( ini.find(EosComPortKey) )
    EosComPort = ini.get_string();

if ( ini.find(EosComSetupKey) )
    EosComSetup = ini.get_string();

if ( ini.find(EosComSpeedKey) )
    EosComSpeed = ini.get_string();

if ( ini.find(EosComStopKey) )
    EosComStop = ini.get_string();

if ( ini.find(EosFixedWidthKey) )
    EosFixedWidth = ini.get_string();

if ( ini.find(EosComDelimitCharKey) )
    EosComDelimitChar = ini.get_string( EosComDelimitCharKey );

if ( ini.find(EosHaveAlarmShotKey) )
    EosHaveAlarmShot = ini.get_string();
else
    EosHaveAlarmShot = AChar;

if ( ini.find(EosHaveGoodShotKey) )
    EosHaveGoodShot = ini.get_string();
else
    EosHaveGoodShot = GChar;

if ( ini.find(EosHaveWarningShotKey) )
    EosHaveWarningShot = ini.get_string();
else
    EosHaveWarningShot = WChar;

    EosHaveGoodShot = maketext( TEXT("G") );

EosNeedsStxEtx   = ini.get_boolean( EosNeedsStxEtxKey );
EosComCts        = ini.get_boolean( EosComCtsKey );
EosComDsr        = ini.get_boolean( EosComDsrKey );
EosComXonXoff    = ini.get_boolean( EosComXonXoffKey );

EosComEol.set_text(         MainDialog.control(COM_EOL_EBOX)     );
EosComPort.set_text(        MainDialog.control(COM_PORT_EBOX)    );
EosFixedWidth.set_text(     MainDialog.control(FIXED_WIDTH_EBOX) );
EosComDelimitChar.set_text( MainDialog.control(DELIMITER_EBOX)   );
EosComSetup.set_text(       MainDialog.control(SETUP_EBOX)       );
EosComSpeed.set_text(       MainDialog.control(SPEED_EBOX)       );

set_checkbox( MainDialog.control(CTS_XBOX),      EosComCts );
set_checkbox( MainDialog.control(DSR_XBOX),      EosComDsr );
set_checkbox( MainDialog.control(XON_XOFF_XBOX), EosComXonXoff );
set_checkbox( MainDialog.control(STX_ETX_XBOX),  EosNeedsStxEtx );

set_com_port_indexes();

i = EosComBitsIndex + BIT_7_RADIO;
CheckRadioButton( MainDialog.handle(), BIT_7_RADIO, BIT_8_RADIO, i );

i = EosComStopIndex + STOP_1_RADIO;
CheckRadioButton( MainDialog.handle(), STOP_1_RADIO, STOP_2_RADIO, i );

i = EosComStopIndex + NO_PARITY_RADIO;
CheckRadioButton( MainDialog.handle(), NO_PARITY_RADIO, MARK_PARITY_RADIO, i );

s = exe_directory();
s.cat_path( SerialInIniFile );
ini.set_file( s.text() );
}

/***********************************************************************
*                             MAIN_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;
HWND w;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_DBINIT:
        init_main_dialog();
        return TRUE;

    case WM_INITDIALOG:
        MainDialog = hWnd;
        MainWindow.shrinkwrap( hWnd );
        MainDialog.post( WM_DBINIT );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case INIT_COM_PORT_PB:
                get_settings_from_screen();
                init_serial_port();
                return TRUE;

            case LISTEN_XB:
                w = GetDlgItem( hWnd, LISTEN_XB );
                if ( is_checked(w) )
                    {
                    set_text( w, TEXT( "Listening") );
                    begin_listening();
                    }
                else
                    {
                    set_text( w, TEXT("Click to Listen") );
                    stop_listening();
                    }
                return TRUE;

            case IDCANCEL:
                MainWindow.close();
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
switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
STRING_CLASS title;
WNDCLASS wc;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
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
    TEXT( "MAIN_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) main_dialog_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialog.handle() )
        status = IsDialogMessage( MainDialog.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
