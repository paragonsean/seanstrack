// file: plugs.c
//
// Implementation of the plugs
// tcp/ip stack for embedded systems
//
// Author: dvb, 2001-2002
//
// ex:set tabstop=4:
// ex:set expandtab:
// ex:set shiftwidth=4:

#include "plugs.h"
#include "plugs_private.h"


// +-----------------
// | The Globals

s_plugs_globals ng_plugs_globals;

// +------------------------
// | Debug bit 4 means show debug on system plugs
// |

#if (PLUGS_DEBUG & 4)
	#define nk_plugs_default_debug (ne_plugs_flag_debug_tx | ne_plugs_flag_debug_rx)
#else
	#define nk_plugs_default_debug (0)
#endif

// +------------------------
// | Local Prototypes

static void r_add_to_arp_cache_from_ip_packet
		(
		s_plugs_adapter_entry *a,
		ns_plugs_ethernet_packet *p
		);

static void r_add_to_arp_cache
		(
		s_plugs_adapter_entry *a,
		net_32 ip_address,
		net_32 ethu32,net_16 ethl16
		);

static int r_plugs_dns_proc
		(
		int plug_handle,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length
		);

static int r_tcp_dispatch
		(
		s_plugs_adapter_entry *a,
		int plug_handle,
		s_plugs_plug *plug,
		ns_plugs_packet *sub_protocol,
		void *payload,
		int payload_length
		);

static int r_tcp_send
		(
		int plug_handle,
		s_plugs_plug *plug,
		int flags,
		void *payload,
		long payload_length
		);

void r_tcp_plug_set_inert (s_plugs_plug* plug);


static int wellspring_of_sequence_numbers(void)
{
   static int seed;

   seed += 70000013;
   seed += nr_timer_milliseconds() * 1000219;

   return seed;
}
   

static s_plugs_plug *r_plug_from_handle(int plug_handle);

static void r_plugs_irq_handler(int context,int irq_number, int
                                interruptee_pc);

static void r_set_irqs_for_plug (s_plugs_plug* plug, int on_off);


// ---------------------------
// Byte-swapping subroutines

host_16 nr_n2h16(net_16 n)
	{
	return nm_n2h16(n);
	}

host_32 nr_n2h32(net_32 n)
	{
	return nm_n2h32(n);
	}



static s_plugs_plug *r_plug_from_handle(int plug_handle)
	{
	s_plugs_plug *plug;
	s_plugs_globals *g = &ng_plugs_globals;

	plug_handle -= nk_plugs_plug_number_base;
	plug = &g->plug[plug_handle];

	return plug;
	}

// --------------------
// r_calculate_checksum(base,byte_count,ip_header)
//
// Return complement of 1's-complement
// checksum of the specified range
// Return in network byte order.
//
// If ip_header pointer is included, add in
// the pseudo header as used by udp & tcp
// And possibly add in the (host-ordered)
// pseudo_length, too.
//

net_16 r_calculate_checksum
		(
		void *base,
		int byte_count,
		ns_plugs_ip_packet *ip_packet,
		host_16 pseudo_length
		)
	{
	net_16 *w = base;
	host_32 x;
	unsigned long result = 0;

	while(byte_count > 0)
		{
		x = nr_n2h16(*w++);
		if(byte_count == 1)
			x &= 0xff00;

		result += x;
		
		byte_count -= 2;
		}
	
	// | Add in udp/tcp pseudo-header, maybe
	if(ip_packet)
		{
		result += nr_n2h16(ip_packet->source_ip_address & 0x0000ffff);
		result += nr_n2h16(ip_packet->source_ip_address >> 16);
		result += nr_n2h16(ip_packet->destination_ip_address & 0x0000ffff);
		result += nr_n2h16(ip_packet->destination_ip_address >> 16);
		result += ip_packet->protocol;
		result += pseudo_length;
		}

	
	// | strangely, tcp/ip checksums require
	// | that the carry-bit be added back in.
	// | And, do it twice, to account for the case
	// | where adding the halves overflows. You
	// | only need to do it twice at most. Really.
	// |

	result = (result & 0xffff) + (result >> 16);
	result = (result & 0xffff) + (result >> 16);

	// |
	// | Never have checksum == 0
	// |

	if(result == 0)
		result = 0xffff;

	return ~(nr_h2n16((host_16)result));
	}

#if PLUGS_PING
// --------------------------------------------------------
// ICMP Handler
//
// This handler only responds to one thing: a ping (echo request)
//

#define k_plugs_ping_reply_size 1536

static int r_plugs_icmp_proc
		(
		int plug_handle,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length
		)
	{
	char response_b[k_plugs_ping_reply_size]; // (raw bytes) upper bound for ping we can reply to!
	ns_plugs_icmp_packet *icmpp = payload;
	ns_plugs_icmp_packet *response = (ns_plugs_icmp_packet *)response_b;
	if(icmpp->type == ne_plugs_icmp_ping_request)
		{
		char *ws,*wd;
		int i;
		ns_plugs_ip_packet *ipp;

		// | copy payload portion of ping request
		ws = payload;
		ws += 4;
		wd = response->payload;

		if(payload_length >= k_plugs_ping_reply_size)
			payload_length = k_plugs_ping_reply_size;

		for(i = payload_length - 4; i > 0; i--)
			*wd++ = *ws++;
			
		response->type = ne_plugs_icmp_ping_reply;
		response->code = 0;
		
		// | Send the almost-identical packet on out
		// | Use send_to to send it to the originator

		ipp = p[ne_plugs_ip].header;
		nr_plugs_send_to(plug_handle,response,payload_length,0,ipp->source_ip_address,0);
		}
	
	return 0;
	}
#endif // PLUGS_PING


// ---------------------------------------------------------

// Arp Handler
//
// When we get an arp "reply" packet,
// we'll save its information into the
// cache IF there is an entry for it already
// there.
// When we get an arp "request" packet,
// we'll reply to it if its about us.

int r_plugs_arp_proc
		(
		int plug_handle,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length
		)
	{
	ns_plugs_arp_packet *ap = payload;
	ns_plugs_arp_packet response;
	s_plugs_adapter_entry *a = context;

	switch(ap->op)
		{
		case nm_h2n16(1):		// arp request
			if(a->settings.ip_address == ap->target_ip_address)
				{
				// | Request for us!
				// | construct an answer and send it.

				response.hardware_type = nm_h2n16(1);
				response.protocol_type = nm_h2n16(0x0800);
				response.hardware_size = 6;		// 6 for ethernet
				response.protocol_size = 4;		// 4 for IP
				response.op = nm_h2n16(2);			// reply
				response.sender_ethernet_address = a->settings.ethernet_address;
				response.sender_ip_address = a->settings.ip_address;
				response.target_ethernet_address = ap->sender_ethernet_address;
				response.target_ip_address = ap->sender_ip_address;

				nr_plugs_send
						(
						plug_handle,
						&response,
						28,
						ne_plugs_flag_ethernet_broadcast
						);
				}
			break;

		case nm_h2n16(2):		// arp reply
			// |
			// | walk through our arp cache, and see if we want this reply
			// |
				{
				s_plugs_arp_cache_entry *w;
				int i;

				w = a->arp_cache;
				for(i = 0; i < nk_plugs_arp_cache_count; i++)
					{
					if(w->ip_address == ap->sender_ip_address)
						w->ethernet_address = ap->sender_ethernet_address;
					w++;
					}
				}
			break;
			} // switch
	return 0;
	}

// Nothing wrong with two plugs listening
// on the same port. No sir! Perfectly
// legal. But we wont let it happen
// accidentally.

#define nk_plugs_minimum_fresh_port_number 32769

net_16 get_port_number(s_plugs_globals *g)
	{
	int v;
	s_plugs_plug *plug;
	int i;

try_again:
	v = g->port_number++; // will be zero the very first time...
	if(v == 0)
		v = g->port_number = nk_plugs_minimum_fresh_port_number;
	
	v = nr_h2n16(v);
	
	plug = g->plug;
	for(i = 0; i < PLUGS_PLUG_COUNT; i++)
		if(v == (int)plug++->local_port)
			goto try_again;

	return v;
	}











// +---------------------------------------
// | This routine takes the index of an uninitialized
// | adapter. The "network settings" have already been
// | stashed in it, but only the Ethernet address is
// | relevant for now.
// |
// | This routine will initialize the adapter (low level),
// | and perform a dhcp lookup using it.
// |
// | If successful, it will fill in the dhcp-assigned
// | network settings in the adapter's settings, and
// | return zero-or-positive.
// |
// | If unsuccessful, will return a negative value
// |

static int r_plugs_adapter_dhcp(int adapter_index)
    {
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_adapter_entry *a = &g->adapter[adapter_index];
    ns_plugs_network_settings ns; // for dhcp routine to fill in...
    int result = 0;

    // |
    // | Step 1: initialize the adapter hardware
    // |

	result = (a->adapter_description->reset_proc)
			(
			a->adapter_address,
			&a->adapter_storage,
			&a->settings
			);
	if(result < 0)
		goto go_home;

#if PLUGS_IRQ
	if((a->adapter_irq != 0) && (a->adapter_description->set_irq_proc))
		{
		nr_installuserisr2(a->adapter_irq,r_plugs_irq_handler,(int)a);
		result = (a->adapter_description->set_irq_proc)
				(
				a->adapter_address,
				&a->adapter_storage,
				1
				);
        if(result < 0)
            goto go_home;
		}
#endif // PLUGS_IRQ

    // | Step 2: let the dhcp routine do all the hard work...

    result = r_plugs_dhcp(adapter_index,&ns);
    if(result < 0)
        goto go_home;

    // | Step 3: stash the settings received for just us

    a->settings = ns;

go_home:
    nr_plugs_print_error_message("[r_plugs_adapter]",result);
    return result;
    }









int nr_plugs_initialize
		(
		long flags,
		ns_plugs_network_settings *network_settings,
		void *adapter_address,
		int adapter_irq,
		ns_plugs_adapter_description *adapter_description
		)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	int result = 0;
	s_plugs_adapter_entry *a;
    int first_adapter = !(flags & ne_plugs_flag_add_adapter);
    int adapter_index;


	// +---------------------------------
	// | Initialize can be used to start up the plugs
	// | stack from scratch, or to add one more adapter
	// |

	if(first_adapter)
		{
		// +----------------------
		// | clear and reset everything
		// | Only the 1st adapter gets a DNS socket
		// |

		nr_zerorange((char *)g,sizeof(s_plugs_globals));

		// +---------------
		// | Mark the globals as Up And Running

		g->running = 1;
        }

    // +---------------------------------------
    // | Allocate the adapter spot, and bump
    // | the adapter_count. At the end, if there was any problem, we'll
    // | decrement the adapter_count and return and error

    adapter_index = g->adapter_count++;

	a = &g->adapter[adapter_index];
	nr_zerorange((char *)a,sizeof(s_plugs_adapter_entry));

	a->adapter_address = adapter_address;
	a->adapter_irq = adapter_irq;
	a->adapter_description = adapter_description;

	// +-----------------
	// | Install this adapter's network settings.
	// | If there's a flash, maybe get settings from flash
	// |

