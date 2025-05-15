//////////////////////// vtw_global16.h ///////////////////////////
//
// Copyright 2002,2003, 2004 Visi-Trak Worldwide

/*
4-24-06 vtw_global8.h	Added position loop parameters.

10-25-06 renamed to vtw_global10.h

12-5-06 Moved CS_DETECTED to bit 9 in V312.  Added bit CS_FROM_SS.  This is a CS from the start shot signal from the control.

1-17-07 added definitions for TOSTRING(x), PRINT_CHAR(c), PRINT_CHAR_W_BRACKETS(c) and PRINT_STR(s)
	 
1-22-07 vtw_global11.h
Added SHOT_CONTROL_ENABLED and DIGITAL_SERVO_AMP bits to CONFIG_WORD2 V314.
Added EXTERNAL_SERVO_AMP to the cont. status word, V311
Added REVB_EXT_INT to the mon. status word, V312

2-16-07 vtw_global12.h
Made additional changes for position control.

1-18-08 vtw_global12.h
Added fixed4 pid structure.  Finished converting all parameters to the "parameter(x)" macro format. 

11-13-09 vtw_global15.h
Version for V8 Cyclone II firmware, supports 8 bit encoder divide by.

Added V452 to set the fake shot time between shots

3-19-10 vtw_global16.h
Added support for limit switches #7 and #8.  
Added fieldbus support.  added variable 453 to select fieldbus type

7-12-10 V12, Elminated hi res support
*/

#define NUM_OF_AXES 2
//#define TURN_OFF_CONT_OUTPUTS_EXCEPT_LS_WARN_MASK 0xfe00ffff // Will leave the warning output and ls's unchanged.
//#define TURN_OFF_ALL_CONT_OUTPUTS_MASK 0x0000ffff
//#define TURN_OFF_LS_AND_ZSPEED_OUTPUTS_MASK 0x09fffffe

#define ONCE 0
#define CONTINUOUS 1


// Variables 80 - 89 reserved for pressure control

#define CLK_FREQ vtg.v[300]
#define VEL1_CLK_FREQ vtg.v[301] // Read only
#define PN_ID vtg.v[302]
#define OUTPUT_STATUS_WORD1 vtg.v[303]
#define OUTPUT_STATUS_WORD2 vtg.v[304]
#define OUTPUT_STATUS_WORD3 vtg.v[305]
#define INPUT_STATUS_WORD1 vtg.v[306]
#define INPUT_STATUS_WORD2 vtg.v[307]
#define INPUT_STATUS_WORD3 vtg.v[308]
#define INPUT_STATUS_WORD4 vtg.v[309]
#define VEL2_CLK_FREQ vtg.v[310] // Read only
#define CONT_STATUS_WORD vtg.v[311] // Read only
	// Masks
	#define FATAL_ERROR_MASK (1<<0)
	#define WARNING_MASK (1<<1)
	#define PROGRAM_LOADED_MASK (1<<2)
	#define PROGRAM_RUNNING_MASK (1<<3)
	#define EXTERNAL_SERVO_AMP_MASK (1<<4)
	// Bit definitions
	#define FATAL_ERROR  (CONT_STATUS_WORD & FATAL_ERROR_MASK)
	#define WARNING		 (CONT_STATUS_WORD & WARNING_MASK)
	#define PROGRAM_LOADED (CONT_STATUS_WORD & PROGRAM_LOADED_MASK)
	#define PROGRAM_RUNNING (CONT_STATUS_WORD & PROGRAM_RUNNING_MASK)
	#define EXTERNAL_SERVO_AMP	(CONT_STATUS_WORD & EXTERNAL_SERVO_AMP_MASK)
	// Sets
	#define SET_FATAL_ERROR  CONT_STATUS_WORD |= FATAL_ERROR_MASK
	#define SET_WARNING		 CONT_STATUS_WORD |= WARNING_MASK
	#define SET_PROGRAM_LOADED CONT_STATUS_WORD |= PROGRAM_LOADED_MASK
	#define SET_PROGRAM_RUNNING CONT_STATUS_WORD |= PROGRAM_RUNNING_MASK
	#define SET_EXTERNAL_SERVO_AMP	CONT_STATUS_WORD |= EXTERNAL_SERVO_AMP_MASK
	// Clears
	#define CLR_FATAL_ERROR  CONT_STATUS_WORD &= ~FATAL_ERROR_MASK
	#define CLR_WARNING		 CONT_STATUS_WORD &= ~WARNING_MASK
	#define CLR_PROGRAM_LOADED CONT_STATUS_WORD &= ~PROGRAM_LOADED_MASK
	#define CLR_PROGRAM_RUNNING CONT_STATUS_WORD &= ~PROGRAM_RUNNING_MASK
	#define CLR_EXTERNAL_SERVO_AMP	CONT_STATUS_WORD &= ~EXTERNAL_SERVO_AMP_MASK
	
#define MON_STATUS_WORD vtg.v[312] // Read only
	// Masks
	#define CS_FROM_SS_MASK (1<<0)
	#define SHOT_IN_PROGRESS_MASK (1<<1)
	#define CURRENTLY_TIMING_MASK (1<<2)
	#define PAST_MIN_STK_LEN_MASK (1<<3)
	#define ZSPEED_MASK (1<<4)			
	#define SHOT_COMPLETE_MASK (1<<5)	
	#define MONITOR_ENABLE_MASK (1<<6)			
	#define PAST_MIN_VEL_EOS_MASK (1<<7)
	#define SHOT_IN_MOTION_MASK (1<<8)
	#define CS_DETECTED_MASK (1<<9)
	#define	REVB_EXT_INT_MASK (1<<10)
	// Bit definitions
	#define CS_FROM_SS				(MON_STATUS_WORD & CS_FROM_SS_MASK)
	#define SHOT_IN_PROGRESS		(MON_STATUS_WORD & SHOT_IN_PROGRESS_MASK)
	#define CURRENTLY_TIMING		(MON_STATUS_WORD & CURRENTLY_TIMING_MASK)
	#define PAST_MIN_STK_LEN		(MON_STATUS_WORD & PAST_MIN_STK_LEN_MASK)
	#define ZSPEED					(MON_STATUS_WORD & ZSPEED_MASK)		
	#define SHOT_COMPLETE			(MON_STATUS_WORD & SHOT_COMPLETE_MASK)
	#define MONITOR_ENABLE			(MON_STATUS_WORD & MONITOR_ENABLE_MASK)
	#define PAST_MIN_VEL_EOS		(MON_STATUS_WORD & PAST_MIN_VEL_EOS_MASK)
	#define SHOT_IN_MOTION			(MON_STATUS_WORD & SHOT_IN_MOTION_MASK)
	#define CS_DETECTED				(MON_STATUS_WORD & CS_DETECTED_MASK)
	#define	REVB_EXT_INT			(MON_STATUS_WORD & REVB_EXT_INT_MASK)
	// Sets
	#define SET_CS_FROM_SS				MON_STATUS_WORD |= CS_FROM_SS_MASK
	#define SET_SHOT_IN_PROGRESS		MON_STATUS_WORD |= SHOT_IN_PROGRESS_MASK
	#define SET_CURRENTLY_TIMING		MON_STATUS_WORD |= CURRENTLY_TIMING_MASK
	#define SET_PAST_MIN_STK_LEN		MON_STATUS_WORD |= PAST_MIN_STK_LEN_MASK
	#define SET_ZSPEED					{MON_STATUS_WORD |= ZSPEED_MASK; if (vtg.control_debug_level==1||vtg.control_debug_level==2) printf("\nDBG V ZSPEED detected\n"); /* Give a zspeed message if debug level 1 */}		
	#define SET_SHOT_COMPLETE			MON_STATUS_WORD |= SHOT_COMPLETE_MASK
	#define SET_MONITOR_ENABLE			MON_STATUS_WORD |= MONITOR_ENABLE_MASK
	#define SET_PAST_MIN_VEL_EOS		MON_STATUS_WORD |= PAST_MIN_VEL_EOS_MASK
	#define SET_SHOT_IN_MOTION			MON_STATUS_WORD |= SHOT_IN_MOTION_MASK
	#define SET_CS_DETECTED				MON_STATUS_WORD |= CS_DETECTED_MASK
	#define	SET_REVB_EXT_INT			MON_STATUS_WORD |= REVB_EXT_INT_MASK
	// Clears
	#define CLR_CS_FROM_SS				MON_STATUS_WORD &= ~CS_FROM_SS_MASK
	#define CLR_SHOT_IN_PROGRESS		MON_STATUS_WORD &= ~SHOT_IN_PROGRESS_MASK
	#define CLR_CURRENTLY_TIMING		MON_STATUS_WORD &= ~CURRENTLY_TIMING_MASK
	#define CLR_PAST_MIN_STK_LEN		MON_STATUS_WORD &= ~PAST_MIN_STK_LEN_MASK
	#define CLR_ZSPEED					MON_STATUS_WORD &= ~ZSPEED_MASK		
	#define CLR_SHOT_COMPLETE			MON_STATUS_WORD &= ~SHOT_COMPLETE_MASK
	#define CLR_MONITOR_ENABLE			MON_STATUS_WORD &= ~MONITOR_ENABLE_MASK
	#define CLR_PAST_MIN_VEL_EOS		MON_STATUS_WORD &= ~PAST_MIN_VEL_EOS_MASK
	#define CLR_SHOT_IN_MOTION			MON_STATUS_WORD &= ~SHOT_IN_MOTION_MASK
	#define CLR_CS_DETECTED				MON_STATUS_WORD &= ~CS_DETECTED_MASK
	#define	CLR_REVB_EXT_INT			MON_STATUS_WORD &= ~REVB_EXT_INT_MASK
		
#define CONFIG_WORD1 vtg.v[313]
	// CONFIG_WORD1 bit masks
	#define ANA_CH1_GAIN (1<<0) // Set for 0-5V input, cleared for 0-2.5V input
	#define ANA_CH2_GAIN (1<<1)
	#define ANA_CH3_GAIN (1<<2)
	#define ANA_CH4_GAIN (1<<3)
	#define ANA_CH5_GAIN (1<<4)
	#define ANA_CH6_GAIN (1<<5)
	#define ANA_CH7_GAIN (1<<6)
	#define ANA_CH8_GAIN (1<<7)
	#define ANA_CH9_GAIN (1<<8)
	#define ANA_CH10_GAIN (1<<9)
	#define ANA_CH11_GAIN (1<<10)
	#define ANA_CH12_GAIN (1<<11)
	#define ANA_CH13_GAIN (1<<12)
	#define ANA_CH14_GAIN (1<<13)
	#define ANA_CH15_GAIN (1<<14)
	#define ANA_CH16_GAIN (1<<15)
	#define SEL_CH17_24_DYN (1<<16)
	#define ENABLE_AXIS2 (1<<17)
	#define ENABLE_AXIS3 (1<<18)
	#define ENABLE_AXIS4 (1<<19)
	#define VEL1_CTR_DIV_BY2 (1<<20)
	#define VEL1_CTR_DIV_BY4 (1<<21)
	#define VEL1_CTR_DIV_BY8 (1<<22)
	#define VEL1_CTR_DIV_BY16 (1<<23)
	#define VEL2_CTR_DIV_BY2 (1<<24)
	#define VEL2_CTR_DIV_BY4 (1<<25)
	#define VEL2_CTR_DIV_BY8 (1<<26)
	#define VEL2_CTR_DIV_BY16 (1<<27)
	#define LOCAL_SHOT_XMIT_ENBL (1<<28)
	#define PLANTWIDE_SHOT_XMIT_ENBL (1<<29)
	#define ZSPEED_OUTPUT_ENBL (1<<30)
	#define LS_OFF_AT_ZSPEED (1<<31)
		

