#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\plookup.h"
#include "..\include\shotname.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"
#include "..\include\textlist.h"
#include "..\include\vdbclass.h"
#include "..\include\verrors.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

static const DWORD MS_TO_WAIT         = 200;
static const int32 WAIT_TIMEOUT_COUNT = 10;

static int32 PARAMETER_X_OFFSET = SHOT_LEN + 1 + ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1;
static int32 PROFILE_X_OFFSET   = PARAMETER_X_OFFSET + 2;

extern HWND    ProgressWindow;
extern BOOLEAN HaveProgressCancel;

int32 shot_range( SHOT_RANGE_ENTRY & dest, PART_NAME_ENTRY & sorc );

void st_resource_message_box( TCHAR * msg_id, TCHAR * title_id );
/***********************************************************************
*                            SHOW_PROGRESS                             *
***********************************************************************/
static void show_progress( PART_NAME_ENTRY sorc )
{
sorc.machine.set_text( ProgressWindow, SAVING_MACHINE_TEXT_BOX );
sorc.part.set_text( ProgressWindow, SAVING_PART_TEXT_BOX );
set_text( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
}

/***********************************************************************
*                         SET_LAST_SHOT_NAME                        *
***********************************************************************/
BOOLEAN set_last_shot_name( PART_NAME_ENTRY & pn )
{
BOOLEAN          b;
PLOOKUP_CLASS    plookup;
SHOT_NAME_CLASS  snc;
SHOT_RANGE_ENTRY range;
int              n;
STRING_CLASS     s;

n = shot_range( range, pn );
b = plookup.init( pn.computer, pn.machine, pn.part );
if ( b )
    {
    plookup.get();
    if ( n > 0 )
        {
        plookup.set_last_shot_name( range.newest.name );
        snc.init( pn.computer, pn.machine, pn.part );
        snc.get();
        n = shot_number_from_shot_name( range.newest.name, snc.shot_number_length() );
        }
    else
        {
        n = 0;
        if ( shot_name(s, pn.computer, pn.machine, pn.part, n) )
            plookup.set_last_shot_name( s );
        }
    plookup.set_last_shot_number( n );
    plookup.put();
    }
return FALSE;
}

/***********************************************************************
*                            GET_SHOTBOX_LIST                          *
***********************************************************************/
int32 get_shotbox_list( TEXT_LIST_CLASS & dest, BITSETYPE type )
{

INT   * ip;
INT     i;
int32   n;
int32   count;
int     separator_count;
TCHAR * cp;
TCHAR * s;
TCHAR   separator;
bool    have_parameter_x;
bool    have_profile_x;

n = ShotBox.get_select_list( &ip );
if ( n <= 0 )
    return 0;

count = 0;

for ( i=0; i<n; i++ )
    {
    s = ShotBox.item_text( ip[i] );
    if ( s )
        {
        have_parameter_x = false;
        have_profile_x   = false;
        cp = s + lstrlen(s);
        cp--;
        /*
        --------------------------------------------------------------
        The last three characters are "X X", where the Xs can be blank
        -------------------------------------------------------------- */
        if ( *cp == XChar )
            have_profile_x = true;
        cp--;
        cp--;
        if ( *cp == XChar )
            have_parameter_x = true;
        cp--;
        /*
        ---------------------------------------------------------------------------
        I need to null the separator after the shot name. If the separator is a tab
        I null the next one. If it is a space I skip the one between date and time.
        --------------------------------------------------------------------------- */
        separator = *cp;
        separator_count = 1;
        if ( separator == SpaceChar )
            separator_count++;
        while ( cp > s )
            {
            cp--;
            if ( *cp == separator )
                {
                separator_count--;
                if ( separator_count == 0 )
                    {
                    *cp = NullChar;
                    break;
                    }
                }
            }
        if ( ((type & PARAMETER_DATA) && have_parameter_x ) || ( (type & PROFILE_DATA) && have_profile_x) )
            {
            dest.append( s );
            count++;
            }
        }
    }

delete[] ip;
return count;
}

/***********************************************************************
*                       DELETE_SHOTBOX_SHOTS                           *
***********************************************************************/
static void delete_shotbox_shots( PART_NAME_ENTRY & pn )
{
DB_TABLE   dt;
DB_TABLE   st;

VDB_CLASS  dv;
VDB_CLASS  sv;

NAME_CLASS dest;
NAME_CLASS sorc;

NAME_CLASS fn;
TEXT_LIST_CLASS t;

BOOL     have_new_graphlst;
BOOL     have_new_shotparm;
int32    n;
STRING_CLASS shot_name;

show_progress( pn );

if ( BackupType & PROFILE_DATA )
    {
    n = get_shotbox_list( t, PROFILE_DATA );
    if ( n )
        {
        have_new_graphlst = FALSE;
        t.rewind();
        t.next();
        sorc.get_graphlst_dbname( pn.computer, pn.machine, pn.part );
        dest.get_part_results_dir_file_name( pn.computer, pn.machine, pn.part, TEMP_GRAPHLST_DB );

        if ( dt.create(dest) )
            {
            if ( st.open(sorc, AUTO_RECLEN, PFL) )
                {
                if ( dt.open(dest, st.record_length(), FL) )
                    {
                    have_new_graphlst = TRUE;

                    while( st.get_next_record(NO_LOCK) )
                        {
                        st.get_alpha( shot_name, GRAPHLST_SHOT_NAME_INDEX );
                        if ( shot_name == t.text() )
                            {
                            fix_shotname( shot_name );
                            fn.get_profile_ii_name( pn.computer,pn.machine, pn.part, shot_name );
                            shot_name.set_text( ProgressWindow, SAVING_FILE_TEXT_BOX );
                            if ( !fn.delete_file() )
                                {
                                /*
                                ---------------------------
                                See if this is an FTII file
                                --------------------------- */
                                if (fn.replace( PROFILE_II_SUFFIX, PROFILE_NAME_SUFFIX) )
                                    fn.delete_file();
                                }
                            t.next();
                            }
                        else
                            {
                            dt.copy_rec( st );
                            dt.rec_append();
                            }
                        }
                    dt.close();
                    }

                st.close();
                }

            SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
            }

        /*
        -------------------------------------------
        Free the memory created by get_shotbox_list
        ------------------------------------------- */
        t.empty();

        /*
        ---------------------------------------------------
        Delete the old graphlst file and rename the new one
        --------------------------------------------------- */
        if ( have_new_graphlst )
            {
            sorc.delete_file();
            dest.moveto( sorc );
            }
        }
    }

if ( BackupType & PARAMETER_DATA )
    {
    n = get_shotbox_list( t, PARAMETER_DATA );
    if ( n )
        {
        have_new_shotparm = FALSE;

        sorc.get_shotparm_csvname( pn.computer, pn.machine, pn.part );
        dest.get_part_results_dir_file_name( pn.computer, pn.machine, pn.part, TEMP_SHOTPARM_CSV );

        if ( dv.create(dest) )
            {
            if ( sv.open_for_read(sorc) )
                {
                if ( dv.open_for_rw(dest) )
                    {
                    have_new_shotparm = TRUE;
                    t.rewind();
                    t.next();
                    while( sv.get_next_record() )
                        {
                        shot_name = sv.ra[SHOTPARM_SHOT_NAME_INDEX];
                        if ( shot_name == t.text() )
                            {
                            t.next();
                            }
                        else
                            {
                            dv.copy_rec( sv );
                            dv.rec_append();
                            }
                        }
                    dv.close();
                    }

                sv.close();
                }
            }

        /*
        -------------------------------------------
        Free the memory created by get_shotbox_list
        ------------------------------------------- */
        t.empty();

        /*
        ---------------------------------------------------
        Delete the old shotparm file and rename the new one
        --------------------------------------------------- */
        if ( have_new_shotparm )
            {
            sorc.delete_file();
            dest.moveto( sorc );
            }
        }
    }

set_last_shot_name( pn );
}

/***********************************************************************
*                           DELETE_ALL_SHOTS                           *
***********************************************************************/
static void delete_all_shots( PART_NAME_ENTRY & pn )
{
DB_TABLE     t;
VDB_CLASS    v;
NAME_CLASS   fn;
TCHAR      * cp;
STRING_CLASS shot_name;

if ( BackupType & PROFILE_DATA )
    {
    fn.get_graphlst_dbname( pn.computer, pn.machine, pn.part );
    if ( t.open(fn, AUTO_RECLEN, FL) )
        {
        while( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( shot_name, GRAPHLST_SHOT_NAME_INDEX );
            shot_name.set_text( ProgressWindow, SAVING_FILE_TEXT_BOX );

            fn.get_profile_ii_name( pn.computer,pn.machine, pn.part, shot_name );
            if ( !fn.delete_file() )
                {
                cp = fn.last_char();
                if ( cp )
                    {
                    *cp = OChar;
                    fn.delete_file();
                    }
                }
            }
        t.empty();
        t.close();
        }
    SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
    }

if ( BackupType & PARAMETER_DATA )
    {
    fn.get_shotparm_csvname( pn.computer, pn.machine, pn.part );
    if ( v.open_for_write(fn) )
        v.close();
    }

set_last_shot_name( pn );
}

/***********************************************************************
*                             DELETE_DATA                              *
***********************************************************************/
void delete_data( void )
{

int               i;
SAVE_LIST_ENTRY * s;
PART_NAME_ENTRY   pn;

for ( i=0; i<NtoSave; i++ )
    {
    if ( HaveProgressCancel )
        break;

    s = SaveList+i;
    s->part.rewind();

    pn.computer = s->name.computer;
    pn.machine  = s->name.machine;

    if ( !waitfor_shotsave_semaphor( pn.computer, pn.machine) )
        {
        st_resource_message_box( TEXT("CANNOT_COMPLY_STRING"), TEXT("SEMAPHOR_FAIL") );
        continue;
        }

    if ( NtoSave == 1 && s->part.count() == 1 )
        {
        if ( s->part.next() )
            {
            pn.part = s->part.text();
            delete_shotbox_shots( pn );
            }
        free_shotsave_semaphore();
        return;
        }

    while ( s->part.next() )
        {
        pn.part = s->part.text();
        delete_all_shots( pn );
        }

    free_shotsave_semaphore();
    }
}