#ifdef nasys_plugs_persistent_network_settings

	// | 0 means use the stored settings in flash

	if((int)network_settings == 0)
		network_settings = &nasys_plugs_persistent_network_settings->settings;

#endif // nasys_plugs_persistent_network_settings

	a->settings = *network_settings;
    a->settings.flags |= flags;      // use the flags passed in, too.

	// +--------------------
	// | Install this adapter's arp proc
	// | Every adapter needs one of these, it is essential for IP
    // |
    // | Turns out to be ok to create the plug before the
    // | possible DHCP negotiation, or hardware initialization.

	result = nr_plugs_create
			(
			&a->arp_plug,
			ne_plugs_arp,
			1,
			r_plugs_arp_proc,
			a, // | use context to hold "our adapter".
			ne_plugs_flag_ethernet_broadcast + nk_plugs_default_debug + adapter_index
			);
    if(result < 0)
        goto go_home;

#if PLUGS_DHCP
    // +--------------------------------------
    // | If the dhcp flag is set (either in the network_settings
    // | flags or in the plugs_initialize flags) then call
    // | the dhcp proc to get network_settings.
    // | (if so, the settings still needs to start with a unique
    // | ethernet address; the rest is filled in by dhcp)
    // |

    if(a->settings.flags & ne_plugs_flag_dhcp)      // includes flags parameter already
        {
        result = r_plugs_adapter_dhcp(adapter_index);
        if(result < 0)
            goto go_home;
        }

#endif // PLUGS_DHCP

    // +----------------------------------------
    // | Initialize the adapter (possibly for a second time,
    // | if a dhcp negotiation occurred...
    // |

	result = (a->adapter_description->reset_proc)
			(
			a->adapter_address,
			&a->adapter_storage,
			&a->settings
			);
	if(result < 0)
		goto go_home;

#if PLUGS_IRQ
	// +--------------------
	// | If requested and available, install
	// | the irq handler
	// |

	if((a->adapter_irq != 0) && (a->adapter_description->set_irq_proc))
		{
		// |
		// | Install our handler
		// |

		nr_installuserisr2(a->adapter_irq,r_plugs_irq_handler,(int)a);

		// |
		// | Tell adapter to irq us
		// |

		result = (a->adapter_description->set_irq_proc)
				(
				a->adapter_address,
				&a->adapter_storage,
				1
				);
        if(result < 0)
            goto go_home;
		}
#endif // PLUGS_IRQ

#if PLUGS_DEBUG
	printf("   [plugs] +-----------------------\n");
	printf("   [plugs] | initialized adapter %s at 0x%08x, ",
			a->adapter_description->adapter_name,
			a->adapter_address);
	nr_plugs_print_ip_address(a->settings.ip_address);
	printf("\n");
#endif // PLUGS_DEBUG

    // +----------------------------
    // | For the first adapter only,
    // | (conditionalized code allowing),
    // | install a plug for doing DNS queries
    // |

#if PLUGS_DNS
    if(first_adapter)
        {
		// +------
		// | Install a DNS handler
		// |

		result = nr_plugs_create
				(
				&g->dns_plug,
				ne_plugs_udp,
				0,
				r_plugs_dns_proc,
				0,
				nk_plugs_default_debug);
        if(result < 0)
            goto go_home;

		result = nr_plugs_connect
				(
				g->dns_plug,
				0,
				nr_n2h32(a->settings.nameserver_ip_address),
				ne_plugs_udp_dns
				);
        if(result < 0)
            goto go_home;
		}
#endif // PLUGS_DNS


#if PLUGS_PING
	// +------
	// | Install this adapter's ICMP ping handler
	// |

	result = nr_plugs_create
			(
			&a->icmp_plug,
			ne_plugs_icmp,
			1,
			r_plugs_icmp_proc,
			0,
			nk_plugs_default_debug + adapter_index
			);
        if(result < 0)
            goto go_home;

	result = nr_plugs_connect
			(
			a->icmp_plug,
			0,
			-1,
			-1 // accept from anyone who sends to us
			);
        if(result < 0)
            goto go_home;
#endif // PLUGS_PING

go_home:
    if(result < 0)
        {
        // +-------------------------
        // | If there was any problem initializing this adapter,
        // | we roll it back out.
        // |

        // (if we had a "destroy adapter" loop, we would iterate
        // through every plug and destroy any associated with this
        // adapter index. but we dont, we do it explicitlike here.)

#if PLUGS_PING
        if(a->icmp_plug)
            {
            nr_plugs_destroy(a->icmp_plug);
            a->icmp_plug = 0;
            }
#endif

#if PLUGS_DNS
        if(first_adapter && g->dns_plug)
            {
            nr_plugs_destroy(g->dns_plug);
            g->dns_plug = 0;
            }
#endif

        if(a->arp_plug)
            {
            nr_plugs_destroy(a->arp_plug);
            a->arp_plug = 0;
            }

        // (reset the adapter only to disable its interrupts)
        // (if the description proc ptr was bad, we crashed
        // already and arent running here.)

	    (a->adapter_description->reset_proc)
			    (
			    a->adapter_address,
			    &a->adapter_storage,
			    &a->settings
			    );
        
        g->adapter_count--;
        }

	return result;
	}

int nr_plugs_terminate(void)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	int i;
	s_plugs_adapter_entry *a;

	// |
	// | Turn off interrupts right now
	// |

	a = g->adapter;
	for(i = 0; i < g->adapter_count; i++)
		{
		(a->adapter_description->set_irq_proc)
				(
				a->adapter_address,
				&a->adapter_storage,
				0
				);
		a++;
		}

	// |
	// | Delete each plug
	// |

	for(i = 0; i < PLUGS_PLUG_COUNT; i++)
		nr_plugs_destroy(i + nk_plugs_plug_number_base);

	// |
	// | and wipe the globals.
	// |
	nr_zerorange((char *)g,sizeof(s_plugs_globals));

	return 0;
	}

// +-----------------------------
// | nr_plugs_force_timeout is intended to allow a
// | user intervention for certain long synchronous
// | activities.
// |
// | dhcp lookup, for example, has a 60-second timeout,
// | which is a long time. If you get tired of waiting,
// | you can call nr_plugs_force_timeout to cut it short,
// | and the routine will return "failed".
// |
// | Has to be called from interrupt level, obviously, since
// | your foreground process is waiting for
// | nr_plugs_initialize to return.
// |
// | Presently (2003): only affects nr_plugs_initialize
// | if dhcp flag is set.
// |

int nr_plugs_force_timeout
        (
        void
        )
    {
    ng_plugs_globals.force_timeout = 1;
    return 0;
    }


int nr_plugs_get_settings
		(
		int adapter_index,
		ns_plugs_network_settings *network_settings_out
		)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_adapter_entry *a = &g->adapter[adapter_index];

	*network_settings_out = a->settings;
	return 0;
	}

int ethernet_address_equal(net_48 *ea1,net_48 *ea2)
	{
	return ea1->u32 == ea2->u32
			&& ea1->l16 == ea2->l16;
	}

//
// Accept ip match, or filter = -1 gets everything
int ip_address_equal(net_32 packet_ip_address,net_32 filter_ip_address)
	{
	return (filter_ip_address == 0xffffffff) || (packet_ip_address == filter_ip_address);
	}

//
// Accept ip match, or filter = -1 gets everything
int port_number_equal(net_16 packet_port,net_16 filter_port)
	{
	return (filter_port == 0xffff) || (packet_port == filter_port);
	}

// debug routine: get_protocol_name
#if PLUGS_DEBUG
char *get_protocol_name(int protocol)
	{
	switch(protocol)
		{
		case ne_plugs_ethernet:
					return "ethernet";
		case ne_plugs_arp:
					return "arp";
		case ne_plugs_ip:
					return "ip";
		case ne_plugs_icmp:
					return "icmp";
		case ne_plugs_udp:
					return "udp";
		case ne_plugs_tcp_raw:
		case ne_plugs_tcp:
					return "tcp";
		default:
					return "unknown";
		}
	}
#endif // PLUGS_DEBUG

