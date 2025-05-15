#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\setpoint.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\computer.h"
#include "..\include\updown.h"
#include "..\include\verrors.h"
#include "..\include\textlist.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"

#define _MAIN_
#include "extern.h"
#include "..\include\chars.h"
#include "..\include\events.h"
#define WM_HELP     0x0053

const int TCP_LEN = 22;  /*192.168.254.040  20000 */
const int BOARD_BUF_LEN = BOARD_NUMBER_LEN + 1 + TCP_LEN;

struct BOARD_ENTRY {
    int          number;
    STRING_CLASS tcp_addr;
    STRING_CLASS tcp_port;
    };

struct MACHINE_ENTRY {
    STRING_CLASS name;
    int board_number;
    int ft_channel;
    };

HINSTANCE MainInstance;
HWND      MainWindow;
HWND      BoardsSetupWindow;

TCHAR           MyClassName[] = TEXT( "V5SETUPS" );
STRING_CLASS    MyWindowTitle;
AUTO_BACKUP_CLASS AutoBack;

static TCHAR   DefaultTcpAddress[] = TEXT( "192.168.254.99" );
static TCHAR   DefaultTcpPort[]    = TEXT( "20000" );
static TCHAR   TabStr[]            = TEXT( "\t" );

/*
--------------
Ini file names
-------------- */
static TCHAR ConfigSection[]          = TEXT( "Config" );
static TCHAR ThisComputerKey[]        = TEXT( "ThisComputer" );
static TCHAR CommonResourceFile[]     = TEXT( "common" );
static TCHAR DisplayIniFile[]         = TEXT( "display.ini" );
static TCHAR EmachineIniFile[]        = TEXT( "emachine.ini" );
static TCHAR V5SetupsResourceFile[]   = TEXT( "v5setups" );
static TCHAR CurrentLanguageKey[]     = TEXT( "CurrentLanguage" );

static TCHAR   BoardLineBuf[BOARD_BUF_LEN+1];

static STRING_CLASS ComputerLineBuf;
static STRING_CLASS MachineLineBuf;
static STRING_CLASS ThisComputer;

STRING_CLASS   ActualComputerName;              /* The network name of this computer */

static BOOLEAN BoardsHaveChanged;
static BOOLEAN MachinesHaveChanged;
static BOOLEAN ProgramIsUpdating;

FONT_LIST_CLASS   FontList;
STRINGTABLE_CLASS StringTable;

/***********************************************************************
*                              ST_MESSAGE_BOX                          *
***********************************************************************/
void st_message_box( TCHAR * msg_id, TCHAR * title_id )
{
STRING_CLASS msg;
STRING_CLASS title;

msg   = StringTable.find( msg_id );
title = StringTable.find( title_id );
resource_message_box( msg.text(), title.text() );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
NAME_CLASS s;

s.get_exe_dir_file_name( TEXT("v5help.hlp") );
WinHelp( MainWindow, s.text(), helptype, context );
}

/***********************************************************************
*                       GET_ACTUAL_COMPUTER_NAME                       *
***********************************************************************/
static void get_actual_computer_name()
{
TCHAR s[MAX_PATH+1];
DWORD n;

n  = MAX_PATH;

GetComputerName( s, &n );
ActualComputerName = TEXT("\\\\" );
ActualComputerName += s;
}

/***********************************************************************
*                        MACHINE_LISTBOX_STRING                        *
***********************************************************************/
static TCHAR * machine_listbox_string( MACHINE_ENTRY & m )
{
MachineLineBuf = m.name;
MachineLineBuf += TabStr;

MachineLineBuf += m.board_number;
MachineLineBuf += TabStr;

MachineLineBuf += m.ft_channel;

return MachineLineBuf.text();
}

