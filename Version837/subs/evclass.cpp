#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\evclass.h"
#include "..\include\subs.h"

/***********************************************************************
*                                 SET                                  *
***********************************************************************/
BOOLEAN NEW_MACHINE_STATE_CLASS::set(TCHAR * data)
{
    TCHAR * cp;

    if (!data)
        return FALSE;
      
    if (replace_tab_with_null(data))
    {
        name = data;
        cp = nextstring(data);
        old_status_value = (BOARD_DATA)extlong(cp, BOARD_DATA_HEX_LEN, HEX_RADIX);
        cp += BOARD_DATA_HEX_LEN + 1;
        new_status_value = (BOARD_DATA)extlong(cp, BOARD_DATA_HEX_LEN, HEX_RADIX);
        return TRUE;
    } 

    return FALSE;
}

/***********************************************************************
*                             DOWN_DATA_CLASS                          *
***********************************************************************/
DOWN_DATA_CLASS::DOWN_DATA_CLASS()
{
lstrcpy( cat,        DOWNCAT_SYSTEM_CAT       );
lstrcpy( subcat,     DOWNCAT_UNSP_DOWN_SUBCAT );
lstrcpy( catname,    NO_DOWNCAT_NAME          );
lstrcpy( subcatname, NO_DOWNCAT_NAME          );
lstrcpy( opnumber,   NO_OPERATOR_NUMBER       );
init_systemtime_struct( st );
}

/***********************************************************************
*                             DOWN_DATA_CLASS                          *
*                                   =                                  *
***********************************************************************/
void DOWN_DATA_CLASS::operator=(const DOWN_DATA_CLASS & sorc )
{
machine = sorc.machine;
part    = sorc.part;
lstrcpy( cat,        sorc.cat        );
lstrcpy( subcat,     sorc.subcat     );
lstrcpy( catname,    sorc.catname    );
lstrcpy( subcatname, sorc.subcatname );
lstrcpy( opnumber,   sorc.opnumber   );
st = sorc.st;
}

/***********************************************************************
*                                 SET                                  *
***********************************************************************/
BOOLEAN DOWN_DATA_CLASS::set(TCHAR * data)
{  
    TCHAR    * cp;
    SYSTEMTIME st;
    BOOLEAN    status;

    if (!data)
        return FALSE;

    status = FALSE;
    cp = data;

    /*
    ------------
    Machine name
    ------------ */
    status = replace_tab_with_null(cp);
    if (status)
    {
        machine = cp;

        /*
        ---------
        Part name
        --------- */
        cp = nextstring(cp);
        status = replace_tab_with_null(cp);
    }

    if (status)
    {
        part = cp;

        /*
        ----
        Date
        ---- */
        cp = nextstring(cp);
        status = replace_tab_with_null(cp);
    }

    if (status)
    {
        extmdy(st, cp);

        /*
        ----
        Time
        ---- */
        cp = nextstring(cp);
        status = replace_tab_with_null(cp);
    }

    if (status)
    {
        exthms(st, cp);

        /*
        ---
        Cat
        --- */
        cp = nextstring(cp);
        status = replace_tab_with_null(cp);
    }

    if (status)
    {
        lstrcpy(cat, cp);

        /*
        ------
        Subcat
        ------ */
        cp = nextstring(cp);
        status = replace_tab_with_null(cp);
    }

    if (status)
    {
        lstrcpy(subcat, cp);

        /*
        -------------
        Category name
        ------------- */
        cp = nextstring(cp);
        status = replace_tab_with_null(cp);
    }

    if (status)
    {
        lstrcpy(catname, cp);

        /*
        ----------------
        Subcategory name
        ---------------- */
        cp = nextstring(cp);
        status = replace_tab_with_null(cp);
    }

    if (status)
    {
        lstrcpy(subcatname, cp);

        /*
        ---------------
        Operator number
        --------------- */
        cp = nextstring(cp);
        lstrcpy(opnumber, cp);
    }

    return status;
}