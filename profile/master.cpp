#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

#include "extern.h"
#include "resource.h"

/***********************************************************************
*                         SAVE_AS_MASTER_TRACE                         *
*       Save the currently displayed trace as the master trace.        *
***********************************************************************/
void save_as_master_trace()
{
AUTO_BACKUP_CLASS ab;
COMPUTER_CLASS    c;
NAME_CLASS dest;
NAME_CLASS sorc;

sorc.get_profile_name( ComputerName, MachineName, PartName, ShotName );
dest.get_master_name( ComputerName, MachineName, PartName );
if ( !sorc.file_exists() )
    {
    /*
    -----------------------------------------------
    I'm going to save a fastrak2 file master trace.
    Delete the old master trace if there is one
    ----------------------------------------------- */
    if ( dest.file_exists() )
        dest.delete_file();
    sorc.get_profile_ii_name( ComputerName, MachineName, PartName, ShotName );
    dest.get_master_ii_name( ComputerName, MachineName, PartName  );
    }
if ( dest.file_exists() )
    {
    if ( resource_message_box(MainInstance, REPLACE_MASTER_STRING, MASTER_EXISTS_STRING, MB_SYSTEMMODAL | MB_YESNO | MB_ICONQUESTION) == IDNO )
        return;
    }

sorc.copyto( dest );

if ( c.find(ComputerName) )
    {
    if ( c.is_this_computer() )
        {
        ab.get_backup_dir();
        ab.backup_part( MachineName, PartName );
        }
    }
}
