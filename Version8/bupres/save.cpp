#include <windows.h>
#include <process.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\chars.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\stringtable.h"

#include "extern.h"
#include "resource.h"

#define _MAIN_
#include "save.h"

HWND    ProgressWindow = 0;
BOOLEAN HaveProgressCancel = FALSE;
BOOLEAN UseBackupListFile  = FALSE;

void backup_data( void );
void restore_data( void );
void delete_data(  void );
void fill_machine_list( void );
void st_resource_message_box( TCHAR * msg_id, TCHAR * title_id );

/***********************************************************************
*                        READ_BACKUP_LIST_FILE                         *
*                                                                      *
* The backup list file came after the SaveList. In order to keep       *
* things simple I will make a SaveList even though the data is         *
* also in the backup list. The only change, then, will be to tell      *
* the backup_part program to use the shots in BackupList.              *
*                                                                      *
***********************************************************************/
static void read_backup_list_file()
{
FILE_CLASS f;
NAME_CLASS s;
STRING_CLASS pn;
int          i;
int          n;

s.get_backup_list_csvname();
if ( !s.file_exists() )
    return;

if ( f.open_for_read(s.text()) )
    {
    n = f.nof_lines();
    if ( n > 0 )
        {
        BackupList = new PROFILE_NAME_ENTRY[n];
        if ( BackupList )
            {
            i = 0;
            f.rewind();
            while ( true )
                {
                s = f.readline();
                if ( s.countchars(CommaChar) == 3  )
                    {
                    s.next_field();
                    BackupList[i].computer = s;

                    s.next_field();
                    BackupList[i].machine = s;


                    s.next_field();
                    BackupList[i].part = s;

                    s.next_field();
                    BackupList[i].shot = s;
                    i++;
                    }
                else
                    {
                    break;
                    }
                if ( i == n )
                    break;
                }
            BackupListCount = i;
            }
        }
    f.close();
    }

n = 0;
s.null();
for ( i=0; i<BackupListCount; i++ )
    {
    if ( s != BackupList[i].machine )
        {
        s = BackupList[i].machine;
        if ( !s.isempty() )
            n++;
        }
    }

s = EmptyString;
if ( n > 0 )
    {
    NtoSave = -1;
    SaveList = new SAVE_LIST_ENTRY[n];
    if ( SaveList )
        {
        for ( i=0; i<BackupListCount; i++ )
            {
            if ( s != BackupList[i].machine )
                {
                if ( !pn.isempty() )
                    pn = EmptyString;
                s = BackupList[i].machine;
                if ( !s.isempty() )
                    {
                    NtoSave++;
                    SaveList[NtoSave].name.set( BackupList[i].computer, BackupList[i].machine );
                    }
                }
            if ( NtoSave >= 0 )
                {
                if ( pn != BackupList[i].part )
                    {
                    pn = BackupList[i].part;
                    SaveList[NtoSave].part.append( pn.text() );
                    }
                }
            }
        }
    /*
    -----------------------------------
    Convert NtoSave from index to count
    ----------------------------------- */
    NtoSave++;
    }
}

