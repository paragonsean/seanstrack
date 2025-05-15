#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\structs.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "extern.h"
#include "resource.h"

extern HWND MainWindow;

static INT * SelectList  = 0;
static int   SelectCount = 0;
static int   SelectIndex = 0;

const static TCHAR NullChar  = TEXT( '\0' );
const static TCHAR SpaceChar = TEXT( ' ' );
const static TCHAR TabChar   = TEXT( '\t' );

void print_me();

/***********************************************************************
*                              FILL_SHOTS                              *
***********************************************************************/
static void fill_shots()
{
STRING_CLASS    buf;
NAME_CLASS      s;
DB_TABLE        t;
LISTBOX_CLASS   lb;
PART_NAME_ENTRY p;

lb.init( MultipleShotDialogWindow, MP_PART_LISTBOX );

p.part = lb.selected_text();
if ( p.part.isempty() )
    return;

p.part.replace( TabChar, NullChar );

if ( p.part.isempty() )
    return;

hourglass_cursor();

p.computer = MachList.computer_name();
p.machine  = MachList.name();

lb.init( MultipleShotDialogWindow, MP_SHOT_LISTBOX );
lb.redraw_off();
lb.empty();

s.get_graphlst_dbname( p.computer, p.machine, p.part );
if ( t.open(s, 0, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( s, GRAPHLST_SHOT_NAME_INDEX );
        s += SpaceChar;

        t.get_alpha( buf, GRAPHLST_DATE_INDEX );
        s.cat_w_char( buf, SpaceChar );

        t.get_alpha( buf, GRAPHLST_TIME_INDEX );
        s += buf;
        lb.add( s.text() );
        }

    t.close();
    }

lb.redraw_on();
restore_cursor();
}

/***********************************************************************
*                            FILL_MACHINES                             *
***********************************************************************/
void fill_machines( HWND w, UINT listbox_id )
{
LISTBOX_CLASS lb;

hourglass_cursor();

lb.init( w, listbox_id );
lb.empty();
lb.redraw_off();

MachList.rewind();
while ( MachList.next() )
    lb.add( MachList.name().text() );

lb.setcursel( MachineName.text() );
lb.redraw_on();

restore_cursor();
}

/***********************************************************************
*                         CURRENT_MACHINE_NAME                         *
***********************************************************************/
static TCHAR * current_machine_name()
{
static STRING_CLASS machine_name;

LISTBOX_CLASS lb;

lb.init( MultipleShotDialogWindow, MP_MACHINE_LISTBOX );

machine_name = lb.selected_text();
if ( machine_name.isempty() )
    machine_name = MachineName;

return machine_name.text();
}

