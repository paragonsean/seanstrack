#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"

static TCHAR TEMP_FILE_NAME[] = TEXT( "visimenu.tmp" );
const int32 NO_MENU_ID = 0xffff;

int CommasPerLine = 3;

struct PW_ENTRY
    {
    public:

    int32 id;
    int32 pw_level;
    };

/***********************************************************************
*                        GET_PW_ENTRY_FROM_LINE                        *
***********************************************************************/
void get_pw_entry_from_line( PW_ENTRY * dest,  TCHAR * sorc )
{
TCHAR * cp;
TCHAR * np;

cp = sorc + lstrlen(sorc);
while ( cp > sorc )
    {
    cp--;
    if ( *cp == CommaChar )
        {
        np = cp + 1;
        dest->pw_level = asctoint32( np );
        *cp = NullChar;
        break;
        }
    }

while ( cp > sorc )
    {
    cp--;
    if ( *cp == CommaChar )
        {
        np = cp + 1;
        dest->id = asctoint32( np );
        /*
        ------------------
        Put the comma back
        ------------------ */
        cp += lstrlen( cp );
        *cp = CommaChar;
        break;
        }
    }
}

/***********************************************************************
*                       SET_MENU_PASSWORD_LEVEL                        *
***********************************************************************/
BOOLEAN set_menu_password_level( TCHAR * dest_file_name, int32 id_to_change, int new_password_level  )
{
FILE_CLASS sf;
FILE_CLASS df;
int         nof_commas;
TCHAR    * cp;
TCHAR    * pp;
BOOLEAN    have_new_file;
NAME_CLASS s;
NAME_CLASS t;
PW_ENTRY   mypw;

/*
--------------------------------------------------------------
Make a name for the temporary file in the same dir as the dest
-------------------------------------------------------------- */
t = dest_file_name;
t.remove_ending_filename();
t.cat_path( TEMP_FILE_NAME );

have_new_file = FALSE;
if ( !sf.open_for_read(dest_file_name) )
    return FALSE;

/*
------------------------------------------------------------------------------
The first line is the file is the number of languages. The number of commas is
3 + the number of languages -1.
------------------------------------------------------------------------------ */
s = sf.readline();
nof_commas = 2 + s.int_value();

if ( df.open_for_write(t) )
    {
    /*
    -----------------------------------------------
    Write out the line with the number of languages
    ----------------------------------------------- */
    df.writeline(s);

    while ( TRUE )
        {
        s = sf.readline();
        if ( s.isempty() )
            break;
        if ( !have_new_file )
            {
            if ( s.countchars(CommaChar) == nof_commas )
                {
                cp = s.text();
                get_pw_entry_from_line( &mypw, cp );
                if ( mypw.id == id_to_change )
                    {
                    if ( mypw.pw_level != new_password_level )
                        {
                        pp = find_last_char( CommaChar, cp );
                        if ( pp )
                            {
                            pp++;
                            *pp = NullChar;
                            s += new_password_level;
                            have_new_file = TRUE;
                            }
                        }
                    else
                        {
                        sf.close();
                        df.close();
                        return TRUE;
                        }
                    }
                }
            }
        df.writeline(s);
        }

    df.close();
    }
sf.close();

if ( have_new_file )
    {
    s = dest_file_name;
    s.delete_file();
    t.moveto( s );
    }

return have_new_file;
}

/**********************************************************************
*                              FIXLINE                                *
* remove Unicode Introducer and convert tabs to commas (removing      *
* eol tabs )                                                          *
**********************************************************************/
static void fixline( TCHAR * sorc )
{
static wchar_t UnicodeIntroducer = L'\xFEFF';
TCHAR * s;
TCHAR replacement;

#ifdef UNICODE
if ( *sorc == UnicodeIntroducer )
    remove_char( sorc );
#endif

replacement = NullChar;

s = sorc + lstrlen(sorc);
while ( s > sorc )
    {
    s--;
    if ( *s == TabChar )
        *s = replacement;
    else
        replacement = CommaChar;
    }
}

