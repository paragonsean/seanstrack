#include <windows.h>
#include <vector>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\servicelog.h"
#include "..\include\socket_server.h"
#include "..\include\subs.h"

extern SERVICE_LOG ServiceLog;

/***********************************************************************
*                             LISTEN_THREAD                            *
***********************************************************************/
void listen_thread( void * socket_server )
{
    SOCKET_SERVER * ss;
    SOCKET  client_socket;
    STRING_CLASS e;
    DWORD id;
    HANDLE h;
    int n;

    ss = (SOCKET_SERVER*)socket_server;

    /*
    --------------------
    Listen on the socket
    -------------------- */
    if (listen(ss->listener_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        /*
        -----------------
        Log winsock error
        ----------------- */
        e = TEXT("ERROR: listen failed with winsock error: ");
        e += WSAGetLastError();
        ServiceLog.add_entry(e, LOG_LEVEL_ERROR);
        return;
    }

    while ( ss->is_listening )
    {
        /*
        ----------------------
        Accept a client socket
        ---------------------- */
        client_socket = accept( ss->listener_socket, NULL, NULL );
        
        if ( client_socket == INVALID_SOCKET )
        {
            /*
            -----------------
            Log winsock error
            ----------------- */
            e = TEXT( "ERROR: accept failed with winsock error: " );
            e += WSAGetLastError(); 
            ServiceLog.add_entry( e, LOG_LEVEL_ERROR );
            return;
        }

        ss->client_sockets.push_back( client_socket );
        n = ss->client_sockets.size();

        h = CreateThread(
            NULL,                                       /* no security attributes        */
            0,                                          /* use default stack size        */
            ( LPTHREAD_START_ROUTINE )ss->callback,     /* thread function               */
            socket_to_void_ptr( client_socket ),        /* argument to thread function   */
            0,                                          /* use default creation flags    */
            &id);                                       /* returns the thread identifier */ 
    }
}

/***********************************************************************
*                            SOCKET_SERVER                             *
***********************************************************************/
SOCKET_SERVER::SOCKET_SERVER()
{

}

/***********************************************************************
*                            ~SOCKET_SERVER                            *
***********************************************************************/
SOCKET_SERVER::~SOCKET_SERVER()
{
    stop();
}

/***********************************************************************
*                               SET_IP_ADDR                            *
***********************************************************************/
void SOCKET_SERVER::set_ip_addr(TCHAR *addr)
{
    copystring(ip_addr, addr); 
}

/***********************************************************************
*                              SET_IP_PORT                             *
***********************************************************************/
void SOCKET_SERVER::set_ip_port( int port )
{
    ip_port = port;
}

/***********************************************************************
*                                 START                                *
***********************************************************************/
bool SOCKET_SERVER::start()
{
    int result;
    STRING_CLASS e;
    sockaddr_in a;
    DWORD id;
    HANDLE h;
    char cbuf[TCP_ADDRESS_LEN + 1];

    /*
    ---------------
    Create a socket
    --------------- */
    listener_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( listener_socket == INVALID_SOCKET )
    {
        /*
        -----------------
        Log winsock error
        ----------------- */
        e = TEXT( "ERROR: failed to create listen socket with winsock error: " );
        e += WSAGetLastError();
        ServiceLog.add_entry( e, LOG_LEVEL_ERROR );
        return false;
    }

    /*
    ------------------------------
    Setup the TCP listening socket
    ------------------------------ */
    a.sin_family = AF_INET;
    a.sin_port = htons( ip_port );
    unicode_to_char( cbuf, ip_addr );
    a.sin_addr.S_un.S_addr = inet_addr( cbuf );

    result = bind( listener_socket, (sockaddr*)&a, sizeof(a) );

    if ( result == SOCKET_ERROR )
    {
        /*
        -----------------
        Log winsock error
        ----------------- */
        e = TEXT( "ERROR: bind failed with winsock error: " );
        e += WSAGetLastError();
        ServiceLog.add_entry( e, LOG_LEVEL_ERROR );
        return false;
    }

    is_listening = true;

    h = CreateThread(
        NULL,                                       /* no security attributes        */
        0,                                          /* use default stack size        */
        ( LPTHREAD_START_ROUTINE )listen_thread,    /* thread function               */
        ( void* )this,                              /* argument to thread function   */
        0,                                          /* use default creation flags    */
        &id);                                       /* returns the thread identifier */

    return true;
}

/***********************************************************************
*                                 START                                *
***********************************************************************/
bool SOCKET_SERVER::start( TCHAR * addr, int port )
{
    set_ip_addr( addr );
    set_ip_port( port );

    return start();
}

/***********************************************************************
*                                 STOP                                 *
***********************************************************************/
void SOCKET_SERVER::stop()
{
    int i;
    int n;

    is_listening = false;

    n = client_sockets.size();

    for ( i = 0; i < n; i++ )
    {
        if ( client_sockets[i] != INVALID_SOCKET )
        {
            closesocket( client_sockets[i] );
            client_sockets[i] = INVALID_SOCKET;
        }
    }

    if ( listener_socket != INVALID_SOCKET )
    {
        closesocket( listener_socket );
        listener_socket = INVALID_SOCKET;
    }
}