#define CONV_HI_CHANNELS (CONFIG_WORD1 & SEL_CH17_24_DYN)
#define ZSPEED_OUTPUT_ENABLED (CONFIG_WORD1 & ZSPEED_OUTPUT_ENBL)

#define CONFIG_WORD2 vtg.v[314]
	// Bits 0 - 3, Axis#1 quadrature divisor (lower 4 bits)
	// Bits 4 - 7, Axis#2 quadrature divisor (lower 4 bits)
	// Bits 24 - 27, Axis#1 quadrature divisor (upper 4 bits)
	// Bits 28 - 31, Axis#2 quadrature divisor (upper 4 bits)
	// Masks
	#define DONT_USE_LVDT_OFFSET_AMP1_MASK (1<<15) // May want to use the LVDT offset channel for something else, therefore we'll provide the means to disable it.
	#define DONT_USE_LVDT_OFFSET_AMP2_MASK (1<<16) // Ditto for the second offset channel
	#define HIGH_TRUE_OSW1_MASK (1<<10)
	#define HIGH_TRUE_ISW1_MASK (1<<11)
	// Bit definitions
	#define DONT_USE_LVDT_OFFSET_AMP1 (CONFIG_WORD2 & DONT_USE_LVDT_OFFSET_AMP1_MASK)
	#define DONT_USE_LVDT_OFFSET_AMP2 (CONFIG_WORD2 & DONT_USE_LVDT_OFFSET_AMP2_MASK)
	#define HIGH_TRUE_OSW1 (CONFIG_WORD2 & HIGH_TRUE_OSW1_MASK)
	#define HIGH_TRUE_ISW1 (CONFIG_WORD2 & HIGH_TRUE_ISW1_MASK)
	// Sets
	#define SET_DONT_USE_LVDT_OFFSET_AMP1 CONFIG_WORD2 |= DONT_USE_LVDT_OFFSET_AMP1_MASK
	#define SET_DONT_USE_LVDT_OFFSET_AMP2 CONFIG_WORD2 |= DONT_USE_LVDT_OFFSET_AMP2_MASK
	#define SET_HIGH_TRUE_OSW1 CONFIG_WORD2 |= HIGH_TRUE_OSW1_MASK
	#define SET_HIGH_TRUE_ISW1 CONFIG_WORD2 |= HIGH_TRUE_ISW1_MASK
	// Clears
	#define CLR_DONT_USE_LVDT_OFFSET_AMP1 CONFIG_WORD2 &= ~DONT_USE_LVDT_OFFSET_AMP1_MASK
	#define CLR_DONT_USE_LVDT_OFFSET_AMP2 CONFIG_WORD2 &= ~DONT_USE_LVDT_OFFSET_AMP2_MASK
	#define CLR_HIGH_TRUE_OSW1 CONFIG_WORD2 &= ~HIGH_TRUE_OSW1_MASK
	#define CLR_HIGH_TRUE_ISW1 CONFIG_WORD2 &= ~HIGH_TRUE_ISW1_MASK

	
#define WARNING_WORD vtg.v[315]
#define FATAL_ERROR_WORD vtg.v[316]
// V[317] Not defined, available
#define TOTAL_STROKE_LEN vtg.v[318]
#define MIN_STROKE_LEN vtg.v[319]
#define MIN_VEL_EOS vtg.v[320]
#define TIM_DEL_BISC vtg.v[321]
#define VEL_SET_EOS vtg.v[322] // Velocity enable point
// vtg.v[323] - vtg.v[327] available
#define	TIM_COLL_INTERVAL vtg.v[328] //uS
#define TIM_COLL_SAMPLES vtg.v[329]
#define TIME_OUT_PERIOD vtg.v[330]

// vtg.v[331] - vtg.v[339] available

// vtg.v[340] to vtg.v[360] reserved for die casting parameters and old velocity loop parameters.

// vtg.v[349] to vtg.v[360] reserved for die casting parameters.

#define BOARD_LEVEL_LVDT_OFFSET1_SETTING vtg.v[361]
#define BOARD_LEVEL_LVDT_OFFSET2_SETTING vtg.v[362]

// vtg.v[363] available

#define VEL1_MUL vtg.v[364]
#define VEL2_MUL vtg.v[365]

// Limit switches
#define LS1_1_SETPOINT vtg.v[366]
#define LS2_1_SETPOINT vtg.v[367]
#define LS3_1_SETPOINT vtg.v[368]
#define LS4_1_SETPOINT vtg.v[369]
#define LS5_1_SETPOINT vtg.v[370]  
#define LS6_1_SETPOINT vtg.v[371]
#define LS7_1_SETPOINT vtg.v[396]
#define LS8_1_SETPOINT vtg.v[397]
#define LS1_2_SETPOINT vtg.v[372]
#define LS2_2_SETPOINT vtg.v[373]
#define LS3_2_SETPOINT vtg.v[374]
#define LS4_2_SETPOINT vtg.v[375]
#define LS5_2_SETPOINT vtg.v[376]
#define LS6_2_SETPOINT vtg.v[377]
#define LS7_2_SETPOINT vtg.v[398]
#define LS8_2_SETPOINT vtg.v[399]

// Limit switch OSW1 masks, used to select which output turns on
#define LS1_1_MASK vtg.v[378]
#define LS2_1_MASK vtg.v[379]
#define LS3_1_MASK vtg.v[380]
#define LS4_1_MASK vtg.v[381]
#define LS5_1_MASK vtg.v[382]
#define LS6_1_MASK vtg.v[383]
#define LS7_1_MASK vtg.v[400]
#define LS8_1_MASK vtg.v[401]
#define LS1_2_MASK vtg.v[384]
#define LS2_2_MASK vtg.v[385]
#define LS3_2_MASK vtg.v[386]
#define LS4_2_MASK vtg.v[387]
#define LS5_2_MASK vtg.v[388]
#define LS6_2_MASK vtg.v[389]
#define LS7_2_MASK vtg.v[402]
#define LS8_2_MASK vtg.v[403]
#define ZSPEED_OUT_MASK vtg.v[394]
#define BINARY_VALVE_MASK vtg.v[395]

// Same thing defined as I/O numbers
#define LS1_1_IO_NUM vtg.v[378]
#define LS2_1_IO_NUM vtg.v[379]
#define LS3_1_IO_NUM vtg.v[380]
#define LS4_1_IO_NUM vtg.v[381]
#define LS5_1_IO_NUM vtg.v[382]
#define LS6_1_IO_NUM vtg.v[383]
#define LS7_1_IO_NUM vtg.v[400]
#define LS8_1_IO_NUM vtg.v[401]
#define LS1_2_IO_NUM vtg.v[384]
#define LS2_2_IO_NUM vtg.v[385]
#define LS3_2_IO_NUM vtg.v[386]
#define LS4_2_IO_NUM vtg.v[387]
#define LS5_2_IO_NUM vtg.v[388]
#define LS6_2_IO_NUM vtg.v[389]
#define LS7_2_IO_NUM vtg.v[402]
#define LS8_2_IO_NUM vtg.v[403]
#define ZSPEED_OUT_IO_NUM vtg.v[394]
#define BINARY_VALVE_IO_NUM vtg.v[395]

// O-scope variables
#define O_SCOPE_TRIG_V vtg.v[390]
#define O_SCOPE_TRIG_CH vtg.v[391]
#define O_SCOPE_SAMPLE_INTERVAL_uS vtg.v[392]
#define UPDATE_BLK_RAMP_TIME vtg.v[393]

// V396 - V403 used for limit switch #7 and #8 
// vtg.v[404] - vtg.v[413] reserved

#define VALVE_TYPE_AMP1	vtg.v[414]
#define VALVE_TYPE_AMP2 vtg.v[415]
// Define some valve types to use with V414 and V415 above
#define BINARY_VALVE 10

#define BINARY_VALVE_PULSE_TIME vtg.v[416]  // uS
#define BINARY_VALVE_DEVENT_TIME vtg.v[417] // uS

// vtg.v[418] - vtg.v[419] reserved

#define NULL_RESULT vtg.v[420]
#define NULL_RESULT_2 vtg.v[421]
#define	UDP_WAIT_TIME  vtg.v[422] // Time in mS
#define LAST_IP_CONNECTED_TO_LOCAL		vtg.v[423]
#define LAST_PORT_CONNECTED_TO_LOCAL	vtg.v[424]
#define LAST_IP_CONNECTED_TO_PLANT		vtg.v[425]
#define LAST_PORT_CONNECTED_TO_PLANT	vtg.v[426]
#define ISW1_BITS_TO_WATCH	vtg.v[427]
#define DELAY_BETWEEN_ANALOG_SAMPLES vtg.v[428]
#define ANALOG_MODE vtg.v[429] // Selects dynamic channel modes 1-8.
#define SERVO_AMP_DEMOD_PHASE_SETTINGS vtg.v[430] // Bits 0-7 == amp1 demod, Bits 8-15 == amp2 demod
												   // Bits 16-23 == osc demod., not used at this time, set to zero 

#define DIGITAL_POTS_SETTINGS vtg.v[431] // bits 0-7 == samp2 cur gain, bits 8-15 == samp2 LVDT gain
										 // bits 16-23 == samp1 cur gain, bits 24-31 == samp1 LVDT gain 

#define ISW1_MASK_ANALOG_REPORT1 vtg.v[433]	// isw1 mask, report #1   Input(s) to initiate a sample
#define ISW4_MASK_ANALOG_REPORT1 vtg.v[434] // isw4 mask, report #1
#define SAMPLE_INTERVAL_REPORT1  vtg.v[435] // Sample interval in uS for report #1.  If zero, a single sample report will be sent
#define ISW1_MASK_ANALOG_REPORT2 vtg.v[436]	// isw1 mask, report #2
#define ISW4_MASK_ANALOG_REPORT2 vtg.v[437] // isw4 mask, report #2
#define SAMPLE_INTERVAL_REPORT2  vtg.v[438] // Sample interval in uS for report #2.  If zero, a single sample report will be sent
#define ISW1_MASK_ANALOG_REPORT3 vtg.v[439]	// isw1 mask, report #3
#define ISW4_MASK_ANALOG_REPORT3 vtg.v[440] // isw4 mask, report #3
#define SAMPLE_INTERVAL_REPORT3  vtg.v[441] // Sample interval in uS for report #3.  If zero, a single sample report will be sent

#define DELAY_BETWEEN_ANALOG_SAMPLES_REVB vtg.v[442]

// vtg.v[443] - vtg.v[446] used for vloop integral gains.

#define CPROG_DATE_TIME	vtg.v[447] // Used by the True-Trak program to know when to download / update the control_all.txt file.
#define UPLOAD_NUMBER vtg.v[448]	// Used by the True-Trak program to know when to download / update setups

// Shot retract settings
#define SHOT_RETRACT_CH vtg.v[449] // Zero to disable, 1, 2, 3 and 4 are valid channel settings.
#define SHOT_RETRACT_ARM vtg.v[450]
#define SHOT_RETRACT_CMD vtg.v[451]
#define FAKE_SHOT_TIME_BETWEEN_SHOTS vtg.v[452]
#define UART2_FIELDBUS vtg.v[453]
#define UART2_BAUD_RATE vtg.v[454] // Clk freq/baudrate - 1 == (33.3333MHz/9600)-1 == 3471 for 9600 baud, 867 for 38.4K,  
#define FIELDBUS_CONFIG_WORD vtg.v[455]
		#define MAP_TO_ISW1 (FIELDBUS_CONFIG_WORD & (1<<0))
		#define MAP_FROM_OSW1 (FIELDBUS_CONFIG_WORD & (1<<1))
		#define ECHO_OSW1_TO_OPTO (FIELDBUS_CONFIG_WORD & (1<<2)) // Only valid if MAP_FROM_OSW1 above is set
