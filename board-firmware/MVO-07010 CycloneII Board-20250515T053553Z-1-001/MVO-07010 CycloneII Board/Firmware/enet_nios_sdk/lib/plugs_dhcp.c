

#define P(x) printf("%s\n",#x)

#include "excalibur.h"
#include "plugs.h"
#include "plugs_private.h"

#include <stdarg.h>

#define k_initial_timeout_msec 1000
#define k_retry_count 60

// |
// | DHCP is "Dynamic Host Configuration Protocol".
// |
// | It is described in:
// |     RFC 1533
// |     RFC 2131
// |

// |
// |           Value   Message Type
// |           -----   ------------
// |             1     DHCPDISCOVER
// |             2     DHCPOFFER
// |             3     DHCPREQUEST
// |             4     DHCPDECLINE
// |             5     DHCPACK
// |             6     DHCPNAK
// |             7     DHCPRELEASE

enum
    {
    ne_plugs_udp_dhcp_server = 67,
    ne_plugs_udp_dhcp_client  = 68,
    ne_plugs_dhcp_signature  = (0x63825363),
    };

#define nm_packed __attribute__ ((packed,aligned(2)))

typedef struct
    {
    net_8 opcode;                      // | 1: request, 2: reply
    net_8 hardware_type;
    net_8 hardware_address_size;
    net_8 hop_count;
    net_32 transaction_id;
    net_16 number_of_seconds;
    net_16 unused_1;
    net_32 client_ip_address;          // | 0 if unknown
    net_32 your_ip_address;            // | address assigned by server
    net_32 server_ip_address;          // | 0 if unknown
    net_32 gateway_ip_address;         // | 0 if unknown
    net_48 client_ethernet_address;    // | (first 6 bytes of 16)
    char unused_2[10];                 // | since we only do ethernet
    char server_hostname[64];
    char boot_filename[128];
    net_32 dhcp_signature;             // | 0x63825363, network byte ordering
    char vendor_options[60];
    } nm_packed ns_plugs_dhcp_packet;  // | payload of a udp packet


enum
    {
    e_dhcp_state_nada = 0,            // just starting
    e_dhcp_state_wf_offer,            // sent "discover", waiting for an offer
    e_dhcp_state_got_offer,           // set by receive-proc, if wf_offer and one comes in
    e_dhcp_state_wf_ack,              // got offer, sent request, waiting for ack
    e_dhcp_state_got_ack,             // set by receive-proc, if wf_ack and one comes in
    e_dhcp_state_happy,               // got some settings, all done
    e_dhcp_state_failed,              // too many retries and timeouts. Forget it.

    e_dhcp_state_null,                // waiting for a timeout
    };



// +------------------------
// | s_dhcp_offer
// |
// | the various fields that define an offer
// | from a dhcp server that we might accept.
// |
// | When we get one we like, we stash this whole
// | offer, and make sure that future interactions are
// | performed only with the server from which we
// | wish to accept an offer.
// |

typedef struct
    {
    ns_plugs_network_settings ns;   // settings offered from server
    net_32 server_ip_address;
    long ip_lease_time;
    } s_dhcp_offer;

typedef struct
    {
    int state;               // from enum above, e_dhcp_state_x
    int adapter_index;       // which adapter we're using
    net_48 ethernet_address; // our own ethernet address, for safe keeping

    s_dhcp_offer offer;      // properties of a received offer

    long retry;              // number of the retry
    unsigned long timeout;   // how long to wait after we send packet
    long last_time;          // msec of last state change, for timeout

    int plug_handle;
    } s_dhcp_state;

static int r_dhcp_new(s_dhcp_state *ds,int adapter_index);
static int r_dhcp_idle(s_dhcp_state *ds,s_dhcp_offer *offer_out);
static int r_dhcp_free(s_dhcp_state *ds);

#if PLUGS_DEBUG

static void r_dhcp_print_offer(s_dhcp_offer *offer_p,char *note)
    {
    printf("%s from server ",note);
    nr_plugs_print_ip_address(offer_p->server_ip_address);

    printf(", for ip address ");
    nr_plugs_print_ip_address(offer_p->ns.ip_address);

    printf(", lease time %ld\n",offer_p->ip_lease_time);
    }

#endif



// +-----------------------------
// | r_read_net_32
// |
// | This is a stupid routine for copying
// | four bytes from a char * to a long. 
// |
// | (because if you just cast it, it doesnt
// | work if the low two address bits are
// | 1, 2, or 3. la la la.)
// |

net_32 r_read_net_32(unsigned char *w)
    {
    net_32 x;
    unsigned char *xp = (unsigned char *)&x;

    *xp++ = *w++;
    *xp++ = *w++;
    *xp++ = *w++;
    *xp = *w;

    return x;
    }

