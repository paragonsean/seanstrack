#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\setpoint.h"
#include "..\include\socket_client.h"
#include "..\include\subs.h"

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

static TCHAR VisiTrakIniFile[]   = TEXT( "visitrak.ini" );
static TCHAR ConfigSection[]     = TEXT( "Config" );
static TCHAR LocalIpAddressKey[] = TEXT( "LocalIpAddress" );
static TCHAR LocalIpPortKey[]    = TEXT( "LocalIpPort" );
static TCHAR DsIpAddressKey[]    = TEXT( "DsIpAddress" );
static TCHAR DsIpPortKey[]       = TEXT( "DsIpPort" );
static TCHAR RemoteIpPortKey[]   = TEXT( "RemoteIpPort" );
static TCHAR StartupIpPortKey[]  = TEXT( "StartupIpPort" );
static TCHAR ComputerTypeKey[]   = TEXT( "ComputerType" );
static TCHAR NullChar            = TEXT( '\0' );

extern TCHAR MyClassName[];

/***********************************************************************
*                               GET_IP_ADDR                            *
***********************************************************************/
void network_get_ip_addr( TCHAR *dest, int type )
{    
    char *addr;
    TCHAR taddr[TCP_ADDRESS_LEN+1];
    char hostname[MAX_COMPUTERNAME_LENGTH+1];
    TCHAR *ip_addr_key;
    HOSTENT *hostent;
    SETPOINT_CLASS sp;
    NAME_CLASS s;

    if (type == IP_TYPE_LOCAL)
    {
        ip_addr_key = LocalIpAddressKey;
    }
    else if (type == IP_TYPE_DATA_ARCHIVER)
    {
        ip_addr_key = DsIpAddressKey;
    }
    else
    {
        /*
        -------------------
        Use default address
        ------------------- */
        copystring( dest, LOOPBACK_ADDRESS );
        dest[TCP_ADDRESS_LEN] = NullChar;
        return;
    }

    /*
    --------------------------------------
    Try to get IP address from config file
    -------------------------------------- */
    s.get_network_csvname();
    sp.get( s );
    s = sp[ip_addr_key].value;

    if ( !s.isempty() )
    {
        copystring( dest, s.text() );
        dest[TCP_ADDRESS_LEN] = NullChar;
    }
    else
    {
        /*
        ------------------------------
        Lookup IP using local hostname
        ------------------------------ */
        gethostname( hostname, MAX_COMPUTERNAME_LENGTH );
        hostent = gethostbyname( hostname );

        if ( hostent )
        {
            addr = inet_ntoa( **(in_addr**)hostent->h_addr_list );
            char_to_tchar( taddr, addr );
            copystring( dest, taddr );
            dest[TCP_ADDRESS_LEN] = NullChar;
        }
        else
        {
            /*
            -------------------
            Use default address
            ------------------- */
            copystring( dest, LOOPBACK_ADDRESS );
            dest[TCP_ADDRESS_LEN] = NullChar;
        }
    }
}

/***********************************************************************
*                               GET_IP_PORT                            *
***********************************************************************/
int network_get_ip_port( int type )
{
    NAME_CLASS s;
    TCHAR *ip_port_key;
    SETPOINT_CLASS sp;

    switch (type)
    {
        case IP_TYPE_LOCAL:
            ip_port_key = LocalIpPortKey;
            break;
        
        case IP_TYPE_DATA_ARCHIVER:
            ip_port_key = DsIpPortKey;
            break;

        case IP_TYPE_REMOTE:
            ip_port_key = RemoteIpPortKey;
            break;

        case IP_TYPE_STARTUP:
            ip_port_key = StartupIpPortKey;
            break;

        default:
            return DEFAULT_PORT;
    }

    s.get_network_csvname();
    sp.get( s );
    s = sp[ip_port_key].value;
    return s.int_value();
}

/***********************************************************************
*                        get_ip_addr_by_hostname                       *
***********************************************************************/
void network_get_ip_addr_from_computername( TCHAR *dest, TCHAR * computername )
{
    char       * caddr;
    char         cp[MAX_COMPUTERNAME_LENGTH+1];
    HOSTENT    * hostent;
    STRING_CLASS s;

    /*
    ---------------------------------
    Get IP address from computer name
    --------------------------------- */
    unicode_to_char( cp, computername );
    hostent = gethostbyname( cp );

    if ( hostent )
    {
        caddr = inet_ntoa( **(in_addr**)hostent->h_addr_list );
        char_to_tchar( dest, caddr );
    }
    else
    {
        dest = NULL;
    }
}

/***********************************************************************
*                           GET_COMPUTER_TYPE                          *
***********************************************************************/
int network_get_computer_type()
{
    STRING_CLASS     s;
    NAME_CLASS       n;
    COMPUTER_CLASS   c;
    int              nof_subdirs;

    s = MyClassName;

    if ( s == TEXT("DataServer") )
    {
        return DATA_ARCHIVER_COMPUTER_TYPE;
    }
    else if ( s == TEXT("EventMan") || s == TEXT("Profile") )
    {
        c.load();
        n = c.whoami();
        n += TEXT( "\\data" );
        nof_subdirs = n.subdir_count();

        if ( nof_subdirs == 0 )
        {
            return WORKSTATION_COMPUTER_TYPE;
        }
        else
        {
            return MONITORING_COMPUTER_TYPE;
        }
    }
    
    return NO_COMPUTER_TYPE;
}

/***********************************************************************
*                           SOCKET_TO_VOID_PTR                         *
***********************************************************************/
void * socket_to_void_ptr(SOCKET socket)
{
    union {
        void * v;
        SOCKET s;
    };

    s = socket;
    return v;
}

/***********************************************************************
*                           VOID_PTR_TO_SOCKET                         *
***********************************************************************/
SOCKET void_ptr_to_socket(void * ptr)
{
    union {
        SOCKET s;
        void * v;
    };

    v = ptr;
    return s;
}