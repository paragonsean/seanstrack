#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\csvclass.h"
#include "..\include\subs.h"


#define DB_OPEN_WAIT_MS        10  /* Clock counts to wait between open tries */
#define DB_OPEN_TIMEOUT_COUNT  30  /* Max tries to open table  */
#define REC_LOCK_CLOCK_COUNT    1  /* Clock counts to wait between lock tries */
#define REC_LOCK_TIMEOUT_COUNT 20  /* Max tries to lock record */

const TCHAR  FIELD_DELIMITER             = TEXT( ',' );
const TCHAR  STRING_DELIMITER            = TEXT( '"' );
const TCHAR  NullChar                    = TEXT( '\0' );
const TCHAR  SpaceChar                   = TEXT( ' ' );
const TCHAR  ForwardSlashChar            = TEXT( '/' );
const TCHAR  EmptyString[]               = TEXT( "" );
      TCHAR  FileTimeoutError[]          = TEXT( "File Lock Timeout" );
      TCHAR  FileNotFoundError[]         = TEXT( "File Not Found" );
      TCHAR  PathNotFoundError[]         = TEXT( "Path Not Found" );
static TCHAR LfChar   = TEXT('\012');
static TCHAR CrChar   = TEXT('\015');
static TCHAR NullChar = TEXT('\0');

SECURITY_ATTRIBUTES SecurityAttributes = {sizeof(SecurityAttributes), 0, FALSE };

/***********************************************************************
*                             FIELDLENGTH                              *
*             This is for use only with non-alpha fields.              *
***********************************************************************/
static short fieldlength( const TCHAR * s )
{
const TCHAR * cp;
short n;

cp = s;
n  = 0;

while ( *cp != FIELD_DELIMITER && *cp != RECORD_DELIMITER )
    {
    n++;
    cp++;
    }

return n;
}

/***********************************************************************
*                         ADD_FIELD_DELIMITER                          *
*          NOTE: Field length does NOT include the delimiter.          *
***********************************************************************/
static void add_field_delimiter( TCHAR * s )
{
/*
---------------------------------------------------
Add a field delimiter if this is not the last field
--------------------------------------------------- */
if ( *s != RECORD_DELIMITER )
    *s = FIELD_DELIMITER;
}

/***********************************************************************
*                              POST_ERROR                              *
***********************************************************************/
void post_error( LPTSTR s )
{
MessageBox( 0, s, EmptyString, MB_OK );
}

/***********************************************************************
*                              POST_ERROR                              *
***********************************************************************/
void post_error( LPTSTR s, LPTSTR s1 )
{
MessageBox( 0, s, s1, MB_OK );
}

/***********************************************************************
*                         CSV_CLASS::CSV_CLASS                         *
***********************************************************************/
CSV_CLASS::CSV_CLASS()
{
rp              = readbuf;
eof_readbuf     = readbuf + READBUF_LEN;
rp              = eof_readbuf;
is_end_of_file  = FALSE;
th              = 0;
have_rec_lock   = FALSE;
have_table_lock = FALSE;
}

/***********************************************************************
*                         CSV_CLASS::~CSV_CLASS                        *
***********************************************************************/
CSV_CLASS::~CSV_CLASS()
{
if ( th )
    {
    CloseHandle( th );
    th = 0;
    }
}

/***********************************************************************
*                           CSV_CLASS::CLOSE                            *
***********************************************************************/
void CSV_CLASS::close()
{
if ( th )
    {
    unlock_record();
    CloseHandle( th );
    th = 0;
    }
}