// nr_plugs_dispatch_packet
//
// Examine an ethernet packet and find out what protocol
// it is. 
// Then loop through every plug and see which ones should
// receive this packet.
//
// This routine is called by an adapters polling proc.
//
int nr_plugs_dispatch_packet(void *pp,int packet_length,void *context)
	{
	ns_plugs_ethernet_packet *packet = pp;	// only one link_layer_type today.
	s_plugs_globals *g = &ng_plugs_globals;
	//int is_protocol[ne_plugs_last_protocol];
	ns_plugs_packet sub_protocol[ne_plugs_last_protocol];
	int i;
	s_plugs_plug *plug;
	int p;
	long plug_bitmask;
	int got_tcp = 0; // so only one tcp plug gets a tcp packet
	s_plugs_adapter_entry *a = context;
        

	// | since most interesting packets are ip-derivative,
	// | we figure out the sizes &c and put them in these vars
	// |

	ns_plugs_ip_packet *ip_packet = 0; // if incoming packet is ip or better...
	void *past_ip_packet = 0; // if incoming packet is ip or better
	int ip_payload_length = 0;

	// | Not running? go home now.
	// |

	if(!g->running)
		return ne_plugs_error_not_initialized; // | plugs not initialized

	g->in_dispatch++;

	// |
	// | Start with no protocol set

	for(i = 0; i < ne_plugs_last_protocol; i++)
		{
		sub_protocol[i].header = 0;
		sub_protocol[i].length = 0;
		}

	// |
	// | Every packet is "ethernet"

	sub_protocol[ne_plugs_ethernet].header = (void *)packet;
	sub_protocol[ne_plugs_ethernet].length = packet_length;

	// |
	// | Check progressive layers of protocol numbers

	p = nm_n2h16(packet->type);

	if(p == ne_plugs_ethernet_arp)
		{
		sub_protocol[ne_plugs_arp].header = &packet->payload;
		sub_protocol[ne_plugs_arp].length = 28;
		}
	else if(p == ne_plugs_ethernet_ip)
		{
		//
		// | It's an IP packet! but what kind?

		ip_packet = (ns_plugs_ip_packet *)packet->payload;

#if PLUGS_DEBUG
	if (ip_packet->flags_fragment & nm_h2n16(0xbfff))
		printf("   [plugs] | Ignored fragmented IP packet (0x%04x)\n",ip_packet->flags_fragment);
#endif // PLUGS_DEBUG

		if
				(
				((ip_packet->version_header_length & 0x00f0) == 0x0040)	// ipv4 only
				&&
				((ip_packet->flags_fragment & nm_h2n16(0xbfff)) == 0)	// ignore fragments!
				)
			{
			int ip_packet_length;
			void *ip_payload;

			// | stash the common ip packet info
			// |
	
			ip_packet_length = nr_n2h16(ip_packet->length);
			sub_protocol[ne_plugs_ip].header = (void *)ip_packet;
			sub_protocol[ne_plugs_ip].length = ip_packet_length;

			// temporarily put header-length into this ip_payload_length variable
			ip_payload_length = (ip_packet->version_header_length & 0x0f) * 4;
			past_ip_packet = (void *)((int)ip_packet + ip_payload_length);

			ip_payload = ((void *)ip_packet) + ip_payload_length;

			// And now put the actual payload length into it
			ip_payload_length = ip_packet_length - ip_payload_length;

			if(ip_packet->protocol == ne_plugs_ip_icmp)
				{
				sub_protocol[ne_plugs_icmp].header = ip_payload;
				sub_protocol[ne_plugs_icmp].length = ip_payload_length;
				}
			else if(ip_packet->protocol == ne_plugs_ip_udp)
				{
				sub_protocol[ne_plugs_udp].header = ip_payload;
				sub_protocol[ne_plugs_udp].length = ip_payload_length;
				}
			else if(ip_packet->protocol == ne_plugs_ip_tcp)
				{
				sub_protocol[ne_plugs_tcp_raw].header = ip_payload;
				sub_protocol[ne_plugs_tcp_raw].length = ip_payload_length;
				sub_protocol[ne_plugs_tcp].header = ip_payload;
				sub_protocol[ne_plugs_tcp].length = ip_payload_length;
				}
			}
		}
	
	// |
	// | The array sub_protocol now shows what protocol(s)
	// | this packet is.

	// | Loop through the plugs and call any
	// | plug that has a receiver-proc and wants this packet.
	// |

	plug_bitmask = 0;				// start with no plug-recipients
	got_tcp = 0; // at most one tcp plug gets each tcp packet

	plug = g->plug;
	for(i = 0; i < PLUGS_PLUG_COUNT; i++)
		{
		// |
		// | Each plug is associated with a particular adapter.
		// | We only look at plugs for the adapter that got
		// | this packet, else skip along.
		// |

		if(a != &g->adapter[plug->flags & ne_plugs_flag_adapter_index_mask])
			goto next_plug;


#if PLUGS_DEBUG
		if(plug->flags & ne_plugs_flag_debug_rx)
			{
			int j;

			// Dont print the "ignores" messages unless debug level 2
			if( sub_protocol[plug->protocol].header || (PLUGS_DEBUG & 2) )
				{
				for(j = ne_plugs_last_protocol - 1; j > 0; j--)
					if(sub_protocol[j].header)
						break;
				printf("   [plugs] +-----------------------\n");
				printf("   [plugs] | plug %d (%s): %s a %d byte %s packet\n",i,
					get_protocol_name(plug->protocol),
					sub_protocol[plug->protocol].header ? "examines" : "ignores",
					packet_length,
					get_protocol_name(j));
				}
			}
#endif // PLUGS_DEBUG
		if(sub_protocol[plug->protocol].header)
			{
			void *payload = 0;
			int payload_length = 0;
			int for_the_plug = 0;		// well check this later
			int ip_address_match;
			int ethernet_address_match;
			int port_match = 0;
			ns_plugs_udp_packet *udpp = 0; // set to packet for udp OR tcp, for port numbers

			// First things first:
			// Do we even care the slightest bit about
			// this ethernet packet?
			// The hardware may or may not already
			// be in promiscuous mode, but regardless,
			// if the packet gets to this routine, we
			// should verify that we want it.

			ethernet_address_match =
					((plug->flags & ne_plugs_flag_ethernet_all) != 0)
					||
						(
						((plug->flags & ne_plugs_flag_ethernet_broadcast) != 0)
						&&
						(packet->destination_address.u32 == 0xffffFFFF)
						&&
						(packet->destination_address.l16 == 0xffff)
						)
					||
						ethernet_address_equal
								(&packet->destination_address,
								&a->settings.ethernet_address
								);

			//
			// Since ip, icmp, udp, and tcp all rely on
			// matching source & destination ip addresses,
			// we check those once out here, if the
			// packet is ip flavored.

			ip_address_match =
					ethernet_address_match
					&&
                    (packet->type == nm_h2n16(ne_plugs_ethernet_ip))
                    &&
                        (
                            ((plug->flags & ne_plugs_flag_ip_all) != 0)
                            ||
                            (
					            ip_address_equal
							            (
							            ip_packet->destination_ip_address,
							            a->settings.ip_address
							            )
					            &&
					            ip_address_equal
							            (
                                        ip_packet->source_ip_address,
							            plug->remote_ip_address
                                        )
                            )
                        );

			//
			// Since udp & tcp rely on port matching, we'll do that here, too
			// udp & tcp have port info in same position (thankyou!)

			if(sub_protocol[ne_plugs_udp].header || sub_protocol[ne_plugs_tcp_raw].header)
				{
				// |
				// | Get port number from udp or tcp header -- first bytes
				// | are port numbers in either case, so cast as udp header.
				// |

				udpp = sub_protocol[ne_plugs_udp].header;
				if(!udpp)
					udpp = sub_protocol[ne_plugs_tcp_raw].header;
				port_match =
						ip_address_match
						&&
						port_number_equal
								(
								udpp->destination_port,
								plug->local_port
								)
						&&
						port_number_equal
								(
								udpp->source_port,
								plug->remote_port
								);
				}

#if PLUGS_DEBUG
			if(plug->flags & ne_plugs_flag_debug_rx)
                {
				printf("   [plugs] | ");
				if(sub_protocol[ne_plugs_udp].header || sub_protocol[ne_plugs_tcp_raw].header)
					printf("port_match = %d, ",port_match);
				if(sub_protocol[ne_plugs_ip].header)
					printf("ip_address_match = %d, ",ip_address_match);
				printf("ethernet_address_match = %d\n",ethernet_address_match);
				//if(sub_protocol[ne_plugs_ip].header && !ip_address_match)
					{
					printf("   [plugs] |      packet source ip address: ");
					nr_plugs_print_ip_address(ip_packet->source_ip_address);

                    if(udpp)
                        printf("/%d",nr_n2h16(udpp->source_port));

					printf("\n   [plugs] | packet destination ip address: ");
					nr_plugs_print_ip_address(ip_packet->destination_ip_address);

                    if(udpp)
                        printf("/%d",nr_n2h16(udpp->destination_port));

					printf("\n   [plugs] |              local ip address: ");
					nr_plugs_print_ip_address(a->settings.ip_address);
                    printf("/%d",nr_n2h16(plug->local_port));

					printf("\n   [plugs] |             remote ip address: ");
					nr_plugs_print_ip_address(plug->remote_ip_address);
                    printf("/%d",nr_n2h16(plug->remote_port));
					printf("\n");
					}
				//if(!ethernet_address_match)
					{
					printf("   [plugs] |      packet source ethernet address: ");
					nr_plugs_print_ethernet_address(&packet->source_address);
					printf("\n   [plugs] | packet destination ethernet address: ");
					nr_plugs_print_ethernet_address(&packet->destination_address);
					printf("\n   [plugs] |              local ethernet address: ");
					nr_plugs_print_ethernet_address(&a->settings.ethernet_address);
					printf("\n");
					}
				}

#endif // PLUGS_DEBUG
			//
			// The packet is a type this plug listens to.
			// Check out other filtration (ethernet address,
			// port numbers, &c.)

			switch(plug->protocol)
				{
				case ne_plugs_ethernet:		// Ethernet proc gets whole packet
					//
					// Filter by ethernet address

					if(ethernet_address_match)
						{
						payload = packet;
						payload_length = packet_length;
						for_the_plug = 1;
						}
					break;

				case ne_plugs_arp:		// ARP proc gets ARP header
					//
					// Filter by ethernet address

					if(ethernet_address_match)
						{
						payload = packet->payload;
						payload_length = sizeof(ns_plugs_arp_packet);
						for_the_plug = 1;
						}
					break;

				case ne_plugs_icmp:	// ICMP proc gets ICMP header, IP gets own payload
				case ne_plugs_ip:
					if(ip_address_match)
						{
						payload = past_ip_packet;
						payload_length = ip_payload_length;
						for_the_plug = 1;
						}
					break;

				case ne_plugs_udp:
					if(port_match)
						{
						payload = ((ns_plugs_udp_packet *)past_ip_packet)->payload;
						payload_length = ip_payload_length - sizeof(ns_plugs_udp_packet);
						for_the_plug = 1;
						}
					break;

				case ne_plugs_tcp_raw:
#if PLUGS_TCP
				case ne_plugs_tcp:
#endif // PLUGS_TCP
					if(port_match && !got_tcp)
						{
						payload = past_ip_packet;
						payload = (void *)((int)payload
								+ (nr_n2h16(((ns_plugs_tcp_packet *)past_ip_packet)->header_length_flags) >> 12) * 4);
						payload_length = nr_n2h16(ip_packet->length)
								- ((int)payload - (int)ip_packet);

#if PLUGS_TCP
						if(plug->protocol == ne_plugs_tcp)
							{
							// | Handle specially elsewhere

							int r;

							r = r_tcp_dispatch(
									a,
									i + nk_plugs_plug_number_base,
									plug,
									sub_protocol,
									payload,
									payload_length);

							// | only one tcp plug can get each
							// | tcp packet to us.
							if(!r)
								got_tcp = 1;
	
							}
						else
#endif // PLUGS_TCP
							{
							for_the_plug = 1;
							got_tcp = 1;
							}
			// The interaction between tcp and tcp_raw is
			// tricky. A tcp_raw plug *could* steal your
			// regular tcp packet, after all! best not to
			// mix them, you know?
						}
					break;

				} // switch

			//
			// If we've determined that this packet should
			// be received by this plug, then either
			// call the plug's callback proc right now,
			// or mark the packet to be received on demand
			// by the plug.

			if(for_the_plug)
				{
#if PLUGS_DEBUG
				if(plug->flags & ne_plugs_flag_debug_rx)
					{
					printf("   [plugs] | packet accepted for plug\n");
					nr_plugs_print_ethernet_packet(packet,packet_length,"   [plugs] | ");
					}
#endif // PLUGS_DEBUG

				// We know *some* plug wants it.
				// If it happens to be in IP packet
				// of any kind, let's just make sure
				// that we have the originator in
				// our ARP cache. Easy info!

				if(sub_protocol[ne_plugs_ip].header)
					r_add_to_arp_cache_from_ip_packet(a,packet);

				if(plug->callback_proc)
					{
#if PLUGS_DEBUG
					if(plug->flags & ne_plugs_flag_debug_rx)
						printf("   [plugs] | packet dispatched to callback proc %08x\n",
								plug->callback_proc);
#endif // PLUGS_DEBUG

                    // Check to see if an exclusive plug is set. If it
                    // is, only call callback routines for the exclusive plug
                    // and the APR plug.
                    // We must also always respond to ARP packets, otherwise,
                    // The sender might lose track of who we are.
                    if ((g->exclusive_plug_handle == 0) || 
                        (plug==r_plug_from_handle(g->exclusive_plug_handle)) ||
                        (plug == r_plug_from_handle (a->arp_plug)))
                        {
					    (plug->callback_proc)
							(i + nk_plugs_plug_number_base,
							plug->callback_context,
							sub_protocol,
							payload,
							payload_length);
                        }
					}
				else
					plug_bitmask |= 1 << i;
				}

#if PLUGS_DEBUG
			else
				{
				if((plug->flags & ne_plugs_flag_debug_rx)
						&& !got_tcp)
					{
					printf("   [plugs] | packet not sent to plug\n");
					}
				}
#endif // PLUGS_DEBUG

			} // packet protocol include plug protocol

	next_plug:
		plug++;
		} // for (i), each plug

#if PLUGS_TCP
	// |
	// | If it was a tcp packet to this IP address, and
	// | nobody handled it, reply with an incisive RST
	// |
	if(sub_protocol[ne_plugs_tcp].header
			&& ethernet_address_equal
				(
				&packet->destination_address,
				&a->settings.ethernet_address
				)
			&& ip_address_equal(ip_packet->destination_ip_address,a->settings.ip_address)
			&& !got_tcp)
		{
		int tcp_flags;
		ns_plugs_tcp_packet *tcp_packet;
		// |
		// | Make sure we have the ethernet address
		// | for this packet before we try to send
		// |

		tcp_packet = sub_protocol[ne_plugs_tcp].header;
		tcp_flags = nr_n2h16(tcp_packet->header_length_flags);

		if(!(tcp_flags & ne_plugs_flag_tcp_rst))
			{
			// |
			// | But, don't respond to rst with rst!
			// |
			r_add_to_arp_cache_from_ip_packet(a,packet);

                        // nr_plugs_print_ethernet_packet
                        //  (
                        //   packet,
                        //   packet_length,
                        //   "NO: "
                        //   );

			r_tcp_send(0,0,
					ne_plugs_flag_tcp_rst | ne_plugs_flag_tcp_ack,
					sub_protocol,0);
			}
		}
#endif // PLUGS_TCP

	// If any plug_bitmask, and queing enabled, queue the packet
go_home:

	g->in_dispatch--;

	return 0;
	}

