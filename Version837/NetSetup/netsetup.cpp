#include <windows.h>
#include <commctrl.h>
#include <htmlhelp.h>
#include <winnetwk.h>   /* link to mpr.lib */
#include <shlobj.h>
#include <regex>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\rectclas.h"
#include "..\include\setpoint.h"
#include "..\include\servicelog.h"
#include "..\include\socket_client.h"
#include "..\include\events.h"
#include "..\include\strarray.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "msres.h"

#define _MAIN_
#include "..\include\chars.h"

#define COMPUTER_LINE_ITEM      1
#define DIRECTORY_LINE_ITEM     2
#define TYPE_LINE_ITEM          3
#define CONNECTIONS_LINE_ITEM   4

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

TCHAR MyClassName[] = TEXT( "NetSetup" );

const int VERSION_LEN               = 7; /* 6.10A or unknown */

//const int NO_COMPUTER_TYPE          = 0;
//const int MONITORING_COMPUTER_TYPE  = 1;
//const int WORKSTATION_COMPUTER_TYPE = 2;

/*
-------------------------------
This was used previous to V8.15
------------------------------- */
const int DATA_SERVER_COMPUTER_TYPE = 4;

static TCHAR ConfigSection[]      = TEXT( "Config" );
static TCHAR ThisComputerKey[]    = TEXT( "ThisComputer" );
static TCHAR VersionKey[]         = TEXT( "Version" );

static TCHAR DataDirString[]      = TEXT("\\data\\");
static TCHAR DataString[]         = TEXT("data");
static TCHAR EllipsisString[]     = TEXT("...");
const  int   EllipsisStringLen    = sizeof(EllipsisString)/sizeof(TCHAR);
static TCHAR NetDirPrefixString[] = TEXT( "\\\\" );
static TCHAR NoComputerName[]     = NO_COMPUTER;
static TCHAR VisiTrakIniFile[]    = TEXT("\\exes\\visitrak.ini");
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
----------------
Network settings
---------------- */
STRING_CLASS LocalIpPort = DEFAULT_PORT;
STRING_CLASS DsIpPort    = DEFAULT_DS_PORT;
STRING_CLASS LocalIpAddress;
STRING_CLASS DsIpAddress;
SOCKET_CLIENT NetSetupClient;

/*
-------------------------------------------------------------------------
The following two widths are used to limit the length of the connect list
------------------------------------------------------------------------- */
static int          AverageCharacterWidth = 5;
static int          MaxConnectedListLen   = 20;   /* Characters */

static STRING_CLASS DupCo1;
static STRING_CLASS DupCo2;
static STRING_CLASS DupMa;
static BOOLEAN DupContinueAnyway = FALSE;

SERVICE_LOG ServiceLog;

