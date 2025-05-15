#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\ftii.h"
#include "..\include\limit_switch_class.h"
#include "..\include\subs.h"

/***********************************************************************
*                         LIMIT_SWITCH_CLASS                           *
***********************************************************************/
LIMIT_SWITCH_CLASS::LIMIT_SWITCH_CLASS()
{
}

/***********************************************************************
*                           LIMIT_SWITCH_CLASS                         *
***********************************************************************/
LIMIT_SWITCH_CLASS::~LIMIT_SWITCH_CLASS()
{
}

/***********************************************************************
*                           LIMIT_SWITCH_CLASS                         *
*                                  GET                                 *
* A return value of FALSE just means there were no old settings        *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_CLASS::get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
BOOLEAN status;

status = TRUE;
empty();

if ( !description.get(computer, machine, part) )
    status = FALSE;
if ( !position.get(computer, machine, part) )
    status = FALSE;
if ( !mywire.get(computer, machine, part) )
    status = FALSE;

return status;
}

/***********************************************************************
*                           LIMIT_SWITCH_CLASS                         *
*                                  PUT                                 *
* A return value of FALSE means I couldn't write the file.             *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_CLASS::put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
BOOLEAN status;

status = TRUE;
if ( !description.put(computer, machine, part) )
    status = FALSE;

if ( !position.put(computer, machine, part) )
    status = FALSE;

if ( !mywire.put(computer, machine, part) )
    status = FALSE;

return status;
}

/***********************************************************************
*                           LIMIT_SWITCH_CLASS                         *
*                                 EMPTY                                *
***********************************************************************/
void LIMIT_SWITCH_CLASS::empty()
{
description.empty();
position.empty();
mywire.empty();
}

/***********************************************************************
*                         LIMIT_SWITCH_DESC_CLASS                       *
***********************************************************************/
LIMIT_SWITCH_DESC_CLASS::LIMIT_SWITCH_DESC_CLASS()
{
}

/***********************************************************************
*                         LIMIT_SWITCH_DESC_CLASS                       *
***********************************************************************/
LIMIT_SWITCH_DESC_CLASS::~LIMIT_SWITCH_DESC_CLASS()
{
}

