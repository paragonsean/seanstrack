#define _VISI_MARKS_H_

#define MAX_MARKS (MAX_PARMS + USER_AVG_VEL_COUNT + USER_AVG_VEL_COUNT + USER_VEL_COUNT + FT_ONOFF_COUNT + FT_ONOFF_COUNT)

/*
-------
Symbols
------- */
#define VLINE_MARK  1
#define BLINE_MARK  2
#define NOLINE_MARK 4
#define BMP_MARK    8
#define DRAW_MARK  16

/*
---------------
Parameter types
--------------- */
#define PARAMETER_MARK TEXT( 'P' )
#define START_POS_MARK TEXT( 'S' )
#define END_POS_MARK   TEXT( 'E' )
#define VEL_POS_MARK   TEXT( 'V' )

struct MARKLIST_ENTRY
    {
    TCHAR        parameter_type;
    short        parameter_number;
    short        color;
    BITSETYPE    symbol;
    TCHAR        label[MARK_LABEL_LEN+1];
    STRING_CLASS offset;
    };

class MARKS_CLASS
    {
    private:
    short          new_index( TCHAR type_to_add, short number_to_add );
    void           set_defaults( short i );

    public:

    STRING_CLASS   computer;
    STRING_CLASS   machine;
    STRING_CLASS   part;
    short          n;
    MARKLIST_ENTRY array[MAX_MARKS];

    MARKS_CLASS();
    void           add( TCHAR type_to_add, short number_to_add );
    short          index( TCHAR type_to_find, short number_to_find );
    BOOLEAN        load( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
    void           remove( short i );
    BOOLEAN        save();
    };
