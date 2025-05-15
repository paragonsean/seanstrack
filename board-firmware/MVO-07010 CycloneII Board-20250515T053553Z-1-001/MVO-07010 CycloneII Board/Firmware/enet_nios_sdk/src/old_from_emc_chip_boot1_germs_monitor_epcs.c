// File: "germs_monitor_epcs.c"
//
// 2002 May
//
// A C-language version of the Altera Germs Monitor
//
//	2002-11-11	dspitzer	- Modified r_get_line to accept LF and/or CR
//
// 12-12-06 Created an EPCS version of germs for VTW's Cyclone II based product.  This version also retains its
// old functionality if targeted for an Apex based system.

//#include "excalibur.h"
#include "nios.h"
#include "my_epcs.h"

//#include <string.h>
//#include <stdio.h>

// This program is compiled for location 0x300000
// nios2-build -b 0x300000 germs_monitor_epcs.c

// To create a hex file
// elf2hex -b 0x300000 -e 0x305000 -i germs_monitor_epcs.elf -o germs_monitor_epcs.hex -w 32


// Nios II / EPCS flash routines
//extern unsigned char nr_epcs_read_status ();
//extern unsigned long nr_epcs_lowest_protected_address();
//extern int nr_epcs_write_status (unsigned char value);
//extern int nr_epcs_protect_region (int bpcode);
//extern int nr_epcs_read_byte (unsigned long address, unsigned char *data);
//extern int nr_epcs_write_byte (unsigned long address, unsigned char data);
//extern int nr_epcs_erase_sector (unsigned long address);
//extern int nr_epcs_erase_bulk ();
//extern int nr_epcs_read_buffer (unsigned long address, int length, unsigned char *data);
//extern int nr_epcs_write_page (unsigned long address, int length, unsigned char *data);
//extern int r_epcs_write_buffer (unsigned long address, int length, unsigned char *data);
//extern int nr_epcs_address_past_config (unsigned long *addr);
//
//EPCS function error codes
//#define na_epcs_success                 0
//#define na_epcs_err_device_not_present  1
//#define na_epcs_err_device_not_ready    2
//#define na_epcs_err_timedout            3
//#define na_epcs_err_write_failed        4
//#define na_epcs_invalid_config          5

// +-----------------------------------------------
// | Various constants to control the monitor
// |


#if nm_cpu_architecture == nios2 // Cyclone II version of board with EPCS flash
	#define GM_FlashBase 0x0
	#define GM_FlashTop  0x200000
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


#define k_line_buffer_length 2048
#define k_default_show_range 64
#define k_words_per_line 4
#define k_word_size 4

typedef unsigned short us;


// +---------------------------
// | Handful of evil globals

typedef struct
{
	char b[k_line_buffer_length]; // general purpose buffer
	unsigned long memory_position; // location to display next
	unsigned long memory_range; // how much to show on next <return>
	unsigned long relocation_offset;
	unsigned long ihex_upper_address; // upper 16 bits of address for ihex (':') fmt data
	unsigned char checksum; // for S-records
    unsigned char even_byte;
	unsigned char ungotchar;		// used in r_get_char & r_unget_char
} globals;


static globals g ; //= {0};  // People hate it when you're polite.

// +--------------------------------
// | Local Prototypes

static int r_hex_char_to_value(char c);
static void r_show_range(unsigned int addr_lo,unsigned int addr_hi);
#ifdef GM_FlashBase
 static int r_in_flash_range(unsigned int addr);
#endif
static unsigned int r_fetch_next_hex_byte(char **wp);
static int r_stash_byte(unsigned int addr,unsigned char value);

static void r_scan_string_for_addr_range
		(
		char **wp,
		char *break_char_out,
		unsigned int *addr_lo_out,
		unsigned int *addr_hi_out
		);

static unsigned int r_scan_string_for_number(char **wp, char *break_char_out);

// Slight abstraction layer of communication port

int r_send_string(char *s);
int r_send_char(int c);
int r_get_line(char *s,int maxlen);
int r_get_char(void);

// Various GERMS commands

int do_g_command(char *command);
int do_e_command(char *command);
int do_r_command(char *command);
int do_m_command(char *command);
int do_s_command(char *command);
int do_colon_command(char *command);
int do_show_info(void);
int do_help(void);


