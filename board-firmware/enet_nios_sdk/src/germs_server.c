#include "excalibur.h"
#include "plugs.h"

#include "plugs_example_designs.h"


// +-----------------------------------------------
// | Various constants to control the monitor
// |

//#define BUILD_FOR_SERIAL 1
#define BUILD_FOR_ETHERNET 1


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
int udp_recv_callback
	(
        int plug_handle,
        void *context,
        ns_plugs_packet *p,
        void *payload,
        int payload_length
        );



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
	void * spc; // context for callback proc

	recv_proc rp; // callback proc to receive text
 	void * rpc; // context for callback proc

	init_proc ip;
	void * ipc;

	perd_proc t;	
	void * tc;

	uc *command; // holds the command we're parsing

	uc checksum; // used only for S-Records
	} s_c_germs_globals;

static s_c_germs_globals g; // bss inited to = {0};

#define RECV_BUFF_LENGTH 1024

typedef struct 
	{
	int plug_handle;
	char lastByte;
	int bNewByte;
	int bConnectFromHost;
	net_32 host_ip_address;
	net_16 host_port_number; 
	unsigned char c[RECV_BUFF_LENGTH]; // receive buffer, used to gather received udp bytes
	unsigned char *head_c;
	unsigned char *tail_c;
	int numBytes;
	} s_c_network_globals;
static s_c_network_globals gNetworkSettings; // bss inited to = {0};

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
int buffGetData(s_c_network_globals * context, char * value);
int readBuffer(s_c_network_globals *u, char *buff);
void loadBuffer(s_c_network_globals *u, char * payload, int payload_length);

int uart_send_byte(void *context, char *c);
int uart_recv_byte(void *context, char *c);
int null_function(void *context);

int udp_send_byte(void *context, char *c);
int udp_recv_byte(void *context, char *c);
int udp_init(void *context);

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
		return "germs_server";
	}


static int r_get_cpu_id(void)
	{
	asm("
		PFX	6
		RDCTL	%i0
	");
	}

static int r_get_data(char *data)
	{
	int num_chars=0;

	while((num_chars = g.rp(g.rpc, data)) <= 0)
		{
		;
		}
	return(num_chars);
	}

// | input a string. do nothing until <return> is pressed.

int r_input_string(char *input_string)
	{
	int char_position;
	int done;
	int c;
	int result;
	char tempBuffer[256];

	// | start by clearing the string
	tempBuffer[0] = 0;

	// receive chars until return
	done = 0;
	char_position = 0;
	result=0;


	 
	  while(!done)
	  {
 	     result=r_get_data(input_string);

	     
   	     
	     if(result<=2)
             {
		c=*input_string;
		if(c == 27) // ESC to cancel
			{
			tempBuffer[0] = 0;
			return -1;
			}
		if(c >= 0x20 && c <= 0x7e)
			{
			// | append one char, iff string is not full
			// |

			if(char_position < (k_input_string_length - 1))
				{
				sprintf(g.b,"%c",c);
				send_string(g.spc,g.b);
				tempBuffer[char_position++] = c;
				}
			}
		else if(c == 8) // backspace
		{
			if(char_position)
				{
				sprintf(g.b,"%c%c%c",8,' ',8);
				send_string(g.spc,g.b);
				char_position--;
				}
		}
		else if(c == 13) // carriage return == done
		{	
		  done = 1;
		  memcpy(input_string, tempBuffer, char_position);
		}
	     }
	     else
	     {
		int i;
		char *c=input_string;
		char_position=result-1;

		for(i=0;i<result;i++,c++)
		{
		  if(*c == 13)
		    char_position=i;
		}

  		done = 1;

	     }
	  }


	input_string[char_position] = 0;

	sprintf(g.b,"\r\n");
	send_string(g.spc,g.b);

	return 0;
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
		send_string(g.spc,"!");
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

		sprintf(g.b,addr_lo <= 0xffff ? "# %04X:" : "# %08X:",addr_lo);
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
		send_string(g.spc,g.b);

		addr_lo += word_count * 2;
		}
	}


int parse_germs_command(uc *command, send_proc callback, void *callback_context)
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
	if(command[0] == 0) // no command letter?
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
	
		case ':':
			do_colon_command();
			break;

		case '#':
			// comment character: do nothing
			break;

		// | c_germs extensions

		case 'v':
			do_show_info();
			break;

		case '?':
		case 'h':
			do_help();
			break;

		default:
		{
			send_string(g.spc,"?\n");
		}
		}

go_home:
	return result;
	}

// return 1 if is break character

