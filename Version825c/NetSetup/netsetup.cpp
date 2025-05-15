#include <windows.h>
#include <commctrl.h>
#include <htmlhelp.h>
#include <winnetwk.h>   /* link to mpr.lib */
#include <shlobj.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\rectclas.h"
#include "..\include\strarray.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "msres.h"

#define _MAIN_
#include "..\include\chars.h"

static BOOLEAN CancelAddNewComputers = FALSE;
static BOOLEAN Starting = FALSE; /* Used by set_computer_listbox_size */

/*
-------------------------------------
Global for RENAME_NEW_COMPUTER_DIALOG
------------------------------------- */
static STRING_CLASS NewComputerName;

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS ComputerWindow;

RECTANGLE_CLASS MainClientRect;

TCHAR MyClassName[] = TEXT("NetSetup");

const int VERSION_LEN               = 7; /* 6.10A or unknown */

const int NO_COMPUTER_TYPE          = 0;
const int MONITORING_COMPUTER_TYPE  = 1;
const int WORKSTATION_COMPUTER_TYPE = 2;
const int DATA_SERVER_COMPUTER_TYPE = 4;

static TCHAR ConfigSection[]      = TEXT( "Config" );
static TCHAR DataServerKey[]      = TEXT( "DataServer" );
static TCHAR ThisComputerKey[]    = TEXT( "ThisComputer" );
static TCHAR VersionKey[]         = TEXT( "Version" );

static TCHAR DataDirString[]      = TEXT( "\\data\\" );
static TCHAR DataString[]         = TEXT( "data" );
static TCHAR EllipsisString[]     = TEXT( "..." );
const  int   EllipsisStringLen    = sizeof(EllipsisString)/sizeof(TCHAR);
static TCHAR NetDirPrefixString[] = TEXT( "\\\\" );
static TCHAR NoComputerName[]     = NO_COMPUTER;
static TCHAR VisiTrakIniFile[]    = TEXT( "\\exes\\visitrak.ini");
static TCHAR UnknownString[]      = UNKNOWN;
static TCHAR NetSetupDataFile[]   = TEXT( "NetSetup.dat" );
static TCHAR CommaSeparator[]     = TEXT( "," );
static TCHAR DataFileSeparator[]  = TEXT( ";" );
static TCHAR V5Ext[]              = TEXT( "\\V5" );
static TCHAR V5dsExt[]            = TEXT( "\\V5ds" );

static STRING_CLASS YesName;
static STRING_CLASS NoName;
static BOOLEAN      Updating = FALSE;

/*
-------------------------------------------------------------------------
The following two widths are used to limit the length of the connect list
------------------------------------------------------------------------- */
static int          AverageCharacterWidth = 5;
static int          MaxConnectedListLen   = 20;   /* Characters */

class NETWORK_COMPUTER_CLASS
    {
    public:

    NAME_CLASS      local;
    NAME_CLASS      net;
    STRING_CLASS    name;
    TCHAR           version[VERSION_LEN+1];
    int             type;
    bool            is_changed;
    bool            online;
    bool            is_shared_full;
    TEXT_LIST_CLASS connectlist;

    NETWORK_COMPUTER_CLASS() { (*version)=NullChar; type = NO_COMPUTER_TYPE; is_changed = false; is_shared_full=false; online=false; }
    NETWORK_COMPUTER_CLASS( NETWORK_COMPUTER_CLASS & sorc );
    ~NETWORK_COMPUTER_CLASS() {}
    void    operator=( NETWORK_COMPUTER_CLASS & sorc );
    BOOLEAN extract_type_from_name();
    BOOLEAN is_connectable( NETWORK_COMPUTER_CLASS & dest );
    BOOLEAN lbline( STRING_CLASS & dest );
    void    cleanup();
    bool    get_online_status();
    void    get_connect_list();
    void    save();
    void    save_data_server_connection();
    void    save_data_server_connection( TCHAR * name_to_save );
    void    remove_data_server_connection();
    };

class NETWORK_COMPUTER_LIST_CLASS
    {
    private:

    static NETWORK_COMPUTER_CLASS empty_entry;

    int                      x;
    NETWORK_COMPUTER_CLASS * nc;
    GENERIC_LIST_CLASS       clist;

    public:

    NETWORK_COMPUTER_LIST_CLASS() { nc = &empty_entry; x = -1; }
    ~NETWORK_COMPUTER_LIST_CLASS();
    NETWORK_COMPUTER_CLASS & entry() { return *nc; }
    void    operator=( NETWORK_COMPUTER_LIST_CLASS & sorc );
    BOOLEAN find( int index_to_find );
    BOOLEAN find( TCHAR * computer_to_find );
    BOOLEAN find( STRING_CLASS computer_to_find ) { return find( computer_to_find.text() ); }
    BOOLEAN find( NETWORK_COMPUTER_CLASS & computer_to_find );
    BOOLEAN netfind( STRING_CLASS net_to_find );
    BOOLEAN add( NETWORK_COMPUTER_CLASS * nc_to_add );
    BOOLEAN remove();
    void    remove_all();
    void    rewind() { clist.rewind(); nc = &empty_entry; x = -1; }
    BOOLEAN next();
    BOOLEAN read();
    BOOLEAN write();
    int     count() { return (int) clist.count(); }
    int     current_index() { return x; }
    };

NETWORK_COMPUTER_CLASS NETWORK_COMPUTER_LIST_CLASS::empty_entry;

NETWORK_COMPUTER_LIST_CLASS ComputerList;
NETWORK_COMPUTER_LIST_CLASS SavedList;

/***********************************************************************
*                       CONVERT_TO_MAILSLOT_NAME                       *
* The mailslot name is really just the network name of the computer    *
* such as \\ws0. You pass this a directory and it makes sure there     *
* are two backslashes at the start and then nullifies the next         *
* backslash that it finds. The result is what is stored in the         *
* RemoteMailslots list.                                                *
***********************************************************************/
bool convert_to_mailslot_name( TCHAR * s )
{
if ( *s != BackSlashChar )
    return false;
s++;

if ( *s != BackSlashChar )
    return false;

s++;

while ( *s != BackSlashChar && *s != NullChar )
    s++;

*s = NullChar;
return true;
}

