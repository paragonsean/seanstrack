#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"

static TCHAR SpaceChar = TEXT(' ');
static TCHAR TabChar   = TEXT('\t');

/***********************************************************************
*                        UPDATE_DOWNTIME_TABLE                         *
* The new downtime table is the same as the old one except the length  *
* of the last field, the part name, has increased from 8 to 50. The    *
* record length thus increased from 53 to 95.                          *
***********************************************************************/
BOOLEAN update_downtime_table(STRING_CLASS & computer, STRING_CLASS & machine )
{
DWORD    file_size;
BOOLEAN  need_to_update;
DB_TABLE sorc;
DB_TABLE dest;
NAME_CLASS d;
NAME_CLASS s;
FILE_CLASS f;
SYSTEMTIME st;
TCHAR cat[DOWNCAT_NUMBER_LEN+1];
TCHAR subcat[DOWNCAT_NUMBER_LEN+1];
TCHAR worker[OPERATOR_NUMBER_LEN+1];
STRING_CLASS part;

need_to_update = FALSE;

s.get_downtime_dbname( computer, machine );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s.text()) )
        {
        if ( f.get_file_size(file_size) )
            {
            if ( file_size > 0 )
                {
                s = f.readline();
                if ( s.len() < (DOWNTIME_RECLEN-2) )
                    need_to_update = TRUE;
                }
            }
        f.close();
        }
    }

if ( need_to_update )
    {
    if ( sorc.open(s, OLD_DOWNTIME_RECLEN, PFL) )
        {
        d = s;
        d.replace( DOWNTIME_DB, TEMP_DOWNTIME_DB );
        if ( dest.open(d, DOWNTIME_RECLEN, FL) )
            {
            while ( sorc.get_next_record(NO_LOCK) )
                {
                sorc.get_date(  st,     DOWNTIME_DATE_OFFSET );
                sorc.get_time(  st,     DOWNTIME_TIME_OFFSET );
                sorc.get_alpha( cat,    DOWNTIME_CAT_OFFSET,       DOWNCAT_NUMBER_LEN  );
                sorc.get_alpha( subcat, DOWNTIME_SUBCAT_OFFSET,    DOWNCAT_NUMBER_LEN  );
                sorc.get_alpha( worker, DOWNTIME_OPERATOR_OFFSET,  OPERATOR_NUMBER_LEN );
                sorc.get_alpha( part,   DOWNTIME_PART_NAME_OFFSET, OLD_PART_NAME_LEN   );

                dest.put_date(  DOWNTIME_DATE_OFFSET,      st );
                dest.put_time(  DOWNTIME_TIME_OFFSET,      st );
                dest.put_alpha( DOWNTIME_CAT_OFFSET,       cat,    DOWNCAT_NUMBER_LEN     );
                dest.put_alpha( DOWNTIME_SUBCAT_OFFSET,    subcat, DOWNCAT_NUMBER_LEN     );
                dest.put_alpha( DOWNTIME_OPERATOR_OFFSET,  worker, OPERATOR_NUMBER_LEN    );
                dest.put_alpha( DOWNTIME_PART_NAME_OFFSET, part,   DOWNTIME_PART_NAME_LEN );
                dest.rec_append();
                }
            dest.close();
            }
        else
            {
            d.null();
            }
        sorc.close();
        if ( !d.isempty() )
            {
            s.delete_file();
            d.moveto( s );
            }
        }
    }

return TRUE;
}

/***********************************************************************
*                         MAKE_DOWNTIME_RECORD                         *
***********************************************************************/
BOOLEAN make_downtime_record( DB_TABLE & t, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat, TCHAR * op, STRING_CLASS & part )
{
TCHAR worker[OPERATOR_NUMBER_LEN+1];

copystring( worker, op, OPERATOR_NUMBER_LEN );
rjust( worker, OPERATOR_NUMBER_LEN, SpaceChar );

t.put_date(  DOWNTIME_DATE_OFFSET,       st );
t.put_time(  DOWNTIME_TIME_OFFSET,       st );
t.put_alpha( DOWNTIME_CAT_OFFSET,        cat,    DOWNCAT_NUMBER_LEN );
t.put_alpha( DOWNTIME_SUBCAT_OFFSET,     subcat, DOWNCAT_NUMBER_LEN );
t.put_alpha( DOWNTIME_OPERATOR_OFFSET,   worker, OPERATOR_NUMBER_LEN );
t.put_alpha( DOWNTIME_PART_NAME_OFFSET,  part,   DOWNTIME_PART_NAME_LEN );

return TRUE;
}

