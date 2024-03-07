#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\param.h"
#include "..\include\stringtable.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"
#include "..\include\vdbclass.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"
#include "extern.h"

static TCHAR Delimiter  = TEXT( '\t' );
static TCHAR CommaChar  = TEXT( ',' );
static TCHAR NChar      = TEXT( 'N' );
static TCHAR NullChar   = TEXT( '\0' );
static TCHAR PeriodChar = TEXT( '.' );
static TCHAR SpaceChar  = TEXT( ' '  );
static TCHAR TabChar    = TEXT( '\t' );
static TCHAR YChar      = TEXT( 'Y' );

static TCHAR CrLfStr[] = TEXT( "\r\n" );
static int   CrLfLen   = 2;
static TCHAR ExportingToCsvKey[] = TEXT( "ExportingToCsv" );
static TCHAR CsvFileNameKey[]    = TEXT( "ExportCsvFile" );
static TCHAR SelectParamsKey[]   = TEXT( "SelectedExportParams" );

#ifdef UNICODE
static UINT ClipFormat = CF_UNICODETEXT;
#else
static UINT ClipFormat = CF_TEXT;
#endif

static HWND MyWindow = 0;
static BOOLEAN OriginalExportToCsv = FALSE;
static BOOLEAN ExportingToCsv      = FALSE;

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
static TCHAR myhelpfile[] = TEXT( "export.hlp" );
NAME_CLASS s;

if ( s.get_exe_dir_file_name(myhelpfile) )
    WinHelp( MyWindow, s.text(), helptype, context );
}

/***********************************************************************
*                     SAVE_SELECTED_PARAMETER_LIST                     *
***********************************************************************/
static void save_selected_parameter_list( INT * p, int n )
{
int     i;
TCHAR * buf;
TCHAR * cp;

if ( n <= 0 || !p )
    return;

buf = maketext( n*(PARAMETER_NUMBER_LEN+1) );
if ( buf )
    {
    cp = buf;
    for ( i=0; i<n; i++ )
        cp = copy_w_char( cp, int32toasc((int32)p[i]), CommaChar );
    cp--;
    *cp = NullChar;
    put_ini_string( DisplayIniFile, ConfigSection, SelectParamsKey, buf );
    delete[] buf;
    }
}

