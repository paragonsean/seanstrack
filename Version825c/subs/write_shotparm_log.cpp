#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\iniclass.h"
#include "..\include\fileclas.h"
#include "..\include\timeclas.h"
#include "..\include\chars.h"

static TCHAR ConfigSection[]        = TEXT( "Config" );
static TCHAR LogShotparmAccessKey[] = TEXT( "LogShotparmAccess" );

/***********************************************************************
*                          WRITE_SHOTPARM_LOG                          *
***********************************************************************/
void write_shotparm_log( STRING_CLASS & ma, STRING_CLASS & pa, TCHAR * subroutine_name, bool openok, TIME_CLASS opentime, TIME_CLASS closetime, TCHAR * filename )
{
static bool  FirstTime = true;

FILE_CLASS   f;
NAME_CLASS   fn;
INI_CLASS    ini;
STRING_CLASS s;

fn.get_local_ini_file_name( MONALL_INI_FILE );
ini.set_file( fn.text() );
ini.set_section( ConfigSection );

if ( !ini.find(LogShotparmAccessKey) )
    return;

if ( !ini.get_bool() )
    return;

fn.get_log_dir_file_name( filename );

if ( FirstTime )
    {
    FirstTime = false;
    fn.ensure_file_directory_existance();
    }

s.init( 200 );
s = opentime.text();
s += MinusChar;
s.cat_w_char( closetime.hhmmss(), SpaceChar );
s.cat_w_char( ma, SpaceChar );
s.cat_w_char( pa, SpaceChar );
s += openok;
s += SpaceChar;
s += subroutine_name;

if ( f.open_for_append(fn) )
    {
    f.writeline( s );
    f.close();
    }
}
