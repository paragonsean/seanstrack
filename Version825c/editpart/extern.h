extern HINSTANCE MainInstance;

extern HWND      MainWindow;
extern HWND      MainDialogWindow;
extern HWND      ParamDialogWindow;

extern BOOLEAN HaveAutoBackupComputer;
extern TCHAR     ConfigSection[];
extern TCHAR     EditPartIniFile[];

#ifdef _FONT_CLASS_
extern FONT_LIST_CLASS FontList;
#endif

#ifdef _MACHINE_CLASS_
extern MACHINE_CLASS   CurrentMachine;
#endif

#ifdef _PART_CLASS_
extern PART_CLASS      CurrentPart;
#endif

#ifdef _VISI_PARAM_H_
extern PARAMETER_CLASS CurrentParam;
#endif

#ifdef _FTANALOG_CLASS_
extern FTANALOG_CLASS  CurrentFtAnalog;
#endif

#ifdef _FTCALC_CLASS_
extern FTCALC_CLASS  CurrentFtCalc;
#endif

#ifdef _LIMIT_SWITCH_CLASS_
extern LIMIT_SWITCH_CLASS CurrentLimitSwitch;
#endif


#ifdef _GENLIST_H
extern GENERIC_LIST_CLASS MachineList;
#endif

#ifdef _STRINGTABLE_CLASS_
extern STRINGTABLE_CLASS StringTable;
#endif


#ifdef _SURETRAK_PARAM_CLASS_
extern SURETRAK_PARAM_CLASS    SureTrakParam;
#endif