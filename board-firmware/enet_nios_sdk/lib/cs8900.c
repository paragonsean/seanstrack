// file: cs8900.c
// 
// This file implements the
// low level routines for an
// adapter that the "plugs"
// embedded tcp/ip stack can
// use.
//
// ex:set tabstop=4:

#include "nios.h"
#include "plugs.h"
#include "cs8900.h"

// |
// | Some types for talking to registers
// |

typedef unsigned char r8;
typedef unsigned short r16;
typedef unsigned long r32;

// +------------------------
// | Global storage allocation:
// | the size of one buffer.
// | We allocate it globally
// | (well, statically) so that
// | it's a fixed memory cost. It
// | used to be on the stack, but
// | that was obnoxious, because it
// | made sudden spikes on the
// | stack pointer. If you're doing
// | Ethernet, you need the buffer!
// |

	static r16 g_frame_buffer[768]; // 750 should be enough, 1500 byte max ethernet frame.

// +-------------------------
// | Here, a "frame" is the readable
// | bytes of an ethernet message. The
// | cs8900 data sheet defines a
// | "packet" as the whole ethernet
// | wire transmission, including
// | some link layer protocol that
// | we software folk can never see.
// | The "frame" is all the bytes,
// | including ethernet addresses,
// | type field, sometimes the CRC.
// |
// | (In the protocol stack code, we
// | just call any ol' bunch of bytes
// | a packet.)
// |

// +-----------------------------------------
// | Memory map of cs8900 peripheral
// | A completely mundane peripheral.
// |

typedef volatile struct
	{
	// +-------------------
	// | "I/O Mode" registers
	int np_cs8900iodata0; // First of Two adjacent half-words.
	int np_cs8900iodata1; // Second of Two adjacent half-words.
	int np_cs8900txcmd;    // w   16 bits
	int np_cs8900txlength; // w   16 bits
	int np_cs8900isq;      // r   16 bits interrupt status queue
	int np_cs8900ppptr;    // r/w 16 bits packet page pointer
	int np_cs8900ppdata0;  // r/w lower 16 bits of packet page data
	int np_cs8900ppdata1;  // r/w upper 16 bits of packet page data (optional)
	} np_cs8900;


// Offsets to registers
// All registers are r16 unless otherwise specified

enum
	{
	ne_cs8900_version =       0x0000, // r   chip revision info
	ne_cs8900_irq_number =    0x0022, // r/w which irq pin to use
	ne_cs8900_rf_count =      0x0050, // r   receive-frame byte counter
	ne_cs8900_rx_cfg =        0x0102, // r/w receiver config
	ne_cs8900_rx_ctl =        0x0104, // r/w receiver frame kinds
	ne_cs8900_tx_cfg =        0x0106, // r/w transmitter configuration
	ne_cs8900_tx_status =     0x0108, // r   transmitter status for recent command
	ne_cs8900_bf_cfg =        0x010a, // r/w buffer config
	ne_cs8900_line_ctl =      0x0112, // r/w phy layer misc
	ne_cs8900_self_ctl =      0x0114, // r/w misc settings
	ne_cs8900_bus_ctl =       0x0116, // r/w misc settings
	ne_cs8900_test_ctl =      0x0118, // r/w loopback, &c
	ne_cs8900_isq =           0x0120, // r   interrupt status queue
	ne_cs8900_rx_evt =        0x0124, // r   receiver event
	ne_cs8900_bf_evt =        0x012c, // r   buffer event
	ne_cs8900_rx_miss_count = 0x0130, // r   receiver frames-missed counter (cleared when read)
	ne_cs8900_tx_coll_count = 0x0132, // r   transmitter collision counter (cleared when read)
	ne_cs8900_line_status =   0x0134, // r   phy status
	ne_cs8900_self_status =   0x0136, // r   misc status
	ne_cs8900_bus_status =    0x0138, // r   misc status
	ne_cs8900_aui_refl =      0x013c, // r   some aui weird thing
	ne_cs8900_tx_cmd =        0x0144, // w   transmit command
	ne_cs8900_tx_length =     0x0146, // w   transmit length
	ne_cs8900_rx_addr_hash =  0x0150, // r/w 8 bytes of hash for mac addrs
	ne_cs8900_rx_addr =       0x0158, // r/w 6 byte mac addr of this node
	ne_cs8900_tx_evt =        0x0218, // r   transmitter event
	ne_cs8900_rx_status =     0x0400, // r   same as most recent rx_event
	ne_cs8900_rx_length =     0x0402, // r   length of recent received frame
	};

