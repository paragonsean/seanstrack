//					tb9.c
//
//			Copyright 2003,2004,2005 Visi-Trak Worldwide

//#include "nios.h"
//#include "vtw_global2.h // Defines in this file.

/*
 tb4.c version in use prior to fixing conversion speed problems with upper channels.

 8-16-05 tb5.c
 1,	Reenabled upper channel conversion in convert_ana_ch_from_dynamic() after fixing problem
	with upper channels.

 2,	Simplified convert_upper_channels() and convert_all_channels() since upper channels are always 
	converted regardless of analog mode.

 7-14-06 tb7.c
	Modified the convert_ana_ch_from_dynamic() function to take into account the analog mode in arriving at the
	result.  Previously this was not done and the results from the upper channels were incorrect.

 1-16-07 still tb7.c
	Added itoa function.

	Quit declaring pointers for Nios peripherals.  Now just use the definitions in excalibur.h

  2-22-07 tb9.c
	Added a new simplified htoi and a new Atoi function.  Added ATOI and HTOI macros.
*/

#ifndef NULL
 #define NULL 0
#endif

#ifndef TURN_ON
	#define TURN_ON 1
#endif

#ifndef	TURN_OFF
	#define TURN_OFF 0
#endif	
	
#ifndef ON
	#define ON 1
#endif	
	
#ifndef OFF	
	#define OFF 0
#endif

#ifndef VOLTAGE_X_1000
	#define VOLTAGE_X_1000 1
#endif	
	
#ifndef RAW_VAL	
	#define RAW_VAL 2
#endif

#define OUT_DAC_DEBUG 0

int set_clr_discrete_output(int output_num, int action)
	{
	int return_val=0;
		
	if (output_num < 100) // Opto discrete I/O
		{
		// Discrete Opto outputs are 1-16 (monitor) and 33-48 (control).	
		if (output_num > 0 && output_num < 17) // Monitor outputs
			{
			if (action >= TURN_ON)			
				OSW1 |= 1<<(output_num-1); // OSW1 bits 0-15
			else
				OSW1 &= ~(1<<(output_num-1)); 	
			}
		else if (output_num > 32 && output_num < 49) // Control output
			{
			if (action >= TURN_ON)	
				OSW1 |= 1<<(output_num-17); // OSW1 bits 16-31
			else
				OSW1 &= ~(1<<(output_num-17)); 	
			}
		else return_val=1; // I/O number not valid			
		}
	else if (output_num < 200) // Opto Ethernet outputs
		{
		// 101-132 outputs numbers correspond to OSW2 bits 0-31.
		// No distinction is made between "monitor" and "control" outputs here.	
		if (output_num > 100 && output_num < 133)
			{
				
			}
		else return_val=1; // I/O number out of range.	
		}	
	else if (output_num < 300) // PLC Ethernet outputs
		{
		// 201-232 output numbers correspond to OSW3 bits 0-31.
		// No distinction is made between "monitor" and "control" outputs here.
		if (output_num > 200 && output_num < 233)
			{
				
			}
		else return_val=1; // I/O number out of range.
		}
	else return_val=1; // I/O number not valid
		
	return(return_val);	
	}


// This function will check the status of the passed input number and return
// 1 (ON) or 0 (OFF).
int chk_discrete_input(int input_num, int *input_state)
	{
	int return_val=0;	
			
	if (input_num < 100) // Opto discrete I/O
		{
		if (input_num > 16 && input_num < 33) // Monitor inputs
			{
			if (ISW1 & (1<<(input_num-17))) *input_state=ON; // ISW1 bits 0-15
			else *input_state=OFF;
			}
		else if (input_num > 48 && input_num < 65) // Control inputs
			{
			if (ISW1 & (1<<(input_num-33))) *input_state=ON; // ISW1 bits 16-31
			else *input_state=OFF;
			}
		else return_val=1; // Input number out of range.
		}
	else if (input_num < 200) // Opto Ethernet inputs
		{
			
		}
	else if (input_num < 300) // PLC Ethernet inputs
		{
			
		}
	else if (input_num < 400) // Fast inputs to main board			
		{
		if (input_num > 299 && input_num < 306) // 300-305 acceptable range
			{	
			if ((na_misc_ins->np_piodata) & (1<<(input_num-300))) *input_state=ON;
			else *input_state=OFF;	
			}
		else return_val=1;
		}	
	else return_val=1; // Input number out of range.	
		
			
	return(return_val);	
	}