/***********************************************************************
*                          COPY_TO_CLIPBOARD                           *
***********************************************************************/
static void copy_to_clipboard()
{
static TCHAR * alarm_type[] = { TEXT("Alarm Low"), TEXT("Alarm High"), TEXT("Warn Low"), TEXT("Warn High") };
static TCHAR * limit_type[] = { TEXT("Min"), TEXT("Max"), TEXT("Min"), TEXT("Max") };
static int NOF_TITLE_ROWS    = 2;
static int NOF_UNITS_ROWS    = 1;

LISTBOX_CLASS sb;
LISTBOX_CLASS pb;

TIME_CLASS        closetime;
TCHAR           * cp;
STRING_CLASS      date_name_header;
FILE_CLASS        f;
int               i;
int               j;
int               k;
int               last_param;
MULTISTRING_CLASS ms;
HGLOBAL           mymem;
int               nof_params;
int               nof_shots;
bool              openok;
TIME_CLASS        opentime;
PARAMETER_CLASS   p;
INT             * pindex;
STRING_CLASS      s1;
STRING_CLASS      s2;
NAME_CLASS        s;
STRING_CLASS      shot_name_header;
INT             * sindex;
DWORD             slen;
VDB_CLASS         t;
STRING_CLASS      time_name_header;

if ( is_checked(MyWindow, EXPORT_CSV_FILE_RADIO) )
    {
    ExportingToCsv = true;
    Delimiter = CommaChar;
    }
else
    {
    ExportingToCsv = false;
    Delimiter = TabChar;
    }

shot_name_header = resource_string( SHOT_NUMBER_STRING );
date_name_header = resource_string( DATE_NAME_STRING );
time_name_header = resource_string( TIME_NAME_STRING );

sb.init( MyWindow, SHOT_LISTBOX );
pb.init( MyWindow, PARAM_NAME_LISTBOX );

nof_params = pb.get_select_list( &pindex );
if ( nof_params < 1 )
    return;

nof_shots = sb.get_select_list( &sindex );
if ( nof_shots < 1 )
    return;

ms.set_array_size( NOF_TITLE_ROWS + nof_shots + NOF_ALARM_LIMIT_TYPES + NOF_UNITS_ROWS );
ms.rewind();
ms.next();

/*
--------------------------------------
The first line is the machine and part
-------------------------------------- */
ms = MachineName;
ms += Delimiter;
ms += PartName;

/*
-------------------------------
The second line is column names
------------------------------- */
ms.next();
ms =  shot_name_header;
ms += Delimiter;
ms.cat_w_char( date_name_header, Delimiter );
ms.cat_w_char( time_name_header, Delimiter );

last_param = nof_params - 1;
for ( i=0; i<nof_params; i++ )
    {
    cp = pb.item_text( pindex[i] );
    if ( i < last_param )
        ms.cat_w_char( cp, Delimiter );
    else
        ms += cp;
    }

/*
-----------------
One line per shot
----------------- */
s.get_shotparm_csvname( ComputerName, MachineName, PartName );

openok = false;
opentime.get_local_time();
closetime = opentime;

if ( t.open_for_read(s) )
    {
    openok = true;
    opentime.get_local_time();
    i = 0;
    while ( t.get_next_record() )
        {
        if ( t.current_record_number() == sindex[i] )
            {
            ms.next();
            ms = t.ra[SHOTPARM_SHOT_NAME_INDEX];
            ms += Delimiter;

            ms.cat_w_char( t.ra[SHOTPARM_DATE_INDEX], Delimiter );
            ms.cat_w_char( t.ra[SHOTPARM_TIME_INDEX], Delimiter );

            for ( j=0; j<nof_params; j++ )
                {
                if ( j < last_param )
                    ms.cat_w_char( t.ra[SHOTPARM_PARAM_1_INDEX + pindex[j]], Delimiter );
                else
                    ms += t.ra[SHOTPARM_PARAM_1_INDEX + pindex[j]];
                }

            i++;
            if ( i >= nof_shots )
                break;
            }
        }
    t.close();
    closetime.get_local_time();
    }

write_shotparm_log( MachineName, PartName, TEXT("update_day_param_display"), openok, opentime, closetime, ShotparmLogFile );

/*
--------------------
Add the alarm limits
-------------------- */
p.find( ComputerName, MachineName, PartName );

for ( k=0; k<NOF_ALARM_LIMIT_TYPES; k++ )
    {
    ms.next();
    ms = alarm_type[k];
    ms += Delimiter;
    ms.cat_w_char( limit_type[k], Delimiter );
    /*
    -------------
    Skip a column
    ------------- */
    ms += Delimiter;

    for ( i=0; i<nof_params; i++ )
        {
        j = pindex[i];
        ms += p.parameter[j].limits[k].value;
        if ( i < last_param )
            ms += Delimiter;
        }
    }

/*
---------
Add units
--------- */
ms.next();
ms = resource_string( UNITS_TITLE_STRING );
ms += Delimiter;
ms += Delimiter;
ms += Delimiter;

for ( i=0; i<nof_params; i++ )
    {
    j = pindex[i];
    if ( i < last_param )
        ms.cat_w_char( units_name(p.parameter[j].units), Delimiter );
    else
        ms += units_name( p.parameter[j].units );
    }

/*
---------------------------
I'm done with the shot list
--------------------------- */
delete[] sindex;

if ( ExportingToCsv )
    {
    /*
    --------------------
    Export to a CSV file
    -------------------- */
    s.get_text( MyWindow, CSV_FILE_NAME_EBOX );
    if ( s.isempty() )
        {
        resource_message_box( MainInstance, EXPORT_NO_CSV_STRING, CANNOT_COMPLY_STRING );
        }
    else
        {
        if ( !s.contains(PeriodChar) )
            s += CSV_SUFFIX;
        if ( f.open_for_write(s) )
            {
            ms.rewind();
            while ( ms.next() )
                f.writeline( ms );
            f.close();
            }
        }
    }
else
    {
    /*
    ----------------------------
    Get the length of the buffer
    ---------------------------- */
    slen = 0;
    ms.rewind();
    while ( ms.next() )
        {
        slen += ms.len();
        slen += CrLfLen;
        }
    slen++; /* Room for a Null Character at the end */

#ifdef UNICODE
    slen *= sizeof( TCHAR );
#endif

    /*
    ---------------------
    Copy to the clipboard
    --------------------- */
    mymem = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, slen );
    if ( !mymem )
        {
        resource_message_box( MainInstance, NO_MEMORY_STRING, CANNOT_COMPLY_STRING );
        }
    else
        {
        cp = (TCHAR *) GlobalLock(mymem);
        if ( cp )
            {
            /*
            -----------------------------------------
            Copy all the records to the Global buffer
            ----------------------------------------- */
            ms.rewind();
            while ( ms.next() )
                {
                cp = copystring( cp, ms.text() );
                cp = copystring( cp, CrLfStr   );
                }
            *cp = NullChar;

            GlobalUnlock( mymem );

            /*
            --------------------------------
            Send the buffer to the Clipboard
            -------------------------------- */
            if ( OpenClipboard(MyWindow) )
                {
                EmptyClipboard();
                if ( SetClipboardData(ClipFormat, mymem) )
                    mymem = 0;
                CloseClipboard();
                }

            if ( mymem )
                GlobalFree( mymem );
            }
        }
    }

