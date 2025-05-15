// Copywrite 2003,2004, Visi-Trak Worldwide

//			ad_loop10.c

// 2-24-04 ad_loop9.c
// New version after problems were found with mux settling time.  The select hi channel
// line is nolonger controlled from this processor.  The main processor now handles that.
// The A/D setups are now in shared on-chip memory.

// 2-26-04 ad_loop10.c
// The mux settling time on the AD7490 was found to be terrible, around 25uS!  To get around
// that, 8 channels are converted only if the velocity is below 200IPS.  If > 200IPS, channels
// 1-4 alternate with channels 5-6.  Channels 1-8 can be any physical channels 1 - 20
// as configured in the ad_setup_memory.  The floating channel has been elminated.

// 6-12-05 ad_loop10.c
// Program for downloading to flash.

#include "nios.h"


#define SETUP1 ad_setup_memory[0]
#define SETUP2 ad_setup_memory[1]	
#define SETUP3 ad_setup_memory[2]	
#define SETUP4 ad_setup_memory[3]	
#define SETUP5 ad_setup_memory[4]	
#define SETUP6 ad_setup_memory[5]	
#define SETUP7 ad_setup_memory[6]	
#define SETUP8 ad_setup_memory[7]	
#define INITIALIZE_AD ad_setup_memory[8] // Set to 0xffff in main().  Main processor should not write this.

#define RESULT1 result[0]
#define RESULT2 result[1]
#define RESULT3 result[2]
#define RESULT4 result[3]
#define RESULT5 result[4]
#define RESULT6 result[5]
#define RESULT7 result[6]
#define RESULT8 result[7]
#define TOGGLE_CHANNELS result[19] // Written from main processor, allows converting in groups of 4 for high velocities.
#define AD_VERSION result[30]
#define ANALOG_DELAY result[31]

#define CMD_REGISTER cmd_memory[0]

#define VER_NO 10


/*///////////////////// BUILDING THIS VERSION /////////////////////////

1, From the SOPC builder command shell

	a, nios2-build -m16 -b 0x3000 ad_loop??.c<CR>
	b, nios2-srec2flash  ad_loop??.srec -flash_address=0x104000 -ram_address=0x3000 -copy_size=0x800<CR>
	c, nios2-run ad_loop??.flash<CR>  To download the flash file.

2, This file is built using:

	a, Nios V3.2 
	b, SOPC builder V5.1
	c, Quartus II V5.1 SP2.01

3,	This firmware is to run under chip version emc_chip_09010200.hexout.flash or later

4,	The device currently targeted is an Altera EP20K200EFC484-1

*/

int prev_setup;

void sample(int setup)
	{
	volatile unsigned short int *ad_setup_memory = na_ad_setup_ram; // 18 bytes of shared memory
	volatile unsigned short int *result = na_ad_result_ram; // 64 bytes of shared memory
	np_spi *adc_spi = na_adc_spi;
	
	adc_spi->np_spitxdata = ad_setup_memory[setup]; // Write setup word to ADC.
	while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
	result[prev_setup] = adc_spi->np_spirxdata;
	prev_setup = setup;
	}

void delay(void)
	{
	unsigned short int x;
	volatile unsigned short int *result = na_ad_result_ram; // 64 bytes of shared memory

	for(x=0; x<ANALOG_DELAY; x++);
	}

int main(void)
	{
	volatile unsigned short int *result = na_ad_result_ram; // 64 bytes of shared memory
	volatile unsigned short int *cmd_memory = na_ad_cmd_ram; // 4 bytes of shared memory
	volatile unsigned short int *ad_setup_memory = na_ad_setup_ram; // 18 bytes of shared memory
	volatile unsigned short int x;
	int first4=1;
	np_spi *adc_spi = na_adc_spi;
	//np_pio *sel_hi_channels = na_sel_ana17_20;
	
	AD_VERSION = VER_NO; // Write version number
	// Prep the adc
	adc_spi->np_spicontrol = 0;
	adc_spi->np_spislaveselect = 1;
	INITIALIZE_AD = 0xffff; // This is the value pointed to by the "8" in sample(8).
	sample(8); // Write 0xffff to initialize
	sample(8);
	sample(0); // Write setup for channel 1
		
	while(1)
		{
		if (CMD_REGISTER)  // If anything in the command register
			{
			// If TOGGLE_CHANNELS is non-zero, 4 channels will be converted followed
			// by the other 4 the next sample.  TOGGLE_CHANNELS is written by the main processor.
			// The main processor can choose to write this based on velocity.  Above a certain
			// velocity, the alternate mode can be selected allow much higher velocities.
			if(TOGGLE_CHANNELS || !first4) 
				{
				if(first4) // Convert first 4 channels
					{
					first4=0;

					sample(1); // Result 0 will be saved

					delay(); // Allow A/D mux time to settle out
					sample(2); // Result 1 will be saved

					delay();
					sample(3); // Result 2 will be saved

					delay();
					sample(4); // Result 3 will be saved
					}
				else  // Convert lower 4 channels
					{
					first4=1;

					sample(5); // Result 4 will be saved

					delay();
					sample(6); // Result 5 will be saved

					delay();
					sample(7); // Result 6 will be saved

					delay();
					sample(0); // Result 7 will be saved
					}
				}
			else
				{
				first4=1;

				sample(1); // Result 0 will be saved

				delay();
				sample(2); // Result 1 will be saved

				delay();
				sample(3); // Result 2 will be saved

				delay();
				sample(4); // Result 3 will be saved

				delay();
				sample(5); // Result 4 will be saved

				delay();
				sample(6); // Result 5 will be saved

				delay();
				sample(7); // Result 6 will be saved

				delay();
				sample(0); // Result 7 will be saved, write setup for channel 1.
				}
			
			CMD_REGISTER = 0;  // Clear the command register
			}
		}
	}


	