int nr_plugs_create(int *plugHandleOut,
		int protocol,
		host_16 port,
		nr_plugs_receive_callback_proc callback,
		void *callback_context,
		long flags)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	int i;
	s_plugs_plug *plug;

	if((!g->running)
            || ( ((unsigned long)(flags & ne_plugs_flag_adapter_index_mask)) >= ((unsigned long)g->adapter_count)) )
		return ne_plugs_error_not_initialized; // uninitialized error

	port = nr_h2n16(port);

	// port number zero means "give me a fresh port number"
	if(port == 0)
		port = get_port_number(g);


	plug = g->plug;

	for(i = 0; i < PLUGS_PLUG_COUNT; i++)
		{
		if(plug->protocol == 0)
			{
			//
			// We found an unused plug, lets use it.
			// (??? how to mark it until its connected?)
			//

			// zero out all fields?

			plug->protocol = protocol;
			plug->local_port = port;
			plug->callback_proc = callback;
			plug->callback_context = callback_context;
			plug->flags = flags;

#if PLUGS_TCP
			plug->tcp_state = e_plugs_tcp_inert;
                        plug->tcp_manual_window_size = 1024;
                        plug->tcp_sn_local = wellspring_of_sequence_numbers();

                        // This statement amounts to: "the current 
                        //  sequence number hasn't been acknowledged yet."
                        plug->tcp_last_acknowledged_sn_local =
                          plug->tcp_sn_local - 0x100;
#endif // PLUGS_TCP

			*plugHandleOut = i + nk_plugs_plug_number_base;
	
			return 0;
			}

		plug++;
		}

	return ne_plugs_error_too_many_plugs;
	}

int nr_plugs_destroy(int plug_handle)
	{
	int result = 0;
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_plug *plug = r_plug_from_handle(plug_handle);

	// | First, we'll close the connection
	// | (Only does anything if it happens to
	// | be a tcp plug, and then only if tcp is
	// | even in the build.)

#if PLUGS_TCP
	result = nr_plugs_connect(plug_handle,0,0,0);
#endif

	// | "Destroying" a plug is easy: just
	// | mark it as unused, and everyone
	// | will ignore it.

	plug->protocol = 0;
	return result;
	}

// +-----------------------
// | nr_plugs_tcp_close
// | 
// | Graceful closure of TCP connection
// | 
// | We send a FIN.  We either do or don't wait for the other side to 
// | ACK, depending on how polite the caller wants to be.
// | 
nr_plugs_tcp_close (int plug_handle, unsigned int ack_wait_timeout)
{
#if PLUGS_TCP
  s_plugs_plug* plug;
  int result;
  unsigned long t_now;
  unsigned long t_start;

  plug = r_plug_from_handle(plug_handle);

  // If this isn't a TCP plug, or it wasn't connected in the first
  // place, then it was a short (and successful) trip.
  //
  if((plug->protocol != ne_plugs_tcp) ||
     (plug->tcp_state <= e_plugs_tcp_listening))
    {
      return 0;    // Consider it closed.
    }

  plug->tcp_state = e_plugs_tcp_disconnecting;

  // Send a FIN packet.
  //
  result = r_tcp_send (
                       plug_handle,
                       plug,
                       ne_plugs_flag_tcp_fin | ne_plugs_flag_tcp_ack,
                       0,
                       0
                       );

  if (result < 0) 
    return result;

  // We expect them to ACK the FIN we just sent them.
  //
  if (ack_wait_timeout > 0) {
    result = r_tcp_wait_for_ack (plug, ack_wait_timeout);
    if (result) 
      return result;
  }

  // Now we expect them to send a FIN of their own.
  // When they send the FIN, the TCP incoming-packet-handling
  // routine (r_tcp_dispatch) will see it and flip our state to 
  // "inert."  All we have to do is watch this variable.
  //
  t_start = nr_timer_milliseconds();
  t_now = t_start;

  while (t_now - t_start < ack_wait_timeout){
    nr_plugs_idle();
    if (plug->tcp_state != e_plugs_tcp_disconnecting) {
      // Plug went "inert" because  a FIN came in.
      return 0;
    }
    t_now = nr_timer_milliseconds();
  }

  // If we got to here: we never got our FIN from the other guy.
  return -1;
#else // PLUGS_TCP
  // this shouldn't be called when PLUGS_TCP is zero,
  // but we will return zero if it is
  return 0;
#endif // PLUGS_TCP
}


//
// nr_plugs_connect
//
// Some kinds of port _must_ be connected 
// before they can send or receive packets.

int nr_plugs_connect(int plug_handle,
		char *remote_name,
		host_32 remote_ip_address,
		host_16 remote_port)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_plug *plug;
	int result = 0;

	plug = r_plug_from_handle(plug_handle);

	if(remote_name)
		{
		result = nr_plugs_name_to_ip(remote_name,&remote_ip_address);
		if(result)
			goto go_home;
		remote_ip_address = nr_n2h32(remote_ip_address);
		}

#if PLUGS_TCP
	// |
	// | Any existing TCP connection?
	// | Close it. We do a partial
	// | close: we send a FIN and pretend
	// | we're completely done. When they
	// | send their ACK and FIN, we'll just
	// | slam them with a RST. This is
	// | impolite, but happens to be what
	// | Windows NT does, too, so I don't
	// | feel too bad, actually. -- dvb
	// |
	if((plug->protocol == ne_plugs_tcp)
			&& (plug->tcp_state > e_plugs_tcp_listening))
		{
                  // Use zero-timeout: don't wait for acknowledge.
                  printf ("Connect: already open.  Closing\n");
                  result = nr_plugs_tcp_close (plug_handle, 0);
                  if(result)
                    goto go_home;
		}
#endif // PLUGS_TCP

	plug->remote_ip_address = nr_h2n32(remote_ip_address);
	plug->remote_port = nm_h2n16(remote_port);

#if PLUGS_TCP
	// |
	// | Open a new connection?
	// | They may have said connect to 0.0.0.0/0,
	// | which just means "hang up and forget it."
	// | If not, well, initiate connection.
	// |

	if((plug->protocol == ne_plugs_tcp) && plug->remote_ip_address)
		{
		int retry_count;
		long t0,t;

		plug->tcp_sn_local = wellspring_of_sequence_numbers();

		for(retry_count = 0; retry_count < nk_plugs_tcp_retry_count; retry_count++)
			{
			t0 = nr_timer_milliseconds();

			result = r_tcp_send(plug_handle,plug,ne_plugs_flag_tcp_syn,0,0);
			if(result)
				goto go_home;
			plug->tcp_state = e_plugs_tcp_connecting;
                        printf ("\n[%08X]: Connecting... |", plug);


			while(nr_timer_milliseconds() - t0 < nk_plugs_tcp_retry_interval)
				{
				nr_plugs_idle();
				if(plug->tcp_state == e_plugs_tcp_connected)
                                   goto done_trying_to_connect;
				if(plug->tcp_state == e_plugs_tcp_inert)
					{
					result = ne_plugs_error_tcp_connection_refused;
                                        
					goto done_trying_to_connect;
					}
					
				}
			}
		result = ne_plugs_error_tcp_timed_out;
                printf ("\n[%08X]: Connection timed out. |");
                
done_trying_to_connect:
		;
		}
#endif // PLUGS_TCP


go_home:
	return result;
	}

// +--------
// | Bracketing the tcp-specific routines...

#if PLUGS_TCP

// +------------------------
// | nr_plugs_get_tcp_local_sequence_number
// |
// | For your plug (specified by-handle), gives 
// | the current local sequence number.  Zero if not connected.
// | 
unsigned long nr_plugs_get_tcp_local_sequence_number (int plug_handle)
{
  s_plugs_plug* plug = r_plug_from_handle (plug_handle);
  
  // Simple relevance check:
  if ((!plug) ||
       (plug->protocol != ne_plugs_tcp))
     return ne_plugs_error_not_initialized;

  if (plug->tcp_state < e_plugs_tcp_connected)
    return 0;
  
  return plug->tcp_sn_local;
}

// +------------------------
// | nr_plugs_get_tcp_last_acknowledged_sequence_number
// |
// | For your plug (specified by-handle), gives 
// | the last local sequence-number officially acknowledged
// | by the remote side.  This is the sequence-number of the 
// | last packet known to have been transmitted correctly.
// | 
unsigned long nr_plugs_get_tcp_last_acknowledged_sequence_number
(int plug_handle)
{
  s_plugs_plug* plug = r_plug_from_handle (plug_handle);
  
  // Simple relevance check:
  if ((!plug) ||
       (plug->protocol != ne_plugs_tcp))
     return ne_plugs_error_not_initialized;

  if (plug->tcp_state < e_plugs_tcp_connected)
    return 0;

  return plug->tcp_last_acknowledged_sn_local;
}

// +------------------------
// | nr_plugs_get_tcp_remote_window_size
// |
// | For your plug (specified by-handle), gives 
// | the last window-size officially reported
// | by the remote side.  
// | 
// | Negative result means: Plug no longer connected.
// | 
unsigned int nr_plugs_get_tcp_remote_window_size (int plug_handle)
{
  s_plugs_plug* plug = r_plug_from_handle (plug_handle);

  // Simple relevance check:
  if ((!plug) ||
       (plug->protocol != ne_plugs_tcp))
     return ne_plugs_error_not_initialized;

  // If the plug has gotten disconnected for some reason,
  if (plug->tcp_state < e_plugs_tcp_connected)
    return -1;

  return plug->tcp_remote_window_size;
}


