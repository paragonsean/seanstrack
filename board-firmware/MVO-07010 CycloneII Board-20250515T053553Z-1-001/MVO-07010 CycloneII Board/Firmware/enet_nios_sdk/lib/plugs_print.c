// file: plugs_print.c
//
// Implementation of convenient packet-printing routines

#include "plugs.h"
#include "nios.h"

// Local Prototypes

static void d_print_payload(char *base, int length,char *title,char *user_title);


// Implementation


static void d_print_title(char *t,char *user_title)
	{
	int i = 0;
	char c;

	if(user_title)
		printf("%s",user_title);

	while((c = t[i++]) != 0)
		printf("%c",c);
	printf(":");
	
	while(i++ < 22)
		printf(" ");
	}

static void d_print_byte(char **wp,int decimal)
	{
	char *w = *wp;

	if(decimal)
		printf("%d",(*w++) & 0x00ff);
	else
		printf("%02x",(*w++) & 0x00ff);
	*wp = w;
	}

static void d_print_bytes(char **wp,int count,char breaker,int decimal)
	{
	char *w = *wp;
	int i;

	while(count--)
		{
		d_print_byte(wp,decimal);
        if(count)
		    printf("%c",breaker);
		}
    printf(" ");
	}

static void d_print_ethernet_address(char **wp)
	{
	d_print_bytes(wp,6,':',0);
	}

static void d_print_ip_address(char **wp)
	{
	d_print_bytes(wp,4,'.',1);
	}

static void d_print_32(char **wp)
	{
	d_print_bytes(wp,4,'_',0);
	}

static void d_print_16(char **wp)
	{
	d_print_bytes(wp,2,'_',0);
	}

static void d_print_8(char **wp)
	{
	d_print_bytes(wp,1,0,0);
	}

static void d_print_cr(void)
	{
	printf("\n");
	}

short d_print_ethernet_header(char **wp,char *user_title)
	{
	short result = nr_n2h16(((ns_plugs_ethernet_packet *)*wp)->type);

	d_print_title("eth",user_title);
	d_print_ethernet_address(wp);
	d_print_ethernet_address(wp);
	d_print_16(wp);
	d_print_cr();

	return result;
	}

static void d_print_arp_packet(char **wp,char *user_title)
	{
	d_print_title("arp",user_title);
	d_print_16(wp); // hw
	d_print_16(wp); // prot
	d_print_8(wp); // hw size
	d_print_8(wp); // prot size
	d_print_16(wp);  // cmd
	d_print_cr();

	d_print_title("arp sender",user_title);
	d_print_ethernet_address(wp);
	d_print_ip_address(wp);
	d_print_cr();

	d_print_title("arp target",user_title);
	d_print_ethernet_address(wp);
	d_print_ip_address(wp);
	d_print_cr();
	}

#define PLUGS_PRINT_PAYLOAD_MAX 128 // print at most this much of payload

static void d_print_payload_bytes(char **wp,int length,char *title,char *user_title)
	{
	int skipped = 0;
	int show = length;

	if(show > PLUGS_PRINT_PAYLOAD_MAX + 32)	// elasticity of 32
		{
		skipped = show - PLUGS_PRINT_PAYLOAD_MAX;
		show = PLUGS_PRINT_PAYLOAD_MAX / 2; // show this many at front & back
		}
	d_print_payload(*wp,show,title,user_title);
	*wp += show;

	if(skipped)
		{
		d_print_title("skipping",user_title);
		printf("(%d)\n",skipped);
		*wp += skipped;
		d_print_payload(*wp,show,title,user_title);
		*wp += show;
		}
	}

static void d_print_icmp_packet(char **wp,int length,char *user_title)
	{
	int i;

	d_print_title("icmp typ,cod,cs",user_title);
	d_print_8(wp);
	d_print_8(wp);
	d_print_16(wp);
	d_print_cr();

	d_print_payload_bytes(wp,length - 4,"icmp payload",user_title);
	}

static void d_print_udp_packet(char **wp,int length,char *user_title)
	{
	d_print_title("udp src/dst port",user_title);
	d_print_16(wp);
	d_print_16(wp);
	d_print_cr();

	d_print_title("udp len cs",user_title);
	d_print_16(wp);
	d_print_16(wp);
	d_print_cr();

	length -= 8;
	if(length)
		d_print_payload_bytes(wp,length,"udp payload",user_title);
	}

