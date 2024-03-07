#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\listbox.h"
#include "..\include\vdbclass.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"
#include <shlobj.h>

#include "resource.h"
#include "resrc1.h"

#define _MAIN_
#include "..\include\chars.h"

static const int MAX_PARMS_V7 = 60;
HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
LISTBOX_CLASS MainListbox;

static TCHAR MyClassName[] = TEXT( "V8toV7");

/***********************************************************************
*                        CONVERT_FROM_V8_TO_V7                         *
***********************************************************************/
static void convert_from_v8_to_v7( TCHAR * part_directory_path )
{
static short  WRITE_PARAM_TIMEOUT = 150;   /* About 1/4 second */
static int    V7_MAX_PARMS = 60;

TCHAR      * cp;
DB_TABLE     db;
FILE_CLASS   dest;
NAME_CLASS   dest_name;
FIELDOFFSET  dif;
FIELDOFFSET  fo;
bool         have_new_file;
int          i;
STRING_CLASS s;
short        flags;
DB_TABLE     sorcdb;
FILE_CLASS   sorc;
NAME_CLASS   sorc_name;
SYSTEMTIME   st;
double       x;
VDB_CLASS    vdb;

have_new_file = false;
sorc_name = part_directory_path;

/*
-----------
Plookup.csv
----------- */
sorc_name.cat_path( TEXT("plookup.csv") );
MainListbox.add( sorc_name.text() );

dest_name = part_directory_path;
dest_name.cat_path( TEXT("newplookup.csv") );
MainListbox.add( dest_name.text() );

if ( sorc.open_for_read(sorc_name) )
    {
    if ( dest.open_for_write(dest_name) )
        {
        /*
        -----------------------------------------
        Skip the first line (V7 has no shot name)
        ----------------------------------------- */
        sorc.readline();
        /*
        ---------------------------------------------
        Copy the next three lines. Skip the last line
        --------------------------------------------- */
        for ( i=0; i<3; i++ )
            dest.writeline( sorc.readline() );
        dest.close();
        have_new_file = true;
        }
    sorc.close();
    }

if ( have_new_file )
    {
    sorc_name.delete_file();
    dest_name.moveto( sorc_name );
    }

/*
--------
Parmlist
-------- */
sorc_name = part_directory_path;
sorc_name.cat_path( TEXT("parmlist.csv") );
MainListbox.add( sorc_name.text() );

dest_name = part_directory_path;
dest_name.cat_path( TEXT("parmlist.txt") );
MainListbox.add( dest_name.text() );

if ( sorc.open_for_update(sorc_name) )
    {
    if ( sorc.nof_lines() > V7_MAX_PARMS )
        {
        sorc.rewind();
        for ( i=0; i<60; i++ )
            sorc.readline();
        sorc.set_eof();
        }
    sorc.close();
    }

if ( sorc.open_for_read(sorc_name) )
    {
    db.ensure_existance( dest_name );
    if (db.open(dest_name, PARMLIST_RECLEN, FL) )
        {
        while ( true )
            {
            cp = sorc.readline();
            if ( !cp )
                break;
            s = cp;
            s.next_field();

            i = s.int_value();
            if ( i > 60 )
                break;
            db.put_short( PARMLIST_PARM_NUMBER_OFFSET, (short) i, PARAMETER_NUMBER_LEN );

            /*
            ----------
            Input Type
            ---------- */
            if ( s.next_field() )
                i = s.int_value();

            if ( i == NO_PARAMETER_TYPE )
                continue;
            db.put_short( PARMLIST_INPUT_TYPE_OFFSET, (short) i, PARMLIST_INPUT_TYPE_LEN );

            if ( s.next_field() )
                db.put_short( PARMLIST_INPUT_NUMBER_OFFSET, (short) s.int_value(), INPUT_NUMBER_LEN );

            if ( s.next_field() )
                db.put_long( PARMLIST_VARIABLE_TYPE_OFFSET, (long) s.uint_value(), BITSETYPE_LEN );

            if ( s.next_field() )
                db.put_short( PARMLIST_UNITS_OFFSET, (short) s.int_value(), UNITS_ID_LEN );

            if ( s.next_field() )
                db.put_alpha( PARMLIST_DESC_OFFSET, s, PARAMETER_NAME_LEN );

            for ( i=0; i<NOF_ALARM_LIMIT_TYPES; i++ )
                {
                switch ( i )
                    {
                    case ALARM_MIN:
                        fo = PARMLIST_LOW_ALARM_LIMIT_OFFSET;
                        break;
                    case ALARM_MAX:
                        fo = PARMLIST_HIGH_ALARM_LIMIT_OFFSET;
                        break;
                    case WARNING_MIN:
                        fo = PARMLIST_LOW_WARN_LIMIT_OFFSET;
                        break;
                    default:
                        fo = PARMLIST_HIGH_WARN_LIMIT_OFFSET;
                        break;
                    }
                if ( s.next_field() )
                    db.put_float( fo, s.float_value(), PARMLIST_FLOAT_LEN ); // Value
                fo += PARMLIST_FLOAT_LEN + 1;
                if ( s.next_field() )
                    db.put_short( fo, (short) s.int_value(), WIRE_LEN ); // Wire
                }
           db.rec_append();
           }

        db.close();
        }
    sorc.close();
    }

sorc_name.delete_file();

/*
--------
FTAnalog
-------- */
sorc_name = part_directory_path;
sorc_name.cat_path( TEXT("ftanalog.csv") );
MainListbox.add( sorc_name.text() );
if ( sorc.open_for_update(sorc_name) )
    {
    if ( sorc.nof_lines() > 60 )
        {
        sorc.rewind();
        for ( i=0; i<60; i++ )
            sorc.readline();
        sorc.set_eof();
        }
    sorc.close();
    }

/*
--------------------
Parameter_sort_order
-------------------- */
sorc_name = part_directory_path;
sorc_name.cat_path( TEXT("parameter_sort_order.csv") );
MainListbox.add( sorc_name.text() );
if ( sorc.open_for_update(sorc_name) )
    {
    s = sorc.readline();
    cp = s.text();
    i = 0;
    while ( true )
        {
        if ( *cp == NullChar )
            break;
        if ( *cp == CommaChar )
            i++;
        if ( i >= 60 )
            {
            *cp = NullChar;
            sorc.rewind();
            sorc.writeline( s );
            sorc.set_eof();
            break;
            }
        cp++;
        }
    sorc.close();
    }

/*
--------
Shotparm
-------- */
sorc_name = part_directory_path;
sorc_name.cat_path( TEXT("results") );
sorc_name.cat_path( TEXT("shotparm.csv") );
MainListbox.add( sorc_name.text() );

dest_name = part_directory_path;
dest_name.cat_path( TEXT("results") );
dest_name.cat_path( TEXT("shotparm.txt") );
MainListbox.add( dest_name.text() );

if ( vdb.open_for_read(sorc_name) )
    {
    db.ensure_existance( dest_name );
    if (db.open(dest_name, SHOTPARM_RECLEN, WL, WRITE_PARAM_TIMEOUT) )
        {
        while ( vdb.get_next_record() )
            {
            s = vdb.ra[SHOTPARM_SHOT_NAME_INDEX];
            db.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, s.int_value(), SHOT_LEN );
            init_systemtime_struct( st );
            vdb.get_date( st, SHOTPARM_DATE_INDEX );
            vdb.get_time( st, SHOTPARM_TIME_INDEX );

            db.put_date( SHOTPARM_DATE_OFFSET, st );
            db.put_time( SHOTPARM_TIME_OFFSET, st );

            i = vdb.ra[SHOTPARM_SAVE_FLAGS_INDEX].int_value();
            db.put_short( SHOTPARM_SAVE_FLAGS_OFFSET, (short) i, SHOTPARM_SAVE_FLAGS_LEN );

            fo = SHOTPARM_PARAM_1_OFFSET;
            for ( i=0; i<MAX_PARMS_V7; i++ )
                {
                x = vdb.ra[SHOTPARM_PARAM_1_INDEX+i].real_value();
                db.put_double( fo, x, SHOTPARM_FLOAT_LEN );
                fo += SHOTPARM_FLOAT_LEN+1;
                }
            db.rec_append();
            }
        db.close();
        }
    vdb.close();
    }