// vtg.v[456] - vtg.v[499] available

// vtg.v[500] - vtg.v[511] reserved for operational variables within the control program
#define HIRES_RESET_COUNTS vtg.v[505] // Number of counts we'll consider the hi-res sensor to be reset
#define HIRES_DID_RESET vtg.v[506]

// "300" flags.  Every one that's added, must make an entry here.
struct
	{
	unsigned dither1_enbl : 1; // Output 401
	unsigned dither2_enbl : 1; // Output 402
	} flags_400;

////////////////////////////////// Control parameters  //////////////////////////////////

// Pressure control parameters.  Pressure control is not axis specific, so we'll leave it here.
#define PRESSURE_CONT_STATUS_WORD vtg.v[80]
	#define PRESS_CONTROL_ENABLED_MASK (1<<0)
	#define KEEP_PRESS_CMD_POSITIVE_MASK (1<<7)
	// Bits 1 - 3 determine what limit switch arms pressure control
	#define LS1_ARM_PRESS (((PRESSURE_CONT_STATUS_WORD&0x0000000e)>>1)==0)
	#define LS2_ARM_PRESS (((PRESSURE_CONT_STATUS_WORD&0x0000000e)>>1)==1)
	#define LS3_ARM_PRESS (((PRESSURE_CONT_STATUS_WORD&0x0000000e)>>1)==2)
	#define LS4_ARM_PRESS (((PRESSURE_CONT_STATUS_WORD&0x0000000e)>>1)==3)
	#define LS5_ARM_PRESS (((PRESSURE_CONT_STATUS_WORD&0x0000000e)>>1)==4)
	#define LS6_ARM_PRESS (((PRESSURE_CONT_STATUS_WORD&0x0000000e)>>1)==5)
	#define LS7_ARM_PRESS (((PRESSURE_CONT_STATUS_WORD&0x0000000e)>>1)==6)
	#define LS8_ARM_PRESS (((PRESSURE_CONT_STATUS_WORD&0x0000000e)>>1)==7)
#define PRESS_LOOP_GAIN vtg.v[81]
//#define PRESS_ARM_SETTING vtg.v[82] 
#define PRESS_ARM_SETTING vtg.v[90]&0x0000ffff // Goose setting for first pressure step   
#define PRESS_LOOP_TIME vtg.v[83]
#define PRESS_RETRACT_SETTING vtg.v[84]
#define PRESS_PARK_SETTING vtg.v[85]
#define PRESS_I_GAIN_SETTING vtg.v[87]
#define PRESS_D_GAIN_SETTING vtg.v[88]

#define PRESS_CONTROL_ENABLED (PRESSURE_CONT_STATUS_WORD & PRESS_CONTROL_ENABLED_MASK)
#define KEEP_PRESS_CMD_POSITIVE (PRESSURE_CONT_STATUS_WORD & KEEP_PRESS_CMD_POSITIVE_MASK)


// Die casting specific parameters, also not axis specific.
#define POS_CHECK_ZSPEED_SETTING vtg.v[349]
#define JS_SETTING			     vtg.v[352]
#define FT_SETTING				 vtg.v[353]
#define FT_STOP_POS_SETTING		 vtg.v[354]
#define RETR_SETTING		     vtg.v[355]
// vtg.v[356] available
#define SENSOR_DEVIATION_SETTING vtg.v[357]


// New axis specific parameter definitions.  Some of the variables used for axis #1 are a little disjointed
// as the original variable numbers are used to maintain compatibility with earlier versions.
//
// Variables 211 and 213 - 222 are available
//
// Axis #1 parameters
// Position loop parameters
#define PROPORTIONAL_GN_SETTING1	vtg.v[200]
#define INTEGRAL_GN_SETTING1		vtg.v[201]
#define DERIVATIVE_GN_SETTING1		vtg.v[202]
#define INTEGRATOR_LIMIT_SETTING1	vtg.v[203]
#define ACCEL_FEEDFORWARD_SETTING1	vtg.v[204]
#define VEL_FEEDFORWARD_SETTING1	vtg.v[205] 
#define	TORQUE_LIMIT_SETTING1		vtg.v[206] // Volts X 1000
#define PEAK_TORQUE_SETTING1		vtg.v[207] // Volts X 1000
#define POS_BREAK_POINT_SETTING1	vtg.v[208] 
#define POS_BREAK_GAIN_SETTING1		vtg.v[209]
#define SETTLING_ACCURACY_SETTING1	vtg.v[210]
// Velocity loop parameters
#define VEL_LOOP_GAIN_SETTING1		vtg.v[340]
#define VEL_LOOP_DERIVATIVE_GN_SETTING1 vtg.v[212]
#define VEL_LOOP_INTEGRAL_GN_SETTING1 vtg.v[443]
//#define VEL_LOOP_DERIVATIVE_GN_SETTING1 vtg.v[344]
//#define VEL_LOOP_INTEGRAL_GN_SETTING1 vtg.v[341]
#define VEL_LOOP_BK_VEL1_SETTING1	vtg.v[341]
#define VEL_LOOP_BK_VEL2_SETTING1	vtg.v[342]
#define VEL_LOOP_BK_VEL3_SETTING1	vtg.v[343]
#define VEL_LOOP_BKGN1_SETTING1		vtg.v[344]
#define VEL_LOOP_BKGN2_SETTING1		vtg.v[345]
#define VEL_LOOP_BKGN3_SETTING1		vtg.v[346]
// Common parameters
#define DITHER_TIME_SETTING1		vtg.v[347]
#define DITHER_AMPL_SETTING1		vtg.v[348]
#define NULL_OFFSET_SETTING1		vtg.v[350]
#define LVDT_OFFSET_SETTING1		vtg.v[351]
#define SAMPLE_TIME_SETTING1		vtg.v[223] // uS
#define SENSOR_TYPE_SETTING1		vtg.v[224] // 1 == Encoder, 2 == Hi-res over serial port

// Axis #2 parameters
// Position loop parameters
#define PROPORTIONAL_GN_SETTING2	vtg.v[225]
#define INTEGRAL_GN_SETTING2		vtg.v[226]
#define DERIVATIVE_GN_SETTING2		vtg.v[227]
#define INTEGRATOR_LIMIT_SETTING2	vtg.v[228]
#define ACCEL_FEEDFORWARD_SETTING2	vtg.v[229]
#define VEL_FEEDFORWARD_SETTING2	vtg.v[230]
#define	TORQUE_LIMIT_SETTING2		vtg.v[231]
#define PEAK_TORQUE_SETTING2		vtg.v[232]
#define POS_BREAK_POINT_SETTING2	vtg.v[233]
#define POS_BREAK_GAIN_SETTING2		vtg.v[234]
#define SETTLING_ACCURACY_SETTING2	vtg.v[235]
// Velocity loop parameters
#define VEL_LOOP_GAIN_SETTING2		vtg.v[236]
#define VEL_LOOP_DERIVATIVE_GN_SETTING2 vtg.v[237]
#define VEL_LOOP_INTEGRAL_GN_SETTING2 vtg.v[444]
#define VEL_LOOP_BK_VEL1_SETTING2	vtg.v[238]
#define VEL_LOOP_BK_VEL2_SETTING2	vtg.v[239]
#define VEL_LOOP_BK_VEL3_SETTING2	vtg.v[240]
#define VEL_LOOP_BKGN1_SETTING2		vtg.v[241]
#define VEL_LOOP_BKGN2_SETTING2		vtg.v[242]
#define VEL_LOOP_BKGN3_SETTING2		vtg.v[243]
// Common parameters
#define DITHER_TIME_SETTING2		vtg.v[244]
#define DITHER_AMPL_SETTING2		vtg.v[245]
#define NULL_OFFSET_SETTING2		vtg.v[246]
#define LVDT_OFFSET_SETTING2		vtg.v[247]
#define SAMPLE_TIME_SETTING2		vtg.v[248]
#define SENSOR_TYPE_SETTING2		vtg.v[249]

// Axis #3 parameters
// Position loop parameters
#define PROPORTIONAL_GN_SETTING3	vtg.v[250]
#define INTEGRAL_GN_SETTING3		vtg.v[251]
#define DERIVATIVE_GN_SETTING3		vtg.v[252]
#define INTEGRATOR_LIMIT_SETTING3	vtg.v[253]
#define ACCEL_FEEDFORWARD_SETTING3	vtg.v[254]
#define VEL_FEEDFORWARD_SETTING3	vtg.v[255]
#define	TORQUE_LIMIT_SETTING3		vtg.v[258]
#define PEAK_TORQUE_SETTING3		vtg.v[259]
#define POS_BREAK_POINT_SETTING3	vtg.v[260]
#define POS_BREAK_GAIN_SETTING3		vtg.v[261]
#define SETTLING_ACCURACY_SETTING3	vtg.v[262]
// Velocity loop parameters
#define VEL_LOOP_GAIN_SETTING3		vtg.v[263]
#define VEL_LOOP_DERIVATIVE_GN_SETTING3 vtg.v[264]
#define VEL_LOOP_INTEGRAL_GN_SETTING3 vtg.v[445]
#define VEL_LOOP_BK_VEL1_SETTING3	vtg.v[265]
#define VEL_LOOP_BK_VEL2_SETTING3	vtg.v[266]
#define VEL_LOOP_BK_VEL3_SETTING3	vtg.v[267]
#define VEL_LOOP_BKGN1_SETTING3		vtg.v[268]
#define VEL_LOOP_BKGN2_SETTING3		vtg.v[269]
#define VEL_LOOP_BKGN3_SETTING3		vtg.v[270]
// Common parameters
#define DITHER_TIME_SETTING3		vtg.v[271]
#define DITHER_AMPL_SETTING3		vtg.v[272]
#define NULL_OFFSET_SETTING3		vtg.v[273]
#define LVDT_OFFSET_SETTING3		vtg.v[274]
#define SAMPLE_TIME_SETTING3		vtg.v[256]
#define SENSOR_TYPE_SETTING3		vtg.v[257]

// Axis #4 parameters
// Position loop parameters
#define PROPORTIONAL_GN_SETTING4	vtg.v[275]
#define INTEGRAL_GN_SETTING4		vtg.v[276]
#define DERIVATIVE_GN_SETTING4		vtg.v[277]
#define INTEGRATOR_LIMIT_SETTING4	vtg.v[278]
#define ACCEL_FEEDFORWARD_SETTING4	vtg.v[279]
#define VEL_FEEDFORWARD_SETTING4	vtg.v[280]
#define	TORQUE_LIMIT_SETTING4		vtg.v[283]
#define PEAK_TORQUE_SETTING4		vtg.v[284]
#define POS_BREAK_POINT_SETTING4	vtg.v[285]
#define POS_BREAK_GAIN_SETTING4		vtg.v[286]
#define SETTLING_ACCURACY_SETTING4	vtg.v[287]
// Velocity loop parameters
#define VEL_LOOP_GAIN_SETTING4		vtg.v[288]
#define VEL_LOOP_DERIVATIVE_GN_SETTING4 vtg.v[289]
#define VEL_LOOP_INTEGRAL_GN_SETTING4 vtg.v[446]
#define VEL_LOOP_BK_VEL1_SETTING4	vtg.v[290]
#define VEL_LOOP_BK_VEL2_SETTING4	vtg.v[291]
#define VEL_LOOP_BK_VEL3_SETTING4	vtg.v[292]
#define VEL_LOOP_BKGN1_SETTING4		vtg.v[293]
#define VEL_LOOP_BKGN2_SETTING4		vtg.v[294]
#define VEL_LOOP_BKGN3_SETTING4		vtg.v[295]
// Common parameters
#define DITHER_TIME_SETTING4		vtg.v[296]
#define DITHER_AMPL_SETTING4		vtg.v[297]
#define NULL_OFFSET_SETTING4		vtg.v[298]
#define LVDT_OFFSET_SETTING4		vtg.v[299]
#define SAMPLE_TIME_SETTING4		vtg.v[281]
#define SENSOR_TYPE_SETTING4		vtg.v[282]


