// 							tiu2.h

//				Copyright 2001,2002 Visi-Trak Worldwide 

// 7-10-03 tiu1.h

// 7-15-03 tiu2.h



#define NUM_OF_AXES 2





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


//misc_outputs[6] and misc_outputs[7] sel axis1 div by 5 or axis2 div by 5 respectively
#ifndef SET_PV_DIV_BY_5
	#define SET_PV_DIV_BY_5 \
								if (axis_num==1) {vtg.misc_outs |= 0x40; misc_out_pio->np_piodata = vtg.misc_outs;} \
								else if (axis_num==2) {vtg.misc_outs |= 0x80; misc_out_pio->np_piodata = vtg.misc_outs;}
#endif



#ifndef CLR_PV_DIV_BY_5
	#define CLR_PV_DIV_BY_5 \
								if (axis_num==1) {vtg.misc_outs &= 0xbf; misc_out_pio->np_piodata = vtg.misc_outs;} \
								else if (axis_num==2) {vtg.misc_outs &= 0x7f; misc_out_pio->np_piodata = vtg.misc_outs;}
#endif	

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
					


// misc_outputs[3] and misc_outputs[4] are used to pulse the preloads for axis1
// and axis2 respectively
#ifndef PULSE_PRELOAD
	#define PULSE_PRELOAD \
							if (axis_num==1) \
								{ \
								vtg.misc_outs |= 0x08; \
								misc_out_pio->np_piodata = vtg.misc_outs; \
								vtg.misc_outs &= 0xf7; \
								misc_out_pio->np_piodata = vtg.misc_outs; \
								} \
							else if (axis_num==2) \
								{ \
								vtg.misc_outs |= 0x10; \
								misc_out_pio->np_piodata = vtg.misc_outs; \
								vtg.misc_outs &= 0xef; \
								misc_out_pio->np_piodata = vtg.misc_outs; \
								}
#endif


#define SET_PV_PLOAD(preload) \
						axis[axis_num].pload = preload; \
						WR_PV_PRELOAD = axis[axis_num].pload; \
						PULSE_PRELOAD;
				









