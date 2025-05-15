#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\textlist.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"

static NAME_CLASS      BaseDir;
static STRING_CLASS    DefaultLanguage( TEXT("ENGLISH") );
static FONT_LIST_CLASS FontList;
static STRING_CLASS    Resources( TEXT("resources") );
static TCHAR           DefaultFontDef[] = TEXT( "-12,0,0,0,400,0,0,0,0,3,2,1,34,Arial" );

const static char CNullChar = '\0';

/**********************************************************************
*                        STRINGTABLE_CLASS                            *
**********************************************************************/
STRINGTABLE_CLASS::STRINGTABLE_CLASS()
{
font_handle = INVALID_FONT_HANDLE;
}

/**********************************************************************
*                        ~STRINGTABLE_CLASS                           *
**********************************************************************/
STRINGTABLE_CLASS::~STRINGTABLE_CLASS()
{
if ( font_handle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( font_handle );
    font_handle = INVALID_FONT_HANDLE;
    }
}

/**********************************************************************
*                           SET_LANGUAGE                              *
**********************************************************************/
void STRINGTABLE_CLASS::set_language( STRING_CLASS & new_language )
{
static TCHAR FONT_FILE_NAME[] = TEXT( "FONT.txt" );

char     * buf = 0;
FILE_CLASS f;
bool       is_ascii;
NAME_CLASS s;

if ( new_language == language )
    return;

#ifdef UNICODE
    is_ascii = false;
#else
    is_ascii = true;
#endif

language = new_language;

if ( font_handle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( font_handle );
    font_handle = INVALID_FONT_HANDLE;
    }

/*
---------------------------------------------------------------------
Set the BaseDir to point to the directory that contains all the files
for this language.
--------------------------------------------------------------------- */
BaseDir.get_exe_dir_file_name( Resources.text() );
BaseDir.cat_path( language );

/*
-----------------------------------------------------------------------
Read the font setup. This is the only thing in the font.txt file.
The file is always unicode and always has a FFFE word at the beginning.
----------------------------------------------------------------------- */
s = BaseDir;
s.cat_path( FONT_FILE_NAME );

if ( !f.read_unicode_file(s) )
    s = DefaultFontDef;

lf.get( s.text() );
font_handle = FontList.get_handle( lf );
}

/**********************************************************************
*                                 =                                   *
**********************************************************************/
void STRINGTABLE_CLASS::operator=( STRINGTABLE_CLASS & sorc )
{
if ( language != sorc.language )
    set_language( sorc.language );
}

/**********************************************************************
*                                find                                 *
*                     Ids must be exactly equal                       *
**********************************************************************/
bool STRINGTABLE_CLASS::find( STRING_CLASS & dest, TCHAR * id_to_find )
{
static char * buf     = 0;
static int    bufsize = 0;

FILE_CLASS   f;

if ( !id_to_find || (*id_to_find == NullChar) )
    {
    dest.null();
    return false;
    }

dest = BaseDir;
dest.cat_path( id_to_find );
dest += STRINGTABLE_NAME_SUFFIX;

f.read_unicode_file( dest );
if ( dest.isempty() )
    dest = id_to_find;

return true;
}

/**********************************************************************
*                                find                                 *
* If the id is found the current index is left pointing to that       *
* record. Ids must be exactly equal                                   *
**********************************************************************/
bool STRINGTABLE_CLASS::find( STRING_CLASS & dest, STRING_CLASS & id_to_find )
{
return find( dest, id_to_find.text() );
}

/**********************************************************************
*                                find                                 *
*                     Ids must be exactly equal                       *
**********************************************************************/
TCHAR * STRINGTABLE_CLASS::find( TCHAR * id_to_find )
{
static STRING_CLASS s( EmptyString );
find( s, id_to_find );
return s.text();
}