// Simple hex string to integer conversion.  No signs are dealt with.
int htoi(char *string)
	{
	int value=0, ctr=0;
	
	if (!string || !*string) return (0);
	while (*string && ctr<8)
		{
		if ((*string >= '0') && (*string <= '9')) value = value * 16 + (*string - '0');
		else if ((*string >= 'A') && (*string <= 'F')) value = value * 16 + (*string - 55); // equiv. to ((*string - 'A') + 10)
		else if ((*string >= 'a') && (*string <= 'f')) value = value * 16 + (*string - 87); // equiv to ((*string - 'a') + 10)
		else
			break;
		string++;
		ctr++; // We won't convert a hex number greater than 8 characters (32 bits).
		}
	return (value);
	}


#define MAX_LENGTH 32+1 // sizeof int + 1

char *Itoa(int i, char* buf, int base) 
	{
    char reverse[MAX_LENGTH+1];	// plus one for the null
    char* s;
    char sign = i < 0;
	
    if (sign) i*=-1; // Take the absolute value of i
    reverse[MAX_LENGTH] = 0;
    s = reverse+MAX_LENGTH;
    do	{
		*--s = "0123456789abcdefghijklmnopqrstuvwxyz"[i % base];
		i /= base;
		} while (i);
    if (sign) *--s = '-';

	sign=0; // We'll now use sign as a counter, it no longer has any meaning as far as the "sign" is concerned.
	while (s[sign]) {buf[sign] = s[sign]; sign++;}
    return (buf);
	}



int Atoi(char *string)
	{
	int value=0, sign=1;
	
	if (!string || !*string) return (0);
	if (*string == '-')
		{
		sign = -1;
		string++;
		}
	else if (*string == '+') string++;

	while (*string)
		{
		if ((*string >= '0') && (*string <= '9')) value = value * 10 + (*string - '0');
		else
			break;
		string++;
		}
	return (sign * value);
	}


// A better gets() that requires a boundary and terminates on CR as well an LF.  If it terminates on a CR, the
// CR will be perserved.  This was a work around for an anomaly in the fastrak2 program that sends out CR and
// then looks for a prompt char.  gets() won't terminate on a CR, so we couldn't download flash files in serial mode.
char *sgets(char *line, int size)
	{
	int i, ch;
	
	for (i = 0; i < size - 2; ++i) 
		{
		//ch = nr_rxchar();
		ch = getchar();
		line[i] = (char)ch;
		if (ch == '\n' || ch == 13) break;
		}

	if (line[i]=='\n') line[i] = 0;
	else if (line[i]==13) line[++i] = 0; // We want to preserve the CR.  The reason we do a size - 2 check in the for loop.
	
	return line;
	}

	
// Basic integer square root function, found this function on the web at:
// http://www.codecodex.com/wiki/index.php?title=Calculate_an_integer_square_root
//
// Return the truncated integer square root of "y" using longs.
// Return -1 on error.
long root(long y)
	{
	long    x_old, x_new;
	long    testy;
	int     nbits;
	int     i;

	if (y <= 0)
		{
		if (y != 0)
			{
			fprintf(stderr, "Domain error in lsqrt().\n");
			return -1L;
			}
		return 0L;
		}
		
	// select a good starting value using binary logarithms: 
	nbits = (sizeof(y) * 8) - 1;    // Subtract 1 for sign bit
	for (i = 4, testy = 16L;; i += 2, testy <<= 2L)
		{
		if (i >= nbits || y <= testy)
			{
			x_old = (1L << (i / 2L));       // x_old = sqrt(testy) 
			break;
			}
		}
		
	// x_old >= sqrt(y) 
	// use the Babylonian method to arrive at the integer square root:
	for (;;) 
		{
		x_new = (y / x_old + x_old) / 2L;
		if (x_old <= x_new)
		   break;
		x_old = x_new;
		}
		
	// make sure that the answer is right: 
	if ((long long) x_old * x_old > y || ((long long) x_old + 1) * ((long long) x_old + 1) <= y)
		{
		fprintf(stderr, "Error in lsqrt().\n");
		return -1L;
		}
		
	return x_old;
	}	
	
	
/**************************************************************/	
	
//////////////////// Macros ///////////////////////

#define ATOI(string, value, sign) \
	{ \
	value=0; sign=1; \
	\
	if (string && *string) \
		{ \
		if (*string == '-') {sign = -1; string++;} \
		\
		while (*string) \
			{ \
			if ((*string >= '0') && (*string <= '9')) value = value * 10 + (*string - '0'); \
			else \
				break; \
			\
			string++; \
			} \
		} \
	value *= sign; \
	}

