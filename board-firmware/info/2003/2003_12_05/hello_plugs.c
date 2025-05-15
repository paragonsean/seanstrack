

// ex:set tabstop=4:
// +--------------------
// | Includes
// |

#include "excalibur.h"
#include "hello_plugs_menu.h"
#include "plugs.h"


#if __nios16__
	#define SMALL 1
#endif


// Set __adapter__ to the current design...

#include "plugs_example_designs.h"


// +--------------------
// | Types

enum
	{
	k_menu_settings = 1,
	k_menu_actions,		// ping & nslookup
	};

// +---------------------------------
// | Global State
// |
// | (If you must have globals, put them
// | in one struct for tidiness! And name
// | the one instance "g".)

typedef struct
	{
	ns_plugs_persistent_network_settings pns;
	int arp_scan_replies[256];	// matrix of who has responded

	int sniff_pause;	// if set, prints dots instead of packets
	int sniff_count;	// total packets sniffed in session
	} ns_globals;

static ns_globals g; // = {0};

// +----------------------------------------
// | Local Prototypes
// |

static int r_reset_settings(int x);
static int show_abbreviated_settings(int x);


// +----------------------------------------
// | Initialization and Utilities
// |


// +----------------------------------------
// | r_get_settings_from_flash
// |
// | If there's flash memory, get the settings from
// | the conventional location in flash. If the
// | index looks crazy, or there's no flash, then
// | reset the settings to our default values.
// |

void r_get_settings_from_flash(void)
	{
	int i;

#ifdef nasys_main_flash
	g.pns = *nasys_plugs_persistent_network_settings;
	if(g.pns.settings_index < 0 || g.pns.settings_index >= nk_plugs_settings_count)
		r_reset_settings(0);
	printf("\n Network Settings %d (of %d)\n\n",g.pns.settings_index+1,nk_plugs_settings_count);
#else
	r_reset_settings(0);
#endif
	}


int r_save_settings(int x)
	{
#ifdef nasys_main_flash
	printf("erasing flash\n");
	nr_flash_erase_sector((void *)-1,(void *)nasys_plugs_persistent_network_settings);
	printf("writing flash\n");
	nr_flash_write_buffer((void *)-1,(void *)nasys_plugs_persistent_network_settings,(void *)&g.pns,sizeof(g.pns));
#endif
	}

// +---------------------------
// | index [0,nk_plugs_settings_count) means use that index
// | -1 means use the persistent index thats already there
// |

int r_set_settings_index(int index)
	{
	int result;

	if(index >= nk_plugs_settings_count)
		index = nk_plugs_settings_count - 1;
	
	if(index >= 0)
		g.pns.settings_index = index;

	nr_plugs_terminate();

	result = nr_plugs_initialize(0, &g.pns.settings[g.pns.settings_index],
			__adapter__,
			__adapter_irq__,
			__adapter_struct_addr__);

	if(result)
		{
		printf("%d: failed to initialize plugs library, exiting.\n",result);
		exit(result);
		}

	return result;
	}


void r_initialize(void)
	{
	nr_zerorange((char *)(&g),sizeof(g));
	r_get_settings_from_flash();
	r_set_settings_index(-1); // initializes plugs, too
	}


// ++=================================================
// || Network Settings Menu Procs
// ||

// Routines

static int show_settings(int x)
	{
#if SMALL
	return show_abbreviated_settings(x);
#else
	printf("\n Network Settings %d (of %d)\n\n",g.pns.settings_index+1,nk_plugs_settings_count);
	printf("      ethernet address: ");
	nr_plugs_print_ethernet_address(&g.pns.settings[g.pns.settings_index].ethernet_address);
	printf("\n");

	printf("            ip address: ");
	nr_plugs_print_ip_address_decimal(g.pns.settings[g.pns.settings_index].ip_address);
	printf("\n");

	printf(" nameserver ip address: ");
	nr_plugs_print_ip_address_decimal(g.pns.settings[g.pns.settings_index].nameserver_ip_address);
	printf("\n");

	printf("           subnet mask: ");
	nr_plugs_print_ip_address_decimal(g.pns.settings[g.pns.settings_index].subnet_mask);
	printf("\n");

	printf("    gateway ip address: ");
	nr_plugs_print_ip_address_decimal(g.pns.settings[g.pns.settings_index].gateway_ip_address);
	printf("\n");

	printf("\n");
#endif SMALL
	}

