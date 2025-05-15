#include <windows.h>
#include <conio.h>
#include <stdio.h>

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
const int            QChar   = (int) 'q';
const char           GetMSLString[] = "V319\r";
const char           SetMSLString[] = "V319=100\r";
const int            BUF_LEN = 512;
const int            SD_SEND = 0x01;
const unsigned short FT3_PORT = 31000;

sockaddr_in       my_address;
SOCKET            mysocket;
int               n;
char              recvbuf[BUF_LEN];
sockaddr        * sa;
char              sendbuf[50];
int               status;
WSADATA           wsaData;

/*
------------------
Initialize Winsock
------------------ */
status = WSAStartup( MAKEWORD(2,0), &wsaData );

mysocket = socket( AF_INET, SOCK_STREAM, 0 );
if ( mysocket == INVALID_SOCKET )
    printf( "socket() failed with error: %d\n", WSAGetLastError() );
else
    printf( "socket() created successfully\n" );

my_address.sin_family      = AF_INET;
my_address.sin_addr.s_addr = inet_addr( "172.16.20.50" );
my_address.sin_port        = htons( FT3_PORT );

sa = (sockaddr *) &my_address;
status = connect( mysocket, sa, (int) sizeof(sockaddr) );
if ( status == SOCKET_ERROR )
    printf( "connect() failed with error: %d\n", WSAGetLastError() );

/*
----------------------
Send an initial buffer
---------------------- */
strcpy( sendbuf, SetMSLString );
n = (int) strlen( SetMSLString );
status = send( mysocket, sendbuf, n, 0 );
n--;
sendbuf[n] = '\0';
if ( status == SOCKET_ERROR )
    printf( "send() failed with error: %d\n", WSAGetLastError() );
else
    printf( "Bytes Sent: %ld [%s]\n", status, sendbuf );
while ( true )
    {
    strcpy( sendbuf, GetMSLString );
    n = (int) strlen( sendbuf );
    status = send( mysocket, sendbuf, n, 0 );
    n--;
    sendbuf[n] = '\0';
    if ( status == SOCKET_ERROR )
        printf( "send() failed with error: %d\n", WSAGetLastError() );
    else
        printf( "Bytes Sent: %ld [%s]\n", status, sendbuf );

    status = recv( mysocket, recvbuf, BUF_LEN, 0 );
    if ( status == SOCKET_ERROR )
        {
        printf( "recv() failed with error: %d\n", WSAGetLastError() );
        break;
        }
    else if ( status > 0 )
        {
        recvbuf[status-1] = '\0';
        printf( "Bytes received: %d  string=[%s]\n", status, recvbuf );
        }
    else
        {
        printf( "Connection closed\n" );
        break;
        }

    if ( getchar() == QChar )
        break;
    }

/*
-------
Cleanup
------- */
status = closesocket( mysocket );
if ( status == SOCKET_ERROR )
    printf( "closesocket() failed with error: %d\n", WSAGetLastError() );
else
    printf( "socket closed successfully\n" );

WSACleanup();
return 0;
}