#include "my_epcs.c"

// +--------------------------------
// | Local Little Routines

typedef void (*r_dull_proc_ptr)(void);

void r_goto(unsigned int addr)
{
  r_dull_proc_ptr p;
#if nm_cpu_architecture == nios_32
  // Nios function addresses are...funny, because the architect was...stupid.
  addr /= 2;
#endif
#ifdef nasys_timer_0
  nasys_timer_0->np_timercontrol = 0;   // turn off the lights on the way out...
#endif
  p = (r_dull_proc_ptr)(addr);
  (*p)();
}


// |
// | send and get character routines
// | uses default printf stuff
// |

int r_send_char(int c)
{
	nr_txchar(c);
	return 0;
}

int r_get_char(void)
{
	int c;

	if ( g.ungotchar != 0 )
	{
		c = g.ungotchar;
		g.ungotchar = 0;
	}
	else
		c = nr_rxchar();

	return c;
}

void r_unget_char( int c )
{
	g.ungotchar = c;
}

// |
// | return 0 if there's no character by the indicated number of milliseconds
// |

int r_get_char_until( int delay )
{
	int start = nr_timer_milliseconds();
	int c;

	do
	{
		if ( ( c = r_get_char() ) > 0 )
			return c;
	}
	while ( ( nr_timer_milliseconds() - start ) < delay );

	return 0;
}

int r_send_string(char *string)
{
	// Just to be pedantic, call sendchar over and over
	while(*string)
		r_send_char(*string++);
	return 0;
}

// |
// | Accumulate a line of text from the
// | input source (call r_get_char),
// | and return the length. ESC means abort, and
// | return length of -1.
// |

int r_get_line(char *string_out,int maxlen)
{
	int len = 0;
	int done = 0;
	int c;
	
	while(!done)
	{
		// Wait for a character
		while((c = r_get_char()) < 0)
			;	

		// Echo it
		r_send_char(c);
			
		// See if it is special
		if(c == 27) // ESC escape
		{
			len = -1; // error return: length of -1
			done = 1;
		}
		else if(c == 8) // Backspace ^H
		{
			if(len)
				len--;			
		}
		else if ( ( 13 == c ) || ( 10 == c ) ) // carriage return or line feed (end of line)
		{
			done = 1;

			c = r_get_char_until( 25 );
			if ( ( 13 != c ) && ( 10 != c ) )	// if the next character is a CR or LF, ignore it as a courtesy...
				r_unget_char( c );				// ...but if it's not, put it back for next time 
		}
		else if(c >= 0x20 && c <= 0x7e) // regular character
		{
			if(len < maxlen)
				string_out[len++] = c;
		}		
	}
	
	// done: set the string end
	// (And, as a courtesy, zero out the rest
	// of the string))
	if(len >= 0)
	{
		int i;
		for(i = len; i <= maxlen; i++)
			string_out[i] = 0;
	}

	return len;
}


static int r_mask_short(int x)
	{
		return x & 0x0000ffff;
	}

#ifdef GM_FlashBase

static int r_in_flash_range(unsigned int addr)
	{
	int result = 0;

	result = (addr >= (unsigned int)GM_FlashBase) && (addr < (unsigned int)GM_FlashTop);
	//if (result) r_send_string ("In flash!!!");

	return result;
	}

#endif

// | Write a byte to memory. Maybe do some special things for
// | special ranges, like flash.

static int r_stash_byte(unsigned int addr,unsigned char value)
	{
	volatile unsigned char *a;
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
                        // nonzero result means "badness."
			#if nm_cpu_architecture == nios2
				result = r_epcs_write_buffer((unsigned long)((addr-GM_FlashBase) & ~1), 2, (unsigned char *)&halfword);
				//sprintf(g.b,"epcs addr = 0x%x addr = 0x%x lpr = 0x%x rval = %d\n",addr-GM_FlashBase, addr, r_epcs_lowest_protected_address(), result);
				//r_send_string (g.b);
			#else
				result = nr_flash_write(GM_FlashBase,(us *)(addr & ~1), halfword);
			#endif
            if (result) 
                r_send_string ("%");
			}
		goto go_home;
		}
