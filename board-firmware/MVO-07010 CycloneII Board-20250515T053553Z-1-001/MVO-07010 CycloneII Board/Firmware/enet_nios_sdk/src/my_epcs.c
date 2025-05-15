// file: my_epcs.c, 

// 12-16-06	My private epcs routines (derived from Altera's epcs.c.  This version doesn't call or reference any 
// Altera object modules.  This could probably be used "as is" in a HAL / IDE version without having to fool 
// with the HAL API to access epcs.


//TODO: Increase address from 2 bytes to 3 -- An Altera comment, don't know about this, 12-16-06

//#include "excalibur.h"
//#include "my_epcs.h"

/*
EPCS memory map.

The CycloneII version of the board uses an EPCS16 flash, that is 2MB in size.  There are 32 64K sectors
defined as follows, the secor address match the psudo addresses used to access these locations:

  ****** 512K Reserved for device configuration ******
  0x000000
  0x010000
  0x020000
  0x030000
  0x040000
  0x050000
  0x060000
  0x070000

  ****** 512K reserved for main firmware ******
  0x080000
  0x090000
  0x0A0000
  0x0B0000
  0x0C0000
  0x0D0000
  0x0E0000
  0x0F0000
  
  ****** Available ******
  0x100000
  0x110000
  0x120000
  0x130000
  0x140000
  0x150000
  0x160000
  0x170000
  0x180000
  0x190000
  0x1A0000

  0x1B0000	GERMS monitor with EPCS support.  This is loaded to SRAM at boot.
  0x1C0000	A/D program for use with a revB external interface board
  0x1D0000	A/D program for use with a revE external interface board	
  0x1E0000	Network settings
  0x1F0000	Variables and the control program	
*/

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

//wait until the write operation is finished
int r_epcs_wait_write_complete ()
{
    volatile unsigned long int i=0;

    // Compute a maximum timeout value for all callers of this function.
    // This value is computed for a maximum timeout of 10s (EPCS4 bulk
    // erase).  The calculation goes as follows:
    //
    // maximum EPCS clock rate: 20MHz
    // read status time: 16 clocks + 1/2 clock period on either side =
    //   17/20M = 850ns
    // 10s / 850ns = 11,764,706 read status commands.
    const unsigned long i_timeout=11770000;

    while ((r_epcs_read_status() & epcs_wip) && (i<i_timeout))
    {
        i++;
    }
    if (i >= i_timeout)
        return (epcs_err_timedout);
    else
        return (epcs_success);
}

//returns true if the chip is present else false
int r_epcs_is_device_present ()
{
    volatile int i;

    // set chip select
    r_epcs_set_cs ();
    // send WRITE ENABLE command
    r_epcs_txchar (epcs_wren);
    r_epcs_rxchar ();  // throw out the garbage
    //clear chip select to set write enable latch
    r_epcs_clear_cs ();
    // some delay between chip select change
    for (i=0;i<20;i++);

    //not check the value
    i = r_epcs_read_status ();
    if (i & epcs_wel)
    {
        // previous write worked,
        // but if the pins are floating,
        // we don't know what the value might be,
        // so lets switch values and check again
        // set chip select
        r_epcs_set_cs ();
        // send WRITE DISABLE command
        r_epcs_txchar (epcs_wrdi);
        r_epcs_rxchar ();  // throw out the garbage
        //clear chip select to set write enable latch
        r_epcs_clear_cs ();
        // some delay between chip select change
        for (i=0;i<20;i++);

        //now check the value again
        i = r_epcs_read_status ();
        if ((i & epcs_wel) == 0)
            return (1);
    }
    //if we made it this far then the device is not present
    return (0);
}

