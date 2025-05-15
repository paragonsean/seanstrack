//     emc_5_07a.c 

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

  12-5-06 emc_5_07.c
		Added position control

*/

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
#include <float.h>
#include "nios.h"
#include "plugs.h"
//#include "ethernet_mon_cont_peripherals1.h"
#include "vtw_global10.h"
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
#define FIRMWARE_VERSION "emc_5_06,"

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

	a, nios2-build -b 0x400000 emc_5_?.c<CR>
	b, nios2-srec2flash  emc_5_?.srec -flash_address=0x140000 -ram_address=0x400000 -copy_size=0x40000<CR>
	c, nios2-run emc_5_?.flash<CR>  To download the flash file.

2, This file is built using:

	a, Nios V3.2 
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


// Flash addresses for saving vtw items
#if nm_cpu_architecture == nios2
	// EPCS flash addresses, we'll allow for some extra room in case we'd like to increase the vars or cprog size
	#define VAR_CPROG_FLASH_SECTOR_ADDR 0x1F0000 // EPCS16 Sector 31
	#define VAR_FLASH_ADDR 0x1F0000
	#define CPROG_FLASH_ADDR 0x1F2000 // to 0x1F4000
	#define NETWORK_SETTINGS ((ns_plugs_persistent_network_settings *)(0x1E0000))
	#define IO_NETWORK_SETTINGS ((enet_io_net_settings *)(0x1E0100))
	#define AD_PROG_FLASH_ADDR  ((unsigned char *)0x1D0000)
#else
	#define VAR_CPROG_FLASH_SECTOR_ADDR 0x108000
	#define VAR_FLASH_ADDR 0x108000
	#define CPROG_FLASH_ADDR 0x109000
	#define NETWORK_SETTINGS ((ns_plugs_persistent_network_settings *)(nasys_main_flash + 0x6000))
	#define IO_NETWORK_SETTINGS ((enet_io_net_settings *)(nasys_main_flash + 0x6100))
	#define AD_PROG_FLASH_ADDR  ((unsigned char *)(nasys_main_flash + 0x4100))
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
	unsigned char *head_c;
	unsigned char *tail_c;
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
//int convert_monitored_channels(int sample_ctr);

//#include "tb3.c" // Return to old way of analog conversions with floating channels after
				// hardware change to external interface board (rev. c).

#if nm_cpu_architecture == nios2 // Could be a different tb file based on the board we're compiling for
	#include "tb6.c"
#else
	#include "tb6.c"
#endif

// Load my private epcs interface routines.  These do not use any of Altera's obj's.
#if nm_cpu_architecture == nios2 // Cyclone II version of board with EPCS flash
	#include "my_epcs.c" 
#endif

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
int vtw_flash_erase_sector(unsigned short *flash_base, unsigned short *sector_address);
int vtw_flash_erase(unsigned short *flash_base);
int vtw_flash_write(unsigned short *flash_base, unsigned short *addr, unsigned short val);
int vtw_flash_write_buffer(unsigned short *flash_base, unsigned short *start_address, unsigned short *buffer, int size);
int vtw_flash_read_buffer(unsigned char *start_address, unsigned char *buffer, int size);

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
		rval = r_epcs_write_buffer ((unsigned long)addr, 2, (void *)&val); // Write 2 bytes
	#else
		rval=nr_flash_write((void *)flash_base, (void *)addr, val); // val is a 16 bit halfword (2 bytes)
	#endif

	return (rval);
	}


// Again, the Apex version wrote halfwords.  We'll mimic that functionality here in EPCS mode as well.
int vtw_flash_write_buffer(unsigned short *flash_base, unsigned short *start_address, unsigned short *buffer, int size)
	{
	int rval, s;
	
	#if nm_cpu_architecture == nios2
		// EPCS flash buffer write function
		rval=r_epcs_write_buffer ((unsigned long)start_address, size*2, (void *)buffer); 
	#else
		rval=nr_flash_write_buffer((void *)flash_base, (void *)start_address, (void *)buffer, size); 
	#endif
	
	return (rval);
	}


// We don't have to come here to read the AMD flash on the Apex board, it can be read directly as
// a memory mapped peripheral.  However, to maintain compatibility with our common flash functions, we'll
// call this function for all flash reads, even from AMD flash on the Apex board.
// This function must be used to read EPCS flash.  Since it is a serial flash, it cannot be directly read.
int vtw_flash_read_buffer(unsigned char *start_address, unsigned char *buffer, int size)
	{
	int rval=0, x;
	
	#if nm_cpu_architecture == nios2
		// EPCS flash read function, we have to call an Altera function to read EPCS flash since it's a serial device.
		//printf("start_address=0x%x, size = %d \n", start_address, size);
		rval=r_epcs_read_buffer ((unsigned long)start_address, size, (void *)buffer);
	#else
		// Since AMD flash is directly addressed, we'll just fetch the data here and fill the buffer.
		for (x=0; x<size/2; x++) // We'll work with half words here
			{
			((unsigned short *)buffer)[x] = ((unsigned short *)start_address)[x];
			}
	#endif
	
	return (rval);
	}


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

#ifdef GM_FlashBase
	#if nm_cpu_architecture == nios2
		result = addr < (ui)GM_FlashTop; // To silence a compilier warning 
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
			vtw_flash_write(GM_FlashBase,(us *)(addr & ~1), halfword);
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
	unsigned short buf;

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
	sprintf(g.b,"# executing at %08x\n",addr);
	send_string((void *)g.spc,g.b);
	//r_asm_go(addr);
	r_goto(addr);

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
		vtw_flash_erase_sector((us *)GM_FlashBase,(us *)addr);
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
				//r_asm_go(record_address);
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
	#if nm_cpu_architecture == nios2
		send_string((void *)g.spc,"#Welcome to FasTrak II, Cyclone II edition\n");
	#else
		send_string((void *)g.spc,"#Welcome to FasTrak II, Apex20KE edition\n");
	#endif
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
      u->tail_c = (char *)u->c;
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
    u->head_c = (char *)u->c;

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

  if (ret_val) u->head_c = (char *)orig_head_c; // Restore ptr to original
  return ret_val;
}