#endif

	a = (unsigned char *)addr;
	*a = value;

	if(*a != value)
		result = -1;
	
	// |
	// | print a bang if it didn't write
	// |

	if(result)
		r_send_string("!");
#ifdef GM_FlashBase
go_home:
#endif
	return result;
	}

static void r_show_range(unsigned int addr_lo,unsigned int addr_hi)
	{
	g.memory_range = addr_hi - addr_lo;
	g.memory_position = addr_hi;

	addr_hi &= ~1;
	addr_lo &= ~1;

	while( (((int)addr_hi) - ((int)addr_lo)) > 0 )
		{
		int word_count;
		int i;
		unsigned int v;

		sprintf(g.b,addr_lo <= 0xffff ? "# %04X:" : "# %08X:",addr_lo);
		word_count = (addr_hi - addr_lo) / k_word_size;
		if(word_count > k_words_per_line)
			word_count = k_words_per_line;

		for(i = 0; i < word_count; i++)
		{
			if (r_in_flash_range(addr_lo+i))
				{
				#if nm_cpu_architecture == nios2
					r_epcs_read_buffer ((unsigned long)((addr_lo-GM_FlashBase) + i * k_word_size), k_word_size, (unsigned char *)&v);
				#endif
				}
			else
				{
				v = *(unsigned long *)(addr_lo + i * k_word_size);
				}
			
			// |
			// | Some processors (like the ARM) can actually
			// | allow v to now contain a value outside the
			// | unsigned integer range!
			// |
			// | How can this be? Well, if there is an address
			// | exception, and the handler is unsophisticated,
			// | it seems to be possible.
			// |
			
			//v = r_mask_short(v);
			
			// | there.
	
			sprintf(g.b + strlen(g.b)," %08X",v);
		}

		for(i = word_count; i < k_words_per_line; i++)
			sprintf(g.b + strlen(g.b),"         ");

		sprintf(g.b + strlen(g.b)," # ");

		for(i = 0; i < word_count * k_words_per_line; i++)
			{
			char c;

			c = *(char *)(addr_lo + i);
			if(c < 0x20 || c > 0x7e)
				c = '.';
			sprintf(g.b + strlen(g.b),"%c",c);
			}

		sprintf(g.b + strlen(g.b),"\n");
		r_send_string(g.b);

		addr_lo += word_count * k_word_size;
		}
	}


int do_germs_command(char *command)
{
	int result = 0;
	char command_letter;

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

	switch(command_letter)
	{
		case 'g':
			do_g_command(command);
			break;
	
		case 'e':
			do_e_command(command);
			break;
	
		case 'r':
			do_r_command(command);
			break;
	
		case 'm':
			do_m_command(command);
			break;
	
		case 's':
			do_s_command(command);
			break;
	
		case ':':
			do_colon_command(command);
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
			r_send_string("(unknown command; try h for help)\n");
		}
	}

go_home:
	return result;
}

// return 1 if is break character

int is_break_char(char x)
	{
	int xx = x;
	if
		(
			(xx <= 0)
		||
			(xx >= 0x20 && xx < 0x2f)
		||
			(xx >= 0x3a && xx < 0x3e)
		||
			(xx > 0x7f)
		)
		return 1;

	return 0;
	}

static int r_hex_char_to_value(char c)
	{
	int value;

	value = c & 0x000f;
	if(c > '9')
		value += 9;
	
	return value;
	}

// +--------------------------------------------
// | read characters forward until either end of string,
// | or a non-alphanumeric-non-blank char. Return that
// | as the break character.