/***********************************************************************
*                           CSV_CLASS::EMPTY                            *
***********************************************************************/
BOOLEAN CSV_CLASS::empty()
{
DWORD status;

status = SetFilePointer( th, 0, 0, FILE_BEGIN );
if ( status != 0xFFFFFFFF )
    {
    SetEndOfFile( th );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               INITBUF                                *
***********************************************************************/
BOOLEAN CSV_CLASS::initbuf( TCHAR ** buf, short * buflen, short slen )
{
TCHAR * cp;

if ( *buf )
    {
    if ( *buflen != slen )
        {
        delete[] *buf;
        *buf = 0;
        }
    }

*buflen = slen;

if ( !(*buf) )
    {
    *buf = new TCHAR[slen+1];
    if ( !(*buf) )
        {
        globalerr = dberr = GetLastError();
        post_error( TEXT("Unable to ALLOCATE CSV_CLASS record buffer") );
        return FALSE;
        }
    }

slen -= short(RECORD_DELIMITER_LEN);
fillchars( *buf, SpaceChar, slen );
cp = *buf + slen;
copychars( cp, RECORD_DELIMITER_STRING, RECORD_DELIMITER_LEN+1 );
return TRUE;
}

/***********************************************************************
*                         CSV_CLASS::OPEN                               *
*                                                                      *
*  If the time_out count is > 0 then I will issue an error if the      *
*  open fails.                                                         *
*  The reclen is the entire length of a record, including the cr/lf.   *
*                                                                      *
***********************************************************************/
BOOLEAN CSV_CLASS::open( TCHAR * dbname, short locktype, short time_out )
{
DWORD access_mode;
DWORD share_mode;
DWORD ms_to_wait;
BOOLEAN first_time;
TCHAR * error_string;

name            = dbname;
locktype        = locktype;
have_table_lock = FALSE;
have_rec_lock   = FALSE;
search_mode     = SEARCHFIRST;
recnum          = 0;

if ( time_out < 0 )
    time_out = 0;

if ( th )
    {
    CloseHandle( th );
    th = 0;
    }

globalerr = dberr = VS_SUCCESS;

first_time = TRUE;
error_string = 0;

ms_to_wait = DB_OPEN_WAIT_MS;

while ( TRUE )
    {
    if ( !th )
        {
        switch (locktype )
            {
            case PWL:
                access_mode = GENERIC_READ | GENERIC_WRITE;
                share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
                break;

            case FL:
                access_mode = GENERIC_READ | GENERIC_WRITE;
                share_mode  = 0;
                break;

            case WL:
                access_mode = GENERIC_READ | GENERIC_WRITE;
                share_mode  = FILE_SHARE_READ;
                break;

            default:
                locktype = PFL;
                access_mode = GENERIC_READ;
                share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
                break;
            }

        th = CreateFile( name.text(), access_mode, share_mode, &SecurityAttributes, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0 );
        if ( th != INVALID_HANDLE_VALUE )
            {
            globalerr = dberr = VS_SUCCESS;
            break;
            }
        else
            {
            th = 0;
            globalerr = dberr = GetLastError();
            if ( globalerr == ERROR_FILE_NOT_FOUND )
                {
                error_string = FileNotFoundError;
                break;
                }
            else if ( globalerr == ERROR_PATH_NOT_FOUND )
                {
                error_string = PathNotFoundError;
                break;
                }
            }
        }

    /*
    ------------------------------------------------------------------------
    The time_out is only zero if the caller specifically asked for 0 retrys.
    ------------------------------------------------------------------------ */
    if ( time_out == 0 )
        break;

    if ( time_out == 1 )
        {
        error_string = FileTimeoutError;
        break;
        }

    if ( time_out < 9 )
        ms_to_wait *= 2;

    if ( first_time )
        first_time = FALSE;
    else
        Sleep( ms_to_wait );

    time_out--;
    }

if ( error_string )
    post_error( error_string, name.text() );

if ( dberr == VS_SUCCESS )
    return TRUE;
else
    return FALSE;
}

/***********************************************************************
*                         CSV_CLASS::OPEN                               *
***********************************************************************/
BOOLEAN CSV_CLASS::open( TCHAR * dbname, int locktype )
{
return open( dbname, locktype, DB_OPEN_TIMEOUT_COUNT );
}

/***********************************************************************
*                           CSV_CLASS::CREATE                           *
***********************************************************************/
BOOLEAN CSV_CLASS::create( TCHAR * dbname )
{
DWORD access_mode;
DWORD share_mode;

if ( name != dbname )
    name = dbname;
access_mode = GENERIC_READ | GENERIC_WRITE;
share_mode  = 0;

th = CreateFile( name.text(), access_mode, share_mode, &SecurityAttributes, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, 0 );

if ( th )
    {
    CloseHandle( th );
    th = 0;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        CSV_CLASS::ENSURE_EXISTANCE                    *
***********************************************************************/
BOOLEAN CSV_CLASS::ensure_existance( STRING_CLASS & dbname )
{
name = dbname;

if ( !file_exists(name.text()) )
    return create( name );

return TRUE;
}

/***********************************************************************
*                             OPEN_STATUS                              *
***********************************************************************/
BOOLEAN CSV_CLASS::open_status()
{
if ( th )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              CSV_CLASS                               *
*                               REWIND                                 *
***********************************************************************/
BOOLEAN CSV_CLASS::rewind()
{
DWORD status;
TCHAR  s[40];

if ( !th )
    return FALSE;

recnum = 0;

status = SetFilePointer( th, current_record_offset(), 0, FILE_BEGIN );
if ( status == 0xFFFFFFFF )
    {
    last_error = GetLastError();
    wsprintf( s, TEXT("0x%lX"), (unsigned long) last_error );
    MessageBox( NULL, TEXT("Can't set file pointer"), s, MB_OK );
    return FALSE;
    }

search_mode = SEARCHFIRST;

return TRUE;
}

/***********************************************************************
*                          CSV_CLASS::GET_REC                           *
***********************************************************************/
BOOLEAN CSV_CLASS::get_rec()
{
BOOLEAN at_eof_line;
BOOL    status;
DWORD   bytes_to_read;
DWORD   bytes_read;
DWORD   chars_read;
return_status = FALSE;

at_eof_line = FALSE;
recbuflen = 0;
cp = recbuf;
while ( true )
    {
    if ( rp == eof_readbuf && !is_end_of_file )
        {
        bytes_to_read = READBUF_LEN * sizeof(TCHAR);
        status = ReadFile( th, readbuf, bytes_to_read, &bytes_read, 0 );
        if ( !status )
            {
            last_error = GetLastError();
            return FALSE;
            }
        else if ( bytes_read <= bytes_to_read )
            {
            chars_read = bytes_read * sizeof(TCHAR);
            readbuf[chars_read] = NullChar;
            if ( chars_read < READBUF_LEN )
                is_end_of_file = TRUE;
            }
        rp = readbuf;
        }

    if ( *rp == LfChar || *rp == CrChar )
        {
        /*
        -------------------------------------------------------------------------
        I'm at the end of the record but I want to keep reading to clear the crlf
        ------------------------------------------------------------------------- */
        at_eof_line = true;
        }
    else
        {
        if ( at_end_of_line )
            {
            /*
            ------------------------------------------------------------
            I've just read the crlf so I am finished reading this record
            ------------------------------------------------------------ */
            *cp = NullChar;
            break;
            }

        *cp = *rp;

        /*
        ---------------------------------------------------------------------------------------------
        The only time I will copy a null char is if I am at the last line and it does not have a crlf
        --------------------------------------------------------------------------------------------- */
        if ( *cp == NullChar )
            break;

        recbuflen++;
        cp++;
        }
    rp++;
    }

return return_status;
}

/***********************************************************************
*                    CSV_CLASS::GET_NEXT_RECORD                        *
*                                                                      *
*  This function normally does NOT store it's result in the global     *
*  variable.                                                           *
*                                                                      *
***********************************************************************/
BOOLEAN CSV_CLASS::get_next_record( BOOLEAN need_lock )
{
if ( th )
    {
    if ( get_rec() )
        {
        recnum++;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                           COMPARE_DATE                               *
*                            12/25/1996                                *
*                            0123456789                                *
***********************************************************************/
short compare_date( TCHAR * np, TCHAR * cp )
{
short status;

status = compare( np+6, cp+6, 4 );

if ( status == 0 )
    status = compare( np, cp, 5 );

return status;
}

/***********************************************************************
*                            COMPARE_INT                               *
***********************************************************************/
short compare_int( TCHAR * np, TCHAR * cp, int slen )
{
int i1;
int i2;

i1 = extlong( np, slen );
i2 = extlong( cp, slen );

if ( i1 < i2 )
    return -1;

if ( i1 > i2 )
    return  1;

return 0;
}

/***********************************************************************
*                     CSV_CLASS::COMPARE_ONE_RECORD                     *
***********************************************************************/
BOOLEAN CSV_CLASS::compare_one_record( short * same, int rn, int nof_key_fields )
{

const int DATE_COMPARE_TYPE   = 1;
const int STRING_COMPARE_TYPE = 2;
const int int_COMPARE_TYPE  = 3;

int   compare_type;
int   i;
int   slen;
TCHAR * cp;
TCHAR * ep;
TCHAR * np;

unlock_record();

if ( !goto_record_number(rn) )
    return FALSE;

if ( !get_rec() )
    return FALSE;

np = recbuf;
cp = searchbuf;
ep = cp;

for ( i=0; i<nof_key_fields; i++ )
    {
    while ( *ep != FIELD_DELIMITER && *ep != RECORD_DELIMITER )
        ep++;

    slen = ep - cp;

    if ( *cp == STRING_DELIMITER )
        {
        compare_type = STRING_COMPARE_TYPE;
        }
    else
        {
        if ( slen == ALPHADATE_LEN && *(cp+2) == ForwardSlashChar )
            compare_type = DATE_COMPARE_TYPE;
        else if ( slen == ALPHATIME_LEN )
            compare_type = STRING_COMPARE_TYPE;
        else
            compare_type = int_COMPARE_TYPE;
        }

    switch ( compare_type )
        {
        case DATE_COMPARE_TYPE:
            *same = compare_date( np, cp );
            break;

        case int_COMPARE_TYPE:
            *same = compare_int( np, cp, slen );
            break;

        default:
            *same = compare( np, cp, slen );
            break;
        }

    if ( *same != 0 )
        break;

    ep++;
    cp = ep;
    np = recbuf + (cp-searchbuf);
    }

return TRUE;
}

/***********************************************************************
*                       RESTORE_FROM_SEARCHBUF                         *
*                                                                      *
*    This is for functions that search and may fail. When they         *
*    fail I want to restore the fields I was searching for.            *
*                                                                      *
***********************************************************************/
void CSV_CLASS::restore_from_searchbuf( void )
{
if ( searchlen > 0 && searchlen < recbuflen )
    {
    copychars( recbuf, searchbuf, searchlen );
    }
}

/***********************************************************************
*                   CSV_CLASS::FILL_SEARCHBUF                            *
***********************************************************************/
BOOLEAN CSV_CLASS::fill_searchbuf( short nof_key_fields )
{
TCHAR * cp;

searchlen = 0;
cp   = recbuf;
while ( nof_key_fields )
    {
    if ( *cp == FIELD_DELIMITER || *cp == RECORD_DELIMITER )
        {
        nof_key_fields--;

        /*
        --------------------------------------
        Give up if there are not enough fields
        -------------------------------------- */
        if ( *cp == RECORD_DELIMITER && nof_key_fields )
            return FALSE;
        }
    searchlen++;
    cp++;
    }

/*
-----------------------------
Make a copy of the key fields
----------------------------- */
copychars( searchbuf, recbuf, searchlen );

return TRUE;
}

/***********************************************************************
*                           CSV_CLASS                                   *
*                        GOTO_KEY_RECORD                               *
***********************************************************************/
BOOLEAN CSV_CLASS::goto_key_record( short nof_key_fields, int firstrec, int lastrec )
{
int   rn;
short   same;
BOOLEAN lastrec_matches;
BOOLEAN status;

if ( !fill_searchbuf(nof_key_fields) )
    return FALSE;

rn = nof_recs();
if ( rn < 1 )
    return FALSE;

rn--;

if ( lastrec > rn )
    lastrec = rn;

if ( firstrec < 0 || firstrec > lastrec )
    return FALSE;

status = compare_one_record( &same, firstrec, nof_key_fields );
if ( status )
    {
    if ( same == 0 )
        {
        return goto_record_number( firstrec );
        }

    /*
    -----------------------------------------------------
    If the first record is > than the search string, quit
    ----------------------------------------------------- */
    if ( same > 0 )
        status = FALSE;

    else if ( lastrec == firstrec )
        status = FALSE;
    }

/*
---------------------
Check the last record
--------------------- */
if ( status )
    {
    status = compare_one_record(&same, lastrec, nof_key_fields );
    }

if ( status )
    {
    /*
    ----------------------------------------------------
    If the last rec is less than the search string, quit
    ---------------------------------------------------- */
    if ( same < 0 )
        status = FALSE;
    }

if ( status )
    {
    if ( same == 0 )
        lastrec_matches = TRUE;
    else
        lastrec_matches = FALSE;

    while ( firstrec < lastrec-1 )
        {
        rn = (firstrec + lastrec)/2;
        status = compare_one_record( &same, rn, nof_key_fields );
        if ( !status )
            break;

        if ( same < 0 )
            firstrec = rn;
        else
            {
            lastrec = rn;
            if ( same == 0 )
                lastrec_matches = TRUE;
            }
        }
    }

if ( status )
    {
    if ( lastrec_matches )
        {
        goto_record_number( lastrec );
        return TRUE;
        }
    }

/*
----------------------------------------------------
Copy the searchbuf fields back to the current record
---------------------------------------------------- */
restore_from_searchbuf();

return FALSE;
}

/***********************************************************************
*                   CSV_CLASS::GOTO_FIRST_KEY_RECORD                    *
***********************************************************************/
BOOLEAN CSV_CLASS::goto_first_key_record( short nof_key_fields )
{
int   lastrec;

lastrec  = nof_recs();
lastrec--;

return goto_key_record( nof_key_fields, 0, lastrec );
}

/***********************************************************************
*              CSV_CLASS::GOTO_FIRST_GREATER_THAN_RECORD                *
*                                                                      *
*  This positions you at the beginning of the record but does          *
*  NOT read the record.                                                *
***********************************************************************/
BOOLEAN CSV_CLASS::goto_first_greater_than_record( short nof_key_fields )
{
int   firstrec;
int   lastrec;
int   rn;
short   same;
BOOLEAN status;

if ( !fill_searchbuf(nof_key_fields) )
    return FALSE;

/*
-------------------------------------
Get the first and last record numbers
------------------------------------- */
firstrec = 0;
lastrec  = nof_recs();
if ( !lastrec )
    return FALSE;

lastrec--;

/*
---------------------
Check the last record
--------------------- */
status = compare_one_record( &same, lastrec, nof_key_fields );

/*
----------------------------------------------------------------
If the last record is less or equal there are no greater records
---------------------------------------------------------------- */
if ( status && same <= 0 )
    {
    restore_from_searchbuf();
    status = FALSE;
    }

/*
-----------------------------------------------------
If the first record is > than the search string, quit
----------------------------------------------------- */
if ( status )
    status = compare_one_record( &same, firstrec, nof_key_fields );

if ( status && same > 0 )
    {
    restore_from_searchbuf();
    return goto_record_number( firstrec );
    }

if ( status )
    {
    while ( firstrec < lastrec-1 )
        {
        rn = (firstrec + lastrec)/2;
        status = compare_one_record( &same, rn, nof_key_fields );
        if ( !status )
            break;

        if ( same <= 0 )
            firstrec = rn;
        else
            lastrec = rn;
        }
    }

if ( status )
    return goto_record_number( lastrec );

restore_from_searchbuf();
return FALSE;
}

/***********************************************************************
*            CSV_CLASS::GOTO_FIRST_EQUAL_OR_GREATER_RECORD              *
*                                                                      *
*  This positions you at the beginning of the record but does          *
*  NOT read the record.                                                *
***********************************************************************/
BOOLEAN CSV_CLASS::goto_first_equal_or_greater_record( short nof_key_fields )
{
int   firstrec;
int   lastrec;
int   rn;
short   same;
BOOLEAN status;

if ( !fill_searchbuf(nof_key_fields) )
    return FALSE;

/*
-------------------------------------
Get the first and last record numbers
------------------------------------- */
firstrec = 0;
lastrec  = nof_recs();
if ( !lastrec )
    return FALSE;

lastrec--;

/*
---------------------
Check the last record
--------------------- */
status = compare_one_record( &same, lastrec, nof_key_fields );

/*
-------------------------------------------------------
If the last record is less there are no greater records
------------------------------------------------------- */
if ( status && same < 0 )
    {
    restore_from_searchbuf();
    return FALSE;
    }

/*
------------------------------------------------------
If the first record is >= than the search string, quit
--------------------------_--------------------------- */
if ( status )
    status = compare_one_record( &same, firstrec, nof_key_fields );

if ( status && same >= 0 )
    {
    restore_from_searchbuf();
    return goto_record_number( firstrec );
    }

if ( status )
    {
    while ( firstrec < lastrec-1 )
        {
        rn = (firstrec + lastrec)/2;
        status = compare_one_record( &same, rn, nof_key_fields );
        if ( !status )
            break;

        if ( same < 0 )
            firstrec = rn;
        else
            lastrec = rn;
        }
    }

restore_from_searchbuf();

if ( status )
    return goto_record_number( lastrec );

return FALSE;
}

/***********************************************************************
*                          GET_NEXT_KEY_MATCH                          *
***********************************************************************/
BOOLEAN CSV_CLASS::get_next_key_match( short nof_key_fields  )
{
int   firstrec;
int   lastrec;


if ( globalerr != VS_SUCCESS || !th )
    return FALSE;

if ( search_mode == SEARCHFIRST )
    {
    if ( !goto_first_key_record(nof_key_fields) )
        return FALSE;
    search_mode = SEARCHNEXT;
    }
else
    {
    firstrec = recnum + 1;
    lastrec  = nof_recs() - 1;
    if ( !goto_key_record(nof_key_fields, firstrec, lastrec) )
        return FALSE;
    }

return get_rec();
}

/***********************************************************************
*                              TRIM_TEXT                               *
***********************************************************************/
static short trim_text( TCHAR * sorc, short max_len )
{
TCHAR * cp;
short   n;

n  = 0;
cp = sorc;

/*
--------------------------------------
Move to one past the end of the string
-------------------------------------- */
while ( *cp != STRING_DELIMITER && n < max_len )
    {
    cp++;
    n++;
    }

/*
----------------------
Backup past any spaces
---------------------- */
while ( n )
    {
    cp--;
    if ( *cp != SpaceChar )
        break;
    n--;
    }

return n;
}

/***********************************************************************
*                      CSV_CLASS::GET_ALPHA                             *
*             Max len = max len NOT including the NULL.                *
***********************************************************************/
BOOLEAN CSV_CLASS::get_alpha( LPTSTR dest, FIELDOFFSET fo, short max_len, BITSETYPE options )
{
TCHAR * cp;
short   n;

*dest = '\0';

if ( globalerr == VS_SUCCESS )
    {
    cp = recbuf + fo;

    /*
    -------------------------
    Increment past the quotes
    ------------------------- */
    cp++;

    if ( options & WITH_TRIM )
        max_len = trim_text( cp, max_len );

    n    = 0;
    while ( *cp != STRING_DELIMITER && n < max_len )
        {
        *dest = *cp;
        dest++;
        cp++;
        n++;
        }

    *dest = NullChar;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              CSV_CLASS                                *
*                           GET_TEXT_FIELD                             *
*      Copy the contents of the field (no quotes) as text              *
*         Automatically remove spaces from the left and right.         *
***********************************************************************/
BOOLEAN CSV_CLASS::get_text_field( TCHAR * dest, FIELDOFFSET fo )
{
TCHAR * cp;
TCHAR * dp;

dp = dest;
cp = recbuf + fo;

if ( *cp == STRING_DELIMITER )
    cp++;

/*
-------------------
Skip Leading Spaces
------------------- */
while ( *cp == SpaceChar )
    cp++;

while ( *cp != FIELD_DELIMITER && *cp != RECORD_DELIMITER && *cp != STRING_DELIMITER )
    *dp++ = *cp++;

*dp = NullChar;

/*
------------------------------------------
Remove spaces from the right of the string
------------------------------------------ */
while ( dp > dest )
    {
    dp--;
    if ( *dp != SpaceChar )
        break;
    *dp = NullChar;
    dp--;
    }

return TRUE;
}

/***********************************************************************
*                         CSV_CLASS::GET_ALPHA                          *
*                                                                      *
*      The default is to trim the string of trailing spaces.           *
***********************************************************************/
BOOLEAN CSV_CLASS::get_alpha( LPTSTR dest, FIELDOFFSET fo, short max_len )
{

return get_alpha( dest, fo, max_len, WITH_TRIM );

}

/***********************************************************************
*                         CSV_CLASS::GET_ALPHA                          *
* I trim the text myself and then pass the length I want in max_len    *
* just as the actual get_alpha would do.                               *
***********************************************************************/
BOOLEAN CSV_CLASS::get_alpha( STRING_CLASS & dest, FIELDOFFSET fo, short max_len )
{
TCHAR * cp;

if ( globalerr == VS_SUCCESS )
    {
    /*
    --------------------------------------------------------------------
    I need to know the length of the string so I can set the string size
    -------------------------------------------------------------------- */
    cp = recbuf + fo + 1;
    max_len = trim_text( cp, max_len );
    if ( max_len )
        {
        dest.upsize( max_len );
        return get_alpha( dest.text(), fo, max_len, NO_TRIM );
        }
    }

return FALSE;
}

/***********************************************************************
*                          CSV_CLASS::GET_CHAR                          *
***********************************************************************/
TCHAR CSV_CLASS::get_char( FIELDOFFSET fo )
{
TCHAR s[2];

get_alpha( s, fo, 1, FALSE );

return *s;
}

/***********************************************************************
*                        CSV_CLASS::GET_BOOLEAN                         *
***********************************************************************/
BOOLEAN CSV_CLASS::get_boolean( FIELDOFFSET fo )
{
TCHAR s[2];
BOOLEAN status;

status = FALSE;
if ( get_alpha(s, fo, 1, FALSE) )
    if ( *s == 'Y' || *s == 'y' )
        status = TRUE;

return status;
}

/***********************************************************************
*                         CSV_CLASS::GET_DATE                           *
*                                                                      *
***********************************************************************/
BOOLEAN CSV_CLASS::get_date( SYSTEMTIME & dest, FIELDOFFSET fo )
{

return extmdy( dest, recbuf + fo );

}

/***********************************************************************
*                         CSV_CLASS::GET_LONG                           *
***********************************************************************/
long CSV_CLASS::get_long( FIELDOFFSET fo )
{
LPTSTR cp;
long x;

x = 0L;
if ( globalerr == VS_SUCCESS )
    {
    cp = recbuf + fo;
    x = extlong( cp, fieldlength(cp) );
    }

return x;
}

/***********************************************************************
*                         CSV_CLASS::GET_DOUBLE                         *
***********************************************************************/
double CSV_CLASS::get_double( FIELDOFFSET fo )
{

LPTSTR cp;
double x;

x = 0.0;
if ( globalerr == VS_SUCCESS )
    {
    cp = recbuf + fo;
    x = extdouble( cp, fieldlength(cp) );
    }

return x;
}

/***********************************************************************
*                         CSV_CLASS::GET_TIME                           *
***********************************************************************/
BOOLEAN CSV_CLASS::get_time( SYSTEMTIME & dest, FIELDOFFSET fo )
{

return exthms( dest, recbuf + fo );

}


/***********************************************************************
*                       POINT_TO_START_OF_FIELD                        *
***********************************************************************/
LPTSTR CSV_CLASS::point_to_start_of_field( FIELDOFFSET fo )
{

LPTSTR cp;

cp = recbuf + fo;

if ( fo )
    {
    /*
    ------------------------------------------------
    Make sure there is a field delimiter before this
    ------------------------------------------------ */
    cp--;
    *cp = FIELD_DELIMITER;
    cp++;
    }

return cp;
}


/***********************************************************************
*                              PUT_ALPHA                               *
*                                                                      *
*   The field length does NOT include the quotes or the delimiter.     *
*                                                                      *
***********************************************************************/
BOOLEAN CSV_CLASS::put_alpha( FIELDOFFSET fo, const TCHAR * sorc, short field_len )
{

LPTSTR cp;

if ( globalerr != VS_SUCCESS )
    return FALSE;

cp = point_to_start_of_field( fo );

*cp++ = STRING_DELIMITER;

copyfromstr( cp, sorc, field_len );

cp += field_len;
*cp++ = STRING_DELIMITER;

add_field_delimiter( cp );

return TRUE;
}

/***********************************************************************
*                          CSV_CLASS::PUT_CHAR                          *
***********************************************************************/
BOOLEAN CSV_CLASS::put_char( FIELDOFFSET fo, TCHAR sorc )
{
TCHAR s[2];

*s     = sorc;
*(s+1) = TEXT('\0');

return put_alpha( fo, s, 1 );
}

/***********************************************************************
*                        CSV_CLASS::PUT_BOOLEAN                         *
***********************************************************************/
BOOLEAN CSV_CLASS::put_boolean( FIELDOFFSET fo, BOOLEAN sorc )
{
TCHAR s[2];

if ( sorc )
    *s = TEXT('Y');
else
    *s = TEXT('N');

*(s+1) = TEXT('\0');

return put_alpha( fo, s, 1 );
}

/***********************************************************************
*                         CSV_CLASS::PUT_DATE                           *
*              The fieldlength is always ALPHADATE_LEN.                *
***********************************************************************/
BOOLEAN CSV_CLASS::put_date( FIELDOFFSET fo, const SYSTEMTIME & sorc )
{

TCHAR * cp;

cp = recbuf + fo;

copychars( cp, alphadate(sorc), ALPHADATE_LEN );

cp += ALPHADATE_LEN;
add_field_delimiter( cp );

return TRUE;
}

/***********************************************************************
*                              PUT_DOUBLE                              *
***********************************************************************/
BOOLEAN CSV_CLASS::put_double( FIELDOFFSET fo, const double sorc, short field_len )
{

LPTSTR cp;

if ( globalerr != VS_SUCCESS )
    return FALSE;

cp = point_to_start_of_field( fo );

insalph( cp, sorc, field_len, ' ' );
cp += field_len;
add_field_delimiter( cp );

return TRUE;
}

/***********************************************************************
*                              PUT_FLOAT                               *
***********************************************************************/
BOOLEAN CSV_CLASS::put_float( FIELDOFFSET fo, const float sorc, short field_len )
{

LPTSTR cp;

if ( globalerr != VS_SUCCESS )
    return FALSE;

cp = point_to_start_of_field( fo );

insalph( cp, sorc, field_len, ' ' );

cp += field_len;
add_field_delimiter( cp );

return TRUE;
}

/***********************************************************************
*                              PUT_LONG                                *
***********************************************************************/
BOOLEAN CSV_CLASS::put_long( FIELDOFFSET fo, const long sorc, short field_len )
{

LPTSTR cp;

if ( globalerr != VS_SUCCESS )
    return FALSE;

cp = point_to_start_of_field( fo );

insalph( cp, sorc, field_len, ' ', DECIMAL_RADIX );

cp += field_len;
add_field_delimiter( cp );

return TRUE;
}

/***********************************************************************
*                         CSV_CLASS::PUT_TIME                           *
*              The fieldlength is always ALPHATIME_LEN.                *
***********************************************************************/
BOOLEAN CSV_CLASS::put_time( FIELDOFFSET fo, SYSTEMTIME & sorc )
{
TCHAR * cp;

cp = recbuf + fo;

copychars( cp, alphatime(sorc), ALPHATIME_LEN );

cp += ALPHATIME_LEN;
add_field_delimiter( cp );

return TRUE;
}

/***********************************************************************
*                        CSV_CLASS::WRITE_RECORD                        *
***********************************************************************/
BOOLEAN CSV_CLASS::write_record()
{
BOOL status;
DWORD bytes_written;
DWORD bytes_to_write;

bytes_to_write = (DWORD) recbuflen;
bytes_to_write *= sizeof( TCHAR );

if ( !(table_lock_type & (WL | FL)) )
    return FALSE;

status = WriteFile( th, recbuf, bytes_to_write, &bytes_written, 0 );
if ( !status || bytes_written != bytes_to_write )
    {
    last_error = GetLastError();
    return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                    CSV_CLASS::REC_UPDATE                              *
***********************************************************************/
BOOLEAN CSV_CLASS::rec_update()
{
if ( globalerr != VS_SUCCESS )
    return FALSE;

if ( !goto_record_number(recnum) )
    return FALSE;

return write_record();
}

/***********************************************************************
*                         CSV_CLASS::REC_APPEND                         *
***********************************************************************/
BOOLEAN CSV_CLASS::rec_append()
{

DWORD n;

if ( globalerr != VS_SUCCESS )
    return FALSE;

n = nof_recs();

if ( !goto_record_number(n) )
    return FALSE;

return write_record();
}

/***********************************************************************
*                         CSV_CLASS::REC_INSERT                         *
***********************************************************************/
BOOLEAN CSV_CLASS::rec_insert()
{

int sn;
int cn;
TCHAR * buf;

if ( globalerr != VS_SUCCESS )
    return FALSE;

/*
---------------------------------
Make a copy of the current record
--------------------------------- */
buf = new TCHAR[recbuflen+1];
if ( !buf )
    return FALSE;

copychars( buf, recbuf, recbuflen+1 );

/*
------------------------------
Copy all of the records up one
------------------------------ */
sn = nof_recs();
if ( sn )
    {
    sn--;
    cn = recnum;

    while ( sn >= cn )
        {
        if ( goto_record_number(sn) )
            if ( get_rec() )
                write_record();
        sn--;
        }
    }

/*
--------------------------
Restore the record to save
-------------------------- */
copychars( recbuf, buf, recbuflen+1 );
if ( goto_record_number(cn) )
    write_record();

delete[] buf;

return TRUE;
}

/***********************************************************************
*                         CSV_CLASS::REC_DELETE                         *
*                          Delete n records                            *
***********************************************************************/
BOOLEAN CSV_CLASS::rec_delete( int ntokill )
{
char * buf;
DWORD dest;
DWORD sorc;

DWORD bytes_copied;
DWORD bytes_to_copy;
DWORD bytes_per_record;
DWORD n;

DWORD current_record_number;
DWORD sorc_record_number;
DWORD number_of_records;

if ( globalerr != VS_SUCCESS )
    return FALSE;

if ( ntokill < 1 )
    return TRUE;

number_of_records     = nof_recs();
current_record_number = recnum;

if ( (current_record_number + ntokill) > number_of_records )
    ntokill = number_of_records - current_record_number;

sorc_record_number = current_record_number + ntokill;

bytes_per_record = (DWORD) recbuflen;
bytes_per_record *= sizeof( TCHAR );
sorc = sorc_record_number * bytes_per_record;
dest = current_record_number * bytes_per_record;

bytes_to_copy = ( number_of_records - sorc_record_number ) * bytes_per_record;

while ( bytes_to_copy > 0 )
    {
    n = bytes_to_copy;
    while ( TRUE )
        {
        buf = new char[n];
        if (buf)
            {
            SetFilePointer( th, sorc, 0, FILE_BEGIN );
            ReadFile( th, buf, n, &bytes_copied, 0 );
            SetFilePointer( th, dest, 0, FILE_BEGIN );
            WriteFile( th, buf, n, &bytes_copied, 0 );
            sorc += n;
            dest += n;
            bytes_to_copy -= n;
            delete[] buf;
            break;
            }
        else if ( n < 2 )
            {
            return FALSE;
            }
        else
            {
            n /= 2;
            }
        }
    }

number_of_records -= ntokill;
if ( goto_record_number(number_of_records) )
    SetEndOfFile( th );

if ( current_record_number >= number_of_records )
    {
    if ( number_of_records > 0 )
        {
        current_record_number = number_of_records - 1;
        if ( goto_record_number(current_record_number) )
            get_rec();
        }
    else
        {
        recnum = 0;
        }
    }

return TRUE;
}

/***********************************************************************
*                          CSV_CLASS::COPY_REC                          *
*                                                                      *
*              Copy a record from one like table to another            *
***********************************************************************/
BOOLEAN CSV_CLASS::copy_rec( CSV_CLASS & sorcdb )
{

if ( sorcdb.recbuflen != recbuflen )
    return FALSE;

copychars( recbuf, sorcdb.recbuf, recbuflen+1 );

return TRUE;
}