sorc_name.delete_file();

/*
--------
Graphlst
-------- */
sorc_name = part_directory_path;
sorc_name.cat_path( TEXT("results") );
sorc_name.cat_path( TEXT("graphlst.txt") );
MainListbox.add( sorc_name.text() );

dest_name = part_directory_path;
dest_name.cat_path( TEXT("results") );
dest_name.cat_path( TEMP_GRAPHLST_DB );
MainListbox.add( dest_name.text() );

if ( sorcdb.open(sorc_name, 0, PFL) )
    {
    db.ensure_existance( dest_name );
    if (db.open(dest_name, GRAPHLST_RECLEN, WL, WRITE_PARAM_TIMEOUT) )
        {
        while ( sorcdb.get_next_record(NO_LOCK) )
            {
            sorcdb.get_alpha( s, GRAPHLST_SHOT_NAME_INDEX );
            fo = (FIELDOFFSET) s.len();
            fo += 3;
            dif = fo - GRAPHLST2_DATE_OFFSET;
            init_systemtime_struct( st );
            sorcdb.get_date( st, fo );
            fo = (FIELDOFFSET) GRAPHLST2_TIME_OFFSET + dif;
            sorcdb.get_time( st, fo );
            fo = (FIELDOFFSET) GRAPHLST2_SAVE_FLAGS_OFFSET + dif;
            flags = sorcdb.get_short( fo );

            db.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, (long)s.int_value(), SHOT_LEN );
            db.put_date( GRAPHLST_DATE_OFFSET, st );
            db.put_time( GRAPHLST_TIME_OFFSET, st );
            db.put_short( GRAPHLST_SAVE_FLAGS_OFFSET, flags, SHOTPARM_SAVE_FLAGS_LEN );
            db.rec_append();
            }
        db.close();
        }
    sorcdb.close();

    sorc_name.delete_file();
    dest_name.moveto( sorc_name );
    }
}

