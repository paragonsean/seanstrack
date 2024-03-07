#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\iniclass.h"
#include "..\include\machine.h"
#include "..\include\machname.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"

#include "PartData.h"

extern BOOLEAN IsOfficeWorkstation;

GENERIC_LIST_CLASS MachineList;

static TCHAR ConfigSection[]            = TEXT( "Config" );
static TCHAR LoadNetworkedMachinesKey[] = TEXT( "LoadNetworkedMachines" );
static TCHAR VisiTrakIniFile[]          = TEXT( "visitrak.ini" );

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
static void cleanup()
{
MACHINE_ENTRY * m;

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;
    m->partlist.empty();
    delete m;
    }

MachineList.remove_all();
}

/***********************************************************************
*                       READ_MACHINES_AND_PARTS                        *
***********************************************************************/
BOOLEAN read_machines_and_parts()
{
COMPUTER_CLASS          c;
INI_CLASS               ini;
MACHINE_ENTRY         * m;
MACHINE_CLASS           ma;
MACHINE_NAME_LIST_CLASS mlist;
NAME_CLASS              s;
bool                    this_is_a_monitoring_computer;

cleanup();

/*
--------------------------------------
Reload the list of computers each time
-------------------------------------- */
c.load();
c.rewind();

/*
---------------------------------------
Load the machines at this computer.
to see if this is a monitoring computer
--------------------------------------- */
this_is_a_monitoring_computer = false;
mlist.add_computer( c.whoami() );
if ( mlist.count() > 0 )
    {
    mlist.rewind();
    while ( mlist.next() )
        {
        if ( ma.find(c.whoami(), mlist.name()) )
            {
            if ( ma.monitor_flags & MA_MONITORING_ON )
                {
                this_is_a_monitoring_computer = true;
                break;
                }
            }
        }
    }
else
    {
    IsOfficeWorkstation = TRUE;
    }

/*
------------------------------------------------------------
See if I should override not accessing the network computers
------------------------------------------------------------ */
if ( this_is_a_monitoring_computer )
    {
    s.get_local_ini_file_name( VisiTrakIniFile );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    if ( ini.find(LoadNetworkedMachinesKey) )
        {
        if ( ini.get_boolean() )
            this_is_a_monitoring_computer = false;
        }
    }

/*
------------------------------------------------------------------------------------------------
If this is a monitoring computer then all the other computers in my list are office workstations
and I don't need to see if they are accessable
------------------------------------------------------------------------------------------------ */
if ( !this_is_a_monitoring_computer )
    {
    while ( c.next() )
        {
        if ( (c.name() != c.whoami()) && c.is_present() )
            mlist.add_computer( c.name() );
        }
    }

if ( mlist.count() > 0 )
    {
    mlist.rewind();
    while ( mlist.next() )
        {
        m = new MACHINE_ENTRY;
        if ( !m )
            {
            cleanup();
            return FALSE;
            }
        m->computer = mlist.computer_name();
        m->name     = mlist.name();
        if ( !get_partlist(m->partlist,  m->computer, m->name) )
            {
            delete m;
            continue;
            }
        MachineList.append( m );
        }
    }

return TRUE;
}

/***********************************************************************
*                      CLEANUP_MACHINES_AND_PARTS                      *
***********************************************************************/
void cleanup_machines_and_parts()
{
cleanup();
}

/***********************************************************************
*                           FIND_MACHINE_ENTRY                         *
***********************************************************************/
MACHINE_ENTRY * find_machine_entry( STRING_CLASS & machine )
{
MACHINE_ENTRY * m;

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;

    if ( m->name == machine )
        break;
    }

return m;
}

/***********************************************************************
*                           MACHINE_IS_LOCAL                           *
***********************************************************************/
BOOLEAN machine_is_local( STRING_CLASS & ma )
{
COMPUTER_CLASS  c;
MACHINE_ENTRY * m;

m = find_machine_entry( ma );
if ( m )
    {
    if ( m->computer == c.whoami() )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            MACHINE_COUNT                             *
*             Returns the number of machines at a computer.            *
***********************************************************************/
int machine_count( STRING_CLASS & computer_to_search )
{
int count;
MACHINE_ENTRY * m;

count = 0;

MachineList.rewind();
while ( TRUE )
    {
    m = (MACHINE_ENTRY *) MachineList.next();
    if ( !m )
        break;

    if ( m->computer == computer_to_search )
        count++;
    }

return count;
}

/***********************************************************************
*                           RELOAD_PARTLIST                            *
***********************************************************************/
BOOLEAN reload_partlist( STRING_CLASS & machine )
{

MACHINE_ENTRY * m;

m = find_machine_entry( machine );
if ( !m )
    return FALSE;
m->partlist.empty();
return get_partlist( m->partlist, m->computer, m->name );
}

/***********************************************************************
*                           FIND_PART_ENTRY                            *
***********************************************************************/
BOOLEAN find_part_entry( MACHINE_ENTRY * m, STRING_CLASS & part )
{
m->partlist.rewind();
while ( m->partlist.next() )
    {
    if ( part == m->partlist.text() )
        return TRUE;
    }
return FALSE;
}