static int show_abbreviated_settings(int x)
	{
	printf("            ip address: ");
	nr_plugs_print_ip_address_decimal(g.pns.settings[g.pns.settings_index].ip_address);
	printf("\n");

	printf("\n\n");
	}

static int input_ip_address(char *prompt, net_32 *ip_inout)
	{
	int result;
	char s[64];

	nr_plugs_ip_to_string(*ip_inout,s);
	result = r_input_string(prompt,0,s);
	nr_plugs_string_to_ip(s,ip_inout);

	return result;
	}

static int r_input_long(char *prompt, long *x_inout)
	{
	int result;
	char s[64];

	nr_plugs_long_to_string(*x_inout,s);
	result = r_input_string(prompt,0,s);
	*x_inout = nr_plugs_string_to_long(s);

	return result;
	}

static int edit_settings(int x)
	{
	char s[64];
	ns_plugs_network_settings *settings;

	settings = &g.pns.settings[g.pns.settings_index];

	nr_plugs_ethernet_to_string(&settings->ethernet_address,s);
	r_input_string("      ethernet address",0,s);
	nr_plugs_string_to_ethernet(s,&settings->ethernet_address);

	input_ip_address("            ip address",&settings->ip_address);
	input_ip_address(" nameserver ip address",&settings->nameserver_ip_address);
	input_ip_address("           subnet mask",&settings->subnet_mask);
	input_ip_address("    gateway ip address",&settings->gateway_ip_address);
	printf("\n");
	r_set_settings_index(g.pns.settings_index);
	}

static int r_reset_settings(int x)
	{
	ns_plugs_network_settings settings = {0,0,0,0,0,0,0};

	settings.ethernet_address.u32=0x11121314;
	settings.ethernet_address.l16=0x1516;
	settings.nameserver_ip_address = nm_ip2n(165,227,1,1); // just some nameserver I knew about
	settings.subnet_mask = nm_ip2n(255,255,255,0);
	settings.gateway_ip_address = nm_ip2n(10,0,0,255);

	settings.ip_address = nm_ip2n(10,0,0,51);
	g.pns.settings[0] = settings;

	settings.ip_address = nm_ip2n(10,0,0,52);
	g.pns.settings[1] = settings;

	settings.ip_address = nm_ip2n(10,0,0,53);
	g.pns.settings[2] = settings;

	settings.ip_address = nm_ip2n(10,0,0,54);
	g.pns.settings[3] = settings;

	g.pns.settings_index = 0;

	return 0;
	}

static int select_settings(int x)
	{
	int c;
	printf("Enter network settings number (1 - %d): ",nk_plugs_settings_count);
	
again:
	c = r_get_char();
	if(c == 27)
		return 0;

	c -= '1';
	if (c < 0 || c >= nk_plugs_settings_count)
		goto again;
	
	printf("%c\n",c+'1');
	
	r_set_settings_index(c);

	return 0;
	}

// ++=================================================
// || Network Action Menu Items
// ||

#if !SMALL
// +-----------------------------------------------------
// | Ping Proc and Pinger

// Utility to do unaligned long-int read from packet
unsigned long get_bytes_32(void *address)
	{
	unsigned char *w = address;
	return ((long)w[0] << 24) + ((long)w[1] << 16) + ((long)w[2] << 8) + w[3];
	}
// Utility to do unaligned long-int write to packet
void put_bytes_32(void *address,unsigned long x)
	{
	unsigned char *w = address;
	w[0] = x >> 24;
	w[1] = x >> 16;
	w[2] = x >> 8;
	w[3] = x;
	}

// +----------------------------------
// | ping_proc -- gets called for icmp packets
// | from the ip address being pinged.

