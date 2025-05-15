//					tb.c
//
//			Copyright 2003, Visi-Trak Worldwide

//#include "nios.h"
//#include "vtw_global.h // Defines in this file.

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


int set_clr_discrete_output(int output_num, int action)
	{
	int return_val=0;
		
	if (output_num < 100) // Opto discrete I/O
		{
		// Discrete Opto outputs are 1-16 (monitor) and 33-48 (control).	
		if (output_num > 0 && output_num < 17) // Monitor outputs
			{
			if (action == TURN_ON)			
				OSW1 |= 1<<(output_num-1); // OSW1 bits 0-15
			else
				OSW1 &= ~(1<<(output_num-1)); 	
			}
		else if (output_num > 32 && output_num < 49) // Control output
			{
			if (action == TURN_ON)	
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




//////////////////// ANALOG FUNCTIONS /////////////////////

int convert_ana_ch(int ch_num, int result_type, int *result)
	{
	//np_spi *adc_spi = na_adc_spi;	
	int ctr=0, error=0, adata=0;

	// Decrement ch_num and force to the correct range if not already
	ch_num--; // Channels numbered 0 - 19, sent to this function as 1 - 20;
	if (ch_num<0) ch_num=0;
	else if (ch_num>19) ch_num=19;

	COMMAND_FROM_MAIN = ch_num | 0x80000000;
	while (COMMAND_FROM_MAIN)
		{
		ctr++;
		if (ctr > 100000) {error=1; break;} // Something hung up, never got a result
		}
	
	if (error);
	else 
		{
		if (result_type == RAW_VAL)	
			{
			printf("Ana ch%d = %x\n", ch_num-1, DATA_FROM_MON);
			*result=(unsigned int)DATA_FROM_MON; // Don't mask out channel number.
			}
		else // VOLTAGE_X_1000
			{
			adata = DATA_FROM_MON & 0x00000fff; 
			
			if (ch_num > 15) // Upper channels
				{
				// Upper channels are always -10 to +10V = -2048 to +2048 input range	
				if (adata & 0x00000800) adata |= 0xfffff000; // Negative?
				// Now convert to voltage * 1000.
				*result = (adata*10000)/2048;
				}
			else // Lower channels, these can only be positive.
				{
				// First look at V313 the config word to determine the range.
				if (vtg.v[313] & 1<<ch_num) // 0-5V input range.
					// Convert to voltage * 1000.
					*result = (adata*5000)/4095; // 0-5V=0-4095.	
				else // 0-2.5V range. 
					// Convert to voltage * 1000.
					*result = (adata*2500)/4095; // 0-2.5V=0-4095.	
				}	
			}	
		}
	return (error);
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
	np_spi *dac_spi = na_dac_spi;
	int ctr=0, error = 0, null_offset, lvdt_offset;
	
	// Force channel number into a correct range if not already.
	if (ch_num > 4) ch_num=4; 
	else if (ch_num < 1) ch_num=1;
	
	if (value_type == VOLTAGE_X_1000) // Voltage sent insted of a raw DAC value
		{
		// If incremental, value is a voltage X 1000 to add to the current voltage.
		if (abs_inc == INCREMENTAL)
			value += (((vtg.dac_word[ch_num]-8192)*10000)/8192);
							
		// Otherwise value is an absolute voltage X 1000 to set the DAC to.

		// Convert voltage X 1000 to a raw DAC value
		value = (unsigned int)(8192 + ((819 * value)/1000));
		}
	else  // Must be RAW_VAL
		{
		// If incremental, dac_val is raw DAC counts to add/sub from the existing DAC value
		if (abs_inc == INCREMENTAL)
			{
			value += (unsigned int)vtg.dac_word[ch_num];
			//printf("value=%d\n", value);
			}

		// Otherwise dac_val is and absolute raw DAC value to send to the DAC
	
		}

	// Do a preliminary sanity check
	if (value > 0x3fff) value = 0x3fff; 
	if (value < 0) value = 0;

	vtg.dac_word[ch_num] = value; // Archive dac_val, does not include the NULL or LVDT offsets.  

	// Add in LVDT and NULL offsets if control channel.
	if (ch_num == CONT_DAC_CHANNEL) // Add in the LVDT and NULL offsets if control channel
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

	value |= 0x4000; // Set bit to tell MAX5170(2) to update DAC.
	
	dac_spi->np_spislaveselect = 1 << (ch_num-1);
	while (!(dac_spi->np_spistatus & np_spistatus_trdy_mask))
		{
		ctr++;
		if (ctr == 100000) {error = 1; break;}
		}
	dac_spi->np_spitxdata = value;
	
	return (error);
	}




#if 0
// This function allows a voltage X 1000 or a raw dac value to be passed.  If voltage X 1000,
// dac_val must be set to 0xffffffff to flag this.
int out_dac(int ch_num, int dac_val, int voltage_x_1000, int abs_inc)
	{
	np_spi *dac_spi = na_dac_spi;
	int ctr=0, error = 0;
	
	// Force channel number into a correct range if not already.
	if (ch_num > 4) ch_num=4; 
	else if (ch_num < 1) ch_num=1;
	
	if (dac_val == 0xffffffff) // Voltage sent insted of a raw DAC value
		{
		// If incremental, voltage_x_1000 is a val to add to the current voltage.
		if (abs_inc == INCREMENTAL)
			voltage_x_1000 += (((vtg.dac_word[ch_num]-8192)*10000)/8192);
						
		// Otherwise voltage_x_1000 is an absolute voltage to set the DAC to.
		if (voltage_x_1000 > 10000) voltage_x_1000 = 10000; // Sanity check
		else if (voltage_x_1000 < -10000) voltage_x_1000 = -10000;
		
		// Convert voltage X 1000 to a raw DAC value
		dac_val = (unsigned int)(8192 + ((819 * voltage_x_1000)/1000));
		}
	else
		{
		// If incremental, dac_val is raw DAC counts to add/sub from the existing DAC value
		if (abs_inc == INCREMENTAL)
			dac_val += (unsigned int)vtg.dac_word[ch_num]

		// Otherwise dac_val is and absolute raw DAC value to send to the DAC
		if (dac_val > 0x3fff) dac_val = 0x3fff; // Sanity check	
		if (dac_val < 0) dac_val = 0;
		}

	vtg.dac_word[ch_num] = dac_val; // Archive dac_val. 
	dac_val |= 0x4000; // Set bit to tell MAX5170(2) to update DAC.
	
	dac_spi->np_spislaveselect = 1 << (ch_num-1);
	while (!(dac_spi->np_spistatus & np_spistatus_trdy_mask))
		{
		ctr++;
		if (ctr == 100000) {error = 1; break;}
		}
	dac_spi->np_spitxdata = dac_val;
	
	return (error);
	}
#endif



#if 0
// This function allows a voltage X 1000 or a raw dac value to be passed.  If voltage X 1000,
// dac_val must be set to 0xffffffff to flag this.
int out_dac(int ch_num, unsigned int dac_val, int voltage_x_1000)
	{
	np_spi *dac_spi = na_dac_spi;
	int ctr=0, error = 0;
	
	// Force channel number into a correct range if not already.
	if (ch_num > 4) ch_num=4; 
	else if (ch_num < 1) ch_num=1;
	
	if (dac_val == 0xffffffff) // Voltage sent insted of a raw DAC value
		{
		if (voltage_x_1000 > 10000) voltage_x_1000 = 10000; // Sanity check
		else if (voltage_x_1000 < -10000) voltage_x_1000 = -10000;
		
		// Convert voltage X 1000 to a raw DAC value
		dac_val = (unsigned int)(8192 + ((819 * voltage_x_1000)/1000));
		}
	else
		{
		if (dac_val > 0x3fff) dac_val = 0x3fff; // Sanity check	
		}

	vtg.dac_word[ch_num] = dac_val; // Archive dac_val. 
	dac_val |= 0x4000; // Set bit to tell MAX5170(2) to update DAC.
	
	dac_spi->np_spislaveselect = 1 << (ch_num-1);
	while (!(dac_spi->np_spistatus & np_spistatus_trdy_mask))
		{
		ctr++;
		if (ctr == 100000) {error = 1; break;}
		}
	dac_spi->np_spitxdata = dac_val;
	
	return (error);
	}
#endif	
	