// +------------------------
// | nr_plugs_tcp_rewind_local_sequence_number
// |
// | Call this to reset your plug's local sequence number
// | to a past value (which you provide, on the honor system).
// | 
// | You only need to do this if you're trying to re-send 
// | a packet that you have reason to believe failed in the 
// | past.
// | 
int nr_plugs_tcp_rewind_local_sequence_number(int plug_handle, 
                                              unsigned long old_sn_local)
{
  s_plugs_plug* plug = r_plug_from_handle (plug_handle);
  
  // Simple relevance check:
  if ((!plug) ||
       (plug->protocol != ne_plugs_tcp))
     return ne_plugs_error_not_initialized;

  plug->tcp_sn_local = old_sn_local;

  return 0;
}


// +------------------------
// | nr_plugs_wait_for_tcp_acknowledge
// |
// | Just a wrapper around r_tcp_wait_for_ack(),
// | but is more user-friendly because it takes a 
// | plug-handle instead of a plug.
// | 

int nr_plugs_wait_for_tcp_acknowledge 
(int plug_handle, 
 int timeout_milliseconds)
{
  s_plugs_plug* plug = r_plug_from_handle (plug_handle);
  return r_tcp_wait_for_ack (plug, timeout_milliseconds);
}

// +------------------------
// | r_tcp_wait_for_ack
// |
// | Sit around waiting for our most-recent TCP packet to be 
// | acknowledged.  
// |
// | Returns zero if the packet is acknowledged within the timeout
// | period--nonzero otherwise.
// |
// | Default timeout period is 100ms
// | 

int r_tcp_wait_for_ack
(s_plugs_plug* plug,
 unsigned int timeout_milliseconds)
{
  unsigned int t_start = nr_timer_milliseconds();
  unsigned int t_now;
  unsigned int gap;

  if (timeout_milliseconds <= 0)
    return 0;

  t_now = t_start;

  while ((t_now - t_start) < timeout_milliseconds) {
    nr_plugs_idle();
    if (plug->tcp_last_acknowledged_sn_local == plug->tcp_sn_local)
      return 0;          // Success!
    
    // While we're waiting around, someone might send us a RST,
    // which would result in our immediate disconnection.
    // That would happen in the r_tcp_dispatch routine that
    // copes with incoming packets. (during nr_plugs_idle, or IRQs)
    //
    if (plug->tcp_state < e_plugs_tcp_connected)
      return -1;
    
    t_now = nr_timer_milliseconds();
  }

  gap = ((unsigned int) plug->tcp_sn_local) - 
        ((unsigned int) plug->tcp_last_acknowledged_sn_local);

  printf ("  No ack.  local=%d.  Last-ack=%d.  Diff=%d\n",
          plug->tcp_sn_local, plug->tcp_last_acknowledged_sn_local,
          gap);
  
  return -1;  // Timeout.  No ack.  Return "NO" code.
}

// +---------------------------------------
// | Listen -- let a tcp port wait for a connection

int nr_plugs_listen(
		int plug_handle,
		nr_plugs_listen_callback_proc listen_proc,
		void *listen_proc_context
		)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_plug *plug;


	plug = r_plug_from_handle(plug_handle);


	if(plug->tcp_state != e_plugs_tcp_inert)
		nr_plugs_connect(plug_handle,0,0,0);

		//return ne_plugs_error;
	
	plug->tcp_listen_proc = listen_proc;
	plug->tcp_listen_proc_context = listen_proc_context;
	plug->tcp_state = e_plugs_tcp_listening;

	plug->remote_ip_address = -1;
	plug->remote_port = -1; // accept from anyone...

	return 0;
	}


int nr_plugs_tcp_ready_for_next (int plug_handle, int window_size)
{
   s_plugs_plug *plug;

   plug = r_plug_from_handle(plug_handle);

   if (!plug)  
      return ne_plugs_error_not_initialized;

   if (!plug->flags & ne_plugs_flag_tcp_manual_flow)
      return ne_plugs_error_not_initialized;

   plug->tcp_manual_window_size = window_size;
   return r_tcp_send(plug_handle, 
                     plug, 
                     ne_plugs_flag_tcp_ack,
                     0,
                     0
                     );

}



// +--------------------------------------------
// | Utility routine for composing and sending
// | tcp packets. Mostly it can figure out
// | what to do just from the flags. 
// | 
// | Exceptions:
// |     if plug is 0, then payload is actually the
// |     sub_protocol array.
// |

int r_tcp_send(int plug_handle,s_plugs_plug *plug,int tcp_flags,void *payload,long payload_length)
	{
	int result;

	unsigned char x[1500];
	ns_plugs_tcp_packet *tcp_reply; // reply
	net_32 destination_ip_address;
	int send_to_flags = plug->flags & ne_plugs_flag_adapter_index_mask;

        //r_set_irqs_for_plug (plug, 0);
	tcp_reply = (ns_plugs_tcp_packet *)x;
	nr_zerorange((char *)tcp_reply,sizeof(ns_plugs_tcp_packet));

	if(!plug)
		{
		// |
		// | If plug is 0, the *only* thing we can
		// | do is send an RST to the sender, presumed
		// | to be the source of an ip_packet overloading
		// | the input parameter "payload"
		// |

		ns_plugs_packet *sub_protocol = payload;
		ns_plugs_ip_packet *ipp = sub_protocol[ne_plugs_ip].header; // what we reply to
		ns_plugs_tcp_packet *tcpp = sub_protocol[ne_plugs_tcp].header;

		destination_ip_address = ipp->source_ip_address;

		tcp_reply->source_port = tcpp->destination_port;
		tcp_reply->destination_port = tcpp->source_port;
		tcp_reply->sequence_number = tcpp->acknowledgement_number;
		tcp_reply->acknowledgement_number = nr_h2n32(nr_n2h32(tcpp->sequence_number) + 1);

		tcp_reply->header_length_flags =
				nr_h2n16(0x5000 | ne_plugs_flag_tcp_rst | ne_plugs_flag_tcp_ack);
		payload = 0;
		payload_length = 0;
		send_to_flags = nk_plugs_default_debug;
		}
	else
		{
		destination_ip_address = plug->remote_ip_address;

		tcp_reply->source_port = plug->local_port;
		tcp_reply->destination_port = plug->remote_port;


                if ((plug->flags & ne_plugs_flag_tcp_manual_flow) &&
                    (plug->tcp_state >= e_plugs_tcp_connected)    )
                  tcp_reply->window_size = nm_h2n16(plug->tcp_manual_window_size);	
                else
                  tcp_reply->window_size = nm_h2n16(1024);	

		// | Always send ack & seq numbers...

		tcp_reply->acknowledgement_number = nr_h2n32(plug->tcp_sn_remote);
		tcp_reply->sequence_number = nr_h2n32(plug->tcp_sn_local);
		tcp_reply->header_length_flags =
				nr_h2n16(0x5000 | (tcp_flags & ne_plugs_flag_tcp_mask));

		// | If SYN or FIN, bump our local seq by 1,
		// | else, if PSH, put payload bump local seq by payload size

		if(tcp_flags & (ne_plugs_flag_tcp_syn | ne_plugs_flag_tcp_fin))
			plug->tcp_sn_local++;
		else if(tcp_flags & ne_plugs_flag_tcp_psh)
			{
			// |
			// | Copy (byte by byte) the payload)
			// |
	
			int i;
	
			plug->tcp_sn_local += payload_length;
			for(i = 0; i < payload_length; i++)
				tcp_reply->payload[i] = ((char *)payload)[i];
			}
	
		if(tcp_flags & ne_plugs_flag_tcp_rst) // reset? window is zero, thank you.
			tcp_reply->window_size = 0;	
		}
	
	// |
	// | Use the overload-feature of nr_plugs_send, and treat
	// | our carefully constructed packet as an ip packet,
	// | since we're giving it the payload already all
	// | put together.
	// |

	// | (payload_length now includes tcp header + tcp payload)

	payload_length += sizeof(ns_plugs_tcp_packet);
	result = nr_plugs_send_to(ne_plugs_ip,tcp_reply,payload_length,
			ne_plugs_flag_tcp_checksum | send_to_flags,
			destination_ip_address,tcp_reply->destination_port);
	
        // r_set_irqs_for_plug (plug, 1);
	return result;
	}

// +-----------------------------------------
// | Routine for handling non-raw tcp connections
// | Return 0 if the packet is handled by this
// | port, else return -1. Whoever calls this
// | routine may decide to send a tcp RST to
// | the sender if no plug handles the tcp connection.

