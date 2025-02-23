#ifndef _DSTAT_CLASS_
#define _DSTAT_CLASS_

#define NO_DOWN_STATE    0
#define HUMAN_DOWN_STATE 1
#define AUTO_DOWN_STATE  2
#define PROG_DOWN_STATE  3
#define PROG_UP_STATE    4
#define MACH_UP_STATE    5
#define NO_CONNECT_STATE 6       /* Ft2 only */

class DSTAT_CLASS
    {
    private:

    TCHAR       cat[DOWNCAT_NUMBER_LEN+1];
    TCHAR       subcat[DOWNCAT_NUMBER_LEN+1];
    SYSTEMTIME  st;

    public:

    BOOLEAN     same_code( TCHAR * newcat, TCHAR * newsubcat );
    BOOLEAN     same_code( TCHAR * code ) { return same_code( code, code+DOWNCAT_NUMBER_LEN ); }

    DSTAT_CLASS() { lstrcpy( cat, NO_DOWNCAT_NUMBER); lstrcpy(subcat, NO_DOWNCAT_NUMBER); }
    ~DSTAT_CLASS() {}
    TCHAR      * category() { return cat; }
    int          down_state();
    BOOLEAN      get( STRING_CLASS & computer, STRING_CLASS & machine );
    BOOLEAN      is_down();
    BOOLEAN      put( STRING_CLASS & computer, STRING_CLASS & machine );
    void         set_cat( TCHAR * new_cat ) { lstrcpyn( cat, new_cat, DOWNCAT_NUMBER_LEN+1 ); }
    void         set_subcat( TCHAR * new_subcat ) { lstrcpyn( subcat, new_subcat, DOWNCAT_NUMBER_LEN+1 ); }
    void         set_time( SYSTEMTIME & new_time ) { st = new_time; }
    TCHAR      * subcategory() { return subcat; }
    long         update_sec();
    SYSTEMTIME * update_time() { return &st; }
    };

#endif