static int dhcp_callback_proc
        (
        int plug_handle,
        void *context,
        ns_plugs_packet *p,
        void *payload,
        int payload_length
        )
    {
    s_dhcp_state *ds = context;
    ns_plugs_dhcp_packet *dp = payload;
    ns_plugs_udp_packet *udpp = p[ne_plugs_udp].header;
    unsigned char *w;
    s_dhcp_offer offer;

    int message_type = 0; // we care about "offer"=2, "ack"=5, "nak"=6

    nr_zerorange((char *)&offer,sizeof(s_dhcp_offer));

    // |
    // | do some misc checks that bail out if we think
    // | we dont want the packet
    // |

    if(udpp->source_port != nm_h2n16(ne_plugs_udp_dhcp_server))
        {
        goto go_home;
        }
    if(udpp->destination_port != nm_h2n16(ne_plugs_udp_dhcp_client))
        {
        goto go_home;
        }
    if(dp->client_ethernet_address.u32 != ds->ethernet_address.u32)
        {
        goto go_home;
        }
    if(dp->client_ethernet_address.l16 != ds->ethernet_address.l16)
        {
        goto go_home;
        }

    // |
    // | ok the packet is for us
    // | so now we scan the vendor options
    // |

    w = dp->vendor_options;

    // |
    // | All these constants come from RFC 1533
    while(*w != 0xff)
        {
        int option_code = *w++;
        int option_length = *w++;

        if(option_code == 53)
            {
            message_type = w[0];

            if(message_type == 2) // "offer"
                offer.ns.ip_address = dp->your_ip_address;
            }
        else if(option_code == 54)
            {
            offer.server_ip_address = r_read_net_32(w); 
            }
        else if(option_code == 51)
            {
            offer.ip_lease_time = nr_n2h32(r_read_net_32(w));
            }
        else if(option_code == 1)
            {
            offer.ns.subnet_mask = r_read_net_32(w);
            }
        else if(option_code == 3)
            {
            offer.ns.gateway_ip_address = r_read_net_32(w);
            }
        else if(option_code == 6)
            {
            offer.ns.nameserver_ip_address = r_read_net_32(w);
            }

        w += option_length;
        }

    // |
    // | Now, by the message_type, was it the kind of message we were waiting for?
    // |

    if(message_type == 2)
        {
        if(ds->state == e_dhcp_state_wf_offer)
            {
            ds->offer = offer;
            ds->state = e_dhcp_state_got_offer;
#if PLUGS_DEBUG
            r_dhcp_print_offer(&ds->offer,"[accepting offer]");
#endif
            }
        else
            {
#if PLUGS_DEBUG
            r_dhcp_print_offer(&offer,"[ ignoring offer]");
#endif
            }
        }
    else if(message_type == 5 && ds->state == e_dhcp_state_wf_ack)
        {
        // |
        // | we are waiting for an ACK on our accepted offer,
        // | and this packet is indeed an ACK, but is it from
        // | the server we want? If so, we are all done.
        // |

        if(offer.server_ip_address == ds->offer.server_ip_address)
            {
            ds->state = e_dhcp_state_got_ack;
            }
        // else, ignore this ACK
        }

go_home:

    return 0;
    }




// +---------------------------------------------------------
// | append_dhcp_message(dhcp_packet, option code, [args*])
// |
// | add another "vendor options" entry to a dhcp packet
// |
// | the args* list is interpreted like so:
// |
// |                     0-255: add a byte
// |                        -1: end of list
// | any other negative number: treat next arg as char *, and add that many bytes
// | (there is no way to say a pointer to one byte.)
// |


static void append_dhcp_message(ns_plugs_dhcp_packet *dp,int option_code, ...)
    {
    va_list args;
    unsigned char *w;
    int v;
    int option_length = 0;
    char *option_length_p;   // second byte of section

    // |
    // | First, find the current end-of-options
    // |

    w = dp->vendor_options;
    while(*w != 0xff)
        w++;                // w now hovering over the old 0xff end-marker

    option_length_p = w + 1;   // second byte of section

    *w++ = option_code;  // first byte is dhcp message tag thingie
    w++;                  // we fill in the length later

    va_start(args,option_code);

    while((v = va_arg(args,int)) != -1)
        {
        if(v < 0)
            {
            // | next arg is pointer with some bytes for us...
            unsigned char *p = va_arg(args,unsigned char *);
            v = -v;

            while(v-- > 0)
                {
                *w++ = *p++;
                option_length++;
                }
            }
        else
            {
            option_length++;
            *w++ = v;
            }
        }

    *option_length_p = option_length;  // fill in the size

    *w++ = 0xff;         // re-mark the "end of options"
    }