int r_tcp_dispatch(
		s_plugs_adapter_entry *a,
		int plug_handle,
		s_plugs_plug *plug,
		ns_plugs_packet *sub_protocol,
		void *payload,
		int payload_length)
	{
	int result = 0;
	host_32 seq;
	host_32 ack;
	host_16 tcp_flags;
        net_16  remote_window;
	ns_plugs_ip_packet *ipp = sub_protocol[ne_plugs_ip].header;
	ns_plugs_tcp_packet *tcpp = sub_protocol[ne_plugs_tcp].header;

	// |
	// | Ensure that we can respond to this
	// | packet, mac-address-wise...
	// |

	r_add_to_arp_cache_from_ip_packet(a,sub_protocol[ne_plugs_ethernet].header);

	// |
	// | Extract some basic info from the incoming packet
	// |

	seq = nr_n2h32(tcpp->sequence_number);
	ack = nr_n2h32(tcpp->acknowledgement_number);
	tcp_flags = nr_n2h16(tcpp->header_length_flags);
        remote_window = nr_n2h16(tcpp->window_size);
	// |
	// | Handle the various tcp packet kinds in
	// | an exploded if{do and go home} list
	// |

	if(tcp_flags & ne_plugs_flag_tcp_syn) // | connection request or confirm
		{
		int x = nr_timer_milliseconds();

		if(plug->tcp_state == e_plugs_tcp_connecting)
			{
			// |
			// | We initiated a connection, and
			// | our remote target has, apparently,
			// | responded.
			// | Swell, we are connected.
			// |

			plug->tcp_sn_remote = seq + 1;
			result = r_tcp_send(plug_handle,plug,ne_plugs_flag_tcp_ack,0,0);
			if(!result){
                           plug->tcp_state = e_plugs_tcp_connected;
                           printf ("\n[%08X]: Connection granted. |", plug);
                        }
                                
			} // if connecting & got syn
		else if(plug->tcp_state == e_plugs_tcp_listening)
			{
			// |
			// | Listening -- call application acceptance routine
			// | (Really, it's an application rejection routine,
			// | since if there isn't one, we automatically accept.
			// |

			if(plug->tcp_listen_proc)
				{
				result = (plug->tcp_listen_proc)
						(plug_handle,
						plug->tcp_listen_proc_context,
						nr_n2h32(ipp->source_ip_address),
						nr_n2h16(tcpp->source_port));

				if(result)
					goto go_home;
				}

			plug->tcp_connection_time = x;
	
			plug->remote_ip_address = ipp->source_ip_address;
			plug->remote_port = tcpp->source_port;
			plug->tcp_sn_remote = seq + 1;
                        plug->tcp_remote_window_size = remote_window;
                        plug->tcp_sn_local =
                          wellspring_of_sequence_numbers();
                        plug->tcp_last_acknowledged_sn_local = 
                          plug->tcp_sn_local - 0x100;

			// |
			// | Send ACK & SYN in return, to accept the connection
			// |

			result = r_tcp_send(plug_handle,plug,ne_plugs_flag_tcp_ack | ne_plugs_flag_tcp_syn,0,0);
			if(!result)
                        {
                           plug->tcp_state = e_plugs_tcp_connected;
                        }
                           

			} // if listening & got syn
		else
			{
			// |
			// | Not listening or connecting? Then
			// | a SYN has no business with us.
			// | Return an error code, we cannot accept.
			// |
                          printf ("TCP connection refused.\n");
	return_error:
			result = ne_plugs_error;
			}
		goto go_home;
		} // if syn

	// +---------------------------------
	// | If we're not connected, no other packet is interesting

	if(plug->tcp_state <= e_plugs_tcp_listening)
		{
                  goto return_error;
		}

	
	// +-------------------------------------------
	// | If the packet has an ACK in it, we'll check
	// | it against our own numbers, and maybe resent
	// | an ack to get back into phase...
	// |
	// | We do this before checking the payload,
	// | because the remote count in the packet
	// | does not include the current payload.
	// |
        // | We also stash-away the local sequence number 
        // | that they're acknowledging.  This lets us 
        // | check (elsewhere) if they've been paying attention.

	if(tcp_flags & ne_plugs_flag_tcp_ack)
        {
           // |
           // | Just sending us an ack?
           // | eh, ok, great. But if we're out of sync, resend
           // | our own ack. (This also ignores a FIN if we're
           // | out of phase.)
           // |
	
           if(seq != plug->tcp_sn_remote)
           {
             //printf ("Out-of-sequence (remote) packet detected.  SEQ!\n");
              r_tcp_send(plug_handle,plug,ne_plugs_flag_tcp_ack,0,0);
              goto go_home;
           }
           plug->tcp_last_acknowledged_sn_local = ack;
           plug->tcp_remote_window_size         = remote_window;
        }
		
	// +---------------------------------
	// | Second things second: if there's any
	// | data with the packet -- and there could
	// | be, even with a FIN packet! -- we must
	// | pass it straight to the plug's
	// | callback.
	
	if(payload_length)
		{
                  int do_ack_packet = 1;
		// |
		// | They're sending us some bytes!
		// | (By the way, the "PSH" flag is
		// | irrelevant, it's used to indicate
		// | that the bytes are special or something.)
		// |

		if(seq == plug->tcp_sn_remote)
			{
                          int callback_result;
			// |
			// | We got some legit bytes, let's acknoweldge
			// | them and pass the data on to the application.
			// |

			plug->tcp_sn_remote += payload_length;
                        plug->tcp_remote_window_size = remote_window;
                        
                        // This is only meaningful (or useful) for
                        // manual flow-control, if enabled.
                        // If manual-flow is disabled, this variable
                        // is ignored.
                        //
			callback_result = (plug->callback_proc)
                                            (plug_handle,
                                             plug->callback_context,
                                             sub_protocol,
                                             payload,
                                             payload_length);
                        if (plug->flags & ne_plugs_flag_tcp_manual_flow) {
                          if (callback_result >= 0)
                            // In manual-flow mode, positive result is
                            // new window-size
                            //
                            plug->tcp_manual_window_size = callback_result;
                          else 
                            // Negative result means: Don't ack this
                            // one.
                            do_ack_packet = 0;
                        }

			}
		else
			{
			// |
			// | Something bad has happened: they have a
			// | sequence number that is not what we expect.
			// |
			// | The best we can do is send an ACK for the
			// | sequence number we think we do want.
			// |
			
			// | (actually, this "else" clause is just a
			// | comment, to fall to the ack below.)
			}
                
                if (do_ack_packet)
                  r_tcp_send(plug_handle,plug,ne_plugs_flag_tcp_ack,0,0);
		} // payload present

	// +---------------------------------
	// | FIN
	// | If they wish to close connection, then do it.

	if (tcp_flags & ne_plugs_flag_tcp_fin) {
          
          if (plug->tcp_state == e_plugs_tcp_disconnecting) {
            // We expected this..  We sent a FIN earlier,
            // and this is their FIN in reply.
            // We don't need to do anything special, except
            // close-down the connection, below.
          } 

          // They were good enough to send us a packet...
          // We have to ack it, even though it's the end.
          // This protocol sure is polite.
          //
          plug->tcp_sn_remote++; // | Bump the ack
          plug->tcp_remote_window_size = remote_window;
 


          if(plug->tcp_state == e_plugs_tcp_connected) {
            // | OK, they're requesting that the connection 
            // | be closed by sending us an unsolicited FIN.
            // | Send a FIN of our own, and wait (a little
            // | while) for their ACK:
            // |
 
	    r_tcp_send(plug_handle,plug,ne_plugs_flag_tcp_fin|ne_plugs_flag_tcp_ack,0,0);
          }else{
	    r_tcp_send(plug_handle,plug,ne_plugs_flag_tcp_ack,0,0);                
          }
	  r_tcp_plug_set_inert(plug);
                
          // |
          // | Notify the application that the
          // | tcp connection has been closed
          // | by sending a payload size of 0.
          //
          (plug->callback_proc)
            (plug_handle,
             plug->callback_context,
             sub_protocol,
             0,
             0);

          goto go_home;
        }
	
	if(tcp_flags & ne_plugs_flag_tcp_rst) {
          // |
          // | They want an immediate disconnect. Fine.
          // |
          r_tcp_plug_set_inert(plug);
          // |
          // | Notify the application that the
          // | tcp connection has been closed
          // | by sending a payload size of 0.
          //
          (plug->callback_proc)
            (plug_handle,
             plug->callback_context,
             sub_protocol,
             0,
             0);
          goto go_home;
        }
go_home:
  return result;
}

void r_tcp_plug_set_inert (s_plugs_plug* plug)
{
#if PLUGS_TCP
  plug->tcp_state = e_plugs_tcp_inert;
  plug->tcp_manual_window_size = 0;
#endif // PLUGS_TCP

  plug->remote_ip_address = 0;
  plug->remote_port = 0;
}


#endif // PLUGS_TCP

// --------------------------
// DNS lookup

#if PLUGS_DNS
// support routines for parsing those
// troublesome unaligned dns queries

// Given a pointer to some bytes to emit to,
// and an int, emit the two bytes bigendian.
// This is done byte-by-byte, so is ok
// to be unaligned.

static void p16(unsigned char **wp,int v)
	{
	unsigned char *w;

	w = *wp;
	*w++ = (v & 0x0000ff00) >> 8;
	*w++ = (v & 0x000000ff);
	*wp = w;
	}

// Given a pointer to some bytes to emit to,
// and a host name, put in the host name
// in the weird form required by dns.
// Advance the pointerpointer.

static void pn(unsigned char **wp,char *name)
	{
	unsigned char *w;
	unsigned char *label_begin;
	unsigned char label_len;
	int c;

	// | Hey man, sorry about the gotos. -- dvb 2001.03.23

	w = *wp;

do_label:
	label_begin = w++;
	label_len = 0;
do_label_char:
	c = *name++;
	if(c == 0 || c == '.')
		{
		*label_begin = label_len;
		if(c)
			goto do_label;
		else
			{
			*w++ = 0;
			*wp = w;
			return;
			}
		}
	label_len++;
	*w++ = c;
	goto do_label_char;
	}

int g16(unsigned char **wp)
	{
	unsigned char *w = *wp;
	int v;

	v = *w++;
	v = (v << 8) + *w++;

	*wp = w;
	return v;
	}

// Skip over strange length-delimited dns "label"

void skip_label(unsigned char **wp)
	{
	unsigned char *w = *wp;
	int label_length;

	while((label_length = *w++) != 0)
		if(label_length > 64)
			{
			w++;
			goto go_home;
			}
		else
			w += label_length;

go_home:
	*wp = w;
	}

static int r_plugs_dns_proc
		(
		int plug_handle,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length
		)
	{
	unsigned char *w = payload;
	unsigned char **wp = &w;
	int v;
	int questions;
	int answers;
	int result = 0;
	s_plugs_globals *g = &ng_plugs_globals;

	// identifier... is it the one we sent out?
	v = g16(wp);
	if(v != g->dns_identifier) // not for us, just ignore
		goto go_home;
	
	// flags... did we get an answer?
	v = g16(wp);
	if(
			(!(v & 0x8000))	// it's not an answer?
			||
			((v & 0x000f) == 3) // it's an error?
			)
		{
	failure:
		g->dns_query_state = 3;
		goto go_home;
		}
	
	// questions & answers
	questions = g16(wp);
	answers = g16(wp);
	if(!answers)
		goto failure;

	w += 4;	// skip over authorities & additional

	// skip over questions
	while(questions--)
		{
		skip_label(wp);

		// skip over query type & class
		w += 4;
		}
	
	// take first answer of type 1 as the answer!
	while(answers--)
		{
		skip_label(wp);
		v = g16(wp);	// query type
		if(v == 1)	// 'A' (ip address)
			{
			long vl;
			w += 8; // skip type, class, & ttl, and assume lenght = 4...

			vl = g16(wp);
			vl = (vl << 16) + g16(wp);
			g->dns_query_state = 2; // answered ok
			g->dns_query_result = nr_h2n32(vl);

			goto go_home;
			}
		else
			{
			// some query type we dont care about...
			w += 6;
			v = g16(wp);	// resource length
			w += v;
			}
		}
	
	// got to here? No answers with our IP address?? aww.
	result = ne_plugs_error_unwanted_reply;

go_home:
	return result;
	}

#endif // PLUGS_DNS


int nr_plugs_name_to_ip(char *host_name,net_32 *host_ip_address_out)
	{
#if PLUGS_DNS
	unsigned char p[256];	// arbitrary upper bound on query size
	int len;		// length of constructed query
	unsigned char *w;
	unsigned char **wp;
	s_plugs_globals *g = &ng_plugs_globals;
	int id;
	long t1,ti;		// current time, last req time
	int retry_count;
	net_32 host_ip_address;
	int result = 0;

	w = p;
	wp = &w;

	g->dns_identifier = g->ip_identifier++;

	// Build the dns message, and then possibly reuse it several times
	// dns header

	p16(wp,g->dns_identifier); // dns id, to recognize answer
	p16(wp,0x0100); // dns flags: recursion desired
	p16(wp,1); // 1 question
	p16(wp,0); // 0 answers
	p16(wp,0); // 0 authorities
	p16(wp,0); // 0 additional

	// dns question
	pn(wp,host_name); // host name to look up
	p16(wp,1); // query type: 'A' IP address
	p16(wp,1); // query class: 1 = internet, blah blah

	w = *wp;
	len = w - p;

	ti = nr_timer_milliseconds() - nk_plugs_dns_retry_interval; // so first timeout always true
	g->dns_query_state = 0; // so we can wait for it to change...
	g->dns_query_result = 0;
	retry_count = nk_plugs_dns_retry_count;

dns_query_loop:
	// Time to send another query?
	t1 = nr_timer_milliseconds();
	if(t1 - ti > nk_plugs_dns_retry_interval)
		{
		retry_count--;
		if(retry_count < 0)
			{
			result = ne_plugs_error_dns_timed_out;
			goto go_home;
			}
		nr_plugs_send(g->dns_plug,p,len,0);
		ti = t1;
		}

	// Idle the system, and wait for godot.
	nr_plugs_idle();
	if(g->dns_query_state) // godot here already?
		{
		if(g->dns_query_state == 3)	// failure?
			result = ne_plugs_error_dns_not_found;
		goto go_home;			// we are done, in any case.
		}
	goto dns_query_loop;

go_home:
	*host_ip_address_out = g->dns_query_result;
	return result;
#else // PLUGS_DNS
	*host_ip_address_out = 0;
	return ne_plugs_error_feature_disabled;
#endif // else PLUGS_DNS
	}


