#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\event_client.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\computer.h"
#include "..\include\servicelog.h"
#include "..\include\stringtable.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\dstat.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "category.h"
#include "resource.h"

#define _MAIN_
#include "..\include\chars.h"

SERVICE_LOG  ServiceLog;
EVENT_CLIENT EventClient;

static const int CODE_LEN = 2 * DOWNCAT_NUMBER_LEN;

HACCEL  AccelHandle;
TCHAR   AccelName[] = TEXT("EDITDOWN_HOT_KEYS");

HINSTANCE MainInstance;
HWND   MainWindow;
HWND   EditDownWindow;

int32  CurrentPasswordLevel = LOWEST_PASSWORD_LEVEL;

static const int NOF_EDITBOXES = CODE_EDITBOX - DATE_EDITBOX + 1;

STRING_CLASS MyComputer;
STRING_CLASS MyMachine;
TCHAR * NotFoundError = 0;
DB_TABLE T;

static TIME_CLASS OriginalTime;
static BOOLEAN    IsInsert   = FALSE;
static BOOLEAN    HaveChange = FALSE;

static int NofLines     = 10000;
static int TopLine      = 0;
static int CurrentLine  = 0;
static int LinesPerPage = 10;

TCHAR MyClassName[]      = TEXT("EditDown");
TCHAR MainHelpFileName[] = TEXT( "v5help.hlp" );
STRING_CLASS MyWindowTitle;

static WNDPROC OldEditProc[NOF_EDITBOXES];

static TCHAR CommonResourceFile[]   = TEXT( "common" );
static TCHAR ConfigSection[]        = TEXT( "Config" );
static TCHAR CurrentLanguageKey[]   = TEXT( "CurrentLanguage" );
static TCHAR DisplayIniFile[]       = TEXT( "display.ini" );
static TCHAR EditDownResourceFile[] = TEXT( "editdown" );

FONT_LIST_CLASS   FontList;
STRINGTABLE_CLASS StringTable;

CATEGORY_CLASS Category;
SUBCAT_CLASS   SubCategory;

static BOOLEAN ProgramIsUpdating = FALSE;

class EDITDOWN_CLASS : public TIME_CLASS
{
private:

TCHAR      category[DOWNCAT_NUMBER_LEN+1];
TCHAR      subcategory[DOWNCAT_NUMBER_LEN+1];
STRING_CLASS part;
STRING_CLASS worker;

void init(){ *category = NullChar; *subcategory = NullChar; }

public:

EDITDOWN_CLASS();
void    operator=( const TIME_CLASS & sorc ) { *((TIME_CLASS *)this) = sorc; }
BOOLEAN operator==( const TIME_CLASS & sorc ) { return (*((TIME_CLASS *)this) == sorc); }
TCHAR * lbline();
TCHAR * ddeline();
void    fill_dstat( DSTAT_CLASS & d );
void    fill_editboxes();
BOOLEAN read_editboxes();
BOOLEAN read( DB_TABLE & t );
BOOLEAN read( TCHAR * lb_sorc_line );
BOOLEAN put_record();
};

/***********************************************************************
*                       ST_RESOURCE_MESSAGE_BOX                        *
***********************************************************************/
int st_resource_message_box( TCHAR * msg_id, TCHAR * title_id, UINT boxtype )
{
STRING_CLASS msg;
STRING_CLASS title;

StringTable.find( msg,  msg_id );
StringTable.find( title, title_id );

return resource_message_box( msg.text(), title.text(), boxtype );
}