// Define control parameter structure
typedef volatile struct
{
// Position loop parameters
int prop_gain;				// Position loop proportional gain
int int_gain;				// Position loop integral gain
int deriv_gain;				// Position loop derivative gain
int int_limit;				// Position loop integrator limit
int accel_feedforward;
int	vel_feedforward;
int torque_limit;
int peak_torque;
int pos_bk_fe1;				// Position loop break gain following error setting
int pos_bk_gn1;				// Position loop gain break
int set_acc;				// Settling accuracy
// Velocity loop gain parameters
int vel_loop_gain;			// Velocity loop gain.
int vel_loop_deriv_gain;
int vel_loop_int_gain;
int bk_vel1; 				// Break velocity #1 in X1 pulses / sec
int bk_vel2;				// Break velocity #2 in X1 pulses / sec
int bk_vel3;				// Break velocity #3 in X1 pulses / sec
int bk_gn1;					// Break gain #1 multiplier X 1000
int bk_gn2;					// Break gain #2 multiplier X 1000
int bk_gn3;					// Break gain #3 multiplier X 1000
// Common parameters
int dither_time;			// Reserved for dither time
int dither_ampl;			// Reserved for dither amplitude
int null_offset;			// Null offset in volts * 1000
int lvdt_offset;			// LVDT offset in volts * 1000
int sample_time;			// Sample time in timer counts - the settings above are in uS and are converted to timer counts in initialize_axes().
int sample_time_setting;    // Sample time in uS
int sensor_type;
} control_parameter_def;



// Define control parameters 
//
// Position loop parameters
#define PROP_GAIN(x)		(axis[x].control_parameters.prop_gain)
#define INT_GAIN(x)			(axis[x].control_parameters.int_gain)
#define DERIV_GAIN(x)		(axis[x].control_parameters.deriv_gain)
#define INT_LIMIT(x)		(axis[x].control_parameters.int_limit)
#define ACCEL_FEEDFORWARD(x) (axis[x].control_parameters.accel_feedforward)
#define VEL_FEEDFORWARD(x)	(axis[x].control_parameters.vel_feedforward)
#define TORQUE_LIMIT(x)		(axis[x].control_parameters.torque_limit)
#define PEAK_TORQUE(x)		(axis[x].control_parameters.peak_torque)
#define POS_BK_GN_FE(x)		(axis[x].control_parameters.pos_bk_fe1)
#define POS_BK_GN(x)		(axis[x].control_parameters.pos_bk_gn1)
#define SET_ACC(x)			(axis[x].control_parameters.set_acc)
// Velocity loop parameters
#define VEL_LOOP_GAIN(x)	(axis[x].control_parameters.vel_loop_gain)
#define VEL_LOOP_DERIV_GAIN(x) (axis[x].control_parameters.vel_loop_deriv_gain)
#define VEL_LOOP_INT_GAIN(x) (axis[x].control_parameters.vel_loop_int_gain)
#define VEL_LOOP_BK_VEL1(x)	(axis[x].control_parameters.bk_vel1)
#define VEL_LOOP_BK_VEL2(x)	(axis[x].control_parameters.bk_vel2)
#define VEL_LOOP_BK_VEL3(x)	(axis[x].control_parameters.bk_vel3)
#define VEL_LOOP_BKGN1(x)	(axis[x].control_parameters.bk_gn1)
#define VEL_LOOP_BKGN2(x)	(axis[x].control_parameters.bk_gn2)
#define VEL_LOOP_BKGN3(x)	(axis[x].control_parameters.bk_gn3)
// Common parameters
#define DITHER_TIME(x)		(axis[x].control_parameters.dither_time)
#define DITHER_AMPL(x)		(axis[x].control_parameters.dither_ampl)
#define NULL_OFFSET(x)		(axis[x].control_parameters.null_offset)
#define LVDT_OFFSET(x)		(axis[x].control_parameters.lvdt_offset)
#define SAMPLE_TIME(x)		(axis[x].control_parameters.sample_time) // Sample time converted to timer counts
#define SAMPLE_TIME_SETTING(x) (axis[x].control_parameters.sample_time_setting) // Sample time setting in uS
#define SENSOR_TYPE(x)		(axis[x].control_parameters.sensor_type)
// Die casting specific parameters (not axis dependant)
#define POS_CHECK_ZSPEED POS_CHECK_ZSPEED_SETTING
#define JS			     JS_SETTING
#define FT				 FT_SETTING
#define FT_STOP_POS		 FT_STOP_POS_SETTING
#define RETR		     RETR_SETTING
#define SENSOR_DEVIATION SENSOR_DEVIATION_SETTING

/////////////////////////////////////////////////////////////////////////////////////////////////




// Digital servo amplifier defines
	#define NO_OF_DIGITAL_POT_BITS 7

	// Servo amp type
	#define AMP_TYPE vtg.amp_type
	#define NO_OR_EXT_AMP 0
	#define ONE_AXIS_INTEGRAL_AMP 1
	#define TWO_AXIS_INTEGRAL_AMP 2
	
	// Function servo_amp_interface() action types
	#define LOAD_DIGITAL_POTS 0			
	#define LOAD_DEMOD_PHASE_SETTINGS 1 
	#define INITIALIZE_SERVO_AMP 2		
	#define LOAD_BOARD_LEVEL_LVDT_OFFSETS 3 
	#define OPEN_VAL_LOOP1  4
	#define OPEN_VAL_LOOP2  5			   
	#define CLOSE_VAL_LOOP1 6
	#define CLOSE_VAL_LOOP2 7	
	#define OPEN_PRESSURE_FEEDBACK2 8
	#define CLOSE_PRESSURE_FEEDBACK2 9			   
	

#define ONE_MS_COUNTER vtg.one_ms_counter
//#define OUR_IP_ADDRESS *((net_32 *)0x106024)

// o_scope / analog reporting stuff
#define ANALOG_REPORTING vtg.analog_reporting
#define O_SCOPE vtg.o_scope

// o_scope timer mode definitions
#define ANALOG_REPORTING_MODE1 1
#define ANALOG_REPORTING_MODE2 2
#define ANALOG_REPORTING_MODE3 3
#define O_SCOPE_MODE 4

// The following definitions are useful when using the set_clr_discrete_output() and
// chk_discrete_input() functions.  They define literal I/O numbers.
//
// Fast inputs
#define CYCLE_START_INPUT 300
#define MASTER_ENABLE_INPUT 301
#define SHOT_XMIT_AGAIN_INPUT 302
#define FAST_AT_HOME_INPUT 304

// Control inputs
//#define ?_INPUT 49 // Formerly PUMP_ON_INPUT
#define START_SHOT_INPUT 50
#define FOLLOW_THROUGH_INPUT 51
#define RETRACT_INPUT 52
#define JOG_SHOT_INPUT 53
#define MANUAL_INPUT 54
#define AT_HOME_INPUT 55
//#define ?_INPUT 56 // Formerly VALVE_TEST_INPUT
//#define ?_INPUT 57 // Formerly EIGHT_IPS_INPUT
#define OK_FOR_FAST_SHOT_INPUT 58
#define VAC_ON_INPUT 59
#define LOW_IMPACT_INPUT 60
#define FILL_TEST_INPUT 61
//#define ?_INPUT 62
//#define ?_INPUT 63
//#define ?_INPUT 64 // Formerly E_STOP_INPUT

// Control outputs
#define SLOW_SHOT_OUTPUT 17
#define ACCUMULATOR_OUTPUT 18
//#define ?_OUTPUT 19
//#define ?_OUTPUT 20
#define VALVE_TEST_IN_OPERATION_OUTPUT 21
#define LS7_OUTPUT 22
#define LS8_OUTPUT 23
//#define ?_OUTPUT 24
#define  BINARY_VALVE_OUTPUT 25
#define LS5_OUTPUT 26
#define LS6_OUTPUT 27

#define WARNING_OUTPUT 28
#define LS1_OUTPUT 29 
#define LS2_OUTPUT 30
#define LS3_OUTPUT 31
#define LS4_OUTPUT 32 

// Monitor inputs
//#define ?_INPUT 33 // Formerly CYCLE_START_INPUT
//#define ?_INPUT 34
//#define ?_INPUT 35
//#define ?_INPUT 36
//#define ?_INPUT 37
//#define ?_INPUT 38
//#define ?_INPUT 39
//#define ?_INPUT 40 
//#define ?_INPUT 41 
//#define ?_INPUT 42
//#define ?_INPUT 43
//#define ?_INPUT 44
//#define ?_INPUT 45
//#define ?_INPUT 46
//#define ?_INPUT 47
//#define ?_INPUT 48 

// Monitor outputs
#define ZSPEED_OUTPUT 1
//#define ?_OUTPUT 2 // Formerly LS1
//#define ?_OUTPUT 3 // Formerly LS2
//#define ?_OUTPUT 4 // Formerly LS3
//#define ?_OUTPUT 5 // Formerly LS4
#define ALARM1_OUTPUT 6
#define ALARM2_OUTPUT 7
#define ALARM3_OUTPUT 8
//#define ?_OUTPUT 9
//#define ?_OUTPUT 10
//#define ?_OUTPUT 11
//#define ?_OUTPUT 12
//#define ?_OUTPUT 13 
//#define ?_OUTPUT 14
//#define ?_OUTPUT 15
//#define ?_OUTPUT 16 


// The following definitions are useful for directly testing / setting / clearing I/O
// without using a function call to set_clr_discrete_output() or chk_discrete_input().
//
// Define misc_ins PIO bits, "fast inputs."
#define INPUT_300 ((na_misc_ins->np_piodata)&0x01)
#define INPUT_301 ((na_misc_ins->np_piodata)&0x02)
#define INPUT_302 ((na_misc_ins->np_piodata)&0x04)
#define INPUT_303 ((na_misc_ins->np_piodata)&0x08)
#define INPUT_304 ((na_misc_ins->np_piodata)&0x10)
#define INPUT_305 ((na_misc_ins->np_piodata)&0x20)

#define CS_FROM_300 INPUT_300
#define MASTER_ENABLE INPUT_301

#define SW6 INPUT_300
#define SW7 INPUT_302
#define SW8 INPUT_303
#define SW9 INPUT_304

// Define dip switches
#define SW5_1 !((na_button_pio->np_piodata)&0x800)
#define SW5_2 !((na_button_pio->np_piodata)&0x400)
#define SW5_3 !((na_button_pio->np_piodata)&0x200)
#define SW5_4 !((na_button_pio->np_piodata)&0x100)
#define SW5_5 !((na_button_pio->np_piodata)&0x080)
#define SW5_6 !((na_button_pio->np_piodata)&0x040)
#define SW5_7 !((na_button_pio->np_piodata)&0x020)
#define SW5_8 !((na_button_pio->np_piodata)&0x010)

#define MAKE_FAKE_SHOTS SW5_2
#define STOP_CONTROL_PROGRAM_SWITCH SW5_3
#define	ENTER_SERIAL_MODE SW5_4
#define HAVE_REVB_EXT_INT SW5_5
#define HAVE_EXTERNAL_AMP SW5_6

