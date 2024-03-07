#include <windows.h>

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
static TCHAR filename[]    = TEXT( "c:\\v5\\exes\\visitrak.ini" );
static TCHAR key[]         = TEXT( "HavePressureControl" );
static TCHAR section[]     = TEXT( "Config" );
static TCHAR sorc[]        = TEXT( "Y" );

WritePrivateProfileString( section, key, sorc, filename );

return( 0 );
}
