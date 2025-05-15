#include <windows.h>
#include <limits.h>
#include <cmath>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\adam_thermo_class.h"
#include "..\include\fileclas.h"
#include "..\include\ftii.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"
#include "..\include\subs.h"

static const TCHAR NullChar = TEXT( '\0' );

/***********************************************************************
*                          ADAM_THERMO_CLASS                       *
***********************************************************************/
ADAM_THERMO_CLASS::ADAM_THERMO_CLASS()
{
value = ED_DATA_NOT_SET;
}

/***********************************************************************
*                         ~ADAM_THERMO_CLASS                       *
***********************************************************************/
ADAM_THERMO_CLASS::~ADAM_THERMO_CLASS()
{
}

/***********************************************************************
*                          ADAM_THERMO_CLASS                       *
*                                 clear                                *
***********************************************************************/
void ADAM_THERMO_CLASS::clear()
{
value = ED_DATA_NOT_SET;
}

/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                is_set                                *
***********************************************************************/
bool ADAM_THERMO_CLASS::is_set()
{
if ( value == ED_DATA_NOT_SET )
    return false;
return true;    
}

/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                               is_not_set                             *
***********************************************************************/
bool ADAM_THERMO_CLASS::is_not_set()
{
if ( value == ED_DATA_NOT_SET )
    return true;
return false;    
}

/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                    =                                 *
***********************************************************************/
void ADAM_THERMO_CLASS::operator=( unsigned new_value )
{
value = new_value;
}

/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                    =                                 *
***********************************************************************/
void ADAM_THERMO_CLASS::operator=( ADAM_THERMO_CLASS & sorc )
{
value = sorc.value;
}

/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                 deg_c                                *
***********************************************************************/
double ADAM_THERMO_CLASS::deg_c()
{
double d;
if ( is_not_set() )
    return 0.0;
d = double( value );
d /= 1000.0;
return d;
}

/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                 deg_f                                *
***********************************************************************/
double ADAM_THERMO_CLASS::deg_f()
{
double d;
if ( is_not_set() )
    return 0.0;
d = double( value );
d *= 9;
d /= 5000.0;
d += 32;
return d;
}
/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                  deg                                 *
***********************************************************************/
double ADAM_THERMO_CLASS::deg( short units )
{
if ( units == DEG_F_UNITS )
    return deg_f();

return deg_c();
}

/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                    =                                 *
***********************************************************************/
void ADAM_THERMO_CLASS::operator=( TCHAR * sorc )
{
if ( sorc == 0 )
    value = ED_DATA_NOT_SET;
else if ( *sorc == NullChar )
    value = ED_DATA_NOT_SET;
else
    value = (unsigned) asctoul( sorc );
}
/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                    =                                 *
***********************************************************************/
void ADAM_THERMO_CLASS::operator=( STRING_CLASS & sorc )
{
if ( sorc.isempty() )
    value = ED_DATA_NOT_SET;
else
    value = sorc.uint_value();
}

/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                   ==                                 *
***********************************************************************/
bool ADAM_THERMO_CLASS::operator==( ADAM_THERMO_CLASS & sorc )
{
return (value==sorc.value);
}

/***********************************************************************
*                            ADAM_THERMO_CLASS                         *
*                                   !=                                 *
***********************************************************************/
bool ADAM_THERMO_CLASS::operator!=( ADAM_THERMO_CLASS & sorc )
{
return (value!=sorc.value);
}
