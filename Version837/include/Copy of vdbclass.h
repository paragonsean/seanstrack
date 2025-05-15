#ifndef _VDB_CLASS_
#define _VDB_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

#define SEARCHFIRST     1
#define SEARCHNEXT      2

#define VS_SUCCESS      0

#define NO_RETRIES      0
#define NO_RECNUM      -1
#define NO_OFFSET      -1
#define NO_FILE_POSITION 0xFFFFFFFF

/*
---------------
File open types
--------------- */
#define OPEN_FOR_READ  0
#define OPEN_FOR_READ_WRITE 1
#define OPEN_FOR_WRITE 2

/*
------------------------
Compare search to record
------------------------ */
#define RECORD_LESS_THAN_SEARCH -1
#define RECORD_GREATER_THAN_SEARCH 1
#define RECORD_EQUALS_SEARCH 0
#define COMPARE_ERROR -2

typedef unsigned short BITSETYPE;

class VDB_CLASS
    {

    private:
    const DWORD READ_BUF_LEN = 4096/sizeof(TCHAR);

    STRING_CLASS   name;
    TCHAR          buf[READ_BUF_LEN+1];
    HANDLE         th;
    int            nof_cols;
    int            nof_recs;
    int            recnum;
    DWORD          filepos;            /* byte position of start of the current record */
    DWORD          nextpos;            /* byte position of start of the next record */
    DWORD          filesize;           /* bytes in file */
    BITSETYPE      search_mode;
    DWORD          dberr;
    bool           record_eq_search( int nof_key_fields );
    bool           record_ge_search( int nof_key_fields );
    bool           record_gt_search( int nof_key_fields );
    bool           record_lt_search( int nof_key_fields );
    bool           goto_eof();
    bool           goto_key_record( int nof_key_fields, int firstrec, int lastrec );
    bool           goto_first_key_record( int nof_key_fields );
    LPTSTR         point_to_start_of_field( int column_index );
    bool           put_rec();

    public:
    VDB_CLASS();
    ~VDB_CLASS();
    STRING_CLASS * record;
    STRING_CLASS * search;
    bool   append( TCHAR * dbname );
    int    current_record_number() { return recnum; }
    void   close();
    bool   copy_rec( VDB_CLASS & sorcdb );
    bool   create( TCHAR * dbname );
    bool   create( STRING_CLASS & dbname ) { return create(dbname.text()); }
    bool   empty();
    bool   ensure_existance( STRING_CLASS & dbname );
    int    field_length( int column_index );
    bool   get_current_record( bool need_lock );
    DWORD  get_last_error() { return dberr; }
    bool   get_next_key_match( int nof_key_fields );
    bool   get_next_record();
    bool   get_prev_record();
    bool   get_rec();
    bool   get_time( SYSTEMTIME & dest, int column_index );
    bool   goto_first_greater_than_record( int nof_key_fields );
    bool   goto_first_equal_or_greater_date( int column_index, SYSTEMTIME & date_to_find );
    bool   goto_first_equal_or_greater_record( int nof_key_fields );
    bool   goto_record_number( int n );
    int    nof_columns();
    DWORD  nof_recs();
    bool   open_for_read( TCHAR * dbname );
    bool   open_for_read( STRING_CLASS dbname );
    bool   open_for_rw( TCHAR * dbname );
    bool   open_for_rw( STRING_CLASS & dbname );
    bool   open_for_write( TCHAR * dbname );
    bool   open_for_write( STRING_CLASS & dbname );
    bool   open_status();
    bool   put_time( int column_index, SYSTEMTIME & sorc );
    bool   rec_append();
    bool   rec_delete( int ntokill );
    bool   rec_delete() { return rec_delete( 1 ); }
    bool   rec_insert();
    bool   rec_update();
    void   reset_search_mode() { search_mode = SEARCHFIRST; }
    void   rewind() { search_mode = SEARCHFIRST; }
    bool   set_nof_cols( int n );
    };
#endif
