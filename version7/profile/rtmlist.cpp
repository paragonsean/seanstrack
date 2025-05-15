#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\v5help.h"

#include "rtmlist.h"
#include "resource.h"
#include "extern.h"

static TCHAR NullChar   = TEXT( '\0' );
static TCHAR TabChar    = TEXT( '\t' );
static TCHAR XStr[]     = TEXT( "X" );
static TCHAR SpaceStr[] = TEXT( " " );

static HWND MyWindow = 0;
static WNDPROC OldLbProc;

static const TCHAR RT_MACHINE_LIST_FILE[] = TEXT( "rtmach.lst" );
static const TCHAR V5HELP_FILE[]          = TEXT( "v5help.hlp" );

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
NAME_CLASS s;

if ( s.get_exe_dir_file_name(V5HELP_FILE) )
    {
    WinHelp( MyWindow, s.text(), helptype, context );
    }
}

/***********************************************************************
*                                 LOAD                                 *
***********************************************************************/
BOOLEAN REALTIME_MACHINE_LIST_CLASS::load()
{
NAME_CLASS s;
FILE_CLASS f;
TCHAR * cp;

empty();

s.get_exe_dir_file_name( RT_MACHINE_LIST_FILE );

if ( f.open_for_read(s.text()) )
    {
    while ( true )
        {
        cp = f.readline();
        if ( !cp )
            break;
        append( cp );
        }
    f.close();
    }

return TRUE;
}

/***********************************************************************
*                                 SAVE                                 *
***********************************************************************/
BOOLEAN REALTIME_MACHINE_LIST_CLASS::save()
{
NAME_CLASS s;
FILE_CLASS f;

s.get_exe_dir_file_name( RT_MACHINE_LIST_FILE );
if ( f.open_for_write(s.text()) )
    {
    rewind();
    while ( next() )
        f.writeline( text() );
    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes()
{
LISTBOX_CLASS lb;
STRING_CLASS  s;
TCHAR       * cp;
int32         i;
int32         n;

RealtimeMachineList.empty();

lb.init( MyWindow, RT_MACHINE_LISTBOX );

n = lb.count();

for ( i=0; i<n; i++ )
    {
    s = lb.item_text( i );
    if ( !s.isempty() )
        {
        cp = s.text() + s.len();
        cp--;
        if ( *cp == *XStr )
            {
            cp--;
            *cp = NullChar;
            RealtimeMachineList.append( s.text() );
            }
        }
    }

RealtimeMachineList.save();
}

/***********************************************************************
*                         TOGGLE_LISTBOX_STRING                        *
***********************************************************************/
static void toggle_listbox_string()
{
LISTBOX_CLASS lb;
TCHAR       * cp;
TCHAR         buf[MAX_PATH+1];
int32         slen;

lb.init( MyWindow, RT_MACHINE_LISTBOX );

lstrcpy( buf, lb.selected_text() );
slen = lstrlen( buf );
if ( slen )
    {
    cp = buf + slen;
    cp--;
    if ( *cp == *XStr )
        *cp = *SpaceStr;
    else
        *cp = *XStr;

    lb.replace( buf );
    }

lb.setcursel( NO_INDEX );
}

/***********************************************************************
*                        MACHINE_LISTBOX_STRING                        *
***********************************************************************/
static TCHAR * machine_listbox_string( STRING_CLASS & machine, TCHAR default_char )
{
static STRING_CLASS buf;

buf = machine;
buf += TabChar;

if ( default_char == NullChar )
    {
    if ( RealtimeMachineList.find(machine.text())  )
        buf += XStr;
    else
        buf += SpaceStr;
    }
else
    {
    buf += default_char;
    }

return buf.text();
}

/***********************************************************************
*                             FILL_LISTBOX                             *
***********************************************************************/
static void fill_listbox( TCHAR default_char )
{
LISTBOX_CLASS lb;
BOOLEAN monitoring;
COMPUTER_CLASS c;

monitoring = FALSE;

static TCHAR MonitorClassName[] = MONITOR_CLASS_NAME;

if ( FindWindow(MonitorClassName, 0) )
    monitoring = TRUE;

lb.init( MyWindow, RT_MACHINE_LISTBOX );
lb.empty();

MachList.rewind();

while( MachList.next() )
    {
    if ( monitoring )
        {
        if ( MachList.computer_name() == c.whoami() )
            continue;
        }
    lb.add( machine_listbox_string(MachList.name(), default_char) );
    }

lb.setcursel( NO_INDEX );
}

/***********************************************************************
*                              SUB_PROC                                *
***********************************************************************/
LRESULT CALLBACK sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
INT i;
LISTBOX_CLASS lb;

if ( msg == WM_KEYDOWN )
    {
    if ( wParam == VK_UP || wParam == VK_DOWN )
        {
        lb.init( MyWindow, RT_MACHINE_LISTBOX );
        i = lb.current_index();
        if ( wParam == VK_UP && i > 0 )
            i--;
        else if ( wParam == VK_DOWN && i < (lb.count() - 1) )
            i++;
        else
            return 0;

        lb.set_current_index( i );
        SetFocus( w );
        return 0;
        }
    }

return CallWindowProc( OldLbProc, w, msg, wParam, lParam );
}

/***********************************************************************
*                               SET_TABS                               *
***********************************************************************/
static void set_tabs()
{
LISTBOX_CLASS lb;
lb.init( MyWindow, RT_MACHINE_LISTBOX );
lb.set_tabs( RT_DISPLAY_STATIC, RT_DISPLAY_STATIC );
}

/***********************************************************************
*                       REALTIME_MACHINE_LIST_PROC                     *
***********************************************************************/
BOOL CALLBACK realtime_machine_list_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;
HWND w;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MyWindow = hWnd;
        w = GetDlgItem( MyWindow, RT_MACHINE_LISTBOX );
        OldLbProc = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) sub_proc );
        set_tabs();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_listbox( NullChar );
        SetFocus( GetDlgItem(MyWindow, RT_MACHINE_LISTBOX) );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, RTML_MENU_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case RT_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    toggle_listbox_string();
                return TRUE;

            case RT_DISPLAY_ALL_BUTTON:
                fill_listbox( *XStr );
                w = GetDlgItem( MyWindow, RT_MACHINE_LISTBOX );
                SetFocus( w );
                return TRUE;

            case RT_DISPLAY_NONE_BUTTON:
                fill_listbox( *SpaceStr );
                w = GetDlgItem( MyWindow, RT_MACHINE_LISTBOX );
                SetFocus( w );
                return TRUE;

            case IDOK:
                save_changes();

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case IDCANCEL:
    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}