/***********************************************************************
*                       ST_RESOURCE_MESSAGE_BOX                        *
***********************************************************************/
void st_resource_message_box( TCHAR * msg_id, TCHAR * title_id )
{
STRING_CLASS msg;
STRING_CLASS title;

StringTable.find( msg,  msg_id );
StringTable.find( title, title_id );

MessageBox( NULL, msg.text(), title.text(), MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( TCHAR * fname, UINT helptype, DWORD context )
{
TCHAR path[MAX_PATH+1];

if ( get_exe_directory(path) )
    {
    lstrcat( path, fname );
    WinHelp( MainWindow, path, helptype, context );
    }
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( UINT helptype, DWORD context )
{
gethelp( MainHelpFileName, helptype, context );
}

/***********************************************************************
*                                 CODE                                 *
***********************************************************************/
static TCHAR * code( TCHAR * cat, TCHAR * subcat )
{
static TCHAR s[CODE_LEN+1];
TCHAR * cp;

cp = s;
lstrcpy( cp, cat );
cp += lstrlen(cp);
lstrcpy( cp, subcat );

return s;
}

/***********************************************************************
*                               EXTCODE                                *
***********************************************************************/
static BOOLEAN extcode( TCHAR * category, TCHAR * subcategory, TCHAR * mycode )
{

if ( lstrlen(mycode) >= DOWNCAT_NUMBER_LEN )
    {
    copychars( category, mycode, DOWNCAT_NUMBER_LEN );
    mycode += DOWNCAT_NUMBER_LEN;
    }
else
    {
    copychars( category, NO_DOWNCAT_NUMBER, DOWNCAT_NUMBER_LEN );
    }

*( category+DOWNCAT_NUMBER_LEN) = NullChar;


if ( lstrlen(mycode) >= DOWNCAT_NUMBER_LEN )
    copychars( subcategory, mycode, DOWNCAT_NUMBER_LEN );
else
    copychars( subcategory, NO_DOWNCAT_NUMBER, DOWNCAT_NUMBER_LEN );
*( subcategory+DOWNCAT_NUMBER_LEN) = NullChar;

return TRUE;
}

/***********************************************************************
*                            FILL_CATBOXES                             *
***********************************************************************/
static void fill_catboxes()
{
LISTBOX_CLASS lb;
TCHAR mycode[CODE_LEN+1];
TCHAR cat[DOWNCAT_NUMBER_LEN+1];
TCHAR subcat[DOWNCAT_NUMBER_LEN+1];

get_text( mycode, EditDownWindow, CODE_EDITBOX, CODE_LEN );
extcode( cat, subcat, mycode );

Category.find( cat );
lb.init( EditDownWindow, CAT_COMBOBOX );
lb.setcursel( Category.name() );

lb.init( EditDownWindow, SUB_CAT_COMBOBOX );
lb.empty();
SubCategory.rewind();
while ( SubCategory.next() )
    {
    if ( lstrcmp(SubCategory.cat(), cat) == 0 )
        lb.add( SubCategory.name() );
    }

if ( SubCategory.find(cat, subcat) )
    lb.setcursel( SubCategory.name() );
}

/***********************************************************************
*                    EDITDOWN_CLASS::EDITDOWN_CLASS                    *
***********************************************************************/
EDITDOWN_CLASS::EDITDOWN_CLASS()
{
lstrcpy( category,    DOWNCAT_SYSTEM_CAT );
lstrcpy( subcategory, DOWNCAT_UP_SUBCAT  );
part.null();
worker.null();
}

/***********************************************************************
*                    EDITDOWN_CLASS::READ_EDITBOXES                    *
***********************************************************************/
BOOLEAN EDITDOWN_CLASS::read_editboxes()
{
SYSTEMTIME st;
LISTBOX_CLASS lb;
TCHAR mydate[ALPHADATE_LEN+1];
TCHAR mytime[ALPHATIME_LEN+1];
TCHAR mycode[CODE_LEN+1];

init_systemtime_struct( st );

if ( get_text(mydate, EditDownWindow, DATE_EDITBOX, ALPHADATE_LEN) )
    extmdy( st, mydate );

if ( get_text(mytime, EditDownWindow, TIME_EDITBOX, ALPHATIME_LEN) )
    exthms( st, mytime );

set( st );

get_text(mycode, EditDownWindow, CODE_EDITBOX, CODE_LEN );
extcode( category, subcategory, mycode );

lb.init( EditDownWindow, PART_COMBOBOX );
part = lb.selected_text();

lb.init( EditDownWindow, OPERATOR_COMBOBOX );
worker = lb.selected_text();

return TRUE;
}

/***********************************************************************
*                    EDITDOWN_CLASS::FILL_EDITBOXES                    *
***********************************************************************/
void EDITDOWN_CLASS::fill_editboxes()
{
LISTBOX_CLASS lb;
SYSTEMTIME    st;

st = system_time();

OriginalTime.set( st );

set_text( EditDownWindow, DATE_EDITBOX, alphadate(st) );
set_text( EditDownWindow, TIME_EDITBOX, alphatime(st) );
set_text( EditDownWindow, CODE_EDITBOX, code(category, subcategory) );

fill_catboxes();

lb.init( EditDownWindow, PART_COMBOBOX );
if ( !part.isempty() )
    lb.setcursel( part.text() );

lb.init( EditDownWindow, OPERATOR_COMBOBOX );
if ( !worker.isempty() )
    lb.setcursel( worker.text() );
}

/***********************************************************************
*                         EDITDOWN_CLASS::READ                         *
***********************************************************************/
BOOLEAN EDITDOWN_CLASS::read( DB_TABLE & t )
{
SYSTEMTIME st;

init_systemtime_struct( st );

t.get_date( st, DOWNTIME_DATE_OFFSET );
t.get_time( st, DOWNTIME_TIME_OFFSET );

set( st );

t.get_alpha( category,    DOWNTIME_CAT_OFFSET,       DOWNCAT_NUMBER_LEN );
t.get_alpha( subcategory, DOWNTIME_SUBCAT_OFFSET,    DOWNCAT_NUMBER_LEN );
t.get_alpha( worker,      DOWNTIME_OPERATOR_OFFSET,  OPERATOR_NUMBER_LEN );
t.get_alpha( part,        DOWNTIME_PART_NAME_OFFSET, DOWNTIME_PART_NAME_LEN );
worker.strip();
return TRUE;
}

/***********************************************************************
*                              FILL_DSTAT                              *
***********************************************************************/
void EDITDOWN_CLASS::fill_dstat( DSTAT_CLASS & d )
{
d.set_cat( category );
d.set_subcat( subcategory );
d.set_time( system_time() );
}

/***********************************************************************
*                         EDITDOWN_CLASS::READ                         *
***********************************************************************/
BOOLEAN EDITDOWN_CLASS::read( TCHAR * lb_sorc_line )
{
TCHAR      * cp;
STRING_CLASS s;
SYSTEMTIME   st;

s = lb_sorc_line;

init();

cp = s.text();

/*
----
Date
---- */
replace_tab_with_null( cp );
init_systemtime_struct( st );
extmdy( st, cp );

/*
----
Time
---- */
cp = nextstring( cp );
replace_tab_with_null( cp );
exthms( st, cp );

set( st );

/*
--------
Category
-------- */
cp = nextstring( cp );
replace_tab_with_null( cp );
copychars( category, cp, DOWNCAT_NUMBER_LEN );
*(category+DOWNCAT_NUMBER_LEN) = NullChar;

cp += DOWNCAT_NUMBER_LEN;

/*
-----------
Subcategory
----------- */
copychars( subcategory, cp, DOWNCAT_NUMBER_LEN );
*(subcategory+DOWNCAT_NUMBER_LEN) = NullChar;

/*
----------------------
Skip the category name
---------------------- */
cp = nextstring( cp );
replace_tab_with_null( cp );

/*
-------------------------
Skip the subcategory name
------------------------- */
cp = nextstring( cp );
replace_tab_with_null( cp );

/*
-------------
Operator Name
------------- */
cp = nextstring( cp );
replace_tab_with_null( cp );
worker = cp;
worker.strip();

/*
---------
Part Name
--------- */
cp = nextstring( cp );
part = cp;

return TRUE;
}

/***********************************************************************
*                        EDITDOWN_CLASS::LBLINE                        *
***********************************************************************/
TCHAR * EDITDOWN_CLASS::lbline()
{
static STRING_CLASS buf;

buf = alphadate( system_time() );
buf += TabChar;
buf.cat_w_char( alphatime(system_time()), TabChar );
buf += category;
buf.cat_w_char( subcategory,   TabChar );

Category.find( category );
buf.cat_w_char( Category.name(), TabChar );

SubCategory.find( category, subcategory );
buf.cat_w_char( SubCategory.name(), TabChar );

buf.cat_w_char( worker, TabChar );
buf += part;

return buf.text();
}

/***********************************************************************
*                        EDITDOWN_CLASS::DDELINE                       *
***********************************************************************/
TCHAR * EDITDOWN_CLASS::ddeline()
{
static STRING_CLASS buf;

buf = MyMachine;
buf += TabChar;
buf.cat_w_char( part, TabChar );
buf.cat_w_char( alphadate(system_time()), TabChar );
buf.cat_w_char( alphatime(system_time()), TabChar );
buf.cat_w_char( category, TabChar );
buf.cat_w_char( subcategory, TabChar );

Category.find( category );
buf.cat_w_char( Category.name(), TabChar );

SubCategory.find( category, subcategory );
buf.cat_w_char( SubCategory.name(), TabChar );

buf += worker;

return buf.text();
}

/***********************************************************************
*                             PUT_RECORD                               *
***********************************************************************/
BOOLEAN EDITDOWN_CLASS::put_record()
{
return make_downtime_record( T, system_time(), category, subcategory, worker.text(), part );
}

/***********************************************************************
*                          ENABLE_PURGE_BUTTON                         *
***********************************************************************/
static void enable_purge_button()
{
TCHAR * cp;
int32   level_needed;

cp = get_ini_string( TEXT("visitrak.ini"), TEXT("ButtonLevels"), TEXT("DowntimePurgeButton") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    {
    level_needed = asctoint32( cp );
    if ( CurrentPasswordLevel >= level_needed )
        EnableWindow( GetDlgItem( EditDownWindow,PURGE_BUTTON), TRUE );
    }
}

/***********************************************************************
*                          PASSWORD_LEVEL_CALLBACK                     *
***********************************************************************/
void password_level_callback( SOCKET_MSG * msg )
{
    if ( !msg->data )
        return;

    CurrentPasswordLevel = asctoint32(msg->data);
    enable_purge_button();
}

/***********************************************************************
*                             SET_TAB_STOPS                            *
***********************************************************************/
static void set_tab_stops()
{
LISTBOX_CLASS lb;
lb.init( EditDownWindow, DOWNTIME_LISTBOX );
lb.set_tabs( TIME_STATIC, PART_STATIC );
}

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
static TCHAR * getline( int i )
{
EDITDOWN_CLASS e;

if ( T.goto_record_number(i) )
    {
    T.get_current_record( NO_LOCK );
    e.read( T );
    return e.lbline();
    }

return NotFoundError;
}

/***********************************************************************
*                             FILL_LISTBOX                             *
***********************************************************************/
static void fill_listbox()
{
int i;
int lastline;
LISTBOX_CLASS lb;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
lb.empty();

lastline = TopLine + LinesPerPage - 1;
if ( lastline >= NofLines )
    lastline = NofLines - 1;

i = TopLine;
while ( i <= lastline )
    {
    lb.add( getline(i) );
    i++;
    }

if ( CurrentLine >= TopLine && CurrentLine < (TopLine+LinesPerPage) )
    lb.setcursel( CurrentLine - TopLine );
}

/***********************************************************************
*                            SET_SCROLLBAR                             *
***********************************************************************/
static void set_scrollbar()
{
HWND w;
SCROLLINFO si;

w = GetDlgItem( EditDownWindow, DOWNTIME_SCROLBAR );

si.cbSize = sizeof(si);
si.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;
si.nMin   = 0;
si.nMax   = NofLines - 1;
si.nPage  = LinesPerPage;
si.nPos   = TopLine;

SetScrollInfo( w, SB_CTL, &si, TRUE );
}

/***********************************************************************
*                            SET_SCROLLBAR_POS                         *
***********************************************************************/
static void set_scrollbar_pos()
{
HWND           w;
SCROLLINFO    si;

w = GetDlgItem( EditDownWindow, DOWNTIME_SCROLBAR );

si.cbSize = sizeof(si);
si.fMask  = SIF_POS;
si.nPos   = TopLine;
SetScrollInfo( w, SB_CTL, &si, TRUE );
}

/***********************************************************************
*                             OK_TO_SCROLL                             *
***********************************************************************/
BOOLEAN ok_to_scroll()
{
if ( !HaveChange )
    return TRUE;

if ( st_resource_message_box(TEXT("CONTINUE_SCROLL"), TEXT("SCROLL_CHANGES"), MB_YESNO) == IDYES )
    {
    HaveChange = FALSE;
    IsInsert   = FALSE;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         SCROLL_DOWN_ONE_LINE                         *
***********************************************************************/
void scroll_down_one_line()
{
int i;
int lastline;

if ( !ok_to_scroll() )
    return;

i        = TopLine + LinesPerPage - 1;
lastline = NofLines - 1;

if ( i < lastline )
    {
    TopLine++;
    set_scrollbar_pos();
    fill_listbox();
    }

}

/***********************************************************************
*                          SCROLL_UP_ONE_LINE                          *
***********************************************************************/
void scroll_up_one_line()
{

if ( !ok_to_scroll() )
    return;

if ( TopLine > 0 )
    {
    TopLine--;
    set_scrollbar_pos();
    fill_listbox();
    }

}

/***********************************************************************
*                          SCROLL_UP_ONE_PAGE                          *
***********************************************************************/
void scroll_up_one_page()
{
int i;

if ( !ok_to_scroll() )
    return;

i = TopLine - LinesPerPage;
maxint( i, 0 );
if ( i < TopLine )
    {
    TopLine = i;
    set_scrollbar_pos();
    fill_listbox();
    }
}

/***********************************************************************
*                        SCROLL_DOWN_ONE_PAGE                          *
***********************************************************************/
void scroll_down_one_page()
{
int i;

if ( !ok_to_scroll() )
    return;

i = TopLine + LinesPerPage;
minint( i, (NofLines - LinesPerPage) );
if ( i > TopLine )
    {
    TopLine = i;
    set_scrollbar_pos();
    fill_listbox();
    }

}

/***********************************************************************
*                              DO_SCROLL                               *
***********************************************************************/
static void do_scroll( WPARAM wParam )
{
int scroll_code;
int pos;

scroll_code = (int) LOWORD( wParam );
pos         = (int) HIWORD( wParam );

switch ( scroll_code )
    {
    case SB_LINEDOWN:
        scroll_down_one_line();
        break;

    case SB_LINEUP:
        scroll_up_one_line();
        break;

    case SB_PAGEDOWN:
        scroll_down_one_page();
        break;

    case SB_PAGEUP:
        scroll_up_one_page();
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        TopLine = pos;
        if ( scroll_code == SB_THUMBPOSITION )
            set_scrollbar_pos();
        fill_listbox();
        break;

    }
}

/***********************************************************************
*                           GET_CURRENTLINE                            *
***********************************************************************/
static void get_currentline()
{
int inserted_index;
int i;
LISTBOX_CLASS lb;

/*
---------------------------------------------
If I am sitting on an inserted line, the file
doesn't know anything about the line I am on.
--------------------------------------------- */
if ( IsInsert )
    inserted_index = CurrentLine - TopLine;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
i = lb.selected_index();

if ( i != NO_INDEX )
    {
    CurrentLine = TopLine + i;
    if ( IsInsert && i >= inserted_index )
        CurrentLine--;
    }
}

/***********************************************************************
*                              DO_SELECT                               *
***********************************************************************/
static void do_select()
{
LISTBOX_CLASS  lb;
EDITDOWN_CLASS e;

/*
-----------------------------------------------------------
If I was inserting and the operator didn't save the changes
reload the listbox before doing anything else.
----------------------------------------------------------- */
if ( IsInsert )
    fill_listbox();

lb.init( EditDownWindow, DOWNTIME_LISTBOX );

e.read( lb.selected_text() );

ProgramIsUpdating = TRUE;

e.fill_editboxes();

ProgramIsUpdating = FALSE;

IsInsert   = FALSE;
HaveChange = FALSE;
}

/***********************************************************************
*                         DO_EDITBOX_KEYPRESS                          *
*                                                                      *
*               -1 tells listbox to handle the keypress.               *
*               -2 tells listbox I handled the keypress.               *
*                                                                      *
***********************************************************************/
static int do_editbox_keypress( int id, int current_index )
{
int i;

if ( HaveChange )
    {
    fill_listbox();
    HaveChange = FALSE;
    IsInsert   = FALSE;
    }

switch ( id )
    {
    case VK_DOWN:
        if ( current_index == (LinesPerPage - 1) )
            {
            i = TopLine + current_index;
            if ( i < (NofLines - 1) )
                {
                CurrentLine = i + 1;
                scroll_down_one_line();
                do_select();
                return -2;
                }
            }
        break;

    case VK_UP:
        if ( current_index == 0 )
            {
            if ( TopLine > 0 )
                {
                CurrentLine = TopLine - 1;
                scroll_up_one_line();
                do_select();
                return -2;
                }
            }
        break;

    case VK_PRIOR:
        if ( current_index == 0 )
            {
            if ( TopLine > 0 )
                {
                CurrentLine = TopLine - LinesPerPage;
                maxint( CurrentLine, 0 );
                scroll_up_one_page();
                do_select();
                return -2;
                }
            }

        break;

    case VK_NEXT:
        if ( current_index == (LinesPerPage - 1) )
            {
            i = TopLine + current_index;
            if ( i < (NofLines - 1) )
                {
                CurrentLine = i + LinesPerPage;
                minint( CurrentLine, (NofLines - 1) );
                scroll_down_one_page();
                do_select();
                return -2;
                }
            }
        break;

    }

return -1;
}

/***********************************************************************
*                            PURGE_DOWNTIME                            *
***********************************************************************/
static BOOLEAN purge_downtimes()
{
int32      n;
NAME_CLASS s;
BOOLEAN    status;

status = FALSE;

if ( T.open_status() )
    T.close();

s.get_downtime_dbname( MyComputer, MyMachine );

if ( s.file_exists() )
    {
    if ( T.open(s, DOWNTIME_RECLEN, FL) )
        {
        n = T.nof_recs();
        if ( n > 0 )
            {
            T.goto_record_number( n-1 );
            T.get_current_record( NO_LOCK );
            }
        T.empty();
        if ( n > 0 )
            T.rec_append();
        T.close();
        status = TRUE;
        }
    }

return status;
}

/***********************************************************************
*                             INIT_GLOBALS                             *
***********************************************************************/
static BOOLEAN init_globals()
{
LISTBOX_CLASS lb;
NAME_CLASS    s;

MyMachine.set_text( EditDownWindow, MACHINE_NAME_TEXTBOX );

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
LinesPerPage = lb.visible_row_count();

if ( T.open_status() )
    T.close();

s.get_downtime_dbname( MyComputer, MyMachine );
if ( !s.file_exists() )
    T.create( s );

if ( T.open(s, DOWNTIME_RECLEN, PFL) )
    {
    NofLines = T.nof_recs();
    TopLine = NofLines - LinesPerPage;
    maxint( TopLine, 0 );
    set_scrollbar();
    CurrentLine = NO_INDEX;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        FILL_GLOBAL_LISTBOXES                         *
***********************************************************************/
static void fill_global_listboxes()
{
LISTBOX_CLASS lb;
DB_TABLE t;
STRING_CLASS worker;
NAME_CLASS   s;
TEXT_LIST_CLASS tlist;

lb.init( EditDownWindow, PART_COMBOBOX );
lb.empty();

if ( get_partlist(tlist, MyComputer, MyMachine) )
    {
    tlist.rewind();
    while ( tlist.next() )
        lb.add( tlist.text() );
    }

lb.init( EditDownWindow, OPERATOR_COMBOBOX );
lb.empty();

s.get_operator_dbname();
if ( t.open(s, OPERATOR_RECLEN, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( worker, OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
        worker.strip();
        lb.add( worker.text() );
        }
    t.close();
    }

lb.init( EditDownWindow, CAT_COMBOBOX );
Category.rewind();
while ( Category.next() )
    lb.add( Category.name() );
}

/***********************************************************************
*                           SELECT_LAST_LINE                           *
***********************************************************************/
static void select_last_line()
{
LISTBOX_CLASS  lb;
HWND w;
int  i;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
i = lb.count();
if ( i > 0 )
    {
    i--;
    lb.setcursel( i );
    get_currentline();
    do_select();
    w = GetDlgItem( EditDownWindow, CODE_EDITBOX );
    SetFocus( w );
    eb_select_all( w );
    }

}

/***********************************************************************
*                        REFRESH_LISTBOX_ENTRY                         *
***********************************************************************/
static void refresh_listbox_entry()
{
EDITDOWN_CLASS e;
LISTBOX_CLASS  lb;

if ( ProgramIsUpdating )
    return;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
e.read_editboxes();
lb.replace( e.lbline() );

}

/***********************************************************************
*                           REFRESH_SUBCAT                             *
***********************************************************************/
static void refresh_subcat()
{
LISTBOX_CLASS  lb;
TCHAR mycode[CODE_LEN+1];
TCHAR cat[DOWNCAT_NUMBER_LEN+1];
TCHAR subcat[DOWNCAT_NUMBER_LEN+1];

lb.init( EditDownWindow, SUB_CAT_COMBOBOX );

get_text( mycode, EditDownWindow, CODE_EDITBOX, CODE_LEN );
if ( lstrlen(mycode) < DOWNCAT_NUMBER_LEN )
    return;

extcode( cat, subcat, mycode );

if ( SubCategory.findname( cat, lb.selected_text()) )
    {
    lstrcpy( subcat, SubCategory.subcat() );
    set_text( EditDownWindow, CODE_EDITBOX, code(cat, subcat) );
    }

}

/***********************************************************************
*                             REFRESH_CAT                              *
***********************************************************************/
static void refresh_cat()
{
LISTBOX_CLASS  lb;
TCHAR mycode[CODE_LEN+1];
TCHAR category[DOWNCAT_NUMBER_LEN+1];
TCHAR subcategory[DOWNCAT_NUMBER_LEN+1];

lb.init( EditDownWindow, CAT_COMBOBOX );

get_text( mycode, EditDownWindow, CODE_EDITBOX, CODE_LEN );
if ( lstrlen(mycode) < DOWNCAT_NUMBER_LEN )
    return;

extcode( category, subcategory, mycode );

if ( Category.findname(lb.selected_text()) )
    {
    lstrcpy( category, Category.cat() );
    set_text( EditDownWindow, CODE_EDITBOX, code(category, subcategory) );
    }

}

/***********************************************************************
*                              ADD_RECORD                              *
***********************************************************************/
static void add_record()
{
LISTBOX_CLASS  lb;
EDITDOWN_CLASS e;
int  i;
int  midline;
int  n;
HWND w;
SYSTEMTIME t;
TIME_CLASS oldtime;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
n = lb.count();

/*
-----------------------------------------------------
Don't do anything if the selected line is not showing
----------------------------------------------------- */
if ( n > 0 )
    if ( CurrentLine < TopLine || CurrentLine >= (TopLine+LinesPerPage) )
        return;

if ( n > 0 )
    i = lb.current_index();
else
    i = 0;

if ( n > 0 )
    e.read( lb.selected_text() );

/*
--------------------------------------------------------------
If this is the last record then I am probably adding something
for the current time.
-------------------------------------------------------------- */
if ( n == 0 || CurrentLine == (NofLines - 1) )
    {
    GetLocalTime( &t );
    e.set( t );
    }

if ( n == LinesPerPage )
    {
    /*
    -------------------------------------------
    The listbox is full so I have to delete one
    ------------------------------------------- */
    midline = LinesPerPage / 2;
    if ( i < midline )
        lb.remove( n-1 );
    else
        {
        TopLine++;
        lb.remove( 0 );
        i--;
        }
    }

if ( n > 0 )
    i++;
lb.insert( i, e.lbline() );
lb.setcursel( i );
get_currentline();

oldtime = OriginalTime;
do_select();

/*
-----------------------------------------------------------------
Restore the old time so save_changes will know this is a new time
----------------------------------------------------------------- */
OriginalTime = oldtime;

IsInsert   = TRUE;
HaveChange = TRUE;

w = GetDlgItem( EditDownWindow, CODE_EDITBOX );
SetFocus( w );
eb_select_all( w );
}

/***********************************************************************
*                              SUB_PROC                                *
* Subclass the four edit controls so if the operator presses an up     *
* or down arrow, the current selection in the listbox changes.         *
***********************************************************************/
LRESULT CALLBACK sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  i;
LISTBOX_CLASS lb;

if ( msg == WM_KEYDOWN )
    {
    if ( wParam == VK_UP || wParam == VK_DOWN )
        {
        lb.init( EditDownWindow, DOWNTIME_LISTBOX );
        i = lb.current_index();
        if ( do_editbox_keypress(wParam, i) == -1 )
            {
            if ( wParam == VK_UP && i > 0 )
                i--;
            else if ( wParam == VK_DOWN && i < (lb.count() - 1) )
                i++;
            else
                return 0;

            lb.setcursel( i );
            }

        get_currentline();
        do_select();
        SetFocus( w );
        eb_select_all( w );
        return 0;
        }

    }

i = GetWindowLong( w, GWL_ID ) - DATE_EDITBOX;
return CallWindowProc( OldEditProc[i], w, msg, wParam, lParam );
}

/***********************************************************************
*                          SUB_CLASS_CONTROLS                          *
***********************************************************************/
static void sub_class_controls( HWND parent )
{
int i;
HWND w;

for ( i=0; i<NOF_EDITBOXES; i++ )
    {
    w = GetDlgItem( parent, DATE_EDITBOX+i );
    OldEditProc[i] = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) sub_proc );
    }
}

/***********************************************************************
*                               RECTIME                                *
***********************************************************************/
SYSTEMTIME & rectime( int32 record_number )
{
static SYSTEMTIME st;

init_systemtime_struct( st );

T.goto_record_number( record_number );
T.get_current_record( NO_LOCK );
T.get_date( st, DOWNTIME_DATE_OFFSET );
T.get_time( st, DOWNTIME_TIME_OFFSET );

return st;
}

/***********************************************************************
*                               RESTART                                *
***********************************************************************/
void restart()
{
EDITDOWN_CLASS e;
LISTBOX_CLASS  lb;
BOOLEAN        have_current_line;
int            row_in_listbox;

have_current_line = FALSE;

/*
-----------------------------------------------------
Don't do anything if the selected line is not showing
----------------------------------------------------- */
if ( CurrentLine >= TopLine && CurrentLine < (TopLine+LinesPerPage) )
    {
    lb.init( EditDownWindow, DOWNTIME_LISTBOX );
    row_in_listbox = lb.current_index();
    e.read( lb.selected_text() );
    have_current_line = TRUE;
    }

init_globals();

if ( have_current_line )
    {
    T.reset_search_mode();
    e.put_record();
    if ( T.get_next_key_match(2, NO_LOCK) )
        {
        CurrentLine = T.current_record_number();
        if ( CurrentLine >= row_in_listbox )
            TopLine = CurrentLine - row_in_listbox;
        else
            TopLine = 0;
        }
    }

set_scrollbar_pos();
fill_listbox();
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes()
{
int32   firstrec;
int32   lastrec;
int32   rn;
int32   new_rec_number;

NAME_CLASS     s;
EDITDOWN_CLASS e;
TIME_CLASS     tc;
BOOLEAN        need_insert;
LISTBOX_CLASS  lb;
HCURSOR        old_cursor;
DSTAT_CLASS    d;

if ( !HaveChange )
    {
    st_resource_message_box( TEXT("NO_CHANGES_MADE"), TEXT("NOTHING_TO_SAVE") );
    return;
    }

new_rec_number = NO_INDEX;
lb.init( EditDownWindow, DOWNTIME_LISTBOX );

/*
----------------------------------------------------
Don't do anything if the current line is not showing
---------------------------------------------------- */
if ( CurrentLine < TopLine || CurrentLine >= (TopLine+LinesPerPage) )
    return;

e.read( lb.selected_text() );

if ( IsInsert && e == OriginalTime )
    {
    if ( st_resource_message_box(TEXT("REPLACE_ORIGINAL"), TEXT("SAME_DATE"), MB_YESNO) == IDNO )
    return;
    }

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

if ( !waitfor_shotsave_semaphor(MyComputer, MyMachine) )
    {
    st_resource_message_box( TEXT("CANNOT_COMPLY_STRING"), TEXT("SEMAPHOR_FAIL_STRING") );
    SetCursor( old_cursor );
    return;
    }

/*
---------------------------
Reopen table with full lock
--------------------------- */
T.close();

s.get_downtime_dbname( MyComputer, MyMachine );

if ( T.open(s, DOWNTIME_RECLEN, FL) )
    {

    /*
    -------------------------------------
    Get the first and last record numbers
    ------------------------------------- */
    firstrec = 0;
    lastrec  = T.nof_recs();
    need_insert = FALSE;

    if ( !lastrec )
        {
        new_rec_number = 0;
        need_insert = TRUE;
        }

    if ( lastrec > 0 )
        lastrec--;

    if ( new_rec_number == NO_INDEX )
        {
        tc.set( rectime(firstrec) );
        if ( e <= tc )
            {
            new_rec_number = 0;
            if ( e < tc )
                need_insert = TRUE;
            }
        }

    if ( new_rec_number == NO_INDEX )
        {
        /*
        ---------------------
        Check the last record
        --------------------- */
        tc.set( rectime(lastrec) );
        if ( e > tc )
            {
            new_rec_number = lastrec+1;
            need_insert = TRUE;
            }
        else if ( e == tc )
            new_rec_number = lastrec;

        }

    if ( new_rec_number == NO_INDEX )
        {
        while ( firstrec < lastrec-1 )
            {
            rn = (firstrec + lastrec)/2;
            tc.set( rectime(rn) );

            if ( e == tc )
                {
                new_rec_number = rn;
                break;
                }

            if ( e > tc )
                firstrec = rn;
            else
                lastrec = rn;
            }

        if ( new_rec_number == NO_INDEX )
            {
            new_rec_number = lastrec;
            need_insert = TRUE;
            }
        }


    if ( new_rec_number != NO_INDEX )
        {
        if ( e.put_record() )
            {
            if ( new_rec_number >= (int32) T.nof_recs() )
                T.rec_append();
            else
                {
                T.goto_record_number( new_rec_number );
                if ( need_insert )
                    T.rec_insert();
                else
                    T.rec_update();
                }

            /*
            -------------------------------------------------
            If I made a new record and this was not an insert
            then I need to delete the original.
            ------------------------------------------------- */
            if ( need_insert && !IsInsert )
                {
                e = OriginalTime;
                e.put_record();
                T.reset_search_mode();
                if ( T.get_next_key_match(2, NO_LOCK) )
                    T.rec_delete();
                }

            /*
            ----------------------------------
            Nothing will happen if this is old
            ---------------------------------- */
            e.fill_dstat( d );
            d.put( MyComputer, MyMachine );
            }
        }

    HaveChange = FALSE;
    IsInsert   = FALSE;
    EventClient.poke_data( DOWN_DATA_INDEX, e.ddeline() );
    }
else
    {
    st_resource_message_box( TEXT("CANT_UPDATE"), TEXT("CANNOT_COMPLY_STRING") );
    }

free_shotsave_semaphore();

SetCursor( old_cursor );

restart();
}

/***********************************************************************
*                           GOTO_CURRENTLINE                           *
***********************************************************************/
static void goto_currentline()
{
LISTBOX_CLASS lb;

lb.init( EditDownWindow, DOWNTIME_LISTBOX );
lb.setcursel( CurrentLine - TopLine );
}

/***********************************************************************
*                           POSITION_MYSELF                            *
***********************************************************************/
static void position_myself()
{
RECT mr;
RECT dr;
long delta;

HWND desktop_window;

GetWindowRect( MainWindow, &mr );
dr = mr;

/*
------------------------------------
Get the bounds of the desktop window
------------------------------------ */
desktop_window = GetDesktopWindow();
if ( desktop_window )
    {
    GetClientRect( desktop_window, &dr );
    }

delta = (dr.right - dr.left) - (mr.right - mr.left);
delta /= 2;

mr.right += delta;
mr.left  += delta;

delta = (dr.bottom - dr.top) - (mr.bottom - mr.top);
delta /= 2;

mr.bottom += delta;
mr.top   += delta;

MoveWindow( MainWindow, mr.left, mr.top, mr.right-mr.left, mr.bottom-mr.top, TRUE );
}

/***********************************************************************
*                     SET_EDITDOWN_SETUP_STRINGS                       *
***********************************************************************/
static void set_editdown_setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { DATE_STATIC,         TEXT("DATE_STRING") },
    { TIME_STATIC,         TEXT("TIME_STRING") },
    { CODE_STATIC,         TEXT("CODE") },
    { CAT_STATIC,          TEXT("CATEGORY") },
    { SUB_CAT_STATIC,      TEXT("SUB_CAT") },
    { WORKER_STATIC,       TEXT("OPERATOR") },
    { PART_STATIC,         TEXT("PART_STRING") },
    { DATE2_STATIC,        TEXT("DATE_STRING") },
    { TIME2_STATIC,        TEXT("TIME_STRING") },
    { CODE2_STATIC,        TEXT("CODE") },
    { CAT2_STATIC,         TEXT("CATEGORY") },
    { SUBCAT2_STATIC,      TEXT("SUB_CAT") },
    { PART2_STATIC,        TEXT("PART_STRING") },
    { WORKER2_STATIC,      TEXT("OPERATOR") },
    { SAVE_CHANGES_BUTTON, TEXT("SAVE_CHANGES") },
    { ADD_BUTTON,          TEXT("NEW_STRING") },
    { MACHINE_STATIC,      TEXT("MACHINE_STRING") },
    { PURGE_BUTTON,        TEXT("PURGE_ALL") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    DOWNTIME_LISTBOX,
    DATE_EDITBOX,
    TIME_EDITBOX,
    CODE_EDITBOX,
    CAT_COMBOBOX,
    SUB_CAT_COMBOBOX,
    PART_COMBOBOX,
    OPERATOR_COMBOBOX,
    MACHINE_NAME_TEXTBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, NULL );
}

/***********************************************************************
*                            EDITDOWN_PROC                             *
***********************************************************************/
BOOL CALLBACK editdown_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        EditDownWindow = hWnd;
        sub_class_controls( hWnd );
        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        position_myself();
        set_tab_stops();
        set_editdown_setup_strings( hWnd );
        if ( init_globals() )
            {
            fill_global_listboxes();
            fill_listbox();
            select_last_line();
            }
        SetFocus( GetDlgItem(hWnd,ADD_BUTTON) );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, EDIT_DOWNTIMES_SCREEN_HELP );
        return TRUE;

    case WM_VSCROLL:
        do_scroll( wParam );
        return TRUE;

    case WM_VKEYTOITEM:
        return do_editbox_keypress( id, cmd );

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case SAVE_CHANGES_BUTTON:
                save_changes();
                if ( id == SAVE_CHANGES_BUTTON )
                    return TRUE;
            case IDCANCEL:
                PostMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;

            case PURGE_BUTTON:
                if ( st_resource_message_box(TEXT("CONTINUE"), TEXT("PURGE_ALL_DOWNTIMES"), MB_YESNO) == IDYES )
                    {
                    if ( purge_downtimes() )
                        {
                        init_globals();
                        fill_listbox();
                        select_last_line();
                        }
                    }
                return TRUE;

            case F9_KEY:
            case INSERT_KEY:
            case ADD_BUTTON:
                add_record();
                return TRUE;

            case DOWNTIME_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    if ( IsInsert && st_resource_message_box(TEXT("RETURN_TO_NEW_STRING"), TEXT("NEW_RECORD_NOT_SAVED"), MB_YESNO) == IDYES )
                        {
                        goto_currentline();
                        }
                    else
                        {
                        get_currentline();
                        do_select();
                        }
                    }
                return TRUE;

            case DATE_EDITBOX:
            case TIME_EDITBOX:
            case CODE_EDITBOX:
                if ( cmd == EN_CHANGE )
                    {
                    HaveChange = TRUE;
                    refresh_listbox_entry();
                    if ( id == CODE_EDITBOX )
                        fill_catboxes();
                    }
                return TRUE;

            case CAT_COMBOBOX:
                if ( cmd == CBN_SELCHANGE )
                    {
                    HaveChange = TRUE;
                    refresh_cat();
                    }
                return TRUE;

            case SUB_CAT_COMBOBOX:
                if ( cmd == CBN_SELCHANGE )
                    {
                    HaveChange = TRUE;
                    refresh_subcat();
                    }
                return TRUE;

            case PART_COMBOBOX:
            case OPERATOR_COMBOBOX:
                if ( cmd == CBN_SELCHANGE )
                    {
                    HaveChange = TRUE;
                    refresh_listbox_entry();
                    }
                return TRUE;

            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_COMMAND:
        switch (id)
            {
            case IDCANCEL:
                PostMessage( hWnd, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_SETFOCUS:
        if ( EditDownWindow )
            SetFocus( GetDlgItem(EditDownWindow, DOWNTIME_LISTBOX) );
        return 0;

    case WM_DBINIT:
        EventClient.register_for_event( PW_LEVEL_INDEX, password_level_callback );
        EventClient.request_data_item( PW_LEVEL_INDEX );
        return 0;

    case WM_EV_SHUTDOWN:
        PostMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                       GET_COMPUTER_AND_MACHINE                       *
***********************************************************************/
static void get_computer_and_machine()
{
STRING_CLASS machine;
STRING_CLASS s;

if ( get_startup_part(machine, s) )
    {
    if ( machine_computer_name( s, machine) )
        {
        MyComputer = s;
        MyMachine  = machine;
        }
    }
}

/**********************************************************************
*                           LOAD_RESOURCES                            *
**********************************************************************/
static void load_resources()
{
INI_CLASS ini;
NAME_CLASS s;

s.get_local_ini_file_name( DisplayIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(CurrentLanguageKey) )
    {
    s = ini.get_string();
    StringTable.set_language( s );
    }
statics_startup();
}

/***********************************************************************
*                         MY_PREVIOUS_INSTANCE                         *
***********************************************************************/
BOOLEAN my_previous_instance()
{
if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static bool init( int cmd_show )
{
WNDCLASS       wc;
COMPUTER_CLASS c;

EditDownWindow = 0;
c.startup();
load_resources();

StringTable.find( MyWindowTitle, TEXT("EDIT_DOWNTIMES") );
if ( my_previous_instance() )
    return false;

get_computer_and_machine();
setup_service_log( ServiceLog );
start_winsock();
EventClient.start();
Category.read();
SubCategory.read();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MAKEINTRESOURCE(EDITDOWN_ICON) );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    0, 0,                             // X,y
    225, 225,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

EditDownWindow  = CreateDialog(
    MainInstance,
    TEXT("EDITDOWN_DIALOG"),
    MainWindow,
    (DLGPROC) editdown_proc );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
return true;
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown()
{
TCHAR * copy;

if ( T.open_status() )
    T.close();

EventClient.unregister_client();
WSACleanup();
if ( NotFoundError )
    {
    copy = NotFoundError;
    NotFoundError = 0;
    delete[] copy;
    }
statics_shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
BOOL status;
MSG   msg;

MainInstance = this_instance;

if ( !init(cmd_show) )
    return 0;

NotFoundError = maketext( StringTable.find(TEXT("NOT_FOUND")) );

AccelHandle = LoadAccelerators( this_instance, AccelName );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( EditDownWindow )
        status = TranslateAccelerator( EditDownWindow, AccelHandle, &msg );

    if ( !status && EditDownWindow )
        status = IsDialogMessage( EditDownWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return msg.wParam;
}