int ping_proc(int plug_handle,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length)
	{
	ns_plugs_icmp_packet *icmpp = payload;
	ns_plugs_ip_packet *ipp = p[ne_plugs_ip].header;
	long time;

	// |
	// | We don't expect them to be pinging us
	// | back or anything, and this plug will
	// | filter for only the remote host
	// | we're pinging, but if they do send
	// | a ping-request, we'll print a message
	// | mentioning it.
	// |
	// | We mostly expect the first case: ping reply to our ping.
	// |

	if(icmpp->type == ne_plugs_icmp_ping_reply)
		{
		time = get_bytes_32(icmpp->payload);
		time = nr_timer_milliseconds() - time;

		printf(" [ping_proc] got ping reply from ");
		nr_plugs_print_ip_address(ipp->source_ip_address);
		printf(" (%d msec)\n",time);
		}
	else if(icmpp->type == ne_plugs_icmp_ping_request)
		{
		printf("ping_proc: got ping request from ");
		nr_plugs_print_ip_address(ipp->source_ip_address);
		printf("!\n");
		}
	else
		printf("ping_proc: got icmp type %d\n",icmpp->type);
	}

#define k_ping_count 4

int r_ping_host(int x) // both PING menu items come here, debug with x=1.
	{
	int ping_plug;
	static char *default_host = "www.altera.com";
	char host[64];
	char *hostp;
	net_32 ip;
	int result;
	char data[64];
	ns_plugs_icmp_packet *ping_request = (ns_plugs_icmp_packet *)data;
				// overlay it on data, for nonzero payload
	int len;
	int i;

	host[0] = 0;

	// x = 1 means have debuggin turned on
	x = x ? (ne_plugs_flag_debug_rx | ne_plugs_flag_debug_tx) : 0;

	while(1)
		{
		printf("\n\n");
		result = r_input_string("  Internet host to ping by name (<ESC> to finish)",
				default_host,host);
		if(result < 0)	
			break;

		// Allocate the plug of icmp type

		result = nr_plugs_create(&ping_plug,
			ne_plugs_icmp,
			0,	// port number, does not matter here
			ping_proc,
			0,
			x);	// flags set depending on menu choice

		if(result)
			{
			printf(" Could not create ping plugs.\n");
			nr_plugs_print_error_message("[ping test]",result);
			continue;
			}

		// |
		// | First, see if, perchance, it was a
		// | dot-separated numerical string
		// |

		if(host[0] >= '0' && host[0] <= '9')
			{
			nr_plugs_string_to_ip(host,&ip);
			ip = nr_n2h32(ip);
			hostp = 0;
			}
		else
			{
			ip = 0;
			hostp = host;
			}

		// Connect to remote host by name

		result = nr_plugs_connect(ping_plug,hostp,ip,0);	// talk to remote host

		if(result)
			{
			printf(" Lookup failure on %s.\n",host);
			nr_plugs_print_error_message("[ping test]",result);

			// Delete the plug, since we can't connect, and we'll allocate again
			nr_plugs_destroy(ping_plug);

			continue;
			}
		printf("Connected!\n\n");


		// | Construct the ping request

		ping_request->type = ne_plugs_icmp_ping_request;
		ping_request->code = 0;

		ping_request->payload[0] = 0;
		ping_request->payload[1] = 3;
		ping_request->payload[2] = 4;
		ping_request->payload[3] = 8;

		// "length" is packet + data

		len = sizeof(ns_plugs_icmp_packet) + 4;
		for(i = 0; i < k_ping_count; i++)
			{
			printf("Sending ping %d of %d.\n",i+1,k_ping_count);

			// | Pass the time as payload
			put_bytes_32(ping_request->payload,nr_timer_milliseconds());

			nr_plugs_send(ping_plug,(void *)data,len,0);

			// Wait 1 second, pumping the ether all the time
				{
				long t0 = nr_timer_milliseconds();
				while(nr_timer_milliseconds() - t0 < 1000)
					nr_plugs_idle();
				}
			}

		// All done pinging, delete the plug

		result = nr_plugs_destroy(ping_plug);
		}
	}


// +--------------------------------------------------
// | DNS lookup routine
// |

int r_dns_lookup(int x)
	{
	static char *default_host = "www.altera.com";
	char host[64];
	net_32 ip;
	int result;

	host[0] = 0;

	printf("\nEnter host names to look up, <ESC> when finished.\n\n");

	while(1)
		{
		printf("\n\n");
		result = r_input_string("  Internet host to look up by name",default_host,host);
		if(result < 0)	
			break;

		result = nr_plugs_name_to_ip(host,&ip);

		if(result)
			{
			printf("Lookup failed!\n");
			nr_plugs_print_error_message("[dns lookup test]",result);
			}
		else
			{
			printf("The IP address of %s is ",host);
			nr_plugs_print_ip_address_decimal(ip);
			printf(".\n");
			}
		}

	return 0;
	}

