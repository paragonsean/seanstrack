// 							tiu.h

//				Copyright 2001,2002 Visi-Trak Worldwide 


// prototypes
int initialize_pv1(void);
int initialize_pv2(void);
int initialize_timer_unit(void);

void pv1_isr(int context);
void pv2_isr(int context);
void timer_unit_isr(int context);




//PV1 interrupt subroutines
//inline void pv1_ls_cam1(void);
//inline void pv1_ls_cam2(void);
//inline void pv1_ls_cam3(void);
//inline void pv1_ls_cam4(void);
//inline void pv1_ls_cam5(void);
//inline void pv1_ls_cam6(void);
//inline void pv1_gp_pos_cam7(void);
//inline void pv1_gp_pos_cam8(void);
//inline void pv1_q1(void);
//inline void pv1_q2(void);
//inline void pv1_q3(void);
//inline void pv1_q4(void);

//PV2 interrupt subroutines
//inline void pv2_ls_cam1(void);
//inline void pv2_ls_cam2(void);
//inline void pv2_ls_cam3(void);
//inline void pv2_ls_cam4(void);
//inline void pv2_ls_cam5(void);
//inline void pv2_ls_cam6(void);
//inline void pv2_gp_pos_cam7(void);
//inline void pv2_gp_pos_cam8(void);
//inline void pv2_q1(void);
//inline void pv2_q2(void);
//inline void pv2_q3(void);
//inline void pv2_q4(void);

// Timer unit interrupt subroutines
//inline void tu1_cam1(void);
//inline void tu1_cam2(void);
//inline void tu1_cam3(void);
//inline void tu1_cam4(void);
//inline void tu1_cam5(void);
//inline void tu1_cam6(void);
//inline void tu1_cam7(void);



// Define PV1 writes
#define	WR_PV1LS1 pv1->a0.wr_ls1
#define	WR_PV1LS2 pv1->a1.wr_ls2
#define	WR_PV1LS3 pv1->a2.wr_ls3
#define	WR_PV1LS4 pv1->a3.wr_ls4
#define	WR_PV1LS5 pv1->a4.wr_ls5
#define	WR_PV1LS6 pv1->a5.wr_ls6
#define	WR_PV1_GP_POS1 pv1->a6.wr_gp_pos1
#define	WR_PV1_GP_POS2 pv1->a7.wr_gp_pos2
#define WR_PV1_INT_ENBL pv1->a12.wr_int_enbl
#define WR_PV1_IMASK pv1->a13.wr_imask
#define WR_PV1_PRELOAD pv1->a14.wr_pos_preload
#define WR_PV1_VEL_CLK_DIVISOR pv1->a15.wr_vel_clk_div

// Define PV2 writes
#define	WR_PV2LS1 pv2->a0.wr_ls1
#define	WR_PV2LS2 pv2->a1.wr_ls2
#define	WR_PV2LS3 pv2->a2.wr_ls3
#define	WR_PV2LS4 pv2->a3.wr_ls4
#define	WR_PV2LS5 pv2->a4.wr_ls5
#define	WR_PV2LS6 pv2->a5.wr_ls6
#define	WR_PV2_GP_POS1 pv2->a6.wr_gp_pos1
#define	WR_PV2_GP_POS2 pv2->a7.wr_gp_pos2
#define WR_PV2_INT_ENBL pv2->a12.wr_int_enbl
#define WR_PV2_IMASK pv2->a13.wr_imask
#define WR_PV2_PRELOAD pv2->a14.wr_pos_preload 
#define WR_PV2_VEL_CLK_DIVISOR pv2->a15.wr_vel_clk_div

// Define timer unit writes
#define WR_TIMER_UNIT_T1 tu1->a0.wr_time1 
#define WR_TIMER_UNIT_T2 tu1->a1.wr_time2
#define WR_TIMER_UNIT_T3 tu1->a2.wr_time3
#define WR_TIMER_UNIT_T4 tu1->a3.wr_time4
#define WR_TIMER_UNIT_T5 tu1->a4.wr_time5
#define WR_TIMER_UNIT_T6 tu1->a5.wr_time6
#define WR_TIMER_UNIT_T7 tu1->a6.wr_time7
#define WR_TIMER_UNIT_CWORD tu1->a7.wr_cword


