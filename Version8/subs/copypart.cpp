#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\machine.h"
#include "..\include\multipart_runlist.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\setpoint.h"
#include "..\include\plookup.h"
#include "..\include\textlist.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\verrors.h"

#include "..\include\ftii.h"
#include "..\include\ftii_file.h"

/***********************************************************************
*                             UPDATE_PART                              *
* The dest may or may not exist. If it exists, only the files that     *
* are newer will be copied. The results will not be copied but the     *
* results directory will be created and the alarmsum, graphlst, and    *
* shotparm files will be created if necessary                          *
***********************************************************************/
BOOLEAN update_part( STRING_CLASS dest_computer, STRING_CLASS dest_machine, STRING_CLASS dest_part, STRING_CLASS sorc_computer, STRING_CLASS sorc_machine, STRING_CLASS sorc_part )
{
bool       need_to_copy;
DB_TABLE   db;
FILETIME   dt;
FILETIME   st;

NAME_CLASS destdir;
NAME_CLASS d;
NAME_CLASS sorcdir;
NAME_CLASS s;

SETPOINT_CLASS  sp;
TEXT_LIST_CLASS t;

sorcdir.get_part_directory( sorc_computer, sorc_machine, sorc_part );
if ( !sorcdir.directory_exists() )
    return FALSE;

destdir.get_part_directory( dest_computer, dest_machine, dest_part );
if ( !destdir.directory_exists() )
    {
    if ( !destdir.create_directory() )
        return FALSE;
    }

t.get_file_list( sorcdir.text(), StarDotStar );
t.rewind();
while ( t.next() )
    {
    s = sorcdir;
    s.cat_path( t.text() );
    d = destdir;
    d.cat_path( t.text() );

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

d.get_part_results_directory( dest_computer, dest_machine, dest_part );
if ( !d.directory_exists() )
    {
    if ( !d.create_directory() )
        return FALSE;
    }

d.get_alarmsum_dbname( dest_computer, dest_machine, dest_part );
db.ensure_existance( d );

d.get_graphlst_dbname( dest_computer, dest_machine, dest_part );
db.ensure_existance( d );

d.get_shotparm_csvname( dest_computer, dest_machine, dest_part );
db.ensure_existance( d );

return TRUE;
}

/***********************************************************************
*                         COPY_PARAMETER_NAMES                         *
***********************************************************************/
static BOOLEAN copy_parameter_names( PART_CLASS & dest, PART_CLASS & sorc )
{
PARAMETER_CLASS d;
PARAMETER_CLASS s;
int32 i;
int32 n;

if ( d.find(dest.computer, dest.machine, dest.name) )
    {
    if ( s.find(sorc.computer, sorc.machine, sorc.name) )
        {
        n = s.count();
        if ( d.count() < n )
            n = d.count();
        for ( i=0; i<n; i++ )
            {
            if ( s.parameter[i].vartype == d.parameter[i].vartype )
                lstrcpy( d.parameter[i].name, s.parameter[i].name );
            }
        d.save();
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                               COPYPART                               *
***********************************************************************/
BOOLEAN copypart( STRING_CLASS & dest_computer, STRING_CLASS & dest_machine, STRING_CLASS & dest_part, STRING_CLASS & sorc_computer, STRING_CLASS & sorc_machine, STRING_CLASS & sorc_part, unsigned flags )
{
static int TSL_INDEX           =  0;
static int MSL_INDEX           =  1;
static int ESV_INDEX           =  2;

static int LS1_INDEX           =  3;           /* Limit Switch Positions */
static int LS2_INDEX           =  4;
static int LS3_INDEX           =  5;
static int LS4_INDEX           =  6;
static int LS5_INDEX           =  7;
static int LS6_INDEX           =  8;

static int LS1_MASK_INDEX      =  9;            /* Limit Switch Wires */
static int LS2_MASK_INDEX      = 10;
static int LS3_MASK_INDEX      = 11;
static int LS4_MASK_INDEX      = 12;
static int LS5_MASK_INDEX      = 13;
static int LS6_MASK_INDEX      = 14;

static int VEL_LOOP_GAIN_INDEX = 15;
static int BREAK_VEL_1_INDEX   = 16;
static int BREAK_VEL_2_INDEX   = 17;
static int BREAK_VEL_3_INDEX   = 18;
static int BREAK_GAIN_1_INDEX  = 19;
static int BREAK_GAIN_2_INDEX  = 20;
static int BREAK_GAIN_3_INDEX  = 21;
static int TIME_INTERVAL_MS_INDEX = 22;
static int BISCUIT_DELAY_MS_INDEX = 23;
static int NOF_TIME_SAMPLES_INDEX = 24;

static int vars[] = { TSL_VN, MSL_VN, ESV_VN,
 LS1_VN, LS2_VN, LS3_VN, LS4_VN, LS5_VN, LS6_VN,
 LS1_MASK_VN, LS2_MASK_VN, LS3_MASK_VN, LS4_MASK_VN, LS5_MASK_VN, LS6_MASK_VN,
 VEL_LOOP_GAIN_VN,
 BREAK_VEL_1_VN,  BREAK_VEL_2_VN,  BREAK_VEL_3_VN,
 BREAK_GAIN_1_VN, BREAK_GAIN_2_VN, BREAK_GAIN_3_VN,
 TIME_INTERVAL_MS_VN, NOF_TIME_SAMPLES_VN,
 BISCUIT_DELAY_MS_VN
 };

const int nof_vars = sizeof(vars)/sizeof(int);

NAME_CLASS     destfile;
NAME_CLASS     sorcfile;
PART_CLASS     p;
PART_CLASS     d;
DB_TABLE       t;
int            i;
BOOLEAN        dest_ftii_exists;
BOOLEAN        need_ftii_updates;
BOOLEAN        part_exists_already;
BOOLEAN        sorc_ftii_exists;
FTII_VAR_ARRAY dva;
FTII_VAR_ARRAY sva;
PLOOKUP_CLASS  plookup;


need_ftii_updates = FALSE;

if ( !p.find(sorc_computer, sorc_machine, sorc_part) )
    return FALSE;

sorc_ftii_exists = FALSE;
sorcfile.get_ft2_part_settings_file_name(sorc_computer, sorc_machine, sorc_part );
if ( sorcfile.file_exists() )
    {
    sorc_ftii_exists = TRUE;
    sva.get( sorcfile );
    }

part_exists_already = part_exists( dest_computer, dest_machine, dest_part );

if ( part_exists_already )
    {
    /*
    ----------------------------------------------------------------------------------
    I may be copying the channels file because it is missing. Copy it now to make sure
    it exists so I don't get a find error.
    ---------------------------------------------------------------------------------- */
    if ( flags & COPY_PART_CHANNELS )
        {
        destfile.get_ftchan_dbname( dest_computer, dest_machine, dest_part );
        if ( !destfile.file_exists() )
            {
            sorcfile.get_ftchan_dbname( sorc_computer, sorc_machine, sorc_part );
            sorcfile.copyto( destfile );
            }
        }

    if ( !d.find(dest_computer, dest_machine, dest_part) )
        return FALSE;

    sorcfile.get_ft2_part_settings_file_name( dest_computer, dest_machine, dest_part );
    if ( sorcfile.file_exists() )
        {
        dest_ftii_exists = TRUE;
        dva.get( sorcfile );
        if ( sorc_ftii_exists )
            need_ftii_updates = TRUE;
        }
    }
else
    {
    /*
    ------------------
    Copy the part data
    ------------------ */
    d = p;
    d.computer = dest_computer;
    d.machine  = dest_machine;
    d.name     = dest_part;

    /*
    -----------------------
    Copy the ftii part data
    ----------------------- */
    if ( sorc_ftii_exists )
        dva = sva;

    /*
    -------------------------
    Create the part directory
    ------------------------- */
    destfile.get_part_dir_file_name( dest_computer, dest_machine, dest_part, 0 );
    destfile.create_directory();

    /*
    --------------------------
    Create a results directory
    -------------------------- */
    destfile.get_part_results_dir_file_name( dest_computer, dest_machine, dest_part, 0 );
    destfile.create_directory();

    sorcfile.get_graphlst_dbname( dest_computer, dest_machine, dest_part );
    t.create( sorcfile );

    sorcfile.get_shotparm_csvname( dest_computer, dest_machine, dest_part );
    t.create( sorcfile );

    sorcfile.get_alarmsum_dbname( dest_computer, dest_machine, dest_part );
    t.create( sorcfile );
    }

if ( flags & COPY_PART_PARAMETERS )
    {
    sorcfile.get_ftanalog_csvname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_ftanalog_csvname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    else
        {
        sorcfile.get_ftanalog_dbname( sorc_computer, sorc_machine, sorc_part );
        destfile.get_ftanalog_dbname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }

    sorcfile.get_ftcalc_csvname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_ftcalc_csvname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }

    sorcfile.get_parmlist_dbname( sorc_computer, sorc_machine, sorc_part );
    destfile.get_parmlist_dbname( dest_computer, dest_machine, dest_part );
    sorcfile.copyto( destfile );

    sorcfile.get_static_text_parameters_csvname( sorc_computer, sorc_machine, sorc_part );
    destfile.get_static_text_parameters_csvname( dest_computer, dest_machine, dest_part );
    sorcfile.copyto( destfile );
    }

if ( flags & COPY_PART_CHANNELS )
    {
    sorcfile.get_ftchan_dbname( sorc_computer, sorc_machine, sorc_part );
    destfile.get_ftchan_dbname( dest_computer, dest_machine, dest_part );
    sorcfile.copyto( destfile );
    if ( part_exists_already )
        {
        d.head_pressure_channel = p.head_pressure_channel;
        d.rod_pressure_channel  = p.rod_pressure_channel;
        for ( i=0; i<MAX_FT2_CHANNELS; i++ )
            d.analog_sensor[i] = p.analog_sensor[i];
        }
    }

if ( flags & COPY_PART_MARKS )
    {
    sorcfile.get_marklist_csvname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_marklist_csvname( dest_computer, dest_machine, dest_part );
        }
    else
        {
        sorcfile.get_marklist_dbname( sorc_computer, sorc_machine, sorc_part );
        destfile.get_marklist_dbname( dest_computer, dest_machine, dest_part );
        }
    sorcfile.copyto( destfile );
    }

if ( flags & COPY_SUBPART_NAMES )
    {
    sorcfile.get_subparts_dbname( sorc_computer, sorc_machine, sorc_part );
    destfile.get_subparts_dbname( dest_computer, dest_machine, dest_part );
    sorcfile.copyto( destfile );
    }

if ( flags & COPY_PART_REFERENCE_TRACE )
    {
    sorcfile.get_master_name(sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_master_name(sorc_computer, sorc_machine, sorc_part );
        sorcfile.copyto( destfile );
        }
    else
        {
        sorcfile.get_master_ii_name( sorc_computer, sorc_machine, sorc_part );
        if ( sorcfile.file_exists() )
            {
            /*
            ------------------------------------------------------------------------------
            I am going to copy a ft2 master trace. Make sure there is no ft1 master trace.
            ------------------------------------------------------------------------------ */
            destfile.get_master_name( dest_computer, dest_machine, dest_part );
            if ( destfile.file_exists() )
                destfile.delete_file();

            destfile.get_master_ii_name( dest_computer, dest_machine, dest_part );
            sorcfile.copyto( destfile );
            }
        }
    }

if ( flags & COPY_PART_SURETRAK_SETUP )
    {
    sorcfile.get_ststeps_dbname( sorc_computer, sorc_machine, sorc_part );
    destfile.get_ststeps_dbname( dest_computer, dest_machine, dest_part );
    sorcfile.copyto( destfile );

    /*
    -----------------------------------------------------
    If the new file (stlimits.dat) exists, copy only that
    ----------------------------------------------------- */
    sorcfile.get_stlimits_datname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_stlimits_datname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    else
        {
        sorcfile.get_stlimits_dbname( sorc_computer, sorc_machine, sorc_part );
        destfile.get_stlimits_dbname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }

    /*
    ----------------------------------------------------------
    Copy the list of general purpose variables if there is one
    ---------------------------------------------------------- */
    sorcfile.get_gpvar_dbname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_gpvar_dbname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    /*
    ---------------------------------------
    Copy the part based suretrak parameters
    --------------------------------------- */
    sorcfile.get_stsetup_csvname(sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_stsetup_csvname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }

    /*
    ------------------------------
    Copy the pressure control data
    ------------------------------ */
    sorcfile.get_stpres_datname(sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_stpres_datname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }

    if ( need_ftii_updates )
        {
        dva = sva[LS1_INDEX];
        dva = sva[LS2_INDEX];
        dva = sva[LS3_INDEX];
        dva = sva[LS4_INDEX];
        dva = sva[LS5_INDEX];
        dva = sva[LS6_INDEX];
        dva = sva[VEL_LOOP_GAIN_INDEX];
        dva = sva[BREAK_VEL_1_INDEX];
        dva = sva[BREAK_VEL_2_INDEX];
        dva = sva[BREAK_VEL_3_INDEX];
        dva = sva[BREAK_GAIN_1_INDEX];
        dva = sva[BREAK_GAIN_2_INDEX];
        dva = sva[BREAK_GAIN_3_INDEX];

        if ( sva.find(ST_ACCEL_1_VN) )
            {
            i = sva.current_index();
            while ( i<sva.count() )
                {
                dva = sva[i];
                i++;
                }
            }
        }
    }

if ( flags & COPY_PART_PRES_CONTROL )
    {
    sorcfile.get_stpres_datname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_stpres_datname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

if ( flags & COPY_PART_WARMUP_PART )
    {
    /*
    ------------------------------------
    Copy the warmup part if there is one
    ------------------------------------ */
    sorcfile.get_warmup_csvname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_warmup_csvname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }

    /*
    ------------------------------------------------------------------------------------------------------
    Copy the multipart_runlist if there is one and I am copying to the same part name on another computer.
    If I am copying to a different part then set the part selection number to NO_PART_SELECTION_NUMBER.
    ------------------------------------------------------------------------------------------------------ */
    if ( (sorc_part != dest_part) || (sorc_computer == dest_computer) )
        d.multipart_runlist_wires.cleanup();
    }

if ( flags & COPY_PART_DISPLAY_SETUP )
    {
    sorcfile.get_part_display_ini_name( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_part_display_ini_name( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }

    sorcfile.get_tc_parameter_list_file_name( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_tc_parameter_list_file_name( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

if ( flags & COPY_PART_CHAXIS_LABEL )
    {
    sorcfile.get_chaxis_label_datname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_chaxis_label_datname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

if ( flags & COPY_PART_PARAM_SORT )
    {
    sorcfile.get_parameter_sort_order_file_name( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_parameter_sort_order_file_name( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

if ( flags & COPY_PART_LS_DESC )
    {
    sorcfile.get_limit_switch_desc_file_name( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_limit_switch_desc_file_name( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }
if ( flags & COPY_PART_LS_POS )
    {
    sorcfile.get_limit_switch_pos_file_name( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_limit_switch_pos_file_name( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }
if ( flags & COPY_PART_LS_WIRES )
    {
    sorcfile.get_limit_switch_wires_file_name( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_limit_switch_wires_file_name( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

/*
----------------------------------------
Copy the calculator setups for this part
---------------------------------------- */
if ( !part_exists_already || (flags & COPY_PART_BASIC) )
    {
    sorcfile.get_calc_file_name( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_calc_file_name( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

if ( flags & COPY_PART_SHOT_NAME )
    {
    sorcfile.get_shot_name_csvname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_shot_name_csvname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    sorcfile.get_external_shot_name_file_csvname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_external_shot_name_file_csvname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

if ( flags & COPY_PART_SHOT_NAME_RESET )
    {
    sorcfile.get_shot_name_reset_csvname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_shot_name_reset_csvname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

/*
-------------------------
Copy the setup sheet file
------------------------- */
if ( !part_exists_already )
    {
    sorcfile.get_setup_sheet_file_name(sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_setup_sheet_file_name( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

if ( part_exists_already )
    {
    if ( flags & COPY_PART_BASIC )
        {
        d.sp[PARTSET_VELOCITY_UNITS_INDEX]   = p.sp[PARTSET_VELOCITY_UNITS_INDEX];
        d.sp[PARTSET_DISTANCE_UNITS_INDEX]   = p.sp[PARTSET_DISTANCE_UNITS_INDEX];
        d.sp[PARTSET_TOTAL_STROKE_LEN_INDEX] = p.sp[PARTSET_TOTAL_STROKE_LEN_INDEX];
        d.sp[PARTSET_MIN_STROKE_LEN_INDEX]   = p.sp[PARTSET_MIN_STROKE_LEN_INDEX];
        d.sp[PARTSET_EOS_VEL_INDEX]          = p.sp[PARTSET_EOS_VEL_INDEX];

        if ( need_ftii_updates )
            {
            dva[TSL_INDEX] = sva[TSL_INDEX];
            dva[MSL_INDEX] = sva[MSL_INDEX];
            dva[ESV_INDEX] = sva[ESV_INDEX];
            }
        }

    if ( flags & COPY_PART_ADVANCED )
        {
        d.sp[PARTSET_BISCUIT_TIME_DELAY_INDEX]   = p.sp[PARTSET_BISCUIT_TIME_DELAY_INDEX];
        d.sp[PARTSET_SLEEVE_FILL_DIST_INDEX]     = p.sp[PARTSET_SLEEVE_FILL_DIST_INDEX];
        d.sp[PARTSET_RUNNER_FILL_DIST_INDEX]     = p.sp[PARTSET_RUNNER_FILL_DIST_INDEX];
        d.sp[PARTSET_MIN_CSFS_VEL_INDEX]         = p.sp[PARTSET_MIN_CSFS_VEL_INDEX];
        d.sp[PARTSET_RISE_CSFS_VEL_INDEX]        = p.sp[PARTSET_RISE_CSFS_VEL_INDEX];
        d.sp[PARTSET_MIN_CSFS_POS_INDEX]         = p.sp[PARTSET_MIN_CSFS_POS_INDEX];
        d.sp[PARTSET_PLUNGER_DIA_INDEX]          = p.sp[PARTSET_PLUNGER_DIA_INDEX];
        d.sp[PARTSET_TIME_FOR_INTENS_PRES_INDEX] = p.sp[PARTSET_TIME_FOR_INTENS_PRES_INDEX];
        d.sp[PARTSET_PRES_FOR_RESP_TIME_INDEX]   = p.sp[PARTSET_PRES_FOR_RESP_TIME_INDEX];

        sorcfile.get_external_parameter_settings_csvname( sorc_computer, sorc_machine, sorc_part );
        if ( sorcfile.file_exists() )
            {
            destfile.get_external_parameter_settings_csvname( dest_computer, dest_machine, dest_part );
            sorcfile.copyto( destfile );
            }

        sorcfile.get_shot_name_csvname( sorc_computer, sorc_machine, sorc_part );
        if ( sorcfile.file_exists() )
            {
            destfile.get_shot_name_csvname( dest_computer, dest_machine, dest_part );
            sorcfile.copyto( destfile );
            }

        sorcfile.get_shot_name_reset_csvname( sorc_computer, sorc_machine, sorc_part );
        if ( sorcfile.file_exists() )
            {
            destfile.get_shot_name_reset_csvname( dest_computer, dest_machine, dest_part );
            sorcfile.copyto( destfile );
            }
        }

    if ( flags & COPY_PART_USER_POSITIONS )
        {
        d.sp[PARTSET_USER_VEL_1_INDEX]  = p.sp[PARTSET_USER_VEL_1_INDEX];
        d.sp[PARTSET_USER_VEL_2_INDEX]  = p.sp[PARTSET_USER_VEL_2_INDEX];
        d.sp[PARTSET_USER_VEL_3_INDEX]  = p.sp[PARTSET_USER_VEL_3_INDEX];
        d.sp[PARTSET_USER_VEL_4_INDEX]  = p.sp[PARTSET_USER_VEL_4_INDEX];

        d.sp[PARTSET_AVG_START_1_INDEX] = p.sp[PARTSET_AVG_START_1_INDEX];
        d.sp[PARTSET_AVG_START_2_INDEX] = p.sp[PARTSET_AVG_START_2_INDEX];
        d.sp[PARTSET_AVG_START_3_INDEX] = p.sp[PARTSET_AVG_START_3_INDEX];
        d.sp[PARTSET_AVG_START_4_INDEX] = p.sp[PARTSET_AVG_START_4_INDEX];

        d.sp[PARTSET_AVG_END_1_INDEX]   = p.sp[PARTSET_AVG_END_1_INDEX];
        d.sp[PARTSET_AVG_END_2_INDEX]   = p.sp[PARTSET_AVG_END_2_INDEX];
        d.sp[PARTSET_AVG_END_3_INDEX]   = p.sp[PARTSET_AVG_END_3_INDEX];
        d.sp[PARTSET_AVG_END_4_INDEX]   = p.sp[PARTSET_AVG_END_4_INDEX];
        }
    }

if ( flags & COPY_PART_ADVANCED )
    {
    sorcfile.get_external_parameter_settings_csvname( sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_external_parameter_settings_csvname( dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

/*
---------------------------------------------
Copy the string changes to the numeric values
--------------------------------------------- */
d.update();

if ( !d.save() )
    {
    if ( !part_exists_already )
        {
        sorcfile.get_part_results_dir_file_name( dest_computer, dest_machine, dest_part, 0 );
        sorcfile.delete_directory();
        sorcfile.get_part_dir_file_name( dest_computer, dest_machine, dest_part, 0 );
        sorcfile.delete_directory();
        }
    return FALSE;
    }

/*
-------------------------------
Save the fastrak2 part settings
------------------------------- */
sorcfile.get_ft2_part_settings_file_name( dest_computer, dest_machine, dest_part );
if ( need_ftii_updates )
    dva.put_into( sorcfile );
else if ( sorc_ftii_exists )
    sva.put( sorcfile );

if ( part_exists_already )
    {
    if ( flags & COPY_PART_PARAM_NAMES )
        copy_parameter_names( d, p );
    }
else
    {
    sorcfile.get_parmlist_csvname(  sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_parmlist_csvname(  dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

/*
-------------------------
Copy the shot events file
------------------------- */
if ( !part_exists_already )
    {
    sorcfile.get_shot_events_setup_file_name(  sorc_computer, sorc_machine, sorc_part );
    if ( sorcfile.file_exists() )
        {
        destfile.get_shot_events_setup_file_name(  dest_computer, dest_machine, dest_part );
        sorcfile.copyto( destfile );
        }
    }

/*
---------------------
Save the plookup data
--------------------- */
plookup.init( dest_computer, dest_machine, dest_part );
if ( !plookup.exists() )
    plookup.put();

return TRUE;
}

/***********************************************************************
*                          COPY_PART_BY_FILES                          *
*                                                                      *
* This creates a dir if necessary and then copies all the files from   *
* the source dir to the new one. It then creates the results files if  *
* necessary and copies the reference trace.                            *
*                                                                      *
***********************************************************************/
BOOLEAN copy_part_by_files( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc )
{
NAME_CLASS d;
NAME_CLASS s;
DB_TABLE   t;

s.get_part_directory( sorc.computer, sorc.machine, sorc.part );
if ( !s.directory_exists() )
    return FALSE;

d.get_part_directory( dest.computer, dest.machine, dest.part );
if ( !d.directory_exists() )
    d.create_directory();

if ( !copy_all_files(d.text(), s.text(), StarDotStar, WITHOUT_SUBS, 0, 0) )
    return FALSE;

d.get_part_results_directory( dest.computer, dest.machine, dest.part );
if ( !d.directory_exists() )
    d.create_directory();

d.get_alarmsum_dbname( dest.computer, dest.machine, dest.part );
t.ensure_existance( d );

d.get_graphlst_dbname( dest.computer, dest.machine, dest.part );
t.ensure_existance( d );

d.get_shotparm_csvname( dest.computer, dest.machine, dest.part );
t.ensure_existance( d );

s.get_master_ii_name( sorc.computer, sorc.machine, sorc.part );
if ( s.file_exists() )
    {
    d.get_master_ii_name( dest.computer, dest.machine, dest.part );
    s.copyto( d );
    }
return TRUE;
}
