#include <windows.h>
#include <conio.h>

int main( void )
{
const int            BUF_LEN = 512;
const unsigned short FT3_PORT = 31000;

WSADATA           wsaData;
SOCKET            ConnectSocket;
char            * sendbuf = "this is a test";
char              recvbuf[BUF_LEN];
int               status;
sockaddr_in       my_address;
sockaddr        * sa;

/*
------------------
Initialize Winsock
------------------ */
status = WSAStartup( MAKEWORD(2,0), &wsaData );

ConnectSocket = socket( AF_INET, SOCK_STREAM, 0 );

my_address.sin_family      = AF_NET;
my_address.sin_addr.s_addr = inet_addr( "192.168.254.99" );
my_address.sin_port        = htons( FT3_PORT );

sa = (sockaddr *) &my_address;
status = connect( ConnectSocket, sa, (int) sizeof(sockaddr) );

/*
----------------------
Send an initial buffer
---------------------- */
status = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );

printf( "Bytes Sent: %ld\n", status );

/*
-------------------------------------------------------
Shutdown the connection since no more data will be sent
------------------------------------------------------- */
status = shutdown( ConnectSocket, SD_SEND );

/*
--------------------------------------------
Receive until the peer closes the connection
-------------------------------------------- */
while ( true )
    {
    status = recv( ConnectSocket, recvbuf, BUF_LEN, 0 );
    if ( status > 0 )
        {
        printf( "Bytes received: %d\n", status );
        }
    else
        {
        if ( status == 0 )
            printf( "Connection closed\n" );
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
        break;
        }
    }

/*
-------
Cleanup
------- */
closesocket( ConnectSocket );
WSACleanup();

return 0;
}