// +----------------------------------
// | r_dhcp_new
// |
// | initialize plugs to a neutral and safe setting,
// | and mark our state as "nada"

static int r_dhcp_new(s_dhcp_state *ds,int adapter_index)
    {
    int result;

    // we use the plugs globals "force_timeout" member
    // to allow a public API that halts the synchronous
    // activity

    ng_plugs_globals.force_timeout = 0;

    nr_zerorange((char *)ds,sizeof(s_dhcp_state)); // clear out the state
    ds->adapter_index = adapter_index;

        {
        ns_plugs_network_settings ns;
        result = nr_plugs_get_settings(ds->adapter_index,&ns);
        if(result)
            goto go_home;

        ds->ethernet_address = ns.ethernet_address;
        }

    ds->state = e_dhcp_state_nada;
    ds->timeout = k_initial_timeout_msec;
    ds->retry = 0;

    // |
    // | create our UDP plug
    // |

    result = nr_plugs_create
            (
            &ds->plug_handle,
            ne_plugs_udp,ne_plugs_udp_dhcp_client,
            dhcp_callback_proc,ds,
            ne_plugs_flag_ip_all | ne_plugs_flag_ethernet_broadcast
                    //+ ne_plugs_flag_debug_rx
            );
    if(result < 0)
        goto go_home;

    // |
    // | accept all packets
    // | since we dont really have an IP address yet
    // | nor do we really know who we shall be talking with
    // |

    result = nr_plugs_listen (ds->plug_handle,0,0);
    if(result < 0)
        goto go_home;

go_home:
    if(result)
        nr_plugs_print_error_message("[r_dhcp_new]: ",result);
    
    return result;
    }

// |
// | Utility to fill out a dhcp request packet with the
// | parts we know about. Most of the options must still
// | be filled out by the caller.
// |

static int r_fill_out_dhcp_packet(s_dhcp_state *ds,ns_plugs_dhcp_packet *dp,int message_type)
    {
    int result = 0;
    nr_zerorange((char *)dp,sizeof(ns_plugs_dhcp_packet));

    dp->opcode = 1;
    dp->hardware_type = 1;
    dp->hardware_address_size = 6;

    dp->dhcp_signature = nm_h2n32(ne_plugs_dhcp_signature);
    dp->vendor_options[0] = 0xff;

    // |
    // | get the ethernet address from the current network settings
    // |

    if(result < 0)
        goto go_home;

    dp->client_ethernet_address = ds->ethernet_address;

    append_dhcp_message(dp,53,message_type,-1);     // dhcp message type, request
    append_dhcp_message(dp,61,                      // client identifier,
            1,                                      // hardware type 1 (ethernet)
            -6,&dp->client_ethernet_address,        // 6 bytes of ethernet
            -1);

go_home:
    return result;
    }


// +----------------------------------
// | r_dhcp_idle
// |
// | depending on state, we might send out a packet and set some
// | variables and such
// |

