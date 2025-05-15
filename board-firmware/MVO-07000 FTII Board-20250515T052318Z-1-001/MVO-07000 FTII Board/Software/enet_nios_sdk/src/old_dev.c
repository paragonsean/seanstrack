//     emc_1_0.c / dev.c
//  Development source file

//  4-1-03   enet_mon_cont4.c
//
//  This is a modified version of Altera's germs_server.c for the Nios processor.
//  This version only supports TCP communications.  UDP and serial I/O is not supported.
//  This version, as the name implies, plays the part of a server.  Therefore it will
//  wait for a client to make a connection.

// 5-5-03  enet_mon_cont5.c

// 5-13-03 enet_mon_cont6.c

// 6-5-03 enet_mon_cont7.c

// 6-24-03 enet_mon_cont8.c  First version for single processor enet_mon_cont.hexout.flash

// 7-2-03 enet_mon_cont9.c Moved monitor and control supervisors to seperate functions.

// 7-8-03 enet_mon_cont10.c Seperated the pos and time based isrs.  Created a seperate function
// for the pos / time based A/D loop.  This version works with control_blocks4.c and .h
// and tiu.h

// 7-10-03 enet_mon_cont11.c  This version works with control_blocks5.c and .h and tiu1.h

// 7-14-03 enet_mon_cont12.c  This supports 2 position axes. This version works with 
// control_blocks6.c and .h and vtw_global2.h.  The tiu?.h items have been included into
// vtw_global2.h.
// This version supports 2 transducer axes.

// 8-1-03 enet_mon_cont14.c

// 8-18-03 enet_mon_cont14a.c   Added new tcp_xmit routine
//		   enet_mon_cont14b.c	Added fake shot capability by pressing SW7	
//		   enet_mon_cont14c.c   Send 1350 bytes of garbage when SW9 is pressed and
//								fake shots when SW7 is pressed

// 8-22-03 enet_mon_cont15.c  Removed Ethernet interrupt, Ethernet now polled.

// 8-22-03 enet_mon_cont16.c  Made buildable for monitor only or monitor w/control, polled ethernet
// 8-28-03 enet_mon_cont16a.c Interrupt driven Ethernet

// 8-29-03 enet_mon_cont17.c  Interrupt driven Ethernet
// 9-2-03  enet_mon_cont17a.c	Polled Ethernet version

// 9-22-03 enet_mon_cont18.c  Allow selecting/deselecting polled Ethernet with the preprocessor
//	Allow enabling/disabling prompt.  This code works with the new prog. div. by function
//  for both axis #1 and #2.

// 9-29-03 emc_1_0.c  1st. production version 1.0
// Changed fake shot generator to slow it down somewhat.

/* To debug with gdb:

	1, Build w/ the following command: nios-build -b 0x400000 -d=com1 enet_mon_cont??.c

	2,	Plug serial cable into the board's first port.

	3,	At command line, type enet_mon_cont??.gdb, the gdb stub will be downloaded.

	4,	When the debugger comes up, switch the serial cable to the board's 2nd. port and 
		click on run.  Select serial, 56K, com1, the program will be downloaded.

	5,	Set a breakpoint if desired, otherwise click on continue to execute the program from
		within the debugger.

This assumes 2 serial ports on the board.  Port 1 is set for 115K and is the main port.  
Port 2 is set to 56K and is set as the debug port within the nios setup.
Eventually need to use both com1 and com2 on the PC along with 2 serial cables.  But for now,
this is the procedure.
*/

#include "nios.h"
#include "plugs.h"
#include "ethernet_mon_cont_peripherals1.h"
#include "vtw_global3.h"
#include "plugs_example_designs.h"
#include "control_program_blocks8.h"

#define TCP_CHUNK_SIZE 1350

#define CONTROL_COMPILE_DEBUG 0
#define CONTROL_INTERRUPT_DEBUG 0
#define MON_DEBUG 0
#define SCI_DEBUG 0

#define CHECK_NULL 0
#define AUTO_NULL 1

// +-----------------------------------------------
// | Various constants to control the monitor
// |

//#define BUILD_FOR_ETHERNET 1
#define BUILD_FOR_CONTROL 1

// Must change chip design for interrupt driven Ethernet, can't just select it here.
#define BUILD_FOR_POLLED_ETHERNET 1

#ifndef GM_FlashBase
 #ifdef nasys_main_flash
 #define GM_FlashBase nasys_main_flash
 #endif
#endif

#ifdef GM_FlashBase

 #ifndef GM_FlashTop
  #ifdef nasys_main_flash_end
   #define GM_FlashTop nasys_main_flash_end
  #else
   #define GM_FlashTop (GM_FlashBase+0x100000)
  #endif
 #endif

 #ifndef GM_FlashExec
  #define GM_FlashExec (GM_FlashBase+0x4000C)
 #endif

#endif

#ifndef NULL
 #define NULL 0
#endif


#define LOCAL_IS_CURRENT 1
#define PLANT_WIDE_IS_CURRENT 2


// Flash addresses for saving vtw items
#define VAR_CPROG_FLASH_SECTOR_ADDR 0x108000
#define VAR_FLASH_ADDR 0x108000
#define CPROG_FLASH_ADDR 0x109000

typedef int (*send_proc)(void *context, char *string);
typedef int (*recv_proc)(void *context, char *string);
typedef int (*init_proc)(void *context);
typedef int (*perd_proc)(void *context);

typedef unsigned char uc;
typedef unsigned int ui;
typedef unsigned long ul;
typedef unsigned short us;


#define k_default_show_range 64
#define k_halfwords_per_line 8
#define k_input_string_length 512

// ethernet defines
#define nk_settings_count 4
#define nk_flash_settings ((ns_plugs_network_settings *)(((char *)nasys_main_flash)+0x6000))



// +---------------------------------
// | This "globals" structure is used
// | to hold most of the state
// | of the germs monitor, most especially
// | the text of the command itself,
// | and the pointers to the callback
// | proc for sending text
// |
// | It's a little inconsistent, some
// | parameters are passed to each
// | routine, and others are here in
// | this global state. Sorry for
// | any confusion -- dvb 2001.
// |

typedef struct
	{
	uc even_byte; // for stash-byte routine to accumulate halfwords
	ui memory_position; // next memory to show in response to <return>
	ui memory_range; // how many bytes to show in response to <return>
	ui relocation_offset; // as set by the 'r' command
	ul ihex_upper_address; // upper 16 bits of address for ihex (':') fmt data

	char b[1024]; // print buffer, used for all sprintf's

	send_proc sp; // callback proc to send text back
	volatile void * spc; // context for callback proc

	recv_proc rp; // callback proc to receive text
 	volatile void * rpc; // context for callback proc

	init_proc ip;
	volatile void * ipc;

	perd_proc t;	
	volatile void * tc;

	volatile uc *command; // holds the command we're parsing

	uc checksum; // used only for S-Records

	char help_menu[1000];
	} s_c_germs_globals;

static s_c_germs_globals g; // bss inited to = {0};


#define RECV_BUFF_LENGTH 1024

typedef volatile struct
	{
	int tcp_plug;
	int connected; // 0:waiting, 1:connected, 2:disconnected
	net_32 remote_ip_address;
	net_16 remote_port;
	unsigned char c[RECV_BUFF_LENGTH]; // receive buffer, used to gather received bytes
	int show_prompt;
	//volatile unsigned char *head_c;
	//volatile unsigned char *tail_c;
	} s_tcp_status;

// bss inited to = {0};
static s_tcp_status local_net_settings, plant_net_settings, io_net_settings;


////////////////////// VTW Data Structures ////////////////////////////////
#define VAR_DEC 0
#define VAR_HEX 1
#define MAX_POS_BASED_SAMPLE 1500


									
typedef struct
{
int v[MAX_VAR+1]; // Variables
unsigned int control_program[MAX_STEP+1][10]; // Normally 204 blocks, 10 words per block;
unsigned int one_ms_counter;
int control_supervisor;
unsigned char var_dec_hex; // Default to decimal
unsigned char misc_outs;
unsigned int isw1;
unsigned int isw2;
unsigned int isw3;
unsigned int osw1;
unsigned int osw2;
unsigned int osw3;
unsigned char first_cs_after_power_up;
unsigned short int dac_word[5]; // For storing current dac value.
unsigned int stack[20];
unsigned char stack_ptr;
unsigned int prog_ctr;
int timer2_timeout;
//int warning;
int saved_warning;
//int fatal_error;
//unsigned int slow_loop_time; // 25mS
//unsigned int fast_loop_time; // 2.5mS
int local_wants_op_packets;
int plant_wide_wants_op_packets;
int local_o_scope_mode;
int plant_wide_o_scope_mode;
int o_scope_in_progress;
int dac_write_in_progress; // To prevent DAC recursion
int tcp_xmit_in_progress;  // To prevent TCP transmit recursion
unsigned int last_pb_time;
//unsigned int gp_config_word;
int op_packet_size;
int o_scope_packet_size;
int counts_to_assume_zspeed;
unsigned int blk_timeout_time;
int io_int1;
int on_off1;
int io_int2;
int on_off2;
unsigned char cont_int_out_status; // Status of the control interrupt output bits
int tst_ctr; // For test
unsigned int junk;
} vtw_globals;

static vtw_globals vtg;


// Operational data packet structure, define as a global structure for run time speed.
struct
{
char header[12]; // "O_BEGIN\n"  
int dac[4]; // DAC voltage * 1000
unsigned short int analog[12]; // Ch 1-8 and 17-20.
int pos;
int vel;
unsigned int isw1;
unsigned int isw4;
unsigned int osw1;
unsigned int mon_status_word1;
unsigned int status_word1;			
unsigned int config_word1;
int warning;
int fatal_error;
int blk_no; // Current control program step
} op_data_packet; 


//typedef volatile struct
//{
//short int ch_17_20[4];
//} scope_data_sample;


typedef volatile struct
{
char header[12];
short int ch_17_20[2000][4];
} scope_data;

scope_data scope_data_packet; // Enable this for fixed array in memory
// scope_data *scope_data_packet; // Enable this for a pointer.


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

combined_sample data_sample[3501];


// Use the data sample array space for the o-scope data packet since the board should not
// be in o-scope mode during a shot.
//scope_data *scope_data_packet = (scope_data *)&data_sample;



// Monitor globals
unsigned char bounce_back;
unsigned char ls_stat[6];
short unsigned int pb_ctr, tb_ctr, sample_ctr, bisc_ctr, tb_time_interval_ctr; // Position and time based array counters.
unsigned int prev_osw1=0;
unsigned int timed_coll_interval; // Working copy
int vel_eos_enable; // Working copy
int vel_eos;  // Working copy



struct
	{
	unsigned int cycle_time;
	unsigned short int biscuit_size;
	short int eos_pos;
	} comp_shot_parameters;


typedef volatile struct
		{
		unsigned short int ch1;
		unsigned short int ch2;
		unsigned short int ch3;
		unsigned short int ch4;
		unsigned short int ch5;
		unsigned short int ch6;
		unsigned short int ch7;
		unsigned short int ch8;
		unsigned short int ch9;
		unsigned short int ch10;
		unsigned short int ch11;
		unsigned short int ch12;
		unsigned short int ch13;
		unsigned short int ch14;
		unsigned short int ch15;
		unsigned short int ch16;
		unsigned short int ch17;
		unsigned short int ch18;
		unsigned short int ch19;
		unsigned short int ch20;
		} ad_setups;

ad_setups ad_setup_words;
volatile unsigned short int *shared_memory = (unsigned short int *)0x2000; // 32 halfwords
//volatile unsigned short int *ad_setup = (unsigned short int *)0x2100; // 32 halfwords
volatile unsigned short int *ad_cmd = (unsigned short int *)0x2200; // 2 halfwords

// +--------------------------------
// | Local Prototypes


static char *r_get_monitor_string(void);
static int r_get_cpu_id(void);
static ui r_hex_char_to_value(uc c);
static void r_show_range(ui addr_lo,ui addr_hi);
static int r_in_flash_range(ui addr);
static ui r_fetch_next_byte(uc **wp);
static int r_stash_byte(ui addr,uc value);
static int r_input_string(char *input_string);
static int r_get_data(char *data);
static void r_scan_string_for_addr_range(uc **wp,uc *break_char_out,ui *addr_lo_out,ui *addr_hi_out);
static ui r_scan_string_for_number(uc **wp, uc *break_char_out);
int send_string(void *context, char *s);
int buffGetData(s_tcp_status * context, char * value);
int readBuffer(s_tcp_status *u, char *buff);
//void loadBuffer(s_tcp_status *u, char * payload, int payload_length);
void loadBuffer(void *context, char * payload, int payload_length);

int uart_send_byte(void *context, char *c);
int uart_recv_byte(void *context, char *c);
int null_function(void *context);

int udp_send_byte(void *context, char *c);
int udp_recv_byte(void *context, char *c);
int udp_init(void *context);

int tcp_listen_proc(int plug_handle, void *context, host_32 remote_ip_address, host_16 remote_port);
int tcp_send_byte(void *context, char *c);
int tcp_recv_byte(void *context, char *c);
int tcp_init(void);
int tcp_proc(int plug_handle, void *context, ns_plugs_packet *p, void *payload, int payload_length);
int tcp_listen_proc(int plug_handle, void *context, host_32 remote_ip_address, host_16 remote_port);
int tcp_xmit(int plug_handle, void *data, int length);
//int tcp_check(void *context);

// Visi-Trak Worldwide function prototypes
int parse_control_program_command(uc *orig_cmd_str);
int parse_variable_command(unsigned char *orig_cmd_str);
int parse_operational_command(unsigned char *orig_cmd_str);
void save_cprog_to_flash(void);
void save_vars_to_flash(void);
void reload_vars_from_flash(void);
void reload_cprog_from_flash(void);
void transmit_monitor_data(unsigned short int shot_num);
void transmit_binary_data(unsigned short int shot_num);
void reset_control_program(int saved_warning_action);
void stop_control(void);
int whos_the_current_plug(void);
inline void control_supervisor(int master_enable_was_off, unsigned int *print_again);
inline void monitor_supervisor(volatile unsigned short int *shot_num);
void update_control_parameters(void);
void ram_clear(void);
//int convert_monitored_channels(int sample_ctr);

#include "tb1.c"