// ----------------------------
// r_add_to_arp_cache
//
// insert an entry to the cache (at extant spot, if possible)
static void r_add_to_arp_cache(s_plugs_adapter_entry *a,net_32 ip_address,net_32 ethu32,net_16 ethl16)
	{
	s_plugs_arp_cache_entry *w;
	int i;

	w = a->arp_cache;
	for(i = 0; i < nk_plugs_arp_cache_count; i++)
		{
		if(w->ip_address == ip_address)
			goto found_a_spot;
		w++;
		}

	// no existing ip entry? choose the next one
	w = &a->arp_cache[a->arp_cache_walker];
	a->arp_cache_walker++;
	if(a->arp_cache_walker == nk_plugs_arp_cache_count)
		a->arp_cache_walker = 0;
found_a_spot:

	w->ip_address = ip_address;
	w->ethernet_address.u32 = ethu32;
	w->ethernet_address.l16 = ethl16;
	}

// --------------------------
// r_add_to_arp_cache_from_ip_packet
//
// Given an ethernet packet known to contain an IP header,
// add the source's address to the cache.
// This is great for imminent replies to packets.

static void r_add_to_arp_cache_from_ip_packet(s_plugs_adapter_entry *a,ns_plugs_ethernet_packet *p)
	{
	net_32 ip_address;
	net_48 ethernet_address;

	// pull the info from the packet
	ip_address = ((ns_plugs_ip_packet *)(&p->payload))->source_ip_address;
	ethernet_address = p->source_address;

	// Is it outside our LAN? If so, any ethernet
	// address actually refers to the router.

	if((ip_address ^ a->settings.ip_address) & a->settings.subnet_mask)
		ip_address = a->settings.gateway_ip_address;
	
	// And put in cache!
	
	r_add_to_arp_cache(a,ip_address,ethernet_address.u32,ethernet_address.l16);
	}


// --------------------------
// nr_plugs_ip_to_ethernet:
// invokes arp if necessary, but ONLY if we're not in_dispatch. Reentrant, doncha know.
// The "flags" parameter is taken from the plug that's calling
// us, and the routine prints errors if ne_plugs_flag_debug_tx is set.
//

int nr_plugs_ip_to_ethernet(int adapter_index,net_32 ip_address,net_48 *ethernet_address_out,long flags)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	int result = 0;
	int i;
	s_plugs_arp_cache_entry *w;
	int requests_sent;
	long t0,t1;			// time we started, current time
	long ti;			// time of last arp-request
	long retry_count;	// how many times we've polled
	s_plugs_adapter_entry *a = &g->adapter[adapter_index];

    // |
    // | Two things indicate an ethernet broadcast:
    // | The ethernet broadcast flag, or ip address 0xffffffff
    // |
    // | The ethernet address for an ethernet broadcast is easy! 0xffffFFFFffff
    // |

    if((flags & ne_plugs_flag_ethernet_broadcast) || (ip_address == 0xffffFFFF))
        {
        ethernet_address_out->u32 = 0xffffFFFF;
        ethernet_address_out->l16 = 0xFFFF;
        goto go_home;
        }

	// Is it outside our LAN? If so, we must
	// send to the router, not direct

	if((ip_address ^ a->settings.ip_address) & a->settings.subnet_mask)
		ip_address = a->settings.gateway_ip_address;
	
	t0 = nr_timer_milliseconds();
	ti = t0 - nk_plugs_arp_retry_interval;	// so first timeout test always does it
	retry_count = 0;

scan_cache:
	// Already in the cache?
	//

	// Receive it right now if possible!
	// nr_plugs_idle();

	w = a->arp_cache;
	for(i = 0; i < nk_plugs_arp_cache_count; i++)
		{
		if(w->ip_address == ip_address)
			{
			// entry there, but no arp-response yet?
			if(!(w->ethernet_address.u32 || w->ethernet_address.l16))
				goto present_but_unresolved;
			*ethernet_address_out = w->ethernet_address;
			goto go_home;
			}
		w++;
		}
	// Add a spot in the ARP cache, with zeroed ethernet

	r_add_to_arp_cache(a,ip_address,0,0);

present_but_unresolved:
	// We put entries in the cache with ethernet_address == 0
	// to signify that we want it. The ARP handler only fills
	// in mac addr's that already have an entry for the ip.

	// Not found in cache
	// If we're already in dispatch, we cannot conduct a
	// transaction, so just fail
	if(g->in_dispatch)
		{
		result = ne_plugs_error_arp;
		goto go_home;
		}

	//
	// Should we send an ARP request?
	t1 = nr_timer_milliseconds();

	if(t1 - ti < nk_plugs_arp_retry_interval)
		goto scan_cache;

	
	ti = t1;	// retry timer shall go again from here...

	if(retry_count >= nk_plugs_arp_retry_count)
		{
		result = ne_plugs_error_arp_timed_out;
		goto go_home;
		}
	
	retry_count++;
	
	// Not in the cache, & is time to send out a request, and wait...
	// If we wait too long, well, report an error then.
		{
		ns_plugs_arp_packet request;

		request.hardware_type = nm_h2n16(1);
		request.protocol_type = nm_h2n16(0x0800);
		request.hardware_size = 6;		// 6 for ethernet
		request.protocol_size = 4;		// 4 for IP
		request.op = nm_h2n16(1);			// request
		request.sender_ethernet_address = a->settings.ethernet_address;
		request.sender_ip_address = a->settings.ip_address;
		request.target_ethernet_address.u32 = 0;
		request.target_ethernet_address.l16 = 0;
		request.target_ip_address = ip_address;

#if PLUGS_DEBUG
		if(flags & ne_plugs_flag_debug_tx)
			{
			printf("   [plugs] | sending arp request for ");
			nr_plugs_print_ip_address(request.target_ip_address);
			printf("\n");
			}
#endif // PLUGS_DEBUG
		result = nr_plugs_send(a->arp_plug,&request,28,0);
		if(result)
			goto go_home;
		}
	goto scan_cache;

go_home:
	return result;
	}

void packet_append_bytes(char **dst,void *src, int length)
	{
	char *src_c = (char *)src;
	char *w = *dst;

	while(length--)
		*w++ = *src_c++;
	
	*dst = w;
	}

void packet_append_n16(char **dst,net_16 x)
	{
	packet_append_bytes(dst,&x,2);
	}

// Send must construct an entire ethernet packet
// for whatever protocol it is sending.
// The address can be deduced from the packet

// Note -- the adapter's send routine
// is not reentrant, so it should disable
// interrupts until it has made its
// best attempt to send.

// Note -- In the case of an ip packet,
// this routine can end up recursively
// calling itself as part of the ARP exchange.
// (The nr_plugs_ip_to_ethernet routine will need
// to send an arp request the first time an
// ip address is encountered.) (If the adapter
// requires mac address.) Usually we'll have
// the address in cache already, though, from the
// incoming ip message.

// plug_handle -- if this is a small integer,
// then it is presumed to be just a protocol
// number. This is so we can send a packet
// without a plug allocated, in some cases.
// Specifically, this lets us send tcp rst
// messages in response to spurious packets.
// If used like this, the adapter index must
// be coded into the low flags bits.

int nr_plugs_send_to(int plug_handle,
		void *payload,
		int payload_length,
		long flags,
		net_32 ip_address,
		net_16 port)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	int result = 0;
	ns_plugs_ethernet_packet *ep;
	char ep_contents[1500];			// this should be supplied by each adapter
	char *ep_w;						// progressive walk along the packet...
	int plug_protocol;
	net_16 plug_local_port;
	int adapter_index;
	s_plugs_adapter_entry *a;

	// |
	// | Extract the protocol, flags, and source port
	// | either from the plug pointer, or inferentially
	// | from the (fake) plug_handle
	// |

	if(plug_handle < ne_plugs_last_protocol) // not a plug handle!
		{
		plug_protocol = plug_handle;
		plug_local_port = 0;
		plug_handle = -1; // so debug printing reveals the fake plug-ness
		adapter_index = flags & ne_plugs_flag_adapter_index_mask;
		}
	else
		{
		s_plugs_plug *plug;

		plug_handle -= nk_plugs_plug_number_base;
		plug = &g->plug[plug_handle];
		
		plug_protocol = plug->protocol;
		plug_local_port = plug->local_port;
		flags |= plug->flags;
		adapter_index = plug->flags & ne_plugs_flag_adapter_index_mask;
		}

	// |
	// | Make sure the adapter index is reasonable,
	// | and then get the adapter description into "a".
	// |
	
	if(adapter_index >= g->adapter_count)
		{
		result = ne_plugs_error_not_initialized;
		goto go_home;
		}
	
	a = &g->adapter[adapter_index];
        
        // Turn off interrupts while sending.
        //
#if 0
        PLUGS_IRQ
        if (nr_plugs_interrupts_enabled())
           (a->adapter_description->set_irq_proc)
              (
                 a->adapter_address,
                 &a->adapter_storage,
                 0
                 );
#endif // PLUGS_IRQ

#if PLUGS_DEBUG
	if(flags & ne_plugs_flag_debug_tx)
		{
		printf("   [plugs] +-----------------------\n");
		printf("   [plugs] | plug %d (%s): sends %d byte payload\n",
			plug_handle,
			get_protocol_name(plug_protocol),
			payload_length);
		}
