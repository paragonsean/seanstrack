#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\subs.h"

static TCHAR Data[]            = TEXT( "data" );
static TCHAR Exes[]            = TEXT( "exes" );
static TCHAR VisiTrakIniFile[] = TEXT( "visitrak.ini" );
static TCHAR ConfigSection[]   = TEXT( "Config" );
static TCHAR ThisComputerKey[] = TEXT( "ThisComputer" );
static TCHAR BackSlashChar     = TEXT( '\\' );
static TCHAR CommaChar         = TEXT( ',' );
static TCHAR CommaString[]     = TEXT( "," );
static TCHAR NullChar          = TEXT( '\0' );

static STRING_CLASS MyComputer;
static STRING_CLASS NoComputer;
static int          NofComputers;
static COMPUTER_ENTRY * First = 0;

/***********************************************************************
*                            COMPUTER_CLASS                            *
***********************************************************************/
COMPUTER_CLASS::COMPUTER_CLASS()
{
c=0;
cx = 0;
search_mode = GET_FIRST_COMPUTER;
}

/***********************************************************************
*                       COMPUTER_CLASS::SHUTDOWN                       *
***********************************************************************/
void COMPUTER_CLASS::shutdown()
{
empty();
}

/***********************************************************************
*                     COMPUTER_CLASS::EMPTY                            *
***********************************************************************/
void COMPUTER_CLASS::empty()
{
COMPUTER_ENTRY * copy;
COMPUTER_ENTRY * ce;

copy         = First;
NofComputers = 0;
First        = 0;

while ( copy )
    {
    ce = copy->next;
    delete copy;
    copy = ce;
    }
}

/***********************************************************************
*                               LOAD                                   *
*  This reloads the computer list.                                     *
*  It does not change my name.                                         *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::load()
{
NAME_CLASS s;
TCHAR    * cp;
FILE_CLASS f;

empty();

if ( MyComputer.isempty() )
    {
    s = exe_directory();
    cp = s.find( Exes );
    if ( cp )
        {
        cp--;
        *cp = NullChar;
        MyComputer = s;
        }
    else
        {
        return FALSE;
        }
    }

s = MyComputer;
s.cat_path( Data );
s.cat_path( COMPUTER_CSV );
if ( s.file_exists() )
    {
    f.open_for_read( s );
    while ( true )
        {
        s = f.readline();
        if ( s.isempty() )
            break;
        add( s.text() );
        }
    f.close();
    }
else
    {
    add( MyComputer );
    }

if ( NofComputers )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                       COMPUTER_CLASS::STARTUP                        *
*                                                                      *
*         The v5 directory is the name of this computer.               *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::startup( STRING_CLASS & my_v5_directory )
{
MyComputer = my_v5_directory;

return load();
}

/***********************************************************************
*                       COMPUTER_CLASS::STARTUP                        *
*                                                                      *
* This can be called if the calling program is located in the \v5\exes *
* directory.                                                           *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::startup()
{
MyComputer.empty();

return load();
}

/***********************************************************************
*                    COMPUTER_CLASS::WHOAMI                            *
***********************************************************************/
STRING_CLASS & COMPUTER_CLASS::whoami()
{
return MyComputer;
}

/***********************************************************************
*                          COMPUTER_CLASS                              *
*                         IS_THIS_COMPUTER                             *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::is_this_computer()
{
if ( c )
    {
    if ( c->name == MyComputer )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        COMPUTER_CLASS::COUNT                         *
***********************************************************************/
int COMPUTER_CLASS::count()
{
return NofComputers;
}

/***********************************************************************
*                         COMPUTER_CLASS::FIND                         *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::find( TCHAR * name_to_find )
{
rewind();
while ( next() )
    {
    if ( c->name == name_to_find )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        COMPUTER_CLASS::REWIND                        *
***********************************************************************/
void COMPUTER_CLASS::rewind()
{
search_mode = GET_FIRST_COMPUTER;
}

