#include <windows.h>
#include <commctrl.h>

#define _MAIN_

#include <map>
#include <vector>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\fifo.h"
#include "..\include\genlist.h"
#include "..\include\events.h"
#include "..\include\servicelog.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\socket_client.h"
#include "..\include\socket_server.h"
#include "..\include\fileclas.h"

#include "resource.h"
#include "extern.h"

#define NEW_AUTO_SHOT_INDEX 0
#define VIEW_VISI_CHOICE    205
#define CLEAR_EVENTS_CHOICE 204
#define SHOW_STATUS_CHOICE  207
#define MINIMIZE_CHOICE     107
#define EXIT_CHOICE         108
#define SHOT_NUMBER_EDITBOX 102
#define MESSAGE_LISTBOX_ID  101
#define WM_USER_INITIATE    WM_USER+0x101
#define SHUTDOWN_TIMER      1
#define SHUTDOWN_DELAY      5000

SERVICE_LOG ServiceLog;

HWND      MainDialogWindow = 0;
HWND      MessageListbox   = 0;

TCHAR        MyClassName[] = VISITRAK_EVENT_MANAGER;
STRING_CLASS MyWindowTitle;

SOCKET_SERVER Server;
TCHAR     ServerName[] = VISITRAK_EVENT_MANAGER;

UINT      ViewVisiState = MF_UNCHECKED;
BOOLEAN   DisplayingMessages = FALSE;

FIFO_CLASS   DaNotifyFifo;
HANDLE       DaThreadEvent;

FIFO_CLASS   SendFifo;
HANDLE       SendEvent;
bool         RunSendThread;

CRITICAL_SECTION MyCriticalSection;

int                IpPort;
TCHAR              IpAddr[TCP_ADDRESS_LEN+1];
int                DsIpPort;
TCHAR              DsIpAddr[TCP_ADDRESS_LEN+1];
SOCKET_CLIENT      DaClient;
static TCHAR       CommaSpace[] = TEXT( ", " );
static const DWORD MS_TO_WAIT = 505;

/*
----------------------
Startup Notify Clients
---------------------- */
std::map <STRING, SOCKET_CLIENT> StartupClients;

class SEND_FIFO_ENTRY
{
    public:

    SEND_FIFO_ENTRY(){};
    ~SEND_FIFO_ENTRY(){};
    SOCKET socket;
    SOCKET_MSG msg;
    void operator=( const SEND_FIFO_ENTRY &sorc );
};

/***********************************************************************
*                             SEND_FIFO_ENTRY                          *
*                                OPERATOR=                             *
***********************************************************************/
void SEND_FIFO_ENTRY::operator=( const SEND_FIFO_ENTRY &sorc )
{
    msg = sorc.msg;
    socket = sorc.socket;
}

class SOCKET_ITEM_TOPIC_ENTRY
{
    public:

    SOCKET_ITEM_TOPIC_ENTRY(){};
    ~SOCKET_ITEM_TOPIC_ENTRY(){};
    void operator=( const SOCKET_ITEM_TOPIC_ENTRY &sorc );
    TCHAR default_value[DEFAULT_BUFLEN];
    std::vector <SOCKET> sockets;
};

/***********************************************************************
*                         SOCKET_ITEM_TOPIC_ENTRY                      *
*                                OPERATOR=                             *
***********************************************************************/
void SOCKET_ITEM_TOPIC_ENTRY::operator=( const SOCKET_ITEM_TOPIC_ENTRY &sorc )
{
    copystring( default_value, sorc.default_value );
    sockets = sorc.sockets;
}

class SOCKET_ITEM_ENTRY
{
    public:

    SOCKET_ITEM_ENTRY(){};
    ~SOCKET_ITEM_ENTRY(){};
    void operator=( const SOCKET_ITEM_ENTRY &sorc );
    TCHAR name[DEFAULT_BUFLEN];
    std::map <STRING, SOCKET_ITEM_TOPIC_ENTRY> topics;
};

SOCKET_ITEM_ENTRY EventItemList[NOF_EVENT_ITEMS];

/***********************************************************************
*                           SOCKET_ITEM_ENTRY                          *
*                                OPERATOR=                             *
***********************************************************************/
void SOCKET_ITEM_ENTRY::operator=(const SOCKET_ITEM_ENTRY &sorc)
{
    copystring(name, sorc.name);
    topics = sorc.topics;
}

