#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\plookup.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\verrors.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

extern HWND    ProgressWindow;
extern BOOLEAN HaveProgressCancel;

/***********************************************************************
*                            GET_FIRST_SHOT                            *
***********************************************************************/
static int32 get_first_shot( DB_TABLE & g, BOOL have_graph, DB_TABLE & p, BOOL have_parameter )
{
int32 n;
int32 graph_shot_number;
int32 parameter_shot_number;
BOOL  move_forward;

n            = RenumberStartShot;
move_forward = TRUE;

if ( have_graph )
    {
    if ( g.get_long(GRAPHLST_SHOT_NUMBER_OFFSET) < n )
        move_forward = FALSE;
    }

if ( have_parameter )
    {
    if ( p.get_long(SHOTPARM_SHOT_NUMBER_OFFSET) < n )
        move_forward = FALSE;
    }

if ( move_forward )
    return n;

/*
------------------------------------------------------
If I am going to move backwards, I need to count shots
------------------------------------------------------ */
while ( have_graph || have_parameter )
    {
    if ( HaveProgressCancel )
        break;

    if ( have_graph )
        graph_shot_number = g.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );

    if ( have_parameter )
        parameter_shot_number = p.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );

    if ( have_graph )
        {
        if ( !have_parameter || graph_shot_number <= parameter_shot_number )
            have_graph = g.get_next_record( NO_LOCK );
        }

    if ( have_parameter )
        {
        if ( !have_graph || parameter_shot_number <= graph_shot_number )
            have_parameter = p.get_next_record( NO_LOCK );
        }

    n++;
    }

return n-1;
}

/***********************************************************************
*                             CHECK_PLOOKUP                            *
***********************************************************************/
void check_plookup( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part, int32 new_shot_number )
{
PLOOKUP_CLASS p;

if ( new_shot_number > p.get_last_shot_number(computer, machine, part) )
    {
    p.set_last_shot_number( new_shot_number );
    p.put();
    }
}