save_selected_parameter_list( pindex, nof_params );
delete[] pindex;
}

/***********************************************************************
*                         FILL_PARAMETER_NAMES                         *
***********************************************************************/
static void fill_parameter_names( void )
{
int           i;
int           n;
LISTBOX_CLASS lb;
TCHAR       * s;
TCHAR       * cp;
BOOLEAN       havemore;

lb.init( MyWindow, PARAM_NAME_LISTBOX );

n = (int) CurrentParam.count();
if ( n <= 0 )
    return;

for ( i=0; i<n; i++ )
    lb.add( CurrentParam.parameter[i].name );

s = maketext( get_long_ini_string( DisplayIniFile, ConfigSection, SelectParamsKey) );
if ( !unknown(s) )
    {
    cp = s;
    while ( true )
        {
        havemore = replace_char_with_null( cp, CommaChar );
        if ( !is_numeric(*cp) )
            break;
        i = (int) asctoint32( cp );
        if ( i < n )
            lb.set_select( i, TRUE );
        if ( !havemore )
            break;
        cp = nextstring( cp );
        }
    }

if ( s )
    delete[] s;
}

/***********************************************************************
*                              FILL_SHOTS                              *
***********************************************************************/
static void fill_shots()
{
STRING_CLASS  buf;
TIME_CLASS    closetime;
LISTBOX_CLASS lb;
HCURSOR       old_cursor;
bool          openok;
TIME_CLASS    opentime;
NAME_CLASS    s;
VDB_CLASS     t;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

lb.init( MyWindow, SHOT_LISTBOX );
lb.empty();
lb.redraw_off();
s.get_shotparm_csvname( ComputerName, MachineName, PartName );

openok = false;
opentime.get_local_time();
closetime = opentime;

if ( t.open_for_read(s) )
    {
    openok = true;
    opentime.get_local_time();
    while ( t.get_next_record() )
        {
        s = t.ra[SHOTPARM_SHOT_NAME_INDEX];
        s += SpaceChar;

        s.cat_w_char( t.ra[SHOTPARM_DATE_INDEX], SpaceChar );
        s += t.ra[SHOTPARM_TIME_INDEX];
        lb.add( s.text() );
        }
    t.close();
    closetime.get_local_time();
    }

write_shotparm_log( MachineName, PartName, TEXT("fill_shots"), openok, opentime, closetime, ShotparmLogFile );

lb.redraw_on();
lb.select_all();
SetCursor( old_cursor );
}