static void d_print_tcp_packet(char **wp,int length,char *user_title)
	{
	int header_length;
	int i;
	host_16 bits;

	d_print_title("tcp src/dst port",user_title);
	d_print_16(wp);
	d_print_16(wp);
	d_print_cr();

	d_print_title("tcp seq ack",user_title);
	d_print_32(wp);
	d_print_32(wp);
	d_print_cr();

	d_print_title("tcp hl/bits ws",user_title);

	bits = **(net_16 **)wp;
	bits = nm_n2h16(bits);

	header_length = (bits >> 12) & 0x0f;	// number of net_32's
	d_print_16(wp);
	d_print_16(wp);

	printf(" (");
	if(bits & ne_plugs_flag_tcp_fin)
		nr_uart_txchar('F',0);
	if(bits & ne_plugs_flag_tcp_syn)
		nr_uart_txchar('S',0);
	if(bits & ne_plugs_flag_tcp_rst)
		nr_uart_txchar('R',0);
	if(bits & ne_plugs_flag_tcp_psh)
		nr_uart_txchar('P',0);
	if(bits & ne_plugs_flag_tcp_ack)
		nr_uart_txchar('A',0);
	if(bits & ne_plugs_flag_tcp_urg)
		nr_uart_txchar('U',0);
	printf(")");

	d_print_cr();


	d_print_title("tcp cs urg",user_title);
	d_print_16(wp);
	d_print_16(wp);
	d_print_cr();

	for(i = 5; i < header_length; i++)
		{
		d_print_title("tcp option",user_title);
		d_print_32(wp);
		d_print_cr();
		}

	length -= (header_length * 4);
	
	if(length)
		d_print_payload_bytes(wp,length,"tcp payload",user_title);
	}
	
static void d_print_ip_packet(char **wp,char *user_title)
	{
	int protocol;
	int header_length;
	int total_length;
	int payload_length;
	char *payload_start;
	int i;

	d_print_title("ip ver/hl,tos,len",user_title);
	header_length = (**wp) & 0x000f; // 4 bits = number of net_32's
	d_print_8(wp); // version, header length
	d_print_8(wp); // tos
	total_length = nr_n2h16(**(net_16 **)wp);
	d_print_16(wp); // length
	d_print_cr();

	d_print_title("ip id frg",user_title);
	d_print_16(wp); // id
	d_print_16(wp); // frag
	d_print_cr();

	d_print_title("ip ttl,prt,cs",user_title);
	d_print_8(wp); // ttl
	protocol = **wp;
	d_print_8(wp); // protocol
	d_print_16(wp); // checksum
	d_print_cr();

	d_print_title("ip source",user_title);
	d_print_ip_address(wp); // source ip address
	d_print_cr();

	d_print_title("ip destination",user_title);
	d_print_ip_address(wp); // destination ip address
	d_print_cr();

	for(i = 5; i < header_length; i++)
		{
		d_print_title("ip option",user_title);
		d_print_32(wp);
		d_print_cr();
		}
	
	payload_length = total_length - (header_length * 4);
	payload_start = *wp;

	switch(protocol)
		{
		case ne_plugs_ip_icmp:
			d_print_icmp_packet(wp,payload_length,user_title);
			break;

		case ne_plugs_ip_udp:
			d_print_udp_packet(wp,payload_length,user_title);
			break;

		case ne_plugs_ip_tcp:
			d_print_tcp_packet(wp,payload_length,user_title);
			break;
		}
	
	payload_length -= (*wp) - payload_start;
	if(payload_length)
		d_print_payload_bytes(wp,payload_length,"ip payload",user_title);
	}

static void d_print_payload(char *base, int length,char *title,char *user_title)
	{
	int i = 0;
	int perLine = 16;

	if(length < 1 || length > 1500)
		{
		printf("d_print_payload: request to print illegal packet size %d\n",length);
		goto goHome;
		}

	while(length --)
		{
		if(!(i % perLine))
			d_print_title(title,user_title);
		printf("%02x ",(*base++) & 0x00ff);
		i++;
		if(i % perLine == 0)
			printf("\n");
		}
	if(i % perLine)
		printf("\n");
goHome:
	return;
	}

void nr_plugs_print_ip_address_decimal(net_32 ip_address)
	{
	unsigned char *w;

	w = (unsigned char *)&ip_address;
	printf("%d.",*w++);
	printf("%d.",*w++);
	printf("%d.",*w++);
	printf("%d",*w);
	}

void nr_plugs_print_ip_address(net_32 ip_address)
	{
	char *w;

	w = (char *)&ip_address;
	d_print_ip_address(&w);
	}

void nr_plugs_print_ethernet_address(net_48 *ethernet_address)
	{
	d_print_ethernet_address((char **)&ethernet_address);
	}

