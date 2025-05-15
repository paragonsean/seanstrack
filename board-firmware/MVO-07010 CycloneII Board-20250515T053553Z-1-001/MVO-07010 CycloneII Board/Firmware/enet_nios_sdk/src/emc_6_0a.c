//     emc_6_0a.c 

// Made some temporary changes to vtw_global7.h and combined_control_blocks3.c
// for the St. Louis show.  Remove these after the show.

/*  4-1-03   enet_mon_cont4.c

This is a modified version of Altera's germs_server.c for the Nios processor.
This version only supports TCP communications.  UDP and serial I/O is not supported.
This version, as the name implies, plays the part of a server.  Therefore it will
wait for a client to make a connection.

5-5-03  enet_mon_cont5.c

5-13-03 enet_mon_cont6.c

6-5-03 enet_mon_cont7.c

6-24-03 enet_mon_cont8.c  First version for single processor enet_mon_cont.hexout.flash

7-2-03 enet_mon_cont9.c Moved monitor and control supervisors to seperate functions.

7-8-03	enet_mon_cont10.c Seperated the pos and time based isrs.  Created a seperate function
		for the pos / time based A/D loop.  This version works with control_blocks4.c and .h
		and tiu.h

7-10-03 enet_mon_cont11.c  This version works with control_blocks5.c and .h and tiu1.h

7-14-03 enet_mon_cont12.c  This supports 2 position axes. This version works with 
		control_blocks6.c and .h and vtw_global2.h.  The tiu?.h items have been included into
		vtw_global2.h.
		This version supports 2 transducer axes.

8-1-03 enet_mon_cont14.c

8-18-03 enet_mon_cont14a.c  Added new network_xmit routine
		enet_mon_cont14b.c	Added fake shot capability by pressing SW7	
		enet_mon_cont14c.c  Send 1350 bytes of garbage when SW9 is pressed and
							fake shots when SW7 is pressed

8-22-03 enet_mon_cont15.c  Removed Ethernet interrupt, Ethernet now polled.

8-22-03 enet_mon_cont16.c  Made buildable for monitor only or monitor w/control, polled ethernet
8-28-03 enet_mon_cont16a.c Interrupt driven Ethernet

8-29-03 enet_mon_cont17.c  Interrupt driven Ethernet
9-2-03  enet_mon_cont17a.c	Polled Ethernet version

9-22-03 enet_mon_cont18.c  Allow selecting/deselecting polled Ethernet with the preprocessor
		Allow enabling/disabling prompt.  This code works with the new prog. div. by function
		for both axis #1 and #2.

9-29-03 emc_1_0.c  1st. production version 1.0
		Changed fake shot generator to slow it down somewhat.

12-2-03 emc_2_0.c  Build emc_2_0 for polled control and polled ethernet
		Major revision, Buildable for several different variations.  Too many changes from
		V1.0 to list.
		SHOT_IN_PROGRESS will now also act as a CS signal to the monitor.  The shot control program
		will set SHOT_IN_PROGRESS at the beginning of the shot.  This will trigger the monitor
		if a CS did not occur.  This was done because control needs the ZSPEED setup and this is
		done by the monitor.

1-6-04	emc_3_0.c	Major revison.  Buildable for both TCP and UDP.  UDP and interrupt control 
		to be the standard configuration. UDP and polled control to be the secondary configuration. 
		Extensive changes to Ethernet routines to simpilfy.  Buildable for handshaking in UDP.  Combined
		interrupt driven and polled control routines and header file.  No longer are there
		seperate files.  Check monitor_supervisor() and nr_plugs_idle() on fixed time
		intervals insted of hammering them as fast and as often as possible.  This will
		be useful if polled control is used as more time will be available for executing
		control.  Elminated the fixed arrays for shot data and for o-scope data.  using
		malloc() instead. 
		
		Added serial mode.  If the board is booted with SW7 held, the board boots into
		serial mode.  Commands can now be sent over the serial port.  You cannot monitor
		and control in this mode, it is just a command interpreter.

		Added commands to set and flash network settings, elminating the need to run
		"hello_plugs."  These can be set over the network or if in serial mode, over the
		serial port.  Serial mode is helpful in that network parameters can be set if
		setting a board up for the first time or if the settings are corrupted for some
		reason and a network connection cannot be established.

		Added code to reset the watchdog from within control_supervisor().  In interrupt
		control mode, if the control program does not exit and wait for interrupt, the
		WD would timeout and reset the board.  Also added the ability to stop the control
		control program by pressing switch SW8.  Again if the control program does not exit
		and wait for interrupt, Ethernet communication cannot take place since it is polled.
		Pressing SW8 will allow the user to stop the control program.

2-25-04 emc_4_0 Made A/D setups in on-chip shared memory.
		Problems found with A/D conversions.  Analog voltage
		was not settling out before next conversion.  Limited
		conversions to 8.  Created 5 analog dynamic channel modes. 

		Mode 1: Ch1-8. This will be the default monitor setting.
		Mode 2: Ch1-4, 9-12.
		Mode 3: Ch1-4, 13-16.
		Mode 4: Ch1-4, 17-20.
		Mode 5: Ch1-6, 17 and 18. This will be the default control setting. 

		Main processor now has control over the select hi channels
		pio line.  Will have to work on how this will impact control.
		For now this is more of a monitor only thing.
		Variable V428 was added to control the delay between analog
		conversions.  28 appears to be a good setting.  That yields
		about 400 IPS.

		Analog toggling mode was added.  If config word V313 bit 31 is zero, toggling
		mode is enabled.  In this mode, at speeds above 203 IPS, only 4 analog channels
		are converted and the next 4 will be converted the next sample.  If bit 31 is set,
		toggling is disabled.

		Added variable V429.  This set the default analog mode.  Values from 1 to 5 are
		valid.

	3-1-04 emc_4_1.c
		Changed process_net_commands() to allow commands.  When uploading chip configuration
		each line (command) can be around 280 bytes.  Perviously we could only handle 80 byte
		commands.

		Checksum check in do_colon_command() was checking for 0xff.  Changed it to zero.

		Created send prompt commands that will wait for an ACK.  After processing net commands
		we'll wait for an ACK after sending a prompt.  When the
		remote guy is uploading files and prompts are enabled, we want to make sure he
		received the prompt since we handshake with it during uploads.

		Changed control code to work with dynamic analog channels only as selected
		in V429.  This affects conditional checking and the update block.

	4-2-04 emc_4_11.c
		Found problem at Honda regarding total stroke length.  If too long, malloc() will
		trash memory in monitor_supervisor() when CS is activated resulting in the board
		resetting.  Since CS is also the GERMS mode button, the board goes immed. into 
		GERMS mode when CS is activated.  Total stroke length setting is now checked and
		if greater than 6000 (75" 20P rod) it will be forced to 6000.

	6-3-04 emc_5_0.c   emc_chip_08000000
		Returned analog inputs to older style (emc_3_1 and earilier) with floating channel.
		This was done after hardware revision to the basic external interface board.
		
		Added code to deal with LVDT interface.

		Redefined SHOT_IN_PROGRESS to begin with motion and end with the timed collection.

		Changed fake shot button from SW7 to dip switch SW5-2.  This is a "special" version that 
		attemps to correct a problem found in the field where noise appearently was causing the 
		board to go into the fake shot mode.  By placing this mode on the dip switch, outside 
		glitching should not be able to cause that.

		Changed the stop control program switch from PB SW8 to dip switch SW5-3 for the same
		reason as the above.

		Also changed serial mode switch to dip switch SW5-4.

		Added analog and I/O change reporting.  Block analog reporting sample interval is limited
		to 350uS or greater.

		Made the OSW1 write routine a macro.  Changed the write routine to double write the OSW1 value 
		to guard against the possibility of bits not changing as they should.

		In monitor_supervisor() OSW1 will be written when CS is detected.  This is more necessary for
		monitor only systems where a stuck alarm output bit may be set resulting in scrapped castings.

	1-24-05 emc_5_01.c
		Changed the op data function to send back the upper channels along with the dynamic channels.
		Since the upper channels are always converted, might as well send them.

    2-1-06 emc_5_02.c
		Fixed some problems with pressure control in combined_control_blocks3.c.  This file itself has not changed.

	3-27-06 emc_5_03.c
		Added 2 new entries to config_word2, V314, OPTO_IO_BOARD_NOT_INSTALLED and NO_CYCLE_START_UNTIL_AT_HOME.
		This is to lock out CS until AT HOME if enabled to do so.  A fast input, #304 has been designated as
		the fast at home input.  If an Opto board is not installed, we'll check the fast input for CS if so
		enabled.  

	7-12-06 emc_5_04.c  
		Now loads tb6.c which corrects a problem in the convert_ana_ch_from_dynamic() function.  The dynamic channels 
		can be lower or upper channels.  This function was not taking that into account.  If checking the
		analog input in a conditional statement in a control block, the results were erroneous.

	9-15-06 emc_5_05.c
		Cycle start detected message will indicate if input 300 was the source.  Otherwise it's the same as before.
		This was done for the cases where spurious cycle starts are happening.  Perhaps it will give us the chance
		further narrow down where the problem is coming from.

		Changed the write to OSW1 to happen every mS.  Prev. the write would only occur if there was a change.  If the
		output bounced, outputs could be wrong when the board thought they were right, and they wound not get updated 
		until another write took place.

    10-26-06 emc_5_06.c
	    Elminated the macro for writing OSW1.  Elminated the double write and the write to 0x5555 when writing OSW1.
		If OSW1 is now being updated every millisecond, no need to do this anymore.

        Elminated the CLR_MONITOR_ENABLE command in the time based isr.  This is cleared by monitor_supervisor()
		and other places but should not be done here.

		Reworked the cycle start stuff in monitor_supervisor() extensively.  Made edge triggered.  Added some delay
		on the input 300 CS input.  Moved CD_DETECTED to bit 9 in the monitor status word, V312.  Bit 0 is now the
		CS signal from the start shot input from the control, CS_FROM_SS.  Therefore the CS signals from input 300 
		and from start shot are now seperate entities apart from CS_DETECTED.
		
		Changed the control programs to turn off CS_FROM_SS at the top of the program and shortly after it is triggered
		at the start of the shot.  We'll also clear CS_FROM_SS here in case somebody tries to run this version with
		an old control program, it should still work.

		New control programs should also work with older firmware such as 5.02 without any problems.  However, for
		best results, V5.06 firmware and later should use the new control programs.

		Found that attempting to make a shot without the CS having been cleared from the previous shot results in a 
		slow shot and a WARNING #2.  By clearing out the CS signal at the top of the control program, this seems to
		have been solved.  Perhaps this is what happened at Port City.

	12-7-06 emc_6_0.c
		First Cyclone II version

*/

/* To debug with gdb:

  nios2-debug --cable='USB-Blaster [USB-0]' emc_6_0.elf

  This will start the debugger over the JTAG port

	
*/


/*
		Porting program from Nios V2.2

  1,	In parse_operational, change line tempstr[x] = *((char *)(0x72+x)); to tempstr[x] = *((char *)(0x74+x));
  2,	In function to load AD processor, change flash_ptr = (unsigned short int *)0x104080 to flash_ptr = (unsigned short int *)0x104100.
  3,	Elminate the r_set_settings_index(int index) function.
  4,	Change the r_reset_settings(int x), r_get_settings_from_flash(void) and int r_save_settings(int x) functions.
  5,	Elminate all the index stuff in g.pns.
  6,	Fix all of the atoi's.

*/

#include <stdio.h>
#include "nios.h"
//#include "excalibur.h"
//#include "nios2.h"
#include "plugs.h"
//#include "ethernet_mon_cont_peripherals1.h"
#include "vtw_global8.h"
#include "discrete_io.h"
#include "plugs_example_designs.h"

#if nm_cpu_architecture == nios2 // Cyclone II version of board with EPCS flash
	#include "my_epcs.h" 
#endif

//////////////////// BUILD OPTIONS //////////////////////
#define BUILD_FOR_CONTROL 1 	// Monitor only assumed if zero
#define BUILD_FOR_INTERRUPT_CONTROL 1	// Ignored if BUILD_FOR_CONTROL == zero

// Must change chip design for interrupt driven Ethernet, can't just select it here.
#define BUILD_FOR_POLLED_ETHERNET 1

#define BUILD_FOR_TCP 0 // If zero, UDP will be assumed
#define HANDSHAKE_WITH_ACK_AND_NAK 1 // Only meaningful in UDP.
///////////////////////////////////////////////////////////

//////////////////////// FIRMWARE VERSION ////////////////////////
#define FIRMWARE_VERSION "emc_6_0a,"

// These items automatically change depending on the build options.
#if !BUILD_FOR_CONTROL
	#define FIRMWARE_TYPE " monitor only,"
	#define CONTROL_TYPE ""
#else
	#define FIRMWARE_TYPE " monitor_and_control,"
	#if BUILD_FOR_INTERRUPT_CONTROL
		#define CONTROL_TYPE " interrupt driven control,"
	#else
		#define CONTROL_TYPE " polled control,"
	#endif
#endif


#if BUILD_FOR_POLLED_ETHERNET
	#define ETHERNET_TYPE " polled Ethernet,"
#else
	#define ETHERNET_TYPE " interrupt driven Ethernet,"
#endif

#if BUILD_FOR_TCP
	#define PROTOCOL " TCP,"
#else
	#define PROTOCOL " UDP,"
#endif

#if HANDSHAKE_WITH_ACK_AND_NAK
	#define HANDSHAKE " handshake"