// Control outputs
#define LS1_1_ON set_clr_discrete_output(LS1_1_IO_NUM, TURN_ON)
#define LS2_1_ON set_clr_discrete_output(LS2_1_IO_NUM, TURN_ON)
#define LS3_1_ON set_clr_discrete_output(LS3_1_IO_NUM, TURN_ON)
#define LS4_1_ON set_clr_discrete_output(LS4_1_IO_NUM, TURN_ON)
#define LS5_1_ON set_clr_discrete_output(LS5_1_IO_NUM, TURN_ON)
#define LS6_1_ON set_clr_discrete_output(LS6_1_IO_NUM, TURN_ON)
#define LS7_1_ON set_clr_discrete_output(LS7_1_IO_NUM, TURN_ON)
#define LS8_1_ON set_clr_discrete_output(LS8_1_IO_NUM, TURN_ON)
#define BINARY_VALVE_ON set_clr_discrete_output(BINARY_VALVE_IO_NUM, TURN_ON)

#define LS1_1_OFF set_clr_discrete_output(LS1_1_IO_NUM, TURN_OFF)
#define LS2_1_OFF set_clr_discrete_output(LS2_1_IO_NUM, TURN_OFF)
#define LS3_1_OFF set_clr_discrete_output(LS3_1_IO_NUM, TURN_OFF)
#define LS4_1_OFF set_clr_discrete_output(LS4_1_IO_NUM, TURN_OFF)
#define LS5_1_OFF set_clr_discrete_output(LS5_1_IO_NUM, TURN_OFF)
#define LS6_1_OFF set_clr_discrete_output(LS6_1_IO_NUM, TURN_OFF)
#define LS7_1_OFF set_clr_discrete_output(LS7_1_IO_NUM, TURN_OFF)
#define LS8_1_OFF set_clr_discrete_output(LS8_1_IO_NUM, TURN_OFF)
#define BINARY_VALVE_OFF set_clr_discrete_output(BINARY_VALVE_IO_NUM, TURN_OFF)
// More to come as needed



// For compatability with old source files
#define TURN_OFF_ALL_CONT_OUTPUTS 	OSW1 &= 0x0000ffff; \
									OSW3 = 0;

#define TURN_OFF_LS_AND_ZSPEED_OUTPUTS		LS1_1_OFF; \
											LS2_1_OFF; \
											LS3_1_OFF; \
											LS4_1_OFF; \
											LS5_1_OFF; \
											LS6_1_OFF; \
											LS7_1_OFF; \
											LS8_1_OFF; \
											ZSPEED_OUTPUT_OFF;

#define TURN_OFF_LS_OUTPUTS					LS1_1_OFF; \
											LS2_1_OFF; \
											LS3_1_OFF; \
											LS4_1_OFF; \
											LS5_1_OFF; \
											LS6_1_OFF; \
											LS7_1_OFF; \
											LS8_1_OFF; 


#define  WRITE_OSW1 	na_opto_control->np_piodata = 0; /* Clear read/write lines */ \
						halfword_ptr = (unsigned short int *)&OSW1; \
						na_opto_data->np_piodirection = 0xffff; \
						/* */ \
 						/* na_opto_data->np_piodata = 0x5555;*/ /* To reduce ground bounce */ \
						if(HIGH_TRUE_OSW1) na_opto_data->np_piodata = halfword_ptr[0]; /* Low half word of OSW1, inverted */ \
						else na_opto_data->np_piodata = ~halfword_ptr[0]; /* Low half word of OSW1 */ \
						na_opto_control->np_piodata = 0x2; /* wr monitor outputs */ \
						na_opto_control->np_piodata = 0;	/* Clear write lines */ \
						/* */ \
						na_opto_data->np_piodata = 0x5555; /*Reenabled this on 5-29-07 after finding the monitor outputs above could glitch if there were a lot of them set.*/ \
						if(HIGH_TRUE_OSW1) na_opto_data->np_piodata = halfword_ptr[1]; /* Upper half word of OSW1, inverted */ \
						else na_opto_data->np_piodata = ~halfword_ptr[1]; /* Upper half word of OSW1 */ \
						na_opto_control->np_piodata = 0x8; /* wr control outputs */ \
						/* na_opto_control->np_piodata = 0;*/	/* Clear write lines */ 
	
#define READ_ISW1	halfword_ptr = (unsigned short int *)&ISW1; \
	na_opto_control->np_piodata = 0; /* Clear read/write lines*/ \
	na_opto_data->np_piodirection = 0xffff; \
	na_opto_data->np_piodata = 0x5555; /* To reduce ground bounce*/ \
	/* */ \
	na_opto_data->np_piodirection = 0; /* S\et all bits to read*/ \
	/* */ \
	na_opto_control->np_piodata = 0x1; /* rd mon inputs */ \
	if(HIGH_TRUE_ISW1) halfword_ptr[0] = na_opto_data->np_piodata; /* Low half word of ISW1, inverted */ \
	else halfword_ptr[0] = ~na_opto_data->np_piodata; /* Low half word of ISW1 */ \
	/* */ \
	na_opto_control->np_piodata = 0; \
	na_opto_data->np_piodirection = 0xffff; \
	na_opto_data->np_piodata = 0x5555; \
	/* */ \
	na_opto_data->np_piodirection = 0; \
	na_opto_control->np_piodata = 0x4; /* rd cont inputs */ \
	if(HIGH_TRUE_ISW1) halfword_ptr[1] = na_opto_data->np_piodata; /* Upper half word of ISW1*/ \
	else halfword_ptr[1] = ~na_opto_data->np_piodata; /* Upper half word of ISW1*/
	

// Service limit switches directly without using the PVU.  This macro can be placed in the one mS timer routine.
#define SERVICE_LS(cur_pos, arm_press, ok_to_enbl) \
	\
	if ((PRESS_CONTROL_ENABLED) && (SHOT_IN_MOTION)) ok_to_enbl=1; \
	\
	if (cur_pos>=LS1_1_SETPOINT) \
		{ \
		LS1_1_ON; \
		if (ok_to_enbl && LS1_ARM_PRESS) arm_press=1; \
		} \
	else LS1_1_OFF; \
    \
	if (cur_pos>=LS2_1_SETPOINT) \
		{ \
		LS2_1_ON; \
		if (ok_to_enbl && LS2_ARM_PRESS) arm_press=1; \
		} \
	else LS2_1_OFF; \
    \
	if (cur_pos>=LS3_1_SETPOINT) \
		{ \
		LS3_1_ON; \
		if (ok_to_enbl && LS3_ARM_PRESS) arm_press=1; \
		} \
	else LS3_1_OFF; \
	\
	if (cur_pos>=LS4_1_SETPOINT) \
		{ \
		LS4_1_ON; \
		if (ok_to_enbl && LS4_ARM_PRESS) arm_press=1; \
		} \
	else LS4_1_OFF; \
	\
	if (cur_pos>=LS5_1_SETPOINT) \
		{ \
		LS5_1_ON; \
		if (ok_to_enbl && LS5_ARM_PRESS) arm_press=1; \
		} \
	else LS5_1_OFF; \
	\
	if (cur_pos>=LS6_1_SETPOINT) \
		{ \
		LS6_1_ON; \
		if (ok_to_enbl && LS6_ARM_PRESS) arm_press=1; \
		} \
	else LS6_1_OFF; \
	\
	if (cur_pos>=LS7_1_SETPOINT) \
		{ \
		LS7_1_ON; \
		if (ok_to_enbl && LS7_ARM_PRESS) arm_press=1; \
		} \
	else LS7_1_OFF; \
	\
	if (cur_pos>=LS8_1_SETPOINT) \
		{ \
		LS8_1_ON; \
		if (ok_to_enbl && LS8_ARM_PRESS) arm_press=1; \
		} \
	else LS8_1_OFF; \
	\
	if (arm_press) out_dac(AXIS2_DAC_CHANNEL, PRESS_ARM_SETTING, PRESS_CMD, ABSOLUTE);	
	
	
// Monitor outputs
#define ZSPEED_OUTPUT_ON set_clr_discrete_output(ZSPEED_OUT_IO_NUM, TURN_ON)
#define ZSPEED_OUTPUT_OFF set_clr_discrete_output(ZSPEED_OUT_IO_NUM, TURN_OFF)

// More to come as needed


#define EI asm("PFX 9"); \
			asm("WRCTL %g7")
				

#define DI asm("PFX 8"); \
			asm("WRCTL %g7")

			
// Compact print macros that have a small fooprint and execute quickly.  These are a better choice
// than printf for diagnosing problems.

// This is a bit of preprocessor magic I ran across for converting integers to strings.  This is useful
// for debugging as __LINE__, which is an integer can be converted into strings for easy printing.
// TOSTRING(x) is used to convert the int.
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// This prints a single char
#define PRINT_CHAR(c) \
	while (!(na_uart1->np_uartstatus & np_uartstatus_trdy_mask)) { } \
	na_uart1->np_uarttxdata = c; 

// This prints a char encased in brackets for testing, may be useful
#define PRINT_CHAR_W_BRACKETS(c) \
	PRINT_CHAR('['); \
	PRINT_CHAR(c); \
	PRINT_CHAR(']'); 

#define PRINT_STR(s) \
	zstr_ctr=0; /* Calling function must declare str_ctr */ \
	while (s[zstr_ctr] != NULL) \
		{ \
		PRINT_CHAR(s[zstr_ctr]); \
		zstr_ctr++; \
		}

// UART 2 defines
#define PRINT_CHAR_UART2(c) \
	while (!(na_uart2->np_uartstatus & np_uartstatus_trdy_mask)) { } \
	na_uart2->np_uarttxdata = c; 

#define PRINT_STR_UART2(s) \
	zstr_ctr=0; /* Calling function must declare str_ctr */ \
	while (s[zstr_ctr] != NULL) \
		{ \
		PRINT_CHAR_UART2(s[zstr_ctr]); \
		zstr_ctr++; \
		}

// Define command registers to talking to the AD nios
#define AD_CMD1 ad_cmd[0]
#define AD_CMD2 ad_cmd[1]
#define ISW4_BITS_TO_WATCH 0x0000003c // Excludes CS and master enable

//#define SETUP1 0x8350
//#define SETUP2 0x8750	
//#define SETUP3 0x8b50	
//#define SETUP4 0x8f50	
//#define SETUP5 0x9350	
//#define SETUP6 0x9750	
//#define SETUP7 0x9b50	
//#define SETUP8 0x9f50	
//#define SETUP9 0xa350	
//#define SETUP10 0xa750	
//#define SETUP11 0xab50	
//#define SETUP12 0xaf50	
//#define SETUP13 0xb350	
//#define SETUP14 0xb750	
//#define SETUP15 0xbb50	
//#define SETUP16 0xbf50	
//#define SETUP17 0xbf50 // All upper channels are muxed to channel 16 therefore they all have the same setup	
//#define SETUP18 0xbf50	
//#define SETUP19 0xbf50	
//#define SETUP20 0xbf50
//#define SETUP21 0xbf50	
//#define SETUP22 0xbf50	
//#define SETUP23 0xbf50	
//#define SETUP24 0xbf50