// Register numbers -- the low 6 bits of most regs, and used by isq
enum
	{
	ne_cs8900_rx_evt_reg = 4,
	ne_cs8900_tx_evt_reg = 8
	};

#define swap_words(_r) ( (((_r) & 0xffff0000) >> 16) | (((_r) & 0x0000ffff) << 16) )


r16 nr_cs8900_read_r16(np_cs8900 *e,int register_address)
	{
	e->np_cs8900ppptr = register_address;
	return e->np_cs8900ppdata0;
	}

unsigned int nr_cs8900_read_register(void *hardware_base_address,int register_number)
	{
	np_cs8900 *e = hardware_base_address;

	e->np_cs8900ppptr = register_number;
	return e->np_cs8900ppdata0;
	}

r32 nr_cs8900_read_r16x2(np_cs8900 *e,int register_address)
	{
	r32 result;

	e->np_cs8900ppptr = register_address;
	result  = (long)(0x0000ffff & e->np_cs8900ppdata0) << 16;
	e->np_cs8900ppptr = register_address + 2;
	result |= (0x0000ffff & e->np_cs8900ppdata0);

	return result;
	}

void nr_cs8900_write_r16(np_cs8900 *e,int register_address,r16 r)
	{
	e->np_cs8900ppptr = register_address;
	e->np_cs8900ppdata0 = r;
	}

// turn on any bits in r_set, and turn off any in r_clear. Setting takes precedence.
void nr_cs8900_set_r16(np_cs8900 *e,int register_address,r16 r_set,r16 r_clear)
	{
	r16 r;

	r = nr_cs8900_read_r16(e,register_address);
	r &= ~r_clear;
	r |= r_set;
	nr_cs8900_write_r16(e,register_address,r);
	}

// --------------------------------------
// Reset the chip to a usable state.
//
// This involves some pin manipulation,
// and then some register manipulation.

// Raise and lower the reset pin
// and then tickle the "byte high enable" bhe
// a few times.
//
// This seems to be the incantation needed.
// We are not accessing any cs8900 registers
// at first; we are using parts of the glue
// logic that tickles particular pins.
//

int nr_cs8900_reset
		(
		void *hardware_base_address,
		ns_plugs_adapter_storage *adapter_storage,
		ns_plugs_network_settings *s
		)
	{
	np_cs8900 *e = hardware_base_address;
	int i;

        volatile char * woggle;

        // The CS8900 chip has a... quirk.
        // You have to apply multiple logic-transitions to its
        // SBHE_n (byte-enable) pin to set it in the appropriate
        // communications-mode.  You have to do this before you 
        // can do thing #1 with it.  This sequence 
        // (a group of byte-writes) results in multiple edge-transitions
        // on the CS8900's byte-enable input--and is otherwise useless.
        //
        woggle = (char*) hardware_base_address;
        woggle[0] = 'w';
        woggle[1] = 'o';
        woggle[2] = 'g';
        woggle[3] = 'g';
        woggle[4] = 'l';
        woggle[5] = 'e';



	// +------------------------------------
	// | 1: set the mac address

		{
		host_32 u32;
		host_16 l16;
	
		u32 = nr_n2h32(s->ethernet_address.u32);
		l16 = nr_n2h16(s->ethernet_address.l16);
	
		nr_cs8900_write_r16(e,ne_cs8900_rx_addr+0,(u32 >> 16) & 0x0000ffff);
		nr_cs8900_write_r16(e,ne_cs8900_rx_addr+2,(u32 & 0x0000ffff));
		nr_cs8900_write_r16(e,ne_cs8900_rx_addr+4,l16);
		}
	
	
	// +-----------------------------------
	// | 2: set the chip to a friendly mode
	// | rx_ctl: What kinds of packets to accept
	// | rx_cfg: What kinds of events to notice and enqueue (hw irq is not on though)
	// | line_ctl: What features of the wire to enable

	nr_cs8900_set_r16(e,ne_cs8900_rx_ctl,0x0180,0); // promiscuous, good crc's only
	nr_cs8900_set_r16(e,ne_cs8900_rx_cfg,0x7100,0); // all err events, and good packets
	nr_cs8900_set_r16(e,ne_cs8900_line_ctl,0x00c0,0); // enable receiver & transmitter

	return 0;
	}

// --------------------------------------
// Set loopback mode: pass 0 for off,
// 1 for on.
// (reg 114: bit 12 enables control, bit 14 is on/off)

