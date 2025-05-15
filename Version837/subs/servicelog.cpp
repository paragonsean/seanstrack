#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\iniclass.h"
#include "..\include\servicelog.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"

static const TCHAR ForwardSlashChar     = TEXT( '/' );
static const TCHAR ColonChar            = TEXT( ':' );
static const TCHAR SpaceChar            = TEXT( ' ' );
static const TCHAR ServiceLogIniFile[]  = TEXT( "servicelog.ini" );
static const TCHAR ConfigSection[]      = TEXT( "Config" );
static const TCHAR LogLevelKey[]        = TEXT( "LogLevel" );
static const TCHAR RetentionDaysKey[]   = TEXT( "RetentionDays" );
static const TCHAR MaxFileSizeKey[]     = TEXT( "MaxFileSizeKBytes" );
static const TCHAR MaxNumArchivesKey[]     = TEXT( "MaxNumArchives" );

extern TCHAR MyClassName[];

/**********************************************************************
*                           SETUP_SERVICE_LOG                         *
**********************************************************************/
void setup_service_log( SERVICE_LOG &log )
{
    STRING_CLASS log_msg;
    NAME_CLASS fn;
    STRING_CLASS s;
    static TCHAR Buf[MAX_LONG_INI_STRING_LEN + 1];
    INI_CLASS ini;   

    fn.get_exe_dir_file_name( ServiceLogIniFile );
    ini.set_file( fn.text() );
    ini.set_section( ConfigSection );

    /*
    ---------------------------------
    Get Log Level from servicelog.ini
    --------------------------------- */
    if ( ini.find(LogLevelKey) )
    {
        log.set_log_level( ini.get_int() );
    }

    /*
    ------------------------------------------
    Get log retention days from servicelog.ini
    ------------------------------------------ */
    if ( ini.find(RetentionDaysKey) )
    {
        log.set_log_retention( ini.get_int() );
    }

    /*
    -------------------------------------
    Get max file size from servicelog.ini
    ------------------------------------- */
    if ( ini.find(MaxFileSizeKey) )
    {
        log.set_max_file_size( ini.get_int() );
    }

    /*
    -------------------------------------------
    Get max num archive files from visitrak.ini
    ------------------------------------------- */
    if ( ini.find(MaxNumArchivesKey) )
    {
        log.set_max_num_archives( ini.get_int() );
    }

    /*
    -----------------------------
    Get and set the log file name
    ----------------------------- */
    fn.get_log_file_name();
    log.set_log_file( fn );
}

/**********************************************************************
*                         GET_TIME_DELTA_STRING                       *
**********************************************************************/
void get_time_delta_string( SYSTEMTIME &start, SYSTEMTIME &end, STRING_CLASS &dest )
{
    WORD dM;
    WORD dS;
    WORD dMS;

    dM = end.wMinute - start.wMinute;
    dS = end.wSecond - start.wSecond;
    dMS = end.wMilliseconds - start.wMilliseconds;
    dest += dM;
    dest += TEXT( ":" );
    dest += dS;
    dest += TEXT( ":" );
    dest += dMS;
}

/***********************************************************************
*                             SERVICE_LOG                              *
*                             SERVICE_LOG                              *
***********************************************************************/
SERVICE_LOG::SERVICE_LOG()
{
    log_level = LOG_LEVEL_SERVICE;
    log_retention_days = 30;
    max_file_size = 1000;
    max_num_archives = MAX_NUM_ARCHIVES;
}

/***********************************************************************
*                             SERVICE_LOG                              *
*                             SET_LOG_FILE                             *
***********************************************************************/
void SERVICE_LOG::set_log_file( STRING_CLASS &filepath )
{
    log_path = filepath;

    if ( !archive_log_file() )
    {
        prune_log_file();
    }
}

/***********************************************************************
*                             SERVICE_LOG                              *
*                            SET_LOG_LEVEL                             *
***********************************************************************/
void SERVICE_LOG::set_log_level( int level )
{
    log_level = level;
}

/***********************************************************************
*                             SERVICE_LOG                              *
*                          SET_LOG_RETENTION                           *
***********************************************************************/
void SERVICE_LOG::set_log_retention( int days )
{
    log_retention_days = days;
}

/***********************************************************************
*                             SERVICE_LOG                              *
*                          SET_MAX_FILE_SIZE                           *
***********************************************************************/
void SERVICE_LOG::set_max_file_size( int kilo_bytes )
{
    max_file_size = kilo_bytes * 1000;
}

/***********************************************************************
*                             SERVICE_LOG                              *
*                        SET_MAX_NUM_ARCHIVES                          *
***********************************************************************/
void SERVICE_LOG::set_max_num_archives( int num )
{
    max_num_archives = num;
}

