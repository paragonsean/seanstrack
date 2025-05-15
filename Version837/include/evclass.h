#ifndef _NEW_MACHINE_STATE_CLASS_
#define _NEW_MACHINE_STATE_CLASS_

class NEW_MACHINE_STATE_CLASS
    {
    private:

    STRING_CLASS name;
    BOARD_DATA old_status_value;
    BOARD_DATA new_status_value;

    public:

    NEW_MACHINE_STATE_CLASS() { old_status_value = new_status_value = 0; }
    ~NEW_MACHINE_STATE_CLASS() {}
    STRING_CLASS & machine_name() { return name; }
    BOARD_DATA new_status() { return new_status_value; }
    BOARD_DATA old_status() { return old_status_value; }
    BOOLEAN    set( TCHAR *data );
    };

#endif

#ifndef _DOWN_DATA_CLASS_
#define _DOWN_DATA_CLASS_

class DOWN_DATA_CLASS
    {
    private:

    STRING_CLASS machine;
    STRING_CLASS part;
    SYSTEMTIME st;
    TCHAR cat[DOWNCAT_NUMBER_LEN+1];
    TCHAR subcat[DOWNCAT_NUMBER_LEN+1];
    TCHAR catname[DOWNCAT_NAME_LEN+1];
    TCHAR subcatname[DOWNCAT_NAME_LEN+1];
    TCHAR opnumber[OPERATOR_NUMBER_LEN+1];

    public:

    DOWN_DATA_CLASS();
    ~DOWN_DATA_CLASS() {}
    void  operator=(const DOWN_DATA_CLASS & sorc );
    STRING_CLASS & machine_name() { return machine; }
    STRING_CLASS & part_name()    { return part; }
    TCHAR    * category()         { return cat; }
    TCHAR    * subcategory()      { return subcat; }
    TCHAR    * category_name()    { return catname; }
    TCHAR    * subcategory_name() { return subcatname; }
    TCHAR    * operator_number()  { return opnumber; }
    SYSTEMTIME * system_time()    { return &st; }
    BOOLEAN    set( TCHAR * data );
    };

#endif