// Define PV1 reads
#define RD_PV1_POS pv1->a0.rd_current_pos
#define RD_PV1_IPEND pv1->a1.rd_ipend
#define RD_PV1_ISTATUS pv1->a2.rd_istatus

#define RD_PV1_IMASK pv1->a3.rd_imask				// For diagnostics
#define RD_PV1_INT_ENBL pv1->a4.rd_int_enbl			//	"	"	"	"
#define RD_PV1_PLOAD pv1->a5.rd_pload				//	"	"	"	"
#define RD_PV1_INT_SIG_VDIV pv1->a6.rd_int_sig_vdiv	//	"	"	"	"

#define RD_PV1_LATCHED_Q1 pv1->a7.rd_latched_q1
#define RD_PV1_LATCHED_Q2 pv1->a8.rd_latched_q2
#define RD_PV1_LATCHED_Q3 pv1->a9.rd_latched_q3
#define RD_PV1_LATCHED_Q4 pv1->a10.rd_latched_q4
#define RD_PV1_VEL pv1->a11.rd_velocity
#define RD_PV1_RAW_VEL_CTR pv1->a12.rd_vel_counter

// Define PV2 reads
#define RD_PV2_POS pv2->a0.rd_current_pos
#define RD_PV2_IPEND pv2->a1.rd_ipend
#define RD_PV2_ISTATUS pv2->a2.rd_istatus

#define RD_PV2_IMASK pv2->a3.rd_imask				// For diagnostics
#define RD_PV2_INT_ENBL pv2->a4.rd_int_enbl			//	"	"	"	"
#define RD_PV2_PLOAD pv2->a5.rd_pload				//	"	"	"	"
#define RD_PV2_INT_SIG_VDIV pv2->a6.rd_int_sig_vdiv	//	"	"	"	"

#define RD_PV2_LATCHED_Q1 pv2->a7.rd_latched_q1
#define RD_PV2_LATCHED_Q2 pv2->a8.rd_latched_q2
#define RD_PV2_LATCHED_Q3 pv2->a9.rd_latched_q3
#define RD_PV2_LATCHED_Q4 pv2->a10.rd_latched_q4
#define RD_PV2_VEL pv2->a11.rd_velocity
#define RD_PV2_RAW_VEL_CTR pv2->a12.rd_vel_counter


// Define timer unit reads
#define RD_TIMER_UNIT_COUNT tu1->a0.rd_counter
#define RD_TIMER_UNIT_IPEND tu1->a1.rd_ipend
#define RD_TIMER_UNIT_ISTATUS tu1->a2.rd_istatus




#define ENBL_PV1_INT  WR_PV1_INT_ENBL = 1 

#define ENBL_PV2_INT  WR_PV2_INT_ENBL = 1	

#define ENBL_TIMER_UNIT_INT	vtg.tu1_cword |= 0x80000000; \
								WR_TIMER_UNIT_CWORD = vtg.tu1_cword;
						 

#define DIS_PV1_INT	  WR_PV1_INT_ENBL = 0   

#define DIS_PV2_INT	  WR_PV2_INT_ENBL = 0  
						

#define DIS_TIMER_UNIT_INT	vtg.tu1_cword &= 0x7fffffff; \
								WR_TIMER_UNIT_CWORD = vtg.tu1_cword;
						 

#define SET_PV1_IMASK(imask) \
						vtg.pv1_imask = imask; \
						WR_PV1_IMASK = vtg.pv1_imask

#define SET_PV2_IMASK(imask) \
						vtg.pv2_imask = imask; \
						WR_PV2_IMASK = vtg.pv2_imask

#define SET_TIMER_UNIT_IMASK(tu_imask) \
						vtg.tu1_cword &= 0xffffff80; \
						vtg.tu1_cword |= tu_imask; \
						WR_TIMER_UNIT_CWORD = vtg.tu1_cword;



#define SET_PV1_VCLKDIV(pv1_div) \
						vtg.pv1_vdiv = pv1_div; \
						WR_PV1_VEL_CLK_DIVISOR = vtg.pv1_vdiv;

#define SET_PV2_VCLKDIV(pv2_div) \
						vtg.pv2_vdiv = pv2_div; \
						WR_PV2_VEL_CLK_DIVISOR = vtg.pv2_vdiv;

#define SET_PV1_PLOAD(preload1) \
						vtg.pv1_pload = preload1; \
						WR_PV1_PRELOAD = vtg.pv1_pload;
					