// +--------------------------------
// | Local Little Routines
#ifdef __nios32__
static void r_asm_go(ui addr)
	{
	asm("
		.equ __nios32__,1
		MOV	%g0,%i0
		LSRI	%g0,1
		NOP
		CALL	%g0
		NOP

		PFX	%hi(_start@h)
		MOVI	%g0,%lo(_start@h)
		PFX	%xhi(_start@h)
		MOVHI	%g0,%xlo(_start@h)

		JMP	%g0
		NOP
	");
	}
#endif
#ifdef __nios16__
static void r_asm_go(ui addr)
	{
	asm("
		.equ __nios32__,1
		MOV	%g0,%i0
		LSRI	%g0,1
		NOP
		CALL	%g0
		NOP

		PFX	%hi(_start@h)
		MOVI	%g0,%lo(_start@h)

		JMP	%g0
		NOP
	");
	}
#endif

static char *r_get_monitor_string(void)
	{
		return "VTW Monitor and Control server";
	}


static int r_get_cpu_id(void)
	{
	asm("
		PFX	6
		RDCTL	%i0
	");
	}


// Global interrupt disable
static void di(void)
	{
	asm("
		PFX 8
		WRCTL %g7
		");
	}

// Global interrupt enable
static void ei(void)
	{
	asm("
		PFX 9
		WRCTL %g7
		");
	}

static int r_in_flash_range(ui addr)
	{
	int result = 0;

#ifdef GM_FlashBase
	result = (addr >= (ui)GM_FlashBase) && (addr < (ui)GM_FlashTop);
#endif

	return result;
	}

// | Write a byte to memory. Maybe do some special things for
// | special ranges, like flash.

static int r_stash_byte(ui addr,uc value)
	{
	volatile uc *a;
	int result = 0;

#ifdef GM_FlashBase
	// | Are we writing to flash range?
	// | We do that by stashing even bytes and writing on odd bytes.
	if(r_in_flash_range(addr))
		{
		if((addr & 1) == 0)
			g.even_byte = value;
		else
			{
			us halfword;
	
			halfword = g.even_byte + (value << 8);
			nr_flash_write(GM_FlashBase,(us *)(addr & ~1), halfword);
			}
		goto go_home;
		}
#endif

	a = (uc *)addr;
	*a = value;

	if(*a != value)
		result = -1;
	
	// |
	// | print a bang if it didn't write
	// |

	if(result)
		send_string((void *)g.spc,"!");
go_home:
	return result;
	}


static void r_show_range(ui addr_lo,ui addr_hi)
	{
	g.memory_range = addr_hi - addr_lo;
	g.memory_position = addr_hi;

	addr_hi &= ~1;
	addr_lo &= ~1;

	while( (((int)addr_hi) - ((int)addr_lo)) > 0 )
		{
		int word_count;
		int i;

		//sprintf(g.b,addr_lo <= 0xffff ? "# %04X:" : "# %08X:",addr_lo);
		sprintf(g.b,addr_lo <= 0xffff ? "M%04X:" : "M%08X:",addr_lo);
		word_count = (addr_hi - addr_lo) / 2;
		if(word_count > k_halfwords_per_line)
			word_count = k_halfwords_per_line;

		for(i = 0; i < word_count; i++)
			sprintf(g.b + strlen(g.b)," %04X",*(unsigned short *)(addr_lo + i * 2));

		for(i = word_count; i < k_halfwords_per_line; i++)
			sprintf(g.b + strlen(g.b),"     ");

		sprintf(g.b + strlen(g.b)," # ");

		for(i = 0; i < word_count * 2; i++)
			{
			uc c;

			c = *(char *)(addr_lo + i);
			if(c < 0x20 || c > 0x7e)
				c = '.';
			sprintf(g.b + strlen(g.b),"%c",c);
			}

		sprintf(g.b + strlen(g.b),"\n");
		send_string((void *)g.spc,g.b); // Reenable after test
		
		addr_lo += word_count * 2;
		}
	}


//int parse_germs_command(uc *command, send_proc callback, void *callback_context)
int parse_germs_command(uc *command)

	{
	int result = 0;
	uc echo_str[81];
	uc command_letter;

	//g.sp = callback;
	//g.spc = callback_context;

#if 0
	{
	  char *ptr;
	  ptr=command;
	  while(*ptr!=0)
 	  {
	    printf("%c", *ptr);
	    nr_delay(1);
	    ptr++;
	  }
	  printf("\n");
	}
#endif 

	if((command[0] == 0) || (command[0] == 13)) // no command letter?
		{
		// | null command means show more memory
		// |

		if(g.memory_range == 0)
			g.memory_range = k_default_show_range;
		r_show_range(g.memory_position,g.memory_position + g.memory_range);

		goto go_home;
		}
	
	command_letter = command[0];
	if(command_letter >= 'A' && command_letter <= 'Z')
		command_letter += 'a' - 'A';
	
	// | Dont pass the command letter on.

	command++;

	g.command = command;

	switch(command_letter)
		{
		#if BUILD_FOR_CONTROL
		case '.': // Control program
			parse_control_program_command(command-1); // Send the orig. unmodified command.
			break;
		#endif

		case 'v': // Variable command
			parse_variable_command(command-1);
			break;
			
		case 'o': // Operational command
			parse_operational_command(command-1);
			break;	

		case 'g':
			do_g_command();
			break;
	
		case 'e':
			do_e_command();
			break;
	
		case 'r':
			do_r_command();
			break;
	
		case 'm':
			do_m_command();
			break;
	
		case 's':
			do_s_command();
			break;

		case 'p': // Show/don't show prompt
			if (command[0] == '1')
				{
				((s_tcp_status *)g.spc)->show_prompt=1;
				send_string((void *)g.spc, "\n+"); // Send prompt
				}
			else ((s_tcp_status *)g.spc)->show_prompt=0;	
			break;
	
		case ':':
			do_colon_command();
			break;

		case '#':
			// comment character: do nothing
			break;

		// | c_germs extensions

		case 'w':
			do_show_info();
			break;

		case '?':
		case 'h':
			do_help();
			break;

		default:
			{
			strcpy(echo_str, "?");
			strcat(echo_str, command-1); //  Orig. unmodified command.
			strcat(echo_str, "\n");
			send_string((void *)g.spc, echo_str);
			}
		}

go_home:
	return result;
	}

// return 1 if is break character

int is_break_char(uc x)
	{
	// 4-7-03 Added checking for CR and LF.  GS
	if
		(
			(x == 0)
		||
			(x == 13) // CR
		||
			(x == 10) // LF
		||
			(x >= 0x20 && x < 0x2f)
		||
			(x >= 0x3a && x < 0x3e)
		||
			(x > 0x7f)
		)
		return 1;

	return 0;
	}


static ui r_hex_char_to_value(uc c)
	{
	ui value;

	value = c & 0x000f;
	if(c > '9')
		value += 9;
	
	return value;
	}


// +--------------------------------------------
// | read characters forward until either end of string,
// | or a non-alphanumeric-non-blank char. Return that
// | as the break character.

static ui r_scan_string_for_number(uc **wp, uc *break_char_out)
	{
	uc *w;
	uc c;
	unsigned int value = 0;

	w = *wp;
	// | skip past any leading blanks for the number

	while(*w == ' ')
		w++;

	while(!is_break_char(c = *w++)) // was *w++?? should be (*w)++ ???_No, *w++ is correct
		{
		if(c != ' ')
			{
			value = value * 16 + r_hex_char_to_value(c);
			}
		}

	*wp = w;
	*break_char_out = c;
	return value;
	}


// |
// | Get either a single address or a hyphen-delimited address range,
// | returned as addr_lo & addr_hi. If they match, only one was
// | entered (or user typed 300-300 for a range, which we pretend
// | is just 300).
// |
static void r_scan_string_for_addr_range(uc **wp,uc *break_char_out,ui *addr_lo_out,ui *addr_hi_out)
	{
	uc break_char;
	ui addr_lo = 0;
	ui addr_hi = 0;

	//printf("pass 1_passed string is %s, g.command is %s, addr_lo = %d, break_char = %c \n", *wp, g.command, addr_lo, break_char); // For test
	addr_lo = r_scan_string_for_number(wp,&break_char);
	//printf("pass 2_passed string is %s, g.command is %s, addr_lo = %d, break_char = %c \n", *wp, g.command, addr_lo, break_char); // For test
	if(break_char == '-')
		addr_hi = r_scan_string_for_number(wp,&break_char);
	else
		addr_hi = addr_lo;
	
	*break_char_out = break_char;
	*addr_lo_out = addr_lo;
	*addr_hi_out = addr_hi;
	//printf("addr_lo = %d and addr_hi = %d\n", addr_lo, addr_hi); // For test
	}


int do_g_command(void)
	{
	uc **wp = (unsigned char **)&g.command;
	ui addr;
	uc break_char;

	addr = r_scan_string_for_number(wp,&break_char);
	sprintf(g.b,"# executing at %08x\n",addr);
	send_string((void *)g.spc,g.b);
	r_asm_go(addr);


	return 0;
	}


int do_e_command(void)
	{
	uc **wp = (unsigned char **)&g.command;
	ui addr;
	uc break_char;

	addr = r_scan_string_for_number(wp,&break_char);
	sprintf(g.b,"# erasing: %08x\n",addr);
	send_string((void *)g.spc,g.b);
	
#ifdef GM_FlashBase
	if(r_in_flash_range(addr))
		nr_flash_erase_sector((us *)GM_FlashBase,(us *)addr);
	else
#endif
		send_string((void *)g.spc,"not flash\n");
	
	g.memory_position = addr; // (so a <return> shows what we just erased
	
	return 0;
	}


int do_r_command(void)
	{
	uc **wp = (unsigned char **)&g.command;
	ui addr_lo;
	ui addr_hi;
	uc break_char;

	r_scan_string_for_addr_range(wp,&break_char,&addr_lo,&addr_hi);
	if(addr_lo == addr_hi)
		addr_lo = 0;
	
	g.relocation_offset = addr_hi - addr_lo;
	
	sprintf(g.b,"# relocation offset: %08X\n",g.relocation_offset);
	send_string((void *)g.spc,g.b);

	return 0;
	}


int do_m_command(void)
	{
	uc **wp = (unsigned char **)&g.command;
	uc break_char = 1;
	unsigned int addr_lo;
	unsigned int addr_hi;
	unsigned int v;
	int result = 0;
	
	//printf("in do_m_command(), command = %s and *wp = %s\n", g.command, *wp); // For test
	r_scan_string_for_addr_range(wp,&break_char,&addr_lo,&addr_hi);
	//printf("in do_m_command(), addr_lo = %d, addr_hi = %d and break_char = %c\n", addr_lo, addr_hi, break_char); // For test
	if(break_char != ':') // anything other than colon after address means "show"
		{

		// | Range given? if not, use default range
		//printf("In do_m_command(), break char was not : \n"); // For test
		//addr_lo = 6000; // For test
		//addr_hi = 6000;

		if(addr_lo == addr_hi)
			{
			addr_lo = addr_hi;
			addr_hi += k_default_show_range;
			}

		r_show_range(addr_lo,addr_hi);
		goto go_home;
		}
	
	// | We've gotten either 1 or 2 addresses and a colon. If 1 address,
	// | we're writing words in memory. If 2 addresses, we're filling a
	// | range of memory with a value.

	// | range fill?
	if(addr_lo != addr_hi)
		{
		v = r_scan_string_for_number(wp,&break_char);

		g.memory_position = addr_lo;

		while(addr_lo < addr_hi)
			{

			r_stash_byte(addr_lo,v & 0xff);
			r_stash_byte(addr_lo+1,(v >> 8) & 0xff);

			addr_lo += 2;
			}
		goto go_home;
		}
	
	// | word-by-word

	addr_lo = addr_hi;
	g.memory_position = addr_lo;
	while(break_char != 0)
		{
		v = r_scan_string_for_number(wp,&break_char);

		r_stash_byte(addr_lo,v & 0xff);
		r_stash_byte(addr_lo+1,(v >> 8) & 0xff);

		addr_lo += 2;
		}

go_home:
	return result;
	}


// +-------------------------------
// | Read a byte from the string pointerpointer wp
// | in hex. Also, add the byte into g.checksum.
// |
static ui r_fetch_next_byte(uc **wp)
	{
	uc *w = *wp;
	ui value = 0;

	if(*w != 0)
		value = r_hex_char_to_value(*w++);
	if(*w != 0)
		value = value * 16 + r_hex_char_to_value(*w++);
	
	*wp = w;
	g.checksum += value;

	return value;
	}

static ul r_fetch_multibyte(uc **wp,int byte_count)
	{
	ui value = 0;

	while(byte_count--)
		value = value * 256 + r_fetch_next_byte(wp); // big endian
	
	return value;
	}

// +---------------------------------------------
// | S-Record is
// | S<type><length><address><data...><checksum>
// |
// | Type is 0 -- starting record
// |       1-3 -- 16/24/32 bit address data record
// |       4-6 -- misc symbol records we ignore
// |       7-9 -- 32/24/16 bit ending record (Germs treats as GO record)

int do_s_command(void)
	{
	uc *w = (unsigned char *)g.command;
	uc **wp = (unsigned char **)&w;
	uc record_type=0;
	ui record_length, i;
	ui record_address;
	ui record_address_width; // in bytes: 2, 3, or 4
	//int i;

	g.checksum = 0; // reset the checksum

	if(*w)
		record_type = *w++;
	
	record_length = r_fetch_next_byte(wp);
	
	switch (record_type)
		{
		case '1': // 16-bit-address data record
			record_address_width = 2;
read_s_record_data:
			record_address = r_fetch_multibyte(wp,record_address_width);
			record_length -= (record_address_width + 1); // no count addr or checksum
			for(i = 0; i < record_length; i++)
				{
				ui value;

				value = r_fetch_next_byte(wp);
				r_stash_byte(record_address + i + g.relocation_offset,value);
				}

			// lastly, checksum on S-Record. Should set g.checksum to 0xff.
			r_fetch_next_byte(wp);
			if(g.checksum != 0xff)
				send_string((void *)g.spc,"^"); // checksum error caret
			break;

		case '2': // 24-bit address data record
			record_address_width = 3;
			goto read_s_record_data;
			
		case '3': // 32-bit address data record
			record_address_width = 4;
			goto read_s_record_data;

		case '7': // 32 bit start-address record
			record_address_width = 4;
read_s_record_start_address:
			record_address = r_fetch_multibyte(wp,record_address_width);
			// checksum
			r_fetch_next_byte(wp);
			if(g.checksum != 0xff)
				send_string((void *)g.spc,"^"); // checksum error caret
			else
				r_asm_go(record_address);
			break;

		case '8': // 24 bit start-address record
			record_address_width = 3;
			goto read_s_record_start_address;

		case '9': // 16 bit start-address record
			record_address_width = 2;
			goto read_s_record_start_address;
		} /* switch */
	return 0;
	}


int do_colon_command(void)
	{
	uc *w = (unsigned char *)g.command;
	uc **wp = &w;
	ui record_type;
	ui data_bytes_count;
	ui record_address;
	ui record_address_width, i; // in bytes: 2, 3, or 4
	
	g.checksum = 0; // reset the checksum

	data_bytes_count = r_fetch_next_byte(wp);

	record_address = r_fetch_multibyte(wp,2);
	record_type = r_fetch_next_byte(wp);

	switch(record_type)
		{
		case 0: // normal data read
			record_address += g.ihex_upper_address;


			for(i = 0; i < data_bytes_count; i++)
				{
				ui value;

				value = r_fetch_next_byte(wp);
				r_stash_byte(record_address + i + g.relocation_offset,value);
				}

			// lastly, checksum on I-Hex Record. Should set g.checksum to 0x00.
			r_fetch_next_byte(wp);
			if(g.checksum != 0xff)
				send_string((void *)g.spc,"^"); // checksum error caret
			break;

		case 4: // upper address bits record:
			g.ihex_upper_address = r_fetch_multibyte(wp,2) << 16;
			break;

		} /* switch */

	return 0;
	}




int do_show_info(void)
	{
	//send_string((void *)g.spc,"#\n\n");
	send_string((void *)g.spc,"#Welcome to FasTrak II\n");
	sprintf(g.b,"#CPU ID: %04X\n",r_get_cpu_id());
	send_string((void *)g.spc,g.b);

	sprintf(g.b,"#System ID: %s\n",r_get_monitor_string());
	send_string((void *)g.spc,g.b);

	//send_string((void *)g.spc,"#\n\n");
	}


int do_help(void)
	{
	// Build the help menu into one big string.	
	strcpy(g.help_menu, "#\n");
	strcat(g.help_menu, "# The following commands are available:\n");
	strcat(g.help_menu, "# g <addr>                  -- execute at addr\n");
	strcat(g.help_menu, "# e <addr>                  -- erase flash memory at addr\n");
	strcat(g.help_menu, "# r <addr1>-<addr2>         -- relocate S-records from addr1 to addr2\n");
	strcat(g.help_menu, "# m <addr>                  -- show memory at addr\n");
	strcat(g.help_menu, "# m <addr1>-<addr2          -- show memory range from addr1 to addr2\n");
	strcat(g.help_menu, "# m <addr>:<val1> <val2>... -- write halfwords at addr\n");
	strcat(g.help_menu, "# m <addr1>-<addr2>:<val>   -- fill range with halfword val\n");
	strcat(g.help_menu, "# s <S-record data>          -- write S-record to memory\n");
	strcat(g.help_menu, "# :<Intel hex data>         -- write Intel hex record to memory\n");
	strcat(g.help_menu, "# w                         -- show processor version\n");
	strcat(g.help_menu, "# help                      -- show this help\n");
	strcat(g.help_menu, "#\n");
	
	send_string((void *)g.spc, g.help_menu);
	}


		


/////////////////////////////// Ethernet stuff here ////////////////////////////////////

// send_string() more for compatibility with Altera code.  This may also be more convenient
// to call when wanting to reference the plug by context.  Since it calls tcp_xmit() below,
// it can handle ascii and binary data of any size.  If desired, tcp_xmit() can also be 
// called directly as it is many times in this file.
int send_string(void *context, char *s)
{
int result, sizeofstr;

	sizeofstr = strlen(s);
	
	//result = nr_plugs_send(((s_tcp_status *)context)->tcp_plug, (void *)s, sizeofstr, 0);
	result = tcp_xmit(((s_tcp_status *)context)->tcp_plug, s, sizeofstr);

	return (result); // If result == 999, tcp_xmit() was already in use, sorry charlie - no data sent.
}



// Original xmit routine
int tcp_xmit(int plug_handle, void *data, int length)
	{
	int num_of_chunks, leftover, y, return_val=0;

	if (!vtg.tcp_xmit_in_progress) // Don't allow recursion on this routine
		{
		//di();
		vtg.tcp_xmit_in_progress++;

		num_of_chunks = length / TCP_CHUNK_SIZE;
		leftover = length % TCP_CHUNK_SIZE;

		// Send chunks first
		if (num_of_chunks) // 1 or more chunks
			{
			for (y=0; y<num_of_chunks; y++)
				{
				if(return_val=nr_plugs_send(plug_handle, (void *)data, TCP_CHUNK_SIZE, 0)) goto ext;
				data+=TCP_CHUNK_SIZE;
				}
			}
	
		// Send any leftovers
		return_val = nr_plugs_send(plug_handle, (void *)data, leftover, 0);

		vtg.tcp_xmit_in_progress--;
		//ei();
		}
	else
		{
		return_val=999; // Xmit already in progress
		}

	ext: ;
	return (return_val);
	}



#if 0
// New xmit routine, may be some problems with this.
//
// tcp_xmit will take a block of data and break it into TCP_CHUNK_SIZE sized chunks
// and transmit them using nr_plugs_send().  Any leftover "partial chunk" will be
// transmitted last.
int tcp_xmit(int plug_handle, void *data, int length)
	{
	int num_of_chunks, leftover, y, return_val=0, x;
	//volatile unsigned char *working_data;

	//working_data = data;

	if (!vtg.tcp_xmit_in_progress) // Don't allow recursion on this routine
		{
		vtg.tcp_xmit_in_progress++;

		while (length)
			{
			if (length >= TCP_CHUNK_SIZE)
				{
				//printf("Sending %d bytes\n", TCP_CHUNK_SIZE);
				if(return_val=nr_plugs_send(plug_handle, (void *)data, TCP_CHUNK_SIZE, 0)) goto ext;
				data += TCP_CHUNK_SIZE;
				length -= TCP_CHUNK_SIZE;
				continue;
				}
			else 
				{
				//printf("Sending %d bytes\n", length);
				if(return_val=nr_plugs_send(plug_handle, (void *)data, length, 0)) goto ext;
				break;
				}
			}

		vtg.tcp_xmit_in_progress--;
		}
	else
		{
		return_val=999; // Xmit already in progress
		}

	ext: ;
	return (return_val);
	}
#endif



// setup the system for a connection using TCP on a given port
int tcp_init(void)
{
  	s_tcp_status *plant, *local, *io;
  	int result=0;
  

  	plant = &plant_net_settings;
  	local = &local_net_settings;
  	io 	= &io_net_settings;

	//printf("Here in tcp_init, u == %x\n", u); // For test
  	result = nr_plugs_initialize(0, 0, __adapter__,
			    __adapter_irq__,
			    __adapter_struct_addr__);

	if (result != 0)
		{
		printf("Error initializing plugs\n");
		}

    
  	result = nr_plugs_create((int *)&local->tcp_plug, ne_plugs_tcp, 20000, tcp_proc, (void *)local, 0);
  	if (result == 0)
		{
		printf("create plug %d error %d\n",local->tcp_plug, result);
    	result = nr_plugs_listen(local->tcp_plug, tcp_listen_proc, (void *)local);
		}
  	else
		{
		printf("error creating plug %d\n", local->tcp_plug);
		}
//#if 0
  	if (result == 0) // No errors, keep going
	  	{	
  	  	result = nr_plugs_create((int *)&plant->tcp_plug, ne_plugs_tcp, 20002, tcp_proc, (void *)plant, 0);
  		if (result == 0)
			{
			printf("create plug %d error %d\n",plant->tcp_plug, result);
    		result = nr_plugs_listen(plant->tcp_plug, tcp_listen_proc, (void *)plant);
			}
  		else
			{
			printf("error creating plug %d\n", plant->tcp_plug);
			}
	  	}
	
  	if (result == 0) // No errors, keep going
		{	
  		result = nr_plugs_create((int *)&io->tcp_plug, ne_plugs_tcp, 20004, tcp_proc, (void *)io, 0);
  		if (result == 0)
			{
			printf("create plug %d error %d\n",io->tcp_plug, result);
    		result = nr_plugs_listen(io->tcp_plug, tcp_listen_proc, (void *)io);
			}
  		else
			{
			printf("error creating plug, result = %d\n", result);
			}
		}
//#endif
	
	// Blank out buffers	
 	local->c[0] = 0;
  	plant->c[0] = 0;
  	io->c[0] = 0;  

	// Clear terminal mode
	local->show_prompt=0;
	plant->show_prompt=0;
	io->show_prompt=0;

  	return (result);
}



int tcp_proc(int plug_handle,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length)
	{
	s_tcp_status *u = (s_tcp_status *)context;
	int i, ss_ctr;
	char sendstr[81], *ptr;
		
	// In the future, need to determine what plug were dealing 
	// with here and act accordingly.  For now we'll treat them
	// all the same.	
	if(payload)
		{
		g.spc = u; // Set context for send_string() 
		ptr = (char *)payload;
		// loadBuffer(u, (char*)payload, payload_length);
		
		// First we'll load the buffer.
		// Could call loadbuffer but we'll save the function call
		// and load the buffer here.
		for(i=0; i<payload_length; i++)
			{
			u->c[i] = ptr[i];
			//printf("payload[i] = %c\n", ptr[i]);
			printf("{%d}", ptr[i]);
			}
 		u->c[i] = 0; // Terminate string		
		
		// Now we'll break the buffer up into strings.
		ss_ctr = 0;
			
		// Block commands must be delimited by a CR.  If no CRs are found
		// in the string, no commands will be executed.  There can be 
		// multiple commands seperated by CRs within one string.
		for(i=0;i<payload_length;i++)
			{
			sendstr[ss_ctr++] = u->c[i];	
			//printf("u->c[%d] = %c (%d)\n", i, u->c[i], u->c[i]); 
			if(u->c[i] == 13 || u->c[i] == 10) // CR or LF?
				{
				if (u->c[i] == 10); // Ignore LF if present
				else
					{
					sendstr[ss_ctr]=0; // Write null terninator over the CR.
					ss_ctr=0;
					//printf("tcp_proc(), Ready to parse %s %d\n", sendstr, sendstr[0]); // For test
					parse_germs_command(sendstr);
					//printf("tcp_proc(), Parsed %s %d\n", sendstr, sendstr[0]); // For test
					}
				}
			}
		if (((s_tcp_status *)g.spc)->show_prompt) send_string((void *)g.spc, "\n+"); // Send prompt
		//printf("tcp_proc(), sent string rn+\n"); // For test
		u->c[0] = 0; // Blank out the buffer - probably not necessary.
		}
	else // Close the connection and start listening again
		{
		nr_plugs_listen(u->tcp_plug, tcp_listen_proc, (void *)u);
		u->connected = 0;
		printf("Received a null packet, shutting down connection on plug %d\n", u->tcp_plug);
		}
	
	return 0;
	}


void loadBuffer(void *context, char *payload, int payload_length)
	{
	int x;
	s_tcp_status *u = (s_tcp_status *)context;
	
	for(x=0; x<payload_length; x++)
		{
		u->c[x] = payload[x];
		//printf("payload[x] = %c\n", payload[x]);
		}
 
		
	u->c[x] = 0; // Terminate string
	//printf("Now at the end of loadbuffer, u->c = %s\n", u->c);
	//printf("and local_net_settings.c = %s\n", local_net_settings.c);
	}


// +------------------------------------
// | This plugs callback routine is responsible
// | for accepting or rejecting an incoming
// | connection request to our tcp plug.
// | We always accept it, and take the
// | opportunity to print out some information
// | about who connected to the plug.
// |

int tcp_listen_proc
		(
		int plug_handle,
		void *context,
		host_32 remote_ip_address,
		host_16 remote_port
		)
	{
	s_tcp_status *u = context;

	u->connected = 1;
	u->remote_ip_address = remote_ip_address;
	u->remote_port = remote_port;
	
	printf("Accepted connection from ");
	nr_plugs_print_ip_address(nr_h2n32(remote_ip_address));
	printf(" port %d\n",remote_port);
	
	g.spc = u; // Set context for send_string()
	do_show_info();
	if (((s_tcp_status *)g.spc)->show_prompt) send_string((void *)g.spc,"+"); // Send prompt
	
	return 0; // "no error" == accept the connection
	}



int whos_the_current_plug(void)
	{	
	// Who's current, local or plant-wide?
	if (((s_tcp_status *)g.spc)->tcp_plug == local_net_settings.tcp_plug)
		return (LOCAL_IS_CURRENT);
	else if (((s_tcp_status *)g.spc)->tcp_plug == plant_net_settings.tcp_plug)
		return (PLANT_WIDE_IS_CURRENT);
	else
		return (0); // Nobody's current? 
	}


void transmit_binary_data(unsigned short int shot_num)
	{
	int length;
	char temp_str[81], *data_ptr;
	
		
	//////////////////////// Transmit position based data first /////////////////////////
	length = sizeof(combined_sample) * (comp_shot_parameters.eos_pos+1); // Take into account location [0]
	data_ptr = (char *)&data_sample[0]; // + sizeof(combined_sample); // Data begins at location [1] not [0].

	// Send an ASCII header string.
	sprintf(temp_str, "P_BEGIN_%d_%d\n", shot_num, length);
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
	
	// Send position based binary data.
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, data_ptr, length);

	// Finish by sending ASCII position based END statment.
	//sprintf(temp_str, "P_END_%d_%d_%d\n", shot_num, comp_shot_parameters.eos_pos, length);
	//tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
	

	//////////////////// Then transmit time based data //////////////////////
	length = sizeof(combined_sample) * TIM_COLL_SAMPLES;
	data_ptr = (char *)&data_sample[comp_shot_parameters.eos_pos+1];
	
	// Send ASCII header.
	sprintf(temp_str, "T_BEGIN_%d_%d\n", shot_num, length);
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
		
	// Send time based binary data.
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, data_ptr, length);

	// Finish by sending ASCII time based END statment.
	//sprintf(temp_str, "T_END_%d_%d_%d\n", shot_num, 2000, length);
	//tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));

	
	///////////////////// Transmit computed shot parameters /////////////////////
	// All computed shat data will be sent
	// regardless of dip switch settings.
	length = sizeof(comp_shot_parameters);
	data_ptr = (char *)&comp_shot_parameters;
	
	// Send ASCII header.
	sprintf(temp_str, "C_BEGIN_%d_%d\n", shot_num, length);
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));

	// Send computed shot binary data.
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, data_ptr, length);

	// Finish by sending ASCII END statment.
	//sprintf(temp_str, "C_END_%d_%d\n+", shot_num, length);
	//tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
	}




