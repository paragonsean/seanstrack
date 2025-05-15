#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\servicelog.h"
#include "..\include\socket_client.h"
#include "..\include\subs.h"

extern SERVICE_LOG ServiceLog;

/***********************************************************************
*                               RECV_MSG                               *
* Returns when dest buffer is full. Handles partial messages. dest     *
* buffer must remain in scope of the caller of recv_msg.               *
***********************************************************************/
bool recv_msg( SOCKET dest_socket, SOCKET_MSG * dest )
{
    int    chars_read;
    int    chars_to_read;
    char * cp;

    chars_to_read = (int) sizeof(SOCKET_MSG);
    cp = (char *) dest;
    while ( chars_to_read > 0 )
    {
        chars_read = recv( dest_socket, cp, chars_to_read, 0 );
        if ( chars_read == SOCKET_ERROR )
        {
            return false;
        }
        cp += chars_read;
        chars_to_read -= chars_read;
    }

    return true;
}

/***********************************************************************
*                              START_WINSOCK                           *
***********************************************************************/
int start_winsock()
{
    int status;
    WSADATA wsaData;
    STRING_CLASS e;

    status = WSAStartup(MAKEWORD(2, 0), &wsaData);

    if (status != 0)
    {
        e = TEXT("ERROR: WSAStartup failed. Winsock error: ");
        e += status;
        ServiceLog.add_entry(e, LOG_LEVEL_ERROR);
        return status;
    }

    return status;
}

/***********************************************************************
*                             SOCKET_CLIENT                            *
***********************************************************************/
SOCKET_CLIENT::SOCKET_CLIENT()
{
    callback            = NULL;
    client_socket       = INVALID_SOCKET;
    ip_port             = DEFAULT_PORT;
    send_and_close      = FALSE;
}

/***********************************************************************
*                            ~SOCKET_CLIENT                            *
***********************************************************************/
SOCKET_CLIENT::~SOCKET_CLIENT()
{
    is_running = FALSE;
    if ( client_socket != INVALID_SOCKET )
    {
    closesocket( client_socket );
        client_socket = INVALID_SOCKET;
    }
}

/***********************************************************************
*                      SOCKET_CLIENT::SET_IP_ADDR                      *
***********************************************************************/
void SOCKET_CLIENT::set_ip_addr( TCHAR * addr )
{
    copystring( ip_addr, addr );
}

/***********************************************************************
*                       SOCKET_CLIENT::SET_PORT                        *
***********************************************************************/
void SOCKET_CLIENT::set_port( int port )
{
    ip_port = port;
}

/***********************************************************************
*                   SOCKET_CLIENT::CONNECT_TO_SERVER                   *
***********************************************************************/
BOOLEAN SOCKET_CLIENT::connect_to_server()
{
    return connect_to_server( ip_addr, ip_port );
}

/***********************************************************************
*                            CONNECT_TO_SERVER                         *
***********************************************************************/
BOOLEAN SOCKET_CLIENT::connect_to_server( TCHAR * addr, int port )
{
    sockaddr_in  a;
    char         cbuf[TCP_ADDRESS_LEN+1];
    STRING_CLASS e;
    int          status;

    set_ip_addr( addr );
    ip_port = port;

    /*
    ---------------
    Create a socket
    --------------- */
    client_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if (client_socket == INVALID_SOCKET)
    {
        e = TEXT("ERROR: Could not create socket. Winsock error: ");
        e += WSAGetLastError();
        ServiceLog.add_entry(e, LOG_LEVEL_ERROR);
        return FALSE;
    }

    /*
    -------------------------------
    Connect to the address and port
    ------------------------------- */
    a.sin_family = AF_INET;
    a.sin_port   = htons( port );
    unicode_to_char( cbuf, addr );
    a.sin_addr.S_un.S_addr = inet_addr( cbuf );

    status = connect( client_socket, (sockaddr *)&a, sizeof(a) );

    if ( status == SOCKET_ERROR || client_socket == INVALID_SOCKET )
    {
        e = TEXT( "ERROR: Could not connect to server " );
        e += addr;
        e += TEXT( " on port " );
        e += port;
        e += TEXT( ". Winsock error: " );
        e += WSAGetLastError();
        ServiceLog.add_entry( e, LOG_LEVEL_ERROR );
        return FALSE;
    }

    return TRUE;
}

/***********************************************************************
*                              SEND_TO_SERVER                          *
***********************************************************************/
int SOCKET_CLIENT::send_to_server( char * buf, int size )
{
    int status;
    STRING_CLASS e;

    if (client_socket == INVALID_SOCKET)
    {
        return 0;
    }

    /*
    ----------------------
    Send an initial buffer
    ---------------------- */
    status = send( client_socket, buf, size, 0 );

    if (status == SOCKET_ERROR)
    {
        e = TEXT( "ERROR: Send failed with winsock error: " );
        e += WSAGetLastError();
        ServiceLog.add_entry( e, LOG_LEVEL_ERROR );
    }

    return status;
}

/***********************************************************************
*                           START_READ_THREAD                          *
***********************************************************************/
void SOCKET_CLIENT::start_read_thread()
{
    DWORD id;
    HANDLE h;

    h = CreateThread(
        NULL,                                       /* no security attributes        */
        0,                                          /* use default stack size        */
        ( LPTHREAD_START_ROUTINE )callback,         /* thread function               */
        (void *)&client_socket,                     /* argument to thread function   */
        0,                                          /* use default creation flags    */
        &id );                                      /* returns the thread identifier */
}

/***********************************************************************
*                              WRITE_THREAD                            *
***********************************************************************/
void write_thread( void * socket_client )
{
    SOCKET_CLIENT *sc;
    int status;
    STRING_CLASS e;

    sc = ( SOCKET_CLIENT *) socket_client;

    while ( sc->is_running )
    {
        if ( WaitForSingleObject(sc->send_event, INFINITE) == WAIT_FAILED )
        {
            break;
        }

        while ( sc->msg_fifo.size() > 0 )
        {
            status = send( sc->client_socket, (char*)&sc->msg_fifo.front(), sc->msg_size, 0 );
            sc->msg_fifo.pop();

            if ( status == SOCKET_ERROR )
            {
                e = TEXT( "ERROR: Send failed with winsock error: " );
                e += WSAGetLastError();
                ServiceLog.add_entry( e, LOG_LEVEL_ERROR );
            }
        }

        if ( sc->send_and_close )
        {
            closesocket( sc->client_socket );
            sc->client_socket = INVALID_SOCKET;
            break;
        }
    }
}

/***********************************************************************
*                           START_WRITE_THREAD                         *
***********************************************************************/
void SOCKET_CLIENT::start_write_thread()
{
    DWORD id;
    HANDLE h;

    send_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    is_running = TRUE;

    h = CreateThread(
        NULL,                                       /* no security attributes        */
        0,                                          /* use default stack size        */
        ( LPTHREAD_START_ROUTINE )write_thread,     /* thread function               */
        ( void * )this,                             /* argument to thread function   */
        0,                                          /* use default creation flags    */
        &id );                                      /* returns the thread identifier */
}

/***********************************************************************
*                                 CLOSE                                *
***********************************************************************/
void SOCKET_CLIENT::close()
{
    is_running = FALSE;

    if (client_socket != INVALID_SOCKET)
    {
        closesocket( client_socket );
        client_socket = INVALID_SOCKET;
    }
}