//returns the lowest protected address
unsigned long r_epcs_lowest_protected_address()
{
    int bp;

    //verify device presence
    if (!r_epcs_is_device_present())
        return (epcs_err_device_not_present);

    bp = r_epcs_read_status () & epcs_bp;
    switch (bp)
    {
#if ((epcs_64K) || (epcs_1M))
        case epcs_protect_all:
            return (0);
        default:
            return (epcs_bulk_size - (epcs_sector_size * (bp>>2)));
#else
        case epcs_protect_none:
        case epcs_protect_top_eighth:
        case epcs_protect_top_quarter:
            return (epcs_bulk_size - (epcs_sector_size * (bp>>2)));
        case epcs_protect_top_half:
            return (epcs_bulk_size >> 1);
        default:
            return (0);
#endif
    }
    //this should never happen, but to make the compiler happy...
    return (0);
}

int r_epcs_write_status (unsigned char value)
{
    volatile int i;

    //verify device presence
    if (!r_epcs_is_device_present())
        return (epcs_err_device_not_present);

    // set chip select
    r_epcs_set_cs ();
    // send WRITE ENABLE command
    r_epcs_txchar (epcs_wren);
    r_epcs_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    r_epcs_clear_cs ();
    // some dealy between chip select change
    for (i=0; i<20; i++);

    // set chip select
    r_epcs_set_cs ();

    // send WRSR command
    r_epcs_txchar (epcs_wrsr);
    r_epcs_rxchar ();   // throw out the garbage
    // write the byte
    r_epcs_txchar (value);
    r_epcs_rxchar ();

    // clear chip select
    r_epcs_clear_cs ();

    //now wait until the write operation is finished
    if (i=r_epcs_wait_write_complete ())
        return (i);

    //now verify that the write was successful
    i = r_epcs_read_status ();
    if (i != (int)value) return (epcs_err_write_failed);

    return (epcs_success);
}

//set the status register to protect the selected region
int r_epcs_protect_region (int bpcode)
{
    unsigned char value;

    //verify device presence
    if (!r_epcs_is_device_present())
		{
		printf("EPCS device not present!!!\n");
        return (epcs_err_device_not_present);
		}

    value = r_epcs_read_status ();
    value = value & (!epcs_bp);
    value |= bpcode;
    return (r_epcs_write_status (value));
}

//read 1 byte from the memory
int r_epcs_read_byte (unsigned long address, unsigned char *data)
{
    //verify device presence
    if (!r_epcs_is_device_present())
        return (epcs_err_device_not_present);

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
    // read the byte
    r_epcs_txchar (0); // send out some garbage while were reading
    *data = (unsigned char) r_epcs_rxchar ();

    // clear chip select
    r_epcs_clear_cs ();

    return (epcs_success);
}

//write 1 byte to the memroy
int r_epcs_write_byte (unsigned long address, unsigned char data)
{
    volatile int i;
    unsigned char verify;

    //verify device presence
    if (!r_epcs_is_device_present())
        return (epcs_err_device_not_present);

    // make sure the address is writable
    if (address >= r_epcs_lowest_protected_address())
        return (epcs_err_write_failed);

    // set chip select
    r_epcs_set_cs ();
    // send WRITE ENABLE command
    r_epcs_txchar (epcs_wren);
    r_epcs_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    r_epcs_clear_cs ();
    //maybe we need some delay here?
    for (i=0; i<20; i++);

    //set chip select
    r_epcs_set_cs ();
    // send WRITE command
    r_epcs_txchar (epcs_write);
    r_epcs_rxchar ();   // throw out the garbage
    // send high byte of address
    r_epcs_txchar ((address >> 16)&0x000000ff);
    r_epcs_rxchar ();   // throw out the garbage
    // send middle byte of address
    r_epcs_txchar ((address >> 8)&0x000000ff);
    r_epcs_rxchar ();   // throw out the garbage
    // send low byte of address
    r_epcs_txchar (address&0x000000ff);
    r_epcs_rxchar ();   // throw out the garbage
    // send the data
    r_epcs_txchar (data);
    r_epcs_rxchar ();   // throw out the garbage

    // clear chip select to set complete the write cycle
    r_epcs_clear_cs ();

    //now wait until the write operation is finished
    if (i=r_epcs_wait_write_complete ())
        return (i);

    //now verify that the write was successful
    r_epcs_read_byte (address, &verify);
    if (verify != data) return (epcs_err_write_failed);

    return (epcs_success);
}

