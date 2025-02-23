#ifndef _DS_BACKUP_CLASS_
#define _DS_BACKUP_CLASS_

#ifndef _STRUCTS_
#include "..\include\structs.h"
#endif

#ifndef _GENLIST_H
#include "..\include\genlist.h"
#endif

#ifndef _TIME_CLASS_
#include "..\include\timeclas.h"
#endif

#ifndef _SHOT_CLASS_
#include "..\include\shotclas.h"
#endif

struct DS_SAVE_COUNTS
    {
    SHOT_CLASS last_saved_shot;
    int32      current_count;
    int32      skip_count;
    int32      acquire_count;
    int32      skip_seconds;
    BITSETYPE  flags;
    BOOLEAN    acquiring;

    void    clear();
    BOOLEAN need_to_save( SHOT_CLASS sorc );
    void    operator=( const DS_SAVE_COUNTS & sorc );
    TCHAR * text();
    };

class DS_BACKUP_ENTRY
    {
    public:

    PART_NAME_ENTRY name;
    BOOLEAN         this_is_the_warmup_part;
    STRING_CLASS    current_part_before_this_part;
    TIME_CLASS      last_downtime;
    STRING_CLASS    last_examined_shot_name;
    int32           zip_count;
    NAME_CLASS      part_zip_file_path;
    NAME_CLASS      machine_zip_file_path;
    DS_SAVE_COUNTS  profile;
    DS_SAVE_COUNTS  params;

    DS_BACKUP_ENTRY();
    ~DS_BACKUP_ENTRY();
    void    operator=( const DS_BACKUP_ENTRY & sorc );
    BOOLEAN read();
    BOOLEAN read_part_config();
    BOOLEAN read_save_config();
    BOOLEAN read_zip_paths();
    void    start_new_part( TCHAR * newpart );
    void    zip_shot_data();
    void    zip_machine_data();
    void    zip_all_data();
    void    copy_profiles( SHOT_CLASS & shot );
    void    copy_parameters( SHOT_CLASS & shot );
    void    copy_alarms( SHOT_CLASS & shot );
    BOOLEAN copy_setup_files_for_one_part();
    void    check_for_new_shot( TCHAR * current_part );
    void    check_for_new_downtime();
    void    write();
    BOOLEAN write_part_config();
    BOOLEAN write_save_config();
    void    write_zip_paths();
    };

class DS_BACKUP_CLASS
    {
    private:

    GENERIC_LIST_CLASS backuplist;

    public:

    DS_BACKUP_ENTRY  * current_entry;

    DS_BACKUP_CLASS();
    ~DS_BACKUP_CLASS();
    void    operator=( DS_BACKUP_CLASS & sorc );
    BOOLEAN add( DS_BACKUP_ENTRY * b );
    void    clear_max_zip_count();
    int32   count() { return (int32) backuplist.count(); }
    void    empty();
    BOOLEAN find( TCHAR * machine_to_find );
    inline BOOLEAN find( STRING_CLASS & machine_to_find ) { return find( machine_to_find.text() ); }
    void    load();
    BOOLEAN next();
    void    reload_save_configs();
    void    reload_zip_paths();
    void    rewind();
    void    set_max_zip_count( int32 new_count );
    void    shutdown();
    void    process_mailslot_message( TCHAR * msg );
    void    zip_all_parts();
    };

#endif
