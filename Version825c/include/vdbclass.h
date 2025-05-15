#ifndef _VDB_CLASS_
#define _VDB_CLASS_

#ifndef _MULTISTRING_CLASS_
#include "..\include\multistring.h"
#endif

#define SEARCHFIRST     1
#define SEARCHNEXT      2

#define VS_SUCCESS      0

#define NO_RETRIES      0
#define NO_RECLEN      -1
#define NO_RECNUM      -1
#define NO_OFFSET      -1
#define NO_FILE_POSITION 0xFFFFFFFF

#define MAX_RECNUM_FIELD_LENGTH 12;

/*
---------------
File open types
--------------- */
#define NOT_OPEN                   0
#define OPEN_FOR_READ              1
#define OPEN_FOR_READ_WRITE        2
#define OPEN_FOR_WRITE             3

/*
------------------------
Compare search to record
------------------------ */
#define RECORD_LESS_THAN_SEARCH -1
#define RECORD_GREATER_THAN_SEARCH 1
#define RECORD_EQUALS_SEARCH 0
#define COMPARE_ERROR -2
#define READ_BUF_LEN 4096
#define READ_BUF_BYTES (READ_BUF_LEN * sizeof(TCHAR))

class VDB_CLASS
    {
    private:
    STRING_CLASS   name;
    TCHAR          buf[READ_BUF_LEN+1];
    HANDLE         th;
    int            nkeys;              /* Number of key fields */
    int            nrecs;
    int            open_type;
    int            recnum;
    int            reclen;
    DWORD          filepos;            /* byte position of start of the current record */
    DWORD          nextpos;            /* byte position of start of the next record */
    DWORD          filesize;           /* bytes in file */
    BITSETYPE      search_mode;
    DWORD          dberr;

    public:
    VDB_CLASS();
    ~VDB_CLASS();
    MULTISTRING_CLASS ra;    /* Record array */
    MULTISTRING_CLASS sa;    /* Search array */
    int    calculated_record_length();
    int    current_record_length();
    int    current_record_number() { return recnum; }
    void   close();
    int    compare_key_fields();
    bool   copy_rec( VDB_CLASS & sorcdb );
    bool   create( TCHAR * dbname );
    bool   create( STRING_CLASS & dbname ) { return create(dbname.text()); }
    bool   empty();
    bool   ensure_existance( STRING_CLASS & dbname );
    bool   get_bool( int col );
    bool   get_date( SYSTEMTIME & dest, int col );
    bool   get_first_equal_or_greater_date( int date_col, SYSTEMTIME & date_to_find );
    bool   get_first_equal_or_greater_key_match( int nof_key_fields );
    bool   get_first_greater_than_key_match( int nof_key_fields );
    bool   get_first_key_match( int nof_key_fields, bool include_greater_in_search );
    bool   get_first_key_match( int nof_key_fields );
    DWORD  get_last_error() { return dberr; }
    bool   get_last_record();
    bool   get_next_key_match( int nof_key_fields );
    bool   get_next_record();
    bool   get_prev_record();
    bool   get_rec();
    bool   get_rec( int n );
    int    get_recnum();
    bool   get_time( SYSTEMTIME & dest, int col );
    bool   goto_last_record();
    int    goto_next_record();
    int    goto_prev_record();
    bool   goto_record_number( int n );
    int    goto_start_of_record();
    int    nof_columns();
    int    nof_recs();
    bool   open( TCHAR * dbname, int new_open_type );
    bool   open_for_read( TCHAR * dbname );
    bool   open_for_read( STRING_CLASS dbname );
    bool   open_for_rw( TCHAR * dbname );
    bool   open_for_rw( STRING_CLASS & dbname );
    bool   open_for_write( TCHAR * dbname );
    bool   open_for_write( STRING_CLASS & dbname );
    bool   open_status();
    bool   put_bitsetype( int col, BITSETYPE sorc );
    bool   put_bool( int col, bool sorc );
    bool   put_date( int col, const SYSTEMTIME & sorc );
    bool   put_time( int col, SYSTEMTIME & sorc );
    bool   rec_append();
    bool   rec_delete( int ntokill );
    bool   rec_delete() { return rec_delete( 1 ); }
    bool   rec_insert();
    bool   rec_update();
    void   reset_search_mode() { search_mode = SEARCHFIRST; }
    void   rewind() { search_mode = SEARCHFIRST; }
    bool   set_nof_cols( int n );
    bool   write_record();
    };
#endif