#else
	#define HANDSHAKE " no handshake"
#endif
//////////////////////////////////////////////////////////////////

#define BUILD_FOR_UDP !BUILD_FOR_TCP
#define BUILD_FOR_POLLED_CONTROL !BUILD_FOR_INTERRUPT_CONTROL

#if BUILD_FOR_CONTROL
	#include "combined_control_blocks3.h" // Both polled and interrupt driven control are supported in the "combined" file.
#endif

#define USE_DIAG_ARRAY 1  // Set to one if it is desired save diagnostic data to an array and transmit after the shot

// Protect or unprotect EPCS flash.  Has no effect on the old Apex20KE based board. Protecting flash may dramatically
// increase write times when writing single bytes or halfwords as the device will be unprotected and protected each write.
#define PROTECT_EPCS_AT_POWER_UP 1 // Set to 1 to protect EPCS flash.  

/*///////////////////// Ethernet rules ///////////////////////////

1,	In UDP, anybody who calls nr_plugs_send_to() directly, must not wait for 
	an ACK or NAK or call the chk_for_ack_or_nak() function.  Only call nr_plugs_send_to()
	if you don't care about ACKs and NAKs.  

2,	If the remote guy sends an ACK or NAK, all other data/commands in the packet will
	be discarded.  ACKs and NAKs must be by themselves.

3,	This may be obvious but ACKs and NAKs themselves are never ACKed.

4,	Never wait for an ACK after sending a prompt. 

5,	Any ASCII data sent by me is never ACK'ed by the remote guy.

6,	Only binary data is ACK'ed and only if the "don't ack" flag is cleared.

7,	ASCII data that needs to be ACK'ed must use the "string" binary data type and send itself
	as binary data (use the send_binary() function which is also defined as SEND_STRING).

8,	Handshaking is never done in TCP mode, that is handshaking that originates here.

///////////////////////////////////////////////////////*/


/*////////////////////// Notes //////////////////////////

1,	With interrupt Ethernet, problems were found when an isr triggers, such as control.
	If the isr is short, all's OK.  However if the isr is long, such as executing a control
	block, problems were found.  For a test, a time delay was added to the 1mS isr and the
	same problem appeared.  It was also found that it made no difference whether or not the
	Ethernet isr had a lower or higher priority than the control isr.  For this reason, 
	interrupt driven Ethernet was scrapped for now.

2,	With interrupt driven control, some strange problems were found related to the control_supervisor
	isr.  in the lower portion of the isr, if a while loop was used, som problems were found.
	The while loop was replaced with a goto loop_again.  Also there seemed to be some issues
	with the INITIALIZE_CONTROL macro.  This was replaced with an initialize_control() function.
	This seemed to solve that problem.  There does not seem to any good explanation for these
	problems.  The code seems to be definitely more stable by making these changes.

	1-22-04 This is probably related to the ground bounce/setup time problems discussed
	in item 6 below.

3,	Make interrupt driven control the main configuration and make polled control the backup
	configuration.  That way both are available in case some unexpected problem occurs with
	interrupt driven control.

4,	If building for TCP, both control and Ethernet will have to be polled.

5,	in nr_plugs_idle() (plugs.c), changed to allow one recursion and rebuilt the plugs library.
	This is more of an issue if using TCP.

6,	Have had alot of problems with data getting mixed up, although everything looks OK.  
	Noticed that auto variables are saved in high memory.  Wonder if the stack or something
	having to do with the stack is corrupting data/variables onec in a while.  Noticed
	that when a problem is occuring, printf's used to attempt to find the problem will
	cause the problem to move or go away completely!!!  Found that if a critical block
	of data is made static, it's stored much lower in memory any apparently out of harms
	way.

	1-21-04 Found the above problem to be either ground bounce on the larger GSI74116 SRAMs
	or setup time on the address/data lines or both. All address, data and signal lines to the 
	SRAMs and FLASH use the slow slew rate feature of the PLD in an attempt to minimize bounce.
	One setup cycle was inserted into the SRAM timing.  Flash reading and writing was not
	affected.  The smaller 72116 SRAMs did not have this problem.  They could operate at
	full speed with no setup or wait states and without the slow slew
	rate feature.  PLD version 04000000 incorperates these changes.

	
///////////////////////////////////////////////////////*/

/*///////////////////// BUILDING THIS VERSION /////////////////////////

1, From the SOPC builder command shell

	a, nios2-build -b 0x2000000 emc_6_?.c<CR>
	b, nios2-srec2flash  emc_5_?.srec -flash_address=0x140000 -ram_address=0x400000 -copy_size=0x40000<CR>
	c, nios2-run emc_5_?.flash<CR>  To download the flash file.


2, To program EPCS flash over a USB Blaster cable:

Creating flash file for the FPGA configuration
sof2flash --epcs --input=D:/qdesigns_5_1/CycloneII/emc_chip_12010201/emc_chip_12010201.sof --output=emc_chip_12010201.flash

Programming EPCS flash with the FPGA configuration
nios2-flash-programmer --cable='USB-Blaster [USB-0]' --epcs --base=0x0020b000 emc_chip_12010201.flash

Creating flash file for the project
elf2flash --epcs --after=emc_chip_12010201.flash --input=emc_6_0.elf --output=emc_6_0.flash --boot=$SOPC_KIT_NIOS2/components/altera_nios2/boot_loader_epcs.srec

Programming EPCS flash with the project
nios2-flash-programmer --cable='USB-Blaster [USB-0]' --epcs --base=0x0020b000 emc_6_0.flash


2, This file is built using:

	a, Nios II V5.1 
	b, SOPC builder V5.1
	c, Quartus II V5.1 SP2.01

3,	This firmware is to run under chip version emc_chip_10010200.hexout.flash or later

4,	The device currently targeted is an Altera EP20K200EFC484-1

*/

#ifndef MAX_VAR
	#define MAX_VAR 511
#endif

#ifndef MAX_STEP
	#define MAX_STEP 203
#endif

// This should be at least 32 bytes or so, something like that.
#define CHUNK_SIZE 1350

#define CONTROL_COMPILE_DEBUG 0
#define CONTROL_INTERRUPT_DEBUG 0
#define MON_DEBUG 0

#define CHECK_NULL 0
#define AUTO_NULL 1

#define NO_ACK 0
#define ACK 6
#define NAK 21

#if nm_cpu_architecture == nios2 // Cyclone II version of board with EPCS flash
	#define GM_FlashBase 0
	#define GM_FlashTop 0x200000
	#define GM_FlashExec nasys_main_flash // EPCS boot loader
#else
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
#endif

#ifndef NULL
 #define NULL 0
#endif

// For sending strings with handshaking with the send_binary() function for readability
#define SEND_STRING send_binary

#define SEND_PROMPT  send_string((void *)g.spc, prompt_str);
#define SEND_PROMPT1 send_string((void *)g.spc, prompt_str1);
#define SEND_PROMPT_ACK  SEND_STRING((void *)g.spc, STRING, prompt_str, strlen(prompt_str), ACK);
#define SEND_PROMPT1_ACK SEND_STRING((void *)g.spc, STRING, prompt_str1, strlen(prompt_str1), ACK);

// Define binary data types
#define POS_BASED_DATA 0
#define TIME_BASED_DATA 1
#define COMPUTED_SHOT_DATA 2
#define O_SCOPE_DATA 3
#define OP_DATA 4
#define STRING 5 // An oxymoron
#define IO_CHANGE_REPORTING 6
#define SINGLE_SAMPLE_ANALOG_REPORTING 7
#define BLOCK_SAMPLE_ANALOG_REPORTING 8


#define LOCAL_IS_CURRENT 1
#define PLANT_WIDE_IS_CURRENT 2

#define FREE 1
#define NO_FREE 0


// Flash addresses for saving vtw items
#if nm_cpu_architecture == nios2
	// EPCS flash addresses, we'll allow for some extra room in case we'd like to increase the vars or cprog size
	#define VAR_CPROG_FLASH_SECTOR_ADDR 0x1F0000 // EPCS16 Sector 31
	#define VAR_FLASH_ADDR 0x1F0000
	#define CPROG_FLASH_ADDR 0x1F2000 // to 0x1F4000
	#define NETWORK_SETTINGS ((ns_plugs_persistent_network_settings *)(0x1E0000))
	#define IO_NETWORK_SETTINGS ((enet_io_net_settings *)(0x1E0100))
#else
	#define VAR_CPROG_FLASH_SECTOR_ADDR 0x108000
	#define VAR_FLASH_ADDR 0x108000
	#define CPROG_FLASH_ADDR 0x109000
	#define NETWORK_SETTINGS ((ns_plugs_persistent_network_settings *)(nasys_main_flash + 0x6000))
	#define IO_NETWORK_SETTINGS ((enet_io_net_settings *)(nasys_main_flash + 0x6100))
#endif

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
// | this global state. 
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
	ns_plugs_persistent_network_settings pns; // So we can change network settings without hello_plugs
	} s_c_germs_globals;

static s_c_germs_globals g; // bss inited to = {0};


#define RECV_BUFF_LENGTH 1024

typedef volatile struct
	{
	int plug;
	int connected; // 0:waiting, 1:connected, 2:disconnected
	net_32 remote_ip_address;
	net_16 remote_port;
	unsigned char c[RECV_BUFF_LENGTH]; // receive buffer, used to gather received bytes
	int show_prompt;
	int ack; // Lets us know when we received an ACK from the remote computer, UDP only 
	int nak; // Lets us know when we received an NAK from the remote computer, UDP only
	int send_op_packets;
	int send_o_scope_data;
	int op_data_send_in_progress;
	int o_scope_send_in_progress;
	volatile unsigned char *head_c;
	volatile unsigned char *tail_c;
	} s_connection_status;

// bss inited to = {0};
static s_connection_status local_net_settings, plant_net_settings, io_net_settings;


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
unsigned short int dac_word[5]; // For storing current dac value, raw counts.
unsigned int stack[20];
unsigned char stack_ptr;
unsigned int prog_ctr;
unsigned char samp; // Servo amp interface
//int prev_pot_settings;
//int prev_demod_phase_settings;
int timer2_timeout;
int amp_type;
//int warning;
int saved_warning;
int block_ana_report;
//int fatal_error;
//unsigned int slow_loop_time; // 25mS
//unsigned int fast_loop_time; // 2.5mS
int dac_write_in_progress; // To prevent DAC recursion
int xmit_in_progress;  // To prevent transmit recursion
unsigned int last_pb_time;
int serial_show_prompt;
//unsigned int gp_config_word;
int counts_to_assume_zspeed;
unsigned int blk_timeout_time;
int immed_in_process;
int io_int1; // Used for checking descrete I/O to generate control interrupts
int on_off1;
int io_int2;
int on_off2;
int analog_reporting;
int o_scope;
unsigned char cont_int_out_status; // Status of the control interrupt output bits
int been_below_trig_voltage; // Used by o_scope isr
int xmitting_shot_data;
int tst_ctr; // For test
unsigned int junk;
} vtw_globals;

static vtw_globals vtg;


typedef volatile struct
{
char header; // 'B' == Binary data
unsigned char data_type;
unsigned short int flags; 
unsigned short int data_set_num;
unsigned short int packet_num;
unsigned short int num_of_packets;
unsigned short int num_of_bytes;
} binary_header;

// Binary header flags bit definition
#define DONT_ACK 0



typedef volatile struct
{
unsigned short int dyn_ch1;
unsigned short int dyn_ch2;
unsigned short int dyn_ch3;
unsigned short int dyn_ch4;
unsigned short int dyn_ch5;
unsigned short int dyn_ch6;
unsigned short int dyn_ch7;
unsigned short int dyn_ch8;
unsigned short int cana_ch1;
unsigned short int cana_ch2;
unsigned short int cana_ch3;
unsigned short int cana_ch4;
unsigned short int wana_ch1;
unsigned short int wana_ch2;
unsigned short int wana_ch3;
unsigned short int wana_ch4;
} op_packet_analog_channels;


// New struct with 16 analog channels
struct
{
int dac[4]; // DAC voltage * 1000
op_packet_analog_channels channels; // 16 channels
unsigned short int dummy_channels[4]; // To make the V5 packet consistant with the V4.xx packet
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




// Whenever an input changes during idle time, this is the data transmitted.  This data is binary data type 6.
typedef volatile struct
{
unsigned int isw1;		
unsigned int isw4;
unsigned int isw1_bits_that_changed;
unsigned int isw4_bits_that_changed;		
unsigned int one_ms_counter; 
} input_change_report;



/*
typedef volatile struct
{
int io_num;
short int all_ana_channels[2000][24];
} scope_data;


typedef volatile struct
{
int io_num;
short int all_ana_channels[24];
} scope_data_one_sample;
*/

//scope_data scope_data_packet; // Enable this for fixed array in memory



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
	unsigned int sample_num;  // For buffering
	} combined_sample; 

//combined_sample data_sample[3501];
combined_sample *data_sample; 
int num_of_shot_data_samples; // Calculated in monitor_supervisor()


// This structure contains additional data that can be used for diagnostic purposes
typedef volatile struct
{
unsigned int prog_ctr; // Control block currently executing
int warning;
int fatal_error;
unsigned short int dac_word1;
unsigned short int dac_word2;
unsigned short int dac_word3;
unsigned short int dac_word4;
int not_used1; // Reserved for future expansion
int not_used2;
int not_used3;
int not_used4;
} adjunct_combined_sample;


