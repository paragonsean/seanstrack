#include <windows.h>
#include "..\include\ugctrl.h"

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\autoback.h"
#include "..\include\chars.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\events.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\stringtable.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"
#include "..\include\statics.h"

#include "resource.h"
#include "password.h"
#include "extern.h"

static TCHAR DefaultFontName[] = TEXT( "MS Sans Serif" );
static TCHAR DefaultPasswordLogFormat[] = TEXT( "c d t n" );
static TCHAR DefaultPasswordName[] = DEFAULT_PASSWORD_NAME;

static STRING_CLASS PasswordLogFormat;

static HWND    EnterWindow = 0;
static HWND    EditWindow  = 0;
static HWND    CoOptionsWindow  = 0;
static BOOLEAN ProgramIsUpdating  = FALSE;

static STRING_CLASS LogFileName;

static PASSWORD_CLASS Password;

class PW_GRID_CLASS : public CUGCtrl
{
private:
int     my_font_index;
void    createfont();

public:

PW_GRID_CLASS();
~PW_GRID_CLASS();
void OnCharDown( UINT *vcKey, BOOL processed );
int  OnEditContinue( int oldcol, long oldrow, int * newcol, long * newrow );
int  OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag);
void OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed );
void OnKeyDown( UINT *vcKey, BOOL processed );
void OnSetup();
void delete_row();
void fill_grid();
void insert_row();
void set_row_colors();
void set_row_numbers();
};

PW_GRID_CLASS Grid;
const int  NofCols  = 3;

NAME_CLASS CoOptionsExeDir;

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                             PW_GRID_CLASS                            *
***********************************************************************/
PW_GRID_CLASS::PW_GRID_CLASS()
{
createfont();
}