#define HTOI(string, value) \
	{ \
	value=0; \
	\
	if (string && *string) \
		{ \
		while (*string) \
			{ \
			if ((*string >= '0') && (*string <= '9')) value = value * 16 + (*string - '0'); \
			else if ((*string >= 'A') && (*string <= 'F')) value = value * 16 + (*string - 55); /* equiv. to ((*string - 'A') + 10) */ \
			else if ((*string >= 'a') && (*string <= 'f')) value = value * 16 + (*string - 87); /* equiv to ((*string - 'a') + 10) */ \
			else \
				break; \
			\
			string++; \
			} \
		} \
	}


#define ABS(x) \
			{ \
			if(x<0) x=0-x; \
			}
/////////////////////////////////////////////////////////////////////////

// This function converts a time in uS to period value that can be loaded into
// a Nios timer peripheral. period will have to be broken up into a low and high 
// halfwords before loading since the Nios timer peripheral is a 16 bit device.
int convert_uS_to_timer(unsigned int time_uS, unsigned int *period)
	{
	unsigned long long timer_counts;
	int return_val=0;

	timer_counts = ((unsigned long long)CLK_FREQ * (unsigned long long)time_uS) / 1000000;
	//printf("timer_counts = %u \n", (unsigned int)timer_counts);

	if (timer_counts > 0x00000000ffffffff)
		{
		timer_counts = 0x00000000ffffffff; // Force to 32 bits
		return_val=1; // Over range
		//printf("Timer counts over ranged, set to 0xffffffff \n");
		}

	*period = (unsigned int)timer_counts;

	return (return_val);
	}


// The flipper routine will allow flipping (swapping) a subset of contiguous bits within an unsigned int.
int flipper(unsigned int *val, int start_bit, int no_of_bits)
	{
	int y, tv, tv1, temp_val;
	unsigned int mask=1;

	if ((start_bit + no_of_bits) > ((int)sizeof(int) * 8)) return(99);

	tv1=0;
	tv = *val; 
	temp_val = *val;

	// Mask has the no_of_bits of 1s set.
	mask = (mask<<no_of_bits)-1;
	
	tv &= (mask << start_bit);    
	tv >>= start_bit;                   
	for(y=no_of_bits-1; y>=0; y--) // 7 digital pot bits with possible expansion to 8
		{
		tv1 |= ((tv & 0x00000001) << y);
		tv >>= 1;
		}
	tv1 <<= start_bit; 
	temp_val &= ~(mask << start_bit);
	temp_val |= tv1;	
	*val = temp_val;

	return(0);
	}


// The inverter routine will allow inverting a subset of contiguous bits within an unsigned int.
int inverter(unsigned int *val, int start_bit, int no_of_bits)
	{
	int tv, temp_val;
	unsigned int mask=1;
	
	if ((start_bit + no_of_bits) > ((int)sizeof(int) * 8)) return(99);
	
	tv = *val; 
	temp_val = *val;
	
	// Mask has the no_of_bits of 1s set.
	mask = (mask<<no_of_bits)-1;
	
	tv &= (mask << start_bit);    
	tv >>= start_bit;
	tv = ~tv;
	tv <<= start_bit;
	tv &= (mask << start_bit);
	temp_val &= ~(mask << start_bit);
	temp_val |= tv;
	*val = temp_val;
	
	return (0);
	}


// Function to set len number of bytes in buffer buf with characters ch
//void memset(void *buf, int ch, int len)
//	{
//	char *working_buf;
//	int x;
//
//	working_buf=(char *)buf;
//	for(x=0; x<len; x++)
//		{
//		working_buf[x]=(char)ch;
//		}
//	}



////////////////////// Seven segment display functions //////////////////////

// Define the PIO bits that turn on each segment.
#define SEG_A 0x40
#define SEG_B 0x20
#define SEG_C 0x10
#define SEG_D 0x08
#define SEG_E 0x04
#define SEG_F 0x02
#define SEG_G 0x01
#define DEC_PT 0x80

