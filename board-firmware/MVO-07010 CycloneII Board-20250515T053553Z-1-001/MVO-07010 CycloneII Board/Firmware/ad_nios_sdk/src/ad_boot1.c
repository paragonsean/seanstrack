// Copywrite 2003,2004, 2005, 2006, Visi-Trak Worldwide

//			ad_boot1.c

// 2-11-05 ad_boot.c
// AD processor boot code.  Main code will be loaded into memory at location 0x3000 by main processor.

// 12-7-06 ad_boot1.c  Recompiled for Nios II and for the new addresses on the Cyclone II board.  No more asm code.

#include "nios.h"



#define CMD_REGISTER cmd_memory[0]
#define AD_PROGRAM_MEMORY 0x209000

// This program is compiled for location 0x200800
// nios2-build -b 0x200800 ad_boot1.c

// To create a hex file
// elf2hex -b 0x200800 -e 0x200900 -i ad_boot1.elf -o ad_boot1.hex -w 32

typedef void (*ptr_to_ad_proc)(void);

int main(void)
	{
	volatile unsigned short int *cmd_memory = na_ad_cmd_ram; // 4 bytes of shared memory
	ptr_to_ad_proc p;	
	
	do
		{
		while(CMD_REGISTER != 0x9999) // Wait for main processor to load our program
			{

			}
		
		// We're now loaded, we'll jump to 0x3000 and begin execution.
		CMD_REGISTER = 0;  // Clear the command register
		
		p = (ptr_to_ad_proc)(AD_PROGRAM_MEMORY);
		(*p)();

		} while (1);
	}


	
