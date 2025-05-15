//     sim_shot3.c

// 8-13-03  sim_shot1 -- A further stripped down version of sim_shot.

// This program acts as a TCP server.  It will accept a connection on one or both of 
// its two ports, 20000 and 20002.  A data set (shot) is manufactured and transmitted
// every CS_TIME in mS (4 seconds) to Visi-Trak Worldwide's plot program. 
// That's all there is to it, nothing else. 
// This was written in an attempt to isolate the Ethernet problems we have been having.

// A PC running Visi-Trak Worldwide's V5.74 plot program can interface to the board
// over a TCP link.  When the plot program's initial screen appears, click on Monitor and 
// then FasTrak II board setup.  Set up the IP address and port number.  The port number
// should be 20000.  Click on Monitor and then FasTrak II monitor to begin the monitor
// process.  There will be a "connected" or "not connected" message in the plot
// data window.  Once connected, the board will send a data set (shot) approx. every 
// 4 seconds.  The plot screen will update.

// After 5 - 10 "shots", the board locks up and no further data is sent.  We had this
// type of problem with earlier 2.?? versions of Nios as well although it appears to be
// more frequent and more severe with V3.02.

// This program will run on Altera's original Apex 20K200E based Nios development board
// configured with the "standard_lan91c111_32" design and a single lan91c111 Ethernet
// daughter board.  standard_lan91c111_32 was synthesized using SOPC builder 2.8 
// and Nios 3.02 in Quartus II V2.2 SP1.


// Spurious interrupts were disabled within the Nios editing screen within SOPC builder 2.8.
// After upgrading to Nios 3.02, spurious interrupts have been a problem.  Why?

// I can only create 2 plugs.  An attempt to create a third plug results in an 
// error -96, why? 

#include "nios.h"
#include "plugs.h"
#include "plugs_example_designs.h"


#define TCP_CHUNK_SIZE 1350  // Number of bytes in one "packet", changing this doesn't seem to make much difference
#define TCP_CHUNK_SIZE2 500
#define TCP_CHUNK_SIZE3 16
#define NUM_OF_TIME_BASED_POINTS 500
#define NUM_OF_DATA_SAMPLES 1010

// Define shot settings
#define SS_DIST 13		 // Inches
#define SHOT_END_POS 24  //  " " "
#define SS_VEL 10		 // Inches per second
#define FS_VEL 50		 //  "    "    "   "
#define CS_TIME 4000	 // Cycle time in mS
#define BISC_SIZE 1		 // Biscuit size inches

#define BUILD_FOR_ETHERNET 1

#ifndef NULL
 #define NULL 0
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
	//volatile unsigned char *head_c;
	//volatile unsigned char *tail_c;
	} s_tcp_status;

// bss inited to = {0};
static s_tcp_status local_net_settings, plant_net_settings, io_net_settings;


////////////////////// VTW Data Structures ////////////////////////////////
typedef struct
{
int tcp_xmit_in_progress;  // To prevent TCP transmit recursion
} vtw_globals;

static vtw_globals vtg;

									
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

combined_sample data_sample[NUM_OF_DATA_SAMPLES];


struct
	{
	unsigned int cycle_time;
	unsigned short int biscuit_size;
	short int eos_pos;
	} comp_shot_parameters;



// Local Prototypes
int send_string(void *context, char *s);
int tcp_listen_proc(int plug_handle, void *context, host_32 remote_ip_address, host_16 remote_port);
int tcp_init(void);
int tcp_proc(int plug_handle, void *context, ns_plugs_packet *p, void *payload, int payload_length);
int tcp_xmit(int plug_handle, unsigned char *data, int length);


// Visi-Trak Worldwide function prototypes
void transmit_binary_data(unsigned short int shot_num);
void transmit_headers(unsigned short int shot_num);
int whos_the_current_plug(void);




//////////////// Misc. little functions here ////////////////////////