// Define the segments that make up the various char we can display.  These are passes to the seven_seg_interface()
// function to control the display.
enum
	{
	seven_seg_blank = 0,
	seven_seg_dp = (0 | DEC_PT),
	seven_seg_special_code = (0 | SEG_A),
	seven_seg_minus_sign = (0 | SEG_G),
	seven_seg_0 = (0 | SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),
	seven_seg_1 = (0 | SEG_B | SEG_C),
	seven_seg_2 = (0 | SEG_A | SEG_B | SEG_G | SEG_E | SEG_D),
	seven_seg_3 = (0 | SEG_A | SEG_B | SEG_C | SEG_D | SEG_G),
	seven_seg_4 = (0 | SEG_B | SEG_C | SEG_F | SEG_G),
	seven_seg_5 = (0 | SEG_A | SEG_C | SEG_D | SEG_F | SEG_G),
	seven_seg_6 = (0 | SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G | SEG_A),
	seven_seg_7 = (0 | SEG_A | SEG_B | SEG_C),
	seven_seg_8 = (0 | SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),
	seven_seg_9 = (0 | SEG_A | SEG_B | SEG_C | SEG_F | SEG_G),
	seven_seg_A = (0 | SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G),
	seven_seg_C = (0 | SEG_A | SEG_D | SEG_E | SEG_F),
	seven_seg_E = (0 | SEG_A | SEG_D | SEG_E | SEG_F | SEG_G),
	seven_seg_F = (0 | SEG_A | SEG_E | SEG_F | SEG_G),
	seven_seg_G = (0 | SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),
	seven_seg_H = (0 | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G),
	seven_seg_I = (0 | SEG_B | SEG_C),
	seven_seg_J = (0 | SEG_B | SEG_C | SEG_D | SEG_E),
	seven_seg_L = (0 | SEG_D | SEG_E | SEG_F),
	seven_seg_O = (0 | SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),
	seven_seg_P = (0 | SEG_A | SEG_B | SEG_E | SEG_F | SEG_G),
	seven_seg_S = (0 | SEG_A | SEG_C | SEG_D | SEG_F | SEG_G),
	seven_seg_U = (0 | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),
	seven_seg_Y = (0 | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G),
	seven_seg_b = (0 | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),
	seven_seg_c = (0 | SEG_D | SEG_E | SEG_G),
	seven_seg_d = (0 | SEG_B | SEG_C | SEG_D | SEG_E | SEG_G),
	seven_seg_g = (0 | SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G),
	seven_seg_h = (0 | SEG_C | SEG_E | SEG_F | SEG_G),
	seven_seg_i = (0 | SEG_C),
	seven_seg_n = (0 | SEG_C | SEG_E | SEG_G),
	seven_seg_o = (0 | SEG_C | SEG_D | SEG_E | SEG_G),
	seven_seg_r = (0 | SEG_E | SEG_G),
	seven_seg_u = (0 | SEG_C | SEG_D | SEG_E),
	};

// Seven segment display special functions.
enum
	{
	blank_entire_display,
	light_entire_display,
	show_dashes,
	count,
	};



// Makes display count from 00 to 99. Good for testing the displays 
void display_count(void)
	{
	int tens, ones;
	unsigned char digits[] = {seven_seg_0, seven_seg_1, seven_seg_2, seven_seg_3, seven_seg_4, seven_seg_5,
								seven_seg_6, seven_seg_7, seven_seg_8, seven_seg_9};
	
	for(tens=0; tens<10; tens++)
		{
		for(ones=0; ones<10; ones++)
			{
			//seven_seg_interface(((unsigned char *)&chars)[tens], ((unsigned char *)&chars)[ones]);
			seven_seg_interface(digits[tens], digits[ones]);
			nr_delay(150);
			}
		}
	}



void seven_seg_interface(unsigned char msd, unsigned char lsd)
{
int x, tens=0, ones=0;
unsigned short int wr_val=0;

// "0123456789ACEFGHIJLOPSUYbcdghinoru" // The chars we can display

if (msd == seven_seg_special_code) // lsd will contain the special code
	{
	if (lsd == blank_entire_display) wr_val = 0x0000;
	else if (lsd == light_entire_display) wr_val = 0xffff;
	else if (lsd == show_dashes)
		{
		wr_val = SEG_F | SEG_C;
		wr_val <<= 8;
		wr_val |= (SEG_F | SEG_C);
		}
	else if (lsd == count) display_count();
	}
else
	{
	wr_val = (msd<<8) | lsd;
	}

na_seven_seg_pio->np_piodata = ~wr_val; 
}




//////////////////// ANALOG FUNCTIONS /////////////////////

