#include <windows.h>

#include "..\include\visiparm.h"

#include "..\include\nameclas.h"
#include "..\include\output_label_class.h"
#include "..\include\subs.h"

static const TCHAR NullChar         = TEXT( '\0' );
static const TCHAR LeftCurlyBraket  = TEXT( '{' );
static const TCHAR RightCurlyBraket = TEXT( '}' );
static const TCHAR OptoTag[]        = TEXT( "Opto" );
static const TCHAR TerminalTag[]    = TEXT( "Terminal" );
static const TCHAR WireTag[]        = TEXT( "Wire" );

static TCHAR * OptoWires[HIGH_FT2_OUTPUT_WIRE] = {
               TEXT("1"), TEXT("2"),  TEXT("3"),  TEXT("4"),  TEXT("5"),  TEXT("6"),  TEXT("7"),  TEXT("8"),
               TEXT("9"), TEXT("10"), TEXT("11"), TEXT("12"), TEXT("13"), TEXT("14"), TEXT("15"), TEXT("16"),
               TEXT("1"), TEXT("2"),  TEXT("3"),  TEXT("4"),  TEXT("5"),  TEXT("6"),  TEXT("7"),  TEXT("8"),
               TEXT("9"), TEXT("10"), TEXT("11"), TEXT("12"), TEXT("13"), TEXT("14"), TEXT("15"), TEXT("16")
               };

static OUTPUT_LABEL_CLASS output_label;

/***********************************************************************
*                   DYNAMIC_OUTPUT_WIRE_DESCRIPTION                    *
***********************************************************************/
BOOLEAN dynamic_output_wire_description( STRING_CLASS & dest, int wire_number, STRING_CLASS & pattern )
{
static bool firstime = true;

int          i;
STRING_CLASS sorc;

if ( firstime )
    {
    output_label.init();
    firstime = false;
    }

dest.null();
i = wire_number - 1;
if ( i < 0 || i >= HIGH_FT2_OUTPUT_WIRE )
    return FALSE;

sorc = pattern;
while ( sorc.next_field(LeftCurlyBraket) )
    {
    dest += sorc;
    if ( sorc.next_field(RightCurlyBraket) )
        {
        if ( sorc == OptoTag )
            {
            dest += OptoWires[i];
            }
        else if ( sorc == TerminalTag )
            {
            dest += output_label[i];
            }
        else if ( sorc == WireTag )
            {
            dest += wire_number;
            }
        else
            {
            dest += TEXT( "??" );
            }
        }
    }

dest += sorc;

return TRUE;
}
