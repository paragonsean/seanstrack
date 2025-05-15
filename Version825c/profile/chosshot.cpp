#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\structs.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"
#include "extern.h"

#define OK_BUTTON         1
#define ZABISH_SCREEN_TYPE    2

INT ShotListboxId = SINGLE_SHOT_LISTBOX;

static long LastX = 40;
static long LastY = 50;

PART_NAME_ENTRY MyPart;

static HWND   MyWindow;

static short NSelected;
const static TCHAR NullChar  = TEXT( '\0' );
const static TCHAR SpaceChar = TEXT( ' ' );
const static TCHAR TabChar   = TEXT( '\t' );
const static TCHAR ZeroChar  = TEXT( '0' );

extern short MyScreenType;

void toggle_realtime();
void fill_machines( HWND w, UINT listbox_id );
RECT & my_desktop_client_rect();
bool overlaying();

extern void add_to_backup_list();   /* Profile.cpp */

/***********************************************************************
*                         CURRENT_MACHINE_NAME                         *
***********************************************************************/
static STRING_CLASS & current_machine_name()
{
static STRING_CLASS my_machine_name;
LISTBOX_CLASS lb;

lb.init( MyWindow, CS_MACHINE_LISTBOX );

my_machine_name = lb.selected_text();
if ( my_machine_name.isempty() )
    my_machine_name = MachineName;

return my_machine_name;
}

/***********************************************************************
*                         CURRENT_PART_NAME                            *
***********************************************************************/
static STRING_CLASS & current_part_name()
{
static STRING_CLASS my_part_name;
TCHAR       * cp;
LISTBOX_CLASS lb;

lb.init( MyWindow, CS_PART_LISTBOX );
my_part_name = lb.selected_text();
if ( my_part_name.isempty() )
    my_part_name = PartName;

cp = my_part_name.find( TabChar );
if ( cp )
    *cp = NullChar;

return my_part_name;
}

/***********************************************************************
*                            SET_LISTBOX_ID                            *
***********************************************************************/
static void set_listbox_id()
{
INT  hidden_box_id;
HWND w;

if ( ShotListboxId == SINGLE_SHOT_LISTBOX )
    hidden_box_id = MULTIPLE_SHOT_LISTBOX;
else
    hidden_box_id = SINGLE_SHOT_LISTBOX;

w = GetDlgItem( MyWindow, hidden_box_id );
SendMessage( w, LB_RESETCONTENT, TRUE, 0L );
ShowWindow( w, SW_HIDE );

w = GetDlgItem( MyWindow, ShotListboxId );
ShowWindow( w, SW_SHOW );
}

/***********************************************************************
*                          SET_LISTBOX_STYLE                           *
***********************************************************************/
void set_listbox_style()
{
if ( overlaying() && ShotListboxId == SINGLE_SHOT_LISTBOX )
    ShotListboxId = MULTIPLE_SHOT_LISTBOX;

else if ( !overlaying()  && ShotListboxId == MULTIPLE_SHOT_LISTBOX )
    ShotListboxId = SINGLE_SHOT_LISTBOX;

else
    return;

set_listbox_id();
}

/***********************************************************************
*                              FILL_SHOTS                              *
***********************************************************************/
static void fill_shots()
{
int             i;
int             nof_selected_shots;
NAME_CLASS      s;
STRING_CLASS    selected_shot_number[MAX_OVERLAY_SHOTS];
STRING_CLASS    shot_name;
STRING_CLASS    temp;
DB_TABLE        t;
HWND            w;
LRESULT         x;

hourglass_cursor();

MyPart.set( current_part_name() );

nof_selected_shots = 0;
if ( overlaying() )
    {
    for ( i=0; i<NofOverlayShots; i++ )
        selected_shot_number[i] = OverlayShot[i].shot;
    nof_selected_shots = NofOverlayShots;
    }
else if ( HaveHeader )
    {
    selected_shot_number[0] = ShotName;
    nof_selected_shots = 1;
    }

set_listbox_style();

w = GetDlgItem( MyWindow, ShotListboxId );

SendMessage( w, LB_RESETCONTENT, TRUE, 0L );
SendMessage( w, WM_SETREDRAW, 0,  0l );
NSelected = 0;

s.get_graphlst_dbname( MyPart.computer, MyPart.machine, MyPart.part );
if ( s.file_exists() )
    {
    if ( t.open(s, 0, PFL) )
        {
        while ( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( shot_name, GRAPHLST_SHOT_NAME_INDEX );
            s = shot_name;
            s += TabChar;

            t.get_alpha( temp, GRAPHLST_DATE_INDEX );
            s += temp;
            s += TabChar;

            t.get_alpha( temp, GRAPHLST_TIME_INDEX );
            s += temp;

            x = SendMessage( w, LB_ADDSTRING, 0, (LPARAM)(LPSTR) s.text() );
            if ( x != LB_ERR && NSelected < nof_selected_shots )
                {
                for ( i=0; i<nof_selected_shots; i++ )
                    {
                    if ( shot_name == selected_shot_number[i] )
                        {
                        NSelected++;
                        if ( ShotListboxId == SINGLE_SHOT_LISTBOX )
                            SendMessage( w, LB_SETCURSEL, x, 0L );
                        else
                            SendMessage( w, LB_SETSEL, TRUE, x );
                        break;
                        }
                    }
                }
            }
        t.close();
        }
    }

SendDlgItemMessage( MyWindow, ShotListboxId, WM_SETREDRAW, 1,  0l );
InvalidateRect( GetDlgItem(MyWindow, ShotListboxId), NULL, TRUE );

restore_cursor();
}