static int r_dhcp_idle(s_dhcp_state *ds,s_dhcp_offer *offer_out)
    {
    int result = 0;
    ns_plugs_dhcp_packet dhcp_packet;
    int send_the_packet = 0;
    int state_after_send = 0;
    unsigned long time_now = nr_timer_milliseconds();

    // |
    // | Has there been a forced timeout?
    // | If so, we claim that DHCP failed
    // |

    nr_plugs_idle();

    if(ng_plugs_globals.force_timeout)
        {
        ds->state = e_dhcp_state_failed;
        goto go_home;
        }

    // |
    // | First, a cascaded if-statement to handle the various
    // | possible "state" values.
    // |
    // | Why isn't this a switch() statement? I dunno, just felt
    // | like using if/else if.
    // |

    if(ds->state == e_dhcp_state_nada)
        {
        // |
        // | The Beginning: send out our dhcp "discover" message
        // |

        result = r_fill_out_dhcp_packet(ds,&dhcp_packet,1);     // "discover"
        if(result)
            goto go_home;

        append_dhcp_message(&dhcp_packet,55,1,3,6,-1); // request list, subnet mask, routers, name servers
        
        send_the_packet= 1;
        state_after_send = e_dhcp_state_wf_offer;
        }
    else if(ds->state == e_dhcp_state_got_offer)
        {
        // |
        // | We sent out a "discover", and some server
        // | send out an "offer".
        // |
        // | We will now take the offer, and send out 
        // | a "request" that echoes what was in 
        // | the offer.
        // |

        r_fill_out_dhcp_packet(ds,&dhcp_packet,3);  // "request"

        // |
        // | request the ip address they offered
        // |
        append_dhcp_message(&dhcp_packet,50,-4,&ds->offer.ns.ip_address,-1); //"request",ip address

        // |
        // | server id -- who answered our "discover"
        // |
        append_dhcp_message(&dhcp_packet,54,-4,&ds->offer.server_ip_address,-1);

        // |
        // | request list (same list we asked for in "discover"
        // |
        append_dhcp_message(&dhcp_packet,55,1,3,6,-1); // request list, subnet mask, routers, name servers

        send_the_packet = 1;
        state_after_send = e_dhcp_state_wf_ack;
        }
    else if(ds->state == e_dhcp_state_got_ack)
        {
        // |
        // | all done! the ack'd our inquiry
        // |

        ds->state = e_dhcp_state_happy;
        }
    else if((ds->state == e_dhcp_state_happy)
            || (ds->state == e_dhcp_state_failed))
        {
        // nop. stay happy or failed
        }
    else
        {
        // |
        // | fell through to here? just check the 
        // | the time, and if too much time has passed,
        // | reset the state to "nada" and start the whole
        // | darned thing over.
        // |
        if(time_now - ds->last_time > ds->timeout)
            {
            ds->last_time = time_now;
            ds->retry ++;
#if PLUGS_DEBUG
            printf("[dhcp] %d timing out\n",ds->retry);
#endif
            
            if(ds->retry < k_retry_count)
                ds->state = e_dhcp_state_nada;
            else
                ds->state = e_dhcp_state_failed;
            }
        }
 
    // |
    // | depending what happened above, we might
    // | have a packet to send, and then the need to
    // | bump our state to some "_wf_" (waiting-for).
    // |
    // | (we only bump the state if the packet goes out
    // | without an error.)
    // |

    if(send_the_packet)
        {
        // |
        // | we send all our dhcp<-->server messages as 
        // | ethernet broadcast packets so that any other
        // | servers will be in on the conversation, and know
        // | that we have not chosen them.
        // |

        result = nr_plugs_send_to
                (
                ds->plug_handle,
                &dhcp_packet,sizeof(dhcp_packet),
                ne_plugs_flag_ethernet_broadcast,
                -1,
                nm_h2n16(ne_plugs_udp_dhcp_server)
                );
        if(result < 0)
            goto go_home;

        ds->state = state_after_send;
        ds->last_time = time_now;
        }
        
go_home:
    if(result < 0)
        {
        nr_plugs_print_error_message("[r_dhcp_idle]: ",result);

        // | on an error, we reset our state to "null", which ends up
        // | waiting for a timeout and then going to "nada" and
        // | starting all over again.
        ds->state = e_dhcp_state_null;
        ds->last_time = time_now;
        }
    else
        {

        // |
        // | return values:
        // |    x < 0: dhcp has really failed
        // |        0: whatever, keep trying
        // | positive: duration of lease
        // |

        if(ds->state == e_dhcp_state_happy)
            {
            result = ds->offer.ip_lease_time;
            if(result < 0)
                result = 1;
            *offer_out = ds->offer;
            }
        else if(ds->state == e_dhcp_state_failed)
            {
            result = ne_plugs_error_dhcp_failed;
            }
        // (else, result is already zero.)
        }

    return result;
    }

// +----------------------------------
// | close down the plug, and that is all
// |
static int r_dhcp_free(s_dhcp_state *ds)
    {
    int result = 0;

    if(ds->plug_handle)
        result = nr_plugs_destroy(ds->plug_handle);

    return result;
    }

// +-------------------------
// | this is the main dhcp routine. It should be called
// | with the adapter initialized, but no support plugs
// | (ping, dns, whatever) created.
// |

int r_plugs_dhcp(int adapter_index,ns_plugs_network_settings *settings_out)
    {
    int result;
    ns_plugs_network_settings settings;
    s_dhcp_state ds;
    s_dhcp_offer offer;

    result = r_dhcp_new(&ds,adapter_index);
    if(result < 0)
        goto go_home;

    while(1)
        {
        result = r_dhcp_idle(&ds,&offer);
        if(result != 0)
            goto go_home;
        }

go_home:

    // snatch the ethernet address before freeing the ds

    offer.ns.ethernet_address = ds.ethernet_address;
    offer.ns.flags = ne_plugs_flag_dhcp; // indicates the settings came via dhcp

    r_dhcp_free(&ds);  // dont log the error from this
#if PLUGS_DEBUG
    nr_plugs_print_error_message("[r_plugs_dhcp] ",result);
#endif

    *settings_out = offer.ns;
    return result;
    }

// end of file