void transmit_monitor_data(unsigned short int shot_num)
	{
	int y, z;
	char temp_str1[81], temp_str2[81], temp_str3[81], bigstr[250];
	np_pio *switches = na_button_pio;

	// Format of data strings sent:
	
	// P_22337_1,MSxxxxxxxxPaaaaaaaaQ1bbbbbbbbQ2ccccccccQ3ddddddddQ4eeeeeeee<LF>
	// P_22337_1,CH9_xxxCH10_xxxCH11_xxxCH12_xxxCH13_xxxCH14_xxxCH15_xxxCH16_xxx<LF>
	// P_22337_1,ISW1xxxx_ISW2xxxx_ISW3xxxx_ISW4xx_OSW1xxxx_OSW2xxxx_OSW3xxxx<LF>
	// P_END_22337_745<LF>

	// T_22337_1,Paaaaaaaa_ISW1xxxx_ISW2xxxx_ISW3xxxx_ISW4xx<LF>
	// T_22337_1,OSW1xxxx_OSW2xxxx_OSW3xxxx<LF>
	// T_22337_1,CH9_xxxCH10_xxxCH11_xxxCH12_xxxCH13_xxxCH14_xxxCH15_xxxCH16_xxx<LF>
	// T_END_22337_2000<LF>	
		
	// C_22337_1,xxxx #Biscuit size, X4 counts <LF>
	// C_22337_2,xxxxxxxx #Cycle time in mS. <LF>
	// C_22337_3,xxxxx #Eos position in X1 counts <LF>
	// C_END_22337_3<LF>

	bigstr[0] = 0;  // Clear out bigstr.
	
	if ((switches->np_piodata) & 0x040) // Dip switch6 open, send binary data.
		transmit_binary_data(shot_num);
	else
		{
		// Transmit position based data first.
		for (y=1; y<=comp_shot_parameters.eos_pos; y++)
		//for (y=1; y<=3; y++)
			{
			// Line 1 of this sample.
			//sprintf(temp_str1, "P_%d_%d,MS%uP%dQ1%uQ2%uQ3%uQ4%u\n", shot_num, y, 
			sprintf(temp_str1, "P_%d_%d,MS%uP%dQ1%xQ2%xQ3%xQ4%x\n", shot_num, y, 
						data_sample[y].one_ms_timer, data_sample[y].position, 
							data_sample[y].vel_count_q1, data_sample[y].vel_count_q2,
								data_sample[y].vel_count_q3, data_sample[y].vel_count_q4);
		

			// Line 2 of this sample.
			sprintf(temp_str2, "P_%d_%d,CH1%xCH2%xCH3%xCH4%xCH5%xCH6%xCH7%xCH8%x\n",
					shot_num, y,
						data_sample[y].ana_ch1, data_sample[y].ana_ch2, data_sample[y].ana_ch3,
							data_sample[y].ana_ch4, data_sample[y].ana_ch5, data_sample[y].ana_ch6,
								data_sample[y].ana_ch7, data_sample[y].ana_ch8);
		
		
			// Line 3 of this sample.
			sprintf(temp_str3, "P_%d_%d,ISW1%xISW20ISW30ISW4%xOSW1%xOSW20OSW30\n",
					shot_num, y, data_sample[y].isw1, data_sample[y].isw4, data_sample[y].osw1);
		

			if ((switches->np_piodata) & 0x010) // Dip switch8 open, send an abbreviated packet
				{
				// Only the first and last 5 data samples will be sent
				if (y<=5 || y>(comp_shot_parameters.eos_pos-5))
					{
					send_string((void *)g.spc, temp_str1);
					send_string((void *)g.spc, temp_str2);
					send_string((void *)g.spc, temp_str3);
					}
				}
			else if ((switches->np_piodata) & 0x020) // Dip switch7 open, send all date, but slow it down
				{
				send_string((void *)g.spc, temp_str1);
				send_string((void *)g.spc, temp_str2);
				send_string((void *)g.spc, temp_str3);
				nr_delay(20);
				}
			else   // Both Dip sws 8 and 7 are closed, Send all data full steam ahead.
				{
				strcpy(bigstr, temp_str1);
				strcat(bigstr, temp_str2);
				strcat(bigstr, temp_str3);
				send_string((void *)g.spc, bigstr);

				//send_string((void *)g.spc, temp_str1);
				//send_string((void *)g.spc, temp_str2);
				//send_string((void *)g.spc, temp_str3);
				}
			}
		// Finish by sending position based END statment.
		sprintf(temp_str1, "P_END_%d_%d\n", shot_num, y-1);
		send_string((void *)g.spc, temp_str1);

		// Transmit time based data.
		//for (y=1; y<=2000; y++)
		for (y=comp_shot_parameters.eos_pos+1, z=1; y<=comp_shot_parameters.eos_pos+2000; y++, z++)
		//for (y=comp_shot_parameters.eos_pos+1, z=1; y<=comp_shot_parameters.eos_pos+4; y++, z++) // Only transmit 10 samples for initial testing
			{
			// Line 1 of this sample.
			sprintf(temp_str1, "T_%d_%d,P%dISW1%xISW20ISW30ISW4%x\n",
				shot_num, z, data_sample[y].position, data_sample[y].isw1, data_sample[y].isw4);
		

			// Line 2 of this sample.
			sprintf(temp_str2, "T_%d_%d,OSW1%xOSW20OSW30\n", shot_num, z, data_sample[y].osw1);
		

			// Line 3 of this sample.
			sprintf(temp_str3, "T_%d_%d,CH1%xCH2%xCH3%xCH4%xCH5%xCH6%xCH7%xCH8%x\n",
				shot_num, z,
					data_sample[y].ana_ch1, data_sample[y].ana_ch2, data_sample[y].ana_ch3,
						data_sample[y].ana_ch4, data_sample[y].ana_ch5, data_sample[y].ana_ch6,
							data_sample[y].ana_ch7, data_sample[y].ana_ch8);
		

			if ((switches->np_piodata) & 0x010) // Dip switch8 open, send an abbreviated packet
				{
				// Only the first and last 5 data samples will be sent.
				if (z<=5 || z>(2000-5))
					{
					send_string((void *)g.spc, temp_str1);
					send_string((void *)g.spc, temp_str2);
					send_string((void *)g.spc, temp_str3);
					}
				}
			else if ((switches->np_piodata) & 0x020) // Dip switch7 open, send all data, but slow it down.
				{
				send_string((void *)g.spc, temp_str1);
				send_string((void *)g.spc, temp_str2);
				send_string((void *)g.spc, temp_str3);
				nr_delay(20);
				}
			else   // Both Dip sws 8 and 7 are closed, Send all data full steam ahead.
				{
				strcpy(bigstr, temp_str1);
				strcat(bigstr, temp_str2);
				strcat(bigstr, temp_str3);
				send_string((void *)g.spc, bigstr);

				//send_string((void *)g.spc, temp_str1);
				//send_string((void *)g.spc, temp_str2);
				//send_string((void *)g.spc, temp_str3);
				}
			}
		// Finish by sending time based END statment.
		sprintf(temp_str1, "T_END_%d_%d\n", shot_num, z-1);
		send_string((void *)g.spc, temp_str1);

		// Transmit computed shot parameters.  All computed shat data will be sent
		// regardless of dip switch settings.
		sprintf(temp_str1, "C_%d_1,%d #Biscuit size, X4 counts\n",
					shot_num, comp_shot_parameters.biscuit_size);
		send_string((void *)g.spc, temp_str1);

		sprintf(temp_str1, "C_%d_2,%d #Cycle time in mS\n",
					shot_num, comp_shot_parameters.cycle_time);
		send_string((void *)g.spc, temp_str1);

		sprintf(temp_str1, "C_%d_3,%d #Eos position in X1 counts\n",
					shot_num, comp_shot_parameters.eos_pos);
		send_string((void *)g.spc, temp_str1);

		sprintf(temp_str1, "C_END_%d_3\n+", shot_num);
		send_string((void *)g.spc, temp_str1);
		}
	}

///////////////////////////////////////////////////////////////////











////////////////////////// VTW functions /////////////////////////

int parse_variable_command(unsigned char *orig_cmd_str)
	{
	int a, strings, readback_err=0, set_err=0, no_of_strings;
	int var, var1, var2, value, first_value;	
	unsigned char echo_str[81], *ptr, *p_range, *p_set, *p_set_multiple;
	
	// First get rid of comment if present
	if ((ptr = (char *)strchr(orig_cmd_str, '#')) != NULL) *ptr = 0; 
	
	ptr = orig_cmd_str; ptr++;
	// Set pointers to point to the various delimiters.
	// Pointers will == NULL if delimiters not found.
	p_range = (char *)strchr(orig_cmd_str, '-'); 
	p_set = (char *)strchr(orig_cmd_str, '=');
	p_set_multiple = (char *)strchr(orig_cmd_str, ','); 

	if (p_range && p_set) // '-' could be a minus sign in the setting, not a range
		{
		if (p_range > p_set) // It is a minus sign, there is no range.
			p_range = NULL;
		}

	
	// Adjust for case
	if(*ptr >= 'A' && *ptr <= 'Z')
		*ptr = *ptr + ('a' - 'A');
	
	if (*ptr == 'd') vtg.var_dec_hex = VAR_DEC; // Variables read back in decimal
	else if (*ptr == 'h') vtg.var_dec_hex = VAR_HEX;  // Vars read back in hex
	else if (*ptr == 'f') save_vars_to_flash(); 
	else if (*ptr == 'r') reload_vars_from_flash(); // Reload variables from flash memory
	else if (!p_set) // Readback only
		{
		if (!p_range) // Simple readback of one variable
			{
			var = atoi(ptr);
			if (var<0 || var>MAX_VAR) readback_err=1;
			else
				{	
				sprintf(echo_str,vtg.var_dec_hex == VAR_DEC ? "V%d_%d\n" : "V%d_%x\n",var,vtg.v[var]);	
				send_string((void *)g.spc, echo_str);	
				}
			}
		else // A range of variables was requested
			{
			// var1 and var2 determine the range
			var1 = atoi(ptr); var2 = atoi(p_range+1); 
			
			if (var1<0 || var1>MAX_VAR || var2<0 || var2>MAX_VAR || var2<var1) readback_err=1; // Invalid ranges
			else
				{
				var2++;
				// var2++ done so that the range includes last var asked for:
				// for example, if var2 were not incremented, the command
				// V22-32 would include the 10 vars 22-31, 32 would be
				// excluded.  By incrementing var2, we cover the entire range
				// asked for, in this case 11 vars.
					
				// Limit no of vars to 5 per string.  If 5 does not 
				// evenly go into var2-var1, need an extra string for the
				// remainder.	
				no_of_strings = (((var2-var1)%5) > 0) ?  ((var2-var1)/5)+1 : (var2-var1)/5; // 5 vars per string
				
				for (strings = 0; strings < no_of_strings; strings++)
					{
					// This big mess of a sprintf formats echo_str for
					// decimal or hex.  The sequential values for 5
					// vars starting at x also formatted.  Each for loop
					// another 5 vars are formatted and sent.
					sprintf(echo_str,vtg.var_dec_hex == VAR_DEC ? "V%d_%d_%d_%d_%d_%d\n" : "V%d_%x_%x_%x_%x_%x\n",
						var1+(5*strings),vtg.v[(var1+(5*strings))+0],vtg.v[(var1+(5*strings))+1],vtg.v[(var1+(5*strings))+2],
							vtg.v[(var1+(5*strings))+3],vtg.v[(var1+(5*strings))+4]);
					send_string((void *)g.spc, echo_str);		
					}
				}
			}
		if (readback_err)
			{
			sprintf(echo_str, "?%s\n", orig_cmd_str);
			send_string((void *)g.spc, echo_str);		
			}			
		} // readback
	else // Must be variable set
		{
		if (!p_set_multiple && !p_range) // Set a single variable
			{
			var=atoi(ptr);	
			if (p_set[1] == 'h' || p_set[1] == 'H') value = htoi(p_set+2); // Hex
			else value = atoi(p_set+1); // Decimal	
			vtg.v[var]=value;	
			}
		else if (p_set_multiple && !p_range) // Write successive variables with different values beginning with the one specified
			{
			var = atoi(ptr); // Var number
			first_value = atoi(p_set+1); // First value after the '='
			value = atoi(p_set_multiple+1); // Value after the first comma
			ptr = p_set_multiple;
			vtg.v[var++]=first_value; // Set first var after the '='
			
			do // Now start setting the values after the commas
				{
				vtg.v[var++]=value;
				if ((ptr = (char *)strchr(ptr+1, ',')) != NULL)
					{
					if (ptr[1] == 'h' || ptr[1] == 'H') value=htoi(ptr+2); // Move past the comma and 'h'
					else value=atoi(ptr+1); // Move past the comma	
					} 	
				} while (ptr != NULL);
			}
		else if (p_range && !p_set_multiple) // Write a range of vars with the same value
			{
			// Together, var1 and var2 establish the range.
			var1 = atoi(ptr); var2 = atoi(p_range+1);
			if (var1<0 || var1>MAX_VAR || var2<0 || var2>MAX_VAR || var2<var1) set_err=1; // Invalid ranges	
			else
				{
				var2++;
				// var2++ done so that the range includes last var asked for:
				// for example, if var2 were not incremented, the command
				// V22-32=555 would include the 10 vars 22-31, 32 would be
				// excluded.  By incrementing var2, we cover the entire range
				// asked for, in this case 11 vars.		
				
				// Now get value that the range of vars will be set to
				if (p_set[1] == 'h' || p_set[1] == 'H') value=htoi(p_set+2); 
				else value=atoi(p_set+1);
				
				for (a=0; a<(var2-var1); a++)
					{
					vtg.v[var1+a]=value;	
					}
				}
			}
		else if (p_range && p_set_multiple) set_err=1; // Invalid 
		if (set_err)
			{
			sprintf(echo_str, "?%s\n", orig_cmd_str);
			send_string((void *)g.spc, echo_str);		
			}					
		}// Var set 
	}	



