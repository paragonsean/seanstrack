// | file: plugs_private.h
// |
// | Internal structures for
// | the "plugs" embedded tcp/ip stack
// | library.
// | Applications should not need to
// | access these structures.
// |
// | David Van Brink / 2001 / Altera Corporation
// |

#ifndef _plugs_private_
#define _plugs_private_

#include "plugs.h"
#include "nios.h"

// +-------------------------------------
// | Low level constants & tuning

#define nk_plugs_adapter_count 1
#define nk_plugs_plug_number_base 0x2300 // arbitrary
#define nk_plugs_arp_cache_count 8
#define nk_plugs_arp_retry_interval 1009 // milliseconds between retries
#define nk_plugs_arp_retry_count 4       // most requests to send
#define nk_plugs_dns_retry_interval 1900 // milliseconds between dns queries
#define nk_plugs_dns_retry_count 5
#define nk_plugs_tcp_retry_interval 1355
#define nk_plugs_tcp_retry_count 3

// +-------------------------------------
// | Private flags

enum
	{
	ne_plugs_flag_tcp_checksum = 0x1000, // Internal _send_ flag, treats packet as tcp+header
	};

// +-------------------------------------
// | TCP states

enum
	{
	e_plugs_tcp_inert = 0,     // plug created, doing nothing
	e_plugs_tcp_listening,     // waiting for incoming SYN
	e_plugs_tcp_connecting,    // waiting for answering SYN
	e_plugs_tcp_connected,     // session established
        e_plugs_tcp_disconnecting  // Waiting for reply to our FIN
	};

// +-------------------------------------
// | State for each plug

typedef struct
	{
	int protocol;		// | from our enum, udp, arp, &c. 0 means "unused".
	nr_plugs_receive_callback_proc callback_proc;
	void *callback_context;

	// |
	// | acceptance filters, to listen
	// | to trim out some of packets.
	// | This also specified what we're "connected" to.
	// |
	net_16 local_port;
	net_32 remote_ip_address;		// | (ip/port is treated as MAC addr for ethernet plug)
	net_16 remote_port;
	long flags;		// | accept-all, accept-broadcast

	// |
	// | tcp state information
	// |

#if PLUGS_TCP
	int tcp_state;
        int tcp_manual_window_size;
        int tcp_remote_window_size;
        int tcp_flow_off;                      // used for manual
                                               // control mode.
                                               // Forces ack-packet
                                               // window size to zero
              
	long tcp_connection_time;
	nr_plugs_listen_callback_proc tcp_listen_proc;
	void *tcp_listen_proc_context;
	host_32 tcp_sn_local;			// | local sequence number
	host_32 tcp_sn_remote;			// | remote sequence number

          // Last local sequence number that we've seen
          // acknowledged by the remote side.
        host_32 tcp_last_acknowledged_sn_local;
#endif

	// |
	// | statistics for plug
	// |
	
	long packets_read;
	long bytes_read;
	long packets_lost;
	long bytes_lost;

	} s_plugs_plug;

// +---------------------------------
// | arp cache -- where we remember associations of
// | IP addresses to ethernet addresses
typedef struct
	{
	net_32 ip_address;					// | if zero, is available cache spot
	net_48 ethernet_address;				// | if zero, is unknown
	} s_plugs_arp_cache_entry;

// +-------------------------------------------
// | each adapter -- gets its own ping and arp
// | plug, and hardware info
// |

typedef struct
	{
	ns_plugs_network_settings settings;
	void *adapter_address;
	int adapter_irq;
	ns_plugs_adapter_description *adapter_description;

	int arp_plug; // | handler for arp requests

#if PLUGS_PING
	int icmp_plug; // | handler for ping requests
#endif

	// | arp cache
	// |
	s_plugs_arp_cache_entry arp_cache[nk_plugs_arp_cache_count];
	int arp_cache_walker;				// | position for next arp-entry

	ns_plugs_adapter_storage adapter_storage;
	} s_plugs_adapter_entry;


// +-------------------------------------------
// | Global State
typedef struct
	{
	int running;				// | nonzero means we're running

	int adapter_count;
	s_plugs_adapter_entry adapter[PLUGS_ADAPTER_COUNT];

	long packets_read;
	long bytes_read;
	long packets_lost;
	long bytes_lost;
	long packets_sent;
	long bytes_sent;

	host_16 ip_identifier;			// | increments 1 for every ip packet
	host_16 port_number;			// | incrementing port number

	s_plugs_plug plug[PLUGS_PLUG_COUNT];
	int in_dispatch;			// | to prevent reentrency
	int in_idle;				// | another reentrancy (combine these?)
	
#if PLUGS_DNS
	// | Support for DNS queries
	int dns_plug; // | handler for dns queries
	int dns_identifier; // | id for current outstanding query
	int dns_query_state; // | 0: waiting, 2: answered ok, 3: answered error
	net_32 dns_query_result; // | ip address returned from query
#endif

    // | Support for Ethernet Debugging
    int interruptee_pc;         // When ISR is active, save return addr
    int exclusive_plug_handle;  // Sometimes its cool to only monitor
                                // a single plug, store that plug here

    int force_timeout;          // the synchronous dhcp can aborted by
                                // setting this to nonzero
	} s_plugs_globals;

extern s_plugs_globals ng_plugs_globals;

// +----------------------------------------------------------
// | Low level internal routines (called by the adapter)

int dispatch_packet(ns_plugs_ethernet_packet *packet,int packet_length);

// | the main (internal-only) dhcp routine

int r_plugs_dhcp(int adapter_index,ns_plugs_network_settings *settings_out);

#endif // _plugs_private_

// end of file