/***********************************************************************
*                         LIMIT_SWITCH_DESC_CLASS                       *
*                                  GET                                 *
* A return value of FALSE just means there were no old settings        *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_DESC_CLASS::get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
int            i;
NAME_CLASS     s;
FILE_CLASS     f;

/*
----------------------------------------------
The limit switch wires have their own file now
---------------------------------------------- */
s.get_limit_switch_desc_file_name( computer, machine, part );
if ( s.file_exists() )
    {
    f.open_for_read( s.text() );
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        desc[i] = f.readline();
    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         LIMIT_SWITCH_DESC_CLASS                       *
*                                  PUT                                 *
* A return value of FALSE means I couldn't write the file.             *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_DESC_CLASS::put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
int            i;
FILE_CLASS     f;
NAME_CLASS   s;

s.get_limit_switch_desc_file_name( computer, machine, part );
if ( f.open_for_write(s) )
    {
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        {
        f.writeline( desc[i] );
        }
    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         LIMIT_SWITCH_DESC_CLASS                       *
*                                   []                                 *
***********************************************************************/
STRING_CLASS & LIMIT_SWITCH_DESC_CLASS::operator[]( int index )
{
static STRING_CLASS myemptystring;

if ( index>=0 && index<MAX_FTII_LIMIT_SWITCHES )
    return desc[index];

return myemptystring;
}

/***********************************************************************
*                         LIMIT_SWITCH_DESC_CLASS                       *
*                                EMPTY                                 *
***********************************************************************/
void LIMIT_SWITCH_DESC_CLASS::empty()
{
int i;

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    desc[i].empty();
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_CLASS                       *
***********************************************************************/
LIMIT_SWITCH_POS_CLASS::LIMIT_SWITCH_POS_CLASS()
{
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_CLASS                       *
***********************************************************************/
LIMIT_SWITCH_POS_CLASS::~LIMIT_SWITCH_POS_CLASS()
{
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_CLASS                       *
*                                  GET                                 *
* A return value of FALSE just means there was no file.                *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_POS_CLASS::get( STRING_CLASS & path )
{
int            i;
FILE_CLASS     f;

if ( f.open_for_read(path) )
    {
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        pos[i] = f.readline();
    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_CLASS                       *
*                                  GET                                 *
* A return value of FALSE just means there were no old settings        *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_POS_CLASS::get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
NAME_CLASS     s;

/*
--------------------------------------------------
The limit switch positions have their own file now
-------------------------------------------------- */
s.get_limit_switch_pos_file_name( computer, machine, part );
if ( s.file_exists() )
    return get(s);

return FALSE;
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_CLASS                       *
*                                  PUT                                 *
* A return value of FALSE means I couldn't write the file.             *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_POS_CLASS::put( STRING_CLASS & path )
{
int            i;
FILE_CLASS     f;

if ( f.open_for_write(path) )
    {
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        {
        f.writeline( pos[i] );
        }
    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_CLASS                       *
*                                  PUT                                 *
* A return value of FALSE means I couldn't write the file.             *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_POS_CLASS::put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
NAME_CLASS   s;

s.get_limit_switch_pos_file_name( computer, machine, part );
return put( s );
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_CLASS                       *
*                                   []                                 *
***********************************************************************/
STRING_CLASS & LIMIT_SWITCH_POS_CLASS::operator[]( int index )
{
static STRING_CLASS myemptystring;

if ( index>=0 && index<MAX_FTII_LIMIT_SWITCHES )
    return pos[index];

return myemptystring;
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_CLASS                       *
*                                EMPTY                                 *
***********************************************************************/
void LIMIT_SWITCH_POS_CLASS::empty()
{
int i;

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    pos[i].empty();
}

/***********************************************************************
*                         LIMIT_SWITCH_POS_CLASS                       *
*                                REAL_POS                              *
***********************************************************************/
double LIMIT_SWITCH_POS_CLASS::real_pos( int index )
{
if ( index>=0 && index<MAX_FTII_LIMIT_SWITCHES )
    return pos[index].real_value();

return 0.0;
}

/***********************************************************************
*                         LIMIT_SWITCH_WIRE_CLASS                      *
***********************************************************************/
LIMIT_SWITCH_WIRE_CLASS::LIMIT_SWITCH_WIRE_CLASS()
{
}

/***********************************************************************
*                         LIMIT_SWITCH_WIRE_CLASS                      *
***********************************************************************/
LIMIT_SWITCH_WIRE_CLASS::~LIMIT_SWITCH_WIRE_CLASS()
{
}

/***********************************************************************
*                         LIMIT_SWITCH_WIRE_CLASS                      *
*                                MASK                                  *
* Convert a wire number to a bit mask, assuming wire 1 is bit 0, etc   *
***********************************************************************/
unsigned LIMIT_SWITCH_WIRE_CLASS::mask( int index )
{
unsigned mywire;
unsigned mask;

if ( index<0 || index>=MAX_FTII_LIMIT_SWITCHES )
    return 0;

mask = 0;
mywire = wire[index].uint_value();

if ( mywire > 0 )
    {
    mask = 1;
    mywire--;
    mask <<= mywire;
    }

return mask;
}

/***********************************************************************
*                         LIMIT_SWITCH_WIRE_CLASS                      *
*                            ASCII_MASK                                *
***********************************************************************/
TCHAR * LIMIT_SWITCH_WIRE_CLASS::ascii_mask( int index )
{
static TCHAR buf[MAX_INTEGER_LEN+1];

ultoascii( buf, mask(index), HEX_RADIX );
rjust( buf, 9, TEXT('0') );
buf[0] = TEXT('H');

return buf;
}

/***********************************************************************
*                         LIMIT_SWITCH_WIRE_CLASS                      *
*                            WIRE_NUMBER                               *
***********************************************************************/
int LIMIT_SWITCH_WIRE_CLASS::wire_number( int index )
{
return wire[index].int_value();
}

/***********************************************************************
*                        LIMIT_SWITCH_WIRE_CLASS                       *
*                                  GET                                 *
* A return value of FALSE just means there were no old settings        *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_WIRE_CLASS::get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
static int maskvn[MAX_FTII_LIMIT_SWITCHES] = { LS1_MASK_VN, LS2_MASK_VN, LS3_MASK_VN, LS4_MASK_VN, LS5_MASK_VN, LS6_MASK_VN };
int            i;
FTII_VAR_ARRAY va;
NAME_CLASS     s;
FILE_CLASS     f;

/*
----------------------------------------------
The limit switch wires have their own file now
---------------------------------------------- */
s.get_part_dir_file_name( computer, machine, part, LIMIT_SWITCH_WIRES_FILE );
if ( s.file_exists() )
    {
    f.open_for_read( s.text() );
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        wire[i] = f.readline();
    f.close();
    return TRUE;
    }

/*
------------------------------------------------------------------
If that file doesn't exist then look in the ftii_part_settings.txt
------------------------------------------------------------------ */
s.get_part_dir_file_name( computer, machine, part, FTII_PART_SETTINGS_FILE );
if ( s.file_exists() )
    {
    va.upsize( MAX_FTII_LIMIT_SWITCHES );
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        va[i].set( maskvn[i], (unsigned) 0 );

    va.get_from( s );

    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        wire[i] = (unsigned int) wire_from_mask( va[i].uval() );
    return TRUE;
    }

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    wire[i].null();

return FALSE;
}

/***********************************************************************
*                         LIMIT_SWITCH_WIRE_CLASS                      *
*                                   PUT                                *
* A return value of FALSE means I couldn't write the file.             *
***********************************************************************/
BOOLEAN LIMIT_SWITCH_WIRE_CLASS::put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
int          i;
FILE_CLASS   f;
NAME_CLASS   s;

s.get_part_dir_file_name( computer, machine, part, LIMIT_SWITCH_WIRES_FILE );
if ( f.open_for_write(s) )
    {
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        f.writeline( wire[i] );
    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         LIMIT_SWITCH_WIRE_CLASS                      *
*                                    []                                *
***********************************************************************/
STRING_CLASS & LIMIT_SWITCH_WIRE_CLASS::operator[]( int index )
{
static STRING_CLASS myemptystring;

if ( index>=0 && index<MAX_FTII_LIMIT_SWITCHES )
    return wire[index];

return myemptystring;
}

/***********************************************************************
*                         LIMIT_SWITCH_WIRE_CLASS                      *
*                                 EMPTY                                *
***********************************************************************/
void LIMIT_SWITCH_WIRE_CLASS::empty()
{
int i;

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    wire[i] = 0;
}