#endif // PLUGS_DEBUG

	ep = (ns_plugs_ethernet_packet *)ep_contents;
	ep_w = (char *)ep_contents;

	if(plug_protocol == ne_plugs_ethernet)
		{
		// ethernet protocol means your "payload" is, in fact,
		// the whole ethernet packet. 1st 14 bytes is the header.

		packet_append_bytes(&ep_w,payload,payload_length);
		}
	else if(plug_protocol == ne_plugs_arp)
		{
		// arp protocol: payload is presumed to be
		// a 28-byte arp packet. We don't even
		// check the length.

		ns_plugs_arp_packet *arp_packet;
		net_48 broadcast_ethernet_address;

		arp_packet = (ns_plugs_arp_packet *)payload;

		// ethernet header
		broadcast_ethernet_address.u32 = 0xffffFFFF;
		broadcast_ethernet_address.l16 = 0xffff;
		packet_append_bytes(&ep_w,
				(flags & ne_plugs_flag_ethernet_broadcast)
					? &broadcast_ethernet_address
					: &arp_packet->target_ethernet_address,
				6);
		packet_append_bytes(&ep_w,&a->settings.ethernet_address,6);
		packet_append_n16(&ep_w,nm_h2n16(ne_plugs_ethernet_arp));

		// arp packet as delivered to us
		packet_append_bytes(&ep_w,payload,28);
		}

	else if
			(
			plug_protocol == ne_plugs_ip
			|| plug_protocol == ne_plugs_icmp
			|| plug_protocol == ne_plugs_udp
			|| plug_protocol == ne_plugs_tcp_raw
			)
		{
		// | icmp, udp, tcp: payload is just the data to be
		// | sent to the port and address specified in
		// | the nr_plugs_connect call.

		ns_plugs_ip_packet *ip_packet;
		net_48 ep_ethernet_address;

		// | look up ethernet destination
		result = nr_plugs_ip_to_ethernet
				(
				adapter_index,
				ip_address,
				&ep_ethernet_address,flags
				);
		if(result)
			goto go_home;

		// |
		// | ethernet header

		packet_append_bytes(&ep_w,&ep_ethernet_address,6);
		packet_append_bytes(&ep_w,&a->settings.ethernet_address,6);
		packet_append_n16(&ep_w,nm_h2n16(ne_plugs_ethernet_ip));

		// |
		// | construct ip header in place (not appending)

		ip_packet = (ns_plugs_ip_packet *)ep_w;
		ep_w += sizeof(ns_plugs_ip_packet);

		ip_packet->version_header_length = 0x45; // 0x45 for no options
		ip_packet->tos = 0;			// zero
		ip_packet->length = nr_h2n16(sizeof(ns_plugs_ip_packet) + payload_length
				+ ((plug_protocol == ne_plugs_udp)
				? sizeof(ns_plugs_udp_packet) : 0)); // header + payload length

		ip_packet->identifier = nr_h2n16(g->ip_identifier++);		// bump for each packet
		ip_packet->flags_fragment = nm_h2n16(0);		// fragment miscellany
		ip_packet->time_to_live = 128;		// some typical value

		ip_packet->protocol = (plug_protocol == ne_plugs_icmp) ? ne_plugs_ip_icmp :
				(plug_protocol == ne_plugs_udp) ? ne_plugs_ip_udp :
				ne_plugs_ip_tcp;

		ip_packet->header_checksum = 0;
		ip_packet->source_ip_address = a->settings.ip_address;
		ip_packet->destination_ip_address = ip_address;

		// |
		// | maybe append udp header
		
		if(plug_protocol == ne_plugs_udp)
			{
			int udp_packet_length = sizeof(ns_plugs_udp_packet) + payload_length;

			packet_append_n16(&ep_w,plug_local_port);
			packet_append_n16(&ep_w,port);
			packet_append_n16(&ep_w,nr_n2h16(udp_packet_length));
			packet_append_n16(&ep_w,0);		// no stinking checksum
			}

		// |
		// | append the payload

		packet_append_bytes(&ep_w,payload,payload_length);

		// |
		// | After the whole packet is together,
		// | work backwards out and do the checksums.
		// | maybe calculate icmp checksum, so courteously!

		if(plug_protocol == ne_plugs_icmp)
			{
			ns_plugs_icmp_packet *icmp_packet;

			icmp_packet = (ns_plugs_icmp_packet *)ip_packet->payload;

			// | clear checksum, first
			icmp_packet->checksum = 0;
			icmp_packet->checksum = r_calculate_checksum(ip_packet->payload,payload_length,0,0);
			}
		else if(plug_protocol == ne_plugs_udp)
			{
			ns_plugs_udp_packet *udp_packet = (ns_plugs_udp_packet *)ip_packet->payload;
			int udp_packet_length = sizeof(ns_plugs_udp_packet) + payload_length;

			udp_packet->checksum = r_calculate_checksum(
					udp_packet,
					udp_packet_length,
					ip_packet,
					udp_packet_length);
			}
		else if(plug_protocol == ne_plugs_tcp_raw
				|| (flags & ne_plugs_flag_tcp_checksum))
			{
			ns_plugs_tcp_packet *tcp_packet = (ns_plugs_tcp_packet *)ip_packet->payload;

			tcp_packet->checksum = 0;
			tcp_packet->checksum = r_calculate_checksum(
					tcp_packet,
					payload_length,
					ip_packet,
					payload_length);
			}

		ip_packet->header_checksum = r_calculate_checksum(ip_packet,
				sizeof(ns_plugs_ip_packet),0,0);
		}
#if PLUGS_DEBUG
	else
		printf("   [plugs] | nr_plugs_send: unknown plug %d protocol %d\n",
				plug_handle,plug_protocol);
#endif // PLUGS_DEBUG
	

#if PLUGS_DEBUG
	if(flags & ne_plugs_flag_debug_tx)
		nr_plugs_print_ethernet_packet(ep,ep_w - ep_contents,"   [plugs] | ");
#endif // PLUGS_DEBUG

	// |
	// | Send the packet to the hardware adapter
	// |

	{
		int watchdog = 0; // we will try a few time

send_again:
                // TPA: Before sending, check the mailbox.
                nr_plugs_idle();
		result = (a->adapter_description->tx_frame_proc)
				(
				a->adapter_address,
				&a->adapter_storage,
				ep_contents,
				ep_w - ep_contents
				);


                // Positive, nonzero result means: retry.
		if(result > 0 && watchdog < 20)
		{
			// failed to send for some reason?
			// bump the watchdog and do it again
			// (unless weve tried enough: just return the error)
			watchdog++;
			nr_plugs_idle();
			nr_delay(2);
                        printf ("T");
			goto send_again;
		}
                // Negative result means: "I hate you."
                if (result < 0) 
                  goto go_home;
	}

go_home:

#if PLUGS_DEBUG
	if(result)
		{
		if(flags & ne_plugs_flag_debug_tx)
			printf("   [plugs] | ERROR nr_plugs_send_to result = %d\n",result);
		}
#endif // PLUGS_DEBUG


        // Turn on interrupts when done sending, if we're using
        // interrupts.
        
#if 0
        PLUGS_IRQ
        if (nr_plugs_interrupts_enabled() && 
            (adapter_index < g->adapter_count))
           (a->adapter_description->set_irq_proc)
                (
                 a->adapter_address,
                 &a->adapter_storage,
                 1
                 );
        // Interrupts might have happened
        // while we were off-line.  Check.
        nr_plugs_idle();
#endif //PLUGS_IRQ

	return result;
	}

int nr_plugs_send(int plug_handle,
		void *payload,
		int payload_length,
		long flags)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_plug *plug;
	int plugIndex;
	int result;


	plug = r_plug_from_handle(plug_handle);

	// |
	// | For a tcp connection, shunt it through
	// | r_tcp_send (which eventually
	// | calls through to nr_plugs_send_to,
	// | after building the tcp packet &c.)
	// |

#if PLUGS_TCP
	if(plug->protocol == ne_plugs_tcp)
		{
		result = r_tcp_send
				(
				plug_handle,
				plug,
				ne_plugs_flag_tcp_ack | ne_plugs_flag_tcp_psh,
				payload,
				payload_length
				);
		}
	else
#endif // PLUGS_TCP
		{
		result = nr_plugs_send_to(plug_handle,payload,payload_length,flags,
				plug->remote_ip_address,
				plug->remote_port);
		}

	return result;
	}


// +------------------------
// | Mundane stuff, like setting
// | the LED and such

int nr_plugs_set_mac_led(int adapter_index,int led_onoff)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_adapter_entry *a = &g->adapter[adapter_index];

	return (a->adapter_description->set_led_proc)
			(
			a->adapter_address,
			&a->adapter_storage,
			led_onoff
			);
	}

int nr_plugs_set_mac_loopback(int adapter_index,int loopback_onoff)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_adapter_entry *a = &g->adapter[adapter_index];

	return (a->adapter_description->set_loopback_proc)
			(
			a->adapter_address,
			&a->adapter_storage,
			loopback_onoff
			);
	}

#if PLUGS_IRQ

void r_plugs_irq_handler(int context, int irq_unumber, int interruptee_pc)
	{
	int result; // gets lost
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_adapter_entry *a = (s_plugs_adapter_entry *)context;

    // | Store interruptee_pc in case some callback routine (debug)
    // | needs to do something with it (put a trap there)
    g->interruptee_pc = interruptee_pc<<1;


	result = (a->adapter_description->check_for_events_proc)
			(
			a->adapter_address,
			&a->adapter_storage,
			nr_plugs_dispatch_packet,
			a				// context is adapter pointer
			);

    // | Clear this before leaving just to be neat
    g->interruptee_pc = 0;

	}
#endif

// +-----------------------------------
// | nr_plugs_get_interruptee_pc
// | Return the interruptee_pc  stored
// | by the ISR above
int nr_plugs_get_interruptee_pc ()
    {
	s_plugs_globals *g = &ng_plugs_globals;
    return (g->interruptee_pc);
    }

// +-----------------------------------
// | nr_plugs_interrupts_enalbed
// | Return TRUE if plugs interrupts are enabled
// | otherwise false
int nr_plugs_interrupts_enabled ()
    {
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_adapter_entry *a = g->adapter;

    return (a->adapter_irq);
    }

void r_set_irqs_for_plug (s_plugs_plug* plug, int on_off)
{
   s_plugs_adapter_entry *a; 
   s_plugs_globals *g = &ng_plugs_globals;

   if (!nr_plugs_interrupts_enabled())
      return;

   a = &g->adapter[plug->flags & ne_plugs_flag_adapter_index_mask];
   (a->adapter_description->set_irq_proc)
      (
         a->adapter_address,
         &a->adapter_storage,
         on_off
         );


   // Interrupts might have transpired while
   // we were off-line.  check.
   if (on_off)
      nr_plugs_idle();
}




// +-----------------------------------
// | nr_plugs_idle
// | Poll for events and other interesting things.
//
int nr_plugs_idle(void)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	int adapter_index;
	s_plugs_adapter_entry *a;
	int result = 0;

	if(!g->running)
		return ne_plugs_error_not_initialized; // | plugs not initialized

	if(g->in_idle)
		return ne_plugs_error;

	// +--------------------
	// | Can't call more than one deep for check_for_events, thank you.
	// |


	a = g->adapter;
	for(adapter_index = 0; adapter_index < g->adapter_count; adapter_index++)
		{
#if PLUGS_IRQ
		if(!a->adapter_irq)
#endif // PLUGS_IRQ
			{
	        g->in_idle++;
			result = (a->adapter_description->check_for_events_proc)
					(
					a->adapter_address,
					&a->adapter_storage,
					nr_plugs_dispatch_packet,
					a				// context is adapter pointer
					);
	        g->in_idle--;
			}
		a++;
		}

	return result;
	}

int nr_plugs_idle_exclusive (int plug_handle)
	{
	s_plugs_globals *g = &ng_plugs_globals;
	s_plugs_adapter_entry *a = g->adapter;

    // | Save and clear adapter_irq so nr_plugs_idle call
    // | will always check for events
    int adapter_irq = a->adapter_irq;
    a->adapter_irq = 0;

    // | Set the exclusive plug
    g->exclusive_plug_handle = plug_handle;

    // | Now check for events
    nr_plugs_idle ();

    // | Clear the exclusive plug
    g->exclusive_plug_handle = 0;

    // | Reset the adapter_irq
    a->adapter_irq = adapter_irq;
	}




// end of file