/***********************************************************************
*                             SHOW_MESSAGE                             *
***********************************************************************/
void show_message( const TCHAR * sorc )
{
    if ( MessageListbox && DisplayingMessages )
    {
        SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM)sorc );
    }
}

/***********************************************************************
*                             SEND_THREAD                              *
***********************************************************************/
void send_thread( void * )
{
    SEND_FIFO_ENTRY * entry;
    STRING_CLASS log_entry;
    int i;
    
    while ( RunSendThread )
    {
        if ( SendFifo.count() > 0 )
        {
            entry = (SEND_FIFO_ENTRY*)SendFifo.pop();
            send( entry->socket, (char*)&entry->msg, sizeof(SOCKET_MSG), 0 );
            ResetEvent( SendEvent );
            delete entry;
        }

        if ( WaitForSingleObject(SendEvent, MS_TO_WAIT) == WAIT_FAILED )
        {
            log_entry = TEXT( "ERROR: send_thread() unable to continue." );
            ServiceLog.add_entry( log_entry );
        }
    }

    for ( i=0; i<SendFifo.count(); i++ )
    {
        entry = (SEND_FIFO_ENTRY*)SendFifo.pop();
        delete entry;
    }
}

/***********************************************************************
*                             DS_NOTIFY                                *
***********************************************************************/
void da_notify()
{
    SOCKET_MSG * msg;
   
    if ( DaNotifyFifo.count() == 0 )
        return;

    msg = ( SOCKET_MSG* )DaNotifyFifo.pop();

    if ( DaClient.connect_to_server() )
    {
        DaClient.send_to_server( (char*)msg, sizeof(SOCKET_MSG) );
        closesocket( DaClient.client_socket );
        DaClient.client_socket = INVALID_SOCKET;
    }

    delete msg;
}

/***********************************************************************
*                         DS_NOTIFY_THREAD                             *
***********************************************************************/
void * da_notify_thread( void * )
{
    STRING_CLASS log_entry;

    while ( TRUE )
    {
        da_notify();

        if ( WaitForSingleObject(DaThreadEvent, MS_TO_WAIT) == WAIT_FAILED )
        {
            log_entry = TEXT("ERROR: da_notify_thread() unable to continue.");
            ServiceLog.add_entry(log_entry);
        }

        ResetEvent( DaThreadEvent );
    }
}

/***********************************************************************
*                     SETUP_DATA_ARCHIVER_CLIENT                       *
***********************************************************************/
void setup_data_archiver_client()
{
    DWORD id;

    network_get_ip_addr( DsIpAddr, IP_TYPE_DATA_ARCHIVER );
    DsIpPort = network_get_ip_port( IP_TYPE_DATA_ARCHIVER );
    DaClient.set_ip_addr( DsIpAddr );
    DaClient.set_port( DsIpPort );
    DaClient.msg_size = sizeof( SOCKET_MSG );
    
    /*
    -------------------------------------
    Create Data Archiver Thread and Event
    ------------------------------------- */
    DaThreadEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

    CreateThread(
        NULL,                                       /* no security attributes        */
        0,                                          /* use default stack size        */
        ( LPTHREAD_START_ROUTINE )da_notify_thread, /* thread function               */
        (void*)NULL,                                /* argument to thread function   */
        0,                                          /* use default creation flags    */
        &id);
}

/***********************************************************************
*                             STARTUP_NOTIFY                           *
***********************************************************************/
void startup_notify()
{
    COMPUTER_CLASS c;
    STRING_CLASS s;
    SOCKET_MSG msg;

    msg.cmd = POKE_DATA_ITEM;
    msg.item = MONITOR_STARTUP_INDEX;
    copystring( msg.data,  EmptyString );
    copystring( msg.topic, DEFAULT_TOPIC );
    c.startup();
    c.rewind();

    while ( c.next() )
    {
        if ( !c.is_this_computer() )
        {
            s = c.name();
            root_from_path( s );
            StartupClients[s.text()].set_ip_addr( s.text() );
            StartupClients[s.text()].set_port( network_get_ip_port(IP_TYPE_LOCAL) );
            StartupClients[s.text()].msg_size = sizeof( SOCKET_MSG );
            if ( StartupClients[s.text()].connect_to_server() )
            {
                StartupClients[s.text()].start_write_thread();
                StartupClients[s.text()].msg_fifo.push( msg );
                StartupClients[s.text()].send_and_close = TRUE;
                SetEvent( StartupClients[s.text()].send_event );
            }
        }
    }
}