#endif SMALL

// +--------------------------------------------------
// | Arp Scan plug handler and main routine
// |

int r_arp_plug_proc(int arp_plug,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length)
	{
	ns_plugs_arp_packet *ap = payload; // get payload in proper type

	if(ap->op == nm_n2h16(ne_plugs_arp_reply)) // is it a reply we want?
		{
		// is it a reply within our own subnet (where we care?)
		if(((ap->sender_ip_address ^ g.pns.settings[g.pns.settings_index].ip_address) & g.pns.settings[g.pns.settings_index].subnet_mask) == 0)
			{
			int x;

			x = (ap->sender_ip_address >> 24) & 0xff;

			printf("arp_proc: reply from ");
			nr_plugs_print_ip_address(ap->sender_ip_address);
			printf("\n");

			g.arp_scan_replies[x] = x + 0x1000;
			}
		}
	}

int r_arp_scan(int x)
	{
	int arp_plug;
	long i;
	ns_plugs_arp_packet a;
	int result;

	printf("\n\n About to send ARP requests\nfor 256 addresses in subnet.\n");

	// Create the Plug for the arp protocol

	result = nr_plugs_create(&arp_plug,ne_plugs_arp,0,r_arp_plug_proc,0,ne_plugs_flag_ethernet_broadcast);
	if(result)
		{
		nr_plugs_print_error_message("[arp scan test]",result);
		goto go_home;
		}

	// clear the scan reply table

	for(i = 0; i < 256; i++)
		g.arp_scan_replies[i] = 0;
	
	// set up an arp request to use over & over

	a.hardware_type = nm_h2n16(1);
	a.protocol_type = nm_h2n16(0x0800);
	a.hardware_size = 6;
	a.protocol_size = 4;
	a.op = nm_h2n16(ne_plugs_arp_request);
	a.sender_ethernet_address = g.pns.settings[g.pns.settings_index].ethernet_address;
	a.sender_ip_address = g.pns.settings[g.pns.settings_index].ip_address;
	a.target_ethernet_address.u32 = 0;
	a.target_ethernet_address.l16 = 0;

	// Send out all the requests

	for(i = 0; i < 256; i++)
        	{
        	a.target_ip_address =
                	(g.pns.settings[g.pns.settings_index].ip_address & 0x00ffffff)
                	+ (i << 24);
        	nr_plugs_send(arp_plug,&a,sizeof(a),0);
        	nr_delay(5);
        	nr_plugs_idle();
        	}

	// Wait 5 seconds, pumping the ether all the time
		{
		long t0 = nr_timer_milliseconds();
		while(nr_timer_milliseconds() - t0 < 5000)
			nr_plugs_idle();
		}
	
	// ----------------------------------------
	// Print out the scan result, in a sort of pretty way
	printf("\n");
	for(i = 0; i < 256; i++)
		{
		if(g.arp_scan_replies[i])
			printf("%3d ",i);
		else
			printf(" .  ");
		if(i % 16 == 15)
			printf("\n");
		}
	printf("\n");

	// close down the plug
	nr_plugs_destroy(arp_plug);
go_home:
	return 0;
	}

// +--------------------------------------------------
// | Packet Sniffer handler and main routine
// |

int r_eth_plug_proc(int plugHandle,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length)
	{
	g.sniff_count++;

	if(g.sniff_pause)
		printf(".");
	else
		{
		printf("------------------\n");
		printf("Packet Sniffer\n(Press <ESC> to finish sniffing, <SPACE> to pause)\n\n");

		// Use convenient packet-printing utility

		nr_plugs_print_ethernet_packet(payload,payload_length," [sniff] ");
		}

	return 0;
	}

int r_sniffer(int x)
	{
	int eth_plug;
	int i;
	ns_plugs_arp_packet a;
	int c;

	printf("\n\n About to begin displaying all\nethernet packets received.\n");

	// Create the Plug for the eth protocol

	nr_plugs_create(&eth_plug,
		ne_plugs_ethernet,
		0,
		r_eth_plug_proc,
		0,
		ne_plugs_flag_ethernet_all); // flag says, "anything the interface gets"
	
	// Wait for an escape key, and give the
	// plugs library lots of time to do its stuff.

	g.sniff_pause = 0;
	g.sniff_count = 0;

	while((c = nr_uart_rxchar(0)) != 27)
		{
		if(c == ' ')
			g.sniff_pause = !g.sniff_pause;
		nr_plugs_idle();
		}

	nr_plugs_destroy(eth_plug);

	printf("\nSniffed %d packets.\n\n",g.sniff_count);
	}