/***********************************************************************
*                             MENU_UPDATE                              *
*                                                                      *
* The old_file_name is the current file. It was renamed before         *
* restoring the new file from v5exes.                                  *
* The old file will be read and the password level of each id will     *
* be saved in an array. Then the new file will be copied, replacing    *
* the password level of any that existed before.                       *
*                                                                      *
***********************************************************************/
BOOLEAN menu_update( TCHAR * dest_file_name, TCHAR * old_file_name, TCHAR * directory_name )
{
FILE_CLASS sf;
FILE_CLASS df;
int32      i;
int32      n;
int32      language_count;
int32      start_position;
TCHAR    * cp;
BOOLEAN    have_new_file;
STRING_CLASS s;
PW_ENTRY * pwlist;
PW_ENTRY   mypw;
NAME_CLASS old_menu_file;
NAME_CLASS dest_menu_file;
NAME_CLASS temp_menu_file;

old_menu_file = directory_name;
old_menu_file.cat_path( old_file_name );

dest_menu_file = directory_name;
dest_menu_file.cat_path( dest_file_name );

temp_menu_file = directory_name;
temp_menu_file.cat_path( TEMP_FILE_NAME );

/*
---------------------------
Get the number of languages
--------------------------- */
n = 0;
language_count = 0;

sf.open_for_read( old_menu_file );
cp = sf.readline();
fixline( cp );
if ( is_numeric(cp) )
    {
    language_count = asctoint32( cp );
    CommasPerLine = 2 + language_count;
    /*
    -----------------------
    Skip the language lines
    ----------------------- */
    for ( i=0; i<language_count; i++ )
        sf.readline();
    /*
    -------------------------
    Count the number of lines
    ------------------------- */
    start_position = (int32) sf.getpos();
    n = 0;
    while ( TRUE )
        {
        cp = sf.readline();
        if ( !cp )
            break;
        fixline( cp );
        if ( (countchars(CommaChar, cp) == CommasPerLine) && (*cp != ZeroChar) )
            n++;
        }
    if ( n > 0 )
        {
        pwlist = new PW_ENTRY[n];
        if ( !pwlist )
            {
            sf.close();
            return FALSE;
            }
        i = 0;
        sf.setpos( start_position );
        while ( TRUE )
            {
            cp = sf.readline();
            if ( !cp )
                break;
                fixline( cp );
            if ( countchars(CommaChar, cp) == CommasPerLine )
                {
                get_pw_entry_from_line( (pwlist+i), cp );
                i++;
                if ( i >= n )
                    break;
                }
            }
        }
    }
sf.close();

/*
-------------
Nothing to do
------------- */
if ( n == 0 )
    return TRUE;

have_new_file = FALSE;
if ( df.open_for_write(temp_menu_file) )
    {
    sf.open_for_read( dest_menu_file );
    language_count = 0;

    cp = sf.readline();
    fixline( cp );
    df.writeline( cp );
    if ( is_numeric(cp) )
        {
        have_new_file = TRUE;
        language_count = asctoint32( cp );
        CommasPerLine = 2 + language_count;
        /*
        -----------------------
        Skip the language lines
        ----------------------- */
        for ( i=0; i<language_count; i++ )
            {
            cp = sf.readline();
            fixline( cp );
            df.writeline( cp );
            }

        while ( TRUE )
            {
            s = sf.readline();
            if ( s.isempty() )
                break;
            fixline( s.text() );
            cp = s.text();
            if ( (s.countchars(CommaChar) == CommasPerLine) && (*cp != ZeroChar) )
                {
                get_pw_entry_from_line( &mypw, s.text() );
                for ( i=0; i<n; i++ )
                    {
                    if ( mypw.id == pwlist[i].id )
                        {
                        if ( mypw.pw_level != pwlist[i].pw_level )
                            {
                            cp = s.findlast( CommaChar );
                            cp++;
                            *cp = NullChar;
                            s += pwlist[i].pw_level;
                            }
                        break;
                        }
                    }
                }
            df.writeline( s );
            }
        }
    sf.close();
    df.close();

    if ( have_new_file )
        {
        dest_menu_file.delete_file();
        temp_menu_file.moveto( dest_menu_file );
        }
    }
delete[] pwlist;

return TRUE;
}