/***********************************************************************
*                          ACCEPT_CONNECTION                           *
***********************************************************************/
void accept_connection( void * socket )
{
    int i; 
    SOCKET client_socket;
    SOCKET_MSG msg;
    SOCKET_MSG * sm;
    STRING s;
    STRING_CLASS e;
    std::vector<SOCKET>::iterator socket_itr;
    std::vector<SOCKET> send_list;
    std::map<STRING, SOCKET_ITEM_TOPIC_ENTRY>::iterator topic_itr;
    SEND_FIFO_ENTRY *entry;

    client_socket = void_ptr_to_socket( socket );

    while ( TRUE )
    { 
        if ( recv_msg(client_socket, &msg) )
        {
            switch ( msg.cmd )
            {
            case REGISTER_FOR_ITEM:
                s = TEXT( "REGISTER_FOR_ITEM: " );
                s += EventItemList[msg.item].name;
                s += CommaSpace;
                s += msg.topic;
                show_message( s.c_str() );
                EnterCriticalSection( &MyCriticalSection );
                EventItemList[msg.item].topics[msg.topic].sockets.push_back( client_socket );
                LeaveCriticalSection( &MyCriticalSection );
                break;

            case POKE_DATA_ITEM:
                s = TEXT( "POKE_DATA_ITEM: " );
                s += EventItemList[msg.item].name;
                s += CommaSpace;
                s += msg.topic;
                s += CommaSpace;
                s += msg.data;
                show_message( s.c_str() );
                e = s.c_str();
                ServiceLog.add_entry( e, LOG_LEVEL_DEBUG );
                
                if ( msg.item == SHUTDOWN_INDEX )
                {
                    /*
                    ----------------------
                    Close all open sockets
                    ---------------------- */
                    for ( i = 0; i < NofItems; i++ )
                    {
                        for ( topic_itr = EventItemList[i].topics.begin(); topic_itr != EventItemList[i].topics.end(); topic_itr++ )
                        {
                            for ( socket_itr = topic_itr->second.sockets.begin(); socket_itr != topic_itr->second.sockets.end(); socket_itr++ )
                            {
                                send( *socket_itr, (char*)&msg, sizeof(SOCKET_MSG), 0 );
                                closesocket( *socket_itr );
                                *socket_itr = INVALID_SOCKET;
                            }
                        }
                    }
                    RunSendThread = false;
                    PostMessage( MainWindow, WM_CLOSE, 0, 0L );
                    break;
                }

                EnterCriticalSection( &MyCriticalSection );
                copystring( EventItemList[msg.item].topics[msg.topic].default_value, msg.data );
                send_list = EventItemList[msg.item].topics[msg.topic].sockets;
                LeaveCriticalSection( &MyCriticalSection );

                for ( i = 0; i < (int)send_list.size(); i++ )
                {
                    entry = new( SEND_FIFO_ENTRY );
                    entry->socket = send_list[i];
                    entry->msg = msg;
                    SendFifo.push( entry );
                    SetEvent( SendEvent );
                }
                if ( msg.item == DS_NOTIFY_INDEX )
                {
                    sm = new SOCKET_MSG;
                    *sm = msg;
                    DaNotifyFifo.push( sm );
                    SetEvent( DaThreadEvent );
                }
                if ( msg.item == COMPUTER_SETUP_INDEX )
                {
                    /*
                    ---------------------------------------
                    Reset connections with remote computers
                    --------------------------------------- */
                    startup_notify();
                    setup_data_archiver_client();
                }
                break;

            case UNREGISTER_CLIENT:
                s = TEXT( "UNREGISTER_CLIENT" );
                show_message( s.c_str() );
                EnterCriticalSection( &MyCriticalSection );
                /*
                -----------------------------------------------
                Remove client_socket entries from EventItemList
                ----------------------------------------------- */
                for ( i = 0; i < NofItems; i++ )
                {
                    for ( topic_itr = EventItemList[i].topics.begin(); topic_itr != EventItemList[i].topics.end(); topic_itr++ )
                    {
                        for ( socket_itr = topic_itr->second.sockets.begin(); socket_itr != topic_itr->second.sockets.end(); socket_itr++ )
                        {
                            if ( *socket_itr == client_socket )
                            {
                                topic_itr->second.sockets.erase( socket_itr );
                                socket_itr = topic_itr->second.sockets.begin();
                            }

                            if ( topic_itr->second.sockets.size() == 0 )
                            {
                                EventItemList[i].topics.erase( topic_itr );
                                topic_itr = EventItemList[i].topics.begin();
                                break;
                            }
                        }

                        if ( EventItemList[i].topics.size() == 0 )
                        {
                            break;
                        }
                    }
                }
                LeaveCriticalSection( &MyCriticalSection );

                /*
                -----------------------
                Close the client_socket
                ----------------------- */
                closesocket( client_socket );
                client_socket = INVALID_SOCKET;
                break;

            case UNREGISTER_FOR_ITEM:
                s = TEXT( "UNREGISTER_FOR_ITEM: " );
                s += EventItemList[msg.item].name;
                s += ItemList[msg.item].name;
                s += CommaSpace;
                s += msg.topic;
                s += CommaSpace;
                s += msg.data;
                show_message( s.c_str() );

                /*
                -----------------------------------------------
                Remove client_socket entries from EventItemList
                ----------------------------------------------- */
                EnterCriticalSection( &MyCriticalSection );
                for ( socket_itr = EventItemList[msg.item].topics[msg.topic].sockets.begin(); socket_itr != EventItemList[msg.item].topics[msg.topic].sockets.end(); socket_itr++ )
                {
                    if ( *socket_itr == client_socket )
                    {
                        EventItemList[msg.item].topics[msg.topic].sockets.erase( socket_itr );
                        socket_itr = EventItemList[msg.item].topics[msg.topic].sockets.begin();
                    }

                    if ( EventItemList[msg.item].topics[msg.topic].sockets.size() == 0 )
                    {
                        EventItemList[msg.item].topics.erase( msg.topic );
                        break;
                    }
                }
                LeaveCriticalSection( &MyCriticalSection );
                break;

            case REQUEST_DATA_ITEM:
                s = TEXT( "REQUEST_DATA_ITEM: " );
                s += EventItemList[msg.item].name;
                s += CommaSpace;
                s += msg.topic;
                s += CommaSpace;
                s += EventItemList[msg.item].topics[msg.topic].default_value;
                show_message( s.c_str() );
                EnterCriticalSection( &MyCriticalSection );
                copystring( msg.data, EventItemList[msg.item].topics[msg.topic].default_value );
                send_list = EventItemList[msg.item].topics[msg.topic].sockets;
                LeaveCriticalSection( &MyCriticalSection );
                for ( i = 0; i < (int)EventItemList[msg.item].topics[msg.topic].sockets.size(); i++ )
                {
                    entry = new( SEND_FIFO_ENTRY );
                    entry->socket = send_list[i];
                    entry->msg = msg;
                    SendFifo.push( entry );
                    SetEvent( SendEvent );
                }
                break;
            }
        }
        else
        {
            e = TEXT( "recv_msg winsock error: " );
            e += WSAGetLastError();
            ServiceLog.add_entry( e, LOG_LEVEL_ERROR );
            return;
        }
    }
}

