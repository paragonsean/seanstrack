#ifndef _FTII_PARAMETERS_
#define _FTII_PARAMETERS_

#ifndef _FTII_CURVE_TYPES_
#include "..\include\ftii_curve_types.h"
#endif

#ifndef _FTII_MIN_MAX_DATA_
#include "..\include\ftii_min_max.h"
#endif

#ifndef _FTII_SHOT_DATA_
#include "..\include\ftii_shot_data.h"
#endif

#ifndef _MACHINE_CLASS_
#include "..\include\machine.h"
#endif

#ifndef _PART_CLASS_
#include "..\include\part.h"
#endif

#ifndef _VISI_PARAM_H_
#include "..\include\param.h"
#endif

#ifndef _ANALOG_SENSOR_
#include "..\include\asensor.h"
#endif

#ifndef _FTANALOG_CLASS_
#include "..\include\ftanalog.h"
#endif

#ifndef _FTCALC_CLASS_
#include "..\include\ftcalc.h"
#endif

#ifndef _LIMIT_SWITCH_CLASS_
#include "..\include\limit_switch_class.h"
#endif

double * calculate_ftii_parameters( FTII_PROFILE & f, MACHINE_CLASS & machine, PART_CLASS & part, PARAMETER_CLASS & param, FTANALOG_CLASS & ftanalog, FTCALC_CLASS & ftcalc, LIMIT_SWITCH_CLASS & limits );

#endif