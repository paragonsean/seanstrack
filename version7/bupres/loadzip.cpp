#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\subs.h"

#include "resource.h"
#include "extern.h"

static TCHAR   FilterEnd[]  = TEXT( "*.ZIP\0" );
static TCHAR   DefaultZipDir[] = TEXT( "\\" );
static TCHAR * Filter       = 0;                   /* TEXT( "Zip Files\0*.ZIP\0" ); */
static TCHAR * ChooseString = 0;
static TCHAR   BupString[]  = TEXT( "Bup" );
static TCHAR   DataString[] = TEXT( "Data" );

/***********************************************************************
*                             INIT_STRINGS                             *
***********************************************************************/
static void init_strings()
{
int32   slen;
TCHAR * s;

slen = 2 + lstrlen( FilterEnd );
s = resource_string( MainInstance, ZIP_FILES_STRING );
Filter = maketext( s, slen );
if ( Filter )
    {
    s = Filter;
    s = nextstring( Filter );
    lstrcpy( s, FilterEnd );
    s = nextstring( s );
    *s = NullChar;
    }

s = resource_string( MainInstance, CHOOSE_ZIP_FILE_STRING );
ChooseString = maketext( s );
}

/***********************************************************************
*                           CONVERT_V6_TO_V7                           *
*  When the v6 files have been restored they look like this            *
*  \v5\backup\bup                                                      *
*  machset.txt                                                         *
*  e20                                                                 *
*                                                                      *
***********************************************************************/
static void convert_v6_to_v7()
{
NAME_CLASS d;
NAME_CLASS s;

/*
----------------------------------------
Rename \v5\backup\bup to \v5\backup\data
---------------------------------------- */
s = BackupDir;
s.cat_path( BupString );

if ( !s.directory_exists() )
    return;

d = BackupDir;
d.cat_path( DataString );

s.moveto( d );

convert_v5_data_dir_to_v7( d );
}

/***********************************************************************
*                          LOAD_ZIP_FILE                               *
***********************************************************************/
void load_zip_file()
{
HCURSOR      old_cursor;
OPENFILENAME fh;
NAME_CLASS   buf;
NAME_CLASS   newdir;
TCHAR        fname[MAX_PATH+1];
TCHAR        oridir[MAX_PATH+1];
TCHAR      * cp;

fh.lStructSize       = sizeof( fh );
fh.hwndOwner         = MainWindow;
fh.hInstance         = 0;
fh.lpstrFilter       = Filter;
fh.lpstrCustomFilter = 0;
fh.nMaxCustFilter    = 0;
fh.nFilterIndex      = 0;
fh.lpstrFile         = fname;
fh.nMaxFile          = MAX_PATH;
fh.lpstrFileTitle    = 0;
fh.nMaxFileTitle     = 0;
fh.lpstrInitialDir   = oridir;
fh.lpstrTitle        = ChooseString;
fh.Flags             = OFN_EXPLORER;
fh.nFileOffset       = 0;
fh.nFileExtension    = 0;
fh.lpstrDefExt       = 0;
fh.lCustData         = 0;
fh.lpfnHook          = 0;
fh.lpTemplateName    = 0;

*fname = NullChar;

if ( !Filter )
    init_strings();

cp = get_ini_string( VisiTrakIniFile, BackupSection, RDLoadZipDirKey );
if ( !unknown(cp) )
    {
    copystring( oridir, cp );
    }
else
    {
    cp = get_ini_string( VisiTrakIniFile, BackupSection, TEXT("BDZipName") );
    if ( unknown(cp) )
        {
        cp = 0;
        }
    else
        {
        copystring( oridir, cp );
        if ( !dir_from_filename(oridir) )
            cp = 0;
        }
    }

if ( !cp )
    copystring( oridir, DefaultZipDir );

append_backslash_to_path( oridir );

if ( GetOpenFileName( &fh ) )
    {
    old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );
    if ( BackupDir.directory_exists() )
        BackupDir.delete_directory();

    newdir = BackupDir;
    newdir.remove_ending_backslash();

    if ( !newdir.directory_exists() )
        newdir.create_directory();

    if ( newdir.directory_exists() )
        {
        if ( GetCurrentDirectory(sizeof(oridir), oridir) == 0 )
            *oridir = NullChar;

        SetCurrentDirectory( newdir.text() );

        /*
        -------------------------------------------
        Assume pkunzip.exe is is the exes directory
        ------------------------------------------- */
        buf.get_exe_dir_file_name( TEXT("pkzip25 -ext -dir \"") );
        buf += fname;
        buf += TEXT( "\"" );

        execute_and_wait( buf.text(), newdir.text() );

        /*
        ------------------------------------------
        If this is an old backup, convert it to V7
        ------------------------------------------ */
        buf = BackupDir;
        buf += TEXT( "Bup" );
        if ( buf.directory_exists() )
            convert_v6_to_v7();
        if ( *oridir )
            SetCurrentDirectory( oridir );
        }

    if ( dir_from_filename(fname) )
        put_ini_string( VisiTrakIniFile, BackupSection, RDLoadZipDirKey, fname );

    SetCursor( old_cursor );
    }
}
