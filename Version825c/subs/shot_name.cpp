#include <windows.h>
#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\setpoint.h"
#include "..\include\iniclass.h"
#include "..\include\plookup.h"
#include "..\include\shotname.h"
#include "..\include\static_text_parameter.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

const TCHAR * PATTERN_INDEX      = TEXT( "Pattern"      );
const TCHAR * FIELD_LENGTH_INDEX = TEXT( "Field Length" );

static TCHAR * Description[] = {
    { TEXT( "Field Length" ) },
    { TEXT( "Pattern" ) },
    { TEXT( "Path" ) }
    };

static TCHAR * DefaultValue[] = {
    { TEXT( "6" ) },
    { DEFAULT_SHOT_NAME_PATTERN },
    { TEXT( "" ) }
    };

static TCHAR * ExternalShotNameDescription[] = {
    { TEXT( "Enable" ) },
    { TEXT( "Command" ) },
    { TEXT( "File Name" ) }
    };

static TCHAR * ExternalShotNameDefaultValue[] = {
    { TEXT( "N" ) },
    { TEXT( "" ) },
    { TEXT( "" ) }
    };

static TCHAR AChar             = TEXT( 'A' );
static TCHAR AsterixChar       = TEXT( '*' );
static TCHAR LeftCurlyBracket  = TEXT( '{' );
static TCHAR RightCurlyBracket = TEXT( '}' );
static TCHAR NullChar          = TEXT( '\0' );
static TCHAR PChar             = TEXT( 'P' );
static TCHAR PCharLow          = TEXT( 'p' );
static TCHAR SChar             = TEXT( 'S' );
static TCHAR SCharLow          = TEXT( 's' );
static TCHAR SpaceChar         = TEXT( ' ' );
static TCHAR ZChar             = TEXT( 'Z' );
static TCHAR ZeroChar          = TEXT( '0' );
static const TCHAR CheckDigitTag[]     = TEXT( "{check digit}" );
static const TCHAR HondaYearTag[]      = TEXT( "{yh}" );
static const TCHAR YearTag[]           = TEXT( "{yy}" );
static const TCHAR ShiftYearTag[]      = TEXT( "{ys}" );
static const TCHAR ShiftHondaYearTag[] = TEXT( "{yhs}" );
static const TCHAR HexMonthTag[]       = TEXT( "{mx}" );
static const TCHAR MonthTag[]          = TEXT( "{mm}" );
static const TCHAR ShiftHexMonthTag[]  = TEXT( "{mxs}" );
static const TCHAR ShiftMonthTag[]     = TEXT( "{ms}" );
static const TCHAR DayTag[]            = TEXT( "{dd}" );
static const TCHAR ShiftDayTag[]       = TEXT( "{ds}" );
static const TCHAR ShiftTag[]          = TEXT( "{shift}" );
static const TCHAR JulianTag[]         = TEXT( "{j}" );
static const TCHAR ShiftJulianTag[]    = TEXT( "{js}" );
static const TCHAR HoursTag[]          = TEXT( "{hh}" );

/***********************************************************************
*                          APPEND_CHECK_DIGIT                          *
***********************************************************************/
static BOOLEAN append_check_digit( STRING_CLASS & shot_name )
{
union {
unsigned u;
TCHAR    c;
};

TCHAR  * cp;
unsigned sum;

u   = 0;
sum = 0;

cp = shot_name.text();
if ( !cp )
    return FALSE;

while ( *cp != NullChar )
    {
    c = *cp;
    sum += u;
    cp++;
    }
u = sum % unsigned(26);
u += 65;
shot_name += c;

return TRUE;
}

/***********************************************************************
*                               HONDA_YEAR                             *
***********************************************************************/
static TCHAR honda_year( TIME_CLASS & t )
{
int   y;
TCHAR c;
c = ZeroChar;
y = t.system_time().wYear;
if ( y > 2009 && y < 2032 )
    {
    y -= 2010;
    c = AChar;
    c += ( TCHAR ) y;
    }
return c;
}

/***********************************************************************
*                         SHOT_NAME_PARTS_CLASS                        *
***********************************************************************/
SHOT_NAME_PARTS_CLASS::SHOT_NAME_PARTS_CLASS()
{
shot_number        = NO_SHOT_NUMBER;
shot_number_length = 0;
check_digit        = NullChar;
}

