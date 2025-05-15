#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\dbclass.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\setpoint.h"
#include "..\include\subs.h"
#include "..\include\computer.h"
#include "..\include\updown.h"
#include "..\include\verrors.h"
#include "..\include\textlist.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"

#include "resource.h"

#define _MAIN_
#include "extern.h"
#include "..\include\chars.h"
#include "..\include\events.h"
#define WM_HELP     0x0053
#define MAX_BOARD_TYPE_LEN 10

static const short NOF_BOARD_TYPES = 3;
static BITSETYPE BoardType[] = { SINGLE_MACHINE_BOARD_TYPE, MUX_BOARD_TYPE, FTII_BOARD_TYPE };
const int TCP_LEN = 22;  /*192.168.254.040  20000 */
const int BOARD_BUF_LEN = BOARD_NUMBER_LEN + 1 + TCP_LEN + 1 + HEX_FT_ADDRESS_LEN + 1 + MAX_BOARD_TYPE_LEN + 1 + MHZ_LEN;

struct BOARD_ENTRY {
    short     address;
    short     number;
    BITSETYPE type;
    short     mhz;
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

static TCHAR   TabStr[]          = TEXT( "\t" );

/*
--------------
Ini file names
-------------- */
static TCHAR   VisiTrakIniFile[] = TEXT("\\V5\\Exes\\Visitrak.ini");
static TCHAR   ConfigSection[]   = TEXT( "Config" );
static TCHAR   ThisComputerKey[] = TEXT( "ThisComputer" );

static TCHAR   BoardLineBuf[BOARD_BUF_LEN+1];

static STRING_CLASS ComputerLineBuf;
static STRING_CLASS MachineLineBuf;
static STRING_CLASS ThisComputer;

STRING_CLASS   ActualComputerName;              /* The network name of this computer */

static BOOLEAN BoardsHaveChanged;
static BOOLEAN MachinesHaveChanged;
static BOOLEAN ProgramIsUpdating;

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
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

n = 1 + MACHINE_MUX_RADIO_8 - MACHINE_MUX_RADIO_1;
for ( x=0; x<n; x++ )
    {
    if ( is_checked(BoardsSetupWindow, MACHINE_MUX_RADIO_1 + x) )
        {
        m->ft_channel = x+1;
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

x = m->ft_channel - 1 + MACHINE_MUX_RADIO_1;
CheckRadioButton( BoardsSetupWindow, MACHINE_MUX_RADIO_1, MACHINE_MUX_RADIO_8, x );

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
COMPUTER_CLASS c;
LISTBOX_CLASS  lb;

ThisComputer.set_text( BoardsSetupWindow, THIS_COMPUTER_TBOX );

lb.init( BoardsSetupWindow, COMPUTER_LISTBOX );
lb.empty();
lb.redraw_off();

c.rewind();
while( c.next() )
    lb.add( c.name().text() );

lb.setcursel( 0 );
lb.redraw_on();

BoardsHaveChanged = FALSE;
}

/***********************************************************************
*                        BOARD_FROM_LISTBOXLINE                        *
***********************************************************************/
static BOARD_ENTRY * board_from_listboxline( TCHAR * sorc )
{
static BOARD_ENTRY b;

TCHAR        * cp;
TCHAR        * ap;
STRING_CLASS   type_name;

int32          i;

cp = BoardLineBuf;
lstrcpy( cp, sorc );

b.number  = MIN_FT_NUMBER;
b.address = MIN_FT_ADDRESS;
b.type    = BoardType[0];
b.mhz     = DEF_FT_MHZ;

/*
------------
Board number
------------ */
if ( replace_tab_with_null(cp) )
    {
    b.number = (short) asctoint32( cp );
    cp = nextstring( cp );

    if ( replace_tab_with_null(cp) )
        {
        ap = cp;
        cp = nextstring( cp );
        replace_tab_with_null( cp );
        }

    /*
    --------------------------------
    Board type
    cp now points to the type string
    -------------------------------- */
    for ( i=0; i<NOF_BOARD_TYPES; i++ )
        {
        type_name = resource_string( SINGLE_BOARD_STRING+i );
        if ( type_name == cp )
            {
            b.type = BoardType[i];
            break;
            }
        }

    /*
    -------------
    Board Address
    ------------- */
    if ( b.type & FTII_BOARD_TYPE )
        {
        b.tcp_addr = ap;
        }
    else
        {
        b.tcp_addr = EmptyString;
        b.address = (short) extlong( ap, lstrlen(ap), HEX_RADIX );
        }

    cp = nextstring( cp );
    if ( b.type & FTII_BOARD_TYPE )
        b.tcp_port = cp;
    else
        b.mhz = (short) asctoint32( cp );
    }

return &b;
}

/***********************************************************************
*                           SHOW_BOARD_BOXES                           *
***********************************************************************/
static void show_board_boxes( BOARD_ENTRY & b )
{
int old_board_ids[] = {MHZ_4_BUTTON, MHZ_12_BUTTON, CLOCK_SPEED_TXT, MHZ_TXT,
BOARD_ADDRESS_UPDOWN, BOARD_ADDR_TXT, BOARD_ADDRESS_EDITBOX };
const int nof_old_ids = sizeof(old_board_ids)/sizeof(int);

int ftii_board_ids[] = {TCP_ADDR_EBOX, TCP_PORT_EBOX, TCP_ADDR_TXT, TCP_PORT_TXT };
const int nof_ftii_ids = sizeof(ftii_board_ids)/sizeof(int);

int i;
WINDOW_CLASS   w;

if ( b.type & FTII_BOARD_TYPE )
    {
    for ( i=0; i<nof_old_ids; i++ )
        {
        w = GetDlgItem( BoardsSetupWindow, old_board_ids[i] );
        w.hide();
        }

    for ( i=0; i<nof_ftii_ids; i++ )
        {
        w = GetDlgItem( BoardsSetupWindow, ftii_board_ids[i] );
        w.show();
        }
    }
else
    {
    for ( i=0; i<nof_old_ids; i++ )
        {
        w = GetDlgItem( BoardsSetupWindow, old_board_ids[i] );
        w.show();
        }

    for ( i=0; i<nof_ftii_ids; i++ )
        {
        w = GetDlgItem( BoardsSetupWindow, ftii_board_ids[i] );
        w.hide();
        }
    }
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

set_text( BoardsSetupWindow, BOARD_ADDRESS_EDITBOX, ultohex((unsigned long) b->address) );

for ( i=0; i<NOF_BOARD_TYPES; i++ )
    {
    if ( b->type & BoardType[i] )
        {
        x = SINGLE_BOARD_TYPE_BUTTON + i;
        CheckRadioButton( BoardsSetupWindow, SINGLE_BOARD_TYPE_BUTTON, FTII_BOARD_TYPE_BUTTON, x );
        break;
        }
    }

show_board_boxes(*b);

if ( b->type & FTII_BOARD_TYPE )
    {
    b->tcp_addr.set_text( BoardsSetupWindow, TCP_ADDR_EBOX );
    b->tcp_port.set_text( BoardsSetupWindow, TCP_PORT_EBOX );
    }
else
    {
    if ( b->mhz == 4 )
        x = MHZ_4_BUTTON;
    else
        x = MHZ_12_BUTTON;
    CheckRadioButton( BoardsSetupWindow, MHZ_4_BUTTON, MHZ_12_BUTTON, x );
    }

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
int       i;

cp = BoardLineBuf;

insalph( cp, b.number,  BOARD_NUMBER_LEN, SpaceChar, DECIMAL_RADIX );
cp += BOARD_NUMBER_LEN;
cp = copystring( cp, TabStr   );

if ( b.type & FTII_BOARD_TYPE )
    {
    cp = copystring( cp, b.tcp_addr.text() );
    }
else
    {
    insalph( cp, b.address, HEX_FT_ADDRESS_LEN, SpaceChar, HEX_RADIX );
    cp += HEX_FT_ADDRESS_LEN;
    }

cp = copystring( cp, TabStr   );

for ( i=0; i<NOF_BOARD_TYPES; i++ )
    {
    if ( b.type & BoardType[i] )
        {
        cp = copystring( cp, resource_string(SINGLE_BOARD_STRING+i) );
        break;
        }
    }

cp = copystring( cp, TabStr   );

if ( b.type & FTII_BOARD_TYPE )
    {
    cp = copystring( cp, b.tcp_port.text() );
    }
else
    {
    insalph( cp, b.mhz, MHZ_LEN, SpaceChar, DECIMAL_RADIX );
    cp += MHZ_LEN;
    }

*cp = NullChar;

return BoardLineBuf;
}

/***********************************************************************
*                          BOARD_LISTBOX_STRING                        *
*                                                                      *
*              This uses the current record in the table               *
***********************************************************************/
TCHAR * board_listbox_string( DB_TABLE & t )
{
BOARD_ENTRY b;

b.number  = t.get_short( BOARDS_NUMBER_OFFSET );
b.address = t.get_short( BOARDS_ADDRESS_OFFSET );
b.type    = (BITSETYPE) t.get_long( BOARDS_TYPE_OFFSET );
b.mhz     = t.get_short( BOARDS_MHZ_OFFSET );

return board_listbox_string( b );
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

b.type    = (BITSETYPE) FTII_BOARD_TYPE;
b.number  = t.get_short( FTII_BOARDS_NUMBER_OFFSET );
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

s.get_boards_dbname(ThisComputer);
if ( s.file_exists() )
    {
    if ( t.open(s, BOARDS_RECLEN, PFL) )
        {
        while ( t.get_next_record(FALSE) )
            lb.add( board_listbox_string(t) );
        t.close();
        }
    }

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
const  int NOF_BOARD_TABS    = 3;
const  int NOF_MACHINE_TABS  = 2;
static int board_title_ids[NOF_BOARD_TABS] = { BOARD_ADDR_TITLE, BOARD_TYPE_TITLE, CLOCK_MHZ_TITLE };
static int machine_title_ids[NOF_MACHINE_TABS] = { FT_BOARD_TITLE, FT_MUX_CHANNEL_TITLE };

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
TCHAR          s[HEX_FT_ADDRESS_LEN+1];

if ( ProgramIsUpdating )
    return;

get_text( s, BoardsSetupWindow, BOARD_ADDRESS_EDITBOX, HEX_FT_ADDRESS_LEN+1 );
b.address = (short) hextoul( s );

b.number = NO_BOARD_NUMBER;
for ( i=0; i<MAX_BOARDS; i++ )
    {
    if ( is_checked(BoardsSetupWindow, BOARD_RADIO_1 + i) )
        {
        b.number = i+1;
        break;
        }
    }

for ( i=0; i<NOF_BOARD_TYPES; i++ )
    {
    if ( is_checked(BoardsSetupWindow, SINGLE_BOARD_TYPE_BUTTON + i) )
        {
        b.type = BoardType[i];
        break;
        }
    }

if ( b.type & FTII_BOARD_TYPE )
    {
    b.tcp_addr.get_text( BoardsSetupWindow, TCP_ADDR_EBOX );
    b.tcp_port.get_text( BoardsSetupWindow, TCP_PORT_EBOX );
    }
else
    {
    if ( is_checked(BoardsSetupWindow, MHZ_4_BUTTON) )
        b.mhz = 4;
    else
        b.mhz = 12;
    }

show_board_boxes( b );

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
DB_TABLE       t;
DB_TABLE       ft;
BOARD_ENTRY  * b;

if ( !BoardsHaveChanged )
    return;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );

n = lb.count();

s.get_boards_dbname();
if ( t.open(s, BOARDS_RECLEN, FL) )
    {
    t.empty();
    s.get_ft2_boards_dbname( ThisComputer );
    if ( ft.open(s, FTII_BOARDS_RECLEN, FL) )
        {
        ft.empty();

        for ( i=0; i<n; i++ )
            {
            b = board_from_listboxline( lb.item_text(i) );
            if ( b->type & FTII_BOARD_TYPE  )
                {
                ft.put_short( FTII_BOARDS_NUMBER_OFFSET,  b->number,          BOARD_NUMBER_LEN );
                ft.put_alpha( FTII_BOARDS_ADDRESS_OFFSET, b->tcp_addr.text(), TCP_ADDRESS_LEN );
                ft.put_alpha( FTII_BOARDS_PORT_OFFSET,    b->tcp_port.text(), TCP_PORT_LEN );
                ft.rec_append();
                }
            else
                {
                t.put_short( BOARDS_NUMBER_OFFSET,  b->number,  BOARD_NUMBER_LEN );
                t.put_short( BOARDS_ADDRESS_OFFSET, b->address, DECIMAL_FT_ADDRESS_LEN );
                t.put_long(  BOARDS_TYPE_OFFSET,    b->type,    BITSETYPE_LEN );
                t.put_short( BOARDS_MHZ_OFFSET,     b->mhz,     BOARDS_DB_MHZ_LEN );
                t.put_short( BOARDS_CYCLES_PER_TIME_COUNT_OFFSET,  DEF_CYCLES_PER_TIME_COUNT, BOARDS_DB_CYCLES_LEN );
                t.put_short( BOARDS_COUNTS_PER_POS_MARK_OFFSET,    DEF_COUNTS_PER_POS_MARK,   BOARDS_DB_COUNTS_LEN );
                t.rec_append();
                }
            }
        ft.close();
        }
    t.close();
    }

AutoBack.backup_data_dir();
BoardsHaveChanged = FALSE;
}

/***********************************************************************
*                        UPDATE_UPDOWN_EDITBOX                         *
***********************************************************************/
static void update_updown_editbox()
{
TCHAR s[HEX_FT_ADDRESS_LEN+1];
short i;

UP_DOWN_CLASS u( BoardsSetupWindow, BOARD_ADDRESS_UPDOWN );

i = u.getvalue();

insalph( s, i, HEX_FT_ADDRESS_LEN, SpaceChar, HEX_RADIX );
s[HEX_FT_ADDRESS_LEN] = NullChar;

set_text( BoardsSetupWindow, BOARD_ADDRESS_EDITBOX, s );
}

/***********************************************************************
*                         INIT_UPDOWN_CONTROL                          *
***********************************************************************/
static void init_updown_control()
{
UP_DOWN_CLASS u( BoardsSetupWindow, BOARD_ADDRESS_UPDOWN );
u.sethex();
u.setrange( MIN_FT_ADDRESS, MAX_FT_ADDRESS );
u.setincrement( FT_ADDRESS_INCREMENT );

u.setbuddy( BOARD_ADDRESS_EDITBOX );
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
b.address = MIN_FT_ADDRESS;
b.type    = BoardType[0];
b.mhz     = DEF_FT_MHZ;

lb.init( BoardsSetupWindow, BOARD_LISTBOX );

n = lb.count();
if ( n >= MAX_BOARDS )
    {
    resource_message_box( MainInstance, ONLY_EIGHT_BOARDS_STRING, NO_CREATE_BOARD_STRING );
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
        if ( b.address > MIN_FT_ADDRESS )
            b.address -= FT_ADDRESS_INCREMENT;
        }
    else
        {
        b = *board_from_listboxline( lb.item_text(n-1) );
        if ( b.number < MAX_MUX_CHANNELS )
            b.number++;

        if ( b.address < MAX_FT_ADDRESS )
            b.address += FT_ADDRESS_INCREMENT;
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
*                          BOARDS_SETUP_PROC                           *
***********************************************************************/
BOOL CALLBACK boards_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;
HWND w;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        BoardsSetupWindow = hWnd;
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        init_updown_control();
        set_tab_stops();
        SendMessage( GetDlgItem(hWnd, COMPUTER_NAME_EDITBOX), EM_LIMITTEXT, COMPUTER_NAME_LEN, 0L );
        fill_computers();
        fill_boards();
        fill_machines();
        SetFocus( GetDlgItem(hWnd, COMPUTER_LISTBOX) );
        return TRUE;

    case WM_VSCROLL:
        w = (HWND) lParam;
        if ( w == GetDlgItem(BoardsSetupWindow, BOARD_ADDRESS_UPDOWN) )
            {
            update_updown_editbox();
            return TRUE;
            }
        break;

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
            case SINGLE_BOARD_TYPE_BUTTON:
            case MUX_BOARD_TYPE_BUTTON:
            case FTII_BOARD_TYPE_BUTTON:
            case MHZ_4_BUTTON:
            case MHZ_12_BUTTON:
                if ( cmd == BN_CLICKED )
                    {
                    refresh_board_listbox_entry();
                    return TRUE;
                    }
                break;

            case BOARD_ADDRESS_EDITBOX:
            case TCP_ADDR_EBOX:
                if ( cmd == EN_CHANGE )
                    {
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

            case MACHINE_MUX_RADIO_1:
            case MACHINE_MUX_RADIO_2:
            case MACHINE_MUX_RADIO_3:
            case MACHINE_MUX_RADIO_4:
            case MACHINE_MUX_RADIO_5:
            case MACHINE_MUX_RADIO_6:
            case MACHINE_MUX_RADIO_7:
            case MACHINE_MUX_RADIO_8:
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
            SetFocus( GetDlgItem(BoardsSetupWindow, COMPUTER_LISTBOX) );
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

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( int cmd_show )
{
WNDCLASS wc;
COMPUTER_CLASS c;

BoardsSetupWindow    = 0;

c.startup();

ThisComputer = c.whoami();
get_actual_computer_name();

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
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown()
{
COMPUTER_CLASS c;

client_dde_shutdown();
c.shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
BOOL status;
MSG   msg;

InitCommonControls();

MainInstance = this_instance;
MyWindowTitle = resource_string( MAIN_WINDOW_TITLE );

if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return 0;

init( cmd_show );

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