/***********************************************************************
*                              FILL_PARTS                              *
*   You should set MyPart before calling this with with_shots == TRUE  *
***********************************************************************/
static void fill_parts( BOOLEAN with_shots )
{
COMPUTER_CLASS c;
LISTBOX_CLASS  lb;
STRING_CLASS   s;
BOOLEAN        computer_is_online;

lb.init( MyWindow, CS_PART_LISTBOX );
lb.empty();

s = current_machine_name();
if ( MachList.find(s) )
    {
    if ( c.find(MachList.computer_name()) )
        {
        hourglass_cursor();
        computer_is_online = c.is_present();
        restore_cursor();
        if ( !computer_is_online )
            {
            lb.init( MyWindow, ShotListboxId );
            lb.empty();
            return;
            }
        }
    }

if ( !with_shots )
    {
    if ( MachList.find(s) )
        {
        MyPart.computer = MachList.computer_name();
        MyPart.machine  =  MachList.name();
        MyPart.part.null();
        }
    else
        return;
    }

if ( MyPart.machine.isempty() )
    {
    MachList.rewind();
    MachList.next();
    MyPart.computer = MachList.computer_name();
    MyPart.machine  = MachList.name();
    }

fill_parts_and_shot_count_listbox( MyWindow, CS_PART_LISTBOX, MyPart.computer, MyPart.machine );

if ( with_shots )
    {
    lb.findprefix( MyPart.part.text() );
    fill_shots();
    }
else
    {
    lb.setcursel( -1 );
    lb.init( MyWindow, ShotListboxId );
    lb.empty();
    }
}

/***********************************************************************
*                      COPY_SELECTIONS_TO_EXTERN                       *
***********************************************************************/
void copy_selections_to_extern()
{
int        n;
int        i;
LISTBOX_CLASS lb;
STRING_CLASS  s;

lb.init( MyWindow, ShotListboxId );

i = lb.current_index();
if ( i != NO_INDEX )
    {
    if ( ShotListboxId == MULTIPLE_SHOT_LISTBOX )
        {
        n = lb.select_count();

        if ( !lb.is_selected(i) )
            {
            if ( n == 0 )
                {
                lb.setcursel( i );
                NSelected = 1;
                return;
                }

            else if ( n > MAX_OVERLAY_SHOTS )
                {
                /*
                ------------------------------------------------------
                This entry has just been selected but there is no room
                ------------------------------------------------------ */
                lb.set_select( (INT) i, FALSE );
                NSelected--;
                resource_message_box( MyWindow, MainInstance, DEL_SOME_FIRST_STRING, MAX_OVERLAY_REACHED_STRING, MB_OK );
                return;
                }
            }
        }

    s = lb.item_text( i );
    if ( !s.isempty() )
        {
        ComputerName = MyPart.computer;
        MachineName  = MyPart.machine;
        PartName     = MyPart.part;
        fix_shotname( s );
        ShotName = s;
        }
    }

SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
if ( RealTime )
    toggle_realtime();
}

/***********************************************************************
*                          CHOOSE_THESE_SHOTS                          *
***********************************************************************/
static void choose_these_shots()
{
LRESULT n;

/*
------------------------------------------------------------
If this is a multiple selection listbox, see if this is just
an arrow key.
------------------------------------------------------------ */
if ( ShotListboxId == MULTIPLE_SHOT_LISTBOX )
    {
    n = SendDlgItemMessage( MyWindow, ShotListboxId, LB_GETSELCOUNT, 0, 0L );

    if ( n == LB_ERR )
        return;

    if ( n == NSelected )
        return;
    else
        NSelected = short( n );
    }

copy_selections_to_extern();
}