int nr_cs8900_set_loopback
		(
		void *hardware_base_address,
		ns_plugs_adapter_storage *adapter_storage,
		int loopback_onoff
		)
	{
	np_cs8900 *e = hardware_base_address;
	r16 r_set;	// bits to set
	r16 r_clear;    // bits to clear

	r_set = loopback_onoff ? 0x0200 : 0;
	r_clear = r_set ^ 0x0200;

	nr_cs8900_set_r16(e,ne_cs8900_test_ctl,r_set,r_clear);
	}
 
// --------------------------------------
// Set the LED manually: pass 0 for off,
// 1 for on, or -1 to let the chip do with
// it as it will.
// (reg 114: bit 12 enables control, bit 14 is on/off)

int nr_cs8900_set_led
		(
		void *hardware_base_address,
		ns_plugs_adapter_storage *adapter_storage,
		int led_onoff
		)
	{
	np_cs8900 *e = hardware_base_address;
	r16 r_set;	// bits to set
	r16 r_clear;	// bits to clear

	if(led_onoff > 0)
		{
		r_set = 0x5000; // control & on
		r_clear = 0;
		}
	else if(led_onoff == 0) // control & off
		{
		r_set = 0x1000;
		r_clear = 0x4000;
		}
	else			// control off & off
		{
		r_set = 0;
		r_clear = 0x5000;
		}
	
	nr_cs8900_set_r16(e,ne_cs8900_self_ctl,r_set,r_clear);

	return 0;
	}

// ----------------------------------------
// Turn on this chips promiscuous mode. Or off.


int nr_cs8900_set_promiscuous
		(
		void *hardware_base_address,
		ns_plugs_adapter_storage *adapter_storage,
		int promiscuous_onoff
		)
	{
	int r_set = 0;
	int r_clear = 0;

	if(promiscuous_onoff)
		r_set = 0x0080;
	else
		r_clear = 0x0080;

	// set or clear that bit
	nr_cs8900_set_r16(hardware_base_address,ne_cs8900_rx_ctl,r_set,r_clear);

	return 0;
	}

// -----------------------------------------
// Enable or disable interrupts for this adapter

int nr_cs8900_set_irq
		(
		void *hardware_base_address,
		ns_plugs_adapter_storage *adapter_storage,
		int irq_onoff)
	{
	np_cs8900 *e = hardware_base_address;

	if(irq_onoff)
		{
		nr_cs8900_write_r16(e,ne_cs8900_irq_number,0); // set to pin zero
		nr_cs8900_set_r16(e,ne_cs8900_bus_ctl,0x8000,0); // turn on EnableIRQ bit
		}
	else
		nr_cs8900_set_r16(e,ne_cs8900_bus_ctl,0,0x8000); // turn off EnableIRQ bit

	return 0;
	}

// ---------------------------------
// check for event:
// read the isq, and if it's something
// we understand, dispatch it.
// else ignore it. (The next read of
// the isq will perform an "implied
// skip", you're not supposed to read
// the isq until you're all done with
// the last thing it told you.
//
int nr_cs8900_check_for_events
		(
		void *hardware_base_address,
		ns_plugs_adapter_storage *adapter_storage,
		nr_plugs_adapter_dispatch_packet_proc proc,
		void *context
		)
	{
	np_cs8900 *e = hardware_base_address;
	r16 isq;
	r16 frame_length;
	r16 frame_length_r16;
	r16 *w;
	r16 rd;
	unsigned int i;
	int err;
	int watchdog = 50;	// read no more than this many packets

read_isq:
	isq = e->np_cs8900isq;

	switch(isq & 0x003f)	// which register is it reporting?
		{
		case ne_cs8900_rx_evt_reg:
			if(isq & 0x0100)	// rxOK flag: a frame, hooray
				{
				frame_length = nr_cs8900_read_r16(e,ne_cs8900_rx_length);
				frame_length_r16 = (frame_length + 1) / 2; // number of 32-bit words in frame
				w = g_frame_buffer;

				// |
				// | Here's where we might consider reading 802.3
				// | frames, by skipping over 8 bytes sometimes.
				// |

				for(i = 0; i < frame_length_r16; i ++)
					{
					rd = e->np_cs8900iodata0;
					*w++ = rd;
					}

				err = (proc)(g_frame_buffer,frame_length,context);
				}
			break;
		}
	
	// keep trying til isq comes up blank
	if(isq && watchdog-- > 0)
		goto read_isq;
	
	return watchdog ? -1 : 0;
	}

// The low-level transmit routine
//
// We follow the cs8900 transmission ettiquette here.
// return 0 for AOK, or -1 if we couldn't send for some reason
//