// Version that uses channel 8 for the conversion. 
// This function should not be called during a shot. 
int convert_ana_ch(int ch_num, int result_type, int *result)
	{
	//volatile unsigned int *ptr;
	int adata=0, return_val=0, try_;
	unsigned int spi_data;
		
			
		// Decrement ch_num and force to the correct range if not already
		ch_num--; // Channels numbered 0 - 19, sent to this function as 1 - 20;
		if (ch_num<0) ch_num=0;
		else if (ch_num>23) ch_num=23;

		// We'll try 5 times to get a valid result.
		for(try_=0; try_<6; try_++)
			{
			// We'll use the channel8 for all conversions in this routine.
			//ptr = &ad_setup_words.ch1;
			//FLOATING_CHANNEL_SETUP = halfword_ptr[ch_num];
			RESULT8_SETUP = ad_setup_words[ch_num]; // Write the setup for the physical channel we want.
			AD_CMD1 = 3; // Command an A/D loop	
			while(AD_CMD1); // Wait for A/D loop to complete
			//spi_data = FLOATING_CHANNEL_RESULT;
			spi_data = RESULT8;

			// Now verify the result we got back is from the physical channel we wanted.
			// The upper 4 bits in the result are the channel number.
			if(ch_num<16)
				{
				if((spi_data>>12) == (unsigned int)ch_num) break;
				}
			else
				{
				if(HAVE_REVB_EXT_INT)
					{
					if(ch_num==16 && (spi_data>>12)==0x000c) break;
					else if(ch_num==17 && (spi_data>>12)==0x000d) break;
					else if(ch_num==18 && (spi_data>>12)==0x000e) break;
					else if(ch_num==19 && (spi_data>>12)==0x000f) break;
					}
				else
					{
					// Otherwise we have a rev. E ext. int
					if((spi_data>>12) == 0x000f) break; // All upper channels are muxed through channel 16
					}
				}
			}

		if (try_ == 6) // Failed to get back a result from the specified channel.
			{
			//printf("AD conversion failure, spi_data>>12=0x%x, ch_num=0x%x\n", spi_data>>12, ch_num);
			return_val=1;
			goto ext;
			} 

		if (result_type == RAW_VAL)	
			{
			//printf("Ana ch%d = %x\n", ch_num-1, spi_data);
			*result=spi_data; // Don't mask out channel number.
			}
		else // VOLTAGE_X_1000
			{
			adata = spi_data & 0x00000fff; 
			
			if (ch_num > 15) // Upper channels
				{
				// Upper channels are always -10 to +10V = -2048 to +2048 input range	
				//if (adata & 0x00000800) adata |= 0xfffff000; // Negative?
				// Now convert to voltage * 1000.
				//*result = (adata*10000)/2048;

				// Range is now 0 - 0xfff == -10 - +10V, 0x800 is 0V or midrange.
				*result = ((adata-0x800)*10000)/2048;
				// For test
				//printf("In function, adata=0x%x, result=%d\n", adata, ((adata-0x800)*10000)/2048);
				}
			else // Lower channels, these can only be positive.
				{
				// First look at V313 the config word to determine the range.
				if (CONFIG_WORD1 & 1<<ch_num) // 0-5V input range.
					// Convert to voltage * 1000.
					*result = (adata*5000)/4095; // 0-5V=0-4095.	
				else // 0-2.5V range. 
					// Convert to voltage * 1000.
					*result = (adata*2500)/4095; // 0-2.5V=0-4095.	
				}	
			}
		
	ext: ;
	set_analog_mode(ANALOG_MODE); // Restore analog mode to the default state
	return (return_val);
	}