int parse_operational_command(unsigned char *orig_cmd_str)
	{
	np_pio *misc_ins_pio = na_misc_ins;	
	np_timer *o_scope_timer = na_o_scope_timer;
	unsigned char echo_str[81], *ptr;
	int ch_num, val, return_val=0, incremental=0, axis_num=1;
	//unsigned short int ana_result;
	unsigned int ana_result;
	unsigned int uval;
	unsigned int period;
	volatile unsigned short int *halfword_ptr;
	
	// First get rid of comment if present
	if ((ptr = (char *)strchr(orig_cmd_str, '#')) != NULL) *ptr = 0; 
	
	ptr = orig_cmd_str; ptr++;

		// Adjust for case
	if(*ptr >= 'A' && *ptr <= 'Z')
		*ptr = *ptr + ('a' - 'A');

	switch (*ptr)
		{
		case 'c': // Commands
			ptr++;
			#if BUILD_FOR_CONTROL
			if (*ptr=='1') // Command to set / clear O-scope mode
				{
				ptr += 2;
				val = atoi(ptr);

				
				// Now let's find out who's ask'in, local or plant-wide?
				if (whos_the_current_plug() == LOCAL_IS_CURRENT)
					vtg.local_o_scope_mode = val;  // Zero to disable, anything else to enable.
				else if (whos_the_current_plug() ==	PLANT_WIDE_IS_CURRENT)
					vtg.plant_wide_o_scope_mode = val;  // Zero to disable, anything else to enable.
				
				if (val != 0)  // Someone commanded o_scope mode, start timer.
					{
					// Check and see if the timer is already running, if not, we'll start it
					if (!(o_scope_timer->np_timerstatus & np_timerstatus_run_mask))
						{
						// Now load the time value into the timer
						convert_uS_to_timer(O_SCOPE_SAMPLE_INTERVAL_uS, &period); // Get period
						halfword_ptr = (unsigned short int *)&period;
						//printf("pl = %x and ph = %x\n", periodl, periodh); // For test
						o_scope_timer->np_timerperiodl = halfword_ptr[0];
						o_scope_timer->np_timerperiodh = halfword_ptr[1];
						o_scope_timer->np_timercontrol = 7; // Start timer, continuous mode
						}
					}
				}
			#endif

			// Allow op data packets in monitor only mode also
			if (*ptr=='3') // Command to enable operational data packets
				{
				ptr += 2;
				// Next WD reset timer interrupt, op data packet will be sent according to
				// val.  0==Don't send any, 1==Send one, 2==Keep sending.  One or both plugs
				// can be receiving data simultaneously.
				if (((unsigned int)val=atoi(ptr)) > 2) val=0; // Don't send anything.

				// Now let's find out who's askin, local or plant-wide?
				if (whos_the_current_plug() == LOCAL_IS_CURRENT)
					{ 
					vtg.local_wants_op_packets = val;
					}
				else if (whos_the_current_plug() ==	PLANT_WIDE_IS_CURRENT)
					{
					vtg.plant_wide_wants_op_packets = val;
					}
				}

			#if BUILD_FOR_CONTROL
			if (*ptr=='4') // Initialize/update axes
				{
				ptr += 2;
				if (atoi(ptr)==1)
					initialize_axes();
				else if (atoi(ptr)==2)
					update_control_parameters();
				else
					{
					sprintf(echo_str,"R_ERROR:2 #Syntax error, bad command number\n");	
					send_string((void *)g.spc, echo_str);
					}
				}
			else if (*ptr=='5') // Check servo null
				{
				ptr += 2; // Point to axis number
				null_valve(atoi(ptr), CHECK_NULL);
				}
			else if (*ptr=='6') // Auto null servo
				{
				ptr += 2; // Now point to axis number
				null_valve(atoi(ptr), AUTO_NULL);
				}
			#endif // BUILD_FOR_CONTROL
			break;

		#if BUILD_FOR_CONTROL
		case 'r': // Read back control fatal error or warning words.
			ptr++;
			if (*ptr=='f' || *ptr=='F')
				{
				sprintf(echo_str,"R_FATAL_ERROR:21 #Fatal_Error=%d\n", FATAL_ERROR_WORD);	
				}
			else if (*ptr=='w' || *ptr=='W')
				{
				sprintf(echo_str,"R_WARNING:22 #Warning=%d\n", vtg.saved_warning);		
				}
			else
				{
				sprintf(echo_str,"R_ERROR:2 #Syntax error\n");	
				}
			send_string((void *)g.spc, echo_str);		
			break;	
		#endif // BUILD_FOR_CONTROL

		case 's': // Read back current position
			ptr++;
			axis_num=atoi(ptr);
			if (axis_num > 2 || axis_num < 1)
				sprintf(echo_str,"Error :%d #invalid axis specification", 3); // invalid	axis number
			else
				sprintf(echo_str,"R_POS%d:11 #%d\n",axis_num, RD_PV_POS);

			send_string((void *)g.spc, echo_str);
			break;
		case 'f': // Read back current velocity
			ptr++;
			axis_num=atoi(ptr);
			if (axis_num > 2 || axis_num < 1)
				sprintf(echo_str,"Error :%d #invalid axis specification", 3); // invalid	axis number.
			else
				sprintf(echo_str,"R_VEL%d:15 #%d\n",axis_num, (axis[axis_num].vel_ctr_freq/RD_PV_VEL)); 

			send_string((void *)g.spc, echo_str);
			break;
		case 'a': // convert analog channels
			ptr++;
			if (*ptr == 'A' || *ptr == 'a') // Convert all channels
				{
				// Since we want data from all 20 channels, we'll force it here
				// insted of using the convert_ana_ch() function.
				//AD_CMD1 = 4; // Do an A/D loop
				//while (AD_CMD1); // Wait until cleared indicating we have A/D data

				for (ch_num=1; ch_num<=20; ch_num++)
					{
					if (convert_ana_ch(ch_num, RAW_VAL, &ana_result))
						{
						sprintf(echo_str,"Error :%d #unable to convert analog channel(s)\n", 5); // invalid channel number.
						send_string((void *)g.spc, echo_str);
						break;
						}
					else  // Everything's cool, transmit the data.
						{
						sprintf(echo_str,"O_CH%d=%x\n", ch_num, ana_result);
						send_string((void *)g.spc, echo_str);
						}

					//sprintf(echo_str,"O_CH%d=%x\n", ch_num, shared_memory[ch_num-1]);
					//send_string((void *)g.spc, echo_str);
					}
				}
			else
				{
				ch_num = atoi(ptr);
				if (ch_num < 1 || ch_num > 20)
					{
					sprintf(echo_str,"Error :%d #invalid channel number\n", 4); // invalid channel number.
					send_string((void *)g.spc, echo_str);
					}
				else
					{
					if (convert_ana_ch(ch_num, RAW_VAL, &ana_result))
						{
						sprintf(echo_str,"Error :%d #unable to convert analog channel(s)\n", 5); // invalid channel number.
						send_string((void *)g.spc, echo_str);
						}
					else  // Everything's cool, transmit the data.
						{
						sprintf(echo_str,"O_CH%d=%x\n", ch_num, ana_result);
						send_string((void *)g.spc, echo_str);
						}
					}
				}
			break;
		case 'd': // Output to DAC channels
			ptr++;
			ch_num = atoi(ptr);
			if (ch_num<1 || ch_num>4)
				{
				sprintf(echo_str,"Error :%d #invalid channel number\n", 4); // invalid channel number.
				send_string((void *)g.spc, echo_str);
				break;
				}
			
			ptr++;
			if (*ptr == '=') // We're setting the DAC.
				{
				ptr++;
				if (*ptr == '@') // We're setting an incremental value
					{
					incremental=1;
					ptr++;
					}
					
				if (*ptr == 'V' || *ptr == 'v')
					{
					ptr++;
					val = atoi(ptr);
					if (val < -10000 || val > 10000)
						{
						sprintf(echo_str,"Error :%d #DAC value out of range\n", 6); // invalid channel number.
						send_string((void *)g.spc, echo_str);
						break;
						}
					
					if (incremental)
						return_val = out_dac(ch_num, val, VOLTAGE_X_1000, INCREMENTAL);
					else
						return_val = out_dac(ch_num, val, VOLTAGE_X_1000, ABSOLUTE);
							
					if (return_val)
						{
						sprintf(echo_str,"Error :%d #unable to send value to DAC channel\n", 7); // invalid channel number.
						send_string((void *)g.spc, echo_str);
						break;
						}
					}
				else
					{
					if (*ptr == 'H' || *ptr == 'h') // Hex value
						//uval = (unsigned int)htoi(ptr+1);	
						val = htoi(ptr+1);	
					else	// Decimal value
						//uval = (unsigned int)atoi(ptr);
						val = atoi(ptr);
					
					//if (uval > 0x3fff)
					if (!incremental)
						{
						if (val > 0x3fff || val < 0)
							{
							sprintf(echo_str,"Error :%d #DAC value out of range\n", 6); // invalid channel number.
							send_string((void *)g.spc, echo_str);
							break;
							}
						}	

					if (incremental)
						//return_val = out_dac(ch_num, uval, RAW_VAL, INCREMENTAL);
						return_val = out_dac(ch_num, val, RAW_VAL, INCREMENTAL);
					else
						//return_val = out_dac(ch_num, uval, RAW_VAL, ABSOLUTE);
						return_val = out_dac(ch_num, val, RAW_VAL, ABSOLUTE);

					if (return_val)
						{
						sprintf(echo_str,"Error :%d #unable to send value to DAC channel\n", 7); // invalid channel number.
						send_string((void *)g.spc, echo_str);
						break;
						}
					}
				}
			else  // We're reading the current data the DAC is set to
				{
				if (ch_num == 1) sprintf(echo_str, "O_DAC1=%d\n", vtg.dac_word[1]);
				else if (ch_num == 2) sprintf(echo_str, "O_DAC2=%d\n", vtg.dac_word[2]); // Not implemented
				else if (ch_num == 3) sprintf(echo_str, "O_DAC3=%d\n", vtg.dac_word[3]); // Not implemented
				else if (ch_num == 4) sprintf(echo_str, "O_DAC4=%d\n", vtg.dac_word[4]);	
				send_string((void *)g.spc, echo_str);
				}		
			break;
		case 'i': // Read ISW1 - ISW4
			ptr++;
			ch_num = atoi(ptr); // Use ch_num for convenience, not really a channel number.
			
			if (ch_num == 1) sprintf(echo_str, "O_ISW1=%x\n", ISW1);
			else if (ch_num == 2) sprintf(echo_str, "O_ISW2=%x\n", ISW2); // Not implemented
			else if (ch_num == 3) sprintf(echo_str, "O_ISW3=%x\n", ISW3); // Not implemented
			else if (ch_num == 4) sprintf(echo_str, "O_ISW4=%x\n", ISW4);
			else // Bad ISW specification.
				{
				sprintf(echo_str,"Error :%d ISWy specification invalid, must be between 1 and 4#\n", 2); // invalid channel number.
				send_string((void *)g.spc, echo_str);
				break;
				}
				
			send_string((void *)g.spc, echo_str);	
			break;	
		case 'o': // Set or get OSW1 - OSW3
			ptr++;
			ch_num = atoi(ptr); // Use ch_num for convenience, not really a channel number.
			if (ch_num<1 || ch_num>3)				
				{
				sprintf(echo_str,"Error :%d OSWy specification invalid, must be between 1 and 3#\n", 2); // invalid channel number.
				send_string((void *)g.spc, echo_str);
				break;
				}
			
			ptr++;
			if (*ptr == '=') // We're going to set the OSW
				{
				ptr++; // Move past the = sign.
				if (*ptr == '~') // Clear bits
					{
					ptr++;
					if (*ptr == 'h' || *ptr == 'H') ptr++;
					uval = (unsigned int)htoi(ptr); // Value always hex
				
					if (ch_num == 1) OSW1 &= ~uval;
					else if (ch_num == 2) OSW2 &= ~uval; // Not implemented
					else if (ch_num == 3) OSW3 &= ~uval; // Not implemented
					}
				else // Set bits
					{	
					if (*ptr == 'h' || *ptr == 'H') ptr++;
					uval = (unsigned int)htoi(ptr); // Value always hex
			
					if (ch_num == 1) OSW1 |= uval;
					else if (ch_num == 2) OSW2 |= uval; // Not implemented
					else if (ch_num == 3) OSW3 |= uval; // Not implemented
					}
				}	
			else // We're going to get the current state of OSW
				{
				if (ch_num == 1) sprintf(echo_str, "O_OSW1=%x\n", OSW1);
				else if (ch_num == 2) sprintf(echo_str, "O_OSW2=%x\n", OSW2); // Not implemented
				else if (ch_num == 3) sprintf(echo_str, "O_OSW3=%x\n", OSW3); // Not implemented	
				
				send_string((void *)g.spc, echo_str); // Transmit the result.
				}	
			break;	

		// Many more to come	
		default: // Invalid operational command
			sprintf(echo_str,"?%s\n",orig_cmd_str);
			send_string((void *)g.spc, echo_str);
			break;			
		}
	}





void save_vars_to_flash(void)
	{
	int x;
	unsigned short temp_cprog[((MAX_STEP+1)*20)];

	// First buffer the saved cprog before erasing flash sector
	for (x=0; x<=((MAX_STEP+1)*20); x++) 
		temp_cprog[x] = ((unsigned short int *)CPROG_FLASH_ADDR)[x];

	// Now erase flash sector
	nr_flash_erase_sector(GM_FlashBase, (unsigned short int *)VAR_CPROG_FLASH_SECTOR_ADDR);
	
	// Now write variables to flash
	nr_flash_write_buffer(GM_FlashBase, (unsigned short int *)VAR_FLASH_ADDR, 
								(unsigned short int *)vtg.v, ((MAX_VAR+1)*2));

	// Then the buffered control program back to flash
	nr_flash_write_buffer(GM_FlashBase, (unsigned short int *)CPROG_FLASH_ADDR, 
								temp_cprog, ((MAX_STEP+1)*20));
	}




void reload_vars_from_flash(void)
	{
	int x;
	unsigned short *varptr = (unsigned short *)VAR_FLASH_ADDR;

	for (x=0; x<=((MAX_VAR+1)*2); x++) // ((MAX_VAR+1)*2) is the # of var halfwords saved in flash
		{
		((unsigned short *)vtg.v)[x] = varptr[x];
		}
	}



int initialize_axes(void)
	{
	np_pio *misc_out_pio = (np_pio *)na_misc_outs;
	//np_pio *axis1_interrupt = na_axis1_int;
	np_pio *ls_pio = na_ls_int_input;
	np_pio *axis1_div = (np_pio *)na_axis1_div_pio;
	np_pio *axis2_div = (np_pio *)na_axis2_div_pio;
	int axis_num=1, y;
	unsigned int temp=0;

	// Associate pvu's with their respective axis
	axis[1].pvu = va_pv1;
	axis[2].pvu = va_pv2;

	////////////////////////// First begin with axis 1 //////////////////////////////
	SET_PV_PLOAD(0);
	axis[axis_num].dac_channel = AXIS1_DAC_CHANNEL;
	convert_uS_to_timer(SLOW_LOOP_TIME, (unsigned int *)&SLOW_LOOP);
	convert_uS_to_timer(FAST_LOOP_TIME, (unsigned int *)&FAST_LOOP);
	
	// Set the quadrature divider
	temp = CONFIG_WORD2 & 0x0000000f;
	axis1_div->np_piodata = temp;
	
	// Set the velocity timer freq based on config word 1
	if (CONFIG_WORD1 & VEL1_CTR_DIV_BY2) {SET_PV_VCLKDIV(0x1);}
	else if (CONFIG_WORD1 & VEL1_CTR_DIV_BY4) {SET_PV_VCLKDIV(0x2);}
	else if (CONFIG_WORD1 & VEL1_CTR_DIV_BY8) {SET_PV_VCLKDIV(0x4);}
	else if (CONFIG_WORD1 & VEL1_CTR_DIV_BY16) {SET_PV_VCLKDIV(0x8);}
	else {SET_PV_VCLKDIV(0x1);}

	// Set v301 (VEL1_CLK_FREQ), timer 1 clk freq.
	VEL1_CLK_FREQ = axis[axis_num].vel_ctr_freq;
	VEL1_MUL = axis[axis_num].vel_multiplier;

	
	// Set pv1 limit switches
	WR_PVLS1 = LS1_1_SETPOINT;
	WR_PVLS2 = LS2_1_SETPOINT;
	WR_PVLS3 = LS3_1_SETPOINT;
	WR_PVLS4 = LS4_1_SETPOINT;
	WR_PVLS5 = LS5_1_SETPOINT;
	WR_PVLS6 = LS6_1_SETPOINT;
	WR_PV_GP_POS1 = 0x7fffff;
	WR_PV_GP_POS2 = 0x7fffff;

	ADC_CHANNEL_FOR_LVDT = AXIS1_ADC_CHANNEL_FOR_LVDT_SETTING;
	VALVE_TYPE = AXIS1_VALVE_TYPE;
		
	//SET_PV_IMASK(0xf00);
	//////////////////////////////////////////////////////////////////////////////	


	/////////////////////////// Now do axis 2 ////////////////////////////////////
	axis_num=2;

	SET_PV_PLOAD(0);
	axis[axis_num].dac_channel = AXIS2_DAC_CHANNEL;
	convert_uS_to_timer(SLOW_LOOP_TIME_2, (unsigned int *)&SLOW_LOOP);
	convert_uS_to_timer(FAST_LOOP_TIME_2, (unsigned int *)&FAST_LOOP);
	
	// Set the quadrature divider
	temp = (CONFIG_WORD2 & 0x000000f0) >> 4;
	axis2_div->np_piodata = temp;

	// Set the velocity timer freq based on config word 1
	if (CONFIG_WORD1 & VEL2_CTR_DIV_BY2) {SET_PV_VCLKDIV(0x1);}
	else if (CONFIG_WORD1 & VEL2_CTR_DIV_BY4) {SET_PV_VCLKDIV(0x2);}
	else if (CONFIG_WORD1 & VEL2_CTR_DIV_BY8) {SET_PV_VCLKDIV(0x4);}
	else if (CONFIG_WORD1 & VEL2_CTR_DIV_BY16) {SET_PV_VCLKDIV(0x8);}
	else {SET_PV_VCLKDIV(0x1);}

	// Set v310 (VEL2_CLK_FREQ), timer 2 clk freq.
	VEL2_CLK_FREQ = axis[axis_num].vel_ctr_freq;
	VEL2_MUL = axis[axis_num].vel_multiplier;

	ADC_CHANNEL_FOR_LVDT = AXIS2_ADC_CHANNEL_FOR_LVDT_SETTING;
	VALVE_TYPE = AXIS2_VALVE_TYPE;
	/////////////////////////////////////////////////////////////////////////////////////

	// Now finish up
	PULSE_PERIPHERAL_RESET;
					
	// Clear limit switch status indicators
	//for (y=0; y<6; y++)
	//	ls_stat[y] = 0;

	//OSW1 &= TURN_OFF_LS_AND_ZSPEED_OUTPUTS_MASK; // Clear ls and zspeed outputs 
	TURN_OFF_LS_AND_ZSPEED_OUTPUTS;
	prev_osw1 = 0xffffffff; // Force osw1 to be written

	ls_pio->np_pioedgecapture = 0;
	ls_pio->np_piointerruptmask = 0x3f; // Enable limit switches

	#if BUILD_FOR_CONTROL
		// update the control parameter array for each channel based on the variable settings
		update_control_parameters();
	#endif
	}



void ram_clear(void)
	{
	int y;

	for (y=0; y<=3500; y++)
		{
		data_sample[y].ana_ch1 = 0;
		data_sample[y].ana_ch2 = 0;
		data_sample[y].ana_ch3 = 0;
		data_sample[y].ana_ch4 = 0;
		data_sample[y].ana_ch5 = 0;
		data_sample[y].ana_ch6 = 0;
		data_sample[y].ana_ch7 = 0;
		data_sample[y].ana_ch8 = 0;
		data_sample[y].vel_count_q1 = 0xffffffff;
		data_sample[y].vel_count_q2 = 0xffffffff;
		data_sample[y].vel_count_q3 = 0xffffffff;
		data_sample[y].vel_count_q4 = 0xffffffff;
		data_sample[y].isw1 = 0;
		data_sample[y].isw4 = 0;
		data_sample[y].osw1 = 0;
		data_sample[y].one_ms_timer = 0;
		data_sample[y].position = 0;
		data_sample[y].sample_num = 0;
		}

	}




// This isr must be enabled after reaching min. stroke length.  If the cylinder bounces back
// one X4 count, zspeed will be enabled.  This isr has a higher priority than the PB isr.
void bounceback_isr(int context)
	{
	np_pio *bounceback_pio = na_bounceback_int;
	np_pio *axis1_interrupt = na_axis1_int;

	bounceback_pio->np_pioedgecapture = 0;

	SET_ZSPEED;
	//printf("{1}\n"); // For test
	comp_shot_parameters.eos_pos = pb_ctr;  // The previous sample will be the last sample,
	if (ZSPEED_OUTPUT_ENABLED) ZSPEED_OUTPUT_ON;

	// Need to mask out both PB and bounceback interrupts here
	bounceback_pio->np_piointerruptmask = 0;
	axis1_interrupt->np_piointerruptmask = 0;
	}


// Any change in any of the 6 ls status bits will trigger this interrupt.  The
// ls_status byte will contain the correct state of the ls, either on or off.  This routine
// simply sets or clears a given ls based on the status bits.
void ls_service_isr(int context)
	{
	np_pio *ls_pio = na_ls_int_input;
	unsigned char ls_status;

	ls_status = ls_pio->np_piodata;
	ls_pio->np_pioedgecapture = 0;

	//printf("*0x%x", ls_status); // For test
	if (ls_status & 0x01) LS1_1_ON; else LS1_1_OFF;
	if (ls_status & 0x02) LS2_1_ON; else LS2_1_OFF;
	if (ls_status & 0x04) LS3_1_ON; else LS3_1_OFF;
	if (ls_status & 0x08) LS4_1_ON; else LS4_1_OFF;
	if (ls_status & 0x10) LS5_1_ON; else LS5_1_OFF;
	if (ls_status & 0x20) LS6_1_ON; else LS6_1_OFF;
	}



// Note:  There is an issue with pb data collection.  After min stk len is passed and
// tb sampling begins, tb sample can overwrite pb data that has already been saved
// because the sample counter is not updated once a tb sample has begun.  If multiple
// pb samples occur while a tb sample is also active, tb will not be updating its sample
// counter resulting in pb data being written over.  A quick fix was to take out writing
// the Q1-Q4 data during time based so at least the velocity data is correct.  If some
// of the other data is written over, it is not noticable in most cases.  Still, some
// way around this must be found.  An attempt was made to mask out pb interrupts until
// tb data has been written and flagging that we did this.  Pb could then skip sampling the
// analogs and some other data as it would have already been written to the correct location.
// However, when this was tried, a lot of spurious interupts were generated, resetting the
// board into GERMS mode.  Why? - I don't know.  Will have to investigate.
void pos_based_monitor_isr(int context)
	{
	unsigned char y;
	int pos=0, vel=0, pos_sample=0, axis_num=1;
	volatile unsigned short int *halfword_ptr;
	np_pio *axis1_interrupt = na_axis1_int;
	np_pio *misc_ins_pio = na_misc_ins;
	np_pio *bounceback_pio = na_bounceback_int;

	//axis1_interrupt->np_pioedgecapture = 0;
	
	if (MONITOR_ENABLE && !(ZSPEED))
		{
		//printf("$"); // For test

		// Send command to convert analog channels.  If command register already set, a
		// conversion is already in progress, nothing else to do.  Writing any non-zero
		// value into the command register will initiate an A/D loop.			
		AD_CMD1 = 1; 

		pos = RD_PV_POS;
		vel = RD_PV_VEL;
		
		vtg.last_pb_time = RD_PV_RAW_VEL_CTR;

		sample_ctr = ++pb_ctr; 
		tb_ctr = 1;
		tb_time_interval_ctr = 0; // For tb intervals > 1 mS
		
		// We must get the latched velocity data saved as quickly as possible
		// otherwise another X4 pulse could come in and change something before we get
		// it saved.  We might as well go ahead and save everything else now before
		// doing the A/D loop. 
		// These items we want to update every pb sample, even if a time based sample
		// has occured.  We can't mooch time based data here.
		data_sample[sample_ctr].vel_count_q1 = RD_PV_LATCHED_Q1;
		data_sample[sample_ctr].vel_count_q2 = RD_PV_LATCHED_Q2;
		data_sample[sample_ctr].vel_count_q3 = RD_PV_LATCHED_Q3;
		data_sample[sample_ctr].vel_count_q4 = RD_PV_LATCHED_Q4;
		data_sample[sample_ctr].sample_num = 0; //pb_ctr;
		data_sample[sample_ctr].position = pos;
		data_sample[sample_ctr].one_ms_timer = ONE_MS_COUNTER;	

		// Now compute soft velocity
		//SOFT_VELOCITY = data_sample[sample_ctr].vel_count_q1 - data_sample[sample_ctr-1].vel_count_q1;			

		if (!(PAST_MIN_STK_LEN))
			{
			if (pos >= MIN_STROKE_LEN)
				{
				SET_PAST_MIN_STK_LEN;

				// Enable bounceback detection
				bounceback_pio->np_pioedgecapture = 0;
				bounceback_pio->np_piointerruptmask = 1;
				}
			}
				
		if (vel <= vel_eos_enable)
			SET_PAST_MIN_VEL_EOS; // faster than velocity enable point 
			//SET_PAST_MIN_VEL_EOS; // For test
		
	//#if 0
		// If we have just did a time based loop, the analog data has already
		// been written by the time based loop.  No need to do it again.
		// Also, the input and output status has already been written, no need to do
		// that again either.
		// Since the time based loop has incremented the sample counter, 
		// the analog and I/O data is already written in the correct location for 
		// this pb sample!
	  
			{
			data_sample[sample_ctr].isw1 = ISW1;
			data_sample[sample_ctr].isw4 = (unsigned int)(ISW4);
			data_sample[sample_ctr].osw1 = OSW1;

			// Get A/D data now
			while(AD_CMD1); // Wait until A/D loop completes
			data_sample[sample_ctr].ana_ch1 = shared_memory[0];
			data_sample[sample_ctr].ana_ch2 = shared_memory[1];
			data_sample[sample_ctr].ana_ch3 = shared_memory[2];
			data_sample[sample_ctr].ana_ch4 = shared_memory[3];
			if (CONV_HI_CHANNELS) // Copy over high channel data
				{
				data_sample[sample_ctr].ana_ch5 = shared_memory[16];
				data_sample[sample_ctr].ana_ch6 = shared_memory[17];
				data_sample[sample_ctr].ana_ch7 = shared_memory[18];
				data_sample[sample_ctr].ana_ch8 = shared_memory[19];
				}
			else  // Copy channels 5 - 8.
				{
				data_sample[sample_ctr].ana_ch5 = shared_memory[4];
				data_sample[sample_ctr].ana_ch6 = shared_memory[5];
				data_sample[sample_ctr].ana_ch7 = shared_memory[6];
				data_sample[sample_ctr].ana_ch8 = shared_memory[7];
				}
			}
	//#endif	

		if ((PAST_MIN_STK_LEN) && (PAST_MIN_VEL_EOS))
			{
			// Check for zspeed
			if (vel > vel_eos && !(ZSPEED)) // ZSPEED!!!
				{ 
			    SET_ZSPEED;
				//printf("{2}\n"); // For test
				comp_shot_parameters.eos_pos = pb_ctr; // This will be the last sample
				//comp_shot_parameters.eos_pos = 200; // For test
				if (ZSPEED_OUTPUT_ENABLED) ZSPEED_OUTPUT_ON;  

				//WR_PV_GP_POS1 = 0x7fffff;
				//WR_PV_GP_POS2 = 0x7fffff;

				// We don't want any more PB or bounceback interrupts at this point,
				// mask them out.
				bounceback_pio->np_piointerruptmask = 0;
				axis1_interrupt->np_piointerruptmask = 0;
				}
			else
				{
				// Clear the edge capture register here.
				axis1_interrupt->np_pioedgecapture = 0;
				}
			}
		else 
			{
			// Clear the edge capture register here also.
			axis1_interrupt->np_pioedgecapture = 0;
			}
		}
	}




