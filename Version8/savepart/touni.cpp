#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"

static const char NullChar = TEXT( '\0' );

/***********************************************************************
*                            ASCII_TO_UNICODE                          *
*   This assumes you are in ascii mode and want to convert an ascii    *
*   string into a unicode string. The buffer is assumed to be twice    *
*   as long as slen. slen is the number of bytes to convert.           *
*                                                                      *
*   It is ok for the dest and sorc to be the same address as I copy    *
*   the last byte first.                                               *
***********************************************************************/
static BOOLEAN ascii_to_unicode( char * sorc, unsigned int slen )
{
unsigned int dlen;
char * dest;

if ( !sorc )
    return FALSE;

dest = sorc;
dlen = slen * 2;

dest += dlen;
sorc += slen;

while ( slen )
    {
    dest--;
    *dest = NullChar;
    dest--;
    sorc--;
    *dest = *sorc;
    slen--;
    }

return TRUE;
}

/***********************************************************************
*                                TOUNI                                 *
***********************************************************************/
BOOLEAN touni( const TCHAR * dest, const TCHAR * sorc )
{
DWORD   access_mode;
DWORD   share_mode;
DWORD   file_size;
DWORD   new_file_size;
DWORD   bytes_read;
DWORD   bytes_written;
char  * buf;
HANDLE  fh;
BOOLEAN status;

share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
access_mode = GENERIC_READ;

fh = CreateFile( sorc, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;

file_size = GetFileSize( fh, 0 );
if ( file_size == 0 )
    {
    CloseHandle( fh );
    return TRUE;
    }

new_file_size = file_size * 2;

buf = new char[new_file_size];
if ( !buf )
    {
    CloseHandle( fh );
    return FALSE;
    }

status = FALSE;
if ( ReadFile( fh, buf, file_size, &bytes_read, 0)  )
    {
    ascii_to_unicode( buf, file_size );

    CloseHandle( fh );

    share_mode  = 0;
    access_mode = GENERIC_WRITE;

    fh = CreateFile( dest, access_mode, share_mode, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
    if ( fh != INVALID_HANDLE_VALUE )
        {
        WriteFile( fh, buf, new_file_size, &bytes_written, 0 );
        if ( new_file_size == bytes_written )
            status = TRUE;
        }
    }

if ( fh != INVALID_HANDLE_VALUE )
    CloseHandle( fh );

delete[] buf;
return status;
}

