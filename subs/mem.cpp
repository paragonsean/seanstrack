#include <windows.h>

#include "..\include\visiparm.h"

static TCHAR SpaceChar = TEXT( ' ' );
static TCHAR NineChar  = TEXT( '9' );
static TCHAR NullChar  = TEXT( '\0' );
static TCHAR TabChar   = TEXT( '\t' );
static TCHAR TildaChar = TEXT( '~' );
static TCHAR ZeroChar  = TEXT( '0' );

const static char CNullChar = '\0';

/***********************************************************************
*                             UPPER_CASE                               *
***********************************************************************/
void upper_case( TCHAR & c )
{
union { unsigned u; TCHAR ch; };
u = 0;
ch = c;
c = (TCHAR) CharUpper( LPTSTR(u) );
}

/***********************************************************************
*                              UPPER_CASE                              *
***********************************************************************/
void upper_case( TCHAR * s )
{
union { unsigned u; TCHAR ch; };
u = 0;

while ( *s )
    {
    ch = *s;
    *s = (TCHAR) CharUpper( LPTSTR(u) );
    s++;
    }
}

/***********************************************************************
*                              LOWER_CASE                              *
***********************************************************************/
void lower_case( TCHAR & c )
{
union { unsigned u; TCHAR ch; };

u = 0;
ch = c;

c = (TCHAR) CharLower( LPTSTR(u) );
}

/***********************************************************************
*                             LOWER_CASE                               *
***********************************************************************/
void lower_case( TCHAR * s )
{
union { unsigned u; TCHAR ch; };
u = 0;

while ( *s )
    {
    ch = *s;
    *s = (TCHAR) CharLower( LPTSTR(u) );
    s++;
    }
}

/***********************************************************************
*                         UNICODE_TO_CHAR                              *
***********************************************************************/
char unicode_to_char( TCHAR sorc )
{
union {
char  c;
TCHAR t;
};

t = sorc;
return c;
}

/***********************************************************************
*                           TCHAR_TO_INT                               *
***********************************************************************/
int tchar_to_int( TCHAR sorc_tchar )
{
union { unsigned u; TCHAR c; };
u = 0;

c = sorc_tchar;
return int( u );
}

/***********************************************************************
*                             UL_TO_CHAR                               *
***********************************************************************/
char ul_to_char( unsigned long sorc )
{
union {
char  c;
unsigned long u;
};

u = sorc;
return c;
}

/***********************************************************************
*                         UNICODE_TO_CHAR                              *
***********************************************************************/
void unicode_to_char( char * dest, TCHAR * sorc )
{
union {
char  c;
TCHAR t;
};

while ( TRUE )
    {
    t = *sorc;
    *dest = c;
    if ( *sorc == NullChar )
        break;
    dest++;
    sorc++;
    }

*dest = CNullChar;
}

/***********************************************************************
*                           CHAR_TO_UNICODE                            *
***********************************************************************/
void char_to_unicode( wchar_t * dest, TCHAR * sorc )
{
union {
char  c;
TCHAR t;
};

#ifdef UNICODE
while ( true )
    {
    *dest = *sorc;
    if ( *sorc == NullChar )
        break;
    dest++;
    sorc++;
    }
return;
#endif

t = 0;
while ( TRUE )
    {
    c = *sorc;
    *dest = t;
    if ( *sorc == NullChar )
        break;
    dest++;
    sorc++;
    }
}

/***********************************************************************
*                               DIGIT_TO_UL                            *
***********************************************************************/
unsigned long digit_to_ul( TCHAR sorc )
{
union {
TCHAR         c;
unsigned long u;
};
u = 0;
if ( sorc >= ZeroChar && sorc<=NineChar )
    c = sorc - ZeroChar;
return u;
}

/***********************************************************************
*                              CHAR_TO_TCHAR                           *
***********************************************************************/
TCHAR char_to_tchar( char sorc )
{
union {
char  c;
TCHAR t;
};
t = NullChar;
c = sorc;
return t;
}

/***********************************************************************
*                            CHAR_TO_TCHAR                             *
***********************************************************************/
void char_to_tchar( TCHAR * dest, char * sorc, int max_len )
{
const char AsciiNull = '\0';

union {
char  c;
TCHAR t;
};

if ( !dest )
    return;

t = NullChar;

if ( sorc )
    {
    while ( true )
        {
        c = *sorc;
        *dest = t;
        if ( *sorc == AsciiNull )
            break;
        dest++;
        sorc++;

        max_len--;
        if ( !max_len )
            {
            *dest = NullChar;
            break;
            }
        }
    }
}