void nr_plugs_print_ethernet_packet(ns_plugs_ethernet_packet *p, int length,char *user_title)
	{
	short t;
	char *base;
	char *base_end;

	base = (char *)p;
	base_end = base + length;

	if(length < 1 || length > 1500)
		{
		printf("nr_plugs_print_packet: request to print illegal packet size %d\n",length);
		goto goHome;
		}

	t = d_print_ethernet_header(&base,user_title);
	length -= 14;

	if(t == ne_plugs_ethernet_arp)
		d_print_arp_packet(&base,user_title);
	else if(t == ne_plugs_ethernet_ip)
		d_print_ip_packet(&base,user_title);
	
	length = base_end - base;
	if(length)
		d_print_payload_bytes(&base,length,"eth payload",user_title);

goHome:
	return;
	}

void nr_plugs_print_icmp_packet(ns_plugs_icmp_packet *p, int length, char *user_title)
	{
	d_print_icmp_packet((char **)&p,length,user_title);
	}


void nr_plugs_ip_to_string(net_32 ip, char *s)
	{
	unsigned char *octet = (unsigned char *)&ip;

	sprintf(s,"%d.%d.%d.%d",
			octet[0],
			octet[1],
			octet[2],
			octet[3]);
	}

void nr_plugs_ethernet_to_string(net_48 *ethernet, char *s)
	{
	unsigned char *octet = (unsigned char *)ethernet;

	sprintf(s,"%02x:%02x:%02x:%02x:%02x:%02x",
			octet[0],
			octet[1],
			octet[2],
			octet[3],
			octet[4],
			octet[5]);
	}

long nr_plugs_string_to_long(char *s)
	{
	long x;
	int c;

	x = 0;
	while((c = *s++) != 0)
		{
		if(c == '-')
			x = -x;
		else if(c >= '0' && c <= '9')
			x = x * 10 + c - '0';
		}
	
	return x;
	}

void nr_plugs_long_to_string(long x,char *s)
	{
	sprintf(s,"%d",x);
	}

static void d_plugs_string_to_bytes(char *s,unsigned char *bytes_out,int byte_count,int radix)
	{
	host_32 ip = 0;
	unsigned char *octet = bytes_out;
	int x;
	unsigned int c;
	unsigned int v; // value under construction

	x = 0;
	v = 0;
	while(1)
		{
		c = *s++;

		if(c < '0' || c == ':') // catch '.' or ':' or 0 (end of string)
			{
			*octet++ = v;
			v = 0;
			x++;
			if((x >= byte_count) || (c == 0))
				break;
			}
		else
			{
			// convert c to int, hex or decimal, loosely
			c -= '0';
			if(c > 9)
				c = (c + '0' - 'A' + 10) & 0x0f;
			v = (v * radix) + c;
			}

		ip = (ip & 0xffffff00) +
			(((ip & 0x000000ff) * 10 + c - '0') & 0x000000ff);
		}
	}

void nr_plugs_string_to_ip(char *s,net_32 *ip_out)
	{
	d_plugs_string_to_bytes(s,(unsigned char *)ip_out,4,10);
	}

void nr_plugs_string_to_ethernet(char *s,net_48 *ethernet_out)
	{
	d_plugs_string_to_bytes(s,(unsigned char *)ethernet_out,6,16);
	}

// +--------------------------------------
// | This routine prints an error message with
// | the kind of error, if known, and only if
// | the error parameter is nonzero.

void nr_plugs_print_error_message(char *caption, int error)
	{
	char *s = 0;

#define rpe(x) case x: s = #x; break;

	if(error < 0)
		{
		switch(error)
			{
			rpe(ne_plugs_error)
			rpe(ne_plugs_error_first)
			rpe(ne_plugs_error_not_initialized)
			rpe(ne_plugs_error_feature_disabled)
			rpe(ne_plugs_error_too_many_plugs)
			rpe(ne_plugs_error_unwanted_reply)
			rpe(ne_plugs_error_dns_not_found)
			rpe(ne_plugs_error_dns_timed_out)
			rpe(ne_plugs_error_arp)
			rpe(ne_plugs_error_arp_timed_out)
			rpe(ne_plugs_error_tcp_connection_refused)
			rpe(ne_plugs_error_tcp_timed_out)
			rpe(ne_plugs_error_dhcp_failed)
		
			default:
				s = "unknown";
				break;
			}

		// |
		// | print <caption><space><error> or just <error> if no caption
		// |

		printf("%s%sError %d: %s\n",caption ? caption : "",caption ? " " : "",error,s);
		}
	}


