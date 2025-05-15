#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

static const DWORD MS_TO_WAIT         = 200;
static const int32 WAIT_TIMEOUT_COUNT = 10;
static TCHAR BackslashChar = TEXT( '\\' );
static DB_TABLE T;

/***********************************************************************
*                        GET_SHOTSAVE_SEMAPHORE                        *
***********************************************************************/
BOOLEAN get_shotsave_semaphore( STRING_CLASS & computer, STRING_CLASS & machine )
{
NAME_CLASS s;

if ( T.open_status() )
    return TRUE;

s.get_machine_dir_file_name( computer, machine, SEMAPHOR_DB );
if ( !s.file_exists()  )
    T.create( s );

return T.open( s, SEMAPHOR_RECLEN, FL, NO_RETRIES );
}

/***********************************************************************
*                       FREE_SHOTSAVE_SEMAPHORE                        *
***********************************************************************/
void free_shotsave_semaphore()
{
T.close();
}

/***********************************************************************
*                    WAITFOR_SHOTSAVE_SEMAPHOR                         *
***********************************************************************/
BOOLEAN waitfor_shotsave_semaphor( STRING_CLASS & computer, STRING_CLASS & machine )
{
int32  count;
HANDLE myevent;

/*
---------------------------------------------------
Get a full lock on the semaphor db for this machine
--------------------------------------------------- */
count   = 0;
myevent = 0;
while ( !get_shotsave_semaphore(computer, machine) )
    {
    if ( !myevent )
        {
        /*
        ------------------------------------------
        Create an event that I can use for a timer
        ------------------------------------------ */
        myevent = CreateEvent( NULL, FALSE, FALSE, NULL );
        if ( !myevent )
            return FALSE;
        }

    WaitForSingleObject( myevent, MS_TO_WAIT );
    count++;
    if ( count > WAIT_TIMEOUT_COUNT )
        {
        CloseHandle( myevent );
        return FALSE;
        }
    }

if ( myevent )
    CloseHandle( myevent );

return TRUE;
}
