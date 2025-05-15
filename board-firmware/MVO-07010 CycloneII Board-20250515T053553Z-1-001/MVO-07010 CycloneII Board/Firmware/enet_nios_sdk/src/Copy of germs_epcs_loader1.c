// File: "germs_epcs_loader1.c"
//
// 7-13-07 
//
// A program to load the germs monitor with epcs support at boot from flash to ram.  This program is the first code
// executed at boot-up.  If SW8 is pressed, the basic GERMS monitor in on-chip rom will be forced to execute.  Otherwise
// this loader will look for the presence of the GERMS monitor with EPCS serial support in flash at location 0x1b0000.
// If found, it will be loaded into SRAM at location 0x2000000 and executed.  If GERMS with EPCS support is not found
// in flash, the basic GERMS monitor will be executed from on-chip rom.

#include "excalibur.h"
#include <string.h>
#include <stdio.h>
#include "my_epcs.h"

// This program is compiled for location 0x210000
// nios2-build -b 0x210000 germs_epcs_loader1.c


// Nios II / EPCS flash routines
//extern unsigned char r_epcs_read_status ();
//extern unsigned long r_epcs_lowest_protected_address();
//extern int r_epcs_write_status (unsigned char value);
//extern int r_epcs_protect_region (int bpcode);
//extern int r_epcs_read_byte (unsigned long address, unsigned char *data);
//extern int r_epcs_write_byte (unsigned long address, unsigned char data);
//extern int r_epcs_erase_sector (unsigned long address);
//extern int r_epcs_erase_bulk ();
//extern int r_epcs_read_buffer (unsigned long address, int length, unsigned char *data);
//extern int r_epcs_write_page (unsigned long address, int length, unsigned char *data);
//extern int r_epcs_write_buffer (unsigned long address, int length, unsigned char *data);
//extern int r_epcs_address_past_config (unsigned long *addr);
//
//EPCS function error codes
//#define epcs_success                 0
//#define epcs_err_device_not_present  1
//#define epcs_err_device_not_ready    2
//#define epcs_err_timedout            3
//#define epcs_err_write_failed        4
//#define epcs_invalid_config          5



// This program is intended for the CycloneII board and a nios_32 processor.
// This program will not support the nios2 processor without a lot of modification.

#define GM_FlashBase 0
#define GM_FlashTop 0x200000
//#define GM_FlashExec nasys_main_flash // EPCS boot loader

#define FIRMWARE_BASE_ADDR_IN_FLASH 0x80100
#define FIRMWARE_END_ADDR_IN_FLASH 0x100000
#define FIRMWARE_BASE_ADDR_IN_SRAM 0x2010000

#define GERMS_BASE_ADDR_IN_FLASH 0x1b0100
#define GERMS_END_ADDR_IN_FLASH 0x1c0000
#define GERMS_BASE_ADDR_IN_SRAM na_sram_0_base

#define SW8 ((na_misc_ins->np_piodata)&0x08)
#define SW6 ((na_misc_ins->np_piodata)&0x01)

// +--------------------------------
// | EPCS flash functions

// We could load in all of the epcs code, however to save space, we'll copy here only what we need.
//#include "my_epcs.c" 


// #if 0
//wait until all the data is shifted out
void r_epcs_wait_tx_complete ()
	{
    while (!(epcs_spi->vp_epcsstatus & epcsstatus_tmt_mask));
	}

//set the chip-select
void r_epcs_set_cs ()
	{
    epcs_spi->vp_epcscontrol = epcscontrol_sso_mask;
	}

//clear the chip-select
void r_epcs_clear_cs ()
	{
    int value;
    r_epcs_wait_tx_complete (epcs_spi);

    //clear only the sso bit
    value = epcs_spi->vp_epcscontrol;
    value = value & (~epcscontrol_sso_mask);

    epcs_spi->vp_epcscontrol = value;
	}

//send 8 bits to the memory
void r_epcs_txchar (unsigned char data)
	{
    while (!(epcs_spi->vp_epcsstatus & epcsstatus_trdy_mask));
    epcs_spi->vp_epcstxdata = data;
	}

//receive 8 bits from the memory
unsigned char r_epcs_rxchar ()
	{
    while (!(epcs_spi->vp_epcsstatus & epcsstatus_rrdy_mask));
    return (epcs_spi->vp_epcsrxdata);
	}


//read the memories status register
unsigned char r_epcs_read_status ()
	{
    unsigned char value;
    // set chip select
    r_epcs_set_cs ();

    // send READ command
    r_epcs_txchar (epcs_rdsr);
    r_epcs_rxchar ();   // throw out the garbage
    // read the byte
    r_epcs_txchar (0); // send out some garbage while were reading
    value = r_epcs_rxchar ();

    // clear chip select
    r_epcs_clear_cs ();

    return (value);
	}


//read buffer from memory
int r_epcs_read_buffer (unsigned long address, int length, unsigned char *data)
	{
    volatile int i;

    //verify device presence
    //if (!r_epcs_is_device_present()) return (epcs_err_device_not_present);

    // set chip select
    r_epcs_set_cs ();

    // send READ command
    r_epcs_txchar (epcs_read);
    r_epcs_rxchar ();
    // send high byte of address
    r_epcs_txchar ((address >> 16)&0x000000ff);
    r_epcs_rxchar ();
    // send middle byte of address
    r_epcs_txchar ((address >> 8)&0x000000ff);
    r_epcs_rxchar ();
    // send low byte of address
    r_epcs_txchar (address&0x000000ff);
    r_epcs_rxchar ();
    while (length-- > 0)
		{
        // read the byte
        r_epcs_txchar (0); // send out some garbage while were reading
        *data++ = r_epcs_rxchar ();
		//printf("addr=0x%x, data=0x%x\n", length+1, *data-1);
		}

    // clear chip select
    r_epcs_clear_cs ();

    //return (epcs_success);
	}
