#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\machine.h"
#include "..\include\setpoint.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\warmupcl.h"

static const TCHAR CommaChar     = TEXT( ',' );
static const TCHAR NullChar      = TEXT( '\0' );
static const TCHAR NChar         = TEXT( 'N' );
static const TCHAR LowerNChar    = TEXT( 'n' );
static       TCHAR ZeroString[]  = TEXT( "0" );
static       TCHAR EmptyString[] = TEXT( "" );

static       TCHAR MultipartSetupSection[] = TEXT( "Multipart" );
static       TCHAR SureTrakIniFile[]       = TEXT( "suretrak.ini" );
static       TCHAR EnabledKey[]            = TEXT( "Enabled" );
static       TCHAR MultipartBasePartKey[]  = TEXT( "Multipart" );
static       TCHAR MultipartBaseWireKey[]  = TEXT( "Wire" );

static TCHAR * Description[] = {
    { TEXT( "Is Enabled" ) },
    { TEXT( "Run If On" ) },
    { TEXT( "Wire Number" ) },
    { TEXT( "Part Name" ) }
    };

/***********************************************************************
*                            WARMUP_CLASS                              *
***********************************************************************/
WARMUP_CLASS::WARMUP_CLASS()
{
wire[0]     = NullChar;
is_enabled  = FALSE;
run_if_on   = FALSE;
}

/***********************************************************************
*                            WARMUP_CLASS                              *
***********************************************************************/
WARMUP_CLASS::WARMUP_CLASS( const WARMUP_CLASS & sorc )
{
copystring( wire,     sorc.wire     );
partname    = sorc.partname;
is_enabled  = sorc.is_enabled;
run_if_on   = sorc.run_if_on;
}

/***********************************************************************
*                            WARMUP_CLASS                              *
*                                GET                                   *
***********************************************************************/
BOOLEAN WARMUP_CLASS::get( STRING_CLASS & cn, STRING_CLASS & mn, STRING_CLASS & pn )
{
NAME_CLASS s;
SETPOINT_CLASS sp;

wire[0]     = NullChar;
partname.null();
is_enabled  = FALSE;
run_if_on   = FALSE;

s.get_warmup_csvname( cn, mn, pn );
if ( sp.get(s) )
    {
    is_enabled = sp[WARMUP_ENABLED_INDEX].boolean_value();
    run_if_on  = sp[WARMUP_RUN_IF_ON_INDEX].boolean_value();
    copystring( wire, sp[WARMUP_WIRE_INDEX].value.text(), WARMUP_WIRE_LEN );
    partname   = sp[WARMUP_PART_NAME_INDEX].value;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            WARMUP_CLASS                              *
*                                PUT                                   *
***********************************************************************/
BOOLEAN WARMUP_CLASS::put( STRING_CLASS & cn, STRING_CLASS & mn, STRING_CLASS & pn )
{
int            i;
NAME_CLASS     s;
SETPOINT_CLASS sp;

s.get_warmup_csvname( cn, mn, pn );

if ( s.file_exists() )
    sp.get( s );

sp.setsize( WARMUP_SETPOINT_COUNT );

for( i=0; i<WARMUP_SETPOINT_COUNT; i++ )
    {
    if ( sp[i].desc.isempty() )
        sp[i].desc = Description[i];
    }

sp[WARMUP_ENABLED_INDEX].value   = is_enabled;
sp[WARMUP_RUN_IF_ON_INDEX].value = run_if_on;
sp[WARMUP_WIRE_INDEX].value      = wire;
sp[WARMUP_PART_NAME_INDEX].value = partname;

return sp.put( s );
}

/***********************************************************************
*                            WARMUP_CLASS                              *
*                           create_runlist                             *
*                                                                      *
* Create a runlist table in the machine directory. The first entry     *
* will be for the current part. The second will be for the warmup      *
* part.                                                                *
*                                                                      *
***********************************************************************/
BOOLEAN WARMUP_CLASS::create_runlist( STRING_CLASS & co, STRING_CLASS & ma, STRING_CLASS & pa )
{
FILE_CLASS    f;
NAME_CLASS    s;
BOOLEAN       need_table;
unsigned long x;
MACHINE_CLASS m;
TCHAR         hex[MAX_INTEGER_LEN+1];

hex[0] = NullChar;

s.get_runlist_csvname( co, ma );

/*
----------------------------------------------------------------------------------------
Delete the table unless the warmup part is enabled and the parent and warmup parts exist
---------------------------------------------------------------------------------------- */
need_table = is_enabled;

m.find( co, ma );

/*
------------------------------------
Make sure there is a valid opto wire
------------------------------------ */
if ( need_table )
    {
    x = asctoul( wire );
    if( !m.is_ftii )
        {
        if ( x < LOW_ST_INPUT_WIRE || x > HIGH_ST_INPUT_WIRE )
            need_table = FALSE;
        }
    }

if ( need_table )
    need_table = part_exists( co, ma, pa );

if ( need_table )
    need_table = part_exists( co, ma, partname );

if ( !need_table )
    {
    s.delete_file();
    return TRUE;
    }

/*
---------------------------------------------------------------
Convert the wire number into the bit in the suretrak input word
--------------------------------------------------------------- */
if( m.is_ftii )
    x--;
else
    x -= LOW_ST_INPUT_WIRE;
x = 1<<x;

if ( f.open_for_write(s) )
    {
    ultoascii( hex, (unsigned int) x, HEX_RADIX );

    /*
    ---------------------------------
    Write the entry for the main part
    --------------------------------- */
    if ( run_if_on )
        s = ZeroString;
    else
        s = hex;

    s += CommaChar;
    s += pa;
    f.writeline( s.text() );

    /*
    -----------------------------------
    Write the entry for the warmup part
    ----------------------------------- */
    if ( run_if_on )
        s = hex;
    else
        s = ZeroString;

    s += CommaChar;
    s += partname;
    f.writeline( s.text() );

    f.close();
    return TRUE;
    }

return FALSE;
}
