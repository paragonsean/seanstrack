#include <windows.h>
#include <shlobj.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

HINSTANCE    MainInstance;

static TCHAR MyTitle[]     = TEXT( "SerialTestSetup" );
static TCHAR MyLinkName[]  = TEXT( "SerialTestSetup.lnk" );
static TCHAR MyPath[]      = TEXT( "c:\\serialtest\\serialtest.exe" );
static TCHAR MyClassName[] = TEXT( "SerialTestInstall" );

/***********************************************************************
*                         CREATE_DESKTOP_LINK                          *
*  The title is what will appear on the desktop,                       *
*  e.g., Data Archive Restore                                          *
*                                                                      *
*  the link_name is the name of the link itself                        *
*  e.g. "data archive restore.lnk"                                     *
*                                                                      *
*  the exe_path is the full path to the executable                     *
*  e.g. C:\V5\EXES\DSRESTORE.EXE                                       *
***********************************************************************/
static void create_desktop_link( TCHAR * title, TCHAR * link_name, TCHAR * exe_path )
{
static TCHAR ShellFolderKey[] = TEXT( "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders" );

TCHAR buf[MAX_PATH];
WCHAR ws[MAX_PATH];

ULONG buf_size = MAX_PATH;
HKEY  hCU;
DWORD lpType;

HRESULT        result;
IShellLink   * psl;
IPersistFile * ppf;
bool           have_key;

/*
---------------------------
Get the path of the desktop
--------------------------- */
have_key = false;
if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, ShellFolderKey, 0, KEY_QUERY_VALUE, &hCU) == ERROR_SUCCESS )
    {
    buf_size = MAX_PATH * sizeof(TCHAR);
    if ( RegQueryValueEx( hCU, TEXT( "Common Desktop"), NULL, &lpType, (unsigned char *) &buf, &buf_size) == ERROR_SUCCESS )
        have_key = true;
    RegCloseKey(hCU);
    }

/*
-----------------------------------------
Win95 doesn't normally have the above key
----------------------------------------- */
if ( !have_key )
    {
    if ( RegOpenKeyEx(HKEY_CURRENT_USER, ShellFolderKey, 0, KEY_QUERY_VALUE, &hCU) == ERROR_SUCCESS )
        {
        buf_size = MAX_PATH * sizeof(TCHAR);
        if ( RegQueryValueEx( hCU, TEXT("Desktop"), NULL, &lpType, (unsigned char *) &buf, &buf_size) == ERROR_SUCCESS )
            have_key = true;
        RegCloseKey(hCU);
        }
    }

append_filename_to_path(  buf, link_name );

if ( CoInitialize(0) == S_OK )
    {
    /*
    ------------------------------------------
    Get a pointer to the IShellLink interface.
    ------------------------------------------ */
    result = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**) &psl );
    if (SUCCEEDED(result) )
        {
        /*
        ------------------------------------------------------------------------------------------------
        Query IShellLink for the IPersistFile interface for saving the shell link in persistent storage.
        ------------------------------------------------------------------------------------------------ */
        result = psl->QueryInterface( IID_IPersistFile, (void**) &ppf );
        if ( SUCCEEDED(result) )
            {
            /*
            --------------------------------------
            Set the path to the shell link target.
            -------------------------------------- */
            psl->SetPath( exe_path );

            /*
            --------------------------------------
            Set the description of the shell link.
            -------------------------------------- */
            psl->SetDescription( title );

            /*
            ----------------------
            Ensure string is ANSI.
            ---------------------- */
            #ifdef UNICODE
                lstrcpy( ws, buf );
            #else
                MultiByteToWideChar(CP_ACP, 0, buf, -1, ws, MAX_PATH);
            #endif

            /*
            ------------------------------------------------
            Save the link via the IPersistFile::Save method.
            ------------------------------------------------ */
            result = ppf->Save(ws, TRUE);

            /*
            --------------------------------
            Release pointer to IPersistFile.
            -------------------------------- */
            ppf->Release();
            }

        /*
        ------------------------------
        Release pointer to IShellLink.
        ------------------------------ */
        psl->Release();
        }

    CoUninitialize();
    }
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MainInstance = this_instance;
create_desktop_link( MyTitle, MyLinkName, MyPath );
return( 0 );
}
