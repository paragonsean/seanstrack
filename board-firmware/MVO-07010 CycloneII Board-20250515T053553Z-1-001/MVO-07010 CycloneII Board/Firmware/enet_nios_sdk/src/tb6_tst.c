//					tb6.c
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

 7-14-06
	Modified the convert_ana_ch_from_dynamic() function to take into account the analog mode in arriving at the
	result.  Previously this was not done and the results from the upper channels were incorrect.
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
	np_pio *misc_ins_pio = na_misc_ins;
		
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
			if ((misc_ins_pio->np_piodata) & (1<<(input_num-300))) *input_state=ON;
			else *input_state=OFF;	
			}
		else return_val=1;
		}	
	else return_val=1; // Input number out of range.	
		
			
	return(return_val);	
	}




// Function to convert a ascii hex string to int.  No signs are dealt with
int htoi (char *str)
{
int x, return_val = 0;
char ch;

for (x = 0; x<=7; x++)
	{
	if (str[x] >= '0' && str[x] <= '9') ch = str[x] & 0x0f;
	else if (str[x] >= 'a' && str[x] <= 'f') ch = (str[x] & 0x0f)+9;
	else if (str[x] >= 'A' && str[x] <= 'F') ch = (str[x] & 0x0f)+9;
	else break; // Non hex digit encountered, exit 

	if (x == 0) //First time through
		{
		return_val = ch;
		}
	else
		{
		return_val <<= 4; return_val |= ch; 
		}
		
	}
return (return_val);
}



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
				if((spi_data>>12) == 0x000f) break; // All upper channels are muxed through channel 16
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
// Since channels 17-24 are always converted, they are available as well.
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

	// Since the upper channels are always converted regardless of analog mode, we'll just copy them now.
	for (x=16; x<24; x++) ptr_result[x] = ptr_source[x];	// Copy channels 17 - 24

		
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

		for (x=16; x<24; x++)
			{
			adata = ptr_result[x] & 0x0fff;
			ptr_result[x] = ((adata-0x800)*10000)/2048;
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
	//np_spi *dac_spi = na_dac_spi;
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
			
			//dac_spi->np_spislaveselect = 1 << (ch_num-1);
			//while (!(dac_spi->np_spistatus & np_spistatus_trdy_mask))
			//	{
			//	ctr++;
			//	if (ctr == 100000) {error = 1; break;}
			//	}
			//dac_spi->np_spitxdata = value;
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