int plugs_init(void)
	{
  	s_connection_status *plant, *local, *io;
  	int result=0, ctr=1, tmp;
		

  	plant = &plant_net_settings;
  	local = &local_net_settings;
  	io 	= &io_net_settings;

	//printf("Here in plugs_init, u == %x\n", u); // For test

  	//result = nr_plugs_initialize(0, 0, __adapter__,
	//			__adapter_irq__,
	//			__adapter_struct_addr__);

	result = nr_plugs_initialize(0, &g.pns.settings, __adapter__,
				__adapter_irq__,
				__adapter_struct_addr__);

	printf("Just initialized plugs\n"); // For test

	if (result != 0)
		{
		printf("Error initializing plugs\n");
		}

	#if BUILD_FOR_TCP
		result = nr_plugs_create((int *)&local->plug, ne_plugs_tcp, 20000, net_recv_proc, (void *)local, 0);
	#else //UDP 
		//result = nr_plugs_create((int *)&local->plug, ne_plugs_udp, 20000, net_recv_proc, (void *)local, 0);
		//nr_plugs_connect((int)local->plug, 0, -1, -1);

		//result = nr_plugs_create((int *)&local_net_settings.plug, ne_plugs_udp, 20000, net_recv_proc, (void *)&local_net_settings, 0);
		//nr_plugs_connect((int)local_net_settings.plug, 0, -1, -1);

		result = nr_plugs_create((int *)&tmp, ne_plugs_udp, 20000, net_recv_proc, (void *)local, 0);
		local->plug = tmp;
		nr_plugs_connect((int)local->plug, 0, -1, -1);
	#endif
  	if (result == 0)
		{
		printf("create plug %d error %d\n",local->plug, result);
		//printf("create plug %d error %d\n",local_net_settings.plug, result);
		#if BUILD_FOR_TCP
    		result = nr_plugs_listen(local->plug, tcp_listen_proc, (void *)local);
		#endif
		}	
	else
		{
		printf("error creating plug %d\n", local->plug);
		//printf("error creating plug %d\n", local_net_settings.plug);
		}

	if (result == 0) // Keep going
		{
		#if BUILD_FOR_TCP
			result = nr_plugs_create((int *)&plant->plug, ne_plugs_tcp, 20002, net_recv_proc, (void *)plant, 0);
		#else // UDP
			//result = nr_plugs_create((int *)&plant->plug, ne_plugs_udp, 20002, net_recv_proc, (void *)plant, 0);
			//nr_plugs_connect((int)plant->plug, 0, -1, -1);

			//result = nr_plugs_create((int *)&plant_net_settings.plug, ne_plugs_udp, 20000, net_recv_proc, (void *)&plant_net_settings, 0);
			//nr_plugs_connect((int)plant_net_settings.plug, 0, -1, -1);

			result = nr_plugs_create((int *)&tmp, ne_plugs_udp, 20002, net_recv_proc, (void *)plant, 0);
			plant->plug = tmp;
			nr_plugs_connect((int)plant->plug, 0, -1, -1);
		#endif
  		if (result == 0)
			{
			printf("create plug %d error %d\n",plant->plug, result);
			//printf("create plug %d error %d\n",plant_net_settings.plug, result);
			#if BUILD_FOR_TCP
    			result = nr_plugs_listen(plant->plug, tcp_listen_proc, (void *)plant);
			#endif
			}
		else
			{
			printf("error creating plug %d\n", plant->plug);
			//printf("error creating plug %d\n", plant_net_settings.plug);
			}
		}
  	
	if (result == 0) // Keep going
		{
		#if BUILD_FOR_TCP
			result = nr_plugs_create((int *)&io->plug, ne_plugs_tcp, 20004, net_recv_proc, (void *)io, 0);
		#else // UDP
			//result = nr_plugs_create((int *)&io->plug, ne_plugs_udp, 20004, net_recv_proc, (void *)io, 0); 
			//nr_plugs_connect((int)io->plug, 0, -1, -1);

			//result = nr_plugs_create((int *)&io_net_settings.plug, ne_plugs_udp, 20000, net_recv_proc, (void *)&io_net_settings, 0);
			//nr_plugs_connect((int)io_net_settings.plug, 0, -1, -1);

			result = nr_plugs_create((int *)&tmp, ne_plugs_udp, 20004, net_recv_proc, (void *)io, 0);
			io->plug = tmp;
			nr_plugs_connect((int)io->plug, 0, -1, -1);
		#endif
  		if (result == 0)
			{
			printf("create plug %d error %d\n",io->plug, result);
			//printf("create plug %d error %d\n",io_net_settings.plug, result);
			#if BUILD_FOR_TCP
    			result = nr_plugs_listen(io->plug, tcp_listen_proc, (void *)io);
			#endif
			}
		else
			{
			printf("error creating plug %d\n", io->plug);
			//printf("error creating plug %d\n", io_net_settings.plug);
			}
		}

//#if 0
	// Initialize plugs	
 	local->c[0] = 0;
	local->tail_c=(char *)local->c;
	local->head_c=(char *)local->c;
	local->connected=0;
	local->show_prompt=0;
	local->ack=0; // UDP only
	local->nak=0; //   "   "
	local->send_op_packets=0;
	local->send_o_scope_data=0;
	local->op_data_send_in_progress=0;
	local->o_scope_send_in_progress=0;
  	plant->c[0] = 0;
	plant->tail_c=(char *)plant->c;
	plant->head_c=(char *)plant->c;
	plant->connected=0;
	plant->show_prompt=0;
	plant->ack=0; // UDP only
	plant->nak=0; //   "   "
	plant->send_op_packets=0;
	plant->send_o_scope_data=0;
	plant->op_data_send_in_progress=0;
	plant->o_scope_send_in_progress=0;
  	io->c[0] = 0;  
	io->tail_c=(char *)io->c;
	io->head_c=(char *)io->c;
	io->connected=0;
	io->show_prompt=0;
	io->ack=0; // UDP only
	io->nak=0; //   "   "
	io->send_op_packets=0;
	io->send_o_scope_data=0;
	io->op_data_send_in_progress=0;
	io->o_scope_send_in_progress=0;
//#endif

#if 0	
	// Initialize plugs	
 	local_net_settings.c[0] = 0;
	local_net_settings.tail_c=local_net_settings.c;
	local_net_settings.head_c=local_net_settings.c;
	local_net_settings.connected=0;
	local_net_settings.show_prompt=0;
	local_net_settings.ack=0; // UDP only
	local_net_settings.nak=0; //   "   "
	local_net_settings.send_op_packets=0;
	local_net_settings.send_o_scope_data=0;
	local_net_settings.op_data_send_in_progress=0;
	local_net_settings.o_scope_send_in_progress=0;
  	plant_net_settings.c[0] = 0;
	plant_net_settings.tail_c=plant_net_settings.c;
	plant_net_settings.head_c=plant_net_settings.c;
	plant_net_settings.connected=0;
	plant_net_settings.show_prompt=0;
	plant_net_settings.ack=0; // UDP only
	plant_net_settings.nak=0; //   "   "
	plant_net_settings.send_op_packets=0;
	plant_net_settings.send_o_scope_data=0;
	plant_net_settings.op_data_send_in_progress=0;
	plant_net_settings.o_scope_send_in_progress=0;
  	io_net_settings.c[0] = 0;  
	io_net_settings.tail_c=io_net_settings.c;
	io_net_settings.head_c=io_net_settings.c;
	io_net_settings.connected=0;
	io_net_settings.show_prompt=0;
	io_net_settings.ack=0; // UDP only
	io_net_settings.nak=0; //   "   "
	io_net_settings.send_op_packets=0;
	io_net_settings.send_o_scope_data=0;
	io_net_settings.op_data_send_in_progress=0;
	io_net_settings.o_scope_send_in_progress=0;
#endif

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
			else printf("%d RECVD %s", payload_length, payload); // For test
			//	{
			//	printf("RECVD "); // For test
			//	for (x=0; x<payload_length; x++) {printf("%c", (char *)payload[x]);}
			//	printf("\n");
			//	}
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
		//printf("%s\n", sendstr); // For test
		printf("%d PROC %s", payload_length, sendstr); // For test

		if (u == &io_net_settings) 
			{
			process_modbus_response(sendstr, payload_length);
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
	printf("Erasing network setup flash\n");
	vtw_flash_erase_sector((void *)-1,(void *)nasys_plugs_persistent_network_settings);

	g.pns.signature = 0x00005afe;

	printf("Writing network setup to flash\n");
	vtw_flash_write_buffer((void *)-1,(void *)nasys_plugs_persistent_network_settings,(void *)&g.pns,sizeof(g.pns));
	printf("Writing remote IO net settings to flash\n");
	vtw_flash_write_buffer((void *)-1,(void *)PTR_TO_REMOTE_IO_NET_SETTINGS_IN_FLASH,(void *)&remote_io_net_settings,sizeof(remote_io_net_settings));
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
						update_servo_amp_interface();
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
							update_servo_amp_interface();
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
							update_servo_amp_interface();
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
				update_control_parameters();
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
	np_pio *misc_ins_pio = na_misc_ins;	
	//np_timer *o_scope_timer = na_o_scope_timer;
	unsigned char *ptr, tempstr[15], dig_pot_val;
	int ch_num, val, return_val=0, incremental=0, axis_num=1, x, result_type, tempi=0;
	//unsigned short int ana_result;
	unsigned int ana_result;
	unsigned int uval, command;
	//unsigned int period;
	volatile unsigned short int all_channels[24]; // All analog channels
	//volatile unsigned short int *halfword_ptr;
	
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
			command = atoi(ptr);
			
			if (command==1) // Command to set / clear O-scope mode
				{
				ptr += 2;
				val = atoi(ptr);

				vtg.been_below_trig_voltage=0;  // Clear this commin or goin
				((s_connection_status *)g.spc)->send_o_scope_data=val; // Zero to disable, anything else to enable.

				if (val != 0)  // Someone commanded o_scope mode, start timer.
					{
					start_o_scope_timer(O_SCOPE_MODE);
					}
				else	// Someone turned off o-scope mode, at least for one plug
					{
					// If both local and plant want scope data, deallocate memory.
					if(!local_net_settings.send_o_scope_data && !plant_net_settings.send_o_scope_data) 
						{
						stop_o_scope_timer(FREE);
						}
					}
				}
			
			// Allow op data packets in monitor only mode also
			//if (*ptr=='3') // Command to enable operational data packets
			if (command==3) // Command to enable operational data packets
				{
				ptr += 2;
				// Next WD reset timer interrupt, op data packet will be sent according to
				// val.  0==Don't send any, 1==Send one, 2==Keep sending.  One or both plugs
				// can be receiving data simultaneously.
				if (((int)val=atoi(ptr)) > 2) val=0; // Don't send anything.

				// Update analog mode, user may have changed it before going into this mode.
				set_analog_mode(ANALOG_MODE);

				((s_connection_status *)g.spc)->send_op_packets=val; 
				}

			#if BUILD_FOR_CONTROL
			//if (*ptr=='4') // Initialize/update axes
			if (command==4) // Initialize/update axes
				{
				ptr += 2;
				if (atoi(ptr)==1)
					initialize_axes();
				else if (atoi(ptr)==2)
					update_control_parameters();
				else
					{
					sprintf(g.b,"R_ERROR:2 #Syntax error, bad command number\n");	
					send_string((void *)g.spc, g.b);
					}
				}
			//else if (*ptr=='5') // Check servo null
			else if (command==5) // Check servo null
				{
				ptr += 2; // Point to axis number
				null_valve(atoi(ptr), CHECK_NULL);
				}
			//else if (*ptr=='6') // Auto null servo
			else if (command==6) // Auto null servo
				{
				ptr += 2; // Now point to axis number
				null_valve(atoi(ptr), AUTO_NULL);
				}
			//else if (*ptr=='7') // Set servo amp1 LVDT phase setting
			else if (command==7) // Set servo amp1 LVDT phase setting
				{
				ptr += 2; // Now point to the phase setting, should be between 0 and 255
				tempi = atoi(ptr);
				if(tempi < 0) tempi = 0;
				else if(tempi > 255) tempi = 255;
				SERVO_AMP_DEMOD_PHASE_SETTINGS &= 0xffffff00;
				SERVO_AMP_DEMOD_PHASE_SETTINGS |= tempi;
				servo_amp_interface(LOAD_DEMOD_PHASE_SETTINGS);
				save_vars_to_flash(); 
				}
			//else if (*ptr=='8') // Set servo amp2 LVDT phase setting
			else if (command==8) // Set servo amp2 LVDT phase setting
				{
				ptr += 2; // Now point to the phase setting, should be between 0 and 255
				tempi = atoi(ptr);
				if(tempi<0) tempi=0;
				else if(tempi>255) tempi=255;
				tempi <<= 8;
				SERVO_AMP_DEMOD_PHASE_SETTINGS &= 0xffff00ff;
				SERVO_AMP_DEMOD_PHASE_SETTINGS |= tempi;
				servo_amp_interface(LOAD_DEMOD_PHASE_SETTINGS);
				save_vars_to_flash();
				}
			//else if (*ptr=='9') // Set servo amp LVDT osc phase setting.  Not used for anything at this time
			else if (command==9) // Set servo amp LVDT osc phase setting.  Not used for anything at this time
				{
				ptr += 2; // Now point to the phase setting, should be between 0 and 255
				tempi = atoi(ptr);
				if(tempi<0) tempi=0;
				else if(tempi>255) tempi=255;
				tempi <<= 16;
				SERVO_AMP_DEMOD_PHASE_SETTINGS &= 0xff00ffff;
				SERVO_AMP_DEMOD_PHASE_SETTINGS |= tempi;
				servo_amp_interface(LOAD_DEMOD_PHASE_SETTINGS);
				save_vars_to_flash();
				}
			//else if (*ptr=='10') // Open/close valve loop on amp1
			else if (command==10) // Open/close valve loop on amp1
				{
				ptr += 3; // Should be 0 or 1
				tempi = atoi(ptr);

				if(tempi) servo_amp_interface(OPEN_VAL_LOOP1);
				else servo_amp_interface(CLOSE_VAL_LOOP1);
				}
			//else if (*ptr=='11') // Open/close valve loop on amp2
			else if (command==11) // Open/close valve loop on amp2
				{
				ptr += 3; // Should be 0 or 1
				tempi = atoi(ptr);

				if(tempi) servo_amp_interface(OPEN_VAL_LOOP2);
				else servo_amp_interface(CLOSE_VAL_LOOP2);
				}
			//else if (*ptr=='12') // Open/close pressure feedback on amp2
			else if (command==12) // Open/close pressure feedback on amp2
				{
				ptr += 3; // Should be 0 or 1
				tempi = atoi(ptr);

				if(tempi) servo_amp_interface(OPEN_PRESSURE_FEEDBACK2);
				else servo_amp_interface(CLOSE_PRESSURE_FEEDBACK2);
				}

			#endif // BUILD_FOR_CONTROL
			break;

		#if BUILD_FOR_CONTROL
		case 'r': // Read back control fatal error or warning words.
			ptr++;
			if (*ptr=='f' || *ptr=='F')
				{
				sprintf(g.b,"R_FATAL_ERROR:21 #Fatal_Error=%d\n", FATAL_ERROR_WORD);	
				}
			else if (*ptr=='w' || *ptr=='W')
				{
				sprintf(g.b,"R_WARNING:22 #Warning=%d\n", vtg.saved_warning);		
				}
			else
				{
				sprintf(g.b,"R_ERROR:2 #Syntax error\n");	
				}
			send_string((void *)g.spc, g.b);		
			break;	
		#endif // BUILD_FOR_CONTROL

		case 's': // Read back current position
			ptr++;
			axis_num=atoi(ptr);
			if (axis_num > 2 || axis_num < 1)
				sprintf(g.b,"Error :%d #invalid axis specification", 3); // invalid	axis number
			else
				sprintf(g.b,"R_POS%d:11 #%d\n",axis_num, RD_PV_POS);

			send_string((void *)g.spc, g.b);
			break;
		case 'f': // Read back current velocity
			ptr++;
			axis_num=atoi(ptr);
			if (axis_num > 2 || axis_num < 1)
				sprintf(g.b,"Error :%d #invalid axis specification", 3); // invalid	axis number.
			else
				sprintf(g.b,"R_VEL%d:15 #%d\n",axis_num, (axis[axis_num].vel_ctr_freq/RD_PV_VEL)); 

			send_string((void *)g.spc, g.b);
			break;
		case 'a': // convert analog channels
			ptr++;
			if (*ptr == 'A' || *ptr == 'a') // Convert all channels
				{
				ptr++;
				if (*ptr == 'V' || *ptr == 'v') result_type = VOLTAGE_X_1000;  // Convert all channels returning volts * 1000
				else result_type = RAW_VAL;

				convert_all_channels(all_channels, result_type);
				for (ch_num=0; ch_num<24; ch_num++)
					{
					if(result_type == RAW_VAL)
						{
						sprintf(g.b,"O_CH%d=%x\n", ch_num+1, all_channels[ch_num]);
						send_string((void *)g.spc, g.b);
						}
					else
						{
						sprintf(g.b,"O_CH%d=%d\n", ch_num+1, (short int)all_channels[ch_num]);
						send_string((void *)g.spc, g.b);
						}
					}
				}
			else
				{
				ch_num = atoi(ptr);
				if (ch_num < 1 || ch_num > 24)
					{
					sprintf(g.b,"Error :%d #invalid channel number\n", 4); // invalid channel number.
					send_string((void *)g.spc, g.b);
					}
				else
					{
					if (convert_ana_ch(ch_num, RAW_VAL, &ana_result))
						{
						sprintf(g.b,"Error :%d #unable to convert analog channel(s)\n", 5); // invalid channel number.
						send_string((void *)g.spc, g.b);
						}
					else  // Everything's cool, transmit the data.
						{
						sprintf(g.b,"O_CH%d=%x\n", ch_num, ana_result);
						send_string((void *)g.spc, g.b);
						}
					}
				}
			break;
		case 'd': // Output to DAC channels / or set digital pot gains
			ptr++;
			switch(*ptr)
				{
				#if BUILD_FOR_CONTROL
				case 'p': // Set digital pot gains
				case 'P':
					ptr++;
					tempi = atoi(ptr); // Get pot to set
					ptr+=2;
					dig_pot_val = (unsigned char)atoi(ptr);
					if (tempi<1) tempi = 1;
					if (tempi>4) tempi = 4;
					if (tempi == 1)
						{
						DIGITAL_POTS_SETTINGS &= 0x00ffffff;
						DIGITAL_POTS_SETTINGS |= ((int)dig_pot_val << 24);
						}
					else if (tempi == 2)
						{
						DIGITAL_POTS_SETTINGS &= 0xff00ffff;
						DIGITAL_POTS_SETTINGS |= ((int)dig_pot_val << 16);
						}
					else if (tempi == 3)
						{
						DIGITAL_POTS_SETTINGS &= 0xffff00ff;
						DIGITAL_POTS_SETTINGS |= ((int)dig_pot_val << 8);
						}
					else if (tempi == 4)
						{
						DIGITAL_POTS_SETTINGS &= 0xffffff00;
						DIGITAL_POTS_SETTINGS |= ((int)dig_pot_val << 0);
						}

					//printf("DIGITAL_POTS_SETTINGS = 0x%x\n", DIGITAL_POTS_SETTINGS); // For test
					servo_amp_interface(LOAD_DIGITAL_POTS);
					save_vars_to_flash();
					break;
				#endif
				default:   // Output to DAC channels
					ch_num = atoi(ptr);
					if (ch_num<1 || ch_num>4)
						{
						sprintf(g.b,"Error :%d #invalid channel number\n", 4); // invalid channel number.
						send_string((void *)g.spc, g.b);
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
								sprintf(g.b,"Error :%d #DAC value out of range\n", 6); // invalid channel number.
								send_string((void *)g.spc, g.b);
								break;
								}
							
							if (incremental)
								return_val = out_dac(ch_num, val, VOLTAGE_X_1000, INCREMENTAL);
							else
								return_val = out_dac(ch_num, val, VOLTAGE_X_1000, ABSOLUTE);
									
							if (return_val)
								{
								sprintf(g.b,"Error :%d #unable to send value to DAC channel\n", 7); // invalid channel number.
								send_string((void *)g.spc, g.b);
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
									sprintf(g.b,"Error :%d #DAC value out of range\n", 6); // invalid channel number.
									send_string((void *)g.spc, g.b);
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
								sprintf(g.b,"Error :%d #unable to send value to DAC channel\n", 7); // invalid channel number.
								send_string((void *)g.spc, g.b);
								break;
								}
							}
						}
					else  // We're reading the current data the DAC is set to
						{
						if (ch_num == 1) sprintf(g.b, "O_DAC1=%d\n", vtg.dac_word[1]);
						else if (ch_num == 2) sprintf(g.b, "O_DAC2=%d\n", vtg.dac_word[2]); // Not implemented
						else if (ch_num == 3) sprintf(g.b, "O_DAC3=%d\n", vtg.dac_word[3]); // Not implemented
						else if (ch_num == 4) sprintf(g.b, "O_DAC4=%d\n", vtg.dac_word[4]);	
						send_string((void *)g.spc, g.b);
						}		
					break;
				}
			break;
		case 'i': // Read ISW1 - ISW4
			ptr++;
			ch_num = atoi(ptr); // Use ch_num for convenience, not really a channel number.
			
			if (ch_num == 1) sprintf(g.b, "O_ISW1=%x\n", ISW1);
			else if (ch_num == 2) sprintf(g.b, "O_ISW2=%x\n", ISW2); // Not implemented
			else if (ch_num == 3) sprintf(g.b, "O_ISW3=%x\n", ISW3); // Not implemented
			else if (ch_num == 4) sprintf(g.b, "O_ISW4=%x\n", ISW4);
			else // Bad ISW specification.
				{
				sprintf(g.b,"Error :%d ISWy specification invalid, must be between 1 and 4#\n", 2); // invalid channel number.
				send_string((void *)g.spc, g.b);
				break;
				}
				
			send_string((void *)g.spc, g.b);	
			break;	
		case 'o': // Set or get OSW1 - OSW3
			ptr++;
			ch_num = atoi(ptr); // Use ch_num for convenience, not really a channel number.
			if (ch_num<1 || ch_num>3)				
				{
				sprintf(g.b,"Error :%d OSWy specification invalid, must be between 1 and 3#\n", 2); // invalid channel number.
				send_string((void *)g.spc, g.b);
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

					// For test
					//sprintf(g.b, "# Clearing OSW1 to %x\n", uval);
					//send_string((void *)g.spc, g.b); // Transmit the result.
					}
				else // Set bits
					{	
					if (*ptr == 'h' || *ptr == 'H') ptr++;
					uval = (unsigned int)htoi(ptr); // Value always hex
			
					if (ch_num == 1) OSW1 |= uval;
					else if (ch_num == 2) OSW2 |= uval; // Not implemented
					else if (ch_num == 3) OSW3 |= uval; // Not implemented

					// For test
					//sprintf(g.b, "# Setting OSW1 %x\n", uval);
					//send_string((void *)g.spc, g.b); // Transmit the result.
					}
				}	
			else // We're going to get the current state of OSW
				{
				if (ch_num == 1) sprintf(g.b, "O_OSW1=%x\n", OSW1);
				else if (ch_num == 2) sprintf(g.b, "O_OSW2=%x\n", OSW2); // Not implemented
				else if (ch_num == 3) sprintf(g.b, "O_OSW3=%x\n", OSW3); // Not implemented	
				
				send_string((void *)g.spc, g.b); // Transmit the result.
				}	
			break;	

		case 'v': // Transmit chip and firmware version numbers
		// First get and transmit the chip version
		for(x=0; x<8; x++)
			{
			tempstr[x] = *((char *)(0x74+x));
			}
		tempstr[8]=0;
		sprintf(g.b, "R:5#%s\n", tempstr);
		send_string((void *)g.spc, g.b); // Transmit the result.

		// Now transmit the firmware version
		sprintf(g.b, "R:6#%s%s%s%s%s%s, ad_loop%d\n", FIRMWARE_VERSION, FIRMWARE_TYPE, CONTROL_TYPE, ETHERNET_TYPE, PROTOCOL, HANDSHAKE, AD_VERSION);
		send_string((void *)g.spc, g.b); // Transmit the result.
		break; 

		// Many more to come	
		default: // Invalid operational command
			sprintf(g.b,"?%s\n",orig_cmd_str);
			send_string((void *)g.spc, g.b);
			break;			
		}
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
								(unsigned short int *)&vtg.v, ((MAX_VAR+1)*2));
	
	// Then the buffered control program back to flash
	vtw_flash_write_buffer(GM_FlashBase, (unsigned short int *)CPROG_FLASH_ADDR, 
								temp_cprog, ((MAX_STEP+1)*20));

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
	
	// Get servo valve amp type
	AMP_TYPE = (CONFIG_WORD2 >> 8) & 0x00000003;
	//printf("AMP_TYPE = %d\n", AMP_TYPE); // For test
		
	
	////////////////////////// First begin with axis 1 //////////////////////////////
	SET_PV_PLOAD(0);
	DAC_OUTPUT_CHANNEL = AXIS1_DAC_CHANNEL;
	BOARD_LEVEL_LVDT_OFFSET_DAC_CHANNEL = AXIS1_BOARD_LEVEL_LVDT_OFFSET_CHANNEL;
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

	VALVE_TYPE = AXIS1_VALVE_TYPE;


	if(AMP_TYPE == ONE_AXIS_INTEGRAL_AMP || AMP_TYPE == TWO_AXIS_INTEGRAL_AMP) 
		{
		ADC_CHANNEL_FOR_LVDT = 17; // CANA_CH1
		ADC_CHANNEL_FOR_COMMAND_MONITORING = 21; // WANA_CH1
		}
	else 
		{
		ADC_CHANNEL_FOR_LVDT = 21; // WANA_CH1
		ADC_CHANNEL_FOR_COMMAND_MONITORING = 22; // WANA_CH2
		}
		
	//SET_PV_IMASK(0xf00);
	//////////////////////////////////////////////////////////////////////////////	


	/////////////////////////// Now do axis 2 ////////////////////////////////////
	axis_num=2;

	SET_PV_PLOAD(0);
	DAC_OUTPUT_CHANNEL = AXIS2_DAC_CHANNEL;
	BOARD_LEVEL_LVDT_OFFSET_DAC_CHANNEL = AXIS2_BOARD_LEVEL_LVDT_OFFSET_CHANNEL;
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

	VALVE_TYPE = AXIS2_VALVE_TYPE;

	if(AMP_TYPE == ONE_AXIS_INTEGRAL_AMP || AMP_TYPE == TWO_AXIS_INTEGRAL_AMP)
		{
		ADC_CHANNEL_FOR_LVDT = 19; // CANA_CH3
		ADC_CHANNEL_FOR_COMMAND_MONITORING = 22; // WANA_CH2
		}
	else 
		{
		ADC_CHANNEL_FOR_LVDT = 23; // WANA_CH3
		ADC_CHANNEL_FOR_COMMAND_MONITORING = 24; // WANA_CH4
		}
	/////////////////////////////////////////////////////////////////////////////////////

	// Now finish up
	PULSE_PERIPHERAL_RESET;
					
	// Clear limit switch status indicators
	//for (y=0; y<6; y++)
	//	ls_stat[y] = 0;

	if(ZSPEED_OUTPUT_ENABLED)
		{
		TURN_OFF_LS_AND_ZSPEED_OUTPUTS;
		}
	else
		{
		TURN_OFF_LS_OUTPUTS;
		}

	ls_pio->np_pioedgecapture = 0;
	ls_pio->np_piointerruptmask = 0x3f; // Enable limit switches

	#if BUILD_FOR_CONTROL
		// update the control parameter array for each channel based on the variable settings
		update_control_parameters();
		servo_amp_interface(INITIALIZE_SERVO_AMP);
		update_servo_amp_interface();
	#endif
	}