/***********************************************************************
*                              SELECT_ALL                              *
***********************************************************************/
static void select_all( int id )
{
LISTBOX_CLASS lb;
INT listbox_id;

if ( id == ALL_SHOTS_BUTTON )
    listbox_id = SHOT_LISTBOX;
else if ( id == ALL_PARAMETERS_BUTTON )
    listbox_id = PARAM_NAME_LISTBOX;
else
    return;

lb.init( MyWindow, listbox_id );
lb.select_all();
}

/***********************************************************************
*                    SET_EXPORT_SHOTS_SETUP_STRINGS                    *
***********************************************************************/
static void set_export_shots_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { SHOT_TITLE_STATIC,     TEXT("SHOT_NUMBER_STRING") },
    { DATE_TITLE_STATIC,     TEXT("DATE_STRING") },
    { PARAM_TITLE_STATIC,    TEXT("PARAMETERS_STRING") },
    { ALL_SHOTS_BUTTON,      TEXT("ALL_STRING") },
    { ALL_PARAMETERS_BUTTON, TEXT("ALL_STRING") },
    { IDOK,                  TEXT("BEGIN_EXPORT") },
    { EXPORT_TO_CLIPBOARD_RADIO, TEXT("EXPORT_TO_CLIP") },
    { EXPORT_CSV_FILE_RADIO,     TEXT("EXPORT_TO_CSV") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    SHOT_LISTBOX,
    PARAM_NAME_LISTBOX,
    CSV_FILE_NAME_EBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("EXPORT_SHOT_PARAMS")) );
w.refresh();
}

/***********************************************************************
*                          EXPORT_SHOTS_PROC                           *
***********************************************************************/
BOOL CALLBACK export_shots_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static STRING_CLASS s;
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MyWindow = hWnd;
        set_export_shots_setup_strings( hWnd );
        OriginalExportToCsv = boolean_from_ini( DisplayIniFile, ConfigSection, ExportingToCsvKey );
        ExportingToCsv = OriginalExportToCsv;
        if ( ExportingToCsv )
            id = EXPORT_CSV_FILE_RADIO;
        else
            id = EXPORT_TO_CLIPBOARD_RADIO;
        CheckRadioButton( MyWindow, EXPORT_TO_CLIPBOARD_RADIO, EXPORT_CSV_FILE_RADIO, id );
        s = get_ini_string( DisplayIniFile, ConfigSection, CsvFileNameKey );
        if ( unknown(s.text()) )
            s.empty();
        s.set_text( MyWindow, CSV_FILE_NAME_EBOX );
        s.empty();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_shots();
        fill_parameter_names();
        SetFocus( GetDlgItem(hWnd,SHOT_LISTBOX) );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, EXPORT_MENU_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case ALL_SHOTS_BUTTON:
            case ALL_PARAMETERS_BUTTON:
                select_all( id );
                return TRUE;

            case IDOK:
                copy_to_clipboard();
                if ( ExportingToCsv != OriginalExportToCsv )
                    boolean_to_ini( DisplayIniFile, ConfigSection, ExportingToCsvKey, ExportingToCsv );
                if ( SendDlgItemMessage(MyWindow, CSV_FILE_NAME_EBOX, EM_GETMODIFY, 0, 0L) == TRUE )
                    {
                    s.get_text( MyWindow, CSV_FILE_NAME_EBOX );
                    put_ini_string( DisplayIniFile, ConfigSection, CsvFileNameKey, s.text() );
                    s.empty();
                    }

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}
