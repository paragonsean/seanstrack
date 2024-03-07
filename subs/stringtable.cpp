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

static TCHAR EmptyStringtableText[] = TEXT( "" );
static TCHAR DefaultLanguage[]      = TEXT( "ENGLISH" );

static FONT_LIST_CLASS FontList;

/**********************************************************************
*                              cleanup                                *
**********************************************************************/
void STRINGTABLE_ENTRY::cleanup()
{
if ( id )
    {
    delete[] id;
    id = 0;
    }
if ( s )
    {
    delete[] s;
    s = 0;
    }
}

/**********************************************************************
*                          STRINGTABLE_ENTRY                          *
**********************************************************************/
STRINGTABLE_ENTRY::STRINGTABLE_ENTRY()
{
id = 0;
s = 0;
}

/**********************************************************************
*                           ~STRINGTABLE_ENTRY                        *
**********************************************************************/
STRINGTABLE_ENTRY::~STRINGTABLE_ENTRY()
{
cleanup();
}

/**********************************************************************
*                                   =                                 *
**********************************************************************/
void STRINGTABLE_ENTRY::operator=( const STRINGTABLE_ENTRY & sorc )
{
cleanup();
if ( sorc.id )
    id = maketext( sorc.id );
if ( sorc.s )
    s = maketext( sorc.s );
}

/**********************************************************************
*                               ACQUIRE                               *
* Move the strings from one entry to another. For internal use only.  *
**********************************************************************/
void STRINGTABLE_ENTRY::acquire( STRINGTABLE_ENTRY & sorc )
{
cleanup();
id = sorc.id;
s  = sorc.s;
sorc.id = 0;
sorc.s  = 0;
}

/**********************************************************************
*                             cleanup                                 *
**********************************************************************/
void STRINGTABLE_CLASS::cleanup()
{
x = -1;
n = 0;
if ( se )
    {
    delete[] se;
    se = 0;
    }
}

/**********************************************************************
*                        STRINGTABLE_CLASS                            *
**********************************************************************/
STRINGTABLE_CLASS::STRINGTABLE_CLASS()
{
font_handle = INVALID_FONT_HANDLE;
delimiter = SemiColonChar;
x  = -1;
n  = 0;
se = 0;
language = DefaultLanguage;
}

/**********************************************************************
*                        ~STRINGTABLE_CLASS                           *
**********************************************************************/
STRINGTABLE_CLASS::~STRINGTABLE_CLASS()
{
cleanup();
}

/**********************************************************************
*                                   =                                 *
**********************************************************************/
void STRINGTABLE_CLASS::operator=( const STRINGTABLE_CLASS & sorc )
{
int i;

cleanup();

delimiter = sorc.delimiter;
if ( sorc.n > 0 )
    {
    se = new STRINGTABLE_ENTRY[sorc.n];
    if ( se )
        {
        for ( i=0; i<sorc.n; i++ )
            se[i] = sorc.se[i];
        n = sorc.n;
        x = sorc.x;
        }
    }
}

/**********************************************************************
*                       STRIP_WHITESPACE_FROM_EOL                     *
**********************************************************************/
static void strip_whitespace_from_eol( TCHAR * sorc )
{
TCHAR * ep;

ep = sorc + lstrlen( sorc );
while ( ep > sorc )
    {
    ep--;
    if ( *ep == SpaceChar || *ep == SemiColonChar || *ep == TabChar )
        *ep = NullChar;
    else
        break;
    }
}