/***********************************************************************
*                    EXTRACT_COMPUTER_NAME_FROM_DIR                    *
***********************************************************************/
static bool extract_computer_name_from_dir( STRING_CLASS & dest, TCHAR * sorc )
{
STRING_CLASS s;
TCHAR * bp;
TCHAR * ep;

s = sorc;
bp = s.text();

if ( *bp != BackSlashChar || *bp == NullChar )
    return false;

while ( *bp == BackSlashChar )
    bp++;

if ( *bp == NullChar )
    return false;

ep = bp;
while ( *ep != BackSlashChar && *ep != NullChar )
    ep++;
*ep = NullChar;
dest = bp;

return true;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               FIND                                   *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::find( int index_to_find )
{
rewind();
while ( next() )
    {
    if ( x == index_to_find )
        return TRUE;
    }

return FALSE;
}
/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                             NETFIND                                  *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::netfind( STRING_CLASS net_to_find )
{
if ( net_to_find.isempty() )
    return FALSE;

rewind();
while ( next() )
    {
    if ( nc->net == net_to_find )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               FIND                                   *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::find( TCHAR * computer_to_find )
{
if ( !computer_to_find )
    return FALSE;

if ( *computer_to_find == NullChar )
    return FALSE;

if ( strings_are_equal(computer_to_find, NoComputerName) )
    return FALSE;

rewind();
while ( next() )
    {
    if ( nc->name == computer_to_find )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               FIND                                   *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::find( NETWORK_COMPUTER_CLASS & computer_to_find )
{
return find( computer_to_find.name.text() );
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               NEXT                                   *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::next()
{
NETWORK_COMPUTER_CLASS * e;

e = (NETWORK_COMPUTER_CLASS *) clist.next();
if ( e )
    {
    x++;
    nc = e;
    return TRUE;
    }

return FALSE;
}

/**********************************************************************
*                            FIX_V6_PATH                              *
*                  Convert c:\v5\data\c01 to c:\v5                    *
**********************************************************************/
static void fix_v6_path( TCHAR * sorc )
{
int     n;

n = 0;

while ( *sorc != NullChar )
    {
    if ( *sorc == BackSlashChar )
        {
        if ( n > 0 )
            {
            *sorc = NullChar;
            return;
            }
        else
            {
            n++;
            }
        }
    sorc++;
    }
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                              READ                                    *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::read()
{
FILE_CLASS               f;
bool                     is_v6_format;
int                      n;
NETWORK_COMPUTER_CLASS * ncc;
STRING_ARRAY_CLASS       s;
STRING_ARRAY_CLASS       v6name;
STRING_ARRAY_CLASS       v7name;

s = exe_directory();
s.cat_path( NetSetupDataFile );

is_v6_format = false;
n = 0;

if ( f.open_for_read(s.text()) )
    {
    while ( true )
        {
        s = f.readline();
        if ( s.len() == 0 )
            break;

        ncc = new NETWORK_COMPUTER_CLASS;
        if ( !ncc )
            return FALSE;

        s.split( DataFileSeparator );
        s.rewind();

        s.next();
        if ( n==0 && !s.contains(BackSlashChar) )
            is_v6_format = true;

        if ( is_v6_format )
            {
            s.next();
            fix_v6_path( s.text() );
            }
        ncc->local = s.text();

        s.next();
        if ( is_v6_format )
            {
            /*
            -------------------------------------------------------------------------------------
            I need to remember the short computer name so I can replace it with the network name.
            ------------------------------------------------------------------------------------- */
            v6name.add( s.text() );
            s.next();
            }

        ncc->net = s.text();
        extract_computer_name_from_dir( ncc->name, s.text() );
        if ( is_v6_format )
            v7name.add( ncc->name.text() );

        s.next();
        ncc->type = asctoint32( s.text() );

        while( s.next() )
            {
            if ( s.len() > 0 )
                {
                ncc->connectlist.append( s.text(), s.len() );
                }
            }

        add( ncc );
        ncc = 0;
        n++;
        s.remove_all();
        }

    f.close();
    }

if ( is_v6_format )
    {
    rewind();
    while ( next() )
        {
        s.remove_all();
        nc->connectlist.rewind();
        while ( nc->connectlist.next() )
            {
            if ( v6name.find(nc->connectlist.text()) )
                {
                v7name.find( v6name.index() );
                s.add( v7name.text() );
                }
            else
                {
                s.add( v6name.text() );
                }
            }
        nc->connectlist.empty();
        s.rewind();
        while ( s.next() )
            nc->connectlist.append( s.text() );
        }
    }

if ( n > 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                             WRITE                                    *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::write()
{
FILE_CLASS f;
STRING_ARRAY_CLASS s;

if ( count() == 0 )
    return FALSE;

s = exe_directory();
s.cat_path( NetSetupDataFile );

if ( f.open_for_write(s.text()) )
    {
    rewind();
    while ( next() )
        {
        s = nc->local.text();
        s.add( nc->net.text() );
        s.add( int32toasc((int32) nc->type) );
        nc->connectlist.rewind();
        while( nc->connectlist.next() )
            s.add( nc->connectlist.text() );

        s.join( DataFileSeparator );
        f.writeline( s.text() );
        }

    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               =                                      *
***********************************************************************/
void NETWORK_COMPUTER_LIST_CLASS::operator=( NETWORK_COMPUTER_LIST_CLASS & sorc )
{
NETWORK_COMPUTER_CLASS * cp;

remove_all();

sorc.rewind();
while ( sorc.next() )
    {
    cp = new NETWORK_COMPUTER_CLASS( sorc.entry() );
    add( cp );
    }
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                           REMOVE_ALL                                 *
***********************************************************************/
void NETWORK_COMPUTER_LIST_CLASS::remove_all()
{
if ( clist.count() > 0 )
    {
    clist.rewind();
    while ( TRUE )
        {
        nc = (NETWORK_COMPUTER_CLASS *) clist.next();
        if ( !nc )
            break;
        delete nc;
        }

    clist.remove_all();
    }

nc = &empty_entry;
x = -1;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                              REMOVE                                  *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::remove()
{
if ( x >= 0 )
    {
    delete nc;
    clist.remove();
    rewind();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                                ADD                                   *
*  This adds a NETWORK_COMPUTER_CLASS to the list. You pass it a       *
*  pointer to a NETWORK_COMPUTER_CLASS that you have made with new.    *
*  The destructor will delete the NETWORK_COMPUTER_CLASS.              *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::add( NETWORK_COMPUTER_CLASS * nc_to_add )
{
return clist.append( (void *) nc_to_add );
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                   ~NETWORK_COMPUTER_LIST_CLASS                       *
***********************************************************************/
NETWORK_COMPUTER_LIST_CLASS::~NETWORK_COMPUTER_LIST_CLASS()
{
remove_all();
}

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                         NETWORK_COMPUTER_CLASS                       *
*                                  =                                   *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::operator=( NETWORK_COMPUTER_CLASS & sorc )
{
local          = sorc.local;
net            = sorc.net;
type           = sorc.type;
is_changed     = sorc.is_changed;
is_shared_full = sorc.is_shared_full;
online         = sorc.online;
connectlist    = sorc.connectlist;
name           = sorc.name;
copystring( version, sorc.version, VERSION_LEN );
}

/***********************************************************************
*                         NETWORK_COMPUTER_CLASS                       *
*                         NETWORK_COMPUTER_CLASS                       *
***********************************************************************/
NETWORK_COMPUTER_CLASS::NETWORK_COMPUTER_CLASS( NETWORK_COMPUTER_CLASS & sorc )
{
*this = sorc;
}

/***********************************************************************
*                         NETWORK_COMPUTER_CLASS                       *
*                         EXTRACT_TYPE_FROM_NAME                       *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_CLASS::extract_type_from_name()
{
TCHAR * cp;
TCHAR   c;
MACHINE_NAME_LIST_CLASS mylist;

cp = net.text();
while ( *cp == BackSlashChar )
    cp++;

c  = *cp;
if ( c != NullChar )
    cp++;

if ( net.contains(V5dsExt) )
    {
    type = DATA_SERVER_COMPUTER_TYPE;
    }
else if ( (c == WChar || c == WCharLow) && is_numeric(*cp) )
    {
    type = MONITORING_COMPUTER_TYPE;
    }
else
    {
    type = WORKSTATION_COMPUTER_TYPE;
    mylist.add_computer( net );
    if ( mylist.count() > 0 )
        type = MONITORING_COMPUTER_TYPE;
    }
return TRUE;
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                           GET_ONLINE_STATUS                          *
***********************************************************************/
bool NETWORK_COMPUTER_CLASS::get_online_status()
{
NAME_CLASS s;
FILE_CLASS f;

s = net;
s.cat_path( VisiTrakIniFile );

if ( s.file_exists() )
    {
    online = true;
    GetPrivateProfileString( ConfigSection, VersionKey, UnknownString, version, VERSION_LEN+1, s.text() );
    s = net;
    s.cat_path( TEXT("killme.tmp") );
    if ( f.open_for_write(s.text()) )
        {
        f.close();
        s.delete_file();
        is_shared_full = true;
        }
    else
        {
        is_shared_full = false;
        }
    }
else
    {
    online = false;
    }

return online;
}

/***********************************************************************
*                          GET_COMPUTER_LIST                           *
* Pass me a v5 directory and I will fill the text list with the csv    *
* records for each computer. This does not include the data archiver.  *
*                                                                      *
* The csv record is the path to the computer's v5 directory.           *
* \\W01\V5                                                             *
* c:\v5                                                                *
***********************************************************************/
static BOOLEAN get_computer_list( TEXT_LIST_CLASS & dest, STRING_CLASS & directory )
{
FILE_CLASS   f;
NAME_CLASS   s;
BOOLEAN      status;
DB_TABLE     t;

status = FALSE;
s = directory;
s.cat_path( DataString );
s.cat_path( COMPUTER_CSV );
if ( s.file_exists() )
    {
    f.open_for_read( s.text() );
    while ( true )
        {
        s = f.readline();
        if ( s.isempty() )
            break;
        dest.append( s.text() );
        }
    f.close();
    status = TRUE;
    }

return status;
}

/***********************************************************************
*                   SEND_NEW_COMPUTER_SETUP_MESSAGE                    *
* This tells the remote computer that his list of known computers has  *
* changed. It only has an effect on eventman, and hence on monitoring  *
* computers.                                                           *
***********************************************************************/
static void send_new_computer_setup_message( const TCHAR * netdirectory )
{
static TCHAR local_mailslot_name[] = TEXT( "\\\\.\\" );
static TCHAR suffix[] = TEXT( "mailslot\\v5\\eventman" );
static TCHAR reload_computers_event[] = TEXT( "CFG,COMPUTER_SETUP,RELOAD" );

TCHAR  s[MAX_PATH+1];
HANDLE send_handle;
DWORD  slen;
DWORD  bytes_written;

slen = lstrlen(reload_computers_event) * sizeof(TCHAR);
copystring( s, netdirectory );
if ( !convert_to_mailslot_name(s) )
    copystring( s, local_mailslot_name );
append_filename_to_path( s, suffix );

send_handle = CreateFile(
    s,
    GENERIC_WRITE,
    FILE_SHARE_READ,              /* required to write to a mailslot */
    (LPSECURITY_ATTRIBUTES) NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    (HANDLE) NULL);

if ( send_handle != INVALID_HANDLE_VALUE)
    {
    WriteFile( send_handle, reload_computers_event, (DWORD) slen, &bytes_written, (LPOVERLAPPED) NULL );
    CloseHandle( send_handle );
    }
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                           GET_CONNECT_LIST                           *
* This is the list of computer names that I am connected to. I extract *
* the network computer name from each entry that begins with a \\.     *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::get_connect_list()
{
INI_CLASS       ini;
NAME_CLASS      s;
TEXT_LIST_CLASS t;

get_computer_list( t, net );

connectlist.empty();
if ( t.count() > 0 )
    {
    t.rewind();
    while ( t.next() )
        {
        if ( extract_computer_name_from_dir(s, t.text()) )
            {
            if ( !connectlist.find(s.text()) )
                connectlist.append( s.text() );
            }
        }
    }

t.empty();

s = net;
s.cat_path( VisiTrakIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
if ( ini.find(DataServerKey) )
    {
    s = ini.get_string();
    if ( !unknown(s.text()) && s != NoComputerName )
        connectlist.append( s.text() );
    }
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                           IS_CONNECTABLE                             *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_CLASS::is_connectable( NETWORK_COMPUTER_CLASS & dest )
{
BOOLEAN status;

status = FALSE;

switch( type )
    {
    case MONITORING_COMPUTER_TYPE:
        if (  dest.type & (WORKSTATION_COMPUTER_TYPE | DATA_SERVER_COMPUTER_TYPE) )
            status = TRUE;
        break;

    case WORKSTATION_COMPUTER_TYPE:
        if (  dest.type & MONITORING_COMPUTER_TYPE )
            status = TRUE;
        break;

    case DATA_SERVER_COMPUTER_TYPE:
        if (  dest.type & MONITORING_COMPUTER_TYPE )
            status = TRUE;
        break;
    }

return status;
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                     SAVE_DATA_SERVER_CONNECTION                      *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::save_data_server_connection( TCHAR * name_to_save )
{
INI_CLASS ini;
NAME_CLASS s;

s = net;
s.cat_path( VisiTrakIniFile );

ini.set_file( s.text() );
ini.set_section( ConfigSection );

s = ini.get_string( DataServerKey );
if ( s != name_to_save )
    ini.put_string( DataServerKey, name_to_save );
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                     SAVE_DATA_SERVER_CONNECTION                      *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::save_data_server_connection()
{
NAME_CLASS s;

if ( extract_computer_name_from_dir(s, ComputerList.entry().net.text()) )
    save_data_server_connection( s.text() );
else
    remove_data_server_connection();
}

/***********************************************************************
*                    REMOVE_DATA_SERVER_CONNECTION                     *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::remove_data_server_connection()
{
save_data_server_connection( NoComputerName );
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                                SAVE                                  *
*                    Write the computer.csv file                       *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::save()
{
TEXT_LIST_CLASS tc;
TEXT_LIST_CLASS ori;
BOOLEAN         status;
BOOLEAN         have_data_server_connection;
FILE_CLASS      f;
NAME_CLASS      s;
DB_TABLE        t;
NAME_CLASS      my;

is_changed = false;

/*
------------------------------------------------------------------
See if there is a data archiver in the list of connected computers
------------------------------------------------------------------ */
have_data_server_connection = FALSE;
tc = connectlist;
if ( type == MONITORING_COMPUTER_TYPE )
    {
    tc.sort();
    tc.rewind();
    while ( tc.next() )
        {
        if ( ComputerList.find(tc.text()) )
            {
            if ( ComputerList.entry().type == DATA_SERVER_COMPUTER_TYPE )
                {
                save_data_server_connection();
                have_data_server_connection = TRUE;
                break;
                }
            }
        }
    }

if ( !have_data_server_connection )
    remove_data_server_connection();

s = net;
s.cat_path( DataString );
s.cat_path( COMPUTER_CSV );

/*
---------------------
Get the existing list
--------------------- */
get_computer_list( ori, net );

status = f.open_for_write( s.text() );
if ( status )
    {
    /*
    --------------------------------------------------------------------------------
    Computer records are just v5 directories, now. The first is the local directory.
    -------------------------------------------------------------------------------- */
    f.writeline( local.text() );

    tc.rewind();
    while ( tc.next() )
        {
        if ( ComputerList.find(tc.text()) )
            {
            if ( ComputerList.entry().type != DATA_SERVER_COMPUTER_TYPE )
                f.writeline( ComputerList.entry().net.text() );
            }
        else
            {
            /*
            -----------------------------------------------------------
            This computer is not in my list. Copy it back from the list
            I made from the file.
            ----------------------------------------------------------- */
            ori.rewind();
            while ( ori.next() )
                {
                if ( extract_computer_name_from_dir(s, ori.text()) )
                    {
                    if ( s == tc.text() )
                        {
                        f.writeline( ori.text() );
                        break;
                        }
                    }
                }
            }
        }

    f.close();

    /*
    -----------------------------------------------------------------------------
    Send a mailslot message to tell this computer to update his list of computers
    ----------------------------------------------------------------------------- */
    send_new_computer_setup_message( net.text() );
    }

tc.empty();
ori.empty();
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                               CLEANUP                                *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::cleanup()
{
connectlist.empty();
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                                LBLINE                                *
***********************************************************************/
BOOLEAN  NETWORK_COMPUTER_CLASS::lbline( STRING_CLASS & dest )
{
const int MAX_COMPUTER_TYPE_STRING_LEN = 30; //It's ok if this is too small.
TCHAR * cp;
int     cw;
int     n;
int     n1;
UINT    id;

switch ( type )
    {
    case DATA_SERVER_COMPUTER_TYPE:
        id = DATA_SERVER_COMPUTER_STRING;
        break;

    case MONITORING_COMPUTER_TYPE:
        id = MONITORING_COMPUTER_STRING;
        break;

    default:
        id = WORKSTATION_COMPUTER_STRING;
        break;
    }

n = name.len() + 1;
n += net.len()+1;
n += MAX_COMPUTER_TYPE_STRING_LEN;
n += MaxConnectedListLen + EllipsisStringLen;
n += lstrlen(version)+1;

n1 = YesName.len();
maxint( n1, NoName.len() );
n1 *= 2;
n1 += 3;

n += n1;

if ( dest.upsize(n) )
    {
    dest = name;
    dest += TabChar;
    dest += net;
    dest += TabChar;
    dest += resource_string( id );
    dest += TabChar;
    if ( connectlist.count() > 0 )
        {
        cw = 0;
        connectlist.rewind();
        while ( connectlist.next() )
            {
            if ( cw < MaxConnectedListLen )
                {
                cw += connectlist.len();
                if ( cw >= MaxConnectedListLen )
                    {
                    dest += EllipsisString;
                    }
                else
                    {
                    dest += connectlist.text();
                    dest += SpaceChar;
                    }
                }
            }
        cp = dest.text() + dest.len();
        cp--;
        if ( *cp == SpaceChar )
            *cp = NullChar;
        }
    dest += TabChar;

    if ( lstrlen(version) > 0 )
        dest += version;
    dest += TabChar;

    if ( is_shared_full )
        dest += YesName.text();
    else if ( online )
        dest += NoName.text();

    dest += TabChar;

    if ( online )
        dest += YesName.text();
    else
        dest += NoName.text();

    if ( is_changed )
        dest += AsterixChar;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         EXTRACT_COMPUTER_NAME                        *
*        This extracts a computer's name from the listbox line.        *
***********************************************************************/
static TCHAR * extract_computer_name( TCHAR * sorc )
{
static TCHAR s[MAX_PATH+1];
TCHAR * dest;

*s = NullChar;

if ( sorc )
    {
    dest  = s;
    while ( TRUE )
        {
        *dest = *sorc;

        if ( *dest == TabChar )
            *dest = NullChar;

        if ( *dest == NullChar )
            break;

        dest++;
        sorc++;
        }
    }

return s;
}

static STRING_CLASS DupCo1;
static STRING_CLASS DupCo2;
static STRING_CLASS DupMa;
static BOOLEAN DupContinueAnyway = FALSE;

/***********************************************************************
*                    DUPLICATE_MACHINE_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK duplicate_machine_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        DupContinueAnyway = FALSE;
        DupMa.set_text( hWnd, DUP_MA_TBOX );
        DupCo1.set_text( hWnd, DUP_CO_1_TBOX );
        DupCo2.set_text( hWnd, DUP_CO_2_TBOX );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                DupContinueAnyway = TRUE;
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                       NO_DUPLICATE_MACHINES                          *
* Sorc is the network path of the computer I want to add. I check      *
* against the machines I know to see if there is a duplicate.          *
***********************************************************************/
static BOOLEAN no_duplicate_machines( STRING_CLASS  & sorc )
{
MACHINE_NAME_LIST_CLASS mylist;
MACHINE_NAME_LIST_CLASS newlist;

ComputerList.rewind();
while ( ComputerList.next() )
    {
    if ( ComputerList.entry().online && (ComputerList.entry().type != DATA_SERVER_COMPUTER_TYPE) )
        mylist.add_computer( ComputerList.entry().net );
    }

if ( mylist.count() )
    {
    newlist.add_computer( sorc );
    if ( newlist.count() )
        {
        newlist.rewind();
        while ( newlist.next() )
            {
            if ( mylist.find(newlist.name()) )
                {
                DupMa  = newlist.name();
                DupCo1 = mylist.computer_name();
                DupCo2 = newlist.computer_name();
                DialogBox( MainInstance, TEXT("DUPLICATE_MACHINE_DIALOG"), ComputerWindow.handle(), (DLGPROC) duplicate_machine_dialog_proc );
                return DupContinueAnyway;
                }
            }
        }
    }
return TRUE;
}

/***********************************************************************
*                             CHECK_FOR_V5                             *
***********************************************************************/
static bool check_for_v5( TCHAR * rootdir )
{
INI_CLASS  ini;
NAME_CLASS root;
NAME_CLASS s;
NETWORK_COMPUTER_CLASS * nce;
bool is_data_server;
bool return_status;
int  choice;

return_status = false;

s = rootdir;

if ( ComputerList.netfind(s) )
    {
    resource_message_box( MainInstance, COMPUTER_ALREADY_IN_LIST_STRING, CANT_COMPLY_STRING, MB_OK | MB_SYSTEMMODAL );
    return return_status;
    }

is_data_server = false;

if ( s.contains(V5dsExt) )
    is_data_server = true;

root = s;
s.cat_path( VisiTrakIniFile );

if ( s.file_exists() )
    {
    nce = new NETWORK_COMPUTER_CLASS;
    if ( nce )
        {
        nce->net = root;
        extract_computer_name_from_dir( nce->name, root.text() );

        nce->extract_type_from_name();
        ini.set_file( s.text() );
        nce->local = ini.get_string( ConfigSection, ThisComputerKey );
        nce->get_online_status();

        choice = IDCANCEL;

        if ( !CancelAddNewComputers )
            {
            if ( no_duplicate_machines(nce->net) )
                {
                nce->get_connect_list();
                choice = IDOK;
                }
            }

        if ( choice == IDOK )
            {
            ComputerList.add( nce );
            return_status = true;
            }
        else
            {
            delete nce;
            }
        }
    }
else
    {
    resource_message_box( MainInstance, NO_VISITRAK_INI_FILE, CANT_COMPLY_STRING, MB_OK | MB_SYSTEMMODAL );
    }

return return_status;
}

/***********************************************************************
*                        FILL_COMPUTER_LISTBOX                         *
***********************************************************************/
static void fill_computer_listbox()
{
LISTBOX_CLASS lb;
INT           i;
STRING_CLASS  s;

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
lb.empty();

if ( ComputerList.count() <= 0 )
    return;

ComputerList.rewind();
while ( ComputerList.next() )
    {
    if ( ComputerList.entry().lbline(s) )
        i = lb.add( s.text() );
    }
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes()
{
NETWORK_COMPUTER_CLASS * cp;
LISTBOX_CLASS lb;
STRING_ARRAY_CLASS s;
STRING_CLASS       msg;
INT * ip;
int   n;
int   i;

set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX, EmptyString );

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
n = lb.get_select_list( &ip );
if ( n < 1 )
    return;

for ( i=0; i<n; i++ )
    s.add( extract_computer_name(lb.item_text(ip[i])) );

s.rewind();
i = 1;
while ( s.next() )
    {
    msg = s.text();
    msg += SpaceChar;
    msg += i;
    msg += TEXT( " of " );
    msg += n;
    msg.set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX );

    if ( ComputerList.find(s.text()) )
        {
        /*
        ----------------------------------
        Copy the changes to the saved list
        ---------------------------------- */
        if ( SavedList.find(ComputerList.entry().name) )
            {
            SavedList.entry() = ComputerList.entry();
            }
        else
            {
            cp = new NETWORK_COMPUTER_CLASS( ComputerList.entry() );
            SavedList.add( cp );
            }

        /*
        ------------------------------------------------------------
        Save actually modifies which entry is current so I save last
        ------------------------------------------------------------ */
        ComputerList.entry().save();
        }
    msg = TEXT( "Complete" );
    msg.set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX );
    }

/*
-----------------------------------------------------
Redisplay the computers to show the new changed flags
----------------------------------------------------- */
fill_computer_listbox();

/*
----------------------
Reselect the same list
---------------------- */
for ( i=0; i<n; i++ )
    lb.set_select( ip[i], TRUE );

/*
--------------------------
I own the array of indexes
-------------------------- */
delete[] ip;
}

static STRING_CLASS DupChkCo1;
static STRING_CLASS DupChkCo2;
static STRING_CLASS DupChkMa;

/***********************************************************************
*                 DUPLICATE_MACHINE_CHECK_DIALOG_PROC                  *
***********************************************************************/
BOOL CALLBACK duplicate_machine_check_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        DupChkMa.set_text(  hWnd, DUP_MA_TBOX );
        DupChkCo1.set_text( hWnd, DUP_CO_1_TBOX );
        DupChkCo2.set_text( hWnd, DUP_CO_2_TBOX );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                     CHECK_FOR_DUPLICATE_MACHINE                      *
***********************************************************************/
static void check_for_duplicate_machine()
{
MACHINE_NAME_LIST_CLASS mylist;
MACHINE_NAME_LIST_CLASS newlist;
int          count;
count = 0;

ComputerList.rewind();
while ( ComputerList.next() )
    {
    if ( ComputerList.entry().online && (ComputerList.entry().type != DATA_SERVER_COMPUTER_TYPE) )
        {
        newlist.add_computer( ComputerList.entry().net );
        if ( mylist.count() )
            {
            newlist.rewind();
            while ( newlist.next() )
                {
                if ( mylist.find(newlist.name()) )
                    {
                    DupChkMa  = newlist.name();
                    DupChkCo1 = mylist.computer_name();
                    DupChkCo2 = newlist.computer_name();
                    DialogBox( MainInstance, TEXT("DUPLICATE_MACHINE_CHECK_DIALOG"), ComputerWindow.handle(), (DLGPROC) duplicate_machine_check_dialog_proc );
                    count++;
                    }
                }
            }
        mylist.acquire( newlist, NO_SORT );
        }
    }
if ( count == 0 )
    {
    MessageBox( ComputerWindow.handle(), TEXT("Check Complete"), TEXT("No Duplicates Found"), MB_OK );
    }
}

/***********************************************************************
*                           REMOVE_COMPUTERS                           *
***********************************************************************/
static void remove_computers( HWND w )
{
LISTBOX_CLASS lb;
STRING_ARRAY_CLASS s;
INT * ip;
int   n;
int   i;

lb.init( w, COMPUTER_LISTBOX );
n = lb.get_select_list( &ip );
if ( n < 1 )
    return;

for ( i=0; i<n; i++ )
    s.add( extract_computer_name(lb.item_text(ip[i])) );

/*
--------------------------
I own the array of indexes
-------------------------- */
delete[] ip;

i = resource_message_box( w, MainInstance, CONFIRM_REMOVE_STRING, WILL_REMOVE_STRING, MB_OKCANCEL | MB_ICONQUESTION | MB_SYSTEMMODAL );
if ( i == IDCANCEL )
    return;

s.rewind();
while ( s.next() )
    {
    if ( ComputerList.find(s.text()) )
        ComputerList.remove();

    if ( SavedList.find(s.text()) )
        SavedList.remove();
    }

fill_computer_listbox();
}

/***********************************************************************
*                        SET_COMPUTER_NAME_EBOX                        *
***********************************************************************/
static void set_computer_name_ebox()
{
static UINT disablelist[] = {
    CONNECTED_COMPUTERS_TBOX,
    AVAILABLE_COMPUTERS_TBOX,
    TYPE_GBOX
    };
const int NTO_DISABLE = sizeof(disablelist)/sizeof(UINT);

TCHAR           * cp;
int               id;
LISTBOX_CLASS     lb;
TEXT_LIST_CLASS * mylist;
INT               n;
int               ok_types;
STRING_CLASS      s;
HWND              w;

Updating = TRUE;

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );

/*
------------------------------------------------------------
The name cannot be edited if more than one line are selected
------------------------------------------------------------ */
n = lb.select_count();
if ( n > 1 )
    {
    for ( id=0; id<NTO_DISABLE; id++ )
        {
        w = ComputerWindow.control( disablelist[id] );
        EnableWindow( w, FALSE );
        }

    lb.init( ComputerWindow.control(CONNECTED_COMPUTER_LISTBOX) );
    lb.empty();

    lb.init( ComputerWindow.control(AVAILABLE_COMPUTER_LISTBOX) );
    lb.empty();

    for ( id=MONITORING_COMPUTER_RADIO; id<=DATA_SERVER_RADIO; id++ )
        {
        w = ComputerWindow.control( id );
        set_checkbox( w, FALSE );
        EnableWindow( w, FALSE );
        }

    Updating   = FALSE;
    return;
    }

for ( id=0; id<NTO_DISABLE; id++ )
    {
    w = ComputerWindow.control( disablelist[id] );
    EnableWindow( w, TRUE );
    }

for ( id=MONITORING_COMPUTER_RADIO; id<=DATA_SERVER_RADIO; id++ )
    {
    w = ComputerWindow.control( id );
    EnableWindow( w, TRUE );
    }

cp = lb.selected_text();
if ( !cp )
    {
    Updating = FALSE;
    return;
    }

s = extract_computer_name( cp );

ComputerList.rewind();
while ( ComputerList.next() )
    {
    if ( ComputerList.entry().name == s )
        {

        /*
        ------------------------------------
        Press the radio button for this type
        ------------------------------------ */
        switch( ComputerList.entry().type )
            {
            case MONITORING_COMPUTER_TYPE:
                id = MONITORING_COMPUTER_RADIO;
                break;

            case DATA_SERVER_COMPUTER_TYPE:
                id = DATA_SERVER_RADIO;
                break;

            default:
                id = OFFICE_WORKSTATION_RADIO;
                break;
            }

        CheckRadioButton( ComputerWindow.handle(), MONITORING_COMPUTER_RADIO, DATA_SERVER_RADIO, id );

        /*
        ---------------------------------------
        Fill the listbox of connected computers
        --------------------------------------- */
        lb.init( ComputerWindow.handle(), CONNECTED_COMPUTER_LISTBOX );
        lb.empty();
        if ( ComputerList.entry().connectlist.count() > 0 )
            {
            ComputerList.entry().connectlist.rewind();
            while ( ComputerList.entry().connectlist.next() )
                lb.add( ComputerList.entry().connectlist.text() );
            }

        /*
        ---------------------------------------
        Fill the listbox of available computers
        --------------------------------------- */
        lb.init( ComputerWindow.handle(), AVAILABLE_COMPUTER_LISTBOX );
        lb.empty();
        mylist = &ComputerList.entry().connectlist;

        /*
        -----------------------------------------------------------
        Only list the types of computers that I should connect with
        ----------------------------------------------------------- */
        ok_types = 0;
        switch( ComputerList.entry().type )
            {
            case MONITORING_COMPUTER_TYPE:
                ok_types = WORKSTATION_COMPUTER_TYPE | DATA_SERVER_COMPUTER_TYPE;
                break;

            case WORKSTATION_COMPUTER_TYPE:
                ok_types = MONITORING_COMPUTER_TYPE;
                break;

            case DATA_SERVER_COMPUTER_TYPE:
                ok_types = MONITORING_COMPUTER_TYPE;
                break;
            }

        ComputerList.rewind();
        while ( ComputerList.next() )
            {
            if ( ComputerList.entry().type & ok_types )
                {
                /*
                -------------------------------------------------
                This is available if it is not in my connect list
                ------------------------------------------------- */
                if ( !mylist->find(ComputerList.entry().name.text()) )
                    lb.add( ComputerList.entry().name.text() );
                }
            }

        break;
        }
    }

Updating = FALSE;
}

/***********************************************************************
*                       DO_CONNECTED_COMPUTER_SELECT                   *
***********************************************************************/
static void do_connected_computer_select()
{
LISTBOX_CLASS clb;
LISTBOX_CLASS lb;
INT           i;
INT           n;
STRING_CLASS  s;
STRING_CLASS  dead_computer_name;

clb.init( ComputerWindow.handle(), CONNECTED_COMPUTER_LISTBOX );
dead_computer_name = clb.selected_text();
i = clb.current_index();
clb.remove( i );

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
s = extract_computer_name( lb.selected_text() );
if ( ComputerList.find(s.text()) )
    {
    ComputerList.entry().is_changed = true;
    ComputerList.entry().connectlist.empty();
    n = clb.count();
    for ( i=0; i<n; i++ )
        ComputerList.entry().connectlist.append( clb.item_text(i) );
    i = lb.current_index();

    if ( ComputerList.entry().lbline(s) )
        lb.replace( s.text() );

    lb.init( ComputerWindow.handle(), AVAILABLE_COMPUTER_LISTBOX );
    lb.add( dead_computer_name.text() );
    }

}

/***********************************************************************
*                        DO_STANDARD_CONNECTIONS                       *
***********************************************************************/
static void do_standard_connections()
{
NETWORK_COMPUTER_CLASS * e;
INT                      i;
INT                      n;
INT                      sn;
LISTBOX_CLASS lb;
STRING_CLASS  s;

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );

sn = lb.select_count();
if ( sn < 1 )
    return;

n = lb.count();
for ( i=0; i<n; i++ )
    {
    if ( lb.is_selected(i) )
        {
        if ( ComputerList.find(extract_computer_name(lb.item_text(i))) )
            {
            e = &ComputerList.entry();
            e->connectlist.empty();
            e->is_changed = true;

            ComputerList.rewind();
            while ( ComputerList.next() )
                {
                if ( e->is_connectable(ComputerList.entry()) )
                    e->connectlist.append( ComputerList.entry().name.text() );
                }

            if ( lb.set_current_index(i) )
                {
                if ( e->lbline(s) )
                    lb.replace( s.text() );
                }
            }

        if ( sn == 1 )
            set_computer_name_ebox();
        }
    }
}

/***********************************************************************
*                       DO_AVAILABLE_COMPUTER_SELECT                   *
***********************************************************************/
static void do_available_computer_select()
{
STRING_CLASS  s;
STRING_CLASS  new_computer_name;
LISTBOX_CLASS clb;
LISTBOX_CLASS lb;
INT           i;

lb.init( ComputerWindow.handle(), AVAILABLE_COMPUTER_LISTBOX );
new_computer_name = lb.selected_text();
i = lb.current_index();
lb.remove( i );

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
s = extract_computer_name( lb.selected_text() );
if ( ComputerList.find(s.text()) )
    {
    if ( !ComputerList.entry().connectlist.find(new_computer_name.text()) )
        {
        ComputerList.entry().connectlist.append( new_computer_name.text() );
        ComputerList.entry().is_changed = true;
        i = lb.current_index();
        if ( ComputerList.entry().lbline(s) )
            lb.replace( s.text() );

        lb.init( ComputerWindow.handle(), CONNECTED_COMPUTER_LISTBOX );
        lb.add( new_computer_name.text() );
        }
    }
}

/***********************************************************************
*                          SET_COMPUTER_TYPE                           *
***********************************************************************/
static void set_computer_type( int button )
{
STRING_CLASS  s;
LISTBOX_CLASS lb;
int           i;
int           newtype;

if ( Updating )
    return;

newtype = WORKSTATION_COMPUTER_TYPE;

switch ( button )
    {
    case MONITORING_COMPUTER_RADIO:
        newtype = MONITORING_COMPUTER_TYPE;
        break;

    case DATA_SERVER_RADIO:
        newtype = DATA_SERVER_COMPUTER_TYPE;
        break;
    }

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
s = extract_computer_name( lb.selected_text() );
if ( ComputerList.find(s.text()) )
    {
    if ( ComputerList.entry().type != newtype )
        {
        ComputerList.entry().type       = newtype;
        ComputerList.entry().is_changed = true;
        i = lb.current_index();
        if ( ComputerList.entry().lbline(s) )
            lb.replace( s.text() );
        set_computer_name_ebox();
        }
    }

}

/***********************************************************************
*                          browse_callback_proc                        *
***********************************************************************/
int CALLBACK browse_callback_proc( HWND w, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
ITEMIDLIST * p;
TCHAR buf[MAX_PATH];

if ( uMsg == BFFM_SELCHANGED )
    {
    p = (ITEMIDLIST *) lParam;
    if ( p != NULL )
        {
        if ( SHGetPathFromIDList(p, buf) )
            {
            check_for_v5( buf );
            fill_computer_listbox();
            }
        }
    }
return 0;
}

/***********************************************************************
*                            BROWSE_COMPUTERS                          *
***********************************************************************/
void browse_computers( HWND w )
{
BROWSEINFO    bi;
TCHAR         buf[MAX_PATH];
LPMALLOC      ip;
ITEMIDLIST  * network_neighborhood;
ITEMIDLIST ** np;
ITEMIDLIST  * pidlBrowse;

/*
--------------------------------------------------------------------
If this fails it will set the network neighborhood to NULL, which is
what I want.
-------------------------------------------------------------------- */
np = &network_neighborhood;
SHGetSpecialFolderLocation( NULL, CSIDL_NETWORK, np );

bi.hwndOwner      = w;
bi.pidlRoot       = network_neighborhood;
bi.pszDisplayName = buf;
bi.lpszTitle      = TEXT("Choose My Network Places, Entire Network, Microsoft Windows Network, Visitrak, and click on the V5 or V5DS folder");
bi.ulFlags        = 0;
bi.lpfn           = browse_callback_proc;
bi.lParam         = 0;
bi.iImage         = 0;

/*
-----------------------------------------------------------------------------
I don't actually use the returned choice. I do each v5 folder as it is chosen
but I need to delete the itemidlist returned by SHBrowseForFolder.
----------------------------------------------------------------------------- */
if ( SUCCEEDED(SHGetMalloc(&ip)) )
    {
    pidlBrowse = SHBrowseForFolder( &bi );
    if ( pidlBrowse != NULL )
        ip->Free( pidlBrowse );

    if ( network_neighborhood != NULL )
        ip->Free( network_neighborhood );

    ip->Release();
    }
}

/***********************************************************************
*                          ADD_COMPUTER_BY_PATH                        *
***********************************************************************/
void add_computer_by_path( HWND hWnd )
{
NAME_CLASS s;

s.get_text( hWnd, ENTER_COMPUTER_NAME_EB );
s.remove_ending_backslash();
if ( check_for_v5(s.text()) )
    fill_computer_listbox();
}

/***********************************************************************
*                    ADD_COMPUTER_BY_PATH_DIALOG_PROC                  *
***********************************************************************/
BOOL CALLBACK add_computer_by_path_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            add_computer_by_path( hWnd);

            case IDCANCEL:
            EndDialog( hWnd, 0 );
            return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                                GETHELP                               *
***********************************************************************/
void gethelp( HWND w )
{
TCHAR s[MAX_PATH+1];

get_exe_directory( s );
append_filename_to_path(  s, TEXT("Netsetup.chm") );

HtmlHelp( w, s, HH_DISPLAY_TOPIC, NULL );
}

/***********************************************************************
*                         SET_COMPUTER_LISTBOX_TABS                    *
***********************************************************************/
static void set_computer_listbox_tabs()
{
static int title_ids[] = {
    DIRECTORY_TBOX,
    TYPE_TBOX,
    CONNECTIONS_TBOX,
    VERSION_TBOX,
    SHARED_FULL_TBOX,
    ONLINE_TBOX
    };
const int nof_title_ids = sizeof(title_ids)/sizeof(int);
HDC             dc;
RECTANGLE_CLASS r;
WINDOW_CLASS    w;

set_listbox_tabs_from_title_positions( ComputerWindow.handle(), COMPUTER_LISTBOX, title_ids, nof_title_ids );

/*
---------------------------------------------------------------------
Calculate the maximum width of the list of connections in the listbox
--------------------------------------------------------------------- */
w = ComputerWindow.control( VERSION_TBOX );
w.getrect( r );
MaxConnectedListLen = r.left;

w = ComputerWindow.control( CONNECTIONS_TBOX );
w.getrect( r );
MaxConnectedListLen -= r.left;

dc = w.get_dc();

AverageCharacterWidth = pixel_width( dc, TEXT("0") );
if ( AverageCharacterWidth < 1 )
    AverageCharacterWidth = 1;

MaxConnectedListLen -= pixel_width( dc, EllipsisString );
w.release_dc( dc );

MaxConnectedListLen--;
MaxConnectedListLen /= AverageCharacterWidth;

if ( MaxConnectedListLen < 1 )
    MaxConnectedListLen = 20;
}

/***********************************************************************
*                         SET_COMPUTER_LISTBOX_SIZE                    *
***********************************************************************/
static void set_computer_listbox_size()
{
RECTANGLE_CLASS r;
WINDOW_CLASS    w;
long dw;
long dw3;

MainWindow.get_client_rect( r );
if ( r == MainClientRect )
    return;

dw = r.width() - MainClientRect.width();
MainClientRect = r;
if ( dw != 0 )
    {
    w = ComputerWindow.control( VERSION_TBOX );
    w.get_move_rect( r );
    r.moveto( r.left+dw, r.top );
    w.move( r );

    w = ComputerWindow.control( SHARED_FULL_TBOX );
    w.get_move_rect( r );
    r.moveto( r.left+dw, r.top );
    w.move( r );

    w = ComputerWindow.control( ONLINE_TBOX );
    w.get_move_rect( r );
    r.moveto( r.left+dw, r.top );
    w.move( r );

    /*
    ---------------------------------------------------------------------
    Divide the new width between the computer, directory, and connections
    --------------------------------------------------------------------- */
    dw3 = 2*dw/3;

    w = ComputerWindow.control( CONNECTIONS_TBOX );
    w.get_move_rect( r );
    r.moveto( r.left+dw3, r.top );
    w.move( r );

    w = ComputerWindow.control( TYPE_TBOX );
    w.get_move_rect( r );
    r.moveto( r.left+dw3, r.top );
    w.move( r );

    dw3 = dw/3;

    w = ComputerWindow.control( DIRECTORY_TBOX );
    w.get_move_rect( r );
    r.moveto( r.left+dw3, r.top );
    w.move( r );

    w = ComputerWindow.control( COMPUTER_LISTBOX );

    w.get_move_rect( r );
    r.right += dw;
    w.move( r );
    }
}

/***********************************************************************
*                          COMPUTER_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK computer_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_SIZE:
        if ( !Starting )
            {
            set_computer_listbox_size();
            set_computer_listbox_tabs();
            }
        return TRUE;

    case WM_DBINIT:
        set_computer_listbox_tabs();
        if ( SavedList.read() )
            {
            ComputerList = SavedList;
            ComputerList.rewind();
            while( ComputerList.next() )
                {
                ComputerList.entry().net.set_text( w, CHECKING_DIR_TBOX );
                if ( ComputerList.entry().get_online_status() )
                    {
                    ComputerList.entry().get_connect_list();
                    }
                }
            fill_computer_listbox();
            set_text( w, CHECKING_DIR_TBOX, EmptyString );
            }
        else
            {
            browse_computers(w);
            }
        Starting = FALSE;
        return TRUE;

    case WM_INITDIALOG:
        ComputerWindow = w;
        MainWindow.shrinkwrap( w );
        set_text( w, CHECKING_DIR_TBOX, EmptyString );
        ComputerWindow.post( WM_DBINIT );
        return TRUE;

    case WM_HELP:
        gethelp( w );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVE_CHANGES_BUTTON:
                save_changes();
                return TRUE;

            case HELP_BUTTON:
                gethelp( w );
                return TRUE;

            case IDCANCEL:
                MainWindow.close();
                return TRUE;

            case REMOVE_COMPUTER_BUTTON:
                remove_computers( w );
                return TRUE;

            case BROWSE_COMPUTERS_BUTTON:
                browse_computers( w );
                return TRUE;

            case ADD_COMPUTER_BY_PATH_BUTTON:
                DialogBox( MainInstance, TEXT("ADD_COMPUTER_BY_PATH_DIALOG"), ComputerWindow.handle(), (DLGPROC) add_computer_by_path_dialog_proc );
                return TRUE;

            case CHECK_FOR_DUPLICATE_MACHINES_PB:
                check_for_duplicate_machine();
                return TRUE;

            case STANDARD_CONNECTIONS_BUTTON:
                if ( cmd == BN_CLICKED )
                    {
                    do_standard_connections();
                    return TRUE;
                    }
                break;

            case MONITORING_COMPUTER_RADIO:
            case OFFICE_WORKSTATION_RADIO:
            case DATA_SERVER_RADIO:
                set_computer_type( id );
                return TRUE;

            case CONNECTED_COMPUTER_LISTBOX:
                if ( cmd == LBN_SELCHANGE && !Updating )
                    {
                    do_connected_computer_select();
                    return TRUE;
                    }
                break;

            case AVAILABLE_COMPUTER_LISTBOX:
                if ( cmd == LBN_SELCHANGE && !Updating )
                    {
                    do_available_computer_select();
                    return TRUE;
                    }
                break;

            case COMPUTER_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    set_computer_name_ebox();
                    return TRUE;
                    }
                break;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static RECTANGLE_CLASS r;

switch (msg)
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        Starting = TRUE;
        break;

    case WM_DBINIT:
        MainWindow.get_client_rect( MainClientRect );
        return 0;

    case WM_SIZE:
        if ( !Starting )
            {
            MainWindow.get_client_rect( r );
            ComputerWindow.move( r );
            }
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
STRING_CLASS title;
WNDCLASS wc;

CoInitialize(0);

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("NETSETUP_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title   = resource_string( MAIN_WINDOW_TITLE_STRING );
YesName = resource_string( YES_STRING );
NoName  = resource_string( NO_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT("COMPUTER_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) computer_dialog_proc );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
NETWORK_COMPUTER_CLASS nce;
nce.cleanup();

if ( SavedList.count() )
    SavedList.write();

CoUninitialize();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL status;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( ComputerWindow.handle() )
        status = IsDialogMessage( ComputerWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