#define SET_PV2_PLOAD(preload2) \
						vtg.pv2_pload = preload2; \
						WR_PV2_PRELOAD = vtg.pv2_pload;


// The following define the misc output signals.  The following declaration
// must be made in any function using these definitions:
// np_pio *misc_out_pio = na_misc_outs;

// PULSE_PERIPHERAL_RESET is defined near the bottom of this file

//#define PULSE_PERIPHERAL_RESET \
//							vtg.misc_outs |= 0x01; \
//							misc_out_pio->np_piodata = vtg.misc_outs; \
//							vtg.misc_outs &= 0xfe; \
//							misc_out_pio->np_piodata = vtg.misc_outs;
							

#define PULSE_PRELOAD1 \
							vtg.misc_outs |= 0x08; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xf7; \
							misc_out_pio->np_piodata = vtg.misc_outs;

#define PULSE_PRELOAD2 \
							vtg.misc_outs |= 0x10; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xef; \
							misc_out_pio->np_piodata = vtg.misc_outs;

#define PULSE_WATCHDOG_RESET \
							vtg.misc_outs |= 0x02; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xfd; \
							misc_out_pio->np_piodata = vtg.misc_outs;

#define PULSE_WATCHDOG_SET \
							vtg.misc_outs |= 0x20; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xdf; \
							misc_out_pio->np_piodata = vtg.misc_outs;

#define SEL_CH13_16 \
							vtg_misc_outs &= 0xfb; \
							misc_out_pio->np_piodata = vtg.misc_outs;

#define SEL_CH17_20 \
							vtg_misc_outs |= 0x04; \
							misc_out_pio->np_piodata = vtg.misc_outs;







// Define bits

// PV unit istatus and ipend words are as follows:
//
// LSB	ls1_match	:1
//		ls2_match   :1
//		ls3_match	:1
//		ls4_match	:1
//		ls5_match	:1
//		ls6_match	:1
//		gp_pos1_match :1
//		gp_pos2_match :1
//		quadrature_q1 :1
//		quadrature_q2 :1
//		quadrature_q3 :1
//		quadrature_q4 :1
// MSB	Bits 12 - 31 not used :20



// Timer unit control word
//
// LSB	imask :7
//		Bits 8 - 30 Not Used  :24
// MSB	int_enbl :1

// Timer istatus and ipend words are as follows:
//
// LSB	timer1_match	:1
//		timer2_match	:1 
//		timer3_match	:1
//		timer4_match	:1
//		timer5_match	:1
//		timer6_match	:1
//		timer7_match	:1
// MSB	Bit7 - Bit31 not used	:25



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
unsigned int wr_not_used;
unsigned int rd_latched_q2;
} pvu_a8;

