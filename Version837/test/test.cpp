#include <windows.h>
#include <string>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\multistring.h"
#include "..\include\setpoint.h"
#include "..\include\vdbclass.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"
#include "resrc1.h"

#define _MAIN_
#include "..\include\chars.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS OkButton;
WINDOW_CLASS MyTbox;
LISTBOX_CLASS MainListbox;

static TCHAR MyClassName[] = TEXT( "Test");

void run_getaddrinfo(HWND hWnd)
{
    STRING_CLASS input;
    char *addr;
    TCHAR taddr[512];
    TCHAR * msg;
    HOSTENT *hostent;
    int error;
    int status;
    WSADATA wsaData;
    STRING_CLASS e;
    STRING s;
    char cbuf[512];

    status = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (status != 0)
    {
        e = TEXT("WSAStartup failed: ");
        e += status;
        MessageBox(NULL, TEXT("Error"), e.text(), MB_OK);
    }

    input.get_text(hWnd, MY_TBOX);
    msg = input.text();
    unicode_to_char(cbuf, msg);
    hostent = gethostbyname(cbuf);
    
    if (hostent)
    {
        addr = inet_ntoa(**(in_addr**)hostent->h_addr_list);
        char_to_tchar(taddr, addr);
        MessageBox(NULL, taddr, TEXT("Address 0"), MB_OK);
    }
    else
    {
        e = WSAGetLastError();
        MessageBox(NULL, e.text(), TEXT("Error"), MB_OK);
    }

    WSACleanup();
}

void run_gethostname()
{
    static const int len = 100;
    char name[len];
    TCHAR tname[len];
    int status;
    int error;
    STRING_CLASS e;
    WSADATA wsaData;

    status = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (status != 0)
    {
        e = TEXT("WSAStartup failed: ");
        e += status;
        MessageBox(NULL, TEXT("Error"), e.text(), MB_OK);
    }

    status = gethostname(name, len);
    char_to_tchar(tname, name);
    if (status == 0)
        MessageBox(NULL, tname, TEXT("gethostname"), MB_OK);
    else
    {        
        error = WSAGetLastError();
        e = error;
        MessageBox(NULL, e.text(), TEXT("WSA Error"), MB_OK);
    }

    WSACleanup();
}

void run_root_from_path( HWND hWnd )
{
    STRING_CLASS input;

    input.get_text( hWnd, MY_TBOX );
    root_from_path( input );
    MessageBox( NULL, input.text(), TEXT("root_from_path"), MB_OK );
}

void process_mailslot_message(HWND hWnd)
{
    STRING_CLASS input;
    TCHAR * cp;
    TCHAR * p;
    TCHAR * msg;

    input.get_text(hWnd, MY_TBOX);
    msg = input.text();

    /*
    -----------------------
    Skip the topic and item
    ----------------------- */
    cp = findchar(CommaChar, msg);
    if (cp)
    {
        cp++;
        cp = findchar(CommaChar, cp);
    }

    if (cp)
    {
        /*
        ---------------------------------------------------------------
        Point the start of the message at the start of the machine name
        --------------------------------------------------------------- */
        cp++;
        msg = cp;

        /*
        -----------------------------------------
        Look for the comma after the machine name
        ----------------------------------------- */
        cp = findchar(CommaChar, cp);
    }

    if (cp)
    {
        *cp = NullChar;
        /*
        ----------------------------
        Find this machine in my list
        ---------------------------- */
        //if (find(msg))
        {
            cp++;
            p = cp;
            cp = findchar(CommaChar, cp);
            if (cp)
            {
                *cp = NullChar;
                cp++;
                if (*cp == NChar)
                    //current_entry->check_for_new_shot(p);
                    MessageBox(NULL, p, TEXT("Part"), MB_OK);
                else
                    //current_entry->check_for_new_downtime();
                    MessageBox(NULL, TEXT("Downtime"),TEXT("No Part"), MB_OK);
            }
        }
    }
}

void new_shot_callback(HWND hWnd)
{ 
    TCHAR * cp;
    STRING_CLASS part;
    STRING_CLASS s;

    s.get_text(hWnd, MY_TBOX);

    if (s.next_field())
    {
        MessageBox(NULL, s.text(), TEXT("Machine"), MB_OK);
        /*
        ----------------------------
        Find this machine in my list
        ---------------------------- */
        //if (find(s.text()))
        {
            if (s.next_field())
            {
                part = s;
                if (s.next_field())
                {
                    if (s == TEXT("NS"))
                        MessageBox(NULL, part.text(), TEXT("Machine"), MB_OK);
                    else
                        MessageBox(NULL, TEXT("No Shot"), TEXT("Downtime"), MB_OK);
                }
            }
        }
    }
}