void time_based_monitor_isr(int context)
	{
	//unsigned char istatus;
	int pos=0, vel=0, axis_num=1, input_state;
	volatile unsigned short int *halfword_ptr;
	np_timer *one_ms_timer = na_one_ms_timer;
	//np_spi *adc_spi = na_adc_spi;
	//np_pio *sel_hi_channels = na_sel_ana17_20;
	np_pio *opto_control = na_opto_control;
	np_pio *opto_data = na_opto_data;
	//np_pio *axis1_interrupt = na_axis1_int;
	np_pio *misc_ins_pio = na_misc_ins;
	//vp_pvu *pv1 = va_pv1;
	np_pio *bounceback_pio = na_bounceback_int;
	np_pio *axis1_interrupt = na_axis1_int;
	np_pio *cont_int_output = na_cont_int_output;

		
	one_ms_timer->np_timerstatus = 0; // Clear timer int signal.
	ONE_MS_COUNTER++;
	
	//printf("*%d*",ONE_MS_COUNTER); // For test

	if (PAST_MIN_STK_LEN) // Prep for time based data sample
		{
		//printf("!"); // For test
		//if (tb_ctr == 2001) // Shots over, prep for end of data collection
		if (tb_ctr == (TIM_COLL_SAMPLES+1)) // Shot's over, prep for end of data collection
			{
			if (bisc_ctr < TIM_DEL_BISC)
				{
				bisc_ctr++; // Still waiting for time delay biscuit
				}
			else  // Data collection is complete!!!
				{
				// Now compute biscuit size
				comp_shot_parameters.biscuit_size = TOTAL_STROKE_LEN - (RD_PV_POS);
		
				CLR_PAST_MIN_STK_LEN;
				CLR_MONITOR_ENABLE;
				CLR_PAST_MIN_VEL_EOS;
				CLR_ZSPEED; 
				SET_SHOT_COMPLETE;
				if (ZSPEED_OUTPUT_ENBL) ZSPEED_OUTPUT_OFF;
				//printf("ISR, shot complete\n"); // For test
				}
			//printf("@"); // For test
			}
		else if (++tb_time_interval_ctr == timed_coll_interval) // Assume timed_coll_interval is mS
			{
			tb_time_interval_ctr = 0;

			pos = RD_PV_POS;
			vel = RD_PV_VEL;

			// Send command to convert analog channels.  If command register already set, a
			// conversion is already in progress, nothing else to do.  Writing any non-zero
			// value into the command register will initiate an A/D loop.			
			AD_CMD1 = 2; 

			if (!(ZSPEED))
				{
				// When tb_ctr exceeds pb_ctr + vtg.counts_to_assume_zspeed, 
				// force a ZSPEED.
				//if ((pb_ctr + vtg.counts_to_assume_zspeed) <= tb_ctr)
				//if (RD_PV_RAW_VEL_CTR > (vtg.last_pb_time + vtg.counts_to_assume_zspeed))
				if (tb_ctr == vtg.counts_to_assume_zspeed)
					{
					SET_ZSPEED;
					//printf("{3}\n"); // For test
					if (ZSPEED_OUTPUT_ENABLED) ZSPEED_OUTPUT_ON;
					comp_shot_parameters.eos_pos = pb_ctr; 
					bounceback_pio->np_piointerruptmask = 0;
					axis1_interrupt->np_piointerruptmask = 0;
					}
				}

			//axis1_interrupt->np_piointerruptmask = 0;
			
			// add  pb_ctr+tb_ctr every write.  If pb samples are
			// comming in, we won't write over already saved data.		
			data_sample[pb_ctr+tb_ctr].sample_num = 0; //pb_ctr;
			data_sample[pb_ctr+tb_ctr].position = pos; 

		// Don't write the quadrant data during tb.  Velocity spikes can occur.
		// This data is not needed in pb anyway.
			//data_sample[pb_ctr+tb_ctr].vel_count_q1 = RD_PV_LATCHED_Q1; 
			//data_sample[pb_ctr+tb_ctr].vel_count_q2 = RD_PV_LATCHED_Q2; 
			//data_sample[pb_ctr+tb_ctr].vel_count_q3 = RD_PV_LATCHED_Q3; 
			//data_sample[pb_ctr+tb_ctr].vel_count_q4 = RD_PV_LATCHED_Q4; 
			data_sample[pb_ctr+tb_ctr].isw1 = ISW1; 
			data_sample[pb_ctr+tb_ctr].isw4 = (unsigned int)(ISW4); 
			data_sample[pb_ctr+tb_ctr].osw1 = OSW1; 
			data_sample[pb_ctr+tb_ctr].one_ms_timer = ONE_MS_COUNTER; 

			// Get A/D data now
			while(AD_CMD1); // Wait until A/D loop completes
			data_sample[pb_ctr+tb_ctr].ana_ch1 = shared_memory[0]; 
			data_sample[pb_ctr+tb_ctr].ana_ch2 = shared_memory[1]; 
			data_sample[pb_ctr+tb_ctr].ana_ch3 = shared_memory[2]; 
			data_sample[pb_ctr+tb_ctr].ana_ch4 = shared_memory[3]; 

			if (CONV_HI_CHANNELS) // Copy over high channel data
				{
				data_sample[pb_ctr+tb_ctr].ana_ch5 = shared_memory[16]; 
				data_sample[pb_ctr+tb_ctr].ana_ch6 = shared_memory[17]; 
				data_sample[pb_ctr+tb_ctr].ana_ch7 = shared_memory[18]; 
				data_sample[pb_ctr+tb_ctr].ana_ch8 = shared_memory[19]; 
				}
			else // Copy channels 5 - 8.
				{
				data_sample[pb_ctr+tb_ctr].ana_ch5 = shared_memory[4]; 
				data_sample[pb_ctr+tb_ctr].ana_ch6 = shared_memory[5]; 
				data_sample[pb_ctr+tb_ctr].ana_ch7 = shared_memory[6]; 
				data_sample[pb_ctr+tb_ctr].ana_ch8 = shared_memory[7]; 
				}
			tb_ctr++;

			//axis1_interrupt->np_piointerruptmask = 1;	
			//printf("*"); // For test
			//printf("[%d-%d]", pb_ctr, tb_ctr); // For test
			}
		}
	
	
	quit: ;
	// Check if need to write Opto outputs.  We'll do this every mS or whatever the
	// interval is - most of the time it will be 1mS.

	// Ground bounce was found to be a big problem with the Dual Opto interface board.
	// Simply writing a value to the 16 bit latches (without pulsing the wr signal) can
	// cause a large ground spike, especially if there is a large change in the bit pattern,
	// such as from all zeros to all ones.  By placing 0x5555 on the output latches
	// before writing the actual value, ground bounce was greatly reduced as shown on an
	// O-scope.  The same thing is done during Opto reads.  Since the read value is also
	// placed on the inputs to the output latches, the same potential for ground bounce
	// exists when reading the Opto inputs.  By placing 0x5555 on the output latches
	// before each read, the O-scope shows ground bounce to be greatly reduced.
	if (OSW1 != prev_osw1)
		{
		opto_control->np_piodata = 0; // Clear read/write lines
		halfword_ptr = (unsigned short int *)&OSW1;
		opto_data->np_piodirection = 0xffff;

 		opto_data->np_piodata = 0x5555; // To reduce ground bounce
		opto_data->np_piodata = ~halfword_ptr[0]; // Low half word of OSW1
		opto_control->np_piodata = 0x2; // wr monitor outputs
		opto_control->np_piodata = 0;	// Clear write lines

		opto_data->np_piodata = 0x5555;
		opto_data->np_piodata = ~halfword_ptr[1]; // Upper half word of OSW1
		opto_control->np_piodata = 0x8; // wr control outputs
		//opto_control->np_piodata = 0;	// Clear write lines
			
		prev_osw1 = OSW1;
		}
		
	// Always read Opto inputs and update ISW1
	halfword_ptr = (unsigned short int *)&ISW1;

	opto_control->np_piodata = 0; // Clear read/write lines
	opto_data->np_piodirection = 0xffff;
	opto_data->np_piodata = 0x5555; // To reduce ground bounce

	opto_data->np_piodirection = 0; // Set all bits to read
	
	opto_control->np_piodata = 0x1; // rd mon inputs
	halfword_ptr[0] = ~opto_data->np_piodata; // Low half word of ISW1
		
	opto_control->np_piodata = 0;
	opto_data->np_piodirection = 0xffff;
	opto_data->np_piodata = 0x5555;
		
	opto_data->np_piodirection = 0;
	opto_control->np_piodata = 0x4; // rd cont inputs
	halfword_ptr[1] = ~opto_data->np_piodata; // Upper half word of ISW1

	#if BUILD_FOR_CONTROL
		// See if any control interrupts need to be generated.  We will generate the
		// signals to cause a control interrupt based on block time out or any one or both
		// of 2 I/O numbers that can be checked.
		if (vtg.blk_timeout_time)
			{
			if (vtg.blk_timeout_time == ONE_MS_COUNTER)
				{
				SET_BLOCK_TIMEOUT_INTERRUPT; // Control isr must clear this
				}
			}

		// Check I/Os and generate a control interrupt if necessary.
		if (vtg.io_int1)
			{
			chk_discrete_input(vtg.io_int1, &input_state);
			if (input_state == vtg.on_off1) 
				{
				SET_IO_1_INTERRUPT; // Control isr must clear this
				}
			}

		if (vtg.io_int2)
			{
			chk_discrete_input(vtg.io_int2, &input_state);
			if (input_state == vtg.on_off2) 
				{
				SET_IO_2_INTERRUPT; // Control isr must clear this
				}
			}
	#endif
	}




// Isr to reset watchdog timer.  This triggers every 500mS. The watchdog timer will 
// timeout in 550mS.  If that were to happen the board would reset.  By writing the WD 
// timer's periodl register every 500mS, we should keep the watchdog well behaved.
// 
// If BUILD_FOR_CONTROL, the watchdog timer never starts.  This timer will operate
// in monitor only mode to send back op data packets if desired.
void reset_watchdog(int context)
	{
	static int ctr;
	int axis_num=1;
	np_pio *misc_ins_pio = na_misc_ins;
	np_timer *wd_timer = na_watchdog;
	np_timer *wd_rst_timer = na_WD_rst_timer;
			

	wd_rst_timer->np_timerstatus = 0; // Clear timer int signal.

	#if BUILD_FOR_CONTROL
		wd_timer->np_timerperiodl = 0x55; // Write anything to periodl to reset watchdog.
	#endif

	//// For test, this will allow the WD to time out 10 seconds after reset.
	//if (vtg.tst_ctr < 20)
	//	{
	//	printf("Resetting WD timer, time %d\n", vtg.tst_ctr);
	//	wd_timer->np_timerperiodl = 0x55; // Write anything to periodl to reset watchdog.
	//	vtg.tst_ctr++; //for test
	//	}

	if (vtg.local_wants_op_packets || vtg.plant_wide_wants_op_packets)
		{
		// If==1, Send only one
		// If==2, Keep sending
		
		//#if 0
		// first transfer our data to the structure.
		//for (x=1; x<5; x++) op_data_packet.dac[x] = vtg.dac_word[x];
		if (!SHOT_IN_PROGRESS)
			{
			AD_CMD1 = 6;
			}

		get_dac(1, VOLTAGE_X_1000, &op_data_packet.dac[0]);
		get_dac(2, VOLTAGE_X_1000, &op_data_packet.dac[1]);
		get_dac(3, VOLTAGE_X_1000, &op_data_packet.dac[2]);
		get_dac(4, VOLTAGE_X_1000, &op_data_packet.dac[3]);

		op_data_packet.pos = RD_PV_POS;
		op_data_packet.vel = (axis[axis_num].vel_ctr_freq/RD_PV_VEL); // Vel in pps
		op_data_packet.isw1 = ISW1;
		op_data_packet.isw4 = ISW4;
		op_data_packet.osw1 = OSW1;
		op_data_packet.mon_status_word1 = MON_STATUS_WORD;
		op_data_packet.config_word1 = CONFIG_WORD1;
		#if BUILD_FOR_CONTROL
			op_data_packet.status_word1 = CONT_STATUS_WORD;
			op_data_packet.warning = vtg.saved_warning;
			op_data_packet.fatal_error = FATAL_ERROR_WORD;
			op_data_packet.blk_no = vtg.prog_ctr;
		#endif

		while (AD_CMD1);
		op_data_packet.analog[0] = RESULT1;
		op_data_packet.analog[1] = RESULT2;
		op_data_packet.analog[2] = RESULT3;
		op_data_packet.analog[3] = RESULT4;
		op_data_packet.analog[4] = RESULT5;
		op_data_packet.analog[5] = RESULT6;
		op_data_packet.analog[6] = RESULT7;
		op_data_packet.analog[7] = RESULT8;
		op_data_packet.analog[8] = RESULT17;
		op_data_packet.analog[9] = RESULT18;
		op_data_packet.analog[10] = RESULT19;
		op_data_packet.analog[11] = RESULT20;
		//#endif
		
		#if 0
		// For test, fill it all with printable chars. terminated with LF's (0x0a).
		op_data_packet.dac[0] = 0x0a303030;
		op_data_packet.dac[1] = 0x0a313131;
		op_data_packet.dac[2] = 0x0a323232;
		op_data_packet.dac[3] = 0x0a333333;
		op_data_packet.analog[0] = 0x350a;
		op_data_packet.analog[1] = 0x360a;
		op_data_packet.analog[2] = 0x370a;
		op_data_packet.analog[3] = 0x380a;
		op_data_packet.analog[4] = 0x390a;
		op_data_packet.analog[5] = 0x3a0a;
		op_data_packet.analog[6] = 0x3b0a;
		op_data_packet.analog[7] = 0x3c0a;
		op_data_packet.analog[8] = 0x3d0a;
		op_data_packet.analog[9] = 0x3e0a;
		op_data_packet.analog[10] = 0x3f0a;
		op_data_packet.analog[11] = 0x400a;
		op_data_packet.pos = 0x0a414141;
		op_data_packet.vel = 0x0a424241; // Vel in pps
		op_data_packet.isw1 = 0x0a434343;
		op_data_packet.isw4 = 0x0a444444;
		op_data_packet.osw1 = 0x0a454545;
		op_data_packet.status_word1 = 0x0a464646;
		op_data_packet.config_word1 = 0x0a474747;
		op_data_packet.warning = 0x0a484848;
		op_data_packet.fatal_error = 0x0a494949;
		op_data_packet.blk_no = 0x0a0a4a4a;
		#endif
		
		if (vtg.plant_wide_wants_op_packets)
			{
			// If connected, transmit here
			//printf("plant_wide wants data packets\n"); // For test
			if (plant_net_settings.connected)
				{
				// If tcp_xmit() is tied up by somebody else, we'll simply not send a pcaket this time.
				//tcp_xmit(plant_net_settings.tcp_plug, (char *)&op_data_packet, size); // For test
				tcp_xmit(plant_net_settings.tcp_plug, (char *)&op_data_packet, vtg.op_packet_size);
				if (vtg.plant_wide_wants_op_packets == 1) vtg.plant_wide_wants_op_packets = 0; // We only wanted one
				}
			}
		if (vtg.local_wants_op_packets)
			{
			//printf("local wants data packets\n"); // For test
			if (local_net_settings.connected)
				{
				// If tcp_xmit() is tied up by somebody else, we'll simply not send a pcaket this time.
				//tcp_xmit(local_net_settings.tcp_plug, (char *)&op_data_packet, size); // For test
				tcp_xmit(local_net_settings.tcp_plug, (char *)&op_data_packet, vtg.op_packet_size);
				if (vtg.local_wants_op_packets == 1) vtg.local_wants_op_packets = 0;
				}
			}
		//op_data_packet.sizeof_this_struct = size; // For test  restore struct size
		}
	else
		ctr=0;
	}