int is_break_char(uc x)
	{
	if
		(
			(x == 0)
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

	while(!is_break_char(c = *w++))
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

	addr_lo = r_scan_string_for_number(wp,&break_char);
	if(break_char == '-')
		addr_hi = r_scan_string_for_number(wp,&break_char);
	else
		addr_hi = addr_lo;
	
	*break_char_out = break_char;
	*addr_lo_out = addr_lo;
	*addr_hi_out = addr_hi;
	}


int do_g_command(void)
	{
	uc **wp = &g.command;
	ui addr;
	uc break_char;

	addr = r_scan_string_for_number(wp,&break_char);
	sprintf(g.b,"# executing at %08x\n",addr);
	send_string(g.spc,g.b);
	r_asm_go(addr);


	return 0;
	}

int do_e_command(void)
	{
	uc **wp = &g.command;
	ui addr;
	uc break_char;

	addr = r_scan_string_for_number(wp,&break_char);
	sprintf(g.b,"# erasing: %08x\n",addr);
	send_string(g.spc,g.b);
	
#ifdef GM_FlashBase
	if(r_in_flash_range(addr))
		nr_flash_erase_sector((us *)GM_FlashBase,(us *)addr);
	else
#endif
		send_string(g.spc,"not flash\n");
	
	g.memory_position = addr; // (so a <return> shows what we just erased
	
	return 0;
	}

int do_r_command(void)
	{
	uc **wp = &g.command;
	ui addr_lo;
	ui addr_hi;
	uc break_char;

	r_scan_string_for_addr_range(wp,&break_char,&addr_lo,&addr_hi);
	if(addr_lo == addr_hi)
		addr_lo = 0;
	
	g.relocation_offset = addr_hi - addr_lo;
	
	sprintf(g.b,"# relocation offset: %08X\n",g.relocation_offset);
	send_string(g.spc,g.b);

	return 0;
	}

int do_m_command(void)
	{
	uc **wp = &g.command;
	uc break_char = 1;
	unsigned int addr_lo;
	unsigned int addr_hi;
	unsigned int v;
	int result = 0;

	r_scan_string_for_addr_range(wp,&break_char,&addr_lo,&addr_hi);

	if(break_char != ':') // anything other than colon after address means "show"
		{

		// | Range given? if not, use default range

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
	uc *w = g.command;
	uc **wp = &w;
	uc record_type;
	ui record_length;
	ui record_address;
	ui record_address_width; // in bytes: 2, 3, or 4
	int i;

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
				send_string(g.spc,"^"); // checksum error caret
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
				send_string(g.spc,"^"); // checksum error caret
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
	uc *w = g.command;
	uc **wp = &w;
	ui record_type;
	ui data_bytes_count;
	ui record_address;
	ui record_address_width; // in bytes: 2, 3, or 4
	int i;

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
				send_string(g.spc,"^"); // checksum error caret
			break;

		case 4: // upper address bits record:
			g.ihex_upper_address = r_fetch_multibyte(wp,2) << 16;
			break;

		} /* switch */

	return 0;
	}

int do_show_info(void)
	{
	send_string(g.spc,"\n\n");
	send_string(g.spc,"Welcome to GERMS Monitor for Nios\n");
	sprintf(g.b,"CPU ID: %04X\n",r_get_cpu_id());
	send_string(g.spc,g.b);

	sprintf(g.b,"System ID: %s\n",r_get_monitor_string());
	send_string(g.spc,g.b);

	send_string(g.spc,"\n\n");
	}

int do_help(void)
	{
	send_string(g.spc,"\n");
	send_string(g.spc,"# The following commands are available:\n");
	send_string(g.spc,"# g <addr>                  -- execute at addr\n");
	send_string(g.spc,"# e <addr>                  -- erase flash memory at addr\n");
	send_string(g.spc,"# r <addr1>-<addr2>         -- relocate S-records from addr1 to addr2\n");
	send_string(g.spc,"# m <addr>                  -- show memory at addr\n");
	send_string(g.spc,"# m <addr1>-<addr2          -- show memory range from addr1 to addr2\n");
	send_string(g.spc,"# m <addr>:<val1> <val2>... -- write halfwords at addr\n");
	send_string(g.spc,"# m <addr1>-<addr2>:<val>   -- fill range with halfword val\n");
	send_string(g.spc,"# s<S-record data>          -- write S-record to memory\n");
	send_string(g.spc,"# :<Intel hex data>         -- write Intel hex record to memory\n");
	send_string(g.spc,"# v                         -- show processor version\n");
	send_string(g.spc,"# help                      -- show this help\n");
	send_string(g.spc,"\n");
	}

int send_string(void *context, char *s)
	{
	int result = 0;
	char c;

	while((c = *s++) != 0)
		{
		if(c == '\n')
			c = '\r';
			g.sp(g.spc, &c);
		}
	
	result = 0;
	}

// lowest level serial routines...
int uart_send_byte(void *context, char *c)
{
	nr_uart_txchar(*c,0);
};

int uart_recv_byte(void *context, char *c)
{
	char a;

	a = nr_uart_rxchar(0);
	*c = a;
	return (a < 0) ? 0 : 1;
};

int null_function(void *context)
{
  return 0;
};


// ******************************ETHERNET STUFF BELOW HERE*******************************
// lowest level udp network routines
int udp_send_byte(void *context, char *c)
{
  nr_plugs_send_to(((s_c_network_globals *)context)->plug_handle, c, 1, 0, 
		   ((s_c_network_globals *)context)->host_ip_address, 
		   ((s_c_network_globals *)context)->host_port_number);
}

int udp_recv_byte(void *context, char *c)
{
  // remove a byte from the buffer...
  // no bytes in buffer, return "no bytes here"
  s_c_network_globals * u;
  int num=0;
  u = (s_c_network_globals *)context;

  nr_plugs_idle();

  num=readBuffer(u,c);

  return(num);
}


// setup the system for a connection using UDP on a given port
int udp_init(void *context)
{
  s_c_network_globals * u;
  u = (s_c_network_globals *)context;
  nr_plugs_initialize(0, 0, __adapter__,
			    __adapter_irq__,
			    __adapter_struct_addr__);

  nr_plugs_create(&(u->plug_handle), ne_plugs_udp, 20000, udp_recv_callback, u, 0);
  nr_plugs_connect(u->plug_handle, 0, -1, -1);

  u->tail_c=u->c;
  u->head_c=u->c;
  u->bConnectFromHost=0;


}

// check function
int udp_check(void *context)
{
  nr_plugs_idle();
}


// callback function
int udp_recv_callback
	(
        int plug_handle,
        void *context,
        ns_plugs_packet *p,
        void *payload,
        int payload_length
        )
{
	s_c_network_globals *u = (s_c_network_globals *)context;

	loadBuffer(u, (char*)payload, payload_length);
	 	 
        u->host_ip_address=((ns_plugs_ip_packet *)(p[ne_plugs_ip].header))->source_ip_address;
	u->host_port_number=((ns_plugs_udp_packet *)(p[ne_plugs_udp].header))->source_port;
	
//	u->bConnectFromHost=1;
	
};


// Pull one character off the circular buffer
// of received udp packets/bytes, or 0 for
// 'nothing there'
int readBuffer_oneChar(s_c_network_globals *u)
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

int readBuffer(s_c_network_globals *u, char *buff)
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

int loadBuffer_oneChar(s_c_network_globals *u, char c)
{
  *(u->head_c)++ = c;
  if(u->head_c == u->tail_c)
  {
    u->head_c = u->tail_c = u->c; // reset head & tail
    return -1; // failure code
  }

  if(u->head_c == ((u->c) + RECV_BUFF_LENGTH))
    u->head_c = u->c;

  return 0; // safe error code
  }

void loadBuffer(s_c_network_globals *u, char* payload, int payload_length)
{

  // write two bytes of length of "packet" into circular queue

  if ( loadBuffer_oneChar(u,(payload_length >> 8) & 0xff) < 0)
    return;

  if ( loadBuffer_oneChar(u,(payload_length) & 0xff) < 0)
    return;

   // write the payload contents after length,
   // ready to abort at any time (result < 0)

   while(payload_length-- > 0)
   {
     if ( loadBuffer_oneChar(u,*payload++) < 0)
       return;
   }
}


int main(void)
	{
	int result;
	char input_string[256];

#if BUILD_FOR_ETHERNET
	g.sp = udp_send_byte;
	g.spc = &gNetworkSettings; // no context
	g.rp = udp_recv_byte;
	g.rpc = &gNetworkSettings; // no context
	g.ip = udp_init;
	g.ipc = &gNetworkSettings;
	g.t = udp_check;
	g.tc = &gNetworkSettings;
#else
	g.sp = uart_send_byte;
	g.spc = 0; // no context
	g.rp = uart_recv_byte;
	g.rpc = 0; // no context 
	g.ip = null_function;
	g.ipc = 0;
	g.t = null_function;
	g.tc = 0;
#endif

	// call initialization function for both cases...
	g.ip(g.ipc);

	do_show_info();

	while(1)
		{
		send_string(g.spc,"+");

		result = r_input_string(input_string);
		if(result < 0)
			do_show_info();
		else
			result = parse_germs_command(input_string,send_string,0);
		}
}


