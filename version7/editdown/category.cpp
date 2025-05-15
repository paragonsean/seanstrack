#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

#include "resource.h"
#include "category.h"

static TCHAR * NoNameString   = 0;
static TCHAR * NoNumberString = 0;

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                            CATEGORY_CLASS                            *
***********************************************************************/
CATEGORY_CLASS::CATEGORY_CLASS()
{
n = 0;
x = NO_INDEX;
cp = 0;

if ( !NoNameString )
    NoNameString = maketext( resource_string(NO_NAME_STRING) );

if ( !NoNumberString )
    NoNumberString = maketext( resource_string(NO_NUMBER_STRING) );
}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void CATEGORY_CLASS::cleanup()
{

if ( cp )
    {
    delete[] cp;
    cp = 0;
    n  = 0;
    x  = NO_INDEX;
    }
}

/***********************************************************************
*                           ~CATEGORY_CLASS                            *
***********************************************************************/
CATEGORY_CLASS::~CATEGORY_CLASS()
{
cleanup();
}

/***********************************************************************
*                               READ                                   *
***********************************************************************/
BOOLEAN CATEGORY_CLASS::read()
{
NAME_CLASS s;
DB_TABLE   t;
int        nof_categories;
BOOLEAN    status;

status = FALSE;

cleanup();

s.get_downcat_dbname();
if ( t.open(s, DOWNCAT_RECLEN, PFL) )
    {
    nof_categories = t.nof_recs();
    if ( nof_categories > 0 )
        {
        cp = new CATEGORY_ENTRY[nof_categories];
        if ( cp )
            {
            status = TRUE;
            n      = 0;
            x      = NO_INDEX;
            while( t.get_next_record(NO_LOCK) )
                {
                t.get_alpha( cp[n].number, DOWNCAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                t.get_alpha( cp[n].name,   DOWNCAT_NAME_OFFSET,   DOWNCAT_NAME_LEN   );
                n++;
                if ( n >= nof_categories )
                    break;
                }
            }
        }
    t.close();
    }

return status;
}

/***********************************************************************
*                                NAME                                  *
***********************************************************************/
TCHAR * CATEGORY_CLASS::name()
{
if ( x != NO_INDEX )
    return cp[x].name;

return NoNameString;
}

/***********************************************************************
*                                CAT                                   *
***********************************************************************/
TCHAR * CATEGORY_CLASS::cat()
{
if ( x != NO_INDEX )
    return cp[x].number;

return NoNumberString;
}

/***********************************************************************
*                               NEXT                                   *
***********************************************************************/
BOOLEAN CATEGORY_CLASS::next()
{
if ( n <= 0 || x >= (n-1) )
    return FALSE;

if ( x == NO_INDEX )
    x = 0;
else
    x++;

return TRUE;
}

/***********************************************************************
*                                 FIND                                 *
***********************************************************************/
BOOLEAN CATEGORY_CLASS::find( TCHAR * cat_to_find )
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( lstrcmp(cp[i].number, cat_to_find) == 0 )
        {
        x = i;
        return TRUE;
        }
    }

x = NO_INDEX;
return FALSE;
}

/***********************************************************************
*                              FINDNAME                                *
***********************************************************************/
BOOLEAN CATEGORY_CLASS::findname( const TCHAR * name_to_find )
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( lstrcmp(cp[i].name, name_to_find) == 0 )
        {
        x = i;
        return TRUE;
        }
    }

x = NO_INDEX;
return FALSE;
}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void SUBCAT_CLASS::cleanup()
{

if ( sp )
    {
    delete[] sp;
    sp = 0;
    n  = 0;
    x  = NO_INDEX;
    }
}

/***********************************************************************
*                           ~SUBCAT_CLASS                              *
***********************************************************************/
SUBCAT_CLASS::~SUBCAT_CLASS()
{
cleanup();
}

/***********************************************************************
*                               READ                                   *
***********************************************************************/
BOOLEAN SUBCAT_CLASS::read()
{
DB_TABLE    t;
int         nof_categories;
BOOLEAN     status;
short       record_length;
NAME_CLASS  s;

status = FALSE;

cleanup();

s.get_downscatc_dbname();
if ( s.file_exists() )
    {
    record_length = DOWNSCATC_RECLEN;
    }
else
    {
    record_length = DOWNSCAT_RECLEN;
    s.get_downscat_dbname();
    }

if ( t.open(s, record_length, PFL) )
    {
    nof_categories = t.nof_recs();
    if ( nof_categories > 0 )
        {
        sp = new SUBCAT_ENTRY[nof_categories];
        if ( sp )
            {
            status = TRUE;
            n      = 0;
            while( t.get_next_record(NO_LOCK) )
                {
                t.get_alpha( sp[n].catnumber, DOWNSCAT_CAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                t.get_alpha( sp[n].subnumber, DOWNSCAT_SUB_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                t.get_alpha( sp[n].name,      DOWNSCAT_SUB_NAME_OFFSET,   DOWNCAT_NAME_LEN   );
                n++;
                if ( n >= nof_categories )
                    break;
                }
            }
        }
    t.close();
    }

return status;
}

/***********************************************************************
*                                NAME                                  *
***********************************************************************/
TCHAR * SUBCAT_CLASS::name()
{
if ( x != NO_INDEX )
    return sp[x].name;

return NoNameString;
}

/***********************************************************************
*                                CAT                                   *
***********************************************************************/
TCHAR * SUBCAT_CLASS::cat()
{
if ( x != NO_INDEX )
    return sp[x].catnumber;

return NoNumberString;
}

/***********************************************************************
*                              SUBCAT                                  *
***********************************************************************/
TCHAR * SUBCAT_CLASS::subcat()
{
if ( x != NO_INDEX )
    return sp[x].subnumber;

return NoNumberString;
}

/***********************************************************************
*                               NEXT                                   *
***********************************************************************/
BOOLEAN SUBCAT_CLASS::next()
{
if ( n <= 0 || x >= (n-1) )
    return FALSE;

if ( x == NO_INDEX )
    x = 0;
else
    x++;

return TRUE;
}

/***********************************************************************
*                                 FIND                                 *
***********************************************************************/
BOOLEAN SUBCAT_CLASS::find( TCHAR * cat_to_find, TCHAR * subcat_to_find )
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( lstrcmp(sp[i].catnumber, cat_to_find) == 0 && lstrcmp(sp[i].subnumber, subcat_to_find) == 0 )
        {
        x = i;
        return TRUE;
        }
    }

x = NO_INDEX;
return FALSE;
}

/***********************************************************************
*                              FINDNAME                                *
***********************************************************************/
BOOLEAN SUBCAT_CLASS::findname( TCHAR * cat_to_find, TCHAR * name_to_find )
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( lstrcmp(sp[i].catnumber, cat_to_find) == 0 && lstrcmp(sp[i].name, name_to_find) == 0 )
        {
        x = i;
        return TRUE;
        }
    }

x = NO_INDEX;
return FALSE;
}