/**********************************************************************
*                               APPEND                                *
**********************************************************************/
bool STRINGTABLE_CLASS::append( STRING_CLASS & path )
{
static wchar_t UnicodeIntroducer = L'\xFEFF';

FILE_CLASS f;
TCHAR    * cp;
DWORD      pos;
int        i;
int        language_index;
int        nof_chars;
int        nof_languages;
int        nof_lines;
STRINGTABLE_ENTRY * nse;

language_index = 0;

if ( !f.open_for_read(path) )
    return false;

delimiter = SemiColonChar;

/*
----------------------------
Read the number of languages
---------------------------- */
cp = f.readline();

#ifdef UNICODE 
if ( *cp == UnicodeIntroducer )
    cp++;
#endif 

nof_languages = asctoint32( cp );

/*
---------------------------------
For each language, read the font.
--------------------------------- */
for ( i=0; i<nof_languages; i++ )
    {
    cp = f.readline();
    strip_whitespace_from_eol( cp );

    /*
    ------------------------------------------------------------
    If this is an excel file the line will be enclosed in quotes
    ------------------------------------------------------------ */
    remove_all_occurances( cp, DoubleQuoteChar );

    if ( replace_char_with_null(cp, CommaChar) )
        {
        if ( language == cp )
            {
            language_index = i;

            /*
            ----------------------------------------------------------------
            Only read the font if this is the first file (read, not append).
            ---------------------------------------------------------------- */
            if ( font_handle == INVALID_FONT_HANDLE )
                {
                cp = nextstring( cp );
    
                /*
                ----------------------------
                Get a handle to the new font
                ---------------------------- */
                lf.get( cp );
                font_handle = FontList.get_handle( lf );
                }
            }
        }
    }

pos = f.getpos();

/*
------------------------------------------------------------------------------------------
Count the number of lines. The delimiter can be either a semicolon or tab.
A good line should have a delimiter after the id and nof_languages-1 delimiters thereafter
------------------------------------------------------------------------------------------ */
nof_lines = 0;
while ( true )
    {
    cp = f.readline();
    if ( !cp )
        break;

    strip_whitespace_from_eol( cp );
    nof_chars = countchars( delimiter, cp );

    /*
    -----------------------------------------------------
    I only have to check the first line for the delimiter
    ----------------------------------------------------- */
    if ( nof_lines == 0 && nof_chars != nof_languages )
        {
        delimiter = TabChar;
        nof_chars = countchars( delimiter, cp );
        }

    if ( nof_chars == nof_languages )
        nof_lines++;        
    }

f.setpos( pos );

if ( nof_lines > 0 )
    {
    /*
    -----------------------------------------------
    Allocate enough entries for the new and the old
    ----------------------------------------------- */
    nof_lines += n;
    nse = new STRINGTABLE_ENTRY[nof_lines];
    if ( nse )
        {
        /*
        -------------------------------------------------
        Move the entrys from the old array to the new one
        ------------------------------------------------- */
        if ( se )
            {
            for ( i=0; i<n; i++ )
                nse[i].acquire( se[i] );
            delete[] se;
            }

        se = nse;

        /*
        ----------------------------------
        Read the new entries from the file
        ---------------------------------- */
        while ( n < nof_lines )
            {
            cp = f.readline();
            if ( !cp )
                break;

            strip_whitespace_from_eol( cp );
            remove_all_occurances( cp, DoubleQuoteChar );
            
            nof_chars = countchars( delimiter, cp );
            
            if ( nof_chars == nof_languages )
                {
                /*
                ---------------------------------
                The first column is the id string
                --------------------------------- */
                replace_char_with_null( cp, delimiter );
                se[n].id = maketext( cp );
                
                /*
                ----------------------------
                Skip to the current language
                ---------------------------- */ 
                for ( i=0; i<=language_index; i++ )
                    {
                    cp = nextstring( cp );
                    replace_char_with_null( cp, delimiter );
                    }
               
                se[n].s = maketext( cp );        
                n++;
                }
            }
        }
    }

f.close(); 
return true;
}

/**********************************************************************
*                                READ                                 *
**********************************************************************/
bool STRINGTABLE_CLASS::read( STRING_CLASS & path )
{
cleanup();
return append( path );
}

/**********************************************************************
*                                find                                 *
* If the id is found the current index is left pointing to that       *
* record. Ids must be exactly equal                                   *
**********************************************************************/
TCHAR * STRINGTABLE_CLASS::find( TCHAR * id_to_find )
{
int i;
TCHAR * s1;
TCHAR * s2;

if ( !id_to_find || (*id_to_find == NullChar) || (*id_to_find == delimiter) )
    return EmptyStringtableText;
    
for ( i=0; i<n; i++ )
    {
    s1 = id_to_find;
    s2 = se[i].id;
    while (true)
        {
        if ( *s1 == delimiter || *s1 == NullChar )
            {
            if ( *s2 == NullChar )
                {
                x = i;
                return se[i].s;
                }
            }
        if ( *s1 != *s2 )
            break;
        s1++;
        s2++;
        }
    }
    
return EmptyStringtableText;
}
/**********************************************************************
*                                find                                 *
* If the id is found the current index is left pointing to that       *
* record. Ids must be exactly equal                                   *
**********************************************************************/
TCHAR * STRINGTABLE_CLASS::find( STRING_CLASS & id_to_find )
{
return find( id_to_find.text() );
}

/**********************************************************************
*                               rewind                                *
**********************************************************************/
void STRINGTABLE_CLASS::rewind()
{
x = -1;
}

/**********************************************************************
*                                 next                                *
**********************************************************************/
bool STRINGTABLE_CLASS::next()
{
if ( x < (n-1) )
    {
    x++;
    return true;
    }
    
return false;
}

/**********************************************************************
*                                  id                                 *
**********************************************************************/
TCHAR * STRINGTABLE_CLASS::id()
{
if ( x >=0 && x < n )
    return se[x].id;

return EmptyStringtableText;    
}

/**********************************************************************
*                                 text                                *
**********************************************************************/
TCHAR * STRINGTABLE_CLASS::text()
{
if ( x >=0 && x < n )
    {
    if ( se[x].s )
        return se[x].s;
    }
    
return EmptyStringtableText;    
}