// This is a simpler version of the above function that does a conversion on the channels
// already selected as dynamic.  ch_num, if 1 to 8, does not necessiarly correspond to physical channels
// 1 - 8 unless the analog mode is 1 as defined in V429. 
//
// Since channels 17-24 are always converted (with a revE ext. int.), they are available as well.
// This function does not alter the analog mode.
int convert_ana_ch_from_dynamic(int ch_num, int result_type, int *result)
	{
	int adata=0, return_val=0;
	unsigned int spi_data;
		
		ch_num--;
		
		if (ch_num<0) ch_num=0;
		else if (ch_num>7)
			{
			if (ch_num<16) ch_num=16;
			else if (ch_num>23) ch_num=23;
			}

		// If a revB ext.int., we can only do the dynamic channels with one AD loop.  Upper channels are not saved each loop,
		// they can only be obtained by setting the analog mode appropriately and getting them through the dynamic channels.
		if(HAVE_REVB_EXT_INT && ch_num>7) {*result=0; return(1);} 
		else if (HAVE_EXTERNAL_AMP && ch_num>19) {*result=0; return(1);} // CANA channels do not exist unless we have an internal digital servo amp.

		AD_CMD1 = 3; // Command an A/D loop	
		while(AD_CMD1); // Wait for A/D loop to complete
		spi_data = shared_memory[ch_num];

		if (result_type == RAW_VAL)	
			{
			//printf("Ana ch%d = %x\n", ch_num-1, spi_data);
			*result=spi_data; // Don't mask out channel number.
			}
		else // VOLTAGE_X_1000
			{
			adata = spi_data & 0x00000fff; 
			
			if (ch_num > 15 || ad_setup[ch_num] & 0x00080000) // Upper channels including an upper channel in the dynamic channels
				{
				// Upper channels are always -10 to +10V = -2048 to +2048 input range	
				//if (adata & 0x00000800) adata |= 0xfffff000; // Negative?
				// Now convert to voltage * 1000.
				//*result = (adata*10000)/2048;

				// Range is now 0 - 0xfff == -10 - +10V, 0x800 is 0V or midrange.
				*result = ((adata-0x800)*10000)/2048;
				// For test
				//printf("In function, adata=0x%x, result=%d\n", adata, ((adata-0x800)*10000)/2048);
				}
			else // Lower channels, these can only be positive.
				{
				// First look at V313 the config word to determine the range.
				if (CONFIG_WORD1 & 1<<ch_num) // 0-5V input range.
					// Convert to voltage * 1000.
					*result = (adata*5000)/4095; // 0-5V=0-4095.	
				else // 0-2.5V range. 
					// Convert to voltage * 1000.
					*result = (adata*2500)/4095; // 0-2.5V=0-4095.	
				}	
			}
		
	ext: ;
	return (return_val);
	}




// Convert the upper 8 channels (cana_ch1 to wana_ch4) and save the result to ptr_result.
// ptr_result is an 8 element array.  The analog mode does not matter since the upper 
// channels are always converted regardless of the analog mode.
void convert_upper_channels(volatile unsigned short *ptr_result, int result_type)
	{
	int x, adata;
	volatile unsigned short int *ptr_source = na_ad_result_ram; // 64 bytes of shared memory;

	if(HAVE_REVB_EXT_INT) 
		{
		// The only upper channels we have are the WANA channels, 17-20.
		set_analog_mode(4);

		AD_CMD1=1;
		while (AD_CMD1);
		for (x=0; x<4; x++) ptr_result[x] = ptr_source[x+4]; // Copy over upper channels

		if (result_type==VOLTAGE_X_1000)
			{
			for (x=0; x<4; x++)
				{
				adata = ptr_result[x] & 0x0fff;
				ptr_result[x] = ((adata-0x800)*10000)/2048;
				}
			}

		set_analog_mode(ANALOG_MODE);
		}
	else
		{
		AD_CMD1=1;
		while (AD_CMD1);
		for (x=0; x<8; x++) ptr_result[x] = ptr_source[x+16]; // Copy over upper channels

		if (result_type==VOLTAGE_X_1000)
			{
			for (x=0; x<8; x++)
				{
				adata = ptr_result[x] & 0x0fff;
				ptr_result[x] = ((adata-0x800)*10000)/2048;
				}
			}
		}
	}


// Convert the 8 dynamic channels and save the result to ptr_dest.  The actual channels converted
// depends on the analog mode.
// This function does not alter the analog mode.
// ptr_result is an 8 element array.
void convert_dyn_channels(volatile unsigned short *ptr_result, int result_type)
	{
	int x, adata, channel;
	volatile unsigned short int *ptr_source = na_ad_result_ram; // 64 bytes of shared memory;

	AD_CMD1=1;
	while (AD_CMD1);
	for (x=0; x<8; x++) ptr_result[x] = ptr_source[x]; // Copy over channels 1 - 8

	if (result_type==VOLTAGE_X_1000)
		{
		for (x=0; x<8; x++)
			{
			channel = (ptr_result[x] & 0xf000) >> 12;
			adata = ptr_result[x] & 0x0fff;
			if (ad_setup[x] & 0x00080000) // Upper channel, can't check channel as 16 is shared between lower and upper channels
				{
				ptr_result[x] = ((adata-0x800)*10000)/2048;
				}
			else  // Lower channel
				{
				if (CONFIG_WORD1 & 1<<channel) // 0-5V input range.
					// Convert to voltage * 1000.
					ptr_result[x] = (adata*5000)/4095; // 0-5V=0-4095.	
				else // 0-2.5V range. 
					// Convert to voltage * 1000.
					ptr_result[x] = (adata*2500)/4095; // 0-2.5V=0-4095.
				}
			}
		}
	}