#if !SMALL
// +----------------------------------------------------
// | TCP Telnet routines
// |

typedef struct
	{
	int connected; // 0:waiting, 1:connected, 2:disconnected
	net_32 remote_ip_address;
	net_16 remote_port;

	long bytes_sent;
	long bytes_received;
	} s_telnet_status;

int tcp_listen_proc(int plug_handle,
		void *context,
		host_32 remote_ip_address,
		host_16 remote_port)
	{
	volatile s_telnet_status *status = context;

	status->connected = 1;
	status->remote_ip_address = remote_ip_address;
	status->remote_port = remote_port;

	status->bytes_sent = 0;
	status->bytes_received = 0;

	printf("[tcp_listen_proc] Accepted connection from ");
	nr_plugs_print_ip_address(nr_h2n32(remote_ip_address));
	printf(" port %d\n",remote_port);

	return 0;
	}

int tcp_proc(int plug_handle,
		void *context,
		ns_plugs_packet *p,
		void *payload,
		int payload_length)
	{
	volatile s_telnet_status *status = context;
	int i;

	if(payload)
		{
		unsigned char *s = payload;

		status->bytes_received += payload_length;

		for(i = 0; i < payload_length; i++)
			printf("%c",s[i]);

			{
			char reply[100];
			int reply_length = 0;
			// |
			// | Handle funny telnet requests
			// |

			for(i = 0; i < payload_length; i++)
				{
				if(s[i] == 255)
					{
					i++;
					if(s[i] == 253 || s[i] == 254)
						{
						reply[reply_length++] = 255;
						reply[reply_length++] = s[i++] - 2;
						reply[reply_length++] = s[i++];
						}
					}
				}

			if(reply_length)
				nr_plugs_send(plug_handle,reply,reply_length,0);
			}
		}
	else
		{
		status->connected = 2; // let them know we are disconnected
		}
	
	return 0;
	}

// +---------------------
// | Once a session is established, either by
// | listening or connecting, we can come
// | here and trade keystrokes until
// | either they disconnect or we get an
// | ESC key.
// |

int r_telnet_session(int tcp_plug, volatile s_telnet_status *status)
	{
	int still_running;
	int result = 0;

	still_running = 1;
	while(still_running)
		{
		int c;

		if((c = nr_uart_rxchar(0)) > 0)
			{
			if(c == 27)
				still_running = 0;
			else
				{
				nr_plugs_send(tcp_plug,&c,1,0);
				status->bytes_sent++;
				}
			}

		if(status->connected == 2)
			still_running = 0;

		nr_plugs_idle();
		}
	
	result = nr_plugs_connect(tcp_plug,0,0,0);
	
	printf("\nConnection closed.\n");
	printf("%d bytes sent, %d bytes received.\n\n",status->bytes_sent,status->bytes_received);

go_home:
	return result;
	}

int r_telnet_accept(int x)
	{
	long local_port = 23;
	int result;
	int tcp_plug = 0;
	int still_running;
	s_telnet_status status = {0,0,0,0,0};

	result = r_input_long("Port to listen on",&local_port);
	if(result)
		goto go_home;

	result = nr_plugs_create
			(
			&tcp_plug,
			ne_plugs_tcp,
			local_port,
			tcp_proc,
			&status,
			0
			);

	result = nr_plugs_listen
			(
			tcp_plug,
			tcp_listen_proc,
			&status
			);
	
	still_running = 1;

	// |
	// | Wait for connection to be established, then
	// | go to the session routine. (Allow ESC to
	// | bail out.)
	// |

	while(status.connected == 0)
		{
		nr_plugs_idle();
		if(nr_uart_rxchar(0) == 27)
			{
			nr_plugs_idle();
			printf("\nAborted.\n");
			goto go_home;
			}
		}
	
	result = r_telnet_session(tcp_plug,&status);

go_home:
	if(tcp_plug)
		result = nr_plugs_destroy(tcp_plug);
	return 0;
	}

