#ifndef _SOCKET_CLIENT_CLASS_
#define _SOCKET_CLIENT_CLASS_

#include <queue>
#ifndef _EVENTS_H
#include "events.h"
#endif

#define LOOPBACK_ADDRESS   TEXT( "127.0.0.1" )
#define DEFAULT_PORT   27015
#define IP_ADDR_LEN    16

bool recv_msg( SOCKET dest_socket, SOCKET_MSG * dest );

class SOCKET_CLIENT
{
public:
    SOCKET  client_socket;
    int     msg_size;
    TCHAR   ip_addr[TCP_ADDRESS_LEN+1];
    int     ip_port;
    BOOLEAN send_and_close;
    BOOLEAN is_running;
    std::queue<SOCKET_MSG> msg_fifo;
    HANDLE send_event;

    SOCKET_CLIENT();
    ~SOCKET_CLIENT();

    void( * callback )( void* );
    BOOLEAN connect_to_server();
    BOOLEAN connect_to_server( TCHAR * addr, int port );
    int     send_to_server( char * buf );
    int     send_to_server( char * buf, int size );
    void    set_ip_addr( TCHAR * addr );
    void    set_port( int port );
    void    start_read_thread();
    void    start_write_thread();
    void    close();
};

#endif