inline void monitor_supervisor(volatile unsigned short int *shot_num)
	{
	int y, axis_num=1;
	volatile unsigned short int *halfword_ptr;
	//vp_pvu *pv1 = va_pv1;
	np_pio *misc_ins_pio = na_misc_ins;
	np_pio *misc_out_pio = na_misc_outs;
	np_pio *axis1_interrupt = na_axis1_int;
	np_pio *bounceback_pio = na_bounceback_int;
		
	// 1 of 3 things can happen here:
	//
	// 1, We get cycle start and set currently timing.
	// 2, We move past min stk len and clear currently timing.
	// 3, We get shot complete.	
	
	if (SHOT_COMPLETE)
		{
		CLR_SHOT_COMPLETE;
		CLR_MONITOR_ENABLE;
		CLR_CS_DETECTED;
				

		printf("Shot complete - eos=%d\n", comp_shot_parameters.eos_pos);
			
		// Reenable this to transmit automatically after each shot.		
		//#if 0
		if (local_net_settings.connected)
			{
			if (CONFIG_WORD1 & 0x10000000) // xmit enabled?
				{
				g.spc = &local_net_settings; // Set send_string() context	
				transmit_monitor_data(*shot_num);
				printf("transmitted to local net, shot_num %d\n", *shot_num);
				}
			}
		if (plant_net_settings.connected)
			{
			if (CONFIG_WORD1 & 0x20000000) // xmit enabled?
				{
				g.spc = &plant_net_settings; // Set send_string() context	
				transmit_monitor_data(*shot_num);
				printf("Transmitted to plant-wide net, shot_num %d\n", *shot_num);
				}
			}
		//#endif
		
		CLR_SHOT_IN_PROGRESS;

		#if MON_DEBUG	
				//printf("\n");
				//printf("Shot complete - vtg.gp_config_word = 0x%x\n", vtg.gp_config_word);
		#endif
		
		(*shot_num)++;
		
		g.spc = &local_net_settings; // Set back to local context

		#if MON_DEBUG
			// For test
			printf("Local plug=%d, connected=%d, rem_ip_add=%d, rem_port=%d\n", 
				local_net_settings.tcp_plug, local_net_settings.connected, local_net_settings.remote_ip_address, local_net_settings.remote_port);

			printf("Plant_wide plug=%d, connected=%d, rem_ip_add=%d, rem_port=%d\n", 
				plant_net_settings.tcp_plug, plant_net_settings.connected, plant_net_settings.remote_ip_address, plant_net_settings.remote_port);
		#endif
		}
	else if (CURRENTLY_TIMING)
		{
		if (PAST_MIN_STK_LEN)
			{
			CLR_CURRENTLY_TIMING; // We will complete a shot at this point
			}
		else if (ONE_MS_COUNTER > (unsigned int)TIME_OUT_PERIOD) // The dreaded 30 sec. timeout.
			{
			CLR_MONITOR_ENABLE;
			CLR_CURRENTLY_TIMING;
			CLR_CS_DETECTED;
			CLR_SHOT_IN_PROGRESS;
			bounceback_pio->np_piointerruptmask = 0;
			axis1_interrupt->np_piointerruptmask = 0;
				
			// Transmit a timeout message here
			printf("R:19 #Cycle start timeout\r\n");
			if (local_net_settings.connected)
				{
				g.spc = &local_net_settings; // Set send_string() context	
				send_string((void *)g.spc, "R:19 #Cycle start timeout\n");
				if (((s_tcp_status *)g.spc)->show_prompt) send_string((void *)g.spc, "+");
				}
			} 
		}
	else if (CS_PRESENT && !(MONITOR_ENABLE)) 
		{
		// Transmit a cycle start detected message here
		printf("CS detected!\n");
		#if MON_DEBUG
			//printf("vtg.gp_config_word = %x\n", vtg.gp_config_word);
			printf("OSW1 = %x\n", OSW1);
		#endif

		if (local_net_settings.connected)
			{
			g.spc = &local_net_settings; // Set send_string() context	
			send_string((void *)g.spc, "R:20 #Cycle start detected\n");
			if (((s_tcp_status *)g.spc)->show_prompt) send_string((void *)g.spc, "+");
			}

		// Set CS detected bit in config word.
				

		SET_CURRENTLY_TIMING;
		SET_CS_DETECTED;
		SET_SHOT_IN_PROGRESS;

		if (!vtg.first_cs_after_power_up)
			{
			comp_shot_parameters.cycle_time = ONE_MS_COUNTER;
			printf("# Cycle time = %dmS\n",comp_shot_parameters.cycle_time);
			}
		vtg.first_cs_after_power_up = 0;
		vtg.last_pb_time = 0; //  For zspeed detection within the time based monitor isr
			
		#if MON_DEBUG
			printf("Before reset, vel1 ctr is %d\n", RD_PV_RAW_VEL_CTR);
		#endif
		//DIS_PV_INT;
		initialize_axes();  // This will update the ls's and vclk_div

		ONE_MS_COUNTER = 0;
		pb_ctr = 0;
		bisc_ctr = 0;
		CLR_PAST_MIN_STK_LEN;
		CLR_PAST_MIN_VEL_EOS;
		
		// If a shot is in progress, this will clear it
		CLR_MONITOR_ENABLE;
		

		#if MON_DEBUG
			printf("After reset, vel1 ctr is %d\n", RD_PV_RAW_VEL_CTR);
		#endif

					
		// TIM_COLL_SAMPLES is the number of time collection points.
		// Right now, we are not using this parameter.
		// However to prep. for the 
		// day that we do, we'll limit it to a 
		// max of 2000 points or a min of 100 points.
		if (TIM_COLL_SAMPLES < 100) TIM_COLL_SAMPLES = 100;
		if (TIM_COLL_SAMPLES > 2000) TIM_COLL_SAMPLES = 2000;
			
		// Load the monitor setup items.
		vel_eos_enable = 0xffffffff/(MIN_VEL_EOS*VEL1_MUL);
		vel_eos = 0xffffffff/(VEL_SET_EOS*VEL1_MUL);
		
		// Convert Time collection interval to milliseconds.  If >= 1000, must be uS, convert to mS
		if (TIM_COLL_INTERVAL >= 1000) timed_coll_interval = TIM_COLL_INTERVAL / 1000;
		else timed_coll_interval = TIM_COLL_INTERVAL;
		
		
		// Calculate here the number of timed collection loops
		// to assume zspeed based vel eos setting.  Since there is not
		// an overflowing velocity counter and therefore no way to generate
		// a true zspeed signal, if the cylinder suddenly stops, zspeed
		// would never be generated.  Worse yet, if a stray position based
		// sample occurs well into timed collection, the data would be 
		// skewed.
		// vtg.counts_to_assume_zspeed = the number of time based samples that must
		// pass without a position based update before assuming zspeed.
		vtg.counts_to_assume_zspeed = (1000/timed_coll_interval)/VEL_SET_EOS;
		if (vtg.counts_to_assume_zspeed < 2) 
			vtg.counts_to_assume_zspeed = 2; // Set to at least 2.
		else if (vtg.counts_to_assume_zspeed > (int)(TIM_COLL_SAMPLES-10)) 
			vtg.counts_to_assume_zspeed = (TIM_COLL_SAMPLES-10); // Set somewhat short of the max time coll points.
	

		#if MON_DEBUG
			// Now let's see what we have, do some debug printing
			printf("Now this is what we have after CS:\n");
	
			printf("timed interval counts = %d\n", timed_coll_interval);
			printf("counts to assume zspeed = %d\n", vtg.counts_to_assume_zspeed);
			printf("vel eos = %d\n", vel_eos); 
		#endif	

		// Clear ram data collection arrays. 
		ram_clear();

		// Enable pos. and time interrupts
		//pos_int_bits->np_piointerruptmask = 0x1f;
		//tim_int_bit->np_piointerruptmask = 1;
			
		// Unmask ls interrupt
		//vtg.pv1_imask = 0xf00;
		//SET_PV1_IMASK(vtg.pv1_imask);

		bounceback_pio->np_piointerruptmask = 0; // Leave bounceback interrupt disabled
		axis1_interrupt->np_pioedgecapture = 0;
		axis1_interrupt->np_piointerruptmask = 1; // Enable PB interrupts

		SET_MONITOR_ENABLE;
		//printf("At end of loop, vtg.gp_config_word = %x\n", vtg.gp_config_word);
 		}
	}
			
			


void xmit_chunks(void)
	{
	int vel_increment, pos, x; 
	volatile char *data_ptr;
	np_pio *misc_ins_pio = na_misc_ins;

	
	while (SW9); // Stay here until button released
	nr_delay(300); // Debounce switch

	data_ptr = (char *)&data_sample[0]; // + sizeof(combined_sample); // Data begins at location [1] not [0].

	do
		{
		if (!local_net_settings.connected && !plant_net_settings.connected) // No connection
			{
			#if BUILD_FOR_POLLED_ETHERNET
				nr_plugs_idle();
			#endif
			}
		else
			{
			// xmit if we have a connection
			if (local_net_settings.connected)
				{
				// Send TCP_CHUNK_SIZE bytes of data
				g.spc = &local_net_settings;
				//tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, data_ptr, TCP_CHUNK_SIZE);
				if(nr_plugs_send(((s_tcp_status *)g.spc)->tcp_plug, (void *)data_ptr, TCP_CHUNK_SIZE, 0)) printf("xmit failed!!!\n\n");
				else printf("SUCCESS!!! transmitted to local net\n\n");
				}
			if (plant_net_settings.connected)
				{
				g.spc = &plant_net_settings; // Set send_string() context	
				//tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, data_ptr, TCP_CHUNK_SIZE);
				if(nr_plugs_send(((s_tcp_status *)g.spc)->tcp_plug, (void *)data_ptr, TCP_CHUNK_SIZE, 0)) printf("xmit failed!!!\n\n");
				else printf("SUCCESS!!! transmitted to local net\n\n");
				}
			//nr_delay(100);
			#if BUILD_FOR_POLLED_ETHERNET
				nr_plugs_idle();
			#endif
			}
		} while (!(SW9));
	
	while (SW9); // Wait for button to be released
	nr_delay(300); // Debounce switch
	}




#define SS_DIST 13		 // Inches
#define SHOT_END_POS 25  //  " " "
#define SS_VEL 10		 // Inches per second
#define FS_VEL 50		 //  "    "    "   "
#define TIME_TO_WAIT_BETWEEN_SHOTS 3000	 // Time in mS
#define NUM_OF_TIME_BASED_POINTS 2000
#define BISC_SIZE 1		 // Biscuit size inches


void xmit_fake_shots(void)
	{
	int vel_increment, pos=0, x; 
	int temp_q1=4000, temp_q2=5000, temp_q3=6000, temp_q4=7000;
	int ms=0, shot_num=1;
	unsigned int xmit_again;
	np_pio *misc_ins_pio = na_misc_ins;

	// Build data, this will be transmitted continously
	
	// Manufacture a data set (shot data), this will be transmitted continously
	// to the plot program.
	
	// Slow shot
	vel_increment = 16666666 / (SS_VEL*20);
	for (x=1; x<(SS_DIST*20)+1; x++)
		{
		temp_q1 += vel_increment;
		temp_q2 += vel_increment;
		temp_q3 += vel_increment;
		temp_q4 += vel_increment;
		pos += 4;
		ms += 5;

		data_sample[x].ana_ch1 = 409; // 1V 2000 PSI
		data_sample[x].ana_ch2 = 818 | 0x1000; // 2V 4000 PSI
		data_sample[x].vel_count_q1 = temp_q1;
		data_sample[x].vel_count_q2 = temp_q2;
		data_sample[x].vel_count_q3 = temp_q3;
		data_sample[x].vel_count_q4 = temp_q4;
		data_sample[x].one_ms_timer = ms;
		data_sample[x].position = pos;
		data_sample[x].sample_num = x;  
		}

	// Fast shot
	vel_increment = 16666666 / (FS_VEL*20);
	for (x=(SS_DIST*20)+1; x<(SHOT_END_POS*20)+1; x++)
		{
		temp_q1 += vel_increment;
		temp_q2 += vel_increment;
		temp_q3 += vel_increment;
		temp_q4 += vel_increment;
		pos += 4;
		ms += 1;

		data_sample[x].ana_ch1 = 600; 
		data_sample[x].ana_ch2 = 1000 | 0x1000; 
		data_sample[x].vel_count_q1 = temp_q1;
		data_sample[x].vel_count_q2 = temp_q2;
		data_sample[x].vel_count_q3 = temp_q3;
		data_sample[x].vel_count_q4 = temp_q4;
		data_sample[x].one_ms_timer = ms;
		data_sample[x].position = pos;
		data_sample[x].sample_num = x;  
		}

	// Time based data
	for (x=(SHOT_END_POS*20)+1; x<(SHOT_END_POS*20)+(NUM_OF_TIME_BASED_POINTS+2); x++)
		{
		ms += 1;

		data_sample[x].ana_ch1 = 800; 
		data_sample[x].ana_ch2 = 1000 | 0x1000; 
		data_sample[x].vel_count_q1 = temp_q1;
		data_sample[x].vel_count_q2 = temp_q2;
		data_sample[x].vel_count_q3 = temp_q3;
		data_sample[x].vel_count_q4 = temp_q4;
		data_sample[x].one_ms_timer = ms;
		data_sample[x].position = pos;
		data_sample[x].sample_num = x;  
		}

	// Computed shot parameters
	comp_shot_parameters.cycle_time = 4000;
	comp_shot_parameters.biscuit_size = BISC_SIZE*80;
	comp_shot_parameters.eos_pos = SHOT_END_POS*20;

	while (SW7); // Stay here until button released
	nr_delay(300); // Debounce switch

	do
		{
		if (!local_net_settings.connected && !plant_net_settings.connected) // No connection
			{
			#if BUILD_FOR_POLLED_ETHERNET
				nr_plugs_idle();
			#endif
			}
		else
			{
			// First xmit a CS message
			if (local_net_settings.connected)
				{
				g.spc = &local_net_settings; // Set send_string() context	
				send_string((void *)g.spc, "R:20 #Cycle start detected\n");
				if (((s_tcp_status *)g.spc)->show_prompt) send_string((void *)g.spc, "+");
				}
			if (plant_net_settings.connected)
				{
				g.spc = &local_net_settings; // Set send_string() context	
				send_string((void *)g.spc, "R:20 #Cycle start detected\n");
				if (((s_tcp_status *)g.spc)->show_prompt) send_string((void *)g.spc, "+");
				}
			
			nr_delay(500);

			// xmit if we have a connection
			if (local_net_settings.connected)
				{
				g.spc = &local_net_settings; // Set send_string() context	
				transmit_monitor_data(shot_num);
				printf("SUCCESS!!! transmitted to local net, shot_num %d\n\n", shot_num);
				}
			if (plant_net_settings.connected)
				{
				g.spc = &plant_net_settings; // Set send_string() context	
				transmit_monitor_data(shot_num);
				printf("SUCCESS!!! Transmitted to plant-wide net, shot_num %d\n\n", shot_num);
				}
	
			shot_num++;
			nr_delay(TIME_TO_WAIT_BETWEEN_SHOTS);
			#if BUILD_FOR_POLLED_ETHERNET
				nr_plugs_idle();
			#endif
			}
		} while (!(SW7));
	
	while (SW7); // Wait for button to be released
	nr_delay(300); // Debounce switch
	}


void dummy_o_scope_isr(int context)
	{
	}



//////////////////////////// CONTROL FUNCTIONS //////////////////////////////
#if BUILD_FOR_CONTROL

#include "control_blocks8.c"