static int r_get_cpu_id(void)
	{
	asm("
		PFX	6
		RDCTL	%i0
	");
	}



static char *r_get_monitor_string(void)
	{
	return "VTW Monitor and Control server";
	}


int do_show_info(void)
	{
	send_string(g.spc,"#Welcome to FasTrak II\n");
	sprintf(g.b,"#CPU ID: %04X\n",r_get_cpu_id());
	send_string(g.spc,g.b);

	sprintf(g.b,"#System ID: %s\n",r_get_monitor_string());
	send_string(g.spc,g.b);
	}




////////////////////////////////////////////////////////////////////////////



//////////////////////////// TCP stuff here ////////////////////////////////

// send_string() more for compatibility with Altera code.  
int send_string(void *context, char *s)
{
int result, sizeofstr;

	sizeofstr = strlen(s);
	
	//result = nr_plugs_send(((s_tcp_status *)context)->tcp_plug, s, sizeofstr, 0);
	result = tcp_xmit(((s_tcp_status *)context)->tcp_plug, s, sizeofstr);

	return (result); // If result == 999, tcp_xmit() was already in use, sorry charlie - no data sent.
}




// tcp_xmit will take a block of data and break it into TCP_CHUNK_SIZE sized chunks
// and transmit them using nr_plugs_send().  Any leftover "partial chunk" will be
// transmitted last.
int tcp_xmit(int plug_handle, unsigned char *data, int length)
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
				printf("Sending %d bytes\n", TCP_CHUNK_SIZE);
				if(return_val=nr_plugs_send(plug_handle, data, TCP_CHUNK_SIZE, 0)) goto ext;
				data += TCP_CHUNK_SIZE;
				length -= TCP_CHUNK_SIZE;
				continue;
				}
			else if (length >= TCP_CHUNK_SIZE2)
				{
				printf("Sending %d bytes\n", TCP_CHUNK_SIZE2);
				if(return_val=nr_plugs_send(plug_handle, data, TCP_CHUNK_SIZE2, 0)) goto ext;
				data += TCP_CHUNK_SIZE2;
				length -= TCP_CHUNK_SIZE2;
				continue;
				}
			else if (length >= TCP_CHUNK_SIZE3)
				{
				printf("Sending %d bytes\n", TCP_CHUNK_SIZE3);
				if(return_val=nr_plugs_send(plug_handle, data, TCP_CHUNK_SIZE3, 0)) goto ext;
				data += TCP_CHUNK_SIZE3;
				length -= TCP_CHUNK_SIZE3;
				continue;
				}
			else
				{
				printf("Sending %d bytes one at a time\n", length);
				for (y=0; y<length; y++)
					{
					if(return_val=nr_plugs_send(plug_handle, data++, 1, 0)) goto ext;
					}
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




#if 0
// tcp_xmit will take a block of data and break it into TCP_CHUNK_SIZE sized chunks
// and transmit them using nr_plugs_send().  Any leftover "partial chunk" will be
// transmitted last.
int tcp_xmit(int plug_handle, unsigned char *data, int length)
	{
	int num_of_chunks, leftover, y, return_val=0, x;
	volatile unsigned char *working_data;

	working_data = data;

	if (!vtg.tcp_xmit_in_progress) // Don't allow recursion on this routine
		{
		vtg.tcp_xmit_in_progress++;

		num_of_chunks = length / TCP_CHUNK_SIZE;
		leftover = length % TCP_CHUNK_SIZE;

		// Send chunks first
		if (num_of_chunks) // 1 or more chunks
			{
			//printf ("num of chunks = %d\n\n", num_of_chunks);
			for (y=0; y<num_of_chunks; y++)
				{
				//printf("Sending chunk %d\n", y);
				//nr_delay(150);
				if(return_val=nr_plugs_send(plug_handle, working_data, TCP_CHUNK_SIZE, 0)) goto ext;
				working_data+=TCP_CHUNK_SIZE;
				}
			}
	
		// Send any leftovers
		//nr_delay(150);
		#if 0
		if (!num_of_chunks) 
			{
			if (leftover) printf("Sending ONLY a leftover, %d bytes\n", leftover);
			else printf("No data to send, why are we here?\n");
			}
		else if (leftover) printf("Sending leftover now, %d bytes\n", leftover);
		else printf("No leftover\n");
		#endif

		#if 0
		if (leftover)
			{
			for (y=0; y<leftover; y++)
				{
				//printf("Sending chunk %d\n", y);
				//nr_delay(150);
				if(return_val=nr_plugs_send(plug_handle, working_data++, 1, 0)) goto ext;
				//working_data+=1;
				}

			//if(return_val=nr_plugs_send(plug_handle, working_data, leftover, 0)) goto ext;
			}
		#endif

		while (leftover)
			{
			if (leftover >= 500)
				{
				printf("Sending 500 byte leftover\n");
				if(return_val=nr_plugs_send(plug_handle, working_data, 500, 0)) goto ext;
				working_data += 500;
				leftover -= 500;
				continue;
				}
			else if (leftover >= 16)
				{
				printf("Sending 16 byte leftover\n");
				if(return_val=nr_plugs_send(plug_handle, working_data, 16, 0)) goto ext;
				working_data += 16;
				leftover -= 16;
				continue;
				}
			else
				{
				printf("Sending %d byte leftover\n", leftover);
				for (y=0; y<leftover; y++)
					{
					if(return_val=nr_plugs_send(plug_handle, working_data++, 1, 0)) goto ext;
					}
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

    
  	result = nr_plugs_create(&(local->tcp_plug), ne_plugs_tcp, 20000, tcp_proc, local, 0);
  	if (result == 0)
		{
		printf("create plug %d error %d\n",local->tcp_plug, result);
    	result = nr_plugs_listen(local->tcp_plug, tcp_listen_proc, local);
		}
  	else
		{
		printf("error creating plug %d\n", local->tcp_plug);
		}

  	if (result == 0) // No errors, keep going
	  	{	
  	  	result = nr_plugs_create(&(plant->tcp_plug), ne_plugs_tcp, 20002, tcp_proc, plant, 0);
  		if (result == 0)
			{
			printf("create plug %d error %d\n",plant->tcp_plug, result);
    		result = nr_plugs_listen(plant->tcp_plug, tcp_listen_proc, plant);
			}
  		else
			{
			printf("error creating plug %d\n", plant->tcp_plug);
			}
	  	}
#if 0	// Can' install the 3rd. plug, why???  We get an error -96 if attempted.
  	if (result == 0) // No errors, keep going
		{	
  		result = nr_plugs_create(&(io->tcp_plug), ne_plugs_tcp, 20004, tcp_proc, io, 0);
  		if (result == 0)
			{
			printf("create plug %d error %d\n",io->tcp_plug, result);
    		result = nr_plugs_listen(io->tcp_plug, tcp_listen_proc, io);
			}
  		else
			{
			printf("error creating plug, result = %d\n", result);
			}
		}
#endif
	
	// Blank out buffers	
 	local->c[0] = 0;
  	plant->c[0] = 0;
  	io->c[0] = 0;  

  	return (result);
}



// Our tcp_proc does nothing
int tcp_proc(int plug_handle,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length)
	{
	}


// This plugs callback routine is responsible
// for accepting or rejecting an incoming
// connection request to our tcp plug.
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
	send_string(g.spc,"+"); // Send prompt
	
	return 0; // "no error" == accept the connection
	}

///////////////////////////////////////////////////////////////////////////



////////////////////////////// VTW functions //////////////////////////////

void ram_clear(void)
	{
	int y;

	for (y=0; y<=NUM_OF_DATA_SAMPLES; y++)
		{
		data_sample[y].ana_ch1 = 0;
		data_sample[y].ana_ch2 = 0;
		data_sample[y].ana_ch3 = 0;
		data_sample[y].ana_ch4 = 0;
		data_sample[y].ana_ch5 = 0;
		data_sample[y].ana_ch6 = 0;
		data_sample[y].ana_ch7 = 0;
		data_sample[y].ana_ch8 = 0;
		data_sample[y].vel_count_q1 = 0;
		data_sample[y].vel_count_q2 = 0;
		data_sample[y].vel_count_q3 = 0;
		data_sample[y].vel_count_q4 = 0;
		data_sample[y].isw1 = 0;
		data_sample[y].isw4 = 0;
		data_sample[y].osw1 = 0;
		data_sample[y].one_ms_timer = 0;
		data_sample[y].position = 0;
		data_sample[y].sample_num = 0;
		}

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
	printf("Sending pos based header, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
	
	// Send position based binary data.
	printf("Sending pos data \n");
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, data_ptr, length);

	// Finish by sending ASCII position based END statment.
	sprintf(temp_str, "P_END_%d_%d_%d\n", shot_num, comp_shot_parameters.eos_pos, length);
	printf("Sending pos based trailer, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
	

	//////////////////// Then transmit time based data //////////////////////
	length = sizeof(combined_sample) * NUM_OF_TIME_BASED_POINTS;
	data_ptr = (char *)&data_sample[comp_shot_parameters.eos_pos+1];
	
	// Send ASCII header.
	sprintf(temp_str, "T_BEGIN_%d_%d\n", shot_num, length);
	printf("Sending time based header, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
		
	// Send time based binary data.
	printf("Sending time based data \n");
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, data_ptr, length);

	// Finish by sending ASCII time based END statment.
	sprintf(temp_str, "T_END_%d_%d_%d\n", shot_num, NUM_OF_TIME_BASED_POINTS, length);
	printf("Sending time based trailer, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));

	
	///////////////////// Transmit computed shot parameters /////////////////////
	// All computed shat data will be sent
	// regardless of dip switch settings.
	length = sizeof(comp_shot_parameters);
	data_ptr = (char *)&comp_shot_parameters;
	
	// Send ASCII header.
	sprintf(temp_str, "C_BEGIN_%d_%d\n", shot_num, length);
	printf("Sending csp header, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));

	// Send computed shot binary data.
	printf("Sending csp data \n");
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, data_ptr, length);

	// Finish by sending ASCII END statment.
	sprintf(temp_str, "C_END_%d_%d\n+", shot_num, length);
	printf("Sending csp trailer, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
	}



void transmit_headers(unsigned short int shot_num)
	{
	int length;
	char temp_str[81], *data_ptr;
	
		
	//////////////////////// Transmit position based data first /////////////////////////
	length = sizeof(combined_sample) * (comp_shot_parameters.eos_pos+1); // Take into account location [0]
	data_ptr = (char *)&data_sample[0]; // + sizeof(combined_sample); // Data begins at location [1] not [0].

	// Send an ASCII header string.
	sprintf(temp_str, "P_BEGIN_%d_%d\n", shot_num, length);
	printf("Sending pos based header, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
	
	// Finish by sending ASCII position based END statment.
	sprintf(temp_str, "P_END_%d_%d_%d\n", shot_num, comp_shot_parameters.eos_pos, length);
	printf("Sending pos based trailer, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
	

	//////////////////// Then transmit time based data //////////////////////
	length = sizeof(combined_sample) * NUM_OF_TIME_BASED_POINTS;
	data_ptr = (char *)&data_sample[comp_shot_parameters.eos_pos+1];
	
	// Send ASCII header.
	sprintf(temp_str, "T_BEGIN_%d_%d\n", shot_num, length);
	printf("Sending time based header, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
		
	// Finish by sending ASCII time based END statment.
	sprintf(temp_str, "T_END_%d_%d_%d\n", shot_num, NUM_OF_TIME_BASED_POINTS, length);
	printf("Sending time based trailer, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));

	
	///////////////////// Transmit computed shot parameters /////////////////////
	// All computed shat data will be sent
	// regardless of dip switch settings.
	length = sizeof(comp_shot_parameters);
	data_ptr = (char *)&comp_shot_parameters;
	
	// Send ASCII header.
	sprintf(temp_str, "C_BEGIN_%d_%d\n", shot_num, length);
	printf("Sending csp header, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));

	// Finish by sending ASCII END statment.
	sprintf(temp_str, "C_END_%d_%d\n+", shot_num, length);
	printf("Sending csp trailer, strlen(temp_str) = %d \n",strlen(temp_str));
	tcp_xmit(((s_tcp_status *)g.spc)->tcp_plug, temp_str, strlen(temp_str));
	}

/////////////////////////////////////////////////////////////////////////////////////////


int main(void)
	{
	int vel_increment, pos; 
	int temp_q1=4000, temp_q2=5000, temp_q3=6000, temp_q4=7000;
	int x, shot_num=1, ms=0;
	unsigned int xmit_again;
	
			
	g.spc = &local_net_settings; // no context
	g.rpc = &local_net_settings; // no context
	g.ipc = &local_net_settings;
	g.tc = &local_net_settings;


	// Initialize TCP.
	tcp_init();

	// Clear the shot data array
	ram_clear();



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
	comp_shot_parameters.cycle_time = CS_TIME;
	comp_shot_parameters.biscuit_size = BISC_SIZE*80;
	comp_shot_parameters.eos_pos = SHOT_END_POS*20;


// Loop to transmit data here.
wait: ; // Wait here if no connection
	if (!local_net_settings.connected && !plant_net_settings.connected);

	xmit_again = nr_timer_milliseconds() + CS_TIME;
	
	while (local_net_settings.connected || plant_net_settings.connected)
		{
		// Keep going as long as we have a connection

		while (nr_timer_milliseconds() < xmit_again);
		xmit_again += CS_TIME;
		
		if (local_net_settings.connected)
			{
			g.spc = &local_net_settings; // Set send_string() context	
			transmit_binary_data(shot_num);
			printf("SUCCESS!!! transmitted to local net, shot_num %d\n\n", shot_num);
			}
		if (plant_net_settings.connected)
			{
			g.spc = &plant_net_settings; // Set send_string() context	
			transmit_headers(shot_num);
			printf("SUCCESS!!! Transmitted to plant-wide net, shot_num %d\n\n", shot_num);
			}
	
		shot_num++;
		}

	goto wait;
	}








