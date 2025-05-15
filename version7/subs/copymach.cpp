#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\machine.h"
#include "..\include\setpoint.h"
#include "..\include\plookup.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\verrors.h"

static TCHAR DefaultPartName[] = TEXT( "NEWPART" );

/***********************************************************************
*                            UPDATE_MACHINE                            *
* This updates all the setups for a machine. If the machine being      *
* updated exists the monitoring state is not changed. Otherwise        *
* monitoring is turned off.                                            *
* The results are not copied but the downtime directory is created     *
* if it does not exist as is the downtime.txt file                     *
***********************************************************************/
BOOLEAN update_machine( STRING_CLASS dest_computer, STRING_CLASS dest_machine, STRING_CLASS sorc_computer, STRING_CLASS sorc_machine )
{
bool       need_to_copy;
bool       monitoring_is_on;
DB_TABLE   db;
FILETIME   dt;
FILETIME   st;

NAME_CLASS destdir;
NAME_CLASS d;
NAME_CLASS sorcdir;
NAME_CLASS s;

SETPOINT_CLASS  sp;
TEXT_LIST_CLASS t;
unsigned        u;

sorcdir.get_machine_directory( sorc_computer, sorc_machine );
if ( !sorcdir.directory_exists() )
    return FALSE;

monitoring_is_on = false;
destdir.get_machine_directory( dest_computer, dest_machine );
if ( destdir.directory_exists() )
    {
    /*
    ------------------------------------------------
    I need to see if this machine is being monitored
    ------------------------------------------------ */
    d.get_machset_csvname( dest_computer, dest_machine );
    sp.get_one_value( d, d, MACHSET_MONITOR_FLAGS_INDEX );
    if ( d.uint_value() & MA_MONITORING_ON )
        monitoring_is_on = true;
    }
else
    {
    if ( !destdir.create_directory() )
        return FALSE;
    }

t.get_file_list( sorcdir.text(), StarDotStar );
t.rewind();
while ( t.next() )
    {
    if ( strings_are_equal(t.text(), SEMAPHOR_DB) )
        continue;

    s = sorcdir;
    s.cat_path( t.text() );
    d = destdir;
    d.cat_path( t.text() );

    if ( strings_are_equal(t.text(), MACHSET_CSV) )
        {
        sp.get( s );
        u = sp[MACHSET_MONITOR_FLAGS_INDEX].value.uint_value();
        if ( monitoring_is_on )
            u |= MA_MONITORING_ON;
        else
            u &= ~MA_MONITORING_ON;
        sp[MACHSET_MONITOR_FLAGS_INDEX].value = u;
        sp.put( d );
        }
    else
        {
        need_to_copy = true;
        if ( d.get_last_write_time(dt) )
            {
            if ( s.get_last_write_time(st) )
                {
                if ( CompareFileTime(&st, &dt) < 0 )
                    need_to_copy = false;
                }
            }
        if ( need_to_copy )
            s.copyto( d );
        }
    }

d.get_semaphor_dbname( dest_computer, dest_machine );
db.ensure_existance( d );

d.get_machine_results_directory( dest_computer, dest_machine );
if ( !d.directory_exists() )
    {
    if ( !d.create_directory() )
        return FALSE;
    }

d.get_downtime_dbname( dest_computer, dest_machine );
db.ensure_existance( d );

return TRUE;
}

/***********************************************************************
*                           GET_DEFAULT_PART                           *
* The default part used to be "DEFAULT", which never existed, or the   *
* first part in the list. Now I look for the "NEWPART" and if I don't  *
* find that I use current part and if I don't find that I use the      *
* first part in the list.                                              *
***********************************************************************/
static BOOLEAN get_default_part( STRING_CLASS & dest, MACHINE_CLASS & sorc )
{
NAME_CLASS      s;
SETPOINT_CLASS  sp;
TEXT_LIST_CLASS t;

if ( get_partlist(t, sorc.computer, sorc.name) )
    {
    /*
    ------------
    Default Part
    ------------ */
    dest = DefaultPartName;
    if ( t.find( dest.text()) )
        return TRUE;

    /*
    ------------
    Current Part
    ------------ */
    s.get_machset_csvname( sorc.computer, sorc.name );
    sp.get_one_value( dest, s, MACHSET_CURRENT_PART_INDEX );
    if ( t.find(dest.text()) )
        return TRUE;

    /*
    ----------
    First Part
    ---------- */
    t.rewind();
    if ( t.next() )
        {
        dest = t.text();
        return TRUE;
        }

    dest.null();
    }

return FALSE;
}

/***********************************************************************
*                             COPYMACHINE                              *
***********************************************************************/
BOOLEAN copymachine( STRING_CLASS & computer, STRING_CLASS & machine, MACHINE_CLASS & sorc )
{
DSTAT_CLASS    dstat;
MACHINE_CLASS  m;
NAME_CLASS     d;
NAME_CLASS     s;
STRING_CLASS   part;
PLOOKUP_CLASS  plookup;
SYSTEMTIME     st;

if ( !get_default_part(part, sorc) )
    return FALSE;

/*
----------------------
Copy the machine class
---------------------- */
m = sorc;

/*
------------------
Rename and save it
------------------ */
m.computer     = computer;
m.name         = machine;
m.current_part = part;

/*
-------------------
Turn off monitoring
------------------- */
m.monitor_flags &= ~MA_MONITORING_ON;

m.save();

s.get_ft2_editor_settings_file_name( sorc.computer, sorc.name );
d.get_ft2_editor_settings_file_name( m.computer,    m.name );
if ( s.file_exists() )
    s.copyto( d );

s.get_ft2_global_settings_file_name( sorc.computer, sorc.name );
d.get_ft2_global_settings_file_name( m.computer,    m.name );
if ( s.file_exists() )
    s.copyto( d );

s.get_ft2_ctrl_prog_steps_file_name( sorc.computer, sorc.name );
d.get_ft2_ctrl_prog_steps_file_name( m.computer,    m.name );
if ( s.file_exists() )
    s.copyto( d );

s.get_ft2_machine_settings_file_name( sorc.computer, sorc.name );
d.get_ft2_machine_settings_file_name( m.computer,     m.name );
if ( s.file_exists() )
    s.copyto( d );

/*
-------------
Copy one part
------------- */
copypart( computer, machine, part, sorc.computer, sorc.name, part, COPY_PART_ALL );

/*
----------------------
Create the dstat table
---------------------- */
GetLocalTime( &st );
dstat.set_time( st );
dstat.set_cat( DOWNCAT_SYSTEM_CAT );
dstat.set_subcat( DOWNCAT_PROG_EXIT_SUBCAT );
dstat.put( computer, machine );

return TRUE;
}
