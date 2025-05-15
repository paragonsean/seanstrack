/*
---------------------------
Types for get_zip_file_path
--------------------------- */
const  int   PART_PATH    = 0;
const  int   MACHINE_PATH = 1;

#ifdef _MAIN_

HINSTANCE MainInstance              = 0;
HWND      MainWindow                = 0;
HWND      MainDialogWindow          = 0;

NAME_CLASS BackupDirectory;
BOOLEAN    SkipDowntimeZip              = FALSE;
BOOLEAN    Using_Machine_Based_Zip_Paths = FALSE;
BOOLEAN    NeedToReloadZipPaths      = FALSE;
BOOLEAN    HaveMirror                = FALSE;
int        ParamLinesPerFile         = 0;

NAME_CLASS DefaultMachineZipPath;
NAME_CLASS DefaultPartZipPath;
NAME_CLASS MirrorRoot;            /* Keep a second copy of the backup files here */
NAME_CLASS DsBackupIniPath;
NAME_CLASS VisiTrakIniPath;

TCHAR DsBackupIniFile[]        = TEXT( "dsbackup.ini" );
TCHAR VisiTrakIniFile[]        = TEXT( "visitrak.ini" );
TCHAR MachinePartFile[]        = TEXT( "machinepart.txt" );

TCHAR BackupMachinePathString[]= TEXT( "BackupMachinePath" );
TCHAR BackupPartPathString[]   = TEXT( "BackupPartPath" );

TCHAR BackupSection[]          = TEXT( "Backup" );
TCHAR ConfigSection[]          = TEXT( "Config" );
TCHAR DefaultSection[]         = TEXT( "Default" );
TCHAR ShotparmSection[]        = TEXT( "Shotparm" );

TCHAR MachineBasedZipPathsKey[]= TEXT( "MachineBasedZipPaths" );
TCHAR RecordFormatKey[]        = TEXT( "RecordFormat" );
TCHAR FileNameKey[]            = TEXT( "FileName" );
TCHAR HaveAlarmShotKey[]       = TEXT( "HaveAlarmShot" );
TCHAR HaveGoodShotKey[]        = TEXT( "HaveGoodShot" );
TCHAR HaveMirrorKey[]          = TEXT( "HaveMirror" );
TCHAR HaveWarningShotKey[]     = TEXT( "HaveWarningShot" );
TCHAR MirrorRootKey[]          = TEXT( "MirrorRoot" );
TCHAR ParamLinesPerFileKey[]   = TEXT( "ParamLinesPerFile" );

TCHAR * HaveAlarmShot   = 0;
TCHAR * HaveGoodShot    = 0;
TCHAR * HaveWarningShot = 0;
DS_BACKUP_CLASS BList;

#else

extern HINSTANCE  MainInstance;
extern HWND       MainWindow;
extern HWND       MainDialogWindow;
extern NAME_CLASS BackupDirectory;
extern BOOLEAN    SkipDowntimeZip;
extern BOOLEAN    Using_Machine_Based_Zip_Paths;
extern BOOLEAN    NeedToReloadZipPaths;
extern BOOLEAN    HaveMirror;
extern int        ParamLinesPerFile;

extern NAME_CLASS DefaultMachineZipPath;
extern NAME_CLASS DefaultPartZipPath;
extern NAME_CLASS MirrorRoot;
extern NAME_CLASS DsBackupIniPath;
extern NAME_CLASS VisiTrakIniPath;

extern TCHAR DsBackupIniFile[];
extern TCHAR VisiTrakIniFile[];
extern TCHAR MachinePartFile[];

extern TCHAR BackupMachinePathString[];
extern TCHAR BackupPartPathString[];

extern TCHAR BackupSection[];
extern TCHAR ConfigSection[];
extern TCHAR DefaultSection[];
extern TCHAR ShotparmSection[];

extern TCHAR MachineBasedZipPathsKey[];
extern TCHAR RecordFormatKey[];
extern TCHAR FileNameKey[];

extern TCHAR * HaveAlarmShot;
extern TCHAR * HaveGoodShot;
extern TCHAR * HaveWarningShot;
#ifdef _DS_BACKUP_CLASS_
extern DS_BACKUP_CLASS BList;
#endif

#endif