// Closed loop diagnostic structure.  Not using this yet, but it could be in the future
typedef volatile struct
{
unsigned int prog_ctr;
short int block_type; // Identifier
short int zone;
unsigned int one_ms_timer;
unsigned int isw1;
unsigned int isw4;
unsigned int osw1;
int actual_pos;
int actual_vel;
int command;
int actual;
int error;
unsigned short int dac_word1;
unsigned short int dac_word2;
unsigned short int dac_word3;
unsigned short int dac_word4;
short int computed_command;
unsigned short int cmd_voltage;
unsigned short int lvdt_feedback;
unsigned short int not_used1; // Keep everything on 32 bit word boundaries.
int warning;
int fatal_error;
int not_used2;
int not_used3;
} control_diagnostic_data;

control_diagnostic_data *cont_diag_data; 


// Monitor globals
unsigned char bounce_back;
unsigned char ls_stat[6];
short unsigned int pb_ctr, tb_ctr, sample_ctr, bisc_ctr, tb_time_interval_ctr; // Position and time based array counters.
int shot_num=0;
unsigned int timed_coll_interval; // Working copy
int vel_eos_enable; // Working copy
int vel_eos;  // Working copy
int analog_mode6_ctr;
// Look for at home is to inform main() that the cylinder is at home and to send a message to the remote
// computer.  This is inform the remote computer that it is safe to load the control program at this time.
int look_for_at_home; 

// Pressure control globals



struct
	{
	unsigned int cycle_time;
	unsigned short int biscuit_size;
	short int eos_pos;
	unsigned short ch9;
	unsigned short ch10;
	unsigned short ch11;
	unsigned short ch12;
	unsigned short ch13;
	unsigned short ch14;
	unsigned short ch15;
	unsigned short ch16;
	} comp_shot_parameters;



// Definition for all analog channels
typedef volatile struct
	{
	unsigned short int ana_ch1;
	unsigned short int ana_ch2;
	unsigned short int ana_ch3;
	unsigned short int ana_ch4;
	unsigned short int ana_ch5;
	unsigned short int ana_ch6;
	unsigned short int ana_ch7;
	unsigned short int ana_ch8;
	unsigned short int ana_ch9;
	unsigned short int ana_ch10;
	unsigned short int ana_ch11;
	unsigned short int ana_ch12;
	unsigned short int ana_ch13;
	unsigned short int ana_ch14;
	unsigned short int ana_ch15;
	unsigned short int ana_ch16;
	unsigned short int cana_ch1;
	unsigned short int cana_ch2;
	unsigned short int cana_ch3;
	unsigned short int cana_ch4;
	unsigned short int wana_ch1;
	unsigned short int wana_ch2;
	unsigned short int wana_ch3;
	unsigned short int wana_ch4;
	} analog_channels;


// AD setup words.  Lower 16 bits are the channel setup, lower 4 bits of upper 16 bit word is the mux setup.
#define ANA_CH1_SETUP 0x00008350
#define ANA_CH2_SETUP 0x00008750
#define ANA_CH3_SETUP 0x00008b50
#define ANA_CH4_SETUP 0x00008f50
#define ANA_CH5_SETUP 0x00009350
#define ANA_CH6_SETUP 0x00009750
#define ANA_CH7_SETUP 0x00009b50
#define ANA_CH8_SETUP 0x00009f50
#define ANA_CH9_SETUP 0x0000a350
#define ANA_CH10_SETUP 0x0000a750
#define ANA_CH11_SETUP 0x0000ab50
#define ANA_CH12_SETUP 0x0000af50
#define ANA_CH13_SETUP 0x0000b350
#define ANA_CH14_SETUP 0x0000b750
#define ANA_CH15_SETUP 0x0000bb50
#define ANA_CH16_SETUP 0x0000bf50
// All upper channels use ch16 setup since they are muxed to that channel
#define CANA_CH1_SETUP 0x0008bf50 // Control channel #1 (channel #17)
#define CANA_CH2_SETUP 0x0009bf50 // Control channel #2 (channel #18)
#define CANA_CH3_SETUP 0x000abf50 // Control channel #3 (channel #19)
#define CANA_CH4_SETUP 0x000bbf50 // Control channel #4 (channel #20)
#define WANA_CH1_SETUP 0x000cbf50 // Wide input range channel #1 (channel #21)
#define WANA_CH2_SETUP 0x000dbf50 // Wide input range channel #2 (channel #22)
#define WANA_CH3_SETUP 0x000ebf50 // Wide input range channel #3 (channel #23)
#define WANA_CH4_SETUP 0x000fbf50 // Wide input range channel #4 (channel #24)



// ad setups array locations
#define ANA_CH1 0
#define ANA_CH2 1
#define ANA_CH3 2
#define ANA_CH4 3
#define ANA_CH5 4
#define ANA_CH6 5
#define ANA_CH7 6
#define ANA_CH8 7
#define ANA_CH9 8
#define ANA_CH10 9
#define ANA_CH11 10
#define ANA_CH12 11
#define ANA_CH13 12
#define ANA_CH14 13
#define ANA_CH15 14
#define ANA_CH16 15
#define CANA_CH1 16
#define CANA_CH2 17
#define CANA_CH3 18
#define CANA_CH4 19
#define WANA_CH1 20
#define WANA_CH2 21
#define WANA_CH3 22
#define WANA_CH4 23


unsigned int ad_setup_words[24] = { ANA_CH1_SETUP,
									ANA_CH2_SETUP,
									ANA_CH3_SETUP,
									ANA_CH4_SETUP,
									ANA_CH5_SETUP,
									ANA_CH6_SETUP,
									ANA_CH7_SETUP,
									ANA_CH8_SETUP,
									ANA_CH9_SETUP,
									ANA_CH10_SETUP,
									ANA_CH11_SETUP,
									ANA_CH12_SETUP,
									ANA_CH13_SETUP,
									ANA_CH14_SETUP,
									ANA_CH15_SETUP,
									ANA_CH16_SETUP,
									CANA_CH1_SETUP,
									CANA_CH2_SETUP,
									CANA_CH3_SETUP,
									CANA_CH4_SETUP,
									WANA_CH1_SETUP,
									WANA_CH2_SETUP,
									WANA_CH3_SETUP,
									WANA_CH4_SETUP
									};


typedef volatile struct
{
short int control_channels[2000][4]; // Can be cana or wana depending on the servo amplifier.
} scope_data;

scope_data *scope_data_packet; // Enable this for a pointer.

typedef volatile struct
{
int report_number; // 1, 2 or 3
analog_channels channels;
} input_based_analog_single;


typedef volatile struct
{
int report_number; // 1, 2 or 3
unsigned int sample_interval_in_uS; // For reference
analog_channels channels[2000];
} input_based_analog_block;

input_based_analog_block *analog_block_report;


volatile unsigned short int *shared_memory = na_ad_result_ram; // 64 bytes of shared memory; // 32 halfwords
volatile unsigned int *ad_setup = na_ad_setup_ram; // 36 bytes of shared memory
volatile unsigned short int *ad_cmd = na_ad_cmd_ram; // 4 bytes of shared memory

// Due to a bug in the Cyclone II legacy SDK support, we have to include the spi structure info here
#if nm_cpu_architecture == nios2 // Cyclone II version of board with EPCS flash
	// SPI Registers
	typedef volatile struct
	  {
	  int np_spirxdata;       // Read-only, 1-16 bit
	  int np_spitxdata;       // Write-only, same width as rxdata
	  int np_spistatus;       // Read-only, 9-bit
	  int np_spicontrol;      // Read/Write, 9-bit
	  int np_spireserved;     // reserved
	  int np_spislaveselect;  // Read/Write, 1-16 bit, master only
	  int np_spiendofpacket;  // Read/write, same width as txdata, rxdata.
	  } np_spi;

	// SPI Status Register Bits
	enum
	  {
	  np_spistatus_eop_bit  = 9,
	  np_spistatus_e_bit    = 8,
	  np_spistatus_rrdy_bit = 7,
	  np_spistatus_trdy_bit = 6,
	  np_spistatus_tmt_bit  = 5,
	  np_spistatus_toe_bit  = 4,
	  np_spistatus_roe_bit  = 3,

	  np_spistatus_eop_mask  = (1 << 9),
	  np_spistatus_e_mask    = (1 << 8),
	  np_spistatus_rrdy_mask = (1 << 7),
	  np_spistatus_trdy_mask = (1 << 6),
	  np_spistatus_tmt_mask  = (1 << 5),
	  np_spistatus_toe_mask  = (1 << 4),
	  np_spistatus_roe_mask  = (1 << 3),
	  };

	// SPI Control Register Bits
	enum
	  {
	  np_spicontrol_sso_bit   = 10,
	  np_spicontrol_ieop_bit  = 9,
	  np_spicontrol_ie_bit    = 8,
	  np_spicontrol_irrdy_bit = 7,
	  np_spicontrol_itrdy_bit = 6,
	  np_spicontrol_itoe_bit  = 4,
	  np_spicontrol_iroe_bit  = 3,

	  np_spicontrol_sso_mask   = (1 << 10),
	  np_spicontrol_ieop_mask  = (1 << 9),
	  np_spicontrol_ie_mask    = (1 << 8),
	  np_spicontrol_irrdy_mask = (1 << 7),
	  np_spicontrol_itrdy_mask = (1 << 6),
	  np_spicontrol_itoe_mask  = (1 << 4),
	  np_spicontrol_iroe_mask  = (1 << 3),
	  };
#endif

char ack_str[] = {'\006','\n','\0'};  // ACK<LF>
char nak_str[] = {'\025','\n','\0'};  // ACK<LF>
char hello_str[] = {'K','_','#',' ','H','e','l','l','o','\n','\0'}; // K_# Hello<LF>
char prompt_str[] = {'+','\0'}; // +
char prompt_str1[] = {'\n','+','\0'}; // <LF>+
char timeout_str[] = {'R',':','1','9',' ','#','C','y','c','l','e',' ','s','t','a','r','t',' ','t','i','m','e','o','u','t','\n','\0'};   
char cs_detected_str[] = {'R',':','2','0',' ','#','C','y','c','l','e',' ','s','t','a','r','t',' ','d','e','t','e','c','t','e','d','!','\n','\0'};   
// If the CS signal occured on the fast input #300, we'll ID it here.
char cs_detected_300_str[] = {'R',':','2','0',' ','#','C','y','c','l','e',' ','s','t','a','r','t',' ','d','e','t','e','c','t','e','d','!',' ','3','0','0','\n','\0'};   
// +--------------------------------
// | Local Prototypes


static char *r_get_monitor_string(void);
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
int readBuffer_oneChar(s_connection_status *u);
int readBuffer(s_connection_status *u, char *buff);
int loadBuffer_oneChar(s_connection_status *u, char c);
int loadBuffer(s_connection_status *u, char* payload, int payload_length);


int uart_send_byte(void *context, char *c);
int uart_recv_byte(void *context, char *c);
int null_function(void *context);

int udp_send_byte(void *context, char *c);
int udp_recv_byte(void *context, char *c);
int udp_init(void *context);

int tcp_listen_proc(int plug_handle, void *context, host_32 remote_ip_address, host_16 remote_port);
int tcp_send_byte(void *context, char *c);
int tcp_recv_byte(void *context, char *c);
int plugs_init(void);
int net_recv_proc(int plug_handle, void *context, ns_plugs_packet *p, void *payload, int payload_length);
int process_net_commands(s_connection_status *u, void *payload, int payload_length);
int tcp_listen_proc(int plug_handle, void *context, host_32 remote_ip_address, host_16 remote_port);
int network_xmit(void *context, void *data, int length, int ack, net_32 remote_ip, net_16 remote_port);
//int tcp_check(void *context);

// Visi-Trak Worldwide function prototypes
int parse_control_program_command(uc *orig_cmd_str);
int parse_variable_command(unsigned char *orig_cmd_str);
int parse_operational_command(unsigned char *orig_cmd_str);
void save_cprog_to_flash(void);
void save_vars_to_flash(void);
void reload_vars_from_flash(void);
void reload_cprog_from_flash(void);
int transmit_monitor_data(void *context);
void reset_control_program(int saved_warning_action);
void stop_control(void);
int whos_the_current_plug(void);
inline void control_supervisor(int context);
inline void monitor_supervisor(void);
void update_control_parameters(void);
void ram_clear(void);
int chk_for_ack_or_nak(void *context);
int send_binary(void *context, unsigned char data_type, void *data, int length, int ack);
void shut_down_connection(void *context);
void send_op_packets(void *context);
void initialize_control(void);
void set_analog_mode(int mode);
int servo_amp_interface(int action);
int update_servo_amp_interface(void);
void load_ad_processor_program(void);
void stop_o_scope_timer(int free_);
int start_o_scope_timer(int mode);
int analog_reporting_single(int report_num);
int input_change_reporting(unsigned int isw1_bits_that_changed, unsigned int isw4_bits_that_changed);
// VTW common flash prototypes
int vtw_flash_erase_sector(unsigned short *flash_base, unsigned short *sector_address);
int vtw_flash_erase(unsigned short *flash_base);
int vtw_flash_write(unsigned short *flash_base, unsigned short *addr, unsigned short val);
int vtw_flash_write_buffer(unsigned short *flash_base, unsigned short *start_address, unsigned short *buffer, int size);
int vtw_flash_read_buffer(unsigned char *start_address, unsigned char *buffer, int size);
//int convert_monitored_channels(int sample_ctr);


