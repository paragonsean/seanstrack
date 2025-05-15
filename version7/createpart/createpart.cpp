#include <windows.h>
#include <math.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\asensor.h"
#include "..\include\dbclass.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\computer.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\param.h"
#include "..\include\param_index_class.h"
#include "..\include\part.h"
#include "..\include\marks.h"
#include "..\include\fileclas.h"
#include "..\include\ftanalog.h"
#include "..\include\stparam.h"
#include "..\include\stpres.h"
#include "..\include\stsetup.h"
#include "..\include\runlist.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"


#define _MAIN_
#include "createpart.h"
#include "resource.h"

#include "..\include\chars.h"

HINSTANCE MainInstance;
HWND      MainWindow = 0;

TCHAR     MyClassName[]= TEXT( "CreatePart" );

STRING_CLASS       NewPart;
STRING_CLASS       Computer;
STRING_CLASS       Machine;
STRING_CLASS       Part;
static STRING_CLASS            NotUsedString;                         /* Used by Multipart Runlist load/save */

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                           EXTRACT_PART_NAME                          *
* Pass the string with the part name in it. It is either contiguous    *
* or is delimited with quotes. Extract the name and put it into dest   *
* and return a pointer to the next string or to a null character.      *
***********************************************************************/
TCHAR * extract_part_name( STRING_CLASS & dest, TCHAR * sorc )
{
TCHAR * ep;
TCHAR   oldchar;

if ( *sorc == DoubleQuoteChar )
    {
    sorc++;
    ep = sorc;
    while ( (*ep != DoubleQuoteChar) && (*ep != NullChar) )
        ep++;
    if ( *ep == DoubleQuoteChar )
        {
        *ep = NullChar;
        dest = sorc;
        *ep = DoubleQuoteChar;
        ep++;
        if ( *ep == CommaChar )
            ep++;
        }
    }
else
    {
    ep = sorc;
    while ( (*ep != CommaChar) && (*ep != NullChar) )
        ep++;
    oldchar = *ep;
    if ( *ep == CommaChar )
        *ep = NullChar;
    dest = sorc;
    *ep = oldchar;
    if ( *ep != NullChar )
        ep++;
    }
return ep;
}

/***********************************************************************
*                          PARSE_COMMAND_LINE                          *
* The command line is the sorc name and dest name of part to create from an existing part.                   *
***********************************************************************/
BOOLEAN parse_command_line( TCHAR * cmd_line )
{
STRING_CLASS s;
TCHAR      * cp;

cp = findstring( MyClassName, cmd_line );
if ( cp )
    cp = findchar( SpaceChar, cp );

if ( cp )
    cp++; /* skip space */
else
    cp = cmd_line;

s = cp;
s.strip();
cp = extract_part_name( Part, s.text() );
if ( *cp != NullChar )
    extract_part_name( NewPart, cp );

if ( Part.isempty() )
    return FALSE;

if ( NewPart.isempty() )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                              STARTUP                                 *
***********************************************************************/
BOOLEAN startup( TCHAR * cmd_line )
{
COMPUTER_CLASS          c;
MACHINE_NAME_LIST_CLASS m;
NAME_CLASS              s;
INI_CLASS               ini;

if ( !parse_command_line(cmd_line) )
    return FALSE;

c.startup();
Computer = c.whoami();

ANALOG_SENSOR::get();
units_startup();

/*
---------------------------------------------
At the present time there is only one machine
--------------------------------------------- */
m.add_computer( Computer );
m.rewind();
if ( m.next() )
    Machine = m.name();
else
    return FALSE;

copypart( Computer, Machine, NewPart, Computer, Machine, Part, COPY_PART_ALL );

return TRUE;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
units_shutdown();
ANALOG_SENSOR::shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
TCHAR * my_cmd_line;

MainInstance = this_instance;

my_cmd_line = GetCommandLine();
if ( !startup(my_cmd_line) )
    return 0;

shutdown();
return(0);
}
