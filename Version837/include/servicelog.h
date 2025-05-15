#ifndef _SERVICE_LOG_CLASS_
#define _SERVICE_LOG_CLASS_

#ifndef _FILE_CLASS_
#include "..\include\fileclas.h"
#endif

#define LOG_LEVEL_NONE      0
#define LOG_LEVEL_WARNING   1
#define LOG_LEVEL_ERROR     2
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_SERVICE   4
#define LOG_LEVEL_DEBUG     5
#define MAX_NUM_ARCHIVES    100

class SERVICE_LOG
{
    public: 
        SERVICE_LOG();
        void set_log_file( STRING_CLASS &filepath );
        void set_log_level( int level );
        void set_log_retention( int days );
        void set_max_file_size( int kilo_bytes );
        void set_max_num_archives( int num );
        void add_entry( STRING_CLASS &msg, int level=LOG_LEVEL_SERVICE );

    private:
        FILE_CLASS log_file;
        NAME_CLASS log_path;
        int log_level;
        int log_retention_days;
        void prune_log_file();
        BOOLEAN archive_log_file();
        DWORD max_file_size;        /* bytes */
        int max_num_archives;
        STRING_CLASS last_msg;
};

#endif