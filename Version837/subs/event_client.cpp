#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\servicelog.h"
#include "..\include\socket_client.h"
#include "..\include\events.h"
#include "..\include\event_client.h"
#include "..\include\subs.h"

extern SERVICE_LOG ServiceLog;
static TCHAR NullChar = TEXT( '\0' );

/***********************************************************************
*                           CLIENT_CALLBACK                            *
***********************************************************************/
void client_callback( void * event_client )
{
    EVENT_CLIENT * ec;
    SOCKET_MSG     msg;
    STRING_CLASS   s;

    ec = ( EVENT_CLIENT * )event_client;

    while ( ec->is_reading )
    {
        if ( recv_msg(ec->client_socket, &msg) )
        {
            if ( msg.item < NOF_EVENT_ITEMS )
                ec->registered_callbacks[msg.item]( &msg );
        }
        else
        {
                /*
                -----------------
                Log winsock error
                ----------------- */
                s = TEXT( ": ERROR: client_callback() recv failed with winsock error: ");
                s += WSAGetLastError();
                ServiceLog.add_entry( s, LOG_LEVEL_ERROR );
                return;
        }
    }
}

/***********************************************************************
*                              SOCKET_MSG                              *
***********************************************************************/
SOCKET_MSG::SOCKET_MSG()
{
    int i;

    cmd  = NO_EVENTMAN_COMMAND;
    item = NO_INDEX;
    for ( i=0; i<SOCKET_MSG_BUFLEN; i++ )
    {
        data[i]  = NullChar;
        topic[i] = NullChar;
    }
}

/***********************************************************************
*                              SOCKET_MSG                              *
***********************************************************************/
SOCKET_MSG::SOCKET_MSG( const SOCKET_MSG & sorc )
{
operator=( sorc );
}

/***********************************************************************
*                             ~SOCKET_MSG                              *
***********************************************************************/
SOCKET_MSG::~SOCKET_MSG()
{
}

/***********************************************************************
*                        SOCKET_MSG::operator=                         *
***********************************************************************/
void SOCKET_MSG::operator=(const SOCKET_MSG & sorc)
{
    cmd  = sorc.cmd;
    item = sorc.item;
    copystring( topic, sorc.topic, SOCKET_MSG_BUFLEN );
    copystring( data,  sorc.data,  SOCKET_MSG_BUFLEN );
}

/***********************************************************************
*                             EVENT_CLIENT                             *
***********************************************************************/
EVENT_CLIENT::EVENT_CLIENT()
{
    msg_size = (int) sizeof(SOCKET_MSG);
    callback = client_callback;
    is_reading =FALSE;
}

/***********************************************************************
*                            ~EVENT_CLIENT                             *
***********************************************************************/
EVENT_CLIENT::~EVENT_CLIENT()
{
}

/***********************************************************************
*                           GET_LOCAL_IP_ADDR                          *
***********************************************************************/
TCHAR * EVENT_CLIENT::get_local_ip_addr()
{
    return ip_addr;
}

/***********************************************************************
*                           GET_LOCAL_IP_PORT                          *
***********************************************************************/
int EVENT_CLIENT::get_local_port()
{
    return ip_port;
}

/***********************************************************************
*                            REGISTER_FOR_EVENT                        *
***********************************************************************/
void EVENT_CLIENT::register_for_event( int item, void(*event_callback)(SOCKET_MSG*) )
{
    register_for_event( item, DEFAULT_TOPIC, event_callback );
}

/***********************************************************************
*                            REGISTER_FOR_EVENT                        *
***********************************************************************/
void EVENT_CLIENT::register_for_event( int item, TCHAR *topic, void(*event_callback)(SOCKET_MSG*) )
{
    SOCKET_MSG msg;

    msg.cmd = REGISTER_FOR_ITEM;
    msg.item = item;
    copystring( msg.topic, topic );
    registered_callbacks[item] = event_callback;
    msg_fifo.push( msg );
    SetEvent( send_event );
}