// Load my private epcs interface routines.  These do not use any of Altera's obj's.
#if nm_cpu_architecture == nios2 // Cyclone II version of board with EPCS flash
	#include "my_epcs.c" 
#endif
// +--------------------------------
// | Local Little Routines

typedef void (*r_dull_proc_ptr)(void);

static void r_goto(unsigned int addr)
{
  r_dull_proc_ptr p;
#if nm_cpu_architecture == nios_32
  // Nios function addresses are...funny, because the architect was...
  addr /= 2;
#endif
  p = (r_dull_proc_ptr)(addr);
  (*p)();
}

static char *r_get_monitor_string(void)
	{
		return "VTW Monitor and Control server";
	}


static int r_in_flash_range(ui addr)
	{
	int result = 0;
	
#ifdef GM_FlashBase // This is only here to silence a compile warning.  In Nios2 mode, GM_FlashBase is zero.  An ui is always >= zero.
	#if nm_cpu_architecture == nios2
		result = addr < (ui)GM_FlashTop; 
	#else
		result = (addr >= (ui)GM_FlashBase) && (addr < (ui)GM_FlashTop);
	#endif
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
			vtw_flash_write((void *)GM_FlashBase,(us *)(addr & ~1), halfword);
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
	unsigned short buf;

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
			{
			if(r_in_flash_range(addr_lo + i * 2))
				{
				vtw_flash_read_buffer((void *)(addr_lo + i * 2), (void *)&buf, 2); // Get halfword (2 bytes) from flash
				}
			else
				{
				buf = *(unsigned short *)(addr_lo + i * 2); // Otherwise we'll treat it as a memory mapped peripheral.
				}

			sprintf(g.b + strlen(g.b)," %04X",buf);
			}

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
		if(send_string((void *)g.spc,g.b)) break; 
		//printf("%s", g.b); // For test
		
		addr_lo += word_count * 2;
		}
	//send_string((void *)g.spc,"[*]"); // For test
	}



// Function to convert a ascii hex string to int.  No signs are dealt with
int htoi (char *str)
{
int x, return_val = 0;
char ch;

for (x = 0; x<=7; x++)
	{
	if (str[x] >= '0' && str[x] <= '9') ch = str[x] & 0x0f;
	else if (str[x] >= 'a' && str[x] <= 'f') ch = (str[x] & 0x0f)+9;
	else if (str[x] >= 'A' && str[x] <= 'F') ch = (str[x] & 0x0f)+9;
	else break; // Non hex digit encountered, exit 

	if (x == 0) //First time through
		{
		return_val = ch;
		}
	else
		{
		return_val <<= 4; return_val |= ch; 
		}
		
	}
return (return_val);
}


//int parse_germs_command(uc *command, send_proc callback, void *callback_context)
int parse_germs_command(uc *command)

	{
	int result = 0;
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
		//do_help(); // For test

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
		case 27: // Escape
			main();
		break;

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
	
		case 'n':
			parse_net_setup_command(command-1);
			break;

		case 's':
			do_s_command();
			break;

		case 'p': // Show/don't show prompt
			if (command[0] == '1')
				{
				if ((int)g.spc==9999) vtg.serial_show_prompt=1; // Serial mode wants prompts
				else ((s_connection_status *)g.spc)->show_prompt=1; // Current plug wants prompts
				}
			else 
				{
				if ((int)g.spc==9999) vtg.serial_show_prompt=0; // Serial mode doesn't want prompts
				else ((s_connection_status *)g.spc)->show_prompt=0;	// Current plug doesn't want prompts
				}
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
			sprintf(g.b, "?[%d]\n", (int)command_letter); // For test

			send_string((void *)g.spc, g.b);
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
	#if nm_cpu_architecture == nios2
		if (addr < GM_FlashTop)
			{
			return 1; // Can't possibly execute code from EPCS flash, so we'll depart here
			}
	#endif

	sprintf(g.b,"# executing at %08x\n",addr);
	send_string((void *)g.spc,g.b);
	r_goto(addr);
	
	return 0;
	}


int do_e_command(void)
	{
	uc **wp = (unsigned char **)&g.command;
	ui addr;
	uc break_char;
	
	addr = r_scan_string_for_number(wp,&break_char);
	//printf("break_char = %c\n", break_char);
	
	if (break_char == '!') // Bulk erase
		{
		// Erase the entire flash memory
		send_string((void *)g.spc, "# Attempting to erase entire flash!!!\n");
		if(vtw_flash_erase((us *)GM_FlashBase)) send_string((void *)g.spc, "# Unable to erase, flash may be protected!!!\n");  
		else send_string((void *)g.spc, "# Erased entire flash!!!\n");
		}
#if nm_cpu_architecture == nios2
	else if (break_char == ')') // Protect entire flash, EPCS only at this time
		{
		if(r_epcs_protect_region(epcs_protect_all)) send_string((void *)g.spc, "# Unable to protect flash!!!\n");
		else send_string((void *)g.spc, "# Protected entire flash\n");
		}
	else if (break_char == '(') // Unprotect entire flash, EPCS only at this time
		{
		if(r_epcs_protect_region(epcs_protect_none)) send_string((void *)g.spc, "# Unable to unprotect flash!!!\n");
		else send_string((void *)g.spc, "# Unprotected entire flash!!!\n");
		}
#endif
	 else
		{
		#ifdef GM_FlashBase
			if(r_in_flash_range(addr))
				{
				#if nm_cpu_architecture == nios2
					sprintf(g.b,"# Attempting to erase sector: %08x\n",addr);
					send_string((void *)g.spc, g.b);
					if(vtw_flash_erase_sector((us *)GM_FlashBase,(us *)addr)) send_string((void *)g.spc, "# Unable to erase sector, flash may be protected!!!\n");  
					else send_string((void *)g.spc, "# Erased sector!!!\n");
				#else
					sprintf(g.b,"# erasing: %08x\n",addr);
					send_string((void *)g.spc,g.b);
					vtw_flash_erase_sector((us *)GM_FlashBase,(us *)addr);
				#endif
				}
			else
		#endif
			send_string((void *)g.spc,"not flash\n");
		
		g.memory_position = addr; // (so a <return> shows what we just erased
		}
	
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
	//printf("in do_m_command(), addr_lo = 0x%x, addr_hi = 0x%x and break_char = %c\n", addr_lo, addr_hi, break_char); // For test
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

		if(break_char != 0)
			{
			r_stash_byte(addr_lo,v & 0xff); 
			r_stash_byte(addr_lo+1,(v >> 8) & 0xff); 
			
			addr_lo += 2;
			}
		else break;
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
				r_goto(record_address);
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
			//if(g.checksum != 0xff)
			if(g.checksum != 0)
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
	send_string((void *)g.spc,"#Welcome to FasTrak II, Cyclone II edition\n");
	sprintf(g.b,"#CPU Architecture: %s\n",nm_cpu_architecture_string);
	send_string((void *)g.spc,g.b);

	sprintf(g.b,"#System Name: %s\n",nm_system_name_string);
	send_string((void *)g.spc,g.b);

	sprintf(g.b,"#System ID: %s\n",nm_monitor_string);
	send_string((void *)g.spc,g.b);
	
	return 0;
	}




int do_help(void)
	{
	//dump_lan91c111(); // For test
	//dump_plug_info(((s_connection_status *)g.spc)->plug);

	send_string((void *)g.spc, "#\n");
	send_string((void *)g.spc, "# The following commands are available:\n");
	send_string((void *)g.spc, "# g <addr>                  -- execute at addr\n");
	send_string((void *)g.spc, "# e <addr>                  -- erase flash memory at addr\n");
	send_string((void *)g.spc, "# r <addr1>-<addr2>         -- relocate S-records from addr1 to addr2\n");
	send_string((void *)g.spc, "# m <addr>                  -- show memory at addr\n");
	send_string((void *)g.spc, "# m <addr1>-<addr2          -- show memory range from addr1 to addr2\n");
	send_string((void *)g.spc, "# m <addr>:<val1> <val2>... -- write halfwords at addr\n");
	send_string((void *)g.spc, "# m <addr1>-<addr2>:<val>   -- fill range with halfword val\n");
	send_string((void *)g.spc, "# s <S-record data>          -- write S-record to memory\n");
	send_string((void *)g.spc, "# :<Intel hex data>         -- write Intel hex record to memory\n");
	send_string((void *)g.spc, "# w                         -- show processor version\n");
	send_string((void *)g.spc, "# help                      -- show this help\n");
	send_string((void *)g.spc, "#\n");
	}




/////////////////////////////// Common flash routines //////////////////////////////////

// Theese common routines access either the AMD flash on the Apex20KE based boards or EPCS on the Cyclone II board
// depending on the CPU architecture detected.  In theory, this program can then be compiled for either the Apex
// or Cyclone II devices.  Calling routines call a "common" (not to be confused with CFI) flash interface
// and we decide here which type of flash we have and call the approiate routines.
 
// Nios / AMD flash routines
//int nr_flash_erase_sector(unsigned short *flash_base, unsigned short *sector_address)
//int nr_flash_erase(unsigned short *flash_base)
//int nr_flash_write(unsigned short *flash_base, unsigned short *addr, unsigned short val)
//int nr_flash_write_buffer(unsigned short *flash_base, unsigned short *start_address, unsigned short *buffer, int size)

// Private Nios II / EPCS flash routines
//extern unsigned char r_epcs_read_status ();
//extern unsigned long r_epcs_lowest_protected_address();
//extern int r_epcs_write_status (unsigned char value);
//extern int r_epcs_protect_region (int bpcode);
//extern int r_epcs_read_byte (unsigned long address, unsigned char *data);
//extern int r_epcs_write_byte (unsigned long address, unsigned char data);
//extern int r_epcs_erase_sector (unsigned long address);
//extern int r_epcs_erase_bulk ();
//extern int r_epcs_read_buffer (unsigned long address, int length, unsigned char *data);
//extern int r_epcs_write_page (unsigned long address, int length, unsigned char *data);
//extern int r_epcs_write_buffer (unsigned long address, int length, unsigned char *data);
//extern int r_epcs_address_past_config (unsigned long *addr);
//
//EPCS function error codes
//#define na_epcs_success                 0
//#define na_epcs_err_device_not_present  1
//#define na_epcs_err_device_not_ready    2
//#define na_epcs_err_timedout            3
//#define na_epcs_err_write_failed        4
//#define na_epcs_invalid_config          5

// VTW common flash prototypes
//int vtw_flash_erase_sector(unsigned short *flash_base, unsigned short *sector_address);
//int vtw_flash_erase(unsigned short *flash_base);
//int vtw_flash_write(unsigned short *flash_base, unsigned short *addr, unsigned short val);
//int vtw_flash_write_buffer(unsigned short *flash_base, unsigned short *start_address, unsigned short *buffer, int size);
//int vtw_flash_read_buffer(unsigned char *start_address, unsigned char *buffer, int size);

int vtw_flash_erase_sector(unsigned short *flash_base, unsigned short *sector_address)
	{
	int rval, s;

	#if nm_cpu_architecture == nios2
		// EPCS flash sector erase
		rval=r_epcs_erase_sector((unsigned long)sector_address);
	#else
		rval=nr_flash_erase_sector((void *)flash_base,(void *)sector_address);
	#endif

	return (rval);
	}

int vtw_flash_erase(unsigned short *flash_base)
	{
	int rval, s;

	#if nm_cpu_architecture == nios2
		// EPCS flash bulk erase function
		rval=r_epcs_erase_bulk();
	#else
		rval=nr_flash_erase((void *)flash_base);
	#endif

	return (rval);
	}


// Writes a 16 bit halfword to flash.  EPCS functions support byte writes as well.  The old Apex code didn't write 
// single bytes to flash as the flash device used on that board was 16 bits wide.
int vtw_flash_write(unsigned short *flash_base, unsigned short *addr, unsigned short val)
	{	
	int rval, s;

	#if nm_cpu_architecture == nios2
		// EPCS flash buffer write function
		//printf("in fw, flash_base=%d, start_address=0x%x, val=0x%x\n", flash_base, addr, val);
		rval = r_epcs_write_buffer ((unsigned long)addr, 2, (void *)&val); // Write 2 bytes
	#else
		rval=nr_flash_write((void *)flash_base, (void *)addr, unsigned short val); // val is a 16 bit halfword (2 bytes)
	#endif

	return (rval);
	}


// Again, the Apex version wrote halfwords.  We'll mimic that functionality here in EPCS mode as well.
int vtw_flash_write_buffer(unsigned short *flash_base, unsigned short *start_address, unsigned short *buffer, int size)
	{
	int rval, s;
	
	#if nm_cpu_architecture == nios2
		// EPCS flash buffer write function
		//printf("in fwb, flash_base=%d, start_address=0x%x, buffer=0x%x, size=%d\n", flash_base, start_address, buffer, size);
		rval=r_epcs_write_buffer ((unsigned long)start_address, size*2, (void *)buffer); 
	#else
		rval=nr_flash_write_buffer((void *)flash_base, (void *)start_address, (void *)buffer, int size); 
	#endif
	
	return (rval);
	}