typedef volatile union
{
unsigned int wr_not_used;
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



typedef volatile union
{
unsigned int wr_time1;
unsigned int rd_counter;
} tu_a0;

typedef volatile union
{
unsigned int wr_time2;
unsigned int rd_ipend;
} tu_a1;

typedef volatile union
{
unsigned int wr_time3;
unsigned int rd_istatus;
} tu_a2;

typedef volatile union
{
unsigned int wr_time4;
unsigned int rd_not_used;
} tu_a3;

typedef volatile union
{
unsigned int wr_time5;
unsigned int rd_not_used;
} tu_a4;

typedef volatile union
{
unsigned int wr_time6;
unsigned int rd_not_used;
} tu_a5;

typedef volatile union
{
unsigned int wr_time7;
unsigned int rd_not_used;
} tu_a6;

typedef volatile union
{
unsigned int wr_cword;
unsigned int rd_not_used;
} tu_a7;


// Main Timer Unit registers
typedef volatile struct
{
tu_a0 a0;
tu_a1 a1;
tu_a2 a2;
tu_a3 a3;
tu_a4 a4;
tu_a5 a5;
tu_a6 a6;
tu_a7 a7;
} vp_timer_unit;


// Rename nios.h generated address definition for the tiu.
#define va_pv1 ((vp_pvu * ) 0x00004d40)
//#define va_pv2 ((vp_pvu * ) 0x00000580)
//#define va_tu1 ((vp_timer_unit * ) 0x000004a0)




/////////////// Shared memory definitions /////////////////
//
// 0 - 9 -> ana_ch_setup_words structure, 10 words
// 10 - 13 -> pos_based_ana_ch_setup_words structure, 4 words
// 14 - 15 -> Value to write DAC channels 1 - 4, 4 half-words, 2 words
// 16 -> Junk location
// 17 -> Configuration word
// 18 -> Position data array address in main memory
// 19 -> Timed data array address in main memory
// 20 -> One ms counter
// 21 -> OSW1
// 22 -> OSW2
// 23 -> OSW3
// 24 -> ISW1
// 25 -> ISW2
// 26 -> ISW3
// 27 -> Limit switch service word (istatus from pv1, upper 2 bits masked)
// 28 - 34 -> Monitor setup array, 7 words
// 35 - 36 -> Computed shat parameters array, 2 words 
// 37 - 38 -> Not defined
// 39 -> Computed shot parameters array address in main memory
// 40 - 53   -> pos_based_sample, 14 words
// 54 - 67   -> pos_based_sample, 14 words
// 68 - 81   -> pos_based_sample, 14 words
// 82 - 95   -> pos_based_sample, 14 words
// 96 - 109  -> pos_based_sample, 14 words
// 110 - 122 -> tim_based_cont_conv_sample structure, 13 words
// 123 -> Timed collection interval counter
// 124 -> Number of timed collection samples to assume zspeed
// 125 -> Command from main processor
// 126 -> Data from main processor
// 127 -> Data from monitor processor





#define AD_SETUP_WORD_ARRAY_TIM_BASED shared_memory[0]
#define AD_SETUP_WORD_ARRAY_POS_BASED shared_memory[10]
#define DAC_WORD_ARRAY shared_memory[14]  // Words to send to DAC
#define JUNK	shared_memory[16]
#define CONFIG_WORD shared_memory[17]
#define MAIN_MEM_POS_ARRAY_LOCATION shared_memory[18]
#define MAIN_MEM_TIM_ARRAY_LOCATION shared_memory[19]
#define ONE_MS_COUNTER shared_memory[20]
#define OSW1		shared_memory[21] // Opto based outputs
#define OSW2		shared_memory[22] // Opto based Ethernet outputs
#define OSW3		shared_memory[23] // Ethernet PLC outputs
#define ISW1		shared_memory[24] // Opto based inputs
#define ISW2		shared_memory[25] // Opto based Ethernet inputs
#define ISW3		shared_memory[26] // Ethernet PLC inputs
#define LS_TO_SERVICE		shared_memory[27]
#define MON_SETUP_ARRAY		shared_memory[28] // 7 words
#define COMP_SHOT_PAR_ARRAY	shared_memory[35] // 2 words
#define MAIN_MEM_COMP_SHOT_DATA_ARRAY_LOCATION shared_memory[39]
#define SHARED_MEM_POS_DATA_BUFFER shared_memory[40]
#define SHARED_MEM_TIM_DATA_BUFFER shared_memory[110]
#define	TIMED_COLL_INTERVAL_COUNTER shared_memory[123]
#define COUNTS_TO_ASSUME_ZSPEED shared_memory[124]
#define COMMAND_FROM_MAIN	shared_memory[125]
#define DATA_FROM_MAIN		shared_memory[126]
#define DATA_FROM_MON		shared_memory[127]




// Define CONFIG_WORD bits
#define MASTER_ENABLE			CONFIG_WORD & 0x00000001
#define SHOT_IN_PROGRESS		CONFIG_WORD & 0x00000002
#define CURRENTLY_TIMING		CONFIG_WORD & 0x00000004
#define CLEAR_1MS_COUNTER		CONFIG_WORD & 0x00000008
#define CS_DETECTED				CONFIG_WORD & 0x00000010
#define PAST_MIN_STK_LEN		CONFIG_WORD & 0x00000020
#define ZSPEED					CONFIG_WORD & 0x00000040
#define SHOT_COMPLETE			CONFIG_WORD & 0x00000080
#define REQ_TO_BUFFER_DATA		CONFIG_WORD & 0x00000100
#define ACK_REQ_TO_BUFFER_DATA	CONFIG_WORD & 0x00000200
#define CONV_HI_CHANNELS		CONFIG_WORD & 0x00000400
#define MONITOR_ENABLE			CONFIG_WORD & 0x00000800
#define PAST_MIN_VEL_EOS		CONFIG_WORD & 0x00001000

#define SET_MASTER_ENABLE		CONFIG_WORD |= 0x00000001
#define SET_SHOT_IN_PROGRESS	CONFIG_WORD |= 0x00000002
#define SET_CURRENTLY_TIMING	CONFIG_WORD |= 0x00000004
#define SET_CLEAR_1MS_COUNTER	CONFIG_WORD |= 0x00000008
#define SET_CS_DETECTED			CONFIG_WORD |= 0x00000010
#define SET_PAST_MIN_STK_LEN	CONFIG_WORD |= 0x00000020
#define SET_ZSPEED				CONFIG_WORD |= 0x00000040
#define SET_SHOT_COMPLETE		CONFIG_WORD |= 0x00000080
#define SET_REQ_TO_BUFFER_DATA		CONFIG_WORD |= 0x00000100
#define SET_ACK_REQ_TO_BUFFER_DATA	CONFIG_WORD |= 0x00000200
#define SET_CONV_HI_CHANNELS		CONFIG_WORD |= 0x00000400
#define SET_MONITOR_ENABLE			CONFIG_WORD |= 0x00000800
#define SET_PAST_MIN_VEL_EOS		CONFIG_WORD |= 0x00001000

#define CLR_MASTER_ENABLE		CONFIG_WORD &= 0xfffffffe
#define CLR_SHOT_IN_PROGRESS	CONFIG_WORD &= 0xfffffffd
#define CLR_CURRENTLY_TIMING	CONFIG_WORD &= 0xfffffffb
#define CLR_CLEAR_1MS_COUNTER	CONFIG_WORD &= 0xfffffff7
#define CLR_CS_DETECTED			CONFIG_WORD &= 0xffffffef
#define CLR_PAST_MIN_STK_LEN	CONFIG_WORD &= 0xffffffdf
#define CLR_ZSPEED				CONFIG_WORD &= 0xffffffbf
#define CLR_SHOT_COMPLETE		CONFIG_WORD &= 0xffffff7f
#define CLR_REQ_TO_BUFFER_DATA		CONFIG_WORD &= 0xfffffeff
#define CLR_ACK_REQ_TO_BUFFER_DATA	CONFIG_WORD &= 0xfffffdff
#define CLR_CONV_HI_CHANNELS		CONFIG_WORD &= 0xfffffbff
#define CLR_MONITOR_ENABLE			CONFIG_WORD &= 0xfffff7ff
#define CLR_PAST_MIN_VEL_EOS		CONFIG_WORD &= 0xffffefff


#define PULSE_PERIPHERAL_RESET \
							vtg.misc_outs |= 0x01; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xfe; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							SET_CLEAR_1MS_COUNTER;


////////////////// Monitor data structures ////////////////////
typedef volatile struct
	{
	// 56 bytes, 14 words
	unsigned short int ana_ch1;
	unsigned short int ana_ch2;
	unsigned short int ana_ch3;
	unsigned short int ana_ch4;
	unsigned short int ana_ch5;
	unsigned short int ana_ch6;
	unsigned short int ana_ch7;
	unsigned short int ana_ch8;
	unsigned int vel_count_q1;
	unsigned int vel_count_q2;
	unsigned int vel_count_q3;
	unsigned int vel_count_q4;
	unsigned int isw1;
	//unsigned int isw2;
	//unsigned int isw3;  // Ethernet based inputs
	unsigned int isw4;
	unsigned int osw1;
	//unsigned int osw2;
	//unsigned int osw3;  // Ethernet based outputs
	unsigned int one_ms_timer;
	int position;
	int sample_num;  // For buffering
	} pos_based_sample;


typedef volatile struct
	{
	// 52 bytes, 13 words
	unsigned short int ana_ch1_timed;
	unsigned short int ana_ch2_timed;
	unsigned short int ana_ch3_timed;
	unsigned short int ana_ch4_timed;
	unsigned short int ana_ch5_timed;
	unsigned short int ana_ch6_timed;
	unsigned short int ana_ch7_timed;
	unsigned short int ana_ch8_timed;
	unsigned int isw1_timed;
	//unsigned int isw2_timed;
	//unsigned int isw3_timed;  // Ethernet based inputs
	unsigned int isw4_timed;
	unsigned int osw1_timed;
	//unsigned int osw2_timed;
	//unsigned int osw3_timed;  // Ethernet based outputs
	int pos_timed;
	int sample_num; 
	} tim_based_sample;


// This is a one size fits all data sample used for both
// pos and time based samples.  The astute will notice that it is that same
// as the pos based sample!!!  How about that.
typedef volatile struct
	{
	// 56 bytes, 14 words
	unsigned short int ana_ch1;
	unsigned short int ana_ch2;
	unsigned short int ana_ch3;
	unsigned short int ana_ch4;
	unsigned short int ana_ch5;
	unsigned short int ana_ch6;
	unsigned short int ana_ch7;
	unsigned short int ana_ch8;
	unsigned int vel_count_q1;
	unsigned int vel_count_q2;
	unsigned int vel_count_q3;
	unsigned int vel_count_q4;
	unsigned int isw1;
	//unsigned int isw2;
	//unsigned int isw3;  // Ethernet based inputs
	unsigned int isw4;
	unsigned int osw1;
	//unsigned int osw2;
	//unsigned int osw3;  // Ethernet based outputs
	unsigned int one_ms_timer;
	int position;
	int sample_num;  // For buffering
	} combined_sample;  




typedef volatile struct
	{
	// Computed shot parameters array
	unsigned int cycle_time;
	unsigned short int biscuit_size;
	short int eos_pos;
	} comp_shot_data;

//////////////// shared_memory data structures //////////////////
#if 0
typedef volatile struct
	{
	// 56 bytes, 14 words
	unsigned short int ana_ch1;
	unsigned short int ana_ch2;
	unsigned short int ana_ch3;
	unsigned short int ana_ch4;
	unsigned short int ana_ch5;
	unsigned short int ana_ch6;
	unsigned short int ana_ch7;
	unsigned short int ana_ch8;
	unsigned int vel_count_q1;
	unsigned int vel_count_q2;
	unsigned int vel_count_q3;
	unsigned int vel_count_q4;
	unsigned int isw1;
	//unsigned int isw2;
	//unsigned int isw3;  // Ethernet based inputs
	unsigned int isw4;
	unsigned int osw1;
	//unsigned int osw2;
	//unsigned int osw3;  // Ethernet based outputs
	unsigned int one_ms_timer;
	int position;
	int sample_num;  // For buffering
	} buffered_pos_based_sample;


typedef volatile struct
	{
	// 60 bytes, 15 words
	unsigned short int ana_ch1_timed;
	unsigned short int ana_ch2_timed;
	unsigned short int ana_ch3_timed;
	unsigned short int ana_ch4_timed;
	unsigned short int ana_ch5_timed;
	unsigned short int ana_ch6_timed;
	unsigned short int ana_ch7_timed;
	unsigned short int ana_ch8_timed;
	unsigned short int ana_ch9_timed;
	unsigned short int ana_ch10_timed;
	unsigned short int ana_ch11_timed;
	unsigned short int ana_ch12_timed;
	unsigned short int ana_ch13_timed;
	unsigned short int ana_ch14_timed;
	unsigned short int ana_ch15_timed;
	unsigned short int ana_ch16_timed;
	unsigned short int ana_ch17_timed;
	unsigned short int ana_ch18_timed;
	unsigned short int ana_ch19_timed;
	unsigned short int ana_ch20_timed;
	unsigned int isw1_timed;
	//unsigned int isw2_timed;
	//unsigned int isw3_timed;  // Ethernet based inputs
	unsigned int isw4_timed;
	unsigned int osw1_timed;
	//unsigned int osw2_timed;
	//unsigned int osw3_timed;  // Ethernet based outputs
	int pos_timed;
	int sample_num; 
	} buffered_tim_based_sample;
#endif

typedef volatile struct
	{
	// Monitor setup items, 28 bytes, 7 words
	int total_stk_len;   
	int min_stk_len;
	int vel_eos_enable;   
	int vel_eos;	    
	int one_ms_counts; 
	unsigned int timed_coll_interval;
    unsigned int time_delay_biscuit;	
	} shared_mon_setup;

typedef volatile struct
	{
	// Computed shot parameters, 8 bytes, 2 words
	// Items computed by monitor processor and saved in shared memory
	unsigned short int biscuit_size;
	short int eos_pos;
	} shared_comp_shot_data;

