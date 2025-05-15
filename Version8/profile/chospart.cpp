#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "extern.h"
#include "resource.h"

static TCHAR TabChar = TEXT( '\t' );
static TCHAR NullChar = TEXT( '\0' );

extern HWND MainWindow;
static HWND MyWindow;
void fill_machines( HWND w, UINT listbox_id );

/***********************************************************************
*                         CURRENT_MACHINE_NAME                         *
***********************************************************************/
static TCHAR * current_machine_name()
{
static STRING_CLASS machine_name;
LISTBOX_CLASS lb;

lb.init( MyWindow, CP_MACHINE_LB );
machine_name = lb.selected_text();
if ( machine_name.isempty() )
    machine_name = MachineName;

return machine_name.text();
}

/***********************************************************************
*                              FILL_PARTS                              *
***********************************************************************/
static void fill_parts()
{
COMPUTER_CLASS c;
STRING_CLASS   s;
BOOLEAN        computer_is_online;
LISTBOX_CLASS  lb;

lb.init( MyWindow, CP_PART_LB );

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
            lb.empty();
            return;
            }
        }
    }

if ( MachList.find(current_machine_name()) )
    {
    fill_parts_and_counts_listbox( MyWindow, CP_PART_LB, MachList.computer_name(), MachList.name(), TabChar );
    }
}

/***********************************************************************
*                      COPY_SELECTIONS_TO_EXTERN                       *
***********************************************************************/
static void copy_selections_to_extern()
{
STRING_CLASS  s;
LISTBOX_CLASS lb;
TCHAR       * cp;

lb.init( MyWindow, CP_PART_LB );

if ( MachList.find(current_machine_name()) )
    {
    s = lb.selected_text();
    cp = s.find( TabChar );
    if ( cp )
        *cp = NullChar;
    if ( !s.isempty() )
        {
        ComputerName = MachList.computer_name();
        MachineName =  MachList.name();
        PartName = s;
        }
    }
}

/***********************************************************************
*                           CHOOSE_THIS_PART                           *
***********************************************************************/
static void choose_this_part()
{
copy_selections_to_extern();
SendMessage( MainWindow, WM_NEWPART, 0, 0L );
SendMessage( MyWindow,   WM_CLOSE,   0, 0L );
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
    { IDOK,              TEXT("OK_STRING") },
    { CP_MACHINE_STATIC, TEXT("MACHINE_LABEL_STRING") },
    { CP_PART_STATIC,    TEXT("PART_LABEL_STRING") },
    { CP_SHOTS_STATIC,   TEXT("SHOTS_LABEL_STRING") },
    { CP_PARMS_STATIC,   TEXT("PARAMS_LABEL_STRING") }
    };
const int nr = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    CP_MACHINE_LB,
    CP_PART_LB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nr; i++ )
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
StringTable.find( s, TEXT("CHOOSE_PART_CAPTION") );
w.set_title( s.text() );
w.refresh();
}

/***********************************************************************
*                             SET_MY_TABS                              *
***********************************************************************/
static void set_my_tabs()
{
LISTBOX_CLASS lb;
lb.init( MyWindow, CP_PART_LB );
lb.set_tabs( CP_PARMS_STATIC, CP_SHOTS_STATIC );
}

/***********************************************************************
*                            CHOSPART_PROC                             *
***********************************************************************/
BOOL CALLBACK chospart_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int tabs[2] = { 42, 75 };
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MyWindow = hWnd;
        set_my_tabs();
        return TRUE;

    case WM_DBINIT:
        fill_statics( hWnd );
        fill_machines( MyWindow, CP_MACHINE_LB );
        fill_parts();
        SetFocus( GetDlgItem(MyWindow, CP_MACHINE_LB) );
        return TRUE;

    case WM_CLOSE:
        ShowWindow( hWnd, SW_HIDE );
        return TRUE;

    case WM_SETFOCUS:
        SetFocus( GetDlgItem(MyWindow, CP_MACHINE_LB) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case CP_PART_LB:
                if ( cmd == LBN_DBLCLK )
                    choose_this_part();
                return TRUE;

            case CP_MACHINE_LB:
                if ( cmd == LBN_SELCHANGE )
                    fill_parts();
                return TRUE;

            case IDOK:
                choose_this_part();
                return TRUE;
            }
    }

return FALSE;
}
