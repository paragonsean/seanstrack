#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\machine.h"
#include "..\include\multipart_runlist.h"
#include "..\include\multistring.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\wire_class.h"

const static TCHAR CommaChar = TEXT( ',' );
const static TCHAR NullChar  = TEXT( '\0' );
const static TCHAR SpaceChar = TEXT( ' ' );

static TCHAR ConfigSection[]             = TEXT( "Config" );
static TCHAR EnabledString[]             = TEXT( "Enabled" );
static TCHAR InputWiresString[]          = TEXT( "Input Wires" );
static TCHAR PartSelectionNumberString[] = TEXT( "Part Selection Number" );
static TCHAR VisiTrakIniFile[]           = TEXT( "visitrak.ini" );
static TCHAR ZeroString[]                = TEXT( "0" );

static const unsigned UZero = 0;

/***********************************************************************
*                          BITS_FROM_WIRELIST                          *
***********************************************************************/
unsigned bits_from_wirelist( STRING_CLASS & wirelist )
{
TCHAR      * cp;
int          i;
STRING_CLASS s;
unsigned     u;
WIRE_CLASS   w;

u = UZero;
if ( !wirelist.isempty() )
    {
    s = wirelist;
    cp = s.find( CommaChar );
    if ( cp )
        *cp = NullChar;

    while ( true )
        {
        i = s.int_value();
        if ( i>0 && i<=32 )
            u |= w.wirebit( i );
        if ( !s.next_field() )
            break;
        }
    }

return u;
}

/***********************************************************************
*                   MULTIPART_RUNLIST_SETTINGS_CLASS                   *
***********************************************************************/
MULTIPART_RUNLIST_SETTINGS_CLASS::MULTIPART_RUNLIST_SETTINGS_CLASS()
{
init();
}

/***********************************************************************
*                   ~MULTIPART_RUNLIST_SETTINGS_CLASS                  *
***********************************************************************/
MULTIPART_RUNLIST_SETTINGS_CLASS::~MULTIPART_RUNLIST_SETTINGS_CLASS()
{
}

/***********************************************************************
*                    MULTIPART_RUNLIST_SETTINGS_CLASS                  *
*                                    =                                 *
***********************************************************************/
void MULTIPART_RUNLIST_SETTINGS_CLASS::operator=( const MULTIPART_RUNLIST_SETTINGS_CLASS & sorc )
{
enable              = sorc.enable;
mask                = sorc.mask;
wires               = sorc.wires;
}

/***********************************************************************
*                         MULTIPART_RUNLIST_SETINGS                    *
*                                  init                                *
***********************************************************************/
void MULTIPART_RUNLIST_SETTINGS_CLASS::init()
{
enable              = false;
mask                = UZero;
}

/***********************************************************************
*                         MULTIPART_RUNLIST_SETINGS                    *
*                                   put                                *
*   Save the multipart_runlist_settings                                *
*   Enabled,Y                                                          *
*   Input Wires,1,2,3                                                  *
***********************************************************************/
bool MULTIPART_RUNLIST_SETTINGS_CLASS::put( STRING_CLASS & co, STRING_CLASS & ma )
{
FILE_CLASS f;
NAME_CLASS s;

s.get_multipart_runlist_settings_csvname( co, ma );
if ( f.open_for_write(s) )
    {
    s  = EnabledString;
    s += CommaChar;
    s += bool_to_ascii( enable );
    f.writeline( s );

    s  = InputWiresString;
    s += CommaChar;
    if ( wires.isempty() )
        s += ZeroString;
    else
        s += wires;
    f.writeline( s );

    f.close();
    return true;
    }

return false;
}

/***********************************************************************
*                         MULTIPART_RUNLIST_SETINGS                    *
*                                   get                                *
* If there is no multipart_runlist_settings file then I need to check  *
* the ini file to see if the multipart runlist is set in the co-options*
* screen. If that is the case I need to read those settings.           *
***********************************************************************/
bool MULTIPART_RUNLIST_SETTINGS_CLASS::get( STRING_CLASS & co, STRING_CLASS & ma )
{
static TCHAR HaveMultipartRunlistKey[] = TEXT( "HaveMultipartRunlist" );
static TCHAR MultipartBaseWireKey[]    = TEXT( "Wire" );
static TCHAR MultipartSetupSection[]   = TEXT( "Multipart" );

TCHAR    * cp;
FILE_CLASS f;
int        i;
INI_CLASS  ini;
NAME_CLASS s;
WIRE_CLASS w;

s.get_multipart_runlist_settings_csvname( co, ma );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
        {
        s = f.readline();
        s.next_field();
        s.next_field();
        enable = ascii_to_bool( s.text() );

        wires.empty();
        n  = 0;
        s  = f.readline();
        cp = s.find( CommaChar );
        if ( cp )
            {
            cp++;
            if ( *cp != NullChar )
                {
                wires = cp;
                n = 1 + wires.countchars( CommaChar );
                }
            wires.remove_all_occurances( SpaceChar );
            }
        if ( wires.isempty() )
            wires = ZeroString;
        f.close();
        }
    else
        {
        return false;
        }
    }