/***********************************************************************
*                            BROWSE_FOR_PART                           *
***********************************************************************/
static void browse_for_part( HWND w )
{
TCHAR        buf[MAX_PATH];
BROWSEINFO   bi;
ITEMIDLIST * pidlBrowse;
ITEMIDLIST * startdir;
LPMALLOC     ip;
BOOL         havebuf;
static STRING_CLASS lastpath;

startdir = NULL;

bi.hwndOwner      = w;
bi.pidlRoot       = startdir;
bi.pszDisplayName = buf;
bi.lpszTitle      = TEXT("Choose a part folder");
bi.ulFlags        = 0;
bi.lpfn           = 0;
bi.lParam         = 0;
bi.iImage         = 0;
buf[0] = 0;

havebuf = FALSE;
if ( SUCCEEDED(SHGetMalloc(&ip)) )
    {
    pidlBrowse = SHBrowseForFolder( &bi );
    if ( pidlBrowse != NULL )
        {
        havebuf = SHGetPathFromIDList( pidlBrowse, buf );

        ip->Free( pidlBrowse );
        }
    ip->Release();
    }

if ( havebuf )
    {
    MainListbox.add( buf );
    convert_from_v8_to_v7( buf );
    lastpath = buf;
    }
}

/***********************************************************************
*                        resize_main_listbox                           *
************************************************************************/
static void resize_main_listbox( LPARAM lParam )
{
int width;
int height;

width  = LOWORD( lParam );
height = HIWORD( lParam );

MoveWindow( MainListbox.handle(), 0, 0, width, height, TRUE );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
STRING_CLASS  s;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_SIZE:
        resize_main_listbox( lParam );
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case CHOOSE_PART_CHOICE:
                browse_for_part( hWnd );
                return 0;

            case EXIT_CHOICE:
                MainWindow.close();
                return 0;
            }

        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
WNDCLASS wc;
HWND     lw;
HWND     w;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

w = CreateWindow(
    MyClassName,
    TEXT("Main Window Title"),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 300,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

lw = CreateWindow (
    TEXT("LISTBOX"),
    "0",
    WS_CHILD | WS_VISIBLE | LBS_NOSEL,
    0, 0,
    80, 20,
    w,
    (HMENU) MAIN_LISTBOX_ID,
    MainInstance,
    NULL
    );
MainListbox.init( lw );
MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
