#include <windows.h>

/***********************************************************************
*  Note: The ith entry is the one for the ith parameter.               *
*  There is room for all the parameters but, obviously, only the       *
*  ones that are ftanalog parameters are used for anything.            *
***********************************************************************/
#include "..\include\visiparm.h"
#include "..\include\chars.h"
#include "..\include\nameclas.h"
#include "..\include\ftcalc.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"


/***********************************************************************
*                         FTCALC_CLASS::CLEANUP                        *
***********************************************************************/
void FTCALC_CLASS::cleanup()
{
int i;
for ( i=0; i<MAX_PARMS; i++ )
    cmd[i].empty();
}

/***********************************************************************
*                         FTCALC_CLASS::VALUE                          *
***********************************************************************/
double FTCALC_CLASS::value( int dest_parameter_number, double * parameters )
{
int          i;
TCHAR      * cp;
TCHAR        c;
TCHAR        op_char;
double       total;
double       x;

if ( dest_parameter_number <= 0 || dest_parameter_number > MAX_PARMS )
    return 0.0;

i = dest_parameter_number - 1;
if ( cmd[i].isempty() )
    return 0.0;

cp = cmd[i].text();
total = 0.0;

op_char = PlusChar;

while ( true )
    {
    if ( *cp == NullChar )
        break;

    if ( *cp == PChar || *cp == PCharLow || *cp == CChar || *cp == CCharLow  )
        {
        c = *cp;
        upper_case( c );
        cp++;
        x = 0.0;
        if ( c == PChar )
            {
            i = (int) asctoint32( cp );
            if ( i > 0 && i <= MAX_PARMS )
                {
                i--;
                x = parameters[i];
                }
            }
        else
            {
            x = extdouble( cp );
            }
        if ( op_char  == PlusChar )
            total += x;
        else if ( op_char == MinusChar )
            total -= x;
        else if ( op_char == AsterixChar )
            total *= x;
        else if ( op_char == ForwardSlashChar )
            if ( not_zero(x) )
                total /= x;
        }
    else if ( *cp == PlusChar || *cp == MinusChar || *cp == AsterixChar || *cp == ForwardSlashChar )
        {
        op_char = *cp;
        }
    else
        {
        for ( i=0; i<sp.count(); i++ )
            {
            if ( sp[i].desc.isempty() )
                continue;
            if ( start_chars_are_equal(sp[i].desc.text(),cp) )
                {
                x = sp[i].value.real_value();
                cp += (sp[i].desc.len() - 1);
                if ( op_char  == PlusChar )
                    total += x;
                else if ( op_char == MinusChar )
                    total -= x;
                else if ( op_char == AsterixChar )
                    total *= x;
                else if ( op_char == ForwardSlashChar )
                    if ( not_zero(x) )
                        total /= x;
                break;
                }
            }
        }
    cp++;
    }

return total;
}

/***********************************************************************
*                           FTCALC_CLASS::LOAD                         *
***********************************************************************/
BOOLEAN FTCALC_CLASS::load( STRING_CLASS & computer_to_find, STRING_CLASS & machine_to_find, STRING_CLASS & part_to_find )
{
int          i;
FILE_CLASS   f;
NAME_CLASS   s;

cleanup();
s.get_ftcalc_csvname( computer_to_find, machine_to_find, part_to_find );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
        {
        while ( true )
            {
            s = f.readline();
            if ( s.isempty() )
                break;
            if ( s.countchars(CommaChar) == 1 )
                {
                s.next_field();
                i = s.int_value();
                if ( i > 0 && i<= MAX_PARMS )
                    {
                    i--;
                    s.next_field();
                    cmd[i] = s;
                    }
                }
            }
        f.close();
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                          FTCALC_CLASS::SAVE                        *
***********************************************************************/
BOOLEAN FTCALC_CLASS::save( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
int          i;
FILE_CLASS   f;
NAME_CLASS   s;

s.get_ftcalc_csvname( computer, machine, part );
if ( f.open_for_write(s) )
    {
    for ( i=0; i<MAX_PARMS; i++ )
        {
        if ( cmd[i].isempty() )
            continue;

        s = i+1;
        s += CommaChar;
        s += cmd[i];
        f.writeline( s );
        }
    f.close();
    return TRUE;
    }

return FALSE;
}