//#define RESULT1 shared_memory[0]
//#define RESULT2 shared_memory[1]
//#define RESULT3 shared_memory[2]
//#define RESULT4 shared_memory[3]
//#define RESULT5 shared_memory[4]
//#define RESULT6 shared_memory[5]
//#define RESULT7 shared_memory[6]
//#define RESULT8 shared_memory[7]
//#define RESULT9 shared_memory[8]
//#define RESULT10 shared_memory[9]
//#define RESULT11 shared_memory[10]
//#define RESULT12 shared_memory[11]
//#define RESULT13 shared_memory[12]
//#define RESULT14 shared_memory[13]
//#define RESULT15 shared_memory[14]
//#define RESULT16 shared_memory[15]
//#define RESULT17 shared_memory[16]
//#define RESULT18 shared_memory[17]
//#define RESULT19 shared_memory[18]
//#define RESULT20 shared_memory[19]
//#define RESULT21 shared_memory[20]
//#define RESULT22 shared_memory[21]
//#define RESULT23 shared_memory[22]
//#define RESULT24 shared_memory[23]
//#define FLOATING_CHANNEL_SETUP shared_memory[29]
//#define FLOATING_CHANNEL_RESULT shared_memory[30]
//#define ANALOG_DELAY shared_memory[31]


// AD processor channel setups
#define RESULT1_SETUP ad_setup[0]
#define RESULT2_SETUP ad_setup[1]
#define RESULT3_SETUP ad_setup[2]
#define RESULT4_SETUP ad_setup[3]
#define RESULT5_SETUP ad_setup[4]
#define RESULT6_SETUP ad_setup[5]
#define RESULT7_SETUP ad_setup[6]
#define RESULT8_SETUP ad_setup[7]


#define RESULT1 shared_memory[0]
#define RESULT2 shared_memory[1]
#define RESULT3 shared_memory[2]
#define RESULT4 shared_memory[3]
#define RESULT5 shared_memory[4]
#define RESULT6 shared_memory[5]
#define RESULT7 shared_memory[6]
#define RESULT8 shared_memory[7]

// Upper channels used for shot control, these results are not for shot monitoring.
#define RESULT17 shared_memory[16]
#define RESULT18 shared_memory[17]
#define RESULT19 shared_memory[18]
#define RESULT20 shared_memory[19]
#define RESULT21 shared_memory[20]
#define RESULT22 shared_memory[21]
#define RESULT23 shared_memory[22]
#define RESULT24 shared_memory[23]
#define ANALOG_DELAY_REVB shared_memory[26]
#define TOGGLE_CHANNELS shared_memory[27] // Allows converting in groups of 4 for high velocities, rev.B ext. int. only.
#define AD_STATUS shared_memory[28]
#define AD_VERSION shared_memory[30]
#define ANALOG_DELAY shared_memory[31]

#if 0
// Define vtg.gp_config_word bits
//#define ???_Available			vtg.gp_config_word & 0x00000001
#define SHOT_IN_PROGRESS		vtg.gp_config_word & 0x00000002
#define CURRENTLY_TIMING		vtg.gp_config_word & 0x00000004
#define CLEAR_1MS_COUNTER		vtg.gp_config_word & 0x00000008
#define CS_DETECTED				vtg.gp_config_word & 0x00000010
#define PAST_MIN_STK_LEN		vtg.gp_config_word & 0x00000020
#define ZSPEED					vtg.gp_config_word & 0x00000040
#define SHOT_COMPLETE			vtg.gp_config_word & 0x00000080
#define REQ_TO_BUFFER_DATA		vtg.gp_config_word & 0x00000100
#define ACK_REQ_TO_BUFFER_DATA	vtg.gp_config_word & 0x00000200
#define CONV_HI_CHANNELS		vtg.gp_config_word & 0x00000400
#define MONITOR_ENABLE			vtg.gp_config_word & 0x00000800
#define PAST_MIN_VEL_EOS		vtg.gp_config_word & 0x00001000

//#define SET_???_Available		vtg.gp_config_word |= 0x00000001
#define SET_SHOT_IN_PROGRESS	vtg.gp_config_word |= 0x00000002
#define SET_CURRENTLY_TIMING	vtg.gp_config_word |= 0x00000004
#define SET_CLEAR_1MS_COUNTER	vtg.gp_config_word |= 0x00000008
#define SET_CS_DETECTED			vtg.gp_config_word |= 0x00000010
#define SET_PAST_MIN_STK_LEN	vtg.gp_config_word |= 0x00000020
#define SET_ZSPEED				vtg.gp_config_word |= 0x00000040
#define SET_SHOT_COMPLETE		vtg.gp_config_word |= 0x00000080
#define SET_REQ_TO_BUFFER_DATA		vtg.gp_config_word |= 0x00000100
#define SET_ACK_REQ_TO_BUFFER_DATA	vtg.gp_config_word |= 0x00000200
#define SET_CONV_HI_CHANNELS		vtg.gp_config_word |= 0x00000400
#define SET_MONITOR_ENABLE			vtg.gp_config_word |= 0x00000800
#define SET_PAST_MIN_VEL_EOS		vtg.gp_config_word |= 0x00001000

//#define CLR_???_available		vtg.gp_config_word &= 0xfffffffe
#define CLR_SHOT_IN_PROGRESS	vtg.gp_config_word &= 0xfffffffd
#define CLR_CURRENTLY_TIMING	vtg.gp_config_word &= 0xfffffffb
#define CLR_CLEAR_1MS_COUNTER	vtg.gp_config_word &= 0xfffffff7
#define CLR_CS_DETECTED			vtg.gp_config_word &= 0xffffffef
#define CLR_PAST_MIN_STK_LEN	vtg.gp_config_word &= 0xffffffdf
#define CLR_ZSPEED				vtg.gp_config_word &= 0xffffffbf
#define CLR_SHOT_COMPLETE		vtg.gp_config_word &= 0xffffff7f
#define CLR_REQ_TO_BUFFER_DATA		vtg.gp_config_word &= 0xfffffeff
#define CLR_ACK_REQ_TO_BUFFER_DATA	vtg.gp_config_word &= 0xfffffdff
#define CLR_CONV_HI_CHANNELS		vtg.gp_config_word &= 0xfffffbff
#define CLR_MONITOR_ENABLE			vtg.gp_config_word &= 0xfffff7ff
#define CLR_PAST_MIN_VEL_EOS		vtg.gp_config_word &= 0xffffefff
#endif



#define OSW1	vtg.osw1 // Opto or Ethernet based outputs
#define OSW2	vtg.osw2 // Fieldbus outputs only
#define OSW3	vtg.osw3 // Fieldbus outputs only
#define ISW1	vtg.isw1 // Opto or Ethernet based inputs
#define ISW2	vtg.isw2 // Fieldbus inputs only
#define ISW3	vtg.isw3 // Not supported
#define ISW4	na_misc_ins->np_piodata // Fast on-board inputs

// tb defines
#ifndef NULL
 #define NULL 0
#endif

#ifndef TURN_ON
	#define TURN_ON 1
#endif

#ifndef	TURN_OFF
	#define TURN_OFF 0
#endif	
	
#ifndef ON
	#define ON 1
#endif	
	
#ifndef OFF	
	#define OFF 0
#endif

#ifndef VOLTAGE_X_1000
	#define VOLTAGE_X_1000 1
#endif	
	
#ifndef RAW_VAL	
	#define RAW_VAL 2
#endif

#ifndef PRESS_CMD	
	#define PRESS_CMD 3
#endif

#ifndef PRESS_RAW	
	#define PRESS_RAW 4
#endif

#ifndef ABSOLUTE	
	#define ABSOLUTE 0
#endif

#ifndef INCREMENTAL	
	#define INCREMENTAL 1
#endif
	
#define DONT_CLEAR_SAVED_WARNING 0
#define CLEAR_SAVED_WARNING 1



/////////////////////// Items formerly in tiu2.h ////////////////////////////////


// The following define the misc output signals.  The following declaration
// must be made in any function using these definitions:
// np_pio *na_misc_outs = na_misc_outs;


#ifndef PULSE_PERIPHERAL_RESET
   #define PULSE_PERIPHERAL_RESET \
							vtg.misc_outs |= 0x01; \
							na_misc_outs->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xfe; \
							na_misc_outs->np_piodata = vtg.misc_outs;
#endif						
							

#ifndef PULSE_WATCHDOG_RESET
   #define PULSE_WATCHDOG_RESET \
							vtg.misc_outs |= 0x02; \
							na_misc_outs->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xfd; \
							na_misc_outs->np_piodata = vtg.misc_outs;
#endif


#ifndef PULSE_WATCHDOG_FAULT
   #define PULSE_WATCHDOG_FAULT \
							vtg.misc_outs |= 0x20; \
							na_misc_outs->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xdf; \
							na_misc_outs->np_piodata = vtg.misc_outs;
#endif

#ifndef KEEP_WATCHDOG_ALIVE
	// If using an external (external to the processor core) watchdog.  misc_outputs[6] is the reset line.
	#define KEEP_WATCHDOG_ALIVE \
							vtg.misc_outs |= 0x80; \
							na_misc_outs->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0x7f; \
							na_misc_outs->np_piodata = vtg.misc_outs

	// If using an Avalon watchdog timer, write anything to retrigger.
	//#define KEEP_WATCHDOG_ALIVE na_watchdog->np_timerperiodl = 0x55; 	
#endif

#ifndef ENABLE_WD_CTR
	#define ENABLE_WD_CTR \
							vtg.misc_outs |= 0x40; \
							na_misc_outs->np_piodata = vtg.misc_outs
#endif

#ifndef DISABLE_WD_CTR
	#define DISABLE_WD_CTR \
							vtg.misc_outs &= 0xbf; \
							na_misc_outs->np_piodata = vtg.misc_outs
#endif

	
//#define PULSE_CONTROL_INTERRUPT \
							vtg.misc_outs |= 0x40; \
							na_misc_outs->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xbf; \
							na_misc_outs->np_piodata = vtg.misc_outs;



// Since many locations in the pvu and timer unit have different read and write 
// meanings, use a union for each addr, one for the write value and one for the 
// read.  In some cases, the word is further borken down into bit fields which 
// have defined above.
//
typedef volatile union
{
unsigned int wr_ls1;
unsigned int rd_current_pos;
} pvu_a0;

typedef volatile union
{
unsigned int wr_ls2;
unsigned int rd_ipend;
} pvu_a1;

typedef volatile union
{
unsigned int wr_ls3;
unsigned int rd_istatus;
} pvu_a2;

typedef volatile union
{
unsigned int wr_ls4;
unsigned int rd_imask;
} pvu_a3;

typedef volatile union
{
unsigned int wr_ls5;
unsigned int rd_int_enbl;
} pvu_a4;

typedef volatile union
{
unsigned int wr_ls6;
unsigned int rd_pload;
} pvu_a5;

typedef volatile union
{
unsigned int wr_gp_pos1;
unsigned int rd_int_sig_vdiv;
} pvu_a6;

typedef volatile union
{
unsigned int wr_gp_pos2;
unsigned int rd_latched_q1;
} pvu_a7;


typedef volatile union
{
unsigned int wr_ls7;
unsigned int rd_latched_q2;
} pvu_a8;

typedef volatile union
{
unsigned int wr_ls8;
unsigned int rd_latched_q3;
} pvu_a9;

typedef volatile union
{
unsigned int wr_not_used;
unsigned int rd_latched_q4;
} pvu_a10;

typedef volatile union
{
unsigned int wr_not_used;
unsigned int rd_velocity;
} pvu_a11;

typedef volatile union
{
unsigned int wr_int_enbl;
unsigned int rd_vel_counter;
} pvu_a12;

typedef volatile union
{
unsigned int wr_imask;
unsigned int rd_not_used;
} pvu_a13;

typedef volatile union
{
unsigned int wr_pos_preload;
unsigned int rd_not_used;
} pvu_a14;

