#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

#ifdef UNICODE
static TCHAR FinalChar = TEXT( '\xffff' );
#else
static TCHAR FinalChar = TEXT( '\xff' );
#endif

static TCHAR BackSlashChar     = TEXT( '\\' );
static TCHAR CommaChar         = TEXT( ',' );
static TCHAR CrChar            = TEXT( '\r' );
static TCHAR DoubleQuoteChar   = TEXT( '"' );
static TCHAR LfChar            = TEXT( '\n' );
static TCHAR NullChar          = TEXT( '\0' );
static TCHAR OneChar           = TEXT( '1' );
static TCHAR SpaceChar         = TEXT( ' ' );
static TCHAR ZeroChar          = TEXT( '0' );

static TCHAR BackSlashString[] = TEXT( "\\" );
static TCHAR FalseString[]     = TEXT( "N" );
static TCHAR TrueString[]      = TEXT( "Y" );

TCHAR STRING_CLASS::emptystring[] = TEXT( "" );

/***********************************************************************
*                               EMPTY                                  *
***********************************************************************/
void STRING_CLASS::empty()
{
if ( s )
    {
    delete[] s;
    s      = 0;
    maxlen = 0;
    }
}

/***********************************************************************
*                            STRING_CLASS                              *
***********************************************************************/
STRING_CLASS::STRING_CLASS()
{
s      = 0;
maxlen = 0;
}

/***********************************************************************
*                            STRING_CLASS                              *
***********************************************************************/
STRING_CLASS::STRING_CLASS( const TCHAR * sorc )
{
s      = 0;
maxlen = 0;
operator=( sorc );
}

/***********************************************************************
*                            STRING_CLASS                              *
***********************************************************************/
STRING_CLASS::STRING_CLASS( const STRING_CLASS & sorc )
{
s      = 0;
maxlen = 0;
operator=( sorc );
}

/***********************************************************************
*                           ~STRING_CLASS                              *
***********************************************************************/
STRING_CLASS::~STRING_CLASS()
{
empty();
}

/***********************************************************************
*                                NULL                                  *
***********************************************************************/
void STRING_CLASS::null()
{
TCHAR * cp;

if ( s )
    {
    cp = s;
    cp += maxlen;
    while ( cp > s )
        {
        *cp = NullChar;
        cp--;
        }
    *cp = NullChar;
    }
}