/***********************************************************************
*                       MACHINE_FROM_LISTBOXLINE                       *
***********************************************************************/
static MACHINE_ENTRY * machine_from_listboxline( TCHAR * sorc )
{
static MACHINE_ENTRY m;
TCHAR * cp;

MachineLineBuf = sorc;

m.name.null();
m.board_number = MIN_FT_NUMBER;
m.ft_channel   = MIN_FT_CHANNEL;

cp = MachineLineBuf.text();

if ( replace_tab_with_null(cp) )
    {
    m.name = cp;

    cp = nextstring( cp );
    if ( replace_tab_with_null(cp) )
        {
        m.board_number = asctoint32( cp );

        cp = nextstring( cp );
        m.ft_channel = asctoint32( cp );
        }
    }

return &m;
}

/***********************************************************************
*                     REFRESH_MACHINE_LISTBOX_ENTRY                    *
***********************************************************************/
static void refresh_machine_listbox_entry()
{
LISTBOX_CLASS   lb;
INT             n;
INT             x;
MACHINE_ENTRY * m;

/*
-------------------------
React to human input only
------------------------- */
if ( ProgramIsUpdating )
    return;

lb.init( BoardsSetupWindow, MACHINE_LISTBOX );

m = machine_from_listboxline( lb.selected_text() );
n = 1 + MACHINE_BOARD_RADIO_8 - MACHINE_BOARD_RADIO_1;

for ( x=0; x<n; x++ )
    {
    if ( is_checked(BoardsSetupWindow, MACHINE_BOARD_RADIO_1 + x) )
        {
        m->board_number = x+1;
        break;
        }
    }

if ( lb.replace(machine_listbox_string(*m)) )
    MachinesHaveChanged = TRUE;

}