/***********************************************************************
*                      MAKE_DOWNTIME_EVENT_STRING                      *
* NOTE: !! The caller is responsible for deleting the string !!        *
***********************************************************************/
TCHAR * make_downtime_event_string( STRING_CLASS & machine, STRING_CLASS & part, SYSTEMTIME & st, TCHAR * cat, TCHAR * subcat, TCHAR * worker )
{
static const int DDE_LINE_LEN  = 1 + 1 + ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1 + 2*(DOWNCAT_NUMBER_LEN + 1) + 2*(DOWNCAT_NAME_LEN + 1) + OPERATOR_NUMBER_LEN;

int          slen;
STRING_CLASS s;

slen = DDE_LINE_LEN + machine.len() + part.len();
s.init( slen );

s = machine;
s += TabChar;
s.cat_w_char( part, TabChar );
s.cat_w_char( alphadate(st), TabChar );
s.cat_w_char( alphatime(st), TabChar );
s.cat_w_char( cat,           TabChar );
s.cat_w_char( subcat,        TabChar );
s.cat_w_char( category_name(cat), TabChar );
s.cat_w_char( subcategory_name(cat, subcat), TabChar );
s += worker ;

return s.release();
}

/***********************************************************************
*                            CATEGORY_NAME                             *
***********************************************************************/
TCHAR * category_name( TCHAR * number_to_match )
{
static TCHAR name[DOWNCAT_NAME_LEN+1];
static TCHAR number[DOWNCAT_NUMBER_LEN+1];
DB_TABLE t;
NAME_CLASS s;

lstrcpy( name, NO_DOWNCAT_NAME );
s.get_data_dir_file_name( DOWNCAT_DB );
if ( t.open( s.text(), DOWNCAT_RECLEN, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( number, DOWNCAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
        if ( compare(number_to_match, number, DOWNCAT_NUMBER_LEN) == 0 )
            {
            t.get_alpha( name,   DOWNCAT_NAME_OFFSET,   DOWNCAT_NAME_LEN   );
            break;
            }
        }
    t.close();
    }

return name;
}

/***********************************************************************
*                          SUBCATEGORY_NAME                            *
***********************************************************************/
TCHAR * subcategory_name( TCHAR * cat_to_match, TCHAR * subcat_to_match )
{
static TCHAR cat[DOWNCAT_NUMBER_LEN+1];
static TCHAR subcat[DOWNCAT_NUMBER_LEN+1];
static TCHAR name[DOWNCAT_NAME_LEN+1];
DB_TABLE t;
short    record_length;
NAME_CLASS s;

lstrcpy( name, NO_DOWNCAT_NAME );
s.get_data_dir_file_name( DOWNSCATC_DB );
if ( s.file_exists() )
    {
    record_length = DOWNSCATC_RECLEN;
    }
else
    {
    record_length = DOWNSCAT_RECLEN;
    s.get_data_dir_file_name( DOWNSCAT_DB );
    }

if ( t.open(s.text(), record_length, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( cat,    DOWNSCAT_CAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
        t.get_alpha( subcat, DOWNSCAT_SUB_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
        if ( compare(cat_to_match, cat, DOWNCAT_NUMBER_LEN) == 0 && compare(subcat_to_match, subcat, DOWNCAT_NUMBER_LEN) == 0 )
            {
            t.get_alpha( name, DOWNSCAT_SUB_NAME_OFFSET, DOWNCAT_NAME_LEN   );
            break;
            }
        }
    t.close();
    }

return name;
}