/***********************************************************************
*                           GET_CURRENT_PART                           *
***********************************************************************/
BOOLEAN get_current_part( PART_NAME_ENTRY & p )
{
LISTBOX_CLASS lb;

if ( !MultipleShotDialogWindow )
    return FALSE;

if ( !MachList.find(current_machine_name()) )
    return FALSE;

p.computer = MachList.computer_name();
p.machine  = MachList.name();

lb.init( MultipleShotDialogWindow, MP_PART_LISTBOX );
p.part = lb.selected_text();
replace_char_with_null( p.part.text(), TabChar );
if ( p.part.isempty() )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                              FILL_PARTS                              *
***********************************************************************/
static void fill_parts()
{
LISTBOX_CLASS lb;

lb.init( MultipleShotDialogWindow, MP_SHOT_LISTBOX );
lb.empty();

if ( MachList.find(current_machine_name()) )
    fill_parts_and_shot_count_listbox( MultipleShotDialogWindow, MP_PART_LISTBOX, MachList.computer_name(), MachList.name() );

lb.init( MultipleShotDialogWindow, MP_PART_LISTBOX );
lb.setcursel( -1 );
}

/***********************************************************************
*                           PRINT_NEXT_SHOT                            *
***********************************************************************/
void print_next_shot()
{
LISTBOX_CLASS lb;
INT     i;
STRING_CLASS s;

if ( SelectCount > 0 )
    {
    if ( SelectIndex >= SelectCount )
        {
        PrintingMultiplePlots = FALSE;

        if ( SelectList )
            {
            delete[] SelectList;
            SelectList = 0;
            }

        SelectCount = 0;
        SelectIndex = 0;
        StringTable.find( s, TEXT("BEGIN_PRINTING") );
        s.set_text( MultipleShotDialogWindow, MP_OK_BUTTON );
        resource_message_box( MainInstance, PRINTING_COMPLETE_STRING, MULTIPLE_PRINT_STRING );
        return;
        }

    lb.init( MultipleShotDialogWindow, MP_SHOT_LISTBOX );

    /*
    ------------------------------------
    Unhighlight the last profile printed
    ------------------------------------ */
    i = SelectIndex - 1;
    if ( i >= 0 )
        {
        i = SelectList[i];
        lb.set_select( i, FALSE );
        }

    i = SelectList[SelectIndex];
    lb.set_select( i, TRUE );
    ShotName = lb.item_text( i );
    if ( !ShotName.isempty() )
        {
        ShotName.replace( SpaceChar, NullChar );
        fix_shotname( ShotName );

        if ( SelectIndex == 0 )
            {
            StringTable.find( s, TEXT("STOP_PRINTING") );
            s.set_text( MultipleShotDialogWindow, MP_OK_BUTTON );
            }

        PrintingMultiplePlots = TRUE;
        PostMessage( MainWindow, WM_NEWSHOT, 0, 0L );
        }
    SelectIndex++;
    }
}

/***********************************************************************
*                      CHECK_FOR_NEWSHOT_TO_SHOW                       *
***********************************************************************/
static void check_for_newshot_to_show()
{
LISTBOX_CLASS   lb;
PART_NAME_ENTRY p;

INT * plist;
INT i;
INT n;

if ( PrintingMultiplePlots )
    return;

lb.init( MultipleShotDialogWindow, MP_SHOT_LISTBOX );
n = lb.get_select_list( &plist );
if ( n == 1 )
    {
    if ( get_current_part(p) )
        {
        i = lb.current_index();
        if ( lb.is_selected(i) )
            {
            ShotName = lb.item_text(i);
            if ( !ShotName.isempty() )
                {
                ComputerName = p.computer;
                MachineName  = p.machine;
                PartName     = p.part;
                ShotName.replace( SpaceChar, NullChar );
                fix_shotname( ShotName );
                PostMessage( MainWindow, WM_NEWSHOT, 0, 0L );
                }
            }
        }
    }

if ( n > 0 && plist )
    {
    if ( plist )
        delete[] plist;
    }
}

/***********************************************************************
*                            BEGIN_PRINTING                            *
***********************************************************************/
void begin_printing()
{
LISTBOX_CLASS lb;
PART_NAME_ENTRY p;

if ( get_current_part(p) )
    {
    lb.init( MultipleShotDialogWindow, MP_SHOT_LISTBOX );
    SelectCount = lb.get_select_list( &SelectList );
    if ( SelectCount > 0 )
        {
        lb.set_select( -1, FALSE );

        SelectIndex = 0;
        ComputerName = p.computer;
        MachineName  = p.machine;
        PartName     = p.part;

        PostMessage( MultipleShotDialogWindow, WM_NEWSHOT, 0, 0L );
        }
    }
}

/***********************************************************************
*                             SET_MP_TABS                              *
***********************************************************************/
static void set_mp_tabs()
{
LISTBOX_CLASS  lb;

lb.init( MultipleShotDialogWindow, MP_PART_LISTBOX );
lb.set_tabs( MP_SHOTS_TBOX, MP_SHOTS_TBOX );
}

/***********************************************************************
*                    SET_PRINT_MULTIPLE_SETUP_STRINGS                  *
***********************************************************************/
static void set_print_multiple_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { MP_OK_BUTTON, TEXT("BEGIN_PRINTING") },
    { MP_SHOT_NAME_STATIC, TEXT("SHOT_NUMBER_STRING") },
    { MP_DATE_STATIC,      TEXT("DATE_STRING") },
    { MP_MACHINE_STATIC,   TEXT("MACHINE_STRING") },
    { MP_PART_STATIC,      TEXT("PART_STRING") },
    { MP_SHOTS_TBOX,       TEXT("SHOTS_STRING") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    MP_MACHINE_LISTBOX,
    MP_PART_LISTBOX,
    MP_SHOT_LISTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    StringTable.find( s, rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("PRINT_MULTIPLE")) );
w.refresh();
}

/***********************************************************************
*                      PRINT_MULTIPLE_SHOTS_PROC                       *
***********************************************************************/
BOOL CALLBACK print_multiple_shots_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_DBINIT:
        fill_machines( MultipleShotDialogWindow, MP_MACHINE_LISTBOX );
        fill_parts();
        SetFocus( GetDlgItem(MultipleShotDialogWindow, MP_MACHINE_LISTBOX) );
        return TRUE;

    case WM_INITDIALOG:
        MultipleShotDialogWindow = hWnd;
        set_print_multiple_setup_strings( hWnd );
        set_mp_tabs();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_SETFOCUS:
        SetFocus( GetDlgItem(MultipleShotDialogWindow, MP_MACHINE_LISTBOX) );
        return TRUE;

    case WM_NEWSHOT:
        print_next_shot();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;

            case MP_SHOT_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    check_for_newshot_to_show();
                return TRUE;

            case MP_PART_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    if ( MachList.find(current_machine_name()) )
                        fill_shots();
                return TRUE;

            case MP_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    fill_parts();
                return TRUE;

            case MP_OK_BUTTON:
                if ( SelectCount > 0 )
                    {
                    SelectIndex = SelectCount;
                    print_next_shot();
                    }
                else
                    begin_printing();
                return TRUE;
            }
        break;

    case WM_DESTROY:
        DialogIsActive = FALSE;
        MultipleShotDialogWindow = 0;
        break;
    }

return FALSE;
}