/***********************************************************************
*                              CHAR_TO_TCHAR                           *
*            This assumes you have allocated the dest buffer.          *
***********************************************************************/
void char_to_tchar( TCHAR * dest, char * sorc )
{
const char AsciiNull = '\0';

union {
char  c;
TCHAR t;
};

if ( !dest || !sorc )
    return;

t = NullChar;

/*
----------------
Convert to TCHAR
---------------- */
while ( true )
    {
    c = *sorc;
    *dest = t;
    if (*sorc == AsciiNull )
        break;
    dest++;
    sorc++;
    }
}

/***********************************************************************
*                               CHAR_TO_TCHAR                          *
*  This will allocate a dest buffer. You are responsible for deleting  *
*  the returned dest buffer.                                           *
***********************************************************************/
TCHAR * char_to_tchar( char * sorc )
{
const char AsciiNull = '\0';
char  * ap;
TCHAR * dest;
int     n;

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

if ( n == 0 )
    return 0;

dest = new TCHAR[n+1];
if ( !dest )
    return dest;

char_to_tchar( dest, sorc );

return dest;
}

/***********************************************************************
*                              FILLCHARS                               *
***********************************************************************/
void fillchars( TCHAR * dest, TCHAR c, short n )
{
while ( n )
    {
    *dest = c;
    dest++;
    n--;
    }
}

/***********************************************************************
*                              COUNTCHARS                              *
***********************************************************************/
int32 countchars( TCHAR c, const TCHAR * sorc )
{
int32 count;

count = 0;

while ( *sorc )
    {
    if ( *sorc == c )
        count++;
    sorc++;
    }

return count;
}

/***********************************************************************
*                               FINDCHAR                               *
***********************************************************************/
const TCHAR * findchar( TCHAR c, const TCHAR * sorc, short maxlen )
{

while ( maxlen )
    {
    if ( *sorc == c )
        return sorc;

    if ( *sorc == TEXT('\0') )
        break;

    sorc++;
    maxlen--;
    }

return 0;
}

/***********************************************************************
*                               FINDCHAR                               *
***********************************************************************/
TCHAR * findchar( TCHAR c, TCHAR * sorc, short maxlen )
{

while ( maxlen )
    {
    if ( *sorc == c )
        return sorc;

    if ( *sorc == TEXT('\0') )
        break;

    sorc++;
    maxlen--;
    }

return 0;
}

/***********************************************************************
*                           FIND_LAST_CHAR                             *
***********************************************************************/
TCHAR * find_last_char( TCHAR c, TCHAR * sorc )
{
TCHAR * cp;

cp = 0;

if ( sorc )
    {
    while ( *sorc != NullChar )
        {
        if ( *sorc == c )
            cp = sorc;
        sorc++;
        }
    }

return cp;
}

/***********************************************************************
*                               FINDSTRING                             *
*          Return pointer to first occurance of s_to_find in s.        *
***********************************************************************/
TCHAR * findstring( const TCHAR * s_to_find, TCHAR * s )
{
const TCHAR * fs;
TCHAR * cp;
TCHAR sorc_char;
TCHAR dest_char;

if ( !s )
    return 0;

if ( !s_to_find )
    return 0;

while ( (*s) != NullChar )
    {
    fs = s_to_find;
    cp = s;
    while ( TRUE )
        {
        if ( *fs == NullChar )
            return s;
        sorc_char = *cp;
        upper_case( sorc_char );
        dest_char = *fs;
        upper_case( dest_char );
        if ( sorc_char != dest_char )
            break;

        cp++;
        fs++;
        }
    s++;
    }

return 0;
}

/***********************************************************************
*                            FIND_LAST_STRING                          *
*          Return pointer to last occurance of s_to_find in s.         *
***********************************************************************/
TCHAR * find_last_string( const TCHAR * s_to_find, TCHAR * s )
{
const TCHAR * fs;
TCHAR * cp;
TCHAR * sp;
TCHAR sorc_char;
TCHAR dest_char;
int   flen;
int   slen;

if ( !s )
    return 0;

if ( !s_to_find )
    return 0;

flen = lstrlen( s_to_find );
slen = lstrlen( s );

if ( flen > slen )
    return 0;

sp = s + slen - flen;
while ( true )
    {
    fs = s_to_find;
    cp = sp;
    while ( true )
        {
        if ( *fs == NullChar )
            return sp;
        sorc_char = *cp;
        upper_case( sorc_char );
        dest_char = *fs;
        upper_case( dest_char );
        if ( sorc_char != dest_char )
            break;
        cp++;
        fs++;
        }
    if ( sp == s )
        break;
    sp--;
    }

return 0;
}

