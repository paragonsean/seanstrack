/***********************************************************************
* shot_file_has_been_updated and shotparm_has_been_updated refer to    *
* the updates after I get the external parameter file.                 *
*                                                                      *
* If there is no external parameter file then these are not valid and  *
* I set them to true when I set the shot_file_has_been_saved and       *
* shotparm_has_been_saved to true;                                     *
*                                                                      *
* If the external parameters have been read before I save the file     *
* then the update for that file is not needed.                         *
***********************************************************************/
#ifndef _SHOT_SAVE_
#define _SHOT_SAVE_

const unsigned CSV_SHOT_FILE_HAS_BEEN_SAVED             =   1;
const unsigned NEED_TO_POST_NEWSHOT_MESSAGE             =   2;
const unsigned NEED_TO_SEND_NEW_DATA_MESSAGE            =   4;
const unsigned NEED_TO_SEND_PARAMETER_EVENT_STRING      =   8;
const unsigned OUTPUT_FILES_HAVE_BEEN_CREATED           =  16;
const unsigned SHOTPARM_HAS_BEEN_SAVED                  =  32;
const unsigned SHOTPARM_HAS_BEEN_UPDATED                =  64;
const unsigned SHOT_FILE_HAS_BEEN_SAVED                 = 128;
const unsigned SHOT_FILE_HAS_BEEN_UPDATED               = 256;
const unsigned SHOT_SAVE_COMPLETE_MESSAGE_HAS_BEEN_SENT = 512;

const int   MAX_SHOTSAVE_BUFFERS  = 40;
const DWORD SHOTSAVE_WAIT_TIMEOUT = 1005;

class SHOT_BUFFER_ENTRY
    {
    public:
    SHOT_BUFFER_ENTRY();
    ~SHOT_BUFFER_ENTRY();
    EXTERNAL_PARAMETER_STATUS_CLASS  external_parameter_status;
    FTII_FILE_DATA                   f;
    unsigned                         flags;
    bool                             has_alarm;
    bool                             has_warning;
    DOUBLE_ARRAY_CLASS               param_value;
    PROFILE_NAME_ENTRY               pn;
    bool                             save_failed;
    };

#ifdef _MAIN_
BOOLEAN             SaveShotNameFiles = FALSE;
SHOT_BUFFER_ENTRY * BufPtr[MAX_SHOTSAVE_BUFFERS];
HANDLE              ShotSaveEvent    = 0;

#else

extern BOOLEAN   SaveShotNameFiles;
extern HANDLE    ShotSaveEvent;
extern SHOT_BUFFER_ENTRY * BufPtr[MAX_SHOTSAVE_BUFFERS];

#endif
bool add_to_shotsave( bool new_has_alarm, bool new_has_warning, PART_NAME_ENTRY & partname, FTII_FILE_DATA & fsorc, DOUBLE_ARRAY_CLASS & psorc, EXTERNAL_PARAMETER_STATUS_CLASS & external_parameter_status );

#endif