// #endif


// +--------------------------------
// | Local Little Routines

typedef void (*r_dull_proc_ptr)(void);

static void r_goto(unsigned int addr)
	{
	r_dull_proc_ptr p;
  
	#if nm_cpu_architecture == nios_32
		// Nios function addresses are...funny, because the architect was...
		addr /= 2;
	#endif
	p = (r_dull_proc_ptr)(addr);
	(*p)();
	}



// Extract the size of an executable program in flash.  The loader code srec2flash places in flash contains this info.
unsigned int get_range(unsigned int flash_address)
	{
	unsigned int range=0;
	unsigned short *hw_ptr, ushort;

	// Now begin to extract the copy size from the little loader srec2flash places at the beginning of the
	// flash file.  We'll search for a 0x3233 first.  This is a MOV instruction.  Always after this MOV is a
	// 3 instruction sequence (PFX->MOVI->MOVHI) that has the copy range imbedded.  This must be extracted with some bit fiddling.
	hw_ptr = (unsigned short int *)flash_address; 
	hw_ptr += 0x10;  // The first 16 bytes won't have what we're looking for so we'll skip them.
				
	do
		{
		r_epcs_read_buffer ((unsigned long)hw_ptr, 2, (void *)&ushort);
		if (hw_ptr >= ((unsigned short *)(flash_address + 0x100))) break; // We scaned all we could and still didn't find it!!!
		hw_ptr++;
		} while (ushort != 0x3233); 

	if (ushort == 0x3233)
		{
		// The PFX instruction is in the following format:
		// 10011 + 11 bits that represent the immed. value.  These are the upper 11 bits of the low halfword of the copy size.
		r_epcs_read_buffer ((unsigned long)hw_ptr, 2, (void *)&ushort); // The PFX instruction
		ushort &= 0x07ff;
		range = (unsigned int)ushort;
		range <<= 5;  // Shift the 11 bits to the MSB position
	
		hw_ptr++;  // Now pointing to the MOVI instruction
		r_epcs_read_buffer ((unsigned long)hw_ptr, 2, (void *)&ushort); // The MOVI instruction
		ushort = (ushort&0x03e0)>>5;
		range |= (unsigned int)ushort;  // We now have the low halfword of the range.
		
		hw_ptr++;  // Now pointing to the MOVHI instruction
		r_epcs_read_buffer ((unsigned long)hw_ptr, 2, (void *)&ushort); // The MOVI instruction
		ushort = (ushort&0x03e0)>>5;
		
		range |= ((unsigned int)ushort<<16);
		}

	return (range);
	}



int exe_prog_fr_flash(unsigned int flash_address, unsigned int sram_address)
	{
	char nios_id[2];
	unsigned int range;
	
	nios_id[0]=0; nios_id[1]=0;
	r_epcs_read_buffer ((unsigned long)(flash_address+12), 2, (void *)&nios_id);

	if (nios_id[0] == 'N' && nios_id[1] == 'i') // Program available in EPCS flash?  If so, we'll load and execute it. 
		{
		range = get_range(flash_address-0x100); // Back up to the loader
		r_epcs_read_buffer ((unsigned long)flash_address, range, (void *)sram_address); 

		// And finally execute the code in sram.
		r_goto(sram_address); 
		}
	}



// This is strucured such that the basic germs monitor is at the bottom of the heap and is our last resort.
// Depending on the pushbuttons, we'll try to load the main operating firmware first. If that fails to load or crashes
// during execution, we'll come back here and attempt to load the germs with epcs serial flash support.  If that
// fails to load or it crashes, we'll come here again and attempt to execute the basic germs from on-chip rom.
// The idea is to always try to keep some aspect of the board operational to give the user something to work with.
//
// Either of the two germs modes can be forced with pushbuttons SW6 or SW8.
int main(void)
	{
	if (SW8) // If SW8 is pressed, we'll force the basic germs mode.
		{
		
		}
	else if (SW6) // If SW6 is pressed, boot into GERMS with EPCS support if present in flash
		{
		// Attempt to load and execute GERMS.  If this fails (or crashes), 
		// we'll fall back to the basic GERMS monitor in on-chip rom.
		exe_prog_fr_flash(GERMS_BASE_ADDR_IN_FLASH, GERMS_BASE_ADDR_IN_SRAM); 
		}
	else  // Load the main firmware from flash if present, if not we'll try to load germs EPCS.  If all else fails, we'll run basic GERMS from on-chip rom.
		{
		// Attempt to load and execute firmware.  If this fails or crashes,  
		// attempt to load and execute germs EPCS.  If this fails or crashes,  
		// we'll fall on down and execute the basic germs monitor.
		exe_prog_fr_flash(FIRMWARE_BASE_ADDR_IN_FLASH, FIRMWARE_BASE_ADDR_IN_SRAM); 
		exe_prog_fr_flash(GERMS_BASE_ADDR_IN_FLASH, GERMS_BASE_ADDR_IN_SRAM); 
		}

	// Our final fallback position, execute the basic germs monitor.  We'll come here when all else fails.
	r_goto(na_germs_rom_base);
	}