/***********************************************************************
*                        START_CHARS_ARE_EQUAL                         *
***********************************************************************/
bool start_chars_are_equal( const TCHAR * sorc, const TCHAR * s )
{
while ( true )
    {
    if ( *sorc == NullChar )
        return true;

    if ( *sorc != *s )
        return false;

    sorc++;
    s++;
    }
}

/***********************************************************************
*                               FINDCHAR                               *
***********************************************************************/
TCHAR * findchar( TCHAR c, TCHAR * sorc )
{
while ( *sorc )
   {
   if ( c == *sorc )
       return sorc;
   sorc++;
   }
return 0;
}

/***********************************************************************
*                               FINDCHAR                               *
***********************************************************************/
const TCHAR * findchar( TCHAR c, const TCHAR * sorc )
{
while ( *sorc )
   {
   if ( c == *sorc )
       return sorc;
   sorc++;
   }
return 0;
}

/***********************************************************************
*                              COPYCHARS                               *
***********************************************************************/
void copychars( LPTSTR dest, const TCHAR * sorc, short n )
{

while ( n > 0 )
    {
    *dest = *sorc;
    dest++;
    sorc++;
    n--;
    }

}

/***********************************************************************
*                               CHARLEN                                *
*  This returns the length of a string but is limited by max_len.      *
***********************************************************************/
short charlen( const TCHAR * sorc, short max_len )
{

short n;

n = 0;

while ( TRUE )
    {
    if ( n == max_len )
        break;

    if ( *sorc == NullChar )
        break;

    sorc++;
    n++;
    }

return n;
}

/***********************************************************************
*                             COPYFROMSTR                              *
*                                                                      *
*  Copy n chars from a string. Pad with spaces if not long enough.     *
***********************************************************************/
void copyfromstr( LPTSTR dest, const TCHAR * sorc, short n )
{

short slen;

slen = charlen( sorc, n );

copychars( dest, sorc, slen );

if ( slen < n )
    {
    dest += slen;
    slen = n - slen;
    fillchars( dest, TEXT(' '), slen );
    }

}

/***********************************************************************
*                               COMPARE                                *
*        This looks for an exact (case insensitive) match.             *
***********************************************************************/
short compare( const TCHAR * s1, const TCHAR * s2 )
{
TCHAR c1;
TCHAR c2;

while ( TRUE )
    {
    c1 = *s1;
    c2 = *s2;
    upper_case( c1 );
    upper_case( c2 );

    if ( c1 < c2 )
        return -1;

    if ( c1 > c2 )
        return  1;

    if ( c2 == NullChar )
        break;

    s1++;
    s2++;
    }

return 0;
}

/***********************************************************************
*                               COMPARE                                *
***********************************************************************/
short compare( const TCHAR * s1, const TCHAR * s2, short max_len )
{
TCHAR c1;
TCHAR c2;

while ( max_len )
    {
    c1 = *s1;
    c2 = *s2;
    upper_case( c1 );
    upper_case( c2 );

    if ( c1 < c2 )
        return -1;

    if ( c1 > c2 )
        return  1;

    if ( c2 == NullChar )
        break;

    s1++;
    s2++;
    max_len--;
    }

return 0;
}

/***********************************************************************
*                            COMPARE_TEXT                              *
*      This can be used to sort a GENERIC_LIST_CLASS of strings.       *
*                         -1 => sorc1 <  sorc2                         *
*                          0 => sorc1 == sorc2                         *
*                          1 => sorc1 >  sorc2                         *
***********************************************************************/
int compare_text( const void * sorc1, const void * sorc2 )
{
int           result;
const TCHAR * s1;
const TCHAR * s2;

if ( !sorc1 )
    return -1;

if ( !sorc2 )
    return 1;

s1 = (const TCHAR *) sorc1;
s2 = (const TCHAR *) sorc2;

result = CompareString( LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, s1, -1, s2, -1 );

if ( result == CSTR_LESS_THAN )
    return -1;

else if ( result == CSTR_GREATER_THAN )
    return 1;

return 0;
}

/***********************************************************************
*                           STRINGS_ARE_EQUAL                          *
***********************************************************************/
BOOLEAN strings_are_equal( const TCHAR * s1, const TCHAR * s2, short max_len )
{
if ( compare(s1, s2, max_len) == 0 )
    return TRUE;
return FALSE;
}