int nr_cs8900_tx_frame
		(
		void *hardware_base_address,
		ns_plugs_adapter_storage *adapter_storage,
		char *ethernet_frame,
		int frame_length
		)
	{
	np_cs8900 *e = hardware_base_address;
	r16 r;
	r16 rd;
	r16 *w;
	int frame_length_r16;
	int result = 0;
	long watch_dog = 0;

	// |
	// | Is the line plugged in?
	r = nr_cs8900_read_r16(e,ne_cs8900_line_status);
	if(!(r & 0x0080))
		{
#if PLUGS_DEBUG
		printf("   [cs8900] nr_cs8900_tx_frame: line is down\n");
#endif
		result = -1;
		goto goHome;
		}

	frame_length_r16 = (frame_length + 1) / 2;
	
	// |
	// | step 1: issue tx command

	e->np_cs8900txcmd = 0x00c9; // xmit only after delivering whole frame
	e->np_cs8900txlength = frame_length;


	// |
	// | step 2: wait for bid to be accepted or rejected
	// | (We assume an infallible cs8900 here... perhaps
	// | we should bail out of this after a few millions
	// | tries? Long enough for 1500 bytes or so to be
	// | sent. !!!)

check_bid:
	if(watch_dog++ == 10000000)
		{
#if PLUGS_DEBUG
		printf("   [cs8900] nr_cs8900_tx_frame: stuck waiting for Rdy4TxNow\n");
#endif
		result = -1;
		goto goHome;
		}

	r = nr_cs8900_read_r16(e,ne_cs8900_bus_status);
	if(r & 0x0080) // txbiderr
		{
		result = -1;
		goto goHome;
		}
	if(!(r & 0x0100))
		goto check_bid;

	// |
	// | step 3: write the data out

	w = (r16 *)ethernet_frame;
	while(frame_length_r16-- > 0)
          {
		rd = *w++;
		e->np_cs8900iodata0 = rd;
		}

goHome:
	return result;
	}



int nr_cs8900_dump_registers
		(
		void *hardware_base_address,
		ns_plugs_adapter_storage *adapter_storage
		)
	{
#if PLUGS_DEBUG
	np_cs8900 *e = hardware_base_address;
	printf("-------------------------\n");

#define dump_reg_16(_r) printf("%15s (%04xh) =         %04xh\n", \
		#_r,ne_cs8900_##_r, \
		nr_cs8900_read_r16(e,ne_cs8900_##_r) );
#define dump_reg_32(_r) printf("%15s (%04xh) =     %08xh\n", \
		#_r,ne_cs8900_##_r, \
		nr_cs8900_read_r16x2(e,ne_cs8900_##_r) );
#define dump_reg_48(_r) printf("%15s (%04xh) = %04x%04x%04xh\n", \
		#_r,ne_cs8900_##_r, \
		nr_cs8900_read_r16(e,ne_cs8900_##_r), \
		nr_cs8900_read_r16(e,ne_cs8900_##_r+2), \
		nr_cs8900_read_r16(e,ne_cs8900_##_r+4) );
	dump_reg_32(version);
	dump_reg_16(rf_count);
	dump_reg_16(rx_cfg);
	dump_reg_16(rx_ctl);
	dump_reg_16(tx_cfg);
	dump_reg_16(tx_status);
	dump_reg_16(bf_cfg);
	dump_reg_16(line_ctl);
	dump_reg_16(self_ctl);
	dump_reg_16(bus_ctl);
	dump_reg_16(test_ctl);
	dump_reg_16(isq);
	dump_reg_16(rx_evt);
	dump_reg_16(bf_evt);
	dump_reg_16(rx_miss_count);
	dump_reg_16(tx_coll_count);
	dump_reg_16(line_status);
	dump_reg_16(self_status);
	dump_reg_16(bus_status);
	dump_reg_16(aui_refl);
	dump_reg_16(tx_cmd);
	dump_reg_16(tx_length);
	dump_reg_16(rx_addr_hash);
	dump_reg_48(rx_addr);
	dump_reg_16(tx_evt);
	dump_reg_16(rx_status);
	dump_reg_16(rx_length);
#undef dump_reg
#endif
	return 0;
	}

ns_plugs_adapter_description ng_cs8900 =
	{
	&nr_cs8900_reset,
	&nr_cs8900_set_led,
	&nr_cs8900_set_loopback,
	&nr_cs8900_check_for_events,
	&nr_cs8900_tx_frame,
	&nr_cs8900_dump_registers,
	&nr_cs8900_set_promiscuous,
	&nr_cs8900_set_irq,
	"cs8900"
	};

// end of file