static unsigned int r_scan_string_for_number(char **wp, char *break_char_out)
	{
	char *w;
	char c;
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
static void r_scan_string_for_addr_range(char **wp,char *break_char_out,unsigned int *addr_lo_out,unsigned int *addr_hi_out)
	{
	char break_char;
	unsigned int addr_lo = 0;
	unsigned int addr_hi = 0;

	addr_lo = r_scan_string_for_number(wp,&break_char);
	if(break_char == '-')
		addr_hi = r_scan_string_for_number(wp,&break_char);
	else
		addr_hi = addr_lo;
	
	*break_char_out = break_char;
	*addr_lo_out = addr_lo;
	*addr_hi_out = addr_hi;
	}


int do_g_command(char *command)
{
	unsigned int addr;
	char break_char;

	addr = r_scan_string_for_number(&command,&break_char);
	#if nm_cpu_architecture == nios2
		if (addr < GM_FlashTop)
			{
			return 1; // Can't possibly execute code from EPCS flash, so we'll depart here
			}
	#endif
	
	sprintf(g.b,"# executing at %08x\n",addr);
	r_send_string(g.b);
	r_goto(addr);
	
	return 0;
}

int do_e_command(char *command)
{
	unsigned int addr;
	char break_char;
        char response [5];
        int do_entire_chip = 0;
        int result;

        if (command [0] == '!')
          {
            printf ("Erase Entire CHIP?  That takes forever!  Are you sure? (y/n)");
            result = r_get_line(response,1);

            if (response[0] == 'y')
              {
                printf ("I told you. This will take a while...");
                do_entire_chip = 1;
              }
            else
              printf ("Good choice.\n");
            command++;
          }
            
	addr = r_scan_string_for_number(&command,&break_char);
	sprintf(g.b,"# erasing: %08x\n",addr);
	r_send_string(g.b);
	
#ifdef GM_FlashBase
	if(r_in_flash_range(addr))
          {
            if (do_entire_chip == 1)
              {
				#if nm_cpu_architecture == nios2
					r_epcs_erase_bulk();
				#else
					nr_flash_erase (GM_FlashBase);
				#endif
              }
            else
              {
				#if nm_cpu_architecture == nios2
					r_epcs_erase_sector((unsigned long)addr-GM_FlashBase);
				#else
					nr_flash_erase_sector(GM_FlashBase, (void *) addr);
				#endif
              }
              printf ("Done.\n");
          }
        
	else
#endif
	r_send_string("not flash\n");
	
	g.memory_position = addr; // (so a <return> shows what we just erased
	return 0;
	}

int do_r_command(char *command)
	{
	unsigned int addr_lo;
	unsigned int addr_hi;
	char break_char;

	r_scan_string_for_addr_range(&command,&break_char,&addr_lo,&addr_hi);
	if(addr_lo == addr_hi)
		addr_lo = 0;
	
	g.relocation_offset = addr_hi - addr_lo;
	
	sprintf(g.b,"# relocation offset: %08X\n",(int)g.relocation_offset);
	r_send_string(g.b);

	return 0;
	}

int do_m_command(char *command)
	{
	char break_char = 1;
	unsigned int addr_lo;
	unsigned int addr_hi;
	unsigned int v;
	int result = 0;

	r_scan_string_for_addr_range(&command,&break_char,&addr_lo,&addr_hi);

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
		v = r_scan_string_for_number(&command,&break_char);

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
		v = r_scan_string_for_number(&command,&break_char);

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
static unsigned int r_fetch_next_hex_byte(char **wp)
	{
	char *w = *wp;
	unsigned int value = 0;

        if (*w == 0)
          printf ("null?");

	if(*w != 0)
		value = r_hex_char_to_value(*w++);
	if(*w != 0)
		value = value * 16 + r_hex_char_to_value(*w++);
	
	*wp = w;
	g.checksum += value;

	return value;
	}

static unsigned int r_fetch_multibyte(char **wp,int byte_count)
	{
	unsigned int value = 0;

	while(byte_count--)
		value = value * 256 + r_fetch_next_hex_byte(wp); // big endian
	
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

int do_s_command(char *command)
	{
	char record_type = 0 ;
	unsigned int record_length;
	unsigned int record_address;
	unsigned int record_address_width; // in bytes: 2, 3, or 4
	unsigned int i;

	g.checksum = 0; // reset the checksum

	if(*command)
		record_type = *command++;
	
	record_length = r_fetch_next_hex_byte(&command);
	
	switch (record_type)
		{
		case '1': // 16-bit-address data record
			record_address_width = 2;
read_s_record_data:
			record_address = r_fetch_multibyte(&command,record_address_width);
			record_length -= (record_address_width + 1); // no count addr or checksum
			for(i = 0; i < record_length; i++)
				{
				unsigned int value;

				value = r_fetch_next_hex_byte(&command);
				r_stash_byte(record_address + i + g.relocation_offset,value);
				}

			// lastly, checksum on S-Record. Should set g.checksum to 0xff.
			r_fetch_next_hex_byte(&command);
			if(g.checksum != 0xff)
                          {
                            
                               printf ("bad sum: %d^", g.checksum);
                            
				r_send_string(g.b); // checksum error
                                // caret
                          }
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
			record_address = r_fetch_multibyte(&command,record_address_width);
			// checksum
			r_fetch_next_hex_byte(&command);
			if(g.checksum != 0xff)
				r_send_string("^"); // checksum error caret
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

int do_colon_command(char *command)
	{
	unsigned int record_type;
	unsigned int data_bytes_count;
	unsigned int record_address;
	unsigned int i;
        unsigned char foo;

	g.checksum = 0; // reset the checksum

	data_bytes_count = r_fetch_next_hex_byte(&command);

	record_address = r_fetch_multibyte(&command,2);
	record_type = r_fetch_next_hex_byte(&command);

	switch(record_type)
		{
		case 0: // normal data read
			record_address += g.ihex_upper_address;


			for(i = 0; i < data_bytes_count; i++)
				{
				unsigned int value;

				value = r_fetch_next_hex_byte(&command);
				r_stash_byte(record_address + i + g.relocation_offset,value);
				}

			// lastly, checksum on I-Hex Record. Should set g.checksum to 0x00.
			foo = r_fetch_next_hex_byte(&command);
			if(g.checksum != 0x00)
                          {
                            printf ("bad sum: %X [%X]^", g.checksum, foo);
                                // caret
                            printf ("there were %X data-bytes in this line.\n", data_bytes_count);
                          }
			break;

		case 4: // upper address bits record:
			g.ihex_upper_address = r_fetch_multibyte(&command,2) << 16;
			break;

		} /* switch */

	return 0;
	}

int do_show_info(void)
	{
	r_send_string("\n\n");
	#if nm_cpu_architecture == nios2
		r_send_string("Welcome to GERMS Monitor with EPCS serial flash support \n");
	#else
		r_send_string("Welcome to GERMS Monitor\n");
	#endif
	sprintf(g.b,"CPU Architecture: %s\n",nm_cpu_architecture_string);
	r_send_string(g.b);

	sprintf(g.b,"System Name: %s\n",nm_system_name_string);
	r_send_string(g.b);

	sprintf(g.b,"System ID: %s\n",nm_monitor_string);
	r_send_string(g.b);

	r_send_string("\n\n");
	
	return 0;
	}

int do_help(void)
	{
	r_send_string("\n");
	r_send_string("# The following commands are available:\n");
	r_send_string("# g <addr>                  -- execute at addr\n");
	r_send_string("# e <addr>                  -- erase flash memory at addr.  \n                               Start address with '!' to erase entire flash\n");
	r_send_string("# r <addr1>-<addr2>         -- relocate S-records from addr1 to addr2\n");
	r_send_string("# m <addr>                  -- show memory at addr\n");
	r_send_string("# m <addr1>-<addr2          -- show memory range from addr1 to addr2\n");
	r_send_string("# m <addr>:<val1> <val2>... -- write halfwords at addr\n");
	r_send_string("# m <addr1>-<addr2>:<val>   -- fill range with halfword val\n");
	r_send_string("# s<S-record data>          -- write S-record to memory\n");
	r_send_string("# :<Intel hex data>         -- write Intel hex record to memory\n");
	r_send_string("# v                         -- show processor version\n");
	r_send_string("# help                      -- show this help\n");
	r_send_string("\n");
	return 0;
	}

int main(void)
	{
	int result;
	char command[k_line_buffer_length];

	g.ungotchar = 0;	// this one has to be intialized

	do_show_info();

	#if nm_cpu_architecture == nios2
		// Unprotect EPCS
		if (r_epcs_protect_region(epcs_protect_none)) r_send_string("Unable to unprotect EPCS!!!\n");
		else r_send_string("Unprotected EPCS!!!\n");
	#endif

	

	while(1)
		{
		r_send_string("+");

		result = r_get_line(command,k_line_buffer_length);
		if(result < 0)
			do_show_info();
		else
			result = do_germs_command(command);
		}
	}

// end of file
