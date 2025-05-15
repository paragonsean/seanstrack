#ifndef _SHOT_SAVE_
#define _SHOT_SAVE_

const int   MAX_SHOTSAVE_BUFFERS  = 20;
const DWORD SHOTSAVE_WAIT_TIMEOUT = 1005;

struct SHOT_BUFFER_ENTRY
    {
    STRING_CLASS   machine;
    STRING_CLASS   part;
    PROFILE_HEADER ph;
    BOOLEAN        save_failed;
    BOOLEAN        has_alarm;
    short          nof_parameters;
    float          parameters[MAX_PARMS];
    short          nof_curves;
    unsigned short points[MAX_CURVES][MAX_POINTS];
    };

#ifdef _MAIN_

SHOT_BUFFER_ENTRY * BufPtr[MAX_SHOTSAVE_BUFFERS];
HANDLE              ShotSaveEvent    = 0;

#else

extern HANDLE    ShotSaveEvent;
extern SHOT_BUFFER_ENTRY * BufPtr[MAX_SHOTSAVE_BUFFERS];

#endif
BOOLEAN add_to_shotsave(STRING_CLASS & ma, STRING_CLASS & pa, PROFILE_HEADER & new_ph, float new_points[MAX_CURVES][MAX_POINTS], float * new_parameters, short nof_new_parameters, BOOLEAN has_alarm );

#endif