/***********************************************************************
*                        ~SHOT_NAME_PARTS_CLASS                        *
***********************************************************************/
SHOT_NAME_PARTS_CLASS::~SHOT_NAME_PARTS_CLASS()
{
}

/***********************************************************************
*                                clear                                 *
***********************************************************************/
void SHOT_NAME_PARTS_CLASS::clear()
{
name.empty();
s.empty();
shot_number = NO_SHOT_NUMBER;
shot_number_length = 0;
check_digit = NullChar;
}

/***********************************************************************
*                                  =                                   *
***********************************************************************/
void SHOT_NAME_PARTS_CLASS::operator=( SHOT_NAME_PARTS_CLASS & sorc )
{
name = sorc.name;
s    = sorc.s;
shot_number        = sorc.shot_number;
shot_number_length = sorc.shot_number_length;
check_digit        = sorc.check_digit;
}

/***********************************************************************
*                                 set                                  *
***********************************************************************/
void SHOT_NAME_PARTS_CLASS::set( STRING_CLASS & sorc, int new_shot_number_length )
{
TCHAR * cp;
int     shot_number;

clear();

if ( sorc.len() < 1 )
    return;

name               = sorc;
name.remove_leading_chars( SpaceChar );

s                  = sorc;
shot_number_length = new_shot_number_length;

cp = s.last_char();
if ( cp )
    {
    if ( (*cp>=AChar) && (*cp<=ZChar) )
        {
        check_digit = *cp;
        *cp = NullChar;
        }
    else
        {
        cp++;
        }
    }

if ( s.len() < shot_number_length )
    {
    shot_number = s.int_value();
    s.null();
    }

cp -= shot_number_length;
shot_number = (int) extlong( cp, (short) shot_number_length );
*cp = NullChar;
}