/***********************************************************************
*                           STRINGS_ARE_EQUAL                          *
***********************************************************************/
BOOLEAN strings_are_equal( const TCHAR * s1, const TCHAR * s2 )
{
short len1;
short len2;

len1 = lstrlen( s1 );
len2 = lstrlen( s2 );

if ( len2 > len1 )
    len1 = len2;

return strings_are_equal( s1, s2, len1 );
}

/***********************************************************************
*                              COPYCHARSTOTAB                          *
***********************************************************************/
TCHAR * copyCharsToTab( LPTSTR dest, const TCHAR * sorc, short max_len )
{

while ( (max_len > 0) && (*sorc != TAB_CHAR) )
    {
    *dest = *sorc;
    dest++;
    sorc++;
    max_len--;
    }

*dest = NullChar;

while ( *sorc == TAB_CHAR )
    sorc++;

return ((TCHAR *)sorc);
}


/***********************************************************************
*                             COPYSTRING                               *
*                                                                      *
*          Copy sorc to dest. Return pointer to NullChar;              *
***********************************************************************/
TCHAR * copystring( TCHAR * dest, const TCHAR * sorc )
{
if ( !sorc )
    {
    if ( dest )
        *dest = NullChar;
    }
else if ( dest )
    {
    while ( TRUE )
        {
        *dest = *sorc;
        if ( !(*sorc) )
            break;
        dest++;
        sorc++;
        }
    }

return dest;
}

/***********************************************************************
*                              CATSTRING                               *
***********************************************************************/
TCHAR * catstring( TCHAR * dest, const TCHAR * sorc)
{
if ( dest )
    {
    while ( *dest )
        dest++;
    }

return copystring( dest, sorc );
}

/***********************************************************************
*                             COPYSTRING                               *
*                                                                      *
*          Copy sorc to dest. Return pointer to NullChar;              *
***********************************************************************/
TCHAR * copystring( TCHAR * dest, const TCHAR * sorc, int maxlen )
{
if ( !sorc )
    {
    if ( dest )
        *dest = NullChar;
    }
else if ( dest )
    {
    while ( true )
        {
        if ( maxlen <= 0 )
            {
            *dest = NullChar;
            break;
            }
        *dest = *sorc;
        if ( !(*sorc) )
            break;
        dest++;
        sorc++;
        maxlen--;
        }
    }

return dest;
}

/***********************************************************************
*                              NEXTSTRING                              *
***********************************************************************/
TCHAR * nextstring( TCHAR * s )
{
s += lstrlen(s);
s++;
return s;
}

/***********************************************************************
*                        REPLACE_CHAR_WITH_NULL                        *
* This replaces the first occurance of any chars in cset with a NULL   *
***********************************************************************/
BOOLEAN replace_char_with_null( TCHAR * sorc, const TCHAR * cset )
{
int32 slen;
int32 i;
const TCHAR * cp;

slen = lstrlen( cset );

while ( *sorc )
    {
    cp = cset;
    for ( i=0; i<slen; i++ )
        {
        if ( *sorc == *cp )
            {
            *sorc = NullChar;
            return TRUE;
            }
        cp++;
        }
    sorc++;
    }

return FALSE;
}