else
    {
    /*
    ------------------------------------------------------------------------------------------------
    There is no multipart_runlist_settings file. See if there are settings in the VISITRAK.INI file.
    ------------------------------------------------------------------------------------------------ */
    s.get_exe_dir_file_name( VisiTrakIniFile );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    enable = ini.get_bool( HaveMultipartRunlistKey );

    wires.empty();
    n = 0;
    ini.set_section( MultipartSetupSection );
    for ( i=0; i<MAX_V7_MULTIPART_RUNLIST_WIRES; i++ )
        {
        s = MultipartBaseWireKey;
        s += int32toasc( i );

        if ( ini.find(s.text()) )
            {
            s = ini.get_string();
            if ( s.isnumeric() )
                {
                if ( i>0 )
                    wires += CommaChar;
                wires += s;
                }
            }
        }

    if ( !wires.isempty() )
        n = 1 + wires.countchars( CommaChar );
    }

/*
------------------------------------------------------
Make a mask the bits from all the input wires that can
be used for choosing the part.
------------------------------------------------------ */
mask = UZero;
if ( !wires.isempty() )
    {
    s = wires;
    cp = s.find( CommaChar );
    if ( cp )
        *cp = NullChar;

    while ( true )
        {
        i = s.int_value();
        if ( i>0 && i<=32 )
            mask |= w.wirebit( i );
        if ( !s.next_field() )
            break;
        }
    }

return true;
}

/***********************************************************************
*                              PART_TO_RUN                             *
***********************************************************************/
bool MULTIPART_RUNLIST_SETTINGS_CLASS::part_to_run( STRING_CLASS & dest, STRING_CLASS & co, STRING_CLASS & ma, unsigned current_multipart_runlist_wires )
{
MULTIPART_RUNLIST_WIRES_CLASS my_wires;;
STRING_CLASS                  pa;
TEXT_LIST_CLASS               partlist;

/*
-----------------------------------------------------
Turn off any wires that are not used for this machine
----------------------------------------------------- */
get( co, ma );
current_multipart_runlist_wires &= mask;

/*
----------------------------------------------------------------------
Check each part to see if it's runlist is the same as the current list
---------------------------------------------------------------------- */
if ( get_partlist(partlist, co, ma) )
    {
    partlist.rewind();
    while ( partlist.next() )
        {
        pa = partlist.text();
        my_wires.get( co, ma, pa );
        if ( my_wires.part_selection_number == NO_PART_SELECTION_NUMBER )
            continue;
        if ( my_wires.bits == current_multipart_runlist_wires )
            {
            dest = pa;
            return true;
            }
        }
    }

dest.empty();
return false;
}

/***********************************************************************
*                        PART_SELECTION_NUMBER                         *
***********************************************************************/
int MULTIPART_RUNLIST_SETTINGS_CLASS::part_selection_number( STRING_CLASS & wirelist )
{
int               i;
MULTISTRING_CLASS ms;
MULTISTRING_CLASS sorc;
unsigned int      u;
unsigned int      u_total;

u_total = 0;

sorc = wirelist;
sorc.remove_all_occurances( SpaceChar );
if ( sorc.isempty() || sorc == ZeroString )
    return 0;

sorc.split( CommaChar );

ms = wires;
ms.split( CommaChar );

sorc.rewind();
while ( sorc.next() )
    {
    i = ms.find( sorc );
    if ( i >= 0 )
        {
        u = 1;
        if ( i > 0 )
            u <<= i;
        u_total |= u;
        }
    }

return utoi( u_total );
}

/***********************************************************************
*                             GET_WIRELIST                             *
* Convert a part_selection_number into a comma separated wire list     *
***********************************************************************/
bool MULTIPART_RUNLIST_SETTINGS_CLASS::get_wirelist( STRING_CLASS & dest,  int selection_number )
{
bool              firstime;
int               i;
MULTISTRING_CLASS ms;
int               n;
unsigned int      u;
unsigned int      u_select_number;

if ( selection_number < 0 )
    {
    dest = ZeroString;
    return false;
    }

if ( selection_number == 0 )
    {
    dest = ZeroString;
    return true;
    }

if ( wires.isempty() )
    {
    dest = ZeroString;
    return false;
    }

firstime = true;

ms = wires;
ms.split( CommaChar );
n = ms.count();

u = 1;
u_select_number = (unsigned int) selection_number;

for ( i=0; i<n; i++ )
    {
    if ( u & u_select_number )
        {
        if ( firstime )
            {
            firstime = false;
            dest     = ms[i];
            }
        else
            {
            dest += CommaChar;
            dest += ms[i];
            }
        }
    u <<= 1;
    }

return true;
}

/***********************************************************************
*                       MAX_PART_SELECTION_NUMBER                      *
***********************************************************************/
int MULTIPART_RUNLIST_SETTINGS_CLASS::max_part_selection_number()
{
unsigned int u;

if ( n < 1 )
    return 0;

u = 1;
u <<= n;
u -= 1;

return utoi( u );
}

