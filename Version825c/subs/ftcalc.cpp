#include <windows.h>

/***********************************************************************
*  Note: The ith entry is the one for the ith parameter.               *
*  There is room for all the parameters but, obviously, only the       *
*  ones that are ftanalog parameters are used for anything.            *
***********************************************************************/
#include "..\include\visiparm.h"
#include "..\include\chars.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\part.h"
#include "..\include\ftcalc.h"
#include "..\include\subs.h"

/***********************************************************************
*                       SOURCE_DATA::SOURCE_DATA                       *
***********************************************************************/
SOURCE_DATA::SOURCE_DATA()
{
position = 0;
velocity = 0;
time     = 0;
is_set   = false;
}


/***********************************************************************
*                       ~SOURCE_DATA::SOURCE_DATA                      *
***********************************************************************/
SOURCE_DATA::~SOURCE_DATA()
{
}

/***********************************************************************
*                          SOURCE_DATA::CLEAR                          *
***********************************************************************/
void SOURCE_DATA::clear()
{
position = 0;
velocity = 0;
time     = 0;
is_set   = false;
}

/***********************************************************************
*                        SOURCE_DATA::OPERATOR=                        *
***********************************************************************/
void SOURCE_DATA::operator=( const SOURCE_DATA & sorc )
{
position = sorc.position;
velocity = sorc.velocity;
time     = sorc.time;
is_set   = sorc.is_set;
}

/***********************************************************************
*                           SOURCE_DATA::SET                           *
***********************************************************************/
void SOURCE_DATA::set( double new_position, double new_time, double new_velocity )
{
position = new_position;
time     = new_time;
velocity = new_velocity;
is_set   = true;
}

/***********************************************************************
*                         FTCALC_CLASS::FTCALC_CLASS                   *
***********************************************************************/
FTCALC_CLASS::FTCALC_CLASS()
{
}

/***********************************************************************
*                    FTCALC_CLASS::CLEAR_SOURCE_DATA                   *
***********************************************************************/
void FTCALC_CLASS::clear_source_data()
{
int i;

for ( i=0; i<MAX_PARMS; i++ )
    {
    sourcedata[i].position = 0.0;
    sourcedata[i].velocity = 0.0;
    sourcedata[i].time     = 0.0;
    sourcedata[i].is_set   = false;
    }
}

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
double FTCALC_CLASS::value( int dest_parameter_number, double * parameters, PART_CLASS & part )
{
int i;

if ( dest_parameter_number <= 0 || dest_parameter_number > MAX_PARMS )
    return 0.0;

i = dest_parameter_number - 1;
if ( cmd[i].isempty() )
    return 0.0;

return value( cmd[i], parameters, part );
}

/***********************************************************************
*                         FTCALC_CLASS::VALUE                          *
***********************************************************************/
double FTCALC_CLASS::value( STRING_CLASS & command, double * parameters, PART_CLASS & part )
{
int i;
TCHAR * cp;
TCHAR c;
TCHAR c1;
TCHAR op_char;
double total;
double x;

if ( command.isnumeric() )
    return command.real_value();

cp = command.text();

/*
----------------------------------------------------------------------------
If the first character is not an alpha character then this is just a number.
---------------------------------------------------------------------------- */
c = *cp;

total   = 0.0;
op_char = PlusChar;

while ( true )
    {
    if ( *cp == NullChar )
        break;

    c = *cp;
    upper_case( c );
    if ( c == PChar || c == CChar || c == MChar  || c == TChar || c == VChar )
        {
        cp++;
        c1 = *cp;
        upper_case( c1 );
        x = 0.0;
        if ( c == PChar )
            {
            if ( c1 == AChar )
                {
                cp++;
                x = part.plunger_diameter;
                x *= x;
                x *= 3.14159;
                x /= 4.0;
                }
            else if ( c1 == DChar )
                {
                cp++;
                x = part.plunger_diameter;
                }
            else if ( c1 == OChar )
                {
                 /*
                ------------------------
                This is a POSxxx request
                ------------------------ */
                cp += 2;
                x = 0.0;
                i = (int) asctoint32( cp );
                if ( i > 0 && i <= MAX_PARMS )
                    {
                    i--;
                    x = sourcedata[i].position;
                    }
                }
            else
                {
                i = (int) asctoint32( cp );
                if ( i > 0 && i <= MAX_PARMS )
                    {
                    i--;
                    x = parameters[i];
                    }
                /*
                ------------------------------------------
                I know this is an integer so I can go past
                the end of the digits and then back up one
                ------------------------------------------ */
                while ( is_numeric(*cp) )
                    cp++;
                cp--;
                }
            }
        else if ( c == MChar )
            {
            /*
            ---
            MSL
            --- */
            cp += 2;
            x = part.min_stroke_length;
            }
        else if ( c == TChar )
            {
            if ( c1 == IChar )
                {
                 /*
                ------------------------
                This is a TIMxxx request
                ------------------------ */
                cp += 2;
                x = 0.0;
                i = (int) asctoint32( cp );
                if ( i > 0 && i <= MAX_PARMS )
                    {
                    i--;
                    x = sourcedata[i].time;
                    }
                }
            else
                {
                /*
                ---
                TSL
                --- */
                c += 2;
                x = part.total_stroke_length;
                }
            }
        else if ( c == VChar )
            {
             /*
            ------------------------
            This is a VELxxx request
            ------------------------ */
            cp += 2;
            x = 0.0;
            i = (int) asctoint32( cp );
            if ( i > 0 && i <= MAX_PARMS )
                {
                i--;
                x = sourcedata[i].velocity;
                }
            }
        else
            {
            /*
            -----------------------
            Must be 'C', a constant
            ----------------------- */
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
*                         FTCALC_CLASS::REMOVE                         *
***********************************************************************/
BOOLEAN FTCALC_CLASS::remove( int index_to_remove )
{
int i;
int n;

n = MAX_PARMS;
n--;
if ( index_to_remove < 0 || index_to_remove > n )
    return FALSE;

i = index_to_remove;
while ( i < n )
    {
    cmd[i]        = cmd[i+1];
    sourcedata[i] = sourcedata[i+1];
    i++;
    }
cmd[n].empty();
sourcedata[n].clear();

return TRUE;
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
