#ifndef _EVENTS_H
#define _EVENTS_H

#define SOCKET_MSG_BUFLEN 512

#define NEW_MACH_STATE_INDEX    0
#define SHOT_NUMBER_INDEX       1
#define MONITOR_SETUP_INDEX     2
#define PART_SETUP_INDEX        3
#define ALARM_INDEX             4
#define MACH_SETUP_INDEX        5
#define DOWN_DATA_INDEX         6
#define PW_LEVEL_INDEX          7
#define PLOT_MIN_INDEX          8
#define DS_NOTIFY_INDEX         9
#define COMPUTER_SETUP_INDEX    10
#define SHUTDOWN_INDEX          11
#define MONITOR_STARTUP_INDEX   12
#define DEFAULT_TOPIC           TEXT("DEF")
#define VISITRAK_EVENT_MANAGER  TEXT( "EventMan" )

/*
-----------------
eventman commands
----------------- */
#define NO_EVENTMAN_COMMAND     0
#define REGISTER_FOR_ITEM       1
#define POKE_DATA_ITEM          2
#define UNREGISTER_FOR_ITEM     3
#define REQUEST_DATA_ITEM       4
#define UNREGISTER_CLIENT       5

/*
-----------------------
Eventman socket message
----------------------- */
#define MAX_DDE_DEFAULT_DATA_LEN 25
#define MAX_DDE_DEFAULT_ITEM_LEN 16
#define DEFAULT_BUFLEN 512

class SOCKET_MSG
{
public:
    int cmd;
    int item;
    TCHAR topic[SOCKET_MSG_BUFLEN];
    TCHAR data[SOCKET_MSG_BUFLEN];

    SOCKET_MSG();
    SOCKET_MSG( const SOCKET_MSG & sorc );
    ~SOCKET_MSG();
    void operator=( const SOCKET_MSG & sorc );
};

struct ITEM_ENTRY
    {
    TCHAR   name[MAX_DDE_DEFAULT_ITEM_LEN+1];
    TCHAR   default_value[MAX_DDE_DEFAULT_DATA_LEN+1];
    HSZ     handle;
    };

#define NOF_EVENT_ITEMS 13

#ifdef _MAIN_

int NofItems = 13;

ITEM_ENTRY ItemList[] = {
    { TEXT("NEW_MACH_STATE"), NO_MACHINE TEXT("\t0000\t0000\015\012"), 0 },
    { TEXT("SHOT_NUMBER"),    TEXT("1\015\012"),                       0 },
    { TEXT("MONITOR_SETUP"),  NO_MACHINE TEXT("\015\012"),             0 },
    { TEXT("PART_SETUP"),     NO_PART,                                 0 },
    { TEXT("PARAM_ALARM"),    TEXT("0\015\012"),                       0 },  /* Parameter Number [1,60] */
    { TEXT("MACH_SETUP"),     NO_MACHINE TEXT("\015\012"),             0 },
    { TEXT("DOWN_DATA"),      NO_MACHINE TEXT("\015\012"),             0 },
    { TEXT("PW_LEVEL"),       TEXT("0\015\012"),                       0 },
    { TEXT("PLOT_MIN"),       TEXT("0"),                               0 },
    { TEXT("DS_NOTIFY"),      TEXT("0"),                               0 },
    { TEXT("COMPUTER_SETUP"), TEXT("0\015\012"),                       0 },
    { TEXT("SHUTDOWN"),       TEXT("\0"),                              0 },
    { TEXT("STARTUP"),        TEXT("\0"),                              0 }
    };
#else

extern int NofItems;
extern ITEM_ENTRY ItemList[];

#endif

#endif