// Interrupt driven control supervisor
// If context == 1, timer2 is the interrupt source, if == 0, the control interrupt pio 
// is the source.
void control_supervisor_isr(int context)
	{
	ub *block;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	np_pio *misc_out_pio = na_misc_outs;
	np_timer *timer2 = na_timer2;
	int axis_num=1, reenter=0, blk_exe_return_val=0, x, input_state;
	static int immed_in_process;
	unsigned short int int_data; // Only 9 bits are used
	char temp_str[81];

	int_data = control_int->np_pioedgecapture;
	control_int->np_pioedgecapture = 0; // We could clear out only the bit serviced, allowing other pending ints to be serviced as well
	
	// Very important!!!  Mask out any edge capture bits that have their interrupts masked.
	// Even if a bit's interrupt is masked, that bit will still register in the edge
	// capture register when it goes high!!! - even though it will not generate an interrupt.
	int_data &= control_int->np_piointerruptmask;
	// Now int_data reflects bits that have changed and are enabled to cause interrupts.

	// Clear all of them here, we'll only deal with one.  Since they are prioritized
	// beginning with bit 0 (highest priority), this should be OK.
	CLR_ALL_CONTROL_INTERRUPTS; 
	
	// Abort if master enable is off
	chk_discrete_input(MASTER_ENABLE_INPUT, &input_state);
	if (input_state == OFF)
		{
		#if CONTROL_INTERRUPT_DEBUG
			printf("CPS MASTER_ENABLE is off\n");
		#endif
		WARNING_WORD = MASTER_ENABLE_DROPPED_OUT;
		}
	else
		{
	//printf("$"); // For test
		//int_data = control_int->np_pioedgecapture;
		//control_int->np_pioedgecapture = 0; // We could clear out only the bit serviced, allowing other pending ints to be serviced as well
	
		// The function associated with the bit that caused the interrupt will be called.
		// This info is in the when_cont_isr structure.  The type of execution, IMMED or PROGRAM
		// along with the zone to go to are also contained in the structure.  The bits are
		// prioritized, 0 - 8 with 0 being the highest priority.  Any lower priority bits that
		// may also be set are cleared out and ignored.
		//
		// int_data bits:
		//
		// Block ending or start / stop items first.
		//   bit 0 == Stop the control program.
		//   bit 1 == Start/reset the control program.
		//   bit 2 == Block time out, always block ending, generate a warning.
		//   bit 3 == GP pos1, usually the block ending position.
		//   bit 4 == I/O #1 Usually a block ending I/O change.
		// Now non block ending items.
		//   bit 5 == GP pos2, general purpose (non-block ending) position based interrupt.
		//   bit 6 == I/O #2 Use for a non-block ending I/O change.
		//   bit 7 == The pb axis1_int, used here for accel control based on X1 counts, never block ending.
		//   bit 8 == Timer2 timeout.
		for (x=0; x<9; x++)
			{
			if (int_data & (1<<x))
				{
				#if CONTROL_INTERRUPT_DEBUG
					printf("CPS-Interrupt%d just triggered\n", x);
					//printf("CPS-when_cont_isr[%d].blk_to_execute = 0x%x\n", x, when_cont_isr[x].blk_to_execute);
					printf("CPS-when_cont_isr[%d].blk_id = 0x%x\n", x, when_cont_isr[x].blk_id);
					printf("CPS-when_cont_isr[%d].zone_to_goto = %d\n", x, when_cont_isr[x].zone_to_goto);
				#endif
			//printf("{%d, EC=0x%x IM=0x%x}", x, int_data, control_int->np_piointerruptmask); // For test
				//blk_exe_return_val = when_cont_isr[x].blk_to_execute(NULL, NULL, when_cont_isr[x].zone_to_goto);
				blk_exe_return_val = block_function_ptr[when_cont_isr[x].blk_id](NULL, NULL, when_cont_isr[x].zone_to_goto);
			//printf("{&}"); // For test

				if (blk_exe_return_val)
					{
					if (blk_exe_return_val == REENTER_ON_INTERRUPT)
						{
						reenter=1;
						#if CONTROL_INTERRUPT_DEBUG
							printf("CPS-Blk. executed at interrupt wants to reenter\n");
						#endif
						}
					else 
						{
						#if CONTROL_INTERRUPT_DEBUG
							printf("CPS-Blk. executed at interrupt had an execution error\n");
						#endif
						FATAL_ERROR_WORD=BLOCK_EXECUTION_ERROR;
						}
					}
				else
					{
					#if CONTROL_INTERRUPT_DEBUG
						printf("CPS-Blk. executed at interrupt finished\n");
					#endif
					}
					
				break;  // Go until the first match then break.
				}
			} // for loop		
		}

	//do
	while (!reenter && PROGRAM_LOADED && PROGRAM_RUNNING)
		{
		// Abort if master enable is off
		chk_discrete_input(MASTER_ENABLE_INPUT, &input_state);
		if (input_state == OFF)
			{
			WARNING_WORD = MASTER_ENABLE_DROPPED_OUT;
			}

		if (vtg.stack_ptr > 19)  // Stop control if stack size is overloaded
			{
			FATAL_ERROR_WORD=STACK_OVERFLOW;
			}
	
		if (FATAL_ERROR_WORD)
			{
			// Format the error message here.  Do it first, otherwise the program
			// block will be reset.
			sprintf(temp_str, "F_Cont. Fatal err:%d # prog_step=%d, blk_exe_return_val=%d\n", FATAL_ERROR_WORD, vtg.prog_ctr, blk_exe_return_val);
					
			INITIALIZE_CONTROL;  // This will clear any pending interrupts

			SET_FATAL_ERROR;
					
			out_dac(AXIS1_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 1 to the retract setting
			axis_num=2;
			out_dac(AXIS2_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 2 to zero
			TURN_OFF_ALL_CONT_OUTPUTS;
			CLR_PROGRAM_RUNNING;
			reenter=1; // We'll leave the isr and wait for someone to start (interrupt) us up again.

			// Generate wathcdog fault here
			PULSE_WATCHDOG_FAULT;	
		
			// If connected, transmit a message here
			if (plant_net_settings.connected)
				{
				if (CONFIG_WORD1 & 0x20000000) // xmit enabled?
					{
					g.spc = &plant_net_settings; // Set send_string() context	
					send_string((void *)g.spc, temp_str);
					}
				}
			if (local_net_settings.connected)
				{
				g.spc = &local_net_settings; // Deafult back to local	
				if (CONFIG_WORD1 & 0x10000000) // xmit enabled?
					{
					send_string((void *)g.spc, temp_str);
					}
				}

			break; // Leave the do loop
			}

		if (WARNING_WORD)
			{
			// Kind'a like a fatal error except program execution
			// will continue.

			// Format the warning message here.  Do it first to so that we have
			// the program info. before it is reset.
			sprintf(temp_str, "W_Cont. warning:%d # prog_step=%d\n", WARNING_WORD, vtg.prog_ctr);
			// For test
			//printf("W_Cont. warning:%d # prog_step=%d\n", WARNING_WORD, vtg.prog_ctr);
					
			
			vtg.saved_warning = WARNING_WORD; // Archive warning	
			INITIALIZE_CONTROL;
			SET_WARNING;
			set_clr_discrete_output(WARNING_OUTPUT, TURN_OFF); // Enable warning by turning off
			reenter=0; // We'll stay in the isr and begin executing the control prog. @ blk. #1								
					
			// If connected, transmit a message here
			if (plant_net_settings.connected)
				{
				if (CONFIG_WORD1 & 0x20000000) // xmit enabled?
					{
					g.spc = &plant_net_settings; // Set send_string() context	
					send_string((void *)g.spc, temp_str);
					}
				}
			if (local_net_settings.connected)
				{
				g.spc = &local_net_settings; // Deafult back to local	
				if (CONFIG_WORD1 & 0x10000000) // xmit enabled?
					{
					send_string((void *)g.spc, temp_str);
					}
				}
			}

		// Control supervisor here
		//
		// vtg.control_program[vtg.prog_ctr][0] is a ptr that points to the block function
		// to call.  This is set up during compile.
		
		#define LOCATION_OF_BLOCK_TO_EXECUTE vtg.control_program[vtg.prog_ctr][0]
		#define IMMED_EXE_BLK vtg.control_program[IMMED_EXECUTE_BLOCK][0]
		// Use ub as a generic block type

		// If a block is compiled for IMMED execution and the control prog. is running, 
		// we'll execute it here.  If the control prog. is not running, an IMMED block
		// will be initiated by the block compile function.
		if (IMMED_EXE_BLK) // There is an IMMED block to execute
			{
			if (!immed_in_process) // Initial entry into the block
				{
				immed_in_process=1;
				block = (ub *)&IMMED_EXE_BLK;
				//blk_exe_return_val = block->blk_to_execute((c_block)&IMMED_EXE_BLK, IMMED, 1);
				blk_exe_return_val = block_function_ptr[block->blk_id]((c_block)&IMMED_EXE_BLK, IMMED, 1);	
				if (blk_exe_return_val)
					{
					if (blk_exe_return_val == REENTER_ON_INTERRUPT) break; 
					else {FATAL_ERROR_WORD=BLOCK_EXECUTION_ERROR; continue;}
					}
				else // We finished the IMMED block without a reenter or error.
					{
					// Clear out the immed block location
					IMMED_EXE_BLK = NULL; // Proceed to normal program execution below.
					}
				}
			else  // We finished running an IMMED block after reentering 1 or more times.
				{
				IMMED_EXE_BLK = NULL; // Proceed to normal program execution below.
				}
			}
	
		// Normal program execution
		immed_in_process=0;
		block = (ub *)&LOCATION_OF_BLOCK_TO_EXECUTE;
		#if CONTROL_INTERRUPT_DEBUG
			//printf("CPS-block->blk_to_execute=0x%x\n", block->blk_to_execute);
			printf("CPS-block->blk_id=0x%x\n", block->blk_id);
			//for (x=0; x<10; x++)
			//	{
			//	printf("vtg.control_program[%d][%d]=0x%x\n", vtg.prog_ctr, x, vtg.control_program[vtg.prog_ctr][x]);
			//	}
		#endif
	//printf("[%d]", vtg.prog_ctr); // For test
		//blk_exe_return_val = block->blk_to_execute((c_block)&LOCATION_OF_BLOCK_TO_EXECUTE, PROGRAM, 1);	
		blk_exe_return_val = block_function_ptr[block->blk_id]((c_block)&LOCATION_OF_BLOCK_TO_EXECUTE, PROGRAM, 1);
	//printf("[*]"); // For test
		if (blk_exe_return_val)
			{
			if (blk_exe_return_val == REENTER_ON_INTERRUPT) break; 
			else FATAL_ERROR_WORD=BLOCK_EXECUTION_ERROR;
			}
		} //while (1);
	}




// Function to decode Control program commands
int parse_control_program_command(unsigned char *orig_cmd_str)
	{
	unsigned char echo_str[81], *ptr;
	
	//printf("Parse control code\n"); // For test
	ptr = orig_cmd_str; ptr++;
	switch(*ptr)
		{
		case 'F':
		case 'f':  
			// Save control program to flash.  Vars will also be saved
			save_cprog_to_flash();
			break;
			
		case 'H':
		case 'h':
			// Halt control program
			//stop_control();
			//printf(".H command \n"); // For test
			FATAL_ERROR_WORD = PROGRAM_STOPPED_BY_USER;  // This will stop the control program
			break;
			
		case 'G':
		case 'g':
			//printf(".G command \n"); // For test
			// Start the control program
			reset_control_program(CLEAR_SAVED_WARNING);
			break;
			
		case 'R':
		case 'r':
			// Reload control program from flash
			reload_cprog_from_flash();
			break;
				
		default: // Probably a line of control code to compile
			{
			if (*ptr >= '0' && *ptr <= '9') 
				// Line of control program to compile, must parse to find what type of block it is
				{
				do // Move ptr to 1st. valid block type char
					{
					if (*ptr == NULL) break; // Didn't find any valid block types
					else if (*ptr == 'U' || *ptr == 'V' || *ptr == 'D' || *ptr == 'P' || *ptr == 'J' || *ptr == 'E')
					   break;
					ptr++;	
					}while (1);
				}
			switch (*ptr)
				{
				case 'U':
					#if CONTROL_COMPILE_DEBUG
						printf("Ready to complile update block %s\n", orig_cmd_str+1);
					#endif
					if (update_block_compile(orig_cmd_str+1))
						{
						sprintf(echo_str, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, echo_str);
						}
					#if CONTROL_COMPILE_DEBUG
						printf("Update block has been compiled \n\n");
					#endif
					break;
					
				case 'D':
					#if CONTROL_COMPILE_DEBUG
						printf("Ready to complile dwell block %s\n", orig_cmd_str+1);
					#endif
					if (dwell_block_compile(orig_cmd_str+1))
						{
						sprintf(echo_str, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, echo_str);
						}
					#if CONTROL_COMPILE_DEBUG
						printf("dwell block has been compiled \n\n");
					#endif
					break;
				
				case 'V':
					#if CONTROL_COMPILE_DEBUG
						printf("Ready to complile velocity block %s\n", orig_cmd_str+1);
					#endif
					if (velocity_block_compile(orig_cmd_str+1))
						{
						sprintf(echo_str, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, echo_str);
						}
					#if CONTROL_COMPILE_DEBUG
						printf("velocity block has been compiled \n\n");
					#endif
					break;
				
				case 'L':
					#if CONTROL_COMPILE_DEBUG
						printf("Ready to complile position block %s\n", orig_cmd_str+1);
					#endif
					if (position_block_compile(orig_cmd_str+1))
						{
						sprintf(echo_str, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, echo_str);
						}
					#if CONTROL_COMPILE_DEBUG
						printf("position block has been compiled \n\n");
					#endif
					break;
					
				case '-':
				case 'J':
					#if CONTROL_COMPILE_DEBUG
						printf("Ready to complile jump block %s\n", orig_cmd_str+1);
					#endif
					if (jump_block_compile(orig_cmd_str+1))
						{
						sprintf(echo_str, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, echo_str);
						}
					#if CONTROL_COMPILE_DEBUG
						printf("jump block has been compiled \n\n");
					#endif
					break;
					
				case 'E':
					#if CONTROL_COMPILE_DEBUG
						printf("Ready to complile end block %s\n", orig_cmd_str+1);
					#endif
					if (end_block_compile(orig_cmd_str+1))
						{
						sprintf(echo_str, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, echo_str);
						}
					#if CONTROL_COMPILE_DEBUG	
						printf("end block has been compiled \n\n");
					#endif
					break;
							
				default: // Invalid block type	
					sprintf(echo_str, "?%s\n", orig_cmd_str);	
					send_string((void *)g.spc, echo_str);	
					break;
				}	
			}
		}
	}



// isr to send o_scope data packets
void o_scope_isr(int context)
	{
	int x;
	static int sample_ctr;
	short int adata;
	np_timer *o_scope_timer = na_o_scope_timer;

	o_scope_timer->np_timerstatus = 0; // Clear timer int signal.

	if (vtg.local_o_scope_mode || vtg.plant_wide_o_scope_mode)
		{
		if (!(SHOT_IN_PROGRESS)) // No shot in progress, keep going
			{
			if (sample_ctr == 2000) // Transmit data
				{
				// First stop timer
				o_scope_timer->np_timercontrol = 8;

				sample_ctr = 0;

				if (vtg.plant_wide_o_scope_mode)
					{
					// If connected, transmit here
					//printf("plant_wide wants o_scope packets\n"); // For test
					if (plant_net_settings.connected)
						{
						//printf("collected 2000 points of o_scope data, xmit to plant net\n");  // For test
						// If tcp_xmit() is tied up by somebody else, we'll simply not send a pcaket this time.
						tcp_xmit(plant_net_settings.tcp_plug, (char *)&scope_data_packet, vtg.o_scope_packet_size);
						}
					}
				if (vtg.local_o_scope_mode)
					{
					//printf("local wants o_scope packets\n"); // For test
					if (local_net_settings.connected)
						{
						//printf("collected 2000 points of o_scope data, xmit to local net\n"); // For test
						// If tcp_xmit() is tied up by somebody else, we'll simply not send a pcaket this time.
						tcp_xmit(local_net_settings.tcp_plug, (char *)&scope_data_packet, vtg.o_scope_packet_size);
						}
					}

				// Restart timer
				o_scope_timer->np_timercontrol = 7; // Start timer, continuous mode
				}
			else  // Take another sample
				{
				AD_CMD1 = 1; // Command an A/D loop
				while (AD_CMD1);  // Wait for A/D loop to finish
			
				#if 0
				// Code to write o_scope data in raw form

				for (x=0; x<4; x++)
					scope_data_packet.ch_17_20[sample_ctr][x] = shared_memory[x+16];
				#endif

				// Code to write o_scope data converted to volts X 1000.  This will be slower.

				// Convert the 4 channels to voltage X 1000.  We could have called the
				// convert_ana_ch() function 4 times and received converted data, however
				// that would result in A/D loops, somewhat inefficient. 
				for (x=0; x<4; x++) 
					{
					adata = shared_memory[x+16];

					// Upper channels are always -10 to +10V = -2048 to +2048 input range	
					if (adata & 0x0800) adata |= 0xf000; // Negative?
					// Now convert to voltage * 1000.
					scope_data_packet.ch_17_20[sample_ctr][x] = (adata*10000)/2048;
					}
				
				// Check if trigger requirments met.  Trigger always assumes a rising slope.
				if (sample_ctr == 0) // Still waiting for trigger
					{
					if (O_SCOPE_TRIG_CH > 16 && O_SCOPE_TRIG_CH < 21) // Only channels 17-20 are valid
						{
						if (O_SCOPE_TRIG_V > -10001 && O_SCOPE_TRIG_V < 10001) // Tigger spec within a +/-10V range?
							{
							if (scope_data_packet.ch_17_20[0][O_SCOPE_TRIG_CH-17] >= O_SCOPE_TRIG_V)
								{
								//printf("Trigger voltage met, %d volts\n", scope_data_packet.ch_17_20[0][O_SCOPE_TRIG_CH-17]); // For test
								sample_ctr++; // Trig. met, data already written to location 0, we'll begin collecting data now 
								}
							}
						}
					}
				else  // Trigger has already been met
					sample_ctr++;
				}
			}
		else  // Reset sample_ctr, don't send o_scope data during a shot
			{
			//printf ("Detected a shot taking place, resetting sample_ctr\n"); // For test
			sample_ctr=0;
			}
		}
	else  // Stop timer and reset sample_ctr, nobody wants o_scope data
		{
		//printf("Stopping o_scope timer and clearing sample_ctr, nobody wants scope data\n");
		o_scope_timer->np_timercontrol = 8;
		sample_ctr=0;
 		}
	}



// function to start / restart control program.  This wipes the slate clean.
void reset_control_program(int saved_warning_action)
	{
	int x;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	np_timer *timer2 = na_timer2;

	INITIALIZE_CONTROL; // This will leave the start interrupt bit unmasked.
	//printf("control_int->np_piointerruptmask=0x%x\n", control_int->np_piointerruptmask); // For test
	//control_int->np_piointerruptmask|=0x80; // For test
	//printf("control_int->np_piointerruptmask=0x%x\n", control_int->np_piointerruptmask); // For test
	SET_START_PROGRAM_INTERRUPT; // Set the interrupt bit

	if (saved_warning_action == CLEAR_SAVED_WARNING)
		{
		vtg.saved_warning = 0;
		}
	}


void stop_control(void)
	{
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	
	UNMASK_STOP_INTERRUPT; // Make sure stop interrupt is unmasked
	SET_STOP_PROGRAM_INTERRUPT; // Set the interrupt bit
	}			
	
	
void save_cprog_to_flash(void)
	{
	int x;
	//int y; // For test
	unsigned short tempv[((MAX_VAR+1)*2)];

	// First buffer the saved variables before erasing the flash sector
	for (x=0; x<=((MAX_VAR+1)*2); x++) 
		tempv[x] = ((unsigned short int *)VAR_FLASH_ADDR)[x];

	// Now erase flash sector
	nr_flash_erase_sector(GM_FlashBase, (unsigned short int *)VAR_CPROG_FLASH_SECTOR_ADDR);

	
	// Fill control program array with junk for testing
	//for (x=0;x<=MAX_STEP; x++)
	//	{
	//	for (y=0;y<=9;y++)
	//		{
	//		//vtg.control_program[x][y] = 0x12345678;
	//		switch (y)
	//			{
	//			case 0: vtg.control_program[x][y] = 0x00000000; break;
	//			case 1: vtg.control_program[x][y] = 0x11111111; break;
	//			case 2: vtg.control_program[x][y] = 0x22222222; break;
	//			case 3: vtg.control_program[x][y] = 0x33333333; break;
	//			case 4: vtg.control_program[x][y] = 0x44444444; break;
	//			case 5: vtg.control_program[x][y] = 0x55555555; break;
	//			case 6: vtg.control_program[x][y] = 0x66666666; break;
	//			case 7: vtg.control_program[x][y] = 0x77777777; break;
	//			case 8: vtg.control_program[x][y] = 0x88888888; break;
	//			case 9: vtg.control_program[x][y] = 0x99999999; break;
	//			}
	//		 
	//		}
	//	}
	

	// Then write the control program
	nr_flash_write_buffer(GM_FlashBase, (unsigned short int *)CPROG_FLASH_ADDR, 
								(unsigned short int *)vtg.control_program, ((MAX_STEP+1)*20));

	// Now write the buffered variables back to flash
	nr_flash_write_buffer(GM_FlashBase, (unsigned short int *)VAR_FLASH_ADDR, 
								tempv, ((MAX_VAR+1)*2));
	}


void reload_cprog_from_flash(void)
	{
	int x;
	unsigned short *cprogptr = (unsigned short *)CPROG_FLASH_ADDR;

	for (x=0; x<=((MAX_STEP+1)*20); x++) // ((MAX_STEP+1)*20) is the # of cprog halfwords saved in flash
		{
		((unsigned short *)vtg.control_program)[x] = cprogptr[x];
		}
	}
	

// This function will assign the parameter structure for each axis with the values set
// in the variables.  This allows control code to switch axes on the fly without
// having to have seperate code for each axis.
void update_control_parameters(void)
	{
	int axis_num=1;

	// Set the control parameters for axis 1
	VEL_LOOP_GAIN = VEL_LOOP_GAIN_SETTING;			
	VEL_LOOP_BK_VEL1 = VEL_LOOP_BK_VEL1_SETTING; 				
	VEL_LOOP_BK_VEL2 = VEL_LOOP_BK_VEL2_SETTING;				
	VEL_LOOP_BK_VEL3 = VEL_LOOP_BK_VEL3_SETTING;				
	VEL_LOOP_BKGN1 = VEL_LOOP_BKGN1_SETTING;					
	VEL_LOOP_BKGN2 = VEL_LOOP_BKGN2_SETTING;					
	VEL_LOOP_BKGN3 = VEL_LOOP_BKGN3_SETTING;					
	DITHER_TIME = DITHER_TIME_SETTING;				
	DITHER_AMPL = DITHER_AMPL_SETTING;				
	POS_CHECK_ZSPEED = POS_CHECK_ZSPEED_SETTING;			
	NULL_OFFSET = NULL_OFFSET_SETTING;				
	LVDT_OFFSET = LVDT_OFFSET_SETTING;				
	JS = JS_SETTING;				
	FT = FT_SETTING;		
	FT_STOP_POS = FT_STOP_POS_SETTING;	
	RETR = RETR_SETTING;			
	SENSOR_DEVIATION = SENSOR_DEVIATION_SETTING;		
	POS_LOOP_GAIN = POS_LOOP_GAIN_SETTING;			
	POS_BK_GN_FE = POS_BK_GN_FE_SETTING;				
	POS_BK_GN = POS_BK_GN_SETTING;

	// Set the control parameters for axis2
	axis_num=2;
	VEL_LOOP_GAIN = VEL_LOOP_GAIN_2_SETTING;			
	VEL_LOOP_BK_VEL1 = VEL_LOOP_BK_VEL1_2_SETTING; 				
	VEL_LOOP_BK_VEL2 = VEL_LOOP_BK_VEL2_2_SETTING;				
	VEL_LOOP_BK_VEL3 = VEL_LOOP_BK_VEL3_2_SETTING;				
	VEL_LOOP_BKGN1 = VEL_LOOP_BKGN1_2_SETTING;					
	VEL_LOOP_BKGN2 = VEL_LOOP_BKGN2_2_SETTING;					
	VEL_LOOP_BKGN3 = VEL_LOOP_BKGN3_2_SETTING;					
	DITHER_TIME = DITHER_TIME_2_SETTING;				
	DITHER_AMPL = DITHER_AMPL_2_SETTING;				
	POS_CHECK_ZSPEED = POS_CHECK_ZSPEED_2_SETTING;			
	NULL_OFFSET = NULL_OFFSET_2_SETTING;				
	LVDT_OFFSET = LVDT_OFFSET_2_SETTING;				
	JS = JS_2_SETTING;				
	FT = FT_2_SETTING;		
	FT_STOP_POS = FT_STOP_POS_2_SETTING;	
	RETR = RETR_2_SETTING;			
	SENSOR_DEVIATION = SENSOR_DEVIATION_2_SETTING;		
	POS_LOOP_GAIN = POS_LOOP_GAIN_2_SETTING;			
	POS_BK_GN_FE = POS_BK_GN_FE_2_SETTING;				
	POS_BK_GN = POS_BK_GN_2_SETTING;
	}




int null_valve(int axis_num, int action)
	{
	int return_val=0, failed=0, correction_increment, null_voltage, x, ctr;
	char echo_str[81];
	

	// Note:
	// NULL_OFFSET is the working null value that is in the axis structure.  The value of
	// the variable axis_num determines the structure member to retreive the null from.

	// NULL_OFFSET_SETTING and NULL_OFFSET_2_SETTING are the vtg.v[] variable settings.
	// These are not usable in NULL_OFFSET until update_control_parameters() or 
	// initialize_axes() are called, normally at cycle start.  Command OC4<CR> will

	// VALVE_TYPE is setup but none are defined at this time.
		

	if (axis_num < 1 || axis_num > 2)
		{
		sprintf(echo_str,"R_AXIS%d_INVALID_AXIS_SPECIFIER:21 \n",axis_num); 
		send_string((void *)g.spc, echo_str);
		return_val = 99;
		} 
	else if (action == CHECK_NULL)
		{
		// Derive the current voltage and write the result to a reserved variable

		// Zero out the result variable
		NULL_RESULT = 0;

		// Send a zero volt command
		out_dac(DAC_OUTPUT_CHANNEL, 0, VOLTAGE_X_1000, ABSOLUTE);

		// Then get null value, suptract out the null and lvdt compensations
		convert_ana_ch(ADC_CHANNEL_FOR_LVDT, VOLTAGE_X_1000, &null_voltage);
		null_voltage -= LVDT_OFFSET;  // Subtract out the lvdt offset.  Null offset stays.

		// Write the result to variable holding the null result
		NULL_RESULT = null_voltage;  // Save result to a variable, this may be useful for something.
		sprintf(echo_str,"R_AXIS%d_NULL_RESULT:22 #%d\n", axis_num, null_voltage); 
		send_string((void *)g.spc, echo_str);
		}
	else if (action == AUTO_NULL)
		{
		NULL_OFFSET = 0; // Axis# accounted for here.
		 
		// Send a zero volt command
		out_dac(DAC_OUTPUT_CHANNEL, 0, VOLTAGE_X_1000, ABSOLUTE);

		// Read null voltage
		convert_ana_ch(ADC_CHANNEL_FOR_LVDT, VOLTAGE_X_1000, &null_voltage);
		if (null_voltage > 500 || null_voltage < -500) // Too far gone
			{
			sprintf(echo_str,"R_AXIS%d_BEGINNING_NULL_EXCEEDS_ALLOWABLE_RANGE:23 #%d\n", axis_num, null_voltage); 
			send_string((void *)g.spc, echo_str);
			update_control_parameters();  // Restore parameters to initial settings
			}
		else if (null_voltage > 100 || null_voltage < -100) // +/- 100mV allowable range.
			{
			if (null_voltage > 0) correction_increment = -5;
			else correction_increment = 5;

			// We'll attempt to correct, up to 1V + or -
			for (x=correction_increment, ctr=0; ctr<200; x+=correction_increment, ctr++) // 5 mV increments
				{
				NULL_OFFSET = x; // Set working null 
				out_dac(DAC_OUTPUT_CHANNEL, 0, VOLTAGE_X_1000, ABSOLUTE);
				// nr_delay(200); // Allow valve to settle out, add this later if necessary
				convert_ana_ch(ADC_CHANNEL_FOR_LVDT, VOLTAGE_X_1000, &null_voltage);
				null_voltage -= LVDT_OFFSET;  // Subtract out the lvdt offset, leave null offset there
				if (null_voltage < -25 || null_voltage > 25)
					{
					// Not there yet, keep going
					}
				else  // Success!!!
					{
					break;
					}
				}

			if (ctr>=100) // Unable to correct
				{
				NULL_OFFSET = 0; // Zero will be written back to NULL_OFFSET_SETTING
				failed = 1;
				}

			if (axis_num==1)
				{
				// Update the setting from the working copy.
				NULL_OFFSET_SETTING = NULL_OFFSET;
				}
			else if (axis_num==2)
				{
				// Ditto
				NULL_OFFSET_2_SETTING = NULL_OFFSET;
				}
			// Save variables to flash
			save_vars_to_flash();

			// Transmit a success message, control program ASCII file will have
			// to be updated by application software.
			if (failed)
				{
				sprintf(echo_str,"R_AXIS%d_AUTO_NULL_FAILED_UNABLE_TO_CORRECT:24 \n",axis_num); 
				send_string((void *)g.spc, echo_str);
				update_control_parameters();  // Restore parameters to initial settings
				}
			else
				{
				sprintf(echo_str,"R_AXIS%d_AUTO_NULL_SUCCESS:25 #%d\n",axis_num, NULL_OFFSET); 
				send_string((void *)g.spc, echo_str);
				}
			}
		else  // Null good, no need to correct
			{
			sprintf(echo_str,"R_AXIS%d_NULL_ALREADY_GOOD_NO_NEED_TO_CORRECT:26 \n",axis_num); 
			send_string((void *)g.spc, echo_str);
			}
		}
	else  // Unknown action.
		{
		return_val=99;
		}

	ext: ;
	return (return_val);
	}



// Timer2 isr, used only for control

// Commands to start/stop timer
// timer2->np_timercontrol = 7; // Start timer, continuous mode
// timer2->np_timercontrol = 5; // Start timer, single mode
// timer2->np_timercontrol = 8; // Stop timer
void timer2_isr(int context)
	{
	np_timer *timer2 = na_timer2;
	np_pio *cont_int_output = na_cont_int_output;
	
	timer2->np_timerstatus = 0; // Clear timer int signal.
	vtg.timer2_timeout=1; // May be useful for something.

	// Now set the interrupt signal to interrupt control.
	SET_TIMER2_TIMEOUT_INTERRUPT;
	}
#endif
/////////////////////////////////////////////////////////////////////////////





#if 0
// This function is useful for troubleshooting Ethernet problems.
// It must be copied to plugs.c and compliled there (make -f makefile all) from within
// the lib directory.  It is only archived here since plugs.c is an Altera generated file
// and changes every time a Quartus design is compiled.  It is not meant to be compiled 
// in this source file.  Do not remove the #if 0 and #endif directives.

int print_sg_plugs_global(void)
	{
	int x;
	s_plugs_globals *g = &ng_plugs_globals;

	printf("Running = %d\n", g->running);
	printf("adapter_count = %d\n", g->adapter_count);
	for (x=0; x<PLUGS_ADAPTER_COUNT; x++)
		printf("adapter[%d] = %d\n", x, g->adapter[x]);
	printf("packets read = %d\n", g->packets_read);
	printf("bytes_read = %d\n", g->bytes_read);
	printf("packets_lost = %d\n", g->packets_lost);
	printf("packets_sent = %d\n", g->packets_sent);
	printf("bytes_sent = %d\n", g->bytes_sent);	
	printf("ip_identifier = %d\n", g->ip_identifier);
	printf("port_number = %d\n", g->port_number);
	for (x=0; x<PLUGS_PLUG_COUNT; x++)
		printf("plug[%d] = %d\n", x, g->plug[x]);
	printf("in_dispatch = %d\n", g->in_dispatch);
	printf("in_idle = %d\n", g->in_idle);

	#if PLUGS_DNS
		printf("dns_plug = %d\n", g->dns_plug);
		printf("dns_identifier = %d\n", g->dns_identifier);
		printf("dns_query_state = %d\n", g->dns_query_state);
		printf("dns_query_result = %d\n", g->dns_query_result);
	#endif

	printf("interruptee_pc = %d\n", g->interruptee_pc);
	printf("exclusive_plug_handle = %d\n\n\n", g->exclusive_plug_handle);
	}
#endif



#if 0
void test_isr(int context)
	{
	int x;

	for (x=0; x<5; x++)
		{
		if (x==1) {asm ("TRAP #31");} // Call a lower priority isr and see what happens
		printf ("We're in the test isr\n");
		nr_delay(2000);
		}
	printf("We're exiting the test isr\n");
	}
		

void test1_isr(int context)
	{
	int x;

	for (x=0; x<5; x++)
		{
		printf ("We're in the TEST1 isr\n");
		nr_delay(2000);
		}
	printf("We're exiting the TEST1 isr\n");
	}	
#endif	
					
////////////////////////////////////////////////////////////////////////


int main(void)
	{
	// All pointers declared as volatile per Altera.
	int result, x, y, char_position, c, just_connected;
	int blk_exe_return_val=0, idle_error;
	unsigned int timer_load_value;
	unsigned int print_again; // For test
	unsigned short int junk;
	volatile int dd, ee;
	char input_string[256], tempBuffer[256], echo_str[81];
	np_pio *misc_out_pio = na_misc_outs;
	np_pio *misc_ins_pio = na_misc_ins;
	np_pio *axis1_interrupt = na_axis1_int;
	//np_pio *sel_hi_channels = na_sel_ana17_20;
	np_pio *opto_control = na_opto_control;
	np_pio *opto_data = na_opto_data;
	np_pio *ls_pio = na_ls_int_input;
	np_pio *bounceback_pio = na_bounceback_int;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	np_timer *timer2 = na_timer2;
	np_timer *wd_timer = na_watchdog;
	np_timer *wd_rst_timer = na_WD_rst_timer;
	np_timer *one_ms_timer = na_one_ms_timer;
	np_timer *o_scope_timer = na_o_scope_timer;
	np_spi *dac_spi = na_dac_spi;
	//np_spi *adc_spi = na_adc_spi;
	//vp_pvu *pv1 = va_pv1;	
	//vp_pvu *pv2 = va_pv2;
	int num=0, master_enable_was_off=0;
	int axis_num=1; // Used for any interactions with PV1.
	unsigned short int tempus, shot_num;
	volatile unsigned short int *halfword_ptr;
	char dac_ctr, osw_ctr, temp_str[81];
	//
	s_tcp_status * u;					// For test
	unsigned int start_time, end_time;	// For test
	unsigned short int tempu;			// For test
	int err_ctr;						// For test
	unsigned int report_time;			// For test
	
		
	//g.sp = tcp_send_byte;
	g.spc = &local_net_settings; // no context
	//g.rp = tcp_recv_byte;
	g.rpc = &local_net_settings; // no context
	//g.ip = tcp_init;
	g.ipc = &local_net_settings;
	//g.t = tcp_check;
	g.tc = &local_net_settings;

	//printf("Here at #1\n"); // For test	
	// Initialize VTW stuff
	//
	// Restore variables and control program
	// First clear out the memory so that when vars and cprog are updated to flash,
	// no garbage get saved also.
	//for (x=0; x<=(MAX_VAR+1); x++) vtg.v[x] = 0;
	//for (x=0; x<=((MAX_STEP+1)*10); x++) ((int *)vtg.control_program)[x] = 0;
	//vtg.x = 0;
	
	reload_vars_from_flash();
#if BUILD_FOR_CONTROL
	reload_cprog_from_flash();
	if (vtg.control_program[1][0] == 0 || vtg.control_program[1][0] > START_)
		{
		// No block #1, no control program in flash
		CLR_PROGRAM_LOADED;
		CLR_PROGRAM_RUNNING;
		}
	else SET_PROGRAM_LOADED;
#endif		
	vtg.misc_outs = 0;
	misc_out_pio->np_piodata = vtg.misc_outs;

	// Setup pointers to the control block functions
	INITIALIZE_CONTROL_BLOCKS;

	vtg.var_dec_hex = VAR_DEC;
	comp_shot_parameters.cycle_time = 0;
	vtg.first_cs_after_power_up = 1;
	shot_num = 1;
	vtg.local_wants_op_packets = 0;		 // Default to no op packets. 1==Send one, 2==Keep sending
	vtg.plant_wide_wants_op_packets = 0; //			"				"			"			"
	vtg.local_o_scope_mode = 0;
	vtg.plant_wide_o_scope_mode = 0;
	vtg.o_scope_in_progress = 0;
	vtg.tcp_xmit_in_progress = 0;
	vtg.tst_ctr=0; // for test
	vtg.dac_write_in_progress = 0;

	// Clear the control interrupt pio and mask all of the control interrupt pio bits,
	// Even if monitor only.  We'll always do this.
	control_int->np_piointerruptmask = 0;
	cont_int_output->np_piodata = (vtg.cont_int_out_status = 0);
	

	// Write ADC setups
	ad_setup_words.ch1 = 0x8350;  
	ad_setup_words.ch2 = 0x8750;	
	ad_setup_words.ch3 = 0x8b50;	
	ad_setup_words.ch4 = 0x8f50;	
	ad_setup_words.ch5 = 0x9350;	
	ad_setup_words.ch6 = 0x9750;	
	ad_setup_words.ch7 = 0x9b50;	
	ad_setup_words.ch8 = 0x9f50;	
	ad_setup_words.ch9 = 0xa350;	
	ad_setup_words.ch10 = 0xa750;	
	ad_setup_words.ch11 = 0xab50;	
	ad_setup_words.ch12 = 0xaf50;	
	ad_setup_words.ch13 = 0xb350;	
	ad_setup_words.ch14 = 0xb750;	
	ad_setup_words.ch15 = 0xbb50;	
	ad_setup_words.ch16 = 0xbf50;	
	ad_setup_words.ch17 = 0xb350;	
	ad_setup_words.ch18 = 0xb750;	
	ad_setup_words.ch19 = 0xbb50;	
	ad_setup_words.ch20 = 0xbf50;


	// Initialize the DACs to 0V.  They will power up this way, however
	// force it here.  vtg.dac_word[] will be initialized.
	out_dac(1, 8192, RAW_VAL, ABSOLUTE); // 8192 == mid-scale == 0V, remember -10V - +10V DAC range.
	out_dac(2, 8192, RAW_VAL, ABSOLUTE);
	out_dac(3, 8192, RAW_VAL, ABSOLUTE);
	out_dac(4, 8192, RAW_VAL, ABSOLUTE);
	
		
	// Set the operational status word header and trailer.  Do it now to save time.
	// We probably don't want to be during this during a shot.
	//op_data_packet.sizeof_this_struct = sizeof(op_data_packet);
	//sprintf(op_data_packet.header, "O_%d\n", op_data_packet.sizeof_this_struct); // Total number of bytes in structure including header and trailer strings.
	sprintf(op_data_packet.header, "O_BEGIN\n");
	vtg.op_packet_size = sizeof(op_data_packet);
	//strcpy(op_data_packet.trailer, "O_END\n"); 

	// Set header and trailer for o_scope data
	//scope_data_packet.sizeof_this_struct = sizeof(scope_data);
	//sprintf((char *)scope_data_packet.header, "S_%d\n", scope_data_packet.sizeof_this_struct);
	sprintf((char *)scope_data_packet.header, "S_BEGIN\n");
	vtg.o_scope_packet_size = sizeof(scope_data_packet);
	//strcpy((char *)scope_data_packet.trailer, "S_END\n");

#if MON_DEBUG
	// Now let's see what we have
	printf("Now this is what we have:\n");
	
	printf("clock freq = %d\n", CLK_FREQ);
	printf("tim1 clk freq is %d and tim2 clk freq is %d\n", VEL1_CLK_FREQ, VEL2_CLK_FREQ);	
	printf("vel1 mul is %d and vel2 mul is %d\n", VEL1_MUL, VEL2_MUL);
#endif

	//DIS_PV_INT;
	CLR_CURRENTLY_TIMING;
	CLR_SHOT_COMPLETE;
	CLR_MONITOR_ENABLE;
	CLR_PAST_MIN_STK_LEN;
	
	
// Initialize TCP.
tcp_init();

// Install timer2 interrupt handler

//nr_installuserisr(na_timer2_irq, control_supervisor_isr, 1);
nr_installuserisr(na_WD_rst_timer_irq, reset_watchdog, 0);
wd_rst_timer->np_timercontrol = 7; // Timer should reset to 500mS, we'll start this even in monitor only mode for op data

#if BUILD_FOR_CONTROL
	// Install the o_scope_timer interrupt handler
	nr_installuserisr(na_o_scope_timer_irq, o_scope_isr, 0); 
	// Install WD reset timer isr
	//nr_installuserisr(na_WD_rst_timer_irq, reset_watchdog, 0); 
	nr_installuserisr(na_control_int_irq, control_supervisor_isr, 0); // control interrupt
	nr_installuserisr(na_timer2_irq, timer2_isr, 0); 
	// Start the WD timer.  It will run forever
	//wd_rst_timer->np_timercontrol = 7; // Timer should reset to 500mS
	wd_timer->np_timercontrol = 4;
if (PROGRAM_LOADED)	{reset_control_program(CLEAR_SAVED_WARNING);} // Start the control program
	//INITIALIZE_CONTROL; // Elminate this after putting back above line
	//CLR_PROGRAM_RUNNING; // This also
#else
	nr_installuserisr(na_o_scope_timer_irq, dummy_o_scope_isr, 0); 
	PULSE_WATCHDOG_RESET;
#endif



// For test - Install the test isr's
//nr_installuserisr(30, test_isr, 0);
//nr_installuserisr(31, test1_isr, 0);

//vtg.gp_config_word = 0; // Clear configuration word
MON_STATUS_WORD = 0; // Clear monitor status word
			
opto_data->np_piodirection = 0; // Set all bits to read
opto_control->np_piodata = 0x1; // Power up in rd mon inputs mode
	
nr_installuserisr(na_axis1_int_irq, pos_based_monitor_isr, 0); // pv unit interrupt
nr_installuserisr(na_bounceback_int_irq, bounceback_isr, 0); // bounceback interrupt
nr_installuserisr(na_ls_int_input_irq, ls_service_isr, 0); // limit switch interrupt
nr_installuserisr(na_one_ms_timer_irq , time_based_monitor_isr, 0); // timer interrupt

// initialize_axes() will load the limit switches, set vclk_div and setup the axis array.
initialize_axes();

ONE_MS_COUNTER = 0;
//ENBL_PV_INT;


one_ms_timer->np_timercontrol = 7; // Start timer

print_again = nr_timer_milliseconds() + 1000; // For test

#if MON_DEBUG
// For test
printf("Local plug=%d, connected=%d, rem_ip_add=%d, rem_port=%d\n", 
		local_net_settings.tcp_plug, local_net_settings.connected, local_net_settings.remote_ip_address, local_net_settings.remote_port);

printf("Plant_wide plug=%d, connected=%d, rem_ip_add=%d, rem_port=%d\n", 
		plant_net_settings.tcp_plug, plant_net_settings.connected, plant_net_settings.remote_ip_address, plant_net_settings.remote_port);
#endif


//osw_ctr = 0; // for test

//print_sg_plugs_global(); // For test
CLR_SHOT_IN_PROGRESS;


//////// A/D test code /////////

#if 0
// Test code to measure the time an A/D loop takes
for (x=0; x<6; x++) // Do six loops and then move on
	{
	start_time = RD_PV_RAW_VEL_CTR;
	AD_CMD1 = 6;
	while (AD_CMD1==6);
	end_time = RD_PV_RAW_VEL_CTR;
	end_time -= start_time;
	printf("It took %d counts for the AD conversion\n", end_time);
	nr_delay(1000);
	}


// Test code to check A/D errors
while (1)
	{
	//report_time = nr_timer_milliseconds()+3600000;
	report_time = nr_timer_milliseconds()+1000;
	err_ctr=0;

	while (nr_timer_milliseconds() < report_time)
		{
		AD_CMD1 = 6;
		while (AD_CMD1==6);

		// First get channels 1 - 8
		for (x=0; x<8; x++)
			{
			tempu = shared_memory[x];
			tempu &= 0xf000;
			tempu >>= 12;
			if (tempu != x) {printf("Bad AD value channel #%d\n", x+1); err_ctr++;}
			}

		// Now check upper channels, 17 - 20
		for (x=16; x<20; x++)
			{
			tempu = shared_memory[x];
			tempu &= 0xf000;
			tempu >>= 12;
			if (tempu != (x-4)) {printf("Bad AD value channel #%d\n", x+1); err_ctr++;}
			}

		// Now get channels 9 - 16, these channels use the floating channel, therefore
		// a conversion must be initiated for each channel read.
		halfword_ptr = (unsigned short int *)&ad_setup_words.ch1;
		for (x=8; x<16; x++)
			{
			FLOATING_CHANNEL_SETUP = halfword_ptr[x];
			AD_CMD1 = 6;
			while (AD_CMD1==6);
			tempu = FLOATING_CHANNEL_RESULT;
			tempu &= 0xf000;
			tempu >>= 12;
			if (tempu != x) {printf("Bad AD value channel #%d\n", x+1); err_ctr++;}		
			}
		}

	//printf("After 1 hour, there were %d errors\n", err_ctr);
	printf("After 1 second, there were %d errors\n", err_ctr);
	}
#endif

#if 0
// Fast I/O test code
while (1)
{
chk_discrete_input(300, &x); if (x) printf("300 is ON\n"); else printf("300 is OFF\n");
chk_discrete_input(301, &x); if (x) printf("301 is ON\n"); else printf("301 is OFF\n");
chk_discrete_input(302, &x); if (x) printf("302 is ON\n"); else printf("302 is OFF\n");
chk_discrete_input(303, &x); if (x) printf("303 is ON\n"); else printf("303 is OFF\n");
chk_discrete_input(304, &x); if (x) printf("304 is ON\n"); else printf("304 is OFF\n");
chk_discrete_input(305, &x); if (x) printf("305 is ON\n\n\n"); else printf("305 is OFF\n\n\n");
nr_delay(1500);
}
#endif

#if 0
// Trap test code
another_trap: ;
asm ("TRAP #30");
printf("Just finished trap 30 about to wait 15 seconds\n");
nr_delay(15000);
goto another_trap;
#endif

////////////////////////////////

while (1)
	{

//#if BUILD_FOR_POLLED_ETHERNET
//	#if BUILD_FOR_CONTROL
//		if (!(SHOT_IN_PROGRESS))
//			{
//			nr_plugs_idle();
//			}
//	#else
//		nr_plugs_idle();
//	#endif
//#endif	



#if BUILD_FOR_POLLED_ETHERNET
	
	nr_plugs_idle();
	
#endif
	
	if (SW7) xmit_fake_shots();	
	if (SW9) xmit_chunks();		
	
#if BUILD_FOR_CONTROL		
	if (MASTER_ENABLE) // Master enable is present) // Master enable is present
		{
		//printf("-M-"); // For test	
		monitor_supervisor(&shot_num);
	
		//if (CS_DETECTED && !(ZSPEED));
		//else
		//	{
		//	printf("-C-"); // For test
		//	control_supervisor(master_enable_was_off, print_again); // print_again for test
		//	}
		
		if (master_enable_was_off)
			{
			master_enable_was_off=0;
			reset_control_program(CLEAR_SAVED_WARNING);
			}
		}
	else // Master enable is not present. Disable control.
		{
		if (master_enable_was_off == 0)
			{
			master_enable_was_off=1;	
			//DIS_PV_INT;
			stop_control();
			}
		}
#else
	monitor_supervisor(&shot_num);
#endif
	} // while(1)		
}