void ram_clear(void)
	{
	int y;

	for (y=0; y<num_of_shot_data_samples; y++)
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
	CLR_SHOT_IN_MOTION;
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
	//int axis_num=2;
	np_pio *ls_pio = na_ls_int_input;
	unsigned char ls_status;
	//static unsigned char prev_ls_status;
	static int ls1_set, ls2_set, ls3_set, ls4_set, ls5_set, ls6_set, x;


	ls_status = ls_pio->np_piodata;
	ls_pio->np_pioedgecapture = 0;

	// If first time through, reset the ls indicators.
	if (x != 0x55555555) {x=0x55555555; ls1_set=0; ls2_set=0; ls3_set=0; ls4_set=0; ls5_set=0; ls6_set=0;}

	//printf("*0x%x", ls_status); // For test
	if (ls_status & 0x01)
		{
		if (!ls1_set)
			{
			LS1_1_ON;
						
			if ((PRESS_CONTROL_ENABLED) && (SHOT_IN_MOTION)) // For now, we'll assume that LS1 is the intensifier enable
				{
				out_dac(AXIS2_DAC_CHANNEL, PRESS_ARM_SETTING, PRESS_CMD, ABSOLUTE);
				//printf("[*, %d, %d, %d, %d]", AXIS2_DAC_CHANNEL, PRESS_ARM_SETTING, PRESS_CMD, ABSOLUTE); // For test
				}
			ls1_set=1;
			}
		}
	else {LS1_1_OFF; ls1_set=0;}

	if (ls_status & 0x02) {if(!ls2_set) {LS2_1_ON; ls2_set=1;}} else {LS2_1_OFF; ls2_set=0;}
	if (ls_status & 0x04) {if(!ls3_set) {LS3_1_ON; ls3_set=1;}} else {LS3_1_OFF; ls3_set=0;}
	if (ls_status & 0x08) {if(!ls4_set) {LS4_1_ON; ls4_set=1;}} else {LS4_1_OFF; ls4_set=0;}
	if (ls_status & 0x10) {if(!ls5_set) {LS5_1_ON; ls5_set=1;}} else {LS5_1_OFF; ls5_set=0;}
	if (ls_status & 0x20) {if(!ls6_set) {LS6_1_ON; ls6_set=1;}} else {LS6_1_OFF; ls6_set=0;}
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
			if (sample_ctr == 1) // First time through
				{
				SET_SHOT_IN_MOTION; // We're moving!
				SET_SHOT_IN_PROGRESS;
				}

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
			data_sample[sample_ctr].ana_ch5 = shared_memory[4];
			data_sample[sample_ctr].ana_ch6 = shared_memory[5];
			data_sample[sample_ctr].ana_ch7 = shared_memory[6];
			data_sample[sample_ctr].ana_ch8 = shared_memory[7];
				
			if (ANALOG_MODE == 6)
				{
				if (analog_mode6_ctr > 15) analog_mode6_ctr = 4; 
					
				RESULT5_SETUP = ad_setup_words[analog_mode6_ctr++];
				RESULT6_SETUP = ad_setup_words[analog_mode6_ctr++];
				RESULT7_SETUP = ad_setup_words[analog_mode6_ctr++];
				RESULT8_SETUP = ad_setup_words[analog_mode6_ctr++];
				}
			#if BUILD_FOR_CONTROL
			else if (ANALOG_MODE == 7) // Control analog mode w/integral amp
				{
				// Overwrite ISW1 and ISW4 with the DAC commands for axis #1 and #2 for diagnostics
				data_sample[sample_ctr].ana_ch3 = vtg.dac_word[AXIS1_DAC_CHANNEL]; // Normally DAC output #1
				data_sample[sample_ctr].ana_ch4 = vtg.dac_word[AXIS2_DAC_CHANNEL]; // Normally DAC output #2
				}
			#endif
			}
	//#endif	

		if ((PAST_MIN_STK_LEN) && (PAST_MIN_VEL_EOS))
			{
			// Check for zspeed
			if (vel > vel_eos && !(ZSPEED)) // ZSPEED!!!
				{ 
			    SET_ZSPEED; // This will also turn off analog channel toggling.
				CLR_SHOT_IN_MOTION;
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
	int pos=0, vel=0, axis_num=1, input_state, x;
	volatile unsigned short int *halfword_ptr;
	int ana_result;
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
	unsigned short int saved_pb_ctr;

		
	one_ms_timer->np_timerstatus = 0; // Clear timer int signal.
	ONE_MS_COUNTER++;
	
	//printf("*%d*",ONE_MS_COUNTER); // For test

	if (PAST_MIN_STK_LEN) // Prep for time based data sample
		{
		//printf("!"); // For test
		// If pos based collection exceeded tot stk len, time based coll will be < 2000 points. 
		if ((tb_ctr==(TIM_COLL_SAMPLES+1)) || ((pb_ctr+tb_ctr)==num_of_shot_data_samples)) // Shot's over, prep for end of data collection
			{
			//printf("[%d]", pb_ctr+tb_ctr); // For test
			if (bisc_ctr < TIM_DEL_BISC)
				{
				bisc_ctr++; // Still waiting for time delay biscuit
				}
			else  // Data collection is complete!!!
				{
				// Now compute biscuit size
				comp_shot_parameters.biscuit_size = TOTAL_STROKE_LEN - (RD_PV_POS);
				
				// Now save analog channels 9-16.  These are saved once per shot and placed
				// into the computed shot data structure.  
				halfword_ptr = &comp_shot_parameters.ch9;
				for (x=9; x<17; x++)
					{
					convert_ana_ch(x, RAW_VAL, &ana_result);
					halfword_ptr[x-9]=(unsigned short int)ana_result; // Save result to comp shot parameters
					//printf("ana ch%d = 0x%x\n", x, ana_result); // For test
					}
				
				CLR_PAST_MIN_STK_LEN;
				//CLR_MONITOR_ENABLE; // Don't want to do this here, the SHOT_COMPLETE section in monitor_supervisor() will clear this.
				CLR_PAST_MIN_VEL_EOS;
				CLR_ZSPEED; 
				SET_SHOT_COMPLETE;
				if (ZSPEED_OUTPUT_ENABLED) ZSPEED_OUTPUT_OFF;
				//printf("ISR, shot complete\n"); // For test
				}
			//printf("@"); // For test
			}
		else if (++tb_time_interval_ctr == timed_coll_interval) // Assume timed_coll_interval is mS
			{
			saved_pb_ctr = pb_ctr; // Used to detect if a PB sample has occured during this routine
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
					SET_ZSPEED; // This will also turn off analog channel toggling.
					CLR_SHOT_IN_MOTION;
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
			data_sample[pb_ctr+tb_ctr].ana_ch5 = shared_memory[4]; 
			data_sample[pb_ctr+tb_ctr].ana_ch6 = shared_memory[5]; 
			data_sample[pb_ctr+tb_ctr].ana_ch7 = shared_memory[6]; 
			data_sample[pb_ctr+tb_ctr].ana_ch8 = shared_memory[7]; 
			
			
			if (ANALOG_MODE == 6)
				{
				if (ZSPEED) // PB mode 6 sampling at higher speeds will be trashed unless we wait for zspeed first.
					{
					if (analog_mode6_ctr > 15) analog_mode6_ctr = 4; 
					
					RESULT5_SETUP = ad_setup_words[analog_mode6_ctr++];
					RESULT6_SETUP = ad_setup_words[analog_mode6_ctr++];
					RESULT7_SETUP = ad_setup_words[analog_mode6_ctr++];
					RESULT8_SETUP = ad_setup_words[analog_mode6_ctr++];
					}
				}

			#if BUILD_FOR_CONTROL
			else if (ANALOG_MODE == 7) // Control analog mode w/integral amp
				{
				// Overwrite ISW1 and ISW4 with the DAC commands for axis #1 and #2 for diagnostics
				data_sample[sample_ctr].ana_ch3 = vtg.dac_word[AXIS1_DAC_CHANNEL]; // Normally DAC output #1
				data_sample[sample_ctr].ana_ch4 = vtg.dac_word[AXIS2_DAC_CHANNEL]; // Normally DAC output #2
				}
			#endif

			// Check to see if PB update has occured since this isr was first activated.  Since the PB
			// routine has a higher priority, this could have happened.  If it has, don't increment
			// the tb_ctr which will in effect invaildate everything that has been written this cycle.
		//	DI; // Global interrupt disable, make sure a PB interrupt does not occur here
			if (saved_pb_ctr == pb_ctr) tb_ctr++;
		//	EI; // Reenable interrupts

			//axis1_interrupt->np_piointerruptmask = 1;	
			//printf("*"); // For test
			//printf("[%d-%d]", pb_ctr, tb_ctr); // For test
			}
		}
	
	
	quit: ;
	

	// Write OSW1 every mS, was finding outputs were changing.  Since we'll write continuously, no need to set the
	// output to 0x5555 first.
	opto_control->np_piodata = 0; // Clear read/write lines
	halfword_ptr = (unsigned short int *)&OSW1;
	opto_data->np_piodirection = 0xffff;
	
 	//opto_data->np_piodata = 0x5555; // To reduce ground bounce
	opto_data->np_piodata = ~halfword_ptr[0]; // Low half word of OSW1
	opto_control->np_piodata = 0x2; // wr monitor outputs
	opto_control->np_piodata = 0;	// Clear write lines
	
	//opto_data->np_piodata = 0x5555;
	opto_data->np_piodata = ~halfword_ptr[1]; // Upper half word of OSW1
	opto_control->np_piodata = 0x8; // wr control outputs
	//opto_control->np_piodata = 0;	// Clear write lines
	
	
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
		#if BUILD_FOR_INTERRUPT_CONTROL
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
	#endif
	}




void send_op_packets(void *context)
	{
	//volatile unsigned int *ptr;
	//unsigned short int saved_floating_channel_setup;
	int axis_num=1, x;
	volatile unsigned short int dyn_channels[8];
	np_pio *misc_ins_pio = na_misc_ins;
	s_connection_status *u = context;
	//np_pio *sel_hi_channels = na_sel_ana17_20;
	
	//ptr = &ad_setup_words.ch1;
	if (!vtg.xmitting_shot_data) // Don't send op packets if shot data is being sent
		{
		u->op_data_send_in_progress = 1;

		// If==1, Send only one
		// If==2, Keep sending
		
		if(!AD_CMD1) AD_CMD1 = 6;
		//printf("*"); // For test
		while (AD_CMD1);


		//convert_dyn_channels((volatile unsigned short int *)&op_data_packet.channels.dyn_ch1, RAW_VAL);

		//#if 0
		get_dac(1, VOLTAGE_X_1000, &op_data_packet.dac[0]);
		get_dac(2, VOLTAGE_X_1000, &op_data_packet.dac[1]);
		get_dac(3, VOLTAGE_X_1000, &op_data_packet.dac[2]);
		get_dac(4, VOLTAGE_X_1000, &op_data_packet.dac[3]);

		op_data_packet.pos = RD_PV_POS;
		op_data_packet.vel = (axis[axis_num].vel_ctr_freq/RD_PV_VEL); // Vel in pps
		// For test
		//printf("op_status pos = %d\n", op_data_packet.pos);
		//printf("op_status vel = %d\n", op_data_packet.vel);
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
		//op_data_packet.warning = 0x31323334; // For test
		
		// Upper chnnels are always converted regardless of analog mode
		op_data_packet.channels.dyn_ch1 = RESULT1;
		op_data_packet.channels.dyn_ch2 = RESULT2;
		op_data_packet.channels.dyn_ch3 = RESULT3;
		op_data_packet.channels.dyn_ch4 = RESULT4;
		op_data_packet.channels.dyn_ch5 = RESULT5;
		op_data_packet.channels.dyn_ch6 = RESULT6;
		op_data_packet.channels.dyn_ch7 = RESULT7;
		op_data_packet.channels.dyn_ch8 = RESULT8;
		op_data_packet.channels.cana_ch1 = RESULT17; // If monitor only or an ext. amp, the cana channels will be garbage.
		op_data_packet.channels.cana_ch2 = RESULT18;
		op_data_packet.channels.cana_ch3 = RESULT19;
		op_data_packet.channels.cana_ch4 = RESULT20;
		op_data_packet.channels.wana_ch1 = RESULT21;
		op_data_packet.channels.wana_ch2 = RESULT22;
		op_data_packet.channels.wana_ch3 = RESULT23;
		op_data_packet.channels.wana_ch4 = RESULT24;
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
		op_data_packet.analog[12] = 0x350a;
		op_data_packet.analog[13] = 0x360a;
		op_data_packet.analog[14] = 0x370a;
		op_data_packet.analog[15] = 0x380a;
		op_data_packet.analog[16] = 0x390a;
		op_data_packet.analog[17] = 0x3a0a;
		op_data_packet.analog[18] = 0x3b0a;
		op_data_packet.analog[19] = 0x3c0a;
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
		
		if(send_binary((void *)u, OP_DATA, (void *)&op_data_packet, sizeof(op_data_packet), NO_ACK))
			{
			printf("Problem sending op packets\n"); // For test
			u->send_op_packets = 0; // Some problem with transmit, we won't send any more.
			}

		if (u->send_op_packets == 1) u->send_op_packets = 0; // We only wanted one
								
		u->op_data_send_in_progress = 0;
		}
	}





inline void monitor_supervisor(void)
	{
	int y, axis_num=1, shot_data_size, proceed=0, pos=0, input_300_sensed=0;
	static int cs_ctr, wait_for_cs_to_go_low;
	volatile unsigned short int *halfword_ptr;
	//vp_pvu *pv1 = va_pv1;
	np_pio *misc_ins_pio = na_misc_ins;
	np_pio *misc_out_pio = na_misc_outs;
	np_pio *axis1_interrupt = na_axis1_int;
	np_pio *bounceback_pio = na_bounceback_int;
	np_pio *opto_control = na_opto_control;
	np_pio *opto_data = na_opto_data;
		
	// 1 of 3 things can happen here:
	//
	// 1, We get cycle start and set currently timing.
	// 2, We move past min stk len and clear currently timing.
	// 3, We get shot complete.	
	
	
	// For test
	//if (MONITOR_ENABLE) printf("m"); // For test
	//else printf("nm");

	if (MONITOR_ENABLE && !CURRENTLY_TIMING) // We're past the min stk len.  We'll complete the shot at this point.
		{
		//printf("!"); // For test
		if (SHOT_COMPLETE) // Check if the shot is complete
			{
			CLR_SHOT_COMPLETE;
			CLR_MONITOR_ENABLE;
			CLR_CS_DETECTED;
			CLR_CS_FROM_SS;  // The control program should clear this, however we'll do it here also.
			look_for_at_home = 1;  // Inform main() to begin looking for at home so we can send a message to the host.
					
			printf("Shot complete - eos=%d\n", comp_shot_parameters.eos_pos);
				
			if (CONFIG_WORD1 & 0x10000000) // xmit enabled?
				{
				if(!transmit_monitor_data((void *)&local_net_settings))
					printf("Transmitted to local net\n");
				}
			
			if (CONFIG_WORD1 & 0x20000000) // xmit enabled?
				{
				if(!transmit_monitor_data((void *)&plant_net_settings))
					printf("Transmitted to plant-wide net\n");
				}
			
					
			CLR_SHOT_IN_PROGRESS;

			#if MON_DEBUG	
					//printf("\n");
					//printf("Shot complete - vtg.gp_config_word = 0x%x\n", vtg.gp_config_word);
			#endif
			
			g.spc = &local_net_settings; // Set back to local context

			#if MON_DEBUG
				// For test
				printf("Local plug=%d, connected=%d, rem_ip_add=%d, rem_port=%d\n", 
					local_net_settings.plug, local_net_settings.connected, local_net_settings.remote_ip_address, local_net_settings.remote_port);

				printf("Plant_wide plug=%d, connected=%d, rem_ip_add=%d, rem_port=%d\n", 
					plant_net_settings.plug, plant_net_settings.connected, plant_net_settings.remote_ip_address, plant_net_settings.remote_port);
			#endif
			free((void *)data_sample); // Deallocate memory
			}
		}
	else if (MONITOR_ENABLE && CURRENTLY_TIMING)
		{
		//printf("$"); // For test
		if (PAST_MIN_STK_LEN)
			{
			CLR_CURRENTLY_TIMING; // We will complete a shot at this point
			}
		else if (ONE_MS_COUNTER > (unsigned int)TIME_OUT_PERIOD) // The dreaded 30 sec. timeout.
			{
			CLR_MONITOR_ENABLE;
			CLR_CURRENTLY_TIMING;
			CLR_CS_DETECTED;
			CLR_CS_FROM_SS;  // The control program should clear this, however we'll do it here also.
			CLR_SHOT_IN_PROGRESS;
			CLR_SHOT_IN_MOTION;
			bounceback_pio->np_piointerruptmask = 0;
			axis1_interrupt->np_piointerruptmask = 0;
			free((void *)data_sample); // Deallocate shot data memory	
				
			// Transmit a timeout message here.  Send to local or plant.
			// Anybody or everybody who's connected will get it.
			printf("%s", timeout_str);
			#if BUILD_FOR_UDP
				// Send string with handshaking
				SEND_STRING((void *)&local_net_settings, STRING, timeout_str, strlen(timeout_str), ACK);
				SEND_STRING((void *)&plant_net_settings, STRING, timeout_str, strlen(timeout_str), ACK);
			#else
				send_string((void *)&local_net_settings, timeout_str);
				send_string((void *)&plant_net_settings, timeout_str);
			#endif
			//if(local_net_settings.show_prompt)
			//	network_xmit((void *)&local_net_settings, (void *)prompt_str, strlen(prompt_str), NO_ACK, 0, 0);
			//if(plant_net_settings.show_prompt)
			//	network_xmit((void *)&plant_net_settings, (void *)prompt_str, strlen(prompt_str), NO_ACK, 0, 0);
			} 
		}
	else if (!MONITOR_ENABLE && CS_FROM_300) // CS from input 300. 
		{
		// We'll keep comming here as long as cs is active.  First we'll flag the rising edge
		//printf("csp"); // For test
		// After a shot, CS must drop out before we'll detect another one. 
		if (!wait_for_cs_to_go_low) 
			{
			// We'll begin counting.  We want the cs to be present for a period of time before we consider it legit.,
			// otherwise it might be noise.
			cs_ctr++;
			//printf("$"); // For test
			if (cs_ctr==50) // About 250mS with a monitor_supervisor scan time of 5mS which is set in main().
				{
				proceed=1;
				wait_for_cs_to_go_low=1; // We won't process anymore cycle starts until it first goes low.
				input_300_sensed=1; // Identify who caused a CS in the first place
				//printf("300 "); // For test
				}
			}
		}
	// If a CS has not triggered but a control start shot has, we'll force the monitor
	// to think that a CS has triggered.  The control program will set CS_FROM_SS
	// when the shot portion begins to execute.  The control system needs the ZSPEED stuff
	// set up, therefore we must also monitor when shot control is active.
	else if (!MONITOR_ENABLE && CS_FROM_SS) // CS from start shot 
		{
		//printf("csd"); // For test
		// After a shot, CS must drop out before we'll detect another one.  
		if (!wait_for_cs_to_go_low)
			{
			wait_for_cs_to_go_low=1;
			proceed=1;
			//printf("SS CS "); // For test
			}
		}
	else  // If we're here, CS must have dropped out, clear everything out.
		{
		cs_ctr=0;
		wait_for_cs_to_go_low=0;
		//printf("Clearing CS stuff\n"); // For test
		}
		
	// If proceed, a CS has been detected, we'll proceed with shot initiation.	
	if (proceed) 
		{
		//printf("proceed=1\n"); // For test
		// Force total stroke length to something workable if too long, force to 75" 20P rod
		if(TOTAL_STROKE_LEN > 6000)
			{
			TOTAL_STROKE_LEN = 6000; 
			printf("Total stroke length setting too long, resized to 6000 X4 counts\n");
			}
			
		// Figure out how many data samples we need for this shot and allocate the memory.
		shot_data_size=(sizeof(combined_sample)*TIM_COLL_SAMPLES)+(sizeof(combined_sample)*(TOTAL_STROKE_LEN/4));
		num_of_shot_data_samples = shot_data_size / sizeof(combined_sample);
		data_sample = (combined_sample *)malloc(shot_data_size);
		if (data_sample != NULL)  // Success in allocating memory
			{
			#if MON_DEBUG
				printf("data_sample=0x%x, shot_data_size=%d\n", data_sample, shot_data_size); 
				printf("Successfully allocated %d bytes, %d samples for shot mon. at 0x%x\n", shot_data_size, num_of_shot_data_samples, data_sample);
			#endif
				
			// Set CS detected bit in config word.
			SET_CURRENTLY_TIMING;
			SET_CS_DETECTED;
			
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
			look_for_at_home = 0;
			analog_mode6_ctr = 4;
			CLR_PAST_MIN_STK_LEN;
			CLR_PAST_MIN_VEL_EOS;
			ANALOG_DELAY = (unsigned short int)DELAY_BETWEEN_ANALOG_SAMPLES; // Set the delay between samples.

			set_analog_mode(ANALOG_MODE);


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
			shot_num++;

			#if MON_DEBUG
				//printf("vtg.gp_config_word = %x\n", vtg.gp_config_word);
				printf("OSW1 = %x\n", OSW1);
			#endif
			// Transmit a CS message here.  Send to local or plant.
			// Anybody or everybody who's connected will get it. Send with handshaking
			#if BUILD_FOR_UDP
				if (input_300_sensed)
					{
					printf("%s", cs_detected_300_str); // Echo out the serial port
					SEND_STRING((void *)&local_net_settings, STRING, cs_detected_300_str, strlen(cs_detected_300_str), ACK);
					SEND_STRING((void *)&plant_net_settings, STRING, cs_detected_300_str, strlen(cs_detected_300_str), ACK);
					}
				else
					{
					printf("%s", cs_detected_str);
					SEND_STRING((void *)&local_net_settings, STRING, cs_detected_str, strlen(cs_detected_str), ACK);
					SEND_STRING((void *)&plant_net_settings, STRING, cs_detected_str, strlen(cs_detected_str), ACK);
					}
			#else
				if (input_300_sensed)
					{
					printf("%s", cs_detected_300_str);
					send_string((void *)&local_net_settings, cs_detected_300_str);
					send_string((void *)&plant_net_settings, cs_detected_300_str);
					}
				else
					{
					printf("%s", cs_detected_str);
					send_string((void *)&local_net_settings, cs_detected_str);
					send_string((void *)&plant_net_settings, cs_detected_str);
					}
			#endif
			//if(local_net_settings.show_prompt)
			//	network_xmit((void *)&local_net_settings, (void *)prompt_str, strlen(prompt_str), NO_ACK, 0, 0);
			//if(plant_net_settings.show_prompt)
			//	network_xmit((void *)&plant_net_settings, (void *)prompt_str, strlen(prompt_str), NO_ACK, 0, 0);
			}
		}
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
	int vel_increment, pos=0, x, shot_data_size; 
	int temp_q1=4000, temp_q2=5000, temp_q3=6000, temp_q4=7000;
	int ms=0;
	unsigned int xmit_again;
	np_pio *misc_ins_pio = na_misc_ins;
	np_pio *button_pio = na_button_pio;
	
	// Build data, this will be transmitted continously
	
	// Manufacture a data set (shot data), this will be transmitted continously
	// to the plot program.

	// Allocate enough memory for the fake shot.
	free((void *)data_sample); // Make sure memory freed up before reassigning.
	shot_data_size=(sizeof(combined_sample)*NUM_OF_TIME_BASED_POINTS)+(sizeof(combined_sample)*((SHOT_END_POS+2)*20));
	num_of_shot_data_samples = shot_data_size / sizeof(combined_sample);
	data_sample = (combined_sample *)malloc(shot_data_size);
	if (data_sample != NULL) // Success in allocating memory
		{
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

		//while (SW7); // Stay here until button released
		nr_delay(300); // Debounce switch

		do
			{
			#if BUILD_FOR_POLLED_ETHERNET
				nr_plugs_idle();
			#endif

			// Transmit a CS message here.  Send to local or plant.
			// Anybody or everybody who's connected will get it.
			send_string((void *)&local_net_settings, "R:20 #Cycle start detected\n");
			send_string((void *)&plant_net_settings, "R:20 #Cycle start detected\n");
			//if(local_net_settings.show_prompt)
			//	network_xmit((void *)&local_net_settings, (void *)prompt_str, strlen(prompt_str), NO_ACK, 0, 0);
			//if(plant_net_settings.show_prompt)
			//	network_xmit((void *)&plant_net_settings, (void *)prompt_str, strlen(prompt_str), NO_ACK, 0, 0);
			
			nr_delay(500);

			// Transmit shot data here.  Whoever is not connected won't get anything.
			if(!transmit_monitor_data((void *)&local_net_settings))
				printf("SUCCESS!!! transmitted to local net\n\n");
			
			if(!transmit_monitor_data((void *)&plant_net_settings))
				printf("SUCCESS!!! Transmitted to plant-wide net\n\n");
			

			nr_delay(TIME_TO_WAIT_BETWEEN_SHOTS);
			#if BUILD_FOR_POLLED_ETHERNET
				nr_plugs_idle();
			#endif
				
			} while (MAKE_FAKE_SHOTS);
		
		//while (SW7); // Wait for button to be released
		nr_delay(300); // Debounce switch
		free((void *)data_sample);
		}
	}


// Isr to reset watchdog timer.  This triggers every 500mS. The watchdog timer will 
// timeout in 550mS.  If that were to happen the board would reset.  By writing the WD 
// timer's periodl register every 500mS, we should keep the watchdog well behaved.
void reset_watchdog(int context)
	{
	int axis_num=1;
	np_pio *misc_ins_pio = na_misc_ins;
	np_timer *wd_timer = na_watchdog;
	np_timer *wd_rst_timer = na_WD_rst_timer;
			
	wd_rst_timer->np_timerstatus = 0; // Clear timer int signal.
	wd_timer->np_timerperiodl = 0x55; // Write anything to periodl to reset watchdog.

	if (local_net_settings.send_op_packets) send_op_packets((void *)&local_net_settings);
	if (plant_net_settings.send_op_packets) send_op_packets((void *)&plant_net_settings);
	}



void set_analog_mode(int mode)
	{
	if (mode<1 || mode>9) mode=1; // Set back to 8 after test

	// First 4 channels must always be channels 1 - 4

	// Mode 6 is a psudo dynamic mode, channels 9,10,11,12 and 13,14,15,16 will mapped into channels 5,6,7,8
	// every third sample.
	if (mode==1 || mode==6) 
		{
		RESULT1_SETUP = ad_setup_words[ANA_CH1]; 
		RESULT2_SETUP = ad_setup_words[ANA_CH2]; 
		RESULT3_SETUP = ad_setup_words[ANA_CH3]; 
		RESULT4_SETUP = ad_setup_words[ANA_CH4]; 
		RESULT5_SETUP = ad_setup_words[ANA_CH5]; 
		RESULT6_SETUP = ad_setup_words[ANA_CH6]; 
		RESULT7_SETUP = ad_setup_words[ANA_CH7]; 
		RESULT8_SETUP = ad_setup_words[ANA_CH8]; 
		}
	else if (mode==2)
		{
		RESULT1_SETUP = ad_setup_words[ANA_CH1]; 
		RESULT2_SETUP = ad_setup_words[ANA_CH2]; 
		RESULT3_SETUP = ad_setup_words[ANA_CH3]; 
		RESULT4_SETUP = ad_setup_words[ANA_CH4]; 
		RESULT5_SETUP = ad_setup_words[ANA_CH9]; 
		RESULT6_SETUP = ad_setup_words[ANA_CH10]; 
		RESULT7_SETUP = ad_setup_words[ANA_CH11]; 
		RESULT8_SETUP = ad_setup_words[ANA_CH12]; 
		}
	else if (mode==3)
		{
		RESULT1_SETUP = ad_setup_words[ANA_CH1]; 
		RESULT2_SETUP = ad_setup_words[ANA_CH2]; 
		RESULT3_SETUP = ad_setup_words[ANA_CH3]; 
		RESULT4_SETUP = ad_setup_words[ANA_CH4]; 
		RESULT5_SETUP = ad_setup_words[ANA_CH13]; 
		RESULT6_SETUP = ad_setup_words[ANA_CH14]; 
		RESULT7_SETUP = ad_setup_words[ANA_CH15]; 
		RESULT8_SETUP = ad_setup_words[ANA_CH16]; 
		}
	else if (mode==4) // Allows monitoring all 4 upper channels
		{
		RESULT1_SETUP = ad_setup_words[ANA_CH1]; 
		RESULT2_SETUP = ad_setup_words[ANA_CH2]; 
		RESULT3_SETUP = ad_setup_words[ANA_CH3]; 
		RESULT4_SETUP = ad_setup_words[ANA_CH4]; 
		RESULT5_SETUP = ad_setup_words[WANA_CH1]; 
		RESULT6_SETUP = ad_setup_words[WANA_CH2]; 
		RESULT7_SETUP = ad_setup_words[WANA_CH3]; 
		RESULT8_SETUP = ad_setup_words[WANA_CH4]; 
		}
	else if (mode==5)  // Default control mode w/external amplifier, monitors upper channels 17 and 18 only.
		{
		RESULT1_SETUP = ad_setup_words[ANA_CH1]; 
		RESULT2_SETUP = ad_setup_words[ANA_CH2]; 
		RESULT3_SETUP = ad_setup_words[ANA_CH3]; 
		RESULT4_SETUP = ad_setup_words[ANA_CH4]; 
		RESULT5_SETUP = ad_setup_words[ANA_CH5]; 
		RESULT6_SETUP = ad_setup_words[ANA_CH6]; 
		RESULT7_SETUP = ad_setup_words[WANA_CH1]; 
		RESULT8_SETUP = ad_setup_words[WANA_CH2]; 
		}
	else if (mode==7)  // Default control mode w/integral amplifier.
		{
		RESULT1_SETUP = ad_setup_words[ANA_CH1]; 
		RESULT2_SETUP = ad_setup_words[ANA_CH2]; 
		RESULT3_SETUP = ad_setup_words[ANA_CH3];  // Actually used for firmware generated command amp1
		RESULT4_SETUP = ad_setup_words[ANA_CH4];  // Firmware generated command amp2
		RESULT5_SETUP = ad_setup_words[CANA_CH1]; // Converted LVDT feedback, amp1 
		RESULT6_SETUP = ad_setup_words[CANA_CH3]; // Converted LVDT feedback, amp2 
		RESULT7_SETUP = ad_setup_words[WANA_CH1]; // Actual command sig. wired to wide channel 1, if these channels are unavailable, firmware generated command sig. above could be used.
		RESULT8_SETUP = ad_setup_words[WANA_CH2]; // Actual command sig. wired to wide channel 2
		}
	else if (mode==8)  
		{
		RESULT1_SETUP = ad_setup_words[ANA_CH1]; 
		RESULT2_SETUP = ad_setup_words[ANA_CH2]; 
		RESULT3_SETUP = ad_setup_words[ANA_CH3]; 
		RESULT4_SETUP = ad_setup_words[ANA_CH4]; 
		RESULT5_SETUP = ad_setup_words[CANA_CH1]; // Converted LVDT feedback, amp1 
		RESULT6_SETUP = ad_setup_words[CANA_CH2]; // Raw LVDT feedback, amp1
		RESULT7_SETUP = ad_setup_words[CANA_CH3]; // Converted LVDT feedback, amp2
		RESULT8_SETUP = ad_setup_words[CANA_CH4]; // Raw LVDT feedback, amp2
		}
	// Mode 9 is only for a test, remove it later
	else if (mode==9)  
		{
		RESULT1_SETUP = ad_setup_words[ANA_CH1]; 
		RESULT2_SETUP = ad_setup_words[ANA_CH2]; 
		RESULT3_SETUP = ad_setup_words[ANA_CH3]; 
		RESULT4_SETUP = ad_setup_words[ANA_CH4]; 
		RESULT5_SETUP = ad_setup_words[ANA_CH5]; 
		RESULT6_SETUP = ad_setup_words[ANA_CH6]; 
		RESULT7_SETUP = ad_setup_words[CANA_CH1]; 
		RESULT8_SETUP = ad_setup_words[CANA_CH2]; 
		}
	}






///////////////////// O_scope, analog and I/O reporting functions /////////////////////

// Used for both the o_scope and block analog reporting.
// This function assumes that the sample report intervals are non-zero if analog reporting is desired.
// A zero interval indicates a single sample and that can be taken care of elsewhere.
int start_o_scope_timer(int mode)
	{
	unsigned int period;
	np_timer *o_scope_timer = na_o_scope_timer;
	volatile unsigned short int *halfword_ptr;

	// Check and see if the timer is already running, if not, we'll start it
	if (!(o_scope_timer->np_timerstatus & np_timerstatus_run_mask))
		{
		#if BUILD_FOR_CONTROL
		if (mode == O_SCOPE_MODE)
			{
			if(scope_data_packet==NULL)
				{
				scope_data_packet=(scope_data *)malloc(sizeof(scope_data));
				if (scope_data_packet == NULL) return (4); // Insufficient memory
				}
			convert_uS_to_timer(O_SCOPE_SAMPLE_INTERVAL_uS, &period); // Get period
			O_SCOPE=1;
			}
		else  // Must be a block analog report
		#endif
			{
			if (mode == ANALOG_REPORTING_MODE1)
				{
				if(analog_block_report==NULL)
					{
					analog_block_report=(input_based_analog_block *)malloc(sizeof(input_based_analog_block));
					if (analog_block_report == NULL) return (4); // Insufficient memory
					}
				convert_uS_to_timer(SAMPLE_INTERVAL_REPORT1, &period); // Get period
				ANALOG_REPORTING=1; // ANALOG_REPORTING contains the report number as well as indicating that a block analog sample is in progress.
				}
			else if (mode == ANALOG_REPORTING_MODE2)
				{
				if(analog_block_report==NULL)
					{
					analog_block_report=(input_based_analog_block *)malloc(sizeof(input_based_analog_block));
					if (analog_block_report == NULL) return (4); // Insufficient memory
					}
				convert_uS_to_timer(SAMPLE_INTERVAL_REPORT2, &period); // Get period
				ANALOG_REPORTING=2;
				}
			else if (mode == ANALOG_REPORTING_MODE3)
				{
				if(analog_block_report==NULL)
					{
					analog_block_report=(input_based_analog_block *)malloc(sizeof(input_based_analog_block));
					if (analog_block_report == NULL) return (4); // Insufficient memory
					}
				convert_uS_to_timer(SAMPLE_INTERVAL_REPORT3, &period); // Get period
				ANALOG_REPORTING=3;
				}
			else return (2);  // Illegal mode
			}

		halfword_ptr = (unsigned short int *)&period;
		//printf("pl = %x and ph = %x\n", periodl, periodh); // For test
		o_scope_timer->np_timerperiodl = halfword_ptr[0];
		o_scope_timer->np_timerperiodh = halfword_ptr[1];
		o_scope_timer->np_timercontrol = 7; // Start timer, continuous mode
		}
	else return(1); // Timer already running

	return (0);
	}


void stop_o_scope_timer(int free_)
	{
	np_timer *o_scope_timer = na_o_scope_timer;
	
	o_scope_timer->np_timercontrol = 8; // Stop timer

	if (free_)
		{
		if(analog_block_report!=NULL) free((void *)analog_block_report);
		if(scope_data_packet!=NULL) free((void *)scope_data_packet);

		ANALOG_REPORTING=0;
		O_SCOPE=0;
		set_analog_mode(ANALOG_MODE); // Restore default analog mode.
		}
	}



int analog_reporting_single(int report_num)
	{
	int return_val=0, sample_interval=0, x;
	volatile unsigned short *ptr1, *ptr2;
	input_based_analog_single single_analog_report;

	convert_all_channels((volatile unsigned short int *)&single_analog_report.channels, RAW_VAL);
					
	single_analog_report.report_number = report_num;
		
	send_binary((void *)&local_net_settings, SINGLE_SAMPLE_ANALOG_REPORTING, (void *)&single_analog_report, sizeof(input_based_analog_single), ACK);
	send_binary((void *)&plant_net_settings, SINGLE_SAMPLE_ANALOG_REPORTING, (void *)&single_analog_report, sizeof(input_based_analog_single), ACK);
	
	///////////////////////////// For test, print out data /////////////////////////////
	#if 0
		printf("Single analog report number = %d\n", single_analog_report.report_number);
				
		printf("Ana ch1 = 0x%x\n",single_analog_report.channels.ana_ch1);
		printf("Ana ch2 = 0x%x\n",single_analog_report.channels.ana_ch2);
		printf("Ana ch3 = 0x%x\n",single_analog_report.channels.ana_ch3);
		printf("Ana ch4 = 0x%x\n",single_analog_report.channels.ana_ch4);
		printf("Ana ch5 = 0x%x\n",single_analog_report.channels.ana_ch5);
		printf("Ana ch6 = 0x%x\n",single_analog_report.channels.ana_ch6);
		printf("Ana ch7 = 0x%x\n",single_analog_report.channels.ana_ch7);
		printf("Ana ch8 = 0x%x\n",single_analog_report.channels.ana_ch8);
		printf("Ana ch9 = 0x%x\n",single_analog_report.channels.ana_ch9);
		printf("Ana ch10 = 0x%x\n",single_analog_report.channels.ana_ch10);
		printf("Ana ch11 = 0x%x\n",single_analog_report.channels.ana_ch11);
		printf("Ana ch12 = 0x%x\n",single_analog_report.channels.ana_ch12);
		printf("Ana ch13 = 0x%x\n",single_analog_report.channels.ana_ch13);
		printf("Ana ch14 = 0x%x\n",single_analog_report.channels.ana_ch14);
		printf("Ana ch15 = 0x%x\n",single_analog_report.channels.ana_ch15);
		printf("Ana ch16 = 0x%x\n",single_analog_report.channels.ana_ch16);
		printf("Cana ch1 = 0x%x\n",single_analog_report.channels.cana_ch1);
		printf("Cana ch2 = 0x%x\n",single_analog_report.channels.cana_ch2);
		printf("Cana ch3 = 0x%x\n",single_analog_report.channels.cana_ch3);
		printf("Cana ch4 = 0x%x\n",single_analog_report.channels.cana_ch4);
		printf("Wana ch1 = 0x%x\n",single_analog_report.channels.wana_ch1);
		printf("Wana ch2 = 0x%x\n",single_analog_report.channels.wana_ch2);
		printf("Wana ch3 = 0x%x\n",single_analog_report.channels.wana_ch3);
		printf("Wana ch4 = 0x%x\n\n",single_analog_report.channels.wana_ch4);
	#endif
	/////////////////////////////////////////////////////////////////////////////////////

	return(return_val);
	}



int input_change_reporting(unsigned int isw1_bits_that_changed, unsigned int isw4_bits_that_changed)
	{
	input_change_report io_report;
	np_pio *misc_ins_pio = na_misc_ins;

	io_report.isw1 = ISW1;
	io_report.isw4 = (unsigned int)ISW4;
	io_report.isw1_bits_that_changed = isw1_bits_that_changed;
	io_report.isw4_bits_that_changed = isw4_bits_that_changed;
	io_report.one_ms_counter = ONE_MS_COUNTER;
	send_binary((void *)&local_net_settings, IO_CHANGE_REPORTING, (void *)&io_report, sizeof(input_change_report), ACK);
	send_binary((void *)&plant_net_settings, IO_CHANGE_REPORTING, (void *)&io_report, sizeof(input_change_report), ACK);

	///////////////////////////// For test, print out data /////////////////////////////
	#if 0
		printf("I/O change report\n");
				
		printf("ISW1 = 0x%x\n",io_report.isw1);
		printf("ISW4 = 0x%x\n",io_report.isw4);
		printf("isw1 bits that have changed = 0x%x\n",io_report.isw1_bits_that_changed);
		printf("isw4 bits that have changed = 0x%x\n",io_report.isw4_bits_that_changed);
		printf("One mS counter = %d\n\n",io_report.one_ms_counter);
	#endif
	/////////////////////////////////////////////////////////////////////////////////////

	return (0);
	}



// isr to send o_scope data packets and for block analog reporting
void o_scope_isr(int context)
	{
	int x;
	static int sample_ctr;
	short int adata;
	volatile unsigned short int upper_channels[8];
	np_timer *o_scope_timer = na_o_scope_timer;
	// For test
	//unsigned int time1, time2;
	//int axis_num=1;

	//printf("o_scope isr\n");
	o_scope_timer->np_timerstatus = 0; // Clear timer int signal.

	//if ((local_net_settings.send_o_scope_data || plant_net_settings.send_o_scope_data) && !ANALOG_REPORTING)
	if (O_SCOPE)
		{
		if (!(MONITOR_ENABLE)) // No shot in progress, keep going
			{
			if (sample_ctr == 2000) // Transmit data
				{
				// First stop timer
				stop_o_scope_timer(NO_FREE);

				sample_ctr = 0;
				vtg.been_below_trig_voltage = 0;

				if (plant_net_settings.send_o_scope_data)
					{
					//printf("plant_wide wants o_scope packets\n"); // For test
					plant_net_settings.o_scope_send_in_progress=1;
					//printf("collected 2000 points of o_scope data, xmit to plant net\n");  // For test
					// If network_xmit() is tied up by somebody else, we'll simply not send a pcaket this time.
					if(send_binary((void *)&plant_net_settings, O_SCOPE_DATA, (void *)scope_data_packet, sizeof(scope_data), NO_ACK))
						plant_net_settings.send_o_scope_data=0; // Some problem with xmit, we won't send any more.	
					plant_net_settings.o_scope_send_in_progress=0;
					}
				if (local_net_settings.send_o_scope_data)
					{
					//printf("local wants o_scope packets\n"); // For test
					local_net_settings.o_scope_send_in_progress=1;
					//printf("collected 2000 points of o_scope data, xmit to local net\n"); // For test
					// If network_xmit() is tied up by somebody else, we'll simply not send a pcaket this time.
					if(send_binary((void *)&local_net_settings, O_SCOPE_DATA, (void *)scope_data_packet, sizeof(scope_data), NO_ACK))
						local_net_settings.send_o_scope_data=0; // Some problem with xmit, we won't send any more.	
					local_net_settings.o_scope_send_in_progress=0;
					}

				// Restart timer
				start_o_scope_timer(O_SCOPE_MODE);
				}
			else  // Take another sample
				{
				convert_upper_channels(upper_channels, VOLTAGE_X_1000);
				
				// Convert the 4 channels to voltage X 1000.  We could have called the
				// convert_ana_ch() function 4 times and received converted data, however
				// that would result in A/D loops, somewhat inefficient. 
				for (x=0; x<4; x++) 
					{
					if(AMP_TYPE == ONE_AXIS_INTEGRAL_AMP || AMP_TYPE == TWO_AXIS_INTEGRAL_AMP)
						scope_data_packet->control_channels[sample_ctr][x] = upper_channels[x]; // CANA
					else
						scope_data_packet->control_channels[sample_ctr][x] = upper_channels[x+4];  //	WANA_CH1 - WANA_CH4
					}

				// Check if trigger requirments met.  Trigger always assumes a rising slope.
				if (sample_ctr == 0) // Still waiting for trigger
					{
					// Adjust trigger channel to WANA_CH1 - 4 if using an external amp
					if(AMP_TYPE == NO_OR_EXT_AMP)
						{
						if (O_SCOPE_TRIG_CH == 17) O_SCOPE_TRIG_CH = 21;
						else if (O_SCOPE_TRIG_CH == 18) O_SCOPE_TRIG_CH = 22;
						else if (O_SCOPE_TRIG_CH == 19) O_SCOPE_TRIG_CH = 23;
						else if (O_SCOPE_TRIG_CH == 20) O_SCOPE_TRIG_CH = 24;
						}

					// Always assume trigger channel is the first channel.
					if (O_SCOPE_TRIG_CH > 16 && O_SCOPE_TRIG_CH < 25) // Only channels 17-24 are valid
						{
						if (O_SCOPE_TRIG_V > -10001 && O_SCOPE_TRIG_V < 10001) // Tigger spec within a +/-10V range?
							{
							if (vtg.been_below_trig_voltage) // Capture on rising edge, we've been below the trig point.
								{
								if (scope_data_packet->control_channels[0][O_SCOPE_TRIG_CH<21 ? O_SCOPE_TRIG_CH-17 : O_SCOPE_TRIG_CH-21] >= O_SCOPE_TRIG_V)
								//if (scope_data_packet->control_channels[0][0] >= O_SCOPE_TRIG_V)
									{
									//printf("Trigger voltage met, %d volts on control channel %d (%d actual channel)\n", scope_data_packet->control_channels[0][O_SCOPE_TRIG_CH<21 ? O_SCOPE_TRIG_CH-17 : O_SCOPE_TRIG_CH-21], O_SCOPE_TRIG_CH<21 ? O_SCOPE_TRIG_CH-17 : O_SCOPE_TRIG_CH-21, O_SCOPE_TRIG_CH); // For test
									sample_ctr++; // Trig. met, data already written to location 0, we'll begin collecting data now 
									}
								}
							else  // Still waiting for voltage to fall below trig point before looking for trigger.
								{
								if (scope_data_packet->control_channels[0][O_SCOPE_TRIG_CH<21 ? O_SCOPE_TRIG_CH-17 : O_SCOPE_TRIG_CH-21] < O_SCOPE_TRIG_V)
								//if (scope_data_packet->control_channels[0][0] < O_SCOPE_TRIG_V)
									{
									//printf("Went below trigger voltage, %d volts on control channel %d (%d actual channel)\n", scope_data_packet->control_channels[0][O_SCOPE_TRIG_CH<21 ? O_SCOPE_TRIG_CH-17 : O_SCOPE_TRIG_CH-21], O_SCOPE_TRIG_CH<21 ? O_SCOPE_TRIG_CH-17 : O_SCOPE_TRIG_CH-21, O_SCOPE_TRIG_CH); // For test
									vtg.been_below_trig_voltage=1; 
									}
								}
							}
						}
					}
				else  // Trigger has already been met
					{
					sample_ctr++;
					}
				}
			}
		else  // Reset sample_ctr, don't send o_scope data during a shot
			{
			//printf ("Detected a shot taking place, resetting sample_ctr\n"); // For test
			stop_o_scope_timer(FREE);
			sample_ctr=0;
			}
		}
	else if (ANALOG_REPORTING)  // Analog reporting, if non-zero, number is the report number from 1 to 3.
		{
		//time1 = RD_PV_RAW_VEL_CTR; // For test
		//if (!(MONITOR_ENABLE)) // No shot in progress, keep going
		if (!(SHOT_IN_PROGRESS))
			{
			if (sample_ctr == 2000) // Transmit data
				{
				// First stop timer
				stop_o_scope_timer(NO_FREE); // Can't free up memory before we get the data.
				sample_ctr = 0;

				// Finish writing the data structure
				analog_block_report->report_number = ANALOG_REPORTING; // Report number 1, 2 or 3
				if (ANALOG_REPORTING == 1) analog_block_report->sample_interval_in_uS = SAMPLE_INTERVAL_REPORT1; // For reference
				else if (ANALOG_REPORTING == 2) analog_block_report->sample_interval_in_uS = SAMPLE_INTERVAL_REPORT2;
				else if (ANALOG_REPORTING == 3) analog_block_report->sample_interval_in_uS = SAMPLE_INTERVAL_REPORT3;

				// Now send data
				local_net_settings.o_scope_send_in_progress=1;
				send_binary((void *)&local_net_settings, BLOCK_SAMPLE_ANALOG_REPORTING, (void *)analog_block_report, sizeof(input_based_analog_block), ACK);
				local_net_settings.o_scope_send_in_progress=0;
				
				plant_net_settings.o_scope_send_in_progress=1;
				send_binary((void *)&plant_net_settings, BLOCK_SAMPLE_ANALOG_REPORTING, (void *)analog_block_report, sizeof(input_based_analog_block), ACK);
				plant_net_settings.o_scope_send_in_progress=0;

				///////////////////////////// For test, print out 5 samples /////////////////////////////
				#if 0
					//typedef volatile struct
					//{
					//int report_number; // 1, 2 or 3
					//unsigned int sample_interval_in_uS; // For reference
					//analog_channels channels[2000];
					//} input_based_analog_block;

					for(sample_ctr=0; sample_ctr<5; sample_ctr++)
						{
						printf("Sample %d\n", sample_ctr*100);
						printf("Report number = %d\n", analog_block_report->report_number);
						printf("Sample interval = %d\n",analog_block_report->sample_interval_in_uS);
						
						printf("Ana ch1 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch1);
						printf("Ana ch2 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch2);
						printf("Ana ch3 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch3);
						printf("Ana ch4 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch4);
						printf("Ana ch5 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch5);
						printf("Ana ch6 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch6);
						printf("Ana ch7 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch7);
						printf("Ana ch8 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch8);
						printf("Ana ch9 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch9);
						printf("Ana ch10 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch10);
						printf("Ana ch11 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch11);
						printf("Ana ch12 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch12);
						printf("Ana ch13 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch13);
						printf("Ana ch14 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch14);
						printf("Ana ch15 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch15);
						printf("Ana ch16 = 0x%x\n",analog_block_report->channels[sample_ctr*100].ana_ch16);
						printf("Cana ch1 = 0x%x\n",analog_block_report->channels[sample_ctr*100].cana_ch1);
						printf("Cana ch2 = 0x%x\n",analog_block_report->channels[sample_ctr*100].cana_ch2);
						printf("Cana ch3 = 0x%x\n",analog_block_report->channels[sample_ctr*100].cana_ch3);
						printf("Cana ch4 = 0x%x\n",analog_block_report->channels[sample_ctr*100].cana_ch4);
						printf("Wana ch1 = 0x%x\n",analog_block_report->channels[sample_ctr*100].wana_ch1);
						printf("Wana ch2 = 0x%x\n",analog_block_report->channels[sample_ctr*100].wana_ch2);
						printf("Wana ch3 = 0x%x\n",analog_block_report->channels[sample_ctr*100].wana_ch3);
						printf("Wana ch4 = 0x%x\n\n",analog_block_report->channels[sample_ctr*100].wana_ch4);
						}
					sample_ctr=0;
				#endif
				/////////////////////////////////////////////////////////////////////////////////////////

				stop_o_scope_timer(FREE);  // Free up memory now, we're finished
				}
			else  // Take another sample
				{
				convert_all_channels((volatile unsigned short *)&analog_block_report->channels[sample_ctr], RAW_VAL);
				sample_ctr++;
				}
			}
		else  // Reset sample_ctr, don't send o_scope data during a shot
			{
			//printf ("Detected a shot taking place, resetting sample_ctr\n"); // For test
			stop_o_scope_timer(FREE);
			sample_ctr=0;
			}

		//time2 = RD_PV_RAW_VEL_CTR; // For test
		//if (sample_ctr==50) printf("[%d]\n", time2-time1); // For test
		}
	else  // Stop timer and reset sample_ctr, nobody wants o_scope data
		{
		//printf("Stopping o_scope timer and clearing sample_ctr, nobody wants scope data\n");
		stop_o_scope_timer(FREE);
		sample_ctr=0;
		}
	}



//////////////////////////// CONTROL FUNCTIONS //////////////////////////////
#if BUILD_FOR_CONTROL

#include "combined_control_blocks3.c" // Both polled and interrupt driven control are supported in the "combined" file.

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
			
		case 'H':
		case 'h':
			// Halt control program
			stop_control();
			//printf(".H command \n"); // For test
			//FATAL_ERROR_WORD = PROGRAM_STOPPED_BY_USER;  // This will stop the control program
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
					if (*ptr == (unsigned char)NULL) break; // Didn't find any valid block types
					else if (*ptr == 'U' || *ptr == 'V' || *ptr == 'X' || *ptr == 'D' || *ptr == 'P' || *ptr == 'J' || *ptr == 'E')
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
						sprintf(g.b, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, g.b);
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
						sprintf(g.b, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, g.b);
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
						sprintf(g.b, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, g.b);
						}
					#if CONTROL_COMPILE_DEBUG
						printf("velocity block has been compiled \n\n");
					#endif
					break;
				
				case 'X':
					#if CONTROL_COMPILE_DEBUG
						printf("Ready to complile pressure block %s\n", orig_cmd_str+1);
					#endif

					if (pressure_block_compile(orig_cmd_str+1))
						{
						sprintf(g.b, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, g.b);
						}
					#if CONTROL_COMPILE_DEBUG
						printf("pressure block has been compiled \n\n");
					#endif
					break;

				case 'L':
					#if CONTROL_COMPILE_DEBUG
						printf("Ready to complile position block %s\n", orig_cmd_str+1);
					#endif

					if (position_block_compile(orig_cmd_str+1))
						{
						sprintf(g.b, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, g.b);
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
						sprintf(g.b, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, g.b);
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
						sprintf(g.b, "Error %s :2 #Syntax error\n", orig_cmd_str);
						send_string((void *)g.spc, g.b);
						}

					#if CONTROL_COMPILE_DEBUG	
						printf("end block has been compiled \n\n");
					#endif
					break;
							
				default: // Invalid block type	
					sprintf(g.b, "?%s\n", orig_cmd_str);	
					send_string((void *)g.spc, g.b);	
					break;
				}	
			}
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


// This function will assign the parameter structure for each axis with the values set
// in the variables.  This allows control code to switch axes on the fly without
// having to have seperate code for each axis.
void update_control_parameters(void)
	{
	int axis_num=1;

	// Set the axis dependant control parameters for axis1
	// Position loop / PID settings
	PROP_GAIN =	PROPORTIONAL_GN_SETTING1;	
	INT_GAIN = INTEGRAL_GN_SETTING1;		
	DERIV_GAIN = DERIVATIVE_GN_SETTING1;		
	INT_LIMIT =	INTEGRATOR_LIMIT_SETTING1;	
	ACCEL_FEEDFORWARD =	ACCEL_FEEDFORWARD_SETTING1;	
	VEL_FEEDFORWARD = VEL_FEEDFORWARD_SETTING1;	 
	TORQUE_LIMIT = TORQUE_LIMIT_SETTING1;		
	PEAK_TORQUE = PEAK_TORQUE_SETTING1;		
	POS_BK_GN_FE =	POS_BREAK_POINT_SETTING1;	 
	POS_BK_GN =	POS_BREAK_GAIN_SETTING1;		
	SET_ACC = SETTLING_ACCURACY_SETTING1;	
	// Velocity loop parameters
	VEL_LOOP_GAIN =	VEL_LOOP_GAIN_SETTING1;		
	VEL_LOOP_DERIV_GAIN = VEL_LOOP_DERIVATIVE_GN_SETTING1; 
	VEL_LOOP_BK_VEL1 =	VEL_LOOP_BK_VEL1_SETTING1;	
	VEL_LOOP_BK_VEL2 =	VEL_LOOP_BK_VEL2_SETTING1;	
	VEL_LOOP_BK_VEL3 =	VEL_LOOP_BK_VEL3_SETTING1;	
	VEL_LOOP_BKGN1 = VEL_LOOP_BKGN1_SETTING1;		
	VEL_LOOP_BKGN2 = VEL_LOOP_BKGN2_SETTING1;		
	VEL_LOOP_BKGN3 = VEL_LOOP_BKGN3_SETTING1;		
	// Common parameters
	DITHER_TIME = DITHER_TIME_SETTING1;		
	DITHER_AMPL = DITHER_AMPL_SETTING1;		
	NULL_OFFSET = NULL_OFFSET_SETTING1;		
	LVDT_OFFSET = LVDT_OFFSET_SETTING1;		
	SAMPLE_TIME = SAMPLE_TIME_SETTING1;		
	DERIV_TIME = DERIVATIVE_TIME_SETTING1;	

	// Set the control parameters for axis2
	axis_num=2;
	// Position loop / PID settings
	PROP_GAIN =	PROPORTIONAL_GN_SETTING2;	
	INT_GAIN = INTEGRAL_GN_SETTING2;		
	DERIV_GAIN = DERIVATIVE_GN_SETTING2;		
	INT_LIMIT =	INTEGRATOR_LIMIT_SETTING2;	
	ACCEL_FEEDFORWARD =	ACCEL_FEEDFORWARD_SETTING2;	
	VEL_FEEDFORWARD = VEL_FEEDFORWARD_SETTING2;	 
	TORQUE_LIMIT = TORQUE_LIMIT_SETTING2;		
	PEAK_TORQUE = PEAK_TORQUE_SETTING2;		
	POS_BK_GN_FE =	POS_BREAK_POINT_SETTING2;	 
	POS_BK_GN =	POS_BREAK_GAIN_SETTING2;		
	SET_ACC = SETTLING_ACCURACY_SETTING2;	
	// Velocity loop parameters
	VEL_LOOP_GAIN =	VEL_LOOP_GAIN_SETTING2;		
	VEL_LOOP_DERIV_GAIN = VEL_LOOP_DERIVATIVE_GN_SETTING2; 
	VEL_LOOP_BK_VEL1 =	VEL_LOOP_BK_VEL1_SETTING2;	
	VEL_LOOP_BK_VEL2 =	VEL_LOOP_BK_VEL2_SETTING2;	
	VEL_LOOP_BK_VEL3 =	VEL_LOOP_BK_VEL3_SETTING2;	
	VEL_LOOP_BKGN1 = VEL_LOOP_BKGN1_SETTING2;		
	VEL_LOOP_BKGN2 = VEL_LOOP_BKGN2_SETTING2;		
	VEL_LOOP_BKGN3 = VEL_LOOP_BKGN3_SETTING2;		
	// Common parameters
	DITHER_TIME = DITHER_TIME_SETTING2;		
	DITHER_AMPL = DITHER_AMPL_SETTING2;		
	NULL_OFFSET = NULL_OFFSET_SETTING2;		
	LVDT_OFFSET = LVDT_OFFSET_SETTING2;		
	SAMPLE_TIME = SAMPLE_TIME_SETTING2;		
	DERIV_TIME = DERIVATIVE_TIME_SETTING2;
	}


int null_valve(int axis_num, int action)
	{
	int return_val=0, failed=0, correction_increment, null_voltage, x, ctr;
		
	//printf("Valve test, axis_num=%d, action=%d\n",axis_num, action);
	// Note:
	// NULL_OFFSET is the working null value that is in the axis structure.  The value of
	// the variable axis_num determines the structure member to retreive the null from.

	// NULL_OFFSET_SETTING1 and NULL_OFFSET_SETTING2 are the vtg.v[] variable settings.
	// These are not usable in NULL_OFFSET until update_control_parameters() or 
	// initialize_axes() are called, normally at cycle start.  Command OC4<CR> will

	// VALVE_TYPE is setup but none are defined at this time.
		
	if (axis_num < 1 || axis_num > 2)
		{
		sprintf(g.b,"R_AXIS%d_INVALID_AXIS_SPECIFIER:21 \n",axis_num); 
		send_string((void *)g.spc, g.b);
		return_val = 99;
		} 
	else if (action == CHECK_NULL)
		{
		// Derive the current voltage and write the result to a reserved variable

		// Zero out the result variable
		NULL_RESULT = 0;

		// Send a zero volt command, subtract out the lvdt offset
		out_dac(DAC_OUTPUT_CHANNEL, 0-LVDT_OFFSET, VOLTAGE_X_1000, ABSOLUTE);
	//printf("outputting %d volts, dac channel = %d\n", 0-LVDT_OFFSET, DAC_OUTPUT_CHANNEL);
		// Then get null value
	//printf("ADC_CHANNEL_FOR_LVDT=%d\n", ADC_CHANNEL_FOR_LVDT); // For test
		convert_ana_ch(ADC_CHANNEL_FOR_LVDT, VOLTAGE_X_1000, &null_voltage);
		
		// Write the result to variable holding the null result
		NULL_RESULT = null_voltage;  // Save result to a variable, this may be useful for something.
	//printf("Null voltage = %d (0x%x)\n", null_voltage, null_voltage);	
		sprintf(g.b,"R_AXIS%d_NULL_RESULT:22 #%d\n", axis_num, null_voltage); 
		send_string((void *)g.spc, g.b);
		}
	else if (action == AUTO_NULL)
		{
		// Send a zero volt command
		out_dac(DAC_OUTPUT_CHANNEL, 0-LVDT_OFFSET, VOLTAGE_X_1000, ABSOLUTE);

		// Read null voltage
		convert_ana_ch(ADC_CHANNEL_FOR_LVDT, VOLTAGE_X_1000, &null_voltage);
		if (null_voltage > 500 || null_voltage < -500) // Too far gone
			{
			sprintf(g.b,"R_AXIS%d_BEGINNING_NULL_EXCEEDS_ALLOWABLE_RANGE:23 #%d\n", axis_num, null_voltage); 
			send_string((void *)g.spc, g.b);
			update_control_parameters();  // Restore parameters to initial settings
			}
		else if (null_voltage > 100 || null_voltage < -100) // +/- 100mV allowable range.
			{
			if (null_voltage > 0) correction_increment = -5; // 5 mV increments
			else correction_increment = 5;

			// We'll attempt to correct, up to 1V + or -
			for (ctr=0; ctr<200; ctr++) 
				{
				// NULL_OFFSET is not zeroed before beginning.  We begin wherever it is.
				NULL_OFFSET += correction_increment; // Set working null
				printf("NULL_OFFSET=%d, NULL_OFFSET-LVDT_OFFSET=%d\n",NULL_OFFSET ,NULL_OFFSET-LVDT_OFFSET);  // For test
				out_dac(DAC_OUTPUT_CHANNEL, NULL_OFFSET-LVDT_OFFSET, VOLTAGE_X_1000, ABSOLUTE);
				nr_delay(25); // Allow valve to settle out, add this later if necessary
				convert_ana_ch(ADC_CHANNEL_FOR_LVDT, VOLTAGE_X_1000, &null_voltage);
				printf("null_voltage=%d\n\n", null_voltage); // For test
				if (null_voltage < -25 || null_voltage > 25)
					{
					// Not there yet, keep going
					}
				else  // Success!!!
					{
					break;
					}
				}

			// Transmit a success message, control program ASCII file will have
			// to be updated by application software.
			if (ctr>=100) 
				{
				sprintf(g.b,"R_AXIS%d_AUTO_NULL_FAILED_UNABLE_TO_CORRECT:24 \n",axis_num); 
				send_string((void *)g.spc, g.b);
				update_control_parameters();  // Restore parameters to initial settings
				}
			else
				{
				sprintf(g.b,"R_AXIS%d_AUTO_NULL_SUCCESS:25 #%d\n",axis_num, NULL_OFFSET); 
				send_string((void *)g.spc, g.b);

				if (axis_num==1)
					{
					// Update the setting from the working copy.
					NULL_OFFSET_SETTING1 = NULL_OFFSET;
					}
				else if (axis_num==2)
					{
					// Ditto
					NULL_OFFSET_SETTING2 = NULL_OFFSET;
					}
				// Save variables to flash
				save_vars_to_flash();
				}
			}
		else  // Null good, no need to correct
			{
			sprintf(g.b,"R_AXIS%d_NULL_ALREADY_GOOD_NO_NEED_TO_CORRECT:26 \n",axis_num); 
			send_string((void *)g.spc, g.b);
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
	vtg.timer2_timeout=1; // Used for polled control

	#if BUILD_FOR_INTERRUPT_CONTROL
		// Now set the interrupt signal to interrupt control.
		SET_TIMER2_TIMEOUT_INTERRUPT;
	#endif
	}



// This is our isr with interrupt driven control, otherwise we call this for polled interrupt
void control_supervisor(int context)
	{
	ub *block;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	np_pio *misc_out_pio = na_misc_outs;
	np_pio *misc_ins_pio = na_misc_ins;
	np_pio *button_pio = na_button_pio;
	np_timer *timer2 = na_timer2;
	np_timer *wd_timer = na_watchdog;
	int axis_num=1, blk_exe_return_val=0, x, input_state, y;
	int go=0; // For test
	unsigned short int int_data; // Only 9 bits are used
	char tempstr[81];
	static int ctr, begin_ms; // For test
		
	#if BUILD_FOR_INTERRUPT_CONTROL		
		//printf("$"); // For test
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
		
		// Find out which interrupt interrupted us
		for (x=0; x<9; x++)
			{
			if (int_data & (1<<x)) break;
			}
		// This is the call back to the currently executing control block
		// upon returning from an interrupt.
		blk_exe_return_val = block_function_ptr[when_cont_isr[x].blk_id]((c_block)NULL, (int)NULL, when_cont_isr[x].zone_to_goto);
	#endif
	
	do_again: ;
		// Keep watchdog reset.  Watchdog reset has a lower priority than this control interrupt.
		// Therefore if we hang around here too long the board may reset.  We'll hammer
		// the watchdog timer here to keep it reset.
		wd_timer->np_timerperiodl = 0x55; // Write anything to periodl to reset watchdog.
		
		// If we're hung in this ISR, the user can press SW8 to kill the control program.
		//if(SW8) {STOP_CONTROL;}
		if(STOP_CONTROL_PROGRAM_SWITCH) {STOP_CONTROL;} // Now using dip switch SW5-3

		// If a block is compiled for IMMED execution and the control prog. is running, 
		// we'll execute it here.  If the control prog. is not running, an IMMED block
		// will be initiated by the block compile function.
		if (IMMED_EXE_BLK && !blk_exe_return_val) // There is an IMMED block to execute
			{
			if (!vtg.immed_in_process) // Initial entry into the block
				{
				vtg.immed_in_process=1;
				block = (ub *)&IMMED_EXE_BLK;

				// This is the call to a control block function when executing
				// an IMMED block.
				blk_exe_return_val = block_function_ptr[block->blk_id]((c_block)&IMMED_EXE_BLK, IMMED, 1);	
				if (blk_exe_return_val != REENTER_ON_INTERRUPT) // Anything other that REENTER_ON_INTERRUPT we'll consider the end of the block
					{
					IMMED_EXE_BLK = (int)NULL; // Proceed to normal program execution below.
					vtg.immed_in_process=0;
					}
				// else we'll deal with any return value in this loop

				}
			else  // We finished running an IMMED block after reentering 1 or more times.
				{
				IMMED_EXE_BLK = (int)NULL; // Proceed to normal program execution below.
				vtg.immed_in_process=0;
				}
			} // IMMED
		
		#define LOCATION_OF_BLOCK_TO_EXECUTE vtg.control_program[vtg.prog_ctr][0]
		
		// This is the call to a new control block.  Only call if there is not
		// a pending return value from either a return from interrupt or IMMED.
		if(!blk_exe_return_val)  
			{
			// Normal program execution
			block = (ub *)&LOCATION_OF_BLOCK_TO_EXECUTE;

		// For test
		//if (vtg.prog_ctr == 1) printf("\n\n\n\n");
		//if (vtg.prog_ctr >= 20 && vtg.prog_ctr <= 50) {printf("[%d_%d_", vtg.prog_ctr, RD_PV_POS); go=1;}
		//printf("[%d]", vtg.prog_ctr);

			#if CONTROL_INTERRUPT_DEBUG
				//printf("CPS-block->blk_to_execute=0x%x\n", block->blk_to_execute);
				printf("CPS-block->blk_id=0x%x, prog_ctr=%d\n", block->blk_id, vtg.prog_ctr);
				//for (x=0; x<10; x++)
				//	{
				//	printf("vtg.control_program[%d][%d]=0x%x\n", vtg.prog_ctr, x, vtg.control_program[vtg.prog_ctr][x]);
				//	}
			#endif

			//printf("%d\n", vtg.prog_ctr);
			blk_exe_return_val = block_function_ptr[block->blk_id]((c_block)&LOCATION_OF_BLOCK_TO_EXECUTE, PROGRAM, 1);
		// For test
		//if (go==1) printf("%d_%d]", blk_exe_return_val, block->blk_id);
			}
		
		// We could have a return val from return from interrupt, IMMED or a
		// new block that we executed above, we'll handle them all here.  FATAL_ERROR_WORD
		// and WARNING_WORD will be set here it they occured during a block.  Control
		// blocks themselves will not set these directly.
		if(blk_exe_return_val)
			{
			// Handle return values here 
			if (blk_exe_return_val == REENTER_ON_INTERRUPT) goto ext; // Interrupt cont only 
			else if ((blk_exe_return_val>NO_PROBLEM) && (blk_exe_return_val<REENTER_ON_INTERRUPT))
				WARNING_WORD=blk_exe_return_val;
			else // Anything else we'll treat as a fatal error
				{
				//printf("Detected a fatal error in return val\n");
				FATAL_ERROR_WORD=blk_exe_return_val;
				}
			}

		if (FATAL_ERROR_WORD)
			{
			printf("FATAL ERROR %d!!!\n", FATAL_ERROR_WORD); // For test
			// Format the error message here.  Do it first, otherwise the program
			// block will be reset.
			sprintf(tempstr, "F_Cont. Fatal err:%d # prog_step=%d\n", FATAL_ERROR_WORD, vtg.prog_ctr);
			initialize_control();
																
			SET_FATAL_ERROR;
					
			out_dac(AXIS1_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 1 to the retract setting
			axis_num=2;
			out_dac(AXIS2_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 2 to zero
			TURN_OFF_ALL_CONT_OUTPUTS;
			CLR_PROGRAM_RUNNING;
			blk_exe_return_val=0; 
			
			// Generate wathcdog fault here
			PULSE_WATCHDOG_FAULT;	
			
			// Transmit the fatal error message
			send_string((void *)&local_net_settings, tempstr);
			send_string((void *)&plant_net_settings, tempstr);
			//network_xmit((void *)&local_net_settings, tempstr, strlen(tempstr), NO_ACK, 0, 0);
			//network_xmit((void *)&plant_net_settings, tempstr, strlen(tempstr), NO_ACK, 0, 0);	
			goto ext; //break; // Exit and wait for a start program interrupt
			}
	
		if (WARNING_WORD)
			{
			printf("WARNING!!! %d\n", WARNING_WORD); // For test
			// Kind'a like a fatal error except program execution
			// will continue.

			// Format the warning message here.  Do it first to so that we have
			// the program info. before it is reset.
			sprintf(tempstr, "W_Cont. warning:%d # prog_step=%d\n", WARNING_WORD, vtg.prog_ctr);
			// For test
			//printf("W_Cont. warning:%d # prog_step=%d\n", WARNING_WORD, vtg.prog_ctr);
					
			
			vtg.saved_warning = WARNING_WORD; // Archive warning	
			//initialize_control();
											
			SET_WARNING;
			set_clr_discrete_output(WARNING_OUTPUT, TURN_OFF); // Enable warning by turning off
			blk_exe_return_val=0; // Begin executing a fresh block.
										
			// Transmit the warning error message
			send_string((void *)&local_net_settings, tempstr);
			send_string((void *)&plant_net_settings, tempstr);
			//network_xmit((void *)&local_net_settings, tempstr, strlen(tempstr), NO_ACK, 0, 0);
			//network_xmit((void *)&plant_net_settings, tempstr, strlen(tempstr), NO_ACK, 0, 0);	
			reset_control_program(DONT_CLEAR_SAVED_WARNING);
			goto ext;
			}
	
		#if BUILD_FOR_INTERRUPT_CONTROL
			// Keep looping and executing control blocks until REENTER_ON_INTERRUPT.
			if(PROGRAM_LOADED && PROGRAM_RUNNING) goto do_again;
		#endif
		// Otherwise will only pass through once, execute 1 block, if polled.
		
	ext: ;
	}



void initialize_control(void)
	{
	int x;
	np_timer *timer2 = na_timer2;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;

	
	INITIALIZE_CONTROL_BLOCKS;
	MASK_ALL_CONT_INTERRUPTS; 
	CLR_ALL_CONTROL_INTERRUPTS;
	vtg.blk_timeout_time = 0; 
	vtg.immed_in_process = 0; 
	IMMED_EXE_BLK = (int)NULL; 
	vtg.io_int1 = 0; 
	vtg.io_int2 = 0; 
	/* Clear out the IMMED block location. */ 
	for (x=0; x<10; x++) vtg.control_program[IMMED_EXECUTE_BLOCK][x] = 0; 
	vtg.prog_ctr = 1; 
	vtg.stack_ptr = 0; 
	timer2->np_timercontrol = 8; 
	vtg.timer2_timeout = 0; 

	#if BUILD_FOR_INTERRUPT_CONTROL
		for (x=0; x<9; x++) 
			{ 
			when_cont_isr[x].blk_id = 0; 
			when_cont_isr[x].zone_to_goto = 0; 
			} 
		when_cont_isr[0].blk_id = END_; 
		when_cont_isr[1].blk_id = START_; 
		UNMASK_START_PROG_INTERRUPT; 
		UNMASK_STOP_INTERRUPT;
	#endif
	}



// function to start / restart control program.  This wipes the slate clean.
void reset_control_program(int saved_warning_action)
	{
	int x, axis_num=1;
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	np_pio *misc_out_pio = na_misc_outs;
	np_pio *misc_ins_pio = na_misc_ins;
	np_timer *timer2 = na_timer2;
	

	if (MASTER_ENABLE)
		{	
		initialize_control(); // This will leave the start interrupt bit unmasked.
		//printf("control_int->np_piointerruptmask=0x%x\n", control_int->np_piointerruptmask); // For test
		//control_int->np_piointerruptmask|=0x80; // For test
		//printf("control_int->np_piointerruptmask=0x%x\n", control_int->np_piointerruptmask); // For test

		#if BUILD_FOR_INTERRUPT_CONTROL
			SET_START_PROGRAM_INTERRUPT; // Set the interrupt bit
		#else // Polled control, we'll set up everything here
			// Also need to clear the watchdog output and the warning output here	
			SET_PROGRAM_RUNNING;
			CLR_FATAL_ERROR;
			CLR_WARNING;
			//OSW1 &= TURN_OFF_CONT_OUTPUTS_EXCEPT_LS_WARN_MASK; 
			TURN_OFF_CONT_OUTPUTS_EXCEPT_LS_WARN;
			WARNING_WORD=0;
			FATAL_ERROR_WORD=0;	
			
			//out_dac(CONT_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE);
			out_dac(AXIS1_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 1 to the retract setting
			axis_num=2;
			// If there is not a valid DAC channel specified for axis2, out_dac() will not do anything
			out_dac(AXIS2_DAC_CHANNEL, RETR, VOLTAGE_X_1000, ABSOLUTE); // Set DAC channel 2 to zero

			// Need to disable WD and warning output here.
			set_clr_discrete_output(WARNING_OUTPUT, TURN_ON); // Disable warning by turning on
			PULSE_WATCHDOG_RESET;
			update_control_parameters();
		#endif

		if (saved_warning_action == CLEAR_SAVED_WARNING)
			{
			vtg.saved_warning = 0;
			}
		}
	else
		{
		send_string((void *)g.spc,"#Unable to start control program, master enable is disabled\n");
		}
	}




void stop_control(void)
	{
	np_pio *control_int = na_control_int;
	np_pio *cont_int_output = na_cont_int_output;
	np_pio *misc_out_pio = na_misc_outs;
	int axis_num=1;
	
	//printf("[stop_control] About to stop control program\n"); // For test

	#if BUILD_FOR_INTERRUPT_CONTROL
		if (PROGRAM_RUNNING)
			{
			UNMASK_STOP_INTERRUPT; // Make sure stop interrupt is unmasked
			SET_STOP_PROGRAM_INTERRUPT; // Set the interrupt bit
			}
	#else
		if (PROGRAM_RUNNING) STOP_CONTROL;
	#endif
	}




//#define TOGGLE_SCLK		vtg.samp &= 0xfe; \
						servo_amp_interface->np_piodata = vtg.samp; \
						for(z=0; z<1000; z++) { } \
						vtg.samp |= 0x01; \
						servo_amp_interface->np_piodata = vtg.samp; \
						for(z=0; z<1000; z++) { }

#define TOGGLE_SCLK		vtg.samp |= 0x01; \
						servo_amp_interface->np_piodata = vtg.samp; \
						for(z=0; z<1000; z++) { } \
						vtg.samp &= 0xfe; \
						servo_amp_interface->np_piodata = vtg.samp; \
						for(z=0; z<1000; z++) { }

#define ENABLE_SSN		vtg.samp &= 0xfb; \
						servo_amp_interface->np_piodata = vtg.samp

#define DISABLE_SSN		vtg.samp |= 0x04; \
						servo_amp_interface->np_piodata = vtg.samp

int servo_amp_interface(int action)
	{
	int x, y, z;
	int flip_amp2_cur, flip_amp2_lvdt, flip_amp1_cur, flip_amp1_lvdt, tempi=0;
	int invert_amp2_cur, invert_amp2_lvdt, invert_amp1_cur, invert_amp1_lvdt;
	unsigned int val, tv, tv1;
	np_pio *servo_amp_interface = na_samp_interface;
	np_pio *lvdt_demod1_phase = na_lvdt_demod1_phase;
	np_pio *lvdt_demod2_phase = na_lvdt_demod2_phase;
	np_pio *lvdt_osc_phase = na_lvdt_osc_phase;	
	
	// Invert value sent to current gain pots so that an increasing value results in an
	// increasing gain.
	invert_amp2_cur = 1;
	invert_amp2_lvdt = 0;
	invert_amp1_cur = 1;
	invert_amp1_lvdt = 0;

	// Data is xmitted LSB first.  AD7376 wants MSB first.  Must flip all 4 settings.
	flip_amp2_cur = 1;
	flip_amp2_lvdt = 1;
	flip_amp1_cur = 1;
	flip_amp1_lvdt = 1;

	
	if(AMP_TYPE == ONE_AXIS_INTEGRAL_AMP || AMP_TYPE == TWO_AXIS_INTEGRAL_AMP)
		{
		if(action == LOAD_DIGITAL_POTS)
			{
			// Not dependant on axis_num.  All 4 are always loaded.  If the amplifier is single axis,
			// The values for axis2 will be ignored.

			// Now send the settings to the digital pots.  We'll brute force an SPI interface.  
			// Speed is not an issue here so this is acceptable.  We don't need a hardware based SPI interface.
			ENABLE_SSN; 
			for (x=0; x<4; x++)
				{
				val = DIGITAL_POTS_SETTINGS; 

				// Invert
				//;printf("Before inverting, val = 0x%x\n", val); // For test
				if(invert_amp2_cur) inverter(&val, 0, NO_OF_DIGITAL_POT_BITS);
				if(invert_amp2_lvdt) inverter(&val, 8, NO_OF_DIGITAL_POT_BITS);
				if(invert_amp1_cur) inverter(&val, 16, NO_OF_DIGITAL_POT_BITS);
				if(invert_amp1_lvdt) inverter(&val, 24, NO_OF_DIGITAL_POT_BITS);
				//printf("After inverting, val = 0x%x\n", val); // For test

				// Flip, value saved little endian, sent to digital pot big endian
				if(flip_amp2_cur) flipper(&val, 0, NO_OF_DIGITAL_POT_BITS);
				if(flip_amp2_lvdt) flipper(&val, 8, NO_OF_DIGITAL_POT_BITS);
				if(flip_amp1_cur) flipper(&val, 16, NO_OF_DIGITAL_POT_BITS);
				if(flip_amp1_lvdt) flipper(&val, 24, NO_OF_DIGITAL_POT_BITS);
				//printf("After flipping, val = 0x%x\n", val); // For test

				val >>= (x*8); 
				//printf("\n\n\nval = 0x%x\n", val);
								
				for(y=0; y<NO_OF_DIGITAL_POT_BITS; y++)
					{
					if((val>>y) & 0x00000001) vtg.samp |= 0x02; else vtg.samp &= 0xfd; // Set cler mosi bit
					servo_amp_interface->np_piodata = vtg.samp;
					//printf("val = 0x%x, val>>y == 0x%x, vtg.samp = 0x%x\n", val, val>>y, vtg.samp); // For test
					TOGGLE_SCLK;
					}
				}
			DISABLE_SSN;
			//for(z=0; z<1000; z++) { }
			//TOGGLE_SCLK;
			
			
			//vtg.prev_pot_settings = DIGITAL_POTS_SETTINGS; // So we can detect changed and reload pots

			//tim2 = RD_PV_RAW_VEL_CTR;
			//printf("Time for digital pot transmission is %d uS\n", ((tim2-tim1)*60)/1000);
			}
		else if(action == LOAD_DEMOD_PHASE_SETTINGS)
			{
			// Demod settings are packed into variable SERVO_AMP_DEMOD_PHASE_SETTINGS.
			// Bits 0-7 are demod1, bits 8-15 are demod2 and bits 16-23 are the osc demod.
			// The osc demod is not used at this time.
			//printf("Loading demod settings!!!\n"); // For test
			lvdt_demod1_phase->np_piodata = SERVO_AMP_DEMOD_PHASE_SETTINGS;
			lvdt_demod2_phase->np_piodata = (SERVO_AMP_DEMOD_PHASE_SETTINGS >> 8);
			lvdt_osc_phase->np_piodata = (SERVO_AMP_DEMOD_PHASE_SETTINGS >> 16);

			//vtg.prev_demod_phase_settings = SERVO_AMP_DEMOD_PHASE_SETTINGS; // So we can detect changed and reload demod settings
			}
		else if(action == OPEN_VAL_LOOP1)
			{
			vtg.samp |= 0x08;
			servo_amp_interface->np_piodata = vtg.samp;
			}
		else if(action == OPEN_VAL_LOOP2)
			{
			vtg.samp |= 0x10;
			servo_amp_interface->np_piodata = vtg.samp;
			}
		else if(action == CLOSE_VAL_LOOP1)
			{
			vtg.samp &= 0xf7;
			servo_amp_interface->np_piodata = vtg.samp;
			}
		else if(action == CLOSE_VAL_LOOP2)
			{
			vtg.samp &= 0xef;
			servo_amp_interface->np_piodata = vtg.samp;
			}
		else if(action == OPEN_PRESSURE_FEEDBACK2)
			{
			vtg.samp |= 0x20;
			servo_amp_interface->np_piodata = vtg.samp;
			}
		else if(action == CLOSE_PRESSURE_FEEDBACK2)
			{
			vtg.samp &= 0xdf;
			servo_amp_interface->np_piodata = vtg.samp;
			}
		else if(action == INITIALIZE_SERVO_AMP)
			{
			//samp->ssn = 1;
			//samp->sclk = 1;
			//samp->open_val_loop1 = 0;
			//samp->open_val_loop2 = 0;

			//vtg.samp = 0x07;
			//vtg.samp = 0x06; // SCLK low initially
			vtg.samp = 0x26; // SCLK low initially, disconnect pressure feedback
			servo_amp_interface->np_piodata = vtg.samp;
			//printf("Initializing servo amp\n"); // For test
			out_dac(AXIS1_BOARD_LEVEL_LVDT_OFFSET_CHANNEL, BOARD_LEVEL_LVDT_OFFSET1_SETTING, VOLTAGE_X_1000, ABSOLUTE);
			out_dac(AXIS2_BOARD_LEVEL_LVDT_OFFSET_CHANNEL, BOARD_LEVEL_LVDT_OFFSET2_SETTING, VOLTAGE_X_1000, ABSOLUTE);
			}
		else if(action == LOAD_BOARD_LEVEL_LVDT_OFFSETS) 
			{
			//printf("Updating LVDT board level offset\n"); // For test
			out_dac(AXIS1_BOARD_LEVEL_LVDT_OFFSET_CHANNEL, BOARD_LEVEL_LVDT_OFFSET1_SETTING, VOLTAGE_X_1000, ABSOLUTE);
			out_dac(AXIS2_BOARD_LEVEL_LVDT_OFFSET_CHANNEL, BOARD_LEVEL_LVDT_OFFSET2_SETTING, VOLTAGE_X_1000, ABSOLUTE);
			}
		else return(99); // Bad action value
		}
	else return(98); // Not an integral digital servo amp
		
	return(0);
	}


// The variable parse function will look for changes in any of the variables concerning
// the digital servo amplifier and will call this function if necessary.  This elminates the
// necessity of having to have something in main always looking for changes.
int update_servo_amp_interface(void)
	{
	//printf("Here in update_servo_amp_interface(), AMP_TYPE = %d\n", AMP_TYPE);
	if(AMP_TYPE == ONE_AXIS_INTEGRAL_AMP || AMP_TYPE == TWO_AXIS_INTEGRAL_AMP)
		{
		//printf("About to call servo amp interface\n");
		servo_amp_interface(LOAD_DIGITAL_POTS);
		servo_amp_interface(LOAD_DEMOD_PHASE_SETTINGS);
		servo_amp_interface(LOAD_BOARD_LEVEL_LVDT_OFFSETS);
		}
	else return(99);

	return(0);
	}

#endif  // BUILD_FOR_CONTROL
/////////////////////////////////////////////////////////////////////////////


void load_ad_processor_program(void)
	{
	volatile unsigned short int *ad_mem_ptr, *flash_ptr;
	int x;

	ad_mem_ptr = na_ad_ram;
	//flash_ptr = (unsigned short int *)0x104100; // Start of ad program in flash.  The sram copy routine
												// begins at 0x10400.  That's not used in this case so
												// we'll begin copying at the actual program.

	vtw_flash_read_buffer(AD_PROG_FLASH_ADDR, (unsigned char *)ad_mem_ptr, na_ad_ram_size);
	
	//for(x=0; x<na_ad_ram_size; x++)
	//	{
	//	ad_mem_ptr[x] = flash_ptr[x];
	//	}

	AD_CMD1 = 0x9999; // Let ad processor know that its program has been loaded
	}


//void dump_lan91c111(void)
//	{
//	s_plugs_globals *g = &ng_plugs_globals;	
//	s_plugs_adapter_entry *a;	
//
//	//a = &g->adapter[adapter_index];
//	a = &g->adapter[0]; // Only one for now
//
//	nr_lan91c111_dump_registers(a->adapter_address, &a->adapter_storage);
//	}



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


////////////////////////// Ethernet I/O / MODBUS Functions //////////////////////////

#include "discrete_io.c"

//////////////////////////////////////////////////////////////////////


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
	//float f1=6.0022, f2=9.000987;
		
	//g.sp = tcp_send_byte;
	g.spc = &local_net_settings; // no context
	//g.rp = tcp_recv_byte;
	g.rpc = &local_net_settings; // no context
	//g.ip = plugs_init;
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
	
	#if nm_cpu_architecture == nios2
		// EPCS flash sector erase
		#if PROTECT_EPCS_AT_POWER_UP
			if (r_epcs_protect_region(epcs_protect_all)) printf("Unable to protect EPCS!!!\n");
		#else
			if (r_epcs_protect_region(epcs_protect_none)) printf("Unable to unprotect EPCS!!!\n");
		#endif
	#endif

	reload_vars_from_flash();
#if BUILD_FOR_CONTROL
	CONT_STATUS_WORD = 0;
	reload_cprog_from_flash();
	#if BUILD_FOR_INTERRUPT_CONTROL
		if (vtg.control_program[1][0] == 0 || vtg.control_program[1][0] > START_)
	#else
		if (vtg.control_program[1][0] == 0 || vtg.control_program[1][0] > END_)
	#endif
			{
			// No block #1, no control program in flash
			CLR_PROGRAM_LOADED;
			CLR_PROGRAM_RUNNING;
			}
		else SET_PROGRAM_LOADED;
#endif

// For test
#if 0
	for(x=0; x <10; x++)
		{
		printf("Block 47[%d] = %d\n", x, vtg.control_program[47][x]);
		if (x==9) printf("\n");
		}
	for(x=0; x <10; x++)
		{
		printf("Block 48[%d] = %d\n", x, vtg.control_program[48][x]);
		if (x==9) printf("\n");
		}
	for(x=0; x <10; x++)
		{
		printf("Block 49[%d] = %d\n", x, vtg.control_program[49][x]);
		if (x==9) printf("\n");
		}
	for(x=0; x <10; x++)
		{
		printf("Block 50[%d] = %d\n", x, vtg.control_program[50][x]);
		if (x==9) printf("\n");
		}
#endif 

ANALOG_DELAY = (unsigned short int)DELAY_BETWEEN_ANALOG_SAMPLES; // Set the delay between samples.

//printf("Before calling set_analog_mode()\n");
//for(x=0; x<24; x++) printf("ch%d setup = 0x%x\n",x+1, ad_setup[x]); 
set_analog_mode(ANALOG_MODE);
ANALOG_REPORTING=0;
O_SCOPE=0;

//printf("Before loading ad processor\n");
//for(x=0; x<24; x++) printf("ch%d setup = 0x%x\n",x+1, ad_setup[x]); 
load_ad_processor_program(); // Load ad processor

//nr_delay(100);
//printf("After loading AD processor\n");
//for(x=0; x<24; x++) printf("ch%d setup = 0x%x\n",x+1, ad_setup[x]); 

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

	// Clear the control interrupt pio and mask all of the control interrupt pio bits,
	// Even if monitor only.  We'll always do this.
	control_int->np_piointerruptmask = 0;
	cont_int_output->np_piodata = (vtg.cont_int_out_status = 0);
	
	// Initialize the DACs to 0V.  They will power up this way, however
	// force it here.  vtg.dac_word[] will be initialized.
	out_dac(1, 8192, RAW_VAL, ABSOLUTE); // 8192 == mid-scale == 0V, remember -10V - +10V DAC range.
	out_dac(2, 8192, RAW_VAL, ABSOLUTE);
	out_dac(3, 8192, RAW_VAL, ABSOLUTE);
	out_dac(4, 8192, RAW_VAL, ABSOLUTE);
	
	
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
	CLR_CS_DETECTED;
	CLR_CS_FROM_SS;  // The control program should clear this, however we'll do it here also.
	CLR_PAST_MIN_STK_LEN;
	
r_get_settings_from_flash();	
// Initialize TCP.
plugs_init();
// Test dump of lan91c111 registers
//dump_lan91c111();


nr_installuserisr(na_WD_rst_timer_irq, reset_watchdog, 0);
wd_rst_timer->np_timercontrol = 7; // Timer should reset to 500mS, we'll start this even in monitor only mode for op data
nr_installuserisr(na_o_scope_timer_irq, o_scope_isr, 0);

#if BUILD_FOR_CONTROL
//nr_installuserisr(na_WD_rst_timer_irq, reset_watchdog, 0);
//wd_rst_timer->np_timercontrol = 7; // Timer should reset to 500mS, we'll start this even in monitor only mode for op data
	nr_installuserisr(na_control_int_irq, control_supervisor, 0); // control interrupt
	nr_installuserisr(na_timer2_irq, timer2_isr, 0); 
	wd_timer->np_timercontrol = 4;
	if (PROGRAM_LOADED)	{reset_control_program(CLEAR_SAVED_WARNING);} // Start the control program
	//initialize_control(); // Elminate this after putting back above line
	//CLR_PROGRAM_RUNNING; // This also
#else
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

#if BUILD_FOR_CONTROL
update_servo_amp_interface();
servo_amp_interface(OPEN_PRESSURE_FEEDBACK2);
#endif

ONE_MS_COUNTER = 0;
//ENBL_PV_INT;

one_ms_timer->np_timercontrol = 7; // Start timer

print_again = nr_timer_milliseconds() + 1000; // For test

#if MON_DEBUG
// For test
printf("Local plug=%d, connected=%d, rem_ip_add=%d, rem_port=%d\n", 
		local_net_settings.plug, local_net_settings.connected, local_net_settings.remote_ip_address, local_net_settings.remote_port);

printf("Plant_wide plug=%d, connected=%d, rem_ip_add=%d, rem_port=%d\n", 
		plant_net_settings.plug, plant_net_settings.connected, plant_net_settings.remote_ip_address, plant_net_settings.remote_port);
#endif


//osw_ctr = 0; // for test

// For test, floating point test
//f1=0;
//for (x=1; x<11; x++)
//	{
//	f1=f1+(1/(1.5*x));
//	}
//f1 *= 1000000;
//printf("f1 = %d\n", (int)f1); // For test

//print_sg_plugs_global(); // For test
CLR_SHOT_IN_PROGRESS;
CLR_SHOT_IN_MOTION;


// For test
//printf("flashed ip address = 0x%x\n", *((unsigned int *)0x106024));
//printf("&local_net_settings=0x%x and &local_net_settings.plug = 0x%x\n",&local_net_settings, &local_net_settings.plug);

//r_get_settings_from_flash();
//show_net_settings();

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

// Reenable this for modbus support. 
//if(initialize_modbus()) printf("MODBUS device not available\n");


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

	if(cur_time>=mon_supervisor_time)
		{
		mon_supervisor_time=cur_time+MON_SUPERVISOR_TIME;

		//if(MASTER_ENABLE) monitor_supervisor();
		monitor_supervisor();
		}

	// Use this for any tests that require periodic intervals
	//if(cur_time>=test_time)
	//	{
	//	test_time=cur_time+TEST_TIME;

	//	ctr++;
	//	if(ctr&0x00000001) out_dac(1, 4000, VOLTAGE_X_1000, ABSOLUTE);
	//	else out_dac(1, 0, VOLTAGE_X_1000, ABSOLUTE);
	//	}

	
	//#if 0
	// Here we'll check for any change of states in ISW1. ISW1_BITS_TO_WATCH are the only
	// bits we care about.  If any of those bits change, we'll xmit ISW1's state.
	if(cur_time>=chk_ana_io_reporting_time)
		{
		//chk_ana_io_reporting_time+=CHK_ANA_IO_REPORTING_TIME;
		chk_ana_io_reporting_time=cur_time+CHK_ANA_IO_REPORTING_TIME;
		
		cur_isw1_bits = ISW1;
		cur_isw4_bits = (unsigned int)ISW4;
		//printf("SHOT_IN_PROGRESS = %d\n", SHOT_IN_PROGRESS);

		if (!SHOT_IN_PROGRESS) // First motion to end of timed collection
			{
			//printf("shot not in progress");
			if (shot_was_in_progress) // Skip any input changes for 1 loop after a shot
				{
				//printf("shot_was_in_progress = %d\n", shot_was_in_progress);
				shot_was_in_progress = 0;
				prev_isw1_bits = cur_isw1_bits;
				prev_isw4_bits = cur_isw4_bits;
				}
			else if ((cur_isw1_bits != prev_isw1_bits) || (cur_isw4_bits != cur_isw4_bits))
				{
			   //printf("*\n");
				isw1_bits_that_have_changed = cur_isw1_bits ^ prev_isw1_bits;
			   //printf("ISW1 bits that have changed = 0x%x\n", isw1_bits_that_have_changed);
				isw4_bits_that_have_changed = cur_isw4_bits ^ prev_isw4_bits;
				isw1_bits_that_have_set = isw1_bits_that_have_changed & cur_isw1_bits;
			   //printf("ISW1 bits that have set = 0x%x\n", isw1_bits_that_have_set);
				isw4_bits_that_have_set = isw4_bits_that_have_changed & cur_isw4_bits;
				isw1_bits_that_have_cleared = isw1_bits_that_have_changed & ~cur_isw1_bits;
			   //printf("ISW1 bits that have cleared = 0x%x\n\n", isw1_bits_that_have_cleared);
				isw4_bits_that_have_cleared = isw4_bits_that_have_changed & ~cur_isw4_bits;
				prev_isw1_bits = cur_isw1_bits;
				prev_isw4_bits = cur_isw4_bits;

				// Now look if any of the isw1 bits are for analog reporting
				if (!O_SCOPE) // Only if an o_scope operation is not currently running
					{
					if ((isw1_bits_that_have_set & ISW1_MASK_ANALOG_REPORT1) || (isw4_bits_that_have_set & ISW4_MASK_ANALOG_REPORT1))
						{
						if (SAMPLE_INTERVAL_REPORT1 == 0) {/*printf("Analog single report #1\n");*/ analog_reporting_single(1);} // Single sample
						else {/*printf("Analog block report #1\n");*/ start_o_scope_timer(ANALOG_REPORTING_MODE1);} // Block sample
						}

					if ((isw1_bits_that_have_set & ISW1_MASK_ANALOG_REPORT2) || (isw4_bits_that_have_set & ISW4_MASK_ANALOG_REPORT2))
						{
						if (SAMPLE_INTERVAL_REPORT2 == 0) {/*printf("Analog single report #2\n");*/ analog_reporting_single(2);} // Single sample
						else {/*printf("Analog block report #2\n");*/ start_o_scope_timer(ANALOG_REPORTING_MODE2);}	// Block sample
						}

					if ((isw1_bits_that_have_set & ISW1_MASK_ANALOG_REPORT3) || (isw4_bits_that_have_set & ISW4_MASK_ANALOG_REPORT3))
						{
						if (SAMPLE_INTERVAL_REPORT3 == 0) {/*printf("Analog single report #3\n");*/ analog_reporting_single(3);} // Single sample
						else {/*printf("Analog block report #3\n");*/ start_o_scope_timer(ANALOG_REPORTING_MODE3);}	// Block sample
						}
					}

				// Check and see if any bits outside of the bits to ignore have changed
				if ((ISW1_BITS_TO_WATCH & isw1_bits_that_have_changed) || (ISW4_BITS_TO_WATCH & isw4_bits_that_have_changed))
					{
					//printf("Bits to watch have changed\n");
					input_change_reporting(ISW1_BITS_TO_WATCH&isw1_bits_that_have_changed, ISW4_BITS_TO_WATCH&isw4_bits_that_have_changed);
					}
				}
			}
		else  // Shot is in progress, we won't worry about input changes.  Just keep updating the prev's
			{
			shot_was_in_progress = 1;
			prev_isw1_bits = cur_isw1_bits;
			prev_isw4_bits = cur_isw4_bits;
			}
		}
	//#endif

	#if BUILD_FOR_CONTROL
		if(cur_time>=look_for_at_home_time)
			{
			//look_for_at_home_time+=LOOK_FOR_AT_HOME_TIME;
			look_for_at_home_time=cur_time+LOOK_FOR_AT_HOME_TIME;

			if (look_for_at_home) // Are we even looking?
				{
				chk_discrete_input(AT_HOME_INPUT, &x);
				if (x)
					{
					look_for_at_home = 0;
					send_string((void *)&local_net_settings, "R_At home detected:27\n");
					}
				}
			}

		//if(cur_time>=service_samp_time)
		//	{
		//	service_samp_time+=SERVICE_SAMP_TIME;

		//	if (AMP_TYPE == ONE_AXIS_INTEGRAL_AMP || AMP_TYPE == TWO_AXIS_INTEGRAL_AMP)
		//		{
		//		if(DIGITAL_POTS_SETTINGS != vtg.prev_pot_settings) servo_amp_interface(LOAD_DIGITAL_POTS);
		//		if(SERVO_AMP_DEMOD_PHASE_SETTINGS != vtg.prev_demod_phase_settings) servo_amp_interface(LOAD_DEMOD_PHASE_SETTINGS);
		//		if(CONFIG_WORD2 & REQUEST_TO_OPEN_VAL_LOOP1) servo_amp_interface(OPEN_VAL_LOOP1); else servo_amp_interface(CLOSE_VAL_LOOP1);
		//		if(CONFIG_WORD2 & REQUEST_TO_OPEN_VAL_LOOP2) servo_amp_interface(OPEN_VAL_LOOP2); else servo_amp_interface(CLOSE_VAL_LOOP2);
		//		}
		//	}
	#endif

	if(cur_time>=chk_ethernet_io_time)
		{
		chk_ethernet_io_time=cur_time+CHK_ETHERNET_IO_TIME;
				
		//modbus_set_single_coil(io_bit, on_off, ACK);
		//io_bit++;
		//if(io_bit==8 && on_off==TURN_ON) {io_bit=0; on_off=TURN_OFF;}
		//else if(io_bit==8 && on_off==TURN_OFF)  {io_bit=0; on_off=TURN_ON;}
		//printf("I/O bit == %d, on_off = %d\n", io_bit, on_off);

#if 0 // Disable for now
		modbus_write_multiple_coils(0, io_word, ACK);
		io_bit++;
		if(io_bit==4) {io_bit=0; io_word=0;}
		else if(io_bit==1) io_word=0x55555555;
		else if(io_bit==2) io_word=0xaaaaaaaa;
		else if(io_bit==3) io_word=0xffffffff; 
#endif 
		}

///////////////////////////////////////////////////////////////////
	
	// Process any packets that may be present
#if BUILD_FOR_CONTROL
	if (!SHOT_IN_MOTION)
#endif
		{
		if(payload_length=readBuffer(&local_net_settings, payload))
			process_net_commands(&local_net_settings, payload, payload_length);
		if(payload_length=readBuffer(&plant_net_settings, payload))
			process_net_commands(&plant_net_settings, payload, payload_length);
		}

	if(payload_length=readBuffer(&io_net_settings, payload))
		{/*printf("!\n");*/ process_net_commands(&io_net_settings, payload, payload_length);}
	
	//if (SW7) xmit_fake_shots();
	if (MAKE_FAKE_SHOTS) xmit_fake_shots();
		
#if BUILD_FOR_CONTROL		
	if (MASTER_ENABLE) // Master enable is present) // Master enable is present
		{
		//printf("-M-"); // For test	
			
		#if BUILD_FOR_POLLED_CONTROL  
			//printf("-C-"); // For test
			if(PROGRAM_LOADED && PROGRAM_RUNNING) control_supervisor(0); 
		#endif

		if (master_enable_was_off)
			{
			master_enable_was_off=0;
			reset_control_program(CLEAR_SAVED_WARNING);
			//printf("Starting control after master enable dropped out\n"); // For test
			}
		}
	else // Master enable is not present. Disable control.
		{
		if (!master_enable_was_off)
			{
			master_enable_was_off=1;	
			stop_control();
			//printf("Stopping control, Master enable dropped out\n"); // For test
			}
		}
#endif
	} // while(1)		
}








