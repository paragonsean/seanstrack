#include <vector>

class SOCKET_SERVER
{
public:
    SOCKET_SERVER();
    ~SOCKET_SERVER(); 
    void set_ip_addr( TCHAR *addr );
    void set_ip_port( int port );
    bool start();
    bool start( TCHAR * addr, int port );
    void stop();
    void( *callback )( void* );
    TCHAR ip_addr[TCP_ADDRESS_LEN+1];
    int ip_port;
    SOCKET listener_socket;
    bool is_listening;
    std::vector <SOCKET> client_sockets;
};