/***********************************************************************
*                               ADD_LINES                              *
***********************************************************************/
static void add_lines()
{
TCHAR sorc[] = TEXT( "c:\\sources\\version8\\test\\results\\shotparm.txt" );
FIELDOFFSET fo;
long     i;
int      pi;
DB_TABLE t;
NAME_CLASS s;
SYSTEMTIME   st;

s = sorc;

if ( t.open(s, SHOTPARM_RECLEN, PWL) )
    {
    t.empty();
    for ( i=0; i<100001; i++ )
        {
        GetLocalTime( &st );
        t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, i, SHOT_LEN );
        t.put_date( SHOTPARM_DATE_OFFSET, st );
        t.put_time( SHOTPARM_TIME_OFFSET, st );
        t.put_short( SHOTPARM_SAVE_FLAGS_OFFSET, (short) 3, SHOTPARM_SAVE_FLAGS_LEN );
        fo = SHOTPARM_PARAM_1_OFFSET;
        for ( pi=0; pi<MAX_PARMS; pi++ )
            {
            t.put_double( fo, 0.0, SHOTPARM_FLOAT_LEN );
            fo += SHOTPARM_FLOAT_LEN+1;
            }

        t.rec_append();
        }
    t.close();
    }
}

/***********************************************************************
*                           CREATE_FILES                               *
************************************************************************/
static void create_files()
{
TCHAR sorc[] = TEXT( "c:\\sources\\version8\\test\\results\\000001.pr2" );
TCHAR myroot[] = TEXT( "c:\\sources\\version8\\test\\results" );
int i;

NAME_CLASS s;
NAME_CLASS sn;
NAME_CLASS d;
s = sorc;

for ( i=2; i<10001; i++)
    {
    sn = i;
    sn.rjust( 6, ZeroChar );
    sn += TEXT( ".pr2" );
    d = myroot;
    d.cat_path( sn );
    s.copyto( d );
    }
}

/***********************************************************************
*                              READ_FILES                              *
***********************************************************************/
static void read_files()
{
TCHAR sorc[] = TEXT( "c:\\sources\\version7\\test\\results\\000001.csv" );
TCHAR myroot[] = TEXT( "c:\\sources\\version7\\test\\results" );
int i;
DWORD t1;
DWORD t2;
SETPOINT_CLASS s;
NAME_CLASS sn;
NAME_CLASS d;

t1 = GetTickCount();
for ( i=1; i<1001; i++)
    {
    sn = i;
    sn.rjust( 6, ZeroChar );
    sn += TEXT( ".csv" );
    d = myroot;
    d.cat_path( sn );
    s.get( d );
    }
t2 = GetTickCount();

sn = (unsigned int) t1;
d  = (unsigned int) t2;
MessageBox( 0, d.text(), sn.text(), MB_OK );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
STRING_CLASS  s;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case IDOK:
                if ( cmd == BN_CLICKED )
                    {
                    cmd = GetDlgItemInt( hWnd, MY_TBOX, 0, 0);
                    }
                return 0;

            case CREATE_CHOICE:
                add_lines();
                return 0;

            case DELETE_CHOICE:
                return 0;

            case EXIT_CHOICE:
                MainWindow.close();
                return 0;

            case ID_RUN_NEW:
                new_shot_callback(hWnd);
                return 0;

            case ID_RUN_PROCESS:
                process_mailslot_message(hWnd);
                return 0;

            case ID_RUN_GETADDRINFO:
                run_getaddrinfo(hWnd);
                return 0;

            case ID_RUN_GETHOSTNAME:
                run_gethostname();
                return 0;

            case ID_RUN_ROOT:
                run_root_from_path(hWnd);
                return 0;
            }

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
WNDCLASS wc;
HWND     w;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

w = CreateWindow(
    MyClassName,
    TEXT("Main Window Title"),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 300,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MyTbox = CreateWindow (
    TEXT("edit"),
    TEXT("0"),
    WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
    0, 0,
    200, 20,
    w,
    (HMENU) MY_TBOX,
    MainInstance,
    NULL
    );

OkButton = CreateWindow (
    TEXT("button"),
    TEXT("OK"),
    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
    201, 0,
    30, 20,
    w,
    (HMENU) IDOK,
    MainInstance,
    NULL
    );

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
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
