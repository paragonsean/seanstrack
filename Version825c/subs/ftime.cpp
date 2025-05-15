#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

/***********************************************************************
*                         GET_FILE_WRITE_TIME                          *
***********************************************************************/
BOOLEAN get_file_write_time( SYSTEMTIME & t, TCHAR * fname )
{
WIN32_FIND_DATA fdata;
HANDLE          fh;

fh = FindFirstFile( fname, &fdata );

if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;

FileTimeToSystemTime( &fdata.ftLastWriteTime, &t );
FindClose( fh );

return TRUE;
}

/***********************************************************************
*                         GET_FILE_WRITE_TIME                          *
* In the MSDN Library under "File Times" it says this is the proper    *
* way to get the local time because it adjusts for daylight savings    *
***********************************************************************/
BOOLEAN get_file_write_time( FILETIME & dest, TCHAR * fname )
{
WIN32_FIND_DATA fdata;
HANDLE          fh;
SYSTEMTIME      localst;
SYSTEMTIME      st;

fh = FindFirstFile( fname, &fdata );

if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;
FindClose( fh );

FileTimeToSystemTime( &fdata.ftLastWriteTime, &st );
SystemTimeToTzSpecificLocalTime( NULL, &st, &localst );
SystemTimeToFileTime( &localst, &dest );

return TRUE;
}

/***********************************************************************
*                        GET_FILE_CREATION_TIME                        *
* In the MSDN Library under "File Times" it says this is the proper    *
* way to get the local time because it adjusts for daylight savings    *
***********************************************************************/
BOOLEAN get_file_creation_time( FILETIME & dest, TCHAR * fname )
{
WIN32_FIND_DATA fdata;
HANDLE          fh;
SYSTEMTIME      localst;
SYSTEMTIME      st;

fh = FindFirstFile( fname, &fdata );

if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;
FindClose( fh );

FileTimeToSystemTime( &fdata.ftCreationTime, &st );
SystemTimeToTzSpecificLocalTime( NULL, &st, &localst );
SystemTimeToFileTime( &localst, &dest );

return TRUE;
}
