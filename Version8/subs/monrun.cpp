#include <windows.h>

static TCHAR MonitorClassName[]   = TEXT( "MonAll" );
static TCHAR MonitorWindowTitle[] = TEXT( "Board Monitor" );

BOOLEAN monitor_is_running()
{

if ( FindWindow( MonitorClassName, MonitorWindowTitle ) )
    return TRUE;

return FALSE;
}