/***********************************************************************
*                        REPLACE_CHAR_WITH_NULL                        *
***********************************************************************/
BOOLEAN replace_char_with_null( TCHAR * cp, TCHAR c )
{
cp = findchar( c, cp, lstrlen(cp) );

if ( cp )
    {
    *cp = NullChar;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        REPLACE_TAB_WITH_NULL                         *
***********************************************************************/
BOOLEAN replace_tab_with_null( TCHAR * cp )
{
return replace_char_with_null( cp, TabChar );
}

/***********************************************************************
*                             COPY_W_CHAR                              *
***********************************************************************/
TCHAR * copy_w_char( TCHAR * dest, TCHAR * sorc, TCHAR c )
{
dest  = copystring( dest, sorc );
*dest = c;
dest++;

return dest;
}

/***********************************************************************
*                               KILLTEXT                               *
***********************************************************************/
void killtext( TCHAR * s )
{
if ( !s )
    return;

delete[] s;
s = 0;
}

/***********************************************************************
*                               MAKETEXT                               *
***********************************************************************/
TCHAR * maketext( int32 n )
{
TCHAR * s;

s = new TCHAR[n+1];
if ( s )
    *s = NullChar;

return s;
}

/***********************************************************************
*                               MAKETEXT                               *
***********************************************************************/
TCHAR * maketext( const TCHAR * sorc, int32 chars_to_add )
{
int32 n;
TCHAR * s;

s = 0;
if ( sorc )
    {
    n = lstrlen(sorc) + chars_to_add;
    s = maketext( n );
    if ( s )
        lstrcpy( s, sorc );
    }

return s;
}

/***********************************************************************
*                               MAKETEXT                               *
***********************************************************************/
TCHAR * maketext( const TCHAR * sorc )
{
return maketext(sorc, 0);
}

/***********************************************************************
*                          MAKETEXT_FROM_CHAR                          *
*                        Convert char to TCHAR                         *
***********************************************************************/
TCHAR * maketext_from_char( char * sorc, int len )
{
TCHAR * buf;
TCHAR * cp;

if ( len <=0 )
    return 0;

if ( !sorc )
    return 0;

buf = maketext( len );
if ( buf )
    {
    cp = buf;
    while ( len )
        {
        *cp = (TCHAR) *sorc;
        cp++;
        sorc++;
        len--;
        }
    *cp = NullChar;
    }

return buf;
}

/***********************************************************************
*                          MAKETEXT_FROM_CHAR                          *
*                        Convert char to TCHAR                         *
***********************************************************************/
TCHAR * maketext_from_char( char * sorc )
{
int len;
char * cp;

if ( !sorc )
    return 0;

cp = sorc;
len = 0;
while ( *cp != CNullChar )
    {
    cp++;
    len++;
    }

if ( len )
    return maketext_from_char( sorc, len );

return 0;
}

/***********************************************************************
*                             INSERT_CHAR                              *
***********************************************************************/
void insert_char( TCHAR * sorc, TCHAR c )
{
TCHAR * cp;

cp = sorc;
while ( *cp )
    cp++;

while ( true )
    {
    *(cp+1) = *(cp);
    if ( cp == sorc )
        break;
    cp--;
    }

*cp = c;
}

/***********************************************************************
*                         MOVE_STRING_FORWARD                          *
* This moves the string forward. It doesn't modify any characters in   *
* the gap created.                                                     *
***********************************************************************/
void move_string_forward( TCHAR * s, int n )
{
TCHAR * sorc;
TCHAR * dest;
int slen;

if ( !s )
    return;
if ( n <= 0 )
    return;

slen = lstrlen( s );

/*
--------------------------------
Point sorc at the null character
-------------------------------- */
sorc = s + slen;

/*
-----------------------------------------------
Point dest n characters past the null character
----------------------------------------------- */
dest = sorc + n;

slen++;
while ( slen )
    {
    *dest-- = *sorc--;
    slen--;
    }
}

/***********************************************************************
*                             REMOVE_CHAR                              *
***********************************************************************/
void remove_char( TCHAR * sorc )
{
TCHAR * dest;

dest = sorc;
while ( TRUE )
    {
    sorc++;
    *dest = *sorc;
    if ( *sorc == NullChar )
        break;
    dest++;
    }
}

/***********************************************************************
*                         REMOVE_ALL_OCCURANCES                        *
***********************************************************************/
void remove_all_occurances( TCHAR * sorc, TCHAR offending_char )
{
while ( *sorc != NullChar )
    {
    if ( *sorc == offending_char )
        remove_char( sorc );
    else
        sorc++;
    }
}

/***********************************************************************
*                                 TRIM                                 *
***********************************************************************/
void trim( TCHAR * s )
{
TCHAR * cp;

cp = s + lstrlen(s);
while ( cp > s )
    {
    cp--;
    if ( *cp != SpaceChar )
        break;
    *cp = NullChar;
    }
}

/***********************************************************************
*                                STRIP                                 *
*  Strip leading and trailing spaces from a Null terminated string     *
***********************************************************************/
void strip( TCHAR * s )
{
TCHAR * cp;

cp = s;
while ( *cp == SpaceChar )
    remove_char( s );

if ( *cp != NullChar )
    trim( cp );
}

/***********************************************************************
*                              MATCHCASE                               *
*      Convert case of chars in place based on passed pattern.         *
***********************************************************************/
void  matchcase( TCHAR * dest, const TCHAR * sorc )
{
union { unsigned u; TCHAR ch; };
u = 0;

while ( true )
    {
    if ( *sorc == NullChar || *dest == NullChar )
        break;

    ch = *sorc;
    if ( IsCharUpper(u) )
        upper_case( *dest );
    else
        lower_case( *dest );

    sorc++;
    dest++;
    }
}

/***********************************************************************
*                                 ITOU                                 *
***********************************************************************/
int itou( int sorc )
{
union
    {
    int          iu;
    unsigned int u;
    };

iu = sorc;
return u;
}

/***********************************************************************
*                                 UTOI                                 *
***********************************************************************/
int utoi( unsigned int sorc )
{
union
    {
    int          iu;
    unsigned int u;
    };
u = sorc;
return iu;
}