/***********************************************************************
*                            ~PW_GRID_CLASS                            *
***********************************************************************/
PW_GRID_CLASS::~PW_GRID_CLASS()
{
RemoveFont( my_font_index );
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                             OnEditFinish                             *
***********************************************************************/
int PW_GRID_CLASS::OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag)
{
STRING_CLASS s;
int r;
int n;

if ( col == 1 )
    {
    n   = GetNumberRows();
    for ( r=0; r<n; r++ )
        {
        if ( r != row )
            {
            s = QuickGetText( col, r );
            if ( s == string )
                {
                resource_message_box( m_ctrlWnd, MainInstance, PW_DUPLICATE_STRING, INPUT_ERROR_STRING, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
                return FALSE;
                }
            }
        }
    }

return TRUE;
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                             OnEditContinue                           *
***********************************************************************/
int PW_GRID_CLASS::OnEditContinue( int oldcol, long oldrow, int * newcol, long * newrow )
{
GotoCell( *newcol, *newrow );
return FALSE;
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              OnLClicked                              *
***********************************************************************/
void PW_GRID_CLASS::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed )
{
if ( updn )
    StartEdit();
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                               OnKeyDown                              *
***********************************************************************/
void PW_GRID_CLASS::OnKeyDown( UINT *vcKey, BOOL processed )
{
if ( (*vcKey) == VK_INSERT )
    insert_row();
else if ( (*vcKey) == VK_DELETE )
    delete_row();

}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              OnCharDown                              *
***********************************************************************/
void PW_GRID_CLASS::OnCharDown( UINT *vcKey, BOOL processed )
{
if ( (*vcKey) == VK_TAB )
    {
    int n;
    int row;
    int col;

    col = GetCurrentCol();
    n   = GetNumberRows();
    row = GetCurrentRow();

    if ( col < (NofCols-1) )
        {
        col++;
        }
    else if ( row < (n-1) )
        {
        row++;
        col = 0;
        }
    else
        {
        row = 0;
        col = 0;
        }
    GotoCell( col, row );
    }
else if( !processed )
    {
    StartEdit( *vcKey );
    }
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                            SET_ROW_COLORS                            *
***********************************************************************/
void PW_GRID_CLASS::set_row_colors()
{
int  col;
long row;
long n;
COLORREF color;

n = GetNumberRows();
for ( row=0; row<n; row++ )
    {
    if ( (row/2)*2 == row )
        color = RGB( 252, 240, 190 );
    else
        color = RGB( 255, 255, 255 );

    for ( col=0; col<NofCols; col++ )
        QuickSetBackColor( col, row, color );
    }
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              CREATEFONT                              *
***********************************************************************/
void PW_GRID_CLASS::createfont( void )
{
LOGFONT    lf;

lf.lfHeight          = 18;
lf.lfWidth           = 0;
lf.lfEscapement      = 0;
lf.lfOrientation     = 0;
lf.lfWeight          = FW_DONTCARE;
lf.lfItalic          = FALSE;
lf.lfUnderline       = FALSE;
lf.lfStrikeOut       = FALSE;
lf.lfCharSet         = ANSI_CHARSET;
lf.lfOutPrecision    = 0;
lf.lfClipPrecision   = 0;
lf.lfQuality         = DEFAULT_QUALITY;
lf.lfPitchAndFamily  = 0;

lstrcpy( lf.lfFaceName, DefaultFontName );

if (PRIMARYLANGID(GetUserDefaultLangID())==LANG_JAPANESE)
    {
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lstrcpy(lf.lfFaceName,TEXT("MS –¾’©"));
    }

ClearAllFonts();

my_font_index = AddFont(
    lf.lfHeight,
    lf.lfWidth,
    lf.lfEscapement,
    lf.lfOrientation,
    lf.lfWeight,
    lf.lfItalic,
    lf.lfUnderline,
    lf.lfStrikeOut,
    lf.lfCharSet,
    lf.lfOutPrecision,
    lf.lfClipPrecision,
    lf.lfQuality,
    lf.lfPitchAndFamily,
    lf.lfFaceName
    );

if ( SetDefFont(my_font_index) != UG_SUCCESS )
    MessageBox( 0, TEXT("SetDefFont"), TEXT("No Font at this index"), MB_OK );
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              DELETE_ROW                              *
***********************************************************************/
void PW_GRID_CLASS::delete_row()
{
if ( GetNumberRows() > 0 )
    DeleteRow( GetCurrentRow() );

if ( GetNumberRows() > 0 )
    {
    set_row_colors();
    set_row_numbers();
    }

RedrawAll();
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              INSERT_ROW                              *
***********************************************************************/
void PW_GRID_CLASS::insert_row()
{
CUGCell cell;
int  col;
long n;
long row;

n = GetNumberRows();

if ( n > 0 )
    row = GetCurrentRow();
else
    row = 0;

InsertRow( row );

GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );
cell.SetText( EmptyString );

for ( col=0; col<NofCols; col++ )
    SetCell( col, row, &cell );

set_row_colors();
set_row_numbers();

RedrawAll();

}

/***********************************************************************
*                              PW_GRID_CLASS                           *
*                             SET_ROW_NUMBERS                          *
***********************************************************************/
void PW_GRID_CLASS::set_row_numbers()
{
CUGCell cell;
long    row;
long    n;

GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_LTHIN );

n = GetNumberRows();
for ( row=0; row<n; row++ )
    {
    cell.SetText( int32toasc(row) );
    SetCell( -1, row, &cell );
    }
}

/***********************************************************************
*                              PW_GRID_CLASS                           *
*                               FILL_GRID                              *
***********************************************************************/
void PW_GRID_CLASS::fill_grid()
{
CUGCell cell;
int    row;
int    n;

n = Password.count();

if ( n != GetNumberRows() )
    SetNumberRows( n, FALSE );

GetGridDefault( &cell );

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );
for ( row=0; row<n; row++ )
    {
    cell.SetText( int32toasc(Password.pw[row].level) );
    SetCell( 0, row, &cell );

    cell.SetText( Password.pw[row].name );
    SetCell( 1, row, &cell );

    cell.SetText( Password.pw[row].owner );
    SetCell( 2, row, &cell );
    }

set_row_numbers();
set_row_colors();
}

/***********************************************************************
*                              PW_GRID_CLASS                           *
*                                 OnSetup                              *
***********************************************************************/
void PW_GRID_CLASS::OnSetup()
{
CUGCell cell;
HDC     dc;
int     h;
int     wm1;
int     w[NofCols];
RECT    r;

Grid.GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );

dc = GetDC( m_ctrlWnd );
if ( dc != NULL )
    {
    h = (int) character_height( dc );
    wm1 = (int) pixel_width( dc, TEXT("000") );
    w[0]= (int) pixel_width( dc, TEXT("Label") );
    w[1]= (int) pixel_width( dc, TEXT("a") );
    w[1] *= PASSWORD_LEN;
    ReleaseDC( m_ctrlWnd, dc );
    }

h += 2;

GetWindowRect( Grid.m_ctrlWnd, &r );
r.right -= GetVS_Width();
r.left  += wm1;
r.left  += w[0];
r.left  += w[1];
w[2] = (r.right - r.left );

SetDefRowHeight( h );
wm1 += 2;
SetSH_Width( wm1 );

SetNumberCols( NofCols );
Set3DHeight( 0 );

cell.SetText( EmptyString );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN | UG_BDR_LTHIN );
SetCell( -1, -1, &cell );

SetColWidth( 0, w[0] );
cell.SetText( resource_string(PW_COL_0_STRING) );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN );
SetCell( 0, -1, &cell );

SetColWidth( 1, w[1] );
cell.SetText( resource_string(PW_COL_1_STRING) );
SetCell( 1, -1, &cell );

SetColWidth( 2, w[2] );
cell.SetText( resource_string(PW_COL_2_STRING) );
SetCell( 2, -1, &cell );

fill_grid();
}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void PASSWORD_CLASS::cleanup()
{
if ( pw )
    {
    delete[] pw;
    pw = 0;
    n  = 0;
    }
}

/***********************************************************************
*                           ~PASSWORD_CLASS                            *
***********************************************************************/
PASSWORD_CLASS::~PASSWORD_CLASS()
{
cleanup();
}

/**********************************************************************
*                             AM_I_ENCRYPTED                          *
**********************************************************************/
static BOOLEAN am_i_encrypted()
{
NAME_CLASS s;
INI_CLASS  ini;
s.get_local_ini_file_name( DisplayIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
return ini.get_boolean(PasswordsAreEncriptedKey);
}

#ifdef UNICODE
static int MaxCharValue = 0xFFFF;
static int Adjustor     = 0xFFDF;
#else
static int MaxCharValue = 0xFF;
static int Adjustor     = 0xDF;
#endif

/**********************************************************************
*                            ENCRYPT_PASSWORD                         *
**********************************************************************/
void encrypt_password( TCHAR * sorc )
{
union {
    TCHAR c;
    int   ic;
    };
int  ec;
int  i;

ec = (int) TEXT( '{' );
for ( i=0; i<PASSWORD_LEN; i++ )
    {
    if ( *sorc == SpaceChar )
        continue;
    ic = 0;
    c  = *sorc;
    ic += ec;
    if ( ic > MaxCharValue )
        ic -= Adjustor;
    *sorc = c;
    sorc++;
    ec++;
    }
}

/**********************************************************************
*                            DECRYPT_PASSWORD                         *
**********************************************************************/
static void decrypt_password( TCHAR * sorc )
{
union {
    TCHAR c;
    int   ic;
    };
TCHAR ec;
int   i;

ec = (int) TEXT( '{' );
for ( i=0; i<PASSWORD_LEN; i++ )
    {
    if ( *sorc == SpaceChar )
        continue;
    ic = 0;
    c  = *sorc;
    ic -= ec;
    if ( ic < 0 )
        ic += Adjustor;
    *sorc = c;
    sorc++;
    ec++;
    }
}

/***********************************************************************
*                         PASSWORD_CLASS::LOAD                         *
***********************************************************************/
bool PASSWORD_CLASS::load()
{
NAME_CLASS s;
DB_TABLE   t;
TCHAR    p[PASSWORD_LEN+1];
int32      i;
short      reclen;
BOOLEAN  is_encrypted;

cleanup();
is_encrypted = am_i_encrypted();
s.get_new_password_dbname();
if ( s.file_exists() )
    {
    reclen = NEW_PASSWORD_RECLEN;
    }
else
    {
    s.get_password_dbname();
    reclen = PASSWORD_RECLEN;
    }

if ( !s.file_exists() )
    return false;

if ( t.open(s, reclen, PFL) )
    {
    if ( setsize(t.nof_recs()) )
        {
        i = 0;
        while ( i < n )
            {
            if ( !t.get_next_record(FALSE) )
                break;
            pw[i].level = t.get_long( PASSWORD_LEVEL_OFFSET );
            t.get_alpha( p,  PASSWORD_OFFSET, PASSWORD_LEN, 0 );
            if ( is_encrypted )
                decrypt_password( p );
            strip( p );
            copystring( pw[i].name, p );
            if ( reclen == NEW_PASSWORD_RECLEN )
                t.get_alpha( pw[i].owner, PASSWORD_OWNER_OFFSET, PASSWORD_OWNER_LEN );
            i++;
            }
        }

    t.close();
    }

if ( n > 0 )
    return true;

return false;
}

/***********************************************************************
*                         PASSWORD_CLASS::SAVE                         *
***********************************************************************/
bool PASSWORD_CLASS::save()
{
NAME_CLASS s;
DB_TABLE   t;
TCHAR    p[PASSWORD_LEN+1];
int32      i;
BOOLEAN  is_encrypted;

if ( !pw )
    return false;
is_encrypted = am_i_encrypted();

s.get_new_password_dbname();
if ( !s.file_exists() )
    t.create( s );

if ( t.open(s, NEW_PASSWORD_RECLEN, FL) )
    {
    t.empty();
    for ( i=0; i<n; i++ )
        {
        t.put_long(  PASSWORD_LEVEL_OFFSET,  pw[i].level, PASSWORD_LEVEL_LEN );
	    copyfromstr( p, pw[i].name, PASSWORD_LEN );
	    p[PASSWORD_LEN] = NullChar;
	    if ( is_encrypted )
	        encrypt_password( p );
	    t.put_alpha( PASSWORD_OFFSET,        p,          PASSWORD_LEN );
        t.put_alpha( PASSWORD_OWNER_OFFSET,  pw[i].owner, PASSWORD_OWNER_LEN );
        t.rec_append();
        }

    t.close();
    }

return true;
}

/***********************************************************************
*                         PASSWORD_CLASS::FIND                        *
***********************************************************************/
int32 PASSWORD_CLASS::find( TCHAR * password_to_find )
{
int32 i;

if ( pw )
    {
    for ( i=0; i<n; i++ )
        {
        if ( strings_are_equal(pw[i].name, password_to_find) )
            return i;
        }
    }

return NO_INDEX;
}

/***********************************************************************
*                          PASSWORD_CLASS                              *
*                               []                                     *
***********************************************************************/
PASSWORD_ENTRY & PASSWORD_CLASS::operator[]( int32 i )
{
static PASSWORD_ENTRY EmptyPasswordEntry;

if ( pw )
    {
    if ( i>=0 && i<n )
        return pw[i];
    }

return EmptyPasswordEntry;
}

/***********************************************************************
*                          PASSWORD_CLASS                              *
*                          DEFAULT_LEVEL                               *
***********************************************************************/
int32 PASSWORD_CLASS::default_level()
{
int32 i;
int32 default_level;

default_level = NO_PASSWORD_LEVEL;
i = find( DefaultPasswordName );
if ( i != NO_INDEX )
    default_level = pw[i].level;

return default_level;
}

/***********************************************************************
*                     PASSWORD_CLASS::HIGHEST_LEVEL                    *
***********************************************************************/
int32 PASSWORD_CLASS::highest_level()
{
int32 i;
int32 h;

h = NO_PASSWORD_LEVEL;

if ( pw )
    {
    for ( i=0; i<n; i++ )
        {
        if ( h < pw[i].level )
            h = pw[i].level;
        }
    }

return h;
}

/***********************************************************************
*                            PASSWORD_CLASS                            *
*                               SETSIZE                                *
***********************************************************************/
bool PASSWORD_CLASS::setsize( int32 new_count )
{
if ( new_count < 0 )
    return false;

if ( n != new_count )
    {
    cleanup();

    if ( new_count > 0 )
        {
        pw = new PASSWORD_ENTRY[new_count];
        if ( !pw )
            new_count = 0;
        }

    n = new_count;
    }

if ( n > 0 )
    return true;

return false;
}

/***********************************************************************
*                      CHECK_FOR_PASSWORD_LOGGING                      *
***********************************************************************/
bool check_for_password_logging()
{
LogFileName = get_long_ini_string( VisiTrakIniFile, ConfigSection, TEXT("PasswordLogFile") );
if ( !unknown(LogFileName.text()) )
    {
    PasswordLogFormat = get_long_ini_string( VisiTrakIniFile, ConfigSection, TEXT("PasswordLogFormat") );
    if ( unknown(PasswordLogFormat.text()) )
        PasswordLogFormat = DefaultPasswordLogFormat;
    return true;
    }
else
    {
    LogFileName.empty();
    return false;
    }
}

/***********************************************************************
*                             LOG_PASSWORD                             *
***********************************************************************/
static void log_password( TCHAR * owner )
{
TCHAR * dest;
TCHAR * fs;
TCHAR * s;
int32   nof_chars;
TIME_CLASS   t;
FILE_CLASS   f;
TCHAR buf[MAX_PATH];
DWORD buf_size = MAX_PATH;

if ( !check_for_password_logging() )
    return;

if ( PasswordLogFormat.isempty() )
    PasswordLogFormat = DefaultPasswordLogFormat;

t.get_local_time();
GetComputerName( buf, &buf_size );

nof_chars = PasswordLogFormat.len();
nof_chars += ALPHADATE_LEN;
nof_chars += ALPHATIME_LEN;
nof_chars += PASSWORD_OWNER_LEN;
nof_chars  += 20;

s = maketext( nof_chars );
if ( s )
    {
    dest = s;
    fs   = PasswordLogFormat.text();

    while ( true )
        {
        if ( *fs == CChar || *fs == CCharLow )
            {
            dest = copystring( dest, buf );
            }
        else if ( *fs == DChar || *fs == DCharLow )
            {
            dest = copystring( dest, t.mmddyyyy() );
            }
        else if ( *fs == TChar || *fs == TCharLow )
            {
            dest = copystring( dest, t.hhmmss() );
            }
        else if ( *fs == NChar || *fs == NCharLow )
            {
            dest = copystring( dest, owner );
            }
        else
            {
            *dest++ = *fs;
            }

        if ( *fs == NullChar )
            break;

        fs++;
        }

    if ( f.open_for_append( LogFileName.text() ) )
        {
        f.writeline( s );
        f.close();
        }

    delete[] s;
    }
}

/***********************************************************************
*                        DEFAULT_PASSWORD_LEVEL                        *
* This returns the current default password level or NO_PASSWORD_LEVEL *
* if none exists.                                                      *
***********************************************************************/
int default_password_level()
{
PASSWORD_CLASS p;

p.load();
return p.default_level();
}

/***********************************************************************
*                      SET_DEFAULT_PASSWORD_LEVEL                      *
***********************************************************************/
void set_default_password_level( bool need_log )
{
int            default_level;
int            i;
PASSWORD_CLASS p;

default_level = NO_PASSWORD_LEVEL;

p.load();
i = p.find( DefaultPasswordName );
if ( i != NO_INDEX )
    {
    default_level = p[i].level;
    if ( need_log && check_for_password_logging() )
        {
        if ( is_empty(p[i].owner) )
            copystring( p[i].owner, UnknownString );
        log_password( p[i].owner );
        }
    }

if ( default_level != NO_PASSWORD_LEVEL )
    poke_data( DDE_CONFIG_TOPIC, ItemList[PW_LEVEL_INDEX].name, int32toasc(default_level) );
}

/***********************************************************************
*                      COPY_PASSWORDS_TO_NETWORK                       *
***********************************************************************/
static void copy_passwords_to_network()
{
COMPUTER_CLASS c;
INI_CLASS      ini;
TCHAR        * cp;
NAME_CLASS     s;
NAME_CLASS     dest;
NAME_CLASS     sorc;
BOOLEAN is_encrypted;
BOOLEAN ok_to_copy;

is_encrypted = is_checked( EditWindow, ENCRYPT_PASSWORDS_XBOX );
sorc.get_new_password_dbname();

c.rewind();
while ( c.next() )
    {
    if ( !c.is_this_computer() )
        {
        if ( c.is_present() )
            {
            ok_to_copy = TRUE;
            if ( is_encrypted )
                {
                s.get_exe_dir_file_name( c.name(), VisiTrakIniFile );
                ini.set_file( s.text() );
                ini.set_section( ConfigSection );
                s = ini.get_string( VersionKey );
                cp = s.text();
                if ( asctoint32(cp) < 7 )
                    {
                    ok_to_copy = FALSE;
                    }
                else
                    {
                    cp = s.find( PeriodChar );
                    if ( !cp )
                        {
                        ok_to_copy = FALSE;
                        }
                    else
                        {
                        cp++;
                        if ( asctoint32(cp) < 7 )
                            ok_to_copy = FALSE;
                        }
                    }
                }
            if ( ok_to_copy )
                {
                /*
                -------------------------------------------
                Update the ini file on the network computer
                ------------------------------------------- */
                s.get_exe_dir_file_name( c.name(), DisplayIniFile );
                ini.set_file( s.text() );
                ini.set_section( ConfigSection );
                ini.put_boolean( PasswordsAreEncriptedKey, is_encrypted );



                /*
                ----------------------
                Copy the password file
                ---------------------- */
	            dest.get_new_password_dbname( c.name() );
	            sorc.copyto( dest );
                }
	        }
	     else
            {
			s = StringTable.find( TEXT("SOME_VERSIONS_TOO_OLD_STRING") );
            dest = StringTable.find( TEXT("CANNOT_COMPLY_STRING") );
            MessageBox( 0, dest.text(), s.text(), MB_OK );
            }
        }
    }
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static bool save_changes()
{
AUTO_BACKUP_CLASS ab;
NAME_CLASS s;
INI_CLASS  ini;
int        row;
int        col;
int        n;
bool       ok_to_save;

ok_to_save = true;

n = Grid.GetNumberRows();
if ( n > 0 )
    {
    for ( row=0; row<n; row++ )
        {
        for ( col=0; col<2; col++ )
            {
            s = Grid.QuickGetText( col, row );
            if ( s.isempty() )
                {
                ok_to_save = false;
                goto finished;
                }
            }
        }

    finished:
    if ( !ok_to_save )
        {
        resource_message_box( EditWindow, MainInstance, PW_BLANK_STRING, INPUT_ERROR_STRING, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
        Grid.GotoCell( col, row );
        SetFocus( Grid.m_ctrlWnd );
        return false;
        }

    s.get_local_ini_file_name( DisplayIniFile );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    ini.put_boolean(PasswordsAreEncriptedKey, is_checked(EditWindow, ENCRYPT_PASSWORDS_XBOX) );

    if ( Password.setsize(n) )
        {
        for ( row=0; row<n; row++ )
            {
            s = Grid.QuickGetText( 0, row );
            Password.pw[row].level = s.int_value();
            s = Grid.QuickGetText( 1, row );
            lstrcpyn( Password.pw[row].name,  s.text(), PASSWORD_LEN+1 );
            s = Grid.QuickGetText( 2, row );
            lstrcpyn( Password.pw[row].owner, s.text(), PASSWORD_OWNER_LEN+1 );
            }
        Password.save();

        if ( is_checked(EditWindow, COPY_TO_NETWORK_XBOX) )
            {
            copy_passwords_to_network();
            }
        ab.get_backup_dir();
        ab.backup_data_dir();
        }
    }

return true;
}

/***********************************************************************
*                            COOPTIONSHELP                             *
***********************************************************************/
static void cooptionshelp( UINT helptype, DWORD context )
{
static TCHAR helpfile[] = TEXT( "v5help.hlp" );

NAME_CLASS s;

if ( s.get_exe_dir_file_name(helpfile) )
    WinHelp( CoOptionsWindow, s.text(), helptype, context );
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( HWND hWnd, UINT helptype, DWORD context )
{
static TCHAR helpfile[] = TEXT( "password.hlp" );

NAME_CLASS s;

if ( s.get_exe_dir_file_name(helpfile) )
    WinHelp( hWnd, s.text(), helptype, context );
}

/***********************************************************************
*                         EXIT_EDIT_PASSWORDS                          *
***********************************************************************/
static void exit_edit_passwords()
{
EndDialog( EditWindow, 0 );
EditWindow = 0;
PostMessage( MainWindow, WM_NEW_DATA, PASSWORD_DIALOG_REQ, 0L );
}

/***********************************************************************
*                    SET_EDIT_PASSWORDS_SETUP_STRINGS                  *
***********************************************************************/
static void set_edit_passwords_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK, TEXT("SAVE_CHANGES") },
    { COPY_TO_NETWORK_XBOX, TEXT("COPY_TO_NETWORK") },
    { ENCRYPT_PASSWORDS_XBOX, TEXT("ENCRYPT_PW") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("EDIT_PW")) );
w.refresh();
}

/***********************************************************************
*                          EDIT_PASSWORDS_PROC                         *
*                     Proc for EDIT_PASSWORDS_DIALOG                   *
***********************************************************************/
BOOL CALLBACK edit_passwords_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        EditWindow = hWnd;
        set_edit_passwords_setup_strings( hWnd );
        RECT r;
        RECT sr;
        GetClientRect(hWnd,&r);
        GetWindowRect( GetDlgItem(hWnd, IDOK), &sr );
        r.bottom += sr.top - sr.bottom;
        Grid.CreateGrid( WS_CHILD|WS_VISIBLE, r, hWnd, PW_GRID_ID );
        set_checkbox( hWnd, ENCRYPT_PASSWORDS_XBOX, am_i_encrypted() );
        break;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( hWnd, HELP_CONTEXT, PASSWORD_EDIT_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( !save_changes() )
                    return TRUE;

            case IDCANCEL:
                exit_edit_passwords();
                return TRUE;

            }
        break;

    case WM_CLOSE:
        exit_edit_passwords();
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         EXIT_CO_OPTIONS                              *
***********************************************************************/
static void exit_co_options()
{
HWND w;
w = CoOptionsWindow;
if ( w )
    {
    CoOptionsWindow = 0;
    EndDialog( w, 0 );
    PostMessage( MainWindow, WM_NEW_DATA, PASSWORD_DIALOG_REQ, 0L );
    }
}

/***********************************************************************
*                           FILL_CO_OPTIONS                            *
***********************************************************************/
static void fill_co_options()
{
int          i;
int          id;
HWND         w;
BOOLEAN      b;
INI_CLASS    ini;
NAME_CLASS s;

w = CoOptionsWindow;

/*
-----------
Display.ini
----------- */
s.get_exe_dir_file_name( DisplayIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

/*
------------------
Snapshot is active
------------------ */
b = TRUE;
if ( ini.exists() )
    {
    if ( ini.find(SnapshotKey) )
        b = ini.get_boolean();
    }
set_checkbox( w, ENABLE_SNAPSHOT_XBOX, b );

if ( ini.find(LowLimitTypeKey) )
    {
    id = LOW_LIMIT_LOW_ALARM_RADIO;
    i  = ini.get_int();
    switch( i )
        {
        case ALARM_MIN:
            id = LOW_LIMIT_LOW_ALARM_RADIO;
            break;

        case ALARM_MAX:
            id = LOW_LIMIT_HIGH_ALARM_RADIO;
            break;

        case WARNING_MIN:
            id = LOW_LIMIT_LOW_WARN_RADIO;
            break;

        case WARNING_MAX:
            id = LOW_LIMIT_HIGH_WARN_RADIO;
            break;
        }
    }
CheckRadioButton( w, LOW_LIMIT_LOW_ALARM_RADIO, LOW_LIMIT_HIGH_ALARM_RADIO, id );

if ( ini.find(HighLimitTypeKey) )
    {
    id = LOW_LIMIT_LOW_ALARM_RADIO;
    i  = ini.get_int();
    switch( i )
        {
        case ALARM_MIN:
            id = HIGH_LIMIT_LOW_ALARM_RADIO;
            break;

        case ALARM_MAX:
            id = HIGH_LIMIT_HIGH_ALARM_RADIO;
            break;

        case WARNING_MIN:
            id = HIGH_LIMIT_LOW_WARN_RADIO;
            break;

        case WARNING_MAX:
            id = HIGH_LIMIT_HIGH_WARN_RADIO;
            break;
        }
    }
CheckRadioButton( w, HIGH_LIMIT_LOW_ALARM_RADIO, HIGH_LIMIT_HIGH_ALARM_RADIO, id );

/*
------------
Visitrak.ini
------------ */
s.get_exe_dir_file_name( VisiTrakIniFile );
ini.set_file( s.text() );

set_checkbox( w, PART_BASED_DISPLAY_XBOX, ini.get_boolean(ConfigSection, PartBasedDisplayKey) );
set_checkbox( w, POPUP_DOWNTIME_XBOX,     ini.get_boolean(ConfigSection, UseDowntimeEntryKey) );
set_checkbox( w, HAVE_SURETRAK_XBOX,      ini.get_boolean(ConfigSection, HaveSureTrakKey)     );
set_checkbox( w, NO_NETWORK_XBOX,         ini.get_boolean(ConfigSection, NoNetworkKey) );

if ( ini.find(ButtonLevelsSection, DowntimePurgeButtonKey) )
    set_text( w, PURGE_DOWNTIMES_EBOX, ini.get_string() );

if ( ini.find(ButtonLevelsSection, EditOperatorsButtonKey) )
    set_text( w, EDIT_OPERATORS_EBOX, ini.get_string() );

if ( ini.find( ConfigSection, PasswordLevelTimeoutKey) )
    set_text( w, PASSWORD_LEVEL_TIMEOUT_EBOX, ini.get_string() );

/*
----------
Monall.ini
---------- */
s.get_exe_dir_file_name( MonallIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
set_checkbox( w, SUB_TIMEOUT_XBOX,                ini.get_boolean(SubtractTimeoutKey)        );
set_checkbox( w, SAVE_ALL_ALARM_XBOX,             ini.get_boolean(SaveAllAlarmShotsKey)      );
set_checkbox( w, CLEAR_MONITOR_WIRE_ON_DOWN_XBOX, ini.get_boolean(ClearMonitorWireOnDownKey) );
set_checkbox( w, CLEAR_FROM_POPUP_XBOX,           ini.get_boolean(ClearDownWireFromPopupKey) );
set_checkbox( w, CLEAR_ALARMS_EACH_SHOT_XBOX,     ini.get_boolean(ClearAlarmsEachShotKey)    );
set_checkbox( w, TURN_OFF_ALARMS_ON_DOWN_XBOX,    ini.get_boolean(TurnOffAlarmsOnDownKey)    );
set_checkbox( w, LOG_SHOTPARM_ACCESS_XBOX,        ini.get_bool(LogShotparmAccessKey)         );

id = START_FT2_ON_STATUS_RADIO;
if ( ini.find(Ft2StartScreenKey) )
    {
    s = ini.get_string();
    if ( s == START_FT2_ON_REALTIME )
        id = START_FT2_ON_REALTIME_RADIO;
    }
CheckRadioButton( w, START_FT2_ON_STATUS_RADIO, START_FT2_ON_REALTIME_RADIO, id );

s = ini.get_string( Ft2F10LevelKey );
if ( unknown(s.text()) )
    s = EmptyString;
s.set_text( w, F10_CONTROL_MODS_BUTTON_EB );

s = ini.get_string( Ft2F2LevelKey );
if ( unknown(s.text()) )
    s = EmptyString;
s.set_text( w, F2_SET_DAC_BUTTON_EB );

s = ini.get_string( Ft2F3LevelKey );
if ( unknown(s.text()) )
    s = EmptyString;
s.set_text( w, F3_NULL_VALVE_BUTTON_EB );

s = ini.get_string( Ft2F4LevelKey );
if ( unknown(s.text()) )
    s = EmptyString;
s.set_text( w, F4_VALVE_TEST_BUTTON_EB );

s = ini.get_string( Ft2F5LevelKey );
if ( unknown(s.text()) )
    s = EmptyString;
s.set_text( w, F5_TEST_ALARMS_BUTTON_EB );

s = ini.get_string( Ft2F6LevelKey );
if ( unknown(s.text()) )
    s = EmptyString;
s.set_text( w, F6_STATUS_BUTTON_EB );

s = ini.get_string( Ft2F7LevelKey );
if ( unknown(s.text()) )
    s = EmptyString;
s.set_text( w, F7_TERMINAL_BUTTON_EB );

s = ini.get_string( Ft2F8LevelKey );
if ( unknown(s.text()) )
    s = EmptyString;
s.set_text( w, F8_REALTIME_BUTTON_EB );

s = ini.get_string( Ft2F9LevelKey );
if ( unknown(s.text()) )
    s = EmptyString;
s.set_text( w, F9_GLOBALS_BUTTON_EB );

if ( ini.find(DownTimeAlarmWireKey) )
    set_text( w, ALARM_ON_DOWN_WIRE_EBOX, ini.get_string() );

if ( ini.find(MonitorWireKey) )
    set_text( w, MONITOR_WIRE_EBOX, ini.get_string() );

/*
------------
Suretrak.ini
------------ */
s.get_exe_dir_file_name( SureTrakIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( ini.find(ServiceToolsGrayLevelKey) )
    set_text( w, SERVICE_TOOLS_EBOX, ini.get_string() );

set_checkbox( w, USE_VALVE_TEST_OUTPUT_XBOX, ini.get_boolean(UseValveTestOutputKey) );
set_checkbox( w, USE_NULL_VALVE_OUTPUT_XBOX, ini.get_boolean(UseNullValveOutputKey) );

/*
------------
Editpart.ini
------------ */
s.get_exe_dir_file_name( EditPartIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

set_checkbox( w, SHOW_SETUP_SHEET_XBOX,       ini.get_boolean(ShowSetupSheetKey)  );
set_checkbox( w, PRINT_PARAMETERS_XBOX,       ini.get_boolean(PrintParametersKey) );
set_checkbox( w, HIDE_SURETRAK_LS_WIRES_XBOX, ini.get_boolean(HideSureTrakLimitSwitchWiresKey) );

if ( ini.find(ChangeExternalShotNameLevelKey) )
    set_text( w, EXTERNAL_SHOT_NAME_LEVEL_EB, ini.get_string() );

if ( ini.find(ChangeShotNameLevelKey) )
    set_text( w, SHOT_NAME_TEMPLATE_LEVEL_EB, ini.get_string() );

if ( ini.find(ChangeShotResetLevelKey) )
    set_text( w, SHOT_NAME_RESET_LEVEL_EB, ini.get_string() );

if ( ini.find(ParameterTabEditLevelKey) )
    set_text( w, CHANGE_PARAM_LEVEL_EBOX, ini.get_string() );

if ( ini.find(SetupSheetLevelKey) )
    set_text( w,  EDIT_SETUP_SHEET_EBOX, ini.get_string() );

if ( ini.find(EditOtherComputerKey) )
    set_text( w,  OTHER_COMPUTER_EBOX, ini.get_string() );

if ( ini.find(EditOtherCurrentPartKey) )
    set_text( w, OTHER_CURRENT_PART_EBOX, ini.get_string() );

if ( ini.find(EditOtherSuretrakKey) )
    set_text( w, OTHER_SURETRAK_EBOX, ini.get_string() );

if ( ini.find(EditPressureSetupLevelKey) )
    set_text( w, PC_SETUP_LEVEL_EBOX, ini.get_string() );

if ( ini.find(EditPressureProfileLevelKey) )
    set_text( w, PC_PROFILE_LEVEL_EBOX, ini.get_string() );

if ( ini.find(SaveAllPartsLevelKey) )
    set_text( w, SAVE_ALL_PARTS_LEVEL_EBOX, ini.get_string() );

if ( ini.find(SaveChangesLevelKey) )
    set_text( w, SAVE_CHANGES_LEVEL_EBOX, ini.get_string() );

if ( ini.find(VelocityChangesLevelKey) )
    set_text( w, VELOCITY_CHANGES_EBOX, ini.get_string() );

 /*
------------
Emachine.ini
------------ */
s.get_exe_dir_file_name( EmachineIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( ini.find(EditMaOnOtherCoKey) )
    set_text( w, OTHER_COMPUTER_MACHINE_EBOX, ini.get_string() );
else
    set_text( w, OTHER_COMPUTER_MACHINE_EBOX, EmptyString );
}

/***********************************************************************
*                                INI_NAME                              *
* Local version of ini_file_name() that uses the CoOptionsExeDir       *
* so I can save to all the network computers.                          *
***********************************************************************/
TCHAR * ini_name( const TCHAR * file_name )
{
static TCHAR buf[MAX_PATH+1];
TCHAR * cp;

cp = copystring( buf, CoOptionsExeDir.text() );
copystring( cp, file_name );

return buf;
}

/***********************************************************************
*                     SAVE_CO_OPTIONS_TO_COMPUTER                      *
*                                                                      *
*        Saves the options to the ini files in CoOptionsExeDir         *
*                                                                      *
***********************************************************************/
static void save_co_options_to_computer()
{
HWND         w;
INI_CLASS    ini;
NAME_CLASS   s;
STRING_CLASS keyname;
int          i;
double       x;

w = CoOptionsWindow;

/*
-----------
Display.ini
----------- */
ini.set_file( ini_name(DisplayIniFile) );
ini.set_section( ConfigSection );

SnapshotIsActive = is_checked( w, ENABLE_SNAPSHOT_XBOX );
ini.put_boolean( SnapshotKey,       SnapshotIsActive );

i  = ALARM_MIN;
if ( is_checked(w, LOW_LIMIT_HIGH_ALARM_RADIO) )
    i = ALARM_MAX;
else if ( is_checked(w, LOW_LIMIT_LOW_WARN_RADIO) )
    i = WARNING_MIN;
else if ( is_checked(w, LOW_LIMIT_HIGH_WARN_RADIO) )
    i = WARNING_MAX;
LowLimitType = i;
ini.put_int( LowLimitTypeKey, i );

i = ALARM_MAX;
if ( is_checked(w, HIGH_LIMIT_LOW_ALARM_RADIO) )
    i  = ALARM_MIN;
else if ( is_checked(w, HIGH_LIMIT_LOW_WARN_RADIO) )
    i = WARNING_MIN;
else if ( is_checked(w, HIGH_LIMIT_HIGH_WARN_RADIO) )
    i = WARNING_MAX;
HighLimitType = i;
ini.put_int( HighLimitTypeKey, i );

/*
------------
Visitrak.ini
------------ */
ini.set_file( ini_name(VisiTrakIniFile) );
ini.set_section( ConfigSection );

PartBasedDisplay = is_checked( w, PART_BASED_DISPLAY_XBOX );
ini.put_boolean( PartBasedDisplayKey,     PartBasedDisplay );
ini.put_boolean( UseDowntimeEntryKey,     is_checked(w, POPUP_DOWNTIME_XBOX) );
ini.put_boolean( HaveSureTrakKey,         is_checked(w, HAVE_SURETRAK_XBOX)  );
ini.put_boolean( NoNetworkKey,            is_checked( w, NO_NETWORK_XBOX) );

s.get_text( w, PASSWORD_LEVEL_TIMEOUT_EBOX );
if ( s.isempty() )
    {
    ini.remove_key( PasswordLevelTimeoutKey );
    /*
    -------------------------------------------------------------
    I don't kill the timer here. I let the timer routine do that.
    ------------------------------------------------------------- */
    PasswordLevelTimeoutMs = 0;
    }
else
    {
    ini.put_string( PasswordLevelTimeoutKey, s.text() );
    x = s.real_value();
    x *= 60000.0;
    PasswordLevelTimeoutMs = (UINT) x;
    if ( PasswordLevelTimeoutMs > 0 )
        {
        if ( PasswordLevelTimeoutId == 0 )
            {
            PasswordLevelTimeoutId = SetTimer( MainWindow, 0, PasswordLevelTimeoutMs, 0 );
            }
        }
    }

ini.set_section( ButtonLevelsSection );

s.get_text( w, PURGE_DOWNTIMES_EBOX );
ini.put_string( DowntimePurgeButtonKey, s.text() );

s.get_text( w, EDIT_OPERATORS_EBOX );
ini.put_string( EditOperatorsButtonKey, s.text() );

/*
----------
Monall.ini
---------- */
ini.set_file( ini_name(MonallIniFile) );
ini.set_section( ConfigSection );

ini.put_boolean( SubtractTimeoutKey,        is_checked(w, SUB_TIMEOUT_XBOX)                );
ini.put_boolean( SaveAllAlarmShotsKey,      is_checked(w, SAVE_ALL_ALARM_XBOX)             );
ini.put_boolean( ClearMonitorWireOnDownKey, is_checked(w, CLEAR_MONITOR_WIRE_ON_DOWN_XBOX) );
ini.put_boolean( ClearDownWireFromPopupKey, is_checked(w, CLEAR_FROM_POPUP_XBOX)           );
ini.put_boolean( ClearAlarmsEachShotKey,    is_checked(w, CLEAR_ALARMS_EACH_SHOT_XBOX)     );
ini.put_boolean( TurnOffAlarmsOnDownKey,    is_checked(w, TURN_OFF_ALARMS_ON_DOWN_XBOX)    );
ini.put_boolean( LogShotparmAccessKey,      is_checked(w, LOG_SHOTPARM_ACCESS_XBOX)        );

s.get_text( w, ALARM_ON_DOWN_WIRE_EBOX );
ini.put_string( DownTimeAlarmWireKey, s.text() );

s.get_text( w, MONITOR_WIRE_EBOX );
ini.put_string( MonitorWireKey, s.text() );

if ( is_checked(w, START_FT2_ON_REALTIME_RADIO) )
    s = START_FT2_ON_REALTIME;
else
    s = START_FT2_ON_STATUS;
ini.put_string( Ft2StartScreenKey, s.text() );

s.get_text( w, F10_CONTROL_MODS_BUTTON_EB );
ini.put_string(Ft2F10LevelKey, s.text() );

s.get_text( w, F2_SET_DAC_BUTTON_EB );
ini.put_string(Ft2F2LevelKey, s.text()  );

s.get_text( w, F3_NULL_VALVE_BUTTON_EB );
ini.put_string( Ft2F3LevelKey, s.text()  );

s.get_text( w, F4_VALVE_TEST_BUTTON_EB );
ini.put_string( Ft2F4LevelKey, s.text()  );

s.get_text( w, F5_TEST_ALARMS_BUTTON_EB );
ini.put_string( Ft2F5LevelKey, s.text()  );

s.get_text( w, F6_STATUS_BUTTON_EB );
ini.put_string( Ft2F6LevelKey, s.text()  );

s.get_text( w, F7_TERMINAL_BUTTON_EB );
ini.put_string( Ft2F7LevelKey, s.text()  );

s.get_text( w, F8_REALTIME_BUTTON_EB );
ini.put_string( Ft2F8LevelKey, s.text()  );

s.get_text( w, F9_GLOBALS_BUTTON_EB );
ini.put_string( Ft2F9LevelKey, s.text()  );

/*
------------
Suretrak.ini
------------ */
ini.set_file( ini_name(SureTrakIniFile) );
ini.set_section( ConfigSection );

s.get_text( w, SERVICE_TOOLS_EBOX );
ini.put_string( ServiceToolsGrayLevelKey, s.text() );

ini.put_boolean( UseValveTestOutputKey, is_checked(w, USE_VALVE_TEST_OUTPUT_XBOX) );
ini.put_boolean( UseNullValveOutputKey, is_checked(w, USE_NULL_VALVE_OUTPUT_XBOX) );

/*
------------
Editpart.ini
------------ */
ini.set_file( ini_name(EditPartIniFile) );
ini.set_section( ConfigSection );

ini.put_boolean( ShowSetupSheetKey,  is_checked(w, SHOW_SETUP_SHEET_XBOX) );
ini.put_boolean( PrintParametersKey, is_checked(w, PRINT_PARAMETERS_XBOX) );
ini.put_boolean( HideSureTrakLimitSwitchWiresKey, is_checked(w, HIDE_SURETRAK_LS_WIRES_XBOX) );

s.get_text( w, EXTERNAL_SHOT_NAME_LEVEL_EB );
ini.put_string( ChangeExternalShotNameLevelKey, s.text() );

s.get_text( w, SHOT_NAME_TEMPLATE_LEVEL_EB );
ini.put_string( ChangeShotNameLevelKey, s.text() );

s.get_text( w, SHOT_NAME_RESET_LEVEL_EB );
ini.put_string( ChangeShotResetLevelKey, s.text() );

s.get_text( w, CHANGE_PARAM_LEVEL_EBOX );
ini.put_string( ParameterTabEditLevelKey, s.text() );

s.get_text( w, EDIT_SETUP_SHEET_EBOX );
ini.put_string( SetupSheetLevelKey, s.text() );

s.get_text( w, OTHER_COMPUTER_EBOX );
ini.put_string( EditOtherComputerKey, s.text() );

s.get_text( w, OTHER_CURRENT_PART_EBOX );
ini.put_string( EditOtherCurrentPartKey, s.text() );

s.get_text( w, OTHER_SURETRAK_EBOX );
ini.put_string( EditOtherSuretrakKey, s.text() );

s.get_text( w, PC_SETUP_LEVEL_EBOX );
ini.put_string( EditPressureSetupLevelKey, s.text() );

s.get_text( w, PC_PROFILE_LEVEL_EBOX );
ini.put_string( EditPressureProfileLevelKey, s.text() );

s.get_text( w, SAVE_ALL_PARTS_LEVEL_EBOX );
ini.put_string( SaveAllPartsLevelKey, s.text() );

s.get_text( w, SAVE_CHANGES_LEVEL_EBOX );
ini.put_string( SaveChangesLevelKey, s.text() );

s.get_text( w, VELOCITY_CHANGES_EBOX );
ini.put_string( VelocityChangesLevelKey, s.text() );

 /*
------------
Emachine.ini
------------ */
s.get_exe_dir_file_name( EmachineIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

s.get_text( w, OTHER_COMPUTER_MACHINE_EBOX );
ini.put_string( EditMaOnOtherCoKey, s.text() );
}

/***********************************************************************
*                           SAVE_CO_OPTIONS                            *
***********************************************************************/
static BOOLEAN save_co_options()
{
static TCHAR data_string[] = TEXT( "data" );
static TCHAR exe_dir_path_end[] = TEXT( "exes\\" );
COMPUTER_CLASS c;

CoOptionsExeDir.get_exe_dir_file_name( 0 );
CoOptionsExeDir.add_ending_backslash();
save_co_options_to_computer();

if ( !is_checked( CoOptionsWindow, COPY_TO_NETWORK_XBOX) )
    return TRUE;

if ( is_checked( CoOptionsWindow, NO_NETWORK_XBOX) )
    return TRUE;

hourglass_cursor();

c.rewind();
while ( c.next() )
    {
    if ( !c.is_this_computer() )
        {
        if ( c.is_present() )
            {
            CoOptionsExeDir.get_exe_dir_file_name( c.name(), 0 );
            CoOptionsExeDir.add_ending_backslash();
            save_co_options_to_computer();
            }
        }
    }

restore_cursor();

return TRUE;
}

/***********************************************************************
*                          SHOW_CONTEXT_HELP                           *
***********************************************************************/
static BOOL show_context_help( HWND w, LPARAM lParam )
{
struct ID_CONTEXT
    {
    UINT id;
    DWORD context;
    };

static ID_CONTEXT control[] = {
    { PART_BASED_DISPLAY_XBOX,         OPTIONS_USE_PART_BASED_HELP             },
    { POPUP_DOWNTIME_XBOX,             OPTIONS_DOWNTIME_ENTRY_HELP             },
    { ENABLE_SNAPSHOT_XBOX,            OPTIONS_SNAPSHOT_HELP                   },
    { HAVE_SURETRAK_XBOX,              OPTIONS_HAVE_SURETRAK_HELP              },
    { USE_VALVE_TEST_OUTPUT_XBOX,      OPTIONS_VALVE_TEST_OUTPUT_HELP          },
    { USE_NULL_VALVE_OUTPUT_XBOX,      OPTIONS_NULL_VALVE_OUTPUT_HELP          },
    { CLEAR_MONITOR_WIRE_ON_DOWN_XBOX, OPTIONS_CLEAR_MONITOR_WIRE_ON_DOWN_HELP },
    { HIDE_SURETRAK_LS_WIRES_XBOX,     OPTIONS_HIDE_SURETRAK_LS_WIRES_HELP     },
    { SAVE_ALL_ALARM_XBOX,             OPTIONS_SAVE_ALL_ALARM_HELP             },
    { SHOW_SETUP_SHEET_XBOX,           OPTIONS_SHOW_SETUP_SHEET_HELP           },
    { SUB_TIMEOUT_XBOX,                OPTIONS_SUB_TIMEOUT_HELP                },
    { PRINT_PARAMETERS_XBOX,           OPTIONS_PRINT_PARAMETERS_HELP           },
    { CLEAR_FROM_POPUP_XBOX,           OPTIONS_CLEAR_FROM_POPUP_HELP           },
    { COPY_TO_NETWORK_XBOX,            OPTIONS_COPY_TO_NETWORK_HELP            },
    { NO_NETWORK_XBOX,                 OPTIONS_NO_NETWORK_HELP                 },
    { CLEAR_ALARMS_EACH_SHOT_XBOX,     OPTIONS_CLEAR_ALARMS_EACH_SHOT_HELP     },
    { TURN_OFF_ALARMS_ON_DOWN_XBOX,    OPTIONS_TURN_OFF_ALARMS_ON_DOWN_HELP    },
    { LOG_SHOTPARM_ACCESS_XBOX,        OPTIONS_LOG_SHOTPARM_ACCESS_HELP        },
    { START_FT2_ON_STATUS_RADIO,       OPTIONS_FT2_START_SCREEN_HELP           },
    { START_FT2_ON_REALTIME_RADIO,     OPTIONS_FT2_START_SCREEN_HELP           }
    };

const int NOF_CONTROLS = sizeof(control)/sizeof(ID_CONTEXT);

static ID_CONTEXT static_control[] = {
    { VELOCITY_CHANGES_TBOX,                    OPTIONS_VELOCITY_CHANGES_HELP               },
    { MONITOR_WIRE_TBOX,                        OPTIONS_MONITOR_WIRE_HELP                   },
    { PURGE_DOWNTIMES_TBOX,                     OPTIONS_PURGE_DOWNTIMES_HELP                },
    { EDIT_OPERATORS_TBOX,                      OPTIONS_EDIT_OPERATORS_HELP                 },
    { SERVICE_TOOLS_TBOX,                       OPTIONS_SERVICE_TOOLS_HELP                  },
    { CHANGE_PARAM_LEVEL_TBOX,                  OPTIONS_CHANGE_PARAM_LEVEL_HELP             },
    { OTHER_COMPUTER_TBOX,                      OPTIONS_OTHER_COMPUTER_HELP                 },
    { OTHER_CURRENT_PART_TBOX,                  OPTIONS_OTHER_CURRENT_PART_HELP             },
    { OTHER_SURETRAK_TBOX,                      OPTIONS_OTHER_SURETRAK_HELP                 },
    { EDIT_SETUP_SHEET_TBOX,                    OPTIONS_EDIT_SETUP_SHEET_HELP               },
    { ALARM_ON_DOWN_WIRE_TBOX,                  OPTIONS_ALARM_ON_DOWN_HELP                  },
    { PASSWORD_LEVEL_TIMEOUT_TBOX,              OPTIONS_PASSWORD_LEVEL_TIMEOUT_HELP         },
    { SAVE_CHANGES_LEVEL_TBOX,                  OPTIONS_SAVE_CHANGES_HELP                   },
    { SAVE_ALL_PARTS_LEVEL_TBOX,                OPTIONS_SAVE_ALL_PARTS_HELP                 },

    { PC_SETUP_LEVEL_TBOX,                      OPTIONS_PC_SETUP_HELP                       },
    { PC_PROFILE_LEVEL_TBOX,                    OPTIONS_PC_PROFILE_HELP                     },
    { SHOT_NAME_TEMPLATE_LEVEL_TB,              OPTIONS_SHOT_NAME_TEMPLATE_HELP             },
    { SHOT_NAME_RESET_LEVEL_TB,                 OPTIONS_SHOT_NAME_RESET_HELP                },
    { F10_CONTROL_MODS_BUTTON_TB,               OPTIONS_FT2_F10_LEVEL_HELP                  },
    { F2_SET_DAC_BUTTON_TB,                     OPTIONS_FT2_F2_LEVEL_HELP                   },
    { F3_NULL_VALVE_BUTTON_TB,                  OPTIONS_FT2_F3_LEVEL_HELP                   },
    { F4_VALVE_TEST_BUTTON_TB,                  OPTIONS_FT2_F4_LEVEL_HELP                   },
    { F5_TEST_ALARMS_BUTTON_TB,                 OPTIONS_FT2_F5_LEVEL_HELP                   },
    { F6_STATUS_BUTTON_TB,                      OPTIONS_FT2_F6_LEVEL_HELP                   },
    { F7_TERMINAL_BUTTON_TB,                    OPTIONS_FT2_F7_LEVEL_HELP                   },
    { F8_REALTIME_BUTTON_TB,                    OPTIONS_FT2_F8_LEVEL_HELP                   },
    { F9_GLOBALS_BUTTON_TB,                     OPTIONS_FT2_F9_LEVEL_HELP                   },
    { START_SCREEN_TB,                          OPTIONS_FT2_START_SCREEN_HELP               }
    };

const int NOF_STATIC_CONTROLS = sizeof(static_control)/sizeof(ID_CONTEXT);

int32 i;
POINT p;
RECT  r;
WINDOW_CLASS wc;

if ( !CoOptionsWindow )
    return FALSE;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    wc = GetDlgItem( CoOptionsWindow, static_control[i].id );
    if ( wc.getrect(r) )
        {
        if ( PtInRect(&r, p) )
            {
            cooptionshelp( HELP_CONTEXTPOPUP, static_control[i].context );
            return TRUE;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(CoOptionsWindow, control[i].id) == w )
        {
        cooptionshelp( HELP_CONTEXTPOPUP, control[i].context );
        return TRUE;
        }
    }

return TRUE;
}

/***********************************************************************
*                    SET_CO_OPTIONS_SETUP_STRINGS                      *
***********************************************************************/
static void set_co_options_setup_strings( HWND hWnd )
{
STRING_CLASS s;
WINDOW_CLASS w;
int          i;

static RESOURCE_TEXT_ENTRY rsn[] = {
    { PART_BASED_DISPLAY_XBOX,         TEXT("PART_BASED_DISPLAY") },
    { POPUP_DOWNTIME_XBOX,             TEXT("USE_POPUP") },
    { ENABLE_SNAPSHOT_XBOX,            TEXT("ENABLE_SNAPSHOT") },
    { ALARM_ON_DOWN_WIRE_TBOX,         TEXT("ALARM_ON_DOWN") },
    { SAVE_ALL_ALARM_XBOX,             TEXT("SAVE_ALARMS") },
    { SUB_TIMEOUT_XBOX,                TEXT("SUB_TIMEOUT") },
    { MONITOR_WIRE_TBOX,               TEXT("MON_WIRE") },
    { CLEAR_MONITOR_WIRE_ON_DOWN_XBOX, TEXT("CLEAR_MON_WIRE") },
    { PW_LEVELS_STATIC,                TEXT("PW_LEVELS") },
    { BOARD_MONITOR_STATIC,            TEXT("FT2_MONITOR") },
    { HAVE_SURETRAK_XBOX,              TEXT("HAVE_SURETRAK") },
    { USE_VALVE_TEST_OUTPUT_XBOX,      TEXT("USE_VALVE_TEST_OUTPUT") },
    { USE_NULL_VALVE_OUTPUT_XBOX,      TEXT("USE_NULL_VALVE_OUTPUT") },
    { PURGE_DOWNTIMES_TBOX,            TEXT("PURGE_DOWNTIMES") },
    { EDIT_OPERATORS_TBOX,             TEXT("EDIT_OPERATORS_BUTTON") },
    { PART_SETUP_STATIC,               TEXT("PART_SETUP") },
    { CHANGE_PARAM_LEVEL_TBOX,         TEXT("CHANGE_PARAM_SETUPS") },
    { OTHER_COMPUTER_TBOX,             TEXT("EDIT_PART") },
    { OTHER_CURRENT_PART_TBOX,         TEXT("EDIT_CURRENT_PART") },
    { OTHER_SURETRAK_TBOX,             TEXT("EDIT_ST_SETUP") },
    { SHOW_SETUP_SHEET_XBOX,           TEXT("SHOW_SETUP") },
    { EDIT_SETUP_SHEET_TBOX,           TEXT("EDIT_SETUP") },
    { VELOCITY_CHANGES_TBOX,           TEXT("VEL_CHANGE_BUTTON") },
    { RESTART_STATIC,                  TEXT("RESTART") },
    { PLOT_PROG_STATIC,                TEXT("PLOT_PROG") },
    { PRINT_PARAMETERS_XBOX,           TEXT("PRINT_PARAMS") },
    { CLEAR_FROM_POPUP_XBOX,           TEXT("CLEAR_FROM_POPUP") },
    { NO_NETWORK_XBOX,                 TEXT("NO_NETWORK") },
    { LOW_STATIC,                      TEXT("LOW_STRING") },
    { LOW_LIMIT_LOW_ALARM_RADIO,       TEXT("LOW_ALARM") },
    { HIGH_STATIC,                     TEXT("HIGH_STRING") },
    { LOW_LIMIT_LOW_ALARM_RADIO,       TEXT("LOW_ALARM_STRING") },
    { LOW_LIMIT_LOW_WARN_RADIO,        TEXT("LOW_WARNING_STRING") },
    { LOW_LIMIT_HIGH_WARN_RADIO,       TEXT("HIGH_WARNING_STRING") },
    { LOW_LIMIT_HIGH_ALARM_RADIO,      TEXT("HIGH_ALARM_STRING") },
    { HIGH_LIMIT_LOW_ALARM_RADIO,      TEXT("LOW_ALARM_STRING") },
    { HIGH_LIMIT_LOW_WARN_RADIO,       TEXT("LOW_WARNING_STRING") },
    { HIGH_LIMIT_HIGH_WARN_RADIO,      TEXT("HIGH_WARNING_STRING") },
    { HIGH_LIMIT_HIGH_ALARM_RADIO,     TEXT("HIGH_ALARM_STRING") },
    { IDOK,                            TEXT("SAVE_CHANGES") },
    { IDCANCEL,                        TEXT("CANCEL") },
    { COPY_TO_NETWORK_XBOX,            TEXT("COPY_TO_NETWORK") },
    { SAVE_CHANGES_LEVEL_TBOX,         TEXT("SAVE_CHANGES") },
    { SAVE_ALL_PARTS_LEVEL_TBOX,       TEXT("SAVE_ALL_PARTS") },
    { SHOT_NAME_TEMPLATE_LEVEL_TB,     TEXT("SHOT_NAME_TEMPLATE") },
    { SHOT_NAME_RESET_LEVEL_TB,        TEXT("SHOT_COUNT_RESET") },
    { CLEAR_ALARMS_EACH_SHOT_XBOX,     TEXT("CLEAR_ALARMS_AT_CS") },
    { PASSWORD_LEVEL_TIMEOUT_TBOX,     TEXT("RESTORE_DEF_PW") },
    { PC_PROFILE_LEVEL_TBOX,           TEXT("EDIT_PC_PROFILE") },
    { PC_SETUP_LEVEL_TBOX,             TEXT("EDIT_PC_SETUP") },
    { HIDE_SURETRAK_LS_WIRES_XBOX,     TEXT("HIDE_ST_LS_WIRES") },
    { TURN_OFF_ALARMS_ON_DOWN_XBOX,    TEXT("ALARMS_OFF_ON_DOWN") },
    { LOG_SHOTPARM_ACCESS_XBOX,        TEXT("LOG_SHOTPARM_ACCESS") },
    { CHANGES_TO_OTHER_STATIC,         TEXT("CHANGE_TO_OTHER_CO") },
    { CHANGES_2_TO_OTHER_STATIC,       TEXT("CHANGE_TO_OTHER_CO") },
    { EXTERNAL_SHOT_NAME_LEVEL_TB,     TEXT("EXTERNAL_SHOT_NAME") },
    { MA_SETUP_STATIC,                 TEXT("MA_SETUP") },
    { OTHER_COMPUTER_TBOX2,            TEXT("CHANGE_MA_SETUP") },
    { CORNER_PARAM_LIMITS_STATIC,      TEXT("CORNER_DISPLAY_LIMITS") },
    { F2_SET_DAC_BUTTON_TB,            TEXT("F2_RADIO") },
    { F3_NULL_VALVE_BUTTON_TB,         TEXT("F3_RADIO") },
    { F4_VALVE_TEST_BUTTON_TB,         TEXT("F4_RADIO") },
    { F5_TEST_ALARMS_BUTTON_TB,        TEXT("F5_RADIO") },
    { F6_STATUS_BUTTON_TB,             TEXT("F6_RADIO") },
    { F7_TERMINAL_BUTTON_TB,           TEXT("F7_RADIO") },
    { F8_REALTIME_BUTTON_TB,           TEXT("F8_RADIO") },
    { F9_GLOBALS_BUTTON_TB,            TEXT("F9_RADIO") },
    { F10_CONTROL_MODS_BUTTON_TB,      TEXT("F10_RADIO")},
    { START_FT2_ON_STATUS_RADIO,       TEXT("F6_RADIO") },
    { START_FT2_ON_REALTIME_RADIO,     TEXT("F8_RADIO") },
    { START_SCREEN_TB,                 TEXT("START_SCREEN") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    ALARM_ON_DOWN_WIRE_EBOX,
    MONITOR_WIRE_EBOX,
    PURGE_DOWNTIMES_EBOX,
    EDIT_OPERATORS_EBOX,
    CHANGE_PARAM_LEVEL_EBOX,
    OTHER_COMPUTER_EBOX,
    OTHER_CURRENT_PART_EBOX,
    OTHER_SURETRAK_EBOX,
    EDIT_SETUP_SHEET_EBOX,
    VELOCITY_CHANGES_EBOX,
    SAVE_CHANGES_LEVEL_EBOX,
    SAVE_ALL_PARTS_LEVEL_EBOX,
    SHOT_NAME_TEMPLATE_LEVEL_EB,
    SHOT_NAME_RESET_LEVEL_EB,
    PASSWORD_LEVEL_TIMEOUT_EBOX,
    PC_PROFILE_LEVEL_EBOX,
    PC_SETUP_LEVEL_EBOX,
    EXTERNAL_SHOT_NAME_LEVEL_EB,
    F2_SET_DAC_BUTTON_EB,
    F3_NULL_VALVE_BUTTON_EB,
    F4_VALVE_TEST_BUTTON_EB,
    F5_TEST_ALARMS_BUTTON_EB,
    F6_STATUS_BUTTON_EB,
    F7_TERMINAL_BUTTON_EB,
    F8_REALTIME_BUTTON_EB,
    F9_GLOBALS_BUTTON_EB,
    F10_CONTROL_MODS_BUTTON_EB,
    OTHER_COMPUTER_MACHINE_EBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

for ( i=0; i<nof_rsn; i++ )
    {
    w = GetDlgItem( hWnd, rsn[i].id );
    set_statics_font( w );
    s = StringTable.find( rsn[i].s );
    s.set_text( w.handle() );
    }

for ( i=0; i<nof_boxes; i++ )
    {
    w = GetDlgItem( hWnd, box[i] );
    set_statics_font( w );
    }

w = hWnd;
w.set_title( StringTable.find(TEXT("EDIT_OPTIONS")) );
w.refresh();
}

/***********************************************************************
*                        CO_OPTIONS_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK co_options_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CoOptionsWindow = hWnd;
        set_co_options_setup_strings( hWnd );
        fill_co_options();
        break;

    case WM_CONTEXTMENU:
        return show_context_help( (HWND) wParam, lParam );

    case WM_HELP:
        gethelp( hWnd, HELP_CONTEXT, OPTIONS_EDIT_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save_co_options();

            case IDCANCEL:
                exit_co_options();
                return TRUE;

            }
        break;

    case WM_CLOSE:
        exit_co_options();
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        ENABLE_EDIT_PASSWORDS                         *
***********************************************************************/
static void enable_edit_passwords( int32 password_level )
{
WINDOW_CLASS w;
BOOLEAN is_enabled;
int32   required_level;

required_level = Password.highest_level();

if ( password_level >= required_level )
    is_enabled = TRUE;
else
    is_enabled = FALSE;

w = GetDlgItem( EnterWindow, EDIT_PASSWORDS_BUTTON );
w.enable( is_enabled );

w = GetDlgItem( EnterWindow, CO_OPTIONS_BUTTON );
if ( is_enabled )
    w.show();
else
    w.hide();
}

/***********************************************************************
*                          SET_PASSWORD_LEVEL                          *
* Return TRUE if the level has changed from less than the highest to   *
* the highest level.                                                   *
***********************************************************************/
static BOOLEAN set_password_level()
{
BOOLEAN b;
TCHAR s[PASSWORD_LEN+1];
int32 i;
int32 new_password_level;
int32 highest_password_level;

b = FALSE;
highest_password_level = Password.highest_level();

if ( get_text( s, EnterWindow, ENTER_PASSWORD_EBOX, PASSWORD_LEN)  )
    {
    if ( *s == NullChar )
        return b;
    i = Password.find(s);
    if ( i == NO_INDEX )
        {
        resource_message_box( MainInstance, PASSWORD_NOT_FOUND_STRING, CANNOT_COMPLY_STRING );
        }
    else
        {
        new_password_level = Password[i].level;
        if ( CurrentPasswordLevel < highest_password_level && new_password_level >= highest_password_level )
            {
            b = TRUE;
            /*
            -----------------------------------------------------------------
            Update the current level box as I am not going to kill myself now
            ----------------------------------------------------------------- */
            set_text( EnterWindow, CURRENT_PASSWORD_LEVEL_TBOX, int32toasc(new_password_level) );
            set_text( EnterWindow, ENTER_PASSWORD_EBOX, EmptyString );
            }
        poke_data( DDE_CONFIG_TOPIC, ItemList[PW_LEVEL_INDEX].name, int32toasc(Password[i].level) );
        enable_edit_passwords( Password[i].level );
        log_password( Password[i].owner );
        }
    }

return b;
}

/***********************************************************************
*                    SET_ENTER_PASSWORD_SETUP_STRINGS                  *
***********************************************************************/
static void set_enter_password_setup_strings( HWND hWnd )
{
static RESOURCE_TEXT_ENTRY rsn[] = {
    { IDOK,                    TEXT("OK_STRING") },
    { IDCANCEL,                TEXT("CANCEL") },
    { EDIT_PASSWORDS_BUTTON,   TEXT("EDIT_PW") },
    { CO_OPTIONS_BUTTON,       TEXT("EDIT_OPTIONS") },
    { ENTER_PW_STATIC,         TEXT("ENTER_PW") },
    { CURRENT_PW_LEVEL_STATIC, TEXT("CURRENT_PW_LEVEL") },
    { PW_SAVE_CHANGES_STATIC,  TEXT("SAVE_CHANGES") }
    };

const int nof_rsn = sizeof(rsn)/sizeof(RESOURCE_TEXT_ENTRY);

static UINT box[] = {
    ENTER_PASSWORD_EBOX,
    CURRENT_PASSWORD_LEVEL_TBOX,
    SAVE_CHANGES_LEVEL_EBOX
    };

const int nof_boxes = sizeof(box)/sizeof(UINT);

set_setup_strings( hWnd, rsn, nof_rsn, box, nof_boxes, TEXT("SET_PW_LEVEL") );
}

/***********************************************************************
*                          ENTER_PASSWORD_PROC                         *
*                      Window proc for PASSWORD_DIALOG                 *
***********************************************************************/
BOOL CALLBACK enter_password_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
HWND w;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        EnterWindow = hWnd;
        set_enter_password_setup_strings( hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        Password.load();
        set_text( hWnd, CURRENT_PASSWORD_LEVEL_TBOX, int32toasc(CurrentPasswordLevel) );
        w = GetDlgItem( EnterWindow, ENTER_PASSWORD_EBOX );
        set_text( w, EmptyString );
        SetFocus( w );
        enable_edit_passwords( CurrentPasswordLevel );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( hWnd, HELP_CONTEXT, PASSWORD_ENTRY_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case EDIT_PASSWORDS_BUTTON:
                EnterWindow = 0;
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                PostMessage( MainWindow, WM_NEW_DATA, EDIT_PASSWORDS_DIALOG_REQ, 0L );
                return TRUE;

            case CO_OPTIONS_BUTTON:
                EnterWindow = 0;
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                PostMessage( MainWindow, WM_NEW_DATA, CO_OPTIONS_DIALOG_REQ, 0L );
                return TRUE;

            case IDOK:
                if ( set_password_level() )
                    return TRUE;

            case IDCANCEL:
                EnterWindow = 0;
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EnterWindow = 0;
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}