// We don't have to come here to read the AMD flash on the Apex board, it can be read directly as
// a memory mapped peripheral.  However, to maintain compatibility with our common flash functions, we'll
// call this function for all flash reads, even from AMD flash on the Apex board.
// This function must be used to read EPCS flash.  Since it is a serial flash, it cannot be directly read.
int vtw_flash_read_buffer(unsigned char *start_address, unsigned char *buffer, int size)
	{
	int rval, x;
	
	#if nm_cpu_architecture == nios2
		// EPCS flash read function, we have to call an Altera function to read EPCS flash since it's a serial device.
		//printf("start_address=0x%x, size = %d \n", start_address, size);
		rval=r_epcs_read_buffer ((unsigned long)start_address, size, (void *)buffer);
	#else
		// Since AMD flash is directly addressed, we'll just fetch the data here and fill the buffer.
		for (x=0; x<size/2; x++) // We'll work with half words here
			{
			(unsigned short *)buffer[x] = (unsigned short *)start_address[x];
			}
	#endif
	
	return (rval);
	}

/////////////////////////////// Ethernet stuff here ////////////////////////////////////

// This function will limit the string size to CHUNK_SIZE.  If larger, 998 will be returned
// and nothing will be transmitted.  In most cases, the string will be less than 1 line
// so this should not be a problem.  There is no handshaking with this routine and a 
// "connection", UDP or TCP is assumed to exist.
//
// send_string() can also be used to send data out the serial port.  If context == 9999,
// serial port printing will be assumed.
int send_string(void *context, char *s)
	{
	int result=0, sizeofstr;

	if ((sizeofstr = strlen(s)) > CHUNK_SIZE) return (998); // String too big, we'll bail here.

	if((int)context==9999) printf("%s", s); // Send out the serial port
	else result = network_xmit(context, s, sizeofstr, 0, 0, 0); // No ACK when sending strings here
	//result = network_xmit(context, s, sizeofstr, 1, 0, 0); // For test, ACK
	
	//if(result)
	//	{
	//	// For test
	//	printf("Sent the following string, but we failed:\n");
	//	printf("%s", s);
	//	}

	return (result); // If result == 999, network_xmit() was already in use, no data sent.
	}

// This function takes any length of of data, breaks it up into 
// CHUNK_SIZE-sizeof(binary_header) chunks, adds in a "binary header" to bring the packet
// size up to CHUNK_SIZE and transmits it.  Coool!  If UDP, after sending each packet,
// we'll wait for an ACK if ack==1 and handshaking is enabled.
int send_binary(void *context, unsigned char data_type, void *data, int length, int ack)
	{
	static unsigned short int data_set_num, prev_shot_num; // This routine will assign and increment data_set numbers.
	//static unsigned short int shot_num;
	int num_of_packets, leftover, length_to_send, return_val=0;
	int packet_size_wo_header = CHUNK_SIZE-sizeof(binary_header);
	int x, y, z;
	binary_header header;
	s_connection_status *u = (s_connection_status *)context;
	volatile char *working_data = (char *)data, temp[sizeof(binary_header)], *header_ptr;
	char packet_w_header[CHUNK_SIZE]; // previously made static because it was being corrupted as an auto?
	
	// We want to keep the same data set number for all shot data.
	//if (data_type == POS_BASED_DATA) // New shot data set
	//	{
	//	if (u==&local_net_settings) // Only increment shot num if local net
	//		{
	//		shot_num = ++data_set_num;
	//		//printf("Shot num=%d\n", shot_num); // For test
	//		header.data_set_num = shot_num;
	//		}
	//	}
	//else if (data_type==TIME_BASED_DATA || data_type==COMPUTED_SHOT_DATA) header.data_set_num = shot_num;
	//else header.data_set_num = data_set_num++;


	// Data types that are all part of the same data set (shot), shot_num incremented by monitor_supervisor
	// when CS is sensed.
	if (data_type==POS_BASED_DATA || data_type==TIME_BASED_DATA || data_type==COMPUTED_SHOT_DATA
		  || data_type==IO_CHANGE_REPORTING || data_type==SINGLE_SAMPLE_ANALOG_REPORTING 
				|| data_type==BLOCK_SAMPLE_ANALOG_REPORTING)
		{
		if (shot_num != prev_shot_num)
			{
			prev_shot_num = shot_num;
			header.data_set_num = ++data_set_num; // We've begun a new shot, increment data_set_num
			}
		else header.data_set_num = data_set_num;  // We're still within the "old" shot, don't increment
		}
	else header.data_set_num = ++data_set_num;  // Increment for all other types of data


	header_ptr = (char *)&header.header;
	header.header = 'B';
	header.data_type = data_type;
	header.flags = 0;  
	if(!ack || !HANDSHAKE_WITH_ACK_AND_NAK || BUILD_FOR_TCP)
		{
		header.flags |= (1<<DONT_ACK); // Set the DONT_ACK flag, if UDP we won't handshake.
		ack=0;  // Force ack to zero, it may have been set
		}

	num_of_packets = length / packet_size_wo_header;
	if (leftover = length % packet_size_wo_header) num_of_packets++; // Add in another packet if leftover
	header.num_of_packets = num_of_packets;		
	
	// Now send packets
	for (x=1; x<=num_of_packets; x++)
		{
		header.packet_num=x;
		if ((x==num_of_packets) && leftover)
			{
			length_to_send=leftover+sizeof(binary_header); // This is a leftover packet
			header.num_of_bytes=leftover; 
			}
		else 
			{
			length_to_send=packet_size_wo_header+sizeof(binary_header); // Essentially packet_size_wo_header + sizeof(binary_header).
			header.num_of_bytes=packet_size_wo_header; // Full length packet here
			}
		
		// Now load the header
		for (y=0; y<(int)sizeof(binary_header); y++)
			{
			packet_w_header[y]=header_ptr[y];
			}

		// And then the data
		//printf("len to send == %d\n", length_to_send); // For test
		//printf("&packet_w_header=0x%x\n", &packet_w_header);
		for (y=y, z=0; y<length_to_send; y++, z++)
			{
			packet_w_header[y] = working_data[z];
			}
		
		if (return_val = network_xmit((void *)u, (void *)packet_w_header, length_to_send, ack, 0, 0))
			{
			break; 
			}
		working_data += packet_size_wo_header;
		}
	return (return_val);
	}



// If remote_ip and remote_port are zero, a "connection" is assumed and these values will come 
// from the structure.  We can only ACK and NAK with someone we have a connection with, 
// ack is ignored if sending TCP
// 
int network_xmit(void *context, void *data, int length, int ack, net_32 remote_ip, net_16 remote_port)
	{
	int return_val=0, try_=1, x;
	s_connection_status *u = (s_connection_status *)context;
	net_32 working_ip;
	net_16 working_port;

	if (length>CHUNK_SIZE) {return_val=998; printf("network xmit, packet too large\n"); goto ext;} // Packet too big	
	if (remote_ip && remote_port) // Only matters if UDP
		{
		working_ip = remote_ip;
		working_port = remote_port;
		}
	else // Otherwise we assume a "connection" and use the ip and port info contained in the "u" structure.
		{
		if(u->connected)
			{
			working_ip = u->remote_ip_address;
			working_port = u->remote_port;
			}
		else
			{
			return_val=997;
			goto ext;
			}
		}
	
	// First let's see if we care whether or not a transmit is already in progress.
	if(BUILD_FOR_TCP || (BUILD_FOR_UDP && HANDSHAKE_WITH_ACK_AND_NAK && ack))
		{
		if (!vtg.xmit_in_progress) // Don't allow recursion on this routine
			{
			vtg.xmit_in_progress++;
			#if BUILD_FOR_UDP 
				u->ack=0;
				u->nak=0;
													
				while (try_ < 4) // Allow 3 trys
					{
					if(return_val=nr_plugs_send_to(u->plug, (void *)data, length, 0, u->remote_ip_address, u->remote_port)) goto done; //goto ext;
					if (chk_for_ack_or_nak((void *)u)) try_++; // Failed, try again 
					else break;
					} 

				if (try_ == 4) // Failed to send, we'll assume our remote is no longer there.
					{
					// For now we won't shut down the connection.  Failure to get a response
					// is not necessarily the same as shutting the connection down.  The
					// other guy may still be there but unable to talk for some reason.
					//shut_down_connection((void *)u); // Useful for both TCP and UDP.
					//printf("try_ == 4, no ACK!!!\n"); // For test

					// If control and if we have failed to communicate with Ethernet I/O
					// Then we need to kill the control until we can talk to the I/O again.


					return_val=2;
					goto done;
					}
					 
			#else
				return_val=nr_plugs_send(u->plug, (void *)data, length, 0); // TCP xmit
			#endif
			done: ;
			vtg.xmit_in_progress--;
			}
		else
			{
			return_val=999; // Xmit already in progress
			}
		}
	else // We're UDP and we're not handshaking, just spit out the packet, no need to worry about recursion.
		{
		return_val=nr_plugs_send_to(u->plug, (void *)data, length, 0, working_ip, working_port); 
		}

	ext: ;
	//if (return_val) printf("[RV %d]\n", return_val);
	//else printf("\n");
	return (return_val);
	}



// This function will return 0 if an ACK is received, 1 if NAK and 2 if timeout
int chk_for_ack_or_nak(void *context)
	{
	int quit_time, check_time, cur_time; 
	s_connection_status *u = (s_connection_status *)context;
	net_48 base_enet, cur_enet; // For test
	//int corrupted=0, ctr=0; // For test

	//get_baseline_enet_address(&base_enet); // For test
	quit_time = nr_timer_milliseconds() + UDP_WAIT_TIME;
	check_time = (quit_time-UDP_WAIT_TIME)+2;  // Initially 2mS
	while ((cur_time=nr_timer_milliseconds()) < quit_time)
		{
		//ctr++; // For test
		#if BUILD_FOR_POLLED_ETHERNET
			//nr_plugs_idle();  // For test
			//nr_plugs_idle_exclusive(u->plug);
			if(cur_time>=check_time)
				{
				nr_plugs_idle(); // We'll check for ACK/NAK every 2 mS
				check_time=cur_time+2;
				}
			
			// For test
			//nr_delay(50);
			//u->ack=0; // Clear both ACK and NAK
			//u->nak=0;
			//return(0);
		#endif
		//if (!corrupted)
		//	{
		//	get_current_enet_in_cache(&cur_enet);
		//	if((base_enet.u32!=cur_enet.u32) || (base_enet.l16!=cur_enet.l16))
		//		{
		//		printf("enet address corrupted waiting for ACK on loop #%d\n", ctr);
		//		corrupted=1;
		//		}
		//	}

		if (u->ack == 1) // Got an ACK!
			{
			// For test
			//cur_time = cur_time-(quit_time-UDP_WAIT_TIME);
			//printf("time until ack = %dmS\n", cur_time); 

			u->ack=0; // Clear both ACK and NAK
			u->nak=0;
			//if(corrupted) printf("Corrupted - Got an ACK on loop #%d\n", ctr); // For test
			//else printf("[%d]\n", ctr); // For test
			return(0);
			}
		else if (u->nak == 1) // Got a NAK!
			{
			u->nak=0; // Clear both ACK and NAK
			u->ack=0;
			return(1);
			} 
		}
		
	return (2); // Did not get an ACK or NAK
	}




// Used for both TCP and UDP
void shut_down_connection(void *context)
	{
	s_connection_status *u = (s_connection_status *)context;

	#if BUILD_FOR_TCP
		nr_plugs_listen(u->plug, tcp_listen_proc, (void *)u);
	#endif	
	u->connected=0;
	u->remote_ip_address=0;
	u->remote_port=0;
	u->send_op_packets=0;
	u->op_data_send_in_progress;
	u->send_o_scope_data=0;
	u->o_scope_send_in_progress;
	u->show_prompt=0;
	}


// Pull one character off the circular buffer
// of received udp packets/bytes, or 0 for
// 'nothing there'
int readBuffer_oneChar(s_connection_status *u)
{
  int result = -1;
  
  if(u->tail_c != u->head_c)
  {
    result = *(u->tail_c)++;
    if(u->tail_c == ((u->c) + RECV_BUFF_LENGTH))  // end of the world?
      u->tail_c = u->c;
  }

  return result;
}

// Read one "packet" from the circular queue,
// where a "packet" is stored as a 2 byte length (big endian),
// followed by that many bytes of characters.

int readBuffer(s_connection_status *u, char *buff)
{
  int count=0;
  int i;

  count = readBuffer_oneChar(u);
  if(count < 0) // nothing in queue?
    return 0;   // return "0 chars available"

  count = (count * 256) + readBuffer_oneChar(u);

  for(i = 0; i < count; i++)
    *buff++ = readBuffer_oneChar(u);
  
  return count;
}


// Stash one byte onto the circular queue. If
// head hits the tail, reset both pointers
// (rather drastic, but safe) and return -1

int loadBuffer_oneChar(s_connection_status *u, char c)
{
  *(u->head_c)++ = c;
  if(u->head_c == u->tail_c)
  {
    //u->head_c = u->tail_c = u->c; // reset head & tail
	//printf("$"); // For test
    return -1; // failure code, buffer overrun
  }

  if(u->head_c == ((u->c) + RECV_BUFF_LENGTH))
    u->head_c = u->c;

  return 0; // safe error code
  }


int loadBuffer(s_connection_status *u, char* payload, int payload_length)
{
  int ret_val=0;
  volatile unsigned char *orig_head_c;

  orig_head_c = u->head_c;  // Save our original starting ptr so we can return if an error

  // write two bytes of length of "packet" into circular queue.  If at any time we get
  // an error, we'll reset to ptr to the original setting.  This way we preserve commands that
  // were already sent while ignoring any garbage that may exis as a result of a failed attempt
  // to load the buffer

  if (ret_val=loadBuffer_oneChar(u,(payload_length >> 8) & 0xff) == 0) // Continue
	{
    if (ret_val=loadBuffer_oneChar(u,(payload_length) & 0xff) == 0) // Continue
		{
		// write the payload contents after length,
		// ready to abort at any time (result < 0)

		while(payload_length-- > 0)
			{
			if (ret_val=loadBuffer_oneChar(u,*payload++) < 0) break;
			}
		}
	}

  if (ret_val) u->head_c = orig_head_c; // Restore ptr to original
  return ret_val;
}