/***********************************************************************
*                          DO_MACHINE_SELECT                           *
***********************************************************************/
static void do_machine_select()
{
LISTBOX_CLASS  lb;
MACHINE_ENTRY  * m;
INT              x;

lb.init( BoardsSetupWindow, MACHINE_LISTBOX );

m = machine_from_listboxline( lb.selected_text() );

ProgramIsUpdating = TRUE;

x = m->board_number - 1 + MACHINE_BOARD_RADIO_1;
CheckRadioButton( BoardsSetupWindow, MACHINE_BOARD_RADIO_1, MACHINE_BOARD_RADIO_8, x );

ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                             FILL_MACHINES                            *
***********************************************************************/
static void fill_machines()
{
LISTBOX_CLASS  lb;
MACHINE_CLASS  machine;
MACHINE_ENTRY  m;
MACHINE_NAME_LIST_CLASS mlist;

lb.init( BoardsSetupWindow, MACHINE_LISTBOX );
lb.empty();
lb.redraw_off();

mlist.add_computer( ThisComputer );
mlist.rewind();
while ( mlist.next() )
    {
    if ( machine.find(ThisComputer, mlist.name()) )
        {
        m.name = machine.name;
        m.board_number = machine.ft_board_number;
        m.ft_channel   = machine.muxchan;
        lb.add( machine_listbox_string(m) );
        }
    }

lb.redraw_on();
lb.setcursel( 0 );
do_machine_select();
MachinesHaveChanged = FALSE;
}

/***********************************************************************
*                            SAVE_MACHINES                             *
***********************************************************************/
static void save_machines()
{
BOOLEAN         need_to_save;
LISTBOX_CLASS   lb;
INT             i;
INT             n;
NAME_CLASS      s;
MACHINE_ENTRY * m;
SETPOINT_CLASS  sp;

if ( !MachinesHaveChanged )
    return;

lb.init( BoardsSetupWindow, MACHINE_LISTBOX );

n = lb.count();

for ( i=0; i<n; i++ )
    {
    m = machine_from_listboxline( lb.item_text(i) );
    s.get_machset_csvname( ThisComputer, m->name );
    if ( sp.get(s) )
        {
        need_to_save = FALSE;
        if ( sp[MACHSET_FT_BOARD_NO_INDEX].value.int_value() != m->board_number )
            {
            sp[MACHSET_FT_BOARD_NO_INDEX].value = m->board_number;
            need_to_save = TRUE;
            }
        if ( sp[MACHSET_MUX_CHAN_INDEX].value.int_value() != m->ft_channel )
            {
            sp[MACHSET_MUX_CHAN_INDEX].value = m->ft_channel;
            need_to_save = TRUE;
            }
        if ( need_to_save )
            {
            sp.put(s);
            AutoBack.backup_machine( m->name );
            }
        }
    }

MachinesHaveChanged = FALSE;
}

/***********************************************************************
*                           FILL_COMPUTERS                             *
***********************************************************************/
void fill_computers( void )
{
ThisComputer.set_text( BoardsSetupWindow, THIS_COMPUTER_TBOX );
BoardsHaveChanged = FALSE;
}

/***********************************************************************
*                        BOARD_FROM_LISTBOXLINE                        *
***********************************************************************/
static BOARD_ENTRY * board_from_listboxline( TCHAR * sorc )
{
static BOARD_ENTRY b;

TCHAR        * cp;
STRING_CLASS   type_name;

cp = BoardLineBuf;
lstrcpy( cp, sorc );

b.number  = MIN_FT_NUMBER;
b.tcp_addr = DefaultTcpAddress;
b.tcp_port = DefaultTcpPort;

/*
------------
Board number
------------ */
if ( replace_tab_with_null(cp) )
    {
    b.number = (int) asctoint32( cp );
    cp = nextstring( cp );

    if ( replace_tab_with_null(cp) )
        {
        b.tcp_addr = cp;
        cp = nextstring( cp );
        b.tcp_port = cp;
        }
    }

return &b;
}

/***********************************************************************
*                           DO_BOARD_SELECT                            *
***********************************************************************/
static void do_board_select()
{
BOARD_ENTRY  * b;
LISTBOX_CLASS  lb;
int32          current_index;
int32          i;
int32          n;
int32          x;
BOOL           is_enabled[MAX_BOARDS];


lb.init( BoardsSetupWindow, BOARD_LISTBOX );

b = board_from_listboxline( lb.selected_text() );

ProgramIsUpdating = TRUE;

x = b->number - 1 + BOARD_RADIO_1;
CheckRadioButton( BoardsSetupWindow, BOARD_RADIO_1, BOARD_RADIO_8, x );

b->tcp_addr.set_text( BoardsSetupWindow, TCP_ADDR_EBOX );
b->tcp_port.set_text( BoardsSetupWindow, TCP_PORT_EBOX );

n = lb.count();
if ( n > 1 )
    current_index = lb.selected_index();
else
    current_index = 0;

for ( i=0; i<MAX_BOARDS; i++ )
    is_enabled[i] = TRUE;

/*
-------------------------------------------------
Disable any radio button that already has a board
------------------------------------------------- */
for ( i=0; i<n; i++ )
    {
    if ( i != current_index )
        {
        b = board_from_listboxline( lb.item_text(i) );
        is_enabled[b->number-1] = FALSE;
        }
    }

for ( i=0; i<MAX_BOARDS; i++ )
    {
    x = BOARD_RADIO_1 + i;
    EnableWindow( GetDlgItem(BoardsSetupWindow, x), is_enabled[i] );
    }

ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                          BOARD_LISTBOX_STRING                        *
***********************************************************************/
TCHAR * board_listbox_string( BOARD_ENTRY & b )
{
TCHAR   * cp;

cp = BoardLineBuf;

insalph( cp, b.number,  BOARD_NUMBER_LEN, SpaceChar, DECIMAL_RADIX );
cp += BOARD_NUMBER_LEN;
cp = copystring( cp, TabStr   );

cp = copystring( cp, b.tcp_addr.text() );
cp = copystring( cp, TabStr   );

 cp = copystring( cp, b.tcp_port.text() );
*cp = NullChar;

return BoardLineBuf;
}

/***********************************************************************
*                     FTII_BOARD_LISTBOX_STRING                        *
*                                                                      *
*              This uses the current record in the table               *
***********************************************************************/
TCHAR * ftii_board_listbox_string( DB_TABLE & t )
{
BOARD_ENTRY b;

b.tcp_addr.upsize( TCP_ADDRESS_LEN );
b.tcp_port.upsize( TCP_PORT_LEN );

b.number  = (int) t.get_long( FTII_BOARDS_NUMBER_OFFSET );
t.get_alpha( b.tcp_addr.text(), FTII_BOARDS_ADDRESS_OFFSET, TCP_ADDRESS_LEN );
t.get_alpha( b.tcp_port.text(), FTII_BOARDS_PORT_OFFSET,    TCP_PORT_LEN );

return board_listbox_string( b );
}

/***********************************************************************
*                             FILL_BOARDS                              *
***********************************************************************/
static void fill_boards()
{
LISTBOX_CLASS  lb;
DB_TABLE       t;
NAME_CLASS     s;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );
lb.empty();
lb.redraw_off();

s.get_ft2_boards_dbname( ThisComputer );
if ( s.file_exists() )
    {
    if ( t.open(s, FTII_BOARDS_RECLEN, PFL) )
        {
        while ( t.get_next_record(FALSE) )
            lb.add( ftii_board_listbox_string(t) );
        t.close();
        }
    }
else
    {
    t.create( s );
    }

lb.redraw_on();
lb.setcursel( 0 );
do_board_select();
}

/***********************************************************************
*                             SET_TAB_STOPS                            *
***********************************************************************/
static void set_tab_stops()
{
const  int NOF_BOARD_TABS    = 2;
const  int NOF_MACHINE_TABS  = 1;
static int board_title_ids[NOF_BOARD_TABS] = { BOARD_ADDR_TITLE, CLOCK_MHZ_TITLE };
static int machine_title_ids[NOF_MACHINE_TABS] = { FT_BOARD_TITLE };

/*
-------------
Board listbox
------------- */
set_listbox_tabs_from_title_positions( BoardsSetupWindow, BOARD_LISTBOX, board_title_ids, NOF_BOARD_TABS );

/*
---------------
Machine listbox
--------------- */
set_listbox_tabs_from_title_positions( BoardsSetupWindow, MACHINE_LISTBOX, machine_title_ids, NOF_MACHINE_TABS );
}

/***********************************************************************
*                     REFRESH_BOARD_LISTBOX_ENTRY                      *
***********************************************************************/
static void refresh_board_listbox_entry()
{
BOARD_ENTRY b;

LISTBOX_CLASS  lb;
int            i;

if ( ProgramIsUpdating )
    return;

b.number = NO_BOARD_NUMBER;
for ( i=0; i<MAX_BOARDS; i++ )
    {
    if ( is_checked(BoardsSetupWindow, BOARD_RADIO_1 + i) )
        {
        b.number = i+1;
        break;
        }
    }

b.tcp_addr.get_text( BoardsSetupWindow, TCP_ADDR_EBOX );
b.tcp_port.get_text( BoardsSetupWindow, TCP_PORT_EBOX );

lb.init( BoardsSetupWindow, BOARD_LISTBOX );
if ( lb.replace(board_listbox_string(b)) )
    BoardsHaveChanged = TRUE;
}

/***********************************************************************
*                               SAVE_BOARDS                            *
***********************************************************************/
static void save_boards()
{
LISTBOX_CLASS  lb;
INT            i;
INT            n;
NAME_CLASS     s;
DB_TABLE       ft;
BOARD_ENTRY  * b;

if ( !BoardsHaveChanged )
    return;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );

n = lb.count();

s.get_ft2_boards_dbname( ThisComputer );
if ( ft.open(s, FTII_BOARDS_RECLEN, FL) )
    {
    ft.empty();

    for ( i=0; i<n; i++ )
        {
        b = board_from_listboxline( lb.item_text(i) );
        ft.put_short( FTII_BOARDS_NUMBER_OFFSET,  b->number,          BOARD_NUMBER_LEN );
        ft.put_alpha( FTII_BOARDS_ADDRESS_OFFSET, b->tcp_addr.text(), TCP_ADDRESS_LEN );
        ft.put_alpha( FTII_BOARDS_PORT_OFFSET,    b->tcp_port.text(), TCP_PORT_LEN );
        ft.rec_append();
        }
    ft.close();
    }

AutoBack.backup_data_dir();
BoardsHaveChanged = FALSE;
}

