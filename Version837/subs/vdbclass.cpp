#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\chars.h"
#include "..\include\array.h"
#include "..\include\vdbclass.h"
#include "..\include\subs.h"

#define DB_OPEN_WAIT_MS        10  /* Clock counts to wait between open tries */
#define DB_OPEN_TIMEOUT_COUNT  30  /* Max tries to open table  */

/*
------------------------------------------
Parameters for get_first_key_match routine
------------------------------------------ */
const bool    EQUAL_ONLY_SEARCH           = false;
const bool    EQUAL_OR_GREATER_SEARCH     = true;

const int    MAX_RECNUM_LEN              = 12;
const int    SIZE_OF_TCHAR               = sizeof( TCHAR );
const DWORD  TBYTES                      = sizeof( TCHAR );
const int    RECORD_DELIMITER_LEN        = 2;
const DWORD  RECORD_DELIMITER_BYTES      = DWORD(RECORD_DELIMITER_LEN) * TBYTES;
const TCHAR  RECORD_DELIMITER_STRING[]   = { '\015', '\012', '\0' };
const TCHAR  RECORD_DELIMITER_START_CHAR = TEXT( '\015' );
const TCHAR  RECORD_DELIMITER_END_CHAR   = TEXT( '\012' );
const TCHAR  FIELD_DELIMITER             = TEXT( ',' );

static SECURITY_ATTRIBUTES SecurityAttributes = {sizeof(SecurityAttributes), 0, FALSE };

/***********************************************************************
*                               IS_DATE                                *
***********************************************************************/
bool is_date( STRING_CLASS & sorc )
{
TCHAR * cp;

if ( sorc.len() != ALPHADATE_LEN )
    return false;

cp = sorc.text();

cp += 2;
if ( *cp != ForwardSlashChar )
    return false;

cp += 2;
if ( *cp != ForwardSlashChar )
    return false;

return true;
}

/***********************************************************************
*                           COMPARE_DATE                               *
*                            12/25/1996                                *
*                            0123456789                                *
*       -1 means np < cp; 0 means np = cp; 1 means np > cp;            *
***********************************************************************/
static int compare_date( TCHAR * np, TCHAR * cp )
{
short status;

status = compare( np+6, cp+6, 4 );

if ( status == 0 )
    status = compare( np, cp, 5 );

return (int) status;
}

/***********************************************************************
*                        VDB_CLASS::VDB_CLASS                          *
***********************************************************************/
VDB_CLASS::VDB_CLASS()
{
th                = 0;
ra                = 0;
sa                = 0;
buf[0]            = NullChar;
buf[READ_BUF_LEN] = NullChar;
recnum            = NO_RECNUM;
reclen            = NO_RECLEN;
open_type         = NOT_OPEN;
nrecs             = NO_RECNUM;
filepos           = NO_FILE_POSITION;
filesize          = 0;
nextpos           = NO_FILE_POSITION;
search_mode       = SEARCHFIRST;
dberr             = VS_SUCCESS;
}

/***********************************************************************
*                       VDB_CLASS::~VDB_CLASS                          *
***********************************************************************/
VDB_CLASS::~VDB_CLASS()
{
if ( th )
    {
    CloseHandle( th );
    th = 0;
    }
}

/***********************************************************************
*                                CLOSE                                 *
***********************************************************************/
void VDB_CLASS::close()
{
if ( th )
    {
    CloseHandle( th );
    th = 0;
    }

open_type = NOT_OPEN;
}

/***********************************************************************
*                                EMPTY                                 *
***********************************************************************/
bool VDB_CLASS::empty()
{
DWORD status;

status = SetFilePointer( th, 0, 0, FILE_BEGIN );
if ( status != 0xFFFFFFFF )
    {
    SetEndOfFile( th );
    return true;
    }

return false;
}

/***********************************************************************
*                                OPEN                                  *
*                                                                      *
*  If the time_out count is > 0 then I will issue an error if the      *
*  open fails.                                                         *
*  If I timeout I set the dberr to ERROR_TIMEOUT which is a system     *
*  code for just that.                                                 *
***********************************************************************/
bool VDB_CLASS::open( TCHAR * dbname, int new_open_type )
{
DWORD access_mode;
DWORD share_mode;
DWORD ms_to_wait;
bool first_time;
int  time_out;

name            = dbname;
open_type       = new_open_type;
search_mode     = SEARCHFIRST;
reclen          = NO_RECLEN;
recnum          = NO_RECNUM;
nrecs           = NO_RECNUM;
nextpos         = NO_FILE_POSITION;
time_out = DB_OPEN_TIMEOUT_COUNT;

if ( th )
    {
    CloseHandle( th );
    th = 0;
    }

dberr = VS_SUCCESS;

first_time = true;

ms_to_wait = DB_OPEN_WAIT_MS;

switch (open_type )
    {
    case OPEN_FOR_READ:
        access_mode = GENERIC_READ;
        share_mode  = FILE_SHARE_READ;
        break;

    case OPEN_FOR_READ_WRITE:
        access_mode = GENERIC_READ | GENERIC_WRITE;
        share_mode  = 0;
        break;

    case OPEN_FOR_SHARED_READ_WRITE:
        access_mode = GENERIC_READ | GENERIC_WRITE;
        share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        break;

    case OPEN_FOR_WRITE:
        access_mode = GENERIC_WRITE;
        share_mode  = 0;
        break;

    default:
        open_type = NOT_OPEN;
        return false;
    }

while ( true )
    {
    if ( !th )
        {
        th = CreateFile( name.text(), access_mode, share_mode, &SecurityAttributes, OPEN_ALWAYS, FILE_FLAG_RANDOM_ACCESS, 0 );
        if ( th != INVALID_HANDLE_VALUE )
            {
            dberr = VS_SUCCESS;
            break;
            }
        else
            {
            th = 0;
            dberr = GetLastError();
            }
        }

    /*
    ------------------------------------------------------------------------
    The time_out is only zero if the caller specifically asked for 0 retrys.
    This is not used now but I left it in to remind me how it worked.
    ------------------------------------------------------------------------ */
    if ( time_out == 0 )
        break;

    if ( time_out == 1 )
        break;

    if ( time_out < 9 )
        ms_to_wait *= 2;

    if ( first_time )
        first_time = false;
    else
        Sleep( ms_to_wait );

    time_out--;
    }

if ( dberr == VS_SUCCESS )
    {
    filepos  = 0;
    filesize = GetFileSize( th, NULL );
    if ( open_type == OPEN_FOR_WRITE )
        {
        nrecs = 0;
        }
    else
        {
        if ( filesize == 0 )
            {
            nrecs = 0;
            }
        else
            {
            /*
            ---------
            Get nrecs
            --------- */
            goto_last_record();
            rewind();
            }
        }
    return true;
    }

return false;
}