//erase sector which contains address
int r_epcs_erase_sector (unsigned long address)
{
    volatile int i;

    //verify device presence
    if (!r_epcs_is_device_present())
        return (epcs_err_device_not_present);

    // make sure the address is writable
    if (address >= r_epcs_lowest_protected_address())
        return (epcs_err_write_failed);

    // set chip select
    r_epcs_set_cs ();
    // send WRITE ENABLE command
    r_epcs_txchar (epcs_wren);
    r_epcs_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    r_epcs_clear_cs ();
    //maybe we need some delay here?
    for (i=0; i<20; i++);

    //set chip select
    r_epcs_set_cs ();
    // send SE command
    r_epcs_txchar (epcs_se);
    r_epcs_rxchar ();   // throw out the garbage
    // send high byte of address
    r_epcs_txchar ((address >> 16)&0x000000ff);
    r_epcs_rxchar ();   // throw out the garbage
    // send middle byte of address
    r_epcs_txchar ((address >> 8)&0x000000ff);
    r_epcs_rxchar ();   // throw out the garbage
    // send low byte of address
    r_epcs_txchar (address&0x000000ff);
    r_epcs_rxchar ();   // throw out the garbage

    // clear chip select to complete the write cycle
    r_epcs_clear_cs ();

    //now wait until the write operation is finished
    if (i=r_epcs_wait_write_complete ())
        return (i);

    return (epcs_success);
}

//erase entire chip
int r_epcs_erase_bulk ()
{
    volatile int i;

    //verify device presence
    if (!r_epcs_is_device_present())
        return (epcs_err_device_not_present);

    // make sure the address is writable
    if (r_epcs_lowest_protected_address() != epcs_bulk_size)
        return (epcs_err_write_failed);

    // set chip select
    r_epcs_set_cs ();
    // send WRITE ENABLE command
    r_epcs_txchar (epcs_wren);
    r_epcs_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    r_epcs_clear_cs ();
    //maybe we need some delay here?
    for (i=0; i<20; i++);

    //set chip select
    r_epcs_set_cs ();
    // send BE command
    r_epcs_txchar (epcs_be);
    r_epcs_rxchar ();   // throw out the garbage

    // clear chip select to complete the write cycle
    r_epcs_clear_cs ();

    //now wait until the write operation is finished
    if (i=r_epcs_wait_write_complete ())
        return (i);

    return (epcs_success);
}

//read buffer from memory
int r_epcs_read_buffer (unsigned long address, int length,
            unsigned char *data)
{
    volatile int i;

    //verify device presence
    if (!r_epcs_is_device_present())
        return (epcs_err_device_not_present);

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

    return (epcs_success);
}
//write page to memory
int r_epcs_write_page (unsigned long address, int length,
            unsigned char *data)
{
    volatile int i;
    unsigned char verify[epcs_page_size];
    int my_length = length;
    unsigned char *my_data = data;

    //verify device presence
    if (!r_epcs_is_device_present())
        return (epcs_err_device_not_present);

    // set chip select
    r_epcs_set_cs ();
    // send WRITE ENABLE command
    r_epcs_txchar (epcs_wren);
    r_epcs_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    r_epcs_clear_cs ();
    //maybe we need some delay here?
    for (i=0; i<20; i++);

    //set chip select
    r_epcs_set_cs ();
    // send WRITE command
    r_epcs_txchar (epcs_write);
    r_epcs_rxchar ();   // throw out the garbage
    // send high byte of address
    r_epcs_txchar ((address >> 16)&0x000000ff);
    r_epcs_rxchar ();   // throw out the garbage
    // send middle byte of address
    r_epcs_txchar ((address >> 8)&0x000000ff);
    r_epcs_rxchar ();   // throw out the garbage
    // send low byte of address
    r_epcs_txchar (address&0x000000ff);
    r_epcs_rxchar ();   // throw out the garbage
    // send the data
    while (my_length-- > 0)
    {
        // write the byte
        r_epcs_txchar (*my_data++);
        r_epcs_rxchar ();   // throw out the garbage
    }

    // clear chip select to complete the write cycle
    r_epcs_clear_cs ();

    //now wait until the write operation is finished
    if (i=r_epcs_wait_write_complete ())
        return (i);

    //verify that the data was correctly written
    r_epcs_read_buffer (address, length, verify);
    for (i=0; i<length; i++)
    {
        if (verify[i] != data[i]) return (epcs_err_write_failed);
    }
    return (epcs_success);
}

