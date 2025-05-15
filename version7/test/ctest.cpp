#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\dbclass.h"

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

class bozo
{
int x[10];
int  & operator[]( int i );
int zip();
};

int & bozo::operator[]( int i )
{
return x[i];
}

int bozo::zip()
{
int i;
int sum;
sum = operator[](i);
return sum;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
/**************************
TCHAR  * cp;
while ( TRUE )
    {
    if ( lstrlen(cp) < 1 )
        break;
    }
***************************/
//TCHAR * cp;
//TCHAR s[100];
DB_TABLE t;
STRING_CLASS s;

    int    number;  /* Sensor number */
    int    units;   /* Units index */
    double adder;
    double subtrahend;
    double multiplier;
    double divisor;
    int vartype;
    TCHAR desc[ASENSOR_DESC_LEN+1];
    double high;
    double low;

s = "c:\\sources\\version7\\test";
if ( t.open("c:\\sources\\version7\\test\\asensor.txt", ASENSOR_RECLEN, PFL ) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        number  = t.get_long( ASENSOR_NUMBER_OFFSET );
        units   = t.get_long( ASENSOR_UNITS_OFFSET );
        vartype = t.get_long( ASENSOR_VARIABLE_TYPE_OFFSET );
        t.get_alpha( desc, ASENSOR_DESC_OFFSET, ASENSOR_DESC_LEN );
        high       = t.get_double(ASENSOR_HIGH_VALUE_OFFSET);
        low        = t.get_double(ASENSOR_LOW_VALUE_OFFSET);
        multiplier = t.get_double( ASENSOR_MUL_OFFSET );
        subtrahend = t.get_double( ASENSOR_SUB_OFFSET );
        divisor    = t.get_double( ASENSOR_DIV_OFFSET );
        adder      = t.get_double( ASENSOR_ADD_OFFSET );

        }

    t.close();
    }

//cprintf( "\n\r size = %d.\n\r", sizeof(&cp) );

/*        cprintf( "\n\r[ %s ] [%d]\n\r", inet_ntoa(address), (int) host->h_length ); */

cprintf( "\n\rDone, press <Enter> to exit..." );
getline();
cprintf( "\n\r" );

return 0;
}