int r_telnet_connect(int x)
	{
	static char *default_host = "www.altera.com";
	char host[64];
	long remote_port = 23;
	int result;
	int tcp_plug = 0;
	int still_running;
	s_telnet_status status = {0,0,0,0,0};

	host[0] = 0;

	result = r_input_string("  Internet host to connect to",default_host,host);
	if(result)
		goto go_home;

	result = r_input_long("  Port to connect to",&remote_port);
	if(result)
		goto go_home;

	result = nr_plugs_create
			(
			&tcp_plug,
			ne_plugs_tcp,
			0, // let library choose a port number
			tcp_proc,
			&status,
			0
			);

	result = nr_plugs_connect
			(
			tcp_plug,
			host,
			0,
			remote_port
			);
	
	if(result)
		{
		printf("Error connecting to host %s.\n",host);
		nr_plugs_print_error_message("[tcp connect test]",result);
		goto go_home;
		}

	result = r_telnet_session(tcp_plug,&status);
go_home:
	if(tcp_plug)
		result = nr_plugs_destroy(tcp_plug);
	return 0;
	}

// +---------------------
// | Blink the Link LED for a bit,
// | then return.
// |

int r_blink_link_led(int x)
	{
	int i;
	
#ifdef na_lan91c111_0
	printf ("\n Link must be already active for the LAN91C111.\n");
#endif

	printf("\n\n  Blinking the LED...\n");
	for(i = 0; i < 10; i++)
		{
		nr_plugs_set_mac_led(0,0);
		nr_plugs_idle();
		nr_delay(100);
		nr_plugs_set_mac_led(0,1);
		nr_plugs_idle();
		nr_delay(100);
		}
	nr_plugs_set_mac_led(0,-1);
	printf("\n\n  Done blinking the LED.\n");
	}

// +---------------------
// | Guess what the next routine does
// |


#endif SMALL

int r_exit_to_monitor(int x)
	{
	nr_plugs_terminate();

	// Disable timer1, in case nr_timer_milliseconds() got used
#ifdef na_timer1
	na_timer1->np_timercontrol = np_timercontrol_stop_mask;
	na_timer1->np_timerstatus = 0; // clear any last irq
#endif

	exit(0);
	return 0; // (never happens)
	}

int main(void)
	{
	// Call initialization routines

	r_initialize();


	// Set up the menus, and run them

	r_menu_set_idler((r_menu_proc)nr_plugs_idle);

	// Settings menu
	r_menu_add_item(k_menu_settings,"Network Settings",show_settings,0); // 1st item is title
	r_menu_add_item(k_menu_settings,"Select Setup",select_settings,0);
	r_menu_add_item(k_menu_settings,"Save To Flash",r_save_settings,0);
	r_menu_add_item(k_menu_settings,"Enter New Settings",edit_settings,0);
	r_menu_add_item(k_menu_settings,"Reset All Settings",r_reset_settings,0);

	// Remote menu

	r_menu_add_item(k_menu_actions,"Network Actions",show_abbreviated_settings,0); // 1st item is title
#if !SMALL
	r_menu_add_item(k_menu_actions,"Ping Remote Host",r_ping_host,0);
	r_menu_add_item(k_menu_actions,"Ping Remote Host (Debug On)",r_ping_host,1);
	r_menu_add_item(k_menu_actions,"Look Up IP Address",r_dns_lookup,0);
#endif
	r_menu_add_item(k_menu_actions,"ARP Scan",r_arp_scan,0);
	r_menu_add_item(k_menu_actions,"Monitor All Traffic (Sniffer)",r_sniffer,0);
#if !SMALL
	r_menu_add_item(k_menu_actions,"Accept Telnet Connection",r_telnet_accept,0);
	r_menu_add_item(k_menu_actions,"Open Telnet Connection",r_telnet_connect,0);
	r_menu_add_item(k_menu_actions,"Blink Link LED",r_blink_link_led,0);
#endif

	// Main menu
	r_menu_add_item(0,"Main Menu",0,0);
	r_menu_add_link(0,k_menu_settings);
	r_menu_add_link(0,k_menu_actions);
	r_menu_add_item(0,"Exit to monitor",r_exit_to_monitor,0);

	r_menu_run();
	}