// Converts all 24 analog channels and saves the result to ptr_result.
// ptr_result is a 24 element array.
void convert_all_channels(volatile unsigned short *ptr_result, int result_type)
	{
	int x, adata;
	volatile unsigned short int *ptr_source = na_ad_result_ram; // 64 bytes of shared memory;
	
	set_analog_mode(1);
	AD_CMD1=1;
	while (AD_CMD1);
	for (x=0; x<8; x++) ptr_result[x] = ptr_source[x];

	set_analog_mode(2);
	AD_CMD1=1;
	while (AD_CMD1);
	for (x=0; x<4; x++) ptr_result[x+8] = ptr_source[x+4];

	set_analog_mode(3);
	AD_CMD1=1;
	while (AD_CMD1);
	for (x=0; x<4; x++) ptr_result[x+12] = ptr_source[x+4]; // Copy over channels 13 - 16

	// Do the WANA channels
	set_analog_mode(4);
	AD_CMD1=1;
	while (AD_CMD1);
	for (x=0; x<4; x++) ptr_result[x+16] = ptr_source[x+4]; // Copy over channels 17 - 20 the WANA's

	// Do the CANA channels if approiate
	if(!HAVE_REVB_EXT_INT && !HAVE_EXTERNAL_AMP)
		{
		set_analog_mode(8);
		AD_CMD1=1;
		while (AD_CMD1);
		for (x=0; x<4; x++) ptr_result[x+20] = ptr_source[x+4]; // Copy over channels 20 - 23 the CANA's
		}
	else
		{
		for (x=0; x<4; x++) ptr_result[x+20] = 0x0000; // Zero out the result
		}

	if (result_type==VOLTAGE_X_1000)
		{
		for (x=0; x<16; x++)
			{
			adata = ptr_result[x] & 0x0fff;
			if (CONFIG_WORD1 & 1<<x) // 0-5V input range.
				// Convert to voltage * 1000.
				ptr_result[x] = (adata*5000)/4095; // 0-5V=0-4095.	
			else // 0-2.5V range. 
				// Convert to voltage * 1000.
				ptr_result[x] = (adata*2500)/4095; // 0-2.5V=0-4095.
			}

		// Now do the WANA channels
		for (x=16; x<20; x++)
			{
			adata = ptr_result[x] & 0x0fff;
			ptr_result[x] = ((adata-0x800)*10000)/2048;
			}

		// Now the CANA's if appropriate
		if(!HAVE_REVB_EXT_INT && !HAVE_EXTERNAL_AMP)
			{
			for (x=20; x<24; x++)
				{
				adata = ptr_result[x] & 0x0fff;
				ptr_result[x] = ((adata-0x800)*10000)/2048;
				}
			}
		}

	//for(x=0; x<24; x++) printf("ch%d = 0x%x\n", x+1, ptr_source[x]); 
	set_analog_mode(ANALOG_MODE);
	}



int get_dac(int ch_num, int result_type, int *result)
	{
	int return_val=0;
		
	if (ch_num<1 || ch_num>4) return_val = 1;
	else
		{
		if (result_type == VOLTAGE_X_1000)		
			*result = ((vtg.dac_word[ch_num]-8192)*10000)/8192;
		else // Raw value
			*result = (unsigned int)vtg.dac_word[ch_num];	
		}
	return(return_val);	
	}