/***********************************************************************
*                    MULTIPART_RUNLIST_WIRES_CLASS                     *
*                             cleanup                                  *
***********************************************************************/
void MULTIPART_RUNLIST_WIRES_CLASS::cleanup()
{
part_selection_number = NO_PART_SELECTION_NUMBER;
bits = UZero;
wires.empty();
}

/***********************************************************************
*                    MULTIPART_RUNLIST_WIRES_CLASS                     *
***********************************************************************/
MULTIPART_RUNLIST_WIRES_CLASS::MULTIPART_RUNLIST_WIRES_CLASS()
{
part_selection_number = NO_PART_SELECTION_NUMBER;
bits = UZero;
}

/***********************************************************************
*                    ~MULTIPART_RUNLIST_WIRES_CLASS                    *
***********************************************************************/
MULTIPART_RUNLIST_WIRES_CLASS::~MULTIPART_RUNLIST_WIRES_CLASS()
{
}

/***********************************************************************
*                     MULTIPART_RUNLIST_WIRES_CLASS                    *
*                                isempty                               *
***********************************************************************/
bool MULTIPART_RUNLIST_WIRES_CLASS::isempty()
{
if ( wires == ZeroString || wires.isempty() )
    return true;

return false;
}

/***********************************************************************
*                     MULTIPART_RUNLIST_WIRES_CLASS                    *
*                                   =                                  *
***********************************************************************/
void MULTIPART_RUNLIST_WIRES_CLASS::operator=( const MULTIPART_RUNLIST_WIRES_CLASS & sorc )
{
part_selection_number = sorc.part_selection_number;
bits                  = sorc.bits;
wires                 = sorc.wires;
}

/***********************************************************************
*                         MULTIPART_RUNLIST_WIRES                      *
*                                   get                                *
* This reads the list of wires that, if on, will choose this part to   *
* be run.                                                              *
* The file consist of one line like the following. Description         *
* followed by the part selection number.                               *
* Part Selection Number,1                                              *
* It is possible the record will contain a list of wires instead.      *
* This was used in Version 8.15.                                       *
* Input Wires,1,2,5,15                                                 *
***********************************************************************/
bool MULTIPART_RUNLIST_WIRES_CLASS::get( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
TCHAR                          * cp;
FILE_CLASS                       f;
MULTIPART_RUNLIST_SETTINGS_CLASS mrs;
NAME_CLASS                       s;
bool                             status;
WIRE_CLASS                       w;

bits   = UZero;
status = true;
wires.empty();

/*
--------------------------------------------------
Get the multipart runlist setup in the machine dir
-------------------------------------------------- */
if ( !mrs.get(co, ma) )
    return false;

/*
------------------------------------------
Get the line from this part's runlist file
------------------------------------------ */
s.get_multipart_runlist_wires_csvname( co, ma, pa );

if ( !s.file_exists() )
    return false;

if ( !f.open_for_read(s) )
    return false;

s  = f.readline();
f.close();

/*
------------------------------------------------
Get the bit mask and the wire list for this part
------------------------------------------------ */
cp = s.find( CommaChar );
if ( cp )
    {
    *cp = NullChar;
    if ( s == PartSelectionNumberString )
        {
        s.next_field();
        part_selection_number = s.int_value();
        mrs.get_wirelist( wires, part_selection_number );
        }
    else
        {
        wires = cp;
        part_selection_number = mrs.part_selection_number( wires );
        }

    /*
    -----------------------------------------------------------------
    Get the Bits associated with all the wires that turn on this part
    ----------------------------------------------------------------- */
    bits = bits_from_wirelist( wires );

    return true;
    }

return false;
}

/***********************************************************************
*                         MULTIPART_RUNLIST_WIRES                      *
*                                   put                                *
* Part Selection Number,1                                              *
***********************************************************************/
bool MULTIPART_RUNLIST_WIRES_CLASS::put( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
FILE_CLASS                       f;
NAME_CLASS                       s;

s.get_multipart_runlist_wires_csvname( co, ma, pa );
if ( f.open_for_write(s) )
    {
    s  = PartSelectionNumberString;
    s += CommaChar;
    s += part_selection_number;
    f.writeline( s );
    f.close();
    return true;
    }

return false;
}

/***********************************************************************
*                      GET_PART_SELECTION_NUMBER                       *
*                  Sometimes you just need the number                  *
***********************************************************************/
int get_part_selection_number( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
FILE_CLASS f;
NAME_CLASS s;

s.get_multipart_runlist_wires_csvname( co, ma, pa );

if ( !s.file_exists() )
    return false;

if ( !f.open_for_read(s) )
    return false;

s = f.readline();
f.close();

if ( s.next_field() )
    {
    s.next_field();
    if ( !s.isempty() )
        return s.int_value();
    }

return NO_PART_SELECTION_NUMBER;
}