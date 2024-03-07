#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\machname.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"

static STRING_CLASS EmptyString;
static TCHAR NullChar = TEXT( '\0' );
static TCHAR ResultsString[] = TEXT( "Results" );

/***********************************************************************
*                          MACHINE_NAME_LIST_CLASS                     *
***********************************************************************/
MACHINE_NAME_LIST_CLASS::MACHINE_NAME_LIST_CLASS()
{
current_entry = 0;
}

/***********************************************************************
*                MACHINE_NAME_LIST_CLASS::ADD_COMPUTER                 *
*   TRUE just means the computer dir exists, it doesn't                *
*   mean that there were any machines on the computer.                 *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::add_computer( STRING_CLASS & computer )
{
NAME_CLASS      s;
TEXT_LIST_CLASS t;
MACHINE_NAME_LIST_ENTRY * mne;

if ( computer.isempty() )
    return FALSE;

s.get_data_dir_file_name( computer, 0 );
if ( !s.directory_exists() )
    return FALSE;

t.get_directory_list( s.text() );
t.rewind();
while ( t.next() )
    {
    if ( strings_are_equal(ResultsString, t.text()) )
        continue;

    mne = new MACHINE_NAME_LIST_ENTRY;
    if ( mne )
        {
        mne->computer = computer;
        mne->machine  = t.text();
        s.get_machset_csvname( mne->computer, mne->machine );
        if ( s.file_exists() )
            {
            if ( mlist.append( mne ) )
                mne = 0;
            }
        }
    if ( mne )
        delete mne;
    }

return TRUE;
}

/***********************************************************************
*                    MACHINE_NAME_LIST_CLASS::EMPTY                    *
***********************************************************************/
void MACHINE_NAME_LIST_CLASS::empty()
{
MACHINE_NAME_LIST_ENTRY * mne;

current_entry = 0;
mlist.rewind();
while ( TRUE )
    {
    mne = (MACHINE_NAME_LIST_ENTRY *) mlist.next();
    if ( !mne )
        break;
    delete mne;
    }

mlist.remove_all();
}

/***********************************************************************
*                   MACHINE_NAME_LIST_CLASS::REMOVE                    *
***********************************************************************/
void MACHINE_NAME_LIST_CLASS::remove()
{
if ( current_entry )
    {
    delete current_entry;
    current_entry = 0;
    mlist.remove();
    }
}

/***********************************************************************
*                    MACHINE_NAME_LIST_CLASS::NEXT                     *
***********************************************************************/
BOOLEAN  MACHINE_NAME_LIST_CLASS::next()
{
current_entry = (MACHINE_NAME_LIST_ENTRY *) mlist.next();

if ( current_entry )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                     MACHINE_NAME_LIST_CLASS::ADD                     *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::add( MACHINE_NAME_LIST_ENTRY * mne, BITSETYPE sort_type )
{
short computer_compare_status;
short machine_compare_status;

if ( sort_type != NO_SORT )
    {
    rewind();
    while ( next() )
        {
        /*
        ----------------------------------------------------------
        Make the computers equal unless I am sorting by them first
        ---------------------------------------------------------- */
        computer_compare_status = 0;
        if ( sort_type == SORT_BY_COMPUTER )
            computer_compare_status = mne->computer.compare( current_entry->computer );

        machine_compare_status = mne->machine.compare( current_entry->machine );

        if ( computer_compare_status < 0 || (computer_compare_status == 0 && machine_compare_status < 0) )
            {
            if ( mlist.insert(mne) )
                {
                return TRUE;
                }
            else
                {
                delete mne;
                return FALSE;
                }
            }
        }
    }

/*
------------------------------------------------------------
If I didn't have to insert it, append to the end of the list
------------------------------------------------------------ */
if ( mlist.append( mne ) )
    return TRUE;

delete mne;
return FALSE;
}

/***********************************************************************
*                     MACHINE_NAME_LIST_CLASS::ADD                     *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::add( STRING_CLASS & computer, STRING_CLASS & machine, BITSETYPE sort_type )
{
MACHINE_NAME_LIST_ENTRY * mne;

mne = new MACHINE_NAME_LIST_ENTRY;
if ( !mne )
    return FALSE;

mne->computer = computer;
mne->machine  = machine;

return add( mne, sort_type );
}

/***********************************************************************
*                       MACHINE_NAME_LIST_CLASS                        *
*                               ACQUIRE                                *
*          Empty the sorc, transfering each entry to my list.          *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::acquire( MACHINE_NAME_LIST_CLASS & sorc, BITSETYPE sort_type )
{
MACHINE_NAME_LIST_ENTRY * mne;
BOOLEAN status;

status = TRUE;
sorc.current_entry = 0;
sorc.mlist.rewind();
while ( TRUE )
    {
    mne = (MACHINE_NAME_LIST_ENTRY *) sorc.mlist.next();
    if ( !mne )
        break;
    if ( !add(mne, sort_type) )
        status = FALSE;
    }

sorc.mlist.remove_all();
return status;
}

/***********************************************************************
*                    MACHINE_NAME_LIST_CLASS::FIND                     *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::find( TCHAR * name_to_find )
{
if ( !name_to_find )
    return FALSE;

if ( *name_to_find == NullChar )
    return FALSE;

mlist.rewind();

while ( TRUE )
    {
    current_entry = (MACHINE_NAME_LIST_ENTRY *) mlist.next();
    if ( !current_entry )
        break;
    if ( current_entry->machine == name_to_find )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    MACHINE_NAME_LIST_CLASS::FIND                     *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::find( STRING_CLASS & name_to_find )
{
if ( name_to_find.isempty() )
    return FALSE;

return find( name_to_find.text() );
}

/***********************************************************************
*                        MACHINE_NAME_LIST_CLASS                       *
                            GET_COMPUTER_NAME                          *
***********************************************************************/
BOOLEAN MACHINE_NAME_LIST_CLASS::get_computer_name( STRING_CLASS & destco, STRING_CLASS & sorcma )
{
if ( find(sorcma.text()) )
    {
    destco = current_entry->computer;
    return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                     MACHINE_NAME_LIST_CLASS::NAME                    *
***********************************************************************/
STRING_CLASS & MACHINE_NAME_LIST_CLASS::name()
{
if ( current_entry )
    return current_entry->machine;

return EmptyString;
}

/***********************************************************************
*                MACHINE_NAME_LIST_CLASS::COMPUTER_NAME                *
***********************************************************************/
STRING_CLASS & MACHINE_NAME_LIST_CLASS::computer_name()
{
if ( current_entry )
    return current_entry->computer;

return EmptyString;
}

/***********************************************************************
*                         ~MACHINE_NAME_LIST_CLASS                     *
***********************************************************************/
MACHINE_NAME_LIST_CLASS::~MACHINE_NAME_LIST_CLASS()
{
empty();
}