/***********************************************************************
*                            ADD_NEW_BOARD                             *
***********************************************************************/
static void add_new_board()
{
INT            i;
INT            n;
LISTBOX_CLASS  lb;
BOARD_ENTRY    b;
BOARD_ENTRY    pb;

b.number  = MIN_FT_NUMBER;
b.tcp_addr = DefaultTcpAddress;
b.tcp_port = DefaultTcpPort;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );

n = lb.count();
if ( n >= MAX_BOARDS )
    {
    st_message_box( TEXT("ONLY_8_BOARDS"), TEXT("NO_MORE_BOARDS") );
    return;
    }

if ( n > 0 )
    {
    i = lb.current_index();
    b = *board_from_listboxline( lb.selected_text() );

    /*
    ------------------------------------------------------
    See if there is a missing board before the current one
    ------------------------------------------------------ */
    if ( i > 0 )
        pb = *board_from_listboxline( lb.item_text(i-1) );
    else
        pb.number = NO_BOARD_NUMBER;

    if ( pb.number < (b.number - 1) )
        {
        b.number--;
        }
    else
        {
        b = *board_from_listboxline( lb.item_text(n-1) );
        if ( b.number < MAX_MUX_CHANNELS )
            b.number++;
        }

    }

i = lb.add( board_listbox_string(b) );
if ( i != NO_INDEX )
    {
    lb.setcursel( i );
    do_board_select();

    SetFocus( GetDlgItem(BoardsSetupWindow, BOARD_RADIO_1) );

    BoardsHaveChanged = TRUE;
    }
}

