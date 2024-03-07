struct MONITOR_ENTRY
     {
     BOOLEAN        has_changed;
     MACHINE_CLASS  machine;
     PART_CLASS     part;
     STRING_CLASS   display_seconds;
     };

#ifdef _MAIN_

HINSTANCE MainInstance;
HWND      MainWindow;

HWND      MonitorWindow = 0;

STRING_CLASS ComputerName;
STRING_CLASS NewSetupMachine;

TCHAR     NoComputer[] = NO_COMPUTER;
TCHAR     NoMachine[]  = NO_MACHINE;
BOOLEAN   ShuttingDown;

/*
---------------------------------------------
The following are changed ONLY in monitor.cpp
--------------------------------------------- */
MONITOR_ENTRY * MachineList = 0;
int             NofMachines;
int             CurrentMachineIndex = NO_INDEX;

#else

extern HINSTANCE MainInstance;
extern HWND      MainWindow;
extern HWND      MonitorWindow;
extern STRING_CLASS ComputerName;
extern STRING_CLASS NewSetupMachine;
extern TCHAR     NoComputer[];
extern TCHAR     NoMachine[];
extern BOOLEAN   ShuttingDown;

/*
---------------------------------------------
The following are changed ONLY in monitor.cpp
--------------------------------------------- */
extern MONITOR_ENTRY * MachineList;
extern int             NofMachines;
extern int             CurrentMachineIndex;

#endif