/***********************************************************************
*                             THREAD_FUNC                              *
***********************************************************************/
void thread_func( void * not_used  )
{
INT   * ip;
INT     i;
INT     nof_parts;
TCHAR * cp;
NAME_CLASS      s;
TEXT_LIST_CLASS t;

SaveList = 0;

if ( BackupAllTextFiles )
    {
    backup_data();
    BackupAllTextFiles = FALSE;
    }
else if ( UseBackupListFile )
    {
    read_backup_list_file();
    if ( BackupList && SaveList )
        {
        StringTable.find( s, TEXT("BACKING_UP") );
        s.set_text( ProgressWindow );
        backup_data();
        }
    UseBackupListFile  = FALSE;
    if ( BackupList )
        {
        delete[] BackupList;
        BackupList = 0;
        }
    BackupListCount = 0;
    }
else
    {
    NtoSave = MachineBox.get_select_list( &ip );
    if ( NtoSave > 0 )
        {
        SaveList = new SAVE_LIST_ENTRY[NtoSave];
        if ( !SaveList )
            {
            st_resource_message_box( TEXT("NO_MEMORY_STRING"), TEXT("SAVELIST") );
            return;
            }

        i = 0;
        while ( i < NtoSave )
            {
            s = MachineBox.item_text( ip[i] );

            if ( BackupType & DO_RESTORE )
                {
                SaveList[i].name.computer = BackupComputer;
                SaveList[i].name.machine  = s;
                i++;
                }
            else if ( MachList.find(s) )
                {
                SaveList[i].name.set( MachList.computer_name(), s );
                i++;
                }
            else
                {
                MessageBox( NULL, StringTable.find(TEXT("NO_MA")), s.text(), MB_OK | MB_SYSTEMMODAL );
                NtoSave--;
                }
            }

        delete[] ip;
        ip = 0;
        }

    if ( NtoSave > 0 )
        {
        if ( NtoSave == 1 )
            {
            nof_parts = PartBox.get_select_list( &ip );
            if ( nof_parts > 0 )
                {
                for ( i=0; i<nof_parts; i++ )
                    {
                    if ( HaveProgressCancel )
                        break;
                    /*
                    --------------------------------------------
                    The part name is followed by tab+shot number
                    -------------------------------------------- */
                    s = PartBox.item_text( ip[i] );
                    cp = s.find( TabChar );
                    if ( cp )
                        *cp = NullChar;
                    SaveList[0].part.append( s.text() );
                    }
                delete[] ip;
                ip = 0;
                }
            }
        else
            {
            /*
            ----------------------------------
            Copy all the parts in each machine
            ---------------------------------- */
            for ( i=0; i<NtoSave; i++ )
                get_partlist( SaveList[i].part, SaveList[i].name.computer, SaveList[i].name.machine );
            }

        if ( BackupType & DO_BACKUP )
            {
            StringTable.find( s, TEXT("BACKING_UP") );
            s.set_text( ProgressWindow );
            backup_data();
            }

        if ( BackupType & DO_RESTORE )
            {
            StringTable.find( s, TEXT("RESTORING") );
            s.set_text( ProgressWindow );
            restore_data();
            }

        if ( BackupType & DO_DELETE )
            {
            StringTable.find( s, TEXT("DELETING...") );
            s.set_text( ProgressWindow );
            delete_data();
            }
        }
    }

if ( SaveList )
    {
    for ( i=0; i<NtoSave; i++ )
        SaveList[i].part.empty();
    delete[] SaveList;
    SaveList = 0;
    NtoSave  = 0;
    }

if ( ProgressWindow )
    SendMessage( ProgressWindow, WM_CLOSE, 0, 0L );
}

/***********************************************************************
*                         START_BACKUP_THREAD                          *
***********************************************************************/
static void start_backup_thread( void )
{
unsigned long h;

h = _beginthread( thread_func, 0, NULL );
if ( h == -1 )
    st_resource_message_box( TEXT("THREAD_FAIL"), TEXT("CANNOT_COMPLY_STRING") );
}

/***********************************************************************
*                       SAVING PROGRESS PROC                           *
***********************************************************************/
BOOL CALLBACK SavingProgressProc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ProgressWindow = w;
        HaveProgressCancel = FALSE;
        start_backup_thread();
        return TRUE;

    case WM_CLOSE:
        if ( BackupWindow )
            SendMessage( BackupWindow, WM_DB_SAVE_DONE, 0, 0L );
        ProgressWindow = 0;
        EndDialog( w, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVING_CANCEL_BUTTON:
                HaveProgressCancel = TRUE;
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                                 SAVE                                 *
***********************************************************************/
void save( BITSETYPE backup_type, HWND w, int machine_listbox_id, int part_listbox_id )
{
BackupType   = backup_type;
BackupWindow = w;

if ( w )
    {
    MachineBox.init( w, machine_listbox_id );
    PartBox.init(    w, part_listbox_id    );
    }

DialogBox( MainInstance, TEXT("SAVING_PROGRESS"), w, (DLGPROC) SavingProgressProc );
}

/***********************************************************************
*                                 SAVE                                 *
***********************************************************************/
void save( BITSETYPE backup_type, HWND w, int machine_listbox_id, int part_listbox_id, int shots_listbox_id )
{
ShotBox.init( w, shots_listbox_id );
save( backup_type, w, machine_listbox_id, part_listbox_id );
}

/***********************************************************************
*                                 SAVE                                 *
*                                                                      *
*                    This is a backup to a zip file.                   *
*                                                                      *
***********************************************************************/
void save( NAME_CLASS & zip_file_name, BITSETYPE backup_type, HWND w, int machine_listbox_id, int part_listbox_id )
{
BackupZipFile = zip_file_name;

/*
-------------------------------
Make sure there is an extension
------------------------------- */
if ( !BackupZipFile.contains(PeriodChar) )
    BackupZipFile += ZIP_SUFFIX;

save( backup_type, w, machine_listbox_id, part_listbox_id );
}

/***********************************************************************
*                           SAVE_BACKUP_LIST                           *
***********************************************************************/
void save_backup_list( TCHAR * zip_file_name, BITSETYPE backup_type )
{
UseBackupListFile  = TRUE;
BackupZipFile = zip_file_name;
save( backup_type, 0, 0, 0 );
}

/***********************************************************************
*                          SAVE_ALL_TEXT_FILES                         *
***********************************************************************/
void save_all_text_files( TCHAR * zip_file_name  )
{
const BITSETYPE MY_BACKUP_TYPE = 0;

BackupAllTextFiles = TRUE;
BackupZipFile = zip_file_name;
save( MY_BACKUP_TYPE, 0, 0, 0 );
}