/***********************************************************************
*                       DELETE_CURRENT_BOARD                           *
***********************************************************************/
void delete_current_board()
{
INT            x;
LISTBOX_CLASS  lb;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );

if ( lb.count() > 1 )
    {
    x = lb.current_index();
    lb.remove( x );
    x = lb.current_index();
    lb.setcursel( x );
    do_board_select();
    BoardsHaveChanged = TRUE;
    }
}

/***********************************************************************
*                          CHECK_TCP_ADDRESS                           *
*                            Fix "..", 034.                            *
***********************************************************************/
static void check_tcp_address()
{
static TCHAR two_dots[] = TEXT( ".." );
static TCHAR one_dot[]  = TEXT( "." );

STRING_CLASS s;
HWND         w;
int          caret_position;

w = GetDlgItem( BoardsSetupWindow, TCP_ADDR_EBOX );
s.get_text( w );
if ( s.replace(two_dots, one_dot) )
    {
    caret_position = eb_get_caret_pos( w );
    if ( caret_position > 0 )
        caret_position--;
    s.set_text( w );
    eb_set_caret_pos( w, caret_position );
    }
}

/***********************************************************************
*                     SET_BOARDS_SETUP_STRINGS                         *
***********************************************************************/
static void set_boards_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { BOARD_NUMBER_STATIC,   TEXT("BOARD_NUM") },
    { BOARD_NUMBER_2_STATIC, TEXT("BOARD_NUM") },
    { BOARD_NUMBER_3_STATIC, TEXT("BOARD_NUM") },
    { BOARD_ADDR_TITLE,      TEXT("BOARD_ADDR") },
    { CLOCK_MHZ_TITLE,       TEXT("TCP_PORT") },
    { MACHINE_STATIC,        TEXT("MACHINE_STRING") },
    { FT_BOARD_TITLE,        TEXT("BOARD_NUM") },
    { IDOK,                  TEXT("SAVE_CHANGES") },
    { MA_AT_STATIC,          TEXT("MA_AT_CO") },
    { BOARDS_AT_STATIC,      TEXT("BOARDS_AT_CO") },
    { NEW_BOARD_BUTTON,      TEXT("NEW_STRING") },
    { DELETE_BOARD_BUTTON,   TEXT("DELETE_STRING") },
    { TCP_ADDR_TXT,          TEXT("TCP_ADDR") },
    { TCP_PORT_TXT,          TEXT("TCP_PORT") },
    { THIS_CO_STATIC,        TEXT("THIS_CO") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    THIS_COMPUTER_TBOX,
    BOARD_LISTBOX,
    MACHINE_LISTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                          BOARDS_SETUP_PROC                           *
***********************************************************************/
BOOL CALLBACK boards_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        BoardsSetupWindow = hWnd;
        set_boards_setup_strings( hWnd );
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        set_tab_stops();
        fill_computers();
        fill_boards();
        fill_machines();
        SetFocus( GetDlgItem(hWnd, BOARD_LISTBOX) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                AutoBack.get_backup_dir();
                save_boards();
                save_machines();
                return TRUE;

            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return 0;

            case MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_machine_select();
                    return TRUE;
                    }
                break;

            case BOARD_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_board_select();
                    return TRUE;
                    }
                break;

            case DELETE_BOARD_BUTTON:
                delete_current_board();
                return TRUE;

            case NEW_BOARD_BUTTON:
                add_new_board();
                return TRUE;

            case BOARD_RADIO_1:
            case BOARD_RADIO_2:
            case BOARD_RADIO_3:
            case BOARD_RADIO_4:
            case BOARD_RADIO_5:
            case BOARD_RADIO_6:
            case BOARD_RADIO_7:
            case BOARD_RADIO_8:
                if ( cmd == BN_CLICKED )
                    {
                    refresh_board_listbox_entry();
                    return TRUE;
                    }
                break;

            case TCP_ADDR_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    check_tcp_address();
                    refresh_board_listbox_entry();
                    return TRUE;
                    }
                break;

            case MACHINE_BOARD_RADIO_1:
            case MACHINE_BOARD_RADIO_2:
            case MACHINE_BOARD_RADIO_3:
            case MACHINE_BOARD_RADIO_4:
            case MACHINE_BOARD_RADIO_5:
            case MACHINE_BOARD_RADIO_6:
            case MACHINE_BOARD_RADIO_7:
            case MACHINE_BOARD_RADIO_8:
                if ( cmd == BN_CLICKED )
                    {
                    refresh_machine_listbox_entry();
                    return TRUE;
                    }
                break;
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
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        BoardsSetupWindow  = CreateDialog(
            MainInstance,
            TEXT( "BOARDS_SETUP_DIALOG" ),
            MainWindow,
            (DLGPROC) boards_setup_proc );
        return 0;

    case WM_SETFOCUS:
        if ( BoardsSetupWindow )
            SetFocus( GetDlgItem(BoardsSetupWindow, BOARD_LISTBOX) );
        return 0;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, FASTRAK_BOARD_SETUP_HELP );
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
s.get_stringtable_name( V5SetupsResourceFile );
StringTable.read( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );

statics_startup();
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static bool init( int cmd_show )
{
WNDCLASS wc;
COMPUTER_CLASS c;

BoardsSetupWindow    = 0;

c.startup();

ThisComputer = c.whoami();
get_actual_computer_name();

load_resources();
MyWindowTitle = StringTable.find( TEXT("V5SETUPS_TITLE") );

if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return false;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MAKEINTRESOURCE(V5SETUPS_ICON) );
wc.lpszMenuName  = 0;
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
    225, 225,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
return true;
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown()
{
COMPUTER_CLASS c;

client_dde_shutdown();
c.shutdown();
statics_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
BOOL status;
MSG   msg;

InitCommonControls();

MainInstance = this_instance;

if ( !init(cmd_show) )
    return 0;

while ( GetMessage(&msg, NULL, 0, 0) )
   {
   status = FALSE;

   if (BoardsSetupWindow )
      status = IsDialogMessage( BoardsSetupWindow, &msg );

   if ( !status )
      {
      TranslateMessage( &msg );
      DispatchMessage(  &msg );
      }
   }

shutdown();
return msg.wParam;
}
