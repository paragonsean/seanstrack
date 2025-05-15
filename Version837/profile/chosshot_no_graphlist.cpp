#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\structs.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "extern.h"

#define SINGLE_SHOT_LISTBOX   101
#define MULTIPLE_SHOT_LISTBOX 102
#define OK_BUTTON         1
#define ZABISH_SCREEN_TYPE    2

INT ShotListboxId = SINGLE_SHOT_LISTBOX;

static long LastX = 40;
static long LastY = 50;

PART_NAME_ENTRY MyPart;

static HWND   MyWindow;

static short NSelected;

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
TCHAR         * cp;
WIN32_FIND_DATA fdata;
HANDLE          fh;
int             i;
bool            is_shot_file;
int             nof_selected_shots;
NAME_CLASS      s;
STRING_CLASS    selected_shot_number[MAX_OVERLAY_SHOTS];
STRING_CLASS    shot_number;
SYSTEMTIME      st;
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

s.get_part_results_dir_file_name( MyPart.computer, MyPart.machine, MyPart.part, StarDotStar );

fh = FindFirstFile( s.text(), &fdata );
if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( true )
        {
        cp = fdata.cFileName;
        if ( *cp != PeriodChar )
            {
            is_shot_file = false;
            if ( !(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
                {
                /*
                ---------------------------
                See if this is a shot file.
                --------------------------- */
                while ( *cp != PeriodChar && *cp != NullChar )
                    cp++;
                if ( *cp != NullChar )
                    {
                    cp++;
                    if ( *cp == PChar || *cp == PCharLow )
                        {
                        cp++;
                        if ( *cp == RChar || *cp == RCharLow )
                            {
                            cp++;
                            if ( *cp == TwoChar || *cp == OChar || *cp == OCharLow )
                                is_shot_file = true;
                            }
                        }
                    }
                }
            if ( is_shot_file )
                {
                s = fdata.cFileName;
                cp = s.find( PeriodChar );
                if ( cp )
                    *cp = NullChar;
                s += TabChar;

                shot_number = s;

                FileTimeToSystemTime( &fdata.ftCreationTime, &st );
                s += alphadate( st );
                s += SpaceChar;
                s += alphatime( st );

                x = SendMessage( w, LB_ADDSTRING, 0, (LPARAM)(LPSTR) s.text() );
                if ( x != LB_ERR && NSelected < nof_selected_shots )
                    {
                    for ( i=0; i<nof_selected_shots; i++ )
                        {
                        if ( shot_number == selected_shot_number[i] )
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
            }
        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );
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
HWND       w;
LRESULT    n;
LRESULT    x;
INT        i;
TCHAR      s[SHOT_LEN+1+ALPHADATE_LEN+1+ALPHATIME_LEN+1];

w = GetDlgItem( MyWindow, ShotListboxId );

x = SendMessage( w, LB_GETCARETINDEX, 0, 0L );
if ( x != LB_ERR )
    {
    i = (INT) x;
    if ( ShotListboxId == MULTIPLE_SHOT_LISTBOX )
        {
        n = SendMessage( w, LB_GETSELCOUNT, 0, 0L );
        if ( n == LB_ERR )
            return;

        x = SendMessage( w, LB_GETSEL,      i, 0L );
        if ( x != LB_ERR )
            {
            if ( x == 0 )
                {
                /*
                -------------------------------------------------
                This entry has just been unselected, if it is the
                last entry, reselect it
                ------------------------------------------------- */
                if ( n == 0 )
                    {
                    SendMessage( w, LB_SETSEL, TRUE, i );
                    NSelected = 1;
                    return;
                    }
                }

            else if ( n > MAX_OVERLAY_SHOTS )
                {
                /*
                ------------------------------------------------------
                This entry has just been selected but there is no room
                ------------------------------------------------------ */
                SendMessage( w, LB_SETSEL, FALSE, i );
                NSelected--;
                resource_message_box( MyWindow, MainInstance, DEL_SOME_FIRST_STRING, MAX_OVERLAY_REACHED_STRING, MB_OK );
                return;
                }
            }
        }

    x = SendMessage( w, LB_GETTEXT, i, (LPARAM)(LPSTR) s );
    if ( x != LB_ERR )
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
static HFONT      fonthandle = INVALID_FONT_HANDLE;
static FONT_CLASS my_font;
static STRING_CLASS my_language;

struct RESOURCE_TEXT_ENTRY {
    UINT id;
    TCHAR * resource_name;
    };

static RESOURCE_TEXT_ENTRY rlist[] =
    {
    { CS_SHOT_NUMBER_STATIC, TEXT("SHOT_NUMBER_STRING") },
    { CS_MACHINE_STATIC, TEXT("MACHINE_LABEL_STRING") },
    { CS_PART_STATIC, TEXT("PART_LABEL_STRING") },
    { CS_SHOTS_STATIC, TEXT("SHOTS_LABEL_STRING") },
    { CS_DATE_STATIC, TEXT("DATE_NAME_STRING") }
    };
const int nr = sizeof(rlist)/sizeof(RESOURCE_TEXT_ENTRY);
int          i;
long         font_height;
WINDOW_CLASS w;
LPARAM       lParam;
HFONT        old_fonthandle;

if ( !hWnd )
    {
    if ( fonthandle != INVALID_FONT_HANDLE )
        {
        FontList.free_handle( fonthandle );
        fonthandle = INVALID_FONT_HANDLE;
        *my_font.lfFaceName = NullChar;
        }
    return;
    }

old_fonthandle = INVALID_FONT_HANDLE;

if ( my_font == StringTable.fontclass() && my_language == CurrentLanguage )
    return;

my_language = CurrentLanguage;

if ( fonthandle != INVALID_FONT_HANDLE )
    old_fonthandle = fonthandle;

/*
-----------------------------------
Get the height of the existing font
----------------------------------- */
fonthandle = (HFONT) SendMessage( GetDlgItem(hWnd, rlist[0].id), WM_GETFONT, 0, 0 );
if ( fonthandle != 0 )
    {
    i = GetObject( (HGDIOBJ) fonthandle, sizeof(my_font), (LPVOID) &my_font );
    if ( i != 0 )
        {
        font_height = my_font.lfHeight;
        my_font = StringTable.fontclass();
        my_font.lfHeight = font_height;
        my_font.lfWidth  = 0;
        fonthandle = FontList.get_handle( my_font );
        if ( fonthandle != INVALID_FONT_HANDLE )
            {
            lParam = MAKELPARAM( FALSE, 0 );
            for ( i=0; i<nr; i++ )
                {
                w = GetDlgItem( hWnd, rlist[i].id );
                w.post( WM_SETFONT  , (WPARAM) fonthandle, lParam );
                w.set_title( StringTable.find(rlist[i].resource_name) );
                }

            lParam = MAKELPARAM( TRUE, 0 );
            w = hWnd;
            w.set_title( StringTable.find(TEXT("CHOOSE_SHOTS_CAPTION")) );
            InvalidateRect( hWnd, NULL, TRUE );
            }
        }
    }

if ( old_fonthandle != INVALID_FONT_HANDLE )
    FontList.free_handle( old_fonthandle );
}

/***********************************************************************
*                            CHOSSHOT_PROC                             *
***********************************************************************/
BOOL CALLBACK chosshot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int tabs[1] = { 100 };
static int ids[1] = { CS_SHOTS_STATIC };
static int shot_ids[1] = { CS_DATE_STATIC };
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
        set_listbox_tabs_from_title_positions( hWnd, SINGLE_SHOT_LISTBOX, shot_ids, 1 );
        set_listbox_tabs_from_title_positions( hWnd, MULTIPLE_SHOT_LISTBOX, shot_ids, 1 );

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