/***********************************************************************
*                             OPEN_FOR_READ                            *
***********************************************************************/
bool VDB_CLASS::open_for_read( TCHAR * dbname )
{
return open( dbname, OPEN_FOR_READ );
}

/***********************************************************************
*                             OPEN_FOR_READ                            *
***********************************************************************/
bool VDB_CLASS::open_for_read( STRING_CLASS dbname )
{
return open( dbname.text(), OPEN_FOR_READ );
}

/***********************************************************************
*                             OPEN_FOR_RW                              *
***********************************************************************/
bool VDB_CLASS::open_for_rw( TCHAR * dbname )
{
return open( dbname, OPEN_FOR_READ_WRITE );
}

/***********************************************************************
*                             OPEN_FOR_RW                              *
***********************************************************************/
bool VDB_CLASS::open_for_rw( STRING_CLASS & dbname )
{
return open( dbname.text(), OPEN_FOR_READ_WRITE );
}

/***********************************************************************
*                          OPEN_FOR_SHARED_RW                          *
***********************************************************************/
bool VDB_CLASS::open_for_shared_rw( TCHAR * dbname )
{
return open( dbname, OPEN_FOR_SHARED_READ_WRITE );
}

/***********************************************************************
*                          OPEN_FOR_SHARED_RW                          *
***********************************************************************/
bool VDB_CLASS::open_for_shared_rw( STRING_CLASS & dbname )
{
return open( dbname.text(), OPEN_FOR_SHARED_READ_WRITE );
}

/***********************************************************************
*                            OPEN_FOR_WRITE                            *
***********************************************************************/
bool VDB_CLASS::open_for_write( TCHAR * dbname )
{
return open( dbname, OPEN_FOR_WRITE );
}

/***********************************************************************
*                            OPEN_FOR_WRITE                            *
***********************************************************************/
bool VDB_CLASS::open_for_write( STRING_CLASS & dbname )
{
return open( dbname.text(), OPEN_FOR_WRITE );
}

/***********************************************************************
*                               CREATE                                 *
***********************************************************************/
bool VDB_CLASS::create( TCHAR * dbname )
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
    close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             ENSURE_EXISTANCE                         *
***********************************************************************/
bool VDB_CLASS::ensure_existance( STRING_CLASS & dbname )
{
name = dbname;

if ( !file_exists(name.text()) )
    return create( name );

return TRUE;
}