typedef volatile union
{
unsigned int wr_vel_clk_div;
unsigned int rd_not_used;
} pvu_a15;


// Position / velocity unit registers. 
typedef volatile struct
{
pvu_a0 a0;	 
pvu_a1 a1;	 
pvu_a2 a2;	 
pvu_a3 a3;	 
pvu_a4 a4;	 
pvu_a5 a5;	 
pvu_a6 a6;	  
pvu_a7 a7;	 
pvu_a8 a8;	 
pvu_a9 a9;	 
pvu_a10 a10; 
pvu_a11 a11; 
pvu_a12 a12; 
pvu_a13 a13; 
pvu_a14 a14; 
pvu_a15 a15; 
} vp_pvu;



// Rename nios.h generated address definition for the tiu.
//#define va_pv1 ((vp_pvu * ) 0x00004d40)
//#define va_pv2 ((vp_pvu * ) 0x00005100)
#define va_pv1 ((vp_pvu * ) na_pv_unit1_base)
#define va_pv2 ((vp_pvu * ) na_pv_unit2_base)
//#define va_tu1 ((vp_timer_unit * ) 0x000004a0)




typedef volatile struct
{
int ls1;
int ls2;
int ls3;
int ls4;
int ls5;
int ls6;
int ls7;
int ls8;
} ls_def;


////////////////////// PID loop definitions ////////////////////////////////
// Define the PID equation types
#define IDEAL 1
#define SERIAL 2 
#define RECURSIVE 3

#define FLOAT_PID 0 // 0 == Integer PID loop and 1 == floating point PID loop

// For floating point PID
typedef volatile struct
	{
	int loop_type;            // Velocity, position etc.  May be useful.
	int equation;			 // See equation type defines
	int derivative_on_CV; // If zero, derivative applied to the error.  If 1 it's applied to the change in CV (Current Value).  If 2 it's applied to rate of change of the CV.  Probably useful for position control.
	int recursive_integral;
	double prev_CV;    // Privous Current Value (feedback from parameter being controlled such as pos, vel, press etc.).

	double d_state;
	double i_state;			// Integrator state
	double i_max, i_min;	// Integrator limits
	double output;
	double o_max, o_min;	// Output limits
	double	i_gain,    		// integral gain
        	p_gain,    		// proportional gain
        	d_gain;     	// derivative gain
	} vs_pid_float;

	
// Fixed point decimal version.  Uses 4 decimal places.
typedef volatile struct
	{
	int loop_type;          // Velocity, position etc.  May be useful.
	int equation;		    // See equation type defines
	int derivative_on_CV;   // If zero, derivative applied to the error.  If 1 it's applied to the change in CV (Current Value).  If 2 it's applied to rate of change of the CV.  Probably useful for position control.
	int recursive_integral;
	fixed4 prev_CV;         // Privous Current Value (feedback from parameter being controlled such as pos, vel, press etc.).
	fixed4 d_state;
	fixed4 i_state;		    // Integrator state
	fixed4 i_max, i_min;	// Integrator limits
	fixed4 output;
	fixed4 o_max, o_min;	// Output limits
	fixed4 i_gain,    	    // integral gain
           p_gain,    	    // proportional gain
           d_gain;          // derivative gain
	} vs_pid_fixed4;	
	
	
// For integer version 
typedef volatile struct
	{
	int loop_type;            // Velocity, position etc.  May be useful.
	int equation;		   // See equation type defines
	int derivative_on_CV; // If zero, derivative applied to the error.  If 1 it's applied to the change in CV (Current Value).  If 2 it's applied to rate of change of the CV.  Probably useful for position control.
	int recursive_integral;
	long prev_CV;        // Privous Current Value (feedback from parameter being controlled such as pos, vel, press etc.).
	long d_state;
	long i_state;		// Integrator state
	long i_max, i_min;	// Integrator limits
	long output;
	long o_max, o_min;	// Output limits
	long	i_gain,    	// integral gain
        	p_gain,    	// proportional gain
        	d_gain;     // derivative gain
	} vs_pid;
////////////////////////////////////////////////////////////////////////

// Loop types
#define VEL_LOOP 1
#define POS_LOOP 2
#define PRESS_LOOP 3
#define VALVE_LOOP 4

/////////////////////////// Position loop definitions ////////////////////////////
// Return values from the update_pos_loop() function.
#define PLOOP_KEEP_GOING 0
#define CMD_POS_EQ_ACT 1
#define ACT_POS_EQ_FINAL 2
#define ACC_DCC_FINISHED 3
#define MOTION_STOPPED 4

typedef volatile struct
{
int starting_pos;   // X4 counts
int ending_pos;		// "   "   "
int starting_vel;   // X4 cps
int ending_vel;     // "    "
int sample_time_uS;
int servo_here;		// Maintain the commanded position below 
int commanded_pos;  // Result back from the position loop in X4 counts.
} vs_ploop;


// New loop with fixed4 numbers
typedef volatile struct
{
fixed4 starting_pos;   // X4 counts
fixed4 ending_pos;		// "   "   "
fixed4 starting_vel;   // X4 cps
fixed4 ending_vel;     // "    "
int sample_time_uS;
int servo_here;		// Maintain the commanded position below 
fixed4 commanded_pos;  // Result back from the position loop in X4 counts.
} vs_ploop_fixed4;
//////////////////////////////////////////////////////////////////////////////////

// Sensor types
#define ENCODER 1
#define HI_RES 2


// Define axis structure
typedef volatile struct
{
vp_pvu *pvu; // The pvu associated with this axis
unsigned int vel_ctr_freq;
unsigned int vel_multiplier;
unsigned int imask;
unsigned int vdiv;
unsigned int pload;
int dac_channel;  // Dac channel associated with this axis
int dac_channel_for_board_level_lvdt_offset;
int soft_velocity;  // Software calculated velocity, may be used for something in future
int valve_type; // For future use, none defined at this time
int adc_channel_for_lvdt;
int adc_channel_for_command_monitoring;
control_parameter_def control_parameters;
//integral_servo_amp_def samp_setups;
ls_def limit_switch;
#if FLOAT_PID  // Floating point PID loop
	vs_pid_float pid;
	vs_pid_float valve_pid;
#else  // We're using the integer routine
	vs_pid_fixed4 pid;
	vs_pid_fixed4 valve_pid;
#endif
vs_ploop ploop; // Position loop
// 20P equiv. 
//
} axis_def;

// Declare a global axis array
axis_def axis[NUM_OF_AXES+1];  // Location 0 not used

// declare the pressure control PID loop
#if FLOAT_PID  // Floating point PID loop
	vs_pid_float press_pid
#else  // We're using the integer routine
	vs_pid_fixed4 press_pid;
#endif

// PID read/writes based on the axis num.  Each axis has two PID structures, one for the main control loop and one
// for the valve loop which is defined below.
#define PID_STRUCT(x) (axis[x].pid)
#define PID_LOOP_TYPE(x) (axis[x].pid.loop_type)
#define PID_EQUATION(x) (axis[x].pid.equation)
#define PID_DER_ON_CV(x) (axis[x].pid.derivative_on_CV)
#define PID_RECURSIVE_INTEGRAL(x) (axis[x].pid.recursive_integral)
#define PID_PREV_CV(x) (axis[x].pid.prev_CV)
#define PID_D_STATE(x) (axis[x].pid.d_state)
#define PID_I_STATE(x) (axis[x].pid.i_state)
#define PID_I_MAX(x) (axis[x].pid.i_max)
#define PID_I_MIN(x) (axis[x].pid.i_min)
#define PID_OUTPUT(x) (axis[x].pid.output)
#define PID_O_MAX(x) (axis[x].pid.o_max)
#define PID_O_MIN(x) (axis[x].pid.o_min)
#define PID_I_GAIN(x) (axis[x].pid.i_gain)
#define PID_P_GAIN(x) (axis[x].pid.p_gain)
#define PID_D_GAIN(x) (axis[x].pid.d_gain)

// Valve loop pid.
#define VALVE_PID_STRUCT(x) (axis[x].valve_pid)
#define VALVE_PID_EQUATION(x) (axis[x].valve_pid.equation)
#define VALVE_PID_DER_ON_CV(x) (axis[x].valve_pid.derivative_on_CV)
#define VALVE_PID_RECURSIVE_INTEGRAL(x) (axis[x].valve_pid.recursive_integral)
#define VALVE_PID_PREV_CV(x) (axis[x].valve_pid.prev_CV)
#define VALVE_PID_D_STATE(x) (axis[x].valve_pid.d_state)
#define VALVE_PID_I_STATE(x) (axis[x].valve_pid.i_state)
#define VALVE_PID_I_MAX(x) (axis[x].valve_pid.i_max)
#define VALVE_PID_I_MIN(x) (axis[x].valve_pid.i_min)
#define VALVE_PID_OUTPUT(x) (axis[x].valve_pid.output)
#define VALVE_PID_O_MAX(x) (axis[x].valve_pid.o_max)
#define VALVE_PID_O_MIN(x) (axis[x].valve_pid.o_min)
#define VALVE_PID_I_GAIN(x) (axis[x].valve_pid.i_gain)
#define VALVE_PID_P_GAIN(x) (axis[x].valve_pid.p_gain)
#define VALVE_PID_D_GAIN(x) (axis[x].valve_pid.d_gain)

// Pressure control loop pid.
#define PRESS_PID_STRUCT (press_pid)
#define PRESS_PID_EQUATION (press_pid.equation)
#define PRESS_PID_DER_ON_CV (press_pid.derivative_on_CV)
#define PRESS_PID_RECURSIVE_INTEGRAL (press_pid.recursive_integral)
#define PRESS_PID_PREV_CV (press_pid.prev_CV)
#define PRESS_PID_D_STATE (press_pid.d_state)
#define PRESS_PID_I_STATE (press_pid.i_state)
#define PRESS_PID_I_MAX (press_pid.i_max)
#define PRESS_PID_I_MIN (press_pid.i_min)
#define PRESS_PID_OUTPUT (press_pid.output)
#define PRESS_PID_O_MAX (press_pid.o_max)
#define PRESS_PID_O_MIN (press_pid.o_min)
#define PRESS_PID_I_GAIN (press_pid.i_gain)
#define PRESS_PID_P_GAIN (press_pid.p_gain)
#define PRESS_PID_D_GAIN (press_pid.d_gain)

// Position loop
#define PLOOP_STRUCT(x) (axis[x].ploop)
#define STARTING_POS(x) (axis[x].ploop.starting_pos)
#define ENDING_POS(x) (axis[x].ploop.ending_pos)
#define STARTING_VEL(x) (axis[x].ploop.starting_vel)
#define ENDING_VEL(x) (axis[x].ploop.ending_vel)
#define SAMPLE_TIME_uS(x) (axis[x].ploop.sample_time_uS)
#define SERVO_HERE(x) (axis[x].ploop.servo_here)  //SERVO_HERE (axis[axis_num].ploop.servo_here)
#define COMMANDED_POS(x) (axis[x].ploop.commanded_pos)
// Define upper analog channels
//
// First the control channels.  These channels are not present in an integral servo amp board
// is not attached to a rev. D or later external interface board.
//#define CANA_CH1 17
//#define CANA_CH2 18
//#define CANA_CH3 19
//#define CANA_CH4 20
// Now the general purpose wide input range analog channels
//#define WANA_CH1 21
//#define WANA_CH2 22
//#define WANA_CH3 23
//#define WANA_CH4 24