/***********************************************************************
*                         COMPUTER_CLASS::NEXT                         *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::next()
{
if ( search_mode == GET_FIRST_COMPUTER )
    {
    search_mode = GET_NEXT_COMPUTER;
    c = First;
    }
else
    {
    c = c->next;
    }

if ( c )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                         COMPUTER_CLASS::NAME                         *
***********************************************************************/
STRING_CLASS & COMPUTER_CLASS::name()
{
if ( c )
    return c->name;

return NoComputer;
}

/***********************************************************************
*                         COMPUTER_CLASS::ADD                          *
*         The new name is now the name of the v5 directory             *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::add( TCHAR * new_name )
{
TCHAR            nc;
COMPUTER_ENTRY * ce;
COMPUTER_ENTRY * nce;
COMPUTER_ENTRY * prev;
BOOLEAN          need_to_insert;

/*
----------------------------------------------------
If I already have an entry for this computer I'm OK.
directory name only.
---------------------------------------------------- */
if ( find(new_name) )
    return TRUE;

nce = new COMPUTER_ENTRY;
if ( nce )
    {
    nce->name   = new_name;
    nce->online = TRUE;
    nc = *new_name;

    ce   = First;
    prev = 0;

    while ( ce )
        {
        need_to_insert = FALSE;

        if ( (*(ce->name.text()) == BackSlashChar) && (nc != BackSlashChar) )
            need_to_insert = TRUE;
        else if ( nce->name < ce->name )
            need_to_insert = TRUE;

        if ( need_to_insert )
            {
            /*
            -----------
            Insert here
            ----------- */
            nce->next = ce;
            if ( prev )
                prev->next = nce;
            else
                First = nce;

            NofComputers++;
            return TRUE;
            }

        prev = ce;
        ce = ce->next;
        }

    /*
    ---------------------
    Append to end of list
    --------------------- */
    nce->next = 0;
    if ( prev )
        prev->next = nce;
    else
        First = nce;

    NofComputers++;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      COMPUTER_CLASS::IS_PRESENT                      *
*                                                                      *
*       Checks to see if I can access this computer directory          *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::is_present()
{
if ( c )
    {
    c->online = c->name.directory_exists();
    return c->online;
    }

return FALSE;
}

/***********************************************************************
*                      COMPUTER_CLASS::IS_PRESENT                      *
*                                                                      *
*       Checks to see if I can access this computer directory          *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::is_present( STRING_CLASS & name )
{
if ( find(name) )
    return is_present();

return FALSE;
}

/***********************************************************************
*                            ONLINE_STATUS                             *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::online_status()
{
if ( c )
    return c->online;
return FALSE;
}

/***********************************************************************
*                            ONLINE_STATUS                             *
*            This doesn't change the current entry pointer.            *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::online_status( STRING_CLASS & name )
{
COMPUTER_ENTRY * ce;

if ( c )
    {
    if ( c->name == name )
        return c->online;
    }

ce = First;
while ( ce )
    {
    if ( ce->name == name )
        return ce->online;
    ce = ce->next;
    }

return FALSE;
}

/***********************************************************************
*                         SET_ONLINE_STATUS                            *
*            This doesn't change the current entry pointer.            *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::set_online_status( BOOLEAN new_status )
{
if ( c )
    {
    c->online = new_status;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         SET_ONLINE_STATUS                            *
*            This doesn't change the current entry pointer.            *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::set_online_status( STRING_CLASS & name, BOOLEAN new_status )
{
COMPUTER_ENTRY * ce;

if ( c )
    {
    if ( c->name == name )
        {
        c->online = new_status;
        return TRUE;
        }
    }

ce = First;
while ( ce )
    {
    if ( ce->name == name )
        {
        ce->online = new_status;
        return TRUE;
        }
    ce = ce->next;
    }

return FALSE;
}

/***********************************************************************
*                         COMPUTER_CLASS::SAVE                         *
***********************************************************************/
BOOLEAN COMPUTER_CLASS::save()
{
NAME_CLASS s;
FILE_CLASS   f;
BOOLEAN      status;

rewind();
status = FALSE;

s.get_data_dir_file_name( COMPUTER_CSV );
f.open_for_write( s.text() );
while ( next() )
    status = f.writeline( c->name.text() );
f.close();

return status;
}