//write buffer to memroy
int r_epcs_write_buffer (unsigned long address, int length,
            unsigned char *data)
{
    volatile int i;
    int status;

    //verify device presence
    if (!r_epcs_is_device_present())
        return (epcs_err_device_not_present);

    // make sure the full address range is writable
    if ((address >= r_epcs_lowest_protected_address()) ||
        ((address+length-1) >= r_epcs_lowest_protected_address()))
        return (epcs_err_write_failed);

    //send partial first page (if there is one)
    i = address % epcs_page_size;
    if ((i != 0) && (i+length > epcs_page_size))
    {
        i = epcs_page_size-i;
        if (status = r_epcs_write_page (address, i, data))
            return (status); //write failed
        address += i;
        data += i;
        length -= i;
    }

    //send all full pages
    while (length/epcs_page_size > 0)
    {
        if (status = r_epcs_write_page (address, epcs_page_size, data))
            return (status); //write failed
        address += epcs_page_size;
        data += epcs_page_size;
        length -= epcs_page_size;
    }

    //send partial last page (if there is one)
    if (length > 0)
    {
        if (status = r_epcs_write_page (address, length, data))
            return (status); //write failed
    }

    return (epcs_success);
}

#define NR_EPCS_BITREVERSE(x) \
  ((((x) & 0x80) ? 0x01 : 0) | \
   (((x) & 0x40) ? 0x02 : 0) | \
   (((x) & 0x20) ? 0x04 : 0) | \
   (((x) & 0x10) ? 0x08 : 0) | \
   (((x) & 0x08) ? 0x10 : 0) | \
   (((x) & 0x04) ? 0x20 : 0) | \
   (((x) & 0x02) ? 0x40 : 0) | \
   (((x) & 0x01) ? 0x80 : 0))

int r_epcs_address_past_config (unsigned long *addr)
{
    unsigned long i;
    int j;
    unsigned char value;
    unsigned char buf[4];
    int err;

    if (err=r_epcs_read_buffer(0, sizeof(buf) / sizeof(*buf), buf))
        return (err);

    for (i=0; i<epcs_bulk_size - 8; i++)
    {
        if (err=r_epcs_read_byte (i, &value))
            return (err);
        if (value == NR_EPCS_BITREVERSE(0x6A)) break;
        if (value != 0xFF) return (epcs_invalid_config);
    }

    //if we haven't seen any data by the 64th byte,
    //then it doesn't look like there's any configuration data
    if (i >= epcs_bulk_size - 8) return (epcs_invalid_config);

    // If we made it this far, we found the 0x6A byte at address i.  Beyond that,
    // we expect an 8-byte "option register", of which the last 4 bytes are the
    // length, LS-byte first.
    i += 5; // Jump ahead to the length.

    // Read the 4 bytes of the length.
    if (err=r_epcs_read_buffer(i,4,buf))
        return (err);

    // Compute the length.
    *addr = 0;
    for (j = 3; j != ~0; --j)
    {
        *addr <<= 8;
        *addr |= NR_EPCS_BITREVERSE(buf[j]);
    }
    // The last address, oddly enough, is in bits.
    // Convert to bytes, rounding up.
    *addr += 7;
    *addr /= 8;

    return (epcs_success);
}