/***********************************************************************
*                            SAVE_POSITION                             *
***********************************************************************/
static void save_position()
{
RECT r;

if ( MyScreenType != ZABISH_SCREEN_TYPE )
    {
    GetWindowRect( MyWindow, &r );
    LastX = r.left;
    LastY = r.top;
    }
}

/***********************************************************************
*                           POSITION_DIALOG                            *
***********************************************************************/
static void position_dialog()
{
RECT r;

GetWindowRect( MyWindow, &r );

r.right  += LastX - r.left;
r.bottom += LastY - r.top;
r.left   = LastX;
r.top    = LastY;

MoveWindow( MyWindow, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
}

/**********************************************************************
*                            FILL_STATICS                             *
**********************************************************************/
static void fill_statics( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] =
    {
    { IDOK,                  TEXT("OK_STRING") },
    { CS_SHOT_NUMBER_STATIC, TEXT("SHOT_NUMBER_STRING") },
    { CS_MACHINE_STATIC,     TEXT("MACHINE_LABEL_STRING") },
    { CS_PART_STATIC,        TEXT("PART_LABEL_STRING") },
    { CS_SHOTS_STATIC,       TEXT("SHOTS_LABEL_STRING") },
    { CS_DATE_STATIC,        TEXT("DATE_STRING") },
    { CS_TIME_STATIC,        TEXT("TIME_STRING") }
    };
const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    SINGLE_SHOT_LISTBOX,
    MULTIPLE_SHOT_LISTBOX,
    CS_MACHINE_LISTBOX,
    CS_PART_LISTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("CHOOSE_SHOTS_CAPTION")) );
w.refresh();
}

/***********************************************************************
*                            CHOSSHOT_PROC                             *
***********************************************************************/
BOOL CALLBACK chosshot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int tabs[1] = { 100 };
static int ids[1] = { CS_SHOTS_STATIC };
static int shot_ids[2] = { CS_DATE_STATIC, CS_TIME_STATIC };
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_DBINIT:
    case WM_NEWPART:
        fill_statics( hWnd );
        MyPart.set( ComputerName, MachineName, PartName );
        fill_machines( hWnd, CS_MACHINE_LISTBOX );
        fill_parts( TRUE );

    case WM_OVERLAY_CHANGE:
        fill_shots();
        return TRUE;

    case WM_SETFOCUS:
        SetFocus( GetDlgItem(MyWindow, ShotListboxId) );
        return TRUE;

    case WM_SHOWWINDOW:
        /*
        ---------------------------------------------------------------------
        wParam is true if I am being awakened, lParam is 0 if from showwindow
        --------------------------------------------------------------------- */
        if ( (BOOL) wParam &&  int(lParam) == 0 )
            {
            MyPart.set( ComputerName, MachineName, PartName );
            fill_machines( hWnd, CS_MACHINE_LISTBOX );
            fill_parts( TRUE );
            fill_shots();
            }
        return TRUE;

    case WM_INITDIALOG:
        MyWindow = hWnd;
        position_dialog();
        set_listbox_tabs_from_title_positions( hWnd, CS_PART_LISTBOX, ids, 1 );
        set_listbox_tabs_from_title_positions( hWnd, SINGLE_SHOT_LISTBOX, shot_ids, 2 );
        set_listbox_tabs_from_title_positions( hWnd, MULTIPLE_SHOT_LISTBOX, shot_ids, 2 );

        set_listbox_id();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_CLOSE:
        save_position();
        ChosshotWindow = 0;
        MyWindow       = 0;
        DestroyWindow( hWnd );
        fill_statics( MyWindow );
        return TRUE;

    case WM_VKEYTOITEM:
        if ( LOWORD(wParam) == VK_F9 )
            add_to_backup_list();
        return -1;       /* Default action by listbox */

    case WM_COMMAND:
        switch ( id )
            {
            case CS_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    fill_parts( FALSE );
                return TRUE;

            case CS_PART_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    fill_shots();
                return TRUE;

            case SINGLE_SHOT_LISTBOX:
            case MULTIPLE_SHOT_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    choose_these_shots();
                return TRUE;

            case IDCANCEL:
            case OK_BUTTON:
                SendMessage( MyWindow,   WM_CLOSE,   0, 0L );
                return TRUE;
            }
    }

return FALSE;
}