int plugs_init(void)
	{
  	s_connection_status *plant, *local, *io;
  	int result=0, ctr=1;
		

  	plant = &plant_net_settings;
  	local = &local_net_settings;
  	io 	= &io_net_settings;

	//printf("Here in plugs_init, u == %x\n", u); // For test

	// We'll send &g.pns.  If an Apex system, it'll be ignored by plugs.  If a Nios II, Cyclone II system
	// plugs should use this.  Need to comment out the section in plugs.h that links the persistant network settings
	// to flash and remake Altera's object modules.  Plugs will then use the passed value.  That's the way it should 
	// have been from the beginning.  Plugs should not be directly accessing flash.
  	result = nr_plugs_initialize(0, &g.pns.settings, __adapter__,
				__adapter_irq__,
				__adapter_struct_addr__);

	printf("Just initialized plugs\n"); // For test

	if (result != 0)
		{
		printf("Error initializing plugs, rval = %d\n", result);
		}

	#if BUILD_FOR_TCP
		result = nr_plugs_create((int *)&local->plug, ne_plugs_tcp, 20000, net_recv_proc, (void *)local, 0);
	#else //UDP 
		result = nr_plugs_create((int *)&local->plug, ne_plugs_udp, 20000, net_recv_proc, (void *)local, 0);
		nr_plugs_connect((int)local->plug, 0, -1, -1);
	#endif
  	if (result == 0)
		{
		printf("create plug %d error %d\n",local->plug, result);
		#if BUILD_FOR_TCP
    		result = nr_plugs_listen(local->plug, tcp_listen_proc, (void *)local);
		#endif
		}	
	else
		{
		printf("error creating plug %d\n", local->plug);
		}

	if (result == 0) // Keep going
		{
		#if BUILD_FOR_TCP
			result = nr_plugs_create((int *)&plant->plug, ne_plugs_tcp, 20002, net_recv_proc, (void *)plant, 0);
		#else // UDP
			result = nr_plugs_create((int *)&plant->plug, ne_plugs_udp, 20002, net_recv_proc, (void *)plant, 0);
			nr_plugs_connect((int)plant->plug, 0, -1, -1);
		#endif
  		if (result == 0)
			{
			printf("create plug %d error %d\n",plant->plug, result);
			#if BUILD_FOR_TCP
    			result = nr_plugs_listen(plant->plug, tcp_listen_proc, (void *)plant);
			#endif
			}
		else
			{
			printf("error creating plug %d\n", plant->plug);
			}
		}
  	
	if (result == 0) // Keep going
		{
		#if BUILD_FOR_TCP
			result = nr_plugs_create((int *)&io->plug, ne_plugs_tcp, 20004, net_recv_proc, (void *)io, 0);
		#else // UDP
			result = nr_plugs_create((int *)&io->plug, ne_plugs_udp, 20004, net_recv_proc, (void *)io, 0); 
			nr_plugs_connect((int)io->plug, 0, -1, -1);
		#endif
  		if (result == 0)
			{
			printf("create plug %d error %d\n",io->plug, result);
			#if BUILD_FOR_TCP
    			result = nr_plugs_listen(io->plug, tcp_listen_proc, (void *)io);
			#endif
			}
		else
			{
			printf("error creating plug %d error code = %d\n", io->plug, result);
			}
		}

	// Initialize plugs	
 	local->c[0] = 0;
	local->tail_c=local->c;
	local->head_c=local->c;
	local->connected=0;
	local->show_prompt=0;
	local->ack=0; // UDP only
	local->nak=0; //   "   "
	local->send_op_packets=0;
	local->send_o_scope_data=0;
	local->op_data_send_in_progress=0;
	local->o_scope_send_in_progress=0;
  	plant->c[0] = 0;
	plant->tail_c=plant->c;
	plant->head_c=plant->c;
	plant->connected=0;
	plant->show_prompt=0;
	plant->ack=0; // UDP only
	plant->nak=0; //   "   "
	plant->send_op_packets=0;
	plant->send_o_scope_data=0;
	plant->op_data_send_in_progress=0;
	plant->o_scope_send_in_progress=0;
  	io->c[0] = 0;  
	io->tail_c=io->c;
	io->head_c=io->c;
	io->connected=0;
	io->show_prompt=0;
	io->ack=0; // UDP only
	io->nak=0; //   "   "
	io->send_op_packets=0;
	io->send_o_scope_data=0;
	io->op_data_send_in_progress=0;
	io->o_scope_send_in_progress=0;
		
	return (result);
	}



// callback function
int net_recv_proc
	(
        int plug_handle,
        void *context,
        ns_plugs_packet *p,
        void *payload,
        int payload_length
        )
	{
	s_connection_status *u = (s_connection_status *)context;
	volatile char *working_payload = payload;
	int send_ack=1, send_nak=0, x;
				
	if (payload_length)
		{
		// First look at the first character in the payload, if ACK or NAK, we'll
		// set the flags here and not bother sending to the parser.  If the sender
		// has included any other commands or data in with this packet, it will be
		// lost.  ACKs and NAKs must be in their own packet.
		if (working_payload[0] == ACK) {u->ack=1; send_ack=0;} 
		else if (working_payload[0] == NAK) {u->nak=1;  send_ack=0;} 
		else
			{
			// For test
			//printf("About to load ");
			//for(x=0; x<payload_length; x++)
			//	{
			//	printf("[%d]", working_payload[x]);
			//	}
			//printf("\n");

			//printf("PL=%d, payload[0]=%d\n", payload_length, working_payload[0]); // For test

			// If an error loading the buffer, we'll send a NAK.  This could be the result
			// of a buffer overrun.
			if (loadBuffer(u, (char *)payload, payload_length)) {send_ack=0; send_nak=1;}
			}

		#if BUILD_FOR_UDP
			u->remote_ip_address = ((ns_plugs_ip_packet *)(p[ne_plugs_ip].header))->source_ip_address;
			u->remote_port = ((ns_plugs_udp_packet *)(p[ne_plugs_udp].header))->source_port;
			// Now save the remote's IP and port numbers so we can send him a wake up 
			// message when the board powers up.
			if (u == &local_net_settings) // Can't use whos_the_current_plug() here since g.spc has not yet been updated.
				{
				if ((u->remote_ip_address != (net_32)LAST_IP_CONNECTED_TO_LOCAL)
						||  (u->remote_port != LAST_PORT_CONNECTED_TO_LOCAL))  // Somebody new is talking to us, update his info in flash
					{
					//printf("Saving new remote settings, rev. on local plug\n"); // For test
					LAST_IP_CONNECTED_TO_LOCAL = u->remote_ip_address;
					LAST_PORT_CONNECTED_TO_LOCAL = u->remote_port;
					save_vars_to_flash();
					}
				}
			else if (u == &plant_net_settings) // Can't use whos_the_current_plug() here since g.spc has not yet been updated.
				{
				if ((u->remote_ip_address != (net_32)LAST_IP_CONNECTED_TO_PLANT)
						||  (u->remote_port != LAST_PORT_CONNECTED_TO_PLANT))
					{
					//printf("Saving new remote settings, rev. on plant plug\n"); // For test
					LAST_IP_CONNECTED_TO_PLANT = u->remote_ip_address;
					LAST_PORT_CONNECTED_TO_PLANT = u->remote_port;
					save_vars_to_flash();
					}
				}			

			if (u->connected != 1)
				{
				printf("Received an initial UDP packet from ");
				nr_plugs_print_ip_address(nr_h2n32(u->remote_ip_address));
				printf(" port %d\n", nr_n2h16(u->remote_port));
				//printf("\nunconverted ip = 0x%x, %d\n", u->remote_ip_address, u->remote_ip_address);
				//printf("converted ip = 0x%x, %d\n", nr_h2n32(u->remote_ip_address), nr_h2n32(u->remote_ip_address));
				//printf(" port %d\n",u->remote_port);
				u->connected = 1; // Since this is UDP, connected lets us know someone has made contact with this plug. We're not really "connected."
				}

			// Now send an ACK, we won't ack an ack or nak.  
			#if HANDSHAKE_WITH_ACK_AND_NAK
				if (send_ack) network_xmit((void *)u, (void *)ack_str, strlen(ack_str), NO_ACK, 0, 0);
				else if (send_nak) {network_xmit((void *)u, (void *)nak_str, strlen(nak_str), NO_ACK, 0, 0); /*printf("sending_nak_");*/}
			#endif
		#endif


		//printf("Received a packet from ");
		//nr_plugs_print_ip_address(nr_h2n32(u->host_ip_address));
		//printf(" port %d\n",u->host_port_number);
		}
    else // Close the connection and start listening again
		{
		// Send an ACK.  
		#if HANDSHAKE_WITH_ACK_AND_NAK
			network_xmit((void *)u, (void *)ack_str, strlen(ack_str), NO_ACK, 0, 0);
		#endif
		shut_down_connection((void *)u);
		printf("Received a null packet, shutting down connection on plug %d\n", u->plug);
		}	
   	}



// This function processes "packets" after the callback function above receives them.
int process_net_commands(s_connection_status *u, void *payload, int payload_length)
	{
	int i;
	char sendstr[350];
	volatile char *ptr;
			
	// In the future, need to determine what plug were dealing 
	// with here and act accordingly.  For now we'll treat them
	// all the same.	
	
	if(payload_length<349) // Less 1 for the null terminator
		{
		ptr = (char *)payload;
		for(i=0;i<payload_length;i++) sendstr[i] = ptr[i];
		sendstr[i]=0; // Write null terninator.
		printf("%s\n", sendstr); // For test

		if (u == &io_net_settings) 
			{
			//process_modbus_response(sendstr, payload_length);
			}
		else
			{
			g.spc = u; // Set the current context.
			parse_germs_command(sendstr);
			if (u->show_prompt) {SEND_PROMPT_ACK;}
			}
		}
	else
		{
		printf("Payload too large, cannot process!!!\n");
		}
		
	return 0;
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
	s_connection_status *u = context;

	u->connected = 1;
	u->remote_ip_address = remote_ip_address;
	u->remote_port = remote_port;
	
	printf("Accepted connection from ");
	nr_plugs_print_ip_address(nr_h2n32(remote_ip_address));
	printf(" port %d\n",remote_port);
	
	g.spc = u; // Set context for send_string()
	do_show_info();
	if (((s_connection_status *)g.spc)->show_prompt) SEND_PROMPT;
	
	return 0; // "no error" == accept the connection
	}



int whos_the_current_plug(void)
	{	
	// Who's current, local or plant-wide?
	if (((s_connection_status *)g.spc)->plug == local_net_settings.plug)
		return (LOCAL_IS_CURRENT);
	else if (((s_connection_status *)g.spc)->plug == plant_net_settings.plug)
		return (PLANT_WIDE_IS_CURRENT);
	else
		return (0); // Nobody's current? 
	}



int transmit_monitor_data(void *context)
	{
	int pos_data_length, time_data_length, comp_data_length, rval;
	char *data_ptr;
	//unsigned int saved_mask; // For test
	//np_pio *control_int = na_control_int; // For test


	vtg.xmitting_shot_data = 1;

	// Now wait if an op packet is being sent.
	while(local_net_settings.op_data_send_in_progress || plant_net_settings.op_data_send_in_progress);

	//saved_mask = control_int->np_piointerruptmask; // For test
	//control_int->np_piointerruptmask = 0;

	//////////////////////// Transmit position based data first /////////////////////////
	pos_data_length = sizeof(combined_sample) * (comp_shot_parameters.eos_pos+1); // Take into account location [0]
	data_ptr = (char *)&data_sample[0]; // + sizeof(combined_sample); // Data begins at location [1] not [0].

	rval=send_binary(context, POS_BASED_DATA, (void *)data_ptr, pos_data_length, ACK);
	//printf("rval=%d\n", rval);	// For test
	//////////////////// Then transmit time based data //////////////////////
	if (!rval) // Keep going
		{
		time_data_length = sizeof(combined_sample) * TIM_COLL_SAMPLES;
		//printf("time_data_len=%d, TIM_COLL_SAMPLES=%d\n", time_data_length, TIM_COLL_SAMPLES);
		if(((pos_data_length+time_data_length)/(int)sizeof(combined_sample))>=num_of_shot_data_samples) // Pos exceeded tot stk len, therefore we do not have the full 2000 time based data points.
			{
			time_data_length=(num_of_shot_data_samples-comp_shot_parameters.eos_pos)*sizeof(combined_sample);
			time_data_length-=sizeof(combined_sample); // Subtract 1 sample so we fit within the memory boundry.
			//printf("After correction, time_data_length=%d, num_of_shot_data_samples=%d and pos_data_length=%d\n", time_data_length, num_of_shot_data_samples, pos_data_length);
			}
		data_ptr = (char *)&data_sample[comp_shot_parameters.eos_pos+1];
		// For test
		//printf("About to send time data.\n");
		//printf("Context=%d, data_type=%d, dataptr=0x%x, time_data_len=%d\n", context, TIME_BASED_DATA, data_ptr, time_data_length);
		rval=send_binary(context, TIME_BASED_DATA, (void *)data_ptr, time_data_length, ACK);
		}
		
	///////////////////// Transmit computed shot parameters /////////////////////
	if (!rval) // Keep going
		{
		comp_data_length = sizeof(comp_shot_parameters);
		data_ptr = (char *)&comp_shot_parameters;
	
		send_binary(context, COMPUTED_SHOT_DATA, (void *)data_ptr, comp_data_length, ACK);
		}

	vtg.xmitting_shot_data = 0;
	
	return(rval);
	}