/***********************************************************************
*                             SERVICE_LOG                              *
*                              ADD_ENTRY                               *
***********************************************************************/
void SERVICE_LOG::add_entry( STRING_CLASS &msg, int level )
{
    TIME_CLASS t;
    STRING_CLASS s;

    if ( level > log_level )
        return;

    if ( msg == last_msg )
        return;

    last_msg = msg;

    t.get_local_time();
    s = t.text();
    s += TEXT( " " );
    if ( MyClassName )
    {
        s += MyClassName;
        s += TEXT( ": " );
    }
    s += msg;
    log_file.open_for_append( log_path );
    log_file.writeline( s );
    log_file.close();
}

/***********************************************************************
*                             SERVICE_LOG                              *
*                          ARCHIVE_LOG_FILE                            *
***********************************************************************/
BOOLEAN SERVICE_LOG::archive_log_file()
{
    DWORD size_bytes;
    NAME_CLASS n;
    STRING_CLASS s;
    FILE_CLASS f;
    FILETIME file_time;
    TIME_CLASS oldest;
    TIME_CLASS current;
    int i;
    int archive_index;
    int oldest_index;

    log_file.open_for_read( log_path );

    if ( !log_file.get_file_size(size_bytes) )
        return FALSE;  

    if ( size_bytes >= max_file_size )
    {
        /*
        ---------------------------------------------------------
        Set oldest file time to creation time of current log file
        --------------------------------------------------------- */
        log_file.open_for_read( log_path );
        log_file.get_creation_time( &file_time );
        log_file.close();
        oldest.set( file_time );
        oldest_index = 1;

        /*
        -------------------
        Find archive number
        ------------------- */
        for ( i = 1; i < max_num_archives; i++ )
        {
            n = log_path;
            n.remove_extension();
            n += i;
            n += TEXT( "." );
            n += SERVICE_LOG_FILE_NAME_EXT;
            if ( n.file_exists() )
            {
                log_file.open_for_read( n );
                log_file.get_creation_time( &file_time );
                current.set( file_time );
                
                if ( current < oldest )
                {
                    /*
                    ---------------------------------
                    current file is older than oldest
                    --------------------------------- */
                    oldest = current;
                    oldest_index = i;
                }

                log_file.close();
            }
            else
            {
                archive_index = i;
                break;
            }
        }

        if ( i >= max_num_archives )
        {
            archive_index = oldest_index;
        }

        /*
        ----------------------------
        Archive the current log file
        ---------------------------- */
        n = log_path;
        n.remove_extension();
        n += archive_index;
        n += TEXT( "." );
        n += SERVICE_LOG_FILE_NAME_EXT;
        DeleteFile( n.text() );
        MoveFile( log_path.text(), n.text() );
        DeleteFile( log_path.text() );

        return TRUE;
    }
    else
    {
        log_file.close();

        return FALSE;
    }
}

/***********************************************************************
*                             SERVICE_LOG                              *
*                           PRUNE_LOG_FILE                             *
***********************************************************************/
void SERVICE_LOG::prune_log_file()
{
    TCHAR *line;
    TIME_CLASS entry_time;
    TIME_CLASS current_time;
    SYSTEMTIME st;
    STRING_CLASS s;
    static unsigned long log_retention_seconds = ( unsigned long )log_retention_days * SECONDS_PER_DAY;
    FILE_CLASS new_log_file;
    FILE_CLASS f;
    STRING_CLASS new_log_path;
    BOOLEAN status;
    unsigned long dt;
    
    new_log_path = log_path;
    new_log_path += TEXT( ".tmp" );
    new_log_file.open_for_write( new_log_path.text() );
    status = f.open_for_read( log_path ); 
    line = f.readline();

    while ( line )
    { 
        s = line;
        s.next_field( ForwardSlashChar );
        st.wMonth = s.int_value();
        s.next_field( ForwardSlashChar );
        st.wDay = s.int_value();
        s.next_field( ForwardSlashChar );
        st.wYear = s.int_value();
        s.next_field( SpaceChar );
        s.next_field( ColonChar );
        st.wHour = s.int_value();
        s.next_field( ColonChar );
        st.wMinute = s.int_value();
        s.next_field( ColonChar );
        st.wSecond = s.int_value();
        entry_time.set( st );
        current_time.get_local_time();
        
        dt = ( unsigned long )( current_time - entry_time );
        if ( dt < log_retention_seconds )
        {
            new_log_file.writeline( line );
        }

        line = f.readline();
    }

    f.close();
    DeleteFile( log_path.text() );
    new_log_file.close();
    MoveFile( new_log_path.text(), log_path.text() ); 
}