#define NO_OF_DAC_CHANNELS 4
// Dac channels 2 and 4 are for setting offsets
#define AXIS1_DAC_CHANNEL 1
#define AXIS2_DAC_CHANNEL 3  // Can be another control axis or pressure control
#define AXIS1_BOARD_LEVEL_LVDT_OFFSET_CHANNEL 2
#define AXIS2_BOARD_LEVEL_LVDT_OFFSET_CHANNEL 4
//#define PRESSURE_CONTROL_DAC_CHANNEL 99

//#define AXIS1_ADC_CHANNEL_FOR_LVDT_SETTING 17
//#define AXIS2_ADC_CHANNEL_FOR_LVDT_SETTING 19
#define AXIS1_VALVE_TYPE VALVE_TYPE_AMP1 
#define AXIS2_VALVE_TYPE VALVE_TYPE_AMP2


// Define digital servo amp settings
//#define BOARD_LEVEL_LVDT_OFFSET (axis[axis_num].samp_setups.board_level_lvdt_offset)

#define DAC_OUTPUT_CHANNEL(x) (axis[x].dac_channel)
#define BOARD_LEVEL_LVDT_OFFSET_DAC_CHANNEL(x) (axis[x].dac_channel_for_board_level_lvdt_offset)
#define SOFT_VELOCITY(x) (axis[x].soft_velocity) // Software calculated velocity
#define ADC_CHANNEL_FOR_LVDT(x) (axis[x].adc_channel_for_lvdt)
#define ADC_CHANNEL_FOR_COMMAND_MONITORING(x) (axis[x].adc_channel_for_command_monitoring)
#define VALVE_TYPE(x) (axis[x].valve_type)

// Combined pvu writes based on global axis. 
#define	WR_PVLS1(x) axis[x].pvu->a0.wr_ls1
#define	WR_PVLS2(x) axis[x].pvu->a1.wr_ls2
#define	WR_PVLS3(x) axis[x].pvu->a2.wr_ls3
#define	WR_PVLS4(x) axis[x].pvu->a3.wr_ls4
#define	WR_PVLS5(x) axis[x].pvu->a4.wr_ls5
#define	WR_PVLS6(x) axis[x].pvu->a5.wr_ls6
#define	WR_PV_GP_POS1(x) axis[x].pvu->a6.wr_gp_pos1
#define	WR_PV_GP_POS2(x) axis[x].pvu->a7.wr_gp_pos2
#define	WR_PVLS7(x) axis[x].pvu->a8.wr_ls7
#define	WR_PVLS8(x) axis[x].pvu->a9.wr_ls8
//#define WR_PV_INT_ENBL(x) axis[x].pvu->a12.wr_int_enbl
//#define WR_PV_IMASK(x) axis[x].pvu->a13.wr_imask
#define WR_PV_PRELOAD(x) axis[x].pvu->a14.wr_pos_preload
#define WR_PV_VEL_CLK_DIVISOR(x) axis[x].pvu->a15.wr_vel_clk_div

// Combined pvu reads based on global axis. 
#define RD_PV_POS(x) axis[x].pvu->a0.rd_current_pos
#define RD_PV_IPEND(x) axis[x].pvu->a1.rd_ipend
#define RD_PV_ISTATUS(x) axis[x].pvu->a2.rd_istatus

//#define RD_PV_IPEND(x) axis[x].pvu->a1.rd_ipend
//#define RD_PV_ISTATUS(x) axis[x].pvu->a2.rd_istatus

//#define RD_PV_IMASK(x) axis[x].pvu->a3.rd_imask					// For diagnostics
//#define RD_PV_INT_ENBL(x) axis[x].pvu->a4.rd_int_enbl				//	"	"	"	"
//#define RD_PV_PLOAD(x) axis[x].pvu->a5.rd_pload					//	"	"	"	"
//#define RD_PV_INT_SIG_VDIV(x) axis[x].pvu->a6.rd_int_sig_vdiv		//	"	"	"	"

#define RD_PV_LATCHED_Q1(x) axis[x].pvu->a7.rd_latched_q1
#define RD_PV_LATCHED_Q2(x) axis[x].pvu->a8.rd_latched_q2
#define RD_PV_LATCHED_Q3(x) axis[x].pvu->a9.rd_latched_q3
#define RD_PV_LATCHED_Q4(x) axis[x].pvu->a10.rd_latched_q4
#define RD_PV_VEL(x) axis[x].pvu->a11.rd_velocity

//#define RD_PV_VEL SOFT_VELOCITY // Enable to read a software calculated velociy
#define RD_PV_RAW_VEL_CTR(x) axis[x].pvu->a12.rd_vel_counter

// Combined pv items.  axis_num must be set to the correct axis number before using these.
//#define ENBL_PV_INT  WR_PV_INT_ENBL = 1 
//#define DIS_PV_INT	 WR_PV_INT_ENBL = 0   
//#define SET_PV_IMASK(imask__) \
						axis[axis_num].imask=imask__; \
						WR_PV_IMASK(axis_num)=axis[axis_num].imask;

#define SET_PV_VCLKDIV(pv_div) \
						axis[axis_num].vdiv=(unsigned int)pv_div;\
						WR_PV_VEL_CLK_DIVISOR(axis_num)=axis[axis_num].vdiv;\
						if(axis[axis_num].vdiv & 0x1)\
							{axis[axis_num].vel_ctr_freq=CLK_FREQ/2;}\
						else if(axis[axis_num].vdiv&0x2)\
							{axis[axis_num].vel_ctr_freq=CLK_FREQ/4;}\
						else if(axis[axis_num].vdiv&0x4)\
							{axis[axis_num].vel_ctr_freq=CLK_FREQ/8;}\
						else if(axis[axis_num].vdiv&0x8)\
							{axis[axis_num].vel_ctr_freq=CLK_FREQ/16;}\
						axis[axis_num].vel_multiplier=0xffffffff/axis[axis_num].vel_ctr_freq;\
						if((0xffffffff%axis[axis_num].vel_ctr_freq)>(axis[axis_num].vel_ctr_freq/2)){axis[axis_num].vel_multiplier++;} 
					


// misc_outputs[3] and misc_outputs[4] are used to pulse the preloads for axis1
// and axis2 respectively
#ifndef PULSE_PRELOAD
	#define PULSE_PRELOAD \
							if (axis_num==1) \
								{ \
								vtg.misc_outs |= 0x08; \
								na_misc_outs->np_piodata = vtg.misc_outs; \
								vtg.misc_outs &= 0xf7; \
								na_misc_outs->np_piodata = vtg.misc_outs; \
								} \
							else if (axis_num==2) \
								{ \
								vtg.misc_outs |= 0x10; \
								na_misc_outs->np_piodata = vtg.misc_outs; \
								vtg.misc_outs &= 0xef; \
								na_misc_outs->np_piodata = vtg.misc_outs; \
								}
#endif


#define SET_PV_PLOAD(x, preload) \
						axis[x].pload = preload; \
						WR_PV_PRELOAD(x) = axis[x].pload; \
						/* misc_outputs[3] and misc_outputs[4] are used to pulse the preloads for axis1 */ \
						/* and axis2 respectively */ \
						if (x==1) /* Axis #1 */ \
							{ \
							vtg.misc_outs |= 0x08; \
							na_misc_outs->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xf7; \
							na_misc_outs->np_piodata = vtg.misc_outs; \
							} \
						else if (x==2) /* Axis #2 */ \
							{ \
							vtg.misc_outs |= 0x10; \
							na_misc_outs->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xef; \
							na_misc_outs->np_piodata = vtg.misc_outs; \
							}
												

#define START_SERVO_HERE_TIMER \
		/* Start the servo here timer, timer3.  Set the period to the servo sample time */ \
		na_timer3->np_timercontrol = 8; /* Stop timer */ \
		loop_time = SAMPLE_TIME(axis_num); /* SAMPLE_TIME already in timer counts */ \
		halfword_ptr = (unsigned short int *)&loop_time; \
		na_timer3->np_timerperiodl = halfword_ptr[0]; \
		na_timer3->np_timerperiodh = halfword_ptr[1]; \
		na_timer3->np_timercontrol = 7 

#define STOP_SERVO_HERE_TIMER na_timer3->np_timercontrol = 8

// Since there's only one dither timer, we'll use DITHER_TIME(1) to set the freq. for both axes.
// We'll convert DITHER_TIME(1) into a half-period uS value.	
#define START_DITHER_TIMER \
		na_dither_timer->np_timercontrol = 8; \
		convert_uS_to_timer((unsigned int)(((1.0/(float)DITHER_TIME(1))/2.0)*1.0e6), &period); \
		na_dither_timer->np_timerperiodl = ((unsigned short *)&period)[0]; \
		na_dither_timer->np_timerperiodh = ((unsigned short *)&period)[1]; \
		na_dither_timer->np_timercontrol = 7 		
		
#define STOP_DITHER_TIMER na_dither_timer->np_timercontrol = 8; \
		vtg.dither_raw_val1 = 0; \
		vtg.dither_raw_val2 = 0

///////////////////////////////////////////////////////////////////////////////////////

// Define malloc memory blocks in case we go back to using malloc
#ifndef SAMPLE_MEMORY
	#define SAMPLE_MEMORY 0
#endif

#ifndef SCOPE_DATA
	#define SCOPE_DATA 1
#endif

#ifndef ANALOG_BLOCK_REPORT
	#define ANALOG_BLOCK_REPORT 2
#endif

// Two additional blocks are available for future use
//#ifndef ???????
//	#define ??????? 3
//#endif

//#ifndef ???????
//	#define ??????? 4
//#endif

// Macro to write the dac.  The value to write and the value to archive are separated because
// the value to write may contain offsets or dither settings that wouldn't need to be archived.
// ch_num is 1, 2, 3 or 4.
#define OUT_DAC(value_to_write, value_to_archive, ch_num, ctr, error) \
	ctr=0; \
	error=0; \
	if (value_to_write > 0x3fff) value_to_write = 0x3fff; \
	if (value_to_write < 0) value_to_write = 0; \
	while (!(na_dac_spi->np_spistatus & np_spistatus_trdy_mask)) \
		{ \
		ctr++; \
		if (ctr == 100000) {error = 1; break;} \
		} \
	if (!error) \
		{ \
		DI; \
		vtg.dac_word[ch_num] = value_to_archive; \
		value_to_write |= 0x4000; \
		na_dac_spi->np_spislaveselect = 1 << (ch_num-1); \
		na_dac_spi->np_spitxdata = value_to_write; \
		EI; \
		}


	
	
int set_clr_discrete_output(int output_num, int action);
int chk_discrete_input(int input_num, int *input_state);
int htoi(char *str);
int Atoi(char *string);
char *sgets(char *line, int size);
int convert_uS_to_timer(unsigned int time_uS, unsigned int *period);
//void memset(void *buf, int ch, int len);
//int convert_ana_ch(int sample_ctr, int ch_num, int result_type, int *result);
int convert_ana_ch(int ch_num, int result_type, int *result);
int convert_ana_ch_from_dynamic(int ch_num, int result_type, int *result);
void convert_upper_channels(volatile unsigned short *ptr_result, int result_type);
void convert_dyn_channels(volatile unsigned short *ptr_result, int result_type);
void convert_all_channels(volatile unsigned short *ptr_result, int result_type);
int get_dac(int ch_num, int result_type, int *result);
int out_dac(int ch_num, int value, int value_type, int abs_inc);
int flipper(unsigned int *val, int start_bit, int no_of_bits);
int inverter(unsigned int *val, int start_bit, int no_of_bits);
void display_count(void);
void seven_seg_interface(unsigned char msd, unsigned char lsd);
char *Itoa(int i, char* buf, int base);
long root(long x);
void init_Malloc(void);
void *Malloc(size_t size, int alloc_num);
void Free(int alloc_num);
void set_lvdt_offsets(void);


