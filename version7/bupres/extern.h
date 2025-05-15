const BOOL OVERWRITE_EXISTING = FALSE;

#ifdef _MAIN_

HINSTANCE    MainInstance;
HWND         MainWindow;
int          NextShotNumber = 1;
STRING_CLASS BackupComputer;  // No ending backslash
NAME_CLASS   BackupDir;       // BackupComputer + backslash
short        HistoryShotCount = 50;
BOOLEAN      HaveSureTrakControl = FALSE;
BOOLEAN      CreatedNewMachine   = FALSE;
BOOLEAN      BackupAllTextFiles  = FALSE;
MACHINE_NAME_LIST_CLASS MachList;

TCHAR BackupSection[]    = TEXT( "Backup" );
TCHAR ConfigSection[]    = TEXT( "Config" );
TCHAR RDLoadZipDirKey[]  = TEXT( "RDLoadZipDir" );
TCHAR VisiTrakIniFile[]  = TEXT( "visitrak.ini" );

#else

extern HINSTANCE    MainInstance;
extern HWND         MainWindow;
extern int          NextShotNumber;
extern STRING_CLASS BackupComputer;
extern NAME_CLASS   BackupDir;
extern short        HistoryShotCount;
extern BOOLEAN      HaveSureTrakControl;
extern BOOLEAN      CreatedNewMachine;
extern BOOLEAN      BackupAllTextFiles;

#ifdef _MACHINE_NAME_LIST_CLASS_
extern MACHINE_NAME_LIST_CLASS MachList;
#endif
extern TCHAR BackupSection[];
extern TCHAR ConfigSection[];
extern TCHAR RDLoadZipDirKey[];
extern TCHAR VisiTrakIniFile[];

#endif
