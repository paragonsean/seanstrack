#ifndef _AUTO_BACKUP_CLASS_
#define _AUTO_BACKUP_CLASS_

class AUTO_BACKUP_CLASS
    {
    private:
    BOOLEAN  HaveAutoBackupComputer;
    STRING_CLASS AutoBackupComputer;

    public:

    AUTO_BACKUP_CLASS();
   ~AUTO_BACKUP_CLASS();
    BOOLEAN exists();
    STRING_CLASS & dir();
    BOOLEAN get_backup_dir();
    BOOLEAN put_backup_dir();
    void    set_backup_dir( STRING_CLASS & sorc );
    BOOLEAN backup_data_dir();
    BOOLEAN backup_exe_dir();
    BOOLEAN backup_machine( STRING_CLASS & m );
    BOOLEAN backup_part( STRING_CLASS & m, STRING_CLASS & p );
    };
#endif