class NETWORK_COMPUTER_CLASS
    {
    public:

    NAME_CLASS      local;
    NAME_CLASS      net;
    NAME_CLASS      path;
    STRING_CLASS    name;
    TCHAR           version[VERSION_LEN+1];
    STRING_CLASS    ip_addr;
    int             type;
    bool            is_changed;
    bool            online;
    bool            is_shared_full;
    TEXT_LIST_CLASS connectlist;

    NETWORK_COMPUTER_CLASS();
    NETWORK_COMPUTER_CLASS( NETWORK_COMPUTER_CLASS & sorc );
    ~NETWORK_COMPUTER_CLASS();
    void    operator=( NETWORK_COMPUTER_CLASS & sorc );
    BOOLEAN extract_type_from_name();
    BOOLEAN is_connectable( NETWORK_COMPUTER_CLASS & dest );
    BOOLEAN lbline( STRING_CLASS & dest );
    void    cleanup();
    bool    get_online_status();
    void    get_connect_list();
    void    save();
    void    save_data_archiver_connection();
    void    save_data_archiver_connection( TCHAR * name_to_save );
    void    remove_data_archiver_connection();
    void    lookup_ip_addr();
    void    use_ip_path();
    void    use_net_path();
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
    BOOLEAN find( TCHAR * computer_to_find, TCHAR * type );
    BOOLEAN find( TCHAR * computer_to_fine, int type );
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
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::find( TCHAR * computer_to_find, TCHAR * type )
{
    STRING_CLASS s;

    if ( !computer_to_find )
        return FALSE;

    if ( *computer_to_find == NullChar )
        return FALSE;

    if ( strings_are_equal(computer_to_find, NoComputerName) )
        return FALSE;

    rewind();
    while ( next() )
    {
        switch (nc->type)
        {
            case MONITORING_COMPUTER_TYPE:
                s = TEXT("Monitoring Computer");
                break;
            case WORKSTATION_COMPUTER_TYPE:
                s = TEXT("Office Workstation");
                break;
            case DATA_ARCHIVER_COMPUTER_TYPE:
                s = TEXT("Data Archiver");
                break;
            default:
                s = TEXT("No Computer");
                break;
        }

        if ( nc->name == computer_to_find && s == type )
            return TRUE;
    }

    return FALSE;
}

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                               FIND                                   *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::find( TCHAR * computer_to_find, int type )
{
    STRING_CLASS s;

    if (!computer_to_find)
        return FALSE;

    if (*computer_to_find == NullChar)
        return FALSE;

    if (strings_are_equal(computer_to_find, NoComputerName))
        return FALSE;

    rewind();
    while (next())
    {
        if (nc->name == computer_to_find && nc->type == type)
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

/***********************************************************************
*                    NETWORK_COMPUTER_LIST_CLASS                       *
*                              READ                                    *
***********************************************************************/
BOOLEAN NETWORK_COMPUTER_LIST_CLASS::read()
{
    FILE_CLASS               f;
    int                      n;
    NETWORK_COMPUTER_CLASS * ncc;
    STRING_ARRAY_CLASS       s;
    STRING_ARRAY_CLASS       v6name;
    STRING_ARRAY_CLASS       v7name;
    STRING_CLASS             curr_s;

    s = exe_directory();
    s.cat_path( NetSetupDataFile );
    n = 0;

    if ( f.open_for_read(s.text()) )
    {
        while ( true )
        {
            s = f.readline();
            if ( s.len() == 0 )
                break;

            s.split( DataFileSeparator );
            s.rewind();
            s.next();

            /*
            -----------------------
            Get TCP/IP address info
            ----------------------- */
            curr_s = s.text();
            if (curr_s == TEXT("LocalIpPort"))
            {
                s.next();
                LocalIpPort = s.int_value();
                s.remove_all();
                continue;
            }
            else if (curr_s == TEXT("DsIpPort"))
            {
                s.next();
                DsIpPort = s.int_value();
                s.remove_all();
                continue;
            }

            ncc = new NETWORK_COMPUTER_CLASS;
            if (!ncc)
                return FALSE;

            ncc->ip_addr = s.text();
            s.next();
            ncc->local = s.text();
            s.next();
            ncc->net = s.text();
            extract_computer_name_from_dir( ncc->name, s.text() );
            ncc->use_ip_path();
            s.next();
            ncc->type = asctoint32( s.text() );

            /*
            ----------------------------------------
            The old archiver type was 4, now it is 3
            ---------------------------------------- */
            if ( ncc->type == DATA_SERVER_COMPUTER_TYPE )
                ncc->type = DATA_ARCHIVER_COMPUTER_TYPE;

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
    STRING_CLASS sc;

    if ( count() == 0 )
        return FALSE;

    s = exe_directory();
    s.cat_path( NetSetupDataFile );

    if ( f.open_for_write(s.text()) )
    {
        /*
        ---------------------
        Write TCP/IP settings
        --------------------- */
        if ( !LocalIpAddress.isempty() )
        {
            sc = TEXT( "LocalIpAddress;" );
            sc += LocalIpAddress;
            f.writeline( sc.text() );
        }
        if ( !LocalIpPort.isempty() )
        {
            sc = TEXT( "LocalIpPort;" );
            sc += LocalIpPort;
            f.writeline( sc.text() );
        }
        if ( !DsIpPort.isempty() )
        {
            sc = TEXT( "DsIpPort;" );
            sc += DsIpPort;
            f.writeline( sc.text() );
        }

        rewind();
        while ( next() )
        {
            s = nc->ip_addr.text();
            s.add( nc->local.text() );
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
***********************************************************************/
NETWORK_COMPUTER_CLASS::NETWORK_COMPUTER_CLASS()
{
(*version)      = NullChar;
type            = NO_COMPUTER_TYPE;
is_changed      = false;
is_shared_full  = false;
online          = false;
ip_addr         = EmptyString;
}

/***********************************************************************
*                         NETWORK_COMPUTER_CLASS                       *
*                                  =                                   *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::operator=( NETWORK_COMPUTER_CLASS & sorc )
{
local           = sorc.local;
net             = sorc.net;
type            = sorc.type;
is_changed      = sorc.is_changed;
is_shared_full  = sorc.is_shared_full;
online          = sorc.online;
connectlist     = sorc.connectlist;
name            = sorc.name;
ip_addr         = sorc.ip_addr;
path            = sorc.path;
copystring( version, sorc.version, VERSION_LEN );
}

/***********************************************************************
*                         ~NETWORK_COMPUTER_CLASS                      *
***********************************************************************/
NETWORK_COMPUTER_CLASS::~NETWORK_COMPUTER_CLASS()
{
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
    type = DATA_ARCHIVER_COMPUTER_TYPE;
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

s = path;
s.cat_path( VisiTrakIniFile );

if ( s.file_exists() )
    {
    online = true;
    GetPrivateProfileString( ConfigSection, VersionKey, UnknownString, version, VERSION_LEN+1, s.text() );
    s = path;
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
*                        NETWORK_COMPUTER_CLASS                        *
*                              USE_IP_PATH                             *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::use_ip_path()
{
    static const REGEX replace_name_with_ip( TEXT("\\\\\\\\.*\\\\") );
    STRING_CLASS s_ip;
    STRING s;

    s = net.text();
    s_ip = TEXT("\\\\");
    s_ip += ip_addr;
    s_ip += TEXT("\\");
    s = regex_replace( s, replace_name_with_ip, s_ip.text() );
    path = s.c_str();

    return;
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                            USE_NET_PATH                              *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::use_net_path()
{
    path = net;
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
static void send_new_computer_setup_message( TCHAR * netdirectory )
{
    STRING_CLASS s;
    SOCKET_MSG msg;

    msg.cmd = POKE_DATA_ITEM;
    msg.item = COMPUTER_SETUP_INDEX;
    copystring( msg.data, netdirectory );
    copystring( msg.topic, DEFAULT_TOPIC );

    extract_computer_name_from_dir( s, netdirectory );
    NetSetupClient.set_ip_addr( s.text() );
    NetSetupClient.set_port( LocalIpPort.int_value() );
    NetSetupClient.msg_size = sizeof( SOCKET_MSG );

    if ( NetSetupClient.connect_to_server() )
    {
        NetSetupClient.start_write_thread();
        NetSetupClient.msg_fifo.push( msg );
        NetSetupClient.send_and_close = TRUE;
        SetEvent( NetSetupClient.send_event );
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
            if ( !connectlist.find(t.text()) )
                connectlist.append( t.text() );
        }
     }

    t.empty();
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
        if (  dest.type == WORKSTATION_COMPUTER_TYPE || dest.type == DATA_ARCHIVER_COMPUTER_TYPE )
            status = TRUE;
        break;

    case WORKSTATION_COMPUTER_TYPE:
        if (  dest.type == MONITORING_COMPUTER_TYPE )
            status = TRUE;
        break;

    case DATA_ARCHIVER_COMPUTER_TYPE:
        if (  dest.type == MONITORING_COMPUTER_TYPE )
            status = TRUE;
        break;
    }

return status;
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                     SAVE_data_archiver_CONNECTION                    *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::save_data_archiver_connection( TCHAR * name_to_save )
{
    SETPOINT_CLASS sp;
    NAME_CLASS config_filepath;

    config_filepath = path;
    config_filepath += TEXT( "\\data\\" );
    config_filepath += NETWORK_CSV_FILE;
    sp.get( config_filepath );
    sp.setsize (1 );
    sp[0].desc = TEXT( "DsIpAddress" );
    sp[0].value = ip_addr;
    sp.put( config_filepath );
}

/***********************************************************************
*                        NETWORK_COMPUTER_CLASS                        *
*                     SAVE_data_archiver_CONNECTION                    *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::save_data_archiver_connection()
{
NAME_CLASS s;

if ( extract_computer_name_from_dir(s, ComputerList.entry().net.text()) )
    save_data_archiver_connection( s.text() );
else
    remove_data_archiver_connection();
}

/***********************************************************************
*                    remove_data_archiver_CONNECTION                   *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::remove_data_archiver_connection()
{
save_data_archiver_connection( NoComputerName );
}

/***********************************************************************
*                            lookup_ip_addr                            *
***********************************************************************/
void NETWORK_COMPUTER_CLASS::lookup_ip_addr()
{
    char *addr;
    TCHAR taddr[TCP_ADDRESS_LEN+1];
    HOSTENT *hostent;
    char hostname[DEFAULT_BUFLEN];

    unicode_to_char( hostname, name.text() );
    hostent = gethostbyname( hostname );

    if ( hostent )
    {
        addr = inet_ntoa( **(in_addr**)hostent->h_addr_list );
        char_to_tchar( taddr, addr );
        ip_addr = taddr;
    }
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
    BOOLEAN         have_data_archiver_connection;
    FILE_CLASS      f;
    NAME_CLASS      s;
    DB_TABLE        t;
    NAME_CLASS      my;

    is_changed = false;

    /*
    ------------------------------------------------------------------
    See if there is a data archiver in the list of connected computers
    ------------------------------------------------------------------ */
    have_data_archiver_connection = FALSE;
    tc = connectlist;
    if ( type == MONITORING_COMPUTER_TYPE )
    {
        tc.sort();
        tc.rewind();
        while ( tc.next() )
        {
            extract_computer_name_from_dir( s, tc.text() );
            if ( ComputerList.find(s.text(), DATA_ARCHIVER_COMPUTER_TYPE) )
            {
                save_data_archiver_connection();
                have_data_archiver_connection = TRUE;
                break;
            }
        }
    }

    if ( !have_data_archiver_connection )
        remove_data_archiver_connection();

    s = path;
    s.cat_path( DataString );
    s.cat_path( COMPUTER_CSV );

    /*
    ---------------------
    Get the existing list
    --------------------- */
    get_computer_list( ori, path );

    status = f.open_for_write( s );
    if ( status )
    {
        /*
        --------------------------------------------------------------------------------
        Computer records are just v5 directories, now. The first is the local directory.
        -------------------------------------------------------------------------------- */
        s = local;
        s += CommaChar;
        s += name;
        f.writeline( s );

        tc.rewind();
        while ( tc.next() )
        {
            if ( ComputerList.netfind(tc.text()) )
            {
                /*
                -------------------------------------------------
                Verify network path is not the same as local path
                ------------------------------------------------- */
                if (ComputerList.entry().type != DATA_ARCHIVER_COMPUTER_TYPE && path != tc.text())
                {
                    s = ComputerList.entry().path;
                    s += CommaChar;
                    s += ComputerList.entry().name;
                    f.writeline( s );
                }
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
                            s = ori.text();
                            s += CommaChar;
                            s += tc.text();
                            f.writeline( s );
                            break;
                        }
                    }
                }
            }
        }

        f.close();

        /*
        ---------------------------------------------------------------------------
        Send am event message to tell this computer to update his list of computers
        --------------------------------------------------------------------------- */
        send_new_computer_setup_message( path.text() );
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
    case DATA_ARCHIVER_COMPUTER_TYPE:
        id = DATA_ARCHIVER_COMPUTER_STRING;
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
    if ( type == LOCAL_INSTALL_TYPE )
        {
        dest += EllipsisString;
        dest += TabChar;
        }
    else if ( connectlist.count() > 0 )
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
*                            lookup_ip_addr                            *
***********************************************************************/
void lookup_ip_addr( STRING_CLASS &dest, STRING_CLASS name )
{
    char *addr;
    TCHAR taddr[TCP_ADDRESS_LEN+1];
    HOSTENT *hostent;
    char hostname[DEFAULT_BUFLEN];

    unicode_to_char( hostname, name.text() );
    hostent = gethostbyname( hostname );

    if ( hostent )
    {
        addr = inet_ntoa( **(in_addr**)hostent->h_addr_list );
        char_to_tchar( taddr, addr );
        dest = taddr;
    }
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

/***********************************************************************
*                         EXTRACT_COMPUTER_TYPE                        *
*        This extracts a computer's type from the listbox line.        *
***********************************************************************/
static TCHAR * extract_computer_type( TCHAR * sorc )
{
    static TCHAR s[MAX_PATH + 1];
    TCHAR * dest;
    int nof_tabs = 0;

    *s = NullChar;

    if ( sorc )
    {
        dest = s;
        while ( TRUE )
        {
            *dest = *sorc;

            if ( *dest == TabChar )
                nof_tabs++;

            if ( nof_tabs == 3 )
                break;

            dest++;
            sorc++;
        }
    }

    return s;
}

/***********************************************************************
*                         EXTRACT_LINE_ITEM                            *
*        This extracts the item string from the listbox line.          *
***********************************************************************/
static TCHAR * extract_line_item( TCHAR * sorc, int item )
{
    static TCHAR s[MAX_PATH + 1];
    STRING_CLASS sc;
    int i;

    sc = sorc;

    for ( i=0; i<item; i++ )
    {
        sc.next_field( TEXT('\t') );
    }

    copystring( s, sc.text() );

    return s;
}

/***********************************************************************
*                         EXTRACT_NETWORK_PATH                         *
*        This extracts the network path from the listbox line.         *
***********************************************************************/
static TCHAR * extract_network_path( TCHAR * sorc )
{
    static TCHAR s[MAX_PATH + 1];
    STRING_CLASS sc;

    sc = sorc;
    sc.next_field(TEXT('\t'));
    sc.next_field(TEXT('\t'));
    copystring(s, sc.text());

    return s;
}

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
    if ( ComputerList.entry().online && (ComputerList.entry().type != DATA_ARCHIVER_COMPUTER_TYPE) )
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
    STRING_CLASS ip_field;
    HWND w;
    int  choice;

    root = rootdir;
    w = ComputerWindow.control( IP_ADDRESS_IPBOX );
    ip_field.get_text( w );
    
    if ( ComputerList.netfind(root) && ComputerList.entry().ip_addr == ip_field ) 
    {
        resource_message_box( MainInstance, COMPUTER_ALREADY_IN_LIST_STRING, CANT_COMPLY_STRING, MB_OK | MB_SYSTEMMODAL );
        return false;
    }

    nce = new NETWORK_COMPUTER_CLASS;
    nce->net = root;
    nce->ip_addr.get_text( ComputerWindow.control(IP_ADDRESS_IPBOX) );
    nce->use_ip_path();
    s = nce->path;
    s.cat_path( VisiTrakIniFile );

    if ( !s.file_exists() )
    {
        resource_message_box( MainInstance, NO_VISITRAK_INI_FILE, CANT_COMPLY_STRING, MB_OK | MB_SYSTEMMODAL );
        delete nce;
        return false;
    }

    extract_computer_name_from_dir( nce->name, root.text() );
    nce->extract_type_from_name();
    ini.set_file( s.text() );
    nce->local = ini.get_string( ConfigSection, ThisComputerKey );
    nce->get_online_status();
    
    if ( nce->ip_addr == EmptyString )
        nce->lookup_ip_addr();

    choice = IDCANCEL;

    if ( !CancelAddNewComputers )
    {
        if ( no_duplicate_machines(nce->path) )
        {
            nce->get_connect_list();
            choice = IDOK;
        }
    }

    if ( choice == IDOK )
    {
        ComputerList.add( nce );
        return true;
    }

    delete nce;
    return false;
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
    STRING_ARRAY_CLASS s;
    STRING_CLASS       msg;
    int                n;
    int                i;
    NAME_CLASS         config_filepath;
    SETPOINT_CLASS     sp;

    set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX, EmptyString );
    ComputerList.rewind();
    n = ComputerList.count();

    for ( i=0; i<n; i++ )
    {
        /*
        ------------------------------------------
        Save modifies current index so set it here
        ------------------------------------------ */
        ComputerList.find( i );

        msg = s.text();
        msg += SpaceChar;
        msg += i+1;
        msg += TEXT( " of " );
        msg += n;
        msg.set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX );
        ComputerList.entry().save();
    }

    msg = TEXT( "Complete" );
    msg.set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX );

    /*
    -----------------------------------------------------
    Redisplay the computers to show the new changed flags
    ----------------------------------------------------- */
    fill_computer_listbox();

    /*
    --------------------------
    Write to network.csv files
    -------------------------- */
    ComputerList.rewind();
    while ( ComputerList.next() )
    {
        config_filepath = ComputerList.entry().path;
        config_filepath += TEXT( "\\data\\" );
        config_filepath += NETWORK_CSV_FILE;
        DeleteFile( config_filepath.text() );
        sp.get( config_filepath );
        sp.setsize( 4 );
        sp[0].desc = TEXT( "LocalIpAddress" );
        sp[0].value = ComputerList.entry().ip_addr;
        sp[1].desc = TEXT( "LocalIpPort" );
        sp[1].value = LocalIpPort;
        sp[2].desc = TEXT( "DsIpAddress" );
        sp[2].value = DsIpAddress;
        sp[3].desc = TEXT( "DsIpPort" );
        sp[3].value = DsIpPort;
        sp.put( config_filepath );
    }

    if ( ComputerList.count() )
        ComputerList.write();
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
    if ( ComputerList.entry().online && (ComputerList.entry().type != DATA_ARCHIVER_COMPUTER_TYPE) )
        {
        newlist.add_computer( ComputerList.entry().path );
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
    s.add( extract_line_item(lb.item_text(ip[i]), DIRECTORY_LINE_ITEM) );

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
    if ( ComputerList.netfind(s.text()) )
        ComputerList.remove();
    }

fill_computer_listbox();
}

void populate_ip_address()
{
    HWND w;
    STRING_CLASS s;

    w = ComputerWindow.control( IP_ADDRESS_IPBOX );
    s = ComputerList.entry().ip_addr;
    s = TEXT( "Looking up IP address for: " );
    s += ComputerList.entry().name;
    set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX, s.text() );
    if ( ComputerList.entry().ip_addr.isempty() )
        ComputerList.entry().lookup_ip_addr();
    set_text( ComputerWindow.handle(), CHECKING_DIR_TBOX, EmptyString );
    s = ComputerList.entry().ip_addr;
    s.set_text( w );
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

LISTBOX_CLASS lb;
TCHAR * cp;
int     id;
int     i;
INT     n;
STRING_CLASS      s;
TEXT_LIST_CLASS * mylist;
int               ok_types;
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

    for ( id=MONITORING_COMPUTER_RADIO; id<=DATA_ARCHIVER_RADIO; id++ )
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

for ( id=MONITORING_COMPUTER_RADIO; id<=DATA_ARCHIVER_RADIO; id++ )
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

s = extract_network_path( cp );

ComputerList.rewind();
while ( ComputerList.next() )
    {
    if ( ComputerList.entry().net == s )
        {

        /*
        -------------------------
        Populate Direectory field
        ------------------------- */
        w = ComputerWindow.control( DIRECTORY_TBOX );
        ComputerList.entry().net.set_text( w );

        /*
        -------------------------
        Populate IP Address field
        ------------------------- */
        w = ComputerWindow.control( IP_ADDRESS_IPBOX );
        EnableWindow( w, TRUE );
        populate_ip_address();

        /*
        ------------------------------------
        Press the radio button for this type
        ------------------------------------ */
        switch( ComputerList.entry().type )
            {
            case MONITORING_COMPUTER_TYPE:
                id = MONITORING_COMPUTER_RADIO;
                break;

            case DATA_ARCHIVER_COMPUTER_TYPE:
                id = DATA_ARCHIVER_RADIO;
                break;

            default:
                id = OFFICE_WORKSTATION_RADIO;
                break;
            }

        CheckRadioButton( ComputerWindow.handle(), MONITORING_COMPUTER_RADIO, DATA_ARCHIVER_RADIO, id );

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
                ok_types = WORKSTATION_COMPUTER_TYPE | DATA_ARCHIVER_COMPUTER_TYPE;
                break;

            case WORKSTATION_COMPUTER_TYPE:
                ok_types = MONITORING_COMPUTER_TYPE;
                break;

            case DATA_ARCHIVER_COMPUTER_TYPE:
                ok_types = MONITORING_COMPUTER_TYPE;
                break;
            }

        /*
        ----------------------------
        Save selected computer index
        ---------------------------- */
        i = ComputerList.current_index();

        /*
        ------------------------------------
        Populate connected computers listbox
        ------------------------------------ */
        ComputerList.rewind();
        while ( ComputerList.next() )
            {
            if ( ComputerList.entry().type & ok_types )
                {
                /*
                -------------------------------------------------
                This is available if it is not in my connect list
                ------------------------------------------------- */
                if ( !mylist->find(ComputerList.entry().net.text()) )
                    lb.add( ComputerList.entry().net.text() );
                }
            }

        /*
        -------------------------------
        Restore selected computer index
        ------------------------------- */
        ComputerList.find( i );

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
    STRING_CLASS type;
    STRING_CLASS name;

    lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );

    sn = lb.select_count();
    if ( sn < 1 )
        return;

    n = lb.count();
    for ( i=0; i<n; i++ )
    {
        if ( lb.is_selected(i) )
        {
            type = extract_line_item( lb.item_text(i), TYPE_LINE_ITEM );
            name = extract_line_item( lb.item_text(i), COMPUTER_LINE_ITEM );
            if ( ComputerList.find(name.text(), type.text()) )
            {
                    if (type == TEXT("Data Archiver"))
                    {
                        DsIpAddress = ComputerList.entry().ip_addr;
                    }
                e = &ComputerList.entry();
                e->connectlist.empty();
                e->is_changed = true;

                ComputerList.rewind();
                while ( ComputerList.next() )
                {
                    if ( e->is_connectable(ComputerList.entry()) )
                        e->connectlist.append( ComputerList.entry().net.text() );
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

    case DATA_ARCHIVER_RADIO:
        newtype = DATA_ARCHIVER_COMPUTER_TYPE;
        break;
    }

lb.init( ComputerWindow.handle(), COMPUTER_LISTBOX );
s = extract_line_item( lb.selected_text(), DIRECTORY_LINE_ITEM );
if ( ComputerList.netfind(s.text()) )
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
TCHAR        buf[MAX_PATH];
BROWSEINFO   bi;
ITEMIDLIST * pidlBrowse;
ITEMIDLIST * network_neighborhood;
ITEMIDLIST ** np;
LPMALLOC     ip;

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
*                            UPDATE_COMPUTER                           *
***********************************************************************/
void update_computer( HWND hWnd )
{
    ComputerList.entry().ip_addr.get_text( ComputerWindow.control(IP_ADDRESS_IPBOX) );

    if ( is_checked(ComputerWindow.control(MONITORING_COMPUTER_RADIO)) )
    {
        ComputerList.entry().type = MONITORING_COMPUTER_TYPE;
    }
    else if ( is_checked(ComputerWindow.control(OFFICE_WORKSTATION_RADIO)) )
    {
        ComputerList.entry().type = WORKSTATION_COMPUTER_TYPE;
    }
    else if ( is_checked(ComputerWindow.control(DATA_ARCHIVER_RADIO)) )
    {
        ComputerList.entry().type = DATA_ARCHIVER_COMPUTER_TYPE;
    }

    fill_computer_listbox();
    hWnd = ComputerWindow.control( UPDATE_BUTTON );
    EnableWindow( hWnd, FALSE );
}

/***********************************************************************
*                          ADD_COMPUTER_BY_PATH                        *
***********************************************************************/
void add_computer_by_path( HWND hWnd )
{
NAME_CLASS s;

s.get_text( hWnd, DIRECTORY_TBOX );
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
            add_computer_by_path( hWnd );

            case IDCANCEL:
            EndDialog( hWnd, 0 );
            return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                           SET_NETWORK_SETTINGS                       *
***********************************************************************/
void save_network_settings( HWND hWnd )
{
    STRING_CLASS s;

    s.get_text( hWnd, MAIN_IP_PORT_TBOX );
    LocalIpPort = s.int_value();
    s.get_text( hWnd, DA_IP_PORT_TBOX );
    DsIpPort = s.int_value();
}

/***********************************************************************
*                        SET_LOCAL_IP_ADDRESS_PROC                     *
***********************************************************************/
BOOL CALLBACK set_local_ip_address(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int  id;
    id = LOWORD(wParam);
    STRING_CLASS s;
    SETPOINT_CLASS sp;

    switch (msg)
    {
    case WM_INITDIALOG:
        s = LocalIpPort;
        s.set_text( hWnd, MAIN_IP_PORT_TBOX );
        s = DsIpPort;
        s.set_text( hWnd, DA_IP_PORT_TBOX );
        break;

    case WM_COMMAND:
        switch ( id )
        {
        case PORTS_SAVE_BUTTON:
            save_network_settings( hWnd );
            EndDialog( hWnd, 0 );
            break;

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
    DIRECTORY_STATIC,
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

    w = ComputerWindow.control( DIRECTORY_STATIC );
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
    HWND hWnd;
    STRING_CLASS s;
    STRING_CLASS s2;
    LPNMHDR nm;

    id  = LOWORD( wParam );
    cmd = HIWORD( wParam );

    switch ( msg )
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
            if ( ComputerList.read() )
            {
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
                //browse_computers(w);
            }
            Starting = FALSE;
            return TRUE;

        case WM_INITDIALOG:
            ComputerWindow = w;
            MainWindow.shrinkwrap( w );
            set_text( w, CHECKING_DIR_TBOX, EmptyString );
            hWnd = ComputerWindow.control( IP_ADDRESS_IPBOX );
            EnableWindow( hWnd, FALSE );
            ComputerWindow.post( WM_DBINIT );
            hWnd = ComputerWindow.control(UPDATE_BUTTON);
            EnableWindow( hWnd, FALSE );
            return TRUE;

        case WM_HELP:
            gethelp( w );
            return TRUE;

        case WM_NOTIFY:
            nm = (LPNMHDR) lParam;
            switch ( nm->code )
            {
                case IPN_FIELDCHANGED:
                    if ( nm->idFrom == IP_ADDRESS_IPBOX )
                    {
                        s.get_text( ComputerWindow.control(IP_ADDRESS_IPBOX) );
                        if ( ComputerList.entry().ip_addr != s )
                        {
                            /*
                            ---------------------------------------------------
                            IP Address text has changed so enable Update button
                            --------------------------------------------------- */
                            hWnd = ComputerWindow.control( UPDATE_BUTTON );
                            EnableWindow( hWnd, TRUE );
                            return TRUE;
                        }
                    }
                    break;
            }
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

                case ADD_BUTTON:
                    add_computer_by_path( w );
                    return TRUE;

                case SET_IP_PORTS_BUTTON:
                    DialogBox( MainInstance, TEXT("SET_IP_PORTS_DLG"), ComputerWindow.handle(), (DLGPROC) set_local_ip_address );
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
                case DATA_ARCHIVER_RADIO:
                    //set_computer_type( id );
                    hWnd = ComputerWindow.control( UPDATE_BUTTON );
                    EnableWindow( hWnd, TRUE );
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
                        hWnd = ComputerWindow.control( ADD_BUTTON );
                        EnableWindow( hWnd, FALSE );
                        hWnd = ComputerWindow.control( UPDATE_BUTTON );
                        EnableWindow( hWnd, FALSE );
                        return TRUE;
                    }
                    break;

                case DIRECTORY_TBOX:
                    if (cmd == EN_CHANGE )
                    {
                        hWnd = ComputerWindow.control( ADD_BUTTON );
                        EnableWindow( hWnd, TRUE );
                        hWnd = ComputerWindow.control( UPDATE_BUTTON );
                        EnableWindow( hWnd, FALSE );
                        return TRUE;
                    }
                    break;

                case UPDATE_BUTTON:
                    update_computer( w );
                    break;

                case LOOKUP_IP_BUTTON:
                    hWnd = ComputerWindow.control( DIRECTORY_TBOX );
                    s.get_text( hWnd );
                    extract_computer_name_from_dir( s2, s.text() );
                    lookup_ip_addr( s, s2 );
                    hWnd = ComputerWindow.control( IP_ADDRESS_IPBOX );
                    s.set_text( hWnd );
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
    WSADATA wsaData;
    STRING_CLASS e;
    int status;

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
    e = TEXT( "netsetup.log" );
    ServiceLog.set_log_file( e );

    /*
    ------------------
    Initialize Winsock
    ------------------ */
    status = WSAStartup( MAKEWORD(2, 0), &wsaData );

    if (status != 0)
    {
        e = TEXT("WSAStartup failed: ");
        e += status;
        ServiceLog.add_entry( e );
    }
}
/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown()
{
    NETWORK_COMPUTER_CLASS nce;

    nce.cleanup();
    CoUninitialize();
    WSACleanup();
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
    return( msg.wParam );
}
