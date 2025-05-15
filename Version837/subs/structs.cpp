#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\dbclass.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

const static TCHAR NullChar     = TEXT( '\0' );
const static TCHAR SpaceChar    = TEXT( ' ' );

static BITSETYPE FtAnalogCurveType[NOF_DYNAMIC_CHANNELS] =
    {
    CHAN_1_ANALOG_CURVE, CHAN_2_ANALOG_CURVE, CHAN_3_ANALOG_CURVE, CHAN_4_ANALOG_CURVE,
    CHAN_5_ANALOG_CURVE, CHAN_6_ANALOG_CURVE, CHAN_7_ANALOG_CURVE, CHAN_8_ANALOG_CURVE
    };

/***********************************************************************
*                            CURVE_DATA                                *
*                         FT_CHANNEL_NUMBER                            *
*                              [1,8]                                   *
***********************************************************************/
short CURVE_DATA::ft_channel_number()
{

int i;

for ( i=0; i<NOF_DYNAMIC_CHANNELS; i++ )
    {
    if ( type & FtAnalogCurveType[i] )
        return i+1;
    }

return NO_FT_CHANNEL;
}

/***********************************************************************
*                            CURVE_DATA                                *
*                          FT_CHANNEL_TYPE                             *
***********************************************************************/
BITSETYPE CURVE_DATA::ft_channel_type()
{
int i;

for ( i=0; i<NOF_DYNAMIC_CHANNELS; i++ )
    {
    if ( type & FtAnalogCurveType[i] )
        return FtAnalogCurveType[i];
    }

return 0;
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                            PART_NAME_ENTRY                           *
***********************************************************************/
PART_NAME_ENTRY::PART_NAME_ENTRY()
{
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                  ==                                  *
***********************************************************************/
BOOLEAN PART_NAME_ENTRY::operator==(const PART_NAME_ENTRY & sorc )
{
if ( computer != sorc.computer )
    return FALSE;

if ( machine != sorc.machine )
    return FALSE;

if ( part != sorc.part )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                   =                                  *
***********************************************************************/
void PART_NAME_ENTRY::operator=(const PART_NAME_ENTRY & sorc )
{
computer = sorc.computer;
machine  = sorc.machine;
part     = sorc.part;
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                 set                                  *
***********************************************************************/
void PART_NAME_ENTRY::set( STRING_CLASS & cn, STRING_CLASS & mn )
{
computer = cn;
machine  = mn;
part.null();
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                               IS_EQUAL                               *
***********************************************************************/
BOOLEAN PART_NAME_ENTRY::is_equal( const STRING_CLASS & cn, const STRING_CLASS & mn, const STRING_CLASS & pn )
{
if ( computer != cn )
    return FALSE;

if ( machine != mn )
    return FALSE;

if ( part != pn )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            PART_NAME_ENTRY                           *
*                                 EXISTS                               *
***********************************************************************/
BOOLEAN PART_NAME_ENTRY::exists()
{
return part_exists( computer, machine, part );
}

/***********************************************************************
*                          MACHINE_NAME_ENTRY                          *
*                                   =                                  *
***********************************************************************/
void MACHINE_NAME_ENTRY::operator=(const MACHINE_NAME_ENTRY & sorc )
{
computer = sorc.computer;
machine  = sorc.machine;
}

/***********************************************************************
*                         MACHINE_NAME_ENTRY                           *
*                               IS_EQUAL                               *
***********************************************************************/
BOOLEAN MACHINE_NAME_ENTRY::is_equal( const STRING_CLASS & sorc_computer, const STRING_CLASS & sorc_machine )
{
if ( computer != sorc_computer )
    return FALSE;

if ( machine != sorc_machine )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                          MACHINE_NAME_ENTRY                          *
*                          MACHINE_NAME_ENTRY                          *
***********************************************************************/
MACHINE_NAME_ENTRY::MACHINE_NAME_ENTRY()
{
}

/***********************************************************************
*                         MACHINE_NAME_ENTRY                           *
*                               EMPTY                                  *
***********************************************************************/
void MACHINE_NAME_ENTRY::empty()
{
computer.null();
machine.null();
}

/***********************************************************************
*                         MACHINE_NAME_ENTRY                           *
*                              IS_EMPTY                                *
***********************************************************************/
BOOLEAN MACHINE_NAME_ENTRY::is_empty()
{
if ( computer.isempty() || machine.isempty() )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                         PROFILE_NAME_ENTRY                           *
*                                SET                                   *
***********************************************************************/
void PROFILE_NAME_ENTRY::set( const STRING_CLASS & cn, const STRING_CLASS & mn, const STRING_CLASS & pn, const STRING_CLASS & sn )
{
computer = cn;
machine  = mn;
part     = pn;
shot     = sn;
}

/***********************************************************************
*                         PROFILE_NAME_ENTRY                           *
*                                SET                                   *
***********************************************************************/
void PROFILE_NAME_ENTRY::set( const STRING_CLASS & cn, const STRING_CLASS & mn, const STRING_CLASS & pn, const TCHAR * sn )
{
computer = cn;
machine  = mn;
part     = pn;
shot     = sn;
}

/***********************************************************************
*                           PROFILE_NAME_ENTRY                         *
*                                   =                                  *
***********************************************************************/
void PROFILE_NAME_ENTRY::operator=(const PROFILE_NAME_ENTRY & sorc )
{
set( sorc.computer, sorc.machine, sorc.part, sorc.shot );
}

/***********************************************************************
*                           PROFILE_NAME_ENTRY                         *
*                                   =                                  *
***********************************************************************/
void PROFILE_NAME_ENTRY::operator=(const PART_NAME_ENTRY & sorc )
{
set( sorc.computer, sorc.machine, sorc.part, 0 );
}
