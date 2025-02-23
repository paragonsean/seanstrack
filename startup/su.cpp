#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"

static TCHAR MonallExeName[] = TEXT( "monall.exe" );
static TCHAR StartupListName[] = TEXT( "startup.lst" );
static TCHAR NineChar = TEXT( '9' );
static TCHAR ZeroChar = TEXT( '0' );

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int WINAPI WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
static STARTUPINFO info = {
    sizeof(STARTUPINFO),
    0, 0, 0,              // lpReserved, lpDesktop, lpTitle
    0,                    // dwX
    0,                    // dwY
    0, 0, 0, 0, 0,        // dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute
    STARTF_USESHOWWINDOW, // dwFlags
    0,                    // wShowWindow
    0, 0, 0, 0, 0         // cbReserved2, lpReserved2, hStdInput, hStdOutput, hStdError
    };

STRING_CLASS s;
TCHAR * cp;
FILE_CLASS f;
DWORD      myflags;
PROCESS_INFORMATION pi;
BOOL       status;

s = exe_directory();
s.cat_path( StartupListName );

if ( f.open_for_update(s.text()) )
    {
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            break;

        info.wShowWindow = SW_SHOW;
        if ( *cp >= ZeroChar && *cp <= NineChar )
            {
            Sleep( asctoint32(cp) );
            continue;
            }

        info.wShowWindow = SW_SHOW;
        if ( *cp == TEXT('-') )
            {
            info.wShowWindow = SW_SHOWMINIMIZED;
            cp++;
            }

        s = cp;

        /*
        ----------------------------------------------------------------------
        If this is monall, run it at high priority so it won't get swapped out
        if ( s.contains(MonallExeName) )
            myflags = HIGH_PRIORITY_CLASS;
        else
        ---------------------------------------------------------------------- */
            myflags = NORMAL_PRIORITY_CLASS;

        status = CreateProcess( 0,    // lpApplicationName
            s.text(),        // lpCommandLine
            0, 0, 0,         // lpProcessAttributes, lpThreadAttributes, bInheritHandles,
            myflags,         // dwCreationFlags
            0, 0,            // lpEnvironment, lpCurrentDirectory,
            &info,           // lpStartupInfo = input
            &pi              // lpProcessInformation = output
            );
        if ( status == 0 )
            {
            s = (unsigned int) GetLastError();
            MessageBox( 0, TEXT("createprocess"), s.text(), MB_OK );
            }
        }
    f.close();
    }

return 0;
}