int r_reset_settings(int x)
	{
	ns_plugs_network_settings settings = {0,0,0,0,0,0,0};

	settings.ethernet_address.u32=0x11121314;
	settings.ethernet_address.l16=0x1516;
	settings.nameserver_ip_address = nm_ip2n(165,227,1,1); // just some nameserver
	settings.subnet_mask = nm_ip2n(255,255,255,0);
	settings.gateway_ip_address = nm_ip2n(10,0,0,255);

	settings.ip_address = nm_ip2n(10,0,0,51);
	g.pns.settings = settings;

	// Reset the remote I/O net settings
	remote_io_net_settings.ip_address = nm_ip2n(10,0,0,55);
	remote_io_net_settings.port = nr_h2n16(502); // Default to MODBUS
	remote_io_net_settings.type = 1;		  //   "      "     "
	return 0;
	}



// +----------------------------------------
// | r_get_settings_from_flash
// |
// | If there's flash memory, get the settings from
// | the conventional location in flash. If the
// | index looks crazy, or there's no flash, then
// | reset the settings to our default values.
// |

// Get network settings from flash
void r_get_settings_from_flash(void)
	{
	int i;
	
	vtw_flash_read_buffer((void *)NETWORK_SETTINGS, (void *)&g.pns, sizeof(g.pns));
	vtw_flash_read_buffer((void *)IO_NETWORK_SETTINGS, (void *)&remote_io_net_settings, sizeof(remote_io_net_settings));

	if(g.pns.signature != 0x00005afe) r_reset_settings(0);
	}

// Save network settings
int r_save_settings(int x)
	{
	#if nm_cpu_architecture == nios2
		r_epcs_protect_region(epcs_protect_none);
	#endif
	
	printf("Erasing network setup flash\n");
	vtw_flash_erase_sector((void *)-1,(void *)NETWORK_SETTINGS);
	
	g.pns.signature = 0x00005afe;

	printf("Writing network setup to flash\n");
	vtw_flash_write_buffer((void *)-1,(void *)NETWORK_SETTINGS,(void *)&g.pns,sizeof(g.pns));
	
	printf("Writing remote IO net settings to flash\n");
	vtw_flash_write_buffer((void *)-1,(void *)IO_NETWORK_SETTINGS,(void *)&remote_io_net_settings,sizeof(remote_io_net_settings));
	
	#if nm_cpu_architecture == nios2
		r_epcs_protect_region(epcs_protect_all);
	#endif
	}


void show_net_settings(void)
	{
	char tempstr[81], tempstr1[81];

	// Remember, send_string() will xmit onto the net or out the serial port depending
	// on g.spc.  If g.spc == 9999, the string will go out the serial port.
	
	sprintf(tempstr, "\n# Network Settings \n\n");
	send_string((void *)g.spc, tempstr);

	nr_plugs_ethernet_to_string(&g.pns.settings.ethernet_address, tempstr1);
	sprintf(tempstr, "# N1	ethernet address: %s\n", tempstr1);
	send_string((void *)g.spc, tempstr);

	nr_plugs_ip_to_string(g.pns.settings.ip_address, tempstr1);
	sprintf(tempstr, "# N2	ip address: %s\n", tempstr1);
	send_string((void *)g.spc, tempstr);

	nr_plugs_ip_to_string(g.pns.settings.nameserver_ip_address, tempstr1);
	sprintf(tempstr, "# N3	nameserver ip address: %s\n", tempstr1);
	send_string((void *)g.spc, tempstr);

	nr_plugs_ip_to_string(g.pns.settings.subnet_mask, tempstr1);
	sprintf(tempstr, "# N4	subnet mask: %s\n", tempstr1);
	send_string((void *)g.spc, tempstr);

	nr_plugs_ip_to_string(g.pns.settings.gateway_ip_address, tempstr1);
	sprintf(tempstr, "# N5	gateway ip address: %s\n", tempstr1);
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# N6	Reserved\n#\n");
	send_string((void *)g.spc, tempstr);

	nr_plugs_ip_to_string(remote_io_net_settings.ip_address, tempstr1);
	sprintf(tempstr, "# N7	Remote IO ip address: %s\n", tempstr1);
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# N8	Remote IO port: %d\n", nr_n2h16(remote_io_net_settings.port));
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# N9	Remote IO type: %d\n", remote_io_net_settings.type);
	send_string((void *)g.spc, tempstr);
	send_string((void *)g.spc, "#\t1 == MODBUS\n");

	sprintf(tempstr, "# NM	To view this menu\n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# NF	To flash network settings after making changes,\n");
	send_string((void *)g.spc, tempstr);
	sprintf(tempstr, "#	Please note!!! You must reset the board after flashing changes\n#\n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# To change, type \"N\" command, space and then the new setting,\n");
	send_string((void *)g.spc, tempstr);

	sprintf(tempstr, "# For example: N2 169.254.96.254<CR> To change the IP address\n\n");
	send_string((void *)g.spc, tempstr);
	}

///////////////////////////////////////////////////////////////////





////////////////////////// VTW functions /////////////////////////

int parse_variable_command(unsigned char *orig_cmd_str)
	{
	int a, strings, readback_err=0, set_err=0, no_of_strings;
	int var, var1, var2, value, value2;	
	unsigned char *ptr, *p_set, *p_range=0, *p_set_multiple=0, *p_operator=0;
	
	// First get rid of comment if present
	if ((ptr = (char *)strchr(orig_cmd_str, '#')) != NULL) *ptr = 0; 
	
	ptr = orig_cmd_str; ptr++;
	// Set pointers to point to the various delimiters.
	// Pointers will == NULL if delimiters not found.
	p_set = (char *)strchr(orig_cmd_str, '=');

	if(p_set) // We're setting a variable or range of variables
		{	
		if (p_set_multiple = (char *)strchr(orig_cmd_str, ',')) {} // We're setting successive variables with different values seperated by comas
		else  // We're either setting a single variable or a range of variables to the same value, look for a range indicator.
			{
			if (p_range = (char *)strchr(orig_cmd_str, '-')) // A minus sign was found in the string
				{
				if (p_range < p_set) {} // The minus sign is a range indicator, nothing more to do
				else p_range=NULL; // The minus sign is either a minus operator or indicating a negative value, need to parse further
				}
			}
		}
	else  // We're only reading variables
		{
		p_range = (char *)strchr(orig_cmd_str, '-'); // Minus sign here can only be a range indicator
		}

	//printf("p_set=0x%x, p_range=0x%x, p_operator=0x%x\n", p_set, p_range, p_operator); // For test



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
				sprintf(g.b,vtg.var_dec_hex == VAR_DEC ? "V%d_%d\n" : "V%d_%x\n",var,vtg.v[var]);	
				send_string((void *)g.spc, g.b);	
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
					// This big mess of a sprintf formats g.b for
					// decimal or hex.  The sequential values for 5
					// vars starting at x also formatted.  Each for loop
					// another 5 vars are formatted and sent.
					sprintf(g.b,vtg.var_dec_hex == VAR_DEC ? "V%d_%d_%d_%d_%d_%d\n" : "V%d_%x_%x_%x_%x_%x\n",
						var1+(5*strings),vtg.v[(var1+(5*strings))+0],vtg.v[(var1+(5*strings))+1],vtg.v[(var1+(5*strings))+2],
							vtg.v[(var1+(5*strings))+3],vtg.v[(var1+(5*strings))+4]);
					if(send_string((void *)g.spc, g.b)) break;		
					}
				}
			}
		if (readback_err)
			{
			sprintf(g.b, "?%s\n", orig_cmd_str);
			send_string((void *)g.spc, g.b);		
			}			
		} // readback
	else // Must be variable set
		{
		if (!p_set_multiple && !p_range) // Set a single variable
			{
			var=atoi(ptr); // Variable to update
			if (var<0 || var>MAX_VAR) {set_err=1; goto set_err;}
			
			// Get the first operand
			switch(p_set[1])
				{
				case 'h':
				case 'H':
					value = htoi(p_set+2); // Hex
					break;
				case 'v':
				case 'V':
					value = atoi(p_set+2);
					if (value<0 || value>MAX_VAR) {set_err=1; goto set_err;}
					else value = vtg.v[value];
					break;
				default:
					value = atoi(p_set+1); // Decimal
					break;
				}

			// Now look for an operator
			// We'll look for the first occurance of a sign
			for (a=2; p_set[a]!=(unsigned char)NULL; a++)
				{
				if (p_set[a] == '+') {p_operator=p_set+a; break;}
				else if (p_set[a] == '-') {p_operator=p_set+a; break;}
				else if (p_set[a] == '*') {p_operator=p_set+a; break;}
				else if (p_set[a] == '/') {p_operator=p_set+a; break;}
				else if (p_set[a] == '|') {p_operator=p_set+a; break;}
				else if (p_set[a] == '&') {p_operator=p_set+a; break;}
				}

			if (p_operator)
				{
				//printf("Operator! = %c \n", p_operator[0]);
				// Get the second operand
				switch(p_operator[1])
					{
					case 'h':
					case 'H':
						value2 = htoi(p_operator+2); // Hex
						break;
					case 'v':
					case 'V':
						value2 = atoi(p_operator+2);
						if (value2<0 || value2>MAX_VAR) {set_err=1; goto set_err;}
						else value2 = vtg.v[value2];
						break;
					default:
						value2 = atoi(p_operator+1); // Decimal
						break;
					}

				// Get operator and do the math
				switch(p_operator[0])
					{
					case '+':
						vtg.v[var] = value + value2;
						break;
					case '-':
						vtg.v[var] = value - value2;
						break;
					case '*':
						vtg.v[var] = value * value2;
						break;
					case '/':
						vtg.v[var] = value / value2;
						break;
					case '|':
						vtg.v[var] = value | value2;
						break;
					case '&':
						vtg.v[var] = value & value2;
						break;
					default:
						set_err=1;
						goto set_err;
						break;
					}
				}
			else  // No operator
				{
				//printf ("No operator!\n");
				vtg.v[var]=value;
				}
			//printf("$%d:%d\n", var, value); // For test

			#if BUILD_FOR_CONTROL
				// Check for specific variables that may have been changed to take action if needed
				switch(var)
					{
					case 314: // First check if any digital servo amp variables have been changed
					case 431:
					case 430:
					case 361:
					case 362:
					case 414:
					case 415:
						//update_servo_amp_interface();
						break;
					}
			#endif
			}
		else if (p_set_multiple && !p_range) // Write successive variables with different values beginning with the one specified
			{
			var = atoi(ptr); // Var number
			ptr=p_set;
			do
				{
				if (ptr[1] == 'h' || ptr[1] == 'H') value=htoi(ptr+2); // Move past the comma and 'h'
				else if (ptr[1] == 'v' || ptr[1] == 'V')
					{
					value=atoi(ptr+2); // Variable number
					if (value<0 || value>MAX_VAR) {set_err=1; goto set_err;}
					value=vtg.v[value];
					}
				else value=atoi(ptr+1); // Move past the comma	
				vtg.v[var]=value;

				#if BUILD_FOR_CONTROL
					// Check for specific variables that may have been changed to take action if needed
					switch(var)
						{
						case 314: // First check if any digital servo amp variables have been changed
						case 431:
						case 430:
						case 361:
						case 362:
						case 414:
						case 415:
							//update_servo_amp_interface();
							break;
						}
				#endif
				var++;
				ptr = (char *)strchr(ptr+1, ',');
				} while (ptr != NULL);
			}
		else if (p_range && !p_set_multiple) // Write a range of vars with the same value
			{
			// Together, var1 and var2 establish the range.
			var1 = atoi(ptr); var2 = atoi(p_range+1);
			if (var1<0 || var1>MAX_VAR || var2<0 || var2>MAX_VAR || var2<var1) {set_err=1; goto set_err;} // Invalid ranges	
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
				else if (p_set[1] == 'v' || p_set[1] == 'V')
					{
					value=atoi(p_set+2); // Variable number
					if (value<0 || value>MAX_VAR) {set_err=1; goto set_err;}
					value=vtg.v[value];
					}
				else value=atoi(p_set+1);
				
				#if BUILD_FOR_CONTROL
				for (a=0; a<(var2-var1); a++)
					{
					vtg.v[var1+a]=value;
					// Check for specific variables that may have been changed to take action if needed
					switch(var1+a)
						{
						case 314: // First check if any digital servo amp variables have been changed
						case 431:
						case 430:
						case 361:
						case 362:
						case 414:
						case 415:
							//update_servo_amp_interface();
							break;
						}
					}
				#endif
				}
			}
		else if (p_range && p_set_multiple) set_err=1; // Invalid 

set_err: ;
		if (set_err)
			{
			sprintf(g.b, "?%s\n", orig_cmd_str);
			send_string((void *)g.spc, g.b);		
			}
		else  // Update control parameters, a control parameter may have been changed.
			{
			#if BUILD_FOR_CONTROL
				//update_control_parameters();
			#endif
			}					
		}// Var set 
	}



