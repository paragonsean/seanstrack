// 							tiu1.h

//				Copyright 2001,2002 Visi-Trak Worldwide 

// 7-10-03 tiu1.h

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

#define NUM_OF_AXES 2

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



// The following define the misc output signals.  The following declaration
// must be made in any function using these definitions:
// np_pio *misc_out_pio = na_misc_outs;


#ifndef PULSE_PERIPHERAL_RESET
   #define PULSE_PERIPHERAL_RESET \
							vtg.misc_outs |= 0x01; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xfe; \
							misc_out_pio->np_piodata = vtg.misc_outs;
#endif						misc_out_pio->np_piodata = vtg.misc_outs;
							

// misc_outputs[3]
#ifndef PULSE_PRELOAD1
	#define PULSE_PRELOAD1 \
							vtg.misc_outs |= 0x08; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xf7; \
							misc_out_pio->np_piodata = vtg.misc_outs;
#endif


// misc_outputs[4]
#ifndef PULSE_PRELOAD2
	#define PULSE_PRELOAD2 \
							vtg.misc_outs |= 0x10; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xef; \
							misc_out_pio->np_piodata = vtg.misc_outs;
#endif


#ifndef PULSE_WATCHDOG_RESET
   #define PULSE_WATCHDOG_RESET \
							vtg.misc_outs |= 0x02; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xfd; \
							misc_out_pio->np_piodata = vtg.misc_outs;
#endif


#ifndef PULSE_WATCHDOG_FAULT
   #define PULSE_WATCHDOG_FAULT \
							vtg.misc_outs |= 0x20; \
							misc_out_pio->np_piodata = vtg.misc_outs; \
							vtg.misc_outs &= 0xdf; \
							misc_out_pio->np_piodata = vtg.misc_outs;
#endif	


#define SEL_CH13_16 \
							vtg_misc_outs &= 0xfb; \
							misc_out_pio->np_piodata = vtg.misc_outs;

#define SEL_CH17_20 \
							vtg_misc_outs |= 0x04; \
							misc_out_pio->np_piodata = vtg.misc_outs;


//misc_outputs[6]
#ifndef SET_PV1_DIV_BY_5
	#define SET_PV1_DIV_BY_5 \
								vtg.misc_outs |= 0x40; \
								misc_out_pio->np_piodata = vtg.misc_outs;
#endif

// misc_outputs[7]
#ifndef SET_PV2_DIV_BY_5
	#define SET_PV2_DIV_BY_5 \
								vtg.misc_outs |= 0x80; \
								misc_out_pio->np_piodata = vtg.misc_outs;
#endif

#ifndef CLR_PV1_DIV_BY_5
	#define CLR_PV1_DIV_BY_5 \
								vtg.misc_outs &= 0xbf; \
								misc_out_pio->np_piodata = vtg.misc_outs;
#endif

#ifndef CLR_PV2_DIV_BY_5
	#define CLR_PV2_DIV_BY_5 \
								vtg.misc_outs &= 0x7f; \
								misc_out_pio->np_piodata = vtg.misc_outs;
#endif





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
#define va_pv2 ((vp_pvu * ) 0x00005100)
//#define va_tu1 ((vp_timer_unit * ) 0x000004a0)


typedef volatile struct
{
vp_pvu *pvu; // Must assign this in main
unsigned int vel_ctr_freq;
unsigned int vel_multiplier;
unsigned int imask;
unsigned int vdiv;
unsigned int pload;
} axis_def;


// Declare a global axis array
axis_def axis[NUM_OF_AXES+1];  // Location 0 not used

// Combined pvu writes based on global axis. 
#define	WR_PVLS1 axis[axis_num].pvu->a0.wr_ls1
#define	WR_PVLS2 axis[axis_num].pvu->a1.wr_ls2
#define	WR_PVLS3 axis[axis_num].pvu->a2.wr_ls3
#define	WR_PVLS4 axis[axis_num].pvu->a3.wr_ls4
#define	WR_PVLS5 axis[axis_num].pvu->a4.wr_ls5
#define	WR_PVLS6 axis[axis_num].pvu->a5.wr_ls6
#define	WR_PV_GP_POS1 axis[axis_num].pvu->a6.wr_gp_pos1
#define	WR_PV_GP_POS2 axis[axis_num].pvu->a7.wr_gp_pos2
#define WR_PV_INT_ENBL axis[axis_num].pvu->a12.wr_int_enbl
#define WR_PV_IMASK axis[axis_num].pvu->a13.wr_imask
#define WR_PV_PRELOAD axis[axis_num].pvu->a14.wr_pos_preload
#define WR_PV_VEL_CLK_DIVISOR axis[axis_num].pvu->a15.wr_vel_clk_div

// Combined pvu reads based on global axis. 
#define RD_PV_POS axis[axis_num].pvu->a0.rd_current_pos
#define RD_PV_IPEND axis[axis_num].pvu->a1.rd_ipend
#define RD_PV_ISTATUS axis[axis_num].pvu->a2.rd_istatus

#define RD_PV_IMASK axis[axis_num].pvu->a3.rd_imask					// For diagnostics
#define RD_PV_INT_ENBL axis[axis_num].pvu->a4.rd_int_enbl			//	"	"	"	"
#define RD_PV_PLOAD axis[axis_num].pvu->a5.rd_pload					//	"	"	"	"
#define RD_PV_INT_SIG_VDIV axis[axis_num].pvu->a6.rd_int_sig_vdiv	//	"	"	"	"

#define RD_PV_LATCHED_Q1 axis[axis_num].pvu->a7.rd_latched_q1
#define RD_PV_LATCHED_Q2 axis[axis_num].pvu->a8.rd_latched_q2
#define RD_PV_LATCHED_Q3 axis[axis_num].pvu->a9.rd_latched_q3
#define RD_PV_LATCHED_Q4 axis[axis_num].pvu->a10.rd_latched_q4
#define RD_PV_VEL axis[axis_num].pvu->a11.rd_velocity
#define RD_PV_RAW_VEL_CTR axis[axis_num].pvu->a12.rd_vel_counter

// Combined pv items.  axis_num must be set to the correct axis number before using these.
#define ENBL_PV_INT  WR_PV_INT_ENBL = 1 
#define DIS_PV_INT	 WR_PV_INT_ENBL = 0   
#define SET_PV_IMASK(imask__) \
						axis[axis_num].imask=imask__; \
						WR_PV_IMASK=axis[axis_num].imask;

#define SET_PV_VCLKDIV(pv_div) \
						axis[axis_num].vdiv=(unsigned int)pv_div;\
						WR_PV_VEL_CLK_DIVISOR=axis[axis_num].vdiv;\
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
					


#define SET_PV_PLOAD(preload) \
						axis[axis_num].pload = preload; \
						WR_PV_PRELOAD = axis[axis_num].pload; \
						if(axis_num==1) {PULSE_PRELOAD1} \
						else if (axis_num==2) {PULSE_PRELOAD2}
					