// This function allows a voltage X 1000 or a raw dac value to be passed.  Also the output
// value can be absolute or incremental.  Incremental is useful for control loops as some
// value can easily be added to the current DAC voltage or RAW value.
int out_dac(int ch_num, int value, int value_type, int abs_inc)
	{
	int ctr=0, error = 0, null_offset, lvdt_offset, axis_num=99;
	
	//#if OUT_DAC_DEBUG
	//	printf("~");
	//#endif
	if (!vtg.dac_write_in_progress) // Don't allow recursion of this function
		{
		vtg.dac_write_in_progress++;
		//#if OUT_DAC_DEBUG
		//	printf("[ch_num %d value %d value_type %d abs_inc %", ch_num, value, value_type, abs_inc);
		//#endif

		// Force channel number into a correct range if not already.
		//if (ch_num > 4) ch_num=4; 
		//else if (ch_num < 1) ch_num=1;

		if (ch_num > 0 && ch_num < 5) // Good channel specification
			{
			if (value_type == VOLTAGE_X_1000 || value_type == PRESS_CMD) // Voltage sent insted of a raw DAC value
				{
				// If incremental, value is a voltage X 1000 to add to the current voltage.
				if (abs_inc == INCREMENTAL)
					value += (((vtg.dac_word[ch_num]-8192)*10000)/8192);
							
				// Otherwise value is an absolute voltage X 1000 to set the DAC to.

				// Convert voltage X 1000 to a raw DAC value
				//value = (unsigned int)(8192 + ((819 * value)/1000));
				value = 8192 + ((819 * value)/1000);
				}
			else if (value_type == RAW_VAL || value_type == PRESS_RAW) 
				{
				// If incremental, dac_val is raw DAC counts to add/sub from the existing DAC value
				if (abs_inc == INCREMENTAL)
					{
					value += (unsigned int)vtg.dac_word[ch_num];
					//printf("value=%d\n", value);
					}

				// Otherwise dac_val is the absolute raw DAC value to send to the DAC
	
				}

			// Do a preliminary sanity check
			if (value > 0x3fff) value = 0x3fff; 
			if (value < 0) value = 0;		

			if (value_type == VOLTAGE_X_1000 || value_type == RAW_VAL) 
				{
				vtg.dac_word[ch_num] = value; // Archive dac_val, does not include the NULL or LVDT offsets.  
				}
			else if (value_type == PRESS_CMD || value_type == PRESS_RAW) 
				{
				//printf("[%d]", value); // For test
				// Do a final range check				
				//if ((PRESSURE_CONT_STATUS_WORD>>4) == 0) // Allowable command range is +/-2.5V.
				if (((PRESSURE_CONT_STATUS_WORD>>4) & 0x00000007) == 0)
					{
					if (value > 12287) value = 10239; // Don't allow command to go outside a +/-2.5V range.
					if (value < 4096) value = 6145;
					}
				//else if ((PRESSURE_CONT_STATUS_WORD>>4) == 1) // Allowable command range is +/-5V.
				else if (((PRESSURE_CONT_STATUS_WORD>>4) & 0x00000007) == 1)
					{
					if (value > 12287) value = 12287; // Don't allow command to go outside a +/-5V range.
					if (value < 4096) value = 4096;
					}
				// Otherwise our range is +/-10V

				if (KEEP_PRESS_CMD_POSITIVE) // Don't allow command to go negative.
					{
					if (value < 8192) value = 8192;
					}

				vtg.dac_word[ch_num] = value; // Archive dac_val, does not include the NULL or LVDT offsets.
				}
			
			// Add in LVDT and NULL offsets if control channel.
			if (ch_num == AXIS1_DAC_CHANNEL) axis_num=1;
			else if (ch_num == AXIS2_DAC_CHANNEL) axis_num=2;			
 			// Now we know what axis number we're dealing with

			// If it is a control axis, add in the lvdt and null offsets.
			if (axis_num != 99)
				{
				// First convert NULL and LVDT offsets from voltage * 1000 to DAC counts.
				null_offset = (819 * NULL_OFFSET)/1000;
				lvdt_offset = (819 * LVDT_OFFSET)/1000;	
				// Then add to value.
				value += (null_offset+lvdt_offset);
				}
						
			// Do a final sanity check
			if (value > 0x3fff) value = 0x3fff; 
			if (value < 0) value = 0;

			// Now send value to the DAC
			value |= 0x4000; // Set bit to tell MAX5170(2) to update DAC.
			
			na_dac_spi->np_spislaveselect = 1 << (ch_num-1);
			while (!(na_dac_spi->np_spistatus & np_spistatus_trdy_mask))
				{
				ctr++;
				if (ctr == 100000) {error = 1; break;}
				}
			na_dac_spi->np_spitxdata = value;
			#if OUT_DAC_DEBUG
				printf("*%d_0x%x", axis_num, value);
			#endif
			}
		else 
			{
			error = 3; // Bad channel number
			}

		vtg.dac_write_in_progress--;
		}
	else
		{
		#if OUT_DAC_DEBUG
			printf("!");
		#endif
		error = 2; // Attempted to recurse this function
		}
	return (error);
	}