parse_net_setup_command(unsigned char *orig_cmd_str)
	{
	unsigned char *ptr;
	volatile unsigned short int *halfword_ptr;

	// If source_of_command == 1, command came from network, if zero it came from serial port.
	ptr = orig_cmd_str; ptr++;

		// Adjust for case
	if(*ptr >= 'A' && *ptr <= 'Z')
		*ptr = *ptr + ('a' - 'A');

	switch(*ptr)
		{
		case '1': // Set the Ethernet address
			ptr+=2;
			nr_plugs_string_to_ethernet(ptr, &g.pns.settings.ethernet_address);
			break;
		
		case '2': // Set the IP address
			ptr+=2;
			nr_plugs_string_to_ip(ptr, &g.pns.settings.ip_address);
			break;

		case '3': // Set the nameserver IP address
			ptr+=2;
			nr_plugs_string_to_ip(ptr, &g.pns.settings.nameserver_ip_address);
			break;

		case '4': // Set the subnet mask
			ptr+=2;
			nr_plugs_string_to_ip(ptr, &g.pns.settings.subnet_mask);
			break;

		case '5': // Set the gateway IP address
			ptr+=2;
			nr_plugs_string_to_ip(ptr, &g.pns.settings.gateway_ip_address);
			break;

		case '6': // Reserved
			break;

		case '7': // Set the remote I/O IP address
			ptr+=2;
			nr_plugs_string_to_ip(ptr, &remote_io_net_settings.ip_address);
			break;

		case '8': // Set the remote I/O port number
			ptr+=2;
			remote_io_net_settings.port = nr_h2n16(atoi(ptr));
			break;

		case '9': // Set the remote I/O type
			ptr+=2;
			remote_io_net_settings.type = atoi(ptr);
			break;

		case 'm': // Show network setup menu
			show_net_settings(); // Send either to serial port or xmit on the network, depending on who asked
			break;	

		case 'f': // Flash network settings
			r_save_settings(0);
			break;

		default: // Invalid network setup command
			sprintf(g.b,"?%s\n",orig_cmd_str);
			send_string((void *)g.spc, g.b); 
			break;
		} 
	}



int parse_operational_command(unsigned char *orig_cmd_str)
	{

	}






void save_vars_to_flash(void)
	{
	int x;
	unsigned short temp_cprog[((MAX_STEP+1)*20)];

	// Unprotect flash if we're an EPCS
	#if nm_cpu_architecture == nios2
		r_epcs_protect_region(epcs_protect_none);
	#endif

	// First buffer the saved cprog before erasing flash sector
	vtw_flash_read_buffer((void *)CPROG_FLASH_ADDR, (void *)temp_cprog, ((MAX_STEP+1)*40)); // Size in bytes


	// Now erase flash sector
	vtw_flash_erase_sector(GM_FlashBase, (unsigned short int *)VAR_CPROG_FLASH_SECTOR_ADDR);
		
	// Now write variables to flash
	vtw_flash_write_buffer(GM_FlashBase, (unsigned short int *)VAR_FLASH_ADDR, 
								(unsigned short int *)&vtg.v, ((MAX_VAR+1)*2)); // Write halfwords, size is the number of halfwords
	
	// Then the buffered control program back to flash
	vtw_flash_write_buffer(GM_FlashBase, (unsigned short int *)CPROG_FLASH_ADDR, 
								temp_cprog, ((MAX_STEP+1)*20)); // Write halfwords

	// Reprotect flash
	#if nm_cpu_architecture == nios2
		r_epcs_protect_region(epcs_protect_all);
	#endif
	}




void reload_vars_from_flash(void)
	{
	//static char buf[(((MAX_VAR+1)*2)*2)];
	// ((MAX_VAR+1)*2) is the # of var halfwords saved in flash
	// (((MAX_VAR+1)*2)*2) is the # of var bytes saved in flash
	//printf("vtg.v=%d, &vtg.v=0x%x\n", vtg.v, &vtg.v);
	
	vtw_flash_read_buffer((void *)VAR_FLASH_ADDR, (void *)&vtg.v, (((MAX_VAR+1)*2)*2));
	//vtw_flash_read_buffer((void *)VAR_FLASH_ADDR, (void *)buf, (((MAX_VAR+1)*2)*2));
	//printf("buf address = 0x%x\n", buf);
	}
















//////////////////////////// CONTROL FUNCTIONS //////////////////////////////
#if BUILD_FOR_CONTROL

//#include "combined_control_blocks3.c" // Both polled and interrupt driven control are supported in the "combined" file.

// Functions common to both polled and interrupt driven control here

// Function to decode Control program commands
int parse_control_program_command(unsigned char *orig_cmd_str)
	{
	unsigned char *ptr;
	
	//printf("Parse control code\n"); // For test
	ptr = orig_cmd_str; ptr++;
	switch(*ptr)
		{
		case 'F':
		case 'f':  
			// Save control program to flash.  Vars will also be saved
			save_cprog_to_flash();
			break;
			
		case 'R':
		case 'r':
			// Reload control program from flash
			reload_cprog_from_flash();
			break;
				
		default: // Probably a line of control code to compile
			break;
				
		}
	}
	



void save_cprog_to_flash(void)
	{
	int x;
	//int y; // For test
	unsigned short tempv[((MAX_VAR+1)*2)];

	#if nm_cpu_architecture == nios2
		r_epcs_protect_region(epcs_protect_none);
	#endif

	// First buffer the saved variables before erasing the flash sector.
	// (((MAX_VAR+1)*2)*2)) is the number of bytes to read.
	vtw_flash_read_buffer((void *)VAR_FLASH_ADDR, (void *)&tempv, (((MAX_VAR+1)*2)*2));
	
	// Now erase flash sector
	vtw_flash_erase_sector(GM_FlashBase, (unsigned short int *)VAR_CPROG_FLASH_SECTOR_ADDR);
	
	
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
	vtw_flash_write_buffer(GM_FlashBase, (unsigned short int *)CPROG_FLASH_ADDR, 
								(unsigned short int *)vtg.control_program, ((MAX_STEP+1)*20));
	
	// Now write the buffered variables back to flash.  ((MAX_VAR+1)*2) is the number of halfwords to write.
	vtw_flash_write_buffer(GM_FlashBase, (unsigned short int *)VAR_FLASH_ADDR, 
								tempv, ((MAX_VAR+1)*2));

	#if nm_cpu_architecture == nios2
		r_epcs_protect_region(epcs_protect_all);
	#endif
	}


void reload_cprog_from_flash(void)
	{
	// (((MAX_STEP+1)*20)*2) is the # of cprog bytes saved in flash
	vtw_flash_read_buffer((void *)CPROG_FLASH_ADDR, (void *)&vtg.control_program, (((MAX_STEP+1)*20)*2));
	}
#endif



int main(void)
	{
	// All pointers declared as volatile per Altera.
	int result, x, y, char_position, c, just_connected, ctr=0;
	int blk_exe_return_val=0, idle_error, first_loop=1, shot_was_in_progress=0;
	unsigned int timer_load_value, cur_time, check_idle_time=0, mon_supervisor_time=0;
	unsigned int look_for_at_home_time=0, service_samp_time=0, test_time=0;
	unsigned int chk_ethernet_io_time=0;
	unsigned int chk_ana_io_reporting_time=0, prev_isw1_bits=0, cur_isw1_bits, isw1_bits_that_have_set;
	unsigned int isw1_bits_that_have_changed, isw4_bits_that_have_changed;
	unsigned int isw1_bits_that_have_cleared, isw4_bits_that_have_cleared;
	unsigned int prev_isw4_bits=0, cur_isw4_bits, isw4_bits_that_have_set;
	unsigned int print_again; // For test
	unsigned short int junk;
	volatile int dd, ee;
	np_pio *misc_out_pio = na_misc_outs;
	np_pio *misc_ins_pio = na_misc_ins;
	np_pio *axis1_interrupt = na_axis1_int;
	np_pio *seven_seg = na_seven_seg_pio;
	//np_pio *sel_hi_channels = na_sel_ana17_20;
	np_pio *opto_control = na_opto_control;
	np_pio *opto_data = na_opto_data;
	np_pio *ls_pio = na_ls_int_input;
	np_pio *bounceback_pio = na_bounceback_int;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	np_pio *button_pio = na_button_pio;
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
	int axis_num=1, payload_length, tempi; // Used for any interactions with PV1.
	unsigned short int tempus;
	volatile unsigned short int *halfword_ptr;
	char dac_ctr, osw_ctr, line[81];
	unsigned char payload[RECV_BUFF_LENGTH+100]; // Add in some extra for good measure
	//
	//net_48 enet_add;
	unsigned short io_bit=0;
	int on_off=TURN_ON;
	unsigned int io_word=0;
	s_connection_status * u;					// For test
	unsigned int start_time, end_time;	// For test
	unsigned short int tempu;			// For test
	int err_ctr;						// For test
	unsigned int report_time;			// For test
	//net_48 base_enet, cur_enet;			// For test
		
	//g.sp = tcp_send_byte;
	g.spc = &local_net_settings; // no context
	//g.rp = tcp_recv_byte;
	g.rpc = &local_net_settings; // no context
	//g.ip = plugs_init;
	g.ipc = &local_net_settings;
	//g.t = tcp_check;
	g.tc = &local_net_settings;

		
	#if nm_cpu_architecture == nios2
		// EPCS flash sector erase
		#if PROTECT_EPCS_AT_POWER_UP
			if (r_epcs_protect_region(epcs_protect_all)) printf("Unable to protect EPCS!!!\n");
		#else
			if (r_epcs_protect_region(epcs_protect_none)) printf("Unable to unprotect EPCS!!!\n");
		#endif
	#endif


printf("&vtg.v = 0x%x\n", &vtg.v);

reload_vars_from_flash(); 
reload_cprog_from_flash();
	
r_get_settings_from_flash();	
// Initialize TCP.
plugs_init();


vtg.misc_outs = 0;
misc_out_pio->np_piodata = vtg.misc_outs;

vtg.var_dec_hex = VAR_DEC;
comp_shot_parameters.cycle_time = 0;
vtg.first_cs_after_power_up = 1;
vtg.xmit_in_progress = 0;
vtg.tst_ctr=0; // for test
vtg.dac_write_in_progress = 0;
vtg.xmitting_shot_data = 0;
vtg.serial_show_prompt = 0;
UDP_WAIT_TIME = 1000; // For test, later set through control program
	

	
////////// Enter serial/test/setup mode here if SW7 pressed at power up/reset ///////////

//if(SW7)
if(ENTER_SERIAL_MODE) // Now use dip switch SW5-4
	{
	g.spc=(void *)9999; // Set to serial mode
	//nr_pio_showhex(0x55); // Output "SS", well "55" to indicate serial mode on LED display
	seven_seg->np_piodata=~0x1d1d;
	printf("\nSerial/Test/Setup mode!!!\n");
	while(1)
		{
		if(gets(line)) // gets() is a blocking function, we're stuck here until a string is entered.
			{
			parse_germs_command(line);
			if(vtg.serial_show_prompt) SEND_PROMPT1;
			}
		}
	}


/////////////////////////////////////////////////////////////////////////////////////////

// Times in mS, Items to be serviced at regular non-precision intervals
#define CHECK_IDLE_TIME 2
#define MON_SUPERVISOR_TIME 5
#define CHK_ANA_IO_REPORTING_TIME 100
#define CHK_ETHERNET_IO_TIME 1000
#define LOOK_FOR_AT_HOME_TIME 500
#define SERVICE_SAMP_TIME 100
#define CHECK_FOR_CLOSED_VAL_TIME 10
#define TEST_TIME 2000

while (1)
	{
	// Get current time
	cur_time=nr_timer_milliseconds();
	if(first_loop) // Set up the various times to check stuff
		{
		first_loop=0;
		check_idle_time=cur_time+CHECK_IDLE_TIME;
		mon_supervisor_time=cur_time+MON_SUPERVISOR_TIME;
		chk_ana_io_reporting_time=cur_time+CHK_ANA_IO_REPORTING_TIME;
		chk_ethernet_io_time=cur_time+CHK_ETHERNET_IO_TIME;
		look_for_at_home_time=cur_time+LOOK_FOR_AT_HOME_TIME;
		service_samp_time=cur_time+SERVICE_SAMP_TIME;
		test_time=cur_time+TEST_TIME;
		}

////////// Periodic non-precision events based on mS counter cur_time can be placed here
	
	#if BUILD_FOR_POLLED_ETHERNET
		if(cur_time>=check_idle_time)
			{
			check_idle_time=cur_time+CHECK_IDLE_TIME;
			nr_plugs_idle();
			}
	#endif

	
	// Use this for any tests that require periodic intervals
	//if(cur_time>=test_time)
	//	{
	//	test_time=cur_time+TEST_TIME;

	//	ctr++;
	//	if(ctr&0x00000001) out_dac(1, 4000, VOLTAGE_X_1000, ABSOLUTE);
	//	else out_dac(1, 0, VOLTAGE_X_1000, ABSOLUTE);
	//	}

	
	
///////////////////////////////////////////////////////////////////
	
	// Process any packets that may be present
	if(payload_length=readBuffer(&local_net_settings, payload))
		process_net_commands(&local_net_settings, payload, payload_length);
	if(payload_length=readBuffer(&plant_net_settings, payload))
		process_net_commands(&plant_net_settings, payload, payload_length);
		

	if(payload_length=readBuffer(&io_net_settings, payload))
		{/*printf("!\n");*/ process_net_commands(&io_net_settings, payload, payload_length);}
	} // while(1)		
}