/***********************************************************************
*                                  ==                                  *
***********************************************************************/
BOOLEAN SHOT_NAME_PARTS_CLASS::operator==( SHOT_NAME_PARTS_CLASS & sorc )
{
if ( s == sorc.s )
    {
    if ( shot_number == sorc.shot_number )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                                   <                                  *
***********************************************************************/
BOOLEAN SHOT_NAME_PARTS_CLASS::operator<( SHOT_NAME_PARTS_CLASS & sorc )
{
BOOLEAN my_shot_number_is_less;

if ( shot_number < sorc.shot_number )
    my_shot_number_is_less = TRUE;
else
    my_shot_number_is_less = FALSE;

/*
-------------------------------
See if I am a numeric only shot
------------------------------- */
if ( s.isempty() )
    {
    /*
    --------------------------------------------------
    If I am numeric and the sorc is not then I am less
    -------------------------------------------------- */
    if ( !sorc.s.isempty() )
        return TRUE;
    /*
    -----------------------------
    Return the numeric comparison
    ----------------------------- */
    return my_shot_number_is_less;
    }

/*
--------------------------------------------------
I am alpha and the sorc is numeric so I am greater
-------------------------------------------------- */
if ( sorc.s.isempty() )
    return FALSE;

if ( s == sorc.s )
    return my_shot_number_is_less;

return (s<sorc.s);
}

/***********************************************************************
*                                   <=                                 *
***********************************************************************/
BOOLEAN SHOT_NAME_PARTS_CLASS::operator<=( SHOT_NAME_PARTS_CLASS & sorc )
{
if ( operator<(sorc) )
    return TRUE;

return operator==(sorc);
}

/***********************************************************************
*                                   >                                  *
***********************************************************************/
BOOLEAN SHOT_NAME_PARTS_CLASS::operator>( SHOT_NAME_PARTS_CLASS & sorc )
{
BOOLEAN my_shot_number_is_greater;

if ( shot_number > sorc.shot_number )
    my_shot_number_is_greater = TRUE;
else
    my_shot_number_is_greater = FALSE;

/*
-------------------------------
See if I am a numeric only shot
------------------------------- */
if ( s.isempty() )
    {
    /*
    --------------------------------------------------
    If I am numeric and the sorc is not then I am less
    -------------------------------------------------- */
    if ( !sorc.s.isempty() )
        return FALSE;

    /*
    -----------------------------
    Return the numeric comparison
    ----------------------------- */
    return my_shot_number_is_greater;
    }

/*
--------------------------------------------------
I am alpha and the sorc is numeric so I am greater
-------------------------------------------------- */
if ( sorc.s.isempty() )
    return TRUE;

if ( s == sorc.s )
    return my_shot_number_is_greater;

return (s>sorc.s);
}

/***********************************************************************
*                                   >=                                 *
***********************************************************************/
BOOLEAN SHOT_NAME_PARTS_CLASS::operator>=( SHOT_NAME_PARTS_CLASS & sorc )
{
if ( operator>(sorc) )
    return TRUE;

return operator==(sorc);
}

/***********************************************************************
*                              SHOT_NAME                               *
*    The destination string should initially contain the pattern.      *
***********************************************************************/
BOOLEAN shot_name( STRING_CLASS & dest, PART_NAME_ENTRY & pn, int shot_number, TIME_CLASS & t )
{
TCHAR        * cp;
int            parameter_number;
int            slen;
STRING_CLASS   s;
STRING_CLASS   tag;
unsigned long  u;
TIME_CLASS     mytime;
TIME_CLASS     shiftime;
short          shift_number;
SYSTEMTIME     st;
BOOLEAN        is_time;
BOOLEAN        is_shift_time;
STATIC_TEXT_PARAMETER_CLASS stp;

/*
--------------------------------------------------------------
The dest string should arrive containing the pattern. If it is
empty there is nothing for me to do.
-------------------------------------------------------------- */
if ( dest.isempty() )
    return FALSE;

/*
-------------------------------------
Get the shift time in case I need it.
------------------------------------- */
get_shift_number_and_date( &shift_number, &st, t.system_time() );
shiftime.set( st );

/*
-----------------------------------------------------------
Tags are contained within Curly Brackets. They must balance
----------------------------------------------------------- */
slen = dest.countchars( LeftCurlyBracket );
if ( slen != (int) dest.countchars(RightCurlyBracket) )
    return FALSE;

while ( true )
    {
    if ( !tag.get_substring(dest, LeftCurlyBracket, RightCurlyBracket) )
        break;

    /*
    --------------------------------
    Skip past the left curly bracket
    -------------------------------- */
    cp = tag.text();
    cp++;

    /*
    --------------------------------
    See if this is a shot number tag
    -------------------------------- */
    if ( *cp == SChar || *cp == SCharLow )
        {
        cp++;
        slen = 0;
        if ( is_numeric(*cp) )
            slen = asctoint32(cp);
        s = shot_number;
        if ( slen > 0 )
            {
            if ( s.len() > slen )
                s.remove_leading_chars( slen );
            else
                s.rjust( slen, ZeroChar );
            }
        dest.replace( tag.text(), s.text() );
        continue;
        }

    /*
    ------------------------------
    Insert a Static Text Parameter
    ------------------------------ */
    if ( *cp == PChar || *cp == PCharLow )
        {
        cp++;
        slen = 0;
        if ( is_numeric(*cp) )
            parameter_number = asctoint32(cp);
        stp.init( pn.computer, pn.machine, pn.part );
        if ( stp.get(parameter_number) )
            {
            s = stp.value;
            dest.replace( tag.text(), s.text() );
            continue;
            }
        else
            {
            s.null();
            dest.replace( tag.text(), TEXT("None") );
            continue;
            }
        cp--;
        continue;
        }

    /*
    ----------------
    Day of the Month
    ---------------- */
    is_time       = ( tag == DayTag );
    is_shift_time = ( tag == ShiftDayTag );
    if ( is_time || is_shift_time )
        {
        if ( is_shift_time )
            mytime = shiftime;
        else
            mytime = t;
        dest.replace( tag.text(), mytime.dd() );
        continue;
        }

    /*
    -----------------------------
    Hour of the Day
    Hour is not affected by shift
    ----------------------------- */
    is_time = ( tag == HoursTag );
    if ( tag == HoursTag )
        {
        dest.replace( tag.text(), t.hh() );
        continue;
        }

    /*
    ---------------
    Day of the Year
      Julian Date
    --------------- */
    is_time       = ( tag == JulianTag );
    is_shift_time = ( tag == ShiftJulianTag );
    if ( is_time || is_shift_time )
        {
        if ( is_shift_time )
            mytime = shiftime;
        else
            mytime = t;
        st = mytime.system_time();
        dest.replace( tag.text(), mytime.jjj() );
        continue;
        }

    /*
    ---------
    Hex Month
    --------- */
    is_time       = ( tag == HexMonthTag );
    is_shift_time = ( tag == ShiftHexMonthTag );
    if ( is_time || is_shift_time )
        {
        if ( is_shift_time )
            mytime = shiftime;
        else
            mytime = t;
        u = mytime.system_time().wMonth;
        dest.replace( tag.text(), ultohex(u) );
        continue;
        }

    /*
    -----
    Month
    ----- */
    is_time = ( tag == MonthTag );
    is_shift_time = ( tag == ShiftMonthTag );
    if ( is_time || is_shift_time )
        {
        if ( is_shift_time )
            mytime = shiftime;
        else
            mytime = t;
        dest.replace( tag.text(), mytime.mm() );
        continue;
        }

    /*
    ----------
    Honda Year
    ---------- */
    is_time = ( tag == HondaYearTag );
    is_shift_time = ( tag == ShiftHondaYearTag );

    if ( is_time || is_shift_time )
        {
        if ( is_shift_time )
            mytime = shiftime;
        else
            mytime = t;
        s = honda_year( mytime );
        dest.replace( tag.text(), s.text() );
        continue;
        }

    /*
    ----
    Year
    ---- */
    is_time = ( tag == YearTag );
    is_shift_time = ( tag == ShiftYearTag );

    if ( is_time || is_shift_time )
        {
        if ( is_shift_time )
            mytime = shiftime;
        else
            mytime = t;
        dest.replace( tag.text(), mytime.yy() );
        continue;
        }

    /*
    ------------------
    Shift Number [1,9]
    ------------------ */
    if ( tag == ShiftTag )
        {
        s = (int) shift_number;
        dest.replace( tag.text(), s.text() );
        continue;
        }

    /*
    --------------------------------------------------------------------------
    See if this is a check digit tag. The check digit is the last tag so I can
    null the tag and append a check digit.
    -------------------------------------------------------------------------- */
    if ( tag == CheckDigitTag )
        {
        cp = dest.find( LeftCurlyBracket );
        if ( cp )
            {
            *cp = NullChar;
            append_check_digit( dest );
            return TRUE;
            }
        }

    /*
    -------------------------------------------------------------
    If I get here the tag is invalid. Replace it with the string.
    ------------------------------------------------------------- */
    s = tag;
    s.replace( LeftCurlyBracket, AsterixChar );
    s.replace( RightCurlyBracket, AsterixChar );
    dest.replace( tag.text(), s.text() );
    }

return TRUE;
}

/***********************************************************************
*                              SHOT_NAME                               *
*    The destination string should initially contain the pattern.      *
***********************************************************************/
BOOLEAN shot_name( STRING_CLASS & dest, PART_NAME_ENTRY & pn, int shot_number )
{
TIME_CLASS t;

t.get_local_time();
return shot_name( dest, pn, shot_number, t );
}

/***********************************************************************
*                              SHOT_NAME                               *
***********************************************************************/
BOOLEAN shot_name( STRING_CLASS & dest, STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa, int shot_number )
{
NAME_CLASS     s;
PART_NAME_ENTRY pn;
SHOT_NAME_CLASS sn;

sn.init( co, ma, pa );
sn.get();
sn.get_shot_name_template( dest );

pn.set( co, ma, pa );
return shot_name( dest, pn, shot_number );
}

/***********************************************************************
*                           SHOT_NAME_LENGTH                           *
***********************************************************************/
int shot_name_length( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
SHOT_NAME_CLASS sn;

sn.init( co, ma, pa );
sn.get();
return sn.shot_name_length();
}

/***********************************************************************
*                           SHOT_NAME_LENGTH                           *
***********************************************************************/
int shot_name_length( PART_NAME_ENTRY & pn )
{
return shot_name_length( pn.computer, pn.machine, pn.part );
}

/***********************************************************************
*                           SHOT_NAME_LENGTH                           *
***********************************************************************/
int shot_name_length( PROFILE_NAME_ENTRY & pn )
{
return shot_name_length( pn.computer, pn.machine, pn.part );
}

/***********************************************************************
*                      SHOT_NUMBER_FROM_SHOT_NAME                      *
***********************************************************************/
int shot_number_from_shot_name( STRING_CLASS & name, int shot_number_length )
{
int     n;
TCHAR * cp;
bool    have_check_char;

if ( name.isempty() )
    return 0;

cp = name.last_char();

/*
------------------------------------------------------------------
At the present time (v7.14) the only thing that can come after the
shot number is a check char [A,Z].
------------------------------------------------------------------ */
n = shot_number_length;
have_check_char = false;
if ( (*cp>=AChar) && (*cp<=ZChar) )
    {
    have_check_char = true;
    n++;
    cp--;
    }

if ( name.len() < n )
    n = name.len();

if ( have_check_char )
    n--;

if ( n <= 0 )
    return 0;
/*
---------------------------------------------------------------------------
Decrement n one more time because I'm sitting on the last character and not
on the null character.
--------------------------------------------------------------------------- */
n--;
cp -= n;
return (int) extlong( cp, (short) shot_number_length );
}

/***********************************************************************
*                      SHOT_COUNT_BETWEEN_SHOTS                        *
*   Assume that the shot number is at the end of the string.           *
* This only works for names that are equal except for the shot number  *
***********************************************************************/
int shot_count_between_shots( STRING_CLASS & low_shot_name, STRING_CLASS & high_shot_name, int shot_number_length )
{
SHOT_NAME_PARTS_CLASS highshot;
SHOT_NAME_PARTS_CLASS lowshot;

lowshot.set( low_shot_name, shot_number_length );
highshot.set( high_shot_name, shot_number_length );

return highshot.shot_number - lowshot.shot_number;
}

/***********************************************************************
*                             SHOT_NAME_CLASS                          *
***********************************************************************/
SHOT_NAME_CLASS::SHOT_NAME_CLASS()
{
}

/***********************************************************************
*                             ~SHOT_NAME_CLASS                         *
***********************************************************************/
SHOT_NAME_CLASS::~SHOT_NAME_CLASS()
{
}

/***********************************************************************
*                             SHOT_NAME_CLASS                          *
*                                  INIT                                *
***********************************************************************/
BOOLEAN SHOT_NAME_CLASS::init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
return s.get_shot_name_csvname( computer, machine, part );
}

/***********************************************************************
*                            SHOT_NAME_CLASS                           *
*                                EXISTS                                *
***********************************************************************/
BOOLEAN SHOT_NAME_CLASS::exists()
{
return s.file_exists();
}

/***********************************************************************
*                             SET_DEFAULTS                             *
***********************************************************************/
void SHOT_NAME_CLASS::set_defaults()
{
int i;
int mycount;

mycount = SETPOINT_CLASS::count();
if ( mycount < MIN_SHOT_NAME_SETPOINT_COUNT )
    setsize( MIN_SHOT_NAME_SETPOINT_COUNT );

for ( i=0; i<mycount; i++ )
    {
    if ( operator[](i).desc.isempty() )
        operator[](i).desc = Description[i];
    if ( operator[](i).value.isempty() )
        operator[](i).value = DefaultValue[i];
    }

for ( i=mycount; i<MIN_SHOT_NAME_SETPOINT_COUNT; i++ )
    {
    operator[](i).desc = Description[i];
    operator[](i).value = DefaultValue[i];
    }

}

/***********************************************************************
*                            SHOT_NAME_CLASS                           *
*                                  GET                                 *
***********************************************************************/
int SHOT_NAME_CLASS::get()
{
int mycount;

mycount = SETPOINT_CLASS::get( s );
if ( mycount < MIN_SHOT_NAME_SETPOINT_COUNT )
    set_defaults();

return count();
}

/***********************************************************************
*                            SHOT_NAME_CLASS                           *
*                                  PUT                                 *
***********************************************************************/
BOOLEAN SHOT_NAME_CLASS::put()
{
if (SETPOINT_CLASS::count() < MIN_SHOT_NAME_SETPOINT_COUNT )
    set_defaults();

return SETPOINT_CLASS::put( s );
}

/***********************************************************************
*                         SET_SHOT_NAME_TEMPLATE                       *
***********************************************************************/
BOOLEAN SHOT_NAME_CLASS::set_shot_name_template( STRING_CLASS & new_template )
{
if ( count() > SHOT_NAME_PATTERN_INDEX )
    {
    operator[]( SHOT_NAME_PATTERN_INDEX ).value = new_template;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                         GET_SHOT_NAME_TEMPLATE                       *
***********************************************************************/
BOOLEAN SHOT_NAME_CLASS::get_shot_name_template( STRING_CLASS & dest )
{
if ( count() > SHOT_NAME_PATTERN_INDEX )
    {
    dest = operator[]( SHOT_NAME_PATTERN_INDEX ).value;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                            SHOT_NUMBER_LENGTH                        *
* The shot number is at the end of the template or just before the     *
* check digit. I search back for a {sx}, where x is a digit. I return  *
* the value of that digit.                                             *
***********************************************************************/
int SHOT_NAME_CLASS::shot_number_length()
{
static TCHAR pattern[] = TEXT( "{s" );

int     n;
TCHAR   c;
TCHAR * cp;

n = 0;

if ( count() > SHOT_NAME_PATTERN_INDEX )
    {
    cp = operator[]( SHOT_NAME_PATTERN_INDEX ).value.findlast( pattern );
    if ( cp )
        {
        c = *(cp+2);
        if ( is_numeric(c) )
            {
            n = tchar_to_int( c );
            n -= tchar_to_int( ZeroChar );
            }
        }
    }
return n;
}

/***********************************************************************
*                          SET_SHOT_NAME_LENGTH                        *
***********************************************************************/
BOOLEAN SHOT_NAME_CLASS::set_shot_name_length( STRING_CLASS & new_template )
{
if ( count() > SHOT_NAME_LENGTH_INDEX )
    {
    operator[]( SHOT_NAME_LENGTH_INDEX ).value = new_template;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                          GET_SHOT_NAME_LENGTH                        *
***********************************************************************/
BOOLEAN SHOT_NAME_CLASS::get_shot_name_length( STRING_CLASS & dest )
{
if ( count() > SHOT_NAME_LENGTH_INDEX )
    {
    dest = operator[]( SHOT_NAME_LENGTH_INDEX ).value;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                            SHOT_NAME_LENGTH                          *
***********************************************************************/
int SHOT_NAME_CLASS::shot_name_length()
{
if ( count() > SHOT_NAME_LENGTH_INDEX )
    return operator[]( SHOT_NAME_LENGTH_INDEX ).value.int_value();

return 0;
}

/***********************************************************************
*                        EXTERNAL_SHOT_NAME_CLASS                      *
***********************************************************************/
EXTERNAL_SHOT_NAME_CLASS::EXTERNAL_SHOT_NAME_CLASS()
{
}

/***********************************************************************
*                         ~EXTERNAL_SHOT_NAME_CLASS                    *
***********************************************************************/
EXTERNAL_SHOT_NAME_CLASS::~EXTERNAL_SHOT_NAME_CLASS()
{
}

/***********************************************************************
*                         EXTERNAL_SHOT_NAME_CLASS                     *
*                                  INIT                                *
***********************************************************************/
BOOLEAN EXTERNAL_SHOT_NAME_CLASS::init( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
return fn.get_external_shot_name_file_csvname( computer, machine, part );
}

/***********************************************************************
*                             SET_DEFAULTS                             *
***********************************************************************/
void EXTERNAL_SHOT_NAME_CLASS::set_defaults()
{
int i;
int mycount;

mycount = SETPOINT_CLASS::count();
if ( mycount < EXTERNAL_SHOT_NAME_FILE_SETPOINT_COUNT )
    setsize( EXTERNAL_SHOT_NAME_FILE_SETPOINT_COUNT );

for ( i=0; i<mycount; i++ )
    {
    if ( operator[](i).desc.isempty() )
        operator[](i).desc = ExternalShotNameDescription[i];
    if ( operator[](i).value.isempty() )
        operator[](i).value = ExternalShotNameDefaultValue[i];
    }

for ( i=mycount; i<EXTERNAL_SHOT_NAME_FILE_SETPOINT_COUNT; i++ )
    {
    operator[](i).desc = ExternalShotNameDescription[i];
    operator[](i).value = ExternalShotNameDefaultValue[i];
    }

}

/***********************************************************************
*                        EXTERNAL_SHOT_NAME_CLASS                      *
*                                  GET                                 *
************************************************************************/
int EXTERNAL_SHOT_NAME_CLASS::get()
{
static TCHAR ConfigSection[]              = TEXT( "Config" );
static TCHAR GettingShotNameFromFileKey[] = TEXT( "GettingShotNameFromFile" );
static TCHAR GetShotNameCommandKey[]      = TEXT( "GetShotNameCommand" );
static TCHAR MonallIniFile[]              = TEXT( "monall.ini" );
static TCHAR ShotNameFileKey[]            = TEXT( "ShotNameFile" );

int        mycount;
INI_CLASS  ini;
NAME_CLASS s;

/*
-----------------------------------------------------------
I will now read all the settings from the file if it exists
----------------------------------------------------------- */
mycount = SETPOINT_CLASS::get( fn );
if ( mycount >= EXTERNAL_SHOT_NAME_FILE_SETPOINT_COUNT )
    {
    s = operator[](EXTERNAL_SHOT_NAME_FILE_NAME_INDEX).value;
    if ( s == NOT_USED || s == UNKNOWN )
        operator[](EXTERNAL_SHOT_NAME_FILE_NAME_INDEX).value.null();

    s = operator[](EXTERNAL_SHOT_NAME_FILE_COMMAND_INDEX).value;
    if ( s == NOT_USED || s == UNKNOWN )
        operator[](EXTERNAL_SHOT_NAME_FILE_COMMAND_INDEX).value.null();

    return mycount;
    }

set_defaults();

/*
------------------------------------------------------------------
If there was no file I should get the enable state from monall.ini
------------------------------------------------------------------ */
if ( mycount == 0 )
    {
    s.get_local_ini_file_name( MonallIniFile );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );

    if ( ini.find(GettingShotNameFromFileKey) )
        operator[](EXTERNAL_SHOT_NAME_FILE_ENABLED_INDEX).value = ini.get_boolean();

    /*
    ---------
    File name
    --------- */
    s.null();
    if ( ini.find(ShotNameFileKey) )
        {
        s = ini.get_string();
        if ( s == NOT_USED || s == UNKNOWN )
            s.null();
        }
    operator[](EXTERNAL_SHOT_NAME_FILE_NAME_INDEX).value = s;

    /*
    -----------
    Run command
    ----------- */
    s.null();
    if ( ini.find(GetShotNameCommandKey) )
        {
        s = ini.get_string();
        if ( s == NOT_USED || s == UNKNOWN )
            s.null();
        }

    operator[](EXTERNAL_SHOT_NAME_FILE_COMMAND_INDEX).value = s;
    }

return count();
}

/***********************************************************************
*                        EXTERNAL_SHOT_NAME_CLASS                      *
*                                  GET                                 *
************************************************************************/
int EXTERNAL_SHOT_NAME_CLASS::get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
if ( fn.get_external_shot_name_file_csvname(computer, machine, part) )
    return get();

return 0;
}

/***********************************************************************
*                        EXTERNAL_SHOT_NAME_CLASS                      *
*                                  PUT                                 *
***********************************************************************/
BOOLEAN EXTERNAL_SHOT_NAME_CLASS::put()
{
if (SETPOINT_CLASS::count() < EXTERNAL_SHOT_NAME_FILE_SETPOINT_COUNT )
    set_defaults();

return SETPOINT_CLASS::put( fn );
}

/***********************************************************************
*                                 SET_ENABLE                           *
***********************************************************************/
BOOLEAN EXTERNAL_SHOT_NAME_CLASS::set_enable( BOOLEAN is_enabled )
{
if ( count() > EXTERNAL_SHOT_NAME_FILE_ENABLED_INDEX )
    {
    operator[]( EXTERNAL_SHOT_NAME_FILE_ENABLED_INDEX ).value = is_enabled;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                               IS_ENABLED                             *
***********************************************************************/
BOOLEAN EXTERNAL_SHOT_NAME_CLASS::is_enabled()
{
if ( count() > SHOT_NAME_PATTERN_INDEX )
    return operator[]( EXTERNAL_SHOT_NAME_FILE_ENABLED_INDEX ).value.boolean_value();

return FALSE;
}

/***********************************************************************
*                             HAVE_COMMAND                             *
***********************************************************************/
BOOLEAN EXTERNAL_SHOT_NAME_CLASS::have_command()
{
STRING_CLASS s;

s = operator[](EXTERNAL_SHOT_NAME_FILE_COMMAND_INDEX).value;

if ( s.isempty() || s == NOT_USED || s == UNKNOWN )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                                 COMMAND                              *
*             Return the command to get the shot name file.            *
***********************************************************************/
STRING_CLASS & EXTERNAL_SHOT_NAME_CLASS::command()
{
return operator[](EXTERNAL_SHOT_NAME_FILE_COMMAND_INDEX).value;
}

/***********************************************************************
*                                  FILE                                *
*           Return the name of the external shot name file             *
***********************************************************************/
STRING_CLASS & EXTERNAL_SHOT_NAME_CLASS::file()
{
return operator[](EXTERNAL_SHOT_NAME_FILE_NAME_INDEX).value;
}

