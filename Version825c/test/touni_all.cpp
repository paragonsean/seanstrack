#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"

static TCHAR BackSlashChar = TEXT('\\');
static TCHAR NullChar   = TEXT( '\0' );
static TCHAR PeriodChar = TEXT( '.' );
static TCHAR StarDotStar[] = TEXT( "*.*" );
static TCHAR TxtString[] = TEXT( "TXT" );
static TCHAR CsvString[] = TEXT( "CSV" );
static TCHAR DatString[] = TEXT( "DAT" );
static TCHAR IniString[] = TEXT( "INI" );
static TCHAR MsgString[] = TEXT( "MSG" );
static TCHAR PrgString[] = TEXT( "PRG" );
static TCHAR MnuString[] = TEXT( "MNU" );
static TCHAR LstString[] = TEXT( "LST" );

/***********************************************************************
*                            ASCII_TO_UNICODE                          *
*   This assumes you are in ascii mode and want to convert an ascii    *
*   string into a unicode string. The unicode buffer is assumed to be  *
*   twice as long as the ascii buffer.                                 *
*                                                                      *
*   It is ok for the dest and sorc to be the same address as I copy    *
*   the last byte first.                                               *
***********************************************************************/
bool ascii_to_unicode( char * dest, char * sorc, unsigned int slen )
{
unsigned int dlen;

if ( !dest )
    return false;

if ( !sorc )
    return false;

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

return true;
}

/***********************************************************************
*                       APPEND_BACKSLASH_TO_PATH                       *
*  Only if the last char is not a backslash or if the string is empty  *
***********************************************************************/
void append_backslash_to_path( TCHAR * s )
{
int n;

n = lstrlen( s );
if ( n > 0 )
    n--;

s += n;
if ( *s != BackSlashChar )
    {
    s++;
    *s = BackSlashChar;
    s++;
    *s = NullChar;
    }
}

/***********************************************************************
*                                TOUNI                                 *
***********************************************************************/
bool touni( const TCHAR * file_to_convert )
{
DWORD   access_mode;
DWORD   share_mode;
DWORD   file_size;
DWORD   new_file_size;
DWORD   bytes_read;
DWORD   bytes_written;
TCHAR * buf;
HANDLE  fh;

share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
access_mode = GENERIC_READ;

fh = CreateFile( file_to_convert, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh == INVALID_HANDLE_VALUE )
    return false;

file_size = GetFileSize( fh, 0 );
if ( file_size == 0 )
    {
    CloseHandle( fh );
    return true;
    }

new_file_size = file_size * 2;

buf = new TCHAR[new_file_size];
if ( !buf )
    {
    CloseHandle( fh );
    return false;
    }

if ( ReadFile( fh, buf, file_size, &bytes_read, 0)  )
    {
    ascii_to_unicode( buf, buf, file_size );

    CloseHandle( fh );

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
*                          CONVERT_ALL_FILES                           *
***********************************************************************/
static void convert_all_files( TCHAR * dir )
{
TCHAR * cp;
TCHAR s[MAX_PATH+1];
int slen;

WIN32_FIND_DATA fdata;
HANDLE          fh;

lstrcpy( s, dir );
append_backslash_to_path( s );
slen = lstrlen( s );
lstrcat( s, StarDotStar );

cprintf( "\n\rs is [%s]...", s );

fh = FindFirstFile( s, &fdata );

/*
---------------------------------------------------------------
If the handle is invalid, I assume (for now) there are no files
--------------------------------------------------------------- */
if ( fh == INVALID_HANDLE_VALUE )
    {
    cprintf( "\n\rINVALID_HANDLE_VALUE", s );
    return;
    }

while ( TRUE )
    {
    if ( *fdata.cFileName != PeriodChar )
        {
        lstrcpy( s+slen, fdata.cFileName );
        if ( fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
            convert_all_files( s );
            }
        else
            {
            cp = s + lstrlen(s) - 4;
            if ( *cp == PeriodChar )
                {
                cp++;
                if ( !lstrcmpi(cp,TxtString) || !lstrcmpi(cp,CsvString) || !lstrcmpi(cp,DatString) || !lstrcmpi(cp,IniString) || !lstrcmpi(cp,MsgString) || !lstrcmpi(cp,PrgString) || !lstrcmpi(cp,MnuString) || !lstrcmpi(cp,LstString) )
                    {
                    lstrcpy( s+slen, fdata.cFileName );
                    cprintf( "\n\rConverting [%s]...", s );
                    touni( s );
                    }
                }
            }
        }

    if ( !FindNextFile(fh, &fdata) )
        break;
    }

FindClose( fh );
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
TCHAR s[MAX_PATH+1];

if ( GetCurrentDirectory(MAX_PATH, s) != 0 )
    {
    cprintf( "\n\n\r**************************************" );
    cprintf( "\n\r************* WAIT! ******************" );
    cprintf( "\n\r**************************************" );
    cprintf( "\n\rThe current directory is [%s]", s );
    cprintf( "\n\rThis program converts all files from ascii to unicode in ALL SUBDIRECTORIES" );
    cprintf( "\n\rPress <Enter> to continue, N+<Enter> to cancel" );
    cp = getline();
    if ( (*cp) != NullChar )
        return 0;

    convert_all_files( s );
    }

cprintf( "\n\r*** Done ***\n\r" );
return 0;
}