/***********************************************************************
*                               OPEN_STATUS                            *
***********************************************************************/
bool VDB_CLASS::open_status()
{
if ( th )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                         CURRENT_RECORD_LENGTH                        *
*      Return the number of TCHARs read for the current record         *
***********************************************************************/
int VDB_CLASS::current_record_length()
{
return reclen;
}

/***********************************************************************
*                       CALCULATED_RECORD_LENGTH                       *
* Return the number of TCHARs needed to write the current record,      *
* including the FIELD_DELIMITER and the RECORD_DELIMITER.              *
***********************************************************************/
int VDB_CLASS::calculated_record_length()
{
int n;
STRING_CLASS s;

s = recnum;
n = s.len();
n++;
ra.rewind();
while ( ra.next() )
    {
    /*
    ------------
    Field length
    ------------ */
    n += ra.len();

    /*
    ---------------
    Field delimiter
    --------------- */
    n++;
    }

/*
------------------------------------
No field delimiter on the last field
------------------------------------ */
n--;

/*
----------------
Record delimiter
---------------- */
n += RECORD_DELIMITER_LEN;

return n;
}

/***********************************************************************
*                    GET_FIRST_EQUAL_OR_GREATER_DATE                   *
*                               12/25/1996                             *
***********************************************************************/
bool VDB_CLASS::get_first_equal_or_greater_date( int date_col, SYSTEMTIME & date_to_find )
{
TCHAR s[ALPHADATE_LEN+1];
int   firstrec;
int   lastrec;
int   rn;
int   same;

copystring( s, alphadate(date_to_find) );

if ( nrecs == NO_RECNUM )
    {
    if ( !get_last_record() )
        return false;
    }

/*
-------------------------------------
Get the first and last record numbers
------------------------------------- */
firstrec = 0;
lastrec  = nrecs;
if ( lastrec < 1 )
    return false;
lastrec--;

/*
---------------------
Check the last record
--------------------- */
if ( !goto_record_number(lastrec) )
    return false;
if ( !get_rec() )
    return false;

ra.goto_index( date_col );
same = compare_date( ra.text(), s );

/*
-------------------------------------------------------
If the last record is less there are no greater records
------------------------------------------------------- */
if ( same < 0 )
    return false;

/*
------------------------------------------------------
If the first record is >= than the search string, quit
--------------------------_--------------------------- */
if ( !goto_record_number(firstrec) )
    return false;

if ( !get_rec() )
    return false;

ra.goto_index( date_col );
same = compare_date( ra.text(), s );
if ( same >= 0 )
    return true;

while ( firstrec < lastrec-1 )
    {
    rn = (firstrec + lastrec)/2;
    if ( !goto_record_number(rn) )
        return false;

    if ( !get_rec() )
        return false;

    ra.goto_index( date_col );
    same = compare_date( ra.text(), s );

    if ( same < 0 )
        firstrec = rn;
    else
        lastrec = rn;
    }

return true;
}

/***********************************************************************
*                        GO_TO_START_OF_RECORD                         *
* Go to the beginning of the current record (this is used by           *
* goto_record_number to find the begining of the current record when   *
* it is searching).                                                    *
*                                                                      *
* get_previous_rec sets the filepos to the begining of the             *
* current record -2 and then calls this routine.                       *
*                                                                      *
* It is NOT assumed that the actual file pointer is the same as        *
* filepos.                                                             *
*                                                                      *
* Returns the current record number but does NOT set recnum            *
***********************************************************************/
int VDB_CLASS::goto_start_of_record()
{
TCHAR  * bp;
DWORD    bytes_left;
DWORD    bytes_read;
DWORD    bytes_to_read;
int      i;
int      n;
bool     found_start_of_record;
DWORD    status;

found_start_of_record = false;

while ( true )
    {
    if ( filepos == 0 )
        found_start_of_record = true;

    if ( found_start_of_record )
        break;

    bytes_left = filepos + TBYTES;
    bytes_to_read = READ_BUF_LEN;
    bytes_to_read *= TBYTES;
    if ( bytes_to_read > bytes_left )
        bytes_to_read = bytes_left;

    /*
    ------------------------------------------------------------------
    Set the actual file position so the filepos is the last byte read.
    ------------------------------------------------------------------ */
    status = SetFilePointer( th, (filepos + TBYTES - bytes_to_read), 0, FILE_BEGIN );
    status = ReadFile( th, buf, bytes_to_read, &bytes_read, 0 );
    if ( !status )
        {
        dberr = GetLastError();
        return false;
        }
    i = bytes_to_read/TBYTES;
    i--;
    while ( i>=0 )
        {
        if ( buf[i] == RECORD_DELIMITER_END_CHAR )
            {
            /*
            -----------------------------------------
            The next tchar is the start of the record
            ----------------------------------------- */
            i++;
            filepos += SIZE_OF_TCHAR;
//            status = SetFilePointer( th, filepos, 0, FILE_BEGIN );
            found_start_of_record = true;
            break;
            }
        if ( i == 0 )
            {
            found_start_of_record = true;
            break;
            }
        i--;
        filepos -= SIZE_OF_TCHAR;
        }
    }

if ( found_start_of_record )
    {

    /*
    -----------------------------------------
    See if the record number is in the buffer
    ----------------------------------------- */
    n = (int) ( bytes_to_read / TBYTES );
    if ( n > i )
        {
        bp = buf+i;
        while ( i < n )
            {
            if (buf[i] == FIELD_DELIMITER )
                {
                buf[i] = NullChar;
                return asctoint( bp );
                }
            i++;
            }
        }

    /*
    -----------------------------------------------------------------
    The record number was not in the buffer so read enough to get it.
    ----------------------------------------------------------------- */
    SetFilePointer( th, filepos, 0, FILE_BEGIN );
    bytes_to_read = MAX_RECNUM_FIELD_LENGTH;
    bytes_left    = filesize - filepos;
    if ( bytes_left < bytes_to_read )
        bytes_to_read = bytes_left;
    status = ReadFile( th, buf, bytes_to_read, &bytes_read, 0 );

    for ( i=0; i<n; i++ )
        {
        if ( buf[i] == FIELD_DELIMITER )
            {
            buf[i] = NullChar;
            return asctoint( buf );
            }
        }
    }

return NO_RECNUM;
}

/***********************************************************************
*                           GOTO_RECORD_NUMBER                         *
*                              [0,nrecs-1]                             *
*  Change the current record number to the requested record number     *
*  and position at the start of that record.                           *
*    Even though this does not read the record it sets the recnum.     *
***********************************************************************/
bool VDB_CLASS::goto_record_number( int dest_record_number )
{
DWORD high_pos;
int   high_number;

DWORD low_pos;
int   low_number;

int   middle_number;

if ( filesize == 0 )
    return false;

if ( dest_record_number < 0 )
    return false;

if ( dest_record_number == 0 )
    {
    filepos = 0;
    recnum  = 0;
    search_mode = SEARCHNEXT;
    return true;
    }

if ( !goto_last_record() )
    return false;

if ( dest_record_number >= nrecs )
    return false;

search_mode = SEARCHNEXT;

low_number = 0;
low_pos    = 0;

high_number = nrecs - 1;
high_pos    = filepos;

if ( dest_record_number == high_number )
    {
    recnum = dest_record_number;
    return true;
    }

while ( true )
    {
    filepos = ( high_pos + low_pos ) / 2;

#ifdef UNICODE
    /*
    ---------------------------------------
    Make sure I'm on a even number position
    --------------------------------------- */
    if ( filepos%2 )
        filepos++;
#endif
    middle_number = goto_start_of_record();

    if ( middle_number == low_number )
        middle_number = goto_next_record();
    else if ( middle_number == high_number )
        middle_number = goto_prev_record();

    if ( middle_number == dest_record_number )
        {
        recnum = dest_record_number;
        return true;
        }

    if ( middle_number < dest_record_number )
        {
        low_number = middle_number;
        low_pos = filepos;
        }
    else
        {
        high_number = middle_number;
        high_pos = filepos;
        }

    if ( high_number <= low_number )
        break;
    }

search_mode = SEARCHFIRST;
return false;
}

/***********************************************************************
*                                GET_REC                               *
* This set the recnum.                                                 *
************************************************************************/
bool VDB_CLASS::get_rec()
{
BOOL    status;
DWORD   bytes_left;
DWORD   bytes_to_read;
DWORD   bytes_read;
int     i;

/*
---------------------
Check for end of file
--------------------- */
if ( filepos >= filesize )
    return false;

/*
----------------------------------------------------------
For now I assume no record will be longer than 4096 tchars
---------------------------------------------------------- */
bytes_to_read  = (DWORD) READ_BUF_LEN;
bytes_to_read *= sizeof( TCHAR );
bytes_left    = filesize - filepos;
if ( bytes_left < bytes_to_read )
    bytes_to_read = bytes_left;

/*
------------------------------------------------------------------
I always position the actual file pointer before reading a record.
------------------------------------------------------------------ */
SetFilePointer( th, filepos, 0, FILE_BEGIN );
status = ReadFile( th, buf, bytes_to_read, &bytes_read, 0 );
if ( !status )
    {
    dberr = GetLastError();
    }
else if ( bytes_read == bytes_to_read )
    {
    for ( i=0; i<READ_BUF_LEN; i++ )
        {
        if ( buf[i] == RECORD_DELIMITER_START_CHAR )
            {
            buf[i] = NullChar;
            ra.empty_array();
            ra = buf;
            ra.split( FIELD_DELIMITER );
            ra.rewind();
            ra.next();
            recnum = ra.int_value();
            ra.remove(1);
            i += RECORD_DELIMITER_LEN;
            nextpos = filepos + i*SIZE_OF_TCHAR;
            reclen  = nextpos - filepos;
#ifdef UNICODE
            reclen /= SIZE_OF_TCHAR;
#endif
            return true;
            }
        }
    }

return false;
}

/***********************************************************************
*                                GET_REC                               *
***********************************************************************/
bool VDB_CLASS::get_rec( int n )
{
if ( goto_record_number(n) )
    return get_rec();

return false;
}

/***********************************************************************
*                         VDB_CLASS::NOF_RECS                           *
***********************************************************************/
int VDB_CLASS::nof_recs()
{
return nrecs;
}
/***********************************************************************
*                       VDB_CLASS::NOF_COLUMNS                         *
***********************************************************************/
int VDB_CLASS::nof_columns()
{
return ra.count();
}

/***********************************************************************
*                           GOTO_LAST_RECORD                           *
*    Even though this does not read the record it sets the recnum.     *
***********************************************************************/
bool VDB_CLASS::goto_last_record()
{
int n;

if ( dberr != VS_SUCCESS )
    return false;

if ( !th )
    return false;

if ( filesize == 0 )
    return false;

filepos = filesize - SIZE_OF_TCHAR;
filepos -= RECORD_DELIMITER_LEN * SIZE_OF_TCHAR;
nextpos = NO_FILE_POSITION;
n = goto_start_of_record();
if ( n != NO_RECNUM )
    {
    recnum = n;
    nrecs  = n + 1;
    return true;
    }
return false;
}

/***********************************************************************
*                            GET_LAST_RECORD                           *
***********************************************************************/
bool VDB_CLASS::get_last_record()
{
if ( goto_last_record() )
    {
    if ( get_rec() )
        {
        nrecs = recnum + 1;
        return true;
        }
    }

return false;
}

/***********************************************************************
*                           GOTO_NEXT_RECORD                           *
*    Even though this does not read the record it sets the recnum.     *
***********************************************************************/
int VDB_CLASS::goto_next_record()
{
int r;

if ( dberr != VS_SUCCESS)
    return NO_RECNUM;

if ( !th )
    return NO_RECNUM;

if ( filesize > 0 && nrecs == NO_RECNUM )
    search_mode = SEARCHFIRST;

if ( search_mode == SEARCHFIRST )
    {
    r = 0;
    filepos = 0;
    search_mode = SEARCHNEXT;
    }
else
    {
    if ( nextpos == NO_FILE_POSITION )
        get_rec();

    r = recnum + 1;

    filepos = nextpos;
    }

nextpos = NO_FILE_POSITION;
if ( filepos >= filesize || filepos == NO_FILE_POSITION )
    return NO_RECNUM;

recnum = r;

return r;
}

/***********************************************************************
*                          COMPARE_KEY_FIELDS                          *
* Compare the first nkeys fields of the sa and ra arrays. Return       *
* -1 if ra < sa. 0 if equal. 1 if ra > sa. Don't use real numbers;     *
***********************************************************************/
int VDB_CLASS::compare_key_fields()
{
int compare_result;
int i;

if ( nkeys < 1 )
    return 0;

compare_result = 0;
for ( i=0; i<nkeys; i++ )
    {
    if ( is_date(ra[i]) && is_date(sa[i]) )
        {
        compare_result = compare_date( ra.text(), sa.text() );
        if ( compare_result != 0 )
            break;
        }
    else if ( ra.len() != sa.len() )
        {
        if ( ra.isnumeric() && sa.isnumeric() )
            {
            compare_result = ra.int_value() - sa.int_value();
            if ( compare_result < 0 )
                compare_result = -1;
            else if ( compare_result > 0 )
                compare_result = 1;
            }
        }
    else
        {
        compare_result = compare( ra.text(), sa.text() );
        }
    }

return compare_result;
}

/***********************************************************************
*                          GET_FIRST_KEY_MATCH                         *
***********************************************************************/
bool VDB_CLASS::get_first_key_match( int nof_key_fields, bool include_greater_in_search )
{
struct RECORD_DATA
    {
    int compare;
    int number;
    int pos;
    };

int i;
RECORD_DATA high;
RECORD_DATA low;
RECORD_DATA middle;
int range;

if ( filesize == 0 )
    return false;

if ( nof_key_fields < 1 )
    return false;

nkeys = nof_key_fields;

/*
------------------------------------------------------------------
The strings to match are in the sa multistring. If these are empty
there is something wrong.
------------------------------------------------------------------ */
for ( i=0; i<nof_key_fields; i++ )
    {
    if ( sa[i].isempty() )
        return false;
    }

rewind();
get_next_record();

low.compare = compare_key_fields();

/*
-----------------------------------------------------------------------
If the first record is greater than the value I'm looking for I am done
----------------------------------------------------------------------- */
if ( low.compare > 0 )
    return include_greater_in_search;

if ( low.compare == 0 )
    return true;

low.number  = 0;
low.pos     = 0;

if ( !get_last_record() )
    return false;

high.compare = compare_key_fields();
high.number = nrecs - 1;
high.pos    = filepos;

/*
-------------------------------------------------------------------
If the last record is still less than what I'm looking for I'm done
------------------------------------------------------------------- */
if ( high.compare < 0 )
    return false;

/*
-------------------------------------------------------------------------
If I am here I know the zeroth record < desired key values <= last record
------------------------------------------------------------------------- */
while ( true )
    {
    /*
    --------------------------------------------------------------------------
    If the low and high are consecutive or if they are the same then I am done
    The high is the first equal record or else there is no equal record.
    -------------------------------------------------------------------------- */
    range = high.number - low.number;
    if ( range <= 1 )
        {
        if ( include_greater_in_search )
            {
            if ( high.compare >= 0 )
                {
                filepos = high.pos;
                goto_start_of_record();
                get_rec();
                return true;
                }
            else
                {
                return false;
                }
            }
        else
            {
            if ( high.compare == 0 )
                {
                filepos = high.pos;
                goto_start_of_record();
                get_rec();
                return true;
                }
            else
                {
                return false;
                }
            }
        }

    filepos = ( high.pos + low.pos ) / 2;

    #ifdef UNICODE
    /*
    ---------------------------------------
    Make sure I'm on a even number position
    --------------------------------------- */
    if ( filepos%2 )
        filepos++;
    #endif
    middle.pos    = filepos;
    middle.number = goto_start_of_record();
    get_rec();
    middle.compare = compare_key_fields();

    /*
    --------------------------------------------------------------------
    I know there is at least one record between the low and high records
    -------------------------------------------------------------------- */
    if ( high.number == middle.number || low.number == middle.number )
        {
        if ( high.number == middle.number )
            get_prev_record();
        else
            get_next_record();
        middle.pos = filepos;
        middle.compare = compare_key_fields();
        }

    if ( middle.compare >= 0 )
        high = middle;
    else
        low = middle;
    }

return false;
}

/***********************************************************************
*                          GET_FIRST_KEY_MATCH                         *
***********************************************************************/
bool VDB_CLASS::get_first_key_match( int nof_key_fields )
{
return get_first_key_match( nof_key_fields, EQUAL_ONLY_SEARCH );
}

/***********************************************************************
*                GET_FIRST_EQUAL_OR_GREATER_KEY_MATCH                  *
***********************************************************************/
bool VDB_CLASS::get_first_equal_or_greater_key_match( int nof_key_fields )
{
return get_first_key_match( nof_key_fields, EQUAL_OR_GREATER_SEARCH );
}

/***********************************************************************
*                  GET_FIRST_GREATER_THAN_KEY_MATCH                    *
***********************************************************************/
bool VDB_CLASS::get_first_greater_than_key_match( int nof_key_fields )
{
if ( get_first_key_match(nof_key_fields, EQUAL_OR_GREATER_SEARCH) )
    {
    while (compare_key_fields() == RECORD_EQUALS_SEARCH )
        {
        if ( !get_next_record() )
            return false;
        }
    return true;
    }

return false;
}

/***********************************************************************
*                          GET_NEXT_KEY_MATCH                          *
*                                                                      *
* Key fields begin in the zeroth column (not counting the recnum col). *
* You put the strings to search for in the coresponding column of the  *
* search array (sa).                                                   *
* e.g. vdb.sa[0] = "Jay";                                              *
*      vdb.sa[1] = "01/23/2011";                                       *
*      vdb.rewind();                                                   *
*      if ( vdb.get_next_key_match(2) )                                *
*          ...                                                         *
***********************************************************************/
bool VDB_CLASS::get_next_key_match( int nof_key_fields )
{
if ( nof_key_fields < 1 )
    return false;

nkeys = nof_key_fields;

if ( search_mode == SEARCHFIRST )
    return get_first_key_match( nof_key_fields );

if ( get_next_record() )
    {
    if ( compare_key_fields() == 0 )
        return true;
    }

return false;
}

/***********************************************************************
*                            GET_NEXT_RECORD                           *
***********************************************************************/
bool VDB_CLASS::get_next_record()
{
if ( goto_next_record() != NO_RECNUM )
    return get_rec();

return false;
}

/***********************************************************************
*                           GOTO_PREV_RECORD                           *
***********************************************************************/
int VDB_CLASS::goto_prev_record()
{
if ( !th )
    return false;

if ( dberr != VS_SUCCESS )
    return false;

if ( filepos < (3*SIZE_OF_TCHAR) )
    return false;

filepos -= (RECORD_DELIMITER_LEN*SIZE_OF_TCHAR);
return goto_start_of_record();
}

/***********************************************************************
*                    VDB_CLASS::GET_PREV_RECORD                         *
*                                                                      *
*  This function normally does NOT store it's result in the global     *
*  variable.                                                           *
*                                                                      *
***********************************************************************/
bool VDB_CLASS::get_prev_record()
{
if ( goto_prev_record() != NO_RECNUM )
    return get_rec();

return false;
}

/***********************************************************************
*                              GET_RECNUM                              *
***********************************************************************/
int VDB_CLASS::get_recnum()
{
DWORD   bytes_left;
DWORD   bytes_read;
DWORD   bytes_to_read;
TCHAR * cp;
int     i;
BOOL    status;

/*
---------------------
Check for end of file
--------------------- */
if ( filepos >= filesize )
    return NO_RECNUM;

bytes_to_read  = (DWORD) MAX_RECNUM_LEN;
bytes_to_read *= TBYTES;
bytes_left    = filesize - filepos;
if ( bytes_left < bytes_to_read )
    bytes_to_read = bytes_left;
SetFilePointer( th, filepos, 0, FILE_BEGIN );
status = ReadFile( th, buf, bytes_to_read, &bytes_read, 0 );
if ( !status )
    {
    dberr = GetLastError();
    return NO_RECNUM;
    }

if ( bytes_read != bytes_to_read )
    return NO_RECNUM;

i = int(bytes_read)/SIZE_OF_TCHAR;
buf[i] = NullChar;
cp = findchar( FIELD_DELIMITER, buf );
if ( cp )
    {
    *cp = NullChar;
    return asctoint( cp );
    }

return NO_RECNUM;
}

/***********************************************************************
*                                 GET_BOOL                             *
***********************************************************************/
bool VDB_CLASS::get_bool( int col )
{
TCHAR c;

if ( col < ra.count() )
    {
    c = *(ra[col].text());
    if ( c == YChar || c == YCharLow )
        return true;
    }
return false;
}

/***********************************************************************
*                         VDB_CLASS::GET_DATE                          *
*                                                                      *
***********************************************************************/
bool VDB_CLASS::get_date( SYSTEMTIME & dest, int col )
{
if ( ra.goto_index(col) )
    {
    if ( extmdy(dest, ra.text()) )
        return true;
    }
return false;
}

/***********************************************************************
*                         VDB_CLASS::GET_TIME                          *
***********************************************************************/
bool VDB_CLASS::get_time( SYSTEMTIME & dest, int col )
{
if ( ra.goto_index(col) )
    {
    if ( exthms(dest, ra.text()) )
        return true;
    }
return false;
}

/***********************************************************************
*                            PUT_BITSETYPE                             *
***********************************************************************/
bool VDB_CLASS::put_bitsetype( int col, BITSETYPE sorc )
{
union {
    unsigned int u;
    BITSETYPE    b;
    };

u = 0;
b = sorc;

if ( ra.goto_index(col) )
    {
    ra = u;
    return true;
    }

return false;
}

/***********************************************************************
*                              PUT_BOOL                                *
***********************************************************************/
bool VDB_CLASS::put_bool( int col, bool sorc )
{
TCHAR c;

c = NChar;
if ( sorc )
    c = YChar;

if ( ra.goto_index(col) )
    {
    ra = c;
    return true;
    }

return false;
}

/***********************************************************************
*                         VDB_CLASS::PUT_DATE                          *
*              The fieldlength is always ALPHADATE_LEN.                *
***********************************************************************/
bool VDB_CLASS::put_date( int col, const SYSTEMTIME & sorc )
{
if ( ra.goto_index(col) )
    {
    ra = alphadate( sorc );
    return true;
    }
return false;
}

/***********************************************************************
*                         VDB_CLASS::PUT_TIME                          *
*              The fieldlength is always ALPHATIME_LEN.                *
***********************************************************************/
bool VDB_CLASS::put_time( int col, SYSTEMTIME & sorc )
{
if ( ra.goto_index(col) )
    {
    ra = alphatime( sorc );
    return true;
    }
return false;
}

/***********************************************************************
*                        VDB_CLASS::WRITE_RECORD                       *
***********************************************************************/
bool VDB_CLASS::write_record()
{
BOOL         status;
DWORD        bytes_written;
DWORD        bytes_to_write;
int          n;
STRING_CLASS s;

if ( open_type != OPEN_FOR_WRITE && open_type != OPEN_FOR_READ_WRITE && open_type != OPEN_FOR_SHARED_READ_WRITE )
    return false;

if ( ra.count() < 1 )
    return false;

s = recnum;
n = s.len();
ra.rewind();
while ( ra.next() )
    {
    n++;  /* Comma */
    n += ra.len();
    }

n += RECORD_DELIMITER_LEN + 1;
if ( !s.upsize(n) )
    return false;

/*
-------------------------------
s already has the record number
------------------------------- */
ra.rewind();
while ( ra.next() )
    {
    s += FIELD_DELIMITER;
    s += ra;
    }
s += RECORD_DELIMITER_STRING;

bytes_to_write = (DWORD) s.len();
bytes_to_write *= sizeof( TCHAR );

status = WriteFile( th, (void *) s.text(), bytes_to_write, &bytes_written, 0 );
if ( !status || bytes_written != bytes_to_write )
    {
    dberr = GetLastError();
    return false;
    }

return true;
}

/***********************************************************************
*                       VDB_CLASS::SET_NOF_COLS                        *
* This is used after opening a file for writing or to allocate the     *
* arrays when reading the first record.                                *
***********************************************************************/
bool VDB_CLASS::set_nof_cols( int n )
{
if ( ra.count() == n )
    return true;

if ( n < 1 )
    {
    ra.empty_array();
    sa.empty_array();
    }
else
    {
    ra.set_array_size( n );
    sa.set_array_size( n );
    }

return true;
}

/***********************************************************************
*                    VDB_CLASS::REC_UPDATE                              *
***********************************************************************/
bool VDB_CLASS::rec_update()
{

if ( dberr != VS_SUCCESS )
    return FALSE;

if ( !goto_record_number(recnum) )
    return FALSE;

return write_record();
}

/***********************************************************************
*                         VDB_CLASS::REC_APPEND                        *
***********************************************************************/
bool VDB_CLASS::rec_append()
{
if ( dberr != VS_SUCCESS )
    return FALSE;

nextpos = NO_FILE_POSITION;
filepos = filesize;
SetFilePointer( th, filepos, 0, FILE_BEGIN );

recnum = nrecs;
if ( write_record() )
    {
    nrecs++;
    search_mode = SEARCHFIRST;
    filesize = GetFileSize( th, NULL );
    return true;
    }

return false;
}

/***********************************************************************
*                         VDB_CLASS::REC_INSERT                        *
***********************************************************************/
bool VDB_CLASS::rec_insert()
{
//int sn;
//int cn;

if ( dberr != VS_SUCCESS )
    return false;

/*
---------------------------------
Make a copy of the current record
--------------------------------- */
//buf = new TCHAR[recbuflen+1];
//if ( !buf )
//    return FALSE;

//copychars( buf, recbuf, recbuflen+1 );

/*
------------------------------
Copy all of the records up one
------------------------------ */
//sn = nrecs;
//if ( sn )
//    {
//    sn--;
//    cn = recnum;
//
//    while ( sn >= cn )
//        {
//        if ( goto_record_number(sn) )
//            if ( get_rec() )
//                write_record();
//        sn--;
//        }
//    }

/*
--------------------------
Restore the record to save
-------------------------- */
//copychars( recbuf, buf, recbuflen+1 );
//if ( goto_record_number(cn) )
//    write_record();

//delete[] buf;

return true;
}

/***********************************************************************
*                         VDB_CLASS::REC_DELETE                         *
*                          Delete n records                            *
***********************************************************************/
bool VDB_CLASS::rec_delete( int ntokill )
{
DWORD bytes_to_write;
DWORD bytes_transferred;
DWORD n;

DWORD delta;
DWORD dest_pos;
DWORD sorc_pos;

TCHAR * bp;
TCHAR * cp;
TCHAR * ep;
TCHAR oldchar;

STRING_CLASS s;
int current_record_number;
int sorc_record_number;

if ( dberr != VS_SUCCESS )
    return false;

if ( ntokill < 1 )
    return true;

if ( nrecs < 1 )
    return false;

if ( recnum < 0 || recnum >= nrecs )
    return false;

/*
-----------------------------------------------------------
Set the current position to the start of the current record
----------------------------------------------------------- */
SetFilePointer( th, filepos, 0, FILE_BEGIN );

/*
-----------------------------------------------------------------
If I am deleting all of the records then just set the end of file
----------------------------------------------------------------- */
if ( recnum == 0 && (ntokill >= nrecs) )
    {
    SetEndOfFile( th );
    recnum            = NO_RECNUM;
    nrecs             = 0;
    filepos           = 0;
    filesize          = 0;
    nextpos           = NO_FILE_POSITION;
    search_mode       = SEARCHFIRST;
    return true;
    }

/*
----------------------------------------------------------------------------------
I'm going to be copying records to the current record so set it as the destination
---------------------------------------------------------------------------------- */
dest_pos = filepos;
current_record_number = recnum;

/*
----------------------------------------------------------------------------------------------
If I'm not deleteing the rest of the records I will be copying current_record_number + ntokill
---------------------------------------------------------------------------------------------- */
sorc_record_number = current_record_number + ntokill;

/*
---------------------------------------------------------------
If the sorc_record number is past the end of the file I want to
find the start of the previous record (now the last record)
and truncate the file at the beginning of the current record.
--------------------------------------------------------------- */
if ( sorc_record_number >= nrecs )
    {
    /*
    -----------------------------
    Remember the current position
    ----------------------------- */
    sorc_pos = filepos;

    /*
    -------------------------------------------
    I am already sitting at the new end of file
    Get the position of the previous record,
    which will be the last record.
    ------------------------------------------- */
    goto_prev_record();
    dest_pos = filepos;

    /*
    ---------------------------------------------------------
    Truncate the file at the beginning of the record I was on
    Since I did not set recnum when I called goto_prev_record
    recnum is now the number of records.
    --------------------------------------------------------- */
    filepos = sorc_pos;
    SetFilePointer( th, filepos, 0, FILE_BEGIN );
    SetEndOfFile( th );
    nrecs    = recnum;
    filesize = filepos;

    /*
    ----------------------------------------
    Position at the start of the last record
    ---------------------------------------- */
    recnum--;
    filepos = dest_pos;
    SetFilePointer( th, filepos, 0, FILE_BEGIN );
    nextpos  = NO_FILE_POSITION;
    return true;
    }

/*
-------------------------------------------------------------
Move to the first record past the one(s) I am going to delete
------------------------------------------------------------- */
if ( !goto_record_number(sorc_record_number) )
    return false;

sorc_pos = filepos;

/*
-----------------------------------------------------------
Set the number of records to what it will be when I am done
----------------------------------------------------------- */
nrecs -= ntokill;

/*
----------------------------------------
Point to the start of the current record
---------------------------------------- */
SetFilePointer( th, filepos, 0, FILE_BEGIN );

/*
---------------------------------------------------
Set recnum to the number of the record just deleted
--------------------------------------------------- */
recnum = current_record_number;

while ( true )
    {
    ReadFile( th, buf, READ_BUF_BYTES, &bytes_transferred, 0 );
    if ( bytes_transferred == 0 )
        break;

    /*
    ---------------------------------------------------
    Put a null at the end of the characters I just read
    --------------------------------------------------- */
    n = bytes_transferred;
    if ( sizeof(TCHAR) > 1 )
        n /= sizeof(TCHAR);
    *(buf+n ) = NullChar;

    /*
    --------------------------------------------
    bp points to the start of the record to read
    -------------------------------------------- */
    bp = buf;

    /*
    -----------------------------------------------------------
    Go to the first comma, the one just past the record number;
    ----------------------------------------------------------- */
    while ( true )
        {
        cp = findchar( FIELD_DELIMITER, bp );
        ep = 0;
        if ( cp )
            {
            ep = findchar( RECORD_DELIMITER_END_CHAR, bp );
            if ( ep )
                {
                ep++;
                oldchar = *ep;
                *ep = NullChar;
                /*
                ----------------------------------------------------------------------
                I've read this record so point bp at the beginning of the next.
                Actually it is one past the beginning but since I'm ignoring
                the record number (I'm renumbering all the records) it doesn't
                matter as long as there is at least one character in the record number
                ---------------------------------------------------------------------- */
                bp = ep;
                }
            }
        if ( !ep )
            {
            /*
            ---------------------------------------------------
            I don't have the whole record in the buffer.
            Move the source position past what I've just copied
            and go back and read from that point in the file.
            --------------------------------------------------- */
            delta = (DWORD) bp;
            delta -= (DWORD) buf;
            sorc_pos += delta;
            break;
            }

        SetFilePointer( th, dest_pos, 0, FILE_BEGIN );
        s = recnum;
        s += cp;
        recnum++;

        /*
        ---------------------------------------
        Replace the character that I nulled out
        --------------------------------------- */
        *ep = oldchar;

        /*
        -----------------------------------------
        Write this record to the current position
        ----------------------------------------- */
        bytes_to_write = (DWORD) (s.len() * sizeof(TCHAR));
        WriteFile( th, (void *) s.text(), bytes_to_write, &bytes_transferred, 0 );
        dest_pos += bytes_transferred;

        if ( recnum == nrecs )
            {
            SetEndOfFile( th );
            break;
            }
        }

    if ( recnum == nrecs )
        break;

    SetFilePointer( th, sorc_pos, 0, FILE_BEGIN );
    }

if ( goto_record_number(current_record_number) )
    get_rec();

return TRUE;
}

/***********************************************************************
*                          VDB_CLASS::COPY_REC                         *
*              Copy a record from one like table to another            *
***********************************************************************/
bool VDB_CLASS::copy_rec( VDB_CLASS & sorcdb )
{
int i;
int n;
int mycount;

n = sorcdb.ra.count();
if ( n == 0 )
    return false;

/*
-----------------------------------------------------------------------------
If I haven't allocated arrays yet I should make the number of cols equivalent
to the number in the sorcdb.
----------------------------------------------------------------------------- */
if ( ra.count() == 0 )
    {
    if ( !set_nof_cols(n) )
        return false;
    }

mycount = ra.count();

/*
-------------------------------------------------------------------------
If the source table has fewer columns than I only copy what are available
if there are more then only copy mycount.
------------------------------------------------------------------------- */
if ( n > mycount )
    n = mycount;

ra.rewind();
sorcdb.ra.rewind();
for ( i=0; i<n; i++ )
    {
    ra.next();
    sorcdb.ra.next();
    ra = sorcdb.ra;
    }
while ( ra.next() )
    ra.null();

ra.rewind();
return true;
}
