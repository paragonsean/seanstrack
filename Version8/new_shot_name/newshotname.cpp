#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"
#include "..\include\computer.h"

#define _MAIN_
#include "..\include\chars.h"

static TCHAR MyFile[]   = TEXT( "newshotname.txt" );
static TCHAR RealFile[] = TEXT( "value.txt" );
static int   ShotNumberLength = 3;

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int WINAPI WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
COMPUTER_CLASS c;
NAME_CLASS   fn;
NAME_CLASS   s;
STRING_CLASS sn;

TCHAR    * cp;
int        i;
FILE_CLASS f;

c.startup();

fn.get_exe_dir_file_name( MyFile );
if ( f.open_for_read(fn) )
    {
    s = f.readline();
    f.close();

    cp = s.last_char();
    cp -= (ShotNumberLength-1);
    sn = cp;

    i = sn.int_value();
    i++;
    sn = i;
    sn.rjust( ShotNumberLength, ZeroChar );
    *cp = NullChar;
    s += sn;
    if ( f.open_for_write(fn) )
        {
        f.writeline( s );
        f.close();
        s.get_exe_dir_file_name( RealFile );
        fn.copyto( s );
        }
    }

return 0;
}
