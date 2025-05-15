//////////////////////// vtg_global1.h ///////////////////////////
//
// Copyright 2002,2003 Visi-Trak Worldwide


#define X vtg.x
#define CTR vtg.ctr
//#define ZSPEED vtg.zspeed

// Fixed variable definitions
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
	#define CONT_ACTIVE_MASK (1<<0)
	#define FATAL_ERROR_MASK (1<<1)
	#define WARNING_MASK (1<<2)
	#define PROGRAM_LOADED_MASK (1<<3)
	#define PROGRAM_RUNNING_MASK (1<<4)
#define MON_STATUS_WORD vtg.v[312] // Read only
	#define CS_DETECTED_MASK (1<<0)
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
	#define SEL_CH17_20_DYN (1<<16)
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
	#define AXIS1_DIV_BY_5 (1<<30)
	#define AXIS2_DIV_BY_5 (1<<31)

#define CONFIG_WORD2 vtg.v[314]
#define WARNING_WORD vtg.v[315]
#define FATAL_ERROR_WORD vtg.v[316]
// V[317] Not defined, available
#define TOTAL_STROKE_LEN vtg.v[318]
#define MIN_STROKE_LEN vtg.v[319]
#define VEL_SET_EOS vtg.v[320]
#define TIM_DEL_BISC vtg.v[321]
#define MIN_VEL_EOS vtg.v[322] // Velocity enable point
// vtg.v[323] - vtg.v[327] available
#define	TIM_COLL_INTERVAL vtg.v[328] //uS
#define TIM_COLL_SAMPLES vtg.v[329]
#define TIME_OUT_PERIOD vtg.v[330]

// vtg.v[331] - vtg.v[339] available

// Control parameters
#define VEL_LOOP_GAIN    vtg.v[340]
#define VEL_LOOP_BK_VEL1 vtg.v[341]
#define VEL_LOOP_BK_VEL2 vtg.v[342]
#define VEL_LOOP_BK_VEL3 vtg.v[343]
#define VEL_LOOP_BKGN1   vtg.v[344]
#define VEL_LOOP_BKGN2   vtg.v[345]
#define VEL_LOOP_BKGN3   vtg.v[346]
#define DITHER_TIME      vtg.v[347]
#define DITHER_AMPL      vtg.v[348]
#define POS_CHECK_ZSPEED vtg.v[349]
#define NULL_OFFSET      vtg.v[350]
#define LVDT_OFFSET      vtg.v[351]
#define JS_SETTING       vtg.v[352]
#define FT_SETTING		 vtg.v[353]
#define FT_STOP_POS		 vtg.v[354]
#define RETR_SETTING     vtg.v[355]
// vtg.v[356] available
#define SENSOR_DEVIATION vtg.v[357]
#define POS_LOOP_GAIN	 vtg.v[358]
#define POS_BK_GN_FE     vtg.v[359]
#define POS_BK_GN		 vtg.v[360]

// vtg.v[361] - vtg.v[363] available

#define VEL1_MUL vtg.v[364]
#define VEL2_MUL vtg.v[365]

// Limit switches
#define LS1_1 vtg.v[366]
#define LS2_1 vtg.v[367]
#define LS3_1 vtg.v[368]
#define LS4_1 vtg.v[369]
#define LS5_1 vtg.v[370]  
#define LS6_1 vtg.v[371]
#define LS1_2 vtg.v[372]
#define LS2_2 vtg.v[373]
#define LS3_2 vtg.v[374]
#define LS4_2 vtg.v[375]
#define LS5_2 vtg.v[376]
#define LS6_2 vtg.v[377]

// vtg.v[378] - vtg.v[389] available

#define O_SCOPE_TRIG_V vtg.v[390]
#define O_SCOPE_TRIG_CH vtg.v[391]
#define O_SCOPE_SAMPLE_INTERVAL_uS vtg.v[392]
#define ONE_MS_TIMER vtg.v[393]

// vtg.v[394] - vtg.v[511] available

// Control warnings
#define VAC_ABORT_POS_TOO_LONG 1
#define BLOCK_TIMEOUT 2
#define FILL_TEST_ABORT 3
#define MASTER_ENABLE_DROPPED_OUT 4
#define VAC_ABORT_ZSPEED 5
#define TEST_WARNING 10 // For test

// Control fatal errors
#define STACK_OVERFLOW 1
#define BLOCK_TIMEOUT 2
#define INVALID_BLOCK 3
#define BLOCK_EXECUTION_ERROR 4
#define TEST_FATAL_ERROR 10

// Fixed inputs
#define CYCLE_START 300
#define MASTER_ENABLE_INPUT 301
#define INPUT_303 303
#define INPUT_304 304
#define VAC_ON_INPUT 59
#define FILL_TEST 61

// Fixed outputs
#define WARNING_OUTPUT 43

// DAC channel used by control, normally 1 but can be 2, 3 or 4
#define CONT_DAC_CHANNEL 3

// Define command registers to talking to the AD nios
//#define AD_CMD1 ad_cmd[0]
#define AD_CMD1 shared_memory[31]
#define AD_CMD2 ad_cmd[1]

// Define vtg.gp_config_word bits
#define MASTER_ENABLE			vtg.gp_config_word & 0x00000001
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

#define SET_MASTER_ENABLE		vtg.gp_config_word |= 0x00000001
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

#define CLR_MASTER_ENABLE		vtg.gp_config_word &= 0xfffffffe
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


#define OSW1	vtg.osw1 // Opto based outputs
#define OSW2	vtg.osw2 // Opto based Ethernet outputs
#define OSW3	vtg.osw3 // Ethernet PLC outputs
#define ISW1	vtg.isw1 // Opto based inputs
#define ISW2	vtg.isw2 // Opto based Ethernet inputs
#define ISW3	vtg.isw3 // Ethernet PLC inputs
#define ISW4	misc_ins_pio->np_piodata // Fast on board inputs

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

#ifndef ABSOLUTE	
	#define ABSOLUTE 0
#endif

#ifndef INCREMENTAL	
	#define INCREMENTAL 1
#endif
	
#define DONT_CLEAR_SAVED_WARNING 0
#define CLEAR_SAVED_WARNING 1



// Items formerly in tiu2.h



int set_clr_discrete_output(int output_num, int action);
int chk_discrete_input(int input_num, int *input_state);
int htoi (char *str);
int convert_time_to_timer(unsigned int time_uS, unsigned short int *periodl, unsigned short int *periodh);
//int convert_ana_ch(int sample_ctr, int ch_num, int result_type, int *result);
int convert_ana_ch(int ch_num, int result_type, int *result);
int get_dac(int ch_num, int result_type, int *result);
int out_dac(int ch_num, int value, int value_type, int abs_inc);