/***********************************************************************
*                              ACQUIRE                                 *
* slen is the max length not counting the NULL character at the end    *
***********************************************************************/
BOOLEAN STRING_CLASS::acquire( TCHAR * sorc, int slen )
{
empty();

if ( sorc )
    {
    maxlen = slen;
    if ( maxlen > 0 )
        {
        s = sorc;
        *(s+maxlen) = NullChar;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                              ACQUIRE                                 *
***********************************************************************/
BOOLEAN STRING_CLASS::acquire( TCHAR * sorc )
{
empty();

if ( sorc )
    {
    maxlen = lstrlen( sorc );
    if ( maxlen > 0 )
        {
        s = sorc;
        return TRUE;
        }
    }

maxlen = 0;
return FALSE;
}

/***********************************************************************
*                              ACQUIRE                                 *
***********************************************************************/
BOOLEAN STRING_CLASS::acquire( STRING_CLASS & sorc )
{
empty();

if ( sorc.s )
    {
    maxlen = sorc.maxlen;
    s = sorc.release();
    return TRUE;
    }

maxlen = 0;
return FALSE;
}

/***********************************************************************
*                              GET_ASCII                               *
*            If this is not unicode this is the same as =              *
***********************************************************************/
BOOLEAN STRING_CLASS::get_ascii( char * sorc )
{
const char AsciiNull = '\0';

union {
char  c;
TCHAR t;
};

TCHAR * cp;
char  * ap;
int n;

/*
----------------------------------
Get the length of the ascii string
---------------------------------- */
n = 0;
ap = sorc;
while ( *ap != AsciiNull )
    {
    ap++;
    n++;
    }

/*
----------------
Convert to TCHAR
---------------- */
if ( n > 0 )
    {
    if ( init(n) )
        {
        cp = s;
        while ( true )
            {
            t = NullChar;
            c = *sorc;
            *cp = t;
            if (*sorc == AsciiNull )
                break;
            cp++;
            sorc++;
            }
        return TRUE;
        }
    }
return FALSE;
}

/***********************************************************************
*                              RELEASE                                 *
* This is used to give my current buffer to someone else, who is       *
* then responsible for deleting it.                                    *
***********************************************************************/
TCHAR * STRING_CLASS::release()
{
TCHAR * p;

p      = s;
s      = 0;
maxlen = 0;

return p;
}

/***********************************************************************
*                                 LEN                                  *
***********************************************************************/
int32 STRING_CLASS::len()
{
if ( s )
    {
    return (int32) lstrlen(s);
    }

return 0;
}

/***********************************************************************
*                              LAST_CHAR                               *
* Return a pointer to the last character of the string. Return 0 if    *
* no characters                                                        *
***********************************************************************/
TCHAR * STRING_CLASS::last_char()
{
int32 n;
n = len();
if ( n )
    {
    n--;
    return (s+n);
    }
return 0;
}

/***********************************************************************
*                            ADD_FINALCHAR                             *
* This assumes cp points to the NullChar at the end of the string.     *
* ep points to the last character of the allocated memory for this     *
* string.                                                              *
***********************************************************************/
void STRING_CLASS::add_finalchar( TCHAR * cp, TCHAR * ep )
{
if ( cp == ep )
    {
    upsize( maxlen+1 );
    cp = s + maxlen;
    }
else
    {
    cp++;
    }
*cp = FinalChar;
}

/***********************************************************************
*                              NEXT_FIELD                              *
***********************************************************************/
bool STRING_CLASS::next_field( TCHAR separator )
{
TCHAR * cp;
TCHAR * dest;
TCHAR * ep;

if ( !s )
    return false;

if ( maxlen < 1 )
    return false;

ep = s + maxlen;

cp = s;
while ( cp < ep )
    {
    if ( (*cp == NullChar) && (*(cp+1) == FinalChar) )
        {
        cp++;
        *cp = NullChar;
        return false;
        }
    cp++;
    }

/*
--------------------------------
Find the first separator or NULL
-------------------------------- */
cp = s;

while ( *cp != NullChar )
    {
    if ( *cp == separator )
        break;
    cp++;
    }

/*
---------------------------------------------------------------------
If I find a separator then this is the first time I have been called.
--------------------------------------------------------------------- */
if ( *cp == separator )
    {
    /*
    -------------------------------
    End the string at the separator
    ------------------------------- */
    *cp = NullChar;

    /*
    -------------------------------------------------------------
    Make sure that everything after the end of the string is null
    ------------------------------------------------------------- */
    *ep = NullChar;

    cp++;

    /*
    -------------------------------------------------------------------------
    If there is nothing after the separator then I need to add a FinalChar so
    when the next call is made I know to return false.
    ------------------------------------------------------------------------- */
    if ( *cp == NullChar )
        {
        add_finalchar( cp, ep );
        return true;
        }

    while ( *cp != NullChar )
        cp++;

    while ( cp < ep )
        {
        *cp = NullChar;
        cp++;
        }
    return true;
    }

/*
-------------------------------------------------------------------------------------
Either this is the second or moreth field or there is only one field and no separator
------------------------------------------------------------------------------------- */
if ( (cp == ep) || (*(cp+1) == NullChar) )
    {
    add_finalchar( cp, ep );
    return true;
    }

cp++;

/*
-------------------------------------------------
Copy the string beginning at cp down to the start
------------------------------------------------- */
dest = s;

while ( cp < ep )
    {
    *dest = *cp;
    dest++;
    cp++;
    }

/*
-----------------------------
Null to the end of the string
----------------------------- */
while ( dest < ep )
    {
    *dest= NullChar;
    dest++;
    }

*ep = NullChar;

/*
--------------------------------
If there is a separator, null it
-------------------------------- */
cp = s;
while ( true )
    {
    if ( (cp == ep) || (*cp == NullChar) )
        {
        add_finalchar( cp, ep );
        return true;
        }

    if ( *cp == separator )
        {
        *cp = NullChar;
        break;
        }

    cp++;
    }

return true;
}

/***********************************************************************
*                              NEXT_FIELD                              *
***********************************************************************/
bool STRING_CLASS::next_field()
{
return next_field( CommaChar );
}

/***********************************************************************
*                             STRING_CLASS                             *
*                               GET_TEXT                               *
*                      Read the text from a control                    *
***********************************************************************/
bool STRING_CLASS::get_text( HWND w )
{
int32 n;

n = GetWindowTextLength( w );

if ( n <= 0 )
    {
    if ( s )
        *s = NullChar;
    return true;
    }

if ( n > maxlen )
    {
    if ( !init(n) )
        return false;
    }

return ::get_text(s, w, n) != 0 ;
}

/***********************************************************************
*                             STRING_CLASS                             *
*                               SET_TEXT                               *
*                       Send the text to a control                     *
***********************************************************************/
bool STRING_CLASS::set_text( HWND w )
{
TCHAR * cp;
if ( s )
    cp = s;
else
    cp = emptystring;

return ::set_text(w, cp) != 0;
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOLEAN STRING_CLASS::init( int32 new_len )
{
empty();

if ( new_len <= 0 )
    return FALSE;

s = new TCHAR[new_len+1];
if ( s )
    {
    maxlen = new_len;
    new_len++;
    new_len *= sizeof(TCHAR);
    memset( s, 0, new_len );
    return TRUE;
    }

maxlen = 0;
return FALSE;
}

/***********************************************************************
*                               BOOL_VALUE                             *
***********************************************************************/
bool STRING_CLASS::bool_value()
{
if ( s )
    {
    if ( *s == TrueString[0] )
        return true;
    if ( *s == OneChar )
        return true;
    }
return false;
}

/***********************************************************************
*                             BOOLEAN_VALUE                            *
***********************************************************************/
BOOLEAN STRING_CLASS::boolean_value()
{
if ( s )
    {
    if ( *s == TrueString[0] )
        return TRUE;
    if ( *s == OneChar )
        return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( bool sorc )
{
if ( sorc )
    operator=( TrueString );
else
    operator=( FalseString );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( BOOLEAN sorc )
{
if ( sorc )
    operator=( TrueString );
else
    operator=( FalseString );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( int n )
{
if ( maxlen < MAX_INTEGER_LEN )
    {
    if ( !init(MAX_INTEGER_LEN) )
        return;
    }

int32toasc( s, (int32) n, DECIMAL_RADIX );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( unsigned int u )
{
if ( maxlen < MAX_INTEGER_LEN )
    {
    if ( !init(MAX_INTEGER_LEN) )
        return;
    }

ultoascii( s, u, DECIMAL_RADIX );
}

/***********************************************************************
*                                    =                                 *
***********************************************************************/
void STRING_CLASS::operator=( double d )
{
if ( maxlen < MAX_DOUBLE_LEN )
    {
    if ( !init(MAX_DOUBLE_LEN) )
        return;
    }

rounded_double_to_tchar( s, d );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( TCHAR sorc )
{
null();
operator+=( sorc );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( const TCHAR * sorc )
{
int32 n;

if ( sorc == s )
    return;

n = 0;
if ( sorc )
    n = lstrlen( sorc );

if ( n <= 0 )
    {
    /*
    -------------------------------------------
    Don't delete the string, just make it len 0
    ------------------------------------------- */
    null();
    return;
    }

if ( n > maxlen )
    {
    if ( !init(n) )
        return;
    }
else
    {
    null();
    }

lstrcpy( s, sorc );
}

/***********************************************************************
*                               OPERATOR=                              *
***********************************************************************/
void STRING_CLASS::operator=( const STRING_CLASS & sorc )
{
operator=( sorc.s );
}

/***********************************************************************
*                               GET_SUBSTRING                          *
***********************************************************************/
BOOLEAN STRING_CLASS::get_substring( const STRING_CLASS & sorc, TCHAR begin_char, TCHAR end_char )
{
TCHAR * cp;
TCHAR * dp;
TCHAR * sp;
int     slen;

if ( !sorc.s )
    return FALSE;

slen = 0;
cp = sorc.s;
while ( true )
    {
    if ( *cp == NullChar )
        return FALSE;
    if ( *cp == begin_char )
        {
        sp = cp;
        slen = 1;
        while ( true )
            {
            slen++;
            cp++;
            if ( *cp == NullChar )
                return FALSE;

            if ( *cp == end_char )
                {
                if ( !upsize(slen) )
                    return FALSE;

                dp = s;

                while ( slen )
                    {
                    *dp++ = *sp++;
                    slen--;
                    }
                *dp = NullChar;
                return TRUE;
                }
            }
        }
    else
        {
        cp++;
        }
    }
}

/***********************************************************************
*                               COMPARE                                *
*         -1 if I am < sorc, 0 if equal, +1 if I am > sorc             *
**********************************************************************/
int STRING_CLASS::compare( TCHAR * sorc )
{
if ( !s && !sorc )
    return 0;

if ( !s )
    return -1;

if ( !sorc )
    return 1;

return (int) ::compare( s, sorc );
}

/***********************************************************************
*                              OPERATOR<                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator<( const TCHAR * sorc )
{

if ( !s )
    return FALSE;

if ( !sorc )
    return FALSE;

if ( lstrcmpi(s, sorc) < 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR<                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator<( const STRING_CLASS & sorc )
{

if ( !s )
    return FALSE;

if ( !sorc.s )
    return FALSE;

if ( lstrcmpi(s, sorc.s) < 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR<                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator>( const TCHAR * sorc )
{

if ( !s )
    return FALSE;

if ( !sorc )
    return FALSE;

if ( lstrcmpi(s, sorc) > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR>                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator>( const STRING_CLASS & sorc )
{

if ( !s )
    return FALSE;

if ( !sorc.s )
    return FALSE;

if ( lstrcmpi(s, sorc.s) > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR>=                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator>=( const STRING_CLASS & sorc )
{

if ( !s )
    return FALSE;

if ( !sorc.s )
    return FALSE;

if ( lstrcmpi(s, sorc.s) >= 0 )
    return TRUE;

return FALSE;
}
/***********************************************************************
*                              OPERATOR<=                               *
***********************************************************************/
BOOLEAN STRING_CLASS::operator<=( const STRING_CLASS & sorc )
{

if ( !s )
    return FALSE;

if ( !sorc.s )
    return FALSE;

if ( lstrcmpi(s, sorc.s) <= 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR==                              *
***********************************************************************/
BOOLEAN STRING_CLASS::operator==( const STRING_CLASS & sorc )
{
if ( !s )
    return FALSE;

if ( !sorc.s )
    return FALSE;

if ( lstrcmpi(s, sorc.s) == 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              OPERATOR==                              *
***********************************************************************/
BOOLEAN STRING_CLASS::operator==( const TCHAR * sorc )
{

if ( !s )
    return FALSE;

if ( !sorc )
    return FALSE;

if ( lstrcmpi(s, sorc) == 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                                 upsize                               *
* This function checks to see if s is big enough. If not, a new        *
* string is allocated and the old one copied to it. This is only       *
* used to make a string bigger. False means the allocation failed.     *
***********************************************************************/
BOOLEAN STRING_CLASS::upsize( int32 new_len )
{
TCHAR * dest;

if ( new_len <= maxlen )
    return TRUE;

new_len++;

dest = new TCHAR[new_len];
if ( !dest )
    return FALSE;

memset( dest, 0, new_len*sizeof(TCHAR) );

new_len--;

if ( s )
    {
    lstrcpy( dest, s );
    empty();
    }

s      = dest;
maxlen = new_len;

return TRUE;
}

/***********************************************************************
*                               cat_w_char                             *
*             Append a string followed with a character                *
***********************************************************************/
void STRING_CLASS::cat_w_char( const TCHAR * sorc, TCHAR c )
{
int32   n;
TCHAR * dest;

if ( !sorc )
    return;

n = len();
n++;
n += lstrlen( sorc );

if ( n < 1 )
    return;

if ( upsize(n) )
    {
    lstrcat( s, sorc );

    dest  = s + n;
    *dest = NullChar;
    dest--;
    *dest = c;
    }
}

/***********************************************************************
*                               cat_path                               *
***********************************************************************/
void STRING_CLASS::cat_path( const TCHAR * sorc )
{
int32   n;
int32   sorc_len;
TCHAR * cp;
bool    need_backslash;

if ( !sorc )
    return;

sorc_len = lstrlen( sorc );
if ( sorc_len < 1 )
    return;

n = len();
if ( n > 0 )
    {
    /*
    -----------------------------------------------------
    Point cp at the last character of the existing string
    ----------------------------------------------------- */
    cp = s;
    cp += n;
    cp--;

    need_backslash = ( (*cp != BackSlashChar) && (*sorc != BackSlashChar) );

    n += sorc_len;
    if ( need_backslash )
        n++;
    upsize( n );
    if ( need_backslash )
        operator+=( BackSlashString );
    }

operator+=( sorc );
}

/***********************************************************************
*                                  +=                                  *
***********************************************************************/
void STRING_CLASS::operator+=( const TCHAR sorc )
{
int32 n;

n = len();
n++;

if ( upsize(n) )
    {
    n--;
    s[n] = sorc;
    n++;
    s[n] = NullChar;
    }
}

/***********************************************************************
*                                  +=                                  *
***********************************************************************/
void STRING_CLASS::operator+=( const TCHAR * sorc )
{
int32   n;

if ( !sorc )
    return;

n  = len();
n += lstrlen( sorc );

if ( n < 1 )
    return;

if ( upsize(n) )
    lstrcat( s, sorc );
}

/***********************************************************************
*                                   +=                                 *
***********************************************************************/
void STRING_CLASS::operator+=( const STRING_CLASS & sorc )
{
operator+=( sorc.s );
}

/***********************************************************************
*                                   +=                                 *
***********************************************************************/
void STRING_CLASS::operator+=( bool sorc )
{
if ( sorc )
    operator+=( TrueString );
else
    operator+=( FalseString );
}

/***********************************************************************
*                                   +=                                 *
***********************************************************************/
void STRING_CLASS::operator+=( BOOLEAN sorc )
{
if ( sorc )
    operator+=( TrueString );
else
    operator+=( FalseString );
}

/***********************************************************************
*                                    +=                                *
***********************************************************************/
void STRING_CLASS::operator+=( int n )
{
int slen;
int newlen;

slen   = len();
newlen = slen;
newlen += MAX_INTEGER_LEN;

if ( newlen > maxlen )
    {
    if ( !upsize(newlen) )
        return;
    }

int32toasc( s+slen, (int32) n, DECIMAL_RADIX );
}

/***********************************************************************
*                                    +=                                *
***********************************************************************/
void STRING_CLASS::operator+=( double d )
{
int slen;
int newlen;

slen   = len();
newlen = slen;
newlen += MAX_DOUBLE_LEN;

if ( newlen > maxlen )
    {
    if ( !upsize(newlen) )
        return;
    }

rounded_double_to_tchar( s+slen, d );
}

/***********************************************************************
*                                    +=                                *
***********************************************************************/
void STRING_CLASS::operator+=( unsigned int u )
{
int slen;
int newlen;

slen   = len();
newlen = slen;
newlen += MAX_INTEGER_LEN;

if ( newlen > maxlen )
    {
    if ( !upsize(newlen) )
        return;
    }
ultoascii( s+slen, u, DECIMAL_RADIX );
}

/***********************************************************************
*                                ISEMPTY                               *
***********************************************************************/
BOOLEAN STRING_CLASS::isempty()
{
if ( !s )
    return TRUE;

if ( *s == NullChar )
    return TRUE;

return is_empty( s );
}

/***********************************************************************
*                              ISNUMERIC                               *
*  Return TRUE if the string begins with a number "  1" " +.0", etc.   *
***********************************************************************/
BOOLEAN STRING_CLASS::isnumeric()
{
const TCHAR startchars[] = TEXT(" +-." );
const TCHAR numerics[]   = TEXT("0123456789");
TCHAR * cp;
TCHAR   c;

if ( isempty() )
    return FALSE;

cp = s;
while ( *cp != NullChar )
    {
    c = *cp;
    if ( findchar(c, numerics) )
        return TRUE;
    if ( !findchar(c, startchars) )
        return FALSE;
    cp++;
    }

return FALSE;
}

/***********************************************************************
*                                 ISZERO                               *
***********************************************************************/
bool STRING_CLASS::iszero()
{
const TCHAR badchars[] = TEXT("123456789");
const TCHAR okchars[]  = TEXT(" +-.");

TCHAR * cp;
bool    found_zero_char = false;

if ( !s )
    return false;

cp = s;
while ( *cp != NullChar )
    {
    if ( *cp == ZeroChar )
        found_zero_char = true;
    else if ( ::findchar(*cp, badchars) )
        return false;
    else if ( !::findchar(*cp, okchars) )
        break;
    cp++;
    }

return found_zero_char;
}

/***********************************************************************
*                              REAL_VALUE                              *
* I use strtod in extdouble so I have to make sure there are no alpha  *
* characters.                                                          *
***********************************************************************/
double STRING_CLASS::real_value()
{
double  d;
int     n;
TCHAR * cp;

d = 0.0;

if ( s )
    {
    n = 0;
    cp = s;
    while ( *cp != NullChar )
        {
        if ( !is_real_char(*cp) )
            break;
        n++;
        cp++;
        }
    if ( n )
        d =  extdouble(s, n);
    }

return d;
}

/***********************************************************************
*                          REPLACE_LEADING_CHARS                       *
* Replace up to n-1 leading characters. "00001" -> "    1",            *
* "00000" -> "    0". Return the number replaced.                      *                           *
***********************************************************************/
int STRING_CLASS::replace_leading_chars( TCHAR bad_char, TCHAR replacement_char )
{
TCHAR * cp;
int     count;
int     max_count;

count = 0;

if ( s )
    {
    max_count = len() - 1;
    cp = s;
    while ( *cp == bad_char )
        {
        *cp = replacement_char;
        count++;
        if ( count == max_count )
            break;
        cp++;
        }
    }

return count;
}

/***********************************************************************
*                          REMOVE_ALL_OCCURANCES                       *
***********************************************************************/
int STRING_CLASS::remove_all_occurances( TCHAR offending_char )
{
int     count;
TCHAR * cp;

count = 0;

if ( s )
    {
    cp = s;
    while ( *cp != NullChar )
        {
        if ( *cp == offending_char )
            {
            remove_char( cp );
            count++;
            }
        else
            {
            cp++;
            }
        }
    }

return count;
}

/***********************************************************************
*                          REMOVE_LEADING_CHARS                        *
*      Shorten the string by removing characters from the left.        *
***********************************************************************/
bool STRING_CLASS::remove_leading_chars( int new_len )
{
int     n;

if ( new_len > 0 && s )
    {
    n = len();

    while ( n > new_len )
        {
        remove_char( s );
        n--;
        }
    return true;
    }

return false;
}

/***********************************************************************
*                           REMOVE_LEADING_CHARS                       *
* Remove up to n-1 leading characters. "00001" -> "1", "00000" -> "0"  *
* Return the number removed.                                           *
***********************************************************************/
int STRING_CLASS::remove_leading_chars( TCHAR bad_char )
{
TCHAR * cp;
int     count;
int     max_count;
if ( s )
    {
    count = 0;
    max_count = len() - 1;
    cp = s;
    while ( *cp == bad_char )
        {
        remove_char( cp );
        count++;
        if ( count == max_count )
            break;
        }
    }

return count;
}

/***********************************************************************
*                                  FIND                                *
***********************************************************************/
TCHAR * STRING_CLASS::find( TCHAR c )
{
TCHAR * cp;

if ( s )
    {
    cp = s;
    while ( *cp != NullChar )
        {
        if ( *cp == c )
            return cp;
        cp++;
        }
    }

return 0;
}

/***********************************************************************
*                                FINDLAST                              *
***********************************************************************/
TCHAR * STRING_CLASS::findlast( TCHAR c )
{
TCHAR * cp;

if ( s )
    {
    if ( *s == NullChar )
        return 0;
    cp = s + len() -1;
    while ( true )
        {
        if ( *cp == c )
            return cp;
        if ( cp == s )
            break;;
        cp--;
        }
    }

return 0;
}

/***********************************************************************
*                           STRING_CLASS                               *
*                               find                                   *
***********************************************************************/
TCHAR * STRING_CLASS::find( const TCHAR * s_to_find )
{
TCHAR * cp;

cp = 0;
if ( s_to_find && s )
    cp = findstring( s_to_find, s );

return cp;
}

/***********************************************************************
*                           STRING_CLASS                               *
*                             findlast                                 *
***********************************************************************/
TCHAR * STRING_CLASS::findlast( const TCHAR * s_to_find )
{
TCHAR * cp;

cp = 0;
if ( s_to_find && s )
    cp = find_last_string( s_to_find, s );

return cp;
}

/***********************************************************************
*                                 CONTAINS                             *
***********************************************************************/
bool STRING_CLASS::contains( TCHAR c )
{
if ( find(c) )
    return true;

return false;
}

/***********************************************************************
*                      STRING_CLASS::START_EQUALS                      *
***********************************************************************/
bool STRING_CLASS::start_equals( TCHAR * sorc )
{
TCHAR * cp;

if ( !s )
    return false;

cp = s;
while ( true )
    {
    if ( *sorc == NullChar )
        return true;

    if ( *sorc != *cp )
        return false;

    sorc++;
    cp++;
    }
}

/***********************************************************************
*                                 UPPERCASE                            *
***********************************************************************/
void STRING_CLASS::uppercase()
{
if ( s )
    upper_case( s );
}

/***********************************************************************
*                                 LOWERCASE                            *
***********************************************************************/
void STRING_CLASS::lowercase()
{
if ( s )
    lower_case( s );
}

/***********************************************************************
*                           STRING_CLASS                               *
*                             contains                                 *
***********************************************************************/
bool STRING_CLASS::contains( const TCHAR * s_to_find )
{
if ( find(s_to_find) )
    return true;

return false;
}

/***********************************************************************
*                               REPLACE                                *
*        Returns TRUE if the text was found and replaced.              *
*        FALSE means the text was not found.                           *
***********************************************************************/
BOOLEAN STRING_CLASS::replace( const TCHAR * oldtext, const TCHAR * newtext )
{
int32 delta;
int32 old_len;
int32 new_len;
int32 slen;
TCHAR * cp;

if ( !s )
    return FALSE;

cp = findstring( oldtext, s );
if ( !cp )
    return FALSE;

old_len = lstrlen( oldtext );
new_len = lstrlen( newtext );

delta = new_len - old_len;
if ( delta > 0 )
    {
    slen = len() + delta;
    if ( slen > maxlen )
        {
        if ( !upsize( slen) )
            return FALSE;
        cp = findstring( oldtext, s );
        if ( !cp )
            return FALSE;
        }
    move_string_forward( cp, delta );
    }
else if ( delta < 0 )
    {
    copystring( cp+new_len, cp+old_len );
    }

copychars( cp, newtext, new_len );
return TRUE;
}

/***********************************************************************
*                               REPLACE                                *
*        Replaces the first instance of oldc with newc.                *
*        Returns TRUE if oldc was found.                               *
***********************************************************************/
BOOLEAN STRING_CLASS::replace( TCHAR oldc, TCHAR newc )
{
TCHAR * cp;

if ( !s )
    return FALSE;

cp = s;
while ( *cp != NullChar )
    {
    if ( *cp == oldc )
        {
        *cp = newc;
        return TRUE;
        }
    cp++;
    }

return FALSE;
}

/***********************************************************************
*                             REPLACE_ALL                              *
*        Replaces all instances of oldc with newc.                     *
*        Returns the number of replacements.                           *
***********************************************************************/
int STRING_CLASS::replace_all( TCHAR oldc, TCHAR newc )
{
TCHAR * cp;
int     n;

if ( !s )
    return 0;

n  = 0;
cp = s;
while ( *cp != NullChar )
    {
    if ( *cp == oldc )
        {
        *cp = newc;
        n++;
        }
    cp++;
    }

return n;
}

/***********************************************************************
*                              STRIP_CRLF                              *
*         Strip any cr or lf chars from the end of the string          *
***********************************************************************/
void STRING_CLASS::strip_crlf()
{
TCHAR * cp;
int     n;

if ( !s )
    return;

n = len();
if ( n < 1 )
    return;

cp = s + n -1;

while ( true )
    {
    if ( (*cp != CrChar) && (*cp != LfChar) )
        break;

    *cp = NullChar;
    if ( cp == s )
        break;

    cp--;
    }
}

/***********************************************************************
*                        NULL_FIRST_CONTROL_CHAR                       *
* Replace the first control char with a null, else null *(s + maxlen). *
***********************************************************************/
void STRING_CLASS::null_first_control_char()
{
TCHAR * cp;
TCHAR * ep;

if ( !s )
    return;

cp = s;
ep = s + maxlen;

while ( cp < ep )
    {
    if ( *cp < SpaceChar )
        break;
    cp++;
    }

*cp = NullChar;
}

/***********************************************************************
*                                 STRIP                                *
*         Strip spaces and control chars from both ends.               *
***********************************************************************/
void STRING_CLASS::strip()
{
TCHAR * cp;
TCHAR * ep;
int     n;

if ( !s )
    return;

n = len();
if ( n < 1 )
    return;

cp = s + n -1;

while ( true )
    {
    if ( (*cp > SpaceChar) )
        break;

    *cp = NullChar;
    if ( cp == s )
        return;

    cp--;
    }

ep = cp;
while ( cp > s )
    {
    cp--;
    if ( *cp > SpaceChar )
        ep = cp;
    }

/*
----------------------------------------
Ep points to the first non-control char.
---------------------------------------- */
if ( cp < ep )
    {
    while ( true )
        {
        *cp = *ep;
        if ( *ep == NullChar )
            break;
        cp++;
        ep++;
        }
    }
}

/***********************************************************************
*                          STRIP_DOUBLE_QUOTES                         *
*             Return TRUE if there are quotes at both ends.            *
***********************************************************************/
BOOLEAN STRING_CLASS::strip_double_quotes()
{
TCHAR * cp;
int     n;

if ( !s )
    return FALSE;

n = len();
if ( n < 1 )
    return FALSE;

/*
------------------------------
Point cp at the last character
------------------------------ */
cp = s + n -1;

/*
-----------------------------
N is now the number of quotes
----------------------------- */
n = 0;

/*
------------------------
Check the last character
------------------------ */
if ( *cp == DoubleQuoteChar )
    {
    *cp = NullChar;
    n++;
    }

/*
-------------------------
Check the first character
------------------------- */
cp = s;
if ( *cp == DoubleQuoteChar )
    {
    remove_char( cp );
    n++;
    }

if ( n == 2 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                                 rjust                                *
*  Right justify the string padding on the left with the passed char.  *
***********************************************************************/
BOOLEAN STRING_CLASS::rjust( int32 new_len, TCHAR c )
{
int32 current_len;
TCHAR * sorc;
TCHAR * dest;

current_len = len();
if ( current_len >= new_len )
    return TRUE;

if ( !upsize(new_len) )
    return FALSE;

/*
-----------------------
Move the string forward
----------------------- */
sorc = s + current_len;
dest = s + new_len;
while ( true )
    {
    *dest = *sorc;
    if ( sorc == s )
        break;
    sorc--;
    dest--;
    }

/*
--------------------------------------
Fill the start with the fill character
-------------------------------------- */
while ( dest > s )
    {
    dest--;
    *dest = c;
    }

return TRUE;
}

/***********************************************************************
*                                 rjust                                *
***********************************************************************/
BOOLEAN STRING_CLASS::rjust( int32 new_len )
{
return rjust( new_len, SpaceChar );
}
