#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

static const TCHAR NullChar        = TEXT( '\0' );
static const TCHAR BackSlashChar   = TEXT( '\\' );
static const TCHAR ColonChar       = TEXT( ':' );

static BOOLEAN First_Time          = TRUE;
static TCHAR DefaultDirectory[]    = TEXT( "C:" );
static STRING_CLASS ExeDirectory;

/***********************************************************************
*                         COPY_UNICODE_TO_CHAR                         *
***********************************************************************/
void copy_unicode_to_char( char * dest, wchar_t * sorc )
{
const wchar_t UNullChar    = L'\0';
const char    ByteNullChar = '\0';

union {
char    c;
wchar_t t;
};

while ( TRUE )
    {
    t = *sorc;
    *dest = c;
    if ( *sorc == UNullChar )
        break;
    dest++;
    sorc++;
    }

*dest = ByteNullChar;
}

/***********************************************************************
*                     GET_STATIC_EXE_DIRECTORY                         *
*        There is NO backslash at the end of the directory path.       *
***********************************************************************/
static void get_static_exe_directory( void )
{
TCHAR    s[2048];
TCHAR *  cp;
int      slen;
LPWSTR * arglist;
int      nof_args;
bool     is_unicode;

is_unicode = false;
#ifdef UNICODE
is_unicode = true;
#endif

arglist = CommandLineToArgvW(GetCommandLineW(), &nof_args);
if( arglist )
    {
#ifdef UNICODE
    copystring( s, arglist[0], 2047 );
#else
    copy_unicode_to_char( s, arglist[0] );
#endif
    LocalFree(arglist);

    slen = lstrlen(s);
    if ( slen > 2 )
        {
        cp = s + slen;
        cp--;
        while ( TRUE )
            {
            if ( *cp == BackSlashChar || *cp == ColonChar )
                {
                if ( *cp == ColonChar )
                    cp++;
                *cp = NullChar;
                ExeDirectory = s;
                break;
                }
            slen--;
            if ( slen <= 0 )
                break;
            cp--;
            }
        }
    }

if ( ExeDirectory.isempty() )
    ExeDirectory = DefaultDirectory;
}

/***********************************************************************
*                          SET_EXE_DIRECTORY                           *
* This allows you to force the exe directory to be something other     *
* than the one the program started in.                                 *
***********************************************************************/
BOOLEAN set_exe_directory( TCHAR * new_exe_directory )
{
if ( new_exe_directory )
    {
    if ( *new_exe_directory != NullChar )
        {
        ExeDirectory = new_exe_directory;
        First_Time   = FALSE;
        return TRUE;
        }
    }
return FALSE;
}
/***********************************************************************
*                             EXE_DIRECTORY                            *
***********************************************************************/
const TCHAR * exe_directory()
{
if ( First_Time )
    {
    First_Time = FALSE;
    get_static_exe_directory();
    }

return ExeDirectory.text();
}

/***********************************************************************
*                         GET_EXE_DIRECTORY                            *
***********************************************************************/
BOOLEAN get_exe_directory( TCHAR * dest )
{
if ( dest )
    lstrcpy( dest, exe_directory() );

return TRUE;
}

