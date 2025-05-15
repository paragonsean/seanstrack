FONT_LIST_CLASS   FontList;
STRINGTABLE_CLASS StringTable;
StringClass       MyTitle;

static TCHAR BupresResourceFile[] = TEXT( "bupres" );
static TCHAR CommonResourceFile[] = TEXT( "common" );
static TCHAR ConfigSection[]      = TEXT( "Config" );
static TCHAR CurrentLanguageKey[] = TEXT( "CurrentLanguage" );
static TCHAR DisplayIniFile[]     = TEXT( "display.ini" );

/***********************************************************************
*                  SET__SETUP_STRINGS                    *
***********************************************************************/
static void set__setup_strings( HWND hWnd )
{

static RESOURCE_TEXT_ENTRY rsn[] = {
    { , TEXT("") },
    { , TEXT("") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    LANGUAGE_LB
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, TEXT("") );
}

/***********************************************************************
*                   SET__SETUP_STRINGS                       *
***********************************************************************/
static void set__setup_strings( HWND hWnd )
{
WINDOW_CLASS w;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { , TEXT("") },
    { , TEXT("") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

update_statics( hWnd, rsn, nof_rsn );

w = hWnd;
w.set_title( StringTable.find(TEXT("")) );
w.refresh();
}

/***********************************************************************
*                    SET__SETUP_STRINGS                     *
***********************************************************************/
static void set__setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { , TEXT("") },
    { , TEXT("") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    LANGUAGE_LB
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
w.set_title( StringTable.find(TEXT("")) );
w.refresh();
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

/*
-----------------------------------------------------
Read my file first so I can override the common names
----------------------------------------------------- */
s.get_stringtable_name( BupresResourceFile );
StringTable.read( s );

s.get_stringtable_name( CommonResourceFile );
StringTable.append( s );

MyTitle = StringTable.find( TEXT("MY_TITLE") );
statics_startup();
}

/***********************************************************************
*                         MY_PREVIOUS_INSTANCE                         *
***********************************************************************/
BOOLEAN my_previous_instance()
{
if ( is_previous_instance(MyClassName, MyTitle) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static bool init( int cmd_show )
{
COMPUTER_CLASS c;
TCHAR        * cp;
WNDCLASS       wc;

c.startup();

load_resources();
if ( my_previous_instance() )
    return false;
...
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown()
{
statics_shutdown();
}