/***********************************************************************
*                            RENUMBER_SHOTS                            *
***********************************************************************/
void renumber_shots( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
NAME_CLASS dest;
NAME_CLASS sorc;
DB_TABLE g;
DB_TABLE p;

int32    largest_shot_number;
int32    this_shot_number;
int32    parameter_shot_number;
int32    graph_shot_number;

HWND     file_tbox;

BOOL     have_graph;
BOOL     have_parameter;
BOOL     move_forward;
BOOL     need_update;
bool     file_displayed;
BOOLEAN  is_ft1;

if ( !waitfor_shotsave_semaphor(computer, machine) )
    {
    resource_message_box( MainInstance, CANT_DO_THAT_STRING, SEMAPHOR_FAIL_STRING );
    return;
    }

file_tbox = GetDlgItem( ProgressWindow, SAVING_FILE_TEXT_BOX );

set_text( file_tbox, EmptyString );

largest_shot_number = 0;
have_graph = FALSE;
sorc.get_graphlst_dbname( computer, machine, part );
if ( g.open(sorc, GRAPHLST_RECLEN, FL) )
    have_graph = g.get_next_record( NO_LOCK );

have_parameter = FALSE;
sorc.get_shotparm_dbname( computer, machine, part );
if ( p.open(sorc, SHOTPARM_RECLEN, FL) )
    have_parameter = p.get_next_record( NO_LOCK );

move_forward = TRUE;
this_shot_number = get_first_shot( g, have_graph, p, have_parameter );
if ( this_shot_number != RenumberStartShot )
    {
    move_forward = FALSE;
    if ( have_graph )
        {
        g.reset_search_mode();
        have_graph = g.get_prev_record( NO_LOCK );
        }
    if ( have_parameter )
        {
        p.reset_search_mode();
        have_parameter = p.get_prev_record( NO_LOCK );
        }
    }

while ( have_graph || have_parameter )
    {
    if ( HaveProgressCancel )
        break;

    if ( have_graph )
        graph_shot_number = g.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );

    if ( have_parameter )
        parameter_shot_number = p.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );

    file_displayed = false;

    if ( have_graph )
        {
        need_update = TRUE;
        if ( have_parameter )
            {
            if ( move_forward )
                {
                /*
                -------------------------------------------
                Skip the update if the parameter is smaller
                ------------------------------------------- */
                if ( graph_shot_number > parameter_shot_number )
                    need_update = FALSE;
                }
            else
                {
                /*
                --------------------------------------------
                Skip the update if the parameter is bigger )
                -------------------------------------------- */
                if ( graph_shot_number < parameter_shot_number )
                    need_update = FALSE;
                }
            }

        if ( need_update )
            {
            if ( graph_shot_number != this_shot_number )
                {
                set_text( file_tbox, int32toasc(graph_shot_number) );
                file_displayed = true;

                /*
                -------------------------
                Update the graphlst entry
                ------------------------- */
                g.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, this_shot_number, SHOT_LEN );
                g.rec_update();

                if ( this_shot_number > largest_shot_number )
                    largest_shot_number = this_shot_number;

                /*
                ------------------
                Rename the profile
                ------------------ */
                is_ft1 = TRUE;
                sorc.get_profile_name( computer, machine, part, graph_shot_number );
                dest.get_profile_name( computer, machine, part, this_shot_number );
                if ( !sorc.file_exists() )
                    {
                    is_ft1 = FALSE;
                    sorc.get_profile_ii_name( computer, machine, part, graph_shot_number );
                    dest.get_profile_ii_name( computer, machine, part, this_shot_number );
                    }
                if ( sorc.file_exists() )
                    {
                    /*
                    -----------------------------------------
                    Change the internal number in the profile
                    ----------------------------------------- */
                    if ( sorc.moveto(dest) && is_ft1 )
                        change_shot_number_in_profile( dest, this_shot_number );
                    }
                }

            if ( move_forward )
                have_graph = g.get_next_record( NO_LOCK );
            else
                have_graph = g.get_prev_record( NO_LOCK );
            }
        }

    if ( have_parameter )
        {
        need_update = TRUE;
        if ( have_graph )
            {
            if ( move_forward )
                {
                /*
                ---------------------------------------
                Skip the update if the graph is smaller
                --------------------------------------- */
                if ( graph_shot_number < parameter_shot_number )
                    need_update = FALSE;
                }
            else
                {
                /*
                --------------------------------------
                Skip the update if the graph is bigger
                -------------------------------------- */
                if ( graph_shot_number > parameter_shot_number )
                    need_update = FALSE;
                }
            }

        if ( need_update )
            {
            if ( parameter_shot_number != this_shot_number )
                {
                if ( !file_displayed )
                    set_text( file_tbox, int32toasc(parameter_shot_number) );

                /*
                --------------------------------------------
                Update the shot number in the shotparm table
                -------------------------------------------- */
                p.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, this_shot_number, SHOT_LEN );
                p.rec_update();

                if ( this_shot_number > largest_shot_number )
                    largest_shot_number = this_shot_number;
                }

            if ( move_forward )
                have_parameter = p.get_next_record( NO_LOCK );
            else
                have_parameter = p.get_prev_record( NO_LOCK );
            }
        }

    if ( move_forward )
        this_shot_number++;
    else
        this_shot_number--;
    }

g.close();
p.close();

check_plookup( computer, machine, part, largest_shot_number );
free_shotsave_semaphore();
}

/***********************************************************************
*                            RENUMBER_DATA                             *
***********************************************************************/
void renumber_data()
{
int               i;
SAVE_LIST_ENTRY * s;
STRING_CLASS      p;

for ( i=0; i<NtoSave; i++ )
    {
    if ( HaveProgressCancel )
        break;

    s = SaveList+i;
    s->part.rewind();

    s->name.machine.set_text( ProgressWindow, SAVING_MACHINE_TEXT_BOX );
    while ( s->part.next() )
        {
        p = s->part.text();
        SetDlgItemText( ProgressWindow, SAVING_PART_TEXT_BOX, p.text() );
        renumber_shots( s->name.computer, s->name.machine, p );
        }
    }
}
