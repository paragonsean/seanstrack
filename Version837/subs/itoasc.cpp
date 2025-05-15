#include <windows.h>
#ifdef __BORLANDC__
#include <systypes.h>
#endif

#define MAX_INTEGER_LEN 18

#include "../include/visiparm.h"
#include "../include/subs.h"

static const TCHAR DecimalPointChar = TEXT( '.' );
static const TCHAR NullChar         = TEXT( '\0' );
static const TCHAR MinusChar        = TEXT( '-' );
static const TCHAR ZeroChar         = TEXT( '0' );

/***********************************************************************
*                               INT32TOASC                             *
***********************************************************************/
void int32toasc( LPTSTR dest, int32 n, int32 radix )
{
LPTSTR cp;
int32 sign;
int32 i;

sign = n;
if ( n < 0 )
    n = -n;

cp = dest;
do  {
    i = n % radix;
    if ( i <= 9 )
        *cp = TCHAR(i+'0');
    else
        *cp = TCHAR(i+'7');
    cp++;
    n /= radix;
    } while ( n > 0 );

if ( sign < 0 )
    *cp++ = MinusChar;

*cp = NullChar;

reverse(dest);
}

/***********************************************************************
*                               INT32TOASC                             *
***********************************************************************/
TCHAR * int32toasc( int32 n )
{
static TCHAR s[MAX_INTEGER_LEN+1];

int32toasc( s, n, DECIMAL_RADIX );

return s;
}

/***********************************************************************
*                             INT_TO_ASCII                             *
*  decimal_places = 2 => 1000 should be 10.00                          *
***********************************************************************/
void int_to_ascii( TCHAR * dest, int n, int decimal_places )
{
int sign;
int i;
TCHAR * cp;

if ( n == 0 && decimal_places > 0 )
    {
    *dest = ZeroChar;
    dest++;
    *dest = DecimalPointChar;
    dest++;
    while ( decimal_places > 0 )
        {
        *dest = ZeroChar;
        dest++;
        decimal_places--;
        }
    *dest = NullChar;
    return;
    }

sign = n;
if ( sign < 0 )
    n = -n;

cp = dest;
do  {
    i = n % 10;
    *cp = ZeroChar + i;
    cp++;
    if ( decimal_places > 0 )
        {
        decimal_places--;
        if ( decimal_places == 0 )
            {
            *cp = DecimalPointChar;
            cp++;
            }
        }
    n /= 10;
    } while ( n > 0 );

if ( sign < 0 )
    {
    *cp = MinusChar;
    cp++;
    }
*cp = NullChar;

reverse( dest );
}

/***********************************************************************
*                             INT_TO_ASCII                             *
***********************************************************************/
void int_to_ascii( TCHAR * dest, int n )
{
int sign;
int i;
TCHAR * cp;

sign = n;
if ( sign < 0 )
    n = -n;

cp = dest;
do  {
    i = n % 10;
    *cp = ZeroChar + i;
    cp++;
    n /= 10;
    } while ( n > 0 );

if ( sign < 0 )
    {
    *cp = MinusChar;
    cp++;
    }
*cp = NullChar;

reverse( dest );
}

/***********************************************************************
*                               ULTOASCII                              *
***********************************************************************/
void ultoascii( TCHAR * dest, unsigned long n, unsigned long radix )
{
TCHAR  * cp;
unsigned long i;

cp = dest;
do  {
    i = n % radix;
    if ( i <= 9 )
        *cp = TCHAR(i+'0');
    else
        *cp = TCHAR(i+'7');
    cp++;
    n /= radix;
    } while ( n > 0 );

*cp = NullChar;

reverse( dest );
}

/***********************************************************************
*                               ULTOASCII                              *
***********************************************************************/
TCHAR * ultoascii( unsigned long n, unsigned long radix )
{
static TCHAR s[MAX_INTEGER_LEN+1];
ultoascii( s, n, radix );
return s;
}

/***********************************************************************
*                               ULTOASCII                              *
***********************************************************************/
TCHAR * ultoascii( unsigned long n )
{
return ultoascii( n, (unsigned long) DECIMAL_RADIX );
}
