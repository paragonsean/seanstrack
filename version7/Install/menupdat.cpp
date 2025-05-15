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

class NEW_MENU_ENTRY
    {
    public:

    int32 id_to_follow;
    int32 id;
    TCHAR * s;

    void clear( void );
    NEW_MENU_ENTRY() { id_to_follow = NO_MENU_ID; id = NO_MENU_ID; s = 0; }
    ~NEW_MENU_ENTRY() { clear(); }
    BOOLEAN get( TCHAR * sorc );
    };

/***********************************************************************
*                               MENU_ID                                *
*                  The id is the second from the end.                  *
***********************************************************************/
int32 menu_id( TCHAR * sorc )
{
TCHAR * cp;
int     n;

n = 0;
cp = sorc + lstrlen(sorc);
while ( cp > sorc )
    {
    cp--;
    if ( *cp == CommaChar )
        n++;
    if ( n == 2 )
        {
        cp++;
        return asctoint32( cp );
        }
    }

return NO_MENU_ID;
}

/***********************************************************************
*                            NEW_MENU_ENTRY                            *
*                                CLEAR                                 *
***********************************************************************/
void NEW_MENU_ENTRY::clear( void )
{
id_to_follow = NO_MENU_ID;
id = NO_MENU_ID;
if ( s )
    delete[] s;
s = 0;
}

/***********************************************************************
*                            NEW_MENU_ENTRY                            *
*                                 GET                                  *
***********************************************************************/
BOOLEAN NEW_MENU_ENTRY::get( TCHAR * sorc )
{
TCHAR * cp;

clear();

cp = findchar( CommaChar, sorc );
if ( cp )
    {
    cp++;
    if ( countchars(CommaChar, cp) == CommasPerLine )
        {
        id_to_follow = asctoint32( sorc );
        s = maketext( cp );
        id = menu_id( s );
        }

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        SET_MENU_PASSWORD_LEVEL                       *
*                                                                      *
*  This allows you to change the required level for one entry in       *
*  a menu file. For example:                                           *
*                                                                      *
*  set_menu_password_level( "profile.mnu", 110, 99 );                  *
*                                                                      *
*  This will look in the profile.mnu file. If it finds a menu item     *
*  for id = 110 it will change the required password level to 99.      *
*                                                                      *
***********************************************************************/
BOOLEAN set_menu_password_level( TCHAR * dest_file_name, int32 id_to_change, int new_password_level  )
{
FILE_CLASS sf;
FILE_CLASS df;
int32      id;
int         nof_commas;
TCHAR    * cp;
TCHAR    * pp;
BOOLEAN    have_new_file;
NAME_CLASS s;
NAME_CLASS t;

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
                id = menu_id( cp );
                if ( id == id_to_change )
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

/***********************************************************************
*                             MENU_UPDATE                              *
*                                                                      *
* Each entry has to have a menu id followed by the new line to         *
* be put into the file after the id in question.                       *
*                                                                      *
* e.g. Say you want to add a new multiple print item as follows        *
*                                                                      *
*                  1,&Print Multiple Shots,119,0                       *
*                                                                      *
*      and say you want it to follow the existing entry                *
*                                                                      *
*                        1,&Print,117,0                                *
*                                                                      *
*      in the newmenu.mnu file put                                     *
*                                                                      *
*                 117,1,&Print Multiple Shots,119,0                    *
*                                                                      *
***********************************************************************/
BOOLEAN menu_update( TCHAR * sorc_file_name, TCHAR * dest_file_name )
{
FILE_CLASS sf;
FILE_CLASS df;
int32      this_id;
int32      i;
int32      n;
TCHAR    * cp;
BOOLEAN    have_new_file;
TCHAR    * tempfile;

NEW_MENU_ENTRY * newlist;

/*
---------------------------
Get the number of languages
--------------------------- */
sf.open_for_read( dest_file_name );
cp = sf.readline();
if ( is_numeric(cp) )
    {
    CommasPerLine += asctoint32(cp) - 1;
    }
sf.close();

/*
--------------------------------------------------
Read the new file to see how many lines there are.
-------------------------------------------------- */
n = 0;
sf.open_for_read( sorc_file_name );
while ( TRUE )
    {
    cp = sf.readline();
    if ( !cp )
        break;
    if ( countchars(CommaChar, cp) == (CommasPerLine+1) )
        n++;
    }

sf.close();

/*
-------------
Nothing to do
------------- */
if ( n == 0 )
    return TRUE;

/*
------------------------------
Create an array of new entries
------------------------------ */
newlist = new NEW_MENU_ENTRY[n];
if ( !newlist )
    return FALSE;

/*
---------------------------------
Make a list of the new menu items
--------------------------------- */
sf.open_for_read( sorc_file_name );
for ( i=0; i<n; i++ )
    {
    cp = sf.readline();
    if ( !cp )
        break;

    if ( countchars(CommaChar, cp) == (CommasPerLine+1) )
        {
        if ( !newlist[i].get(cp) )
            {
            n = i;
            break;
            }
        }
    }

sf.close();

/*
-----------------------------------------
Ignore any new entries that already exist
----------------------------------------- */
sf.open_for_read( dest_file_name );
while ( TRUE )
    {
    cp = sf.readline();
    if ( !cp )
        break;
    if ( countchars(CommaChar, cp) == CommasPerLine )
        {
        this_id = menu_id( cp );
        for ( i=0; i<n; i++ )
            {
            if ( this_id == newlist[i].id )
                {
                newlist[i].id_to_follow = NO_MENU_ID;
                }
            }
        }
    }

/*
--------------------------------------------------------------
Make a name for the temporary file in the same dir as the dest
-------------------------------------------------------------- */
tempfile = new TCHAR[MAX_PATH+1];
lstrcpy( tempfile, dest_file_name );
cp = tempfile + lstrlen(tempfile);
cp--;

while ( cp > tempfile )
    {
    if ( *cp == BackSlashChar )
        {
        cp++;
        lstrcpy( cp, TEMP_FILE_NAME );
        break;
        }
    cp--;
    }

have_new_file = FALSE;
sf.rewind();
if ( df.open_for_write(tempfile) )
    {
    while ( TRUE )
        {
        cp = sf.readline();
        if ( !cp )
            break;
        df.writeline( cp );
        if ( countchars(CommaChar, cp) == CommasPerLine )
            {
            this_id = menu_id( cp );
            for ( i=0; i<n; i++ )
                {
                if ( this_id == newlist[i].id_to_follow )
                    {
                    df.writeline( newlist[i].s );
                    /*
                    ----------------------------------------------
                    The next new entry may want to follow this one
                    so update this_id with the new one.
                    ----------------------------------------------  */
                    this_id = newlist[i].id;
                    have_new_file = TRUE;
                    }
                }
            }
        }

    df.close();
    }
sf.close();

if ( have_new_file )
    {
    DeleteFile( dest_file_name );
    MoveFile( tempfile, dest_file_name );
    }

for ( i=0; i<n; i++ )
    newlist[i].clear();

delete[] newlist;

delete[] tempfile;

return TRUE;
}
