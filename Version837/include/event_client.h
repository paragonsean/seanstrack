#include "socket_client.h"

#ifndef _EVENTS_H
#include "events.h"
#endif

class EVENT_CLIENT : public SOCKET_CLIENT
{
public:
    EVENT_CLIENT();
    ~EVENT_CLIENT(); 
    void register_for_event( int item, void(*callback)(SOCKET_MSG*) );
    void register_for_event( int item, TCHAR * topic, void(*callback)(SOCKET_MSG*) );
    void unregister_for_event( int item );
    void unregister_for_event( int item, TCHAR *topic );
    void unregister_client();
    void poke_data( int item, TCHAR *new_value );
    void poke_data( int item, TCHAR *new_value, TCHAR * topic );
    void request_data_item( int item );
    void request_data_item( TCHAR * topic, int item );    
    void start_event_read_thread();
    TCHAR* get_local_ip_addr();
    int get_local_port();
    void start();
    void start_remote( TCHAR *computername );
    void stop();
    int ip_port;
    void( *registered_callbacks[NOF_EVENT_ITEMS])(SOCKET_MSG* );
    BOOLEAN is_reading;
};