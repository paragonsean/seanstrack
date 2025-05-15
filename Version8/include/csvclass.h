#ifndef _CSV_CLASS_
#define _CSV_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

#define SEARCHFIRST     1
#define SEARCHNEXT      2

#define VS_SUCCESS      0

#define WITH_LOCK       TRUE
#define NO_LOCK         FALSE

#define NO_RETRIES      0

#define NUMBER_ADDED_CHARS 1        /* Field = number len +; */

#define DATE_LEN       10         /* 12/25/1996 */

static const DWORD OK_STATUS        =  0;
static const DWORD BAD_FILE_OPEN    =  1;
static const DWORD FILE_NOT_FOUND   =  2;
static const DWORD NO_BUFFER_MEMORY =  3;
static const DWORD LINE_TOO_LONG    =  4;
static const DWORD NO_PATH_MEMORY   =  5;
static const DWORD EMPTY_ERROR      =  6;
static const DWORD SETPOS_ERROR     =  7;
static const DWORD FILE_NOT_OPEN    =  8;
static const DWORD WRITE_ERROR      =  9;
static const DWORD END_OF_FILE      = 10;
static const DWORD READ_ERROR       = 11;
static const DWORD FAIL_EOF         = 12;

static const DWORD READBUF_LEN  = 32768;
static const DWORD MAX_LINE_LEN =  2048;

/*
-----------------
Get_alpha options
----------------- */
#define NO_TRIM         0
#define WITH_TRIM       1

/*
----------------
Table Lock Types
---------------- */
#define PFL  1
#define PWL  2
#define WL   4
#define FL   8

typedef unsigned short BITSETYPE;

class CSV_CLASS
    {

    private:

    STRING_CLASS name;
    HANDLE    th;
    TCHAR   * rp;
    TCHAR   * eof_readbuf;;
    TCHAR     readbuf[READBUF_LEN+1];
    TCHAR     recbuf[MAX_LINE_LEN+1];
    TCHAR     searchbuf[MAX_LINE_LEN+1];
    int       recbuflen;          /* Buffer Length NOT counting NULL */
    int       searchlen;
    int       recnum;
    BITSETYPE table_lock_type;
    BOOLEAN   have_table_lock;
    BOOLEAN   is_end_of_file;
    BITSETYPE search_mode;
    DWORD     last_error;

    BOOLEAN   compare_one_record( int * same, int32 rn, int32 nof_key_fields );
    int       current_record_offset() { return recnum * sizeof(TCHAR) * DWORD(recbuflen); }
    BOOLEAN   fill_searchbuf( int nof_key_fields );
    BOOLEAN   goto_key_record( int nof_key_fields, int32 firstrec, int32 lastrec );
    BOOLEAN   goto_first_key_record( int nof_key_fields );
    BOOLEAN   initbuf( TCHAR ** buf, int * buflen, int slen );
    LPTSTR    point_to_start_of_field( int fi );
    void      restore_from_searchbuf();
    BOOLEAN   write_record();

    public:
    CSV_CLASS();
    ~CSV_CLASS();
    int     current_record_number() { return recnum; }
    void    close();
    BOOLEAN copy_rec( CSV_TABLE & sorcdb );
    BOOLEAN create( TCHAR * dbname );
    BOOLEAN create( STRING_CLASS & dbname ) { return create(dbname.text()); }
    BOOLEAN empty();
    BOOLEAN ensure_existance( STRING_CLASS & dbname );
    BOOLEAN get_current_record( BOOLEAN need_lock );
    DWORD   get_last_error()   { return last_error; }
    BOOLEAN get_boolean( int fi );
    TCHAR   get_char( int fi );
    BOOLEAN get_date( SYSTEMTIME & dest, int fi );
    double  get_double( int fi );
    float   get_float(  int fi )
        {
        return (float) get_double( fo );
        }
    int     get_int( int fi );
    BOOLEAN get_text( STRING_CLASS & dest, int fi );
    BOOLEAN get_text( TCHAR * dest,        int fi );
    BOOLEAN get_next_key_match( int nof_key_fields, BOOLEAN need_lock );
    BOOLEAN get_next_record();
    BOOLEAN get_rec();
    BOOLEAN get_time( SYSTEMTIME & dest, int fi );
    BOOLEAN goto_first_greater_than_record( int nof_key_fields );
    BOOLEAN goto_first_equal_or_greater_record( int nof_key_fields );
    BOOLEAN goto_record_number( int n );
    BOOLEAN open( TCHAR * dbname, int reclen, int locktype, int time_out );
    inline BOOLEAN open( STRING_CLASS & dbname, int reclen, int locktype, int time_out ) {return open( dbname.text(), reclen, locktype, time_out ); }
    BOOLEAN open( TCHAR * dbname, int reclen, int locktype );
    inline BOOLEAN open( STRING_CLASS & dbname, int reclen, int locktype ) { return open( dbname.text(), reclen, locktype ); }
    BOOLEAN open_status();
    BOOLEAN put_boolean( int fi, BOOLEAN sorc );
    BOOLEAN put_char( int fi, TCHAR sorc );
    BOOLEAN put_date( int fi, const SYSTEMTIME & sorc );
    BOOLEAN put_double( int fi, const double sorc );
    BOOLEAN put_float(  int fi, const float sorc );
    BOOLEAN put_int( int fi, int sorc  );
    BOOLEAN put_text( int fi, const TCHAR * sorc );
    inline BOOLEAN put_text( int fi, STRING_CLASS & sorc ) { return put_alpha( fi, sorc.text() ); }
    BOOLEAN put_time( int fi, SYSTEMTIME & sorc );
    BOOLEAN rec_append();
    BOOLEAN rec_delete( int32 ntokill );
    BOOLEAN rec_delete() { return rec_delete( 1 ); }
    BOOLEAN rec_insert();
    BOOLEAN rec_update();
    int     record_length() { return recbuflen; }
    void    rewind();
    };
#endif