/***********************************************************************
*                            RESOURCE_STRING                           *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                        TOGGLE_MENU_VIEW_STATE                        *
***********************************************************************/
void toggle_menu_view_state( UINT & state, UINT menu_choice_id )
{
HMENU menu_handle;
if ( state == MF_CHECKED )
    state = MF_UNCHECKED;
else
    state = MF_CHECKED;

menu_handle = GetMenu( MainWindow );
CheckMenuItem( menu_handle, menu_choice_id, state );
}

/***********************************************************************
*                            APPEND_INTEGER                            *
***********************************************************************/
static void append_integer( TCHAR * dest, int sorc )
{
TCHAR buf[MAX_INTEGER_LEN+3];
TCHAR * cp;

cp = copystring( buf, TEXT(" $") );
int32toasc( cp, sorc, HEX_RADIX );
lstrcat( dest,  buf );
}

/***********************************************************************
*                           SIZE_MAIN_DIALOG                           *
***********************************************************************/
static void size_main_dialog()
{
RECT  r;

GetClientRect( MainWindow, &r);

MoveWindow( MainDialogWindow, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
GetClientRect( MainDialogWindow, &r);
MoveWindow( GetDlgItem(MainDialogWindow,MESSAGE_LISTBOX_ID), r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        MessageListbox = GetDlgItem( hWnd, MESSAGE_LISTBOX_ID );
        break;
    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
BOOL CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_SIZE:
        size_main_dialog();
        return TRUE;
        
    case WM_COMMAND:
        switch ( id )
            {
            case VIEW_VISI_CHOICE:
                toggle_menu_view_state( ViewVisiState, id );
                if ( ViewVisiState == MF_CHECKED )
                    DisplayingMessages = TRUE;
                else
                    DisplayingMessages = FALSE;
                return 0;

            case CLEAR_EVENTS_CHOICE:
                SendDlgItemMessage( MainDialogWindow, MESSAGE_LISTBOX_ID, LB_RESETCONTENT, 0, 0L );
                return 0;

            case MINIMIZE_CHOICE:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_CHOICE:
                PostMessage( MainWindow, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                         INIT_EVENT_ITEM_LIST                         *
***********************************************************************/
void init_event_item_list()
{
    SOCKET_ITEM_TOPIC_ENTRY e;
    int i;

    EnterCriticalSection(&MyCriticalSection);
    for (i = 0; i < NOF_EVENT_ITEMS; i++)
    {
        copystring(EventItemList[i].name, ItemList[i].name);
        copystring(e.default_value, ItemList[i].default_value);
        EventItemList[i].topics[DEFAULT_TOPIC] = e;
    }
    LeaveCriticalSection(&MyCriticalSection);
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static BOOL init()
{
WNDCLASS wc;
COMPUTER_CLASS c;
STRING_CLASS log_msg;
int computer_type;
DWORD id;

c.startup();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MyClassName );
wc.lpszMenuName  = TEXT("MainMenu");
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    279, 244,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainWindow )
    return FALSE;

MainDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("MainDialog"),
    MainWindow,
    (DLGPROC) main_dialog_proc );

ShowWindow( MainWindow, SW_HIDE );
UpdateWindow( MainWindow );

InitializeCriticalSection(&MyCriticalSection);
init_event_item_list();

setup_service_log(ServiceLog);

/*
-----------------------------
Setup and start socket server
----------------------------- */
start_winsock();
Server.callback = accept_connection;
network_get_ip_addr(IpAddr, IP_TYPE_LOCAL);
IpPort = network_get_ip_port(IP_TYPE_LOCAL);
Server.start(IpAddr, IpPort);

/*
----------------------------
Create Send Thread and Event
---------------------------- */
RunSendThread = true;
SendEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

CreateThread(
    NULL,                                       /* no security attributes        */
    0,                                          /* use default stack size        */
    (LPTHREAD_START_ROUTINE)send_thread,        /* thread function               */
    (void*)NULL,                                /* argument to thread function   */
    0,                                          /* use default creation flags    */
    &id);

/*
-------------------------------------------
Send startup message if monitoring computer
------------------------------------------- */
computer_type = network_get_computer_type();
if (computer_type == MONITORING_COMPUTER_TYPE)
    {
    startup_notify();
    }

/*
--------------------------
Setup Data Archiver client
-------------------------- */
setup_data_archiver_client();

log_msg = TEXT("STARTUP");
ServiceLog.add_entry(log_msg);

log_msg = TEXT("DataServer IP Address: ");
log_msg += DaClient.ip_addr;
ServiceLog.add_entry(log_msg);

log_msg = TEXT("DataServer IP Port: ");
log_msg += DaClient.ip_port;
ServiceLog.add_entry(log_msg);

return TRUE;
}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
static void cleanup()
{    
    STRING_CLASS log_msg;

    log_msg = TEXT("SHUTDOWN");
    ServiceLog.add_entry(log_msg);

    std::map<STRING, SOCKET_ITEM_TOPIC_ENTRY>::iterator topic_itr;
    int i;
    unsigned int j;

    EnterCriticalSection( &MyCriticalSection );
    
    for ( i = 0; i < NOF_EVENT_ITEMS; ++i )
    {
        for ( topic_itr = EventItemList[i].topics.begin(); topic_itr != EventItemList[i].topics.end(); ++topic_itr )
        {
            for ( j = 0; j < topic_itr->second.sockets.size(); ++j )
            {
                closesocket( topic_itr->second.sockets[j] );
                topic_itr->second.sockets[j] = INVALID_SOCKET;
            }
        }
    }

    LeaveCriticalSection( &MyCriticalSection );
    DeleteCriticalSection( &MyCriticalSection );
    WSACleanup();
    CloseHandle( DaThreadEvent );
    DaThreadEvent = 0;
    Server.is_listening = false;
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;
BOOL status;

MainInstance = this_instance;
MyWindowTitle = resource_string( MAIN_WINDOW_TITLE_STRING );

if ( is_previous_instance(MyClassName, 0) )
    return 0;

InitCommonControls();

if (!init())
    return FALSE;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialogWindow )
        status = IsDialogMessage( MainDialogWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        }
    }
    
cleanup();

return msg.wParam;
}
