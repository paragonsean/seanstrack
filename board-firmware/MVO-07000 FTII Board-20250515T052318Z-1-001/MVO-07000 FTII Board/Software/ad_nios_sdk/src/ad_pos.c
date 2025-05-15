// Copywrite 2003, Visi-Trak Worldwide

//			ad_pos.c

//	This version does not convert all channels.  It converts channels 1 - 8, 17 - 20 and
// one "floating channel."  This was an attempt to decrease the conversion loop time
// while still allowing for the longer delay times between conversions necessary to
// elminate crosstalk between analog channels.  With DELAY_LONG set to 8 and DELAY_SHORT
// set to 6, the max speed was about 425 IPS.

#include "nios.h"

#define DELAY_LONG 8  // Less crosstalk more delay
#define DELAY_SHORT 6 // Less delay more crosstalk
 
#define SETUP1 0x8350
#define SETUP2 0x8750	
#define SETUP3 0x8b50	
#define SETUP4 0x8f50	
#define SETUP5 0x9350	
#define SETUP6 0x9750	
#define SETUP7 0x9b50	
#define SETUP8 0x9f50	
#define SETUP9 0xa350	
#define SETUP10 0xa750	
#define SETUP11 0xab50	
#define SETUP12 0xaf50	
#define SETUP13 0xb350	
#define SETUP14 0xb750	
#define SETUP15 0xbb50	
#define SETUP16 0xbf50	
#define SETUP17 0xb350	
#define SETUP18 0xb750	
#define SETUP19 0xbb50	
#define SETUP20 0xbf50
#define FLOATING_CHANNEL_SETUP shared_memory[29]

#define RESULT1 shared_memory[0]
#define RESULT2 shared_memory[1]
#define RESULT3 shared_memory[2]
#define RESULT4 shared_memory[3]
#define RESULT5 shared_memory[4]
#define RESULT6 shared_memory[5]
#define RESULT7 shared_memory[6]
#define RESULT8 shared_memory[7]
#define RESULT9 shared_memory[8]
#define RESULT10 shared_memory[9]
#define RESULT11 shared_memory[10]
#define RESULT12 shared_memory[11]
#define RESULT13 shared_memory[12]
#define RESULT14 shared_memory[13]
#define RESULT15 shared_memory[14]
#define RESULT16 shared_memory[15]
#define RESULT17 shared_memory[16]
#define RESULT18 shared_memory[17]
#define RESULT19 shared_memory[18]
#define RESULT20 shared_memory[19]
#define FLOATING_CHANNEL_RESULT shared_memory[30]
#define CURPOS	 shared_memory[32] // 2 words
#define Q1_TIME	 shared_memory[34] //  "   "	
#define Q2_TIME	 shared_memory[36] //  "   "
#define Q3_TIME	 shared_memory[38] //  "   "
#define Q4_TIME	 shared_memory[40] //  "   "
#define VELOCITY shared_memory[42] //  "   "

#define CMD_REGISTER	cmd_memory[0]
#define PB_POS			cmd_memory[2] // Incremented here during PB data collection.
#define BOUNCE_BACK_POS cmd_memory[4]
#define LS1_SETTING		cmd_memory[6]
#define LS2_SETTING		cmd_memory[8]
#define LS3_SETTING		cmd_memory[10]
#define LS4_SETTING		cmd_memory[12]
#define LS5_SETTING		cmd_memory[14]
#define LS6_SETTING		cmd_memory[16]
#define CONT_POS1		cmd_memory[18]
#define CONT_POS2		cmd_memory[20]

// Masks
#define Q1_MASK 0x01
#define Q2_MASK 0x02
#define Q3_MASK 0x04
#define Q4_MASK 0x08
#define UD_MASK 0x10
#define X4_MASK 0x20


// Need to do
// 1,	Increase the size of shared memory
// 2,	Increase the size of the command register
// 3,	Add an interrupt input pio, 6 bits
// 4,	Add an interrupt output pio, 10 bits
// 5,	Add a timer input pio, 32 bits





// This program is compiled for location 0x3000
// nios-build -m16 -b 0x3000 ad_loop?.c


// Globals
volatile unsigned short int *shared_memory = (unsigned short int *)0x2000; // 64 bytes
volatile unsigned short int *cmd_memory = (unsigned short int *)0x2200; // 4 bytes
np_spi *int_input = na_??????;
np_spi *int_output = na_??????;

int isr(int context)
	{

	}


// This program uses for loop delays to elminate cross talk between analog channels.
// The less important non-dynamic channels have a shorter delay which will result in more
// cross talk but speeds up the overall conversion loop time
int main(void)
	{
	volatile int x;
	np_spi *adc_spi = na_adc_spi;
	np_pio *sel_hi_channels = na_sel_ana17_20;
	

	FLOATING_CHANNEL_SETUP = SETUP9; // Default floating setup to ch. 9

	// Prep the adc
	adc_spi->np_spicontrol = 0;
	adc_spi->np_spislaveselect = 1;
	
	adc_spi->np_spitxdata = 0xffff;
	while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
	x = adc_spi->np_spirxdata;

	adc_spi->np_spitxdata = 0xffff;
	while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
	x = adc_spi->np_spirxdata;

	adc_spi->np_spitxdata = SETUP1; //Wr ch1
	while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
	x = adc_spi->np_spirxdata;	
	
	
	while (1)
		{
		if (CMD_REGISTER)  // If anything in the command register
			{
			sel_hi_channels->np_piodata = 0; // Select low channels
	
			adc_spi->np_spitxdata = SETUP2; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT1 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_LONG; x++); // Delay to prevent analog bleed through between channels
			adc_spi->np_spitxdata = SETUP3; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT2 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_LONG; x++);
			adc_spi->np_spitxdata = SETUP4; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT3 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_LONG; x++);
			adc_spi->np_spitxdata = SETUP5; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT4 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_SHORT; x++);
			adc_spi->np_spitxdata = SETUP6; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT5 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_SHORT; x++);
			adc_spi->np_spitxdata = SETUP7; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT6 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_SHORT; x++);
			adc_spi->np_spitxdata = SETUP8; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT7 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_SHORT; x++);
			adc_spi->np_spitxdata = FLOATING_CHANNEL_SETUP; 
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT8 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_LONG; x++);
			adc_spi->np_spitxdata = SETUP17; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			sel_hi_channels->np_piodata = 1; // Select hi channels now
			FLOATING_CHANNEL_RESULT = adc_spi->np_spirxdata; // Floating channel
			
			for(x=0; x<DELAY_SHORT; x++); // Short delay here since selecting hi channels will take some time
			adc_spi->np_spitxdata = SETUP18; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT17 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_SHORT; x++);
			adc_spi->np_spitxdata = SETUP19; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT18 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_SHORT; x++);
			adc_spi->np_spitxdata = SETUP20; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT19 = adc_spi->np_spirxdata;

			for(x=0; x<DELAY_SHORT; x++);
			adc_spi->np_spitxdata = SETUP1; // Write setup word to ADC.
			while (!(adc_spi->np_spistatus & np_spistatus_rrdy_mask));
			RESULT20 = adc_spi->np_spirxdata;

			CMD_REGISTER = 0;  // Clear the command register
			}
		}
	}


	
