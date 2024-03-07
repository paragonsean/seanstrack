#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static const TCHAR NullChar        = TEXT( '\0' );

/***********************************************************************
*                            UNICODE_TO_ASCII                          *
*   This assumes you are in ascii mode and want to convert a unicode   *
*   string into an ascii string. slen is the byte count not uchar      *
***********************************************************************/
bool unicode_to_ascii( char * sorc, unsigned int slen )
{
char * dest;

if ( !sorc )
    return false;

dest = sorc;
slen -= 2;
while ( slen > 0 )
    {
    slen -= 2;
    dest++;
    sorc++;
    sorc++;
    *dest = *sorc;
    }

return true;
}

/***********************************************************************
*                                TOASC                                 *
***********************************************************************/
bool toasc( const TCHAR * file_to_convert )
{
DWORD   access_mode;
DWORD   share_mode;
DWORD   file_size;
DWORD   new_file_size;
DWORD   bytes_read;
DWORD   bytes_written;
char  * buf;
HANDLE  fh;

share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
access_mode = GENERIC_READ;

fh = CreateFile( file_to_convert, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh == INVALID_HANDLE_VALUE )
    return false;

file_size = GetFileSize( fh, 0 );
if ( file_size < 2 )
    {
    CloseHandle( fh );
    return true;
    }

new_file_size = file_size / 2;

buf = new char[file_size];
if ( !buf )
    {
    CloseHandle( fh );
    return false;
    }

if ( ReadFile( fh, buf, file_size, &bytes_read, 0)  )
    {
    CloseHandle( fh );

    unicode_to_ascii( buf, file_size );


    share_mode  = 0;
    access_mode = GENERIC_WRITE;

    fh = CreateFile( file_to_convert, access_mode, share_mode, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
    if ( fh == INVALID_HANDLE_VALUE )
        return false;

    WriteFile( fh, buf, new_file_size, &bytes_written, 0 );
    CloseHandle( fh );
    fh = INVALID_HANDLE_VALUE;
    if ( new_file_size == bytes_written )
        return true;
    }
else
    {
    CloseHandle( fh );
    }

delete[] buf;
return false;
}

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
TCHAR * getline( void )
{
static TCHAR buf[103];
buf[0] = 100;

cgets( buf );

return buf+2;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
TCHAR * cp;

cprintf( "\n\rThis program converts UNICODE files to ASCII (REPLACING the original file)." );

while ( 1 )
    {
    cprintf( "\n\rInput file to convert: " );
    cp = getline();
    if ( (*cp) == NullChar )
        break;

    cprintf( "Converting [%s]...", cp );
    toasc( cp );
    cprintf( "\n\r" );
    }

return 0;
}