/***********************************************************************
*                            UNREGISTER_FOR_EVENT                      *
***********************************************************************/
void EVENT_CLIENT::unregister_for_event( int item )
{
    unregister_for_event( item, DEFAULT_TOPIC );
}

/***********************************************************************
*                            UNREGISTER_FOR_EVENT                      *
***********************************************************************/
void EVENT_CLIENT::unregister_for_event( int item, TCHAR *topic )
{
    SOCKET_MSG msg;

    msg.cmd = UNREGISTER_FOR_ITEM;
    msg.item = item;
    copystring( msg.topic, topic );
    msg_fifo.push( msg );
    SetEvent( send_event );
}

/***********************************************************************
*                            UNREGISTER_CLIENT                         *
***********************************************************************/
void EVENT_CLIENT::unregister_client()
{
    SOCKET_MSG msg;

    msg.cmd = UNREGISTER_CLIENT;
    msg_fifo.push( msg );
    SetEvent( send_event );
}

/***********************************************************************
*                                  POKE_DATA                           *
***********************************************************************/
void EVENT_CLIENT::poke_data( int item, TCHAR * new_value )
{
    poke_data(item, new_value, DEFAULT_TOPIC);
}

/***********************************************************************
*                                  POKE_DATA                           *
***********************************************************************/
void EVENT_CLIENT::poke_data( int item, TCHAR * new_value, TCHAR * topic )
{
    SOCKET_MSG msg;

    msg.cmd = POKE_DATA_ITEM;
    msg.item = item;
    copystring( msg.data, new_value );
    copystring( msg.topic, topic );
    msg_fifo.push( msg );
    SetEvent( send_event );
}

/***********************************************************************
*                             REQUEST_DATA_ITEM                        *
***********************************************************************/
void EVENT_CLIENT::request_data_item( int item )
{
    request_data_item( DEFAULT_TOPIC, item );
}

/***********************************************************************
*                             REQUEST_DATA_ITEM                        *
***********************************************************************/
void EVENT_CLIENT::request_data_item( TCHAR *topic, int item )
{
    SOCKET_MSG msg;

    msg.cmd = REQUEST_DATA_ITEM;
    msg.item = item;
    copystring( msg.topic, topic );
    msg_fifo.push( msg );
    SetEvent( send_event );
}

/***********************************************************************
*                          START_EVENT_READ_THREAD                     *
***********************************************************************/
void EVENT_CLIENT::start_event_read_thread()
{
    DWORD id;
    HANDLE h;

    is_reading = TRUE;

    h = CreateThread(
        NULL,                                       /* no security attributes        */
        0,                                          /* use default stack size        */
        ( LPTHREAD_START_ROUTINE )client_callback,  /* thread function               */
        (void *) this,                              /* argument to thread function   */
        0,                                          /* use default creation flags    */
        &id);                                       /* returns the thread identifier */
}

/***********************************************************************
*                          EVENT_CLIENT::START                         *
***********************************************************************/
void EVENT_CLIENT::start()
{
    network_get_ip_addr( ip_addr, IP_TYPE_LOCAL );
    ip_port = network_get_ip_port( IP_TYPE_LOCAL );
    connect_to_server();
    start_event_read_thread();
    start_write_thread();
}

/***********************************************************************
*                      EVENT_CLIENT::START_REMOTE                      *
***********************************************************************/
void EVENT_CLIENT::start_remote( TCHAR *computername)
{
    network_get_ip_addr_from_computername( ip_addr, computername );
    ip_port = network_get_ip_port( IP_TYPE_LOCAL );
    connect_to_server();
    start_event_read_thread();
    start_write_thread();
}

/***********************************************************************
*                           EVENT_CLIENT::STOP                         *
***********************************************************************/
void EVENT_CLIENT::stop()
{
    is_running = FALSE;
    is_reading = FALSE;
    close();
}